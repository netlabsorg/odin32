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

BOOL   SYSTEM InitializeSharedHeap();
void   SYSTEM DestroySharedHeap();

void * SYSTEM _smalloc(int size);
void * SYSTEM _smallocfill(int size, int filler);
#define _sfree(a)  free(a)

#endif