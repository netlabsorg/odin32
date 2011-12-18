/*
 * Miscellaneous definitions
 * Debug prototypes and macros
 */


#ifndef __DBGLOG_H__
#define __DBGLOG_H__

#if !defined(_OS2WIN_H) && !defined(__INCLUDE_WINUSER_H) && !defined(__WINE_WINBASE_H) && !defined(__WINE_WINDEF_H)
  #include <win32type.h>
#endif

#ifdef __cplusplus
      extern "C" {
#endif

#ifdef DEBUG
#define DEBUG_LOGGING 1
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
  #define dprintfNoEOL(a)      WriteLogNoEOL a
#endif
  #define eprintf(a)      WriteLog a
  #define dassert(a, b)   if(!(a)) dprintf b
  #define dbgCheckObj(a)	a->checkObject()
  #define DisableLogging()  DecreaseLogCount()
  #define EnableLogging()   IncreaseLogCount()

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
  #define dprintfNoEOL(a)
  #define eprintf(a)
  #define dassert(a, b)
  #define dbgCheckObj(a)
  #define DisableLogging()
  #define EnableLogging()
#endif


// necessary types
#ifndef __WINE_WINDEF_H
#ifdef ULONG
  #error ULONG definition is bad.
  #define ULONG nope.
#endif
#if !defined(NO_ULONG) && !defined(OS2DEF_INCLUDED)
  typedef unsigned long ULONG;
  typedef unsigned long HMODULE;
#endif
#endif //!__WINE_WINDEF_H

#ifndef SYSTEM
#  define SYSTEM _System
#endif


int  SYSTEM WriteLog(char *tekst, ...);
int  SYSTEM WriteLogNoEOL(char *tekst, ...);
int  SYSTEM WritePrivateLog(void *logfile, char *tekst, ...);

void SYSTEM DecreaseLogCount();
void SYSTEM IncreaseLogCount();

void SYSTEM CheckVersion(ULONG version, char *modname);

void SYSTEM CheckVersionFromHMOD(ULONG version, HMODULE hModule);

int  SYSTEM DebugErrorBox(ULONG  iErrorCode,
                          char*  pszFormat,
                          ...);

//To use private logfiles for dlls, you must have these functions and call
//them when the dll is loaded (open) and the exitlist handler is called (close)
void OpenPrivateLogFiles();
void ClosePrivateLogFiles();

/* enable support for the _interrupt() statement */
#if (defined(__IBMCPP__) || defined(__IBMC__))
#  include <builtin.h>
#ifdef DEBUG
#ifdef __cplusplus

#define DebugInt3() BreakPoint(__FILE__, __FUNCTION__, __LINE__)

void inline BreakPoint(char *szFile, char *szFunction, int iLine)
{
    dprintf(("BREAKPOINT %s %s %d", szFile, szFunction, iLine));
    _interrupt(3);
}

#else
  #define DebugInt3()	_interrupt(3)
#endif
#else
  #define DebugInt3()
#endif

#else
#ifdef DEBUG
  #define DebugInt3()	_asm int 3;
#else
  #define DebugInt3()
#endif

#endif

#ifdef __cplusplus
        }
#endif


#endif //__DBGLOG_H__

