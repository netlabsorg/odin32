/* $Id: hmmutex.h,v 1.3 2001-06-19 10:50:25 sandervl Exp $ */

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
#include "HMOpen32.h"


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

#ifdef USE_OS2SEMAPHORES
class HMDeviceMutexClass : public HMDeviceHandler
#else
class HMDeviceMutexClass : public HMDeviceOpen32Class
#endif
{
public:
  HMDeviceMutexClass(LPCSTR lpDeviceName) : HMDeviceOpen32Class(lpDeviceName) {}

  /* this is a handler method for calls to CreateMutex() */
  virtual DWORD CreateMutex (PHMHANDLEDATA         pHMHandleData,
                             LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fInitialOwner,
                             LPCTSTR               lpszMutexName);

  /* this is a handler method for calls to OpenMutex() */
  virtual DWORD OpenMutex   (PHMHANDLEDATA         pHMHandleData,
                             BOOL                  fInheritHandle,
                             LPCTSTR               lpszMutexName);

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
                               
  /* this is a handle method for calls to ReleaseMutex() */
  virtual BOOL  ReleaseMutex(PHMHANDLEDATA         pHMHandleData);
};


#endif /* _HM_DEVICE_MUTEX_H_ */

