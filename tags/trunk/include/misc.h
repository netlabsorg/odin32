/* $Id: misc.h,v 1.10 1999-11-09 19:21:36 sandervl Exp $ */

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

#ifdef DEBUG_ENABLELOG_LEVEL2
  #define dprintf2(a)     WriteLog a
#else
  #define dprintf2(a)
#endif

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
#ifndef NO_ULONG
  typedef unsigned long ULONG;
  typedef unsigned long HMODULE;
#endif

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
