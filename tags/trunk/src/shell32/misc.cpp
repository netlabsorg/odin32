/* $Id: misc.cpp,v 1.11 2001-04-04 09:02:15 sandervl Exp $ */

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
#include "wine/undocshell.h"
#include <misc.h>
#include <unicode.h>
//#include <string.h>
//#include "winbase.h"
#include <heapstring.h>

/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-MISC)


/* exported via shell32_main.h */
HRESULT (WINAPI* pOleInitialize)(LPVOID reserved);
void    (WINAPI* pOleUninitialize)(void);
HRESULT (WINAPI* pRegisterDragDrop)(HWND hwnd, IDropTarget* pDropTarget);
HRESULT (WINAPI* pRevokeDragDrop)(HWND hwnd);
HRESULT (WINAPI* pDoDragDrop)(LPDATAOBJECT,LPDROPSOURCE,DWORD,DWORD*);
void (WINAPI* pReleaseStgMedium)(STGMEDIUM* pmedium);
HRESULT (WINAPI* pOleSetClipboard)(IDataObject* pDataObj);
HRESULT (WINAPI* pOleGetClipboard)(IDataObject** ppDataObj);

BOOL SHELL_OsIsUnicode(void)
{
 static version = 0;

  if(version == 0) {
    	version = GetVersion();
  }
  /* if high-bit of version is 0, we are emulating NT */
  return !(version & 0x80000000);
}

/*****************************************************************************
 * Name      : ExtractAssociatedIconExA
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

ODINFUNCTION3(HICON,     ExtractAssociatedIconExA,
              HINSTANCE, hInst,
              LPSTR,     lpIconPath,
              LPWORD,    lpiIcon)
{
  dprintf(("SHELL32: undoc ExtractAssociatedIconExA\n"));

  return ExtractAssociatedIconA(hInst,lpIconPath,lpiIcon);
}

/*****************************************************************************
 * Name      : ExtractAssociatedIconW
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

ODINFUNCTION3(HICON,     ExtractAssociatedIconW,
              HINSTANCE, hInst,
              LPWSTR,    lpIconPath,
              LPWORD,    lpiIcon)
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
 * Name      : ExtractAssociatedIconExW
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

ODINFUNCTION3(HICON,     ExtractAssociatedIconExW,
              HINSTANCE, hInst,
              LPWSTR,    lpIconPath,
              LPWORD,    lpiIcon)
{
  dprintf(("SHELL32: undoc ExtractAssociatedIconExW\n"));

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

ODINFUNCTION4(BOOL,    OleStrToStrN,
              LPSTR,   lpMulti,
              INT,     nMulti,
              LPCWSTR, lpWide,
              INT,     nWide)
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

ODINFUNCTION4(BOOL,   StrToOleStrN,
              LPWSTR, lpWide,
              INT,    nWide,
              LPCSTR, lpMulti,
              INT,    nMulti)
{
  return MultiByteToWideChar (0,
                              0,
                              lpMulti,
                              nMulti,
                              lpWide,
                              nWide);
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
              DWORD,   x,
              DWORD,   y)
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

ODINFUNCTION2(int,    StrToOleStr,
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

ODINFUNCTION1(DWORD, InternalExtractIconListA, DWORD, dwArg1)
{
  dprintf(("SHELL32: undoc InternalExtractIconListA \n"));
  return 0;
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

ODINFUNCTION1(DWORD, InternalExtractIconListW, DWORD, dwArg1)
{
  dprintf(("SHELL32: undoc InternalExtractIconListW\n"));
  return 0;
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

ODINFUNCTION1(DWORD, ExtractIconResInfoA, DWORD, dwArg1)
{
  dprintf(("SHELL32: undoc ExtractIconResInfoA\n"));
  return 0;
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

ODINFUNCTION1(DWORD, ExtractIconResInfoW, DWORD, dwArg1)
{
  dprintf(("SHELL32: undoc ExtractIconResInfoW\n"));
  return 0;
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


