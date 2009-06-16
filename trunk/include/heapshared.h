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

#ifdef DEBUG
    void * _System _debug_smalloc(int size, char *pszFile, int linenr);
    void * _System _debug_smallocfill(int size, int filler, char *pszFile, int linenr);
    void   _System _debug_sfree(void *chunk, char *pszFile, int linenr);

    #define _smalloc(a)         _debug_smalloc((a),__FILE__,__LINE__)
    #define _smallocfill(a,b)   _debug_smallocfill((a),(b),__FILE__,__LINE__)
    #define _sfree(a)           _debug_sfree((a),__FILE__,__LINE__)
#else
    void * _System _smalloc(int size);
    void * _System _smallocfill(int size, int filler);
    #define _sfree(a) free(a)
#endif

#endif
