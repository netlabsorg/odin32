/* $Id: oslibdos.h,v 1.36 2001-10-25 13:19:05 sandervl Exp $ */

/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBDOS_H__
#define __OSLIBDOS_H__


#ifdef OS2_INCLUDED
DWORD error2WinError(APIRET rc,DWORD defaultCode = ERROR_NOT_ENOUGH_MEMORY_W);
#else
DWORD error2WinError(DWORD rc,DWORD defaultCode = ERROR_NOT_ENOUGH_MEMORY);
#endif

void  OSLibInitWSeBFileIO();

DWORD OSLibDosAliasMem(LPVOID pb, ULONG cb, LPVOID *ppbAlias, ULONG fl);
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags);
DWORD OSLibDosFreeMem(LPVOID lpMemAddr);
DWORD OSLibDosQueryMem(LPVOID lpMemAddr, DWORD *lpRangeSize, DWORD *lpAttr);
DWORD OSLibDosSetMem(LPVOID lpMemAddr, DWORD size, DWORD flags);
DWORD OSLibDosAllocSharedMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags, LPSTR name);
DWORD OSLibDosGetNamedSharedMem(LPVOID *lplpMemAddr, LPSTR name);
DWORD OSLibDosChangeMaxFileHandles();

#define ODIN_INCREMENT_MAX_FILEHANDLES  64
#define ODIN_DEFAULT_MAX_FILEHANDLES    256
DWORD OSLibDosSetInitialMaxFileHandles(DWORD maxhandles);

BOOL OSLibDosGetFileAttributesEx(LPSTR pszName, ULONG ulDummy, PVOID pBuffer);

#define OSLIB_NOERROR                   0
#define OSLIB_ERROR_INVALID_ADDRESS     1
#define OSLIB_ERROR_ACCESS_DENIED       2
#define OSLIB_ERROR_INVALID_PARAMETER   3

#ifndef __OS2_H__

/* Access protection                                                          */
#define PAG_READ          0x00000001U      /* read access                      */
#define PAG_WRITE         0x00000002U      /* write access                     */
#define PAG_EXECUTE       0x00000004U      /* execute access                   */
#define PAG_GUARD         0x00000008U      /* guard protection                 */
#define PAG_DEFAULT       0x00000400U      /* default (initial) access         */

/* Commit                                                                     */
#define PAG_COMMIT        0x00000010U      /* commit storage                   */
#define PAG_DECOMMIT      0x00000020U      /* decommit storage                 */

/* Allocation attributes                                                      */
#define OBJ_TILE          0x00000040U      /* tile object                      */
#define OBJ_PROTECTED     0x00000080U      /* protect object */
#define OBJ_GETTABLE      0x00000100U      /* gettable by other processes      */
#define OBJ_GIVEABLE      0x00000200U      /* giveable to other processes      */

/* Allocation type (returned from DosQueryMem)                                */
#define PAG_SHARED        0x00002000U     /* shared object                    */
#define PAG_FREE          0x00004000U     /* pages are free                   */
#define PAG_BASE          0x00010000U     /* first page in object             */

#endif

#define OSLIB_ACCESS_READONLY           1
#define OSLIB_ACCESS_READWRITE          2
#define OSLIB_ACCESS_SHAREDENYNONE      4
#define OSLIB_ACCESS_SHAREDENYREAD      8
#define OSLIB_ACCESS_SHAREDENYWRITE     16

DWORD OSLibDosOpen(char *lpszFileName, DWORD flags);
BOOL OSLibDosClose(DWORD hFile);
BOOL OSLibDosDelete(char *lpszFileName);
BOOL OSLibDosCopyFile(LPCSTR lpszOldFile, LPCSTR lpszNewFile, BOOL fFailIfExist);
BOOL OSLibDosMoveFile(LPCSTR lpszOldFile, LPCSTR lpszNewFile);
BOOL OSLibDosRemoveDir(LPCSTR lpszDir);
BOOL OSLibDosCreateDirectory(LPCSTR lpszDir);

#define OSLIB_SETPTR_FILE_CURRENT       0
#define OSLIB_SETPTR_FILE_BEGIN         1
#define OSLIB_SETPTR_FILE_END           2

DWORD OSLibDosSetFilePtr(DWORD hFile, DWORD offset, DWORD method);

#define OSLIB_SEARCHDIR         1
#define OSLIB_SEARCHCURDIR      2
#define OSLIB_SEARCHFILE        3
#define OSLIB_SEARCHENV         4

DWORD OSLibDosSearchPath(DWORD cmd, char *path, char *name, char *full_name, DWORD length_fullname);

#ifndef FIL_QUERYFULLNAME
    #define FIL_QUERYFULLNAME  5
#else
    #if FIL_QUERYFULLNAME != 5
        #error "Hmm! FIL_QUERYFULLNAME isn't 5"
    #endif
#endif

DWORD OSLibDosQueryPathInfo(CHAR *pszPathName,
                            ULONG ulInfoLevel,
                            PVOID pInfoBuf,
                            ULONG cbInfoBuf);

DWORD OSLibDosCreateFile(CHAR *lpFileName, DWORD dwAccess,
                         DWORD dwShare, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                         DWORD dwCreation, DWORD dwFlags, HANDLE hTemplate);

DWORD OSLibDosOpenFile(CHAR *lpszFile, UINT fuMode);

BOOL  OSLibDosLockFile(DWORD hFile, DWORD dwFlags,
                       DWORD OffsetLow, DWORD OffsetHigh,
                       DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh,
                       LPOVERLAPPED lpOverlapped);

BOOL  OSLibDosUnlockFile(DWORD hFile, DWORD OffsetLow, DWORD OffsetHigh,
                         DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh,
                         LPOVERLAPPED lpOverlapped);

BOOL  OSLibDosFlushFileBuffers(DWORD hFile);
BOOL  OSLibDosSetEndOfFile(DWORD hFile);

DWORD OSLibDosGetFileSize(DWORD hFile, LPDWORD lpdwFileSizeHigh);
BOOL  OSLibDosRead(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesRead);
BOOL  OSLibDosWrite(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesWritten);

BOOL  OSLibDosGetFileInformationByHandle(DWORD hFile, BY_HANDLE_FILE_INFORMATION* pHFI);

BOOL  OSLibDosSetFileTime(DWORD hFile, WORD creationdate, WORD creationtime,
                          WORD lastaccessdate, WORD lastaccesstime,
                          WORD lastwritedate, WORD lastwritetime);

BOOL  OSLibDosGetFileTime(DWORD hFile, WORD *creationdate, WORD *creationtime,
                          WORD *lastaccessdate, WORD *lastaccesstime,
                          WORD *lastwritedate, WORD *lastwritetime);

DWORD OSLibDosSetFilePointer(DWORD hFile, DWORD OffsetLow, DWORD *OffsetHigh, DWORD method);

DWORD OSLibDosDupHandle(DWORD hFile, DWORD *hNew);
DWORD OSLibDosSetFilePtr2(DWORD hFile, DWORD offset, DWORD method);

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

BOOL OSLibDosQueryProcTimes(DWORD procid, ULONG *kerneltime, ULONG *usertime);

BOOL OSLibDosTransactNamedPipe( DWORD  hNamedPipe,
                                LPVOID lpInBuffer,
                                DWORD nInBufferSize,
                                LPVOID lpOutBuffer,
                                DWORD nOutBufferSize,
                                LPDWORD lpBytesRead,
                                LPOVERLAPPED lpOverlapped);

BOOL OSLibDosCallNamedPipe( LPCTSTR lpNamedPipeName,
                         LPVOID  lpInBuffer,
                         DWORD   nInBufferSize,
                         LPVOID  lpOutBuffer,
                         DWORD   nOutBufferSize,
                         LPDWORD lpBytesRead,
                         DWORD   nTimeOut );

BOOL OSLibDosPeekNamedPipe(DWORD   hPipe,
                        LPVOID  lpvBuffer,
                        DWORD   cbBuffer,
                        LPDWORD lpcbRead,
                        LPDWORD lpcbAvail,
                        LPDWORD lpcbMessage);

BOOL OSLibDosConnectNamedPipe(DWORD hNamedPipe, LPOVERLAPPED lpOverlapped);

DWORD OSLibDosCreateNamedPipe(LPCTSTR lpName,
                              DWORD   dwOpenMode,
                              DWORD   dwPipeMode,
                              DWORD   nMaxInstances,
                              DWORD   nOutBufferSize,
                              DWORD   nInBufferSize,
                              DWORD   nDefaultTimeOut,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes);

BOOL  OSLibSetNamedPipeState(DWORD hNamedPipe, DWORD dwPipeMode);

DWORD OSLibDosOpenPipe(LPCTSTR lpName,
                       DWORD fuAccess,
                       DWORD fuShare,
                       LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                       DWORD fuCreate,
                       DWORD fuAttrFlags);

BOOL OSLibDosWaitNamedPipe(LPCSTR lpszNamedPipeName,
                            DWORD   dwTimeout);

BOOL OSLibDosDisconnectNamedPipe(DWORD hPipe);

BOOL OSLibDosCreatePipe(PHANDLE phfRead,
                        PHANDLE phfWrite,
                        LPSECURITY_ATTRIBUTES lpsa,
                        DWORD   dwSize);


DWORD OSLibDosFindFirst(LPCSTR lpFileName,WIN32_FIND_DATAA* lpFindFileData);
DWORD OSLibDosFindFirstMulti(LPCSTR lpFileName,WIN32_FIND_DATAA *lpFindFileData,DWORD *count);
BOOL  OSLibDosFindNext(DWORD hFindFile,WIN32_FIND_DATAA* lpFindFileData);
BOOL  OSLibDosFindNextMulti(DWORD hFindFile,WIN32_FIND_DATAA *lpFindFileData,DWORD *count);
BOOL  OSLibDosFindClose(DWORD hFindFile);

DWORD OSLibDosQueryVolumeFS(int drive, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
DWORD OSLibDosQueryVolumeSerialAndName(int drive, LPDWORD lpVolumeSerialNumber, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize);
DWORD OSLibDosDevIOCtl( DWORD hFile, DWORD dwCat, DWORD dwFunc,
                        PVOID pParm, DWORD dwParmMaxLen, DWORD *pdwParmLen,
                        PVOID pData, DWORD dwDataMaxLen, DWORD *pdwDataLen);

BOOL OSLibGetDiskFreeSpace(LPSTR lpRootPathName, LPDWORD lpSectorsPerCluster,
                           LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters,
                           LPDWORD lpTotalNumberOfClusters);

ULONG OSLibDosQuerySysInfo(ULONG iStart, ULONG iLast, PVOID pBuf, ULONG cbBuf);
ULONG OSLibGetDriveType(ULONG diskIndex);
ULONG OSLibGetLogicalDrives();
ULONG OSLibDosQueryCurrentDisk();

BOOL  OSLibDosGetDiskGeometry(HANDLE hDisk, DWORD cDisk, void *pGeom);

#ifdef OS2DEF_INCLUDED
#ifndef FIL_STANDARDL
    typedef struct _LONGLONG {  /* LONGLONG */
        ULONG ulLo;
        LONG ulHi;
    } LONGLONG;
    typedef LONGLONG *PLONGLONG;

    typedef struct _ULONGLONG {  /* ULONGLONG */
        ULONG ulLo;
        ULONG ulHi;
    } ULONGLONG;
    typedef ULONGLONG *PULONGLONG;

   #define FIL_STANDARDL         11     /* LFS - Info level 11, standard file info for large files*/
   #define FIL_QUERYEASIZEL      12     /* LFS - Level 12, return Full EA size for large files */
   #define FIL_QUERYEASFROMLISTL 13     /* LFS - Level 13, return requested EA's */

   typedef struct _FILESTATUS3L     /* fsts3L */
   {
      FDATE    fdateCreation;
      FTIME    ftimeCreation;
      FDATE    fdateLastAccess;
      FTIME    ftimeLastAccess;
      FDATE    fdateLastWrite;
      FTIME    ftimeLastWrite;
      LONGLONG cbFile;
      LONGLONG cbFileAlloc;
      ULONG    attrFile;
   } FILESTATUS3L;
   typedef FILESTATUS3L *PFILESTATUS3L;

   /* Large File Support >2GB */
   typedef struct _FILESTATUS4L      /* fsts4L */
   {
      FDATE    fdateCreation;
      FTIME    ftimeCreation;
      FDATE    fdateLastAccess;
      FTIME    ftimeLastAccess;
      FDATE    fdateLastWrite;
      FTIME    ftimeLastWrite;
      LONGLONG cbFile;
      LONGLONG cbFileAlloc;
      ULONG    attrFile;
      ULONG    cbList;
   } FILESTATUS4L;
   typedef FILESTATUS4L  *PFILESTATUS4L;

   typedef struct _FILELOCKL     /* flock */
   {
      LONGLONG lOffset;
      LONGLONG lRange;
   } FILELOCKL;
   typedef FILELOCKL  *PFILELOCKL;
#endif


   typedef APIRET (* APIENTRY PROC_DosSetFileSizeL)(HFILE hFile, LONGLONG cbSize);
   APIRET OdinDosSetFileSize(HFILE hFile,
                             LONGLONG cbSize);

   typedef APIRET (* APIENTRY PROC_DosSetFilePtrL)(HFILE hFile, LONGLONG ib, ULONG method, PLONGLONG ibActual);
   APIRET OdinDosSetFilePtr(HFILE hFile,
                            LONGLONG ib,
                            ULONG method,
                            PLONGLONG ibActual);

   typedef APIRET (* APIENTRY PROC_DosSetFileLocksL)(HFILE hFile, PFILELOCKL pflUnlock, PFILELOCKL pflLock, ULONG timeout, ULONG flags);
   APIRET OdinDosSetFileLocks(HFILE hFile,
                              PFILELOCKL pflUnlock,
                              PFILELOCKL pflLock,
                              ULONG timeout,
                              ULONG flags);
#endif

#endif

ULONG OSLibDosQueryModuleName(ULONG hModule, int cchName, char *pszName);
ULONG OSLibDosQueryDir(DWORD length, LPSTR lpszCurDir);

HINSTANCE OSLibDosLoadModule(LPSTR szModName);
void      OSLibDosFreeModule(HINSTANCE hinst);

ULONG OSLibDosGetModuleFileName(HMODULE hModule, LPTSTR lpszPath, DWORD cchPath);
BOOL  OSLibDosBeep(DWORD ulFreq, DWORD ulDuration);
ULONG OSLibDosGetProcAddress(HMODULE hModule, LPCSTR lpszProc);
