/* $Id: heapcode.h,v 1.2 2001-02-09 18:32:06 sandervl Exp $ */
/*
 * Code heap functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __HEAPCODE_H__
#define __HEAPCODE_H__

#include <heapshared.h>

extern Heap_t codeHeap;

BOOL   InitializeCodeHeap();
void   DestroyCodeHeap();

#define _cmalloc(size)	_umalloc(codeHeap, size)

#endif