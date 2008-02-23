/* $Id: unknown.c,v 1.1 2002-06-07 08:22:04 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 SHELL32 Subsystem for OS/2
 * 1998/05/19 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <winbase.h>
#include <shellapi.h>
#include <shlobj.h>
#include <winreg.h>
#include <unicode.h>
#include <dbglog.h>

//#include "shell32.h"

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
 * Remark    : SHELL32.229
 * Status    : STUB UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HINSTANCE WIN32API RealShellExecuteA(HWND hwnd, LPCTSTR lpOperation,
                                     LPCTSTR lpFile, LPCTSTR lpParameters,
                                     LPCTSTR lpDirectory, INT nShowCmd)
{
  dprintf (("SHELL32: RealShellExecuteA not implemented.\n"));

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
 * Remark    : SHELL32.232
 * Status    : COMPLETELY UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HINSTANCE WIN32API RealShellExecuteW(HWND hwnd, LPCTSTR lpOperation,
                                     LPCWSTR lpFile, LPCWSTR lpParameters,
                                     LPCWSTR lpDirectory, INT nShowCmd)
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
 * Remark    : SHELL32.230
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API RealShellExecuteExA(LPSHELLEXECUTEINFOA lpExecInfo)
{
  dprintf(("SHELL32: RealShellExecuteExA not implemented.\n"));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL RealShellExecuteExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.231
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API RealShellExecuteExW(LPSHELLEXECUTEINFOW lpExecInfo)
{
  dprintf(("SHELL32: RealShellExecuteExW not implemented.\n"));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD RegenerateUserEnvironment
 * Purpose   :
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : SHELL32.233
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API RegenerateUserEnvironment(DWORD x1, DWORD x2)
{
  dprintf(("SHELL32: RegenerateUserEnvironment not implemented.\n"));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD RestartDialog
 * Purpose   :
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : SHELL32.59
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API RestartDialog(DWORD x1, DWORD x2, DWORD x3)
{
  dprintf(("SHELL32: RestartDialog not implemented.\n"));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD SheConvertPathW
 * Purpose   :
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : SHELL32.275
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SheConvertPathW(DWORD x1, DWORD x2)
{
  dprintf(("SHELL32: SheConvertPathW not implemented.\n"));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD SheShortenPathW
 * Purpose   :
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : SHELL32.287
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SheShortenPathW(DWORD x1, DWORD x2)
{
  dprintf(("SHELL32: SheShortenPathW not implemented.\n"));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD SheShortenPathA
 * Purpose   :
 * Parameters: unknown, probably wrong
 * Variables :
 * Result    :
 * Remark    : SHELL32.286
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SheShortenPathA(DWORD x1, DWORD x2)
{
  dprintf(("SHELL32: SheShortenPathA not implemented.\n"));

  return (0);
}


/*************************************************************************
 * SHRegQueryValueA				[NT4.0:SHELL32.?]
 *
 */
HRESULT WIN32API SHRegQueryValueA(HKEY hkey, LPSTR lpszSubKey,
                                  LPSTR lpszData, LPDWORD lpcbData )
{	dprintf(("SHELL32:UNKNOWN:SHRegQueryValueA(0x%04x %s %p %p semi-stub\n",
		hkey, lpszSubKey, lpszData, lpcbData));
	return RegQueryValueA( hkey, lpszSubKey, lpszData, (LPLONG)lpcbData );
}
