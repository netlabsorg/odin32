/* $Id: os2util.cpp,v 1.4 1999-06-19 13:57:51 sandervl Exp $ */

/*
 * Misc util. procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Peter FitzSimmons
 * Copyright 1998 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_DOSPROCESS
#define INCL_DOSSEL
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <string.h>
#include <stdlib.h>
#include <stdio.h>  /*PLF Wed  98-03-18 05:15:04*/
#include <malloc.h>  /*PLF Wed  98-03-18 05:15:04*/
#include "os2util.h"
#include "misc.h"
#include "version.h"

/***********************************
 * PH: fixups for missing os2win.h *
 ***********************************/

void _System SetLastError(ULONG ulError);

//******************************************************************************
//******************************************************************************
void OS2SetExitList(unsigned long handler)
{
 APIRET rc;

  rc = DosExitList(EXLST_ADD | 0x00002A00, (PFNEXITLIST)handler);
  if(rc) {
    dprintf(("DosExitList returned %d\n", rc));
  }
}
//******************************************************************************
//******************************************************************************
void OS2ClearExitList()
{
  DosExitList(EXLST_EXIT, NULL);
}
//******************************************************************************
//******************************************************************************
void OS2RemoveExitList(unsigned long handler)
{
  DosExitList(EXLST_REMOVE, (PFNEXITLIST)handler);
}
//******************************************************************************
//TODO: not reentrant!
//******************************************************************************
char *OS2GetDllName(ULONG hModule)
{
 static char modname[CCHMAXPATH] = {0};

  DosQueryModuleName(hModule, CCHMAXPATH, modname);
  return(modname);
}
//******************************************************************************
void SYSTEM CheckVersion(ULONG version, char *modname)
{
    dprintf(("CheckVersion of %s, %d\n", modname, version));
    if(version != PE2LX_VERSION){
        static char msg[300];
        int r;
        dprintf(("Version mismatch! %d, %d: %s\n", version, PE2LX_VERSION, modname));
        sprintf(msg, "%s is intended for use with a different release of PE2LX.\n", modname);
        do{
            r = WinMessageBox(HWND_DESKTOP, NULLHANDLE, msg, "Version Mismatch!", 0, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_MOVEABLE);
        }while(r == MBID_RETRY);   // giggle
        if( r != MBID_IGNORE )
            exit(987);
    }
}

void SYSTEM CheckVersionFromHMOD(ULONG version, HMODULE hModule)
{
    char name[_MAX_PATH];

    // query name of dll.
    if(!DosQueryModuleName(hModule, sizeof(name), name))
        CheckVersion(version, name);
}

/*****************************************************************************
 * Name      : HMODULE OS2iGetModuleHandleA
 * Purpose   : replacement for IBM Open32's GetModuleHandle
 * Parameters: LPCTSTR lpszModule
 * Variables :
 * Result    : HMODULE hModule or NULLHANDLE in case of error
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Sun, 1998/04/04 01:55]
 *****************************************************************************/

HMODULE OS2iGetModuleHandleA(PSZ pszModule)
{
  HMODULE hModule;                                          /* module handle */
  APIRET  rc;                                              /* API returncode */
  static HMODULE hModuleExe;                          /* "cached" hModuleExe */
  PTIB pTIB;                              /* parameters for DosGetInfoBlocks */
  PPIB pPIB;

  dprintf(("KERNEL32:GetModuleHandle(%s)\n",
           pszModule));

  /* @@@PH 98/04/04

     this Open32 function is broken for pszModule == NULL
     return(GetModuleHandle(pszModule));

     Open32 always returns -1 here, however it should return the handle
     of the current process. MFC30 crashes.

     SvL, me thinks for PELDR support, you'll have to rewrite
     this code anyway :)

   */

  if (NULL == pszModule)              /* obtain handle to current executable */
  {
    if (hModuleExe != NULLHANDLE)            /* do we have a cached handle ? */
      return (hModuleExe);

    rc = DosGetInfoBlocks(&pTIB,                      /* get the info blocks */
                          &pPIB);
    if (rc != NO_ERROR)                                  /* check for errors */
    {
      SetLastError(rc);                                    /* set error code */
      return (NULLHANDLE);                                 /* signal failure */
    }

    hModuleExe = pPIB->pib_hmte;                        /* set cached module */
    hModule = pPIB->pib_hmte;                       /* module table entry ID */
  }
  else
  {
    rc = DosQueryModuleHandle(pszModule,              /* query module handle */
                              &hModule);

    if (rc != NO_ERROR)                                  /* check for errors */
    {
      SetLastError(rc);                                    /* set error code */
      return (NULLHANDLE);                                 /* signal failure */
    }
  }

  return (hModule);                              /* return determined handle */
}


HMODULE OS2QueryModuleHandle(char *modname)
{
 HMODULE hModule;
 APIRET  rc;

  rc = DosQueryModuleHandle(modname,              /* query module handle */
                            &hModule);
  if(rc)
    return(-1);

  return(hModule);
}

//SvL: only for RT_RCDATA!
ULONG OS2GetResourceSize(HMODULE hinstance, int id)
{
 APIRET rc;
 ULONG  size;

  rc = DosQueryResourceSize(hinstance, RT_RCDATA, id, &size);
  if(rc) {
    dprintf(("DosQueryResourceSize returned %d, %X id = %d\n", rc, hinstance, id));
    return(0);
  }
  return(size);
}

BOOL OS2GetResource(HMODULE hinstance, int id, char *destbuf, int bufLength)
{
 APIRET rc;
 char  *resdata;
 ULONG  size;

   rc = DosQueryResourceSize(hinstance, RT_RCDATA, id, &size);
   if(rc) {
    dprintf(("OS2GetResource: Can't get resource size of %d!!!\n", id));
    return(0);
   }
   rc = DosGetResource(hinstance, RT_RCDATA, id, (PPVOID)&resdata);
   if(rc) {
    dprintf(("OS2GetResource: Can't find resource %d!!!\n", id));
    return(0);
   }
   dprintf(("OS2GetResoure: bufLength %d, size %d, id %d", bufLength, size, id));
   size = min(size, bufLength);
   memcpy(destbuf, resdata, size);
   DosFreeResource(resdata);

   return(FALSE);
}

void  OS2Wait(ULONG msec)
{
   DosSleep(msec);
}

//******************************************************************************
//Wrapper for Dos16AllocSeg
//******************************************************************************
BOOL OS2AllocSel(ULONG size, USHORT *selector)
{ 
   return (Dos16AllocSeg(size, selector, SEG_NONSHARED) == 0);
}
//******************************************************************************
//Wrapper for Dos16FreeSeg
//******************************************************************************
BOOL OS2FreeSel(USHORT selector)
{
   return (Dos16FreeSeg(selector) == 0);
}
//******************************************************************************
//Wrapper for Dos32SelToFlat
//******************************************************************************
PVOID OS2SelToFlat(USHORT selector)
{
   return (PVOID)DosSelToFlat(selector << 16);
}
//******************************************************************************
//Get TIB data
//******************************************************************************
ULONG OS2GetTIB(int tiboff)
{
 PTIB   ptib;
 PPIB   ppib;
 APIRET rc;

   rc = DosGetInfoBlocks(&ptib, &ppib);
   if(rc) {
	return 0;
   }
   switch(tiboff)
   {
   	case TIB_STACKTOP:
		return (ULONG)ptib->tib_pstack;
	case TIB_STACKLOW:
		return (ULONG)ptib->tib_pstacklimit;
	default:
		return 0;
   }
}
//******************************************************************************
//Get PIB data
//******************************************************************************
ULONG OS2GetPIB(int piboff)
{
 PTIB   ptib;
 PPIB   ppib;
 APIRET rc;

   rc = DosGetInfoBlocks(&ptib, &ppib);
   if(rc) {
	return 0;
   }
   switch(piboff) 
   {
   	case PIB_TASKHNDL:
		return ppib->pib_hmte;
	case PIB_TASKTYPE:
		if(ppib->pib_ultype == 3) {
			return TASKTYPE_PM;
		}
		else	return TASKTYPE_VIO;
	default: 
		return 0;
   }
}
//******************************************************************************
//******************************************************************************
