/* $Id: hmsemaphore.h,v 1.5 2001-06-23 16:59:28 sandervl Exp $ */

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

#ifdef USE_OS2SEMAPHORES
typedef struct {
    LONG   currentCount;
    LONG   maximumCount;
    LONG   refCount;
    ULONG  hev;
} SEM_INFO, *PSEM_INFO;
#endif


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
#ifdef USE_OS2SEMAPHORES
  HMDeviceSemaphoreClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}
#else
  HMDeviceSemaphoreClass(LPCSTR lpDeviceName) : HMDeviceOpen32Class(lpDeviceName) {}
#endif

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

#endif

  /* this is a handle method for calls to ReleaseSemaphore() */
  virtual BOOL  ReleaseSemaphore(PHMHANDLEDATA pHMHandleData,
                                 LONG          cReleaseCount,
                                 LPLONG        lpPreviousCount);
};


DWORD HMSemWaitForMultipleObjects (DWORD   cObjects,
                                   PHANDLE lphObjects,
                                   BOOL    fWaitAll,
                                   DWORD   dwTimeout);

DWORD HMSemMsgWaitForMultipleObjects (DWORD   cObjects,
                                      PHANDLE lphObjects,
                                      BOOL    fWaitAll,
                                      DWORD   dwTimeout, 
                                      DWORD   dwWakeMask);

void FixSemName(char *lpszSemaphoreName);

#endif /* _HM_DEVICE_SEMAPHORE_H_ */

