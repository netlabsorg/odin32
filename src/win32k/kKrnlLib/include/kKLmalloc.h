/* $Id: kKLmalloc.h,v 1.1 2001-09-14 01:50:17 bird Exp $
 *
 * Heap.
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/* XLATOFF */
#ifndef _MALLOC_H_
#define _MALLOC_H_
#ifdef __MALLOC_H
    #error("A different version of malloc.h has allready been loaded!")
#endif
#define __malloc_h

#ifdef __cplusplus
extern "C" {
#endif
/* XLATON */

/*******************************************************************************
*   Exported Functions and Variables                                           *
*******************************************************************************/
int         heapInit(unsigned, unsigned, unsigned, unsigned);
void *      malloc(unsigned);
void *      realloc(void *, unsigned);
void        free(void *);
unsigned    _memfree(void);
unsigned    _msize(void *);
int         _heap_check(void);

/* XLATOFF */
#ifdef __cplusplus
}
#endif
#endif /* _MALLOC_H_ */
/* XLATON */

