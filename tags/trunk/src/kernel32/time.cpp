/* $Id: time.cpp,v 1.24 2003-01-03 11:26:43 sandervl Exp $ */

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
//******************************************************************************
DWORD TimeZoneName(LPSTR name, LPWSTR text, BOOL read)
{
    CHAR  AsciiText_1[32];
    CHAR  AsciiText_2[32];
    HKEY  hKeyOptions;
    DWORD dwKeyType;
    DWORD dwDataLength = sizeof(AsciiText_1);
    DWORD dwDataLength_2;
    BOOL  Set = FALSE;

    if (read) {
       ZeroMemory(text, 2*sizeof(AsciiText_1));
    }
    else {
       UnicodeToAscii(text, &AsciiText_2[0]);
       dwDataLength_2 = strlen(AsciiText_2);
    }
    DWORD rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                             "SOFTWARE\\TIME\\TIME_SETTING",
                             0,
                             (read) ? KEY_READ : KEY_ALL_ACCESS,
                             &hKeyOptions);
    if (rc == ERROR_SUCCESS) 
    {
        rc = RegQueryValueExA(hKeyOptions,
                              name,
                              0,
                              &dwKeyType,
                              (LPBYTE)AsciiText_1,
                              &dwDataLength);
        if (rc == ERROR_SUCCESS) {
            if (read) AsciiToUnicode(&AsciiText_1[0], text);
            else Set = (strcmp(AsciiText_1,AsciiText_2) != 0);
        } 
        else Set = (!read);
    } 
    else {
        if (!read) {
            rc = RegCreateKeyA(HKEY_LOCAL_MACHINE,
                               "SOFTWARE\\TIME\\TIME_SETTING",
                               &hKeyOptions);
            Set = (rc == ERROR_SUCCESS);
        }
    }
    if (Set) rc = RegSetValueExA(hKeyOptions,
                                 name,
                                 0,
                                 REG_SZ,
                                 (LPBYTE)AsciiText_2,
                                 dwDataLength_2);

    RegCloseKey(hKeyOptions);
    return rc;
}
//******************************************************************************
DWORD WIN32API GetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZone)
{
    DWORD ret = O32_GetTimeZoneInformation(lpTimeZone);

    //Convert timezone names to unicode as WGSS (wrongly) returns ascii strings
    TimeZoneName("STANDARD_NAME", &lpTimeZone->StandardName[0], TRUE);
    TimeZoneName("DAYLIGHT_NAME", &lpTimeZone->DaylightName[0], TRUE);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetTimeZoneInformation(const LPTIME_ZONE_INFORMATION lpTimeZone)
{
    //Convert timezone names to ascii as WGSS (wrongly) expects that
    TimeZoneName("STANDARD_NAME", &lpTimeZone->StandardName[0], FALSE);
    TimeZoneName("DAYLIGHT_NAME", &lpTimeZone->DaylightName[0], FALSE);

    return O32_SetTimeZoneInformation(lpTimeZone);
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


