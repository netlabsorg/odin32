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

/* ---------- WATCOM C ---------- */
#ifdef __WATCOMC__
  #define CDECL     _cdecl
  #define EXPORT    _export
  #define WIN32API  __stdcall
  #define SYSTEM    __stdcall
#else

/* ---------- GCC/EMX ---------- */
#ifdef __GNUC__
  #define CDECL     _cdecl
  #define EXPORT    _export
  #define WIN32API  __stdcall
  #define SYSTEM    __stdcall
#else

/* ---------- VAC ---------- */
#if (defined(__IBMCPP__) || defined(__IBMC__))
  #define CDECL     __cdecl
  #define EXPORT    _Export
  #define WIN32API  __stdcall
  #define SYSTEM    _System
  #define UNALIGNED
#else

/* ---------- ??? ---------- */
#error No known compiler.
#endif
#endif
#endif


/* map WINE to ODIN */
#define WINAPI WIN32API

#include <builtin.h>


#endif /* _ODIN_H_*/

