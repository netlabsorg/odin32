/* $Id: shell32.cpp,v 1.8 1999-06-24 19:27:49 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * 1998/06/15 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) shell32.c       1.0.0   1998/06/15 PH Merge WINE/SHELLORD.C
 *
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1988 Patrick Haller (adapted for win32os2)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <os2win.h>
#include <shellapi.h>
#include <winreg.h>
#include "shell32.h"

#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <nameid.h>
#include <unicode.h>


//static const char*    lpstrMsgWndCreated = "OTHERWINDOWCREATED";
//static const char*    lpstrMsgWndDestroyed = "OTHERWINDOWDESTROYED";
//static const char*    lpstrMsgShellActivate = "ACTIVATESHELLWINDOW";

//static HWND   SHELL_hWnd = 0;
//static HHOOK  SHELL_hHook = 0;
//static USHORT uMsgWndCreated = 0;
//static USHORT uMsgWndDestroyed = 0;
//static USHORT uMsgShellActivate = 0;



/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/


/*****************************************************************************
 * Name      : HINSTANCE ShellExecuteA
 * Purpose   : Start a program
 * Parameters: HWND    hwnd
 *             LPCTSTR lpOperation
 *             LPCTSTR lpFile
 *             LPCTSTR lpParameters
 *             LPCTSTR lpDirectory
 *             INT     nShowCmd
 * Variables :
 * Result    :
 * Remark    : SHELL32.289
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HINSTANCE WIN32API ShellExecuteA(HWND    hwnd,
                                 LPCTSTR lpOperation,
                                 LPCTSTR lpFile,
                                 LPCTSTR lpParameters,
                                 LPCTSTR lpDirectory,
                                 INT     nShowCmd)
{
  dprintf (("SHELL32: ShellExecuteA(%08xh,%s,%s,%s,%s,%08xh) not implemented.\n",
            hwnd,
            lpOperation,
            lpFile,
            lpParameters,
            lpDirectory,
            nShowCmd));

  return(0); //out of memory
}


/*****************************************************************************
 * Name      : HINSTANCE ShellExecuteW
 * Purpose   : Start a program
 * Parameters: HWND    hwnd
 *             LPCWSTR lpOperation
 *             LPCWSTR lpFile
 *             LPCWSTR lpParameters
 *             LPCWSTR lpDirectory
 *             INT     nShowCmd
 * Variables :
 * Result    :
 * Remark    : SHELL32.293
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HINSTANCE WIN32API ShellExecuteW(HWND    hwnd,
                                 LPCWSTR lpOperation,
                                 LPCWSTR lpFile,
                                 LPCWSTR lpParameters,
                                 LPCWSTR lpDirectory,
                                 INT     nShowCmd)
{
  HINSTANCE hInstance;
  LPSTR     lpOperationA  = UnicodeToAsciiString((LPWSTR)lpOperation);
  LPSTR     lpFileA       = UnicodeToAsciiString((LPWSTR)lpFile);
  LPSTR     lpParametersA = UnicodeToAsciiString((LPWSTR)lpParameters);
  LPSTR     lpDirectoryA  = UnicodeToAsciiString((LPWSTR)lpDirectory);

  dprintf (("SHELL32: ShellExecuteW(%08xh,%s,%s,%s,%s,%08xh).\n",
            hwnd,
            lpOperationA,
            lpFileA,
            lpParametersA,
            lpDirectoryA,
            nShowCmd));

  hInstance = ShellExecuteA(hwnd,
                            lpOperationA,
                            lpFileA,
                            lpParametersA,
                            lpDirectoryA,
                            nShowCmd);

  FreeAsciiString(lpOperationA);
  FreeAsciiString(lpFileA);
  FreeAsciiString(lpParametersA);
  FreeAsciiString(lpDirectoryA);

  return hInstance;
}



/*****************************************************************************
 * Name      : DWORD ShellAboutA
 * Purpose   : display a simple about box
 * Parameters: HWND   hwnd
 *             LPSTR  szApplication
 *             LPSTR  szMoreInformation
 *             HICON  hIcon
 * Variables :
 * Result    :
 * Remark    : SHELL32.287
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

int WIN32API ShellAboutA(HWND    hwnd,
                         LPCTSTR szApp,
                         LPCTSTR szOtherStuff,
                         HICON   hIcon)
{
  dprintf(("SHELL32: ShellAboutA(%08xh,%08xh,%08xh,%08xh) not implemented properly.\n",
           hwnd,
           szApp,
           szOtherStuff,
           hIcon));

  /* @@@PH 98/06/07 */
  if (szOtherStuff == NULL) szOtherStuff = "";
  if (szApp        == NULL) szApp        = "";

  MessageBoxA(NULL,
              szOtherStuff,
              szApp,
              MB_OK);                             /*PLF Sun  97-11-23 22:58:49*/

  return(TRUE);
}


/*****************************************************************************
 * Name      : DWORD ShellAboutW
 * Purpose   : display a simple about box
 * Parameters: HWND    hwnd
 *             LPWSTR  szApplication
 *             LPWSTR  szMoreInformation
 *             HICON   hIcon
 * Variables :
 * Result    :
 * Remark    : SHELL32.288
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

int WIN32API ShellAboutW(HWND    hwnd,
                         LPCWSTR szApp,
                         LPCWSTR szOtherStuff,
                         HICON   hIcon)
{
  LPSTR lpszApp;                     /* temporary buffers for A-W conversion */
  LPSTR lpszOtherStuff;
  int   iResult;

  dprintf(("SHELL32: ShellAboutW(%08xh,%08xh,%08xh,%08xh).\n",
           hwnd,
           szApp,
           szOtherStuff,
           hIcon));

  if (szApp != NULL)                             /* convert string on demand */
    lpszApp = UnicodeToAsciiString((LPWSTR)szApp);
  else
    lpszApp = NULL;

  if (szOtherStuff != NULL)
    lpszOtherStuff = UnicodeToAsciiString((LPWSTR)szOtherStuff);
  else
    lpszOtherStuff = NULL;

  iResult = ShellAboutA(hwnd,                          /* call ascii variant */
                        lpszApp,
                        lpszOtherStuff,
                        hIcon);

  if (lpszApp != NULL)                            /* free strings as created */
     FreeAsciiString(lpszApp);

  if (lpszOtherStuff != NULL)
     FreeAsciiString(lpszOtherStuff);

  return(iResult);
}


/*****************************************************************************
 * Name      : BOOL Shell_NotifyIconA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.296 .297
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API Shell_NotifyIconA(DWORD            dwMessage,
                                PNOTIFYICONDATAA pnid)
{
  dprintf(("SHELL32: Shell_NotifyIconA(%08xh,%08xh) not implemented.\n",
           dwMessage,
           pnid));

  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL Shell_NotifyIconW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.298
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API Shell_NotifyIconW(DWORD dwMessage,
                                         PNOTIFYICONDATAW pnid )
{
  dprintf(("SHELL32: Shell_NotifyIconW(%08xh,%08xh) not implemented.\n",
           dwMessage,
           pnid));

  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL ShellExecuteEx
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API ShellExecuteEx(LPSHELLEXECUTEINFOA lpExecInfo)
{
  dprintf(("SHELL32: ShellExecuteEx (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}



/*****************************************************************************
 * Name      : BOOL ShellExecuteExA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.290 .291
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API ShellExecuteExA(LPSHELLEXECUTEINFOA lpExecInfo)
{
  dprintf(("SHELL32: ShellExecuteExA (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL ShellExecuteExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.292
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API ShellExecuteExW(LPSHELLEXECUTEINFOW lpExecInfo)
{
  dprintf(("SHELL32: ShellExecuteExW (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD ShellMessageBoxW
 * Purpose   : display a messagebox, retrieve message text from resources
 * Parameters: HMODULE hmod
 *             HWND    hwnd
 *             DWORD   idText
 *             DWORD   idTitle
 *             DWORD   uType
 *             LPCVOID arglist
 * Variables :
 * Result    :
 * Remark    : SHELL32.182
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD CDECL ShellMessageBoxW(HMODULE hmod,
                                      HWND    hwnd,
                                      DWORD   idText,
                                      DWORD   idTitle,
                                      DWORD   uType,
                                      LPCVOID arglist)
{
  WCHAR   szText[100],
          szTitle[100],
          szTemp[256];
  LPWSTR pszText = &szText[0],
         pszTitle = &szTitle[0];
  LPVOID args = &arglist;

  dprintf(("SHELL32: ShellMessageBoxW(%08lx,%08lx,%08lx,%08lx,%08lx,%p)\n",
           hmod,
           hwnd,
           idText,
           idTitle,
           uType,
           arglist));

  if (!HIWORD (idTitle))
    LoadStringW(hmod,
                idTitle,
                pszTitle,
                100);
  else
    pszTitle = (LPWSTR)idTitle;

  if (! HIWORD (idText))
    LoadStringW(hmod,
                idText,
                pszText,
                100);
  else
    pszText = (LPWSTR)idText;

  FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                 szText,
                 0,
                 0,
                 szTemp,
                 256,
                 (LPDWORD)args);

  return MessageBoxW(hwnd,
                     szTemp,
                     szTitle,
                     uType);
}


/*****************************************************************************
 * Name      : DWORD ShellMessageBoxA
 * Purpose   : display a messagebox, retrieve message text from resources
 * Parameters: HMODULE hmod
 *             HWND    hwnd
 *             DWORD   idText
 *             DWORD   idTitle
 *             DWORD   uType
 *             LPCVOID arglist
 * Variables :
 * Result    :
 * Remark    : SHELL32.183
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD CDECL ShellMessageBoxA(HMODULE hmod,
                                      HWND    hwnd,
                                      DWORD   idText,
                                      DWORD   idTitle,
                                      DWORD   uType,
                                      LPCVOID arglist)
{
  char   szText[100],
         szTitle[100],
         szTemp[256];
  LPSTR  pszText = &szText[0],
         pszTitle = &szTitle[0];
  LPVOID args = &arglist;

  dprintf(("SHELL32: ShellMessageBoxA (%08lx,%08lx,%08lx,%08lx,%08lx,%p)\n",
           hmod,
           hwnd,
           idText,
           idTitle,
           uType,
           arglist));

  if (!HIWORD (idTitle))
    LoadStringA(hmod,
                idTitle,
                pszTitle,
                100);
  else
    pszTitle = (LPSTR)idTitle;

  if (! HIWORD (idText))
    LoadStringA(hmod,
                idText,
                pszText,
                100);
  else
    pszText = (LPSTR)idText;

  FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                 pszText,
                 0,
                 0,
                 szTemp,
                 256,
                 (LPDWORD)args);

  return MessageBoxA(hwnd,
                     szTemp,
                     pszTitle,
                     uType);
}
