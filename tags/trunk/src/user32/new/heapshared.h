/* $Id: heapshared.h,v 1.1 1999-08-28 14:09:58 sandervl Exp $ */
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

extern Heap_t sharedHeap;

BOOL   InitializeSharedHeap();
void   DestroySharedHeap();

void  *_smalloc(size_t size);

#endif