/* $Id: time.cpp,v 1.21 2002-06-26 07:42:35 sandervl Exp $ */

/*
 * Win32 time/date API functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Copyright 1996 Alexandre Julliard
 * Copyright 1995 Martin von Loewis
 * Copyright 1998 David Lee Lambert

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

#define DBG_LOCALLOG	DBG_time
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
void WIN32API GetLocalTime(LPSYSTEMTIME arg1)
{
  O32_GetLocalTime(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetLocalTime(const SYSTEMTIME * arg1)
{
  return O32_SetLocalTime(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FileTimeToDosDateTime(const FILETIME * arg1,  LPWORD arg2, 
                                    LPWORD arg3)
{
    return O32_FileTimeToDosDateTime(arg1, arg2, arg3);
}

//******************************************************************************
//******************************************************************************
BOOL WIN32API FileTimeToLocalFileTime(const FILETIME * utcft, LPFILETIME localft)
{
   return O32_FileTimeToLocalFileTime(utcft,localft); 
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
BOOL WIN32API FileTimeToSystemTime(const FILETIME * arg1, LPSYSTEMTIME arg2)
{
   return O32_FileTimeToSystemTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DosDateTimeToFileTime(WORD arg1, WORD arg2, LPFILETIME arg3)
{
    return O32_DosDateTimeToFileTime(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTimeZoneInformation(LPTIME_ZONE_INFORMATION arg1)
{
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
void WIN32API GetSystemTime(LPSYSTEMTIME arg1)
{
    O32_GetSystemTime(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SystemTimeToFileTime(const SYSTEMTIME * arg1, 
                                   LPFILETIME arg2)
{
    return O32_SystemTimeToFileTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SystemTimeToTzSpecificLocalTime(LPTIME_ZONE_INFORMATION arg1,
                                              LPSYSTEMTIME arg2,
                                              LPSYSTEMTIME arg3)
{
  return O32_SystemTimeToTzSpecificLocalTime(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetTimeZoneInformation(const LPTIME_ZONE_INFORMATION arg1)
{
  return O32_SetTimeZoneInformation(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetSystemTime(const SYSTEMTIME * arg1)
{
  return O32_SetSystemTime(arg1);
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
