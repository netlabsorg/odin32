/* $Id: os2heap.h,v 1.1 1999-09-15 23:32:59 sandervl Exp $ */

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

typedef struct _tagHEAPELEM {
  struct _tagHEAPELEM *prev;
  struct _tagHEAPELEM *next;
  DWORD  flags;     //set by LocalAlloc only
  DWORD  lockCnt;   //LocalLock/Unlock
} HEAPELEM;

#define HEAP_OVERHEAD (sizeof(HEAPELEM)+60)
//#define HEAP_OVERHEAD sizeof(HEAPELEM)

class OS2Heap
{
public:
    OS2Heap(HANDLE hHeap, DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize);
    ~OS2Heap();

    HANDLE getHeapHandle()  { return(hPrimaryHeap); };

    LPVOID Alloc(DWORD dwFlags, DWORD dwBytes);
    LPVOID ReAlloc(DWORD dwFlags, LPVOID lpMem, DWORD dwBytes);
    BOOL   Free(DWORD dwFlags, LPVOID lpMem);
    DWORD  Size(DWORD dwFlags, PVOID lpMem);
    DWORD  Compact(DWORD dwFlags);
    BOOL   Validate(DWORD dwFlags, LPCVOID lpMem);
    BOOL   Walk(void *lpEntry);

    LPVOID Alloc(DWORD dwFlags, DWORD dwBytes, DWORD LocalAllocFlags);
    BOOL   Lock(LPVOID lpMem);
    BOOL   Unlock(LPVOID lpMem);
    int    GetLockCnt(LPVOID lpMem);
    DWORD  GetFlags(LPVOID lpMem);

   static OS2Heap *find(HANDLE hHeap);

private:
protected:

    VMutex *hmutex;

    DWORD     dwMaximumSize, dwInitialSize, flOptions, totalAlloc, nrHeaps;
    HANDLE    hPrimaryHeap;
    BOOL      fInitialized;
    HEAPELEM *heapelem;

          OS2Heap *next;
   static OS2Heap *heap;
};

#endif
