/* $Id: hmstd.h,v 1.2 2001-12-05 14:16:05 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_STANDARD_H_
#define _HM_DEVICE_STANDARD_H_


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

class HMDeviceStandardClass : public HMDeviceOpen32Class
{
public:
  HMDeviceStandardClass(LPCSTR lpDeviceName) : HMDeviceOpen32Class(lpDeviceName) {}

  /* this is a handler method for calls to ReadFile/Ex */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to WriteFile/Ex */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);
};


#endif /* _HM_DEVICE_STANDARD_H_ */

