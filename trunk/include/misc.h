/* $Id: misc.h,v 1.2 1999-05-27 15:17:58 phaller Exp $ */

#ifndef __MISC_H__
#define __MISC_H__

#ifndef _OS2WIN_H
#include <win32type.h>
#endif

#ifdef __cplusplus
      extern "C" {
#endif

#ifdef __WATCOMC__
#define CDECL _cdecl
#define EXPORT _export
#define WIN32API  __stdcall
#else
#ifndef CDECL
 #define CDECL __cdecl
#endif
#define EXPORT _Export
#define WIN32API  __stdcall /*_Export done in *.def file now */
#include <builtin.h>

#ifdef DEBUG
#define DebugInt3()	_interrupt(3)
#else
#define DebugInt3()
#endif

#endif

#define SYSTEM _System

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

int SYSTEM  WriteLog(char *tekst, ...);
int SYSTEM  WriteLogError(char *tekst, ...);
void SYSTEM CheckVersion(ULONG version, char *modname);
void SYSTEM CheckVersionFromHMOD(ULONG version, HMODULE hModule);

#ifdef __cplusplus
        }
#endif

#include "versionos2.h"
#include "unicode.h"

#endif
