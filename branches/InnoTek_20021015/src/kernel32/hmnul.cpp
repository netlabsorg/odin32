/* $Id: hmnul.cpp,v 1.3 2001-12-05 18:06:01 sandervl Exp $ */

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

DWORD HMDeviceNulClass::CreateFile(LPCSTR lpFileName,
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
                                 LPOVERLAPPED  lpOverlapped,
                                 LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
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

  if(lpCompletionRoutine) {
      dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
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
                                LPOVERLAPPED  lpOverlapped,
                                LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
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

  if(lpCompletionRoutine) {
      dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
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