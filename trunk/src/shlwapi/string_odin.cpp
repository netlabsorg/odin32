 /* $Id: string_odin.cpp,v 1.8 2002-06-07 08:02:20 sandervl Exp $ */

/*
 * Win32 Lightweight SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Path Functions
 *
 * Many of this functions are in SHLWAPI.DLL also
 *
 * Corel WINE 20000324 level (without CRTDLL_* calls)
 */

/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#ifndef __GNUC__
#include <wcstr.h>
#else
#include <wchar.h>
#endif
#define HAVE_WCTYPE_H

#include "debugtools.h"

#include <winreg.h>

#include <heapstring.h>
#include <misc.h>
#include <win/shell.h>
#include <win/winerror.h>
#include <winversion.h>
#include <winuser.h>

#define CINTERFACE

#include "winerror.h"
#include "winnls.h"
#include "winversion.h"
#include "heap.h"

#include "shellapi.h"
#include "shlobj.h"

#include "shlwapi_odin.h"
#include "shlwapi.h"

/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHLWAPI-STRING)




/*****************************************************************************
 * Name      : StrChrIA
 * Purpose   : Searches a string for the first occurrence of a character that
 *             matches the specified character. The comparison is not case sensitive.
 * Parameters: LPCSTR lpStart Address of the string to be searched.
 *             TCHAR  wMatch  Character to be used for comparison.
 * Variables :
 * Result    : Returns the address of the first occurrence of the character in
 *             the string if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : COMPLETELY IMPLEMENTED UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPSTR,  StrChrIA,
              LPCSTR, lpStart,
              char,   cMatch)
{
  LPSTR lpRes;

  cMatch = tolower(cMatch);
  lpRes = strchr(lpStart, cMatch);    // lower case comparsion
  if (NULL == lpRes)
  {
    cMatch = toupper(cMatch);
    lpRes = strchr(lpStart, cMatch);  // upper case comparsion
  }

  return lpRes;
}


/*****************************************************************************
 * Name      : StrChrIW
 * Purpose   : Searches a string for the first occurrence of a character that
 *             matches the specified character. The comparison is not case sensitive.
 * Parameters: LPCSTR lpStart Address of the string to be searched.
 *             TCHAR  wMatch  Character to be used for comparison.
 * Variables :
 * Result    : Returns the address of the first occurrence of the character in
 *             the string if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : COMPLETELY IMPLEMENTED UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPWSTR,   StrChrIW,
              LPCWSTR, lpStart,
              WCHAR,    wMatch)
{
  LPWSTR lpRes;

  wMatch = towlower(wMatch);
  lpRes = (WCHAR*)wcschr((const wchar_t*)lpStart, wMatch);    // lower case comparsion
  if (NULL == lpRes)
  {
    wMatch = towupper(wMatch);
    lpRes = (WCHAR*)wcschr((const wchar_t*)lpStart, wMatch);  // upper case comparsion
  }

  return lpRes;
}

#if 0
/*****************************************************************************
 * Name      : StrCpyA
 * Purpose   : copy a string
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : not exported ?
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

ODINFUNCTIONNODBG2(LPSTR,  StrCpyA,
              LPSTR,  lpDest,
              LPCSTR, lpSource)
{
  return lstrcpyA(lpDest,
                  lpSource);
}
#endif

/*****************************************************************************
 * Name      : StrSpnA
 * Purpose   : find the first occurence of a character in string1
 *             that is not contained in the set of characters specified by
 *             string2.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : COMCTL32undoc.StrSpnW, CRTDLL.strspn
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

ODINFUNCTION2(INT,    StrSpnA,
              LPCSTR, lpString1,
              LPCSTR, lpString2)
{
  // 2001-08-30 PH
  // copied from implementation in COMCTL32
  if ( (lpString1 == NULL) ||
       (lpString2 == NULL) )
    return 0;

  LPSTR lpLoop = (LPSTR)lpString1;

  for (; (*lpLoop != 0); lpLoop++ )
    if ( StrChrA( lpString2, *lpLoop ) )
      return (INT) (lpLoop - lpString1);

  return (INT) (lpLoop - lpString1);
}


/*****************************************************************************
 * Name      : StrSpnW
 * Purpose   : find the first occurence of a character in string1
 *             that is not contained in the set of characters specified by
 *             string2.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : COMCTL32undoc.StrSpnW, CRTDLL.strspn
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

ODINFUNCTION2(INT,     StrSpnW,
              LPCWSTR, lpString1,
              LPCWSTR, lpString2)
{
  // 2001-08-30 PH
  // copied from implementation in COMCTL32
  if ( (lpString1 == NULL) ||
       (lpString2 == NULL) )
    return 0;

  LPWSTR lpLoop = (LPWSTR)lpString1;

  for (; (*lpLoop != 0); lpLoop++ )
    if ( StrChrW( lpString2, *lpLoop ) )
      return (INT) (lpLoop - lpString1);

  return (INT) (lpLoop - lpString1);
}


/*****************************************************************************
 * Name      : StrPBrkA
 * Purpose   : find the first occurence in string1 of any character from string2
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

ODINFUNCTION2(LPSTR,   StrPBrkA,
              LPCSTR,  lpString1,
              LPCSTR,  lpString2)
{
  register LPSTR s1;

  while (*lpString1)
  {
    for (s1 = (LPSTR)lpString2;
         *s1 && *s1 != *lpString1;
         s1++)
         /* empty */
      ;

    if (*s1)
      return (LPSTR)lpString1;

    lpString1++;
  }

  return (LPSTR)NULL;
}


/*****************************************************************************
 * Name      : StrPBrkW
 * Purpose   : find the first occurence in string1 of any character from string2
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

ODINFUNCTION2(LPWSTR,   StrPBrkW,
              LPCWSTR,  lpString1,
              LPCWSTR,  lpString2)
{
  register LPWSTR s1;

  while (*lpString1)
  {
    for (s1 = (LPWSTR)lpString2;
         *s1 && *s1 != *lpString1;
         s1++)
         /* empty */
      ;

    if (*s1)
      return (LPWSTR)lpString1;

    lpString1++;
  }

  return (LPWSTR)NULL;
}


/*************************************************************************
 * StrRStrIA					[SHLWAPI]
 */
LPSTR WINAPI StrRStrIA(LPCSTR lpFirst, LPCSTR lpSrch, LPCSTR unknown)
{
  INT   iLen = lstrlenA(lpFirst) - lstrlenA(lpSrch);

  dprintf(("StrRStrIA %x %x %x NOT IMPLEMENTED correctly", lpFirst, lpSrch, unknown));

  // lpSrch cannot fit into lpFirst
  if (iLen < 0)
    return (LPSTR)NULL;

  LPSTR lpThis = (LPSTR)lpFirst + iLen;

  while (lpThis >= lpFirst)
  {
    LPCSTR p1 = lpThis, p2 = lpSrch;
    while (*p1 && *p2 && toupper(*p1) == toupper(*p2)) { p1++; p2++; }
    if (!*p2) return (LPSTR)lpThis;
    lpThis--;
  }

  return NULL;
}


/*************************************************************************
 * StrRStrIW					[SHLWAPI]
 */
LPWSTR WINAPI StrRStrIW(LPCWSTR lpFirst, LPCWSTR lpSrch, LPCWSTR unknown)
{
  INT   iLen = lstrlenW(lpFirst) - lstrlenW(lpSrch);

  dprintf(("StrRStrIA %x %x %x NOT IMPLEMENTED correctly", lpFirst, lpSrch, unknown));

  // lpSrch cannot fit into lpFirst
  if (iLen < 0)
    return (LPWSTR)NULL;

  LPWSTR lpThis = (LPWSTR)lpFirst + iLen;

  while (lpThis >= lpFirst)
  {
    LPCWSTR p1 = lpThis, p2 = lpSrch;
    while (*p1 && *p2 && toupperW(*p1) == toupperW(*p2)) { p1++; p2++; }
    if (!*p2) return (LPWSTR)lpThis;
    lpThis--;
  }

  return NULL;
}
