/* $Id: windllbase.cpp,v 1.1 1999-09-15 23:39:07 sandervl Exp $ */

/*
 * Win32 Dll base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
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

VMutex dlllistmutex;   //protects linked lists of heaps

//******************************************************************************
//******************************************************************************
Win32DllBase::Win32DllBase(HINSTANCE hinstance, WIN32DLLENTRY DllEntryPoint) 
                 : Win32ImageBase(hinstance),
  	           referenced(0), fSkipEntryCalls(FALSE), 
                   fAttachedToProcess(FALSE), fUnloaded(FALSE)
{
  dllEntryPoint = DllEntryPoint;

  dlllistmutex.enter();
  next = head;
  head = this;
  dlllistmutex.leave();

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
 BOOL rc;

  if(fAttachedToProcess)
	return TRUE;
  
  fAttachedToProcess = TRUE;

  //Allocate TLS index for this module
  tlsAlloc();
  tlsAttachThread();	//setup TLS (main thread)

  if(fSkipEntryCalls || dllEntryPoint == NULL) {
        dprintf(("attachProcess not required for dll %s", szModule));
	return(TRUE);
  }

  dprintf(("attachProcess to dll %s", szModule));

  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);

  sel = SetWin32TIB();
  rc = dllEntryPoint(hinstance, DLL_PROCESS_ATTACH, 0);
  SetFS(sel);

  OS2UnsetExceptionHandler((void *)&exceptFrame);

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
  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);

  rc = dllEntryPoint(hinstance, DLL_THREAD_ATTACH, 0);

  OS2UnsetExceptionHandler((void *)&exceptFrame);
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

  dprintf(("attachThread from dll %s", szModule));

  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);

  rc =  dllEntryPoint(hinstance, DLL_THREAD_DETACH, 0);

  OS2UnsetExceptionHandler((void *)&exceptFrame);
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
Win32DllBase *Win32DllBase::head = NULL;
