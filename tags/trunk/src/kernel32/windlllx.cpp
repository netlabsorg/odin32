/* $Id: windlllx.cpp,v 1.2 1999-09-18 17:47:10 sandervl Exp $ */

/*
 * Win32 LX Dll class (compiled in OS/2 using Odin32 api)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <misc.h>
#include <win32type.h>
#include <windllbase.h>
#include <windlllx.h>
#include <odinlx.h>
#include "oslibmisc.h"

//******************************************************************************
//Create LX Dll object and send process attach message
//System dlls set EntryPoint to 0
//******************************************************************************
BOOL WIN32API RegisterLxDll(HINSTANCE hInstance, WIN32DLLENTRY EntryPoint, 
                            PVOID pResData)
{
 Win32LxDll *windll;

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
   return windll->attachProcess();
}
//******************************************************************************
//Destroy LX Dll object
//******************************************************************************
BOOL WIN32API UnregisterLxDll(HINSTANCE hInstance)
{
#if 1
   return TRUE;
#else
 Win32LxDll *windll;

   windll = (Win32LxDll *)Win32DllBase::findModule(hInstance);
   if(!windll) {
	dprintf(("UnregisterLxDll: Can't find dll with handle %x!!!", hInstance));
	return FALSE;
   }
   windll->detachProcess();
   delete windll;
   return TRUE;
#endif
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
  dprintf(("Win32LxDll::~Win32LxDll %s", szModule));
}
//******************************************************************************
//ASSUMPTION: called by FreeLibrary
//******************************************************************************
ULONG Win32LxDll::Release()
{ 
 ULONG ret = --referenced;

  if(ret == 0) {
	dprintf(("Win32LxDll::Release, referenced == 0\n"));
	//DosFreeModule sends a termination message to the dll.
        //The LX dll informs us when it's removed (UnregisterDll call)
	DosFreeModule(hinstance);
  }
  return(ret);
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxDll::getApi(char *name)
{
  APIRET      rc;
  ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&apiaddr);
  if(rc)  
  {
	if(rc == ERROR_INVALID_HANDLE) 
        {//handle invalid for some silly reason, so load module again (initterm entrypoint not called twice)
		char szErrName[CCHMAXPATH];

		rc = DosLoadModule(szErrName, sizeof(szErrName), szFileName, &hinstance);
		if(!rc)
			rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&apiaddr);
	}
	if(rc) 	return(0);
  }
  return(apiaddr);
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxDll::getApi(int ordinal)
{
 APIRET      rc;
 ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&apiaddr);
  if(rc) {
	if(rc == ERROR_INVALID_HANDLE) 
        {//handle invalid for some silly reason, so load module again (initterm entrypoint not called twice)
		char szErrName[CCHMAXPATH];

		rc = DosLoadModule(szErrName, sizeof(szErrName), szFileName, &hinstance);
		if(!rc)
			rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&apiaddr);
	}
	if(rc) 	return(0);
  }
  return(apiaddr);
}
//******************************************************************************
//******************************************************************************
BOOL Win32LxDll::isLxDll()
{
  return TRUE;
}
//******************************************************************************
//******************************************************************************
