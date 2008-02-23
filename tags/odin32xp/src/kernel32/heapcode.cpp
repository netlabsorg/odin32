/* $Id: heapcode.cpp,v 1.4 2002-07-15 14:28:51 sandervl Exp $ */
/*
 * Code heap functions for OS/2
 *
 * Initially commit 4 kb, add more when required
 *
 * NOTE: If high memory is ever used for this heap, then you must use VirtualAlloc!
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
#include <heapcode.h>

#define DBG_LOCALLOG	DBG_heapcode
#include "dbglocal.h"

Heap_t          codeHeap = 0;
static PVOID    pCodeMem = NULL;

void * _LNK_CONV getmoreCodeMem(Heap_t pHeap, size_t *size, int *clean);
void _LNK_CONV   releaseCodeMem(Heap_t pHeap, void *block, size_t size);

//******************************************************************************
//******************************************************************************
BOOL InitializeCodeHeap()
{
    APIRET rc;

    dprintf(("KERNEL32: InitializeCodeHeap"));
    
    //NOTE: MUST use 64kb here or else we are at risk of running out of virtual
    //      memory space. (when allocating 4kb we actually get 4kb + 60k uncommited)
    rc = DosAllocMem(&pCodeMem, 64*1024, PAG_READ|PAG_WRITE|PAG_COMMIT|PAG_EXECUTE);
    if(rc != 0) {
    	dprintf(("InitializeSharedHeap: DosAllocSharedMem failed with %d", rc));
    	return FALSE;
    }
    codeHeap = _ucreate(pCodeMem, PAGE_SIZE, _BLOCK_CLEAN, _HEAP_REGULAR, getmoreCodeMem, releaseCodeMem);
    if(codeHeap == NULL) {
    	DosFreeMem(pCodeMem);
        pCodeMem = NULL;
	    dprintf(("InitializeSharedHeap: _ucreate failed!"));
	    return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
void DestroyCodeHeap()
{
    dprintf(("KERNEL32: DestroyCodeHeap"));
    if(codeHeap) {
	    _uclose(codeHeap);
	    _udestroy(codeHeap, _FORCE);
	    codeHeap = NULL;
    }
    if(pCodeMem) {
    	DosFreeMem(pCodeMem);
    	pCodeMem = NULL;
    }
}
//******************************************************************************
//******************************************************************************
void * _LNK_CONV getmoreCodeMem(Heap_t pHeap, size_t *size, int *clean)
{
    APIRET rc;
    PVOID newblock;

    dprintf(("KERNEL32: getmoreCodeMem(%08xh, %08xh, %08xh)\n", pHeap, *size, *clean)); 

    /* round the size up to a multiple of 64K */
    //NOTE: MUST use 64kb here or else we are at risk of running out of virtual
    //      memory space. (when allocating 4kb we actually get 4kb + 60k uncommited)
    *size = ( (*size / 65536) + 1) * 65536;

    rc = DosAllocMem(&newblock, *size, PAG_READ|PAG_WRITE|PAG_COMMIT|PAG_EXECUTE);
    if(rc != 0) {
    	dprintf(("getmoreCodeMem: DosAllocMem failed with %d", rc));
    	return FALSE;
    }
    *clean = _BLOCK_CLEAN;
    dprintf(("KERNEL32: getmoreCodeMem %x %d", newblock, *size));
    return newblock;
}
//******************************************************************************
//******************************************************************************
void _LNK_CONV releaseCodeMem(Heap_t pHeap, void *block, size_t size)
{
    dprintf(("KERNEL32: releaseCodeMem %x %d", block, size));
    DosFreeMem(block);
}
//******************************************************************************
//******************************************************************************
