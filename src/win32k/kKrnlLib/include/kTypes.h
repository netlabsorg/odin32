/* $Id: kTypes.h,v 1.2 2001-09-27 03:05:22 bird Exp $
 *
 * Common typedefinitions for kLib.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 */

#ifndef _kTypes_h_
#define _kTypes_h_

/*******************************************************************************
*   Common stuff                                                               *
*******************************************************************************/
#ifndef TRUE
#   define TRUE 1
#endif

#ifndef FALSE
#   define FALSE 0
#endif

/**
 * Maximum value a or b.
 */
#define KMAX(a, b)                  (((a) >= (b)) ? (a) : (b))

/**
 * Minimum value a or b.
 */
#define KMIN(a, b)                  (((a) <= (b)) ? (a) : (b))

/**
 * Lowest value which isn't NULL.
 */
#define KMINNOTNULL(a, b)    ((a) != NULL && (b) != NULL ? KMIN(a,b) : ((a) != NULL ? (a) : (b)))


/**
 * Not referenced parameter warning fix.
 */
#define KNOREF(a) (a=a)

/**
 * Aligns something, a, up to nearest alignment boundrary-
 * Note: Aligment must be a 2**n number.
 */
#define KALIGNUP(alignment, a)      (((a) + (alignment - 1)) & ~(alignment - 1))

/**
 * Aligns something, a, down to nearest alignment boundrary-
 * Note: Aligment must be a 2**n number.
 */
#define KALIGNDOWN(alignment, a)    ((a) & ~(alignment - 1))

/**
 * Offset of struct member.
 */
#define KOFFSETOF(type, member)     ((KSIZE)&((type *)0)->member)

/**
 * Determins the number of entires declared in an array.
 */
#define KCENTRIES(arrayvar)         (sizeof(arrayvar) / sizeof((arrayvar)[0]))


#if !defined(RING0) || defined(INCL_16)
    /**
     * Validates that a pointer is within the expected address range.
     */
    #define KVALIDPTR(pv)       (   (KSIZE)(pv) <= (KSIZE)0xc0000000UL \
                                 && (KSIZE)(pv) >= (KSIZE)0x00010000)
    /**
     * Validates that a memory range is within the expected address range.
     */
    #define KVALIDPTRRANGE(pv, cb) ((KSIZE)(pv) + (KSIZE)(cb) >= (KSIZE)(pv) \
                                 && (KSIZE)(pv) + (KSIZE)(cb) <= (KSIZE)0xc0000000UL \
                                 && (KSIZE)(pv) >= (KSIZE)0x00010000UL)

    #ifndef SSToDS
    /**
     * SSToDS converts stack address to data addresses.
     */
    #define SSToDS(p)       ((void*)p)
    #endif
#else
    /**
     * Validates that a pointer is within the expected address range.
     */
    #define KVALIDPTR(pv)       (   (KSIZE)(pv) <= (KSIZE)0xfffe0000UL \
                                 && (KSIZE)(pv) >= (KSIZE)0x00010000UL)
    /**
     * Validates that a memory range is within the expected address range.
     */
    #define KVALIDPTRRANGE(pv, cb) ((KSIZE)(pv) + (KSIZE)(cb) >= (KSIZE)(pv) \
                                 && (KSIZE)(pv) + (KSIZE)(cb) <= (KSIZE)0xfffe0000UL \
                                 && (KSIZE)(pv) >= (KSIZE)0x00010000UL)

    #ifndef SSToDS
    /**
     * SSToDS converts stack address to data addresses.
     */
    #ifdef KKRNLLIB
        extern unsigned long *pulTKSSBase32;
        #define SSToDS(p) ((void*)((unsigned long)(p) + *pulTKSSBase32))
    #else
        extern unsigned long TKSSBase;
        #define SSToDS(p) ((void*)((unsigned long)(p) + TKSSBase))
    #endif
    #endif
#endif


/*******************************************************************************
*   IBM C Compilers                                                            *
*******************************************************************************/
#ifdef __IBMC__

    #define INLINE      _Inline
    #define KLIBCALL    _Optlink
    #ifndef __builtin_h
    void _Builtin       __interrupt( const unsigned int );
    #endif
    #define INT3()      __interrupt(3)

    typedef unsigned long KSIZE;
    typedef unsigned long KBOOL;

#endif


/*******************************************************************************
*   IBM C++ Compilers                                                          *
*******************************************************************************/
#ifdef __IBMCPP__

    #define INLINE      inline
    #define KLIBCALL    _Optlink
    #ifndef __builtin_h
    void _Builtin       __interrupt( const unsigned int );
    #endif
    #define INT3()      __interrupt(3)

    typedef unsigned long KSIZE;
    typedef unsigned long KBOOL;

#endif



#endif
