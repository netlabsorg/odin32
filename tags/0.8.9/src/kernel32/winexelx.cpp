/* $Id: winexelx.cpp,v 1.13 2004-01-15 10:39:10 sandervl Exp $ */

/*
 * Win32 LX Exe class (compiled in OS/2 using Odin32 api)
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
#define INCL_DOSMODULEMGR
#define INCL_DOSSESMGR
#define INCL_WIN
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef __GNUC__
#include <iostream.h>
#include <fstream.h>
#endif
#include <misc.h>
#include <win32type.h>
#include "winexelx.h"
#include <winconst.h>
#include <win32api.h>
#include <wprocess.h>
#include <odinlx.h>

#include "exceptions.h"
#include "exceptutil.h"

#include "console.h"

#define DBG_LOCALLOG	DBG_winexelx
#include "dbglocal.h"

//******************************************************************************
//Create LX Exe object and call entrypoint
//System dlls set EntryPoint to 0
//******************************************************************************
BOOL WIN32API RegisterLxExe(WINMAIN EntryPoint, PVOID pResData)
{
 APIRET  rc;
 PPIB   ppib;
 PTIB   ptib;

  if (fForceWin32TIB) {
      fSwitchTIBSel = TRUE;
  } else {
      //Signal to TEB management that we're a real OS/2 app and don't
      //require setting FS to our special win32 selector
      fSwitchTIBSel = FALSE;
  }

  //We're an OS/2 app
  fIsOS2Image = TRUE;

  if(WinExe != NULL) //should never happen
    	delete(WinExe);

  rc = DosGetInfoBlocks(&ptib, &ppib);
  if(rc) {
        return FALSE;
  }

  Win32LxExe *winexe;

  winexe = new Win32LxExe(ppib->pib_hmte, pResData);

  if(winexe) {
        InitCommandLine(FALSE);
   	winexe->setEntryPoint((ULONG)EntryPoint);
   	winexe->start();
  }
  else {
      	eprintf(("Win32LxExe creation failed!\n"));
      	DebugInt3();
   	return FALSE;
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32LxExe::Win32LxExe(HINSTANCE hInstance, PVOID pResData)
                 : Win32ImageBase(hInstance),
                   Win32LxImage(hInstance, pResData),
		   Win32ExeBase(hInstance)
{
  dprintf(("Win32LxExe ctor: %s", szModule));
  hinstance = (HINSTANCE)buildHeader(1, 0, IMAGE_SUBSYSTEM_WINDOWS_GUI);

  PPIB ppib;
  if (DosGetInfoBlocks(NULL, &ppib) == 0)
  {
    char buf[CCHMAXPATH];
    if (DosQueryModuleName(ppib->pib_hmte, sizeof(buf), buf) == 0)
    {
      ULONG Flags;
      if (DosQueryAppType(buf, &Flags) == 0)
      {
        dprintf(("Win32LxExe ctor: app type %x", Flags));
        if ((Flags & 0x2) == FAPPTYP_WINDOWCOMPAT)
          iConsoleInit(TRUE);
      }
    }
  }
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
Win32LxExe::~Win32LxExe()
{
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxExe::start()
{
 WINEXCEPTION_FRAME exceptFrame;
 ULONG rc;

  dprintf(("Start executable %X\n", WinExe));

  fExeStarted  = TRUE;

  //Allocate TLS index for this module
  tlsAlloc();
  tlsAttachThread();	//setup TLS (main thread)

  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);

  SetWin32TIB();
  rc = ((WINMAIN)entryPoint)(hinstance, 0, (LPSTR)GetCommandLineA(), SW_SHOWNORMAL_W);
  RestoreOS2TIB();

  OS2UnsetExceptionHandler((void *)&exceptFrame);

  ExitProcess(rc);
  return rc;
}
//******************************************************************************
//******************************************************************************
