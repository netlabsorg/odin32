/* $Id: oslibmisc.cpp,v 1.3 1999-10-09 13:33:24 sandervl Exp $ */

/*
 * Misc OS/2 util. procedures
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
#include "oslibmisc.h"
#include <misc.h>

/***********************************
 * PH: fixups for missing os2win.h *
 ***********************************/

void _System SetLastError(ULONG ulError);

//******************************************************************************
//******************************************************************************
void OSLibSetExitList(unsigned long handler)
{
 APIRET rc;

  rc = DosExitList(EXLST_ADD | 0x00002A00, (PFNEXITLIST)handler);
  if(rc) {
    dprintf(("DosExitList returned %d\n", rc));
  }
}
//******************************************************************************
//******************************************************************************
void OSLibClearExitList()
{
  DosExitList(EXLST_EXIT, NULL);
}
//******************************************************************************
//******************************************************************************
void OSLibRemoveExitList(unsigned long handler)
{
  DosExitList(EXLST_REMOVE, (PFNEXITLIST)handler);
}
//******************************************************************************
//TODO: not reentrant!
//******************************************************************************
char *OSLibGetDllName(ULONG hModule)
{
 static char modname[CCHMAXPATH] = {0};

  if(DosQueryModuleName(hModule, CCHMAXPATH, modname) != 0) {
	return NULL;
  }
  return(modname);
}
//******************************************************************************
/*****************************************************************************
 * Name      : ULONG OSLibiGetModuleHandleA
 * Purpose   : replacement for IBM Open32's GetModuleHandle
 * Parameters: LPCTSTR lpszModule
 * Variables :
 * Result    : HMODULE hModule or NULLHANDLE in case of error
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Sun, 1998/04/04 01:55]
 *****************************************************************************/

ULONG OSLibiGetModuleHandleA(char * pszModule)
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


ULONG OSLibQueryModuleHandle(char *modname)
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
ULONG OSLibGetResourceSize(HMODULE hinstance, int id)
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

ULONG OSLibGetResource(HMODULE hinstance, int id, char *destbuf, int bufLength)
{
 APIRET rc;
 char  *resdata;
 ULONG  size;

   rc = DosQueryResourceSize(hinstance, RT_RCDATA, id, &size);
   if(rc) {
    dprintf(("OSLibGetResource: Can't get resource size of %d!!!\n", id));
    return(FALSE);
   }
   rc = DosGetResource(hinstance, RT_RCDATA, id, (PPVOID)&resdata);
   if(rc) {
    dprintf(("OSLibGetResource: Can't find resource %d!!!\n", id));
    return(FALSE);
   }
   dprintf(("OSLibGetResoure: bufLength %d, size %d, id %d", bufLength, size, id));
   size = min(size, bufLength);
   memcpy(destbuf, resdata, size);
   DosFreeResource(resdata);

   return(TRUE);
}

void  OSLibWait(ULONG msec)
{
   DosSleep(msec);
}

//******************************************************************************
//Wrapper for Dos16AllocSeg
//******************************************************************************
ULONG OSLibAllocSel(ULONG size, USHORT *selector)
{ 
   return (Dos16AllocSeg(size, selector, SEG_NONSHARED) == 0);
}
//******************************************************************************
//Wrapper for Dos16FreeSeg
//******************************************************************************
ULONG OSLibFreeSel(USHORT selector)
{
   return (Dos16FreeSeg(selector) == 0);
}
//******************************************************************************
//Wrapper for Dos32SelToFlat
//******************************************************************************
PVOID OSLibSelToFlat(USHORT selector)
{
   return (PVOID)DosSelToFlat(selector << 16);
}
//******************************************************************************
//Get TIB data
//******************************************************************************
ULONG OSLibGetTIB(int tiboff)
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
		return (ULONG)ptib->tib_pstacklimit;
	case TIB_STACKLOW:
		return (ULONG)ptib->tib_pstack;
	default:
		return 0;
   }
}
//******************************************************************************
//Get PIB data
//******************************************************************************
ULONG OSLibGetPIB(int piboff)
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
//Allocate local thread memory
//******************************************************************************
ULONG OSLibAllocThreadLocalMemory(int nrdwords)
{
 APIRET rc;
 PULONG thrdaddr;

   rc = DosAllocThreadLocalMemory(nrdwords, &thrdaddr);
   if(rc) {
	dprintf(("DosAllocThreadLocalMemory failed %d", rc));
	return 0;
   }
   return (ULONG)thrdaddr;
}
//******************************************************************************
//******************************************************************************
char *OSLibStripPath(char *path)
{
  /* @@@PH what does this function do ? Strip the path from a FQFN name ? */
  char *pszFilename;
  
  pszFilename = strrchr(path, '\\');                 /* find rightmost slash */
  if (pszFilename != NULL)
    return (pszFilename++);              /* return pointer to next character */
  
  pszFilename = strrchr(path, '/');                  /* find rightmost slash */
  if (pszFilename != NULL)
    return (pszFilename++);              /* return pointer to next character */
  
  return (path);                                     /* default return value */
}
//******************************************************************************
//******************************************************************************
