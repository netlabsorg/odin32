/*
 * Shared heap functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __HEAPSHARED_H__
#define __HEAPSHARED_H__

#include <stddef.h>
#include <stdlib.h>
#include <umalloc.h>

#define PAGE_SIZE		4096
#define MAX_HEAPSIZE            (512*1024)
#define MAX_HEAPPAGES		(MAX_HEAPSIZE/PAGE_SIZE)
#define INCR_HEAPSIZE		(16*1024)

extern Heap_t sharedHeap;

BOOL   InitializeSharedHeap();
void   DestroySharedHeap();
DWORD  HeapGetSharedMemBase();

#define _smalloc(size)	_umalloc(sharedHeap, size)
#define _sheapmin()     _uheapmin(sharedHeap)

#endif