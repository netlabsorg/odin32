/* $Id: ntdll.cpp,v 1.1 1999-06-07 22:17:40 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * @(#) ntdll.cpp  1.0.1   1999/05/08 SvL: Changes for compilation with Wine headers
 *                 1.0.0   1998/05/20 PH Start from WINE/NTDLL.C
 *
 * NT basis DLL
 *
 * Copyright 1996 Marcus Meissner
 * Copyright 1998 Patrick Haller (adapted for win32os2)
 */

 /* Changes to the original NTDLL.C from the WINE project

  - includes replaced by the win32os2 standard includes
  - replaced WINAPI by WIN32API
  - moved in some string functions
  - replaced HANDLE32 by HANDLE
  - lstrlen32A -> OS2lstrlenA
  - lstrlen32W -> OS2lstrlenW
*/

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <winnt.h>
#include <ntdef.h>
#include <builtin.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "misc.h"
#include "unicode.h"

#include "ntdll.h"


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

#define NTSTATUS DWORD


/***********************************************************************
*           lstrcpynAtoW   (Not a Windows API)
* Note: this function differs from the UNIX strncpy, it _always_ writes
* a terminating \0
*/
LPWSTR WIN32API lstrcpynAtoW(LPWSTR dst,
                             LPCSTR src,
                             INT  n)
{
  LPWSTR p = dst;
  while ((n-- > 1) && *src) *p++ = (WCHAR)(unsigned char)*src++;
  if (n >= 0) *p = 0;
  return dst;
}


/***********************************************************************
*           lstrcpynWtoA   (Not a Windows API)
* Note: this function differs from the UNIX strncpy, it _always_ writes
* a terminating \0
*/
LPSTR WIN32API lstrcpynWtoA(LPSTR   dst,
                            LPCWSTR src,
                            INT   n)
{
  LPSTR p = dst;
  while ((n-- > 1) && *src) *p++ = (CHAR)*src++;
  if (n >= 0) *p = 0;
  return dst;
}


/***********************************************************************
 *           lstrncmp32W   (Not a Windows API)
 */
INT WIN32API lstrncmpW( LPCWSTR str1, LPCWSTR str2, INT n )
{
  if (!n)
    return 0;

  while ((--n > 0) && *str1 && (*str1 == *str2))
  {
    str1++;
    str2++;
  }

  return (INT)(*str1 - *str2);
}



/***********************************************************************
*           HEAP_strdupA
*/
LPSTR HEAP_strdupA(HANDLE heap,
                   DWORD  flags,
                   LPCSTR str)
{
  LPSTR p = (LPSTR)HeapAlloc(heap,
                      flags,
                      lstrlenA(str) + 1 );

  lstrcpyA(p, str);
  return p;
}


/***********************************************************************
*           HEAP_strdupW
*/
LPWSTR HEAP_strdupW(HANDLE  heap,
                    DWORD   flags,
                    LPCWSTR str)
{
  INT len = lstrlenW(str) + 1;
  LPWSTR p = (LPWSTR)HeapAlloc(heap,
                       flags,
                       len * sizeof(WCHAR) );
  lstrcpyW(p,
              str);
  return p;
}


/***********************************************************************
*           HEAP_strdupWtoA
*/
LPSTR HEAP_strdupWtoA(HANDLE  heap,
                      DWORD   flags,
                      LPCWSTR str )
{
  LPSTR ret;

  if (!str)
    return NULL;

  ret = (LPSTR)HeapAlloc(heap,
                  flags,
                  lstrlenW(str) + 1 );
  UnicodeToAscii((LPWSTR)str,
                 ret);
/*  lstrcpyWtoA(ret,
              str);*/
  return ret;
}

/***********************************************************************
*           HEAP_strdupAtoW
*/
LPWSTR HEAP_strdupAtoW(HANDLE heap,
                       DWORD  flags,
                       LPCSTR str)
{
  LPWSTR ret;

  if (!str)
    return NULL;

  ret = (LPWSTR)HeapAlloc(heap,
                     flags,
                     (lstrlenA(str)+1) * sizeof(WCHAR) );
  AsciiToUnicode((char *)str,
                 ret);
  /* lstrcpyAtoW(ret,
              str );*/

  return ret;
}

