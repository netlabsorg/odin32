/* $Id: hmobjects.h,v 1.2 2001-04-26 13:22:46 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_KERNELOBJECT_H_
#define _HM_DEVICE_KERNELOBJECT_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include "HMDevice.h"


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

class HMDeviceKernelObjectClass : public HMDeviceHandler
{
public:
  HMDeviceKernelObjectClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}

                      /* this is a handler method for calls to CloseHandle() */
  virtual BOOL  CloseHandle(PHMHANDLEDATA pHMHandleData);
};


#endif /* _HM_DEVICE_KERNELOBJECT_H_ */

