/* $Id: environ.cpp,v 1.15 2002-02-10 13:12:51 sandervl Exp $ */

/*
 * Win32 environment file functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1998 Knut St. Osmundsen
 *
 * Parts based on Wine code (ExpandEnvironmentStringsA/W)
 * (memory\environ.c; 991114)
 *
 * Copyright 1996, 1998 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winnt.h>
#include <winnls.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <heapstring.h>

#include <misc.h>
#include <unicode.h>

#define DBG_LOCALLOG	DBG_environ
#include "dbglocal.h"


ODINDEBUGCHANNEL(KERNEL32-ENVIRONMENT)


//list of important OS/2 environment variables that must not be removed
//when creating a new process
static char *lpReservedEnvStrings[] = {
"HOSTNAME",
"TZ",
"USE_HOSTS_FIRST",
"MMBASE",
"USER_INI",
"SYSTEM_INI",
"DPATH",
"LANG",
"NCDEBUG",
"NLSPATH",
"TCPLANG",
"DLSINI",
"INIT_FILE_NAMES",
"INIT_FILE_RANGES",
"NWDBPATH",
"ETC",
"WP_OBJHANDLE",
"SOMIR",
"SOMDDIR",
"TMP",
"TEMP",
};

//******************************************************************************
//******************************************************************************
void InitEnvironment()
{
  CHAR szVar[512];
  static BOOL fInit = FALSE;

  if(fInit) return;

  //TEMP is a standard environment variable in Windows, but is not always
  //present in OS/2, so make sure it is.
  if(GetEnvironmentVariableA("TEMP", szVar, sizeof(szVar)) == 0) 
  {
      if(GetEnvironmentVariableA("TMP", szVar, sizeof(szVar)) == 0) {
          //then we just use the windows directory for garbage
          GetWindowsDirectoryA(szVar, sizeof(szVar));
      }
      SetEnvironmentVariableA("TEMP", szVar);
  }
}
//******************************************************************************
//******************************************************************************
LPSTR WIN32API GetEnvironmentStringsA()
{
  InitEnvironment();
  return (LPSTR) O32_GetEnvironmentStrings();
}
//******************************************************************************
//******************************************************************************
LPWSTR WIN32API GetEnvironmentStringsW()
{
  char *envstrings = (char *)O32_GetEnvironmentStrings();
  char *tmp;
  LPWSTR wenvstrings;
  int len, i;

  InitEnvironment();

  if(envstrings == NULL)
    return(NULL);

  tmp = envstrings;
  len = 0;
  while(*tmp != 0)
  {
    len += strlen(tmp)+1;
    tmp = envstrings + len;
  }
  len++;        //terminating 0
  wenvstrings = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
  for(i=0;i<len;i++)
  {
     wenvstrings[i] = envstrings[i];
  }
  return(wenvstrings);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FreeEnvironmentStringsA(LPSTR envstrings)
{
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FreeEnvironmentStringsW(LPWSTR envstrings)
{
  HeapFree(GetProcessHeap(), 0, envstrings);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetEnvironmentVariableA(LPCSTR lpName, LPCSTR lpValue)
{
  dprintf(("KERNEL32:  SetEnvironmentVariable %s to %s\n", lpName, lpValue));
  return O32_SetEnvironmentVariable(lpName, lpValue);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetEnvironmentVariableW(LPCWSTR lpName, LPCWSTR lpValue)
{
  char *asciiname, *asciivalue;
  BOOL  rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpName);
  asciivalue = UnicodeToAsciiString((LPWSTR)lpValue);
  dprintf(("KERNEL32:  SetEnvironmentVariable %s to %s\n", asciiname, asciivalue));
  rc = O32_SetEnvironmentVariable(asciiname, asciivalue);
  FreeAsciiString(asciivalue);
  FreeAsciiString(asciiname);
  return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetEnvironmentVariableA(LPCSTR lpName, LPSTR lpBuffer,
                                       DWORD nSize)
{
  dprintf(("GetEnvironmentVariableA %s", lpName));
  return O32_GetEnvironmentVariable(lpName, lpBuffer, nSize);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetEnvironmentVariableW(LPCWSTR lpName, LPWSTR lpBuffer,
                                       DWORD nSize)
{
  char *astring, *asciibuffer;
  DWORD rc;

  if (!lpName || !*lpName)
  {
      dprintf(("GetEnvironmentVariableW: invalid name!"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
  }

  if(nSize) {
       asciibuffer = (char *)malloc(nSize+1);
       *asciibuffer = 0;
  }
  else asciibuffer = NULL;

  astring     = UnicodeToAsciiString((LPWSTR)lpName);

  rc = GetEnvironmentVariableA(astring, asciibuffer, nSize);
  if(asciibuffer)
      AsciiToUnicode(asciibuffer, lpBuffer);
  FreeAsciiString(astring);
  if(asciibuffer)
	free(asciibuffer);
  return(rc);
}
/***********************************************************************
 *           ENV_FindVariable
 *
 * Find a variable in the environment and return a pointer to the value.
 * Helper function for GetEnvironmentVariable and ExpandEnvironmentStrings.
 */
static LPCSTR ENV_FindVariable( LPCSTR env, LPCSTR name, INT len )
{
    while (*env)
    {
        if (!lstrncmpiA( name, env, len ) && (env[len] == '='))
            return env + len + 1;
        env += strlen(env) + 1;
    }
    return NULL;
}
/*****************************************************************************
 * Name      : DWORD WIN32API ExpandEnvironmentStringsA
 * Purpose   : The ExpandEnvironmentStringsA function expands environment-variable
 *             strings and replaces them with their defined values.
 * Parameters: LPCSTR lpSrc  pointer to string with environment variables
 *             LPSTR lpDst   pointer to string with expanded environment variables
 *             DWORD nSize   maximum characters in expanded string
 * Variables :
 * Result    : If the function succeeds, the return value is the number of
 *             characters stored in the destination buffer. If the number of
 *             characters is greater than the size of the destination buffer,
 *             the return value is the size of the buffer required to hold
 *             the expanded strings.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : 
 *
 *****************************************************************************/

DWORD WIN32API ExpandEnvironmentStringsA(LPCSTR src, LPSTR dst, DWORD count)
{
  DWORD len, total_size = 1;  /* 1 for terminating '\0' */
  LPCSTR p, var;

  dprintf(("KERNEL32:ExpandEnvironmentStringsA '%s', %08x, %08x",
           src, dst, count
          ));

    if (!count) dst = NULL;

    while (*src)
    {
        if (*src != '%')
        {
            if ((p = strchr( src, '%' )) != NULL) len = p - src;
            else len = strlen(src);
            var = src;
            src += len;
        }
        else  /* we are at the start of a variable */
        {
            if ((p = strchr( src + 1, '%' )) != NULL)
            {
                len = p - src - 1;  /* Length of the variable name */	
                if ((var = ENV_FindVariable( GetEnvironmentStringsA(),
                                             src + 1, len )) != NULL)
                {
                    src += len + 2;  /* Skip the variable name */
                    len = strlen(var);
                }
                else
                {
                    var = src;  /* Copy original name instead */
                    len += 2;
                    src += len;
                }
            }
            else  /* unfinished variable name, ignore it */
            {
                var = src;
                len = strlen(src);  /* Copy whole string */
                src += len;
            }
        }
        total_size += len;
        if (dst)
        {
            if (count < len) len = count;
            memcpy( dst, var, len );
            dst += len;
            count -= len;
        }
    }

    /* Null-terminate the string */
    if (dst)
    {
        if (!count) dst--;
        *dst = '\0';
    }
    return total_size;
}

/*****************************************************************************
 * Name      : DWORD WIN32API ExpandEnvironmentStringsW
 * Purpose   : The ExpandEnvironmentStringsA function expands environment-variable
 *             strings and replaces them with their defined values.
 * Parameters: LPCWSTR lpSrc  pointer to string with environment variables
 *             LPWSTR lpDst   pointer to string with expanded environment variables
 *             DWORD nSize   maximum characters in expanded string
 * Variables :
 * Result    : If the function succeeds, the return value is the number of
 *             characters stored in the destination buffer. If the number of
 *             characters is greater than the size of the destination buffer,
 *             the return value is the size of the buffer required to hold
 *             the expanded strings.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : 
 *
 *****************************************************************************/

DWORD WIN32API  ExpandEnvironmentStringsW(LPCWSTR lpSrc, LPWSTR lpDst,
                                          DWORD nSize)
{
  LPSTR srcA = HEAP_strdupWtoA( GetProcessHeap(), 0, lpSrc );
  LPSTR dstA = lpDst ? (LPSTR)HeapAlloc( GetProcessHeap(), 0, nSize ) : NULL;

  dprintf(("KERNEL32:ExpandEnvironmentStringsW(%08x,%08x,%08x)", lpSrc, lpDst, nSize));

  DWORD ret  = ExpandEnvironmentStringsA( srcA, dstA, nSize );
  if (dstA)
    {
      lstrcpyAtoW( lpDst, dstA );
      HeapFree( GetProcessHeap(), 0, dstA );
    }
  HeapFree( GetProcessHeap(), 0, srcA );
  return ret;
}
//******************************************************************************
// Create a new process environment block based on input from the application
// Make sure important OS/2 variables are added or else some services might
// fail in the child process. (gethostname relies on SET HOSTNAME)
//******************************************************************************
char *CreateNewEnvironment(char *lpEnvironment)
{
    char *tmpenvold = lpEnvironment;
    char *tmpenvnew, *newenv;
    int newsize = 0, len;
        
    dprintf(("New environment:"));
    while(*tmpenvold) {
        dprintf(("%s", tmpenvold));
        len        = strlen(tmpenvold);
        newsize   += len+1;
        tmpenvold += len+1;
    }
    newsize++; //extra null terminator

    for(int i=0;i<sizeof(lpReservedEnvStrings)/sizeof(char *);i++) {
        if(!ENV_FindVariable(lpEnvironment, lpReservedEnvStrings[i], strlen(lpReservedEnvStrings[i]))) {
            len = GetEnvironmentVariableA(lpReservedEnvStrings[i], NULL, 0);
            if(len) {
                newsize += strlen(lpReservedEnvStrings[i]) + 1 + len+1;  //var = value \0
            }
        }
    }
    newsize++; //extra 0 terminator

    newenv = (char *)malloc(newsize);
    if(newenv == NULL) {
        DebugInt3();
        return NULL;
    }
    memset(newenv, 0, newsize);
    tmpenvold  = (char *)lpEnvironment;
    tmpenvnew = newenv;
    while(*tmpenvold) {
        strcat(tmpenvnew, tmpenvold);
        len = strlen(tmpenvnew);
        tmpenvnew += len+1;
        tmpenvold += len+1;
    }
    for(i=0;i<sizeof(lpReservedEnvStrings)/sizeof(char *);i++) {
        if(!ENV_FindVariable(lpEnvironment, lpReservedEnvStrings[i], strlen(lpReservedEnvStrings[i]))) {
            len = GetEnvironmentVariableA(lpReservedEnvStrings[i], NULL, 0);
            if(len) {
                char *tmp = (char *)malloc(len+1);
                len = GetEnvironmentVariableA(lpReservedEnvStrings[i], tmp, len+1);
                if(len) {
                    sprintf(tmpenvnew, "%s=%s", lpReservedEnvStrings[i], tmp);
                    tmpenvnew += strlen(tmpenvnew) + 1;
                }
                free(tmp);
            }
        }
    }
    *tmpenvnew = 0; //final null terminator

#ifdef DEBUG
    tmpenvnew = newenv;
    dprintf(("Combined new environment:"));
    while(*tmpenvnew) {
        dprintf(("%s", tmpenvnew));
        len        = strlen(tmpenvnew);
        tmpenvnew += len+1;
    }
#endif
    return newenv;
}
//******************************************************************************
//******************************************************************************
