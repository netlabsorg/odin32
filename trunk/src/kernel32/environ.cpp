/* $Id: environ.cpp,v 1.11 2001-04-26 17:01:58 sandervl Exp $ */

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
#include <unicode.h>

#define DBG_LOCALLOG	DBG_environ
#include "dbglocal.h"


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
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, FreeEnvironmentStringsW,
              LPWSTR, envstrings)
{
  free(envstrings);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SetEnvironmentVariableA,
              LPCSTR, lpName,
              LPCSTR, lpValue)
{
  dprintf(("KERNEL32:  SetEnvironmentVariable %s to %s\n", lpName, lpValue));
  return O32_SetEnvironmentVariable(lpName, lpValue);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SetEnvironmentVariableW,
              LPCWSTR, lpName,
              LPCWSTR, lpValue)
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
ODINFUNCTION3(DWORD,  GetEnvironmentVariableA,
              LPCSTR, lpName,
              LPSTR,  lpBuffer,
              DWORD,  nSize)
{
  dprintf(("GetEnvironmentVariableA %s", lpName));
  return O32_GetEnvironmentVariable(lpName, lpBuffer, nSize);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, GetEnvironmentVariableW,
              LPCWSTR, lpName,
              LPWSTR,  lpBuffer,
              DWORD,   nSize)
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

  rc = CALL_ODINFUNC(GetEnvironmentVariableA)(astring, asciibuffer, nSize);
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

ODINFUNCTION3(DWORD,  ExpandEnvironmentStringsA,
              LPCSTR, src,
              LPSTR,  dst,
              DWORD,  count)
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

ODINFUNCTION3(DWORD,   ExpandEnvironmentStringsW,
              LPCWSTR, lpSrc,
              LPWSTR,  lpDst,
              DWORD,   nSize)
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
