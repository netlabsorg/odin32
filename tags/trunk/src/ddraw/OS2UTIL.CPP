/* $Id: OS2UTIL.CPP,v 1.7 1999-11-02 22:41:23 hugh Exp $ */

/*
 * OS/2 Utility functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSMEMMGR
#include <os2wrap.h>
#include <misc.h>
#include "os2util.h"


//******************************************************************************
//******************************************************************************
char *OS2AllocMem(ULONG size)
{
 PVOID  lpMem;
 APIRET rc;

  rc = DosAllocMem(&lpMem, size, PAG_READ|PAG_WRITE|PAG_COMMIT);
  if(rc) {
    dprintf(("DDRAW: DosAllocMem returned %d", rc));
    return(NULL);
  }
  return((char *)lpMem);
}
//******************************************************************************
//******************************************************************************
void OS2FreeMem(char *lpMem)
{
 APIRET rc;

  rc = DosFreeMem(lpMem);
  if(rc) {
    dprintf(("DDRAW: DosFreeMem returned %d", rc));
  }
}//******************************************************************************
//******************************************************************************
void OS2MaximizeWindow(HWND hwndClient)
{
  WinSetWindowPos(hwndClient, HWND_TOP, 0, 0, 0, 0, SWP_MAXIMIZE);
}

