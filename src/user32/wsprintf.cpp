/* $Id: wsprintf.cpp,v 1.6 2000-02-16 14:34:41 sandervl Exp $ */

/*
 * Win32 misc user32 API functions for OS/2
 * wsprintf functions
 *
 * Copyright 1996 Alexandre Julliard
 * Copyright 1999 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <os2win.h>

#include <stdarg.h>
#include <string.h>
#include "stackframe.h"
#include "module.h"
#include "global.h"
#include "debugtools.h"

#include <misc.h>

#define DBG_LOCALLOG	DBG_wsprintf
#include "dbglocal.h"

ODINDEBUGCHANNEL(USER32-WSPRINTF)


/****************************************************************************
 * Definitions & Structures                                                 *
 ****************************************************************************/

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


/****************************************************************************
 * Module global variables                                                  *
 ****************************************************************************/

static const CHAR null_stringA[] = "(null)";
static const WCHAR null_stringW[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };


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

    while (spec && *spec && (maxlen > 1))
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
    TRACE("%s\n",buffer);
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

    while (spec && *spec && (maxlen > 1))
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
 *           wvsprintfA   (USER32.587)
 */
ODINFUNCTION3(INT,    wvsprintfA,
              LPSTR,  buffer,
              LPCSTR, spec,
              va_list,args )
{
  return wvsnprintfA( buffer, 0xffffffff, spec, args );
}


/***********************************************************************
 *           wvsprintfW   (USER32.588)
 */
ODINFUNCTION3(INT,     wvsprintfW,
              LPWSTR,  buffer,
              LPCWSTR, spec,
              va_list, args )
{
  return wvsnprintfW( buffer, 0xffffffff, spec, args );
}


/***********************************************************************
 *           wsprintfA   (USER32.585)
 */
INT WINAPIV wsprintfA( LPSTR buffer, LPCSTR spec, ... )
{
    va_list valist;
    INT res;

    TRACE("for %p got:\n",buffer);
    va_start( valist, spec );
    res = wvsnprintfA( buffer, 0xffffffff, spec, valist );
    va_end( valist );
    return res;
}


/***********************************************************************
 *           wsprintfW   (USER32.586)
 */
INT WINAPIV wsprintfW( LPWSTR buffer, LPCWSTR spec, ... )
{
    va_list valist;
    INT res;

    TRACE("wsprintfW for %p\n",buffer);
    va_start( valist, spec );
    res = wvsnprintfW( buffer, 0xffffffff, spec, valist );
    va_end( valist );
    return res;
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




#if 0
//@@@PH old ODIN implementation 1999/11/04
/* String Manipulation Functions */

int __cdecl wsprintfA(char *lpOut, LPCSTR lpFmt, ...)
{
 int     rc;
 va_list argptr;

#ifdef DEBUG
  WriteLog("USER32:  wsprintfA\n");
  WriteLog("USER32:  %s\n", lpFmt);
#endif
  va_start(argptr, lpFmt);
  rc = O32_wvsprintf(lpOut, (char *)lpFmt, argptr);
  va_end(argptr);
#ifdef DEBUG
  WriteLog("USER32:  %s\n", lpOut);
#endif
  return(rc);
}
//******************************************************************************
//******************************************************************************
int __cdecl wsprintfW(LPWSTR lpOut, LPCWSTR lpFmt, ...)
{
  int     rc;
  char   *lpFmtA;
  char    szOut[512];
  va_list argptr;

  dprintf(("USER32: wsprintfW(%08xh,%08xh).\n",
           lpOut,
           lpFmt));

  lpFmtA  = UnicodeToAsciiString((LPWSTR)lpFmt);

  /* @@@PH 98/07/13 transform "%s" to "%ls" does the unicode magic */
  {
    PCHAR   pszTemp;
    PCHAR   pszTemp1;
    ULONG   ulStrings;
    ULONG   ulIndex;                             /* temporary string counter */

    for (ulStrings = 0,                  /* determine number of placeholders */
         pszTemp   = lpFmtA;

         (pszTemp != NULL) &&
         (*pszTemp != 0);

         ulStrings++)
    {
      pszTemp = strstr(pszTemp,
                       "%s");
      if (pszTemp != NULL)                              /* skip 2 characters */
      {
        pszTemp++;
        pszTemp++;
      }
      else
        break;                                     /* leave loop immediately */
    }

    if (ulStrings != 0)                         /* transformation required ? */
    {
                                                     /* now reallocate lpFmt */
      ulStrings += strlen(lpFmtA);           /* calculate total string length */
      pszTemp   = lpFmtA;                              /* save string pointer */
      pszTemp1  = lpFmtA;                              /* save string pointer */

             /* @@@PH allocation has to be compatible to FreeAsciiString !!! */
      lpFmtA     = (char *)malloc(ulStrings + 1);
      if (lpFmtA == NULL)                          /* check proper allocation */
        return (0);                                 /* raise error condition */

      for (ulIndex = 0;
           ulIndex <= ulStrings;
           ulIndex++,
           pszTemp++)
      {
        if ((pszTemp[0] == '%') &&
            (pszTemp[1] == 's') )
        {
          /* replace %s by %ls */
          lpFmtA[ulIndex++] = '%';
          lpFmtA[ulIndex  ] = 'l';
          lpFmtA[ulIndex+1] = 's';
        }
        else
          lpFmtA[ulIndex] = *pszTemp;         /* just copy over the character */
      }

      lpFmtA[ulStrings] = 0;                            /* string termination */

      FreeAsciiString(pszTemp1);          /* the original string is obsolete */
    }
  }

  dprintf(("USER32:  wsprintfW (%s).\n",
           lpFmt));

  va_start(argptr,
           lpFmt);

  rc = O32_wvsprintf(szOut,
                 lpFmtA,
                 argptr);

  AsciiToUnicode(szOut,
                 lpOut);

  FreeAsciiString(lpFmtA);
  return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API wvsprintfA( LPSTR lpOutput, LPCSTR lpFormat, va_list arglist)
{
#ifdef DEBUG
    WriteLog("USER32:  wvsprintfA\n");
#endif
    return O32_wvsprintf(lpOutput,lpFormat,(LPCVOID*)arglist);
}
//******************************************************************************
//******************************************************************************
int WIN32API wvsprintfW(LPWSTR lpOutput, LPCWSTR lpFormat, va_list arglist)
{
 int     rc;
 char    szOut[256];
 char   *lpFmtA;

  lpFmtA  = UnicodeToAsciiString((LPWSTR)lpFormat);
#ifdef DEBUG
  WriteLog("USER32:  wvsprintfW, DOES NOT HANDLE UNICODE STRINGS!\n");
  WriteLog("USER32:  %s\n", lpFormat);
#endif
  rc = O32_wvsprintf(szOut, lpFmtA, (LPCVOID)arglist);

  AsciiToUnicode(szOut, lpOutput);
#ifdef DEBUG
  WriteLog("USER32:  %s\n", lpOutput);
#endif
  FreeAsciiString(lpFmtA);
  return(rc);
}
#endif

