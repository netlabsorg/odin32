/* $Id: winexepe2lx.cpp,v 1.1 1999-09-15 23:39:07 sandervl Exp $ */

/*
 * Win32 PE2LX Exe class
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
#include <winexepe2lx.h>
#include <wprocess.h>
#include <pefile.h>
#include "cio.h"

#include "oslibmisc.h"
#include "oslibdos.h"

#include "conwin.h"          // Windows Header for console only
#include "console.h"

/******************************************************************************/
//******************************************************************************
void WIN32API RegisterPe2LxExe(WIN32EXEENTRY EntryPoint, PIMAGE_TLS_CALLBACK *TlsCallbackAddr,
           		       LPDWORD TlsIndexAddr, ULONG TlsInitSize,
           		       ULONG TlsTotalSize, LPVOID TlsAddress,
                 	       LONG Win32TableId, LONG NameTableId, LONG VersionResId,
                    	       LONG Pe2lxVersion, HINSTANCE hinstance, ULONG dwReserved)
{
  if(WinExe != NULL) //should never happen
    	delete(WinExe);

  CheckVersion(Pe2lxVersion, OSLibGetDllName(hinstance));

  if(getenv("WIN32_IOPL2")) {
    io_init1();
  }

  Win32Pe2LxExe *winexe;

  winexe = new Win32Pe2LxExe(hinstance, NameTableId, Win32TableId);

  if(winexe) {
   	dprintf(("RegisterExe Win32TableId = %x", Win32TableId));
   	dprintf(("RegisterExe NameTableId  = %x", NameTableId));
   	dprintf(("RegisterExe VersionResId = %x", VersionResId));
   	dprintf(("RegisterExe Pe2lxVersion = %x", Pe2lxVersion));

      	winexe->setVersionId(VersionResId);
   	winexe->setEntryPoint((ULONG)EntryPoint);
   	winexe->setTLSAddress(TlsAddress);
   	winexe->setTLSInitSize(TlsInitSize);
   	winexe->setTLSTotalSize(TlsTotalSize);
   	winexe->setTLSIndexAddr(TlsIndexAddr);
   	winexe->setTLSCallBackAddr(TlsCallbackAddr);

   	winexe->start();
  }
  else {
      	eprintf(("Win32Pe2LxExe creation failed!\n"));
      	DebugInt3();
   	return;
  }
}
//******************************************************************************
//******************************************************************************
Win32Pe2LxExe::Win32Pe2LxExe(HINSTANCE hinstance, int NameTableId, int Win32TableId) :
         Win32ImageBase(hinstance), 
         Win32ExeBase(hinstance), 
         Win32Pe2LxImage(hinstance, NameTableId, Win32TableId)
{
  if(GET_CONSOLE(Win32TableId) == 1) {//console app
   dprintf(("Console application!\n"));

   fConsoleApp = TRUE;
   APIRET rc = iConsoleInit();                     /* initialize console subsystem */
   if (rc != NO_ERROR)                                  /* check for errors */
            dprintf(("KERNEL32:Win32Image:Init ConsoleInit failed with %u.\n", rc));
  }
  WinExe = this;
}
//******************************************************************************
//******************************************************************************
Win32Pe2LxExe::~Win32Pe2LxExe()
{
}
//******************************************************************************
//******************************************************************************
