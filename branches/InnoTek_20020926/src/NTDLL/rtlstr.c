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
    target->Length = len - 2;
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
 *	RtlUpperString   (NTDLL.@)
 */
void WINAPI RtlUpperString( STRING *dst, const STRING *src )
{
    unsigned int i, len = min(src->Length, dst->MaximumLength);

    for (i = 0; i < len; i++) dst->Buffer[i] = toupper(src->Buffer[i]);
    dst->Length = len;
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
