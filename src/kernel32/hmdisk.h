/* $Id: hmdisk.h,v 1.1 2000-09-13 21:10:58 sandervl Exp $ */

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
  HMDeviceDiskClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName) {};

  /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  virtual DWORD  CloseHandle(PHMHANDLEDATA pHMHandleData);

  /* this is a handler method for calls to DeviceIoControl() */
  virtual BOOL   DeviceIoControl    (PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                     LPVOID lpInBuffer, DWORD nInBufferSize,
                                     LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                     LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
};


#endif //__HMDISK_H__
