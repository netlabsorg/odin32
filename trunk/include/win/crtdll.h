/* $Id: crtdll.h,v 1.4 2000-02-03 21:41:11 sandervl Exp $ */

#ifndef __WINE_CRTDLL_H
#define __WINE_CRTDLL_H

#define CRTDLL_LC_ALL		0
#define CRTDLL_LC_COLLATE	1
#define CRTDLL_LC_CTYPE		2
#define CRTDLL_LC_MONETARY	3
#define CRTDLL_LC_NUMERIC	4
#define CRTDLL_LC_TIME		5
#define CRTDLL_LC_MIN		LC_ALL
#define CRTDLL_LC_MAX		LC_TIME

/* ctype defines */
#define CRTDLL_UPPER		0x1
#define CRTDLL_LOWER		0x2
#define CRTDLL_DIGIT		0x4
#define CRTDLL_SPACE		0x8
#define CRTDLL_PUNCT		0x10
#define CRTDLL_CONTROL		0x20
#define CRTDLL_BLANK		0x40
#define CRTDLL_HEX		0x80
#define CRTDLL_LEADBYTE		0x8000
#define CRTDLL_ALPHA		(0x0100|CRTDLL_UPPER|CRTDLL_LOWER)

/* function prototypes used in crtdll.c */
extern int LastErrorToErrno(DWORD);

void * __cdecl CRTDLL_malloc( DWORD size );
void   __cdecl CRTDLL_free( void *ptr );

LPSTR  __cdecl CRTDLL__mbsinc( LPCSTR str );
INT    __cdecl CRTDLL__mbslen( LPCSTR str );
LPWSTR __cdecl CRTDLL__wcsdup( LPCWSTR str );
INT    __cdecl CRTDLL__wcsicmp( LPCWSTR str1, LPCWSTR str2 );
INT    __cdecl CRTDLL__wcsicoll( LPCWSTR str1, LPCWSTR str2 );
LPWSTR __cdecl CRTDLL__wcslwr( LPWSTR str );
INT    __cdecl CRTDLL__wcsnicmp( LPCWSTR str1, LPCWSTR str2, INT n );
LPWSTR __cdecl CRTDLL__wcsnset( LPWSTR str, WCHAR c, INT n );
LPWSTR __cdecl CRTDLL__wcsrev( LPWSTR str );
LPWSTR __cdecl CRTDLL__wcsset( LPWSTR str, WCHAR c );
LPWSTR __cdecl CRTDLL__wcsupr( LPWSTR str );
INT    __cdecl CRTDLL_mbstowcs( LPWSTR dst, LPCSTR src, INT n );
INT    __cdecl CRTDLL_mbtowc( WCHAR *dst, LPCSTR str, INT n );
WCHAR  __cdecl CRTDLL_towlower( WCHAR ch );
WCHAR  __cdecl CRTDLL_towupper( WCHAR ch );
LPWSTR __cdecl CRTDLL_wcscat( LPWSTR dst, LPCWSTR src );
LPWSTR __cdecl CRTDLL_wcschr( LPCWSTR str, WCHAR ch );
INT    __cdecl CRTDLL_wcscmp( LPCWSTR str1, LPCWSTR str2 );
DWORD  __cdecl CRTDLL_wcscoll( LPCWSTR str1, LPCWSTR str2 );
LPWSTR __cdecl CRTDLL_wcscpy( LPWSTR dst, LPCWSTR src );
INT    __cdecl CRTDLL_wcscspn( LPCWSTR str, LPCWSTR reject );
INT    __cdecl CRTDLL_wcslen( LPCWSTR str );
LPWSTR __cdecl CRTDLL_wcsncat( LPWSTR s1, LPCWSTR s2, INT n );
INT    __cdecl CRTDLL_wcsncmp( LPCWSTR str1, LPCWSTR str2, INT n );
LPWSTR __cdecl CRTDLL_wcsncpy( LPWSTR s1, LPCWSTR s2, INT n );
LPWSTR __cdecl CRTDLL_wcspbrk( LPCWSTR str, LPCWSTR accept );
LPWSTR __cdecl CRTDLL_wcsrchr( LPWSTR str, WCHAR ch );
INT    __cdecl CRTDLL_wcsspn( LPCWSTR str, LPCWSTR accept );
LPWSTR __cdecl CRTDLL_wcsstr( LPCWSTR str, LPCWSTR sub );
LPWSTR __cdecl CRTDLL_wcstok( LPWSTR str, LPCWSTR delim );
INT    __cdecl CRTDLL_wcstombs( LPSTR dst, LPCWSTR src, INT n );
INT    __cdecl CRTDLL_wctomb( LPSTR dst, WCHAR ch );

#endif /* __WINE_CRTDLL_H */
