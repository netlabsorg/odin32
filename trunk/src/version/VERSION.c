/* $Id: VERSION.c,v 1.1 1999-05-24 20:20:05 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Version resource APIs for OS/2
 *
 * 18-07-1998 SvL: Initial version
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "unicode.h"

/******************************************************************************/
/******************************************************************************/
BOOL WIN32API GetFileVersionInfoA(LPTSTR lpszFile, DWORD dwHandle, DWORD cbBuf, LPVOID lpvData)
{
   dprintf(("GetFileVersionInfoA %s\n", lpszFile));
   return GetVersionStruct(lpszFile, (char *)lpvData, cbBuf);
}
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API GetFileVersionInfoW(LPWSTR lpszFile, DWORD dwHandle, DWORD cbBuf, LPVOID lpvData)
{
 BOOL  rc;
 char *astring = UnicodeToAsciiString(lpszFile);

   dprintf(("GetFileVersionInfoW\n"));
   rc = GetVersionStruct(astring, (char *)lpvData, cbBuf);
   FreeAsciiString(astring);
   return(rc);
}
/******************************************************************************/
/******************************************************************************/
DWORD WIN32API GetFileVersionInfoSizeA(LPTSTR lpszFile, LPDWORD lpdwHandle)
{
   dprintf(("GetFileVersionInfoSizeA %s\n", lpszFile));
    
   if(lpdwHandle) {
	lpdwHandle = 0;
   }
   return GetVersionSize(lpszFile);
}
/******************************************************************************/
/******************************************************************************/
DWORD WIN32API GetFileVersionInfoSizeW(LPWSTR lpszFile, LPDWORD lpdwHandle)
{
 char *astring = UnicodeToAsciiString(lpszFile);
 DWORD rc;

   dprintf(("GetFileVersionInfoSizeW\n"));
   if(lpdwHandle) {
	lpdwHandle = 0;
   }
   rc = GetVersionSize(astring);
   FreeAsciiString(astring);
   return(rc);
}
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API VerQueryValueA(const LPVOID pBlock, LPTSTR lpSubBlock,
			         LPVOID lplpBuffer, PUINT puLen)
{
   dprintf(("VerQueryValueA Not implemented\n"));
   return TRUE;
}
/******************************************************************************/
/******************************************************************************/
/*KSO Sun 24.05.1998*/
BOOL WIN32API VerQueryValueW(const LPVOID pBlock, LPWSTR lpSubBlock,
			         LPVOID lplpBuffer, PUINT puLen)
{
   dprintf(("VerQueryValueW Not implemented\n"));
   return TRUE;
}
/******************************************************************************/
/******************************************************************************/



