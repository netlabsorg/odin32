/* $Id: hmopen32.cpp,v 1.3 1999-06-17 21:52:01 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 */


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */

//#define DEBUG_LOCAL

#ifdef DEBUG_LOCAL
#  define dprintfl(a) dprintf(a)
#else
inline void ignore_dprintf(...){}
#  define dprintfl(a) ignore_dprintf(a)
#endif

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <string.h>
#include "HandleManager.h"
#include "HMOpen32.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

/*****************************************************************************
 * Name      : HMDeviceOpen32Class::_DeviceRequest
 * Purpose   : entry method for special request functions
 * Parameters: ULONG ulRequestCode
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : the standard behaviour is to return an error code for non-
 *             existant request codes
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/
DWORD  HMDeviceOpen32Class::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                        ULONG         ulRequestCode,
                                        ULONG         arg1,
                                        ULONG         arg2,
                                        ULONG         arg3,
                                        ULONG         arg4)
{
  dprintfl(("KERNEL32: HandleManager::Open32::_DeviceRequest %s(%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           ulRequestCode));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::CreateFile (LPCSTR        lpFileName,
                                       PHMHANDLEDATA pHMHandleData,
                                       PVOID         lpSecurityAttributes,
                                       PHMHANDLEDATA pHMHandleDataTemplate)
{
  HFILE hFile;
  HFILE hTemplate;

  dprintfl(("KERNEL32: HandleManager::Open32::CreateFile %s(%s,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData,
           lpSecurityAttributes,
           pHMHandleDataTemplate));

  if (strncmp(lpFileName,       // "support" for local unc names
              "\\\\.\\",
              4) == 0)
    lpFileName+=4;

  // create from template
  if (pHMHandleDataTemplate != NULL)
     hTemplate = pHMHandleDataTemplate->hWinHandle;
  else
     hTemplate = 0;

  hFile = O32_CreateFile(lpFileName,
                         pHMHandleData->dwAccess,
                         pHMHandleData->dwShare,
                         //(LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                         NULL,
                         pHMHandleData->dwCreation,
                         pHMHandleData->dwFlags,
                         hTemplate);
  if (hFile != INVALID_HANDLE_ERROR)
  {
     pHMHandleData->hWinHandle = hFile;
     return (NO_ERROR);
  }
  else
    return(O32_GetLastError());
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  BOOL bRC;

  dprintfl(("KERNEL32: HandleManager::Open32::CloseHandle(%08x)\n",
           pHMHandleData->hWinHandle));

  bRC = O32_CloseHandle(pHMHandleData->hWinHandle);

  dprintfl(("KERNEL32: HandleManager::Open32::CloseHandle returned %08xh\n",
           bRC));

  return (DWORD)bRC;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::ReadFile
 * Purpose   : read data from handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToRead,
 *             LPDWORD       lpNumberOfBytesRead,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::ReadFile(PHMHANDLEDATA pHMHandleData,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToRead,
                                    LPDWORD       lpNumberOfBytesRead,
                                    LPOVERLAPPED  lpOverlapped)
{
  BOOL bRC;

  dprintfl(("KERNEL32: HandleManager::Open32::ReadFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));

  bRC = O32_ReadFile(pHMHandleData->hWinHandle,
                     (PVOID)lpBuffer,
                     nNumberOfBytesToRead,
                     lpNumberOfBytesRead,
                     lpOverlapped);

  dprintfl(("KERNEL32: HandleManager::Open32::ReadFile returned %08xh\n",
           bRC));

  return (DWORD)bRC;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::WriteFile
 * Purpose   : write data to handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToWrite,
 *             LPDWORD       lpNumberOfBytesWritten,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::WriteFile(PHMHANDLEDATA pHMHandleData,
                                 LPCVOID       lpBuffer,
                                 DWORD         nNumberOfBytesToWrite,
                                 LPDWORD       lpNumberOfBytesWritten,
                                 LPOVERLAPPED  lpOverlapped)
{
  BOOL bRC;

  dprintfl(("KERNEL32: HandleManager::Open32::WriteFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

  bRC = O32_WriteFile(pHMHandleData->hWinHandle,
                      lpBuffer,
                      nNumberOfBytesToWrite,
                      lpNumberOfBytesWritten,
                      lpOverlapped);

  dprintfl(("KERNEL32: HandleManager::Open32::WriteFile returned %08xh\n",
           bRC));

  return (DWORD)bRC;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HandleManager::Open32::GetFileType %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData));

  return O32_GetFileType(pHMHandleData->hWinHandle);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::GetFileInformationByHandle
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA               pHMHandleData
 *             BY_HANDLE_FILE_INFORMATION* pHFI
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::GetFileInformationByHandle(PHMHANDLEDATA               pHMHandleData,
                                                      BY_HANDLE_FILE_INFORMATION* pHFI)
{
  dprintfl(("KERNEL32: HandleManager::Open32::GetFileInformationByHandle %s(%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pHFI));

  return O32_GetFileInformationByHandle(pHMHandleData->hWinHandle,
                                        pHFI);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceOpen32Class::SetEndOfFile
 * Purpose   : set end of file marker
 * Parameters: PHMHANDLEDATA              pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceOpen32Class::SetEndOfFile(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HandleManager::Open32::SetEndOfFile %s(%08xh)\n",
           lpHMDeviceName,
           pHMHandleData));

  return O32_SetEndOfFile(pHMHandleData->hWinHandle);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceOpen32Class::SetFileTime
 * Purpose   : set file time
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PFILETIME     pFT1
 *             PFILETIME     pFT2
 *             PFILETIME     pFT3
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceOpen32Class::SetFileTime(PHMHANDLEDATA pHMHandleData,
                                      LPFILETIME pFT1,
                                      LPFILETIME pFT2,
                                      LPFILETIME pFT3)
{
  dprintfl(("KERNEL32: HandleManager::Open32::SetFileTime %s(%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pFT1,
           pFT2,
           pFT3));

  return O32_SetFileTime(pHMHandleData->hWinHandle,
                         pFT1,
                         pFT2,
                         pFT3);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::GetFileSize
 * Purpose   : set file time
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PDWORD        pSize
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::GetFileSize(PHMHANDLEDATA pHMHandleData,
                                       PDWORD        pSize)
{
  dprintfl(("KERNEL32: HandleManager::Open32::GetFileSize %s(%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pSize));

  return O32_GetFileSize(pHMHandleData->hWinHandle,
                         pSize);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::SetFilePointer
 * Purpose   : set file pointer
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LONG          lDistanceToMove
 *             PLONG         lpDistanceToMoveHigh
 *             DWORD         dwMoveMethod
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::SetFilePointer(PHMHANDLEDATA pHMHandleData,
                                          LONG          lDistanceToMove,
                                          PLONG         lpDistanceToMoveHigh,
                                          DWORD         dwMoveMethod)
{
  dprintfl(("KERNEL32: HandleManager::Open32::SetFilePointer %s(%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           lDistanceToMove,
           lpDistanceToMoveHigh,
           dwMoveMethod));

  return O32_SetFilePointer(pHMHandleData->hWinHandle,
                            lDistanceToMove,
                            lpDistanceToMoveHigh,
                            dwMoveMethod);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::LockFile
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD arg2
 *             DWORD arg3
 *             DWORD arg4
 *             DWORD arg5
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::LockFile(PHMHANDLEDATA pHMHandleData,
                                    DWORD         arg2,
                                    DWORD         arg3,
                                    DWORD         arg4,
                                    DWORD         arg5)
{
  dprintfl(("KERNEL32: HandleManager::Open32::LockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           arg2,
           arg3,
           arg4,
           arg5));

  return O32_LockFile(pHMHandleData->hWinHandle,
                      arg2,
                      arg3,
                      arg4,
                      arg5);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::LockFileEx
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwFlags
 *             DWORD dwReserved
 *             DWORD nNumberOfBytesToLockLow
 *             DWORD nNumberOfBytesToLockHigh
 *             LPOVERLAPPED lpOverlapped
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::LockFileEx(PHMHANDLEDATA pHMHandleData,
                                      DWORD         dwFlags,
                                      DWORD         dwReserved,
                                      DWORD         nNumberOfBytesToLockLow,
                                      DWORD         nNumberOfBytesToLockHigh,
                                      LPOVERLAPPED  lpOverlapped)
{

  dprintfl(("KERNEL32: HandleManager::Open32::LockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwFlags,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));


  return(O32_LockFile(pHMHandleData->hWinHandle,
                      lpOverlapped->Offset,
                      lpOverlapped->OffsetHigh,
                      nNumberOfBytesToLockLow,
                      nNumberOfBytesToLockHigh));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::OpenFile
 * Purpose   : this is called from the handle manager if a OpenFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::OpenFile (LPCSTR        lpFileName,
                                     PHMHANDLEDATA pHMHandleData,
                                     OFSTRUCT      *pOFStruct,
                                     UINT          arg3)
{
  HFILE hFile;

  dprintfl(("KERNEL32: HandleManager::Open32::OpenFile %s(%s,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData,
           pOFStruct,
           arg3));

  if (strcmp(lpFileName,       // "support" for local unc names
             "\\\\.\\") == 0)
    lpFileName+=4;

  hFile = O32_OpenFile(lpFileName,
                       pOFStruct,
                       arg3);
  if (hFile != INVALID_HANDLE_ERROR)
     pHMHandleData->hWinHandle = hFile;

  return(hFile);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::UnlockFile
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD arg2
 *             DWORD arg3
 *             DWORD arg4
 *             DWORD arg5
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::UnlockFile(PHMHANDLEDATA pHMHandleData,
                                      DWORD         arg2,
                                      DWORD         arg3,
                                      DWORD         arg4,
                                      DWORD         arg5)
{
  dprintfl(("KERNEL32: HandleManager::Open32::UnlockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           arg2,
           arg3,
           arg4,
           arg5));

  return O32_UnlockFile(pHMHandleData->hWinHandle,
                        arg2,
                        arg3,
                        arg4,
                        arg5);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::UnlockFileEx
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwFlags
 *             DWORD dwReserved
 *             DWORD nNumberOfBytesToLockLow
 *             DWORD nNumberOfBytesToLockHigh
 *             LPOVERLAPPED lpOverlapped
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceOpen32Class::UnlockFileEx(PHMHANDLEDATA pHMHandleData,
                                        DWORD         dwFlags,
                                        DWORD         dwReserved,
                                        DWORD         nNumberOfBytesToLockLow,
                                        DWORD         nNumberOfBytesToLockHigh,
                                        LPOVERLAPPED  lpOverlapped)
{

  dprintfl(("KERNEL32: HandleManager::Open32::UnlockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwFlags,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));

  return(O32_UnlockFile(pHMHandleData->hWinHandle,
                   lpOverlapped->Offset,
                   lpOverlapped->OffsetHigh,
                   nNumberOfBytesToLockLow,
                   nNumberOfBytesToLockHigh));
}
