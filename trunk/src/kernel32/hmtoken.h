/* $Id: hmtoken.h,v 1.2 2000-01-05 19:39:56 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_TOKEN_H_
#define _HM_DEVICE_TOKEN_H_


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

class HMDeviceTokenClass : public HMDeviceKernelObjectClass
{
public:
  HMDeviceTokenClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName) {}

  virtual DWORD  OpenThreadToken(PHMHANDLEDATA pHMHandleData, 
                                 HANDLE  ThreadHandle,
                                 BOOL OpenAsSelf);

  virtual DWORD  OpenProcessToken(PHMHANDLEDATA pHMHandleData, 
  	 	 	          DWORD   dwUserData,
                                  HANDLE  ProcessHandle);

  virtual DWORD  CloseHandle(PHMHANDLEDATA pHMHandleData);
};


#endif /* _HM_DEVICE_MEMMAP_H_ */

