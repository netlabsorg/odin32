/* $Id: heapshared.h,v 1.2 2000-01-18 20:47:30 sandervl Exp $ */
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

void  *_smalloc(size_t size);

#endif