/* $Id: pe.cpp,v 1.6 1999-08-18 12:24:32 sandervl Exp $ */

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
#include <winexe.h>
#include <windll.h>
#include <wprocess.h>
#include "pe.h"

char INFO_BANNER[]      = "Usage: PE winexe commandline";
char szErrorTitle[]     = "Odin";
char szMemErrorMsg[]    = "Memory allocation failure";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "Executable doesn't run on x86 machines";
char szExeErrorMsg[]    = "File isn't an executable";
char szInteralErrorMsg[]= "Internal Error";

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

WININITIALIZEPROC      MyWinInitialize      = 0;
WINTERMINATEPROC       MyWinTerminate       = 0;
WINCREATEMSGQUEUEPROC  MyWinCreateMsgQueue  = 0;
WINDESTROYMSGQUEUEPROC MyWinDestroyMsgQueue = 0;
WINMESSAGEBOXPROC      MyWinMessageBox      = 0;

WIN32CTOR              CreateWin32Exe       = 0;

int main(int argc, char *argv[])
{
 HAB    hab;                             /* PM anchor block handle       */
 HMQ    hmq;                             /* Message queue handle         */
 char  *szCmdLine;
 char   exeName[CCHMAXPATH];
 PPIB   ppib;
 PTIB   ptib;
 Win32Exe *WinExe;
 APIRET  rc;
 HMODULE hmodPMWin, hmodKernel32;

  rc = DosLoadModule(exeName, sizeof(exeName), "PMWIN.DLL", &hmodPMWin);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32INITIALIZE, NULL, (PFN *)&MyWinInitialize);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32TERMINATE, NULL, (PFN *)&MyWinTerminate);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32CREATEMSGQUEUE, NULL, (PFN *)&MyWinCreateMsgQueue);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32DESTROYMSGQUEUE, NULL, (PFN *)&MyWinDestroyMsgQueue);
  rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32MESSAGEBOX, NULL, (PFN *)&MyWinMessageBox);

  rc = DosLoadModule(exeName, sizeof(exeName), "KERNEL32.DLL", &hmodKernel32);
  rc = DosQueryProcAddr(hmodKernel32, 0, "CreateWin32Exe", (PFN *)&CreateWin32Exe);

  if ((hab = MyWinInitialize(0)) == 0L) /* Initialize PM     */
	return(1);

  hmq = MyWinCreateMsgQueue(hab, 0);
  
  if(argc < 2) {
	MyWinMessageBox(HWND_DESKTOP, NULL, INFO_BANNER, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
	return(0);
  }

  strcpy(exeName, argv[1]);
  strupr(exeName);
  if(strstr(exeName, ".EXE") == NULL) {
        strcat(exeName, ".EXE");
  }
  WinExe = CreateWin32Exe(exeName);
  if(WinExe == NULL) {
        MyWinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szMemErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        return(1);
  }
  rc = DosGetInfoBlocks(&ptib, &ppib);
  if(rc) {
        MyWinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szInteralErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        delete WinExe;
        return(1);
  }
  szCmdLine = ppib->pib_pchcmd;
  while(*szCmdLine == ' ' && *szCmdLine )       //skip leading spaces
        szCmdLine++;
  while(*szCmdLine != ' ' && *szCmdLine )       //skip pe (.exe)
        szCmdLine++;
  while(*szCmdLine == ' ' && *szCmdLine )       //skip spaces
        szCmdLine++;

  ULONG curdisk, curlogdisk, flength = CCHMAXPATH;

  DosQueryCurrentDisk(&curdisk, &curlogdisk);
  DosQueryCurrentDir(curdisk, &fullpath[3], &flength);
  fullpath[0] = 'A' + (curdisk-1);
  fullpath[1] = ':';
  fullpath[2] = '\\';
  strcat(fullpath, "\\");
  strcat(fullpath, exeName);
  WinExe->setFullPath(fullpath);
  WinExe->setCommandLine(szCmdLine);

  if(WinExe->init(ReserveMem()) == FALSE) {
        delete WinExe;
        return(1);
  }
  WinExe->start();

  delete WinExe;

  if(hmq) MyWinDestroyMsgQueue( hmq );             /* Tidy up...                   */
  MyWinTerminate( hab );                   /* Terminate the application    */

  DosFreeModule(hmodPMWin);
  DosFreeModule(hmodKernel32);
  return(0);
}
//******************************************************************************
//******************************************************************************
