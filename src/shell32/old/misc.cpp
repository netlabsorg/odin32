/* $Id: misc.cpp,v 1.1 1999-10-09 11:11:30 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 * 1998/06/15 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Copyright 1997 Marcus Meissner
 * Copyright 1988 Patrick Haller (adapted for win32os2)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <wchar.h>
#include <wcstr.h>

#define ICOM_CINTERFACE 1

//#include <os2win.h>
#include <shellapi.h>
#include <winnls.h>
#include "shell32_main.h"
#include <misc.h>
//#include <string.h>
//#include "winbase.h"
#include <heapstring.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-MISC)


/*****************************************************************************
 * Name      : HICON IExtractAssociatedIconExA
 * Purpose   : Return icon for given file (either from file itself or from associated
 *             executable) and patch parameters if needed.
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL.150
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 20:00]
 *****************************************************************************/

ODINFUNCTION3(HICON, ExtractAssociatedIconExA,
              HINSTANCE, hInst,
              LPSTR, lpIconPath,
              LPWORD, lpiIcon)
{
  return ExtractAssociatedIconA(hInst,lpIconPath,lpiIcon);
}


/*****************************************************************************
 * Name      : HICON ExtractAssociatedIconW
 * Purpose   : Return icon for given file (either from file itself or from associated
 *             executable) and patch parameters if needed.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL.36
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

ODINFUNCTION3(HICON, ExtractAssociatedIconW,
              HINSTANCE, hInst,
              LPWSTR, lpIconPath,
              LPWORD, lpiIcon)
{
  LPSTR lpIconPathAscii = UnicodeToAsciiString(lpIconPath);
  HICON hIcon;

  hIcon = ExtractAssociatedIconA(hInst,
                                 lpIconPathAscii,
                                 lpiIcon);

  FreeAsciiString(lpIconPathAscii);

  return (hIcon);
}

/*****************************************************************************
 * Name      : HICON ExtractAssociatedIconExW
 * Purpose   : Return icon for given file (either from file itself or from associated
 *             executable) and patch parameters if needed.
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL.186
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 20:00]
 *****************************************************************************/

ODINFUNCTION3(HICON, ExtractAssociatedIconExW,
              HINSTANCE, hInst,
              LPWSTR, lpIconPath,
              LPWORD, lpiIcon)
{
  return ExtractAssociatedIconW(hInst,lpIconPath,lpiIcon);
}


/*****************************************************************************
 * Name      : OleStrToStrN
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.78
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

ODINFUNCTION4(BOOL, OleStrToStrN,
              LPSTR, lpMulti,
              INT, nMulti,
              LPCWSTR, lpWide,
              INT, nWide)
{
  return WideCharToMultiByte (0,
                              0,
                              lpWide,
                              nWide,
                              lpMulti,
                              nMulti,
                              NULL,
                              NULL);
}


/*****************************************************************************
 * Name      : StrToOleStrN
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.79
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

ODINFUNCTION4(BOOL, StrToOleStrN,
              LPWSTR, lpWide,
              INT, nWide,
              LPCSTR, lpMulti,
              INT, nMulti)
{
  return MultiByteToWideChar (0,
                              0,
                              lpMulti,
                              nMulti,
                              lpWide,
                              nWide);
}


/*****************************************************************************
 * Name      : StrRetToStrN
 * Purpose   : converts a STRRET to a normal string
 * Parameters: the pidl is for STRRET OFFSET
 * Variables :
 * Result    :
 * Remark    : SHELL32.96
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

ODINFUNCTION4(HRESULT, StrRetToStrN,
              LPVOID, dest,
              DWORD, len,
              LPSTRRET, src,
              LPITEMIDLIST, pidl)
{
  switch (src->uType)
  {
    case STRRET_WSTR:
      WideCharToMultiByte(CP_ACP,
                          0,
                          src->u.pOleStr,
                          -1,
                          (LPSTR)dest,
                          len,
                          NULL,
                          NULL);
      SHFree(src->u.pOleStr);
      break;

    case STRRET_CSTRA:
      //if (VERSION_OsIsUnicode())
      //  lstrcpynAtoW((LPWSTR)dest, src->u.cStr, len);
      //else
        strncpy((LPSTR)dest,
                src->u.cStr,
                len);
      break;

    case STRRET_OFFSETA:
      if (pidl)
      {
        //if(VERSION_OsIsUnicode())
        //  lstrcpynAtoW((LPWSTR)dest, ((LPCSTR)&pidl->mkid)+src->u.uOffset, len);
        //else
          strncpy((LPSTR)dest,
                  ((LPCSTR)&pidl->mkid)+src->u.uOffset,
                  len);
        break;
      }

    default:
      dprintf(("SHELL32: StrRetToStrN: unknown type!\n"));

      if (len)
        *(LPSTR)dest = '\0';

      return(FALSE);
  }
  return(TRUE);
}


/*****************************************************************************
 * Name      : SHELL32_654
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.654
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

ODINFUNCTION2(HRESULT, SHELL32_654,
              DWORD, x,
              DWORD, y)
{
  dprintf(("SHELL32: SHELL32_654(%08xh,%08xh) not implemented.\n",
           x,
           y));
  return 0;
}


/*****************************************************************************
 * Name      : StrToOleStr
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.163
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

ODINFUNCTION2(int, StrToOleStr,
              LPWSTR, lpWideCharStr,
              LPCSTR, lpMultiByteString)
{
  return MultiByteToWideChar(0,
                             0,
                             lpMultiByteString,
                             -1,
                             lpWideCharStr,
                             MAX_PATH);
}


/*****************************************************************************
 * Name      : StrCpyNW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.310 - used by winfile.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINFUNCTION3(LPWSTR, StrCpyNW,
              LPWSTR, lpString1,
              LPWSTR, lpString2,
              int, iMaxLength)
{
  return lstrcpynW(lpString1,lpString2,iMaxLength);
}

/*****************************************************************************
 * Name      : StrNCpyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.316 - used by winfile.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINFUNCTION2(LPWSTR, StrNCpyW,
              LPWSTR, lpString1,
              LPWSTR, lpString2)
{
  return lstrcpyW(lpString1,lpString2);
}


/*****************************************************************************
 * Name      : InternalExtractIconListA
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.221 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINPROCEDURE0(InternalExtractIconListA)
{
  dprintf(("SHELL32: undoc InternalExtractIconListA \n"));
}

/*****************************************************************************
 * Name      : InternalExtractIconListW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.222 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINPROCEDURE0(InternalExtractIconListW)
{
  dprintf(("SHELL32: undoc InternalExtractIconListW\n"));
}

/*****************************************************************************
 * Name      : ExtractIconResInfoA
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.192 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINPROCEDURE0(ExtractIconResInfoA)
{
  dprintf(("SHELL32: undoc ExtractIconResInfoA\n"));
}

/*****************************************************************************
 * Name      : ExtractIconResInfoW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.193 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINPROCEDURE0(ExtractIconResInfoW)
{
  dprintf(("SHELL32: undoc ExtractIconResInfoW\n"));
}

/*************************************************************************
 *				FindEnvironmentString	[SHELL.38]
 *
 * Returns a pointer into the DOS environment... Ugh.
 */
LPSTR SHELL_FindStringA(LPSTR lpEnv, LPCSTR entry)
{ UINT16 l;

  //TRACE_(shell)("\n");

  l = strlen(entry);
  for( ; *lpEnv ; lpEnv+=strlen(lpEnv)+1 )
  { if( lstrncmpiA(lpEnv, entry, l) )
      continue;
	if( !*(lpEnv+l) )
	    return (lpEnv + l); 		/* empty entry */
	else if ( *(lpEnv+l)== '=' )
	    return (lpEnv + l + 1);
    }
    return NULL;
}


/*****************************************************************************
 * Name      : CheckEscapesA
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.3 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINPROCEDURE2(CheckEscapesA,
               DWORD, x1,
               DWORD, x2)
{
  dprintf(("SHELL32: undoc CheckEscapesA\n"));
}

/*****************************************************************************
 * Name      : CheckEscapesW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.6 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINPROCEDURE2(CheckEscapesW,
               DWORD, x1,
               DWORD, x2)
{
  dprintf(("SHELL32: undoc CheckEscapesW\n"));
}

/*****************************************************************************
 * Name      : DuplicateIcon
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.138 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

ODINFUNCTION1(DWORD, DuplicateIcon,
              DWORD, x1)
{
  dprintf(("SHELL32: undoc DuplicateIcon\n"));

  return 0;
}

