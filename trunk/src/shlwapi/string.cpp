/* $Id: string.cpp,v 1.1 2000-05-15 02:43:17 phaller Exp $ */

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

#include "shlwapi.h"

/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHLWAPI-STRING)



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


/*************************************************************************
 * StrRetToStrN                                 [SHELL32.96]
 *
 * converts a STRRET to a normal string
 *
 * NOTES
 *  the pidl is for STRRET OFFSET
 */
HRESULT WINAPI StrRetToBufA (LPSTRRET src, LPITEMIDLIST pidl, LPSTR dest, DWORD len)
{
        return StrRetToStrNA(dest, len, src, pidl);
}

HRESULT WINAPI StrRetToStrNA (LPVOID dest, DWORD len, LPSTRRET src, LPITEMIDLIST pidl)
{
        TRACE("dest=0x%p len=0x%lx strret=0x%p pidl=%p stub\n",dest,len,src,pidl);

        switch (src->uType)
        {
          case STRRET_WSTR:
            WideCharToMultiByte(CP_ACP, 0, src->u.pOleStr, -1, (LPSTR)dest, len, NULL, NULL);
            SHFree(src->u.pOleStr);
            break;

          case STRRET_CSTRA:
            lstrcpynA((LPSTR)dest, src->u.cStr, len);
            break;

          case STRRET_OFFSETA:
            lstrcpynA((LPSTR)dest, ((LPCSTR)&pidl->mkid)+src->u.uOffset, len);
            break;

          default:
            FIXME("unknown type!\n");
            if (len)
            {
              *(LPSTR)dest = '\0';
            }
            return(FALSE);
        }
        return S_OK;
}

HRESULT WINAPI StrRetToBufW (LPSTRRET src, LPITEMIDLIST pidl, LPWSTR dest, DWORD len)
{
        return StrRetToStrNW(dest, len, src, pidl);
}

HRESULT WINAPI StrRetToStrNW (LPVOID dest, DWORD len, LPSTRRET src, LPITEMIDLIST pidl)
{
        TRACE("dest=0x%p len=0x%lx strret=0x%p pidl=%p stub\n",dest,len,src,pidl);

        switch (src->uType)
        {
          case STRRET_WSTR:
            lstrcpynW((LPWSTR)dest, src->u.pOleStr, len);
            SHFree(src->u.pOleStr);
            break;

          case STRRET_CSTRA:
            lstrcpynAtoW((LPWSTR)dest, src->u.cStr, len);
            break;

          case STRRET_OFFSETA:
            if (pidl)
            {
              lstrcpynAtoW((LPWSTR)dest, ((LPSTR)&pidl->mkid)+src->u.uOffset, len);
            }
            break;

          default:
            FIXME("unknown type!\n");
            if (len)
            { *(LPSTR)dest = '\0';
            }
            return(FALSE);
        }
        return S_OK;
}
HRESULT WINAPI StrRetToStrNAW (LPVOID dest, DWORD len, LPSTRRET src, LPITEMIDLIST pidl)
{
        if(VERSION_OsIsUnicode())
          return StrRetToStrNW (dest, len, src, pidl);
        return StrRetToStrNA (dest, len, src, pidl);
}

/*************************************************************************
 * StrChrA                                      [NT 4.0:SHELL32.651]
 *
 */
LPSTR WINAPI StrChrA (LPSTR str, CHAR x )
{       LPSTR ptr=str;

        do
        {  if (*ptr==x)
           { return ptr;
           }
           ptr++;
        } while (*ptr);
        return NULL;
}

/*************************************************************************
 * StrChrW                                      [NT 4.0:SHELL32.651]
 *
 */
LPWSTR WINAPI StrChrW (LPWSTR str, WCHAR x )
{       LPWSTR ptr=str;

        TRACE("%s 0x%04x\n",debugstr_w(str),x);
        do
        {  if (*ptr==x)
           { return ptr;
           }
           ptr++;
        } while (*ptr);
        return NULL;
}

/*************************************************************************
 * StrCmpNIW                                    [NT 4.0:SHELL32.*]
 *
 */
INT WINAPI StrCmpNIW ( LPWSTR wstr1, LPWSTR wstr2, INT len)
{       FIXME("%s %s %i stub\n", debugstr_w(wstr1),debugstr_w(wstr2),len);
        return 0;
}

/*************************************************************************
 * StrCmpNIA                                    [NT 4.0:SHELL32.*]
 *
 */
INT WINAPI StrCmpNIA ( LPSTR wstr1, LPSTR wstr2, INT len)
{       FIXME("%s %s %i stub\n", wstr1,wstr2,len);
        return 0;
}


/*************************************************************************
 * StrRChrA                                     [SHELL32.346]
 *
 */
LPSTR WINAPI StrRChrA(LPCSTR lpStart, LPCSTR lpEnd, DWORD wMatch)
{
        if (!lpStart)
            return NULL;

        /* if the end not given, search*/
        if (!lpEnd)
        { lpEnd=lpStart;
          while (*lpEnd)
            lpEnd++;
        }

        for (--lpEnd;lpStart <= lpEnd; lpEnd--)
            if (*lpEnd==(char)wMatch)
                return (LPSTR)lpEnd;

        return NULL;
}
/*************************************************************************
 * StrRChrW                                     [SHELL32.320]
 *
 */
LPWSTR WINAPI StrRChrW(LPWSTR lpStart, LPWSTR lpEnd, DWORD wMatch)
{       LPWSTR wptr=NULL;
        TRACE("%s %s 0x%04x\n",debugstr_w(lpStart),debugstr_w(lpEnd), (WCHAR)wMatch );

        /* if the end not given, search*/
        if (!lpEnd)
        { lpEnd=lpStart;
          while (*lpEnd)
            lpEnd++;
        }

        do
        { if (*lpStart==(WCHAR)wMatch)
            wptr = lpStart;
          lpStart++;
        } while ( lpStart<=lpEnd );
        return wptr;
}

/*************************************************************************
* StrFormatByteSize                             [SHLWAPI]
*/
LPSTR WINAPI StrFormatByteSizeA ( DWORD dw, LPSTR pszBuf, UINT cchBuf )
{       char buf[64];
        TRACE("%lx %p %i\n", dw, pszBuf, cchBuf);
        if ( dw<1024L )
        { sprintf (buf,"%3.0f bytes", (FLOAT)dw);
        }
        else if ( dw<1048576L)
        { sprintf (buf,"%3.2f KB", (FLOAT)dw/1024);
        }
        else if ( dw < 1073741824L)
        { sprintf (buf,"%3.2f MB", (FLOAT)dw/1048576L);
        }
        else
        { sprintf (buf,"%3.2f GB", (FLOAT)dw/1073741824L);
        }
        lstrcpynA (pszBuf, buf, cchBuf);
        return pszBuf;
}
LPWSTR WINAPI StrFormatByteSizeW ( DWORD dw, LPWSTR pszBuf, UINT cchBuf )
{       char buf[64];
        TRACE("%lx %p %i\n", dw, pszBuf, cchBuf);
        if ( dw<1024L )
        { sprintf (buf,"%3.0f bytes", (FLOAT)dw);
        }
        else if ( dw<1048576L)
        { sprintf (buf,"%3.2f KB", (FLOAT)dw/1024);
        }
        else if ( dw < 1073741824L)
        { sprintf (buf,"%3.2f MB", (FLOAT)dw/1048576L);
        }
        else
        { sprintf (buf,"%3.2f GB", (FLOAT)dw/1073741824L);
        }
        lstrcpynAtoW (pszBuf, buf, cchBuf);
        return pszBuf;
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
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPSTR,  StrChrIA,
              LPCSTR, lpStart,
              CHAR,   wMatch)
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
 * Status    : UNTESTED UNKNOWN
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
 * Name      : StrStrIA
 * Purpose   : Finds the first occurrence of a substring within a string. The
 *             comparison is not case sensitive.
 * Parameters: LPCSTR lpFirst
 *             LPCSTR lpSrch
 * Variables :
 * Result    : Returns the address of the first occurrence of the matching
 *             substring if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPSTR,  StrStrIA,
              LPCSTR, lpFirst,
              LPCSTR, lpSrch)
{
  char  ch = lpSrch[0];          // look for 1st character
  LONG  lLen = lstrlenA(lpSrch); // length of search string
  int   iRes;                    // comparsion result

  do
  {
    lpFirst = StrChrIA(lpFirst, // find first matching character
                       ch);
    if (NULL == lpFirst)        // not found
      return NULL;

    iRes   = StrCmpNIA((LPSTR)lpFirst, // compare search string
                       (LPSTR)lpSrch,
                       lLen);

    if (0 == iRes)              // Found!
      return (LPSTR)lpFirst;

    lpFirst = CharNextA(lpFirst); // skip to next character
  }
  while (*lpFirst != 0);        // safe termination

  return NULL;                  // default result
}



/*****************************************************************************
 * Name      : StrStrIW
 * Purpose   : Finds the first occurrence of a substring within a string. The
 *             comparison is not case sensitive.
 * Parameters: LPCWSTR lpFirst
 *             LPCWSTR lpSrch
 * Variables :
 * Result    : Returns the address of the first occurrence of the matching
 *             substring if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPWSTR,  StrStrIW,
              LPCWSTR, lpFirst,
              LPCWSTR, lpSrch)
{
  WCHAR  ch = lpSrch[0];          // look for 1st character
  LONG   lLen = lstrlenW(lpSrch); // length of search string
  int   iRes;                    // comparsion result

  do
  {
    lpFirst = StrChrIW(lpFirst, // find first matching character
                       ch);
    if (NULL == lpFirst)        // not found
      return NULL;

    iRes   = StrCmpNIW((LPWSTR)lpFirst, // compare search string
                       (LPWSTR)lpSrch,
                       lLen);

    if (0 == iRes)              // Found!
      return (LPWSTR)lpFirst;

    lpFirst = CharNextW(lpFirst); // skip to next character
  }
  while (*lpFirst != 0);        // safe termination

  return NULL;                  // default result
}



/*****************************************************************************
 * Name      : StrToIntA
 * Purpose   : convert string to integer (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(INT,StrToIntA,LPSTR,pszPath)
{
  dprintf(("not implemented"));

  return NULL;
}
