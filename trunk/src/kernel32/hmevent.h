/* $Id: hmevent.h,v 1.4 2001-06-21 21:07:53 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_EVENT_H_
#define _HM_DEVICE_EVENT_H_


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
class HMDeviceEventClass : public HMDeviceHandler
#else
class HMDeviceEventClass : public HMDeviceOpen32Class
#endif
{
public:
#ifdef USE_OS2SEMAPHORES
  HMDeviceEventClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}
#else
  HMDeviceEventClass(LPCSTR lpDeviceName) : HMDeviceOpen32Class(lpDeviceName) {}
#endif

  /* this is a handler method for calls to CreateEvent() */
  virtual DWORD CreateEvent (PHMHANDLEDATA         pHMHandleData,
                             LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fManualReset,
                             BOOL                  fInitialState,
                             LPCTSTR               lpszEventName);

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
  
  /* this is a handler method for calls to OpenEvent() */
  virtual DWORD OpenEvent   (PHMHANDLEDATA         pHMHandleData,
                             BOOL                  fInheritHandle,
                             LPCTSTR               lpszEventName);

  /* this is a handle method for calls to ResetEvent() */
  virtual BOOL  ResetEvent  (PHMHANDLEDATA         pHMHandleData);

  /* this is a handle method for calls to SetEvent() */
  virtual BOOL  SetEvent    (PHMHANDLEDATA         pHMHandleData);

  /* this is a handle method for calls to PulseEvent() */
  virtual BOOL  PulseEvent  (PHMHANDLEDATA         pHMHandleData);
};


#endif /* _HM_DEVICE_EVENT_H_ */

