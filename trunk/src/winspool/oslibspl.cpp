/* $Id: oslibspl.cpp,v 1.1 2000-11-15 10:54:23 sandervl Exp $ */

/*
 * Winspool support code
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_SPL
#define  INCL_SPLERRORS
#define  INCL_DOSERRORS
#define  INCL_GPI
#define  INCL_WIN
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include "oslibspl.h"

//******************************************************************************
//******************************************************************************
BOOL OSLibSplEnumPrinters(DWORD flType, OSLIB_PRINTERINFO *pBuf, DWORD cbSize,
                          DWORD *pcReturned, DWORD *pcTotal, DWORD *pcbNeeded)
{
  SPLERR rc;

    rc = SplEnumPrinter(NULL, 0, flType, (PVOID)pBuf, cbSize, pcReturned, pcTotal, pcbNeeded, 0);

    if(rc == 0 || (pBuf == NULL && (rc == ERROR_MORE_DATA || rc == NERR_BufTooSmall))) {
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
