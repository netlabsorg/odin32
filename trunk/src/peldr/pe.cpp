/* $Id: pe.cpp,v 1.3 1999-06-19 10:54:46 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * PELDR main exe loader code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <string.h>
#include <assert.h>
#include "win32type.h"
#include "misc.h"
#include "winexe.h"
#include "windll.h"

char INFO_BANNER[]      = "Usage: PE winexe commandline";
char szErrorTitle[]     = "Odin";
char szMemErrorMsg[]    = "Memory allocation failure";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "Executable doesn't run on x86 machines";
char szExeErrorMsg[]    = "File isn't an executable";
char szInteralErrorMsg[]= "Internal Error";

char fullpath[CCHMAXPATH];

void UpCase(char *mixedcase);

int main(int argc, char *argv[])
{
 HAB    hab;                             /* PM anchor block handle       */
 HMQ    hmq;                             /* Message queue handle         */
 char  *szCmdLine;
 char   exeName[CCHMAXPATH];
 PPIB   ppib;
 PTIB   ptib;
 Win32Exe *WinExe;
 APIRET rc;

  if ((hab = WinInitialize(0)) == 0L) /* Initialize PM     */
        return(1);

  hmq = WinCreateMsgQueue(hab, 0);

  if(argc < 2) {
        WinMessageBox(HWND_DESKTOP, NULL, INFO_BANNER, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        return(0);
  }

  strcpy(exeName, argv[1]);
  UpCase(exeName);
  if(strstr(exeName, ".EXE") == NULL) {
        strcat(exeName, ".EXE");
  }
  WinExe = new Win32Exe(exeName);
  if(WinExe == NULL) {
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szMemErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        return(1);
  }
  rc = DosGetInfoBlocks(&ptib, &ppib);
  if(rc) {
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szInteralErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
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

  if(WinExe->init() == FALSE) {
        delete WinExe;
        return(1);
  }
  WinExe->start();
  delete WinExe;

  if(hmq) WinDestroyMsgQueue( hmq );             /* Tidy up...                   */
  WinTerminate( hab );                   /* Terminate the application    */
  return(0);
}
//******************************************************************************
//******************************************************************************
void UpCase(char *mixedcase)
{
 int i;

  for(i=0;i<strlen(mixedcase);i++) {
        if(mixedcase[i] >= 'a' && mixedcase[i] <= 'z') {
                mixedcase[i] += 'A' - 'a';
        }
  }
}
//******************************************************************************
//******************************************************************************
