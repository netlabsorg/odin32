/* $Id: windllpe2lx.cpp,v 1.1 1999-09-15 23:39:07 sandervl Exp $ */

/*
 * Win32 PE2LX Dll class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <windllpe2lx.h>
#include <wprocess.h>
#include "cio.h"
#include "oslibmisc.h"
#include "oslibdos.h"

#include "conwin.h"          // Windows Header for console only
#include "console.h"

//******************************************************************************
//******************************************************************************
ULONG WIN32API RegisterPe2LxDll(WIN32DLLENTRY pfnDllEntry, PIMAGE_TLS_CALLBACK *TlsCallbackAddr,
            		        LPDWORD TlsIndexAddr, ULONG TlsInitSize,
            		        ULONG TlsTotalSize, LPVOID TlsAddress,
                  	        LONG Win32TableId, LONG NameTableId, LONG VersionResId,
                     	        LONG Pe2lxVersion, HINSTANCE hinstance, ULONG dwAttachType)
{
 char *name;

  Win32Pe2LxDll *winmod = (Win32Pe2LxDll *)Win32DllBase::findModule(hinstance);
  if(dwAttachType == 0)
  { //Process attach
   	if(getenv("WIN32_IOPL2")) {
         	io_init1();
   	}
   	name = OSLibGetDllName(hinstance);
   	CheckVersion(Pe2lxVersion, name);

   	dprintf(("RegisterDll %X %s reason %d\n", hinstance, name, dwAttachType));
   	dprintf(("RegisterDll Win32TableId = %x", Win32TableId));
   	dprintf(("RegisterDll NameTableId  = %x", NameTableId));
   	dprintf(("RegisterDll VersionResId = %x", VersionResId));
   	dprintf(("RegisterDll Pe2lxVersion = %x", Pe2lxVersion));

	//converted win32 dll loaded by OS/2 loader
	winmod = new Win32Pe2LxDll(hinstance, NameTableId, Win32TableId, pfnDllEntry);
	if(winmod == NULL) {
      		eprintf(("Failed to allocate module object!\n"));
       		DebugInt3();
       		return 0;                    //fail dll load
   	}
   	winmod->setTLSAddress(TlsAddress);
   	winmod->setTLSInitSize(TlsInitSize);
   	winmod->setTLSTotalSize(TlsTotalSize);
   	winmod->setTLSIndexAddr(TlsIndexAddr);
   	winmod->setTLSCallBackAddr(TlsCallbackAddr);

   	/* @@@PH 1998/03/17 console devices initialization */
   	iConsoleDevicesRegister();

   	//SvL: 19-8-'98
   	winmod->AddRef();
   	winmod->setVersionId(VersionResId);

   	winmod->attachProcess();
   }
   else {//process detach
   	if(winmod != NULL && !fFreeLibrary) {
      		return 0;   //don't unload (OS/2 dll unload bug)
   	}
	//Runtime environment could already be gone, so don't do this
	// dprintf(("KERNEL32: Dll Removed by FreeLibrary or ExitProcess\n"));
   }
   return 1;   //success
}
//******************************************************************************
//******************************************************************************
Win32Pe2LxDll::Win32Pe2LxDll(HINSTANCE hinstance, int NameTableId, int Win32TableId,
		             WIN32DLLENTRY DllEntryPoint) : 
                        Win32ImageBase(hinstance), 
			Win32DllBase(hinstance, DllEntryPoint), 
                        Win32Pe2LxImage(hinstance, NameTableId, Win32TableId)
{
  dprintf(("Win32Pe2LxDll::Win32Pe2LxDll %s", szModule));
}
//******************************************************************************
//******************************************************************************
Win32Pe2LxDll::~Win32Pe2LxDll()
{
  dprintf(("Win32Pe2LxDll::~Win32Pe2LxDll %s", szModule));
}
//******************************************************************************
//******************************************************************************
ULONG Win32Pe2LxDll::getApi(char *name)
{
  APIRET      rc;
  ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&apiaddr);
  if(rc)  
  {
      	return(0);
  }
  return(apiaddr);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Pe2LxDll::getApi(int ordinal)
{
 APIRET      rc;
 ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&apiaddr);
  if(rc)  	return(0);
  else		return(apiaddr);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Pe2LxDll::isLxDll()
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
