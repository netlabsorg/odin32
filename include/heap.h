/* $Id: heap.h,v 1.2 1999-12-18 19:30:46 achimha Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 heap API functions for OS/2
 * Copyright 1998 Sander van Leeuwen
 */
#ifndef __HEAP_H__
#define __HEAP_H__


#include <winbase.h>

#ifndef HEAP_NO_SERIALIZE
  #define HEAP_NO_SERIALIZE 1
#endif

#ifndef HEAP_ZERO_MEMORY
  #define HEAP_ZERO_MEMORY  8
#endif


//extern HANDLE SystemHeap;
//extern HANDLE SegptrHeap;
static HANDLE SegptrHeap;

//extern int    HEAP_IsInsideHeap( HANDLE heap, DWORD flags, LPCVOID ptr );
//extern SEGPTR HEAP_GetSegptr   ( HANDLE heap, DWORD flags, LPCVOID ptr );

//extern LPVOID HEAP_xalloc      ( HANDLE heap, DWORD flags, DWORD size );
//extern LPVOID HEAP_xrealloc    ( HANDLE heap, DWORD flags, LPVOID lpMem, DWORD size );
extern LPSTR WIN32API HEAP_strdupA     ( HANDLE heap, DWORD flags, LPCSTR str );
//extern LPWSTR HEAP_strdupW     ( HANDLE heap, DWORD flags, LPCWSTR str );
//extern LPWSTR HEAP_strdupAtoW  ( HANDLE heap, DWORD flags, LPCSTR str );
//extern LPSTR  HEAP_strdupWtoA  ( HANDLE heap, DWORD flags, LPCWSTR str );

/* SEGPTR helper macros */

#define SEGPTR_ALLOC(size) \
         (HeapAlloc( SegptrHeap, 0, (size) ))
#define SEGPTR_NEW(type) \
         ((type *)HeapAlloc( SegptrHeap, 0, sizeof(type) ))
#define SEGPTR_STRDUP(str) \
         (HIWORD(str) ? HEAP_strdupA( SegptrHeap, 0, (str) ) : (LPSTR)(str))
#define SEGPTR_STRDUP_WtoA(str) \
         (HIWORD(str) ? HEAP_strdupWtoA( SegptrHeap, 0, (str) ) : (LPSTR)(str))

#if 0
/* define an inline function, a macro won't do */
static SEGPTR SEGPTR_Get(LPCVOID ptr)
{
  return (HIWORD(ptr) ? HEAP_GetSegptr( SegptrHeap, 0, ptr ) : (SEGPTR)ptr);
}

#define SEGPTR_GET(ptr) SEGPTR_Get(ptr)
#define SEGPTR_FREE(ptr) \
         (HIWORD(ptr) ? HeapFree( SegptrHeap, 0, (ptr) ) : 0)
#endif


LPVOID  WIN32API HeapAlloc     (HANDLE hHeap, DWORD dwFlags, DWORD dwBytes);
LPVOID  WIN32API HeapReAlloc   (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, DWORD dwBytes);
BOOL    WIN32API HeapFree      (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
HANDLE  WIN32API HeapCreate    (DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize);
BOOL    WIN32API HeapDestroy   (HANDLE hHeap);
DWORD   WIN32API HeapSize      (HANDLE hHeap, DWORD arg2, PVOID  arg3);
DWORD   WIN32API HeapCompact   (HANDLE hHeap, DWORD dwFlags);
BOOL    WIN32API HeapValidate  (HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
BOOL    WIN32API HeapUnlock    (HANDLE hHeap);
BOOL    WIN32API HeapLock      (HANDLE hHeap);
BOOL    WIN32API HeapWalk      (HANDLE hHeap, void *lpEntry);
HANDLE  WIN32API GetProcessHeap(VOID);
HLOCAL  WIN32API LocalAlloc    (UINT fuFlags, DWORD cbBytes);
HLOCAL  WIN32API LocalDiscard  (HLOCAL hMem);
UINT    WIN32API LocalFlags    (HLOCAL hMem);
HLOCAL  WIN32API LocalFree     (HLOCAL hMem);
HLOCAL  WIN32API LocalHandle   (PCVOID lpMem);
BOOL    WIN32API LocalUnlock   (HLOCAL hMem);
HLOCAL  WIN32API LocalReAlloc  (HLOCAL hMem, DWORD cbBytes, UINT fuFlags);
UINT    WIN32API LocalSize     (HLOCAL hMem);
PVOID   WIN32API LocalLock     (HLOCAL hMem);
UINT    WIN32API LocalShrink   (HLOCAL hMem, UINT   cbNewSize);
UINT    WIN32API LocalCompact  (UINT cbNewSize);
HGLOBAL WIN32API GlobalAlloc   (UINT fuFlags, DWORD dwBytes);
HGLOBAL WIN32API GlobalFree    (HGLOBAL arg1);
HGLOBAL WIN32API GlobalHandle  (LPCVOID arg1);
UINT    WIN32API GlobalFlags   (HGLOBAL arg1);
DWORD   WIN32API GlobalCompact (DWORD dwMinFree);
PVOID   WIN32API GlobalLock    (HGLOBAL arg1);
HGLOBAL WIN32API GlobalReAlloc (HGLOBAL arg1, DWORD arg2, UINT  arg3);
DWORD   WIN32API GlobalSize    (HGLOBAL arg1);
BOOL    WIN32API GlobalUnlock  (HGLOBAL arg1);


#endif
