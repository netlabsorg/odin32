/* $Id: unknown.cpp,v 1.2 1999-06-23 22:28:52 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 SHELL32 Subsystem for OS/2
 * 1998/05/19 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <shellapi.h>
#include <winreg.h>
#include "shell32.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/


/*****************************************************************************
 * Name      : HINSTANCE RealShellExecuteA
 * Purpose   : Start a program
 * Parameters: HWND    hwnd
 *             LPCTSTR lpOperation
 *             LPCTSTR lpFile
 *             LPCTSTR lpParameters
 *             LPCTSTR lpDirectory
 *             INT     nShowCmd
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HINSTANCE WIN32API RealShellExecuteA(HWND    hwnd,
                                     LPCTSTR lpOperation,
                                     LPCTSTR lpFile,
                                     LPCTSTR lpParameters,
                                     LPCTSTR lpDirectory,
                                     INT     nShowCmd)
{
  dprintf (("SHELL32: RealShellExecuteA(%08xh,%s,%s,%s,%s,%08xh) not implemented.\n",
            hwnd,
            lpOperation,
            lpFile,
            lpParameters,
            lpDirectory,
            nShowCmd));

  return(0); //out of memory
}


/*****************************************************************************
 * Name      : HINSTANCE RealShellExecuteW
 * Purpose   : Start a program
 * Parameters: HWND    hwnd
 *             LPCWSTR lpOperation
 *             LPCWSTR lpFile
 *             LPCWSTR lpParameters
 *             LPCWSTR lpDirectory
 *             INT     nShowCmd
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HINSTANCE WIN32API RealShellExecuteW(HWND    hwnd,
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

  dprintf (("SHELL32: RealShellExecuteW(%08xh,%s,%s,%s,%s,%08xh).\n",
            hwnd,
            lpOperationA,
            lpFileA,
            lpParametersA,
            lpDirectoryA,
            nShowCmd));

  hInstance = RealShellExecuteA(hwnd,
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
 * Name      : BOOL RealShellExecuteExA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API RealShellExecuteExA(LPSHELLEXECUTEINFOA lpExecInfo)
{
  dprintf(("SHELL32: RealShellExecuteExA (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL RealShellExecuteExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API RealShellExecuteExW(LPSHELLEXECUTEINFOW lpExecInfo)
{
  dprintf(("SHELL32: RealShellExecuteExW (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}

