/* $Id: os2heap.h,v 1.7 2001-07-28 18:03:38 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Heap class for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __OS2HEAP_H__
#define __OS2HEAP_H__

#include "vmutex.h"
#include <umalloc.h>

#define MAGIC_NR_HEAP  0x87654321
//must be a multiple of 8 bytes (alignment)
typedef struct _tagHEAPELEM {
  DWORD  magic;     //magic number
  LPVOID lpMem;     //pointer returned by malloc
  struct _tagHEAPELEM *prev;
  struct _tagHEAPELEM *next;
} HEAPELEM;

//extra overhead needed for quake 2 or 3 (a long time ago...)
//#define HEAP_OVERHEAD (sizeof(HEAPELEM)+60)
//+8 to make sure we can align the pointer at 8 byte boundary
#define HEAP_OVERHEAD (sizeof(HEAPELEM)+8)

#define GET_HEAPOBJ(ptr) (HEAPELEM *)((char *)ptr - sizeof(HEAPELEM));

class OS2Heap
{
public:
    OS2Heap(DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize);
    ~OS2Heap();

    HANDLE getHeapHandle()  { return(hPrimaryHeap); };

    LPVOID Alloc(DWORD dwFlags, DWORD dwBytes);
    LPVOID ReAlloc(DWORD dwFlags, LPVOID lpMem, DWORD dwBytes);
    BOOL   Free(DWORD dwFlags, LPVOID lpMem);
    DWORD  Size(DWORD dwFlags, PVOID lpMem);
    DWORD  Compact(DWORD dwFlags);
    BOOL   Validate(DWORD dwFlags, LPCVOID lpMem);
    BOOL   Walk(void *lpEntry);

   static OS2Heap *find(HANDLE hHeap);

private:
protected:

    VMutex *hmutex;

    DWORD     dwMaximumSize, dwInitialSize, flOptions, totalAlloc, nrHeaps;
    HANDLE    hPrimaryHeap;
    BOOL      fInitialized;
    HEAPELEM *heapelem;

    char     *pInitialHeapMem;
    Heap_t    uheap;

          OS2Heap *next;
   static OS2Heap *heap;
};

#endif
