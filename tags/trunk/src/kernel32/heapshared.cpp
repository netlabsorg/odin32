/* $Id: heapshared.cpp,v 1.7 2001-02-09 18:31:05 sandervl Exp $ */
/*
 * Shared heap functions for OS/2
 *
 * Initially commit 16 kb, add more when required
 *
 * NOTE: Hardcoded limit of 512 KB (increase when required)
 *
 * TODO: Not process/thread safe (initializing/destroying heap)
 * 
 * ASSUMPTION: Rtl library takes care of protection of heap increase/decrease
 *             (from multiple threads/processes)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_BASE
#define INCL_DOSMEMMGR
#include <os2wrap.h>
#include <misc.h>
#include <heapshared.h>

#define DBG_LOCALLOG	DBG_heapshared
#include "dbglocal.h"

//Global DLL Data
#pragma data_seg(_GLOBALDATA)
       Heap_t  sharedHeap = 0;
static PVOID   pSharedMem = NULL;
static BYTE    pageBitmap[MAX_HEAPPAGES] = {0};
static ULONG   refCount = 0;
#pragma data_seg()

void * _LNK_CONV getmoreShared(Heap_t pHeap, size_t *size, int *clean);
void _LNK_CONV releaseShared(Heap_t pHeap, void *block, size_t size);

//******************************************************************************
//******************************************************************************
BOOL InitializeSharedHeap()
{
 APIRET rc;

  if(pSharedMem == NULL) {
  	dprintf(("KERNEL32: InitializeSharedHeap %x", &sharedHeap));
  	rc = DosAllocSharedMem(&pSharedMem, NULL, MAX_HEAPSIZE, PAG_READ|PAG_WRITE|OBJ_GETTABLE);
  	if(rc != 0) {
		dprintf(("InitializeSharedHeap: DosAllocSharedMem failed with %d", rc));
		return FALSE;
	}
	rc = DosSetMem(pSharedMem, INCR_HEAPSIZE, PAG_READ|PAG_WRITE|PAG_COMMIT);
	if(rc != 0) {
		DosFreeMem(pSharedMem);
		dprintf(("InitializeSharedHeap: DosSetMem failed with %d", rc));
		return FALSE;
	}
  	sharedHeap = _ucreate(pSharedMem, INCR_HEAPSIZE, _BLOCK_CLEAN,
        	              _HEAP_REGULAR|_HEAP_SHARED,
                	      getmoreShared, releaseShared);

  	if(sharedHeap == NULL) {
		DosFreeMem(pSharedMem);
		dprintf(("InitializeSharedHeap: _ucreate failed!"));
		return FALSE;
	}
	for(int i=0;i<INCR_HEAPSIZE/PAGE_SIZE;i++) {
	  	pageBitmap[i] = 1; //mark as committed
	}
  }
  else {
	if(DosGetSharedMem(pSharedMem, PAG_READ|PAG_WRITE) != 0) {
		dprintf(("InitializeSharedHeap: DosGetSharedMem failed!"));
		return FALSE;
	}
	if(_uopen(sharedHeap) != 0) {
		dprintf(("InitializeSharedHeap: unable to open shared heap!"));
		return FALSE;
	}
  }
  refCount++;
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void DestroySharedHeap()
{
  dprintf(("KERNEL32: DestroySharedHeap %d", refCount));
  if(--refCount == 0) {
  	if(sharedHeap) {
		_uclose(sharedHeap);
		_udestroy(sharedHeap, _FORCE);
		sharedHeap = NULL;
	}
	if(pSharedMem) {
		DosFreeMem(pSharedMem);
		pSharedMem = NULL;
	}
  }
  else {
	_uclose(sharedHeap);
  }
}
//******************************************************************************
//******************************************************************************
ULONG GetPageRangeFree(ULONG pageoffset)
{
  dprintf(("KERNEL32: GetPageRangeFree(%08xh)\n",
           pageoffset));

  for(int i=pageoffset;i<MAX_HEAPPAGES;i++) {
	if(pageBitmap[i] == 1) {
		break;
	}
  }
  return i-pageoffset;
}
//******************************************************************************
//******************************************************************************
void * _LNK_CONV getmoreShared(Heap_t pHeap, size_t *size, int *clean)
{
 APIRET rc;
 ULONG newsize;
 PVOID newblock;

 dprintf(("KERNEL32: getmoreShared(%08xh, %08xh, %08xh)\n",
          pHeap,
          *size,
          *clean));

  /* round the size up to a multiple of 4K */
  // *size = (*size / 4096) * 4096 + 4096;
  // @@@PH speed improvement
  *size = (*size + 4096) & 0xFFFFF000;
  *size = max(*size, INCR_HEAPSIZE);

  for(int i=0;i<MAX_HEAPPAGES;i++)
  {
	int nrpagesfree = GetPageRangeFree(i);
	if(nrpagesfree >= *size/PAGE_SIZE) {
  		newblock = (PVOID)((ULONG)pSharedMem + i*PAGE_SIZE);
  		rc = DosSetMem(newblock, *size, PAG_READ|PAG_WRITE|PAG_COMMIT);
  		if(rc != 0) {
			dprintf(("getmoreShared: DosSetMem failed with %d", rc));
			return NULL;
  		}
  		for(int j=0;j < *size/PAGE_SIZE; j++)
                {
  			pageBitmap[i+j] = 1; //mark as committed
  		}

		*clean = _BLOCK_CLEAN;
		dprintf(("KERNEL32: getmoreShared %x %d", newblock, *size));
		return newblock;
	}
	if(nrpagesfree)
		i += nrpagesfree-1;
  }
  dprintf(("KERNEL32: getmoreShared NOTHING LEFT (%d)", *size));
  return NULL;
}
//******************************************************************************
//******************************************************************************
void _LNK_CONV releaseShared(Heap_t pHeap, void *block, size_t size)
{
 ULONG pagenr;

  dprintf(("KERNEL32: releaseShared %x %d", block, size));
  DosSetMem(block, size, PAG_READ|PAG_WRITE|PAG_DECOMMIT);

  pagenr  = (ULONG)block - (ULONG)pSharedMem;
  pagenr /= PAGE_SIZE;
  for(int i=pagenr;i<pagenr+size/PAGE_SIZE;i++) {
  	pageBitmap[i] = 0; //mark as decommitted
  }
}
//******************************************************************************
//******************************************************************************
DWORD  HeapGetSharedMemBase()
{
  dprintf(("KERNEL32: HeapGetSharedMemBase()\n"));
  return (DWORD) pSharedMem;
}
//******************************************************************************
//******************************************************************************
