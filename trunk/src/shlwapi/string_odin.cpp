 /* $Id: string_odin.cpp,v 1.3 2001-07-20 15:37:53 sandervl Exp $ */

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




