/* $Id: hmopen32.h,v 1.8 2001-04-26 13:22:47 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_OPEN32_H_
#define _HM_DEVICE_OPEN32_H_


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

class HMDeviceOpen32Class : public HMDeviceHandler
{
public:
  HMDeviceOpen32Class(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}


  /* this is a special internal method to handle non-standard requests       */
  /* such as GetConsoleMode() for console devices                            */
  virtual DWORD  _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                 ULONG         ulRequestCode,
                                 ULONG         arg1,
                                 ULONG         arg2,
                                 ULONG         arg3,
                                 ULONG         arg4);

  virtual BOOL DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions);

                      /* this is a handler method for calls to CloseHandle() */
  virtual BOOL   CloseHandle(PHMHANDLEDATA pHMHandleData);

                      /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

       /* this is a handler method for calls to GetFileInformationByHandle() */
  virtual DWORD GetFileInformationByHandle(PHMHANDLEDATA pHMHandleData,
                                           BY_HANDLE_FILE_INFORMATION* pHFI);

                /* this is a handler method for calls to WaitForSingleObject */
  virtual DWORD WaitForSingleObject  (PHMHANDLEDATA pHMHandleData,
                                      DWORD  dwTimeout);

              /* this is a handler method for calls to WaitForSingleObjectEx */
  virtual DWORD WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                      DWORD  dwTimeout,
                                      BOOL   fAlertable);
};


#endif /* _HM_DEVICE_OPEN32_H_ */

