/* $Id: hmstd.cpp,v 1.1 2001-01-23 18:31:26 sandervl Exp $ */

/*
 * Handle Manager class for standard in, out & error handles
 *
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 */


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
#include "hmstd.h"

#define DBG_LOCALLOG	DBG_hmstd
#include "dbglocal.h"

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
 * Name      : BOOL HMDeviceStandardClass::ReadFile
 * Purpose   : read data from handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToRead,
 *             LPDWORD       lpNumberOfBytesRead,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceStandardClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                 LPCVOID       lpBuffer,
                                 DWORD         nNumberOfBytesToRead,
                                 LPDWORD       lpNumberOfBytesRead,
                                 LPOVERLAPPED  lpOverlapped)
{
  BOOL         bRC;

  dprintf2(("KERNEL32: HMDeviceStandardClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x) - NOT IMPLEMENTED\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));


  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL ReadFileEx
 * Purpose   : The ReadFileEx function reads data from a file asynchronously.
 *             It is designed solely for asynchronous operation, unlike the
 *             ReadFile function, which is designed for both synchronous and
 *             asynchronous operation. ReadFileEx lets an application perform
 *             other processing during a file read operation.
 *             The ReadFileEx function reports its completion status asynchronously,
 *             calling a specified completion routine when reading is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to read
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to read
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/
BOOL HMDeviceStandardClass::ReadFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToRead,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: ReadFileEx(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpOverlapped,
           lpCompletionRoutine));

  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL HMDeviceStandardClass::WriteFile
 * Purpose   : write data to handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToWrite,
 *             LPDWORD       lpNumberOfBytesWritten,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceStandardClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                      LPCVOID       lpBuffer,
                                      DWORD         nNumberOfBytesToWrite,
                                      LPDWORD       lpNumberOfBytesWritten,
                                      LPOVERLAPPED  lpOverlapped)
{
 DWORD byteswritten;

  dprintf2(("KERNEL32: HMDeviceStandardClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));
  if(lpNumberOfBytesWritten == NULL) {
     lpNumberOfBytesWritten = &byteswritten;
  }
  if(pHMHandleData->dwUserData == STD_INPUT_HANDLE) {
     return FALSE;
  }
  return O32_WriteFile(pHMHandleData->hHMHandle, lpBuffer, nNumberOfBytesToWrite,
                      lpNumberOfBytesWritten, lpOverlapped);
}

/*****************************************************************************
 * Name      : BOOL WriteFileEx
 * Purpose   : The WriteFileEx function writes data to a file. It is designed
 *             solely for asynchronous operation, unlike WriteFile, which is
 *             designed for both synchronous and asynchronous operation.
 *             WriteFileEx reports its completion status asynchronously,
 *             calling a specified completion routine when writing is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to write
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to write
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL HMDeviceStandardClass::WriteFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToWrite,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: WriteFileEx(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpOverlapped,
           lpCompletionRoutine));
  return FALSE;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceStandardClass::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceStandardClass::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintf2(("KERNEL32: HMDeviceStandardClass::GetFileType %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData));

  return FILE_TYPE_CHAR;
}
