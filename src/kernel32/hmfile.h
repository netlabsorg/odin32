/* $Id: hmfile.h,v 1.11 2003-05-06 12:06:10 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef __HMFILE_H__
#define __HMFILE_H__


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include "HMDevice.h"
#include "HMOpen32.h"


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

class HMDeviceFileClass : public HMDeviceHandler
{
public:
  HMDeviceFileClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}

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

  virtual BOOL DuplicateHandle(HANDLE srchandle, PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions);

  virtual BOOL   SetHandleInformation(PHMHANDLEDATA pHMHandleData,
                                      DWORD  dwMask,
                                      DWORD  dwFlags);

                      /* this is a handler method for calls to CloseHandle() */
  virtual BOOL   CloseHandle(PHMHANDLEDATA pHMHandleData);

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
                                          BY_HANDLE_FILE_INFORMATION* pHFI);

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

                         /* this is a handler method for calls to LockFile() */
  virtual BOOL  LockFile(PHMHANDLEDATA pHMHandleData,
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

                   /* this is a handler method for calls to FlushFileBuffers */
  virtual BOOL FlushFileBuffers(PHMHANDLEDATA pHMHandleData);

                /* this is a handler method for calls to GetOverlappedResult */
  virtual BOOL GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                   LPOVERLAPPED  arg2,
                                   LPDWORD       arg3,
                                   BOOL          arg4);

  virtual BOOL GetFileNameFromHandle(PHMHANDLEDATA pHMHandleData, LPSTR lpszFileName, DWORD cbFileName);
};


class HMDeviceInfoFileClass : public HMDeviceHandler
{
public:
  HMDeviceInfoFileClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}

  /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  /* this is a handler method for calls to CloseHandle() */
  virtual BOOL   CloseHandle(PHMHANDLEDATA pHMHandleData);

  /* this is a handler method for calls to GetFileTime() */
  virtual BOOL  GetFileTime (PHMHANDLEDATA pHMHandleData,
                             LPFILETIME    pFT1,
                             LPFILETIME    pFT2,
                             LPFILETIME    pFT3);

  /* this is a handler method for calls to GetFileSize() */
  virtual DWORD GetFileSize(PHMHANDLEDATA pHMHandleData,
                            PDWORD        pSizeHigh);

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

  virtual BOOL GetFileNameFromHandle(PHMHANDLEDATA pHMHandleData, LPSTR lpszFileName, DWORD cbFileName);

};

#endif /* __HMFILE_H__ */

