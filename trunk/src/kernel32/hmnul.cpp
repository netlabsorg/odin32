/* $Id: hmnul.cpp,v 1.1 2001-11-29 00:20:48 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 NUL device access class
 *
 * 2001 Patrick Haller <patrick.haller@innotek.de>
 *
 */



#include <os2win.h>
#include <string.h>
#include <handlemanager.h>
#include "handlenames.h"
#include <heapstring.h>
#include <winioctl.h>
#include "hmdevice.h"
#include "hmnul.h"
#include "oslibdos.h"

#define DBG_LOCALLOG  DBG_hmnul
#include "dbglocal.h"


HMDeviceNulClass::HMDeviceNulClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName)
{
  dprintf(("HMDeviceNulClass::HMDevParPortClass(%s)\n",
           lpDeviceName));
  
  HMDeviceRegister("NUL", this);
  
  // add symbolic links to the "real name" of the device
  HandleNamesAddSymbolicLink("NUL:",       "NUL");
  HandleNamesAddSymbolicLink("\\\\.\\NUL", "NUL");
}

/*****************************************************************************
 * Name      : HMDeviceNulClass::FindDevice
 * Purpose   : Checks if lpDeviceName belongs to this device class
 * Parameters: LPCSTR lpClassDevName
 *             LPCSTR lpDeviceName
 *             int namelength
 * Variables :
 * Result    : checks if name is COMx or COMx: (x=1..8)
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceNulClass::FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength)
{
  // == "NUL"
  if(lstrcmpiA(lpDeviceName, lpClassDevName) != 0) 
    return FALSE;

  return TRUE;
}

DWORD HMDeviceNulClass::CreateFile(HANDLE        hHandle,
                                       LPCSTR lpFileName,
                                       PHMHANDLEDATA pHMHandleData,
                                       PVOID lpSecurityAttributes,
                                       PHMHANDLEDATA pHMHandleDataTemplate)
{
  dprintf(("HMDeviceNulClass::CreateFile(%s,%08xh,%08xh,%08xh)\n",
           lpFileName,
           pHMHandleData,
           lpSecurityAttributes,
           pHMHandleDataTemplate));
  
  // we don't actually open a handle to OS/2's NUL device,
  // we just swallow any I/O
  pHMHandleData->hHMHandle = 0xdeadface;
  SetLastError(ERROR_SUCCESS);
  return NO_ERROR;
}


                      /* this is a handler method for calls to CloseHandle() */
BOOL HMDeviceNulClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("HMDeviceNulClass: close request(%08xh)\n",
          pHMHandleData));
  
  return TRUE;
}


/*****************************************************************************
 * Name      : BOOL HMDeviceNulClass::WriteFile
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
 * Author    : Patrick Haller [2001/11/29]
 *****************************************************************************/

BOOL HMDeviceNulClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                     LPCVOID       lpBuffer,
                                     DWORD         nNumberOfBytesToWrite,
                                     LPDWORD       lpNumberOfBytesWritten,
                                     LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HMDeviceNulClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x)",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

  BOOL  ret;
  ULONG ulBytesWritten;

  if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
    dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
    dprintf(("Warning: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
  }
  
  // this is real NUL I/O
  ret = NO_ERROR;
  ulBytesWritten = nNumberOfBytesToWrite;

  if(lpNumberOfBytesWritten) {
       *lpNumberOfBytesWritten = (ret) ? ulBytesWritten : 0;
  }
  if(ret == FALSE) {
       dprintf(("ERROR: WriteFile failed with rc %d", GetLastError()));
  }

  return ret;
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

BOOL HMDeviceNulClass::WriteFileEx(PHMHANDLEDATA pHMHandleData,
                                       LPVOID       lpBuffer,
                                       DWORD        nNumberOfBytesToWrite,
                                       LPOVERLAPPED lpOverlapped,
                                       LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: WriteFileEx %s (%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpOverlapped,
           lpCompletionRoutine));

  SetLastError(ERROR_INVALID_FUNCTION);
  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceNulClass::ReadFile
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
 * Author    : SvL
 *****************************************************************************/

BOOL HMDeviceNulClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToRead,
                                    LPDWORD       lpNumberOfBytesRead,
                                    LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HMDeviceNulClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));

  BOOL  ret;
  ULONG ulBytesRead;

  if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
    dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
    dprintf(("Warning: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
  }
  
  // this is real NUL I/O
  ret = NO_ERROR;
  ulBytesRead = nNumberOfBytesToRead;

  if(lpNumberOfBytesRead) {
       *lpNumberOfBytesRead = (ret) ? ulBytesRead : 0;
  }
  if(ret == FALSE) {
       dprintf(("ERROR: ReadFile failed with rc %d", GetLastError()));
  }
  return ret;
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
BOOL HMDeviceNulClass::ReadFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToRead,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: ReadFileEx %s (%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpOverlapped,
           lpCompletionRoutine));

  SetLastError(ERROR_INVALID_FUNCTION);
  return FALSE;
}


BOOL HMDeviceNulClass::DeviceIoControl(PHMHANDLEDATA pHMHandleData, 
                                           DWORD dwIoControlCode,
                                           LPVOID lpInBuffer, 
                                           DWORD nInBufferSize,
                                           LPVOID lpOutBuffer, 
                                           DWORD nOutBufferSize,
                                           LPDWORD lpBytesReturned, 
                                           LPOVERLAPPED lpOverlapped)
{
  dprintf(("HMDeviceNulClass::DeviceIoControl: unimplemented dwIoControlCode=%08lx\n", dwIoControlCode));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}