/* $Id: misc.h,v 1.3 1999-05-31 17:04:19 phaller Exp $ */

#ifndef __MISC_H__
#define __MISC_H__

#ifdef __cplusplus
      extern "C" {
#endif

/*
 * Compiler Environment Definition
 */

/* --- WATCOM C --- */
#ifdef __WATCOMC__
  #define CDECL     _cdecl
  #define EXPORT    _export
  #define WIN32API  __stdcall
  #define SYSTEM    __stdcall
#else

/* --- GCC/EMX --- */
#ifdef __GNUC__
  #define CDECL     _cdecl
  #define EXPORT    _export
  #define WIN32API  __stdcall
  #define SYSTEM    __stdcall
#else

/* --- VAC --- */
#ifdef __IBMCPP__
  #define CDECL     _Cdecl
  #define EXPORT    _Export
  #define WIN32API  _System _Export
  #define SYSTEM    _System
#else

/* --- ??? --- */
#error No known compiler.
#endif
#endif
#endif


/* map WINE to ODIN */
#define WINAPI WIN32API
#include <builtin.h>


#ifdef DEBUG
  #define DebugInt3()	_interrupt(3)
#else
  #define DebugInt3()
#endif


#ifdef DEBUG
  #define dprintf(a)      WriteLog a
  #define eprintf(a)      WriteLog a ; WriteLogError a
  #define dassert(a, b)   if(!(a)) WriteLogError b
  #define dbgCheckObj(a)	a->checkObject()
#else
  #define dprintf(a)
  #define eprintf(a)
  #define dassert(a, b)
  #define dbgCheckObj(a)
#endif

int  SYSTEM WriteLog(char *tekst, ...);
int  SYSTEM WriteLogError(char *tekst, ...);
void SYSTEM CheckVersion(ULONG version, char *modname);
void SYSTEM CheckVersionFromHMOD(ULONG version, HMODULE hModule);

#ifdef __cplusplus
        }
#endif


#ifndef _OS2WIN_H
  #include <win32type.h>
#endif

#include "versionos2.h"
#include "unicode.h"

#endif
