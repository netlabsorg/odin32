/* $Id: hmevent.h,v 1.1 1999-07-06 15:48:47 phaller Exp $ */

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
#include "HMObjects.h"


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

class HMDeviceEventClass : public HMDeviceKernelObjectClass
{
public:
  HMDeviceEventClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName) {}

                     /* this is a handler method for calls to CreateEvent() */
  virtual DWORD CreateEvent (PHMHANDLEDATA         pHMHandleData,
                             LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fManualReset,
                             BOOL                  fInitialState,
                             LPCTSTR               lpszEventName);

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

