/* $Id: heap.h,v 1.1 1999-05-24 20:19:41 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 heap API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __HEAP_H__
#define __HEAP_H__

HANDLE WIN32API OS2HeapCreate(DWORD  flOptions,
                              DWORD  dwInitialSize,
                              DWORD  dwMaximumSize);

LPVOID WIN32API OS2HeapAlloc (HANDLE hHeap,
                              DWORD  dwFlags,
                              DWORD  dwBytes);

#ifndef HEAP_NO_SERIALIZE
  #define HEAP_NO_SERIALIZE 1
#endif

#ifndef HEAP_ZERO_MEMORY
  #define HEAP_ZERO_MEMORY  8
#endif


#endif
