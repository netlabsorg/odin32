/* $Id: os2heap.cpp,v 1.6 1999-08-09 22:10:08 phaller Exp $ */

/*
 * Heap class for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <builtin.h>
#include <stdlib.h>
#include <string.h>
#include <odincrt.h>

#include "win32type.h"
#include "os2heap.h"
#include "misc.h"
#include "vmutex.h"

#ifndef HEAP_NO_SERIALIZE
  #define HEAP_NO_SERIALIZE 1
#endif

#ifndef HEAP_ZERO_MEMORY
  #define HEAP_ZERO_MEMORY  8
#endif

VMutex heaplistmutex;   //protects linked lists of heaps

//******************************************************************************
//******************************************************************************
OS2Heap::OS2Heap(HANDLE hHeap, DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize)
{
 OS2Heap *curheap = OS2Heap::heap;

  hPrimaryHeap = hHeap;
  totalAlloc   = 0;
  fInitialized = 0;
  nrHeaps      = 0;
  heapelem     = NULL;

  this->dwMaximumSize = dwMaximumSize;
  this->dwInitialSize = dwInitialSize;
  this->flOptions     = flOptions;

  dprintf(("KERNEL32:  HeapCreate: initial size %d, max size %d (flags %X) returned %X\n", dwInitialSize, dwMaximumSize, flOptions, hPrimaryHeap));

  if(!(flOptions & HEAP_NO_SERIALIZE))
  {
    ODIN_FS_BEGIN
    hmutex = new VMutex();
    ODIN_FS_END
    dassert(hmutex, ("ERROR: new VMutex\n"));
  }
  else  hmutex = NULL;

  heaplistmutex.enter();
  if(curheap != NULL) {
        while(curheap->next != NULL) {
                curheap = curheap->next;
        }
        curheap->next = this;
  }
  else  heap = this;
  next = NULL;

  heaplistmutex.leave();
}
//******************************************************************************
//******************************************************************************
OS2Heap::~OS2Heap()
{
 OS2Heap *curheap = OS2Heap::heap;
 HEAPELEM *hnext;
 int i;

  dprintf(("dtr OS2Heap, hPrimaryHeap = %X\n", hPrimaryHeap));

  if(hmutex)
    hmutex->enter();

  while(heapelem) {
    hnext = heapelem->next;
    free(heapelem);
    heapelem = hnext;
  }
  if(hmutex)
  {
    hmutex->leave();
    ODIN_delete(hmutex);
  }

  heaplistmutex.enter();
  if(heap == this) {
        heap = next;
  }
  else {
        while(curheap->next != NULL) {
                if(curheap->next == this) {
                        curheap->next = next;
                        break;
                }
                curheap = curheap->next;
        }
  }
  heaplistmutex.leave();
  dprintf(("dtr OS2Heap, hPrimaryHeap = %X done\n", hPrimaryHeap));
}
//******************************************************************************
//******************************************************************************
LPVOID OS2Heap::Alloc(DWORD dwFlags, DWORD dwBytes)
{
 LPVOID lpMem;

//  dprintf(("OS2Heap::Alloc\n"));
  lpMem = malloc(dwBytes + HEAP_OVERHEAD);
  if(lpMem == NULL) {
    dprintf(("OS2Heap::Alloc, lpMem == NULL"));
    return(NULL);
  }
  if(dwFlags & HEAP_ZERO_MEMORY) {
    memset((char *)lpMem, 0, dwBytes+HEAP_OVERHEAD);
  }
  totalAlloc += dwBytes;

  if(hmutex)
    hmutex->enter();

  if(heapelem) {
   HEAPELEM *hnext;

    hnext = heapelem;

    heapelem       = (HEAPELEM *)lpMem;
    hnext->prev    = heapelem;
    heapelem->next = hnext;
  }
  else {
    heapelem = (HEAPELEM *)lpMem;
    heapelem->next = NULL;
  }
  heapelem->prev    = NULL;
  heapelem->flags   = 0;    //only used when allocated with LocalAlloc
  heapelem->lockCnt = 0;    //..    ..

  if(hmutex) {
    hmutex->leave();
  }
  return(LPVOID)((char *)lpMem+sizeof(HEAPELEM));
}
//******************************************************************************
//******************************************************************************
LPVOID OS2Heap::Alloc(DWORD dwFlags, DWORD dwBytes, DWORD LocalAllocFlags)
{
 HEAPELEM *helem;
 LPVOID    lpMem = Alloc(dwFlags, dwBytes);

  if(lpMem == NULL)
    return(NULL);

  helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));
  helem->flags   = LocalAllocFlags;
  return(lpMem);
}
//******************************************************************************
//******************************************************************************
BOOL OS2Heap::Lock(LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if(lpMem == NULL)
    return(FALSE);

  helem->lockCnt++;

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL OS2Heap::Unlock(LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if(lpMem == NULL)
    return(FALSE);

  if(helem->lockCnt == 0)
    return(FALSE);

  helem->lockCnt--;

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD OS2Heap::GetFlags(LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if(lpMem == NULL)
    return(FALSE);

  return(helem->flags);
}
//******************************************************************************
//******************************************************************************
int OS2Heap::GetLockCnt(LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if(lpMem == NULL)
    return(666);

  return(helem->lockCnt);
}
//******************************************************************************
//******************************************************************************
DWORD OS2Heap::Size(DWORD dwFlags, PVOID lpMem)
{
//  dprintf(("OS2Heap::Size, %X\n", lpMem));
  if(lpMem == NULL)
    return(0);

  return(_msize((char *)lpMem - sizeof(HEAPELEM)) - HEAP_OVERHEAD);
}
//******************************************************************************
//******************************************************************************
LPVOID OS2Heap::ReAlloc(DWORD dwFlags, LPVOID lpMem, DWORD dwBytes)
{
  LPVOID lpNewMem;
  int    i;

  if (dwBytes == 0) return NULL;         // intercept stupid parameters
  if (lpMem == 0)   return NULL;

  if (Size(0,lpMem) == dwBytes) return lpMem; // if reallocation with same size
                                                // don't do anything

//  dprintf(("OS2Heap::ReAlloc %X %X %d\n", dwFlags, lpMem, dwBytes));
  lpNewMem = Alloc(dwFlags, dwBytes);
  memcpy(lpNewMem, lpMem, Size(0, lpMem));
  Free(0, lpMem);

  dassert(lpNewMem, ("OS2Heap::ReAlloc, no more memory left\n"));

  return(lpNewMem);
}
//******************************************************************************
//******************************************************************************
BOOL OS2Heap::Free(DWORD dwFlags, LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if(lpMem == NULL) {
    dprintf(("OS2Heap::Free lpMem == NULL\n"));
    return(FALSE);
  }
#ifdef DEBUG1
  int size = Size(0, lpMem);
  dprintf(("OS2Heap::Free lpMem = %X, size %d\n", lpMem, size));
  totalAlloc -= size;
#endif
  if(hmutex)
    hmutex->enter();

  if(helem->prev)
    helem->prev->next = helem->next;
  if(helem->next)
    helem->next->prev = helem->prev;
  if(heapelem == helem)
    heapelem = heapelem->next;

  if(hmutex) {
    hmutex->leave();
  }

  free((void *)helem);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD OS2Heap::Compact(DWORD dwFlags)
{
  dprintf(("OS2Heap::Compact, %X- stub\n", dwFlags));
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL OS2Heap::Validate(DWORD dwFlags, LPCVOID lpMem)
{
  dprintf(("OS2Heap::Validate, %X %X - stub? (TRUE)\n", dwFlags, lpMem));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL OS2Heap::Walk(void *lpEntry)
{
  dprintf(("OS2Heap::Walk, %X - stub? (TRUE)\n", lpEntry));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
OS2Heap *OS2Heap::find(HANDLE hHeap)
{
  OS2Heap *curheap = OS2Heap::heap;

  while(curheap != NULL) {
        if(curheap->hPrimaryHeap == hHeap) {
                return(curheap);
        }
        curheap = curheap->next;
  }
  dprintf(("Heap %X not found!\n", hHeap));
  return(NULL);
}
//******************************************************************************
//******************************************************************************
OS2Heap *OS2Heap::heap = NULL;
