/* $Id: windllbase.cpp,v 1.8 2000-01-05 19:39:57 sandervl Exp $ */

/*
 * Win32 Dll base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: Unloading of system dlls is not correct for the PE loader
 *       (they're always put at the head of the list even though there
 *        might be a dependency with a win32 dll)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <misc.h>
#include <win32type.h>
#include <pefile.h>
#include <windllbase.h>
#include <wprocess.h>
#include "exceptions.h"
#include "exceptutil.h"
#include "cio.h"
#include "vmutex.h"
#include "oslibmisc.h"
#include "oslibdos.h"
#include "profile.h"

VMutex dlllistmutex;   //protects linked lists of heaps

//******************************************************************************
//******************************************************************************
Win32DllBase::Win32DllBase(HINSTANCE hinstance, WIN32DLLENTRY DllEntryPoint, 
                           Win32ImageBase *parent)
                 : Win32ImageBase(hinstance),
  	           referenced(0), fSkipEntryCalls(FALSE),
                   fAttachedToProcess(FALSE), fUnloaded(FALSE)
{
  dllEntryPoint = DllEntryPoint;

  dlllistmutex.enter();
  //unload of dlls needs to be done in reverse order of dependencies
  //Note: Only necessary for pe loader; the OS/2 loader takes care of this 
  //for win32k/pe2lx 
  if(parent && parent->isDll()) {
	Win32DllBase *dll = head;
	while(dll) {
		if(dll->getInstanceHandle() == parent->getInstanceHandle()) {
			break;
		}
		dll = dll->next;
	}
	if(dll) {
		//insert this dll in list after it's parent
		next = dll->next;
		dll->next = this;
	}
	else 	DebugInt3();
  }
  else {
  	next = head;
  	head = this;
  }
  dlllistmutex.leave();

#ifdef DEBUG_ENABLELOG_LEVEL2
  dlllistmutex.enter();
  Win32DllBase *dll = head;

  dprintf2(("Win32DllBase::Win32DllBase: List of loaded dlls:"));
  while(dll) {
	dprintf2(("DLL %s", dll->szModule));
	dll = dll->next;
  }
  dlllistmutex.leave();
#endif

  dprintf(("Win32DllBase::Win32DllBase %x %s", hinstance, szModule));
}
//******************************************************************************
//******************************************************************************
Win32DllBase::~Win32DllBase()
{
 Win32DllBase *dll = head;

  dprintf(("Win32DllBase::~Win32DllBase %s", szModule));

  if(errorState == NO_ERROR && !fUnloaded)
  {
	detachProcess();
  }

  dlllistmutex.enter();
  if(head == this) {
	head = next;
  }
  else {
	while(dll && dll->next != this) {
		dll = dll->next;
        }
	if(dll == NULL) {
		dprintf(("~Win32DllBase: Can't find dll!\n"));
		dlllistmutex.leave();
		return;
	}
	dll->next = next;
  }
  dlllistmutex.leave();
}
//******************************************************************************
//ASSUMPTION: called by FreeLibrary
//******************************************************************************
ULONG Win32DllBase::Release()
{
 ULONG ret = --referenced;

  if(ret == 0) {
	dprintf(("Win32DllBase::Release, referenced == 0\n"));
	delete this;
  }
  return(ret);
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::attachProcess()
{
 WINEXCEPTION_FRAME exceptFrame;
 USHORT sel;
 THDB *thdb;
 BOOL rc, fSetExceptionHandler;

  if(fAttachedToProcess)
	return TRUE;

  fAttachedToProcess = TRUE;

  thdb = GetThreadTHDB();
  fSetExceptionHandler = (!thdb || thdb->teb_sel != GetFS());

  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  if(fSetExceptionHandler) {
  	OS2SetExceptionHandler((void *)&exceptFrame);
  	sel = SetWin32TIB();
  }

  //Allocate TLS index for this module
  tlsAlloc();
  tlsAttachThread();	//setup TLS (main thread)

  if(fSkipEntryCalls || dllEntryPoint == NULL) {
        dprintf(("attachProcess not required for dll %s", szModule));
  	if(fSetExceptionHandler) {
  		SetFS(sel);
		OS2UnsetExceptionHandler((void *)&exceptFrame);
	}
	return(TRUE);
  }

  dprintf(("attachProcess to dll %s", szModule));

  rc = dllEntryPoint(hinstance, DLL_PROCESS_ATTACH, 0);

  dprintf(("attachProcess to dll %s DONE", szModule));

  if(fSetExceptionHandler) {
  	SetFS(sel);
  	OS2UnsetExceptionHandler((void *)&exceptFrame);
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::detachProcess()
{
 WINEXCEPTION_FRAME exceptFrame;
 USHORT sel;
 BOOL rc;

  if(fSkipEntryCalls || dllEntryPoint == NULL) {
        tlsDetachThread();	//destroy TLS (main thread)
	fUnloaded = TRUE;
	return(TRUE);
  }

  dprintf(("detachProcess from dll %s", szModule));

  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);

  fUnloaded = TRUE;
  sel = SetWin32TIB();
  rc = dllEntryPoint(hinstance, DLL_PROCESS_DETACH, 0);
  SetFS(sel);
  tlsDetachThread();	//destroy TLS (main thread)
  tlsDelete();

  OS2UnsetExceptionHandler((void *)&exceptFrame);

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::attachThread()
{
 WINEXCEPTION_FRAME exceptFrame;
 BOOL               rc;

  if(fSkipEntryCalls || dllEntryPoint == NULL)
	return(TRUE);

  dprintf(("attachThread to dll %s", szModule));

  rc = dllEntryPoint(hinstance, DLL_THREAD_ATTACH, 0);

  dprintf(("attachThread to dll %s DONE", szModule));

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::detachThread()
{
 WINEXCEPTION_FRAME exceptFrame;
 BOOL               rc;

  if(fSkipEntryCalls || dllEntryPoint == NULL)
	return(TRUE);

  dprintf(("detachThread from dll %s", szModule));

  rc =  dllEntryPoint(hinstance, DLL_THREAD_DETACH, 0);
  return rc;
}
//******************************************************************************
//Send DLL_THREAD_ATTACH message to all dlls for a new thread
//******************************************************************************
void Win32DllBase::attachThreadToAllDlls()
{
  Win32DllBase *dll = Win32DllBase::head;
  while(dll) {
	dll->attachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//Send DLL_THREAD_DETACH message to all dlls for thread that's about to die
//******************************************************************************
void Win32DllBase::detachThreadFromAllDlls()
{
  Win32DllBase *dll = Win32DllBase::head;
  while(dll) {
	dll->detachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//Setup TLS structure for all dlls for a new thread
//******************************************************************************
void Win32DllBase::tlsAttachThreadToAllDlls()
{
  Win32DllBase *dll = Win32DllBase::head;
  while(dll) {
	dll->tlsAttachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//Destroy TLS structure for all dlls for a thread that's about to die
//******************************************************************************
void Win32DllBase::tlsDetachThreadFromAllDlls()
{
  Win32DllBase *dll = Win32DllBase::head;
  while(dll) {
	dll->tlsDetachThread();
	dll = dll->getNext();
  }
}
//******************************************************************************
//******************************************************************************
void Win32DllBase::deleteAll()
{
#ifdef DEBUG
  dlllistmutex.enter();
  Win32DllBase *dll = head;

  dprintf(("Win32DllBase::deleteAll: List of loaded dlls:"));
  while(dll) {
	dprintf(("DLL %s", dll->szModule));
	dll = dll->next;
  }
  dlllistmutex.leave();
#endif

  while(Win32DllBase::head) {
	delete Win32DllBase::head;
  }
}
//******************************************************************************
//Add renaming profile strings for ole32 & netapi32 to odin.ini if they aren't
//already there
//******************************************************************************
void Win32DllBase::setDefaultRenaming()
{
 char renameddll[CCHMAXPATH];

  if(ODIN_PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "OLE32", "", renameddll, 
                                   sizeof(renameddll)-1) <= 1) 
  {
	ODIN_PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "OLE32", "OLE32OS2");
	ODIN_PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "OLE32OS2", "OLE32");
  }
  if(ODIN_PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "OLEAUT32", "", renameddll, 
                                   sizeof(renameddll)-1) <= 1) 
  {
	ODIN_PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "OLEAUT32", "OLAUTOS2");
	ODIN_PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "OLAUTOS2", "OLEAUT32");
  }
  if(ODIN_PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "NETAPI32", "", renameddll, 
                                   sizeof(renameddll)-1) <= 1) 
  {
	ODIN_PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "NETAPI32", "WNETAP32");
	ODIN_PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "WNETAP32", "NETAPI32");
  }
}
//******************************************************************************
//rename dll if necessary:
// Win32 to OS/2 : (i.e. OLE32 -> OLE32OS2)
// or
// OS/2 to Win32 : (i.e. OLE32OS2 -> OLE32)
//******************************************************************************
void Win32DllBase::renameDll(char *dllname, BOOL fWinToOS2)
{
 char modname[CCHMAXPATH];
 char renameddll[CCHMAXPATH];
 char *namestart;
 char *sectionname;

  if(fWinToOS2) {
	sectionname = DLLRENAMEWIN_SECTION;
  }
  else {
	sectionname = DLLRENAMEOS2_SECTION;
  }
  namestart = OSLibStripPath(dllname);
  strcpy(modname, namestart);
  char *dot = strrchr(modname, '.');
  if(dot)
	*dot = 0;
  strupr(modname);
  if(ODIN_PROFILE_GetOdinIniString(sectionname, modname, "", renameddll, 
                                   sizeof(renameddll)-1) > 1) 
  {
	if(namestart == dllname) {
		strcpy(dllname, renameddll);
	}
	else {
		*namestart = 0;
		strcat(dllname, renameddll);
	}
	strcat(dllname, ".dll");
  }
  return;
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModule(char *dllname)
{
 Win32DllBase *dll;
 char szDllName[CCHMAXPATH];
 char *dot, *temp;

  dprintf(("findModule %s", dllname));

  strcpy(szDllName, OSLibStripPath(dllname));
  strupr(szDllName);
  dot = strstr(szDllName, ".");
  if(dot)
	*dot = 0;

  dlllistmutex.enter();
  dll = head;
  while(dll) {
	if(strcmpi(szDllName, dll->szModule) == 0) {
		dlllistmutex.leave();
		return(dll);
        }

	dll = dll->next;
  }
  dlllistmutex.leave();
  return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModule(WIN32DLLENTRY DllEntryPoint)
{
   dprintf(("findModule %X", DllEntryPoint));

   dlllistmutex.enter();
   Win32DllBase *mod = Win32DllBase::head;
   while(mod != NULL) {
	dbgCheckObj(mod);
	if(mod->dllEntryPoint == DllEntryPoint) {
		dlllistmutex.leave();
		return(mod);
        }
	mod = mod->next;
   }
   dlllistmutex.leave();
   return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModule(HINSTANCE hinstance)
{
   dlllistmutex.enter();

   Win32DllBase *mod = Win32DllBase::head;
   while(mod != NULL) {
	dbgCheckObj(mod);
	if(mod->hinstance == hinstance) {
		dlllistmutex.leave();
		return(mod);
        }
	mod = mod->next;
   }
   dlllistmutex.leave();
   return(NULL);
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::isDll()
{
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void Win32DllBase::setThreadLibraryCalls(BOOL fEnable)
{
  // if fEnable == true, do call the ATTACH_THREAD, DETACH_THREAD functions
  // if fEnable == false, do not call the ATTACH_THREAD, DETACH_THREAD functions
  fSkipEntryCalls = !fEnable;
}

//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::head = NULL;
