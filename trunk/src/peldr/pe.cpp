/* $Id: pe.cpp,v 1.23 2000-10-06 15:15:25 sandervl Exp $ */

/*
 * PELDR main exe loader code
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Command line options:
 *   /OPT:[x1=y,x2=z,..]	 
 *   x = CURDIR    -> set current directory to y
 *   (not other options available at this time)
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
#include <wprocess.h>
#include <win\peexe.h>
#include "pe.h"

char INFO_BANNER[]      = "Usage: PE winexe commandline";
char szErrorTitle[]     = "Odin";
char szLoadErrorMsg[]   = "Can't load executable";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "Executable doesn't run on x86 machines";
char szExeErrorMsg[]    = "File isn't an executable";
char szInteralErrorMsg[]= "Internal Error";
char szNoKernel32Msg[]  = "Can't load/find kernel32.dll (rc=%d)";
char szDosInfoBlocks[]  = "DosInfoBlocks failed!";

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

//should be the same as in ..\kernel32\winexepeldr.h
typedef BOOL (* WIN32API WIN32CTOR)(char *, char *, char *, ULONG, BOOL);

WIN32CTOR   CreateWin32Exe       = 0;
ULONG       reservedMemory       = 0;
BOOL        fConsoleApp          = FALSE;

void AllocateExeMem(char *filename);

//******************************************************************************
//******************************************************************************
int main(int argc, char *argv[])
{
 HAB    hab = 0;                             /* PM anchor block handle       */
 HMQ    hmq = 0;                             /* Message queue handle         */
 char   exeName[CCHMAXPATH];
 char   fullpath[CCHMAXPATH];
 APIRET  rc;
 HMODULE hmodPMWin = 0, hmodKernel32 = 0;
 PTIB   ptib;
 PPIB   ppib;
 char  *cmdline, *win32cmdline, *peoptions, *newcmdline;
 BOOL   fQuote = FALSE;
 int    nrTries = 1;

  if(argc >= 2) {
	if(DosGetInfoBlocks(&ptib, &ppib) == 0) {
tryagain:
		cmdline = ppib->pib_pchcmd;
		cmdline += strlen(cmdline)+1; 	//skip pe.exe
                peoptions = strstr(cmdline, "/OPT:[");
		if(peoptions) {
			newcmdline = strchr(peoptions, ']');
			if(newcmdline) {
				cmdline = newcmdline+1;
			}
			else 	DebugInt3();	//should not happen!
		}		
		while(*cmdline == ' ')	cmdline++; //skip leading space
		if(*cmdline == '"') {
			cmdline++;
			fQuote = TRUE;
		}
		win32cmdline = cmdline;

		strncpy(exeName, cmdline, sizeof(exeName)-1);
		exeName[sizeof(exeName)-1] = 0;
		char *p = exeName;
		if(fQuote) {
			while(*p != '"' && *p != 0) p++;
		}
		else {
			for(int i=0;i<nrTries;i++) {
				while(*p != ' ' && *p != 0) p++;
				if(*p == 0) break;
				if(i != nrTries-1) {
					while(*p == ' ' && *p != 0) p++;
				}
			}
		}
		*p = 0;
		strupr(exeName);
                cmdline = strstr(exeName, ".EXE");
		if(cmdline) {
			cmdline[4] = 0;
			win32cmdline += ((ULONG)cmdline - (ULONG)exeName) + 4;
		}
		else {
			win32cmdline += strlen(exeName);
			if(strstr(exeName, ".") == NULL) {
				strcat(exeName, ".EXE");
			}
 		}
		if(fQuote) win32cmdline++;
		while(*win32cmdline == ' ') win32cmdline++; //skip spaces
		if(*win32cmdline == '"') {
			win32cmdline++;
			cmdline = win32cmdline + strlen(win32cmdline) - 1;
			while(*cmdline == ' ') cmdline--;
			if(*cmdline == '"') {
				*cmdline = 0;
			}
                }
		cmdline = exeName + strlen(exeName) - 1;
		while(*cmdline == ' ') cmdline--;
		cmdline[1] = 0;
		if(DosQueryPathInfo(exeName, FIL_QUERYFULLNAME, (PVOID)fullpath, sizeof(fullpath)) == 0) {
			strcpy(exeName, fullpath);
		}
		FILESTATUS3 fstat3;
		if(DosQueryPathInfo(exeName, FIL_STANDARD, (PVOID)&fstat3, sizeof(fstat3))) 
		{
			nrTries++;
			if(*win32cmdline != NULL && !fQuote) {
				goto tryagain;
			}
		}
	}
	else {//should never happen!
filenotfound:
		DebugInt3();
  		rc = DosLoadModule(exeName, sizeof(exeName), "PMWIN.DLL", &hmodPMWin);
  		rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32MESSAGEBOX, NULL, (PFN *)&MyWinMessageBox);
		MyWinMessageBox(HWND_DESKTOP, NULL, szDosInfoBlocks, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
		goto fail;
	}
	AllocateExeMem(exeName);
  }

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
	sprintf(exeName, szNoKernel32Msg, rc);
	MyWinMessageBox(HWND_DESKTOP, NULL, exeName, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
        goto fail;
  }
  rc = DosQueryProcAddr(hmodKernel32, 0, "_CreateWin32PeLdrExe@20", (PFN *)&CreateWin32Exe);

  if(CreateWin32Exe(exeName, win32cmdline, peoptions, reservedMemory, fConsoleApp) == FALSE) {
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
//SvL: Reserve memory for win32 exes without fixups
//     This is done before any Odin or PMWIN dll is loaded, so we'll get
//     a very low virtual address. (which is exactly what we want)
//******************************************************************************
void AllocateExeMem(char *filename)
{
 HFILE  dllfile = 0;
 char   szFileName[CCHMAXPATH], *tmp;
 char   szResult[CCHMAXPATH];
 ULONG  action, ulRead, signature;
 APIRET rc;
 IMAGE_DOS_HEADER doshdr;
 IMAGE_OPTIONAL_HEADER oh;
 IMAGE_FILE_HEADER     fh;
 ULONG  address = 0;
 ULONG  *memallocs;
 ULONG  alloccnt = 0;
 ULONG  diff, i, baseAddress;
 ULONG  ulSysinfo, flAllocMem = 0;

  strcpy(szFileName, filename);

  rc = DosOpen(szFileName, &dllfile, &action, 0, FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_FAIL_IF_NEW, OPEN_SHARE_DENYNONE|OPEN_ACCESS_READONLY, NULL);
  if(rc != 0) {
	if(!strstr(szFileName, ".EXE")) {
		strcat(szFileName,".EXE");
	}
  }
  else	DosClose(dllfile);

  rc = DosOpen(szFileName, &dllfile, &action, 0, FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_FAIL_IF_NEW, OPEN_SHARE_DENYNONE|OPEN_ACCESS_READONLY, NULL);
  if(rc) {
        if(DosSearchPath(SEARCH_IGNORENETERRS|SEARCH_ENVIRONMENT, "PATH",
                         szFileName, szResult, sizeof(szResult)) != 0) {
		goto end; //oops
        }
	rc = DosOpen(szResult, &dllfile, &action, 0, FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_FAIL_IF_NEW, OPEN_SHARE_DENYNONE|OPEN_ACCESS_READONLY, NULL);
  	if(rc) {
		goto end; //oops
        }
  }

  //read dos header
  if(DosRead(dllfile, (LPVOID)&doshdr, sizeof(doshdr), &ulRead)) {
    	goto end;
  }
  if(DosSetFilePtr(dllfile, doshdr.e_lfanew, FILE_BEGIN, &ulRead)) {
    	goto end;
  }
  //read signature dword
  if(DosRead(dllfile, (LPVOID)&signature, sizeof(signature), &ulRead)) {
    	goto end;
  }
  //read pe header
  if(DosRead(dllfile, (LPVOID)&fh, sizeof(fh), &ulRead)) {
    	goto end;
  }
  //read optional header
  if(DosRead(dllfile, (LPVOID)&oh, sizeof(oh), &ulRead)) {
    	goto end;
  }
  if(doshdr.e_magic != IMAGE_DOS_SIGNATURE || signature != IMAGE_NT_SIGNATURE) {
    	goto end;
  }
  fConsoleApp = (oh.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI);

  // check for high memory support
  rc = DosQuerySysInfo(QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT, &ulSysinfo, sizeof(ulSysinfo));
  if (rc == 0 && ulSysinfo > 512)   //VirtualAddresslimit is in MB
  {
  	flAllocMem = PAG_ANY;      // high memory support. Let's use it!
  }

  //Reserve enough space to store 4096 pointers to 1MB memory chunks
  memallocs = (ULONG *)alloca(4096*sizeof(ULONG *));
  if(memallocs == NULL) {
	goto end; //oops
  }

  if(oh.ImageBase < 512*1024*1024) {
	flAllocMem = 0;
  }
  else {
	if(flAllocMem == 0) {
		goto end; //no support for > 512 MB
	}
  }
  while(TRUE) {
    	rc = DosAllocMem((PPVOID)&address, FALLOC_SIZE, PAG_READ | flAllocMem);
    	if(rc) break;

    	if(address + FALLOC_SIZE >= oh.ImageBase) {
        	if(address > oh.ImageBase) {//we've passed it!
            		DosFreeMem((PVOID)address);
            		break;
        	}
        	//found the right address
        	DosFreeMem((PVOID)address);

        	diff = oh.ImageBase - address;
        	if(diff) {
            		rc = DosAllocMem((PPVOID)&address, diff, PAG_READ | flAllocMem);
            		if(rc) break;
        	}
        	rc = DosAllocMem((PPVOID)&baseAddress, oh.SizeOfImage, PAG_READ | PAG_WRITE | flAllocMem);
        	if(rc) break;

        	if(diff) DosFreeMem((PVOID)address);

        	reservedMemory = baseAddress;
        	break;
    	}
	memallocs[alloccnt++] = address;
  }
  for(i=0;i<alloccnt;i++) {
    	DosFreeMem((PVOID)memallocs[i]);
  }
end:
  if(dllfile) DosClose(dllfile);
  return;
}
//******************************************************************************
//******************************************************************************
