/* $Id: heapshared.cpp,v 1.1 1999-08-28 14:09:57 sandervl Exp $ */
/*
 * Shared heap functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_BASE
#define INCL_DOSMEMMGR
#include <os2wrap.h>
#include <misc.h>
#include "heapshared.h"

#define INITIAL_HEAPSIZE	(64*1024)

Heap_t  sharedHeap = 0;
PVOID   pSharedMem = NULL;

void * _LNK_CONV getmoreShared(Heap_t pHeap, size_t *size, int *clean);
void _LNK_CONV releaseShared(Heap_t pHeap, void *block, size_t size);

//******************************************************************************
//******************************************************************************
BOOL InitializeSharedHeap()
{
 APIRET rc;

  dprintf(("USER32: InitializeSharedHeap"));
  rc = DosAllocSharedMem(&pSharedMem, NULL, INITIAL_HEAPSIZE, PAG_READ|PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE);
  if(rc != 0) {
	dprintf(("InitializeSharedHeap: DosAllocSharedMem failed with %d", rc));
	return FALSE;
  }
  sharedHeap = _ucreate(pSharedMem, INITIAL_HEAPSIZE, _BLOCK_CLEAN, 
                        _HEAP_REGULAR|_HEAP_SHARED,
                        getmoreShared, releaseShared);

  if(sharedHeap == NULL) {
	dprintf(("InitializeSharedHeap: _ucreate failed!"));
	return FALSE;
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
void * _LNK_CONV getmoreShared(Heap_t pHeap, size_t *size, int *clean)
{
 APIRET rc;
 ULONG newsize;
 PVOID newblock;

  /* round the size up to a multiple of 64K */
  *size = (*size / 65536) * 65536 + 65536;

  rc = DosAllocSharedMem(&newblock, NULL, *size, PAG_READ|PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE);
  if(rc != 0) {
	dprintf(("getmoreShared: DosAllocSharedMem failed with %d", rc));
	return NULL;
  }
  *clean = _BLOCK_CLEAN;

  return newblock;
}
//******************************************************************************
//******************************************************************************
void _LNK_CONV releaseShared(Heap_t pHeap, void *block, size_t size)
{
  DosFreeMem(block);
}
//******************************************************************************
//******************************************************************************
