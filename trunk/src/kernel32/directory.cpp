/* $Id: directory.cpp,v 1.38 2001-06-24 16:40:44 sandervl Exp $ */

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
 * TODO: 
 *  - System/window directories should be created by install program!
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


/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/


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
 char *endofwinpath, *tmp;
 int   len;

   strcpy(DIR_System, kernel32Path);
   len = strlen(DIR_System);
   if(DIR_System[len-1] == '\\') {
	DIR_System[len-1] = 0; 
   }
   len = PROFILE_GetOdinIniString(ODINDIRECTORIES,"WINDOWS","",DIR_Windows,sizeof(DIR_Windows));
   if (len > 2) {
	if(DIR_Windows[len-1] == '\\') {
		DIR_Windows[len-1] = 0; 
	}
   }
   else {
   	strcpy(DIR_Windows, DIR_System);
	endofwinpath = tmp = strchr(DIR_Windows, '\\');
   	while(tmp) {
		tmp = strchr(endofwinpath+1, '\\');
		if(tmp)
			endofwinpath = tmp;
   	}
   	if(endofwinpath) {
		*endofwinpath = 0; //remove \SYSTEM32
	}
   	else DebugInt3();
   }
   dprintf(("Windows  dir: %s", DIR_Windows));
   dprintf(("System32 dir: %s", DIR_System));
}
/*****************************************************************************
 * Name      : GetCurrentDirectoryA
 * Purpose   : query the current directory
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : returned length is number of characters required or used for current dir
 *             *excluding* terminator
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/09/28 20:44]
 *****************************************************************************/

ODINFUNCTION2(UINT, GetCurrentDirectoryA, UINT,  nBufferLength,
                                          LPSTR, lpBuffer)
{
 UINT rc;

  rc = OSLibDosQueryDir(nBufferLength, lpBuffer);
  if(rc && rc < nBufferLength) {
       dprintf(("CurrentDirectory = %s (%d)", lpBuffer, rc)); 
  }
  else dprintf(("CurrentDirectory returned %d", rc)); 
  return rc;
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

  rc = CALL_ODINFUNC(GetCurrentDirectoryA)(nBufferLength, asciidir);
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


ODINFUNCTION1(BOOL,   SetCurrentDirectoryA,
              LPCSTR, lpstrDirectory)
{
  if(HIWORD(lpstrDirectory) == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  
  // cut off trailing backslashes
  // not if a process wants to change to the root directory
  int len = lstrlenA(lpstrDirectory);
  if ( ( (lpstrDirectory[len - 1] == '\\') ||
         (lpstrDirectory[len - 1] == '/') ) &&
       (len != 1) )
  {
    LPSTR lpTemp = (LPSTR)_alloca(len);
    lstrcpynA(lpTemp,
              lpstrDirectory,
              len); // len is including trailing NULL!!
    lpstrDirectory = lpTemp;
  }

  dprintf(("SetCurrentDirectoryA %s", lpstrDirectory));
  return O32_SetCurrentDirectory((LPSTR)lpstrDirectory);
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

ODINFUNCTION2(BOOL,                CreateDirectoryA,
              LPCSTR,              lpstrDirectory,
              PSECURITY_ATTRIBUTES,arg2)
{
  // 2001-05-28 PH
  // verify filename first (NT4SP6)
  // @@@PH: if (IsBadStringPtr( (LPVOID)lpstrDirectory, 0xFFFF))
  if (lpstrDirectory == NULL)
  {
      SetLastError(ERROR_PATH_NOT_FOUND);
      return FALSE;
  }

  int len = strlen(lpstrDirectory);
  
  // cut off trailing backslashes
  if ( (lpstrDirectory[len - 1] == '\\') ||
       (lpstrDirectory[len - 1] == '/') )
  {
    LPSTR lpTemp = (LPSTR)_alloca(len);
    lstrcpynA(lpTemp,
              lpstrDirectory,
              len ); // len is including trailing NULL!!
    lpstrDirectory = lpTemp;
  }
  
  dprintf(("CreateDirectoryA %s", lpstrDirectory));
  
  // PH Note 2000/06/12:
  // Creation of an existing directory is NO ERROR it seems.
  DWORD dwAttr = GetFileAttributesA(lpstrDirectory);
  if (dwAttr != -1)
    if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
    {
      SetLastError(ERROR_SUCCESS);
      return TRUE;
    }
  
  return(OSLibDosCreateDirectory(lpstrDirectory));
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
  rc = CALL_ODINFUNC(CreateDirectoryA)(astring, arg2);
  FreeAsciiString(astring);
  return(rc);
}

/*****************************************************************************
 * Name      : BOOL WIN32API CreateDirectoryExA
 * Purpose   : The CreateDirectoryExA function creates a new directory with a
 *             specified path that retains the attributes of a specified
 *             template directory. If the underlying file system supports
 *             security on files and directories, the function applies a
 *             specified security descriptor to the new directory.
 *             The new directory retains the other attributes of the specified
 *             template directory. Note that CreateDirectoryEx has a template
 *             parameter, while CreateDirectory does not.
 * Parameters: LPCSTR lpTemplateDirectory  pointer to path string of template
 *                                         directory
 *             LPCSTR lpNewDirectory      pointer to path string of directory
 *                                         to create
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes  pointer to security
 *                                                         descriptor
 *
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API CreateDirectoryExA( LPCSTR lpTemplateDirectory,
                                  LPCSTR lpNewDirectory,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{

  dprintf(("KERNEL32:CreateDirectoryExA(%08x,%08x,%08x) not properly implemented\n",
           lpTemplateDirectory,lpNewDirectory,lpSecurityAttributes
          ));

  return CreateDirectoryA(lpNewDirectory, lpSecurityAttributes);
}

/*****************************************************************************
 * Name      : BOOL WIN32API CreateDirectoryExW
 * Purpose   : The CreateDirectoryExW function creates a new directory with a
 *             specified path that retains the attributes of a specified
 *             template directory. If the underlying file system supports
 *             security on files and directories, the function applies a
 *             specified security descriptor to the new directory.
 *             The new directory retains the other attributes of the specified
 *             template directory. Note that CreateDirectoryEx has a template
 *             parameter, while CreateDirectory does not.
 * Parameters: LPCWSTR lpTemplateDirectory  pointer to path string of template
 *                                          directory
 *             LPCWSTR lpNewDirectory      pointer to path string of directory
 *                                         to create
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes  pointer to security
 *                                                         descriptor
 *
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API CreateDirectoryExW( LPCWSTR lpTemplateDirectory,
                                  LPCWSTR lpNewDirectory,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{

  dprintf(("KERNEL32:CreateDirectoryExW(%08x,%08x,%08x) not properly implemented\n",
           lpTemplateDirectory,lpNewDirectory,lpSecurityAttributes
          ));

  return CreateDirectoryW(lpNewDirectory, lpSecurityAttributes);
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
 int   len;
 char *dir;

	dir = InternalGetSystemDirectoryA();
	len = lstrlenA(dir);
	if(lpBuffer)
		lstrcpynA(lpBuffer, dir, uSize);
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
    asciibuffer = (char *)alloca(uSize+1);

  if(lpBuffer && asciibuffer == NULL) 
  {
    DebugInt3();
  }

  rc = GetSystemDirectoryA(asciibuffer, uSize);
  if(rc && asciibuffer)
    AsciiToUnicode(asciibuffer, lpBuffer);
  
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
 char *dir;
 int   len;

	dir = InternalGetWindowsDirectoryA();
	len = lstrlenA(dir);
	if(lpBuffer)
		lstrcpynA(lpBuffer, dir, uSize);
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
    asciibuffer = (char *)alloca(uSize+1);

  if(lpBuffer && asciibuffer == NULL) 
  {
    DebugInt3();
  }

  rc = GetWindowsDirectoryA(asciibuffer, uSize);
  if(rc && asciibuffer)
    AsciiToUnicode(asciibuffer, lpBuffer);
  
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


ODINFUNCTION1(BOOL, RemoveDirectoryA, LPCSTR, lpstrDirectory)
{
  int len = strlen(lpstrDirectory);
  
  if(lpstrDirectory == NULL) {
     SetLastError(ERROR_INVALID_PARAMETER);
     return FALSE;
  }
  // cut off trailing backslashes
  if ( (lpstrDirectory[len - 1] == '\\') ||
       (lpstrDirectory[len - 1] == '/') )
  {
    LPSTR lpTemp = (LPSTR)_alloca(len);
    lstrcpynA(lpTemp,
              lpstrDirectory,
              len ); // len is including trailing NULL!!
    lpstrDirectory = lpTemp;
  }
  
  dprintf(("RemoveDirectory %s", lpstrDirectory));

  return OSLibDosRemoveDir(lpstrDirectory);
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

    /* See if path is a list of directories to search. If so, only search
       those (according to SDK docs) */
    if ((path != NULL) && strchr(path, ';')) {
        ret = OSLibDosSearchPath(OSLIB_SEARCHDIR, (LPSTR)path, (LPSTR)name, 
                                 full_name, MAX_PATHNAME_LEN);
        goto done;
    }

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
    lstrcpynA( buffer, (LPSTR)full_name, buflen);
    SetLastError(0);
    return strlen(full_name);
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

    lstrcpynAtoW( buffer, full_name, buflen);
    SetLastError(0);
    return ret;
}
