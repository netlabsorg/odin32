/* $Id: time.cpp,v 1.14 2000-12-28 17:10:47 phaller Exp $ */

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

/***********************************************************************
 *           WPRINTF_ParseFormatA
 *
 * Parse a format specification. A format specification has the form:
 *
 * [-][#][0][width][.precision]type
 *
 * Return value is the length of the format specification in characters.
 */
static INT WPRINTF_ParseFormatA( LPCSTR format, WPRINTF_FORMAT *res )
{
    LPCSTR p = format;

    res->flags = 0;
    res->width = 0;
    res->precision = 0;
    if (*p == '-') { res->flags |= WPRINTF_LEFTALIGN; p++; }
    if (*p == '#') { res->flags |= WPRINTF_PREFIX_HEX; p++; }
    if (*p == '0') { res->flags |= WPRINTF_ZEROPAD; p++; }
    while ((*p >= '0') && (*p <= '9'))  /* width field */
    {
        res->width = res->width * 10 + *p - '0';
        p++;
    }
    if (*p == '.')  /* precision field */
    {
        p++;
        while ((*p >= '0') && (*p <= '9'))
        {
            res->precision = res->precision * 10 + *p - '0';
            p++;
        }
    }
    if (*p == 'l') { res->flags |= WPRINTF_LONG; p++; }
    else if (*p == 'h') { res->flags |= WPRINTF_SHORT; p++; }
    else if (*p == 'w') { res->flags |= WPRINTF_WIDE; p++; }
    switch(*p)
    {
    case 'c':
        res->type = (res->flags & WPRINTF_LONG) ? WPR_WCHAR : WPR_CHAR;
        break;
    case 'C':
        res->type = (res->flags & WPRINTF_SHORT) ? WPR_CHAR : WPR_WCHAR;
        break;
    case 'd':
    case 'i':
        res->type = WPR_SIGNED;
        break;
    case 's':
        res->type = (res->flags & (WPRINTF_LONG |WPRINTF_WIDE))
	            ? WPR_WSTRING : WPR_STRING;
        break;
    case 'S':
        res->type = (res->flags & (WPRINTF_SHORT|WPRINTF_WIDE))
	            ? WPR_STRING : WPR_WSTRING;
        break;
    case 'u':
        res->type = WPR_UNSIGNED;
        break;
    case 'X':
        res->flags |= WPRINTF_UPPER_HEX;
        /* fall through */
    case 'x':
        res->type = WPR_HEXA;
        break;
    default: /* unknown format char */
        res->type = WPR_UNKNOWN;
        p--;  /* print format as normal char */
        break;
    }
    return (INT)(p - format) + 1;
}


/***********************************************************************
 *           WPRINTF_ParseFormatW
 *
 * Parse a format specification. A format specification has the form:
 *
 * [-][#][0][width][.precision]type
 *
 * Return value is the length of the format specification in characters.
 */
static INT WPRINTF_ParseFormatW( LPCWSTR format, WPRINTF_FORMAT *res )
{
    LPCWSTR p = format;

    res->flags = 0;
    res->width = 0;
    res->precision = 0;
    if (*p == '-') { res->flags |= WPRINTF_LEFTALIGN; p++; }
    if (*p == '#') { res->flags |= WPRINTF_PREFIX_HEX; p++; }
    if (*p == '0') { res->flags |= WPRINTF_ZEROPAD; p++; }
    while ((*p >= '0') && (*p <= '9'))  /* width field */
    {
        res->width = res->width * 10 + *p - '0';
        p++;
    }
    if (*p == '.')  /* precision field */
    {
        p++;
        while ((*p >= '0') && (*p <= '9'))
        {
            res->precision = res->precision * 10 + *p - '0';
            p++;
        }
    }
    if (*p == 'l') { res->flags |= WPRINTF_LONG; p++; }
    else if (*p == 'h') { res->flags |= WPRINTF_SHORT; p++; }
    else if (*p == 'w') { res->flags |= WPRINTF_WIDE; p++; }
    switch((CHAR)*p)
    {
    case 'c':
        res->type = (res->flags & WPRINTF_SHORT) ? WPR_CHAR : WPR_WCHAR;
        break;
    case 'C':
        res->type = (res->flags & WPRINTF_LONG) ? WPR_WCHAR : WPR_CHAR;
        break;
    case 'd':
    case 'i':
        res->type = WPR_SIGNED;
        break;
    case 's':
        res->type = ((res->flags & WPRINTF_SHORT) && !(res->flags & WPRINTF_WIDE)) ? WPR_STRING : WPR_WSTRING;
        break;
    case 'S':
        res->type = (res->flags & (WPRINTF_LONG|WPRINTF_WIDE)) ? WPR_WSTRING : WPR_STRING;
        break;
    case 'u':
        res->type = WPR_UNSIGNED;
        break;
    case 'X':
        res->flags |= WPRINTF_UPPER_HEX;
        /* fall through */
    case 'x':
        res->type = WPR_HEXA;
        break;
    default:
        res->type = WPR_UNKNOWN;
        p--;  /* print format as normal char */
        break;
    }
    return (INT)(p - format) + 1;
}


/***********************************************************************
 *           WPRINTF_GetLen
 */
static UINT WPRINTF_GetLen( WPRINTF_FORMAT *format, WPRINTF_DATA *arg,
                              LPSTR number, UINT maxlen )
{
    UINT len;

    if (format->flags & WPRINTF_LEFTALIGN) format->flags &= ~WPRINTF_ZEROPAD;
    if (format->width > maxlen) format->width = maxlen;
    switch(format->type)
    {
    case WPR_CHAR:
    case WPR_WCHAR:
        return (format->precision = 1);
    case WPR_STRING:
        if (!arg->lpcstr_view) arg->lpcstr_view = null_stringA;
        for (len = 0; !format->precision || (len < format->precision); len++)
            if (!*(arg->lpcstr_view + len)) break;
        if (len > maxlen) len = maxlen;
        return (format->precision = len);
    case WPR_WSTRING:
        if (!arg->lpcwstr_view) arg->lpcwstr_view = null_stringW;
        for (len = 0; !format->precision || (len < format->precision); len++)
            if (!*(arg->lpcwstr_view + len)) break;
        if (len > maxlen) len = maxlen;
        return (format->precision = len);
    case WPR_SIGNED:
        len = sprintf( number, "%d", arg->int_view );
        break;
    case WPR_UNSIGNED:
        len = sprintf( number, "%u", (UINT)arg->int_view );
        break;
    case WPR_HEXA:
        len = sprintf( number,
                        (format->flags & WPRINTF_UPPER_HEX) ? "%X" : "%x",
                        (UINT)arg->int_view);
        if (format->flags & WPRINTF_PREFIX_HEX) len += 2;
        break;
    default:
        return 0;
    }
    if (len > maxlen) len = maxlen;
    if (format->precision < len) format->precision = len;
    if (format->precision > maxlen) format->precision = maxlen;
    if ((format->flags & WPRINTF_ZEROPAD) && (format->width > format->precision))
        format->precision = format->width;
    return len;
}

/***********************************************************************
 *           WPRINTF_ExtractVAPtr (Not a Windows API)
 */
static WPRINTF_DATA WPRINTF_ExtractVAPtr( WPRINTF_FORMAT *format, va_list* args )
{
    WPRINTF_DATA result;
    switch(format->type)
    {
        case WPR_WCHAR:
            result.wchar_view = va_arg( *args, WCHAR );     break;
        case WPR_CHAR:
            result.char_view = va_arg( *args, CHAR );       break;
        case WPR_STRING:
            result.lpcstr_view = va_arg( *args, LPCSTR);    break;
        case WPR_WSTRING:
            result.lpcwstr_view = va_arg( *args, LPCWSTR);  break;
        case WPR_HEXA:
        case WPR_SIGNED:
        case WPR_UNSIGNED:
            result.int_view = va_arg( *args, INT );         break;
        default:
            result.wchar_view = 0;                          break;
    }
    return result;
}

/***********************************************************************
 *           wvsnprintfA   (Not a Windows API)
 */
INT WINAPI wvsnprintfA( LPSTR buffer, UINT maxlen, LPCSTR spec,
                            va_list args )
{
    WPRINTF_FORMAT format;
    LPSTR p = buffer;
    UINT i, len;
    CHAR number[20];
    WPRINTF_DATA argData;

    while (*spec && (maxlen > 1))
    {
        if (*spec != '%') { *p++ = *spec++; maxlen--; continue; }
        spec++;
        if (*spec == '%') { *p++ = *spec++; maxlen--; continue; }
        spec += WPRINTF_ParseFormatA( spec, &format );
        argData = WPRINTF_ExtractVAPtr( &format, &args );
        len = WPRINTF_GetLen( &format, &argData, number, maxlen - 1 );
        if (!(format.flags & WPRINTF_LEFTALIGN))
            for (i = format.precision; i < format.width; i++, maxlen--)
                *p++ = ' ';
        switch(format.type)
        {
        case WPR_WCHAR:
	    *p = argData.wchar_view;
            if (*p != '\0') p++;
            else if (format.width > 1) *p++ = ' ';
            else len = 0;
            break;
        case WPR_CHAR:
	    *p = argData.char_view;
            if (*p != '\0') p++;
            else if (format.width > 1) *p++ = ' ';
            else len = 0;
            break;
        case WPR_STRING:
            memcpy( p, argData.lpcstr_view, len );
            p += len;
            break;
        case WPR_WSTRING:
            {
                LPCWSTR ptr = argData.lpcwstr_view;
                for (i = 0; i < len; i++) *p++ = (CHAR)*ptr++;
            }
            break;
        case WPR_HEXA:
            if ((format.flags & WPRINTF_PREFIX_HEX) && (maxlen > 3))
            {
                *p++ = '0';
                *p++ = (format.flags & WPRINTF_UPPER_HEX) ? 'X' : 'x';
                maxlen -= 2;
                len -= 2;
                format.precision -= 2;
                format.width -= 2;
            }
            /* fall through */
        case WPR_SIGNED:
        case WPR_UNSIGNED:
            for (i = len; i < format.precision; i++, maxlen--) *p++ = '0';
            memcpy( p, number, len );
            p += len;
            /* Go to the next arg */
            break;
        case WPR_UNKNOWN:
            continue;
        }
        if (format.flags & WPRINTF_LEFTALIGN)
            for (i = format.precision; i < format.width; i++, maxlen--)
                *p++ = ' ';
        maxlen -= len;
    }
    *p = 0;
    //TRACE("%s\n",buffer);
    return (maxlen > 1) ? (INT)(p - buffer) : -1;
}


/***********************************************************************
 *           wvsnprintfW   (Not a Windows API)
 */
INT WINAPI wvsnprintfW( LPWSTR buffer, UINT maxlen, LPCWSTR spec,
                            va_list args )
{
    WPRINTF_FORMAT format;
    LPWSTR p = buffer;
    UINT i, len;
    CHAR number[20];

    while (*spec && (maxlen > 1))
    {
        if (*spec != '%') { *p++ = *spec++; maxlen--; continue; }
        spec++;
        if (*spec == '%') { *p++ = *spec++; maxlen--; continue; }
        spec += WPRINTF_ParseFormatW( spec, &format );
        len = WPRINTF_GetLen( &format, (WPRINTF_DATA*)args, number, maxlen - 1 );
        if (!(format.flags & WPRINTF_LEFTALIGN))
            for (i = format.precision; i < format.width; i++, maxlen--)
                *p++ = ' ';
        switch(format.type)
        {
        case WPR_WCHAR:
	    *p = va_arg( args, WCHAR );
            if (*p != '\0') p++;
            else if (format.width > 1) *p++ = ' ';
            else len = 0;
            break;
        case WPR_CHAR:
	    *p = (WCHAR)va_arg( args, CHAR );
            if (*p != '\0') p++;
            else if (format.width > 1) *p++ = ' ';
            else len = 0;
            break;
        case WPR_STRING:
            {
                LPCSTR ptr = va_arg( args, LPCSTR );
                for (i = 0; i < len; i++) *p++ = (WCHAR)*ptr++;
            }
            break;
        case WPR_WSTRING:
            if (len) memcpy( p, va_arg( args, LPCWSTR ), len * sizeof(WCHAR) );
            p += len;
            break;
        case WPR_HEXA:
            if ((format.flags & WPRINTF_PREFIX_HEX) && (maxlen > 3))
            {
                *p++ = '0';
                *p++ = (format.flags & WPRINTF_UPPER_HEX) ? 'X' : 'x';
                maxlen -= 2;
                len -= 2;
                format.precision -= 2;
                format.width -= 2;
            }
            /* fall through */
        case WPR_SIGNED:
        case WPR_UNSIGNED:
            for (i = len; i < format.precision; i++, maxlen--) *p++ = '0';
            for (i = 0; i < len; i++) *p++ = (WCHAR)number[i];
            (void)va_arg( args, INT ); /* Go to the next arg */
            break;
        case WPR_UNKNOWN:
            continue;
        }
        if (format.flags & WPRINTF_LEFTALIGN)
            for (i = format.precision; i < format.width; i++, maxlen--)
                *p++ = ' ';
        maxlen -= len;
    }
    *p = 0;
    return (maxlen > 1) ? (INT)(p - buffer) : -1;
}

/***********************************************************************
 *           wsnprintfA   (Not a Windows API)
 */
INT WINAPIV wsnprintfA( LPSTR buffer, UINT maxlen, LPCSTR spec, ... )
{
    va_list valist;
    INT res;

    va_start( valist, spec );
    res = wvsnprintfA( buffer, maxlen, spec, valist );
    va_end( valist );
    return res;
}

/***********************************************************************
 *           wsnprintfW   (Not a Windows API)
 */
INT WINAPIV wsnprintfW( LPWSTR buffer, UINT maxlen, LPCWSTR spec, ... )
{
    va_list valist;
    INT res;

    va_start( valist, spec );
    res = wvsnprintfW( buffer, maxlen, spec, valist );
    va_end( valist );
    return res;
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
