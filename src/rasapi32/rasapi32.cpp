/* $Id: rasapi32.cpp,v 1.5 2000-02-21 10:33:34 sandervl Exp $ */
/*
 * RASAPI32
 * 
 * Copyright 1998 Marcus Meissner
 * Copyright 1999 Jens Wiessner
 */

/*	At the moment, these are only empty stubs.
 */

#include <os2win.h>
#include <ras.h>
#include <odinwrap.h>

ODINDEBUGCHANNEL(rasapi32)

/**************************************************************************
 *                 RasConnectionNotificationA	[RASAPI32.550]
 */
DWORD APIENTRY RasConnectionNotificationA( HRASCONN hras, HANDLE handle, DWORD dword )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasConnectionNotificationA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasConnectionNotificationW	[RASAPI32.551]
 */
DWORD APIENTRY RasConnectionNotificationW( HRASCONN hras, HANDLE handle, DWORD dword )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasConnectionNotificationA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasValidateEntryName		[RASAPI32.552]
 */
DWORD APIENTRY RasValidateEntryName(LPCTSTR lpszPhonebook, LPCTSTR lpszEntry)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasValidateEntryName not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasRenameEntry		[RASAPI32.553]
 */
DWORD APIENTRY RasRenameEntry(LPCTSTR lpszPhonebook, LPCTSTR lpszOldEntry, LPCTSTR lpszNewEntry)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasRenameEntry not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasDeleteEntry		[RASAPI32.554]
 */
DWORD APIENTRY RasDeleteEntry(LPCTSTR lpszPhonebook, LPCTSTR lpszEntry)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasDeleteEntry not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetEntryProperties		[RASAPI32.555]
 */
DWORD APIENTRY RasGetEntryProperties(LPCTSTR lpszPhonebook, LPCTSTR lpszEntry, 
	  LPRASENTRY lpRasEntry, LPDWORD lpdwEntryInfoSize,
	  LPBYTE lpbDeviceInfo, LPDWORD lpdwDeviceInfoSize)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetEntryProperties not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasSetEntryProperties		[RASAPI32.556]
 */
DWORD APIENTRY RasSetEntryProperties(LPCTSTR lpszPhonebook, LPCTSTR lpszEntry, 
	  LPRASENTRY lpRasEntry, DWORD dwEntryInfoSize,
	  LPBYTE lpbDeviceInfo, DWORD dwDeviceInfoSize)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasSetEntryProperties not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEnumDevices		[RASAPI32.557]
 */
DWORD APIENTRY RasEnumDevices(LPRASDEVINFO lpRasDevInfo,
		  LPDWORD lpcb, LPDWORD lpcDevices)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEnumDevices not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetCountryInfo		[RASAPI32.558]
 */
DWORD APIENTRY RasGetCountryInfo( LPRASCTRYINFO lpRasCtryInfo, LPDWORD lpdwSize)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetCountryInfo not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasCreatePhonebookEntryA	[RASAPI32.559]
 */
DWORD APIENTRY RasCreatePhonebookEntryA( HWND hwnd, LPSTR lpstr)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasCreatePhonebookEntryA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasCreatePhonebookEntryW	[RASAPI32.560]
 */
DWORD APIENTRY RasCreatePhonebookEntryW( HWND hwnd, LPWSTR lpwstr )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasCreatePhonebookEntryW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasDialA			[RASAPI32.561]
 */
DWORD APIENTRY RasDialA( LPRASDIALEXTENSIONS rasdial, LPSTR str, LPRASDIALPARAMSA rasdial2, DWORD dword, LPVOID lpv, LPHRASCONN rascon )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasDialA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasDialW			[RASAPI32.562]
 */
DWORD APIENTRY RasDialW( LPRASDIALEXTENSIONS rasdial, LPWSTR str, LPRASDIALPARAMSW rasdial2, DWORD dword, LPVOID lpv, LPHRASCONN rascon )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasDialW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEditPhonebookEntryA	[RASAPI32.563]
 */
DWORD APIENTRY RasEditPhonebookEntryA( HWND hwnd, LPSTR str, LPSTR str2 )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEditPhonebookEntryA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEditPhonebookEntryW	[RASAPI32.564]
 */
DWORD APIENTRY RasEditPhonebookEntryW( HWND hwnd, LPWSTR str, LPWSTR str2 )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEditPhonebookEntryA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEnumConnectionsA	[RASAPI32.565]
 */
DWORD WINAPI RasEnumConnectionsA( LPRASCONNA rca, LPDWORD x, LPDWORD y)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEnumConnectionsA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEnumConnectionsW	[RASAPI32.566]
 */
DWORD APIENTRY RasEnumConnectionsW( LPRASCONNW rca, LPDWORD x, LPDWORD y )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEnumConnectionsW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEnumEntriesA		[RASAPI32.567]
 */
DWORD APIENTRY RasEnumEntriesA( LPSTR str, LPSTR str2, LPRASENTRYNAMEA rasentry, 
		LPDWORD lpdw, LPDWORD lpdw2 )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEnumEntriesA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEnumEntriesW		[RASAPI32.568]
 */
DWORD APIENTRY RasEnumEntriesW( LPWSTR str, LPWSTR str2, LPRASENTRYNAMEW rasentry,
		LPDWORD lpdw, LPDWORD lpdw2 )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEnumEntriesW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetConnectStatusA	[RASAPI32.569]
 */
DWORD APIENTRY RasGetConnectStatusA( HRASCONN hras, LPRASCONNSTATUSA rascon )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetConnectStatusA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetConnectStatusW	[RASAPI32.570]
 */
DWORD APIENTRY RasGetConnectStatusW( HRASCONN rasconn, LPRASCONNSTATUSW connstat )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetConnectStatusW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetEntryDialParamsA	[RASAPI32.571]
 */
DWORD APIENTRY RasGetEntryDialParamsA( LPSTR str, LPRASDIALPARAMSA dialparm, LPBOOL lpb)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetEntryDialParamsA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetEntryDialParamsW	[RASAPI32.572]
 */
DWORD APIENTRY RasGetEntryDialParamsW( LPWSTR str, LPRASDIALPARAMSW dialparm, LPBOOL lpb )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetEntryDialParamsW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetErrorStringA		[RASAPI32.573]
 */
DWORD APIENTRY RasGetErrorStringA( UINT uint, LPSTR str, DWORD dword )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetErrorStringA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetErrorStringW		[RASAPI32.574]
 */
DWORD APIENTRY RasGetErrorStringW( UINT uint, LPWSTR str, DWORD dword )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetErrorStringW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetProjectionInfoA	[RASAPI32.575]
 */
DWORD APIENTRY RasGetProjectionInfoA( HRASCONN rascon, RASPROJECTION rasprj, 
		LPVOID lpv, LPDWORD lpdw )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetProjectInfoA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetProjectionInfoW	[RASAPI32.576]
 */
DWORD APIENTRY RasGetProjectionInfoW( HRASCONN rascon, RASPROJECTION rasprj, 
		LPVOID lpv, LPDWORD lpdw )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetProjectInfoW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasHangUpA			[RASAPI32.577]
 */
DWORD APIENTRY RasHangUpA( HRASCONN hras )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasHangUpA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasHangUpW			[RASAPI32.578]
 */
DWORD APIENTRY RasHangUpW( HRASCONN hras )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasHangUpW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasSetEntryDialParamsA	[RASAPI32.579]
 */
DWORD APIENTRY RasSetEntryDialParamsA( LPSTR str, LPRASDIALPARAMSA rasdp, BOOL bo )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasSetEntryDialParamsA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasSetEntryDialParamsW	[RASAPI32.580]
 */
DWORD APIENTRY RasSetEntryDialParamsW( LPWSTR str, LPRASDIALPARAMSW rasdp, BOOL bo)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasSetEntryDialParamsW not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaCloseMac			[RASAPI32.581]
 */
DWORD WINAPI RnaCloseMac(HANDLE hConn)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaCloseMac not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaComplete			[RASAPI32.582]
 */
DWORD APIENTRY RnaComplete(HANDLE hConn, LPCOMPLETE_INFO lpci, LPPROJECTION_INFO lppi, DWORD cEntries)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaComplete not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaGetDevicePort		[RASAPI32.583]
 */
DWORD WINAPI RnaGetDevicePort(HANDLE hConn, LPDEVICE_PORT_INFO lpdpi)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaGetDevicePort not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaGetUserProfile		[RASAPI32.585]
 */
DWORD WINAPI RnaGetUserProfile(HANDLE hConn, LPUSER_PROFILE lpUserProfile)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaGetUserProfile not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaOpenMac			[RASAPI32.586]
 */
/* DWORD WINAPI RnaOpenMac(HANDLE hConn, HANDLE * lphMAC, LPMAC_OPEN lpmo, DWORD dwSize, HANDLE hEvent)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaOpenMac not implemented\n"));
#endif
	return 0;
} */


/**************************************************************************
 *                 RnaSessInitialize		[RASAPI32.587]
 */
DWORD APIENTRY RnaSessInitialize(LPSTR lpszType, LPRNA_FUNCS lpRnaFuncs)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaSessInitialize not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaStartCallback		[RASAPI32.588]
 */
DWORD APIENTRY RnaStartCallback(HANDLE hConn, HANDLE hEvent)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaStartCallback not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaTerminate			[RASAPI32.589]
 */
DWORD APIENTRY RnaTerminate(HANDLE hConn, HANDLE hThread)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaTerminate not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaUICallbackDialog			[RASAPI32.590]
 */
DWORD WINAPI RnaUICallbackDialog(HANDLE hConn, LPSTR lpszLocList, DWORD dwType, BOOL fOptional, LPINT lpuIndex, LPSTR lpszSelectLocation, UINT cbBuff)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaUICallbackDialog not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RnaUIUsernamePassword			[RASAPI32.591]
 */
DWORD WINAPI RnaUIUsernamePassword(HANDLE hConn, LPSTR lpszUsername, UINT cbUsername, LPSTR lpszPassword, UINT cbPassword, LPSTR lpszDomain, UINT cbDomain)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RnaUIUsernamePassword not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasValidateEntryNameA	[RASAPI32.612]
 */
DWORD APIENTRY RasValidateEntryNameA( LPSTR str, LPSTR str2)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasValidateEntryNameA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasRenameEntryA		[RASAPI32.613]
 */
DWORD APIENTRY RasRenameEntryA( LPSTR str, LPSTR str2, LPSTR str3 )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasRenameEntryA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasDeleteEntryA		[RASAPI32.614]
 */
DWORD APIENTRY RasDeleteEntryA( LPSTR str, LPSTR str2)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasDeleteEntryA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetEntryPropertiesA	[RASAPI32.615]
 */
DWORD APIENTRY RasGetEntryPropertiesA( LPSTR str, LPSTR str2, LPRASENTRYA rasentry, LPDWORD dword, LPBYTE pbyte, LPDWORD dword2 )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetEntryPropertiesA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasSetEntryPropertiesA	[RASAPI32.616]
 */
DWORD APIENTRY RasSetEntryPropertiesA( LPSTR str, LPSTR str2, LPRASENTRYA rasentry, DWORD dword, LPBYTE pbyte, DWORD dword2 )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasSetEntryPropertiesA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasEnumDevicesA		[RASAPI32.617]
 */
DWORD APIENTRY RasEnumDevicesA( LPRASDEVINFOA rasdevinfo, LPDWORD dword, LPDWORD dword2)
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasEnumDevicesA not implemented\n"));
#endif
	return 0;
}


/**************************************************************************
 *                 RasGetCountryInfoA		[RASAPI32.618]
 */
DWORD APIENTRY RasGetCountryInfoA( LPRASCTRYINFOA rasctryinfo, LPDWORD dword )
{
#ifdef DEBUG
  dprintf(("RASAPI32: RasGetCountryInfoA not implemented\n"));
#endif
	return 0;
}
