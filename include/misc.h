/* $Id: misc.h,v 1.12 1999-12-12 14:31:29 sandervl Exp $ */

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
#ifdef PRIVATE_LOGGING
  //To use private dll logging, define PRIVATE_LOGGING and
  //add Open/ClosePrivateLogFiles (see below) functions to the dll
  //to open close the private logfile. The logfile handle should
  //be stored in the _privateLogFile variable
  //dprintf can be called like this:
  //dprintf((LOG, "PE file           : %s", szFileName));
  #define LOG		  (void*)_privateLogFile
  #define dprintf(a)      WritePrivateLog a
  #define dprintfGlobal(a)      WriteLog a
#else
  #define dprintf(a)      WriteLog a
#endif
  #define eprintf(a)      WriteLog a ; WriteLogError a
  #define dassert(a, b)   if(!(a)) WriteLogError b
  #define dbgCheckObj(a)	a->checkObject()

#ifdef DEBUG_ENABLELOG_LEVEL2
#ifdef PRIVATE_LOGGING
  #define dprintf2(a)      WritePrivateLog a
#else
  #define dprintf2(a)      WriteLog a
#endif
#else
  #define dprintf2(a)
#endif

#else
  #define dprintfGlobal(a)
  #define dprintf(a)
  #define dprintf2(a)
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
int  SYSTEM WritePrivateLog(void *logfile, char *tekst, ...);

int  SYSTEM WriteLogError(char *tekst, ...);

void SYSTEM CheckVersion(ULONG version, char *modname);

void SYSTEM CheckVersionFromHMOD(ULONG version, HMODULE hModule);

int  SYSTEM DebugErrorBox(ULONG  iErrorCode,
                          char*  pszFormat,
                          ...);

//To use private logfiles for dlls, you must have these functions and call
//them when the dll is loaded (open) and the exitlist handler is called (close)
void OpenPrivateLogFiles();
void ClosePrivateLogFiles();

#ifdef __cplusplus
        }
#endif


#include "versionos2.h"
#include "unicode.h"

#endif
