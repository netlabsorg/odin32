/* $Id: smalloc.h,v 1.2 2000-09-02 21:08:04 bird Exp $
 *
 * Swappable Heap.
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
#ifndef _SMALLOC_H_
#define _SMALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif
/* XLATON */

struct HeapState_s;

/*******************************************************************************
*   Exported Functions and Variables                                           *
*******************************************************************************/
int         swpHeapInit(unsigned, unsigned);
void *      smalloc(unsigned);
void *      srealloc(void *, unsigned);
void        sfree(void *);

unsigned    _swp_msize(void *);
int         _swp_validptr(void *);
int         _swp_validptr2(void *, unsigned);
unsigned    _swp_memfree(void);
unsigned    _swp_memused(void);
int         _swp_state(struct HeapState_s *);
int         _swp_heap_check(void);
void        _swp_heapmin(void);
void        _swp_dump_subheaps(void);
void        _swp_dump_allocated(unsigned);

extern unsigned cbSwpHeapMax;   /* Maximum amount of memory used by the heap. */

/* XLATOFF */
#ifdef __cplusplus
}
#endif
#endif /* _MALLOC_H_ */
/* XLATON */

