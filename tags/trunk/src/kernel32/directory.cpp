/* $Id: directory.cpp,v 1.5 1999-09-28 01:17:01 phaller Exp $ */

/*
 * Win32 Directory functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2win.h>
#include <stdlib.h>
#include <unicode.h>
#include <heapstring.h>


ODINDEBUGCHANNEL(KERNEL32-DIRECTORY)


/*****************************************************************************
 * Name      : GetCurrentDirectoryA
 * Purpose   : query the current directory
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT, GetCurrentDirectoryA, UINT,  nBufferLength,
                                          LPSTR, lpBuffer)
{
  return O32_GetCurrentDirectory(nBufferLength, lpBuffer);
}


/*****************************************************************************
 * Name      : GetCurrentDirectoryW
 * Purpose   : query the current directory
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT, GetCurrentDirectoryW, UINT,   nBufferLength,
                                          LPWSTR, lpBuffer)
{
  char *asciidir = (char *)malloc(nBufferLength+1);
  int  rc;

  rc = O32_GetCurrentDirectory(nBufferLength, asciidir);
  if(rc != 0)
    AsciiToUnicode(asciidir, lpBuffer);
  free(asciidir);
  return(rc);
}


/*****************************************************************************
 * Name      : SetCurrentDirectoryA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL,SetCurrentDirectoryA,LPCSTR,lpPathName)
{
  return O32_SetCurrentDirectory((LPSTR)lpPathName);
}


/*****************************************************************************
 * Name      : SetCurrentDirectoryW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL,SetCurrentDirectoryW,LPCWSTR,lpPathName)
{
  char *asciipath;
  BOOL  rc;

  asciipath = UnicodeToAsciiString((LPWSTR)lpPathName);
  rc = SetCurrentDirectoryA(asciipath);
  FreeAsciiString(asciipath);
  return(rc);
}


/*****************************************************************************
 * Name      : CreateDirectoryA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(BOOL,CreateDirectoryA,LPCSTR,              arg1,
                                    PSECURITY_ATTRIBUTES,arg2)
{
  return O32_CreateDirectory(arg1, arg2);
}


/*****************************************************************************
 * Name      : CreateDirectoryW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(BOOL,CreateDirectoryW,LPCWSTR,             arg1,
                                    PSECURITY_ATTRIBUTES,arg2)
{
  BOOL  rc;
  char *astring;

  astring = UnicodeToAsciiString((LPWSTR)arg1);
  rc = CreateDirectoryA(astring, arg2);
  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      : GetSystemDirectoryA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetSystemDirectoryA,LPSTR,arg1,
                                       UINT, arg2)
{
  LPSTR lpstrEnv = getenv("WIN32.DIR.SYSTEM");          /* query environment */

  if (lpstrEnv != NULL)
  {
    lstrcpynA(arg1,                   /* copy environment variable to buffer */
              lpstrEnv,
              arg2);
    return (lstrlenA(arg1));                /* return number of copies bytes */
  }
  else
                               /* if no override by environment is available */
    return O32_GetSystemDirectory(arg1, arg2);
}


/*****************************************************************************
 * Name      : GetSystemDirectoryW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetSystemDirectoryW,LPWSTR,lpBuffer,
                                       UINT,  uSize)
{
  char *asciibuffer = (char *)malloc(uSize+1);
  UINT  rc;

  rc = GetSystemDirectoryA(asciibuffer, uSize);
  if(rc) AsciiToUnicode(asciibuffer, lpBuffer);
  free(asciibuffer);
  return(rc);
}


/*****************************************************************************
 * Name      : GetWindowsDirectoryA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetWindowsDirectoryA,LPSTR,arg1,
                                        UINT, arg2)
{
  LPSTR lpstrEnv = getenv("WIN32.DIR.WINDOWS");         /* query environment */

  if (lpstrEnv != NULL)
  {
    lstrcpynA(arg1,                   /* copy environment variable to buffer */
              lpstrEnv,
              arg2);
    return (lstrlenA(arg1));                /* return number of copies bytes */
  }
  else
                               /* if no override by environment is available */
    return O32_GetWindowsDirectory(arg1,
                                   arg2);
}


/*****************************************************************************
 * Name      : GetWindowsDirectoryW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetWindowsDirectoryW,LPWSTR,lpBuffer,
                                        UINT,  uSize)
{
  char *asciibuffer = (char *)malloc(uSize+1);
  UINT  rc;

  rc = GetWindowsDirectoryA(asciibuffer, uSize);
  AsciiToUnicode(asciibuffer, lpBuffer);
  free(asciibuffer);
  return(rc);
}


/*****************************************************************************
 * Name      : RemoveDirectoryA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL,RemoveDirectoryA,LPCSTR,arg1)
{
  return O32_RemoveDirectory(arg1);
}


/*****************************************************************************
 * Name      : RemoveDirectoryW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL,RemoveDirectoryW,LPCWSTR,lpPathName)
{
  char *asciipath;
  BOOL  rc;

  asciipath = UnicodeToAsciiString((LPWSTR)lpPathName);
  rc = RemoveDirectoryA(asciipath);
  FreeAsciiString(asciipath);
  return(rc);
}

