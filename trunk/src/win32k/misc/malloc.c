/* $Id: malloc.c,v 1.6 2000-01-24 18:19:00 bird Exp $
 *
 * Common Heap - this forwards to the swappable heap!
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_NOAPI

/******************************************************************************
*  Headerfiles
******************************************************************************/
#include <os2.h>
#include "smalloc.h"
#include "rmalloc.h"
#include "options.h"


/******************************************************************************
*  Global data
******************************************************************************/
#ifndef RING0
    char           fInited;       /* init flag */
#endif



/**
 * Initiate the heap "subsystems" - both the resident and the swappable heaps.
 * @returns   0 on success, not 0 on error.
 * @param     cbResInit  Resident heap initial size.
 * @param     cbResMax   Resident heap maximum size.
 * @param     cbSwpInit  Swappable heap initial size.
 * @param     cbSwpMax   Swappable heap maximum size.
 */
int heapInit(unsigned cbResInit, unsigned cbResMax,
             unsigned cbSwpInit, unsigned cbSwpMax)
{
    int rc;

    rc = resHeapInit(cbResInit, cbResMax);
    if (rc != 0)
        return rc;
    rc = swpHeapInit(cbSwpInit, cbSwpMax);
    if (rc != 0)
        return rc;
    #ifdef RING3
        fInited = TRUE;
    #endif
    return 0;
}


/**
 * malloc - allocates a given amount of memory.
 * @returns  Pointer to allocated memory.
 *           NULL if out of memory. (Or memory to fragmented.)
 * @param    cbSize  Bytes user requests us to allocate. This is aligned
 *                   to four bytes.
 */
void * malloc(unsigned cbSize)
{
    return smalloc(cbSize);
}


/**
 * Reallocate a heapblock.
 * @returns   Pointer to new heapblock.
 * @param     pv     Pointer to the block to realloc.
 * @param     cbNew  The new block size.
 */
void *realloc(void *pv, unsigned cbNew)
{
    return srealloc(pv, cbNew);
}


/**
 * Frees a block.
 * @param    pv  User pointer.
 */
void free(void *pv)
{
    sfree(pv);
}


/**
 * Gets the size of a block.
 * @returns  Bytes in a block.
 */
unsigned _msize(void *pv)
{
    return _swp_msize(pv);
}


/**
 * Checks if pv is a valid heappointer.
 * @returns   1 if valid. 0 if invalid.
 * @param     pv  User data pointer.
 */
int _validptr(void *pv)
{
    return _swp_validptr(pv);
}


/**
 * Checks that the dataaera made up by pv and cbSize valid with in the heap.
 * @returns   1 if valid. 0 if invalid.
 * @param     pv      User data pointer.
 * @param     cbSize  Size of data which has to be valid.
 */
int _validptr2(void *pv, unsigned cbSize)
{
    return _swp_validptr2(pv, cbSize);
}


/**
 * Get amount of free memory (in bytes)
 * @returns  Amount of free memory (in bytes).
 * @remark   Note that this amount is of all free memory blocks and
 *           that these blocks are fragmented.
 *           You'll probably not be able to allocate a single block
 *           of the returned size.
 */
unsigned _memfree(void)
{
    return _swp_memfree();
}


/**
 * Checks heap integrety.
 * @returns  TRUE  when ok.
 *           FALSE on error.
 *           NULL if out of memory. (Or memory to fragmented.)
 */
int _heap_check(void)
{
    return _swp_heap_check();
}


#if !defined(RING0) && defined(__IBMC__)

/**
 * Initialize Memory Functions
 * Called from _exeentry.
 */
int _rmem_init(void)
{
    int rc = heapInit(CB_RES_INIT, CB_RES_MAX, CB_SWP_INIT, CB_SWP_MAX);
    return rc;
}

/**
 * Initialize Memory Functions
 * Called from _exeentry.
 */
int _rmem_term(void)
{
    return 0;
}

#endif
