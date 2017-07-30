/* $Id: oslibmisc.cpp,v 1.19 2004-05-24 08:56:07 sandervl Exp $ */
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
#define INCL_DOSERRORS
#define INCL_DOSSEL
#define INCL_DOSNLS        /* National Language Support values */
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <string.h>
#include <stdlib.h>
#include <stdio.h>  /*PLF Wed  98-03-18 05:15:04*/
#include <malloc.h>  /*PLF Wed  98-03-18 05:15:04*/
#include "oslibmisc.h"
#include <misc.h>
#include <odincrt.h>

#define DBG_LOCALLOG    DBG_oslibmisc
#include "dbglocal.h"

typedef APIRET ( APIENTRY *PFN_IMSETMSGQUEUEPROPERTY )( HMQ, ULONG );

PFN_IMSETMSGQUEUEPROPERTY pfnImSetMsgQueueProperty = NULL;

//******************************************************************************
//TODO: not reentrant!
//******************************************************************************
char *OSLibGetDllName(ULONG hModule)
{
  static char modname[CCHMAXPATH] = {0};

  APIRET rc;
  if((rc = DosQueryModuleName(hModule, CCHMAXPATH, modname)) != 0) {
    dprintf(("KERNEL32: OSLibGetDllName(%x) failed with %d", hModule, rc));
    return NULL;
  }
  return(modname);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibGetDllName(ULONG hModule, char *name, int length)
{
  APIRET rc;
  if((rc = DosQueryModuleName(hModule, length, name)) == 0)
    return TRUE;

  dprintf(("KERNEL32: OSLibGetDllName(%x) failed with %d", hModule, rc));
  return FALSE;
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
    rc = DosQueryModuleHandleStrict(pszModule,        /* query module handle */
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

  rc = DosQueryModuleHandleStrict(modname,            /* query module handle */
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
#if 1
   PVOID  pSelMem;
   ULONG  sel;
   APIRET rc;

   rc = DosAllocMem(&pSelMem, size, PAG_COMMIT|PAG_READ|PAG_WRITE|OBJ_TILE);
   if(rc != NO_ERROR) {
       dprintf(("OSLibAllocSel: DosAllocMem failed with %d", rc));
       DebugInt3();
       return FALSE;
   }
   *selector = (DosFlatToSel((ULONG)pSelMem) >> 16);
   return *selector != 0;
#else
   return (Dos16AllocSeg(size, selector, SEG_NONSHARED) == 0);
#endif
}
//******************************************************************************
//Wrapper for Dos16FreeSeg
//******************************************************************************
ULONG OSLibFreeSel(USHORT selector)
{
#if 1
   PVOID  pSelMem;
   APIRET rc;

   pSelMem = (PVOID)DosSelToFlat(selector << 16);
   rc = DosFreeMem(pSelMem);
   return rc == NO_ERROR;
#else
   return (Dos16FreeSeg(selector) == 0);
#endif
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
const char *OSLibStripPath(const char *path)
{
    const char *pszName;
    register char ch;

    /*
     * Search the filename string finding the modulename start and end.
     * The loop ends when we have passed one char left of the module name.
     */
    pszName = path + strlen(path) - 1;
    while (pszName >= path
           && (ch = *pszName) != '\\'
           && ch != '/'
           && ch != ':'
           )
    {
        pszName--;
    }
    pszName++;

    return pszName;
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
  ULONG  hmq;
  APIRET rc;
  PTIB   ptib;
  PPIB   ppib;

  rc = DosGetInfoBlocks(&ptib, &ppib);
  if(rc != NO_ERROR) {
      dprintf(("DosGetInfoBlocks failed with rc %d", rc));
      DebugInt3();
      return 0;
  }
  if(ppib->pib_ultype == 2) {
      dprintf(("Warning: app type changed back to VIO!!"));
      ppib->pib_ultype = 3;
  }
  hmq = WinQueueFromID(hab, ppib->pib_ulpid, ptib->tib_ptib2->tib2_ultid);

  if(!hmq) {
      dprintf(("WinQueueFromID %x %x %x proc type %x failed with error %x", hab, ppib->pib_ulpid, ptib->tib_ptib2->tib2_ultid, ppib->pib_ultype, WinGetLastError(hab)));

      hmq = (ULONG)WinCreateMsgQueue((HAB)hab, 0);
      if(!hmq) {
          dprintf(("WinCreateMsgQueue failed with error %x", WinGetLastError(hab)));
      }
  }
  return hmq;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinSetCp(ULONG hmq, ULONG codepage)
{
  return WinSetCp(hmq, codepage);
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
BOOL OSLibDisablePopups()
{
   return DosError(FERR_DISABLEEXCEPTION | FERR_DISABLEHARDERR) == NO_ERROR;
}
//******************************************************************************
//******************************************************************************
void OSLibSetBeginLibpath(char *lpszBeginlibpath)
{
    DosSetExtLIBPATH(lpszBeginlibpath, BEGIN_LIBPATH);
}
//******************************************************************************
//******************************************************************************
void OSLibQueryBeginLibpath(char *lpszBeginlibpath, int size)
{
    DosQueryExtLIBPATH(lpszBeginlibpath, BEGIN_LIBPATH);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibImSetMsgQueueProperty( ULONG hmq, ULONG ulFlag )
{
    USHORT sel;
    APIRET rc;

    if( !pfnImSetMsgQueueProperty )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetMsgQueueProperty( hmq, ulFlag );
    SetFS( sel );

    return rc;
}
//******************************************************************************
//******************************************************************************

