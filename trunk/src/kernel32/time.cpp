/* $Id: time.cpp,v 1.1 1999-05-24 20:19:49 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 timer API functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include <winnls.h>

//******************************************************************************
//******************************************************************************
VOID WIN32API GetLocalTime( LPSYSTEMTIME arg1)
{
///    dprintf(("KERNEL32:  GetLocalTime\n"));
    O32_GetLocalTime(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetLocalTime( const SYSTEMTIME * arg1)
{
    dprintf(("KERNEL32:  SetLocalTime\n"));
    return O32_SetLocalTime(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FileTimeToDosDateTime(const FILETIME * arg1, LPWORD arg2, LPWORD  arg3)
{
    dprintf(("KERNEL32:  FileTimeToDosDateTime\n"));
    return O32_FileTimeToDosDateTime(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FileTimeToLocalFileTime(const FILETIME * arg1, LPFILETIME arg2)
{
    dprintf(("KERNEL32:  FileTimeToLocalFileTime\n"));
    return O32_FileTimeToLocalFileTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LocalFileTimeToFileTime(const FILETIME * arg1, LPFILETIME arg2)
{
    dprintf(("KERNEL32:  LocalFileTimeToFileTime\n"));
    return O32_LocalFileTimeToFileTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FileTimeToSystemTime(const FILETIME * arg1, LPSYSTEMTIME  arg2)
{
    dprintf(("KERNEL32:  FileTimeToSystemTime"));
    return O32_FileTimeToSystemTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DosDateTimeToFileTime( WORD arg1, WORD arg2, LPFILETIME arg3)
{
    dprintf(("KERNEL32:  DosDateTimeToFileTime\n"));
    return O32_DosDateTimeToFileTime(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetFileTime( HANDLE arg1, LPFILETIME arg2, LPFILETIME arg3, LPFILETIME  arg4)
{
    dprintf(("KERNEL32:  GetFileTime\n"));
    return O32_GetFileTime(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTimeZoneInformation( LPTIME_ZONE_INFORMATION arg1)
{
    dprintf(("KERNEL32:  GetTimeZoneInformation\n"));
    return O32_GetTimeZoneInformation(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTickCount(void)
{
////    dprintf(("KERNEL32:  GetTickCount\n"));
    return O32_GetTickCount();
}
//******************************************************************************
//******************************************************************************
VOID WIN32API GetSystemTime( LPSYSTEMTIME arg1)
{
    dprintf(("KERNEL32:  OS2GetSystemTime\n"));
    O32_GetSystemTime(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SystemTimeToFileTime( const SYSTEMTIME * arg1, LPFILETIME  arg2)
{
    dprintf(("KERNEL32:  OS2SystemTimeToFileTime\n"));
    return O32_SystemTimeToFileTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SystemTimeToTzSpecificLocalTime( LPTIME_ZONE_INFORMATION arg1, LPSYSTEMTIME arg2, LPSYSTEMTIME  arg3)
{
    dprintf(("KERNEL32:  OS2SystemTimeToTzSpecificLocalTime\n"));
    return O32_SystemTimeToTzSpecificLocalTime(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetTimeZoneInformation( const LPTIME_ZONE_INFORMATION arg1)
{
    dprintf(("KERNEL32:  OS2SetTimeZoneInformation\n"));
    return O32_SetTimeZoneInformation(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetSystemTime(const SYSTEMTIME * arg1)
{
    dprintf(("KERNEL32:  OS2SetSystemTime\n"));
    return O32_SetSystemTime(arg1);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetTimeFormatA(LCID Locale, DWORD dwFlags, LPSYSTEMTIME lpTime,
                            LPCSTR lpFormat, LPSTR lpTimeStr, INT cchTime)
{
  dprintf(("KERNEL32:  OS2GetTimeFormatA not implemented!!\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetTimeFormatW(LCID Locale, DWORD dwFlags, LPSYSTEMTIME lpTime,
                               LPCWSTR lpFormat, LPWSTR lpTimeStr, INT cchTime)
{
  dprintf(("KERNEL32:  OS2GetTimeFormatW not implemented!!\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDateFormatA(LCID Locale, DWORD dwFlags, LPSYSTEMTIME lpTime,
                            LPCSTR lpFormat, LPSTR lpDateStr, int cchDate)
{
  dprintf(("KERNEL32:  OS2GetDateFormatA not implemented!!\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDateFormatW(LCID Locale, DWORD dwFlags, LPSYSTEMTIME lpTime,
                            LPCWSTR lpFormat, LPWSTR lpDateStr, int cchDate)
{
  dprintf(("KERNEL32:  OS2GetDateFormatW not implemented!!\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumTimeFormatsA(TIMEFMT_ENUMPROCA lpDataFmtEnumProc,
                                  LCID Locale, DWORD dwFlags)
{
  dprintf(("KERNEL32:  OS2EnumTimeFormatsA, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumTimeFormatsW(TIMEFMT_ENUMPROCW lpDataFmtEnumProc,
                                  LCID Locale, DWORD dwFlags)
{
  dprintf(("KERNEL32:  OS2EnumTimeFormatsW, not implemented\n"));
  return(FALSE);
}
/*****************************************************************************
 * Name      : DWORD GetSystemTimeAsFileTime
 * Purpose   : The GetSystemTimeAsFileTime function obtains the current system
 *             date and time. The information is in Coordinated Universal Time (UTC) format.
 * Parameters: LLPFILETIME lLPSYSTEMTIMEAsFileTime
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

VOID WIN32API GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
 FILETIME   ft;                             /* code sequence from WIN32.HLP */
 SYSTEMTIME st;

  dprintf(("KERNEL32: GetSystemTimeAsFileTime(%08xh)\n", lpSystemTimeAsFileTime));

  GetSystemTime(&st);
  SystemTimeToFileTime(&st, &ft);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumDateFormatsA(DATEFMT_ENUMPROCA lpDataFmtEnumProc,
                               LCID Locale, DWORD dwFlags)
{
  dprintf(("KERNEL32:  OS2EnumDateFormatsA, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumDateFormatsW(DATEFMT_ENUMPROCW lpDataFmtEnumProc,
                               LCID Locale, DWORD dwFlags)
{
  dprintf(("KERNEL32:  OS2EnumDateFormatsW, not implemented\n"));
  return(FALSE);
}
