/* $Id: char.cpp,v 1.15 2001-04-03 20:55:46 sandervl Exp $ */

/*
 * USER string functions
 *
 * Based on WINE code (dlls\user\lstr.c)
 *
 * Copyright 1993 Yngvi Sigurjonsson (yngvi@hafro.is)
 * Copyright 1996 Alexandre Julliard
 * Copyright 1996 Marcus Meissner
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <os2win.h>

#include "winnls.h"

#include <misc.h>
#include <wine\unicode.h>


#define DBG_LOCALLOG	DBG_char
#include "dbglocal.h"



/***********************************************************************
 *           CharNextA   (USER32.@)
 */
LPSTR WINAPI CharNextA( LPCSTR ptr )
{
    dprintf2(("CharNextA %x", ptr));
    if (!*ptr) return (LPSTR)ptr;
    if (IsDBCSLeadByte( ptr[0] ) && ptr[1]) return (LPSTR)(ptr + 2);
    return (LPSTR)(ptr + 1);
}


/***********************************************************************
 *           CharNextExA   (USER32.@)
 */
LPSTR WINAPI CharNextExA( WORD codepage, LPCSTR ptr, DWORD flags )
{
    dprintf2(("CharNextExA %d %x %x", codepage, ptr, flags));

    if (!*ptr) return (LPSTR)ptr;
    if (IsDBCSLeadByteEx( codepage, ptr[0] ) && ptr[1]) return (LPSTR)(ptr + 2);
    return (LPSTR)(ptr + 1);
}


/***********************************************************************
 *           CharNextExW   (USER32.@)
 */
LPWSTR WINAPI CharNextExW( WORD codepage, LPCWSTR ptr, DWORD flags )
{
    dprintf2(("CharNextExW %d %x %x", codepage, ptr, flags));
    /* doesn't make sense, there are no codepages for Unicode */
    return NULL;
}


/***********************************************************************
 *           CharNextW   (USER32.@)
 */
LPWSTR WINAPI CharNextW(LPCWSTR x)
{
    dprintf2(("CharNextW %x", x));

    if (*x) x++;

    return (LPWSTR)x;
}


/***********************************************************************
 *           CharPrevA   (USER32.@)
 */
LPSTR WINAPI CharPrevA( LPCSTR start, LPCSTR ptr )
{
    dprintf2(("CharPrevA %x %x", start, ptr));

    while (*start && (start < ptr))
    {
        LPCSTR next = CharNextA( start );
        if (next >= ptr) break;
        start = next;
    }
    return (LPSTR)start;
}


/***********************************************************************
 *           CharPrevExA   (USER32.@)
 */
LPSTR WINAPI CharPrevExA( WORD codepage, LPCSTR start, LPCSTR ptr, DWORD flags )
{
    dprintf2(("CharPrevExA %d %x %x %x", codepage, start, ptr, flags));

    while (*start && (start < ptr))
    {
        LPCSTR next = CharNextExA( codepage, start, flags );
        if (next > ptr) break;
        start = next;
    }
    return (LPSTR)start;
}


/***********************************************************************
 *           CharPrevExW   (USER32.@)
 */
LPWSTR WINAPI CharPrevExW( WORD codepage, LPCWSTR start, LPCWSTR ptr, DWORD flags )
{
    /* doesn't make sense, there are no codepages for Unicode */
    dprintf2(("CharPrevExW %d %x %x %x", codepage, start, ptr, flags));
    return NULL;
}


/***********************************************************************
 *           CharPrevW   (USER32.@)
 */
LPWSTR WINAPI CharPrevW(LPCWSTR start,LPCWSTR x)
{
    dprintf2(("CharPrevW %x %x", start, x));

    if (x>start) return (LPWSTR)(x-1);
    else return (LPWSTR)x;
}


/***********************************************************************
 *           CharToOemA   (USER32.@)
 */
BOOL WINAPI CharToOemA( LPCSTR s, LPSTR d )
{
    dprintf2(("CharToOemA %x %x", s, d));
    if ( !s || !d ) return TRUE;
    return CharToOemBuffA( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           CharToOemBuffA   (USER32.@)
 */
BOOL WINAPI CharToOemBuffA( LPCSTR s, LPSTR d, DWORD len )
{
    WCHAR *bufW;

    dprintf2(("CharToOemBuffA %x %x %d", s, d, len));
    bufW = (WCHAR *)HeapAlloc( GetProcessHeap(), 0, len * sizeof(WCHAR) );
    if( bufW )
    {
	MultiByteToWideChar( CP_ACP, 0, s, len, bufW, len );
	WideCharToMultiByte( CP_OEMCP, 0, bufW, len, d, len, NULL, NULL );
	HeapFree( GetProcessHeap(), 0, bufW );
    }
    return TRUE;
}


/***********************************************************************
 *           CharToOemBuffW   (USER32.@)
 */
BOOL WINAPI CharToOemBuffW( LPCWSTR s, LPSTR d, DWORD len )
{
   dprintf2(("CharToOemBuffW %x %x %d", s, d, len));

   if ( !s || !d ) return TRUE;
    WideCharToMultiByte( CP_OEMCP, 0, s, len, d, len, NULL, NULL );
    return TRUE;
}


/***********************************************************************
 *           CharToOemW   (USER32.@)
 */
BOOL WINAPI CharToOemW( LPCWSTR s, LPSTR d )
{
    return CharToOemBuffW( s, d, strlenW( s ) + 1 );
}


/***********************************************************************
 *           OemToCharA   (USER32.@)
 */
BOOL WINAPI OemToCharA( LPCSTR s, LPSTR d )
{
    return OemToCharBuffA( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           OemToCharBuffA   (USER32.@)
 */
BOOL WINAPI OemToCharBuffA( LPCSTR s, LPSTR d, DWORD len )
{
    WCHAR *bufW;

    dprintf2(("OemToCharBuffA %x %x %d", s, d, len));

    bufW = (WCHAR *)HeapAlloc( GetProcessHeap(), 0, len * sizeof(WCHAR) );
    if( bufW )
    {
	MultiByteToWideChar( CP_OEMCP, 0, s, len, bufW, len );
	WideCharToMultiByte( CP_ACP, 0, bufW, len, d, len, NULL, NULL );
	HeapFree( GetProcessHeap(), 0, bufW );
    }
    return TRUE;
}


/***********************************************************************
 *           OemToCharBuffW   (USER32.@)
 */
BOOL WINAPI OemToCharBuffW( LPCSTR s, LPWSTR d, DWORD len )
{
    dprintf2(("OemToCharBuffW %x %x %d", s, d, len));

    MultiByteToWideChar( CP_OEMCP, 0, s, len, d, len );
    return TRUE;
}


/***********************************************************************
 *           OemToCharW   (USER32.@)
 */
BOOL WINAPI OemToCharW( LPCSTR s, LPWSTR d )
{
    return OemToCharBuffW( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           CharLowerA   (USER32.@)
 * FIXME: handle current locale
 */
LPSTR WINAPI CharLowerA(LPSTR x)
{
    LPSTR	s;

    dprintf2(("CharLowerA %x", x));

    if (HIWORD(x))
    {
        s=x;
        while (*s)
        {
            *s=tolower(*s);
            s++;
        }
        return x;
    }
    else return (LPSTR)tolower((char)(int)x);
}


/***********************************************************************
 *           CharUpperA   (USER32.@)
 * FIXME: handle current locale
 */
LPSTR WINAPI CharUpperA(LPSTR x)
{
    dprintf2(("CharUpperA %x", x));

    if (HIWORD(x))
    {
        LPSTR s = x;
        while (*s)
        {
            *s=toupper(*s);
            s++;
        }
        return x;
    }
    return (LPSTR)toupper((char)(int)x);
}


/***********************************************************************
 *           CharLowerW   (USER32.@)
 */
LPWSTR WINAPI CharLowerW(LPWSTR x)
{
    dprintf2(("CharLowerW %x", x));
    if (HIWORD(x)) return strlwrW(x);
    else return (LPWSTR)((UINT)tolowerW(LOWORD(x)));
}


/***********************************************************************
 *           CharUpperW   (USER32.@)
 * FIXME: handle current locale
 */
LPWSTR WINAPI CharUpperW(LPWSTR x)
{
    dprintf2(("CharUpperW %x", x));
    if (HIWORD(x)) return struprW(x);
    else return (LPWSTR)((UINT)toupperW(LOWORD(x)));
}


/***********************************************************************
 *           CharLowerBuffA   (USER32.@)
 * FIXME: handle current locale
 */
DWORD WINAPI CharLowerBuffA( LPSTR str, DWORD len )
{
    DWORD ret = len;

    dprintf2(("CharLowerBuffA %x %d", str, len));

    if (!str) return 0; /* YES */
    for (; len; len--, str++) *str = tolower(*str);
    return ret;
}


/***********************************************************************
 *           CharLowerBuffW   (USER32.@)
 */
DWORD WINAPI CharLowerBuffW( LPWSTR str, DWORD len )
{
    DWORD ret = len;

    dprintf2(("CharLowerBuffW %x %d", str, len));

    if (!str) return 0; /* YES */
    for (; len; len--, str++) *str = tolowerW(*str);
    return ret;
}


/***********************************************************************
 *           CharUpperBuffA   (USER32.@)
 * FIXME: handle current locale
 */
DWORD WINAPI CharUpperBuffA( LPSTR str, DWORD len )
{
    DWORD ret = len;

    dprintf2(("CharUpperBuffA %x %d", str, len));

    if (!str) return 0; /* YES */
    for (; len; len--, str++) *str = toupper(*str);
    return ret;
}


/***********************************************************************
 *           CharUpperBuffW   (USER32.@)
 */
DWORD WINAPI CharUpperBuffW( LPWSTR str, DWORD len )
{
    DWORD ret = len;

    dprintf2(("CharUpperBuffW %x %d", str, len));

    if (!str) return 0; /* YES */
    for (; len; len--, str++) *str = toupperW(*str);
    return ret;
}


/***********************************************************************
 *           IsCharLowerA   (USER.436) (USER32.@)
 * FIXME: handle current locale
 */
BOOL WINAPI IsCharLowerA(CHAR x)
{
    dprintf2(("IsCharLowerA %x", x));
    return islower(x);
}


/***********************************************************************
 *           IsCharLowerW   (USER32.@)
 */
BOOL WINAPI IsCharLowerW(WCHAR x)
{
    dprintf2(("IsCharLowerW %x", x));
    return get_char_typeW(x) & C1_LOWER;
}


/***********************************************************************
 *           IsCharUpperA   (USER.435) (USER32.@)
 * FIXME: handle current locale
 */
BOOL WINAPI IsCharUpperA(CHAR x)
{
    dprintf2(("IsCharUpperA %x", x));
    return isupper(x);
}


/***********************************************************************
 *           IsCharUpperW   (USER32.@)
 */
BOOL WINAPI IsCharUpperW(WCHAR x)
{
    dprintf2(("IsCharUpperW %x", x));
    return get_char_typeW(x) & C1_UPPER;
}


/***********************************************************************
 *           IsCharAlphaNumericW   (USER32.@)
 */
BOOL WINAPI IsCharAlphaNumericW(WCHAR x)
{
    dprintf2(("IsCharAlphaNumericW %x", x));
    return get_char_typeW(x) & (C1_ALPHA|C1_DIGIT|C1_LOWER|C1_UPPER);
}


/***********************************************************************
 *           IsCharAlphaW   (USER32.@)
 */
BOOL WINAPI IsCharAlphaW(WCHAR x)
{
    dprintf2(("IsCharAlphaW %x", x));
    return get_char_typeW(x) & (C1_ALPHA|C1_LOWER|C1_UPPER);
}

//******************************************************************************
//TODO:
//******************************************************************************
BOOL WIN32API IsCharAlphaA( CHAR arg1)
{
    dprintf(("USER32: IsCharAlphaA %x", arg1));
    return O32_IsCharAlpha(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharAlphaNumericA( CHAR arg1)
{
    dprintf(("USER32: IsCharAlphaNumericA %x", arg1));
    return O32_IsCharAlphaNumeric(arg1);
}
