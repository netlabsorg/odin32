/* $Id */

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

LPVOID          HEAP_xalloc    ( HANDLE heap, DWORD flags, DWORD size );
LPVOID          HEAP_xrealloc  ( HANDLE heap, DWORD flags, LPVOID lpMem, DWORD size );
LPSTR           HEAP_strdupA   ( HANDLE heap, DWORD flags, LPCSTR str );
LPWSTR          HEAP_strdupW   ( HANDLE heap, DWORD flags, LPCWSTR str );
LPWSTR          HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str );
LPSTR           HEAP_strdupWtoA( HANDLE heap, DWORD flags, LPCWSTR str );

