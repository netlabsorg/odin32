/* $Id: winexe.cpp,v 1.4 1999-06-17 18:21:43 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 exe class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "misc.h"
#include "nameid.h"
#include "win32type.h"
#include <winexe.h>
#include "wprocess.h"
#include "pefile.h"
#include "cio.h"


// PH: to soothe the compiler
#define BY_HANDLE_FILE_INFORMATION void
#define LPFILETIME void*
#define OFSTRUCT void

#include "console.h"
#include "handlemanager.h"


Win32Exe *WinExe = NULL;

//******************************************************************************
//******************************************************************************
Win32Exe::Win32Exe(char *szFileName) : Win32Image(szFileName), fConsoleApp(FALSE),
                   cmdline(NULL), OS2InstanceHandle(-1)
{
  fConsoleApp = (oh.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI);
  WinExe = this;

  dprintf(("Win32Exe ctor: %s", szFileName));

  HMInitialize();             /* store standard handles within HandleManager */
  dprintf(("KERNEL32/WINEXE: HandleManager Initialized.\n"));
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
         fConsoleApp(FALSE), cmdline(NULL), OS2InstanceHandle(-1)
{
  HMInitialize();             /* store standard handles within HandleManager */
  dprintf(("KERNEL32/WINEXE: HandleManager Initialized.\n"));

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
  if(getenv("WIN32_IOPL2")) {
   io_init1();
  }
  dprintf(("Start executable %X\n", WinExe));

  fExeStarted  = TRUE;
  return ((WIN32EXEENTRY)entryPoint)();
}
//******************************************************************************
//******************************************************************************

