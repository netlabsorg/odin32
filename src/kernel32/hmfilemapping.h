/* $Id: hmfilemapping.h,v 1.1 1999-08-24 14:36:05 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_FILEMAPPING_H_
#define _HM_DEVICE_FILEMAPPING_H_


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

class HMDeviceFileMappingClass : public HMDeviceKernelObjectClass
{
public:
  HMDeviceFileMappingClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName) {}

                              /* identify mapping object by the base address */
  static  int   findByBaseAddress   (LPVOID                     lpBaseAddress);

                /* this is a handler method for calls to CreateFileMapping() */
  virtual DWORD CreateFileMapping   (PHMHANDLEDATA              pHMHandleData,
                                     HANDLE                     hFile,
                                     LPSECURITY_ATTRIBUTES      lpFileMappingAttributes,
                                     DWORD                      flProtect,
                                     DWORD                      dwMaximumSizeHigh,
                                     DWORD                      dwMaximumSizeLow,
                                     LPCTSTR                    lpName);

                  /* this is a handler method for calls to OpenFileMapping() */
  virtual DWORD OpenFileMapping     (PHMHANDLEDATA              pHMHandleData,
                                     BOOL                       fInherit,
                                     LPCTSTR                    lpName);

                    /* this is a handler method for calls to MapViewOfFile() */
  virtual LPVOID MapViewOfFile      (PHMHANDLEDATA              pHMHandleData,
                                     DWORD                      dwDesiredAccess,
                                     DWORD                      dwFileOffsetHigh,
                                     DWORD                      dwFileOffsetLow,
                                     DWORD                      dwNumberOfBytesToMap);

                  /* this is a handler method for calls to MapViewOfFileEx() */
  virtual LPVOID MapViewOfFileEx    (PHMHANDLEDATA              pHMHandleData,
                                     DWORD                      dwDesiredAccess,
                                     DWORD                      dwFileOffsetHigh,
                                     DWORD                      dwFileOffsetLow,
                                     DWORD                      dwNumberOfBytesToMap,
                                     LPVOID                     lpBaseAddress);

                  /* this is a handler method for calls to UnmapViewOfFile() */
  virtual BOOL UnmapViewOfFile      (PHMHANDLEDATA              pHMHandleData,
                                     LPVOID                     lpBaseAddress);

                  /* this is a handler method for calls to FlushViewOfFile() */
  virtual BOOL FlushViewOfFile      (PHMHANDLEDATA              pHMHandleData,
                                     LPVOID                     lpBaseAddress,
                                     DWORD                      dwNumberOfBytesToFlush);

};


#endif /* _HM_DEVICE_FILEMAPPING_H_ */

