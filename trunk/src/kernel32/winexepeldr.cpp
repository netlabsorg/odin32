/* $Id: winexepeldr.cpp,v 1.10 2000-09-12 04:29:59 bird Exp $ */

/*
 * Win32 PE loader Exe class
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
#include <winexepeldr.h>
#include <wprocess.h>
#include <pefile.h>

#include "conwin.h"          // Windows Header for console only
#include "console.h"

#include "exceptions.h"
#include "exceptutil.h"

#include "cio.h"
#include "oslibmisc.h"

#define DBG_LOCALLOG	DBG_winexepeldr
#include "dbglocal.h"

extern char szErrorTitle[];
extern char szErrorModule[];

BOOL fPeLoader = FALSE;

//******************************************************************************
//Called by ring 3 pe loader to create win32 executable
//******************************************************************************
BOOL WIN32API CreateWin32PeLdrExe(char *szFileName, char *szCmdLine, ULONG reservedMem)
{
 APIRET  rc;
 PPIB   ppib;
 PTIB   ptib;
 WINEXCEPTION_FRAME exceptFrame;
 Win32PeLdrExe *WinExe;
 char   *szFullCmdLine;

  fPeLoader = TRUE;

  WinExe = new Win32PeLdrExe(szFileName);

  rc = DosGetInfoBlocks(&ptib, &ppib);
  if(rc) {
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szInteralErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        delete WinExe;
        return FALSE;
  }

  /* Initiate the commandline.
   * (Since PE.EXE takes no options we'll currently just skip the
   * first parameter and used the passed in executable name.)
   */
  szCmdLine = szCmdLine; /* ignored */
  InitCommandLine(szFileName);

  if(getenv("WIN32_IOPL2")) {
    io_init1();
  }

  OS2SetExceptionHandler(&exceptFrame);
  if(WinExe->init(reservedMem) == FALSE) {
	if(szErrorModule[0] != 0) {
	  char szErrorMsg[128];

		sprintf(szErrorMsg, "Can't execute %s due to bad or missing %s", OSLibStripPath(szFileName), szErrorModule);
        	WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
	}
        delete WinExe;
        return FALSE;
  }

  OS2UnsetExceptionHandler(&exceptFrame);
  if(WinExe->isConsoleApp()) {
   	dprintf(("Console application!\n"));

   	APIRET rc = iConsoleInit();                     /* initialize console subsystem */
   	if (rc != NO_ERROR)                                  /* check for errors */
            	dprintf(("KERNEL32:Win32Image:Init ConsoleInit failed with %u.\n", rc));
  }

  WinExe->start();

  delete WinExe;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32PeLdrExe::Win32PeLdrExe(char *szFileName) :
                   Win32ImageBase(-1),
		   Win32ExeBase(-1),
		   Win32PeLdrImage(szFileName, TRUE)
{
  dprintf(("Win32PeLdrExe ctor: %s", szFileName));
}
//******************************************************************************
//******************************************************************************
Win32PeLdrExe::~Win32PeLdrExe()
{
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrExe::init(ULONG reservedMem)
{
 BOOL rc;

  rc = Win32PeLdrImage::init(reservedMem);
  fConsoleApp = (oh.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI);
  return rc;
}
//******************************************************************************
//******************************************************************************
