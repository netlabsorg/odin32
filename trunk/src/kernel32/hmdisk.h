/* $Id: hmdisk.h,v 1.10 2002-05-10 14:55:12 sandervl Exp $ */

#ifndef __HMDISK_H__
#define __HMDISK_H__

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <handlemanager.h>
#include "HMDevice.h"
#include "HMObjects.h"

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

class HMDeviceDiskClass : public HMDeviceKernelObjectClass
{
public:
  HMDeviceDiskClass(LPCSTR lpDeviceName);

  //checks if device name belongs to this class
  virtual BOOL FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength);

  /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

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

  /* this is a handler method for calls to SetFilePointer() */
  virtual DWORD SetFilePointer(PHMHANDLEDATA pHMHandleData,
                               LONG          lDistanceToMove,
                               PLONG         lpDistanceToMoveHigh,
                               DWORD         dwMoveMethod);

  /* this is a handler method for calls to DeviceIoControl() */
  virtual BOOL   DeviceIoControl    (PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                     LPVOID lpInBuffer, DWORD nInBufferSize,
                                     LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                     LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

  /* this is a handler method for calls to GetFileSize() */
  virtual DWORD GetFileSize(PHMHANDLEDATA pHMHandleData,
                            PDWORD        pSizeHigh);

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

private:
          DWORD  OpenDisk(PVOID pDrvInfo);
};


#endif //__HMDISK_H__
