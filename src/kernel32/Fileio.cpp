/* $Id: Fileio.cpp,v 1.71 2003-03-03 16:39:53 sandervl Exp $ */

/*
 * Win32 File IO API functions for OS/2
 *
 * Copyright 1998-2000 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * Some parts based on Wine code (CopyFileExA/W, FindFirstFileExW,
 * GetShortPathNameA/W, GetLongPathNameA/W)
 *
 * Copyright 1993 John Burton
 * Copyright 1993 Erik Bos
 * Copyright 1996 Alexandre Julliard
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
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include <heapstring.h>
#include "handlemanager.h"
#include "oslibdos.h"
#include "oslibwps.h"

#define DBG_LOCALLOG    DBG_fileio
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-FILEIO)

#include <ctype.h>
#include "fileio.h"
#include <win/file.h>


/***********************************************************************
 *           DOSFS_ValidDOSName
 *
 * Return 1 if OS/2 file 'name' is also a valid MS-DOS name
 * (i.e. contains only valid DOS chars, lower-case only, fits in 8.3 format).
 * File name can be terminated by '\0', '\\' or '/'.
 */
static int DOSFS_ValidDOSName( const char *name, int ignore_case )
{
    static const char invalid_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" INVALID_DOS_CHARS;
    const char *p = name;
    const char *invalid = ignore_case ? (invalid_chars + 26) : invalid_chars;
    int len = 0;

    if (*p == '.')
    {
        /* Check for "." and ".." */
        p++;
        if (*p == '.') p++;
        /* All other names beginning with '.' are invalid */
        return (IS_END_OF_NAME(*p));
    }
    while (!IS_END_OF_NAME(*p))
    {
        if (strchr( invalid, *p )) return 0;  /* Invalid char */
        if (*p == '.') break;  /* Start of the extension */
        if (++len > 8) return 0;  /* Name too long */
        p++;
    }
    if (*p != '.') return 1;  /* End of name */
    p++;
    if (IS_END_OF_NAME(*p)) return 0;  /* Empty extension not allowed */
    len = 0;
    while (!IS_END_OF_NAME(*p))
    {
        if (strchr( invalid, *p )) return 0;  /* Invalid char */
        if (*p == '.') return 0;  /* Second extension not allowed */
        if (++len > 3) return 0;  /* Extension too long */
        p++;
    }
    return 1;
}

/***********************************************************************
 *           DOSFS_Hash
 *
 * Transform an OS/2 file name into a hashed DOS name. If the name is a valid
 * DOS name, it is converted to upper-case; otherwise it is replaced by a
 * hashed version that fits in 8.3 format.
 * File name can be terminated by '\0', '\\' or '/'.
 * 'buffer' must be at least 13 characters long.
 */

static void DOSFS_Hash( LPCSTR name, LPSTR buffer, BOOL dir_format,
                        BOOL ignore_case )
{
    static const char invalid_chars[] = INVALID_DOS_CHARS "~.";
    static const char hash_chars[33] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345";

    const char *p, *ext;
    char *dst;
    unsigned short hash;
    int i;

    if (dir_format) strcpy( buffer, "           " );

    if (DOSFS_ValidDOSName( name, ignore_case ))
    {
        /* Check for '.' and '..' */
        if (*name == '.')
        {
            buffer[0] = '.';
            if (!dir_format) buffer[1] = buffer[2] = '\0';
            if (name[1] == '.') buffer[1] = '.';
            return;
        }

        /* Simply copy the name, converting to uppercase */

        for (dst = buffer; !IS_END_OF_NAME(*name) && (*name != '.'); name++)
            *dst++ = FILE_toupper(*name);
        if (*name == '.')
        {
            if (dir_format) dst = buffer + 8;
            else *dst++ = '.';
            for (name++; !IS_END_OF_NAME(*name); name++)
                *dst++ = FILE_toupper(*name);
        }
        if (!dir_format) *dst = '\0';
        return;
    }

    /* Compute the hash code of the file name */
    /* If you know something about hash functions, feel free to */
    /* insert a better algorithm here... */
    if (ignore_case)
    {
        for (p = name, hash = 0xbeef; !IS_END_OF_NAME(p[1]); p++)
            hash = (hash<<3) ^ (hash>>5) ^ FILE_tolower(*p) ^ (FILE_tolower(p[1]) << 8);
        hash = (hash<<3) ^ (hash>>5) ^ FILE_tolower(*p); /* Last character*/
    }
    else
    {
        for (p = name, hash = 0xbeef; !IS_END_OF_NAME(p[1]); p++)
            hash = (hash << 3) ^ (hash >> 5) ^ *p ^ (p[1] << 8);
        hash = (hash << 3) ^ (hash >> 5) ^ *p;  /* Last character */
    }

    /* Find last dot for start of the extension */
    for (p = name+1, ext = NULL; !IS_END_OF_NAME(*p); p++)
        if (*p == '.') ext = p;
    if (ext && IS_END_OF_NAME(ext[1]))
        ext = NULL;  /* Empty extension ignored */

    /* Copy first 4 chars, replacing invalid chars with '_' */
    for (i = 4, p = name, dst = buffer; i > 0; i--, p++)
    {
        if (IS_END_OF_NAME(*p) || (p == ext)) break;
        *dst++ = strchr( invalid_chars, *p ) ? '_' : FILE_toupper(*p);
    }
    /* Pad to 5 chars with '~' */
    while (i-- >= 0) *dst++ = '~';

    /* Insert hash code converted to 3 ASCII chars */
    *dst++ = hash_chars[(hash >> 10) & 0x1f];
    *dst++ = hash_chars[(hash >> 5) & 0x1f];
    *dst++ = hash_chars[hash & 0x1f];

    /* Copy the first 3 chars of the extension (if any) */
    if (ext)
    {
        if (!dir_format) *dst++ = '.';
        for (i = 3, ext++; (i > 0) && !IS_END_OF_NAME(*ext); i--, ext++)
            *dst++ = strchr( invalid_chars, *ext ) ? '_' : FILE_toupper(*ext);
    }
    if (!dir_format) *dst = '\0';
}

//******************************************************************************
//******************************************************************************
HFILE WIN32API CreateFileA(LPCSTR lpszName, DWORD fdwAccess, DWORD fdwShareMode,
                           LPSECURITY_ATTRIBUTES lpsa, DWORD fdwCreate,
                           DWORD  fdwAttrsAndFlags, HANDLE hTemplateFile)
{
  dprintf(("CreateFileA %s", lpszName));
  return(HMCreateFile(lpszName,
                      fdwAccess,
                      fdwShareMode,
                      lpsa,
                      fdwCreate,
                      fdwAttrsAndFlags,
                      hTemplateFile));
}

//******************************************************************************
//******************************************************************************
HFILE WIN32API CreateFileW(LPCWSTR lpszName, DWORD fdwAccess, DWORD fdwShareMode,
                           LPSECURITY_ATTRIBUTES lpsa, DWORD fdwCreate,
                           DWORD fdwAttrsAndFlags, HANDLE hTemplateFile)
{
  HANDLE rc;
  char  *astring;

  astring = UnicodeToAsciiString((LPWSTR)lpszName);
  rc = CreateFileA(astring, fdwAccess, fdwShareMode,
                   lpsa, fdwCreate, fdwAttrsAndFlags,
                   hTemplateFile);
  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, WIN32_FIND_DATAA *lpFindFileData)
{
  return FindFirstFileExA(lpFileName, FindExInfoStandard, lpFindFileData,
                          FindExSearchNameMatch, NULL, 0);
}
/*****************************************************************************
 * Name      : HANDLE WIN32API FindFirstFileExA
 * Purpose   : The FindFirstFileExA function searches a directory for a file
 *             whose name and attributes match those specified in the
 *             function call.
 * Parameters: LPCSTR lpFileName                 pointer to the name of the file
 *                                               to search for
 *             FINDEX_INFO_LEVELS fInfoLevelId   information level of the returned data
 *             LPVOID lpFindFileData             pointer to the returned information
 *             FINDEX_SEARCH_OPS fSearchOp       type of filtering to perform
 *             LPVOID lpSearchFilter             pointer to search criteria
 *             DWORD dwAdditionalFlags           additional search control flags
 * Variables :
 * Result    : If the function succeeds, the return value is a search handle
 *             that can be used in a subsequent call to the FindNextFile or
 *             FindClose functions.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE
 * Remark    :
 * Status    : 
 *
 * Author    : SvL
 *****************************************************************************/
HANDLE WIN32API FindFirstFileExA(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId,
                                 LPVOID lpFindFileData,
                                 FINDEX_SEARCH_OPS fSearchOp,
                                 LPVOID lpSearchFilter,
                                 DWORD dwAdditionalFlags)
{   
  HANDLE hFind;

    if(lpFileName == NULL || lpFindFileData == NULL || lpSearchFilter != NULL) 
    {
        dprintf(("!ERROR!: invalid parameter(s)"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
    }

    if(fSearchOp == FindExSearchLimitToDevices) {
        dprintf(("!ERROR!: FindExSearchLimitToDevices not implemented"));
        SetLastError(ERROR_NOT_SUPPORTED);
        return INVALID_HANDLE_VALUE;
    }
    else
    if(fSearchOp == FindExSearchLimitToDirectories) {     
        //NOTE: According to the SDK docs we are allowed to silently ignore this option
        dprintf(("!WARNING!: FindExSearchLimitToDirectories IGNORED"));
        fSearchOp = FindExSearchNameMatch;
    }
    if(dwAdditionalFlags != 0) {
        dprintf(("!ERROR!: options not implemented 0x%08x 0x%08lx\n", fSearchOp, dwAdditionalFlags ));
        SetLastError(ERROR_NOT_SUPPORTED);
        return INVALID_HANDLE_VALUE;
    }

    dprintf(("FindFirstFileExA %s %x %x %x %x %x", lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags));

    switch(fInfoLevelId)
    {
    case FindExInfoStandard:
    {
        char  *filename;
        int    namelen;

        //Strip the backslash if 'x:\'. This function is supposed
        //to work in that case, but fail with 'x:\directory\'
        namelen = strlen(lpFileName);
        if(namelen == 3 && lpFileName[1] == ':' && lpFileName[2] == '\\')
        {
            filename = (char *)alloca(namelen+1);
            strcpy(filename, lpFileName);
            filename[namelen-1] = 0;
        }
        else  
            filename = (char *)lpFileName;
 
        return (HANDLE)OSLibDosFindFirst(filename, (WIN32_FIND_DATAA *)lpFindFileData);
    }

    default: //should never happen
        dprintf(("!ERROR! unsupported fInfoLevelId"));
        SetLastError(ERROR_INVALID_PARAMETER);
        break;
    }      
    return INVALID_HANDLE_VALUE;
}
//******************************************************************************
//******************************************************************************
HANDLE WINAPI FindFirstFileW(LPCWSTR lpFileName, WIN32_FIND_DATAW *lpFindFileData)
{
    return FindFirstFileExW(lpFileName, FindExInfoStandard, lpFindFileData,
                            FindExSearchNameMatch, NULL, 0);
}
/*****************************************************************************
 * Name      : HANDLE WIN32API FindFirstFileExW
 * Purpose   : The FindFirstFileExW function searches a directory for a file
 *             whose name and attributes match those specified in the
 *             function call.
 * Parameters: LPCWSTR lpFileName                pointer to the name of the file
 *                                               to search for
 *             FINDEX_INFO_LEVELS fInfoLevelId   information level of the returned data
 *             LPVOID lpFindFileData             pointer to the returned information
 *             FINDEX_SEARCH_OPS fSearchOp       type of filtering to perform
 *             LPVOID lpSearchFilter             pointer to search criteria
 *             DWORD dwAdditionalFlags           additional search control flags
 * Variables :
 * Result    : If the function succeeds, the return value is a search handle
 *             that can be used in a subsequent call to the FindNextFile or
 *             FindClose functions.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE
 * Remark    :
 * Status    : 
 *
 * Author    : Wine
 *****************************************************************************/
HANDLE WIN32API FindFirstFileExW(LPCWSTR lpFileName,
                                 FINDEX_INFO_LEVELS fInfoLevelId,
                                 LPVOID lpFindFileData,
                                 FINDEX_SEARCH_OPS fSearchOp,
                                 LPVOID lpSearchFilter,
                                 DWORD dwAdditionalFlags)
{
    HANDLE handle;
    WIN32_FIND_DATAA dataA;
    LPVOID _lpFindFileData;
    LPSTR pathA;

    switch(fInfoLevelId)
    {
      case FindExInfoStandard:
        {
          _lpFindFileData = &dataA;
        }
        break;
      default:
        dprintf(("!ERROR! unsupported fInfoLevelId"));
        SetLastError(ERROR_INVALID_PARAMETER);
        break;
    }

    pathA = HEAP_strdupWtoA( GetProcessHeap(), 0, lpFileName );
    handle = FindFirstFileExA(pathA, fInfoLevelId, _lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
    HeapFree( GetProcessHeap(), 0, pathA );
    if (handle == INVALID_HANDLE_VALUE) return handle;
    
    switch(fInfoLevelId)
    {
      case FindExInfoStandard:
        {
          WIN32_FIND_DATAW *dataW = (WIN32_FIND_DATAW*) lpFindFileData;
          dataW->dwFileAttributes = dataA.dwFileAttributes;
          dataW->ftCreationTime   = dataA.ftCreationTime;
          dataW->ftLastAccessTime = dataA.ftLastAccessTime;
          dataW->ftLastWriteTime  = dataA.ftLastWriteTime;
          dataW->nFileSizeHigh    = dataA.nFileSizeHigh;
          dataW->nFileSizeLow     = dataA.nFileSizeLow;
          MultiByteToWideChar( CP_ACP, 0, dataA.cFileName, -1,
                               dataW->cFileName, sizeof(dataW->cFileName)/sizeof(WCHAR) );
          MultiByteToWideChar( CP_ACP, 0, dataA.cAlternateFileName, -1,
                               dataW->cAlternateFileName,
                               sizeof(dataW->cAlternateFileName)/sizeof(WCHAR) );
        }
        break;
    }
    return handle;
}
//******************************************************************************
// internal function for faster access (SHELL32)
//******************************************************************************
HANDLE WIN32API FindFirstFileMultiA(LPCSTR lpFileName,
                                    WIN32_FIND_DATAA * lpFindFileData,
                                    DWORD * count)
{
    dprintf(("FindFirstFileMultiA %s %x %x", lpFileName, lpFindFileData, count));
    return (HANDLE)OSLibDosFindFirstMulti(lpFileName,lpFindFileData,count);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextFileA(HANDLE hFindFile, WIN32_FIND_DATAA * lpFindFileData)
{
    return OSLibDosFindNext(hFindFile,lpFindFileData);
}
//******************************************************************************
// internal function for faster access (SHELL32)
//******************************************************************************
BOOL WIN32API FindNextFileMultiA(HANDLE hFindFile, WIN32_FIND_DATAA * lpFindFileData,
              DWORD *count)
{
    dprintf(("FindNextFileMultiA %x %x %x", hFindFile, lpFindFileData, count));
    return OSLibDosFindNextMulti(hFindFile,lpFindFileData,count);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextFileW(HANDLE hFindFile, WIN32_FIND_DATAW * lpFindFileData)
{
  WIN32_FIND_DATAA wfda;
  BOOL             rc;

  rc = OSLibDosFindNext(hFindFile,&wfda);

  if(rc == 0) {
        memset(lpFindFileData, 0, sizeof(WIN32_FIND_DATAW));
  }
  else {
        // convert back the result structure
        memcpy(lpFindFileData,
               &wfda,
               sizeof(WIN32_FIND_DATAA));

        lstrcpynAtoW (lpFindFileData->cFileName,
                      wfda.cFileName,
                      sizeof(wfda.cFileName));

        lstrcpynAtoW (lpFindFileData->cAlternateFileName,
                      wfda.cAlternateFileName,
                      sizeof(wfda.cAlternateFileName));
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindClose(HANDLE hFindFile)
{
  return OSLibDosFindClose(hFindFile);
}
//******************************************************************************
//******************************************************************************
INT WIN32API CompareFileTime(FILETIME * lpft1, FILETIME * lpft2)
{
   if (lpft1 == NULL || lpft2 == NULL) {
       SetLastError(ERROR_INVALID_PARAMETER);
       return -1;
   }

   if(lpft1->dwHighDateTime > lpft2->dwHighDateTime)
       return 1;

   if(lpft1->dwHighDateTime < lpft2->dwHighDateTime)
       return -1;

   if(lpft1->dwLowDateTime > lpft2->dwLowDateTime)
       return 1;

   if(lpft1->dwLowDateTime < lpft2->dwLowDateTime)
       return -1;

   return 0; //equal
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CopyFileA(LPCSTR arg1, LPCSTR arg2, BOOL arg3)
{
    dprintf(("CopyFileA %s %s %d", arg1, arg2, arg3));
  return OSLibDosCopyFile(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CopyFileW(LPCWSTR arg1, LPCWSTR arg2, BOOL arg3)
{
  BOOL  rc;
  char *astring1, *astring2;

  astring1 = UnicodeToAsciiString((LPWSTR)arg1);
  astring2 = UnicodeToAsciiString((LPWSTR)arg2);
  rc = CopyFileA(astring1, astring2, arg3);
  FreeAsciiString(astring2);
  FreeAsciiString(astring1);
  return(rc);
}
/*****************************************************************************
 * Name      : BOOL WIN32API CopyFileExA
 * Purpose   : The CopyFileExA function copies an existing file to a new file.
 *             This function preserves extended attributes, OLE structured
 *             storage, NTFS alternate data streams, and file attributes.
 *             Security attributes for the existing file are not copied to
 *             the new file.
 * Parameters: LPCSTR lpExistingFileName   pointer to name of an existing file
 *             LPCSTR lpNewFileName        pointer to filename to copy to
 *             LPPROGRESS_ROUTINE lpProgressRoutine  pointer to the callback function
 *             LPVOID lpData               to be passed to the callback function
 *             LPBOOL pbCancel     flag that can be used to cancel the operation
 *             DWORD dwCopyFlags   flags that specify how the file is copied
 * Variables :
 * Result    : f the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CopyFileExA( LPCSTR             lpExistingFileName,
                           LPCSTR             lpNewFileName,
                           LPPROGRESS_ROUTINE lpProgressRoutine,
                           LPVOID             lpData,
                           LPBOOL             pbCancel,
                           DWORD              dwCopyFlags)
{

  dprintf(("KERNEL32: CopyFileExA(%08x,%08x,%08x,%08x,%08x,%08x) not properly implemented\n",
           lpExistingFileName,
           lpNewFileName,
           lpProgressRoutine,
           lpData,
           pbCancel,
           dwCopyFlags
          ));

  BOOL failIfExists = FALSE;

  /*
   * Interpret the only flag that CopyFile can interpret.
   */
  if((dwCopyFlags & COPY_FILE_FAIL_IF_EXISTS) != 0)
  {
      failIfExists = TRUE;
  }

  return CopyFileA(lpExistingFileName, lpNewFileName, failIfExists);
}


/*****************************************************************************
 * Name      : BOOL WIN32API CopyFileExW
 * Purpose   : The CopyFileExW function copies an existing file to a new file.
 *             This function preserves extended attributes, OLE structured
 *             storage, NTFS alternate data streams, and file attributes.
 *             Security attributes for the existing file are not copied to
 *             the new file.
 * Parameters: LPCWSTR lpExistingFileName   pointer to name of an existing file
 *             LPCWSTR lpNewFileName        pointer to filename to copy to
 *             LPPROGRESS_ROUTINE lpProgressRoutine  pointer to the callback function
 *             LPVOID lpData               to be passed to the callback function
 *             LPBOOL pbCancel     flag that can be used to cancel the operation
 *             DWORD dwCopyFlags   flags that specify how the file is copied
 * Variables :
 * Result    : f the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CopyFileExW( LPCWSTR            lpExistingFileName,
                           LPCWSTR            lpNewFileName,
                           LPPROGRESS_ROUTINE lpProgressRoutine,
                           LPVOID             lpData,
                           LPBOOL             pbCancel,
                           DWORD              dwCopyFlags)
{
    LPSTR sourceA = HEAP_strdupWtoA( GetProcessHeap(), 0, lpExistingFileName );
    LPSTR destA   = HEAP_strdupWtoA( GetProcessHeap(), 0, lpNewFileName );

    BOOL ret = CopyFileExA(sourceA,
                           destA,
                           lpProgressRoutine,
                           lpData,
                           pbCancel,
                           dwCopyFlags);

    HeapFree( GetProcessHeap(), 0, sourceA );
    HeapFree( GetProcessHeap(), 0, destA );

    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteFileA(LPCSTR lpszFile)
{
 BOOL rc;

#if 0
  if((strstr(lpszFile, "odin32_") || strstr(lpszFile, "pe_")) && strstr(lpszFile, ".log")) {
      return TRUE;
  }
#endif

  if(lpszFile == NULL) {
      SetLastError(ERROR_INVALID_PARAMETER); //??
      return FALSE;
  }
  //If the app is deleting a shellink file (.lnk), then we must delete the WPS object
  if(OSLibIsShellLink((LPSTR)lpszFile)) 
  {
      OSLibWinDeleteObject((LPSTR)lpszFile);
  }

  rc = OSLibDosDelete((LPSTR)lpszFile);
  if(!rc) {
    dprintf(("DeleteFileA %s returned FALSE; last error %x", lpszFile, GetLastError()));
    if(GetLastError() == ERROR_BAD_UNIT) {
      return TRUE;
    }
  }
  else  dprintf(("DeleteFileA %s", lpszFile));

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteFileW(LPCWSTR arg1)
{
  BOOL  rc;
  char *astring;

  astring = UnicodeToAsciiString((LPWSTR)arg1);
  rc = DeleteFileA(astring);
  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTempFileNameA(LPCSTR lpPathName, LPCSTR lpPrefixString, 
                               UINT uUnique, LPSTR lpTempFileName)
{
    dprintf(("GetTempFileNameA %s %s", lpPathName, lpPrefixString));
    UINT rc = O32_GetTempFileName(lpPathName, lpPrefixString, uUnique, lpTempFileName);
    dprintf(("GetTempFileNameA: returns %d (%s)\n", rc, rc > 0 && lpTempFileName ? lpTempFileName : "<nothing>"));
    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTempFileNameW(LPCWSTR lpPathName, LPCWSTR lpPrefixString,
                               UINT uUnique, LPWSTR lpTempFileName)
{
  char *asciipath, *asciiprefix;
  char *asciitemp = (char *)malloc(MAX_PATH+1);
  UINT rc;

  asciipath   = UnicodeToAsciiString((LPWSTR)lpPathName);
  asciiprefix = UnicodeToAsciiString((LPWSTR)lpPrefixString);
  rc = GetTempFileNameA(asciipath, asciiprefix, uUnique, asciitemp);
  if(rc)      AsciiToUnicode(asciitemp, lpTempFileName);
  FreeAsciiString(asciiprefix);
  FreeAsciiString(asciipath);
  free(asciitemp);
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReadFile(HANDLE hFile, PVOID pBuffer, DWORD dwLength,
                       PDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
  if(lpNumberOfBytesRead) *lpNumberOfBytesRead = 0;
  if(dwLength == 0) {
      dprintf(("!WARNING!: Nothing to do"));
      //TODO: should we fail here instead?? (wine doesn't)
      return TRUE;
  }
  return (HMReadFile(hFile,
                     pBuffer,
                     dwLength,
                     lpNumberOfBytesRead,
                     lpOverlapped, NULL));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReadFileEx(HANDLE       hFile,
                         LPVOID       lpBuffer,
                         DWORD        nNumberOfBytesToRead,
                         LPOVERLAPPED lpOverlapped,
                         LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  if(nNumberOfBytesToRead == 0) {
      dprintf(("!WARNING!: Nothing to do"));
      //TODO: should we fail here instead?? (wine doesn't)
      return TRUE;
  }
  if(!lpOverlapped || !lpCompletionRoutine) {
      dprintf(("!WARNING!: !lpOverlapped || !lpCompletionRoutine"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }
  //SDK docs say ReadFileEx will fail if this condition is true
  if(GetFileType(hFile) == FILE_TYPE_PIPE && (lpOverlapped->Offset || lpOverlapped->OffsetHigh)) {
      dprintf(("!WARNING!: lpOverlapped->Offset & lpOverlapped->OffsetHigh must be ZERO for named pipes"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }
  return (HMReadFile(hFile,
                     lpBuffer,
                     nNumberOfBytesToRead, NULL,
                     lpOverlapped, lpCompletionRoutine));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WriteFile(HANDLE hFile, LPCVOID buffer, DWORD nrbytes,
                        LPDWORD nrbyteswritten, LPOVERLAPPED lpOverlapped)
{
  if(nrbyteswritten) *nrbyteswritten = 0;
  if(nrbytes == 0) {
      dprintf(("!WARNING!: Nothing to do"));
      //TODO: should we fail here instead?? (wine doesn't)
      return TRUE;
  }

  return (HMWriteFile(hFile,
                      buffer,
                      nrbytes,
                      nrbyteswritten,
                      lpOverlapped, NULL));
}
/*****************************************************************************
 * Name      : BOOL WriteFileEx
 * Purpose   : The WriteFileEx function writes data to a file. It is designed
 *             solely for asynchronous operation, unlike WriteFile, which is
 *             designed for both synchronous and asynchronous operation.
 *             WriteFileEx reports its completion status asynchronously,
 *             calling a specified completion routine when writing is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to write
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to write
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WriteFileEx(HANDLE       hFile,
                          LPCVOID      lpBuffer,
                          DWORD        nNumberOfBytesToWrite,
                          LPOVERLAPPED lpOverlapped,
                          LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  if(nNumberOfBytesToWrite == 0) {
      dprintf(("!WARNING!: Nothing to do"));
      //TODO: should we fail here instead?? (wine doesn't)
      return TRUE;
  }
  if(!lpOverlapped || !lpCompletionRoutine) {
      dprintf(("!WARNING!: !lpOverlapped || !lpCompletionRoutine"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }
  //SDK docs say WriteFileEx will fail if this condition is true
  if(GetFileType(hFile) == FILE_TYPE_PIPE && (lpOverlapped->Offset || lpOverlapped->OffsetHigh)) {
      dprintf(("!WARNING!: lpOverlapped->Offset & lpOverlapped->OffsetHigh must be ZERO for named pipes"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }
  return (HMWriteFile(hFile,
                      (LPVOID)lpBuffer,
                      nNumberOfBytesToWrite, NULL,
                      lpOverlapped, lpCompletionRoutine));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFileAttributesA(LPCSTR lpszFileName)
{
    DWORD rc, error;

    //Disable error popus.
    ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS);

    rc = OSLibGetFileAttributes((LPSTR)lpszFileName);

    SetErrorMode(oldmode);

    //SvL: Open32 returns FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_NORMAL for
    //     directories whereas NT 4 (SP6) only returns FILE_ATTRIBUTE_DIRECTORY
    if(rc != -1 && (rc & FILE_ATTRIBUTE_DIRECTORY)) {
        rc = FILE_ATTRIBUTE_DIRECTORY;
    }

#if 0 // need more tests, maybe there is also a better way to hide simulated b:
    if(rc == -1 && lpszFileName != NULL && !strnicmp(lpszFileName, "B:", 2))
    {
      error = GetLastError();
      if(error = ERROR_DISK_CHANGE)
        SetLastError(ERROR_NOT_READY);
      else
        SetLastError(error);
    }
#endif
    dprintf(("KERNEL32:  GetFileAttributes of %s returned %d\n", lpszFileName, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFileAttributesW(LPCWSTR arg1)
{
  DWORD rc;
  char *astring;

  astring = UnicodeToAsciiString((LPWSTR)arg1);
  dprintf(("GetFileAttributesW %s", astring));
  rc = GetFileAttributesA(astring);
  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes)
{
    dprintf(("KERNEL32: SetFileAttributes of %s", lpFileName));
    return O32_SetFileAttributes(lpFileName, dwFileAttributes);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetFileAttributesW(LPCWSTR lpFileName, DWORD dwFileAttributes)
{
  char *asciifile;
  BOOL  rc;

  asciifile = UnicodeToAsciiString((LPWSTR)lpFileName);
  rc = SetFileAttributesA(asciifile, dwFileAttributes);
  FreeAsciiString(asciifile);
  return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer,
                                LPSTR *lpFilePart)
{
    char *ptr, *lpszFileName;
    DWORD rc;

    dprintf(("KERNEL32:  GetFullPathName called with %s %d %x", lpFileName, nBufferLength, lpBuffer));

    lpszFileName = strdup(lpFileName);

    while((ptr = strchr(lpszFileName, '/')) != NULL)
        *ptr = '\\';

    rc = O32_GetFullPathName(lpszFileName, nBufferLength, lpBuffer, lpFilePart);

    free(lpszFileName);

    return rc;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFullPathNameW(LPCWSTR lpFileName, DWORD nBufferLength,
                                LPWSTR lpBuffer, LPWSTR *lpFilePart)
{
 char *astring = NULL, *asciibuffer = NULL, *asciipart = NULL;
 DWORD rc;

  if(nBufferLength) {
      asciibuffer = (char *)malloc(nBufferLength+1);
  }
  astring     = UnicodeToAsciiString((LPWSTR)lpFileName);

  rc = GetFullPathNameA(astring, nBufferLength,
                        asciibuffer, &asciipart);

  dprintf(("KERNEL32: GetFullPathNameW %s returns %s\n%s",
           astring,
           asciibuffer, asciipart));

  if(rc>0 && rc<nBufferLength && asciibuffer)
    AsciiToUnicode(asciibuffer,
                   lpBuffer);

  if(lpFilePart) {
    if (asciipart == NULL)
      *lpFilePart = NULL;
    else
      *lpFilePart = lpBuffer + ((int)asciipart - (int)asciibuffer);
  }

  FreeAsciiString(astring);
  if(asciibuffer) free(asciibuffer);
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveFileA(LPCSTR arg1, LPCSTR arg2)
{
    dprintf(("KERNEL32: MoveFileA %s %s", arg1, arg2));
    return OSLibDosMoveFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************


/*****************************************************************************
 * Name      : MoveFileExA
 * Purpose   : Move or delete a file
 * Parameters: LPCSTR lpExistingFileName
 *             LPCSTR lpNewFileName
 *             DWORD  dwFlags
 * Variables :
 * Result    :
 * Remark    : "delete on system-reboot" feature is not supported!
 * Status    :
 *
 * Author    : Patrick Haller [2001-08-30]
 *****************************************************************************/

BOOL WIN32API MoveFileExA(LPCSTR lpszOldFilename,
                          LPCSTR lpszNewFilename,
                          DWORD fdwFlags)
{
  dprintf(("KERNEL32:  MoveFileExA %s to %s %x, not complete!\n", 
           lpszOldFilename, 
           lpszNewFilename,
           fdwFlags));
  
  // this parameter combination is illegal
  if ( (fdwFlags & MOVEFILE_DELAY_UNTIL_REBOOT) &&
       (fdwFlags & MOVEFILE_COPY_ALLOWED) )
  {
    // Note: error code not verified
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  
  // first, we take care about the special cases
  if (fdwFlags && MOVEFILE_DELAY_UNTIL_REBOOT)
  {
    // We cannot really support this in any other way than
    // to call the IBMCSFLK driver. As the first place I've encountered
    // this call is Microsoft ACMSETUP wanting to replace OLEPRO32.DLL
    // in the ODIN system directory, we are better skipping the call.
    
    // Anyway, this is only supported under Windows NT
    fdwFlags &= ~MOVEFILE_DELAY_UNTIL_REBOOT;
    
    // Until we support this, we have to intercept
    // lpszNewFilename == NULL
    if (NULL == lpszNewFilename)
    {
      // try to delete the filename
      dprintf(("KERNEL32-MoveFileExA: trying to delete file [%s], skipped.",
               lpszOldFilename));
      
      SetLastError( NO_ERROR );
      return TRUE;
    }
  }
  
  if (fdwFlags && MOVEFILE_COPY_ALLOWED)
  {
    // if lpszOldFilename and lpszNewFilename refer to different
    // volumes, this flag controls if a copy operation is allowed.
  }
  
  if (fdwFlags && MOVEFILE_REPLACE_EXISTING)
  {
    // We can only attempt to 
    // 1 move away the current file if existing,
    // 2 do the current move operation
    // 3 if succesful, delete the backup
    //   otherwise restore the original file
  }
  
  return OSLibDosMoveFile(lpszOldFilename, 
                          lpszNewFilename);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveFileW(LPCWSTR lpSrc, LPCWSTR lpDest)
{
  char *asciisrc, *asciidest;
  BOOL  rc;

  asciisrc  = UnicodeToAsciiString((LPWSTR)lpSrc);
  asciidest = UnicodeToAsciiString((LPWSTR)lpDest);
  rc = MoveFileA(asciisrc, asciidest);
  FreeAsciiString(asciisrc);
  FreeAsciiString(asciidest);
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveFileExW(LPCWSTR lpSrc, LPCWSTR lpDest, DWORD fdwFlags)
{
  dprintf(("KERNEL32: MoveFileExW %ls to %ls %x", 
           lpSrc, 
           lpDest,
           fdwFlags));
  
  char *asciisrc, 
       *asciidest;
  BOOL rc;

  asciisrc  = UnicodeToAsciiString((LPWSTR)lpSrc);
  if (NULL != lpDest)
    asciidest = UnicodeToAsciiString((LPWSTR)lpDest);
  else
    asciidest = NULL;
  
  rc = MoveFileExA(asciisrc, 
                   asciidest,
                   fdwFlags);
  
  if (NULL != asciidest)
    FreeAsciiString(asciidest);
  
  FreeAsciiString(asciisrc);
  
  return(rc);
}
//******************************************************************************
//Behaviour in NT 4, SP6:
//- converts long filename to 8.3 short filname (TODO: not yet done here!)
//- fails on volume that doesn't support 8.3 filenames
//- if lpszShortPath 0 or cchBuffer too small -> return required length
//  (INCLUDING 0 terminator)
//- if lpszLongPath == NULL -> ERROR_INVALID_PARAMETER (return 0)
//- if lpszLongPath empty -> proceed as if nothing is wrong
//- does NOT clear the last error if successful!
//- if successful -> return length of string (excluding 0 terminator)
//******************************************************************************
DWORD WIN32API GetShortPathNameA(LPCTSTR lpszLongPath,
                                 LPTSTR lpszShortPath,
                                 DWORD cchBuffer)
{
  char  short_name[MAX_PATHNAME_LEN];  /* Long pathname in Unix format */
  int length, marker = 0;
  LPSTR tmpshortpath,tmplongpath;
  DWORD attr, sp = 0, lp = 0;
  int tmplen, drive;
 
  dprintf(("KERNEL32:  GetShortPathNameA %s", lpszLongPath));

  if(!lpszLongPath) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
  }
  if (!lpszLongPath[0]) {
      SetLastError(ERROR_BAD_PATHNAME);
      return 0;
  }

  if ( ( tmpshortpath = (char*) HeapAlloc ( GetProcessHeap(), 0, MAX_PATHNAME_LEN ) ) == NULL ) {
      SetLastError ( ERROR_NOT_ENOUGH_MEMORY );
      return 0;
  }

  if ( ( tmplongpath = (char*) HeapAlloc ( GetProcessHeap(), 0, MAX_PATHNAME_LEN ) ) == NULL ) {
      SetLastError ( ERROR_NOT_ENOUGH_MEMORY );
      return 0;
  }
  
  lstrcpyA(tmplongpath,lpszLongPath);
   
  /* check for drive letter */
  if ( lpszLongPath[1] == ':' ) {
      tmpshortpath[0] = lpszLongPath[0];
      tmpshortpath[1] = ':';
      sp = 2;
      lp = 2;
    }

   //todo: check drive validity!

   while ( lpszLongPath[lp] ) {
      marker = 0;
      /* check for path delimiters and reproduce them */
      if ( lpszLongPath[lp] == '\\' || lpszLongPath[lp] == '/' ) {
	if (!sp || tmpshortpath[sp-1]!= '\\') 
        {
	    /* strip double "\\" */
	    tmpshortpath[sp] = '\\';
	    sp++;
        }
        tmpshortpath[sp]=0;/*terminate string*/
	lp++;
	continue;
      }

      tmplen = strcspn ( lpszLongPath + lp, "\\/" ); 
      lstrcpynA ( tmpshortpath+sp, lpszLongPath + lp, tmplen+1 );

      /* Check, if the current element is a valid dos name */
      if ( DOSFS_ValidDOSName ( lpszLongPath + lp, TRUE ) ) {
	sp += tmplen;
	lp += tmplen;
	continue;
      }

      if (tmplongpath[lp + tmplen] == '\\')
      { 
         tmplongpath[lp + tmplen] = 0;
         marker = 1;
      }

      attr = GetFileAttributesA(tmplongpath);

      if (attr == -1) 
      {
         SetLastError ( ERROR_FILE_NOT_FOUND );
         HeapFree ( GetProcessHeap(), 0, tmpshortpath );
         HeapFree ( GetProcessHeap(), 0, tmplongpath );
         return 0;
      }

      DOSFS_Hash(tmpshortpath+sp, short_name, FALSE, TRUE );          
       
      strcpy( tmpshortpath+sp, short_name);
      sp += strlen ( tmpshortpath+sp );
      if (marker)
         tmplongpath[lp + tmplen] = '\\';
      lp += tmplen;
      
    }

    tmpshortpath[sp] = 0;

    lstrcpynA ( lpszShortPath, tmpshortpath, cchBuffer );
    dprintf(("returning %s\n", lpszShortPath));
    tmplen = strlen ( lpszShortPath  );

    HeapFree ( GetProcessHeap(), 0, tmpshortpath );
    HeapFree ( GetProcessHeap(), 0, tmplongpath );

    return tmplen;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetShortPathNameW(LPCWSTR lpszLongPath, LPWSTR lpszShortPath,
                                 DWORD cchBuffer)
{
    LPSTR longpathA, shortpathA;
    DWORD ret = 0;

    longpathA = HEAP_strdupWtoA( GetProcessHeap(), 0, lpszLongPath );
    shortpathA = (LPSTR) HeapAlloc ( GetProcessHeap(), 0, cchBuffer );

    ret = GetShortPathNameA ( longpathA, shortpathA, cchBuffer );
    if (cchBuffer > 0 && !MultiByteToWideChar( CP_ACP, 0, shortpathA, -1, lpszShortPath, cchBuffer ))
        lpszShortPath[cchBuffer-1] = 0;
    HeapFree( GetProcessHeap(), 0, longpathA );
    HeapFree( GetProcessHeap(), 0, shortpathA );

    return ret;
}
//******************************************************************************
//Behaviour in NT 4, SP6: (presumably the same as GetShortPathNameA; TODO check)
//- converts short filename to long filenames (TODO: not yet done here!)
//- if lpszShortPath 0 or cchBuffer too small -> return required length
//  (INCLUDING 0 terminator)
//- if lpszLongPath == NULL -> ERROR_INVALID_PARAMETER (return 0)
//- if lpszLongPath empty -> proceed as if nothing is wrong
//- does NOT clear the last error if successful!
//- if successful -> return length of string (excluding 0 terminator)
//******************************************************************************
DWORD WINAPI GetLongPathNameA( LPCSTR lpszShortPath, LPSTR lpszLongPath,
                               DWORD cchBuffer )
{
  int tmplen;
  char  short_name[MAX_PATHNAME_LEN];  /* Long pathname in Unix format */
  WIN32_FIND_DATAA FindFileData;
  HANDLE hFind;
  DWORD sp = 0, lp = 0,attr;
  LPSTR tmpshortpath,tmplongpath;
  LPSTR lpszShortPath1, lpszLongPath1;

   dprintf(("GetLongPathNameA %s %x %d", lpszShortPath, lpszLongPath, cchBuffer));
  
   if(!lpszShortPath) {
     SetLastError(ERROR_INVALID_PARAMETER);
     return 0;
   }

   if ( ( tmpshortpath = (char*) HeapAlloc ( GetProcessHeap(), 0, MAX_PATHNAME_LEN ) ) == NULL ) {
      SetLastError ( ERROR_NOT_ENOUGH_MEMORY );
      return 0;
   }

   if ( ( tmplongpath = (char*) HeapAlloc ( GetProcessHeap(), 0, MAX_PATHNAME_LEN ) ) == NULL ) {
      SetLastError ( ERROR_NOT_ENOUGH_MEMORY );
      return 0;
   }

   lpszShortPath1 = ODINHelperStripUNC((char*)lpszShortPath);
   lpszLongPath1 = ODINHelperStripUNC((char*)lpszLongPath);

   lstrcpyA(tmpshortpath,lpszShortPath1);

   /* check for drive letter */
   if ( lpszShortPath1[1] == ':' ) {
       tmplongpath[0] = lpszShortPath1[0];

      tmplongpath[1] = ':';
      sp = 2;
      lp = 2;
   }

   //todo: check drive validity!

   while ( lpszShortPath1[lp] ) {

      /* check for path delimiters and reproduce them */
      if ( lpszShortPath1[lp] == '\\' || lpszShortPath1[lp] == '/' ) {
	if (!sp || tmplongpath[sp-1]!= '\\') 
        {
	    /* strip double "\\" */
	    tmplongpath[sp] = '\\';
	    sp++;
        }
        tmplongpath[sp]=0;/*terminate string*/
	lp++;
	continue;
      }

      tmplen = strcspn ( lpszShortPath1 + lp, "\\/" );
      lstrcpynA ( tmplongpath+sp, lpszShortPath1 + lp, tmplen+1 );

      attr = GetFileAttributesA(tmplongpath);
      if (attr != -1) 
      {
	sp += tmplen;
	lp += tmplen;
	continue;
      }
      else
        // it may be hashed name or name with weird characters!
        if ((tmplongpath+sp)[4] == '~')
        {
          //hashed entry Wine does linear dir search.. incredible.. we will be
          //better ;)
          if (strchr(tmplongpath+sp,'_'))
          {
            (tmplongpath+sp)[0] = '*';
            (tmplongpath+sp)[1] = 0;
          }  
          else
          {
            (tmplongpath+sp)[4] = '*';
            (tmplongpath+sp)[5] = 0;
          }
          hFind = FindFirstFileExA(tmplongpath, FindExInfoStandard, &FindFileData,
                                   FindExSearchNameMatch, NULL, 0 );
           
          if (hFind == INVALID_HANDLE_VALUE)
          {
            //no possible variants!
            SetLastError ( ERROR_FILE_NOT_FOUND );
            HeapFree ( GetProcessHeap(), 0, tmpshortpath );
            HeapFree ( GetProcessHeap(), 0, tmplongpath );
            return 0;
          }
          else
           do
           {
             DOSFS_Hash(FindFileData.cFileName, short_name, FALSE, TRUE );          
             //this happens on files like [hello world]
             if (!lstrncmpA(short_name, lpszShortPath1+lp, (lpszShortPath1+lp+tmplen)[-1] == '.' ? tmplen-1 : tmplen ))
             {
               strcpy( tmplongpath+sp, FindFileData.cFileName);
               sp += strlen ( tmplongpath+sp );
               lp += tmplen;
               break;
             }
            }
            while (FindNextFileA(hFind, &FindFileData));
            
          // no FindClose() here or else GetLastError() will not give its error   
          if (GetLastError() == ERROR_NO_MORE_FILES)
          {
             FindClose(hFind);
             SetLastError ( ERROR_FILE_NOT_FOUND );
             HeapFree ( GetProcessHeap(), 0, tmpshortpath );
             HeapFree ( GetProcessHeap(), 0, tmplongpath );
             return 0;
          }
          FindClose(hFind);
        }
        else 
        {
            // if this file can't be found in common or hashed files
            // it does not exist
            SetLastError ( ERROR_FILE_NOT_FOUND );
            HeapFree ( GetProcessHeap(), 0, tmpshortpath );
            HeapFree ( GetProcessHeap(), 0, tmplongpath );
            return 0;
        }
   }
   tmplongpath[sp] = 0;

   lstrcpynA ( lpszLongPath1, tmplongpath, cchBuffer );
   dprintf(("returning %s\n", lpszLongPath));
   tmplen = strlen ( lpszLongPath  );

   HeapFree ( GetProcessHeap(), 0, tmpshortpath );
   HeapFree ( GetProcessHeap(), 0, tmplongpath );

   return tmplen;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI GetLongPathNameW( LPCWSTR lpszShortPath, LPWSTR lpszLongPath,
                               DWORD cchBuffer )
{
 int length;

  dprintf(("GetLongPathNameW %x %ls %d", lpszShortPath, lpszLongPath, cchBuffer));
  dprintf(("WARNING: WIN98 ONLY!!"));

  if(!lpszShortPath) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
  }

  length = lstrlenW(lpszShortPath) + 1;
  if(length > cchBuffer) {
      if(lpszLongPath) {
          *lpszLongPath = 0;
      }
      return(length); //return length required (including 0 terminator)
  }
  lstrcpyW(lpszLongPath, lpszShortPath);
  return(length-1);
}
//******************************************************************************
//******************************************************************************
void WIN32API SetFileApisToANSI()
{
    dprintf(("!WARNING! SetFileApisToANSI() stub\n"));
}

/*****************************************************************************
 * Name      : DWORD GetCompressedFileSizeA
 * Purpose   : The GetCompressedFileSizeA function obtains the compressed
 *             size, in bytes, of a specified file.
 * Parameters: LPCTSTR  lpFileName,    // pointer to name of file
 *             LPDWORD  lpFileSizeHigh // pointer to DWORD to receive
 *                                        high-order doubleword of file size
 * Variables :
 * Result    : size of compressed file
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetCompressedFileSizeA(LPCTSTR lpFileName, LPDWORD lpFileSizeHigh)
{
  dprintf(("KERNEL32: GetCompressedFileSizeA (%s, %08xh) not implemented.\n",
           lpFileName,
           lpFileSizeHigh));

  /* @@@PH: simply return the standard filesize */
  return 0;
}


/*****************************************************************************
 * Name      : DWORD GetCompressedFileSizeW
 * Purpose   : The GetCompressedFileSizeE function obtains the compressed
 *             size, in bytes, of a specified file.
 * Parameters: LPCWSTR  lpFileName,    // pointer to name of file
 *             LPDWORD  lpFileSizeHigh // pointer to DWORD to receive
 *                                        high-order doubleword of file size
 * Variables :
 * Result    : size of compressed file
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetCompressedFileSizeW(LPCWSTR lpFileName, LPDWORD lpFileSizeHigh)
{
  LPCTSTR lpAsciiFileName;                             /* converted filename */
  DWORD   rc;                                             /* function result */

  dprintf(("KERNEL32: GetCompressedFileSizeW (%s, %08xh)\n",
           lpFileName,
           lpFileSizeHigh));

  lpAsciiFileName = UnicodeToAsciiString( (LPWSTR) lpFileName);

  rc = GetCompressedFileSizeA(lpAsciiFileName,
                                 lpFileSizeHigh);

  FreeAsciiString( (char *) lpAsciiFileName);

  return (rc);                                              /* return result */
}


/*****************************************************************************
 * Name      : BOOL GetFileAttributesExA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : KERNEL32.874
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetFileAttributesExA(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                   LPVOID lpFileInformation)
{
  BOOL rc;

  dprintf(("KERNEL32: GetFileAttributesExA(%s,%08xh,%08xh) mostly implemented.\n",
           lpFileName,
           fInfoLevelId,
           lpFileInformation));

  if (lpFileName == NULL) return FALSE;
  if (lpFileInformation == NULL) return FALSE;

  if (fInfoLevelId == GetFileExInfoStandard)
  {
    LPWIN32_FILE_ATTRIBUTE_DATA lpFad = (LPWIN32_FILE_ATTRIBUTE_DATA) lpFileInformation;

    rc = OSLibDosGetFileAttributesEx((LPSTR)lpFileName,
                                     fInfoLevelId,
                                     lpFileInformation);
    return (rc);
  }
  else
  {
    dprintf(("KERNEL32: GetFileAttributesExA - invalid info level %d!\n",
             fInfoLevelId));
    return FALSE;
  }
}


/*****************************************************************************
 * Name      : BOOL GetFileAttributesExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : KERNEL32.875
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetFileAttributesExW(LPCWSTR lpFileName, 
                                   GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                   LPVOID lpFileInformation)
{
  LPSTR nameA = HEAP_strdupWtoA( GetProcessHeap(), 0, lpFileName );
  BOOL res = GetFileAttributesExA( nameA, fInfoLevelId, lpFileInformation);
  HeapFree( GetProcessHeap(), 0, nameA );
  return res;
}

//******************************************************************************
//******************************************************************************
HANDLE WIN32API FindFirstChangeNotificationA(LPCSTR lpPathName,
                                             BOOL bWatchSubtree,
                                             DWORD dwNotifyFilter)
{
  dprintf(("KERNEL32:  FindFirstChangeNotificationA %s, Not implemented (faked)", lpPathName));
  return -1;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextChangeNotification(HANDLE hChange)
{
  dprintf(("KERNEL32: FindNextChangeNotification (%08xh), Not implemented\n",
           hChange));

  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindCloseChangeNotification(HANDLE hChange)
{
  dprintf(("KERNEL32:  OS2FindNextChangeNotification, Not implemented\n"));

  return(TRUE);
}
/*****************************************************************************
 * Name      : HANDLE WIN32API FindFirstChangeNotificationW
 * Purpose   : The FindFirstChangeNotification function creates a change
 *             notification handle and sets up initial change notification
 *             filter conditions. A wait on a notification handle succeeds when
 *             a change matching the filter conditions occurs in the specified
 *             directory or subtree.
 * Parameters: LPCWSTR lpPathName           pointer to name of directory to watch
 *             BOOL bWatchSubtree           flag for monitoring directory or
 *                                          directory tree
 *             DWORD dwNotifyFilter         filter conditions to watch for
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to a find
 *             change notification object.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/
HANDLE WIN32API FindFirstChangeNotificationW(LPCWSTR lpPathName,
                                             BOOL  bWatchSubtree,
                                             DWORD dwNotifyFilter)
{
  LPSTR  lpAsciiPath;
  HANDLE hChange;

    lpAsciiPath = UnicodeToAsciiString( (LPWSTR) lpPathName);
    hChange = FindFirstChangeNotificationA(lpAsciiPath, bWatchSubtree,
                                           dwNotifyFilter );
    if (lpAsciiPath) FreeAsciiString(lpAsciiPath);
    return hChange;
}
/*****************************************************************************
 * Name      : BOOL GetOverlappedResult
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in GetOverlappedResult
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API  GetOverlappedResult(HANDLE hFile,              /* [in] handle of file to check on */
                                   LPOVERLAPPED lpOverlapped, /* [in/out] pointer to overlapped  */
                                   LPDWORD lpTransferred,     /* [in/out] number of bytes transferred  */
                                   BOOL bWait)                /* [in] wait for the transfer to complete ? */
{
  //NOTE: According to the SDK docs lpOverlapped->hEvent can be 0. This function
  //      is supposed to wait on the file handle in that case. We don't support
  //      this, so we just fail.
  if(lpOverlapped == NULL || lpOverlapped->hEvent == 0) {
      dprintf(("!ERROR!: lpOverlapped == NULL || lpOverlapped->hEvent == 0"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }

  return HMGetOverlappedResult(hFile,
                               lpOverlapped,
                               lpTransferred,
                               bWait);
}
//******************************************************************************
//******************************************************************************
