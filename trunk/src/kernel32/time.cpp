/* $Id: time.cpp,v 1.8 2000-05-22 19:07:59 sandervl Exp $ */

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
#include <os2win.h>
#include <winnls.h>
#include "winuser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "unicode.h"

#define DBG_LOCALLOG	DBG_time
#include "dbglocal.h"

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

/******************************************************************************
 *		OLE2NLS_CheckLocale	[intern]
 */
static LCID OLE2NLS_CheckLocale (LCID locale)
{
	if (!locale)
	{ locale = LOCALE_SYSTEM_DEFAULT;
	}

	if (locale == LOCALE_SYSTEM_DEFAULT)
  	{ return GetSystemDefaultLCID();
	}
	else if (locale == LOCALE_USER_DEFAULT)
	{ return GetUserDefaultLCID();
	}
	else
	{ return locale;
	}
}

/******************************************************************************
 *		OLE_GetFormatA	[Internal]
 *
 * FIXME
 *    If datelen == 0, it should return the reguired string length.
 *
 This function implements stuff for GetDateFormat() and
 GetTimeFormat().
  d    single-digit (no leading zero) day (of month)
  dd   two-digit day (of month)
  ddd  short day-of-week name
  dddd long day-of-week name
  M    single-digit month
  MM   two-digit month
  MMM  short month name
  MMMM full month name
  y    two-digit year, no leading 0
  yy   two-digit year
  yyyy four-digit year
  gg   era string
  h    hours with no leading zero (12-hour)
  hh   hours with full two digits
  H    hours with no leading zero (24-hour)
  HH   hours with full two digits
  m    minutes with no leading zero
  mm   minutes with full two digits
  s    seconds with no leading zero
  ss   seconds with full two digits
  t    time marker (A or P)
  tt   time marker (AM, PM)
  ''   used to quote literal characters
  ''   (within a quoted string) indicates a literal '
 These functions REQUIRE valid locale, date,  and format.
 */
static INT OLE_GetFormatA(LCID locale,
			    DWORD flags,
			    DWORD tflags,
			    LPSYSTEMTIME xtime,
			    LPCSTR _format, 	/*in*/
			    LPSTR date,		/*out*/
			    INT datelen)
{
   INT inpos, outpos;
   int count, type, inquote, Overflow;
   char buf[40];
   char format[40];
   char * pos;
   int buflen;
   const char * _dgfmt[] = { "%d", "%02d" };
   const char ** dgfmt = _dgfmt - 1;
   dprintf(("KERNEL32: OLE_GetFormatA"));

   if(datelen == 0) {
     //FIXME_(ole)("datelen = 0, returning 255\n");
     return 255;
   }
   /* initalize state variables and output buffer */
   inpos = outpos = 0;
   count = 0; inquote = 0; Overflow = 0;
   type = '\0';
   date[0] = buf[0] = '\0';

   strcpy(format,_format);
   /* alter the formatstring, while it works for all languages now in wine
   its possible that it fails when the time looks like ss:mm:hh as example*/
   if (tflags & (TIME_NOMINUTESORSECONDS))
   { if ((pos = strstr ( format, ":mm")) != 0)
     { memcpy ( pos, pos+3, strlen(format)-(pos-format)-2 );
     }
   }
   if (tflags & (TIME_NOSECONDS))
   { if ((pos = strstr ( format, ":ss")) != 0)
     { memcpy ( pos, pos+3, strlen(format)-(pos-format)-2 );
     }
   }

   for (inpos = 0;; inpos++) {
      /* TRACE(ole, "STATE inpos=%2d outpos=%2d count=%d inquote=%d type=%c buf,date = %c,%c\n", inpos, outpos, count, inquote, type, buf[inpos], date[outpos]); */
      if (inquote) {
	 if (format[inpos] == '\'') {
	    if (format[inpos+1] == '\'') {
	       inpos += 1;
	       date[outpos++] = '\'';
	    } else {
	       inquote = 0;
	       continue; /* we did nothing to the output */
	    }
	 } else if (format[inpos] == '\0') {
	    date[outpos++] = '\0';
	    if (outpos > datelen) Overflow = 1;
	    break;
	 } else {
	    date[outpos++] = format[inpos];
	    if (outpos > datelen) {
	       Overflow = 1;
	       date[outpos-1] = '\0'; /* this is the last place where
					 it's safe to write */
	       break;
	    }
	 }
      } else if (  (count && (format[inpos] != type))
		   || count == 4
		   || (count == 2 && strchr("ghHmst", type)) )
       {
	    if         (type == 'd') {
	       if        (count == 4) {
		  GetLocaleInfoA(locale,
				   LOCALE_SDAYNAME1
				   + xtime->wDayOfWeek - 1,
				   buf, sizeof(buf));
	       } else if (count == 3) {
			   GetLocaleInfoA(locale,
					    LOCALE_SABBREVDAYNAME1
					    + xtime->wDayOfWeek - 1,
					    buf, sizeof(buf));
		      } else {
		  sprintf(buf, dgfmt[count], xtime->wDay);
	       }
	    } else if (type == 'M') {
	       if (count == 3) {
		  GetLocaleInfoA(locale,
				   LOCALE_SABBREVMONTHNAME1
				   + xtime->wMonth - 1,
				   buf, sizeof(buf));
	       } else if (count == 4) {
		  GetLocaleInfoA(locale,
				   LOCALE_SMONTHNAME1
				   + xtime->wMonth - 1,
				   buf, sizeof(buf));
		 } else {
		  sprintf(buf, dgfmt[count], xtime->wMonth);
	       }
	    } else if (type == 'y') {
	       if (count == 4) {
		      sprintf(buf, "%d", xtime->wYear);
	       } else if (count == 3) {
		  strcpy(buf, "yyy");
		  //WARN_(ole)("unknown format, c=%c, n=%d\n",  type, count);
		 } else {
		  sprintf(buf, dgfmt[count], xtime->wYear % 100);
	       }
	    } else if (type == 'g') {
	       if        (count == 2) {
		  //FIXME_(ole)("LOCALE_ICALENDARTYPE unimp.\n");
		  strcpy(buf, "AD");
	    } else {
		  strcpy(buf, "g");
		  //WARN_(ole)("unknown format, c=%c, n=%d\n", type, count);
	       }
	    } else if (type == 'h') {
	       /* gives us hours 1:00 -- 12:00 */
	       sprintf(buf, dgfmt[count], (xtime->wHour-1)%12 +1);
	    } else if (type == 'H') {
	       /* 24-hour time */
	       sprintf(buf, dgfmt[count], xtime->wHour);
	    } else if ( type == 'm') {
	       sprintf(buf, dgfmt[count], xtime->wMinute);
	    } else if ( type == 's') {
	       sprintf(buf, dgfmt[count], xtime->wSecond);
	    } else if (type == 't') {
	       if        (count == 1) {
		  sprintf(buf, "%c", (xtime->wHour < 12) ? 'A' : 'P');
	       } else if (count == 2) {
		  /* sprintf(buf, "%s", (xtime->wHour < 12) ? "AM" : "PM"); */
		  GetLocaleInfoA(locale,
				   (xtime->wHour<12)
				   ? LOCALE_S1159 : LOCALE_S2359,
				   buf, sizeof(buf));
	       }
	    };
	    /* we need to check the next char in the format string
	       again, no matter what happened */
	    inpos--;
	
	    /* add the contents of buf to the output */
	    buflen = strlen(buf);
	    if (outpos + buflen < datelen) {
	       date[outpos] = '\0'; /* for strcat to hook onto */
		 strcat(date, buf);
	       outpos += buflen;
	    } else {
	       date[outpos] = '\0';
	       strncat(date, buf, datelen - outpos);
		 date[datelen - 1] = '\0';
		 SetLastError(ERROR_INSUFFICIENT_BUFFER);
	       //WARN_(ole)("insufficient buffer\n");
		 return 0;
	    }
	    /* reset the variables we used to keep track of this item */
	    count = 0;
	    type = '\0';
	 } else if (format[inpos] == '\0') {
	    /* we can't check for this at the loop-head, because
	       that breaks the printing of the last format-item */
	    date[outpos] = '\0';
	    break;
         } else if (count) {
	    /* continuing a code for an item */
	    count +=1;
	    continue;
	 } else if (strchr("hHmstyMdg", format[inpos])) {
	    type = format[inpos];
	    count = 1;
	    continue;
	 } else if (format[inpos] == '\'') {
	    inquote = 1;
	    continue;
       } else {
	    date[outpos++] = format[inpos];
	 }
      /* now deal with a possible buffer overflow */
      if (outpos >= datelen) {
       date[datelen - 1] = '\0';
       SetLastError(ERROR_INSUFFICIENT_BUFFER);
       return 0;
      }
   }

   if (Overflow) {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
   };
   /* finish it off with a string terminator */
   outpos++;
   /* sanity check */
   if (outpos > datelen-1) outpos = datelen-1;
   date[outpos] = '\0';

   //TRACE_(ole)("OLE_GetFormatA returns string '%s', len %d\n",
   //	       date, outpos);
   return outpos;
}

/******************************************************************************
 * OLE_GetFormatW [INTERNAL]
 */
static INT OLE_GetFormatW(LCID locale, DWORD flags, DWORD tflags,
			    LPSYSTEMTIME xtime,
			    LPCWSTR format,
			    LPWSTR output, INT outlen)
{
   INT   inpos, outpos;
   int     count, type=0, inquote;
   int     Overflow; /* loop check */
   WCHAR   buf[40];
   int     buflen=0;
   WCHAR   arg0[] = {0}, arg1[] = {'%','d',0};
   WCHAR   arg2[] = {'%','0','2','d',0};
   WCHAR  *argarr[3];
   int     datevars=0, timevars=0;
   argarr[0] = arg0;
   argarr[1] = arg1;
   argarr[2] = arg2;

   dprintf(("KERNEL32: OLE_GetFormatW"));

   if(outlen == 0) {
     //FIXME_(ole)("outlen = 0, returning 255\n");
     return 255;
   }
   /* initialize state variables */
   inpos = outpos = 0;
   count = 0;
   inquote = Overflow = 0;
   /* this is really just a sanity check */
   output[0] = buf[0] = 0;

   /* this loop is the core of the function */
   for (inpos = 0; /* we have several break points */ ; inpos++) {
      if (inquote) {
	 if (format[inpos] == (WCHAR) '\'') {
	    if (format[inpos+1] == '\'') {
	       inpos++;
	       output[outpos++] = '\'';
	    } else {
	       inquote = 0;
	       continue;
	    }
	 } else if (format[inpos] == 0) {
	    output[outpos++] = 0;
	    if (outpos > outlen) Overflow = 1;
	    break;  /*  normal exit (within a quote) */
	 } else {
	    output[outpos++] = format[inpos]; /* copy input */
	    if (outpos > outlen) {
	       Overflow = 1;
	       output[outpos-1] = 0;
	       break;
	    }
	 }
      } else if (  (count && (format[inpos] != type))
		   || ( (count==4 && type =='y') ||
			(count==4 && type =='M') ||
			(count==4 && type =='d') ||
			(count==2 && type =='g') ||
			(count==2 && type =='h') ||
			(count==2 && type =='H') ||
			(count==2 && type =='m') ||
			(count==2 && type =='s') ||
			(count==2 && type =='t') )  ) {
	 if        (type == 'd') {
	    if        (count == 3) {
	       GetLocaleInfoW(locale,
			     LOCALE_SDAYNAME1 + xtime->wDayOfWeek -1,
			     buf, sizeof(buf)/sizeof(WCHAR) );
	    } else if (count == 3) {
	       GetLocaleInfoW(locale,
				LOCALE_SABBREVDAYNAME1 +
				xtime->wDayOfWeek -1,
				buf, sizeof(buf)/sizeof(WCHAR) );
	    } else {
	       wsnprintfW(buf, 5, argarr[count], xtime->wDay );
	    };
	 } else if (type == 'M') {
	    if        (count == 4) {
	       GetLocaleInfoW(locale,  LOCALE_SMONTHNAME1 +
				xtime->wMonth -1, buf,
				sizeof(buf)/sizeof(WCHAR) );
	    } else if (count == 3) {
	       GetLocaleInfoW(locale,  LOCALE_SABBREVMONTHNAME1 +
				xtime->wMonth -1, buf,
				sizeof(buf)/sizeof(WCHAR) );
	    } else {
	       wsnprintfW(buf, 5, argarr[count], xtime->wMonth);
	    }
	 } else if (type == 'y') {
	    if        (count == 4) {
	       wsnprintfW(buf, 6, argarr[1] /* "%d" */,
			 xtime->wYear);
	    } else if (count == 3) {
	       lstrcpynAtoW(buf, "yyy", 5);
	    } else {
	       wsnprintfW(buf, 6, argarr[count],
			    xtime->wYear % 100);
	    }
	 } else if (type == 'g') {
	    if        (count == 2) {
	       //FIXME_(ole)("LOCALE_ICALENDARTYPE unimplemented\n");
	       lstrcpynAtoW(buf, "AD", 5);
	    } else {
	       /* Win API sez we copy it verbatim */
	       lstrcpynAtoW(buf, "g", 5);
	    }
	 } else if (type == 'h') {
	    /* hours 1:00-12:00 --- is this right? */
	    wsnprintfW(buf, 5, argarr[count],
			 (xtime->wHour-1)%12 +1);
	 } else if (type == 'H') {
	    wsnprintfW(buf, 5, argarr[count],
			 xtime->wHour);
	 } else if (type == 'm' ) {
	    wsnprintfW(buf, 5, argarr[count],
			 xtime->wMinute);
	 } else if (type == 's' ) {
	    wsnprintfW(buf, 5, argarr[count],
			 xtime->wSecond);
	 } else if (type == 't') {
	    GetLocaleInfoW(locale, (xtime->wHour < 12) ?
			     LOCALE_S1159 : LOCALE_S2359,
			     buf, sizeof(buf) );
	    if        (count == 1) {
	       buf[1] = 0;
	    }
}
	 /* no matter what happened,  we need to check this next
	    character the next time we loop through */
	 inpos--;
	 /* cat buf onto the output */
	 outlen = lstrlenW(buf);
	 if (outpos + buflen < outlen) {
            lstrcpyW( output + outpos, buf );
	    outpos += buflen;
	 } else {
            lstrcpynW( output + outpos, buf, outlen - outpos );
	    Overflow = 1;
	    break; /* Abnormal exit */
	 }
	 /* reset the variables we used this time */
	 count = 0;
	 type = '\0';
      } else if (format[inpos] == 0) {
	 /* we can't check for this at the beginning,  because that
	 would keep us from printing a format spec that ended the
	 string */
	 output[outpos] = 0;
	 break;  /*  NORMAL EXIT  */
      } else if (count) {
	 /* how we keep track of the middle of a format spec */
	 count++;
	 continue;
      } else if ( (datevars && (format[inpos]=='d' ||
				format[inpos]=='M' ||
				format[inpos]=='y' ||
				format[inpos]=='g')  ) ||
		  (timevars && (format[inpos]=='H' ||
				format[inpos]=='h' ||
				format[inpos]=='m' ||
				format[inpos]=='s' ||
				format[inpos]=='t') )    ) {
	 type = format[inpos];
	 count = 1;
	 continue;
      } else if (format[inpos] == '\'') {
	 inquote = 1;
	 continue;
      } else {
	 /* unquoted literals */
	 output[outpos++] = format[inpos];
      }
   }
   if (Overflow) {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      //WARN_(ole)(" buffer overflow\n");
   };
   /* final string terminator and sanity check */
   outpos++;
   if (outpos > outlen-1) outpos = outlen-1;
   output[outpos] = '0';
   //TRACE_(ole)(" returning %s\n", debugstr_w(output));
	
   return (!Overflow) ? outlen : 0;

}

/******************************************************************************
 *		GetTimeFormat32A	[KERNEL32.422]
 * Makes an ASCII string of the time
 *
 * Formats date according to format,  or locale default if format is
 * NULL. The format consists of literal characters and fields as follows:
 *
 * h  hours with no leading zero (12-hour)
 * hh hours with full two digits
 * H  hours with no leading zero (24-hour)
 * HH hours with full two digits
 * m  minutes with no leading zero
 * mm minutes with full two digits
 * s  seconds with no leading zero
 * ss seconds with full two digits
 * t  time marker (A or P)
 * tt time marker (AM, PM)
 *
 */
INT WINAPI
GetTimeFormatA(LCID locale,        /* in  */
		 DWORD flags,        /* in  */
		 LPSYSTEMTIME xtime, /* in  */
		 LPCSTR format,      /* in  */
		 LPSTR timestr,      /* out */
		 INT timelen       /* in  */)
{ char format_buf[40];
  LPCSTR thisformat;
  SYSTEMTIME t;
  LPSYSTEMTIME thistime;
  LCID thislocale=0;
  DWORD thisflags=LOCALE_STIMEFORMAT; /* standart timeformat */
  INT ret;

  dprintf(("KERNEL32: GetTimeFormatA"));
  //TRACE_(ole)("GetTimeFormat(0x%04lx,0x%08lx,%p,%s,%p,%d)\n",locale,flags,xtime,format,timestr,timelen);
  thislocale = OLE2NLS_CheckLocale ( locale );
  if ( flags & (TIME_NOTIMEMARKER | TIME_FORCE24HOURFORMAT ))
  { //FIXME_(ole)("TIME_NOTIMEMARKER or TIME_FORCE24HOURFORMAT not implemented\n");
  }

  flags &= (TIME_NOSECONDS | TIME_NOMINUTESORSECONDS); /* mask for OLE_GetFormatA*/
  if (format == NULL)
  { if (flags & LOCALE_NOUSEROVERRIDE)  /*use system default*/
    { thislocale = GetSystemDefaultLCID();
    }
    GetLocaleInfoA(thislocale, thisflags, format_buf, sizeof(format_buf));
    thisformat = format_buf;
  }
  else
  { thisformat = format;
  }

  if (xtime == NULL) /* NULL means use the current local time*/
  { GetLocalTime(&t);
    thistime = &t;
  }
  else
  { thistime = xtime;
  }
  ret = OLE_GetFormatA(thislocale, thisflags, flags, thistime, thisformat,
  			 timestr, timelen);
  return ret;
}

/******************************************************************************
 *		GetTimeFormat32W	[KERNEL32.423]
 * Makes a Unicode string of the time
 */
INT WINAPI
GetTimeFormatW(LCID locale,        /* in  */
		 DWORD flags,        /* in  */
		 LPSYSTEMTIME xtime, /* in  */
		 LPCWSTR format,     /* in  */
		 LPWSTR timestr,     /* out */
		 INT timelen       /* in  */)
{	WCHAR format_buf[40];
	LPCWSTR thisformat;
	SYSTEMTIME t;
	LPSYSTEMTIME thistime;
	LCID thislocale=0;
	DWORD thisflags=LOCALE_STIMEFORMAT; /* standart timeformat */
	INT ret;
	
	dprintf(("KERNEL32: GetTimeFormatW"));
	//TRACE_(ole)("GetTimeFormat(0x%04lx,0x%08lx,%p,%s,%p,%d)\n",locale,flags,
	//xtime,debugstr_w(format),timestr,timelen);
	thislocale = OLE2NLS_CheckLocale ( locale );
	if ( flags & (TIME_NOTIMEMARKER | TIME_FORCE24HOURFORMAT ))
	{ //FIXME_(ole)("TIME_NOTIMEMARKER or TIME_FORCE24HOURFORMAT not implemented\n");
	}

	flags &= (TIME_NOSECONDS | TIME_NOMINUTESORSECONDS); /* mask for OLE_GetFormatA*/
	if (format == NULL)
	{ if (flags & LOCALE_NOUSEROVERRIDE)  /*use system default*/
	  { thislocale = GetSystemDefaultLCID();
	  }
	  GetLocaleInfoW(thislocale, thisflags, format_buf, 40);
	  thisformat = format_buf;
	}	
	else
	{ thisformat = format;
	}

	if (xtime == NULL) /* NULL means use the current local time*/
	{ GetSystemTime(&t);
	  thistime = &t;
	}
	else
	{ thistime = xtime;
	}
	ret = OLE_GetFormatW(thislocale, thisflags, flags, thistime, thisformat,
  			 timestr, timelen);
	return ret;
}

/******************************************************************************
 *		GetDateFormat32A	[KERNEL32.310]
 * Makes an ASCII string of the date
 *
 * This function uses format to format the date,  or,  if format
 * is NULL, uses the default for the locale.  format is a string
 * of literal fields and characters as follows:
 *
 * - d    single-digit (no leading zero) day (of month)
 * - dd   two-digit day (of month)
 * - ddd  short day-of-week name
 * - dddd long day-of-week name
 * - M    single-digit month
 * - MM   two-digit month
 * - MMM  short month name
 * - MMMM full month name
 * - y    two-digit year, no leading 0
 * - yy   two-digit year
 * - yyyy four-digit year
 * - gg   era string
 *
 */
INT WINAPI GetDateFormatA(LCID locale,DWORD flags,
			      LPSYSTEMTIME xtime,
			      LPCSTR format, LPSTR date,INT datelen)
{
  char format_buf[40];
  LPCSTR thisformat;
  SYSTEMTIME t;
  LPSYSTEMTIME thistime;
  LCID thislocale;
  INT ret;

  dprintf(("KERNEL32: GetDateFormatA\n"));

  if (!locale) {
     locale = LOCALE_SYSTEM_DEFAULT;
     };

  if (locale == LOCALE_SYSTEM_DEFAULT) {
     thislocale = GetSystemDefaultLCID();
  } else if (locale == LOCALE_USER_DEFAULT) {
     thislocale = GetUserDefaultLCID();
  } else {
     thislocale = locale;
   };
  if (xtime == NULL) {
     GetSystemTime(&t);
     thistime = &t;
  } else {
     thistime = xtime;
  };
  if (format == NULL) {
     GetLocaleInfoA(thislocale, ((flags&DATE_LONGDATE)
				   ? LOCALE_SLONGDATE
				   : LOCALE_SSHORTDATE),
		      format_buf, sizeof(format_buf));
     thisformat = format_buf;
  } else {
     thisformat = format;
  };

  ret = OLE_GetFormatA(thislocale, flags, 0, thistime, thisformat,
		       date, datelen);

  return ret;
}
/******************************************************************************
 *		GetDateFormat32W	[KERNEL32.311]
 * Makes a Unicode string of the date
 *
 * Acts the same as GetDateFormat32A(),  except that it's Unicode.
 * Accepts & returns sizes as counts of Unicode characters.
 *
 */
INT WINAPI GetDateFormatW(LCID locale,DWORD flags,
			      LPSYSTEMTIME xtime,
			      LPCWSTR format,
			      LPWSTR date, INT datelen)
{
  WCHAR format_buf[40];
  LPWSTR thisformat;
  SYSTEMTIME t;
  LPSYSTEMTIME thistime;
  LCID thislocale;
  INT ret;

  dprintf(("KERNEL32: GetDateFormatW\n"));

  if (!locale) {
     locale = LOCALE_SYSTEM_DEFAULT;
     };

  if (locale == LOCALE_SYSTEM_DEFAULT) {
     thislocale = GetSystemDefaultLCID();
  } else if (locale == LOCALE_USER_DEFAULT) {
     thislocale = GetUserDefaultLCID();
  } else {
     thislocale = locale;
   };
  if (xtime == NULL) {
     GetSystemTime(&t);
     thistime = &t;
  } else {
     thistime = xtime;
  };
  if (format == NULL) {
     GetLocaleInfoW(thislocale, ((flags&DATE_LONGDATE)
				   ? LOCALE_SLONGDATE
				   : LOCALE_SSHORTDATE),
		      format_buf, sizeof(format_buf));
     thisformat = format_buf;
  } else {
     thisformat = (WCHAR*)format;
  };

  ret = OLE_GetFormatW(thislocale, flags, 0, thistime, thisformat,
		       date, datelen);

  return ret;
}
/**************************************************************************
 *              EnumTimeFormats32A	(KERNEL32.210)
 */
BOOL WINAPI EnumTimeFormatsA(
  TIMEFMT_ENUMPROCA lpTimeFmtEnumProc, LCID Locale, DWORD dwFlags)
{
  dprintf(("KERNEL32: EnumTimeFormatsA: only US English supported\n"));

  if(!lpTimeFmtEnumProc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }
  if(dwFlags)
    {
      //FIXME_(ole)("Unknown time format (%ld)\n", dwFlags);
    }

  if(!(*lpTimeFmtEnumProc)("h:mm:ss tt")) return TRUE;
  if(!(*lpTimeFmtEnumProc)("hh:mm:ss tt")) return TRUE;
  if(!(*lpTimeFmtEnumProc)("H:mm:ss")) return TRUE;
  if(!(*lpTimeFmtEnumProc)("HH:mm:ss")) return TRUE;

  return TRUE;
}
/**************************************************************************
 *              EnumTimeFormats32W	(KERNEL32.211)
 */
BOOL WINAPI EnumTimeFormatsW(
  TIMEFMT_ENUMPROCW lpTimeFmtEnumProc, LCID Locale, DWORD dwFlags)
{
  WCHAR buf[20];

  dprintf(("KERNEL32: EnumTimeFormatsW: only US English supported\n"));

  if(!lpTimeFmtEnumProc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }
  if(dwFlags)
    {
      //FIXME_(ole)("Unknown time format (%ld)\n", dwFlags);
    }

  AsciiToUnicode("h:mm:ss tt",buf);
  if(!(*lpTimeFmtEnumProc)(buf)) return TRUE;
  AsciiToUnicode("hh:mm:ss tt",buf);
  if(!(*lpTimeFmtEnumProc)(buf)) return TRUE;
  AsciiToUnicode("H:mm:ss",buf);
  if(!(*lpTimeFmtEnumProc)(buf)) return TRUE;
  AsciiToUnicode("HH:mm:ss",buf);
  if(!(*lpTimeFmtEnumProc)(buf)) return TRUE;

  return TRUE;
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
/**************************************************************************
 *              EnumDateFormats32A	(KERNEL32.198)
 */
BOOL WINAPI EnumDateFormatsA(
  DATEFMT_ENUMPROCA lpDateFmtEnumProc, LCID Locale,  DWORD dwFlags)
{
  dprintf(("KERNEL32: EnumDateFormatsA: only US English supported\n"));

  if(!lpDateFmtEnumProc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }
  switch(dwFlags)
    {
      case DATE_SHORTDATE:
	if(!(*lpDateFmtEnumProc)("M/d/yy")) return TRUE;
	if(!(*lpDateFmtEnumProc)("M/d/yyyy")) return TRUE;
	if(!(*lpDateFmtEnumProc)("MM/dd/yy")) return TRUE;
	if(!(*lpDateFmtEnumProc)("MM/dd/yyyy")) return TRUE;
	if(!(*lpDateFmtEnumProc)("yy/MM/dd")) return TRUE;
	if(!(*lpDateFmtEnumProc)("dd-MMM-yy")) return TRUE;
	return TRUE;
      case DATE_LONGDATE:
        if(!(*lpDateFmtEnumProc)("dddd, MMMM dd, yyyy")) return TRUE;
        if(!(*lpDateFmtEnumProc)("MMMM dd, yyyy")) return TRUE;
        if(!(*lpDateFmtEnumProc)("dddd, dd MMMM, yyyy")) return TRUE;
        if(!(*lpDateFmtEnumProc)("dd MMMM, yyyy")) return TRUE;
	return TRUE;
      default:
	//FIXME_(ole)("Unknown date format (%ld)\n", dwFlags);
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
    }
}
/**************************************************************************
 *              EnumDateFormats32W	(KERNEL32.199)
 */
BOOL WINAPI EnumDateFormatsW(
  DATEFMT_ENUMPROCW lpDateFmtEnumProc, LCID Locale, DWORD dwFlags)
{
  WCHAR buf[50];

  dprintf(("KERNEL32: EnumDateFormatsW: only US English supported\n"));

  if(!lpDateFmtEnumProc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }
  switch(dwFlags)
    {
      case DATE_SHORTDATE:
        AsciiToUnicode("M/d/yy",buf);
	if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
	AsciiToUnicode("M/d/yyyy",buf);
	if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
	AsciiToUnicode("MM/dd/yy",buf);
	if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
	AsciiToUnicode("MM/dd/yyyy",buf);
	if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
	AsciiToUnicode("yy/MM/dd",buf);
	if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
	AsciiToUnicode("dd-MMM-yy",buf);
	if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
	return TRUE;
      case DATE_LONGDATE:
        AsciiToUnicode("dddd, MMMM dd, yyyy",buf);
        if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
        AsciiToUnicode("MMMM dd, yyyy",buf);
        if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
        AsciiToUnicode("dddd, dd MMMM, yyyy",buf);
        if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
        AsciiToUnicode("dd MMMM, yyyy",buf);
        if(!(*lpDateFmtEnumProc)(buf)) return TRUE;
	return TRUE;
      default:
	//FIXME_(ole)("Unknown date format (%ld)\n", dwFlags);
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
    }
}

