/* $Id: heapstring.h,v 1.4 1999-09-02 17:12:46 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 compatibility string functions for OS/2
 *
 * Copyright 1999 Patrick Haller
 */


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

int    WIN32API lstrlenA       (LPCSTR arg1);
int    WIN32API lstrlenW       (LPCWSTR arg1);
LPSTR  WIN32API lstrcatA       (LPSTR arg1, LPCSTR arg2);
LPWSTR WIN32API lstrcatW       (LPWSTR arg1, LPCWSTR arg2);
int    WIN32API lstrcmpA       (LPCSTR arg1, LPCSTR  arg2);
int    WIN32API lstrcmpW       (LPCWSTR arg1, LPCWSTR arg2);
int    WIN32API lstrncmpA      (LPCSTR arg1, LPCSTR  arg2, int i);
int    WIN32API lstrncmpW      (LPCWSTR arg1, LPCWSTR arg2, int i);
LPSTR  WIN32API lstrcpyA       (LPSTR arg1, LPCSTR  arg2);
LPWSTR WIN32API lstrcpyW       (LPWSTR dest, LPCWSTR src);
LPSTR  WIN32API lstrcpynA      (LPSTR arg1, LPCSTR  arg2, int arg3);
LPWSTR WIN32API lstrcpynW      (LPWSTR dest, LPCWSTR src, int arg3);
int    WIN32API lstrcmpiA      (LPCSTR arg1, LPCSTR  arg2);
int    WIN32API lstrcmpiW      (LPCWSTR arg1, LPCWSTR arg2);
int    WIN32API lstrcpynAtoW   (LPWSTR unicode, LPSTR ascii, int asciilen);
int    WIN32API lstrcpynWtoA   (LPSTR ascii, LPWSTR unicode, int unilen);
LPSTR  WIN32API lstrcpyWtoA    (LPSTR ascii, LPWSTR unicode);
LPWSTR WIN32API lstrcpyAtoW    (LPWSTR unicode, LPSTR ascii);

LPVOID WIN32API HEAP_xalloc    ( HANDLE heap, DWORD flags, DWORD size );
LPVOID WIN32API HEAP_xrealloc  ( HANDLE heap, DWORD flags, LPVOID lpMem, DWORD size );
LPVOID WIN32API HEAP_malloc    ( DWORD size );
LPVOID WIN32API HEAP_realloc   ( LPVOID lpMem, DWORD size );
VOID   WIN32API HEAP_free      ( LPVOID lpMem );

LPSTR  WIN32API HEAP_strdupA   ( HANDLE heap, DWORD flags, LPCSTR str );
LPWSTR WIN32API HEAP_strdupW   ( HANDLE heap, DWORD flags, LPCWSTR str );
LPWSTR WIN32API HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str );
LPSTR  WIN32API HEAP_strdupWtoA( HANDLE heap, DWORD flags, LPCWSTR str );

INT WIN32API WideCharToLocal(LPSTR pLocal, LPWSTR pWide, INT dwChars);
INT WIN32API LocalToWideChar(LPWSTR pWide, LPSTR pLocal, INT dwChars);

