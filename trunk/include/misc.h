/* $Id: misc.h,v 1.8 1999-09-13 16:28:50 phaller Exp $ */

/*
 * Miscellaneous definitions
 * Debug prototypes and macros
 */


#ifndef __MISC_H__
#define __MISC_H__

#ifndef _OS2WIN_H
  #include <win32type.h>
#endif

#ifdef __cplusplus
      extern "C" {
#endif

/* enable support for the _interrupt() statement */
#if (defined(__IBMCPP__) || defined(__IBMC__))
#  include <builtin.h>
#endif

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


// necessary types
#ifdef ULONG
  #error ULONG definition is bad.
  #define ULONG nope.
#endif
typedef unsigned long ULONG;
typedef unsigned long HMODULE;

#ifndef SYSTEM
#  define SYSTEM _System
#endif


int  SYSTEM WriteLog(char *tekst, ...);

int  SYSTEM WriteLogError(char *tekst, ...);

void SYSTEM CheckVersion(ULONG version, char *modname);

void SYSTEM CheckVersionFromHMOD(ULONG version, HMODULE hModule);

int  SYSTEM DebugErrorBox(ULONG  iErrorCode,
                          char*  pszFormat,
                          ...);


#ifdef __cplusplus
        }
#endif


#include "versionos2.h"
#include "unicode.h"

#endif
