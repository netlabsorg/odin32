/* $Id: hmsemaphore.h,v 1.3 2001-06-19 10:50:25 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_SEMAPHORE_H_
#define _HM_DEVICE_SEMAPHORE_H_


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

#ifdef USE_OS2SEMAPHORES
class HMDeviceSemaphoreClass : public HMDeviceHandler
#else
class HMDeviceSemaphoreClass : public HMDeviceOpen32Class
#endif
{
public:
  HMDeviceSemaphoreClass(LPCSTR lpDeviceName) : HMDeviceOpen32Class(lpDeviceName) {}

  /* this is a handler method for calls to CreateSemaphore() */
  virtual DWORD CreateSemaphore (PHMHANDLEDATA         pHMHandleData,
                                 LPSECURITY_ATTRIBUTES lpsa,
                                 LONG                  lInitialCount,
                                 LONG                  lMaximumCount,
                                 LPCTSTR               lpszSemaphoreName);

  /* this is a handler method for calls to OpenSemaphore() */
  virtual DWORD OpenSemaphore   (PHMHANDLEDATA         pHMHandleData,
                                 BOOL                  fInheritHandle,
                                 LPCTSTR               lpszSemaphoreName);

#ifdef USE_OS2SEMAPHORES
  virtual BOOL  CloseHandle(PHMHANDLEDATA pHMHandleData);

  virtual BOOL DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions);
#endif

  /* this is a handle method for calls to ReleaseSemaphore() */
  virtual BOOL  ReleaseSemaphore(PHMHANDLEDATA pHMHandleData,
                                 LONG          cReleaseCount,
                                 LPLONG        lpPreviousCount);
};


#endif /* _HM_DEVICE_SEMAPHORE_H_ */

