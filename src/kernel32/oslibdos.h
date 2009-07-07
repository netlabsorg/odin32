/* $Id: oslibdos.h,v 1.52 2003-03-27 14:00:53 sandervl Exp $ */

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

#ifdef __cplusplus
extern "C" {
#endif

char* ODINHelperStripUNC(char* strUNC);

#ifdef OS2_INCLUDED
DWORD error2WinError(APIRET rc,DWORD defaultCode = ERROR_NOT_ENOUGH_MEMORY_W);
#else
DWORD error2WinError(DWORD rc,DWORD defaultCode = ERROR_NOT_ENOUGH_MEMORY);
#endif

void  OSLibInitWSeBFileIO();

DWORD OSLibDosChangeMaxFileHandles();

#define ODIN_INCREMENT_MAX_FILEHANDLES  64
#define ODIN_DEFAULT_MAX_FILEHANDLES    256
DWORD OSLibDosSetInitialMaxFileHandles(DWORD maxhandles);

BOOL OSLibDosGetFileAttributesEx(LPSTR pszName, ULONG ulDummy, PVOID pBuffer);

#define OSLIB_NOERROR                   0
#define OSLIB_ERROR_INVALID_ADDRESS     1
#define OSLIB_ERROR_ACCESS_DENIED       2
#define OSLIB_ERROR_INVALID_PARAMETER   3

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

BOOL  OSLibDosSetFileTime(DWORD hFile, LPFILETIME pFT1,
                                    LPFILETIME pFT2,
                                    LPFILETIME pFT3);

BOOL  OSLibDosGetFileTime(DWORD hFile, LPFILETIME pFT1,
                                    LPFILETIME pFT2,
                                    LPFILETIME pFT3);

DWORD OSLibDosSetFilePointer(DWORD hFile, DWORD OffsetLow, DWORD *OffsetHigh, DWORD method);

DWORD OSLibDosDupHandle(DWORD hFile, DWORD *hNew);
DWORD OSLibDosSetFHState(DWORD hFile, DWORD dwFlags);

DWORD OSLibDosSetFilePtr2(DWORD hFile, DWORD offset, DWORD method);

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

DWORD OSLibGetFileAttributes(LPSTR lpFileName);

DWORD OSLibDosQueryVolumeFS(int drive, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
DWORD OSLibDosQueryVolumeSerialAndName(int drive, LPDWORD lpVolumeSerialNumber, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize);

/* Disk/Diskette Control */

#ifndef __OS2_H__
#define IOCTL_DISK 	                   8

#define DSK_LOCKDRIVE                      0x0000
#define DSK_UNLOCKDRIVE                    0x0001
#define DSK_REDETERMINEMEDIA               0x0002
#define DSK_SETLOGICALMAP                  0x0003
#define DSK_BEGINFORMAT                    0X0004
#define DSK_BLOCKREMOVABLE                 0x0020
#define DSK_GETLOGICALMAP                  0x0021
#define DSK_UNLOCKEJECTMEDIA               0X0040
#define DSK_SETDEVICEPARAMS                0x0043
#define DSK_WRITETRACK                     0x0044
#define DSK_FORMATVERIFY                   0x0045
#define DSK_DISKETTECONTROL                0X005D
#define DSK_QUERYMEDIASENSE                0X0060
#define DSK_GETDEVICEPARAMS                0x0063
#define DSK_READTRACK                      0x0064
#define DSK_VERIFYTRACK                    0x0065
#define DSK_GETLOCKSTATUS                  0X0066
#endif

DWORD SYSTEM OSLibDosDevIOCtl( DWORD hFile, DWORD dwCat, DWORD dwFunc,
                               PVOID pParm, DWORD dwParmMaxLen, DWORD *pdwParmLen,
                               PVOID pData, DWORD dwDataMaxLen, DWORD *pdwDataLen);

BOOL OSLibGetDiskFreeSpace(LPSTR lpRootPathName, LPDWORD lpSectorsPerCluster,
                           LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters,
                           LPDWORD lpTotalNumberOfClusters);

ULONG OSLibDosQuerySysInfo(ULONG iStart, ULONG iLast, PVOID pBuf, ULONG cbBuf);
ULONG OSLibGetDriveType(ULONG diskIndex);
ULONG OSLibGetLogicalDrives();
ULONG OSLibGetDriveType(ULONG DriveIndex);
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


   typedef APIRET (* APIENTRY PROC_DosOpenL)(PCSZ  pszFileName, PHFILE phf,
                                             PULONG pulAction,
                                             LONGLONG cbFile,
                                             ULONG ulAttribute,
                                             ULONG fsOpenFlags,
                                             ULONG fsOpenMode,
                                             PEAOP2 peaop2);

   APIRET APIENTRY OdinDosOpenL(PCSZ  pszFileName,
                                PHFILE phf,
                                PULONG pulAction,
                                LONGLONG cbFile,
                                ULONG ulAttribute,
                                ULONG fsOpenFlags,
                                ULONG fsOpenMode,
                                PEAOP2 peaop2);

   #ifndef AFNTY_THREAD                 /* don't declare this if it's allready there */

      typedef struct _MPAFFINITY { /* afnty */
         ULONG          mask[2]; /* CPUs 0 thru 31 in [0], CPUs 32 thru 63 in [1] */
      } MPAFFINITY;
      typedef MPAFFINITY *PMPAFFINITY;


      /* scope values for QueryThreadAffinity */

      #define AFNTY_THREAD       0
      #define AFNTY_SYSTEM       1

   #endif

   typedef APIRET (* APIENTRY PROC_DosQueryThreadAffinity)(ULONG scope,
                                                           PMPAFFINITY pAffinity);

   typedef APIRET (* APIENTRY PROC_DosSetThreadAffinity)(PMPAFFINITY pAffinity);

#endif

ULONG OSLibDosQueryModuleName(ULONG hModule, int cchName, char *pszName);
ULONG OSLibDosQueryDir(DWORD length, LPSTR lpszCurDir);

HINSTANCE OSLibDosLoadModule(LPSTR szModName);
void      OSLibDosFreeModule(HINSTANCE hinst);

ULONG OSLibDosGetModuleFileName(HMODULE hModule, LPTSTR lpszPath, DWORD cchPath);
BOOL  OSLibDosBeep(DWORD ulFreq, DWORD ulDuration);
ULONG OSLibDosGetProcAddress(HMODULE hModule, LPCSTR lpszProc);

BOOL  OSLibDosSetThreadAffinity(DWORD dwThreadAffinityMask);

#define MASK_SYSTEM	0
#define MASK_THREAD     1
BOOL  OSLibDosQueryAffinity(DWORD fMaskType, DWORD *pdwThreadAffinityMask);

DWORD OSLibDosDevConfig(PVOID pdevinfo, ULONG item);
void  OSLibDosSleep(ULONG msecs);

//functions for physical disk & partition information
DWORD OSLibDosGetNumPhysDrives();

#ifdef __cplusplus
}
#endif

#endif //__OSLIBDOS_H__

