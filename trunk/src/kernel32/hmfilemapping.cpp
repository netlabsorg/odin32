/* $Id: hmfilemapping.cpp,v 1.1 1999-08-24 14:36:05 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#undef DEBUG_LOCAL
//#define DEBUG_LOCAL


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "misc.h"

#include "HandleManager.H"
#include "HMFileMapping.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/


/*****************************************************************************
 * Name      : DWORD HMDeviceFileMappingClass::findByBaseAddress
 * Purpose   : file memory mapped object by base address
 * Parameters: LPVOID        lpBaseAddress
 * Variables :
 * Result    : index in Handlemanager handle table
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

int HMDeviceFileMappingClass::findByBaseAddress(LPVOID lpBaseAddress)
{
  dprintf(("KERNEL32: HandleManager::FileMapping::findByBaseAddress(%08xh)\n",
           lpBaseAddress));

  return(-1); // not found
}



/*****************************************************************************
 * Name      : DWORD HMDeviceFileMappingClass::CreateFileMapping
 * Purpose   : create memory mapped file
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LPSECURITY_ATTRIBUTES      lpFileMappingAttributes
 *             DWORD                      flProtect
 *             DWORD                      dwMaximumSizeHigh
 *             DWORD                      dwMaximumSizeLow
 *             LPCTSTR                    lpName
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceFileMappingClass::CreateFileMapping(PHMHANDLEDATA              pHMHandleData,
                                                  HANDLE                     hFile,
                                                  LPSECURITY_ATTRIBUTES      lpFileMappingAttributes,
                                                  DWORD                      flProtect,
                                                  DWORD                      dwMaximumSizeHigh,
                                                  DWORD                      dwMaximumSizeLow,
                                                  LPCTSTR                    lpName)
{
  dprintf(("KERNEL32: HandleManager::FileMapping::CreateFileMapping(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData->hHMHandle,
           hFile,
           lpFileMappingAttributes,
           flProtect,
           dwMaximumSizeHigh,
           dwMaximumSizeLow,
           lpName));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileMappingClass::OpenFileMapping
 * Purpose   : open memory mapped file
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LPOVERLAPPED  arg2
 *             LPDWORD       arg3
 *             BOOL          arg4
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceFileMappingClass::OpenFileMapping(PHMHANDLEDATA pHMHandleData,
                                                BOOL          fInherit,
                                                LPCTSTR       lpName)
{
  dprintf(("KERNEL32: HandleManager::FileMapping::OpenFileMapping(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           fInherit,
           lpName));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileMappingClass::MapViewOfFile
 * Purpose   : map memory mapped file
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD         dwDesiredAccess,
 *             DWORD         dwFileOffsetHigh,
 *             DWORD         dwFileOffsetLow,
 *             DWORD         dwNumberOfBytesToMap
 * Variables :
 * Result    : address to memory mapped region
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

LPVOID HMDeviceFileMappingClass::MapViewOfFile(PHMHANDLEDATA pHMHandleData,
                                               DWORD         dwDesiredAccess,
                                               DWORD         dwFileOffsetHigh,
                                               DWORD         dwFileOffsetLow,
                                               DWORD         dwNumberOfBytesToMap)
{
  dprintf(("KERNEL32: HandleManager::FileMapping::MapViewOfFile(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwDesiredAccess,
           dwFileOffsetHigh,
           dwFileOffsetLow,
           dwNumberOfBytesToMap));

  return(NULL);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileMappingClass::MapViewOfFileEx
 * Purpose   : map memory mapped file
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD         dwDesiredAccess,
 *             DWORD         dwFileOffsetHigh,
 *             DWORD         dwFileOffsetLow,
 *             DWORD         dwNumberOfBytesToMap
 *             LPVOID        lpBaseAddress
 * Variables :
 * Result    : address to memory mapped region
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

LPVOID HMDeviceFileMappingClass::MapViewOfFileEx(PHMHANDLEDATA pHMHandleData,
                                                 DWORD         dwDesiredAccess,
                                                 DWORD         dwFileOffsetHigh,
                                                 DWORD         dwFileOffsetLow,
                                                 DWORD         dwNumberOfBytesToMap,
                                                 LPVOID        lpBaseAddress)
{
  dprintf(("KERNEL32: HandleManager::FileMapping::MapViewOfFileEx(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwDesiredAccess,
           dwFileOffsetHigh,
           dwFileOffsetLow,
           dwNumberOfBytesToMap,
           lpBaseAddress));

  return(NULL);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileMappingClass::UnmapViewOfFile
 * Purpose   : unmap memory mapped file
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LPVOID        lpBaseAddress
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceFileMappingClass::UnmapViewOfFile(PHMHANDLEDATA pHMHandleData,
                                               LPVOID        lpBaseAddress)
{
  dprintf(("KERNEL32: HandleManager::FileMapping::UnmapViewOfFile(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpBaseAddress));

  return(FALSE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceFileMappingClass::FlushViewOfFile
 * Purpose   : flush memory mapped file
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LPVOID        lpBaseAddress
 *             DWORD         dwNumberOfBytesToFlush
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceFileMappingClass::FlushViewOfFile(PHMHANDLEDATA pHMHandleData,
                                               LPVOID        lpBaseAddress,
                                               DWORD         dwNumberOfBytesToFlush)
{
  dprintf(("KERNEL32: HandleManager::FileMapping::FlushViewOfFile(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpBaseAddress,
           dwNumberOfBytesToFlush));

  return(FALSE);
}


