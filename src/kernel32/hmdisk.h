/* $Id: hmdisk.h,v 1.6 2001-11-22 16:03:23 sandervl Exp $ */

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

  /* this is a handler method for calls to ReadFile() */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped);

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

                        /* this is a handler method for calls to ReadFileEx() */
  virtual BOOL  ReadFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToRead,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

                        /* this is a handler method for calls to WriteFile() */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped);

                        /* this is a handler method for calls to WriteFileEx() */
  virtual BOOL  WriteFileEx(PHMHANDLEDATA pHMHandleData,
                            LPVOID       lpBuffer,
                            DWORD        nNumberOfBytesToWrite,
                            LPOVERLAPPED lpOverlapped,
                            LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

                      /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

private:
          DWORD  OpenDisk(PVOID pDrvInfo);
};


#endif //__HMDISK_H__
