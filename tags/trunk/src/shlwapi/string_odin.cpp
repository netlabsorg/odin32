 /* $Id: string_odin.cpp,v 1.4 2001-08-30 19:19:59 phaller Exp $ */

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
#include <wcstr.h>
#define HAVE_WCTYPE_H

#include "debugtools.h"

#include <winreg.h>

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>
#include <winversion.h>
#include <winuser.h>


#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "winerror.h"
#include "winnls.h"
#include "winversion.h"
#include "heap.h"

#include "shellapi.h"
#include "shlobj.h"
#include "wine/undocshell.h"

#include "shlwapi_odin.h"
#include "shlwapi.h"

/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHLWAPI-STRING)


/*****************************************************************************
 * Name      : ChrCmpIA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : PARTIALLY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(INT,    ChrCmpIA,
              INT,    ch1,
              INT,    ch2)
{
  // Note: IsDBCSLeadByte ignored !

  if ( (ch1 >= 'A') && (ch1 <= 'Z') ) ch1 |= 0x20;
  if ( (ch2 >= 'A') && (ch2 <= 'Z') ) ch2 |= 0x20;

  return ch1 - ch2;
}


#if 0
/*************************************************************************
 * OleStrToStrN                                 [SHELL32.78]
 */
BOOL WINAPI OleStrToStrNA (LPSTR lpStr, INT nStr, LPCWSTR lpOle, INT nOle)
{
        TRACE("%p %x %s %x\n", lpStr, nStr, debugstr_w(lpOle), nOle);
        return WideCharToMultiByte (0, 0, lpOle, nOle, lpStr, nStr, NULL, NULL);
}

BOOL WINAPI OleStrToStrNW (LPWSTR lpwStr, INT nwStr, LPCWSTR lpOle, INT nOle)
{
        TRACE("%p %x %s %x\n", lpwStr, nwStr, debugstr_w(lpOle), nOle);

        if (lstrcpynW ( lpwStr, lpOle, nwStr))
        { return lstrlenW (lpwStr);
        }
        return 0;
}

BOOL WINAPI OleStrToStrNAW (LPVOID lpOut, INT nOut, LPCVOID lpIn, INT nIn)
{
        if (VERSION_OsIsUnicode())
          return OleStrToStrNW ((LPWSTR)lpOut, nOut, (LPCWSTR)lpIn, nIn);
        return OleStrToStrNA ((LPSTR)lpOut, nOut, (LPCWSTR)lpIn, nIn);
}

/*************************************************************************
 * StrToOleStrN                                 [SHELL32.79]
 *  lpMulti, nMulti, nWide [IN]
 *  lpWide [OUT]
 */
BOOL WINAPI StrToOleStrNA (LPWSTR lpWide, INT nWide, LPCSTR lpStrA, INT nStr)
{
        TRACE("%p %x %s %x\n", lpWide, nWide, lpStrA, nStr);
        return MultiByteToWideChar (0, 0, lpStrA, nStr, lpWide, nWide);
}
BOOL WINAPI StrToOleStrNW (LPWSTR lpWide, INT nWide, LPCWSTR lpStrW, INT nStr)
{
        TRACE("%p %x %s %x\n", lpWide, nWide, debugstr_w(lpStrW), nStr);

        if (lstrcpynW (lpWide, lpStrW, nWide))
        { return lstrlenW (lpWide);
        }
        return 0;
}

BOOL WINAPI StrToOleStrNAW (LPWSTR lpWide, INT nWide, LPCVOID lpStr, INT nStr)
{
        if (VERSION_OsIsUnicode())
          return StrToOleStrNW (lpWide, nWide, (LPWSTR)lpStr, nStr);
        return StrToOleStrNA (lpWide, nWide, (LPSTR)lpStr, nStr);
}




/************************************************************************
 *      StrToOleStr                     [SHELL32.163]
 *
 */
int WINAPI StrToOleStrA (LPWSTR lpWideCharStr, LPCSTR lpMultiByteString)
{
        TRACE("%p %p(%s)\n",
        lpWideCharStr, lpMultiByteString, lpMultiByteString);

        return MultiByteToWideChar(0, 0, lpMultiByteString, -1, lpWideCharStr, MAX_PATH);

}
int WINAPI StrToOleStrW (LPWSTR lpWideCharStr, LPCWSTR lpWString)
{
        TRACE("%p %p(%s)\n",
        lpWideCharStr, lpWString, debugstr_w(lpWString));

        if (lstrcpyW (lpWideCharStr, lpWString ))
        { return lstrlenW (lpWideCharStr);
        }
        return 0;
}

BOOL WINAPI StrToOleStrAW (LPWSTR lpWideCharStr, LPCVOID lpString)
{
        if (VERSION_OsIsUnicode())
          return StrToOleStrW (lpWideCharStr, (LPCWSTR)lpString);
        return StrToOleStrA (lpWideCharStr, (LPCSTR)lpString);
}
#endif


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
              WORD,   wMatch)
{
  LPSTR lpRes;

  wMatch = tolower(wMatch);
  lpRes = strchr(lpStart, wMatch);    // lower case comparsion
  if (NULL == lpRes)
  {
    wMatch = toupper(wMatch);
    lpRes = strchr(lpStart, wMatch);  // upper case comparsion
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
LPSTR WINAPI StrRStrIA(LPCSTR lpFirst, LPCSTR lpSrch)
{
  INT   iLen = lstrlenA(lpFirst) - lstrlenA(lpSrch);
  
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
LPWSTR WINAPI StrRStrIW(LPCWSTR lpFirst, LPCWSTR lpSrch)
{
  INT   iLen = lstrlenW(lpFirst) - lstrlenW(lpSrch);
  
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
