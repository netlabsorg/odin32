/* $Id: winexepeldr.cpp,v 1.16 2001-10-12 00:49:23 phaller Exp $ */

/*
 * Win32 PE loader Exe class
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <win32api.h>
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


#ifdef PROFILE
#include <perfview.h>
#endif /* PROFILE */


extern char szErrorTitle[];
extern char szErrorModule[];

BOOL fPeLoader = FALSE;

//******************************************************************************
//Called by ring 3 pe loader to create win32 executable
//PE.EXE command line options:
//   /OPT:[x1=y,x2=z,..]	 
//   x = CURDIR    -> set current directory to y
//   (not other options available at this time)
//******************************************************************************
BOOL WIN32API CreateWin32PeLdrExe(char *szFileName, char *szCmdLine,
                                  char *peoptions, 
                                  ULONG reservedMem, BOOL fConsoleApp,
                                  BOOL  fVioConsole)
{
 APIRET  rc;
 PPIB   ppib;
 PTIB   ptib;
 WINEXCEPTION_FRAME exceptFrame;
 Win32PeLdrExe *WinExe;
 char   *szFullCmdLine;

  fPeLoader = TRUE;

  WinExe = new Win32PeLdrExe(szFileName, fConsoleApp);

  rc = DosGetInfoBlocks(&ptib, &ppib);
  if(rc) {
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szInteralErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        delete WinExe;
        return FALSE;
  }
  //Handle special pe cmd line options here (/OPT:[x1=y,x2=z,..])
  if(peoptions) {
	char *option;

	option = strchr(peoptions, '[');
	if(option) {
		option++;
		option = strstr(option, "CURDIR=");
		if(option) {
			char *curdir, *tmp;
			int   curdirlength;

			option += 7;
			tmp = option;
			while(*tmp != ']' && *tmp != ',' && *tmp != 0) {
				tmp++;
			}
			curdirlength = (int)(tmp-option);
			curdir = (char *)malloc(curdirlength+1);
			memcpy(curdir, option, curdirlength);
			curdir[curdirlength] = 0;
			SetCurrentDirectoryA((LPCSTR)curdir);
			free(curdir);
		}
	}
  }
  //exe length + space + (possibly) 2x'"' + cmd line length + 0 terminator
  szFullCmdLine = (char *)malloc(strlen(szFileName) + 3 + strlen(szCmdLine) + 1);
  //Enclose executable name in quotes if it (or it's directory) contains spaces
  if(strchr(szFileName, ' ') != NULL) {
	sprintf(szFullCmdLine, "\"%s\"", szFileName);
  }
  else 	strcpy(szFullCmdLine, szFileName);
  strcat(szFullCmdLine, " ");
  strcat(szFullCmdLine, szCmdLine);
  InitCommandLine(szFullCmdLine);
  dprintf(("Cmd line: %s", szFullCmdLine));
  free(szFullCmdLine);

  if(getenv("WIN32_IOPL2")) {
        io_init1();
  }
  //Init console before loading executable as dlls might want to print 
  //something on the console while being loaded
  if(WinExe->isConsoleApp()) 
  {
   	dprintf(("Console application!\n"));

   	APIRET rc = iConsoleInit(fVioConsole);                /* initialize console subsystem */
   	if (rc != NO_ERROR)                                  /* check for errors */
            	dprintf(("KERNEL32:Win32Image:Init ConsoleInit failed with %u.\n", rc));
  }

  OS2SetExceptionHandler(&exceptFrame);
  if(WinExe->init(reservedMem) == FALSE) 
  {
	if(szErrorModule[0] != 0) {
   	        char szErrorMsg[128];

		sprintf(szErrorMsg, "Can't execute %s due to bad or missing %s", OSLibStripPath(szFileName), szErrorModule);
        	WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
	}
        delete WinExe;
	OS2UnsetExceptionHandler(&exceptFrame);
        return FALSE;
  }
  OS2UnsetExceptionHandler(&exceptFrame);
  
#ifdef PROFILE
  // Note: after this point, we might start collecting performance
  // information about the called functions.
  PerfView_Initialize();
#endif /* PROFILE */
  
  
  WinExe->start();

  delete WinExe;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32PeLdrExe::Win32PeLdrExe(char *szFileName, BOOL fConsoleApp) :
                   Win32ImageBase(-1),
		   Win32ExeBase(-1),
		   Win32PeLdrImage(szFileName, TRUE)
{
  dprintf(("Win32PeLdrExe ctor: %s", szFileName));
  this->fConsoleApp = fConsoleApp;

  //SvL: set temporary full path here as console init needs it
  setFullPath(szFileName);
}
//******************************************************************************
//******************************************************************************
Win32PeLdrExe::~Win32PeLdrExe()
{
    fExitProcess = TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrExe::init(ULONG reservedMem)
{
 BOOL rc;

  rc = Win32PeLdrImage::init(reservedMem);
  return rc;
}
//******************************************************************************
//******************************************************************************
