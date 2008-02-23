/* $Id: time.cpp,v 1.25 2003-01-08 14:25:40 sandervl Exp $ */

/*
 * Win32 time/date API functions
 *
 * Copyright 1998-2002 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>

#include <winnls.h>
#include "winuser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "unicode.h"
#include "oslibtime.h"
#include "winreg.h"

#define DBG_LOCALLOG	DBG_time
#include "dbglocal.h"


//******************************************************************************
//File time (UTC) to MS-DOS date & time values (also UTC)
//******************************************************************************
BOOL WIN32API FileTimeToDosDateTime(const FILETIME *lpFileTime, LPWORD lpDosDate, 
                                    LPWORD lpDosTime)
{
    if(lpFileTime == NULL || lpDosDate == NULL || lpDosTime == NULL )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return O32_FileTimeToDosDateTime(lpFileTime, lpDosDate, lpDosTime);
}
//******************************************************************************
//File time (UTC) to local time
//******************************************************************************
BOOL WIN32API FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalTime)
{
    if(lpFileTime == NULL || lpLocalTime == NULL || lpFileTime == lpLocalTime)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return O32_FileTimeToLocalFileTime(lpFileTime, lpLocalTime); 
}
//******************************************************************************
//Local time to File time (UTC)
//******************************************************************************
BOOL WIN32API LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, 
                                      LPFILETIME lpFileTime)
{
    BOOL ret;

    if(!lpLocalFileTime || !lpFileTime || lpLocalFileTime == lpFileTime) {
        dprintf(("!WARNING!: invalid parameter"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    dprintf(("KERNEL32: LocalFileTimeToFileTime %x%x", lpLocalFileTime->dwHighDateTime, lpLocalFileTime->dwLowDateTime));
    ret = O32_LocalFileTimeToFileTime(lpLocalFileTime, lpFileTime);
    dprintf(("KERNEL32: LocalFileTimeToFileTime %x%x -> %d", lpFileTime->dwHighDateTime, lpFileTime->dwLowDateTime, ret));
    return ret;
}
//******************************************************************************
//File time (UTC) to System time (UTC)
//******************************************************************************
BOOL WIN32API FileTimeToSystemTime(const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime)
{
    BOOL ret;

    if(lpFileTime == NULL || lpSystemTime == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    ret = O32_FileTimeToSystemTime(lpFileTime, lpSystemTime);
    dprintf(("time: %d-%d-%d %d:%d:%d", lpSystemTime->wDay, lpSystemTime->wMonth, lpSystemTime->wYear, lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond));
    return ret;
}
//******************************************************************************
//MS-DOS date & time values (UTC) to File time (also UTC) 
//******************************************************************************
BOOL WIN32API DosDateTimeToFileTime(WORD wDosDate, WORD wDosTime, LPFILETIME pFileTime)
{
    dprintf(("%x %x", wDosDate, wDosTime));
    
    if(pFileTime == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return O32_DosDateTimeToFileTime(wDosDate, wDosTime, pFileTime);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTickCount(void)
{
    return OSLibDosGetTickCount();
}
//******************************************************************************
//The GetLocalTime function retrieves the current local date and time. 
//(in local time)
//******************************************************************************
void WIN32API GetLocalTime(LPSYSTEMTIME lpLocalTime)
{
    if(lpLocalTime == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    O32_GetLocalTime(lpLocalTime);
}
//******************************************************************************
//The SetLocalTime function sets the current local time and date. 
//(in local time)
//******************************************************************************
BOOL WIN32API SetLocalTime(const SYSTEMTIME *lpLocalTime)
{
    if(lpLocalTime == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return O32_SetLocalTime(lpLocalTime);
}
//******************************************************************************
//The GetSystemTime function retrieves the current system date and time. 
//The system time is expressed in Coordinated Universal Time (UTC).
//******************************************************************************
void WIN32API GetSystemTime(LPSYSTEMTIME lpSystemTime)
{
    if(lpSystemTime == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    O32_GetSystemTime(lpSystemTime);
}
//******************************************************************************
//The SetSystemTime function sets the current system time and date. 
//The system time is expressed in Coordinated Universal Time (UCT).
//******************************************************************************
BOOL WIN32API SetSystemTime(const SYSTEMTIME *lpSystemTime)
{
    if(lpSystemTime == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return O32_SetSystemTime(lpSystemTime);
}
//******************************************************************************
//System time (UTC) to File time (UTC)
//******************************************************************************
BOOL WIN32API SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime)
{
    return O32_SystemTimeToFileTime(lpSystemTime, lpFileTime);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SystemTimeToTzSpecificLocalTime(LPTIME_ZONE_INFORMATION lpTimeZone,
                                              LPSYSTEMTIME lpSystemTime,
                                              LPSYSTEMTIME lpLocalTime)
{
    return O32_SystemTimeToTzSpecificLocalTime(lpTimeZone, lpSystemTime, lpLocalTime);
}
//******************************************************************************
static const LPSTR szTZBias           = "Bias";
static const LPSTR szTZActiveTimeBias = "ActiveTimeBias";

static const LPWSTR szTZStandardName  = (LPWSTR)L"StandardName";
static const LPSTR szTZStandardBias   = "StandardBias";
static const LPSTR szTZStandardStart  = "StandardStart";

static const LPWSTR szTZDaylightName  = (LPWSTR)L"DaylightName";
static const LPSTR szTZDaylightBias   = "DaylightBias";
static const LPSTR szTZDaylightStart  = "DaylightStart";
static const LPSTR KEY_WINDOWS_TZ     = "SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation";
//******************************************************************************
DWORD WIN32API GetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZone)
{
    TIME_ZONE_INFORMATION tzone;
    int len;

    HKEY hkey;

    if(RegCreateKeyA(HKEY_LOCAL_MACHINE, KEY_WINDOWS_TZ, &hkey) == ERROR_SUCCESS)
    {
        DWORD type, size;
        DWORD rc;

        size = sizeof(lpTimeZone->Bias);
        rc = RegQueryValueExA(hkey, szTZBias,0,&type, (LPBYTE)&lpTimeZone->Bias, &size);
        if(rc || type != REG_DWORD) {
            goto fail;
        }
        size = sizeof(lpTimeZone->StandardName);
        rc = RegQueryValueExW(hkey, szTZStandardName, 0, &type, (LPBYTE)lpTimeZone->StandardName, &size);
        if(rc || type != REG_SZ) {
            goto fail;
        }
        size = sizeof(lpTimeZone->StandardBias);
        rc = RegQueryValueExA(hkey, szTZStandardBias,0,&type, (LPBYTE)&lpTimeZone->StandardBias, &size);
        if(rc || type != REG_DWORD) {
            goto fail;
        }
        size = sizeof(lpTimeZone->StandardDate);
        rc = RegQueryValueExA(hkey, szTZStandardStart,0,&type, (LPBYTE)&lpTimeZone->StandardDate, &size);
        if(rc || type != REG_BINARY) {
            goto fail;
        }

        size = sizeof(lpTimeZone->DaylightName);
        rc = RegQueryValueExW(hkey, szTZDaylightName, 0, &type, (LPBYTE)lpTimeZone->DaylightName, &size);
        if(rc || type != REG_SZ) {
            goto fail;
        }
        size = sizeof(lpTimeZone->DaylightBias);
        rc = RegQueryValueExA(hkey, szTZDaylightBias,0,&type, (LPBYTE)&lpTimeZone->DaylightBias, &size);
        if(rc || type != REG_DWORD) {
            goto fail;
        }
        size = sizeof(lpTimeZone->DaylightDate);
        rc = RegQueryValueExA(hkey, szTZDaylightStart,0,&type, (LPBYTE)&lpTimeZone->DaylightDate, &size);
        if(rc || type != REG_BINARY) {
            goto fail;
        }
        RegCloseKey(hkey);

        //TODO: we should return whether or we are in standard or daylight time
        return TIME_ZONE_ID_STANDARD;
    }
    else
    {//get it from WGSS
fail:
        DWORD ret = O32_GetTimeZoneInformation(&tzone);

        *lpTimeZone = tzone;

        //Convert timezone names to unicode as WGSS (wrongly) returns ascii strings
        len = sizeof(tzone.StandardName)/sizeof(WCHAR);
        lstrcpynAtoW(lpTimeZone->StandardName, (LPSTR)tzone.StandardName, len);
        lpTimeZone->StandardName[len] = 0;

        lstrcpynAtoW(lpTimeZone->DaylightName, (LPSTR)tzone.DaylightName, len);
        lpTimeZone->DaylightName[len] = 0;
        return ret;
    }
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetTimeZoneInformation(const LPTIME_ZONE_INFORMATION lpTimeZone)
{
    TIME_ZONE_INFORMATION tzone = *lpTimeZone;
    int len;
    HKEY hkey;

    if(RegCreateKeyA(HKEY_LOCAL_MACHINE, KEY_WINDOWS_TZ, &hkey) != ERROR_SUCCESS)
    {
        dprintf(("ERROR: SetTimeZoneInformation: Unable to create key"));
        return FALSE;
    }
    RegSetValueExA(hkey, szTZBias,0,REG_DWORD, (LPBYTE)&lpTimeZone->Bias, sizeof(lpTimeZone->Bias));
    RegSetValueExA(hkey, szTZActiveTimeBias,0,REG_DWORD, (LPBYTE)&lpTimeZone->Bias, sizeof(lpTimeZone->Bias));

    RegSetValueExW(hkey, szTZStandardName, 0, REG_SZ, (LPBYTE)lpTimeZone->StandardName, lstrlenW(lpTimeZone->StandardName));
    RegSetValueExA(hkey, szTZStandardBias,0,REG_DWORD, (LPBYTE)&lpTimeZone->StandardBias, sizeof(lpTimeZone->StandardBias));
    RegSetValueExA(hkey, szTZStandardStart,0,REG_BINARY, (LPBYTE)&lpTimeZone->StandardDate, sizeof(lpTimeZone->StandardDate));

    RegSetValueExW(hkey, szTZDaylightName, 0, REG_SZ, (LPBYTE)lpTimeZone->DaylightName, lstrlenW(lpTimeZone->DaylightName));
    RegSetValueExA(hkey, szTZDaylightBias,0,REG_DWORD, (LPBYTE)&lpTimeZone->DaylightBias, sizeof(lpTimeZone->DaylightBias));
    RegSetValueExA(hkey, szTZDaylightStart,0,REG_BINARY, (LPBYTE)&lpTimeZone->DaylightDate, sizeof(lpTimeZone->DaylightDate));
    RegCloseKey(hkey);

    //Convert timezone names to ascii as WGSS (wrongly) expects that
    len = sizeof(tzone.StandardName)/sizeof(WCHAR);
    lstrcpynWtoA((LPSTR)tzone.StandardName, lpTimeZone->StandardName, len);
    tzone.StandardName[len] = 0;

    lstrcpynWtoA((LPSTR)tzone.DaylightName, lpTimeZone->DaylightName, len);
    tzone.DaylightName[len] = 0;

    return O32_SetTimeZoneInformation(&tzone);
}
/*****************************************************************************
 * Name      : DWORD GetSystemTimeAsFileTime
 * Purpose   : The GetSystemTimeAsFileTime function obtains the current system
 *             date and time. The information is in Coordinated Universal Time (UCT) format.
 * Parameters: LLPFILETIME lLPSYSTEMTIMEAsFileTime
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

VOID WIN32API GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
    SYSTEMTIME st;

    GetSystemTime(&st);
    SystemTimeToFileTime(&st, lpSystemTimeAsFileTime);
}
//******************************************************************************
//******************************************************************************


