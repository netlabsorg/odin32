/* $Id: winexebase.cpp,v 1.1 1999-09-15 23:39:07 sandervl Exp $ */

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

BOOL         fExeStarted = FALSE;
Win32ExeBase *WinExe = NULL;

//******************************************************************************
//******************************************************************************
Win32ExeBase::Win32ExeBase(HINSTANCE hInstance) 
                 : Win32ImageBase(hInstance),
                   fConsoleApp(FALSE),
                   cmdline(NULL)
{
  WinExe = this;
}
//******************************************************************************
//******************************************************************************
Win32ExeBase::~Win32ExeBase()
{
  Win32DllBase::deleteAll();
  WinExe = NULL;
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

  //Allocate TLS index for this module
  tlsAlloc();
  tlsAttachThread();	//setup TLS (main thread)

  //Note: The Win32 exception structure references by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);

  SetWin32TIB();
  rc = ((WIN32EXEENTRY)entryPoint)();
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
