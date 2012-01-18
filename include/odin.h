/*
 * ODIN - Build Environment Definition
 *
 * Copyright (C) 1999 Patrick Haller   <phaller@gmx.net>
 *
 * ------------------------------------------------------------
 * Note: Only compiler linkage definitions and similar stuff
 *       goes here. Nothing else.
 * ------------------------------------------------------------
 *
 */


#ifndef _ODIN_H_
#define _ODIN_H_


/***********************************
 * Compiler Environment Definition *
 ***********************************/

#ifdef CDECL
#  undef CDECL
#endif

#ifdef EXPORT
#  undef EXPORT
#endif

#ifdef WIN32API
#  undef WIN32API
#endif

#ifdef SYSTEM
#  undef SYSTEM
#endif

#ifdef PASCAL
#  undef PASCAL
#endif

#ifdef UNALIGNED
#  undef UNALIGNED
#endif


/* ---------- WATCOM C ---------- */
#ifdef __WATCOMC__
  #define CDECL     _cdecl
  #define EXPORT    _export
  #define WIN32API  __stdcall
  #define WINAPI    __stdcall
  #define SYSTEM    _System
  #define PASCAL    __stdcall
  #ifdef __cplusplus
  #define INLINE    inline
  #else
  #define INLINE    __inline
  #define inline    __inline
  #endif
  #define UNALIGNED
  #define __attribute__(x)

//MN: For some strange reason Watcom doesn't define these for C++!
//    This is not the best place to define them though.
#ifdef __cplusplus
  #define min(a,b)  (((a) < (b)) ? (a) : (b))
  #define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#else

/* ---------- GCC/EMX ---------- */
#ifdef __GNUC__
  #if defined(__GNUC__) && (__GNUC__ <= 3) && (__GNUC_MINOR__ < 2)
    #error You need gcc >= 3.2 to build Odin32
  #endif
  #if !defined(__stdcall__)             /* this is also defined in windef.h if !defined(WIN32OS2) */
    #define __stdcall __attribute__((__stdcall__))
    #define __cdecl   __attribute__((__cdecl__))
  #endif
  #define CDECL     _cdecl
  #define EXPORT    __attribute__ ((dllexport))
  #define WIN32API  __stdcall
  #define WINAPI    __stdcall
  #define PASCAL    __stdcall
  #define INLINE    static __inline__
  #define UNALIGNED
  #if (__GNUC__ < 4)
    #define NONAMELESSUNION
    #define NONAMELESSSTRUCT
  #endif
  #ifdef __INNOTEK_LIBC__
  #define SYSTEM    _System
  #define _LNK_CONV
  #else
  #define SYSTEM    CDECL
  #endif

  #define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      _a < _b ? _a : _b; })
  #define max(a,b) \
    ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      _a > _b ? _a : _b; })

  #define _interrupt(n) __asm__ __volatile__ ("int" #n "\n\tnop")

#else

/* ---------- VAC ---------- */
#if (defined(__IBMCPP__) || defined(__IBMC__))

  #define CDECL     __cdecl
  #define EXPORT    _Export
  #define WIN32API  __stdcall
  #define WINAPI    __stdcall
  #define SYSTEM    _System
  #define PASCAL    __stdcall
  #define UNALIGNED
  #ifndef __cplusplus
    #define INLINE  _Inline
    #ifndef inline
    #define inline  INLINE
    #endif
  #else
    #define INLINE  inline
  #endif
  #define __inline__ INLINE
  #define __attribute__(x)


#ifndef RC_INVOKED
  //Nameless unions or structures are not supported in C mode
  //(nameless unions only in CPP mode and nameless unions only in VAC 3.6.5 CPP mode)
  #ifdef __IBMC__
  #define NONAMELESSUNION
  #define NONAMELESSSTRUCT
  #endif
  #if (__IBMCPP__ == 300)
  #define NONAMELESSSTRUCT
  #endif
#endif

#ifndef RC_INVOKED
  #include <builtin.h>
#endif

#else
#ifdef RC_INVOKED
  //SvL: wrc chokes on calling conventions....
  #define CDECL
  #define EXPORT
  #define WIN32API
  #define WINAPI
  #define CALLBACK
  #define SYSTEM
  #define PASCAL
  #define UNALIGNED
  #define __cdecl
  #define _System
  #define __inline__
  #define INLINE
#else
/* ---------- ??? ---------- */
#error No known compiler.
#endif
#endif
#endif
#endif

#ifdef __cplusplus
#define DEF_VAL(v) = v
#else
#define DEF_VAL(v)
#endif

#ifdef __GNUC__
// __stdcall in GCC for OS/2 incorrectly mangles vararg functions; according to
// MSDN, they should be equal to __cdecl instead of getting the @N suffix
#define WIN32API_VA __cdecl
#else
#define WIN32API_VA WIN32API
#endif

#endif /* _ODIN_H_*/

