/* $Id: misc.h,v 1.6 1999-06-01 15:46:21 phaller Exp $ */

/*
 * Miscellaneous definitions
 * Debug prototypes and macros
 */


#ifndef __MISC_H__
#define __MISC_H__

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


#ifndef ULONG
#  define ULONG unsigned long
#endif

#ifndef HMODULE
#  define HMODULE ULONG
#endif

#ifndef SYSTEM
#  define SYSTEM _System
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
