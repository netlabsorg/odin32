/* $Id: os2heap.cpp,v 1.17 2001-04-27 17:35:41 sandervl Exp $ */

/*
 * Heap class for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * NOTE: ReAlloc allocates memory using Alloc if memory pointer == NULL
 *       WINE controls depend on this, even though it apparently
 *       doesn't work like this in Windows.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>

#include "win32type.h"
#include "os2heap.h"
#include "misc.h"
#include "vmutex.h"
#include "initterm.h"
#include <odin32validate.h>

#define DBG_LOCALLOG	DBG_os2heap
#include "dbglocal.h"

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
    hmutex = new VMutex();
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
    delete(hmutex);
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
  heapelem->magic   = MAGIC_NR_HEAP;

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

  if((ULONG)lpMem > ADDRESS_SPACE_LIMIT) {
	//SvL: Some apps lock and unlock gdi handles; just ignore this here
	dprintf(("Lock: invalid address %x", lpMem));
	return FALSE;
  }

  if(lpMem == NULL)
    	return(FALSE);

  if(helem->magic != MAGIC_NR_HEAP)
  {
    	dprintf(("OS2Heap::Lock ERROR BAD HEAP POINTER:%X\n", lpMem));
    	return FALSE;
  }

  helem->lockCnt++;

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL OS2Heap::Unlock(LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if((ULONG)lpMem > ADDRESS_SPACE_LIMIT) {
	//SvL: Some apps lock and unlock gdi handles; just ignore this here
	dprintf(("Unlock: invalid address %x", lpMem));
	return FALSE;
  }

  if(lpMem == NULL)
    return(FALSE);

  if(helem->lockCnt == 0)
    return(FALSE);

  if(helem->magic != MAGIC_NR_HEAP)
  {
    dprintf(("OS2Heap::UnLock ERROR BAD HEAP POINTER:%X\n", lpMem));
    return FALSE;
  }

  helem->lockCnt--;

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD OS2Heap::GetFlags(LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if(lpMem == NULL)
    return(0);

  if(helem->magic != MAGIC_NR_HEAP)
  {
    dprintf(("OS2Heap::GetFlags ERROR BAD HEAP POINTER:%X\n", lpMem));
    return 0;
  }

  return(helem->lockCnt | (helem->flags << 8));
}
//******************************************************************************
//******************************************************************************
int OS2Heap::GetLockCnt(LPVOID lpMem)
{
 HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));

  if((ULONG)lpMem > ADDRESS_SPACE_LIMIT) {
	//SvL: Some apps lock and unlock gdi handles; just ignore this here
	dprintf(("GetLockCnt: invalid address %x", lpMem));
	return FALSE;
  }

  if(lpMem == NULL)
    	return(0);

  if(helem->magic != MAGIC_NR_HEAP)
  {
    	dprintf(("OS2Heap::GetLockCnt ERROR BAD HEAP POINTER:%X\n", lpMem));
    	return 0;
  }

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
  HEAPELEM *helem = (HEAPELEM *)((char *)lpMem - sizeof(HEAPELEM));
  LPVOID lpNewMem;
  int    i, oldSize;

  if (dwBytes == 0) return NULL;         // intercept stupid parameters

  //NOTE: Allocate memory using Alloc -> WINE controls depend on this, even
  //      though it apparently doesn't work in Windows.
  if (lpMem == 0)   return Alloc(dwFlags, dwBytes);
//  if (lpMem == 0)   return NULL;

  if (helem->magic != MAGIC_NR_HEAP)
  {
    dprintf(("OS2Heap::ReAlloc ERROR BAD HEAP POINTER:%X\n", lpMem));
    return lpMem;
  }

  oldSize = Size(0,lpMem);
  if (oldSize >= dwBytes) {
       dprintf(("ReAlloc with smaller size than original (%d); return old pointer", oldSize));
       return lpMem; // if reallocation with same size don't do anything
  }
  lpNewMem = Alloc(dwFlags, dwBytes);
  memcpy(lpNewMem, lpMem, dwBytes < oldSize ? dwBytes : oldSize);
  Free(0, lpMem);

  if(lpNewMem == NULL)
  {
     dprintf(("OS2Heap::ReAlloc, no more memory left\n"));
  }

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
  /* verify lpMem address */
  if (lpMem >= (LPVOID)ulMaxAddr || lpMem < (LPVOID)0x10000)
  {
    	dprintf(("OS2Heap::Free ERROR BAD HEAP POINTER:%X\n", lpMem));
    	return FALSE;
  }

  if(helem->magic != MAGIC_NR_HEAP)
  {
    	dprintf(("OS2Heap::Free ERROR BAD HEAP POINTER:%X\n", lpMem));
    	return FALSE;
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

  //@@@PH NT programs seem to assume heap 0 is always valid?!
  if (hHeap == 0)
     if (curheap != NULL)
       return curheap;

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
