/* $Id: malloc.cpp,v 1.5 2001-02-11 10:32:51 sandervl Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Memory RTL function wrappers
 *
 * Copyright 1999 Sander van Leeuwen
 *
 */

//#undef __DEBUG_ALLOC__
#include <malloc.h>
#include <umalloc.h>
#include <os2sel.h>

void * _IMPORT _LNK_CONV _debug_calloc( size_t, size_t, const char *, size_t );
void   _IMPORT _LNK_CONV _debug_free( void *, const char *, size_t );
void * _IMPORT _LNK_CONV _debug_malloc( size_t, const char *, size_t );
void * _IMPORT _LNK_CONV _debug_realloc( void *, size_t, const char *, size_t );
void * _IMPORT _LNK_CONV _debug_umalloc(Heap_t , size_t , const char *,size_t);
void * _IMPORT _LNK_CONV _debug_ucalloc(Heap_t , size_t, size_t ,const char *,size_t);

void * _LNK_CONV os2calloc( size_t a, size_t b )
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = calloc(a,b);
	SetFS(sel);
	return rc;
}

void   _LNK_CONV os2free( void *a )
{
    unsigned short sel = RestoreOS2FS();

	free(a);
	SetFS(sel);
}

void * _LNK_CONV os2malloc( size_t a)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = malloc(a);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV os2realloc( void *a, size_t b)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = realloc(a, b);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV os2_debug_calloc( size_t a, size_t b, const char *c, size_t d)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = _debug_calloc(a,b,c,d);
	SetFS(sel);
	return rc;
}

void   _LNK_CONV os2_debug_free( void *a, const char *b, size_t c)
{
    unsigned short sel = RestoreOS2FS();

	_debug_free(a,b,c);
	SetFS(sel);
}

void * _LNK_CONV os2_debug_malloc( size_t a, const char *b, size_t c)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = _debug_calloc(1,a,b,c);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV os2_debug_realloc( void *a, size_t b, const char *c, size_t d)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = _debug_realloc(a,b,c,d);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV os2_umalloc(Heap_t a, size_t b)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = _umalloc(a,b);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV os2_ucalloc(Heap_t a, size_t b, size_t c)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = _ucalloc(a,b,c);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV os2_debug_umalloc(Heap_t a, size_t b, const char *c, size_t d)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = _debug_ucalloc(a, 1, b,c,d);
	SetFS(sel);
	return rc;
}

void * _LNK_CONV os2_debug_ucalloc(Heap_t a, size_t b, size_t c, const char *d, size_t e)
{
    unsigned short sel = RestoreOS2FS();
    void *rc;

	rc = _debug_ucalloc(a,b,c,d,e);
	SetFS(sel);
	return rc;
}

