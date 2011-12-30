/* $Id: heapshared.cpp,v 1.11 2003-01-13 16:51:39 sandervl Exp $ */
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
#define INCL_DOSPROCESS
#include <os2wrap.h>
#include <string.h>
#include <dbglog.h>
#include <stddef.h>
#include <stdlib.h>
#include <umalloc.h>
#include "initterm.h"

#define DBG_LOCALLOG	DBG_heapshared
#include "dbglocal.h"

//
// Global constants (keep it in sync with globaldata.asm!)
//
#define PAGE_SIZE           4096
#define MAX_HEAPSIZE        (8*1024*1024)
#define MAX_HEAPPAGES		(MAX_HEAPSIZE/PAGE_SIZE)
#define INCR_HEAPSIZE		(16*1024)

//
// Global DLL Data (keep it in sync with globaldata.asm!)
//
extern Heap_t  sharedHeap; // = 0
extern PVOID   pSharedMem; // = NULL
extern BYTE    pageBitmap[MAX_HEAPPAGES]; // = {0}
extern ULONG   refCount; // = 0;

static int     privateRefCount = 0;

void * _LNK_CONV getmoreShared(Heap_t pHeap, size_t *size, int *clean);
void _LNK_CONV releaseShared(Heap_t pHeap, void *block, size_t size);

//******************************************************************************
//******************************************************************************
BOOL SYSTEM InitializeSharedHeap()
{
    APIRET rc;
    ULONG  flAllocMem = 0, ulSysinfo;

    //necessary until next WGSS update
    if(++privateRefCount > 1) {
        return TRUE;
    }

    rc = DosQuerySysInfo(QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT, &ulSysinfo, sizeof(ulSysinfo));
    if (rc == 0 && ulSysinfo > 512)   //VirtualAddresslimit is in MB
    {
        flAllocMem = PAG_ANY;
    }

    if(pSharedMem == NULL) {
  	    rc = DosAllocSharedMem(&pSharedMem, NULL, MAX_HEAPSIZE, PAG_READ|PAG_WRITE|OBJ_GETTABLE|flAllocMem);
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
  	    sharedHeap = _ucreate(pSharedMem, INCR_HEAPSIZE, _BLOCK_CLEAN, _HEAP_REGULAR|_HEAP_SHARED,
                	          getmoreShared, releaseShared);

  	    if(sharedHeap == NULL) {
    		DosFreeMem(pSharedMem);
	        dprintf(("InitializeSharedHeap: _ucreate failed!"));
	        return FALSE;
	    }
  	    dprintf(("KERNEL32: First InitializeSharedHeap %x %x", pSharedMem, sharedHeap));
	    for(int i=0;i<INCR_HEAPSIZE/PAGE_SIZE;i++) {
    	  	pageBitmap[i] = 1; //mark as committed
    	}
    }
    else {
    	if(DosGetSharedMem(pSharedMem, PAG_READ|PAG_WRITE) != 0) {
		    dprintf(("InitializeSharedHeap: DosGetSharedMem failed!"));
		    return FALSE;
	    }
  	    dprintf(("KERNEL32: InitializeSharedHeap %x %x refcount %d", pSharedMem, sharedHeap, refCount));
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
#ifdef DEBUG
int _LNK_CONV callback_function(const void *pentry, size_t sz, int useflag, int status,
                                const char *filename, size_t line)
{
    if (_HEAPOK != status) {
       dprintf(("status is not _HEAPOK."));
       return 1;
    }
    if (_USEDENTRY == useflag && sz && filename && line) {
         dprintf(("allocated  %08x %u at %s %d\n", pentry, sz, filename, line));
    }
    else dprintf(("allocated  %08x %u", pentry, sz));

    return 0;
}
#endif
//******************************************************************************
//******************************************************************************
void SYSTEM DestroySharedHeap()
{
    dprintf(("KERNEL32: DestroySharedHeap %d", refCount));
    if(--privateRefCount > 0) {
        return;
    }

#ifdef DEBUG
    _uheap_walk(sharedHeap, callback_function);
    dprintf((NULL));
#endif

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
    dprintf(("KERNEL32: GetPageRangeFree(%08xh)", pageoffset));

    int i;
    for(i=pageoffset;i<MAX_HEAPPAGES;i++) {
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

    dprintf(("KERNEL32: getmoreShared(%08xh, %08xh, %08xh)\n", pHeap, *size, *clean));

    /* round the size up to a multiple of 4K */
    // *size = (*size / 4096) * 4096 + 4096;
    // @@@PH speed improvement
    *size = (*size + 4096) & 0xFFFFF000;
    *size = max(*size, INCR_HEAPSIZE);

    for(int i=0;i<MAX_HEAPPAGES;i++)
    {
    	int nrpagesfree = GetPageRangeFree(i);
    	if(nrpagesfree >= *size/PAGE_SIZE)
        {
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
void * SYSTEM _smalloc(int size)
{
    void *chunk;

    chunk = _umalloc(sharedHeap, size);
    dprintf(("_smalloc %x returned %x", size, chunk));
    return chunk;
}
//******************************************************************************
//******************************************************************************
void * SYSTEM _smallocfill(int size, int filler)
{
    void *chunk;

    chunk =  _umalloc(sharedHeap, size);
    if(chunk) {
        memset(chunk, 0, size);
    }
    dprintf(("_smallocfill %x %x returned %x", size, filler, chunk));
    return chunk;
}
//******************************************************************************
//******************************************************************************
void SYSTEM _sfree(void *block)
{
    dprintf(("_sfree %x", block));
    free(block);
}
//******************************************************************************
//******************************************************************************
void * _System _debug_smalloc(int size, const char *pszFile, int linenr)
{
    void *chunk;

#ifdef __DEBUG_ALLOC__
    chunk = _debug_umalloc(sharedHeap, size, pszFile, linenr);
#else
    chunk = _umalloc(sharedHeap, size);
#endif
    dprintf(("_smalloc %x returned %x", size, chunk));
    return chunk;
}
//******************************************************************************
//******************************************************************************
void * _System _debug_smallocfill(int size, int filler, const char *pszFile, int linenr)
{
    void *chunk;

#ifdef __DEBUG_ALLOC__
    chunk =  _debug_umalloc(sharedHeap, size, pszFile, linenr);
#else
    chunk =  _umalloc(sharedHeap, size);
#endif
    if(chunk) {
        memset(chunk, 0, size);
    }
    dprintf(("_smallocfill %x %x returned %x", size, filler, chunk));
    return chunk;
}
//******************************************************************************
//******************************************************************************
void   _System _debug_sfree(void *chunk, const char *pszFile, int linenr)
{
    dprintf(("_sfree %x", chunk));
#ifdef __DEBUG_ALLOC__
    _debug_free(chunk, pszFile, linenr);
#else
    free(chunk);
#endif
}
//******************************************************************************
//******************************************************************************
