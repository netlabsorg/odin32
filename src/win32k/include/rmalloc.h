/* $Id: rmalloc.h,v 1.1 2000-01-22 18:22:26 bird Exp $
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
*   Exported Functions and Variables                                           *
*******************************************************************************/
int         ResHeapInit(unsigned, unsigned);
void *      rmalloc(unsigned);
void *      rrealloc(void *, unsigned);
void        rfree(void *);

unsigned    _res_msize(void *);
int         _res_validptr(void *);
int         _res_validptr2(void *, unsigned);
unsigned    _res_memfree(void);
int         _res_heap_check(void);
void        _res_heapmin(void);
void        _res_dump_subheaps(void);
void        _res_dump_allocated(unsigned);

/* XLATOFF */
#ifdef __cplusplus
}
#endif
#endif /* _MALLOC_H_ */
/* XLATON */

