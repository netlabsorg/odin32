/* $Id: heapshared.cpp,v 1.2 1999-08-28 19:32:46 sandervl Exp $ */
/*
 * Shared heap functions for OS/2
 *
 * Initially commit 16 kb, add more when required
 *
 * NOTE: Hardcoded limit of 256 KB per process (increase when required)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_BASE
#define INCL_DOSMEMMGR
#include <os2wrap.h>
#include <misc.h>
#include "heapshared.h"

       Heap_t  sharedHeap = 0;
static PVOID   pSharedMem = NULL;
static BYTE    pageBitmap[MAX_HEAPPAGES] = {0};

void * _LNK_CONV getmoreShared(Heap_t pHeap, size_t *size, int *clean);
void _LNK_CONV releaseShared(Heap_t pHeap, void *block, size_t size);

//******************************************************************************
//******************************************************************************
BOOL InitializeSharedHeap()
{
 APIRET rc;

  dprintf(("USER32: InitializeSharedHeap"));
  rc = DosAllocSharedMem(&pSharedMem, NULL, MAX_HEAPSIZE, PAG_READ|PAG_WRITE|OBJ_GETTABLE);
  if(rc != 0) {
	dprintf(("InitializeSharedHeap: DosAllocSharedMem failed with %d", rc));
	return FALSE;
  }
  rc = DosSetMem(pSharedMem, INCR_HEAPSIZE, PAG_READ|PAG_WRITE|PAG_COMMIT);
  if(rc != 0) {
	dprintf(("InitializeSharedHeap: DosSetMem failed with %d", rc));
	return FALSE;
  }
  sharedHeap = _ucreate(pSharedMem, INCR_HEAPSIZE, _BLOCK_CLEAN, 
                        _HEAP_REGULAR|_HEAP_SHARED,
                        getmoreShared, releaseShared);

  if(sharedHeap == NULL) {
	dprintf(("InitializeSharedHeap: _ucreate failed!"));
	return FALSE;
  }
  for(int i=0;i<INCR_HEAPSIZE/PAGE_SIZE;i++) {
  	pageBitmap[i] = 1; //mark as committed
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void DestroySharedHeap()
{
  dprintf(("USER32: DestroySharedHeap"));
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
//******************************************************************************
//******************************************************************************
void  *_smalloc(size_t size)
{
  return _umalloc(sharedHeap, size);
}
//******************************************************************************
//******************************************************************************
ULONG GetPageRangeFree(ULONG pageoffset)
{
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

  /* round the size up to a multiple of 4K */
  *size = (*size / 4096) * 4096 + 4096;
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
  		for(int j=i;j<i+*size/PAGE_SIZE;i++) {
  			pageBitmap[i] = 1; //mark as committed
  		}

		*clean = _BLOCK_CLEAN;
		dprintf(("USER32: getmoreShared %x %d", newblock, *size));
		return newblock;
	}
	if(nrpagesfree)
		i += nrpagesfree-1;
  }
  dprintf(("USER32: getmoreShared NOTHING LEFT (%d)", *size));
  return NULL;
}
//******************************************************************************
//******************************************************************************
void _LNK_CONV releaseShared(Heap_t pHeap, void *block, size_t size)
{
 ULONG pagenr;

  dprintf(("USER32: releaseShared %x %d", block, size));
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
  return (DWORD) pSharedMem;
}
//******************************************************************************
//******************************************************************************
