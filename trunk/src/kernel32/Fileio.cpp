/* $Id: Fileio.cpp,v 1.6 1999-06-17 18:21:42 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 File IO API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "handlemanager.h"
#include "devio.h"

//******************************************************************************
//******************************************************************************
HFILE WIN32API CreateFileA(LPCSTR  lpszName,
                           DWORD  fdwAccess,
                           DWORD  fdwShareMode,
                           LPSECURITY_ATTRIBUTES lpsa,
                           DWORD  fdwCreate,
                           DWORD  fdwAttrsAndFlags,
                           HANDLE hTemplateFile)
{
  HANDLE rc;

  dprintf(("KERNEL32: CreateFileA(%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpszName,
           fdwAccess,
           fdwShareMode,
           lpsa,
           fdwCreate,
           fdwAttrsAndFlags,
           hTemplateFile));

    rc = HMCreateFile(lpszName,
                      fdwAccess,
                      fdwShareMode,
                      lpsa,
                      fdwCreate,
                      fdwAttrsAndFlags,
                      hTemplateFile);

  return(rc);
}
//******************************************************************************
//******************************************************************************
HFILE  WIN32API CreateFileW(LPCWSTR  arg1,
                            DWORD   arg2,
                            DWORD   arg3,
                            PSECURITY_ATTRIBUTES arg4,
                            DWORD   arg5,
                            DWORD   arg6,
                            HANDLE  arg7)
{
 HANDLE rc;
 char  *astring;

   dprintf(("KERNEL32:  CreateFileW"));
   astring = UnicodeToAsciiString((LPWSTR)arg1);
   rc = CreateFileA(astring, arg2, arg3, arg4, arg5, arg6, arg7);
   FreeAsciiString(astring);
   return(rc);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API FindFirstFileA(LPCSTR arg1, WIN32_FIND_DATAA * arg2)
{
    dprintf(("KERNEL32:  FindFirstFile %s\n", arg1));

    return O32_FindFirstFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API FindFirstFileW(LPCWSTR arg1, WIN32_FIND_DATAW *  arg2)
{
 HANDLE rc;
 char  *astring;

    dprintf(("KERNEL32: FindFirstFileW: not implemented!!"));

    astring = UnicodeToAsciiString((LPWSTR)arg1);
//    rc = O32_FindFirstFile(astring, arg2);
    rc = 0;
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextFileA(HANDLE arg1, WIN32_FIND_DATAA * arg2)
{
    dprintf(("KERNEL32:  FindNextFile"));
    return O32_FindNextFile(arg1, arg2);
}
//******************************************************************************
//TODO: convert string in WIN32_FIND_DATAW * structure
//******************************************************************************
BOOL WIN32API FindNextFileW(HANDLE arg1, WIN32_FIND_DATAW * arg2)
{
    dprintf(("KERNEL32:  FindNextFileW Not properly implemented!\n"));
//    return O32_FindNextFile(arg1, arg2);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindClose(HANDLE arg1)
{
    dprintf(("KERNEL32:  FindClose\n"));
    return O32_FindClose(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFileType(HANDLE hFile)
{
  dprintf(("KERNEL32: GetFileType(%08xh)\n",
           hFile));

  return(HMGetFileType(hFile));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFileInformationByHandle(HANDLE                      arg1,
                                          BY_HANDLE_FILE_INFORMATION *arg2)
{
  dprintf(("KERNEL32: GetFIleInformationByHandle(%08xh,%0xh)\n",
           arg1,
           arg2));

  return(HMGetFileInformationByHandle(arg1,arg2));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetEndOfFile( HANDLE arg1)
{
  dprintf(("KERNEL32: SetEndOfFile(%08xh)\n",
           arg1));

  return HMSetEndOfFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetFileTime(HANDLE          arg1,
                          const FILETIME *arg2,
                          const FILETIME *arg3,
                          const FILETIME *arg4)
{
  dprintf(("KERNEL32: SetFileTime(%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3,
           arg4));

  return HMSetFileTime(arg1,
                       arg2,
                       arg3,
                       arg4);
}
//******************************************************************************
//******************************************************************************
INT WIN32API CompareFileTime( FILETIME * arg1, FILETIME *  arg2)
{
    dprintf(("KERNEL32:  CompareFileTime\n"));
    return O32_CompareFileTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CopyFileA(LPCSTR arg1, LPCSTR arg2, BOOL arg3)
{
    dprintf(("KERNEL32:  CopyFile %s %s\n", arg1, arg2));
    return O32_CopyFile(arg1, arg2, arg3);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
BOOL WIN32API CopyFileW(LPCWSTR arg1, LPCWSTR arg2, BOOL  arg3)
{
 BOOL  rc;
 char *astring1, *astring2;

    astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    astring2 = UnicodeToAsciiString((LPWSTR)arg2);
    dprintf(("KERNEL32:  CopyFileW\n"));
    rc = O32_CopyFile(astring1, astring2, arg3);
    FreeAsciiString(astring2);
    FreeAsciiString(astring1);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFileSize( HANDLE arg1, PDWORD  arg2)
{
  dprintf(("KERNEL32: GetFileSize (%08xh, %08xh)\n",
           arg1,
           arg2));

  return HMGetFileSize(arg1,
                       arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteFileA(LPCSTR arg1)
{
    dprintf(("KERNEL32:  DeleteFile %s\n", arg1));
    return O32_DeleteFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteFileW(LPCWSTR arg1)
{
 BOOL  rc;
 char *astring;

    dprintf(("KERNEL32:  DeleteFileW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = O32_DeleteFile(astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTempFileNameA(LPCSTR arg1, LPCSTR arg2, UINT arg3, LPSTR  arg4)
{
    dprintf(("KERNEL32:  GetTempFileName"));
    return O32_GetTempFileName(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTempFileNameW(LPCWSTR lpPathName, LPCWSTR lpPrefixString,
                               UINT uUnique, LPWSTR lpTempFileName)
{
 char *asciipath, *asciiprefix;
 char *asciitemp = (char *)malloc(MAX_PATH+1);
 UINT rc;

    dprintf(("KERNEL32:  GetTempFileNameW"));
    asciipath   = UnicodeToAsciiString((LPWSTR)lpPathName);
    asciiprefix = UnicodeToAsciiString((LPWSTR)lpPrefixString);
    rc = O32_GetTempFileName(asciipath, asciiprefix, uUnique, asciitemp);
    if(rc)      AsciiToUnicode(asciitemp, lpTempFileName);
    FreeAsciiString(asciiprefix);
    FreeAsciiString(asciipath);
    free(asciitemp);
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTempPathA(UINT arg1, LPSTR arg2)
{
    dprintf(("KERNEL32:  GetTempPath\n"));
    return O32_GetTempPath(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTempPathW(UINT nBufferLength, LPWSTR lpBuffer)
{
 char *asciibuffer = (char *)malloc(nBufferLength+1);
 DWORD rc;

    dprintf(("KERNEL32:  GetTempPathW\n"));
    rc = O32_GetTempPath(nBufferLength, asciibuffer);
    if(rc)      AsciiToUnicode(asciibuffer, lpBuffer);
    free(asciibuffer);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReadFile(HANDLE arg1,
                       PVOID  arg2,
                       DWORD  arg3,
                       PDWORD arg4,
                       LPOVERLAPPED  arg5)
{
  BOOL rc;

  rc = HMReadFile(arg1,
                  arg2,
                  arg3,
                  arg4,
                  arg5);

  dprintf(("KERNEL32:  ReadFile %X %d bytes returned %d\n",
           arg1,
           arg3,
           rc));

  return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SetFilePointer(HANDLE hFile,
                              LONG   lDistanceToMove,
                              PLONG  lpDistanceToMoveHigh,
                              DWORD  dwMoveMethod)
{
  dprintf(("KERNEL32: SetFilePointer(%08xh,%08xh,%08xh,%08xh)\n",
           hFile,
           lDistanceToMove,
           lpDistanceToMoveHigh,
           dwMoveMethod));

  return(HMSetFilePointer(hFile,
                         lDistanceToMove,
                         lpDistanceToMoveHigh,
                         dwMoveMethod));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WriteFile(HANDLE  hFile,
                        LPCVOID buffer,
                        DWORD   nrbytes,
                        LPDWORD nrbyteswritten,
                        LPOVERLAPPED lpOverlapped)
{
  dprintf(("KERNEL32: WriteFile(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           hFile,
           buffer,
           nrbytes,
           nrbyteswritten,
           lpOverlapped));

  return (HMWriteFile(hFile,
                      buffer,
                      nrbytes,
                      nrbyteswritten,
                      lpOverlapped));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SearchPathA( LPCSTR arg1, LPCSTR arg2, LPCSTR arg3, DWORD arg4, LPSTR arg5, LPSTR * arg6)
{
    dprintf(("KERNEL32:  SearchPathA\n"));
    return O32_SearchPath(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SearchPathW(LPCWSTR lpPath, LPCWSTR lpFileName, LPCWSTR lpExtension,
                           DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR *lpFilePart)
{
 char *asciipath, *asciifile, *asciiext, *asciibuffer, *asciipart;
 DWORD rc;

    dprintf(("KERNEL32:  SearchPathW"));
    asciibuffer = (char *)malloc(nBufferLength+1);
    asciipath = UnicodeToAsciiString((LPWSTR)lpPath);
    asciifile = UnicodeToAsciiString((LPWSTR)lpFileName);
    asciiext  = UnicodeToAsciiString((LPWSTR)lpFileName);
    rc = O32_SearchPath(asciipath, asciifile, asciiext, nBufferLength, asciibuffer, &asciipart);

    if(rc) {
        AsciiToUnicode(asciibuffer, lpBuffer);
        *lpFilePart = lpBuffer + ((int)asciipart - (int)asciibuffer);
    }
    FreeAsciiString(asciiext);
    FreeAsciiString(asciifile);
    FreeAsciiString(asciipath);
    free(asciibuffer);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFileAttributesA(LPCSTR lpszFileName)
{
 DWORD rc;

    rc = O32_GetFileAttributes((LPSTR)lpszFileName);
    dprintf(("KERNEL32:  GetFileAttributes of %s returned %d\n", lpszFileName, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFileAttributesW(LPCWSTR arg1)
{
 DWORD rc;
 char *astring;

    dprintf(("KERNEL32:  GetFileAttributesW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = O32_GetFileAttributes(astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetFileAttributesA(LPCSTR arg1, DWORD  arg2)
{
    dprintf(("KERNEL32:  SetFileAttributes of %s\n", arg1));
    return O32_SetFileAttributes(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetFileAttributesW(LPCWSTR lpFileName, DWORD dwFileAttributes)
{
 char *asciifile;
 BOOL  rc;

    dprintf(("KERNEL32:  SetFileAttributesW\n"));
    asciifile = UnicodeToAsciiString((LPWSTR)lpFileName);
    rc = O32_SetFileAttributes(asciifile, dwFileAttributes);
    FreeAsciiString(asciifile);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFullPathNameA(LPCSTR arg1, DWORD arg2, LPSTR arg3, LPSTR *arg4)
{
    dprintf(("KERNEL32:  GetFullPathName %s\n", arg1));
    return O32_GetFullPathName(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFullPathNameW(LPCWSTR lpFileName, DWORD nBufferLength,
                                LPWSTR lpBuffer, LPWSTR *lpFilePart)
{
 char *astring, *asciibuffer, *asciipart;
 DWORD rc;

  dprintf(("KERNEL32: GetFullPathNameW\n"));
  asciibuffer = (char *)malloc(nBufferLength+1);
  astring     = UnicodeToAsciiString((LPWSTR)lpFileName);

  rc = O32_GetFullPathName(astring,
                       nBufferLength,
                       asciibuffer,
                       &asciipart);

  dprintf(("KERNEL32: GetFullPathNameW %s returns %s\n",
           astring,
           asciibuffer));

  if(rc)
    AsciiToUnicode(asciibuffer,
                   lpBuffer);

    *lpFilePart = lpBuffer + ((int)asciipart - (int)asciibuffer);

    FreeAsciiString(astring);
    free(asciibuffer);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LockFile(HANDLE arg1,
                       DWORD  arg2,
                       DWORD  arg3,
                       DWORD  arg4,
                       DWORD  arg5)
{
  dprintf(("KERNEL32: LockFile (%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3,
           arg4,
           arg5));

  return HMLockFile(arg1,
                    arg2,
                    arg3,
                    arg4,
                    arg5);
}


/*****************************************************************************
 * Name      : BOOL LockFileEx
 * Purpose   : The LockFileEx function locks a byte range within an open file for shared or exclusive access.
 * Parameters: HANDLE hFile                     handle of file to lock
 *             DWORD  dwFlags                   functional behavior modification flags
 *             DWORD  dwReserved                reserved, must be set to zero
 *             DWORD  nNumberOfBytesToLockLow   low-order 32 bits of length to lock
 *             DWORD  nNumberOfBytesToLockHigh  high-order 32 bits of length to lock
 *             LPOVERLAPPED  LPOVERLAPPED       addr. of structure with lock region start offset
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API LockFileEx(HANDLE       hFile,
                         DWORD        dwFlags,
                         DWORD        dwReserved,
                         DWORD        nNumberOfBytesToLockLow,
                         DWORD        nNumberOfBytesToLockHigh,
                         LPOVERLAPPED lpOverlapped)
{
  dprintf(("Kernel32: LockFileEx(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           hFile,
           dwFlags,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));

  return(HMLockFile(hFile,
                    lpOverlapped->Offset,
                    lpOverlapped->OffsetHigh,
                    nNumberOfBytesToLockLow,
                    nNumberOfBytesToLockHigh));
}




//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveFileA( LPCSTR arg1, LPCSTR  arg2)
{
    dprintf(("KERNEL32:  MoveFileA\n"));
    return O32_MoveFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveFileExA(LPCSTR arg1,
                             LPCSTR arg2,
                             DWORD  fdwFlags)
{
    dprintf(("KERNEL32:  MoveFileExA %s to %s, not complete!\n", arg1, arg2));
    return O32_MoveFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveFileW(LPCWSTR lpSrc, LPCWSTR lpDest)
{
 char *asciisrc, *asciidest;
 BOOL  rc;

    dprintf(("KERNEL32:  MoveFileW\n"));
    asciisrc  = UnicodeToAsciiString((LPWSTR)lpSrc);
    asciidest = UnicodeToAsciiString((LPWSTR)lpDest);
    rc = O32_MoveFile(asciisrc, asciidest);
    FreeAsciiString(asciisrc);
    FreeAsciiString(asciidest);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveFileExW(LPCWSTR arg1,
                          LPCWSTR arg2,
                          DWORD  fdwFlags)
{
    dprintf(("KERNEL32:  MoveFileExW %s to %s, not complete!\n", arg1, arg2));
    return MoveFileW(arg1, arg2);
}
//******************************************************************************
/*****************************************************************************
 * Name      : HFILE WIN32API OpenFile
 * Purpose   : forward OpenFile to Open32
 * Parameters:
 * Variables :
 * Result    : API returncode
 * Remark    : modified for handle translation support
 * Status    : @@@PH verify if 0 is a valid "invalid handle" :)
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 02:53]
 *****************************************************************************/

HFILE WIN32API OpenFile(LPCSTR    arg1,
                        OFSTRUCT *arg2,
                        UINT      arg3)
{
  HFILE hFile;

  dprintf(("KERNEL32: OpenFile(%s, %08xh, %08xh)\n",
           arg1,
           arg2,
           arg3));

  hFile = HMOpenFile(arg1,                                      /* call open32 */
                     arg2,
                     arg3);

  return (hFile);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnlockFile(HANDLE arg1,
                         DWORD  arg2,
                         DWORD  arg3,
                         DWORD  arg4,
                         DWORD  arg5)
{
  dprintf(("KERNEL32: UnlockFile(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3,
           arg4,
           arg5));

  return HMUnlockFile(arg1,
                      arg2,
                      arg3,
                      arg4,
                      arg5);
}


/*****************************************************************************
 * Name      : BOOL UnlockFileEx
 * Purpose   : The UnlockFileEx function unlocks a previously locked byte range in an open file.
 * Parameters: HANDLE hFile                     handle of file to lock
 *             DWORD  dwReserved                reserved, must be set to zero
 *             DWORD  nNumberOfBytesToLockLow   low-order 32 bits of length to lock
 *             DWORD  nNumberOfBytesToLockHigh  high-order 32 bits of length to lock
 *             LPOVERLAPPED  LPOVERLAPPED       addr. of structure with lock region start offset
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UnlockFileEx(HANDLE       hFile,
                           DWORD        dwReserved,
                           DWORD        nNumberOfBytesToLockLow,
                           DWORD        nNumberOfBytesToLockHigh,
                           LPOVERLAPPED lpOverlapped)
{
  dprintf(("Kernel32: UnlockFileEx(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           hFile,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));

  return(HMUnlockFile(hFile,
                      lpOverlapped->Offset,
                      lpOverlapped->OffsetHigh,
                      nNumberOfBytesToLockLow,
                      nNumberOfBytesToLockHigh));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetShortPathNameA(LPCTSTR lpszLongPath, LPTSTR lpszShortPath,
                                 DWORD cchBuffer)
{
 int length;

  dprintf(("KERNEL32:  GetShortPathNameA of %s, just copying it\n", lpszLongPath));
  length = strlen(lpszLongPath) + 1;
  if(length > cchBuffer) {
        *lpszShortPath = 0;
        return(length);
  }
  memcpy(lpszShortPath, lpszLongPath, length);
  return(length-1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetShortPathNameW(LPCWSTR lpszLongPath, LPWSTR lpszShortPath,
                                 DWORD cchBuffer)
{
 int length;

  dprintf(("KERNEL32:  GetShortPathNameW; just copying it\n"));
  length = UniStrlen((UniChar*)lpszLongPath) + 1;
  if(length > cchBuffer) {
        *lpszShortPath = 0;
        return(length);
  }
  memcpy(lpszShortPath, lpszLongPath, length*sizeof(USHORT));
  return(length-1);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API FindFirstChangeNotificationA(LPCSTR lpPathName, BOOL bWatchSubtree,
                                                DWORD dwNotifyFilter)
{
  dprintf(("KERNEL32:  FindFirstChangeNotificationA, Not implemented\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FindNextChangeNotification(HANDLE hChange)
{
  dprintf(("KERNEL32: FindNextChangeNotification (%08xh), Not implemented\n",
           hChange));

  return(0);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API SetFileApisToANSI( VOID ) /*PLF Tue  98-02-10 00:52:22*/
{
    dprintf(("SetFileApisToANSI() stub\n"));
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

DWORD WIN32API GetCompressedFileSizeA(LPCTSTR  lpFileName,
                                      LPDWORD  lpFileSizeHigh)
{
  dprintf(("KERNEL32: GetCompressedFileSizeA (%s, %08xh) not implemented.\n",
           lpFileName,
           lpFileSizeHigh));

  /* PH: simply return the standard filesize */
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

DWORD WIN32API GetCompressedFileSizeW(LPCWSTR  lpFileName,
                                         LPDWORD  lpFileSizeHigh)
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
