/* $Id: os2heap.cpp,v 1.33 2002-07-15 14:28:51 sandervl Exp $ */

/*
 * Heap class for OS/2
 *
 * Copyright 1998-2002 Sander van Leeuwen <sandervl@xs4all.nl>
 *
 *
 * NOTE: Do NOT use high memory here. Risky with 16 bits tcpip stack
 *       If this is ever changed, then you must use VirtualAlloc!
 * 
 * NOTE: ReAlloc allocates memory using Alloc if memory pointer == NULL
 *       WINE controls depend on this, even though it apparently
 *       doesn't work like this in Windows.
 * NOTE: Round up size to next 8 bytes boundary
 *       When reallocating memory block, don't use different
 *       memory block unless new size is larger than old size
 *       (rounded up to next 8 bytes boundary)
 *       (Verified this behaviour in NT4 (Global/Heap(Re)Alloc); 
 *        fixes crashes in Opera 5.12 which relies on this 'feature')
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
#include <umalloc.h>

#include "win32type.h"
#include "os2heap.h"
#include "misc.h"
#include "vmutex.h"
#include "initterm.h"
#include <odin32validate.h>

#define DBG_LOCALLOG	DBG_os2heap
#include "dbglocal.h"

#include <_ras.h>

#ifdef RAS
RAS_TRACK_HANDLE rthHeap = 0;
#endif

#ifndef HEAP_NO_SERIALIZE
  #define HEAP_NO_SERIALIZE 1
#endif

#ifndef HEAP_ZERO_MEMORY
  #define HEAP_ZERO_MEMORY  8
#endif

void * _LNK_CONV getmoreHeapMem(Heap_t pHeap, size_t *size, int *clean);
void   _LNK_CONV releaseHeapMem(Heap_t pHeap, void *block, size_t size);

VMutex heaplistmutex;   //protects linked lists of heaps


//******************************************************************************
// Fast Heap Handle Management
//******************************************************************************

HANDLE   fhhm_lastHandle = 0;
OS2Heap* fhhm_lastHeap   = NULL;


//******************************************************************************
//******************************************************************************
OS2Heap::OS2Heap(DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize)
{
    OS2Heap *curheap = OS2Heap::heap;
  
#ifdef DEBUG
    totalAlloc   = 0;
#endif
    fInitialized = 0;
    nrHeaps      = 0;
    heapelem     = NULL;

    /* round the size up to a multiple of 64K */
    //NOTE: MUST use 64kb here or else we are at risk of running out of virtual
    //      memory space. (when allocating 4kb we actually get 4kb + 60k uncommited)
    dwInitialSize       = ( (dwInitialSize / 65536) + 1) * 65536;

    this->dwMaximumSize = dwMaximumSize;
    this->dwInitialSize = dwInitialSize;
    this->flOptions     = flOptions;

    heaplistmutex.enter();
    if(curheap != NULL) {
        while(curheap->next != NULL) {
                curheap = curheap->next;
        }
        curheap->next = this;
    }
    else  heap = this;
    next = NULL;

#ifdef RAS
    if (!rthHeap)
    {
        RasRegisterObjectTracking (&rthHeap, "Heap*, Global* and Local* memory",
                                   0, RAS_TRACK_FLAG_MEMORY | RAS_TRACK_FLAG_LOG_AT_EXIT, 
                                   NULL, NULL);
    }
#endif

    heaplistmutex.leave();

    APIRET rc;

    rc = DosAllocMem((PPVOID)&pInitialHeapMem, dwInitialSize, PAG_READ|PAG_WRITE|PAG_COMMIT);
    if(rc != 0) {
	    dprintf(("OS2Heap::OS2Heap: DosAllocSharedMem failed with %d", rc));
        DebugInt3();
    }
    uheap = _ucreate(pInitialHeapMem, dwInitialSize, _BLOCK_CLEAN,
                     _HEAP_REGULAR, getmoreHeapMem, releaseHeapMem);
    if(uheap == NULL) {
	    DosFreeMem(pInitialHeapMem);
        pInitialHeapMem = NULL;
	    dprintf(("OS2Heap::OS2Heap: _ucreate failed!"));
        DebugInt3();
    }
    hPrimaryHeap = (HANDLE)uheap;
    dprintf(("KERNEL32:  HeapCreate: initial size %d, max size %d (flags %X) returned %X\n", dwInitialSize, dwMaximumSize, flOptions, hPrimaryHeap));
}
//******************************************************************************
//******************************************************************************
OS2Heap::~OS2Heap()
{
    OS2Heap *curheap = OS2Heap::heap;
    int i;
  
    // invalidate handle cache
    fhhm_lastHandle = 0;
    fhhm_lastHeap   = NULL;
  
    dprintf(("dtr OS2Heap, hPrimaryHeap = %X\n", hPrimaryHeap));

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

    if(uheap) {
    	_uclose(uheap);
    	_udestroy(uheap, _FORCE);
    	uheap = NULL;
    }     
    if(pInitialHeapMem) {
    	DosFreeMem(pInitialHeapMem);
    	pInitialHeapMem = NULL;
    }

    dprintf(("dtr OS2Heap, hPrimaryHeap = %X done\n", hPrimaryHeap));
}
//******************************************************************************
//******************************************************************************
LPVOID OS2Heap::Alloc(DWORD dwFlags, DWORD dwBytes)
{
    HEAPELEM *lpHeapObj;
    LPVOID    lpMem;
    DWORD     dwAllocBytes;

#if 0
    extern HANDLE Heap_ProcessHeap;
    dprintf(("OS2Heap::Alloc(%x:%x:%x) %x %d\n",
             this, getHeapHandle(), Heap_ProcessHeap, dwFlags, dwBytes));
#endif

    //size must be multiple of 8 bytes
    dwAllocBytes = HEAP_ALIGN(dwBytes);

    lpMem = _umalloc(uheap, dwAllocBytes + HEAP_OVERHEAD);
    if(lpMem == NULL) {
        dprintf(("OS2Heap::Alloc, lpMem == NULL"));
        return(NULL);
    }
    
    RasTrackMemAlloc (rthHeap, dwAllocBytes);
    
    if(dwFlags & HEAP_ZERO_MEMORY) {
        memset(lpMem, 0, dwAllocBytes+HEAP_OVERHEAD);
    }
  
#ifdef DEBUG
    totalAlloc += dwAllocBytes;
#endif
  
    //align at 8 byte boundary
    lpHeapObj          = (HEAPELEM *)HEAP_ALIGN(lpMem);
    lpHeapObj->lpMem   = lpMem;
    lpHeapObj->magic   = MAGIC_NR_HEAP;
    lpHeapObj->orgsize = dwBytes;	//original size
    lpHeapObj->cursize = dwBytes;	//original size
 
    return(LPVOID)(lpHeapObj+1);
}
//******************************************************************************
//******************************************************************************
DWORD OS2Heap::Size(DWORD dwFlags, PVOID lpMem)
{
    HEAPELEM *helem = GET_HEAPOBJ(lpMem);

    if(lpMem == NULL) {
    	dprintf(("OS2Heap::Size lpMem == NULL\n"));
    	return -1;
    }
    /* verify lpMem address */
    if (lpMem >= (LPVOID)ulMaxAddr || lpMem < (LPVOID)0x10000)
    {
        dprintf(("OS2Heap::Size ERROR BAD HEAP POINTER:%X\n", lpMem));
    	return -1;
    }

    if(helem->magic != MAGIC_NR_HEAP)
    {
    	dprintf(("OS2Heap::Size ERROR BAD HEAP POINTER:%X\n", lpMem));
    	return -1;
    }
    return helem->cursize;  //return current size of memory block
}
//******************************************************************************
//******************************************************************************
LPVOID OS2Heap::ReAlloc(DWORD dwFlags, LPVOID lpMem, DWORD dwBytes)
{
    HEAPELEM *helem = GET_HEAPOBJ(lpMem);
    LPVOID lpNewMem;
    int    i, maxSize;

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

    maxSize = HEAP_ALIGN(helem->orgsize); 
    if (dwBytes <= maxSize) {
        dprintf(("ReAlloc with smaller size than original (%d); return old pointer", maxSize));
        
        RasTrackMemRealloc (rthHeap, helem->cursize, dwBytes);
        
        //update current size so HeapSize will return the right value
        helem->cursize = dwBytes;  
        return lpMem; // if reallocation with same size don't do anything
    }
    lpNewMem = Alloc(dwFlags, dwBytes);
    memcpy(lpNewMem, lpMem, dwBytes < maxSize ? dwBytes : maxSize);
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
    HEAPELEM *helem = GET_HEAPOBJ(lpMem);

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
#ifdef DEBUG
    totalAlloc -= size;
#endif
#endif

    RasTrackMemFree (rthHeap, helem->cursize);

    free(helem->lpMem);
    
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
    HEAPELEM *helem = GET_HEAPOBJ(lpMem);

    dprintf(("OS2Heap::Validate, %X %X", dwFlags, lpMem));

    /* verify lpMem address */
    if (lpMem >= (LPVOID)ulMaxAddr || lpMem < (LPVOID)0x10000)
    {
    	dprintf(("OS2Heap::Validate BAD HEAP POINTER:%X\n", lpMem));
    	return FALSE;
    }

    if(helem->magic != MAGIC_NR_HEAP)
    {
    	dprintf(("OS2Heap::Validate BAD HEAP POINTER:%X\n", lpMem));
    	return FALSE;
    }
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
/* PH moved to inlineable macro ...
 *  // check against cache first
 * if (fhhm_lastHeap)
 *   if (hHeap == fhhm_lastHandle)
 *   {
 *     return fhhm_lastHeap;
 *   }
 *
 */
 
  OS2Heap *curheap = OS2Heap::heap;
  
  //@@@PH NT programs seem to assume heap 0 is always valid?!
  if (hHeap == 0)
    if (curheap != NULL)
    {
      fhhm_lastHandle = hHeap;
      fhhm_lastHeap   = curheap;
      return curheap;
    }

  while(curheap != NULL) 
  {
    if(curheap->hPrimaryHeap == hHeap) 
    {
      fhhm_lastHandle = hHeap;
      fhhm_lastHeap   = curheap;
      return(curheap);
    }
    curheap = curheap->next;
  }
  dprintf(("Heap %X not found!\n", hHeap));
  fhhm_lastHandle = hHeap;
  fhhm_lastHeap   = NULL;
  return(NULL);
}
//******************************************************************************
//******************************************************************************
OS2Heap *OS2Heap::heap = NULL;


//******************************************************************************
//******************************************************************************
void * _LNK_CONV getmoreHeapMem(Heap_t pHeap, size_t *size, int *clean)
{
    APIRET rc;
    PVOID newblock;

    dprintf(("KERNEL32: getmoreHeapMem(%08xh, %08xh, %08xh)", pHeap, *size, *clean));

    /* round the size up to a multiple of 64K */
    //NOTE: MUST use 64kb here or else we are at risk of running out of virtual
    //      memory space. (when allocating 4kb we actually get 4kb + 60k uncommited)
    *size = ( (*size / 65536) + 1) * 65536;

    rc = DosAllocMem(&newblock, *size, PAG_READ|PAG_WRITE|PAG_COMMIT|PAG_EXECUTE);
    if(rc != 0) {
    	dprintf(("getmoreHeapMem: DosAllocMem failed with %d", rc));
    	return FALSE;
    }
    *clean = _BLOCK_CLEAN;
    dprintf(("KERNEL32: getmoreHeapMem %x %d", newblock, *size));
    return newblock;
}
//******************************************************************************
//******************************************************************************
void _LNK_CONV releaseHeapMem(Heap_t pHeap, void *block, size_t size)
{
    dprintf(("KERNEL32: releaseHeapMem %x %x %d", pHeap, block, size));
    DosFreeMem(block);
}
//******************************************************************************
//******************************************************************************
