/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 compatibility string functions for OS/2
 *
 * Copyright 1999 Patrick Haller
 */

#include <odin.h>

#include <wine\unicode.h>
//SvL: strcase -> case insensitive!
#define strncasecmp lstrncmpiA
#define strcasecmp  lstrcmpiA
#define strncmpiW       lstrncmpiW
#define _strlwr(a)  strlwr(a)


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern BOOL fIsDBCSEnv;

#define IsDBCSEnv()	fIsDBCSEnv

int    WIN32API lstrlenA       (LPCSTR arg1);
int    WIN32API lstrlenW       (LPCWSTR arg1);
LPSTR  WIN32API lstrcatA       (LPSTR arg1, LPCSTR arg2);
LPWSTR WIN32API lstrcatW       (LPWSTR arg1, LPCWSTR arg2);
int    WIN32API lstrcmpA       (LPCSTR arg1, LPCSTR  arg2);
int    WIN32API lstrcmpW       (LPCWSTR arg1, LPCWSTR arg2);
int    WIN32API lstrncmpA      (LPCSTR arg1, LPCSTR  arg2, int i);
int    WIN32API lstrncmpW      (LPCWSTR arg1, LPCWSTR arg2, int i);
int    WIN32API lstrncmpiA     (LPCSTR arg1, LPCSTR  arg2, int i);
int    WIN32API lstrncmpiW     (LPCWSTR arg1, LPCWSTR arg2, int i);
#define lstrcmpniW lstrncmpiW
LPSTR  WIN32API lstrcpyA       (LPSTR arg1, LPCSTR  arg2);
LPWSTR WIN32API lstrcpyW       (LPWSTR dest, LPCWSTR src);
LPSTR  WIN32API lstrcpynA      (LPSTR arg1, LPCSTR  arg2, int arg3);
LPWSTR WIN32API lstrcpynW      (LPWSTR dest, LPCWSTR src, int arg3);
int    WIN32API lstrcmpiA      (LPCSTR arg1, LPCSTR  arg2);
int    WIN32API lstrcmpiW      (LPCWSTR arg1, LPCWSTR arg2);
int    WIN32API lstrcpynAtoW   (LPWSTR unicode, LPCSTR ascii, int unilen);
int    WIN32API lstrcpynWtoA   (LPSTR ascii, LPCWSTR unicode, int asciilen);
LPSTR  WIN32API lstrcpyWtoA    (LPSTR ascii, LPCWSTR unicode);
LPWSTR WIN32API lstrcpyAtoW    (LPWSTR unicode, LPCSTR ascii);
int    WIN32API lstrlenWtoA    ( LPCWSTR ustring, int ulen );
int    WIN32API lstrlenAtoW    ( LPCSTR astring, int alen );
int    WIN32API lstrtrunc      ( LPSTR astring, int max );

LPVOID WIN32API HEAP_xalloc    ( HANDLE heap, DWORD flags, DWORD size );
LPVOID WIN32API HEAP_xrealloc  ( HANDLE heap, DWORD flags, LPVOID lpMem, DWORD size );
LPVOID WIN32API HEAP_malloc    ( DWORD size );
LPVOID WIN32API HEAP_realloc   ( LPVOID lpMem, DWORD size );
DWORD  WIN32API HEAP_size      ( LPVOID lpMem );
BOOL   WIN32API HEAP_free      ( LPVOID lpMem );

LPSTR  WIN32API HEAP_strdupA   ( HANDLE heap, DWORD flags, LPCSTR str );
LPWSTR WIN32API HEAP_strdupW   ( HANDLE heap, DWORD flags, LPCWSTR str );
LPWSTR WIN32API HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str );
LPSTR  WIN32API HEAP_strdupWtoA( HANDLE heap, DWORD flags, LPCWSTR str );

INT WIN32API WideCharToLocal(LPSTR pLocal, LPWSTR pWide, INT dwChars);
INT WIN32API LocalToWideChar(LPWSTR pWide, LPSTR pLocal, INT dwChars);

#ifdef __cplusplus
}
#endif

/*****************************************************************************
 * Special accelerator macros (avoid heap trashing)                          *
 *****************************************************************************/

#ifdef __WIN32OS2__

#ifndef CP_ACP
// from include/win/winnls.h
#define CP_ACP                                  0
#endif

#define STACK_strdupAtoW(strA,strW)                                      \
  if (!strA) strW = NULL;                                                \
  else                                                                   \
  {                                                                      \
    int len = MultiByteToWideChar( CP_ACP, 0, strA, -1, NULL, 0);        \
    strW = (LPWSTR)_alloca( len*sizeof(WCHAR) );                         \
    MultiByteToWideChar(CP_ACP, 0, strA, -1, strW, len);                 \
  }

#define STACK_strdupWtoA(strW,strA)                                      \
  if (!strW) strA = NULL;                                                \
  else                                                                   \
  {                                                                      \
      int len = WideCharToMultiByte( CP_ACP, 0, strW, -1, NULL, 0, 0, NULL);\
      strA = (LPSTR)_alloca(len);                                        \
      WideCharToMultiByte(CP_ACP, 0, strW, -1, strA, len, 0, NULL );     \
  }

#define STACK_strdupA(strDest, strSrc)                                   \
  {                                                                      \
    int iLength = lstrlenA(strSrc) + 1;                                  \
    strDest = (LPSTR)_alloca( iLength );                                 \
    memcpy( strDest, strSrc, iLength);                                   \
  }

#define STACK_strdupW(strDest, strSrc)                                   \
  {                                                                      \
    int iLength = lstrlenW(strSrc) + 1;                                  \
    strDest = (LPWSTR)_alloca( iLength * sizeof(WCHAR) );                \
    memcpy( strDest, strSrc, iLength * sizeof(WCHAR) );                  \
  }


#endif

