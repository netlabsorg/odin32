/* $Id: odin.h,v 1.5 1999-08-12 23:33:36 phaller Exp $ */
/*
 *
 * ODIN - Build Environment Definition
 *
 */


#ifndef _ODIN_H_
#define _ODIN_H_


/*
 * Compiler Environment Definition
 */

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
  #define SYSTEM    __stdcall
  #define PASCAL    _Pascal
  #define UNALIGNED
#else

/* ---------- GCC/EMX ---------- */
#ifdef __GNUC__
  #define CDECL     _cdecl
  #define EXPORT    _export
  #define WIN32API  __stdcall
  #define WINAPI    __stdcall
  #define SYSTEM    __stdcall
  #define PASCAL    _Pascal
  #define UNALIGNED
#else

/* ---------- VAC ---------- */
#if (defined(__IBMCPP__) || defined(__IBMC__))
  #define CDECL     __cdecl
  #define EXPORT    _Export
  #define WIN32API  __stdcall
  #define WINAPI    __stdcall
  #define SYSTEM    _System
  #define PASCAL    _Pascal
  #define UNALIGNED
#else

/* ---------- ??? ---------- */
#error No known compiler.
#endif
#endif
#endif


#include <builtin.h>


#endif /* _ODIN_H_*/

