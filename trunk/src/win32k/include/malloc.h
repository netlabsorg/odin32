/* $Id: malloc.h,v 1.1 1999-09-06 02:19:58 bird Exp $
 *
 * Heap.
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/* XLATOFF */
#ifndef _MALLOC_H_
#define _MALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif
/* XLATON */

/*******************************************************************************
*   Exported Functions and Variables                                           *
*******************************************************************************/
int      heapInit(unsigned);
void *   malloc(unsigned);
void *   realloc(void *, unsigned);
void     free(void *);
unsigned _memfree(void);
unsigned _msize(void *);
int      _validptr(void *);
int      _validptr2(void *, unsigned);
int      _heap_check(void);

extern unsigned  _uHeapMinPtr;/* heap pointers are greater or equal to this.*/
extern unsigned  _uHeapMaxPtr;/* heap pointers are less than this. */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define MINPTR _uHeapMinPtr
#define MAXPTR _uHeapMaxPtr

#define HEAP_SIZE 0x00600000U /* 10MB of heap */

/* HeapPointer assert */
#define ltasserthp(a)         if (!_validptr((void*)(a))){ _ltasserthp((void*)(a), #a,__FILE__,__LINE__); return FALSE;}
#define ltasserthp2(a,b)      if (!_validptr((void*)(a))){ _ltasserthp((void*)(a), #a,__FILE__,__LINE__); b; return FALSE;}
#define _ltasserthp(a,b,c,d)  _kprintf("ltasserthp: pv=%#.8x - '%s' in %s line %d - returns FALSE\n",a,b,c,d)
#define ltasserthps(a,b)          if (!_validptr2((void*)(a),(unsigned)(b))){ _ltasserthsp((void*)(a), (unsigned)(b), #a, #b,__FILE__,__LINE__); return FALSE;}
#define ltasserthps2(a,b,c)       if (!_validptr2((void*)(a),(unsigned)(b))){ _ltasserthsp((void*)(a), (unsigned)(b), #a, #b,__FILE__,__LINE__); c; return FALSE;}
#define _ltasserthps(a,b,c,d,e,f) _kprintf("ltasserthsp: pv=%#.8x cb=%#x - '%s' '%s' in %s line %d - returns FALSE\n",a,b,c,d,e,f)


/* XLATOFF */
#ifdef __cplusplus
}
#endif
#endif /* _MALLOC_H_ */
/* XLATON */

