/* $Id: initterm.cpp,v 1.5 2000-01-26 11:26:36 sandervl Exp $ */

/*
 * PELDR DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*-------------------------------------------------------------*/
/* INITERM.C -- Source for a custom dynamic link library       */
/*              initialization and termination (_DLL_InitTerm) */
/*              function.                                      */
/*                                                             */
/* When called to perform initialization, this sample function */
/* gets storage for an array of integers, and initializes its  */
/* elements with random integers.  At termination time, it     */
/* frees the array.  Substitute your own special processing.   */
/*-------------------------------------------------------------*/


/* Include files */
#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSMISC
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <winimagepeldr.h>

#ifndef PAG_ANY
    #define PAG_ANY    0x00000400
#endif

#ifndef QSV_VIRTUALADDRESSLIMIT
    #define QSV_VIRTUALADDRESSLIMIT 30
#endif

#define FALLOC_SIZE (1024*1024)

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);

ULONG reservedMemory = 0;

void AllocateExeMem(char *filename);

/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
unsigned long _System _DLL_InitTerm(unsigned long hModule, unsigned long
                                    ulFlag)
{
   size_t i;
   APIRET rc;

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
   case 0 :
   {
	PTIB ptib;
	PPIB ppib;
	char *win32app;

         /*******************************************************************/
         /* The C run-time environment initialization function must be      */
         /* called before any calls to C run-time functions that are not    */
         /* inlined.                                                        */
         /*******************************************************************/

	 DosGetInfoBlocks(&ptib, &ppib);
	 win32app = ppib->pib_pchcmd + strlen(ppib->pib_pchcmd) + 1;
	 AllocateExeMem(win32app);

         /*******************************************************************/
         /* A DosExitList routine must be used to clean up if runtime calls */
         /* are required and the runtime is dynamically linked.             */
         /*******************************************************************/
         break;
   }
   case 1 :
         break;
   default  :
         return 0UL;
   }

   /***********************************************************/
   /* A non-zero value must be returned to indicate success.  */
   /***********************************************************/
   return 1UL;
}
//******************************************************************************
//******************************************************************************
ULONG SYSTEM ReserveMem()
{
   return reservedMemory;
}
//******************************************************************************
//SvL: Reserve memory for win32 exes without fixups
//     This is done before any Odin or PMWIN dll is loaded, so we'll get
//     a very low virtual address. (which is exactly what we want)
//******************************************************************************
void AllocateExeMem(char *filename)
{
 HFILE  dllfile;
 char   szFileName[CCHMAXPATH], *tmp;
 ULONG  action, ulRead, signature;
 APIRET rc;
 IMAGE_DOS_HEADER doshdr;
 IMAGE_OPTIONAL_HEADER oh;
 IMAGE_FILE_HEADER     fh;
 ULONG  address = 0;
 ULONG  *memallocs;
 ULONG  alloccnt = 0;
 ULONG  diff, i, baseAddress;
 ULONG  ulSysinfo, flAllocMem;

  strcpy(szFileName, filename);
  tmp = szFileName;
  while(*tmp != ' ' && *tmp != 0)
	tmp++;
  *tmp = 0;

  if(!strchr(szFileName, '.')) {
	strcat(szFileName,".EXE");
  }
  rc = DosOpen(szFileName, &dllfile, &action, 0, FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_FAIL_IF_NEW, OPEN_SHARE_DENYNONE|OPEN_ACCESS_READONLY, NULL);
  if(rc != 0) {
	if(!strstr(szFileName, ".EXE")) {
		strcat(szFileName,".EXE");
	}
  }
  else	DosClose(dllfile);

  rc = DosOpen(szFileName, &dllfile, &action, 0, FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_FAIL_IF_NEW, OPEN_SHARE_DENYNONE|OPEN_ACCESS_READONLY, NULL);
  if(rc) {
	return; //oops
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
  if(!(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)) {
	goto end; //no need to allocate anything now
  }

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

  if(oh.ImageBase < 512*1024*124) {
	flAllocMem = 0;
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
  DosClose(dllfile);
  return;
}
//******************************************************************************
//******************************************************************************
