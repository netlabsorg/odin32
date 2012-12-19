/*
 * NTDLL wide-char functions
 *
 * Copyright 2000 Alexandre Julliard
 * Copyright 2000 Jon Griffiths
 * Copyright 2003 Thomas Mertes
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "winternl.h"
#include "wine/unicode.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ntdll);


/*********************************************************************
 *           _wcsicmp    (NTDLL.@)
 */
INT __cdecl NTDLL__wcsicmp( LPCWSTR str1, LPCWSTR str2 )
{
    return strcmpiW( str1, str2 );
}


/*********************************************************************
 *           _wcslwr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL__wcslwr( LPWSTR str )
{
    return strlwrW( str );
}


/*********************************************************************
 *           _wcsnicmp    (NTDLL.@)
 */
INT __cdecl NTDLL__wcsnicmp( LPCWSTR str1, LPCWSTR str2, INT n )
{
    return strncmpiW( str1, str2, n );
}


/*********************************************************************
 *           _wcsupr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL__wcsupr( LPWSTR str )
{
    return struprW( str );
}


/*********************************************************************
 *           towlower    (NTDLL.@)
 */
WCHAR __cdecl NTDLL_towlower( WCHAR ch )
{
    return tolowerW(ch);
}


/*********************************************************************
 *           towupper    (NTDLL.@)
 */
WCHAR __cdecl NTDLL_towupper( WCHAR ch )
{
    return toupperW(ch);
}


/***********************************************************************
 *           wcscat    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcscat( LPWSTR dst, LPCWSTR src )
{
    return strcatW( dst, src );
}


/*********************************************************************
 *           wcschr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcschr( LPCWSTR str, WCHAR ch )
{
    return strchrW( str, ch );
}


/*********************************************************************
 *           wcscmp    (NTDLL.@)
 */
INT __cdecl NTDLL_wcscmp( LPCWSTR str1, LPCWSTR str2 )
{
    return strcmpW( str1, str2 );
}


/***********************************************************************
 *           wcscpy    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcscpy( LPWSTR dst, LPCWSTR src )
{
    return strcpyW( dst, src );
}


/*********************************************************************
 *           wcscspn    (NTDLL.@)
 */
INT __cdecl NTDLL_wcscspn( LPCWSTR str, LPCWSTR reject )
{
    LPCWSTR start = str;
    while (*str)
    {
        LPCWSTR p = reject;
        while (*p && (*p != *str)) p++;
        if (*p) break;
        str++;
    }
    return str - start;
}


/***********************************************************************
 *           wcslen    (NTDLL.@)
 */
INT __cdecl NTDLL_wcslen( LPCWSTR str )
{
    return strlenW( str );
}


/*********************************************************************
 *           wcsncat    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsncat( LPWSTR s1, LPCWSTR s2, INT n )
{
    LPWSTR ret = s1;
    while (*s1) s1++;
    while (n-- > 0) if (!(*s1++ = *s2++)) return ret;
    *s1 = 0;
    return ret;
}


/*********************************************************************
 *           wcsncmp    (NTDLL.@)
 */
INT __cdecl NTDLL_wcsncmp( LPCWSTR str1, LPCWSTR str2, INT n )
{
    return strncmpW( str1, str2, n );
}


/*********************************************************************
 *           wcsncpy    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsncpy( LPWSTR s1, LPCWSTR s2, INT n )
{
    return strncpyW( s1, s2, n );
}


/*********************************************************************
 *           wcspbrk    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcspbrk( LPCWSTR str, LPCWSTR accept )
{
    LPCWSTR p;
    while (*str)
    {
        for (p = accept; *p; p++) if (*p == *str) return (LPWSTR)str;
        str++;
    }
    return NULL;
}


/*********************************************************************
 *           wcsrchr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsrchr( LPWSTR str, WCHAR ch )
{
    LPWSTR last = NULL;
    while (*str)
    {
        if (*str == ch) last = str;
        str++;
    }
    return last;
}


/*********************************************************************
 *           wcsspn    (NTDLL.@)
 */
INT __cdecl NTDLL_wcsspn( LPCWSTR str, LPCWSTR accept )
{
    LPCWSTR start = str;
    while (*str)
    {
        LPCWSTR p = accept;
        while (*p && (*p != *str)) p++;
        if (!*p) break;
        str++;
    }
    return str - start;
}


/*********************************************************************
 *           wcsstr    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcsstr( LPCWSTR str, LPCWSTR sub )
{
    return strstrW( str, sub );
}


/*********************************************************************
 *           wcstok    (NTDLL.@)
 */
LPWSTR __cdecl NTDLL_wcstok( LPWSTR str, LPCWSTR delim )
{
    static LPWSTR next = NULL;
    LPWSTR ret;

    if (!str)
        if (!(str = next)) return NULL;

    while (*str && NTDLL_wcschr( delim, *str )) str++;
    if (!*str) return NULL;
    ret = str++;
    while (*str && !NTDLL_wcschr( delim, *str )) str++;
    if (*str) *str++ = 0;
    next = str;
    return ret;
}


/*********************************************************************
 *           wcstombs    (NTDLL.@)
 */
INT __cdecl NTDLL_wcstombs( LPSTR dst, LPCWSTR src, INT n )
{
    DWORD len;

    if (!dst)
    {
        RtlUnicodeToMultiByteSize( &len, src, strlenW(src)*sizeof(WCHAR) );
        return len;
    }
    else
    {
        if (n <= 0) return 0;
        RtlUnicodeToMultiByteN( dst, n, &len, src, strlenW(src)*sizeof(WCHAR) );
        if (len < n) dst[len] = 0;
    }
    return len;
}


/*********************************************************************
 *           mbstowcs    (NTDLL.@)
 */
INT __cdecl NTDLL_mbstowcs( LPWSTR dst, LPCSTR src, INT n )
{
    DWORD len;

    if (!dst)
    {
        RtlMultiByteToUnicodeSize( &len, src, strlen(src) );
    }
    else
    {
        if (n <= 0) return 0;
        RtlMultiByteToUnicodeN( dst, n*sizeof(WCHAR), &len, src, strlen(src) );
        if (len / sizeof(WCHAR) < n) dst[len / sizeof(WCHAR)] = 0;
    }
    return len / sizeof(WCHAR);
}


/*********************************************************************
 *                  wcstol  (NTDLL.@)
 */
long __cdecl NTDLL_wcstol(LPCWSTR s,LPWSTR *end,INT base)
{
    return strtolW( s, end, base );
}


/*********************************************************************
 *                  wcstoul  (NTDLL.@)
 */
unsigned long __cdecl NTDLL_wcstoul(LPCWSTR s,LPWSTR *end,INT base)
{
    return strtoulW( s, end, base );
}


/*********************************************************************
 *           iswctype    (NTDLL.@)
 */
INT __cdecl NTDLL_iswctype( WCHAR wc, WCHAR wct )
{
    return (get_char_typeW(wc) & 0xfff) & wct;
}


/*********************************************************************
 *           iswalpha    (NTDLL.@)
 */
INT __cdecl NTDLL_iswalpha( WCHAR wc )
{
    return get_char_typeW(wc) & C1_ALPHA;
}

/*********************************************************************
 *		iswdigit (NTDLL.@)
 *
 * Checks if an unicode char wc is a digit
 *
 * RETURNS
 *  TRUE: The unicode char wc is a digit.
 *  FALSE: Otherwise
 */
INT __cdecl NTDLL_iswdigit( WCHAR wc )
{
    return isdigitW(wc);
}


/*********************************************************************
 *		iswlower (NTDLL.@)
 *
 * Checks if an unicode char wc is a lower case letter
 *
 * RETURNS
 *  TRUE: The unicode char wc is a lower case letter.
 *  FALSE: Otherwise
 */
INT __cdecl NTDLL_iswlower( WCHAR wc )
{
    return islowerW(wc);
}


/*********************************************************************
 *		iswspace (NTDLL.@)
 *
 * Checks if an unicode char wc is a white space character
 *
 * RETURNS
 *  TRUE: The unicode char wc is a white space character.
 *  FALSE: Otherwise
 */
INT __cdecl NTDLL_iswspace( WCHAR wc )
{
    return isspaceW(wc);
}


/*********************************************************************
 *		iswxdigit (NTDLL.@)
 *
 * Checks if an unicode char wc is an extended digit
 *
 * RETURNS
 *  TRUE: The unicode char wc is an extended digit.
 *  FALSE: Otherwise
 */
INT __cdecl NTDLL_iswxdigit( WCHAR wc )
{
    return isxdigitW(wc);
}


/*********************************************************************
 *      _ultow   (NTDLL.@)
 *
 * Converts an unsigned long integer to an unicode string.
 *
 * Assigns a '\0' terminated string to str and returns str.
 * Does not check if radix is in the range of 2 to 36 (as native DLL).
 * For str == NULL just returns NULL (as native DLL).
 */
LPWSTR __cdecl _ultow( unsigned long value, LPWSTR str, INT radix )
{
    WCHAR buffer[33];
    PWCHAR pos;
    WCHAR digit;

    pos = &buffer[32];
    *pos = '\0';

    do {
	digit = value % radix;
	value = value / radix;
	if (digit < 10) {
	    *--pos = '0' + digit;
	} else {
	    *--pos = 'a' + digit - 10;
	} /* if */
    } while (value != 0L);

    if (str != NULL) {
	memcpy(str, pos, (&buffer[32] - pos + 1) * sizeof(WCHAR));
    } /* if */
    return str;
}


/*********************************************************************
 *      _ltow   (NTDLL.@)
 *
 * Converts a long integer to an unicode string.
 *
 * RETURNS
 *  Always returns str.
 *
 * NOTES
 *  Converts value to a '\0' terminated wstring which is copied to str.
 *  The maximum length of the copied str is 33 bytes. If radix
 *  is 10 and value is negative, the value is converted with sign.
 *  Does not check if radix is in the range of 2 to 36.
 *  If str is NULL it just returns NULL.
 */
LPWSTR __cdecl _ltow(
    long value, /* [I] Value to be converted */
    LPWSTR str, /* [O] Destination for the converted value */
    INT radix)  /* [I] Number base for conversion */
{
    unsigned long val;
    int negative;
    WCHAR buffer[33];
    PWCHAR pos;
    WCHAR digit;

    if (value < 0 && radix == 10) {
	negative = 1;
        val = -value;
    } else {
	negative = 0;
        val = value;
    } /* if */

    pos = &buffer[32];
    *pos = '\0';

    do {
	digit = val % radix;
	val = val / radix;
	if (digit < 10) {
	    *--pos = '0' + digit;
	} else {
	    *--pos = 'a' + digit - 10;
	} /* if */
    } while (val != 0L);

    if (negative) {
	*--pos = '-';
    } /* if */

    if (str != NULL) {
	memcpy(str, pos, (&buffer[32] - pos + 1) * sizeof(WCHAR));
    } /* if */
    return str;
}



/*********************************************************************
 *      _itow    (NTDLL.@)
 *
 * Converts an integer to an unicode string.
 *
 * RETURNS
 *  Always returns str.
 *
 * NOTES
 *  Converts value to a '\0' terminated wstring which is copied to str.
 *  The maximum length of the copied str is 33 bytes. If radix
 *  is 10 and value is negative, the value is converted with sign.
 *  Does not check if radix is in the range of 2 to 36.
 *  If str is NULL it just returns NULL.
 *
 * DIFFERENCES
 * - The native function crashes when the string is longer than 19 chars.
 *   This function does not have this bug.
 */
LPWSTR __cdecl _itow(
    int value,  /* [I] Value to be converted */
    LPWSTR str, /* [O] Destination for the converted value */
    INT radix)  /* [I] Number base for conversion */
{
    return _ltow(value, str, radix);
}


/*********************************************************************
 *      _ui64tow   (NTDLL.@)
 *
 * Converts a large unsigned integer to an unicode string.
 *
 * Assigns a '\0' terminated wstring to str and returns str.
 * Does not check if radix is in the range of 2 to 36 (as native DLL).
 * For str == NULL just returns NULL (as native DLL).
 *
 * Difference:
 * - This function does not exist in the native DLL (but in msvcrt).
 *   But since the maintenance of all these functions is better done
 *   in one place we implement it here.
 */
LPWSTR __cdecl _ui64tow( ULONGLONG value, LPWSTR str, INT radix )
{
    WCHAR buffer[65];
    PWCHAR pos;
    WCHAR digit;

    pos = &buffer[64];
    *pos = '\0';

    do {
	digit = value % radix;
	value = value / radix;
	if (digit < 10) {
	    *--pos = '0' + digit;
	} else {
	    *--pos = 'a' + digit - 10;
	} /* if */
    } while (value != 0L);

    if (str != NULL) {
	memcpy(str, pos, (&buffer[64] - pos + 1) * sizeof(WCHAR));
    } /* if */
    return str;
}


/*********************************************************************
 *      _i64tow   (NTDLL.@)
 *
 * Converts a large integer to an unicode string.
 *
 * Assigns a '\0' terminated wstring to str and returns str. If radix
 * is 10 and value is negative, the value is converted with sign.
 * Does not check if radix is in the range of 2 to 36 (as native DLL).
 * For str == NULL just returns NULL (as native DLL).
 *
 * Difference:
 * - The native DLL converts negative values (for base 10) wrong:
 *                     -1 is converted to -18446744073709551615
 *                     -2 is converted to -18446744073709551614
 *   -9223372036854775807 is converted to  -9223372036854775809
 *   -9223372036854775808 is converted to  -9223372036854775808
 *   The native msvcrt _i64tow function and our ntdll function do
 *   not have this bug.
 */
LPWSTR __cdecl _i64tow( LONGLONG value, LPWSTR str, INT radix )
{
    ULONGLONG val;
    int negative;
    WCHAR buffer[65];
    PWCHAR pos;
    WCHAR digit;

    if (value < 0 && radix == 10) {
	negative = 1;
        val = -value;
    } else {
	negative = 0;
        val = value;
    } /* if */

    pos = &buffer[64];
    *pos = '\0';

    do {
	digit = val % radix;
	val = val / radix;
	if (digit < 10) {
	    *--pos = '0' + digit;
	} else {
	    *--pos = 'a' + digit - 10;
	} /* if */
    } while (val != 0L);

    if (negative) {
	*--pos = '-';
    } /* if */

    if (str != NULL) {
	memcpy(str, pos, (&buffer[64] - pos + 1) * sizeof(WCHAR));
    } /* if */
    return str;
}


/*********************************************************************
 *      _wtol    (NTDLL.@)
 *
 * Converts an unicode string to a long integer.
 *
 * On success it returns the integer value otherwise it returns 0.
 * Accepts: {whitespace} [+|-] {digits}
 * No check of overflow: Just assigns lower 32 bits (as native DLL).
 * Does not check for str != NULL (as native DLL).
 */
LONG __cdecl _wtol( LPWSTR str )
{
    ULONG RunningTotal = 0;
    char bMinus = 0;

    while (isspaceW(*str)) {
	str++;
    } /* while */

    if (*str == '+') {
	str++;
    } else if (*str == '-') {
	bMinus = 1;
	str++;
    } /* if */

    while (*str >= '0' && *str <= '9') {
	RunningTotal = RunningTotal * 10 + *str - '0';
	str++;
    } /* while */

    return bMinus ? -RunningTotal : RunningTotal;
}


/*********************************************************************
 *      _wtoi    (NTDLL.@)
 *
 * Converts an unicode string to an integer.
 *
 * On success it returns the integer value otherwise it returns 0.
 * Accepts: {whitespace} [+|-] {digits}
 * No check of overflow: Just assigns lower 32 bits (as native DLL).
 * Does not check for str != NULL (as native DLL).
 */
int __cdecl _wtoi( LPWSTR string )
{
    return _wtol(string);
}


/*********************************************************************
 *      _wtoi64   (NTDLL.@)
 *
 * Converts an unicode string to a large integer.
 *
 * On success it returns the integer value otherwise it returns 0.
 * Accepts: {whitespace} [+|-] {digits}
 * No check of overflow: Just assigns lower 64 bits (as native DLL).
 * Does not check for str != NULL (as native DLL).
 */
LONGLONG  __cdecl _wtoi64( LPWSTR str )
{
    ULONGLONG RunningTotal = 0;
    char bMinus = 0;

    while (isspaceW(*str)) {
	str++;
    } /* while */

    if (*str == '+') {
	str++;
    } else if (*str == '-') {
	bMinus = 1;
	str++;
    } /* if */

    while (*str >= '0' && *str <= '9') {
	RunningTotal = RunningTotal * 10 + *str - '0';
	str++;
    } /* while */

    return bMinus ? -RunningTotal : RunningTotal;
}



/***********************************************************************
 *        _snwprintf (NTDLL.@)
 */
int __cdecl NTDLL_snwprintf(WCHAR *str, unsigned int len, const WCHAR *format, ...)
{
  int retval;
  va_list valist;
  va_start(valist, format);
  retval = vsnprintfW(str, len, format, valist);
  va_end(valist);
  return retval;
}


/***********************************************************************
 *        swprintf (NTDLL.@)
 */
int __cdecl NTDLL_swprintf(WCHAR *str, const WCHAR *format, ...)
{
  int retval;
  va_list valist;
  va_start(valist, format);
  retval = vsnprintfW(str, INT_MAX, format, valist);
  va_end(valist);
  return retval;
}
