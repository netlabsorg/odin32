/* $Id: winexe.cpp,v 1.10 1999-08-22 11:11:11 sandervl Exp $ */

/*
 * Win32 exe class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <nameid.h>
#include <win32type.h>
#include <winexe.h>
#include <wprocess.h>
#include <pefile.h>
#include "exceptions.h"
#include "exceptutil.h"
#include "cio.h"

#include "conwin.h"          // Windows Header for console only
#include "console.h"

Win32Exe *WinExe = NULL;

//******************************************************************************
//Called by ring 3 pe loader to create win32 executable
//******************************************************************************
Win32Exe* WIN32API CreateWin32Exe(char *szFileName) 
{
  return new Win32Exe(szFileName);
}
//******************************************************************************
//******************************************************************************
Win32Exe::Win32Exe(char *szFileName) : Win32Image(szFileName), fConsoleApp(FALSE),
                   cmdline(NULL)
{
  fConsoleApp = (oh.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI);
  WinExe = this;

  dprintf(("Win32Exe ctor: %s", szFileName));

  if(fConsoleApp) {
   dprintf(("Console application!\n"));

   APIRET rc = iConsoleInit();                     /* initialize console subsystem */
   if (rc != NO_ERROR)                                  /* check for errors */
            dprintf(("KERNEL32:Win32Image:Init ConsoleInit failed with %u.\n", rc));
  }
}
//******************************************************************************
//******************************************************************************
Win32Exe::Win32Exe(HINSTANCE hinstance, int NameTableId, int Win32TableId) :
         Win32Image(hinstance, NameTableId, Win32TableId),
         fConsoleApp(FALSE), cmdline(NULL)
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
Win32Exe::~Win32Exe()
{
  Win32Dll::deleteAll();
  WinExe = NULL;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Exe::start()
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

