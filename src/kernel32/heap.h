/* $Id: heap.h,v 1.4 1999-08-04 00:46:14 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 heap API functions for OS/2
 * Copyright 1998 Sander van Leeuwen
 */
#ifndef __HEAP_H__
#define __HEAP_H__

#ifndef HEAP_NO_SERIALIZE
  #define HEAP_NO_SERIALIZE 1
#endif

#ifndef HEAP_ZERO_MEMORY
  #define HEAP_ZERO_MEMORY  8
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
