/* $Id: pe.cpp,v 1.11 1999-11-13 15:39:45 sandervl Exp $ */

/*
 * PELDR main exe loader code
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
#define INCL_DOSMODULEMGR
#define INCL_WIN
#include <os2.h>
#include <bseord.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <win32type.h>
#include <misc.h>
#include <winexepeldr.h>
#include <wprocess.h>
#include "pe.h"

char INFO_BANNER[]      = "Usage: PE winexe commandline";
char szErrorTitle[]     = "Odin";
char szLoadErrorMsg[]   = "Can't load executable";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "Executable doesn't run on x86 machines";
char szExeErrorMsg[]    = "File isn't an executable";
char szInteralErrorMsg[]= "Internal Error";
char szNoKernel32Msg[]  = "Can't load/find kernel32.dll";

char fullpath[CCHMAXPATH];

typedef HAB  (* APIENTRY WININITIALIZEPROC)(ULONG flOptions);
typedef BOOL (* APIENTRY WINTERMINATEPROC)(HAB hab);
typedef HMQ  (* APIENTRY WINCREATEMSGQUEUEPROC) (HAB hab, LONG cmsg);
typedef BOOL (* APIENTRY WINDESTROYMSGQUEUEPROC) (HMQ hmq);
typedef ULONG (* APIENTRY WINMESSAGEBOXPROC) (HWND hwndParent,
		                              HWND hwndOwner,
                		              PCSZ  pszText,
                                	      PCSZ  pszCaption,
                                	      ULONG idWindow,
                                	      ULONG flStyle);
typedef void (* KRNL32EXCEPTPROC) (void *exceptframe);

WININITIALIZEPROC      MyWinInitialize      = 0;
WINTERMINATEPROC       MyWinTerminate       = 0;
WINCREATEMSGQUEUEPROC  MyWinCreateMsgQueue  = 0;
WINDESTROYMSGQUEUEPROC MyWinDestroyMsgQueue = 0;
WINMESSAGEBOXPROC      MyWinMessageBox      = 0;
KRNL32EXCEPTPROC       Krnl32SetExceptionHandler = 0;
KRNL32EXCEPTPROC       Krnl32UnsetExceptionHandler = 0;

WIN32CTOR              CreateWin32Exe       = 0;

int main(int argc, char *argv[])
{
 HAB    hab = 0;                             /* PM anchor block handle       */
 HMQ    hmq = 0;                             /* Message queue handle         */
 char   exeName[CCHMAXPATH];
 APIRET  rc;
 HMODULE hmodPMWin = 0, hmodKernel32 = 0;

  rc = DosLoadModule(exeName, sizeof(exeName), "PMWIN.DLL", &hmodPMWin);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32INITIALIZE, NULL, (PFN *)&MyWinInitialize);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32TERMINATE, NULL, (PFN *)&MyWinTerminate);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32CREATEMSGQUEUE, NULL, (PFN *)&MyWinCreateMsgQueue);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32DESTROYMSGQUEUE, NULL, (PFN *)&MyWinDestroyMsgQueue);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32MESSAGEBOX, NULL, (PFN *)&MyWinMessageBox);

  if ((hab = MyWinInitialize(0)) == 0L) /* Initialize PM     */
	goto fail;

  hmq = MyWinCreateMsgQueue(hab, 0);
  
  if(argc < 2) {
	MyWinMessageBox(HWND_DESKTOP, NULL, INFO_BANNER, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        goto fail;
  }

  rc = DosLoadModule(exeName, sizeof(exeName), "KERNEL32.DLL", &hmodKernel32);
  if(rc) {
	MyWinMessageBox(HWND_DESKTOP, NULL, szNoKernel32Msg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        goto fail;
  }
  rc = DosQueryProcAddr(hmodKernel32, 0, "_CreateWin32PeLdrExe@8", (PFN *)&CreateWin32Exe);

  strcpy(exeName, argv[1]);
  strupr(exeName);
  if(strstr(exeName, ".EXE") == NULL) {
        strcat(exeName, ".EXE");
  }
  if(CreateWin32Exe(exeName, ReserveMem()) == FALSE) {
        goto fail;
  }

  if(hmq) MyWinDestroyMsgQueue( hmq );             /* Tidy up...                   */
  MyWinTerminate( hab );                   /* Terminate the application    */

  DosFreeModule(hmodPMWin);
  DosFreeModule(hmodKernel32);
  return 0;

fail:
  if(hmq) MyWinDestroyMsgQueue( hmq );             /* Tidy up...                   */
  if(hab) MyWinTerminate( hab );                   /* Terminate the application    */

  if(hmodPMWin) 	DosFreeModule(hmodPMWin);
  if(hmodKernel32) 	DosFreeModule(hmodKernel32);
  return(1);
}
//******************************************************************************
//******************************************************************************
