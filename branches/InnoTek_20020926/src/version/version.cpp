/* $Id: version.cpp,v 1.11 2001-04-04 09:02:16 sandervl Exp $ */

/*
 * Win32 Version resource APIs for OS/2
 *
 * Implementation of VERSION.DLL - File Installer routines
 * Copyright 1996,1997 Marcus Meissner
 * Copyright 1997      David Cuthbert
 * Copyright 1998      Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */


#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>
#include <unicode.h>
#include <heapstring.h>
#include <version.h>
#include <versionos2.h>
#include <unicode.h>

ODINDEBUGCHANNEL(VERSION)

BOOL GetFileVersionInfoA(LPSTR lpszFile, DWORD dwHandle, DWORD cbBuf, LPVOID lpvData)
{
  return GetVersionStruct((char *)lpszFile,
                          (char *)lpvData,
                          cbBuf);
}

BOOL GetFileVersionInfoW(LPWSTR lpszFile, DWORD dwHandle, DWORD cbBuf, LPVOID lpvData)
{
  BOOL rc;
  char  *astring = UnicodeToAsciiString(lpszFile);

  rc = GetVersionStruct(astring, (char *)lpvData, cbBuf);
  FreeAsciiString(astring);
  return(rc);
}

DWORD GetFileVersionInfoSizeA(LPSTR lpszFile, LPDWORD lpdwHandle)
{
  if(lpdwHandle)
    *lpdwHandle = 0; //SvL: Set contents to 0

  return GetVersionSize(lpszFile);
}


DWORD GetFileVersionInfoSizeW(LPWSTR lpszFile, LPDWORD lpdwHandle)
{
  char  *astring = UnicodeToAsciiString((LPWSTR)lpszFile);
  DWORD rc;

  if(lpdwHandle)
    *lpdwHandle = 0; //SvL: Set contents to 0

  rc = GetVersionSize(astring);
  FreeAsciiString(astring);
  return(rc);
}



