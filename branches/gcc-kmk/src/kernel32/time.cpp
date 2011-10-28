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
#include <FastInfoBlocks.h>

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

extern "C" {

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
    dprintf(("time: %d-%d-%d %02d:%02d:%02d", lpSystemTime->wDay, lpSystemTime->wMonth, lpSystemTime->wYear, lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond));
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
    dprintf2(("time: %d-%d-%d %02d:%02d:%02d", lpSystemTime->wDay, lpSystemTime->wMonth, lpSystemTime->wYear, lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond));
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
static const LPCSTR szTZBias           = "Bias";
static const LPCSTR szTZActiveTimeBias = "ActiveTimeBias";

static const LPCWSTR szTZStandardName  = (LPCWSTR)L"StandardName";
static const LPCSTR szTZStandardBias   = "StandardBias";
static const LPCSTR szTZStandardStart  = "StandardStart";

static const LPCWSTR szTZDaylightName  = (LPCWSTR)L"DaylightName";
static const LPCSTR szTZDaylightBias   = "DaylightBias";
static const LPCSTR szTZDaylightStart  = "DaylightStart";
static const LPCSTR KEY_WINDOWS_TZ     = "SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation";
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

        dprintf(("Bias         %x", lpTimeZone->Bias));
        dprintf(("StandardName %ls", lpTimeZone->StandardName));
        dprintf(("StandardBias %x", lpTimeZone->StandardBias));
	dprintf(("StandardDate %d-%d-%d-%d", lpTimeZone->StandardDate.wYear, lpTimeZone->StandardDate.wMonth, lpTimeZone->StandardDate.wDay, lpTimeZone->StandardDate.wDayOfWeek));
        dprintf(("DaylightName %ls", lpTimeZone->DaylightName));
        dprintf(("DaylightBias %x", lpTimeZone->DaylightBias));
	dprintf(("DaylightDate %d-%d-%d-%d\n", lpTimeZone->DaylightDate.wYear, lpTimeZone->DaylightDate.wMonth, lpTimeZone->DaylightDate.wDay, lpTimeZone->DaylightDate.wDayOfWeek));

        //TODO: determine daylight or standard time
        return TIME_ZONE_ID_UNKNOWN;
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

        dprintf(("Bias         %x", lpTimeZone->Bias));
        dprintf(("StandardName %ls", lpTimeZone->StandardName));
        dprintf(("StandardBias %x", lpTimeZone->StandardBias));
	dprintf(("StandardDate %d-%d-%d-%d", lpTimeZone->StandardDate.wYear, lpTimeZone->StandardDate.wMonth, lpTimeZone->StandardDate.wDay, lpTimeZone->StandardDate.wDayOfWeek));
        dprintf(("DaylightName %ls", lpTimeZone->DaylightName));
        dprintf(("DaylightBias %x", lpTimeZone->DaylightBias));
	dprintf(("DaylightDate %d-%d-%d-%d\n", lpTimeZone->DaylightDate.wYear, lpTimeZone->DaylightDate.wMonth, lpTimeZone->DaylightDate.wDay, lpTimeZone->DaylightDate.wDayOfWeek));
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
    /*
     *  Speculative time caching.
     *      We assume GetSystemTime is using DosGetDateTime.
     *      We assume DosGetDateTime uses the global info segment.
     *      We assume that SIS_MsCount is updated when the rest of the date/time
     *          members of the global info segment is updated.
     *
     *  Possible sideffects:
     *    - The code doens't take in account changes of timezone, and hence will
     *      be wrong until the next timer tick. This isn't a problem I think.
     *    -
     */
    #if 1
    static  FILETIME    LastFileTime;
    static  ULONG       LastMsCount = -1;
    if (fibGetMsCount() == LastMsCount) {
        *lpSystemTimeAsFileTime = LastFileTime;
    }
    else
    {
        SYSTEMTIME      st;
        ULONG           ulNewMsCount = fibGetMsCount();
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, lpSystemTimeAsFileTime);
        LastFileTime = *lpSystemTimeAsFileTime;
        LastMsCount = ulNewMsCount;
    }
    dprintf2(("Time %08x%08x", lpSystemTimeAsFileTime->dwHighDateTime, lpSystemTimeAsFileTime->dwLowDateTime));
    #else
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, lpSystemTimeAsFileTime);
    dprintf2(("Time %08x%08x", lpSystemTimeAsFileTime->dwHighDateTime, lpSystemTimeAsFileTime->dwLowDateTime));
    #endif
}
//******************************************************************************
//******************************************************************************

} // extern "C"

