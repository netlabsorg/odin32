/* $Id: environ.cpp,v 1.4 2000-05-10 13:13:32 sandervl Exp $ */

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
#include <string.h>
#include <heapstring.h>

#include <misc.h>

#define DBG_LOCALLOG	DBG_environ
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
LPSTR WIN32API GetEnvironmentStringsA(void)
{
    dprintf(("KERNEL32:  OS2GetEnvironmentStringsA\n"));
    return (LPSTR) O32_GetEnvironmentStrings();
}
//******************************************************************************
//******************************************************************************
LPWSTR WIN32API GetEnvironmentStringsW(VOID)
{
 char *envstrings = (char *)O32_GetEnvironmentStrings();
 char *tmp;
 LPWSTR wenvstrings;
 int len, i;

  dprintf(("KERNEL32:  GetEnvironmentStringsW\n"));

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
BOOL WIN32API FreeEnvironmentStringsA(LPSTR envstrings)
{
  dprintf(("KERNEL32:  FreeEnvironmentStringsA\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FreeEnvironmentStringsW(LPWSTR envstrings)
{
  dprintf(("KERNEL32:  FreeEnvironmentStringsW\n"));
  free(envstrings);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetEnvironmentVariableA(LPCSTR arg1, LPCSTR arg2)
{
    dprintf(("KERNEL32:  SetEnvironmentVariable %s to %s\n", arg1, arg2));
    return O32_SetEnvironmentVariable(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetEnvironmentVariableW(LPCWSTR lpName, LPCWSTR lpValue)
{
 char *asciiname, *asciivalue;
 BOOL  rc;

    dprintf(("KERNEL32:  OS2SetEnvironmentVariableW\n"));
    asciiname  = UnicodeToAsciiString((LPWSTR)lpName);
    asciivalue = UnicodeToAsciiString((LPWSTR)lpValue);
    rc = O32_SetEnvironmentVariable(asciiname, asciivalue);
    FreeAsciiString(asciivalue);
    FreeAsciiString(asciiname);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetEnvironmentVariableA(LPCSTR arg1, LPSTR arg2, DWORD  arg3)
{
    dprintf(("KERNEL32:  GetEnvironmentVariable %s\n", arg1));
    return O32_GetEnvironmentVariable(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetEnvironmentVariableW(LPCWSTR lpName, LPWSTR lpBuffer,
                                       DWORD nSize)
{
  char *astring, *asciibuffer;
  DWORD rc;

    dprintf(("KERNEL32:  OS2GetEnvironmentVariableW\n"));
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

DWORD WIN32API ExpandEnvironmentStringsA( LPCSTR src, LPSTR dst, DWORD count )
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
            if ((p = strchr( src, '%' ))) len = p - src;
            else len = strlen(src);
            var = src;
            src += len;
        }
        else  /* we are at the start of a variable */
        {
            if ((p = strchr( src + 1, '%' )))
            {
                len = p - src - 1;  /* Length of the variable name */	
                if ((var = ENV_FindVariable( GetEnvironmentStringsA(),
                                             src + 1, len )))
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
    dprintf(("KERNEL32:ExpandEnvironmentStringsA returned %s %d",
              dst, total_size));
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

DWORD WIN32API ExpandEnvironmentStringsW(LPCWSTR lpSrc, LPWSTR lpDst, DWORD nSize)
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
//******************************************************************************
