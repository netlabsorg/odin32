/* $Id: environ.cpp,v 1.5 2000-09-07 21:40:28 phaller Exp $ */

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

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winnt.h>
#include <winnls.h>
#include <stdlib.h>
#include <string.h>
#include <heapstring.h>

#include <misc.h>

#define DBG_LOCALLOG	DBG_environ
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

ODINDEBUGCHANNEL(KERNEL32-ENVIRONMENT)


//******************************************************************************
//******************************************************************************
ODINFUNCTION0(LPSTR, GetEnvironmentStringsA)
{
  return (LPSTR) O32_GetEnvironmentStrings();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(LPWSTR, GetEnvironmentStringsW)
{
 char *envstrings = (char *)O32_GetEnvironmentStrings();
 char *tmp;
 LPWSTR wenvstrings;
 int len, i;

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
  wenvstrings = (LPWSTR)malloc(len*sizeof(WCHAR));
  for(i=0;
      i<len;
      i++)
  {
     wenvstrings[i] = envstrings[i];
  }
  return(wenvstrings);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, FreeEnvironmentStringsA,
              LPSTR, envstrings)
{
  dprintf(("not correctly implemented.\n"));
  //free(envstrings);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL,   FreeEnvironmentStringsW,
              LPWSTR, envstrings)
{
  dprintf(("not correctly implemented.\n"));
  //free(envstrings);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,   SetEnvironmentVariableA,
              LPCSTR, lpszName,
              LPCSTR, lpszValue)
{
  return O32_SetEnvironmentVariable(lpszName, lpszValue);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,    SetEnvironmentVariableW,
              LPCWSTR, lpszName,
              LPCWSTR, lpszValue)
{
  char *asciiname, *asciivalue;
  BOOL  rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpszName);
  asciivalue = UnicodeToAsciiString((LPWSTR)lpszValue);
  rc = O32_SetEnvironmentVariable(asciiname, asciivalue);
  FreeAsciiString(asciivalue);
  FreeAsciiString(asciiname);
  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD,  GetEnvironmentVariableA,
              LPCSTR, lpName,
              LPSTR,  lpBuffer,
              DWORD,  nSize)
{
  return O32_GetEnvironmentVariable(lpName, lpBuffer, nSize);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD,   GetEnvironmentVariableW,
              LPCWSTR, lpName,
              LPWSTR,  lpBuffer,
              DWORD,   nSize)
{
  char *astring, *asciibuffer;
  DWORD rc;

  asciibuffer = (char *)malloc(nSize+1);
  *asciibuffer = 0;
  astring     = UnicodeToAsciiString((LPWSTR)lpName);

  rc = O32_GetEnvironmentVariable(astring, asciibuffer, nSize);
  AsciiToUnicode(asciibuffer, lpBuffer);
  FreeAsciiString(astring);
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

ODINFUNCTION3(DWORD,  ExpandEnvironmentStringsA,
              LPCSTR, lpSrc,
              LPSTR,  lpDst,
              DWORD,  nSize)
{
    DWORD len, total_size = 1;  /* 1 for terminating '\0' */
    LPCSTR p, var;

    if (!nSize) lpDst = NULL;

    while (*lpSrc)
    {
        if (*lpSrc != '%')
        {
            if ((p = strchr( lpSrc, '%' ))) len = p - lpSrc;
            else len = strlen(lpSrc);
            var = lpSrc;
            lpSrc += len;
        }
        else  /* we are at the start of a variable */
        {
            if ((p = strchr( lpSrc + 1, '%' )))
            {
                len = p - lpSrc - 1;  /* Length of the variable name */	
                if ((var = ENV_FindVariable( GetEnvironmentStringsA(),
                                             lpSrc + 1, len )))
                {
                    lpSrc += len + 2;  /* Skip the variable name */
                    len = strlen(var);
                }
                else
                {
                    var = lpSrc;  /* Copy original name instead */
                    len += 2;
                    lpSrc += len;
                }
            }
            else  /* unfinished variable name, ignore it */
            {
                var = lpSrc;
                len = strlen(lpSrc);  /* Copy whole string */
                lpSrc += len;
            }
        }
        total_size += len;
        if (lpDst)
        {
            if (nSize < len) len = nSize;
            memcpy( lpDst, var, len );
            lpDst += len;
            nSize -= len;
        }
    }

    /* Null-terminate the string */
    if (lpDst)
    {
        if (!nSize) lpDst--;
        *lpDst = '\0';
    }
    dprintf(("KERNEL32:ExpandEnvironmentStringsA returned %s %d",
              lpDst, total_size));
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

ODINFUNCTION3(DWORD,  ExpandEnvironmentStringsW,
              LPCWSTR,lpSrc,
              LPWSTR, lpDst,
              DWORD,  nSize)
{
    LPSTR srcA = HEAP_strdupWtoA( GetProcessHeap(), 0, lpSrc );
    LPSTR dstA = lpDst ? (LPSTR)HeapAlloc( GetProcessHeap(), 0, nSize ) : NULL;

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
//******************************************************************************
