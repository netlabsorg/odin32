/* $Id: heap.h,v 1.1 1999-05-24 20:19:12 ktk Exp $ */

/*
 * Win32 heap definitions
 *
 * Copyright 1996 Alexandre Julliard
 */

#ifndef __WINE_HEAP_H
#define __WINE_HEAP_H

#include "winbase.h"

extern HANDLE SystemHeap;
extern HANDLE SegptrHeap;

extern int HEAP_IsInsideHeap( HANDLE heap, DWORD flags, LPCVOID ptr );
extern SEGPTR HEAP_GetSegptr( HANDLE heap, DWORD flags, LPCVOID ptr );
extern LPVOID HEAP_xalloc( HANDLE heap, DWORD flags, DWORD size );
extern LPVOID HEAP_xrealloc( HANDLE heap, DWORD flags, LPVOID lpMem, DWORD size );
extern LPSTR HEAP_strdupA( HANDLE heap, DWORD flags, LPCSTR str );
extern LPWSTR HEAP_strdupW( HANDLE heap, DWORD flags, LPCWSTR str );
extern LPWSTR HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str );
extern LPSTR HEAP_strdupWtoA( HANDLE heap, DWORD flags, LPCWSTR str );

/* SEGPTR helper macros */

#define SEGPTR_ALLOC(size) \
         (HeapAlloc( SegptrHeap, 0, (size) ))
#define SEGPTR_NEW(type) \
         ((type *)HeapAlloc( SegptrHeap, 0, sizeof(type) ))
#define SEGPTR_STRDUP(str) \
         (HIWORD(str) ? HEAP_strdupA( SegptrHeap, 0, (str) ) : (LPSTR)(str))
#define SEGPTR_STRDUP_WtoA(str) \
         (HIWORD(str) ? HEAP_strdupWtoA( SegptrHeap, 0, (str) ) : (LPSTR)(str))
	/* define an inline function, a macro won't do */
static __inline__ SEGPTR WINE_UNUSED SEGPTR_Get(LPCVOID ptr) {
         return (HIWORD(ptr) ? HEAP_GetSegptr( SegptrHeap, 0, ptr ) : (SEGPTR)ptr);
}
#define SEGPTR_GET(ptr) SEGPTR_Get(ptr)
#define SEGPTR_FREE(ptr) \
         (HIWORD(ptr) ? HeapFree( SegptrHeap, 0, (ptr) ) : 0)


#endif  /* __WINE_HEAP_H */
