/* $Id: directory.cpp,v 1.19 2000-04-14 22:35:26 sandervl Exp $ */

/*
 * Win32 Directory functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 * NOTE: Directory creation has to be done in install program (odin\win) 
 *
 * Parts based on Wine code (991031) (files\directory.c)
 *
 * DOS directories functions
 *
 * Copyright 1995 Alexandre Julliard
 *
 * TODO: System/window directories should be created by install program!
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
#include <options.h>
#include "initterm.h"
#include <win\file.h>
#include <string.h>
#include "oslibdos.h"
#include "profile.h"

#define DBG_LOCALLOG	DBG_directory
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-DIRECTORY)


static char DIR_Windows[MAX_PATHNAME_LEN];
static char DIR_System[MAX_PATHNAME_LEN];

//******************************************************************************
//******************************************************************************
char *InternalGetWindowsDirectoryA()
{
   return DIR_Windows;
}
//******************************************************************************
//******************************************************************************
char *InternalGetSystemDirectoryA()
{
   return DIR_System;
}
//******************************************************************************
//******************************************************************************
void InitDirectories()
{
   	GetWindowsDirectoryA((LPSTR)&DIR_Windows, sizeof(DIR_Windows));
   	GetSystemDirectoryA((LPSTR)&DIR_System, sizeof(DIR_System));
}

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
  if(HIWORD(lpPathName) == 0) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  int len = strlen(lpPathName);
  char *tmp=(char *)alloca(len + 1);

  strcpy(tmp, lpPathName);
  //SvL: Don't remove terminating backslash if it wants to chdir to root dir
  if(tmp[len -1] == '\\' && len != 1)
    tmp[len -1] = 0;

  dprintf(("SetCurrentDirectoryA %s", tmp));
  return O32_SetCurrentDirectory((LPSTR)tmp);
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

ODINFUNCTION2(BOOL,CreateDirectoryA,LPCSTR, arg1,PSECURITY_ATTRIBUTES,arg2)
{
  int len = strlen(arg1);
  char *tmp=(char *)alloca(len + 1);

  strcpy(tmp, arg1);
  if(tmp[len -1] == '\\')
    tmp[len -1] = 0;
  return O32_CreateDirectory(tmp, arg2);
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
 * Remark    : Should return length of system dir even if lpBuffer == NULL
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetSystemDirectoryA,LPSTR,lpBuffer,
                                       UINT,uSize)
{
  char buf[255];
  int  len;

     	lstrcpynA(buf, kernel32Path, sizeof(buf)-1);
	len = lstrlenA(buf);;
	if(buf[len-1] == '\\') {
		buf[len-1] = 0; 
		len--;
	}
	if(lpBuffer)
		lstrcpynA(lpBuffer, buf, uSize);
	return len;
}


/*****************************************************************************
 * Name      : GetSystemDirectoryW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Should return length of system dir even if lpBuffer == NULL
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetSystemDirectoryW,LPWSTR,lpBuffer,
                                       UINT,  uSize)
{
  char *asciibuffer = NULL;
  UINT  rc;

  if(lpBuffer)
  	asciibuffer = (char *)malloc(uSize+1);

  rc = GetSystemDirectoryA(asciibuffer, uSize);
  if(rc && asciibuffer)
    	AsciiToUnicode(asciibuffer, lpBuffer);

  if(asciibuffer)
	free(asciibuffer);
  return(rc);
}


/*****************************************************************************
 * Name      : GetWindowsDirectoryA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Should return length of system dir even if lpBuffer == NULL
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetWindowsDirectoryA,LPSTR,lpBuffer,
                                        UINT,uSize)
{
  static int fWindirExists = FALSE;
  CHAR  buf[255];
  int   len;
  
	//SvL: Use path of kernel32.dll instead of calling Open32 api (which returns \OS2\SYSTEM)
	lstrcpynA(buf, kernel32Path, sizeof(buf)-1);
	strcat(buf, "WIN");
	if(!fWindirExists) {
		O32_CreateDirectory(buf, NULL);
		fWindirExists = TRUE;
	}

	len = lstrlenA(buf);;
	if(buf[len-1] == '\\') {
		buf[len-1] = 0; 
		len--;
	}
	if(lpBuffer)
		lstrcpynA(lpBuffer, buf, uSize);
	return len;
}


/*****************************************************************************
 * Name      : GetWindowsDirectoryW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Should return length of system dir even if lpBuffer == NULL
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT,GetWindowsDirectoryW,LPWSTR,lpBuffer,
                                        UINT,  uSize)
{
  char *asciibuffer = NULL;
  UINT  rc;

  if(lpBuffer)
  	asciibuffer = (char *)malloc(uSize+1);

  rc = GetWindowsDirectoryA(asciibuffer, uSize);
  if(rc && asciibuffer)
    	AsciiToUnicode(asciibuffer, lpBuffer);

  if(asciibuffer)
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
  int len = strlen(arg1);
  char *tmp=(char *)alloca(len + 1);

  strcpy(tmp, arg1);
  if(tmp[len -1] == '\\')
    tmp[len -1] = 0;

  dprintf(("RemoveDirectory %S", arg1));

  return O32_RemoveDirectory(tmp);
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

/***********************************************************************
 *           DIR_TryModulePath
 *
 * Helper function for DIR_SearchPath.
 */
static BOOL DIR_TryModulePath( LPCSTR name, char *full_name )
{
    char buffer[OFS_MAXPATHNAME];
    LPSTR p;

    if (!GetModuleFileNameA( 0, buffer, sizeof(buffer) ))
	buffer[0]='\0';

    if (!(p = strrchr( buffer, '\\' ))) return FALSE;
    if (sizeof(buffer) - (++p - buffer) <= strlen(name)) return FALSE;
    strcpy( p, name );

    return OSLibDosSearchPath(OSLIB_SEARCHFILE, NULL, buffer, full_name, MAX_PATHNAME_LEN);
}


/***********************************************************************
 *           DIR_SearchPath
 *
 * Implementation of SearchPath32A. 'win32' specifies whether the search
 * order is Win16 (module path last) or Win32 (module path first).
 *
 * FIXME: should return long path names.
 */
DWORD DIR_SearchPath( LPCSTR path, LPCSTR name, LPCSTR ext,
                      char *full_name )
{
    DWORD len;
    LPCSTR p;
    LPSTR tmp = NULL;
    BOOL ret = TRUE;

    /* First check the supplied parameters */

    p = strrchr( name, '.' );
    if (p && !strchr( p, '/' ) && !strchr( p, '\\' ))
        ext = NULL;  /* Ignore the specified extension */
    if ((*name && (name[1] == ':')) ||
        strchr( name, '/' ) || strchr( name, '\\' ))
        path = NULL;  /* Ignore path if name already contains a path */
    if (path && !*path) path = NULL;  /* Ignore empty path */

    len = strlen(name);
    if (ext) len += strlen(ext);
    if (path) len += strlen(path) + 1;

    /* Allocate a buffer for the file name and extension */

    if (path || ext)
    {
        if (!(tmp = (LPSTR)HeapAlloc( GetProcessHeap(), 0, len + 1 )))
        {
            SetLastError( ERROR_OUTOFMEMORY );
            return 0;
        }
        if (path)
        {
            strcpy( tmp, path );
            strcat( tmp, "\\" );
            strcat( tmp, name );
        }
        else strcpy( tmp, name );
        if (ext) strcat( tmp, ext );
        name = tmp;
    }
    
    /* If we have an explicit path, everything's easy */

    if (path || (*name && (name[1] == ':')) ||
        strchr( name, '/' ) || strchr( name, '\\' ))
    {
	ret = OSLibDosSearchPath(OSLIB_SEARCHFILE, NULL, (LPSTR)name, full_name, MAX_PATHNAME_LEN);
        goto done;
    }

    /* Try the path of the current executable (for Win32 search order) */
    if (DIR_TryModulePath( name, full_name )) goto done;

    /* Try the current directory */
    if (OSLibDosSearchPath(OSLIB_SEARCHCURDIR, NULL, (LPSTR)name, full_name, MAX_PATHNAME_LEN))
	goto done;

    /* Try the Windows system directory */
    if (OSLibDosSearchPath(OSLIB_SEARCHDIR, (LPSTR)&DIR_System, (LPSTR)name, full_name, MAX_PATHNAME_LEN))
        goto done;

    /* Try the Windows directory */
    if (OSLibDosSearchPath(OSLIB_SEARCHDIR, (LPSTR)&DIR_Windows, (LPSTR)name, full_name, MAX_PATHNAME_LEN))
        goto done;

    /* Try all directories in path */
    ret = OSLibDosSearchPath(OSLIB_SEARCHENV, "PATH", (LPSTR)name, full_name, MAX_PATHNAME_LEN);

done:
    if (tmp) HeapFree( GetProcessHeap(), 0, tmp );
    return ret;
}


/***********************************************************************
 * SearchPath32A [KERNEL32.447]
 *
 * Searches for a specified file in the search path.
 *
 * PARAMS
 *    path	[I] Path to search
 *    name	[I] Filename to search for.
 *    ext	[I] File extension to append to file name. The first
 *		    character must be a period. This parameter is
 *                  specified only if the filename given does not
 *                  contain an extension.
 *    buflen	[I] size of buffer, in characters
 *    buffer	[O] buffer for found filename
 *    lastpart  [O] address of pointer to last used character in
 *                  buffer (the final '\')
 *
 * RETURNS
 *    Success: length of string copied into buffer, not including
 *             terminating null character. If the filename found is
 *             longer than the length of the buffer, the length of the
 *             filename is returned.
 *    Failure: Zero
 * 
 * NOTES
 *    Should call SetLastError(but currently doesn't).
 */
DWORD WINAPI SearchPathA(LPCSTR path, LPCSTR name, LPCSTR ext, DWORD buflen,
                         LPSTR buffer, LPSTR *lastpart )
{
    char full_name[MAX_PATHNAME_LEN];

    if (!DIR_SearchPath( path, name, ext, (LPSTR)full_name )) return 0;
    lstrcpynA( buffer, (LPSTR)full_name, buflen-1);
    buffer[buflen-2] = 0;
    SetLastError(0);
    return strlen(buffer);
}


/***********************************************************************
 *           SearchPath32W   (KERNEL32.448)
 */
DWORD WINAPI SearchPathW(LPCWSTR path, LPCWSTR name, LPCWSTR ext,
                         DWORD buflen, LPWSTR buffer, LPWSTR *lastpart )
{
    char full_name[MAX_PATHNAME_LEN];

    LPSTR pathA = HEAP_strdupWtoA( GetProcessHeap(), 0, path );
    LPSTR nameA = HEAP_strdupWtoA( GetProcessHeap(), 0, name );
    LPSTR extA  = HEAP_strdupWtoA( GetProcessHeap(), 0, ext );
    DWORD ret = DIR_SearchPath( pathA, nameA, extA, (LPSTR)full_name );
    HeapFree( GetProcessHeap(), 0, extA );
    HeapFree( GetProcessHeap(), 0, nameA );
    HeapFree( GetProcessHeap(), 0, pathA );
    if (!ret) return 0;

    lstrcpynAtoW( buffer, full_name, buflen-1 );
    buffer[buflen-2] = 0;
    SetLastError(0);
    return strlen(full_name);
}
