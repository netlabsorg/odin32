/* $Id: hmmmap.h,v 1.2 1999-08-25 08:55:19 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_MEMMAP_H_
#define _HM_DEVICE_MEMMAP_H_


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

class HMDeviceMemMapClass : public HMDeviceKernelObjectClass
{
public:
  HMDeviceMemMapClass(LPCSTR lpDeviceName) : HMDeviceKernelObjectClass(lpDeviceName) {}

  // identify a memmap object by its base address
  static int findByBaseAddress(LPVOID lpBaseAddress);


  /* this is a handler method for calls to CreateFileMapping() */
  virtual DWORD CreateFileMapping   (PHMHANDLEDATA              pHMHandleData,
                                     HANDLE                     hFile,
                                     LPSECURITY_ATTRIBUTES      lpFileMappingAttributes,
                                     DWORD                      flProtect,
                                     DWORD                      dwMaximumSizeHigh,
                                     DWORD                      dwMaximumSizeLow,
                                     LPCSTR                     lpName);

  /* this is a handler method for calls to OpenFileMapping() */
  virtual DWORD OpenFileMapping     (PHMHANDLEDATA              pHMHandleData,
                                     DWORD access,   /* [in] Access mode */
                                     BOOL                       fInherit,
                                     LPCSTR                     lpName);

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
  virtual BOOL   UnmapViewOfFile    (PHMHANDLEDATA              pHMHandleData,
                                     LPVOID                     lpBaseAddress);

                  /* this is a handler method for calls to FlushViewOfFile() */
  virtual BOOL   FlushViewOfFile    (PHMHANDLEDATA              pHMHandleData,
                                     LPVOID                     lpBaseAddress,
                                     DWORD                      dwNumberOfBytesToFlush);


                  /* this is a handler method for calls to CloseHandle() */
  virtual DWORD  CloseHandle(PHMHANDLEDATA pHMHandleData);
};


#endif /* _HM_DEVICE_MEMMAP_H_ */

