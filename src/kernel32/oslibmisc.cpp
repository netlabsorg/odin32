/* $Id: oslibmisc.cpp,v 1.12 2001-03-25 22:35:37 sandervl Exp $ */
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
#define INCL_WIN
#define INCL_BASE
#define INCL_DOSPROCESS
#define INCL_DOSSEL
#define INCL_DOSNLS        /* National Language Support values */
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <string.h>
#include <stdlib.h>
#include <stdio.h>  /*PLF Wed  98-03-18 05:15:04*/
#include <malloc.h>  /*PLF Wed  98-03-18 05:15:04*/
#include "oslibmisc.h"
#include <misc.h>

#define DBG_LOCALLOG	DBG_oslibmisc
#include "dbglocal.h"

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
//******************************************************************************
BOOL OSLibGetDllName(ULONG hModule, char *name, int length)
{
  return DosQueryModuleName(hModule, length, name) == 0;
}
//******************************************************************************
/******************************************************************************
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
  static HMODULE hModuleExe = 0;                        /* "cached" hModuleExe */
  PTIB pTIB;                              /* parameters for DosGetInfoBlocks */
  PPIB pPIB;

  dprintf(("KERNEL32:GetModuleHandle(%x)\n",
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

void OSLibWait(ULONG msec)
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

/**
 * Gets a PIB data.
 * @returns     Requested PIB data.
 *              0 may indicate error or that the PIB data you requested actually is 0.
 * @param       iPIB    PIB data index. (one of the PIB_* defines in oslibmisc.h)
 * @author
 * @remark      Spooky error handling.
 */
ULONG OSLibGetPIB(int iPIB)
{
     PTIB   ptib;
     PPIB   ppib;
     APIRET rc;

    rc = DosGetInfoBlocks(&ptib, &ppib);
    if (rc)
    {
	    dprintf(("KERNEL32: OSLibGetPIB(%d): DosGetInfoBlocks failed with rc=%d\n", iPIB, rc));
        return 0;
    }

    switch(iPIB)
    {
    case PIB_TASKHNDL:
        return ppib->pib_hmte;

    case PIB_TASKTYPE:
        return (ppib->pib_ultype == 3) ? TASKTYPE_PM : TASKTYPE_VIO;

    case PIB_PCHCMD:
        return (ULONG)ppib->pib_pchcmd;

    default:
        dprintf(("KERNEL32: OSLibGetPIB(%d): Invalid PIB data index\n.", iPIB));
        DebugInt3();
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
  char *pszFilename1;

  pszFilename  = strrchr(path, '\\');                 /* find rightmost backslash */
  pszFilename1 = strrchr(path, '/');                  /* find rightmost slash */
  if(pszFilename > pszFilename1 && pszFilename != NULL)
    return (++pszFilename);              /* return pointer to next character */

  if (pszFilename1 != NULL)
    return (++pszFilename1);              /* return pointer to next character */

  return (path);                                     /* default return value */
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinInitialize()
{
  return (ULONG)WinInitialize(0);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinQueryMsgQueue(ULONG hab)
{
//  ULONG hmq;

//  hmq = WinQueryWindowULong(HWND_DESKTOP, QWL_HMQ);
  return (ULONG)WinCreateMsgQueue((HAB)hab, 0);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibQueryCountry()
{
 COUNTRYCODE  Country    = {0};   /* Country code info (0 = current country) */
 COUNTRYINFO  CtryInfo   = {0};   /* Buffer for country-specific information */
 ULONG        ulInfoLen  = 0;
 APIRET       rc         = NO_ERROR;  /* Return code                         */

    rc = DosQueryCtryInfo(sizeof(CtryInfo), &Country,
                          &CtryInfo, &ulInfoLen);

    if (rc != NO_ERROR) {
        return -1;
    }
    return CtryInfo.country;
}
//******************************************************************************
//******************************************************************************
