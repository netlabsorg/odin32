/********************************************************************/
/*  <umalloc.h> header file                                         */
/*                                                                  */
/*  IBM VisualAge C++ for OS/2, Version 3.00                        */
/*  (C) Copyright IBM Corp. 1991, 1995.                             */
/*  - Licensed Material - Program-Property of IBM                   */
/*  - All rights reserved                                           */
/*                                                                  */
/********************************************************************/

#ifndef __umalloc_h

 #define __umalloc_h

 #ifdef __cplusplus
    extern "C" {
 #endif

 #ifndef __size_t
    #define __size_t
    typedef unsigned int size_t;
 #endif

 #ifndef __HEAP_HDR__
   typedef  struct _Heap *Heap_t;
 #endif

 #ifndef  _LNK_CONV
    #ifdef _M_I386
       #define _LNK_CONV   _Optlink
    #else
       #define _LNK_CONV
    #endif
 #endif

 #ifndef _IMPORT
    #ifdef __IMPORTLIB__
       #define _IMPORT _Import
    #else
       #define _IMPORT
    #endif
 #endif

 #define _HEAP_TILED        0x01
 #define _HEAP_SHARED       0x02
 #define _HEAP_REGULAR      0x04

 #define _BLOCK_CLEAN       1
 #define _FORCE             1
 #define _HEAP_MIN_SIZE     256
 #define _RUNTIME_HEAP      ((Heap_t)_reg_heap)

 #define _POOL_PRESERVE     0x01

 typedef struct __stats
    {
     size_t _provided;
     size_t _used;
     size_t _tiled;
     size_t _shared;
     size_t _max_free;
     size_t _allocs;
     size_t _frees;
     size_t _pool_minsize;
     size_t _pool_maxsize;
     size_t _pool_freebytes;
     size_t _pool_allocs;
     size_t _pool_frees;
    } _HEAPSTATS;

 extern Heap_t _IMPORT _reg_heap;

 #ifndef CRTWRAP
 #define CRTWRAP(a)   odin_##a
 #endif

 #ifdef __DEBUG_ALLOC__
#ifndef ORIGINAL_VAC_FUNCTIONS
         #define _debug_ucalloc    CRTWRAP(_debug_ucalloc)
         #define _debug_umalloc    CRTWRAP(_debug_umalloc)
         #define _debug_uheapmin   CRTWRAP(_debug_uheapmin)
         #define _uheap_check      CRTWRAP(_uheap_check)
         #define _udump_allocated  CRTWRAP(_udump_allocated)
         #define _udump_allocated_delta   CRTWRAP(_udump_allocated_delta)
         #define __uheap_check     CRTWRAP(__uheap_check)
         #define __udump_allocated CRTWRAP(__udump_allocated)
         #define __udump_allocated_delta   CRTWRAP(__udump_allocated_delta)
#endif

    void * _IMPORT _LNK_CONV _debug_umalloc(Heap_t , size_t , const char *,size_t);
    void * _IMPORT _LNK_CONV _debug_ucalloc(Heap_t , size_t, size_t ,const char *,size_t);
    int    _IMPORT _LNK_CONV _debug_uheapmin(Heap_t , const char *, size_t);
    void   _IMPORT _LNK_CONV _uheap_check(Heap_t );
    void   _IMPORT _LNK_CONV _udump_allocated(Heap_t ,int );
    void   _IMPORT _LNK_CONV _udump_allocated_delta(Heap_t ,int );
    void   _IMPORT _LNK_CONV __uheap_check( Heap_t ,const char *,size_t );
    void   _IMPORT _LNK_CONV __udump_allocated( Heap_t, int ,const char *, size_t);
    void   _IMPORT _LNK_CONV __udump_allocated_delta( Heap_t, int, const char *, size_t);

#ifndef ORIGINAL_VAC_FUNCTIONS
    #undef _uheap_check
    #undef _umalloc
    #undef _ucalloc
    #undef _uheapmin
    #undef _udump_allocated
    #undef _udump_allocated_delta
#endif

    #define _uheap_check(h)             __uheap_check((h),__FILE__,__LINE__)
    #define _udump_allocated(h,x)       __udump_allocated((h),(x),__FILE__,__LINE__)
    #define _udump_allocated_delta(h,x) __udump_allocated_delta((h),(x),__FILE__,__LINE__)

    #define _umalloc(h,s)     _debug_umalloc((h),(s),__FILE__,__LINE__)
    #define _ucalloc(h,s,q)   _debug_ucalloc((h),(s),(q),__FILE__,__LINE__)
    #define _uheapmin(h)      _debug_uheapmin((h),__FILE__,__LINE__)

 #else
#ifndef ORIGINAL_VAC_FUNCTIONS
         #define _ucalloc    CRTWRAP(_ucalloc)
         #define _umalloc    CRTWRAP(_umalloc)
         #define _uheapmin   CRTWRAP(_uheapmin)
#endif
    void * _IMPORT _LNK_CONV _umalloc(Heap_t , size_t );
    void * _IMPORT _LNK_CONV _ucalloc(Heap_t , size_t, size_t );
    int    _IMPORT _LNK_CONV _uheapmin(Heap_t);

    #define _uheap_check(h)
    #define _udump_allocated(h,s)
    #define _udump_allocated_delta(h,s)
 #endif

 Heap_t _IMPORT _LNK_CONV _ucreate(void *, size_t, int, int ,
                         void *(* _LNK_CONV rtn_get)(Heap_t,size_t *,int *),
                         void  (* _LNK_CONV rtn_rel)(Heap_t,void *, size_t));

 Heap_t _IMPORT _LNK_CONV _uaddmem(Heap_t , void *, size_t, int);
 Heap_t _IMPORT _LNK_CONV _udefault(Heap_t);
 Heap_t _IMPORT _LNK_CONV _mheap(const void *);

 int _IMPORT _LNK_CONV _udestroy(Heap_t ,int );
 int _IMPORT _LNK_CONV _uopen(Heap_t);
 int _IMPORT _LNK_CONV _uclose(Heap_t);
 int _IMPORT _LNK_CONV _ustats(Heap_t,_HEAPSTATS *);

 int _IMPORT _LNK_CONV _uheap_walk(Heap_t, int (* _LNK_CONV callback)
                                          (const void *, size_t,
                                           int, int, const char *, size_t));


 int  _IMPORT _LNK_CONV _uheapset(Heap_t, unsigned int fill);

 int  _IMPORT _LNK_CONV _uheapchk(Heap_t);
 int  _IMPORT _LNK_CONV _uflush(Heap_t,size_t);
 int  _IMPORT _LNK_CONV _upool(Heap_t,size_t,size_t,size_t,size_t);

 #ifndef __malloc_h
  #define _HEAPOK        (0)
  #define _HEAPEMPTY     (1)
  #define _HEAPBADNODE   (2)
  #define _HEAPBADBEGIN  (3)
  #define _FREEENTRY     (4)
  #define _USEDENTRY     (5)
 #endif

 #ifdef __cplusplus
    }
 #endif

#endif
