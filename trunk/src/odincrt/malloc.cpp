/* $Id: malloc.cpp,v 1.9 2003-01-23 12:33:04 sandervl Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Memory RTL function wrappers
 *
 * Copyright 1999 Sander van Leeuwen
 *
 */

#define ORIGINAL_VAC_FUNCTIONS
#include <malloc.h>
#include <umalloc.h>
#include <os2sel.h>

void * _IMPORT _LNK_CONV _debug_calloc( size_t, size_t, const char *, size_t );
void   _IMPORT _LNK_CONV _debug_free( void *, const char *, size_t );
void * _IMPORT _LNK_CONV _debug_malloc( size_t, const char *, size_t );
void * _IMPORT _LNK_CONV _debug_realloc( void *, size_t, const char *, size_t );
void * _IMPORT _LNK_CONV _debug_umalloc(Heap_t , size_t , const char *,size_t);
void * _IMPORT _LNK_CONV _debug_ucalloc(Heap_t , size_t, size_t ,const char *,size_t);

#ifdef DEBUG
unsigned long nrcalls_malloc = 0;
unsigned long nrcalls_free   = 0;
unsigned long totalmemalloc  = 0;

void _LNK_CONV getcrtstat(unsigned long *pnrcalls_malloc,
                          unsigned long *pnrcalls_free,
                          unsigned long *ptotalmemalloc)
{
    *pnrcalls_malloc = nrcalls_malloc;
    *pnrcalls_free   = nrcalls_free;
    *ptotalmemalloc  = totalmemalloc;
}

#endif

void * _LNK_CONV CRTWRAP(calloc)( size_t a, size_t b )
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += a*b;
        nrcalls_malloc++;
#endif
	rc = calloc(a,b);
	SetFS(sel);
	return rc;
}

void   _LNK_CONV CRTWRAP(free)( void *a )
{
    unsigned short sel = RestoreOS2FS();

#ifdef DEBUG
        nrcalls_free++;
        totalmemalloc -= _msize(a);
#endif
	free(a);
	SetFS(sel);
}

void * _LNK_CONV CRTWRAP(malloc)( size_t a)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;


#ifdef DEBUG
        totalmemalloc += a;
        nrcalls_malloc++;
#endif
	rc = malloc(a);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV CRTWRAP(realloc)( void *a, size_t b)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += (b - _msize(a));
#endif

	rc = realloc(a, b);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV CRTWRAP(_debug_calloc)( size_t a, size_t b, const char *c, size_t d)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += a*b;
        nrcalls_malloc++;
#endif

	rc = _debug_calloc(a,b,c,d);
	SetFS(sel);
	return rc;
}

void   _LNK_CONV CRTWRAP(_debug_free)( void *a, const char *b, size_t c)
{
    unsigned short sel = RestoreOS2FS();


#ifdef DEBUG
        nrcalls_free++;
        totalmemalloc -= _msize(a);
#endif
	_debug_free(a,b,c);
	SetFS(sel);
}

void * _LNK_CONV CRTWRAP(_debug_malloc)( size_t a, const char *b, size_t c)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += a;
        nrcalls_malloc++;
#endif
	rc = _debug_calloc(1,a,b,c);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV CRTWRAP(_debug_realloc)( void *a, size_t b, const char *c, size_t d)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += (b - _msize(a));
#endif
	rc = _debug_realloc(a,b,c,d);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV CRTWRAP(_umalloc)(Heap_t a, size_t b)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += b;
        nrcalls_malloc++;
#endif
	rc = _umalloc(a,b);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV CRTWRAP(_ucalloc)(Heap_t a, size_t b, size_t c)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += b*c;
        nrcalls_malloc++;
#endif
	rc = _ucalloc(a,b,c);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV CRTWRAP(_debug_umalloc)(Heap_t a, size_t b, const char *c, size_t d)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += b;
        nrcalls_malloc++;
#endif
	rc = _debug_ucalloc(a, 1, b,c,d);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV CRTWRAP(_debug_ucalloc)(Heap_t a, size_t b, size_t c, const char *d, size_t e)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

#ifdef DEBUG
        totalmemalloc += b*c;
        nrcalls_malloc++;
#endif
	rc = _debug_ucalloc(a,b,c,d,e);
	SetFS(sel);
	return rc;
}

