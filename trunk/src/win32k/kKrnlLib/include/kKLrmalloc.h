/* $Id: kKLrmalloc.h,v 1.1 2001-09-14 01:50:18 bird Exp $
 *
 * Resident Heap.
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/* XLATOFF */
#ifndef _RMALLOC_H_
#define _RMALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif
/* XLATON */
/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct HeapState_s  /* note: this is used by both swappable and resident heaps */
{
    ULONG       cbHeapSize;          /* Amount of memory used by the heap free and used++. */
    ULONG       cbHeapFree;          /* Amount of used space. */
    ULONG       cbHeapUsed;          /* Amount of free space reserved. */
    ULONG       cBlocksUsed;         /* Count of used blocks. */
    ULONG       cBlocksFree;         /* Count of free blocks. */
} HEAPSTATE, *PHEAPSTATE;


/*******************************************************************************
*   Exported Functions and Variables                                           *
*******************************************************************************/
int         resHeapInit(unsigned, unsigned);
void *      rmalloc(unsigned);
void *      rrealloc(void *, unsigned);
void        rfree(void *);

unsigned    _res_msize(void *);
unsigned    _res_memfree(void);
unsigned    _res_memused(void);
int         _res_state(PHEAPSTATE);
int         _res_heap_check(void);
void        _res_heapmin(void);
void        _res_dump_subheaps(void);
void        _res_dump_allocated(unsigned);

extern unsigned cbResHeapMax;           /* Maximum amount of memory used by the heap. */


/* XLATOFF */
#ifdef __cplusplus
}
#endif
#endif /* _MALLOC_H_ */
/* XLATON */

