/* $Id: oslibdos.cpp,v 1.1 1999-08-26 12:56:02 sandervl Exp $ */

/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include <initterm.h>
#include "oslibdos.h"

//******************************************************************************
//******************************************************************************
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags)
{
  return DosAllocMem(lplpMemAddr, size, flags);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosFreeMem(LPVOID lpMemAddr)
{
  return DosFreeMem(lpMemAddr);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosQueryMem(LPVOID lpMemAddr, DWORD *lpRangeSize, DWORD *lpAttr)
{
  return DosQueryMem(lpMemAddr, lpRangeSize, lpAttr);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetMem(LPVOID lpMemAddr, DWORD size, DWORD flags)
{
 APIRET rc;

  rc = DosSetMem(lpMemAddr, size, flags);
  switch(rc) {
  	case ERROR_INVALID_ADDRESS:
		return OSLIB_ERROR_INVALID_ADDRESS;
  	case ERROR_ACCESS_DENIED:
		return OSLIB_ERROR_ACCESS_DENIED;
  	default:
  		return rc;
  }
}
//******************************************************************************
//******************************************************************************
