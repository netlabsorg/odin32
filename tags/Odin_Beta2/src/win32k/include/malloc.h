/* $Id: malloc.h,v 1.6 2000-01-24 18:18:59 bird Exp $
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
int         _validptr(void *);
int         _validptr2(void *, unsigned);
int         _heap_check(void);


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* HeapPointer assert - old ones... */
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

