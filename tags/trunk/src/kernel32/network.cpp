/* $Id: network.cpp,v 1.11 2001-12-10 11:28:59 sandervl Exp $ */
/*
 * Win32 Network apis
 *
 * Copyright 1998 Peter Fitzsimmons
 *           1999 Przemyslaw Dobrowolski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "unicode.h"

#define DBG_LOCALLOG	DBG_network
#include "dbglocal.h"

//******************************************************************************
// GetComputerName
//
// Retrieve the NetBIOS name of the computer
//******************************************************************************
BOOL WIN32API GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize)
{
    char szDefault[] = "NONAME";
    char * szHostname = getenv("HOSTNAME");     // This is wrong;
                                                // We should use NETBIOS computername

    if (!szHostname)    // Hostname not set; assume a default
        szHostname = szDefault;

    *nSize = min(strlen(szHostname) + 1, *nSize);       // Truncate name as reqd.
                                                // NB W95/98 would generate a
                                                // BUFFER_OVERFLOW error here

    if (lpBuffer)
    {
        strncpy(lpBuffer, szHostname, *nSize);  // Copy back name.
        lpBuffer[*nSize - 1] = 0;               // Ensure terminated.
    }

    dprintf(("KERNEL32: GetComputerNameA (Name: %.*s, nSize: %d)", *nSize, lpBuffer, *nSize));

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetComputerNameW(LPWSTR name, LPDWORD size)
{
  LPSTR nameA = NULL;
  BOOL  ret;

  if (name) nameA=(LPSTR)malloc(2**size);

  ret = GetComputerNameA(nameA,size);

  if (ret) AsciiToUnicode(nameA,name);

  free(nameA);

  return ret;
}
//******************************************************************************
//******************************************************************************
