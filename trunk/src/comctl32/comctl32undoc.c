/*
 * Undocumented functions from COMCTL32.DLL
 *
 * Copyright (C) 1999 Achim Hasenmueller
 *
 * Based on the work of the WINE group (www.winehq.com)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include "comctl32.h"

extern HANDLE COMCTL32_hHeap; /* handle to the private heap */


/**************************************************************************
 * Alloc [COMCTL32.71]
 *
 * Allocates memory block from the dll's private heap
 *
 * PARAMS
 *     dwSize [I] size of the allocated memory block
 *
 * RETURNS
 *     Success: pointer to allocated memory block
 *     Failure: NULL
 */


LPVOID WINAPI COMCTL32_Alloc (DWORD dwSize)
{
    LPVOID lpPtr;

    lpPtr = HeapAlloc (COMCTL32_hHeap, HEAP_ZERO_MEMORY, dwSize);

    return lpPtr;
}


/**************************************************************************
 * ReAlloc [COMCTL32.72]
 *
 * Changes the size of an allocated memory block or allocates a memory
 * block using the dll's private heap.
 *
 * PARAMS
 *     lpSrc  [I] pointer to memory block which will be resized
 *     dwSize [I] new size of the memory block.
 *
 * RETURNS
 *     Success: pointer to the resized memory block
 *     Failure: NULL
 *
 * NOTES
 *     If lpSrc is a NULL-pointer, then COMCTL32_ReAlloc allocates a memory
 *     block like COMCTL32_Alloc.
 */

LPVOID WINAPI COMCTL32_ReAlloc (LPVOID lpSrc, DWORD dwSize)
{
    LPVOID lpDest;

    if (lpSrc)
	lpDest = HeapReAlloc (COMCTL32_hHeap, HEAP_ZERO_MEMORY, lpSrc, dwSize);
    else
	lpDest = HeapAlloc (COMCTL32_hHeap, HEAP_ZERO_MEMORY, dwSize);

    return lpDest;
}


/**************************************************************************
 * Free [COMCTL32.73]
 *
 * Frees an allocated memory block from the dll's private heap.
 *
 * PARAMS
 *     lpMem [I] pointer to memory block which will be freed
 *
 * RETURNS
 *     Success: TRUE
 *     Failure: FALSE
 */

BOOL WINAPI COMCTL32_Free (LPVOID lpMem)
{
    return HeapFree (COMCTL32_hHeap, 0, lpMem);
}


/**************************************************************************
 * GetSize [COMCTL32.74]
 *
 * Retrieves the size of the specified memory block from the dll's
 * private heap.
 *
 * PARAMS
 *     lpMem [I] pointer to an allocated memory block
 *
 * RETURNS
 *     Success: size of the specified memory block
 *     Failure: 0
 */

DWORD WINAPI COMCTL32_GetSize (LPVOID lpMem)
{

    return HeapSize (COMCTL32_hHeap, 0, lpMem);
}
