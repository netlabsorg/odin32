/* $Id: winexebase.cpp,v 1.8 2000-04-16 18:05:04 sandervl Exp $ */

/*
 * Win32 exe base class
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
#include <winexebase.h>
#include <windllbase.h>
#include <wprocess.h>
#include <pefile.h>
#include "exceptions.h"
#include "exceptutil.h"
#include "cio.h"

#include "conwin.h"          // Windows Header for console only
#include "console.h"

#define DBG_LOCALLOG	DBG_winexebase
#include "dbglocal.h"

BOOL         fExeStarted = FALSE;
Win32ExeBase *WinExe = NULL;

//******************************************************************************
//******************************************************************************
BOOL IsExeStarted()
{
    return fExeStarted;
}
//******************************************************************************
//******************************************************************************
Win32ExeBase::Win32ExeBase(HINSTANCE hInstance) 
                 : Win32ImageBase(hInstance),
                   fConsoleApp(FALSE),
                   cmdLineA(NULL), cmdLineW(NULL)
{
  WinExe = this;
}
//******************************************************************************
//******************************************************************************
Win32ExeBase::~Win32ExeBase()
{
 QueueItem    *item;
 Win32DllBase *dll;

  //First delete all dlls loaded by LoadLibrary 
  //Then delete all dlls that were loaded by the exe
  //(NOTE: This is what NT does; first delete loadlib dlls in LIFO order and
  //       then the exe dlls)
  Win32DllBase::deleteDynamicLibs();

  dprintf(("Win32ExeBase::~Win32ExeBase"));
#ifdef DEBUG_ENABLELOG_LEVEL2
  item = loadedDlls.Head();
  dll = (Win32DllBase *)loadedDlls.getItem(item);
  dll->printListOfDlls();
#endif

  item = loadedDlls.Head();
  while(item) {
	dll = (Win32DllBase *)loadedDlls.getItem(item);
	if(dll == NULL) {
		dprintf(("ERROR: Win32ExeBase::~Win32ExeBase: dll item == NULL!!"));
		DebugInt3();
		break;
	}
	dll->Release();
	item = loadedDlls.getNext(item);
  }

  Win32DllBase::deleteAll();

  WinExe = NULL;
  if(cmdLineA)
	free(cmdLineA);
  if(cmdLineW)
	free(cmdLineW);
}
//******************************************************************************
//******************************************************************************
ULONG Win32ExeBase::start()
{
 WINEXCEPTION_FRAME exceptFrame;
 ULONG rc;

  if(getenv("WIN32_IOPL2")) {
   	io_init1();
  }
  dprintf(("Start executable %X\n", WinExe));

  fExeStarted  = TRUE;

  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);
  SetWin32TIB();

  //Allocate TLS index for this module
  tlsAlloc();
  tlsAttachThread();	//setup TLS (main thread)

  //Set default FPU control word (no exceptions); same as in NT
  _control87(0x27F, 0xFFF);
  rc = ((WIN32EXEENTRY)entryPoint)(NULL);
  RestoreOS2TIB();

  OS2UnsetExceptionHandler((void *)&exceptFrame);

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32ExeBase::isDll()
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
void Win32ExeBase::setCommandLine(char *cline)
{
 ULONG cmdlength = strlen(cline) + 1;

  cmdLineA = (LPSTR)malloc(cmdlength);
  strcpy(cmdLineA, cline);
  cmdLineW = (LPWSTR)malloc(cmdlength*sizeof(WCHAR));
  AsciiToUnicode(cmdLineA, cmdLineW);
}
//******************************************************************************
//******************************************************************************
