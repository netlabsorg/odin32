/* $Id: hmsemaphore.h,v 1.2 1999-08-31 23:14:03 phaller Exp $ */

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

class HMDeviceSemaphoreClass : public HMDeviceOpen32Class
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

                     /* this is a handle method for calls to ReleaseSemaphore() */
  virtual BOOL  ReleaseSemaphore(PHMHANDLEDATA pHMHandleData,
                                 LONG          cReleaseCount,
                                 LPLONG        lpPreviousCount);
};


#endif /* _HM_DEVICE_SEMAPHORE_H_ */

