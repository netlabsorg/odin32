/* $Id: Fileio.cpp,v 1.30 2000-05-02 20:53:11 sandervl Exp $ */

/*
 * Win32 File IO API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
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

#define DBG_LOCALLOG    DBG_fileio
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-FILEIO)

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(HFILE,  CreateFileA,
              LPCSTR, lpszName,
              DWORD,  fdwAccess,
              DWORD,  fdwShareMode,
              LPSECURITY_ATTRIBUTES, lpsa,
              DWORD,  fdwCreate,
              DWORD,  fdwAttrsAndFlags,
              HANDLE, hTemplateFile)
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
ODINFUNCTION7(HFILE,   CreateFileW,
              LPCWSTR, arg1,
              DWORD,   arg2,
              DWORD,   arg3,
              PSECURITY_ATTRIBUTES, arg4,
              DWORD,   arg5,
              DWORD,   arg6,
              HANDLE,  arg7)
{
  HANDLE rc;
  char  *astring;

  astring = UnicodeToAsciiString((LPWSTR)arg1);
  rc = CreateFileA(astring, arg2, arg3, arg4, arg5, arg6, arg7);
  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HANDLE, FindFirstFileA,
              LPCSTR, lpFileName,
              WIN32_FIND_DATAA *, lpFindFileData)
{
 HANDLE hFind;
 char  *filename;
 int    namelen;

  dprintf(("FindFirstFileA %s", lpFileName));
  if(lpFileName == NULL || lpFindFileData == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return -1;
  }
  namelen = strlen(lpFileName);
  if(lpFileName[namelen-1] == '\\') {
	filename = (char *)alloca(namelen+1);
	strcpy(filename, lpFileName);
	filename[namelen-1] = 0;
  }
  else	filename = (char *)lpFileName;

  return (HANDLE)OSLibDosFindFirst(filename,lpFindFileData);
}
//******************************************************************************
// internal function for faster access (SHELL32)
//******************************************************************************
ODINFUNCTION3(HANDLE, FindFirstFileMultiA,
              LPCSTR, lpFileName,
              WIN32_FIND_DATAA *, lpFindFileData,
              DWORD *,count)
{
  return (HANDLE)OSLibDosFindFirstMulti(lpFileName,lpFindFileData,count);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HANDLE,  FindFirstFileW,
              LPCWSTR, lpFileName,
              WIN32_FIND_DATAW *, lpFindFileData)
{
  HANDLE           rc;
  char             *astring;
  WIN32_FIND_DATAA wfda;

  astring = UnicodeToAsciiString((LPWSTR)lpFileName);
  rc = (HANDLE)OSLibDosFindFirst(astring,&wfda);

  if(rc == -1) {
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
  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,   FindNextFileA,
              HANDLE, hFindFile,
              WIN32_FIND_DATAA *, lpFindFileData)
{
  return OSLibDosFindNext(hFindFile,lpFindFileData);
}
//******************************************************************************
// internal function for faster access (SHELL32)
//******************************************************************************
ODINFUNCTION3(BOOL,   FindNextFileMultiA,
              HANDLE, hFindFile,
              WIN32_FIND_DATAA *, lpFindFileData,
              DWORD *,count)
{
  return OSLibDosFindNextMulti(hFindFile,lpFindFileData,count);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, FindNextFileW,
              HANDLE, hFindFile,
              WIN32_FIND_DATAW *, lpFindFileData)
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
ODINFUNCTION1(BOOL, FindClose,
              HANDLE, hFindFile)
{
  return OSLibDosFindClose(hFindFile);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GetFileType,
              HANDLE, hFile)
{
  return(HMGetFileType(hFile));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, GetFileInformationByHandle,
              HANDLE, arg1,
              BY_HANDLE_FILE_INFORMATION *, arg2)
{
  return(HMGetFileInformationByHandle(arg1,arg2));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, SetEndOfFile,
              HANDLE, arg1)
{
  return HMSetEndOfFile(arg1);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, SetFileTime,
              HANDLE, arg1,
              const FILETIME *, arg2,
              const FILETIME *, arg3,
              const FILETIME *, arg4)
{
  return HMSetFileTime(arg1,
                       arg2,
                       arg3,
                       arg4);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(INT, CompareFileTime,
              FILETIME *, arg1,
              FILETIME *, arg2)
{
  return O32_CompareFileTime(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, CopyFileA,
              LPCSTR, arg1,
              LPCSTR, arg2,
              BOOL, arg3)
{
  return O32_CopyFile(arg1, arg2, arg3);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
ODINFUNCTION3(BOOL, CopyFileW,
              LPCWSTR, arg1,
              LPCWSTR, arg2,
              BOOL, arg3)
{
  BOOL  rc;
  char *astring1, *astring2;

  astring1 = UnicodeToAsciiString((LPWSTR)arg1);
  astring2 = UnicodeToAsciiString((LPWSTR)arg2);
  rc = O32_CopyFile(astring1, astring2, arg3);
  FreeAsciiString(astring2);
  FreeAsciiString(astring1);
  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, GetFileSize,
              HANDLE, arg1,
              PDWORD, arg2)
{
  return HMGetFileSize(arg1,
                       arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, DeleteFileA,
              LPCSTR, lpszFile)
{
  dprintf(("DeleteFileA %s", lpszFile));
  return O32_DeleteFile(lpszFile);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, DeleteFileW,
              LPCWSTR, arg1)
{
  BOOL  rc;
  char *astring;

  astring = UnicodeToAsciiString((LPWSTR)arg1);
  rc = ODIN_DeleteFileA(astring);
  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(UINT, GetTempFileNameA,
              LPCSTR, arg1,
              LPCSTR, arg2,
              UINT, arg3,
              LPSTR, arg4)
{
  return O32_GetTempFileName(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(UINT, GetTempFileNameW,
              LPCWSTR, lpPathName,
              LPCWSTR, lpPrefixString,
              UINT, uUnique,
              LPWSTR, lpTempFileName)
{
  char *asciipath, *asciiprefix;
  char *asciitemp = (char *)malloc(MAX_PATH+1);
  UINT rc;

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
ODINFUNCTION2(UINT, GetTempPathA,
              UINT, arg1,
              LPSTR, arg2)
{
  return O32_GetTempPath(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(UINT, GetTempPathW,
              UINT, nBufferLength,
              LPWSTR, lpBuffer)
{
  char *asciibuffer = (char *)malloc(nBufferLength+1);
  DWORD rc;

  rc = O32_GetTempPath(nBufferLength, asciibuffer);
  if(rc)      AsciiToUnicode(asciibuffer, lpBuffer);
  free(asciibuffer);
  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL,         ReadFile,
              HANDLE,       hFile,
              PVOID,        pBuffer,
              DWORD,        dwLength,
              PDWORD,       lpNumberOfBytesRead,
              LPOVERLAPPED, lpOverlapped)
{
  return (HMReadFile(hFile,
                     pBuffer,
                     dwLength,
                     lpNumberOfBytesRead,
                     lpOverlapped));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, SetFilePointer,
              HANDLE, hFile,
              LONG, lDistanceToMove,
              PLONG, lpDistanceToMoveHigh,
              DWORD, dwMoveMethod)
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
ODINFUNCTION5(BOOL, WriteFile,
              HANDLE, hFile,
              LPCVOID, buffer,
              DWORD, nrbytes,
              LPDWORD, nrbyteswritten,
              LPOVERLAPPED, lpOverlapped)
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
ODINFUNCTION1(DWORD, GetFileAttributesA,
              LPCSTR, lpszFileName)
{
 DWORD rc, error;

    if((NULL!=lpszFileName) && strlen(lpszFileName)==2 && lpszFileName[1] == ':')
    {
        char szDrive[4];
        szDrive[0] = lpszFileName[0];
        szDrive[1] = lpszFileName[1];
        szDrive[2] = '\\';
        szDrive[3] = 0x00;
        rc = O32_GetFileAttributes((LPSTR)szDrive);
    }
    else {
	rc = O32_GetFileAttributes((LPSTR)lpszFileName);
	if(rc == -1 && lpszFileName[strlen(lpszFileName)-1] != '\\') {
		char *filename = (char *)alloca(strlen(lpszFileName)+1);
		strcpy(filename, lpszFileName);
                strcat(filename, "\\");
		rc = O32_GetFileAttributes((LPSTR)filename);
	}
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
ODINFUNCTION1(DWORD, GetFileAttributesW,
              LPCWSTR, arg1)
{
 DWORD rc;
 char *astring;

    dprintf(("KERNEL32:  GetFileAttributesW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = ODIN_GetFileAttributesA(astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SetFileAttributesA,
              LPCSTR, arg1,
              DWORD, arg2)
{
    dprintf(("KERNEL32:  SetFileAttributes of %s\n", arg1));
    return O32_SetFileAttributes(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SetFileAttributesW,
              LPCWSTR, lpFileName,
              DWORD, dwFileAttributes)
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
ODINFUNCTION4(DWORD, GetFullPathNameA,
              LPCSTR, arg1,
              DWORD, arg2,
              LPSTR, arg3,
              LPSTR *, arg4)
{
    char *ptr;
    dprintf(("KERNEL32:  GetFullPathName %s\n", arg1));
      while((ptr = strchr(arg1, '/')) != NULL)
        *ptr = '\\';
    return O32_GetFullPathName(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, GetFullPathNameW,
              LPCWSTR, lpFileName,
              DWORD, nBufferLength,
              LPWSTR, lpBuffer,
              LPWSTR *, lpFilePart)
{
 char *astring, *asciibuffer, *asciipart;
 DWORD rc;

  asciibuffer = (char *)malloc(nBufferLength+1);
  astring     = UnicodeToAsciiString((LPWSTR)lpFileName);

  rc = ODIN_GetFullPathNameA(astring,
                             nBufferLength,
                             asciibuffer,
                             &asciipart);

  dprintf(("KERNEL32: GetFullPathNameW %s returns %s\n",
           astring,
           asciibuffer));

  if(rc)
    AsciiToUnicode(asciibuffer,
                   lpBuffer);

  if(lpFilePart)
    *lpFilePart = lpBuffer + ((int)asciipart - (int)asciibuffer);

  FreeAsciiString(astring);
  free(asciibuffer);
  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, LockFile,
              HANDLE, arg1,
              DWORD, arg2,
              DWORD, arg3,
              DWORD, arg4,
              DWORD, arg5)
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

ODINFUNCTION6(BOOL, LockFileEx,
              HANDLE, hFile,
              DWORD, dwFlags,
              DWORD, dwReserved,
              DWORD, nNumberOfBytesToLockLow,
              DWORD, nNumberOfBytesToLockHigh,
              LPOVERLAPPED, lpOverlapped)
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
ODINFUNCTION2(BOOL, MoveFileA,
              LPCSTR, arg1,
              LPCSTR, arg2)
{
    dprintf(("KERNEL32: MoveFileA %s %s", arg1, arg2));
    return O32_MoveFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, MoveFileExA,
              LPCSTR, arg1,
              LPCSTR, arg2,
              DWORD, fdwFlags)
{
    dprintf(("KERNEL32:  MoveFileExA %s to %s, not complete!\n", arg1, arg2));
    return O32_MoveFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, MoveFileW,
              LPCWSTR, lpSrc,
              LPCWSTR, lpDest)
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
ODINFUNCTION3(BOOL, MoveFileExW,
              LPCWSTR, arg1,
              LPCWSTR, arg2,
              DWORD, fdwFlags)
{
    dprintf(("KERNEL32:  MoveFileExW %s to %s, not complete!\n", arg1, arg2));
    return MoveFileW(arg1, arg2);
}
//******************************************************************************
/*****************************************************************************
ODINFUNCTION3(*, :,
              HFILE, WIN32API,
              OpenFile *, Purpose,
              :, forwardOpenFile to Open32
 * Parameters:
 * Variables :
 * Result    : API returncode
 * Remark    : modified for handle translation support
 * Status    : @@@PH verify if 0 is a valid "invalid handle" :)
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 02:53]
 *****************************************************************************/

ODINFUNCTION3(HFILE, OpenFile,
              LPCSTR, lpszFile,
              OFSTRUCT *, lpOpenBuff,
              UINT, fuMode)
{
  HFILE hFile;

  dprintf(("KERNEL32: OpenFile(%s, %08xh, %08xh)\n",
           lpszFile,
           lpOpenBuff,
           fuMode));

  hFile = HMOpenFile(lpszFile,                                      /* call open32 */
                     lpOpenBuff,
                     fuMode);

  return (hFile);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, UnlockFile,
              HANDLE, arg1,
              DWORD, arg2,
              DWORD, arg3,
              DWORD, arg4,
              DWORD, arg5)
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

ODINFUNCTION5(BOOL, UnlockFileEx,
              HANDLE, hFile,
              DWORD, dwReserved,
              DWORD, nNumberOfBytesToLockLow,
              DWORD, nNumberOfBytesToLockHigh,
              LPOVERLAPPED, lpOverlapped)
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
ODINFUNCTION3(DWORD, GetShortPathNameA,
              LPCTSTR, lpszLongPath,
              LPTSTR, lpszShortPath,
              DWORD, cchBuffer)
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
ODINFUNCTION3(DWORD, GetShortPathNameW,
              LPCWSTR, lpszLongPath,
              LPWSTR, lpszShortPath,
              DWORD, cchBuffer)
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
ODINPROCEDURE0(SetFileApisToANSI)
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

ODINFUNCTION2(DWORD, GetCompressedFileSizeA,
              LPCTSTR, lpFileName,
              LPDWORD, lpFileSizeHigh)
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

ODINFUNCTION2(DWORD, GetCompressedFileSizeW,
              LPCWSTR, lpFileName,
              LPDWORD, lpFileSizeHigh)
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

ODINFUNCTION3(BOOL, GetFileAttributesExA,
              LPCSTR, lpFileName,
              GET_FILEEX_INFO_LEVELS, fInfoLevelId,
              LPVOID, lpFileInformation)
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

ODINFUNCTION3(BOOL, GetFileAttributesExW,
              LPCWSTR, lpFileName,
              GET_FILEEX_INFO_LEVELS, fInfoLevelId,
              LPVOID, lpFileInformation)
{
  LPSTR nameA = HEAP_strdupWtoA( GetProcessHeap(), 0, lpFileName );
  BOOL res = GetFileAttributesExA( nameA, fInfoLevelId, lpFileInformation);
  HeapFree( GetProcessHeap(), 0, nameA );
  return res;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(HANDLE, FindFirstChangeNotificationA,
              LPCSTR, lpPathName,
              BOOL, bWatchSubtree,
              DWORD, dwNotifyFilter)
{
  dprintf(("KERNEL32:  FindFirstChangeNotificationA, Not implemented (faked)\n"));
  return -1;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, FindNextChangeNotification,
              HANDLE, hChange)
{
  dprintf(("KERNEL32: FindNextChangeNotification (%08xh), Not implemented\n",
           hChange));

  return FALSE;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, FindCloseChangeNotification, HANDLE, hChange)
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
ODINFUNCTION3(HANDLE, FindFirstChangeNotificationW, LPCWSTR, lpPathName,
                                                    BOOL, bWatchSubtree,
                                                    DWORD, dwNotifyFilter)
{
  LPSTR  lpAsciiPath;
  HANDLE hChange;

    lpAsciiPath = UnicodeToAsciiString( (LPWSTR) lpPathName);
    hChange = FindFirstChangeNotificationA(lpAsciiPath, bWatchSubtree, 
                                           dwNotifyFilter );
    if (lpAsciiPath) FreeAsciiString(lpAsciiPath);
    return hChange;
}
//******************************************************************************
//******************************************************************************
