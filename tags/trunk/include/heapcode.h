/* $Id: heapcode.h,v 1.1 1999-12-09 00:50:46 sandervl Exp $ */
/*
 * Code heap functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __HEAPCODE_H__
#define __HEAPCODE_H__

#include <heapshared.h>

BOOL   InitializeCodeHeap();
void   DestroyCodeHeap();

void  *_cmalloc(size_t size);

#endif