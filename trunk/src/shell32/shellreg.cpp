/*
	Shell Registry Access
        */
/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#define ICOM_CINTERFACE 1

#include <string.h>
#include <stdio.h>
#include "winerror.h"
#include "winreg.h"
#include "debugtools.h"
#include "winnls.h"
#include "heap.h"

#include "shellapi.h"
#include "shlobj.h"
#include "shell32_main.h"
#include "wine/undocshell.h"



/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(shell32-shellreg)


/*************************************************************************
 * SHRegOpenKeyA				[SHELL32.506]
 *
 */
HRESULT WINAPI SHRegOpenKeyA(
	HKEY hKey,
	LPSTR lpSubKey,
	LPHKEY phkResult)
{
	TRACE("(0x%08x, %s, %p)\n", hKey, debugstr_a(lpSubKey), phkResult);
	return RegOpenKeyA(hKey, lpSubKey, phkResult);
}

/*************************************************************************
 * SHRegOpenKeyW				[NT4.0:SHELL32.507]
 *
 */
HRESULT WINAPI SHRegOpenKeyW (
	HKEY hkey,
	LPCWSTR lpszSubKey,
	LPHKEY retkey)
{
	WARN("0x%04x %s %p\n",hkey,debugstr_w(lpszSubKey),retkey);
	return RegOpenKeyW( hkey, lpszSubKey, retkey );
}

/*************************************************************************
 * SHRegQueryValueExA   [SHELL32.509]
 *
 */
HRESULT WINAPI SHRegQueryValueExA(
	HKEY hkey,
	LPSTR lpValueName,
	LPDWORD lpReserved,
	LPDWORD lpType,
	LPBYTE lpData,
	LPDWORD lpcbData)
{
	TRACE("0x%04x %s %p %p %p %p\n", hkey, lpValueName, lpReserved, lpType, lpData, lpcbData);
	return RegQueryValueExA (hkey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

/*************************************************************************
 * SHRegQueryValueW				[NT4.0:SHELL32.510]
 *
 */
HRESULT WINAPI SHRegQueryValueW(
	HKEY hkey,
	LPWSTR lpszSubKey,
	LPWSTR lpszData,
	LPDWORD lpcbData )
{
	WARN("0x%04x %s %p %p semi-stub\n",
		hkey, debugstr_w(lpszSubKey), lpszData, lpcbData);
	return RegQueryValueW( hkey, lpszSubKey, lpszData, (LPLONG)lpcbData );
}

/*************************************************************************
 * SHRegQueryValueExW	[NT4.0:SHELL32.511]
 *
 * FIXME 
 *  if the datatype REG_EXPAND_SZ then expand the string and change
 *  *pdwType to REG_SZ. 
 */
HRESULT WINAPI SHRegQueryValueExW (
	HKEY hkey,
	LPWSTR pszValue,
	LPDWORD pdwReserved,
	LPDWORD pdwType,
	LPVOID pvData,
	LPDWORD pcbData)
{
	DWORD ret;
	WARN("0x%04x %s %p %p %p %p semi-stub\n",
		hkey, debugstr_w(pszValue), pdwReserved, pdwType, pvData, pcbData);
	ret = RegQueryValueExW ( hkey, pszValue, pdwReserved, pdwType, (LPBYTE)pvData, pcbData);
	return ret;
}


/*****************************************************************************
 * Name      : HRESULT SHRegDeleteKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.512
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

ODINFUNCTION2(HRESULT, SHRegDeleteKeyW,
              HKEY,    hkey,
              LPWSTR,  lpszKey)
{
  return RegDeleteKeyW(hkey,
                       lpszKey);
}


/*************************************************************************
 * SHRegCloseKey			[NT4.0:SHELL32.505]
 *
 */
HRESULT WINAPI SHRegCloseKey (HKEY hkey)
{
	TRACE("0x%04x\n",hkey);
	return RegCloseKey( hkey );
}
