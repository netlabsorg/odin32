/* $Id: os2heap.h,v 1.11 2002-01-06 16:48:47 sandervl Exp $ */

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
//must be a multiple of 8 bytes (alignment)!!
typedef struct _tagHEAPELEM {
  DWORD  magic;     //magic number
  LPVOID lpMem;     //pointer returned by malloc
  DWORD  cursize;   //current size (HeapReAlloc changes this)
  DWORD  orgsize;   //size used to allocate this memory block
} HEAPELEM;

#define HEAP_ALIGNMENT		8
#define HEAP_ALIGNMENT_MASK	(~7)

#define HEAP_OVERHEAD (sizeof(HEAPELEM))

#define HEAP_ALIGN(a)     (((DWORD)a+HEAP_ALIGNMENT-1) & HEAP_ALIGNMENT_MASK)

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

    DWORD     dwMaximumSize, dwInitialSize, flOptions, nrHeaps;
#ifdef DEBUG
    DWORD totalAlloc;
#endif
    HANDLE    hPrimaryHeap;
    BOOL      fInitialized;
    HEAPELEM *heapelem;

    char     *pInitialHeapMem;
    Heap_t    uheap;

          OS2Heap *next;
   static OS2Heap *heap;
};

#endif
