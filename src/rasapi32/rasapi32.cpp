/* $Id: rasapi32.cpp,v 1.1 1999-09-01 07:28:46 sandervl Exp $ */
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
#include "debugtools.h"
#include <debugdefs.h>


DEFAULT_DEBUG_CHANNEL(ras)



/**************************************************************************
 *                 RasConnectionNotification32A	[RASAPI32.550]
 */
DWORD APIENTRY RasConnectionNotificationA( HRASCONN hras, HANDLE handle, DWORD dword )
{
	return 0;
}

/**************************************************************************
 *                 RasConnectionNotification32W	[RASAPI32.551]
 */
DWORD APIENTRY RasConnectionNotificationW( HRASCONN hras, HANDLE handle, DWORD dword )
{
	return 0;
}

/**************************************************************************
 *                 RasCreatePhonebookEntry32A	[RASAPI32.559]
 */
DWORD APIENTRY RasCreatePhonebookEntryA( HWND hwnd, LPSTR lpstr)
{
	return 0;
}

/**************************************************************************
 *                 RasCreatePhonebookEntry32W	[RASAPI32.560]
 */
DWORD APIENTRY RasCreatePhonebookEntryW( HWND hwnd, LPWSTR lpwstr )
{
	return 0;
}

/**************************************************************************
 *                 RasDial32A			[RASAPI32.561]
 */
DWORD APIENTRY RasDialA( LPRASDIALEXTENSIONS rasdial, LPSTR str, LPRASDIALPARAMSA rasdial2, DWORD dword, LPVOID lpv, LPHRASCONN rascon )
{
	return 0;
}

/**************************************************************************
 *                 RasDial32W			[RASAPI32.562]
 */
DWORD APIENTRY RasDialW( LPRASDIALEXTENSIONS rasdial, LPWSTR str, LPRASDIALPARAMSW rasdial2, DWORD dword, LPVOID lpv, LPHRASCONN rascon )
{
	return 0;
}

/**************************************************************************
 *                 RasEditPhonebookEntry32A	[RASAPI32.563]
 */
DWORD APIENTRY RasEditPhonebookEntryA( HWND hwnd, LPSTR str, LPSTR str2 )
{
	return 0;
}

/**************************************************************************
 *                 RasEditPhonebookEntry32W	[RASAPI32.564]
 */
DWORD APIENTRY RasEditPhonebookEntryW( HWND hwnd, LPWSTR str, LPWSTR str2 )
{
	return 0;
}

/**************************************************************************
 *                 RasEnumConnections32A	[RASAPI32.565]
 */
DWORD WINAPI RasEnumConnectionsA( LPRASCONNA rca, LPDWORD x, LPDWORD y)
{
	return 0;
}

/**************************************************************************
 *                 RasEnumConnections32W	[RASAPI32.566]
 */
DWORD APIENTRY RasEnumConnectionsW( LPRASCONNW rca, LPDWORD x, LPDWORD y )
{
	return 0;
}

/**************************************************************************
 *                 RasEnumEntries32A		[RASAPI32.567]
 */
DWORD APIENTRY RasEnumEntriesA( LPSTR str, LPSTR str2, LPRASENTRYNAMEA rasentry, 
		LPDWORD lpdw, LPDWORD lpdw2 )
{
	return 0;
}

/**************************************************************************
 *                 RasEnumEntries32W		[RASAPI32.568]
 */
DWORD APIENTRY RasEnumEntriesW( LPWSTR str, LPWSTR str2, LPRASENTRYNAMEW rasentry,
		LPDWORD lpdw, LPDWORD lpdw2 )
{
	return 0;
}

/**************************************************************************
 *                 RasGetConnectStatus32A	[RASAPI32.569]
 */
DWORD APIENTRY RasGetConnectStatusA( HRASCONN hras, LPRASCONNSTATUSA rascon )
{
	return 0;
}

/**************************************************************************
 *                 RasGetConnectStatus32W	[RASAPI32.570]
 */
DWORD APIENTRY RasGetConnectStatusW( HRASCONN rasconn, LPRASCONNSTATUSW connstat )
{
	return 0;
}

/**************************************************************************
 *                 RasGetEntryDialParams32A	[RASAPI32.571]
 */
DWORD APIENTRY RasGetEntryDialParamsA( LPSTR str, LPRASDIALPARAMSA dialparm, LPBOOL lpb)
{
	return 0;
}

/**************************************************************************
 *                 RasGetEntryDialParams32W	[RASAPI32.572]
 */
DWORD APIENTRY RasGetEntryDialParamsW( LPWSTR str, LPRASDIALPARAMSW dialparm, LPBOOL lpb )
{
	return 0;
}

/**************************************************************************
 *                 RasGetErrorString32A		[RASAPI32.573]
 */
DWORD APIENTRY RasGetErrorStringA( UINT uint, LPSTR str, DWORD dword )
{
	return 0;
}

/**************************************************************************
 *                 RasGetErrorString32W		[RASAPI32.574]
 */
DWORD APIENTRY RasGetErrorStringW( UINT uint, LPWSTR str, DWORD dword )
{
	return 0;
}

/**************************************************************************
 *                 RasGetProjectionInfo32A	[RASAPI32.575]
 */
DWORD APIENTRY RasGetProjectionInfoA( HRASCONN rascon, RASPROJECTION rasprj, 
		LPVOID lpv, LPDWORD lpdw )
{
	return 0;
}

/**************************************************************************
 *                 RasGetProjectionInfo32W	[RASAPI32.576]
 */
DWORD APIENTRY RasGetProjectionInfoW( HRASCONN rascon, RASPROJECTION rasprj, 
		LPVOID lpv, LPDWORD lpdw )
{
	return 0;
}

/**************************************************************************
 *                 RasHangUp32A			[RASAPI32.577]
 */
DWORD APIENTRY RasHangUpA( HRASCONN hras )
{
	return 0;
}

/**************************************************************************
 *                 RasHangUp32W			[RASAPI32.578]
 */
DWORD APIENTRY RasHangUpW( HRASCONN hras )
{
	return 0;
}

/**************************************************************************
 *                 RasSetEntryDialParams32A	[RASAPI32.579]
 */
DWORD APIENTRY RasSetEntryDialParamsA( LPSTR str, LPRASDIALPARAMSA rasdp, BOOL bo )
{
	return 0;
}

/**************************************************************************
 *                 RasSetEntryDialParams32W	[RASAPI32.580]
 */
DWORD APIENTRY RasSetEntryDialParamsW( LPWSTR str, LPRASDIALPARAMSW rasdp, BOOL bo)
{
	return 0;
}



/**************************************************************************
 *                 RasValidateEntryName32A	[RASAPI32.612]
 */
DWORD APIENTRY RasValidateEntryNameA( LPSTR str, LPSTR str2)
{
	return 0;
}

/**************************************************************************
 *                 RasRenameEntry32A		[RASAPI32.613]
 */
DWORD APIENTRY RasRenameEntryA( LPSTR str, LPSTR str2, LPSTR str3 )
{
	return 0;
}

/**************************************************************************
 *                 RasDeleteEntry32A		[RASAPI32.614]
 */
DWORD APIENTRY RasDeleteEntryA( LPSTR str, LPSTR str2)
{
	return 0;
}

/**************************************************************************
 *                 RasGetEntryProperties32A	[RASAPI32.615]
 */
DWORD APIENTRY RasGetEntryPropertiesA( LPSTR str, LPSTR str2, LPRASENTRYA rasentry, LPDWORD dword, LPBYTE pbyte, LPDWORD dword2 )
{
	return 0;
}

/**************************************************************************
 *                 RasSetEntryProperties32A	[RASAPI32.616]
 */
DWORD APIENTRY RasSetEntryPropertiesA( LPSTR str, LPSTR str2, LPRASENTRYA rasentry, DWORD dword, LPBYTE pbyte, DWORD dword2 )
{
	return 0;
}

/**************************************************************************
 *                 RasEnumDevices32A		[RASAPI32.617]
 */
DWORD APIENTRY RasEnumDevicesA( LPRASDEVINFOA rasdevinfo, LPDWORD dword, LPDWORD dword2)
{
	return 0;
}

/**************************************************************************
 *                 RasGetCountryInfo32A		[RASAPI32.618]
 */
DWORD APIENTRY RasGetCountryInfoA( LPRASCTRYINFOA rasctryinfo, LPDWORD dword )
{
	return 0;
}
