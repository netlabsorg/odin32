/* $Id: windlllx.cpp,v 1.9 2000-03-09 19:03:21 sandervl Exp $ */

/*
 * Win32 LX Dll class (compiled in OS/2 using Odin32 api)
 *
 * Copyright 1999-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: Unloading of dlls probably needs to be fixed due to OS/2 bug 
 *       (wrong unload order of dlls)
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
#include <os2newapi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <misc.h>
#include <win32type.h>
#include <windllbase.h>
#include <windlllx.h>
#include "winexepe2lx.h"
#include <odinlx.h>
#include "oslibmisc.h"

#include <exe386.h>

#define DBG_LOCALLOG	DBG_windlllx
#include "dbglocal.h"

//******************************************************************************
//Create LX Dll object and send process attach message
//System dlls set EntryPoint to 0
//Returns: Odin32 module handle
//******************************************************************************
DWORD WIN32API RegisterLxDll(HINSTANCE hInstance, WIN32DLLENTRY EntryPoint, 
                             PVOID pResData)
{
 Win32LxDll *windll;
 Win32DllBase *windlldep;

   windll = (Win32LxDll *)Win32DllBase::findModule(hInstance);
   if(windll) {
	char *name = OSLibGetDllName(hInstance);
	dprintf(("RegisterLxDll: Register existing dll %x %s", hInstance, name));
	return FALSE;
   }
   windll = new Win32LxDll(hInstance, EntryPoint, pResData);
   if(windll == NULL) {
	dprintf(("RegisterLxDll: windll == NULL!!!"));
	return FALSE;
   }
   if(fPe2Lx) {
   	windll->AddRef();

   	if(windll->attachProcess() == 0)
		return 0;

   	return windll->getInstanceHandle();
   }
   IMAGE_DOS_HEADER doshdr;
   struct e32_exe   lxhdr;
   ULONG            offset;
   char             modulename[CCHMAXPATH];
   char             modsize;
   APIRET           rc;
   int              i;

   //SvL: This code reads the import name table of the dll to get the dependencies
   //     on other dlls.
   //DosQueryHeaderInfo is an undocumented api, but works very well.
   //(no need to save FS here as we'll return to OS/2 immediately)
   rc = DosQueryHeaderInfo(hInstance, 0, &doshdr, sizeof(IMAGE_DOS_HEADER), QHINF_READFILE);
   if(rc) {
	goto hdrerror;
   }
   rc = DosQueryHeaderInfo(hInstance, doshdr.e_lfanew, &lxhdr, sizeof(e32_exe), QHINF_READFILE);
   if(rc) {
	goto hdrerror;
   }
   offset = doshdr.e_lfanew + lxhdr.e32_impmod;
   for(i=0;i<lxhdr.e32_impmodcnt;i++) {
   	rc = DosQueryHeaderInfo(hInstance, offset, &modsize, 1, QHINF_READFILE);
   	if(rc) {
		goto hdrerror;
   	}
   	rc = DosQueryHeaderInfo(hInstance, offset+1, &modulename, min(modsize, sizeof(modulename)), QHINF_READFILE);
   	if(rc) {
		goto hdrerror;
   	}
	modulename[modsize] = 0;
   	windlldep = Win32DllBase::findModule(modulename, TRUE);
   	if(windlldep) {
		dprintf(("RegisterLxDll: Add dependency %s -> %s", windll->getModuleName(), modulename));
		windll->addDependency(windlldep);
	}
	offset += modsize + 1;
   }
   return windll->getInstanceHandle();

hdrerror:
   dprintf(("DosQueryHeaderInfo returned %d", rc));
   return windll->getInstanceHandle();
}
//******************************************************************************
//Destroy LX Dll object
//******************************************************************************
BOOL WIN32API UnregisterLxDll(HINSTANCE hInstance)
{
 Win32LxDll *windll;

   //Don't proceed for pe2lx/win32k (os/2 dll unload dependency bug)
   //Don't do it either after ExitProcess has been called
   if(fPe2Lx || WinExe == NULL)
   	return TRUE;

   windll = (Win32LxDll *)Win32DllBase::findModule(hInstance);
   if(!windll) {
	dprintf(("UnregisterLxDll: Can't find dll with handle %x (already deleted)", hInstance));
	return TRUE; //already deleted by Win32LxDll::Release
   }
   dprintf(("UnregisterLxDll %s", windll->getModuleName()));
   //This can only happen for LX dependencies (i.e. wininet loads wsock32)
   delete windll;
   return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32LxDll::Win32LxDll(HINSTANCE hInstance, WIN32DLLENTRY EntryPoint, PVOID pResData) 
                : Win32ImageBase(hInstance),
                  Win32LxImage(hInstance, pResData), 
                  Win32DllBase(hInstance, EntryPoint)
{
  if(EntryPoint == NULL) {
  	fSkipEntryCalls    = TRUE;
  	fAttachedToProcess = TRUE;
  }
}
//******************************************************************************
//******************************************************************************
Win32LxDll::~Win32LxDll()
{
}
//******************************************************************************
//Load it again so OS/2 takes care of the reference count (to make sure
//a dll isn't unloaded when the win32 app still needs it)
//******************************************************************************
void Win32LxDll::loadLibrary()
{
 char   szModuleFailure[CCHMAXPATH] = "";
 ULONG  hInstanceNewDll;
 APIRET rc;

  if(fLoadLibrary) {
	DebugInt3();
	return;
  }

  dprintf(("Win32LxDll::loadLibrary %s", getModuleName()));
  rc = DosLoadModule(szModuleFailure, sizeof(szModuleFailure), getFullPath(), (HMODULE *)&hInstanceNewDll);
  if(rc) {
	dprintf(("DosLoadModule returned %X for %s\n", rc, szModuleFailure));
	DebugInt3(); //should NEVER happen
	return;
  }
  //only do this once, so set the fLoadLibrary flag to true
  setLoadLibrary();
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
ULONG Win32LxDll::AddRef(char *parentname)
#else
ULONG Win32LxDll::AddRef()
#endif
{
 Win32DllBase *dll;
 QueueItem    *item;
 ULONG         ret;

#ifdef DEBUG
  ret = Win32DllBase::AddRef(parentname);
#else
  ret = Win32DllBase::AddRef();
#endif

  if(fPe2Lx)
	return ret;

  if(referenced == 1) 
  {
  	item = loadedDlls.Head();
  	while(item) {
		dll = (Win32DllBase *)loadedDlls.getItem(item);
		if(dll == NULL) {
			dprintf(("ERROR: Win32DllBase::AddRef: dll item == NULL!!"));
			DebugInt3();
			return -1;
		}
#ifdef DEBUG
		dll->AddRef(getModuleName());
#else
		dll->AddRef();
#endif
		item = loadedDlls.getNext(item);
	}
   	if(attachProcess() == 0)
		return 0;
  }
  return ret;  
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxDll::Release()
{ 
 HINSTANCE hinst;
 ULONG     ret;
 APIRET    rc;
 BOOL      fLoadLib = fLoadLibrary;

  if(fDisableUnload) {//only set for kernel32.dll
	fLoadLib = FALSE;
  }
  hinst = hinstance;
  ret = Win32DllBase::Release();
  if(ret == 0 && fLoadLib) {
	//DosFreeModule sends a termination message to the dll.
        //The LX dll informs us when it's removed (UnregisterDll call)
	rc = DosFreeModule(hinst);
	if(rc) {
		dprintf(("Win32LxDll::Release: DosFreeModule %x returned %d", hinst, rc));
	}
  }
  return(ret);
}
//******************************************************************************
//******************************************************************************
BOOL Win32LxDll::isLxDll()
{
  return TRUE;
}
//******************************************************************************
//******************************************************************************
