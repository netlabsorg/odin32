/* $Id: HandleManager.h,v 1.18 2000-03-17 16:06:01 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) HandleManager.Cpp       1.0.0   1998/02/11 PH start
 */

#ifndef _HANDLEMANAGER_H_
#define _HANDLEMANAGER_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 1998/02/11 PH The interface to the handle manager is twofold:
               a) HMxxx routines to be called from the "top" from the
                  KERNEL32 stubs
               b) the HMDeviceHandler class is actually a workaround for
                  the device driver that would handle the requests on Win32.
                  To implement a new pseudo-device, one has create a new
                  HMDeviceHandler class and link it into the table in the
                  HANDLEMANAGER.CPP file.
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#ifdef _OS2WIN_H
#include <winos2def.h>
#endif

#ifndef __WINE_WINDEF_H

typedef struct tagDCB
{
    DWORD DCBlength;
    DWORD BaudRate;
    unsigned fBinary               :1;
    unsigned fParity               :1;
    unsigned fOutxCtsFlow          :1;
    unsigned fOutxDsrFlow          :1;
    unsigned fDtrControl           :2;
    unsigned fDsrSensitivity       :1;
    unsigned fTXContinueOnXoff     :1;
    unsigned fOutX                 :1;
    unsigned fInX                  :1;
    unsigned fErrorChar            :1;
    unsigned fNull                 :1;
    unsigned fRtsControl           :2;
    unsigned fAbortOnError         :1;
    unsigned fDummy2               :17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
} DCB, *LPDCB;

#endif


//LPSECURITY_ATTRIBUTES


/*****************************************************************************
 * Defines & Macros                                                          *
 *****************************************************************************/

                                     /* 1998/02/12 PH Correction of os2win.h */
#undef FILE_TYPE_UNKNOWN
#define FILE_TYPE_UNKNOWN   0x0000

#undef FILE_TYPE_DISK
#define FILE_TYPE_DISK      0x0001

#undef FILE_TYPE_CHAR
#define FILE_TYPE_CHAR      0x0002

#undef FILE_TYPE_PIPE
#define FILE_TYPE_PIPE      0x0003

#undef FILE_TYPE_REMOTE
#define FILE_TYPE_REMOTE    0x8000

#ifndef INVALID_HANDLE_ERROR
#define INVALID_HANDLE_ERROR (-1)
#endif


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

DWORD  HMInitialize(void);          /* initialize the HandleManager */

DWORD  HMTerminate(void);            /* terminate the HandleManager */


 /* handle manager version of GetStdHandle, Open32 can't really help us here */
HANDLE  HMGetStdHandle(DWORD nStdHandle);

 /* handle manager version of GetStdHandle, Open32 can't really help us here */
BOOL    HMSetStdHandle(DWORD  nStdHandle,
                               HANDLE hHandle);


/*****************************************************************************/
/* handle translation buffer management                                      */
/*                                                                           */
/* Since some Win32 applications rely (!) on 16-bit handles, we've got to do */
/* 32-bit to 16-bit and vs vsa translation here.                             */
/* Filehandle-based functions should be routed via the handlemanager instead */
/* of going to Open32 directly.                                              */
/*****************************************************************************/

DWORD  HMHandleAllocate       (PULONG phHandle16,
                               ULONG  hHandle32);

DWORD  HMHandleFree           (ULONG  hHandle16);

DWORD  HMHandleGetUserData    (ULONG  hHandle16);

DWORD  HMHandleValidate       (ULONG  hHandle16);

DWORD  HMHandleTranslateToWin (ULONG  hHandle32,
                               PULONG phHandle16);

DWORD  HMHandleTranslateToOS2 (ULONG  hHandle16,
                               PULONG hHandle32);

DWORD  HMHandleTranslateToOS2i(ULONG  hHandle16);


/*****************************************************************************
 * Forwarders                                                                *
 *****************************************************************************/

// enable C linkage to avoid parameter mangling
#ifdef __cplusplus__
extern "C" {
#endif

HANDLE HMCreateEvent                (LPSECURITY_ATTRIBUTES      lpsa,
                                     BOOL                       bManualReset,
                                     BOOL                       bInitialState,
                                     LPCSTR                     lpName);

HANDLE HMCreateMutex                (LPSECURITY_ATTRIBUTES      lpsa,
                                     BOOL                       bInitialOwner,
                                     LPCSTR                     lpName);

HANDLE HMCreateFile                 (LPCSTR                     lpFileName,
                                     DWORD                      dwDesiredAccess,
                                     DWORD                      dwShareMode,
                                     LPSECURITY_ATTRIBUTES      lpSecurityAttributes,
                                     DWORD                      dwCreationDisposition,
                                     DWORD                      dwFlagsAndAttributes,
                                     HANDLE                     hTemplateFile);

HANDLE HMOpenFile                   (LPCSTR                     lpFileName,
                                     OFSTRUCT*                  pOFStruct,
                                     UINT                       fuMode);

HANDLE HMOpenEvent                  (DWORD                      fdwAccess,
                                     BOOL                       fInherit,
                                     LPCSTR                     lpName);

HANDLE HMOpenMutex                  (DWORD                      fdwAccess,
                                     BOOL                       fInherit,
                                     LPCSTR                    lpName);

BOOL   HMCloseHandle                (HANDLE                     hObject);

BOOL   HMReadFile                   (HANDLE                     hFile,
                                     LPVOID                     lpBuffer,
                                     DWORD                      nNumberOfBytesToRead,
                                     LPDWORD                    lpNumberOfBytesRead,
                                     LPOVERLAPPED               lpOverlapped);

BOOL   HMWriteFile                  (HANDLE                     hFile,
                                     LPCVOID                    lpBuffer,
                                     DWORD                      nNumberOfBytesToWrite,
                                     LPDWORD                    lpNumberOfBytesWritten,
                                     LPOVERLAPPED               lpOverlapped);

DWORD  HMGetFileType                (HANDLE                     hFile);

DWORD  HMDeviceRequest              (HANDLE                     hFile,
                                     ULONG                      ulRequestCode,
                                     ULONG                      arg1,
                                     ULONG                      arg2,
                                     ULONG                      arg3,
                                     ULONG                      arg4);

BOOL   HMGetFileInformationByHandle (HANDLE                     hFile,
                                     BY_HANDLE_FILE_INFORMATION *pHFI);

BOOL   HMSetEndOfFile               (HANDLE                     hFile);

BOOL   HMSetFileTime                (HANDLE                     hFile,
                                     const FILETIME             *pFT1,
                                     const FILETIME             *pFT2,
                                     const FILETIME             *pFT3);

DWORD  HMGetFileSize                (HANDLE                     hFile,
                                     PDWORD                     pSize);

DWORD  HMSetFilePointer             (HANDLE                     hFile,
                                     LONG                       lDistanceToMove,
                                     PLONG                      lpDistanceToMoveHigh,
                                     DWORD                      dwMoveMethod);

BOOL   HMLockFile                   (HFILE                      hFile,
                                     DWORD                      dwFileOffsetLow,
                                     DWORD                      dwFileOffsetHigh,
                                     DWORD                      nNumberOfBytesToLockLow,
                                     DWORD                      nNumberOfBytesToLockHigh);

BOOL   HMLockFileEx                 (HANDLE                     hFile,
                                     DWORD                      dwFlags,
                                     DWORD                      dwReserved,
                                     DWORD                      nNumberOfBytesToLockLow,
                                     DWORD                      nNumberOfBytesToLockHigh,
                                     LPOVERLAPPED               lpOverlapped);

BOOL   HMUnlockFile                 (HANDLE                     hFile,
                                     DWORD                      dwFileOffsetLow,
                                     DWORD                      dwFileOffsetHigh,
                                     DWORD                      cbUnlockLow,
                                     DWORD                      cbUnlockHigh);

BOOL   HMUnlockFileEx               (HANDLE                     hFile,
                                     DWORD                      dwFlags,
                                     DWORD                      dwReserved,
                                     DWORD                      nNumberOfBytesToLockLow,
                                     DWORD                      nNumberOfBytesToLockHigh,
                                     LPOVERLAPPED               lpOverlapped);

DWORD  HMWaitForSingleObject        (HANDLE                     hObject,
                                     DWORD                      dwTimeout);

DWORD  HMWaitForSingleObjectEx      (HANDLE                     hObject,
                                     DWORD                      dwTimeout,
                                     BOOL                       fAlertable);

DWORD  HMWaitForMultipleObjects     (DWORD                      cObjects,
                                     HANDLE                    *lphObjects,
                                     BOOL                       fWaitAll,
                                     DWORD                      dwTimeout);

DWORD  HMWaitForMultipleObjectsEx   (DWORD                      cObjects,
                                     HANDLE                    *lphObjects,
                                     BOOL                       fWaitAll,
                                     DWORD                      dwTimeout,
                                     BOOL                       fAlertable);

DWORD  HMMsgWaitForMultipleObjects  (DWORD 			nCount, 
                                     HANDLE 		       *pHandles, 
                                     BOOL 			fWaitAll,
                                     DWORD 			dwMilliseconds, 
                                     DWORD 			dwWakeMask);

BOOL   HMFlushFileBuffers           (HANDLE                     hObject);

BOOL   HMGetOverlappedResult        (HANDLE                     hObject,
                                     LPOVERLAPPED               lpOverlapped,
                                     LPDWORD                    lpNumberOfBytesTransferred,
                                     BOOL                       bWait);

BOOL   HMReleaseMutex               (HANDLE                     hObject);

BOOL   HMSetEvent                   (HANDLE                     hEvent);

BOOL   HMPulseEvent                 (HANDLE                     hEvent);

BOOL   HMResetEvent                 (HANDLE                     hEvent);

HANDLE HMCreateSemaphore            (LPSECURITY_ATTRIBUTES      lpsa,
                                     LONG                       lInitialCount,
                                     LONG                       lMaximumCount,
                                     LPCSTR                     lpszSemaphoreName);

HANDLE HMOpenSemaphore              (DWORD                      fdwAccess,
                                     BOOL                       fInheritHandle,
                                     LPCSTR                     lpszSemaphoreName);

BOOL   HMReleaseSemaphore           (HANDLE                     hSemaphore,
                                     LONG                       cReleaseCount,
                                     LPLONG                     lpPreviousCount);


HANDLE HMCreateFileMapping(
                HFILE hFile,   /* [in] Handle of file to map */
                SECURITY_ATTRIBUTES *sa, /* [in] Optional security attributes*/
                DWORD protect,   /* [in] Protection for mapping object */
                DWORD size_high, /* [in] High-order 32 bits of object size */
                DWORD size_low,  /* [in] Low-order 32 bits of object size */
                LPCSTR name);    /* [in] Name of file-mapping object */

HANDLE HMOpenFileMapping(
                DWORD access,   /* [in] Access mode */
                BOOL inherit,   /* [in] Inherit flag */
                LPCSTR name );  /* [in] Name of file-mapping object */

LPVOID HMMapViewOfFileEx            (HANDLE                     hFileMappingObject,
                                     DWORD                      dwDesiredAccess,
                                     DWORD                      dwFileOffsetHigh,
                                     DWORD                      dwFileOffsetLow,
                                     DWORD                      dwNumberOfBytesToMap,
			             LPVOID                     lpBaseAddress);

BOOL HMDuplicateHandle(HANDLE  srcprocess,
                       HANDLE  srchandle,
                       HANDLE  destprocess,
                       PHANDLE desthandle,
                       DWORD   fdwAccess,
                       BOOL    fInherit,
                       DWORD   fdwOptions);

DWORD HMOpenThreadToken(HANDLE  ThreadHandle,
                        DWORD   DesiredAccess,
                        DWORD   OpenAsSelf,
                        HANDLE *TokenHandle);

DWORD HMOpenProcessToken(HANDLE  ProcessHandle,
                         DWORD   DesiredAccess,
                         DWORD   dwUserData,
                         HANDLE *TokenHandle);

BOOL HMSetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue);

BOOL HMGetCommState(INT hCommDev, LPDCB lpdcb);

HANDLE HMCreateThread(LPSECURITY_ATTRIBUTES  lpsa,
                      DWORD                  cbStack,
                      LPTHREAD_START_ROUTINE lpStartAddr,
                      LPVOID                 lpvThreadParm,
                      DWORD                  fdwCreate,
                      LPDWORD                lpIDThread,
                      BOOL                   fFirstThread = FALSE);
INT    HMGetThreadPriority(HANDLE hThread);
DWORD  HMSuspendThread(HANDLE hThread);
BOOL   HMSetThreadPriority(HANDLE hThread, int priority);
BOOL   HMGetThreadContext(HANDLE hThread, CONTEXT *lpContext);
BOOL   HMSetThreadContext(HANDLE hThread, const CONTEXT *lpContext);
BOOL   HMTerminateThread(HANDLE hThread, DWORD exitcode);
DWORD  HMResumeThread(HANDLE hThread);
BOOL   HMGetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode);
BOOL   HMSetThreadTerminated(HANDLE hThread);

#ifdef __cplusplus__
  }
#endif

#endif /* _HANDLEMANAGER_H_ */
