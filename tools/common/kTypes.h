/* $Id: kTypes.h,v 1.1 2002-02-24 02:47:28 bird Exp $
 *
 * Common typedefinitions for kLib.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 */

#ifndef _kTypes_h_
#define _kTypes_h_

#ifdef __WATCOMC__
    #define KLIBCALL _Optlink
    typedef unsigned long KBOOL;
    typedef unsigned long KSIZE;
    #define INT3()      __interrupt(3)
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


/*******************************************************************************
*   GNU C++ Compilers                                                       *
*******************************************************************************/
#ifdef __GNUC__

    #define INLINE      static inline
    #define KLIBCALL    _Optlink
    INLINE void INT3() { __asm__ __volatile__ ("int3\n\tnop"); }

    typedef unsigned long KSIZE;
    typedef unsigned long KBOOL;

#endif


/*******************************************************************************
*   Common stuff                                                               *
*******************************************************************************/
#ifndef TRUE
#   define TRUE         1
#endif

#ifndef FALSE
#   define FALSE        0
#endif

#ifndef PAGESIZE
#   define PAGESIZE     0x1000
#endif

#ifndef PAGESHIFT
#   define PAGESHIFT    12
#endif

#ifndef NULL
#   ifdef __cplusplus
#       define NULL 0
#   else
#       define NULL ((void *)0)
#   endif
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
#define KALIGNUP(alignment, a)      (((KSIZE)(a) + ((KSIZE)(alignment) - 1)) & ~((KSIZE)(alignment) - 1))

/**
 * Aligns something, a, down to nearest alignment boundrary-
 * Note: Aligment must be a 2**n number.
 */
#define KALIGNDOWN(alignment, a)    ((KSIZE)(a) & ~((KSIZE)(alignment) - 1))

/**
 * Offset of struct member.
 */
#define KOFFSETOF(type, member)     ((KSIZE) &((type *)0)->member )

/**
 * Size of a struct member.
 */
#define KSIZEOF(type, member)       (((KSIZE) sizeof(((type *)0)->member) )

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


/*
 * Assert macro and function.
 */
#define kASSERT(expr) \
    ((expr) ? (void)0 : (kAssertMsg(#expr, __FILE__, __LINE__, __FUNCTION__) ? INT3() : (void)0))

#ifdef __cplusplus
extern "C"
#endif
KBOOL KLIBCALL kAssertMsg(const char *pszExpr, const char *pszFilename, unsigned uLine, const char *pszFunction);



#endif

