/* $Id: hmdisk.h,v 1.3 2001-04-26 13:22:45 sandervl Exp $ */

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

  /* this is a handler method for calls to DeviceIoControl() */
  virtual BOOL   DeviceIoControl    (PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                     LPVOID lpInBuffer, DWORD nInBufferSize,
                                     LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                     LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
};


#endif //__HMDISK_H__
