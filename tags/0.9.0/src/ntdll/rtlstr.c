/*
 * Rtl string functions
 *
 * Copyright (C) 1996-1998 Marcus Meissner
 * Copyright (C) 2000      Alexandre Julliard
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ntddk.h"
#include "wine/unicode.h"
#include "wine/debug.h"
#include "winnt.h"

WINE_DEFAULT_DEBUG_CHANNEL(ntdll);

#ifdef __WIN32OS2__
#define RtlAllocateHeap  HeapAlloc
#define RtlFreeHeap      HeapFree
#endif


UINT NlsAnsiCodePage = 1252;
BYTE NlsMbCodePageTag = 0;
BYTE NlsMbOemCodePageTag = 0;

static const union cptable *ansi_table;
static const union cptable *oem_table;

inline static const union cptable *get_ansi_table(void)
{
    if (!ansi_table) ansi_table = cp_get_table( 1252 );
    return ansi_table;
}

inline static const union cptable *get_oem_table(void)
{
    if (!oem_table) oem_table = cp_get_table( 437 );
    return oem_table;
}


/**************************************************************************
 *	__wine_init_codepages   (NTDLL.@)
 *
 * Set the code page once kernel32 is loaded. Should be done differently.
 */
void __wine_init_codepages( const union cptable *ansi, const union cptable *oem )
{
    ansi_table = ansi;
    oem_table = oem;
    NlsAnsiCodePage = ansi->info.codepage;
}


/**************************************************************************
 *	RtlInitAnsiString   (NTDLL.@)
 */
void WINAPI RtlInitAnsiString( PSTRING target, LPCSTR source)
{
    if ((target->Buffer = (LPSTR)source))
    {
        target->Length = strlen(source);
        target->MaximumLength = target->Length + 1;
    }
    else target->Length = target->MaximumLength = 0;
}


/**************************************************************************
 *	RtlInitString   (NTDLL.@)
 */
void WINAPI RtlInitString( PSTRING target, LPCSTR source )
{
    return RtlInitAnsiString( target, source );
}


/**************************************************************************
 *	RtlFreeAnsiString   (NTDLL.@)
 */
void WINAPI RtlFreeAnsiString( PSTRING str )
{
    if (str->Buffer) RtlFreeHeap( GetProcessHeap(), 0, str->Buffer );
}


/**************************************************************************
 *	RtlFreeOemString   (NTDLL.@)
 */
void WINAPI RtlFreeOemString( PSTRING str )
{
    RtlFreeAnsiString( str );
}


/**************************************************************************
 *	RtlCopyString   (NTDLL.@)
 */
void WINAPI RtlCopyString( STRING *dst, const STRING *src )
{
    if (src)
    {
        unsigned int len = min( src->Length, dst->MaximumLength );
        memcpy( dst->Buffer, src->Buffer, len );
        dst->Length = len;
    }
    else dst->Length = 0;
}


/**************************************************************************
 *	RtlInitUnicodeString   (NTDLL.@)
 */
void WINAPI RtlInitUnicodeString( PUNICODE_STRING target, LPCWSTR source )
{
    if ((target->Buffer = (LPWSTR)source))
    {
        target->Length = strlenW(source) * sizeof(WCHAR);
        target->MaximumLength = target->Length + sizeof(WCHAR);
    }
    else target->Length = target->MaximumLength = 0;
}


/**************************************************************************
 *	RtlCreateUnicodeString   (NTDLL.@)
 */
BOOLEAN WINAPI RtlCreateUnicodeString( PUNICODE_STRING target, LPCWSTR src )
{
    int len = (strlenW(src) + 1) * sizeof(WCHAR);
    if (!(target->Buffer = RtlAllocateHeap( GetProcessHeap(), 0, len ))) return FALSE;
    memcpy( target->Buffer, src, len );
    target->MaximumLength = len;
    target->Length = len - sizeof(WCHAR);
    return TRUE;
}


/**************************************************************************
 *	RtlCreateUnicodeStringFromAsciiz   (NTDLL.@)
 */
BOOLEAN WINAPI RtlCreateUnicodeStringFromAsciiz( PUNICODE_STRING target, LPCSTR src )
{
    STRING ansi;
    RtlInitAnsiString( &ansi, src );
    return !RtlAnsiStringToUnicodeString( target, &ansi, TRUE );
}


/**************************************************************************
 *	RtlFreeUnicodeString   (NTDLL.@)
 */
void WINAPI RtlFreeUnicodeString( PUNICODE_STRING str )
{
    if (str->Buffer) RtlFreeHeap( GetProcessHeap(), 0, str->Buffer );
}


/**************************************************************************
 *	RtlCopyUnicodeString   (NTDLL.@)
 */
void WINAPI RtlCopyUnicodeString( UNICODE_STRING *dst, const UNICODE_STRING *src )
{
    if (src)
    {
        unsigned int len = min( src->Length, dst->MaximumLength );
        memcpy( dst->Buffer, src->Buffer, len );
        dst->Length = len;
        /* append terminating NULL if enough space */
        if (len < dst->MaximumLength) dst->Buffer[len / sizeof(WCHAR)] = 0;
    }
    else dst->Length = 0;
}


/**************************************************************************
 *	RtlEraseUnicodeString   (NTDLL.@)
 */
void WINAPI RtlEraseUnicodeString( UNICODE_STRING *str )
{
    if (str->Buffer)
    {
        memset( str->Buffer, 0, str->MaximumLength );
        str->Length = 0;
    }
}

/*
    COMPARISON FUNCTIONS
*/

/******************************************************************************
 *	RtlCompareString   (NTDLL.@)
 */
LONG WINAPI RtlCompareString( const STRING *s1, const STRING *s2, BOOLEAN CaseInsensitive )
{
    unsigned int len;
    LONG ret = 0;
    LPCSTR p1, p2;

    len = min(s1->Length, s2->Length);
    p1 = s1->Buffer;
    p2 = s2->Buffer;

    if (CaseInsensitive)
    {
        while (!ret && len--) ret = toupper(*p1++) - toupper(*p2++);
    }
    else
    {
        while (!ret && len--) ret = *p1++ - *p2++;
    }
    if (!ret) ret = s1->Length - s2->Length;
    return ret;
}


/******************************************************************************
 *	RtlCompareUnicodeString   (NTDLL.@)
 */
LONG WINAPI RtlCompareUnicodeString( const UNICODE_STRING *s1, const UNICODE_STRING *s2,
                                     BOOLEAN CaseInsensitive )
{
    unsigned int len;
    LONG ret = 0;
    LPCWSTR p1, p2;

    len = min(s1->Length, s2->Length) / sizeof(WCHAR);
    p1 = s1->Buffer;
    p2 = s2->Buffer;

    if (CaseInsensitive)
    {
        while (!ret && len--) ret = toupperW(*p1++) - toupperW(*p2++);
    }
    else
    {
        while (!ret && len--) ret = *p1++ - *p2++;
    }
    if (!ret) ret = s1->Length - s2->Length;
    return ret;
}


/**************************************************************************
 *	RtlEqualString   (NTDLL.@)
 */
BOOLEAN WINAPI RtlEqualString( const STRING *s1, const STRING *s2, BOOLEAN CaseInsensitive )
{
    if (s1->Length != s2->Length) return FALSE;
    return !RtlCompareString( s1, s2, CaseInsensitive );
}


/**************************************************************************
 *	RtlEqualUnicodeString   (NTDLL.@)
 */
BOOLEAN WINAPI RtlEqualUnicodeString( const UNICODE_STRING *s1, const UNICODE_STRING *s2,
                                      BOOLEAN CaseInsensitive )
{
    if (s1->Length != s2->Length) return FALSE;
    return !RtlCompareUnicodeString( s1, s2, CaseInsensitive );
}


/**************************************************************************
 *	RtlPrefixString   (NTDLL.@)
 *
 * Test if s1 is a prefix in s2
 */
BOOLEAN WINAPI RtlPrefixString( const STRING *s1, const STRING *s2, BOOLEAN ignore_case )
{
    unsigned int i;

    if (s1->Length > s2->Length) return FALSE;
    if (ignore_case)
    {
        for (i = 0; i < s1->Length; i++)
            if (toupper(s1->Buffer[i]) != toupper(s2->Buffer[i])) return FALSE;
    }
    else
    {
        for (i = 0; i < s1->Length; i++)
            if (s1->Buffer[i] != s2->Buffer[i]) return FALSE;
    }
    return TRUE;
}


/**************************************************************************
 *	RtlPrefixUnicodeString   (NTDLL.@)
 *
 * Test if s1 is a prefix in s2
 */
BOOLEAN WINAPI RtlPrefixUnicodeString( const UNICODE_STRING *s1,
                                       const UNICODE_STRING *s2,
                                       BOOLEAN ignore_case )
{
    unsigned int i;

    if (s1->Length > s2->Length) return FALSE;
    if (ignore_case)
    {
        for (i = 0; i < s1->Length / sizeof(WCHAR); i++)
            if (toupper(s1->Buffer[i]) != toupper(s2->Buffer[i])) return FALSE;
    }
    else
    {
        for (i = 0; i < s1->Length / sizeof(WCHAR); i++)
            if (s1->Buffer[i] != s2->Buffer[i]) return FALSE;
    }
    return TRUE;
}

/**************************************************************************
 *	RtlEqualComputerName   (NTDLL.@)
 *
 * Determine if two computer names are the same.
 *
 * PARAMS
 *  left  [I] First computer name
 *  right [I] Second computer name
 *
 * RETURNS
 *  0 if the names are equal, non-zero otherwise.
 *
 * NOTES
 *  The comparason is case insensitive.
 */
NTSTATUS WINAPI RtlEqualComputerName(const UNICODE_STRING *left,
                                     const UNICODE_STRING *right)
{
    NTSTATUS ret;
    STRING upLeft, upRight;

    if (!(ret = RtlUpcaseUnicodeStringToOemString( &upLeft, left, TRUE )))
    {
       if (!(ret = RtlUpcaseUnicodeStringToOemString( &upRight, right, TRUE )))
       {
         ret = RtlEqualString( &upLeft, &upRight, FALSE );
         RtlFreeOemString( &upRight );
       }
       RtlFreeOemString( &upLeft );
    }
    return ret;
}

/**************************************************************************
 *	RtlEqualDomainName   (NTDLL.@)
 *
 * Determine if two domain names are the same.
 *
 * PARAMS
 *  left  [I] First domain name
 *  right [I] Second domain name
 *
 * RETURNS
 *  0 if the names are equal, non-zero otherwise.
 *
 * NOTES
 *  The comparason is case insensitive.
 */
NTSTATUS WINAPI RtlEqualDomainName(const UNICODE_STRING *left,
                                   const UNICODE_STRING *right)
{
    return RtlEqualComputerName(left, right);
}


/*
	COPY BETWEEN ANSI_STRING or UNICODE_STRING
	there is no parameter checking, it just crashes
*/


/**************************************************************************
 *	RtlAnsiStringToUnicodeString   (NTDLL.@)
 *
 * NOTES:
 *  writes terminating 0
 */
NTSTATUS WINAPI RtlAnsiStringToUnicodeString( UNICODE_STRING *uni,
                                              const STRING *ansi,
                                              BOOLEAN doalloc )
{
    DWORD total = RtlAnsiStringToUnicodeSize( ansi );

    if (total > 0xffff) return STATUS_INVALID_PARAMETER_2;
    uni->Length = total - sizeof(WCHAR);
    if (doalloc)
    {
        uni->MaximumLength = total;
        if (!(uni->Buffer = RtlAllocateHeap( GetProcessHeap(), 0, total ))) return STATUS_NO_MEMORY;
    }
    else if (total > uni->MaximumLength) return STATUS_BUFFER_OVERFLOW;

    RtlMultiByteToUnicodeN( uni->Buffer, uni->Length, NULL, ansi->Buffer, ansi->Length );
    uni->Buffer[uni->Length / sizeof(WCHAR)] = 0;
    return STATUS_SUCCESS;
}


/**************************************************************************
 *	RtlOemStringToUnicodeString   (NTDLL.@)
 *
 * NOTES
 *  writes terminating 0
 *  if resulting length > 0xffff it returns STATUS_INVALID_PARAMETER_2
 */
NTSTATUS WINAPI RtlOemStringToUnicodeString( UNICODE_STRING *uni,
                                             const STRING *oem,
                                             BOOLEAN doalloc )
{
    DWORD total = RtlOemStringToUnicodeSize( oem );

    if (total > 0xffff) return STATUS_INVALID_PARAMETER_2;
    uni->Length = total - sizeof(WCHAR);
    if (doalloc)
    {
        uni->MaximumLength = total;
        if (!(uni->Buffer = RtlAllocateHeap( GetProcessHeap(), 0, total ))) return STATUS_NO_MEMORY;
    }
    else if (total > uni->MaximumLength) return STATUS_BUFFER_OVERFLOW;

    RtlOemToUnicodeN( uni->Buffer, uni->Length, NULL, oem->Buffer, oem->Length );
    uni->Buffer[uni->Length / sizeof(WCHAR)] = 0;
    return STATUS_SUCCESS;
}


/**************************************************************************
 *	RtlUnicodeStringToAnsiString   (NTDLL.@)
 *
 * NOTES
 *  writes terminating 0
 *  copies a part if the buffer is too small
 */
NTSTATUS WINAPI RtlUnicodeStringToAnsiString( STRING *ansi,
                                              const UNICODE_STRING *uni,
                                              BOOLEAN doalloc )
{
    NTSTATUS ret = STATUS_SUCCESS;
    DWORD len = RtlUnicodeStringToAnsiSize( uni );

    ansi->Length = len - 1;
    if (doalloc)
    {
        ansi->MaximumLength = len;
        if (!(ansi->Buffer = RtlAllocateHeap( GetProcessHeap(), 0, len ))) return STATUS_NO_MEMORY;
    }
    else if (ansi->MaximumLength < len)
    {
        if (!ansi->MaximumLength) return STATUS_BUFFER_OVERFLOW;
        ansi->Length = ansi->MaximumLength - 1;
        ret = STATUS_BUFFER_OVERFLOW;
    }

    RtlUnicodeToMultiByteN( ansi->Buffer, ansi->Length, NULL, uni->Buffer, uni->Length );
    ansi->Buffer[ansi->Length] = 0;
    return ret;
}


/**************************************************************************
 *	RtlUnicodeStringToOemString   (NTDLL.@)
 *
 * NOTES
 *   allocates uni->Length+1
 *   writes terminating 0
 */
NTSTATUS WINAPI RtlUnicodeStringToOemString( STRING *oem,
                                             const UNICODE_STRING *uni,
                                             BOOLEAN doalloc )
{
    NTSTATUS ret = STATUS_SUCCESS;
    DWORD len = RtlUnicodeStringToOemSize( uni );

    oem->Length = len - 1;
    if (doalloc)
    {
        oem->MaximumLength = len;
        if (!(oem->Buffer = RtlAllocateHeap( GetProcessHeap(), 0, len ))) return STATUS_NO_MEMORY;
    }
    else if (oem->MaximumLength < len)
    {
        if (!oem->MaximumLength) return STATUS_BUFFER_OVERFLOW;
        oem->Length = oem->MaximumLength - 1;
        ret = STATUS_BUFFER_OVERFLOW;
    }

    RtlUnicodeToOemN( oem->Buffer, oem->Length, NULL, uni->Buffer, uni->Length );
    oem->Buffer[oem->Length] = 0;
    return ret;
}


/**************************************************************************
 *	RtlMultiByteToUnicodeN   (NTDLL.@)
 *
 * NOTES
 *  if unistr is too small a part is copied
 */
NTSTATUS WINAPI RtlMultiByteToUnicodeN( LPWSTR dst, DWORD dstlen, LPDWORD reslen,
                                        LPCSTR src, DWORD srclen )
{

    int ret = cp_mbstowcs( get_ansi_table(), 0, src, srclen, dst, dstlen/sizeof(WCHAR) );
    if (reslen)
        *reslen = (ret >= 0) ? ret*sizeof(WCHAR) : dstlen; /* overflow -> we filled up to dstlen */
    return STATUS_SUCCESS;
}


/**************************************************************************
 *	RtlOemToUnicodeN   (NTDLL.@)
 */
NTSTATUS WINAPI RtlOemToUnicodeN( LPWSTR dst, DWORD dstlen, LPDWORD reslen,
                                  LPCSTR src, DWORD srclen )
{
    int ret = cp_mbstowcs( get_oem_table(), 0, src, srclen, dst, dstlen/sizeof(WCHAR) );
    if (reslen)
        *reslen = (ret >= 0) ? ret*sizeof(WCHAR) : dstlen; /* overflow -> we filled up to dstlen */
    return STATUS_SUCCESS;
}


/**************************************************************************
 *	RtlUnicodeToMultiByteN   (NTDLL.@)
 */
NTSTATUS WINAPI RtlUnicodeToMultiByteN( LPSTR dst, DWORD dstlen, LPDWORD reslen,
                                        LPCWSTR src, DWORD srclen )
{
    int ret = cp_wcstombs( get_ansi_table(), 0, src, srclen / sizeof(WCHAR),
                           dst, dstlen, NULL, NULL );
    if (reslen)
        *reslen = (ret >= 0) ? ret : dstlen; /* overflow -> we filled up to dstlen */
    return STATUS_SUCCESS;
}


/**************************************************************************
 *	RtlUnicodeToOemN   (NTDLL.@)
 */
NTSTATUS WINAPI RtlUnicodeToOemN( LPSTR dst, DWORD dstlen, LPDWORD reslen,
                                  LPCWSTR src, DWORD srclen )
{
    int ret = cp_wcstombs( get_oem_table(), 0, src, srclen / sizeof(WCHAR),
                           dst, dstlen, NULL, NULL );
    if (reslen)
        *reslen = (ret >= 0) ? ret : dstlen; /* overflow -> we filled up to dstlen */
    return STATUS_SUCCESS;
}


/*
     CASE CONVERSIONS
*/

/**************************************************************************
 *	RtlUpperChar   (NTDLL.@)
 *
 * Converts an Ascii character to uppercase.
 *
 * PARAMS
 *  ch [I] Character to convert
 *
 * RETURNS
 *  The uppercase character value.
 *
 * NOTES
 *  For the input characters from 'a' .. 'z' it returns 'A' .. 'Z'.
 *  All other input characters are returned unchanged. The locale and
 *  multibyte characters are not taken into account (as native DLL).
 */
CHAR WINAPI RtlUpperChar( CHAR ch )
{
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 'A';
    } else {
        return ch;
    } /* if */
}

/**************************************************************************
 *	RtlUpperString   (NTDLL.@)
 *
 * Converts an Ascii string to uppercase.
 *
 * PARAMS
 *  dst [O] Destination for converted string
 *  src [I] Source string to convert
 *
 * RETURNS
 *  Nothing.
 *
 * NOTES
 *  For the src characters from 'a' .. 'z' it assigns 'A' .. 'Z' to dst.
 *  All other src characters are copied unchanged to dst. The locale and
 *  multibyte characters are not taken into account (as native DLL).
 *  The number of character copied is the minimum of src->Length and
 *  the dst->MaximumLength.
 */
void WINAPI RtlUpperString( STRING *dst, const STRING *src )
{
    unsigned int i, len = min(src->Length, dst->MaximumLength);

    for (i = 0; i < len; i++) dst->Buffer[i] = RtlUpperChar(src->Buffer[i]);
    dst->Length = len;
}


/**************************************************************************
 *	RtlUpcaseUnicodeChar   (NTDLL.@)
 *
 * Converts an Unicode character to uppercase.
 *
 * PARAMS
 *  wch [I] Character to convert
 *
 * RETURNS
 *  The uppercase character value.
 */
WCHAR WINAPI RtlUpcaseUnicodeChar( WCHAR wch )
{
    return toupperW(wch);
}

/**************************************************************************
 *	RtlDowncaseUnicodeChar   (NTDLL.@)
 *
 * Converts an Unicode character to lowercase.
 *
 * PARAMS
 *  wch [I] Character to convert
 *
 * RETURNS
 *  The lowercase character value.
 */
WCHAR WINAPI RtlDowncaseUnicodeChar(WCHAR wch)
{
    return tolowerW(wch);
}

/**************************************************************************
 *	RtlUpcaseUnicodeString   (NTDLL.@)
 *
 * NOTES:
 *  destination string is never 0-terminated because dest can be equal to src
 *  and src might be not 0-terminated
 *  dest.Length only set when success
 */
NTSTATUS WINAPI RtlUpcaseUnicodeString( UNICODE_STRING *dest,
                                        const UNICODE_STRING *src,
                                        BOOLEAN doalloc )
{
    DWORD i, len = src->Length;

    if (doalloc)
    {
        dest->MaximumLength = len;
        if (!(dest->Buffer = RtlAllocateHeap( GetProcessHeap(), 0, len ))) return STATUS_NO_MEMORY;
    }
    else if (len > dest->MaximumLength) return STATUS_BUFFER_OVERFLOW;

    for (i = 0; i < len/sizeof(WCHAR); i++) dest->Buffer[i] = toupperW(src->Buffer[i]);
    dest->Length = len;
    return STATUS_SUCCESS;
}


/**************************************************************************
 *	RtlDowncaseUnicodeString   (NTDLL.@)
 *
 * Converts an Unicode string to lowercase.
 *
 * PARAMS
 *  dest    [O] Destination for converted string
 *  src     [I] Source string to convert
 *  doalloc [I] TRUE=Allocate a buffer for dest if it doesn't have one
 *
 * RETURNS
 *  Success: STATUS_SUCCESS. dest contains the converted string.
 *  Failure: STATUS_NO_MEMORY, if doalloc is TRUE and memory allocation fails, or
 *           STATUS_BUFFER_OVERFLOW, if doalloc is FALSE and dest is too small.
 *
 * NOTES
 *  dest is never NUL terminated because it may be equal to src, and src
 *  might not be NUL terminated. dest->Length is only set upon success.
 */
NTSTATUS WINAPI RtlDowncaseUnicodeString(
	UNICODE_STRING *dest,
	const UNICODE_STRING *src,
	BOOLEAN doalloc)
{
    DWORD i;
    DWORD len = src->Length;

    if (doalloc) {
        dest->MaximumLength = len;
        if (!(dest->Buffer = RtlAllocateHeap( GetProcessHeap(), 0, len ))) {
	    return STATUS_NO_MEMORY;
	} /* if */
    } else if (len > dest->MaximumLength) {
	return STATUS_BUFFER_OVERFLOW;
    } /* if */

    for (i = 0; i < len/sizeof(WCHAR); i++) {
	dest->Buffer[i] = tolowerW(src->Buffer[i]);
    } /* for */
    dest->Length = len;
    return STATUS_SUCCESS;
}

/**************************************************************************
 *	RtlUpcaseUnicodeStringToAnsiString   (NTDLL.@)
 *
 * NOTES
 *  writes terminating 0
 */
NTSTATUS WINAPI RtlUpcaseUnicodeStringToAnsiString( STRING *dst,
                                                    const UNICODE_STRING *src,
                                                    BOOLEAN doalloc )
{
    NTSTATUS ret;
    UNICODE_STRING upcase;

    if (!(ret = RtlUpcaseUnicodeString( &upcase, src, TRUE )))
    {
        ret = RtlUnicodeStringToAnsiString( dst, &upcase, doalloc );
        RtlFreeUnicodeString( &upcase );
    }
    return ret;
}


/**************************************************************************
 *	RtlUpcaseUnicodeStringToOemString   (NTDLL.@)
 *
 * NOTES
 *  writes terminating 0
 */
NTSTATUS WINAPI RtlUpcaseUnicodeStringToOemString( STRING *dst,
                                                   const UNICODE_STRING *src,
                                                   BOOLEAN doalloc )
{
    NTSTATUS ret;
    UNICODE_STRING upcase;

    if (!(ret = RtlUpcaseUnicodeString( &upcase, src, TRUE )))
    {
        ret = RtlUnicodeStringToOemString( dst, &upcase, doalloc );
        RtlFreeUnicodeString( &upcase );
    }
    return ret;
}


/**************************************************************************
 *	RtlUpcaseUnicodeToMultiByteN   (NTDLL.@)
 */
NTSTATUS WINAPI RtlUpcaseUnicodeToMultiByteN( LPSTR dst, DWORD dstlen, LPDWORD reslen,
                                              LPCWSTR src, DWORD srclen )
{
    NTSTATUS ret;
    LPWSTR upcase;
    DWORD i;

    if (!(upcase = RtlAllocateHeap( GetProcessHeap(), 0, srclen ))) return STATUS_NO_MEMORY;
    for (i = 0; i < srclen/sizeof(WCHAR); i++) upcase[i] = toupperW(src[i]);
    ret = RtlUnicodeToMultiByteN( dst, dstlen, reslen, upcase, srclen );
    RtlFreeHeap( GetProcessHeap(), 0, upcase );
    return ret;
}


/**************************************************************************
 *	RtlUpcaseUnicodeToOemN   (NTDLL.@)
 */
NTSTATUS WINAPI RtlUpcaseUnicodeToOemN( LPSTR dst, DWORD dstlen, LPDWORD reslen,
                                        LPCWSTR src, DWORD srclen )
{
    NTSTATUS ret;
    LPWSTR upcase;
    DWORD i;

    if (!(upcase = RtlAllocateHeap( GetProcessHeap(), 0, srclen ))) return STATUS_NO_MEMORY;
    for (i = 0; i < srclen/sizeof(WCHAR); i++) upcase[i] = toupperW(src[i]);
    ret = RtlUnicodeToOemN( dst, dstlen, reslen, upcase, srclen );
    RtlFreeHeap( GetProcessHeap(), 0, upcase );
    return ret;
}


/*
	STRING SIZE
*/

/**************************************************************************
 *      RtlOemStringToUnicodeSize   (NTDLL.@)
 *      RtlxOemStringToUnicodeSize  (NTDLL.@)
 *
 * Return the size in bytes necessary for the Unicode conversion of 'str',
 * including the terminating NULL.
 */
UINT WINAPI RtlOemStringToUnicodeSize( const STRING *str )
{
    int ret = cp_mbstowcs( get_oem_table(), 0, str->Buffer, str->Length, NULL, 0 );
    return (ret + 1) * sizeof(WCHAR);
}


/**************************************************************************
 *      RtlAnsiStringToUnicodeSize   (NTDLL.@)
 *      RtlxAnsiStringToUnicodeSize  (NTDLL.@)
 *
 * Return the size in bytes necessary for the Unicode conversion of 'str',
 * including the terminating NULL.
 */
DWORD WINAPI RtlAnsiStringToUnicodeSize( const STRING *str )
{
    DWORD ret;
    RtlMultiByteToUnicodeSize( &ret, str->Buffer, str->Length );
    return ret + sizeof(WCHAR);
}


/**************************************************************************
 *      RtlMultiByteToUnicodeSize   (NTDLL.@)
 *
 * Compute the size in bytes necessary for the Unicode conversion of 'str',
 * without the terminating NULL.
 */
NTSTATUS WINAPI RtlMultiByteToUnicodeSize( DWORD *size, LPCSTR str, UINT len )
{
    *size = cp_mbstowcs( get_ansi_table(), 0, str, len, NULL, 0 ) * sizeof(WCHAR);
    return STATUS_SUCCESS;
}


/**************************************************************************
 *      RtlUnicodeToMultiByteSize   (NTDLL.@)
 *
 * Compute the size necessary for the multibyte conversion of 'str',
 * without the terminating NULL.
 */
NTSTATUS WINAPI RtlUnicodeToMultiByteSize( DWORD *size, LPCWSTR str, UINT len )
{
    *size = cp_wcstombs( get_ansi_table(), 0, str, len / sizeof(WCHAR), NULL, 0, NULL, NULL );
    return STATUS_SUCCESS;
}


/**************************************************************************
 *      RtlUnicodeStringToAnsiSize   (NTDLL.@)
 *      RtlxUnicodeStringToAnsiSize  (NTDLL.@)
 *
 * Return the size in bytes necessary for the Ansi conversion of 'str',
 * including the terminating NULL.
 */
DWORD WINAPI RtlUnicodeStringToAnsiSize( const UNICODE_STRING *str )
{
    DWORD ret;
    RtlUnicodeToMultiByteSize( &ret, str->Buffer, str->Length );
    return ret + 1;
}


/**************************************************************************
 *      RtlUnicodeStringToOemSize   (NTDLL.@)
 *      RtlxUnicodeStringToOemSize  (NTDLL.@)
 *
 * Return the size in bytes necessary for the OEM conversion of 'str',
 * including the terminating NULL.
 */
DWORD WINAPI RtlUnicodeStringToOemSize( const UNICODE_STRING *str )
{
    return cp_wcstombs( get_oem_table(), 0, str->Buffer, str->Length / sizeof(WCHAR),
                        NULL, 0, NULL, NULL ) + 1;
}


/**************************************************************************
 *      RtlAppendStringToString   (NTDLL.@)
 */
NTSTATUS WINAPI RtlAppendStringToString( STRING *dst, const STRING *src )
{
    unsigned int len = src->Length + dst->Length;
    if (len > dst->MaximumLength) return STATUS_BUFFER_TOO_SMALL;
    memcpy( dst->Buffer + dst->Length, src->Buffer, src->Length );
    dst->Length = len;
    return STATUS_SUCCESS;
}


/**************************************************************************
 *      RtlAppendAsciizToString   (NTDLL.@)
 */
NTSTATUS WINAPI RtlAppendAsciizToString( STRING *dst, LPCSTR src )
{
    if (src)
    {
        unsigned int srclen = strlen(src);
        unsigned int total  = srclen + dst->Length;
        if (total > dst->MaximumLength) return STATUS_BUFFER_TOO_SMALL;
        memcpy( dst->Buffer + dst->Length, src, srclen );
        dst->Length = total;
    }
    return STATUS_SUCCESS;
}


/**************************************************************************
 *      RtlAppendUnicodeToString   (NTDLL.@)
 */
NTSTATUS WINAPI RtlAppendUnicodeToString( UNICODE_STRING *dst, LPCWSTR src )
{
    if (src)
    {
        unsigned int srclen = strlenW(src) * sizeof(WCHAR);
        unsigned int total  = srclen + dst->Length;
        if (total > dst->MaximumLength) return STATUS_BUFFER_TOO_SMALL;
        memcpy( dst->Buffer + dst->Length/sizeof(WCHAR), src, srclen );
        dst->Length = total;
        /* append terminating NULL if enough space */
        if (total < dst->MaximumLength) dst->Buffer[total / sizeof(WCHAR)] = 0;
    }
    return STATUS_SUCCESS;
}


/**************************************************************************
 *      RtlAppendUnicodeStringToString   (NTDLL.@)
 */
NTSTATUS WINAPI RtlAppendUnicodeStringToString( UNICODE_STRING *dst, const UNICODE_STRING *src )
{
    unsigned int len = src->Length + dst->Length;
    if (src->Length == 0) return STATUS_SUCCESS;
    if (len > dst->MaximumLength) return STATUS_BUFFER_TOO_SMALL;
    memcpy( dst->Buffer + dst->Length/sizeof(WCHAR), src->Buffer, src->Length );
    dst->Length = len;
    /* append terminating NULL if enough space */
    if (len < dst->MaximumLength) dst->Buffer[len / sizeof(WCHAR)] = 0;
    return STATUS_SUCCESS;
}


/*
	MISC
*/

/**************************************************************************
 *	RtlIsTextUnicode (NTDLL.@)
 *
 *	Apply various feeble heuristics to guess whether
 *	the text buffer contains Unicode.
 *	FIXME: should implement more tests.
 */
DWORD WINAPI RtlIsTextUnicode(
	LPVOID buf,
	DWORD len,
	DWORD *pf)
{
	LPWSTR s = buf;
	DWORD flags = -1, out_flags = 0;

	if (!len)
		goto out;
	if (pf)
		flags = *pf;
	/*
	 * Apply various tests to the text string. According to the
	 * docs, each test "passed" sets the corresponding flag in
	 * the output flags. But some of the tests are mutually
	 * exclusive, so I don't see how you could pass all tests ...
	 */

	/* Check for an odd length ... pass if even. */
	if (!(len & 1))
		out_flags |= IS_TEXT_UNICODE_ODD_LENGTH;

	/* Check for the special unicode marker byte. */
	if (*s == 0xFEFF)
		out_flags |= IS_TEXT_UNICODE_SIGNATURE;

	/*
	 * Check whether the string passed all of the tests.
	 */
	flags &= ITU_IMPLEMENTED_TESTS;
	if ((out_flags & flags) != flags)
		len = 0;
out:
	if (pf)
		*pf = out_flags;
	return len;
}

/**************************************************************************
 *      RtlCharToInteger   (NTDLL.@)
 *
 * Converts a character string into its integer equivalent.
 *
 * RETURNS
 *  Success: STATUS_SUCCESS. value contains the converted number
 *  Failure: STATUS_INVALID_PARAMETER, if base is not 0, 2, 8, 10 or 16.
 *           STATUS_ACCESS_VIOLATION, if value is NULL.
 *
 * NOTES
 *  For base 0 it uses 10 as base and the string should be in the format
 *      "{whitespace} [+|-] [0[x|o|b]] {digits}".
 *  For other bases the string should be in the format
 *      "{whitespace} [+|-] {digits}".
 *  No check is made for value overflow, only the lower 32 bits are assigned.
 *  If str is NULL it crashes, as the native function does.
 *
 * DIFFERENCES
 *  This function does not read garbage behind '\0' as the native version does.
 */
NTSTATUS WINAPI RtlCharToInteger(
    PCSZ str,      /* [I] '\0' terminated single-byte string containing a number */
    ULONG base,    /* [I] Number base for conversion (allowed 0, 2, 8, 10 or 16) */
    ULONG *value)  /* [O] Destination for the converted value */
{
    CHAR chCurrent;
    int digit;
    ULONG RunningTotal = 0;
    char bMinus = 0;

    while (*str != '\0' && *str <= ' ') {
	str++;
    } /* while */

    if (*str == '+') {
	str++;
    } else if (*str == '-') {
	bMinus = 1;
	str++;
    } /* if */

    if (base == 0) {
	base = 10;
	if (str[0] == '0') {
	    if (str[1] == 'b') {
		str += 2;
		base = 2;
	    } else if (str[1] == 'o') {
		str += 2;
		base = 8;
	    } else if (str[1] == 'x') {
		str += 2;
		base = 16;
	    } /* if */
	} /* if */
    } else if (base != 2 && base != 8 && base != 10 && base != 16) {
	return STATUS_INVALID_PARAMETER;
    } /* if */

    if (value == NULL) {
	return STATUS_ACCESS_VIOLATION;
    } /* if */

    while (*str != '\0') {
	chCurrent = *str;
	if (chCurrent >= '0' && chCurrent <= '9') {
	    digit = chCurrent - '0';
	} else if (chCurrent >= 'A' && chCurrent <= 'Z') {
	    digit = chCurrent - 'A' + 10;
	} else if (chCurrent >= 'a' && chCurrent <= 'z') {
	    digit = chCurrent - 'a' + 10;
	} else {
	    digit = -1;
	} /* if */
	if (digit < 0 || digit >= base) {
	    *value = bMinus ? -RunningTotal : RunningTotal;
	    return STATUS_SUCCESS;
	} /* if */

	RunningTotal = RunningTotal * base + digit;
	str++;
    } /* while */

    *value = bMinus ? -RunningTotal : RunningTotal;
    return STATUS_SUCCESS;
}

/**************************************************************************
 *      RtlIntegerToChar   (NTDLL.@)
 *
 * Converts an unsigned integer to a character string.
 *
 * RETURNS
 *  Success: STATUS_SUCCESS. str contains the converted number
 *  Failure: STATUS_INVALID_PARAMETER, if base is not 0, 2, 8, 10 or 16.
 *           STATUS_BUFFER_OVERFLOW, if str would be larger than length.
 *           STATUS_ACCESS_VIOLATION, if str is NULL.
 *
 * NOTES
 *  Instead of base 0 it uses 10 as base.
 *  Writes at most length characters to the string str.
 *  Str is '\0' terminated when length allowes it.
 *  When str fits exactly in length characters the '\0' is ommitted.
 */
NTSTATUS WINAPI RtlIntegerToChar(
    ULONG value,   /* [I] Value to be converted */
    ULONG base,    /* [I] Number base for conversion (allowed 0, 2, 8, 10 or 16) */
    ULONG length,  /* [I] Length of the str buffer in bytes */
    PCHAR str)     /* [O] Destination for the converted value */
{
    CHAR buffer[33];
    PCHAR pos;
    CHAR digit;
    ULONG len;

    if (base == 0) {
	base = 10;
    } else if (base != 2 && base != 8 && base != 10 && base != 16) {
	return STATUS_INVALID_PARAMETER;
    } /* if */

    pos = &buffer[32];
    *pos = '\0';

    do {
	pos--;
	digit = value % base;
	value = value / base;
	if (digit < 10) {
	    *pos = '0' + digit;
	} else {
	    *pos = 'A' + digit - 10;
	} /* if */
    } while (value != 0L);

    len = &buffer[32] - pos;
    if (len > length) {
	return STATUS_BUFFER_OVERFLOW;
    } else if (str == NULL) {
	return STATUS_ACCESS_VIOLATION;
    } else if (len == length) {
	memcpy(str, pos, len);
    } else {
	memcpy(str, pos, len + 1);
    } /* if */
    return STATUS_SUCCESS;
}

/**************************************************************************
 *      RtlUnicodeStringToInteger (NTDLL.@)
 *
 * Converts an unicode string into its integer equivalent.
 *
 * RETURNS
 *  Success: STATUS_SUCCESS. value contains the converted number
 *  Failure: STATUS_INVALID_PARAMETER, if base is not 0, 2, 8, 10 or 16.
 *           STATUS_ACCESS_VIOLATION, if value is NULL.
 *
 * NOTES
 *  For base 0 it uses 10 as base and the string should be in the format
 *      "{whitespace} [+|-] [0[x|o|b]] {digits}".
 *  For other bases the string should be in the format
 *      "{whitespace} [+|-] {digits}".
 *  No check is made for value overflow, only the lower 32 bits are assigned.
 *  If str is NULL it crashes, as the native function does.
 *
 * DIFFERENCES
 *  This function does not read garbage on string length 0 as the native
 *  version does.
 */
NTSTATUS WINAPI RtlUnicodeStringToInteger(
    const UNICODE_STRING *str, /* [I] Unicode string to be converted */
    ULONG base,                /* [I] Number base for conversion (allowed 0, 2, 8, 10 or 16) */
    ULONG *value)              /* [O] Destination for the converted value */
{
    LPWSTR lpwstr = str->Buffer;
    USHORT CharsRemaining = str->Length / sizeof(WCHAR);
    WCHAR wchCurrent;
    int digit;
    ULONG RunningTotal = 0;
    char bMinus = 0;

    while (CharsRemaining >= 1 && *lpwstr <= ' ') {
	lpwstr++;
	CharsRemaining--;
    } /* while */

    if (CharsRemaining >= 1) {
	if (*lpwstr == '+') {
	    lpwstr++;
	    CharsRemaining--;
	} else if (*lpwstr == '-') {
	    bMinus = 1;
	    lpwstr++;
	    CharsRemaining--;
	} /* if */
    } /* if */

    if (base == 0) {
	base = 10;
	if (CharsRemaining >= 2 && lpwstr[0] == '0') {
	    if (lpwstr[1] == 'b') {
		lpwstr += 2;
		CharsRemaining -= 2;
		base = 2;
	    } else if (lpwstr[1] == 'o') {
		lpwstr += 2;
		CharsRemaining -= 2;
		base = 8;
	    } else if (lpwstr[1] == 'x') {
		lpwstr += 2;
		CharsRemaining -= 2;
		base = 16;
	    } /* if */
	} /* if */
    } else if (base != 2 && base != 8 && base != 10 && base != 16) {
	return STATUS_INVALID_PARAMETER;
    } /* if */

    if (value == NULL) {
	return STATUS_ACCESS_VIOLATION;
    } /* if */

    while (CharsRemaining >= 1) {
	wchCurrent = *lpwstr;
	if (wchCurrent >= '0' && wchCurrent <= '9') {
	    digit = wchCurrent - '0';
	} else if (wchCurrent >= 'A' && wchCurrent <= 'Z') {
	    digit = wchCurrent - 'A' + 10;
	} else if (wchCurrent >= 'a' && wchCurrent <= 'z') {
	    digit = wchCurrent - 'a' + 10;
	} else {
	    digit = -1;
	} /* if */
	if (digit < 0 || digit >= base) {
	    *value = bMinus ? -RunningTotal : RunningTotal;
	    return STATUS_SUCCESS;
	} /* if */

	RunningTotal = RunningTotal * base + digit;
	lpwstr++;
	CharsRemaining--;
    } /* while */

    *value = bMinus ? -RunningTotal : RunningTotal;
    return STATUS_SUCCESS;
}

/**************************************************************************
 *	RtlIntegerToUnicodeString (NTDLL.@)
 *
 * Converts an unsigned integer to a '\0' terminated unicode string.
 *
 * RETURNS
 *  Success: STATUS_SUCCESS. str contains the converted number
 *  Failure: STATUS_INVALID_PARAMETER, if base is not 0, 2, 8, 10 or 16.
 *           STATUS_BUFFER_OVERFLOW, if str is too small to hold the string
 *                  (with the '\0' termination). In this case str->Length
 *                  is set to the length, the string would have (which can
 *                  be larger than the MaximumLength).
 *
 * NOTES
 *  Instead of base 0 it uses 10 as base.
 *  If str is NULL it crashes, as the native function does.
 *
 * DIFFERENCES
 *  Do not return STATUS_BUFFER_OVERFLOW when the string is long enough.
 *  The native function does this when the string would be longer than 16
 *  characters even when the string parameter is long enough.
 */
NTSTATUS WINAPI RtlIntegerToUnicodeString(
    ULONG value,         /* [I] Value to be converted */
    ULONG base,          /* [I] Number base for conversion (allowed 0, 2, 8, 10 or 16) */
    UNICODE_STRING *str) /* [O] Destination for the converted value */
{
    WCHAR buffer[33];
    PWCHAR pos;
    WCHAR digit;

    if (base == 0) {
	base = 10;
    } else if (base != 2 && base != 8 && base != 10 && base != 16) {
	return STATUS_INVALID_PARAMETER;
    } /* if */

    pos = &buffer[32];
    *pos = '\0';

    do {
	pos--;
	digit = value % base;
	value = value / base;
	if (digit < 10) {
	    *pos = '0' + digit;
	} else {
	    *pos = 'A' + digit - 10;
	} /* if */
    } while (value != 0L);

    str->Length = (&buffer[32] - pos) * sizeof(WCHAR);
    if (str->Length >= str->MaximumLength) {
	return STATUS_BUFFER_OVERFLOW;
    } else {
	memcpy(str->Buffer, pos, str->Length + sizeof(WCHAR));
    } /* if */
    return STATUS_SUCCESS;
}
