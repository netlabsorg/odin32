/* $Id: hmdevice.h,v 1.36 2003-06-02 16:25:17 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_H_
#define _HM_DEVICE_H_


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct _HMHANDLEDATA
{
  HANDLE          hHMHandle;             /* a copy of the OS/2 system handle */
  HANDLE          hWin32Handle;          // win32 handle

  DWORD           dwAccess;                     /* access mode of the handle */
  DWORD           dwShare;                       /* share mode of the handle */
  DWORD           dwCreation;                       /* dwCreationDisposition */
  DWORD           dwFlags;                           /* flags and attributes */

  DWORD           dwUserData;
  DWORD           dwInternalType;
  DWORD           dwHandleInformation;               /* Set by SetHandleInformation */

  LPVOID          lpHandlerData;    /* for private use of the device handler */
  LPVOID          lpDeviceData;
} HMHANDLEDATA, *PHMHANDLEDATA;

/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/



class HMDeviceHandler
{
  /***************************************************************************
   * The following methods are called by the handle manager request router.  *
   * They are exact replacements for the corresponding Win32 calls.          *
   ***************************************************************************/

public:
  LPCSTR lpHMDeviceName;                   /* a reference to the device name */

  HMDeviceHandler(LPCSTR lpDeviceName);      /* constructor with device name */

  //checks if device name belongs to this class
  virtual BOOL FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength);

  /***********************************
   * handle generic standard methods *
   ***********************************/

  /* this is a special internal method to handle non-standard requests       */
  /* such as GetConsoleMode() for console devices                            */
  virtual DWORD  _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                 ULONG         ulRequestCode,
                                 ULONG         arg1,
                                 ULONG         arg2,
                                 ULONG         arg3,
                                 ULONG         arg4);

  virtual BOOL DuplicateHandle(HANDLE srchandle, PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions);

  /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  /* this is a handler method for calls to   OpenFile() */
  virtual DWORD  OpenFile   (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             OFSTRUCT*     pOFStruct,
                             UINT          fuMode);

  /* this is a handler method for calls to CloseHandle() */
  virtual BOOL   CloseHandle(PHMHANDLEDATA pHMHandleData);

  virtual BOOL   SetHandleInformation(PHMHANDLEDATA pHMHandleData,
                                      DWORD  dwMask,
                                      DWORD  dwFlags);

  /* this is a handler method for calls to ReadFile/Ex */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to WriteFile/Ex */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);


       /* this is a handler method for calls to GetFileInformationByHandle() */
  virtual BOOL GetFileInformationByHandle(PHMHANDLEDATA pHMHandleData,
                                          BY_HANDLE_FILE_INFORMATION *pHFI);

                     /* this is a handler method for calls to SetEndOfFile() */
  virtual BOOL  SetEndOfFile(PHMHANDLEDATA pHMHandleData);

                      /* this is a handler method for calls to SetFileTime() */
  virtual BOOL  SetFileTime (PHMHANDLEDATA pHMHandleData,
                             LPFILETIME    pFT1,
                             LPFILETIME    pFT2,
                             LPFILETIME    pFT3);

                      /* this is a handler method for calls to GetFileTime() */
  virtual BOOL  GetFileTime (PHMHANDLEDATA pHMHandleData,
                             LPFILETIME    pFT1,
                             LPFILETIME    pFT2,
                             LPFILETIME    pFT3);

                      /* this is a handler method for calls to GetFileSize() */
  virtual DWORD GetFileSize(PHMHANDLEDATA pHMHandleData,
                            PDWORD        pSizeHigh);

                   /* this is a handler method for calls to SetFilePointer() */
  virtual DWORD SetFilePointer(PHMHANDLEDATA pHMHandleData,
                               LONG          lDistanceToMove,
                               PLONG         lpDistanceToMoveHigh,
                               DWORD         dwMoveMethod);

                 /* this is a handler method for calls to FlushFileBuffers() */
  virtual BOOL FlushFileBuffers(PHMHANDLEDATA pHMHandleData);

              /* this is a handler method for calls to GetOverlappedResult() */
  virtual BOOL GetOverlappedResult (PHMHANDLEDATA pHMHandleData,
                                    LPOVERLAPPED  lpOverlapped,
                                    LPDWORD       arg3,
                                    BOOL          arg4);

                         /* this is a handler method for calls to LockFile() */
  virtual BOOL LockFile(PHMHANDLEDATA pHMHandleData,
                         DWORD         arg2,
                         DWORD         arg3,
                         DWORD         arg4,
                         DWORD         arg5);

                       /* this is a handler method for calls to LockFileEx() */
  virtual BOOL LockFileEx(PHMHANDLEDATA pHMHandleData,
                           DWORD         dwFlags,
                           DWORD         dwReserved,
                           DWORD         nNumberOfBytesToLockLow,
                           DWORD         nNumberOfBytesToLockHigh,
                           LPOVERLAPPED  lpOverlapped);

                       /* this is a handler method for calls to UnlockFile() */
  virtual BOOL UnlockFile(PHMHANDLEDATA pHMHandleData,
                           DWORD         arg2,
                           DWORD         arg3,
                           DWORD         arg4,
                           DWORD         arg5);

                     /* this is a handler method for calls to UnlockFileEx() */
  virtual BOOL UnlockFileEx(PHMHANDLEDATA pHMHandleData,
                             DWORD         dwReserved,
                             DWORD         nNumberOfBytesToLockLow,
                             DWORD         nNumberOfBytesToLockHigh,
                             LPOVERLAPPED  lpOverlapped);

  /* this is a handler method for calls to WaitForSingleObject */
  virtual DWORD WaitForSingleObject  (PHMHANDLEDATA pHMHandleData,
                                      DWORD  dwTimeout);

  /* this is a handler method for calls to WaitForSingleObjectEx */
  virtual DWORD WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                      DWORD  dwTimeout,
                                      BOOL   fAlertable);

  virtual DWORD MsgWaitForMultipleObjects(PHMHANDLEDATA pHMHandleData,
                                          DWORD      nCount,
                                          PHANDLE       pHandles,
                                          BOOL       fWaitAll,
                                          DWORD      dwMilliseconds,
                                          DWORD      dwWakeMask);

  virtual DWORD WaitForMultipleObjects (PHMHANDLEDATA pHMHandleData,
                                        DWORD   cObjects,
                                        PHANDLE lphObjects,
                                        BOOL    fWaitAll,
                                        DWORD   dwTimeout);

  /***************************************************************************
   * Events                                                                  *
   ***************************************************************************/

  /* this is a handler method for calls to CreateEvent() */
  virtual DWORD CreateEvent (PHMHANDLEDATA         pHMHandleData,
                             LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fManualReset,
                             BOOL                  fInitialState,
                             LPCSTR                lpszEventName);

  /* this is a handler method for calls to OpenEvent() */
  virtual DWORD OpenEvent   (PHMHANDLEDATA         pHMHandleData,
                             BOOL                  fInheritHandle,
                             LPCSTR                lpszEventName);

  /* this is a handle method for calls to ResetEvent() */
  virtual BOOL  ResetEvent  (PHMHANDLEDATA         pHMHandleData);

  /* this is a handle method for calls to SetEvent() */
  virtual BOOL  SetEvent    (PHMHANDLEDATA         pHMHandleData);

  /* this is a handle method for calls to PulseEvent() */
  virtual BOOL  PulseEvent  (PHMHANDLEDATA         pHMHandleData);


  /***************************************************************************
   * Mutex                                                                   *
   ***************************************************************************/

  /* this is a handler method for calls to CreateMutex() */
  virtual DWORD CreateMutex (PHMHANDLEDATA         pHMHandleData,
                             LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fInitialOwner,
                             LPCSTR                lpszMutexName);

  /* this is a handler method for calls to OpenMutex() */
  virtual DWORD OpenMutex   (PHMHANDLEDATA         pHMHandleData,
                             BOOL                  fInheritHandle,
                             LPCSTR                lpszMutexName);

  /* this is a handle method for calls to ReleaseMutex() */
  virtual BOOL  ReleaseMutex(PHMHANDLEDATA         pHMHandleData);


  /***************************************************************************
   * Semaphores                                                              *
   ***************************************************************************/

  /* this is a handler method for calls to CreateSemaphore() */
  virtual DWORD CreateSemaphore (PHMHANDLEDATA         pHMHandleData,
                                 LPSECURITY_ATTRIBUTES lpsa,
                                 LONG                  lInitialCount,
                                 LONG                  lMaximumCount,
                                 LPCSTR                lpszSemaphoreName);

  /* this is a handler method for calls to OpenSemaphore() */
  virtual DWORD OpenSemaphore   (PHMHANDLEDATA         pHMHandleData,
                                 BOOL                  fInheritHandle,
                                 LPCSTR                lpszSemaphoreName);

  /* this is a handle method for calls to ReleaseSemaphore() */
  virtual BOOL  ReleaseSemaphore(PHMHANDLEDATA pHMHandleData,
                                 LONG          cReleaseCount,
                                 LPLONG        lpPreviousCount);

  /* this is a handler method for calls to CreateFileMapping() */
  virtual DWORD CreateFileMapping   (PHMHANDLEDATA              pHMHandleData,
                                     HANDLE                     hFile,
                                     LPSECURITY_ATTRIBUTES      lpFileMappingAttributes,
                                     DWORD                      flProtect,
                                     DWORD                      dwMaximumSizeHigh,
                                     DWORD                      dwMaximumSizeLow,
                                     LPCSTR                     lpName);

  /* this is a handler method for calls to OpenFileMapping() */
  virtual DWORD OpenFileMapping     (PHMHANDLEDATA              pHMHandleData,
                         DWORD access,   /* [in] Access mode */
                                     BOOL                       fInherit,
                                     LPCSTR                     lpName);

  /* this is a handler method for calls to MapViewOfFileEx() */
  virtual LPVOID MapViewOfFileEx    (PHMHANDLEDATA              pHMHandleData,
                                     DWORD                      dwDesiredAccess,
                                     DWORD                      dwFileOffsetHigh,
                                     DWORD                      dwFileOffsetLow,
                                     DWORD                      dwNumberOfBytesToMap,
                                     LPVOID                     lpBaseAddress);

  /* this is a handler method for calls to DeviceIoControl() */
  virtual BOOL   DeviceIoControl    (PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                     LPVOID lpInBuffer, DWORD nInBufferSize,
                                     LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                     LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

  virtual BOOL   CancelIo           (PHMHANDLEDATA pHMHandleData);

 /* COM ports */
 virtual BOOL SetupComm( PHMHANDLEDATA pHMHandleData,
                         DWORD dwInQueue,
                         DWORD dwOutQueue);

 virtual BOOL GetCommState( PHMHANDLEDATA pHMHandleData,
                            LPDCB lpdcb);
 virtual BOOL WaitCommEvent( PHMHANDLEDATA pHMHandleData,
                             LPDWORD lpfdwEvtMask,
                             LPOVERLAPPED lpo);

 virtual BOOL GetCommProperties( PHMHANDLEDATA pHMHandleData,
                                 LPCOMMPROP lpcmmp);
 virtual BOOL GetCommMask( PHMHANDLEDATA pHMHandleData,
                           LPDWORD lpfdwEvtMask);
 virtual BOOL SetCommMask( PHMHANDLEDATA pHMHandleData,
                           DWORD fdwEvtMask);
 virtual BOOL PurgeComm( PHMHANDLEDATA pHMHandleData,
                         DWORD fdwAction);
 virtual BOOL ClearCommError( PHMHANDLEDATA pHMHandleData,
                              LPDWORD lpdwErrors,
                              LPCOMSTAT lpcst);
 virtual BOOL SetCommState( PHMHANDLEDATA pHMHandleData,
                            LPDCB lpdcb) ;
 virtual BOOL GetCommModemStatus( PHMHANDLEDATA pHMHandleData,
                               LPDWORD lpModemStat );
 virtual BOOL GetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                               LPCOMMTIMEOUTS lpctmo);
 virtual BOOL SetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                               LPCOMMTIMEOUTS lpctmo);
 virtual BOOL TransmitCommChar( PHMHANDLEDATA pHMHandleData,
                                CHAR cChar );
 virtual BOOL SetCommConfig( PHMHANDLEDATA pHMHandleData,
                             LPCOMMCONFIG lpCC,
                             DWORD dwSize );
 virtual BOOL SetCommBreak( PHMHANDLEDATA pHMHandleData );
 virtual BOOL GetCommConfig( PHMHANDLEDATA pHMHandleData,
                             LPCOMMCONFIG lpCC,
                             LPDWORD lpdwSize );
 virtual BOOL EscapeCommFunction( PHMHANDLEDATA pHMHandleData,
                                  UINT dwFunc );
 virtual BOOL ClearCommBreak( PHMHANDLEDATA pHMHandleData);
 virtual BOOL SetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                    LPCOMMCONFIG lpCC,
                                    DWORD dwSize);
 virtual BOOL GetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                    LPCOMMCONFIG lpCC,
                                    LPDWORD lpdwSize);

 virtual DWORD  OpenThreadToken(PHMHANDLEDATA pHMHandleData,
                                 HANDLE  ThreadHandle,
                                 BOOL    OpenAsSelf);

 virtual DWORD  OpenProcessToken(PHMHANDLEDATA pHMHandleData, DWORD dwUserData,
                                  HANDLE  ProcessHandle);

 virtual HANDLE CreateThread(PHMHANDLEDATA          pHMHandleData,
                             LPSECURITY_ATTRIBUTES  lpsa,
                             DWORD                  cbStack,
                             LPTHREAD_START_ROUTINE lpStartAddr,
                             LPVOID                 lpvThreadParm,
                             DWORD                  fdwCreate,
                             LPDWORD                lpIDThread,
                             BOOL                   fRegisterThread);

 virtual INT    GetThreadPriority(HANDLE hThread, PHMHANDLEDATA pHMHandleData);
 virtual DWORD  SuspendThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData);
 virtual BOOL   SetThreadPriority(HANDLE hThread, PHMHANDLEDATA pHMHandleData, int priority);

 virtual BOOL   GetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, PCONTEXT lpContext);
 virtual BOOL   SetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, const CONTEXT *lpContext);
 virtual BOOL   GetThreadTimes(HANDLE hThread, PHMHANDLEDATA pHMHandleData,
                               FILETIME *lpCreationTime, FILETIME *lpExitTime,
                               FILETIME *lpKernelTime, FILETIME *lpUserTime);

 virtual BOOL   TerminateThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, DWORD exitcode);
 virtual DWORD  ResumeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData);
 virtual BOOL   SetThreadTerminated(HANDLE hThread, PHMHANDLEDATA pHMHandleData);

 virtual BOOL   GetExitCodeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, LPDWORD lpExitCode);

 /* Named pipes */
  virtual BOOL  PeekNamedPipe(PHMHANDLEDATA pHMHandleData,
                              LPVOID lpvBuffer,
                              DWORD   cbBuffer,
                              LPDWORD lpcbRead,
                              LPDWORD lpcbAvail,
                              LPDWORD lpcbMessage);

  virtual DWORD CreateNamedPipe(PHMHANDLEDATA pHMHandleData, LPCTSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                                DWORD  nMaxInstances, DWORD  nOutBufferSize,
                                DWORD  nInBufferSize, DWORD  nDefaultTimeOut,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes);

  virtual BOOL ConnectNamedPipe(PHMHANDLEDATA pHMHandleData, LPOVERLAPPED lpOverlapped);

  virtual BOOL DisconnectNamedPipe(PHMHANDLEDATA pHMHandleData);

  virtual BOOL GetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                        LPDWORD lpState,
                                        LPDWORD lpCurInstances,
                                        LPDWORD lpMaxCollectionCount,
                                        LPDWORD lpCollectDataTimeout,
                                        LPTSTR  lpUserName,
                                        DWORD   nMaxUserNameSize);

  virtual BOOL GetNamedPipeInfo(PHMHANDLEDATA pHMHandleData,
                                LPDWORD lpFlags,
                                LPDWORD lpOutBufferSize,
                                LPDWORD lpInBufferSize,
                                LPDWORD lpMaxInstances);


  virtual BOOL TransactNamedPipe(PHMHANDLEDATA pHMHandleData,
                                  LPVOID lpvWriteBuf,
                                  DWORD cbWriteBuf,
                                  LPVOID lpvReadBuf,
                                  DWORD cbReadBuf,
                                  LPDWORD lpcbRead,
                                  LPOVERLAPPED lpo);

  virtual BOOL SetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                      LPDWORD lpdwMode,
                                      LPDWORD lpcbMaxCollect,
                                      LPDWORD lpdwCollectDataTimeout);

  virtual BOOL GetMailslotInfo(PHMHANDLEDATA pHMHandleData,
                               LPDWORD lpMaxMessageSize,
                               LPDWORD lpNextSize,
                               LPDWORD lpMessageCount,
                               LPDWORD lpReadTimeout);

  virtual BOOL SetMailslotInfo(PHMHANDLEDATA pHMHandleData,
                               DWORD  dwReadTimeout);

  virtual BOOL GetFileNameFromHandle(PHMHANDLEDATA pHMHandleData, LPSTR lpszFileName, DWORD cbFileName);

};


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

                            /* register a new device with the handle manager */
DWORD  HMDeviceRegister(LPCSTR           pszDeviceName,
                        HMDeviceHandler *pDeviceHandler);

DWORD  HMDeviceRegisterEx(LPCSTR           pszDeviceName,
                          HMDeviceHandler *pDeviceHandler,
                          VOID            *pDevData);

#ifdef __cplusplus
} // extern "C"
#endif

#include "hmhandle.h"

#endif /* _HM_DEVICE_H_ */

