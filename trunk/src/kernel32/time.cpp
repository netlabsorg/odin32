/* $Id: time.cpp,v 1.16 2001-07-20 15:33:30 sandervl Exp $ */

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
#include "unicode.h"

#define DBG_LOCALLOG	DBG_time
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

ODINDEBUGCHANNEL(KERNEL32-TIME)



#define lstrcpynAtoW(unicode,ascii,asciilen) AsciiToUnicodeN(ascii,unicode,asciilen);

#define WPRINTF_LEFTALIGN   0x0001  /* Align output on the left ('-' prefix) */
#define WPRINTF_PREFIX_HEX  0x0002  /* Prefix hex with 0x ('#' prefix) */
#define WPRINTF_ZEROPAD     0x0004  /* Pad with zeros ('0' prefix) */
#define WPRINTF_LONG        0x0008  /* Long arg ('l' prefix) */
#define WPRINTF_SHORT       0x0010  /* Short arg ('h' prefix) */
#define WPRINTF_UPPER_HEX   0x0020  /* Upper-case hex ('X' specifier) */
#define WPRINTF_WIDE        0x0040  /* Wide arg ('w' prefix) */

typedef enum
{
    WPR_UNKNOWN,
    WPR_CHAR,
    WPR_WCHAR,
    WPR_STRING,
    WPR_WSTRING,
    WPR_SIGNED,
    WPR_UNSIGNED,
    WPR_HEXA
} WPRINTF_TYPE;

typedef struct
{
    UINT         flags;
    UINT         width;
    UINT         precision;
    WPRINTF_TYPE   type;
} WPRINTF_FORMAT;

typedef union {
    WCHAR   wchar_view;
    CHAR    char_view;
    LPCSTR  lpcstr_view;
    LPCWSTR lpcwstr_view;
    INT     int_view;
} WPRINTF_DATA;

static const CHAR null_stringA[] = "(null)";
static const WCHAR null_stringW[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };

//******************************************************************************
//******************************************************************************
ODINPROCEDURE1(GetLocalTime,
               LPSYSTEMTIME, arg1)
{
  O32_GetLocalTime(arg1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, SetLocalTime,
              const SYSTEMTIME *, arg1)
{
  return O32_SetLocalTime(arg1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,FileTimeToDosDateTime,
              const FILETIME *, arg1, 
              LPWORD, arg2, 
              LPWORD,  arg3)
{
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
ODINFUNCTION2(BOOL, FileTimeToSystemTime,
              const FILETIME *, arg1, 
              LPSYSTEMTIME, arg2)
{
  return O32_FileTimeToSystemTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,DosDateTimeToFileTime,
              WORD, arg1, 
              WORD, arg2, 
              LPFILETIME, arg3)
{
    return O32_DosDateTimeToFileTime(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GetTimeZoneInformation,
              LPTIME_ZONE_INFORMATION, arg1)
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
ODINPROCEDURE1(GetSystemTime,
               LPSYSTEMTIME, arg1)
{
    O32_GetSystemTime(arg1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SystemTimeToFileTime,
              const SYSTEMTIME *, arg1, 
              LPFILETIME,  arg2)
{
  return O32_SystemTimeToFileTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, SystemTimeToTzSpecificLocalTime,
              LPTIME_ZONE_INFORMATION, arg1,
              LPSYSTEMTIME, arg2,
              LPSYSTEMTIME, arg3)
{
  return O32_SystemTimeToTzSpecificLocalTime(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, SetTimeZoneInformation,
              const LPTIME_ZONE_INFORMATION, arg1)
{
  return O32_SetTimeZoneInformation(arg1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL,SetSystemTime,
              const SYSTEMTIME *, arg1)
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
