/* $Id: hmmutex.h,v 1.1 1999-07-06 15:48:47 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_MUTEX_H_
#define _HM_DEVICE_MUTEX_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include "HMDevice.h"
#include "HMObjects.h"


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

class HMDeviceMutexClass : public HMDeviceKernelObjectClass
{
public:
  HMDeviceMutexClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName) {}

                     /* this is a handler method for calls to CreateMutex() */
  virtual DWORD CreateMutex (PHMHANDLEDATA         pHMHandleData,
                             LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fInitialOwner,
                             LPCTSTR               lpszMutexName);

                       /* this is a handler method for calls to OpenMutex() */
  virtual DWORD OpenMutex   (PHMHANDLEDATA         pHMHandleData,
                             BOOL                  fInheritHandle,
                             LPCTSTR               lpszMutexName);

                     /* this is a handle method for calls to ReleaseMutex() */
  virtual BOOL  ReleaseMutex(PHMHANDLEDATA         pHMHandleData);
};


#endif /* _HM_DEVICE_MUTEX_H_ */

