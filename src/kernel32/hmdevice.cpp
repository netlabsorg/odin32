/* $Id: hmdevice.cpp,v 1.25 2001-01-22 18:26:50 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <os2win.h>
#include <string.h>
#include <misc.h>
#include "HandleManager.h"
#include "HMDevice.h"

#define DBG_LOCALLOG  DBG_hmdevice
#include "dbglocal.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

#ifndef ERROR_INVALID_FUNCTION
#define ERROR_INVALID_FUNCTION 1
#endif

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/


/*****************************************************************************
 * This pseudo-device logs all device requests to the logfile and returns    *
 * ERROR_INVALID_FUNCTION to virtually all requests -> debugging             *
 *****************************************************************************/
class HMDeviceDebugClass : public HMDeviceHandler
{
  public:
    HMDeviceDebugClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}
};


/*****************************************************************************
 * Name      : HMDeviceHandler::HMDeviceHandler
 * Purpose   : default constructor for a device handler object
 * Parameters: LPCSTR lpDeviceName
 * Variables :
 * Result    :
 * Remark    : this is only to identify the device for debugging purposes
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HMDeviceHandler::HMDeviceHandler(LPCSTR lpDeviceName)
{
                                      /* only a reference on the device name */
  HMDeviceHandler::lpHMDeviceName = lpDeviceName;
}


/*****************************************************************************
 * Name      : HMDeviceHandler::FindDevice
 * Purpose   : Checks if lpDeviceName belongs to this device class
 * Parameters: LPCSTR lpClassDevName
 *             LPCSTR lpDeviceName
 *             int namelength
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceHandler::FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength)
{
    if(stricmp(lpClassDevName, lpDeviceName) == 0) {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 * Name      : HMDeviceHandler::_DeviceReuqest
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
DWORD  HMDeviceHandler::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                        ULONG         ulRequestCode,
                                        ULONG         arg1,
                                        ULONG         arg2,
                                        ULONG         arg3,
                                        ULONG         arg4)
{
  dprintf(("KERNEL32:HandleManager::_DeviceRequest %s(%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData,
           ulRequestCode));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMDeviceHandler::DuplicateHandle
 * Purpose   : dummy version
 * Parameters:
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : the standard behaviour is to return an error code for non-
 *             existant request codes
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/
BOOL HMDeviceHandler::DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions)
{
  dprintf(("KERNEL32:HandleManager::DuplicateHandle %s(%08x,%08x,%08x,%08x,%08x) - NOT IMPLEMENTED!!!!!!!!\n",
           lpHMDeviceName,
           pHMHandleData,
           srcprocess, pHMSrcHandle, destprocess, desthandle));

  return FALSE;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::CreateFile
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

DWORD HMDeviceHandler::CreateFile (LPCSTR        lpFileName,
                                   PHMHANDLEDATA pHMHandleData,
                                   PVOID         lpSecurityAttributes,
                                   PHMHANDLEDATA pHMHandleDataTemplate)
{
  dprintf(("KERNEL32:HandleManager::CreateFile %s(%s,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData,
           lpSecurityAttributes,
           pHMHandleDataTemplate));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32:HandleManager::CloseHandle %s(%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::ReadFile
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

BOOL HMDeviceHandler::ReadFile(PHMHANDLEDATA pHMHandleData,
                               LPCVOID       lpBuffer,
                               DWORD         nNumberOfBytesToRead,
                               LPDWORD       lpNumberOfBytesRead,
                               LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HandleManager::ReadFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));

  SetLastError(ERROR_INVALID_FUNCTION);
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
BOOL HMDeviceHandler::ReadFileEx(PHMHANDLEDATA pHMHandleData,
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

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::WriteFile
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

BOOL HMDeviceHandler::WriteFile(PHMHANDLEDATA pHMHandleData,
                                LPCVOID       lpBuffer,
                                DWORD         nNumberOfBytesToWrite,
                                LPDWORD       lpNumberOfBytesWritten,
                                LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HandleManager::WriteFile %s(%08x,%08x,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

  SetLastError(ERROR_INVALID_FUNCTION);
  return FALSE;
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

BOOL HMDeviceHandler::WriteFileEx(PHMHANDLEDATA pHMHandleData,
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
 * Name      : DWORD HMDeviceHandler::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32:HandleManager::GetFileType %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData));

  return pHMHandleData->dwType;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::GetFileInformationByHandle
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

DWORD HMDeviceHandler::GetFileInformationByHandle(PHMHANDLEDATA               pHMHandleData,
                                                      BY_HANDLE_FILE_INFORMATION* pHFI)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetFileInformationByHandle %s(%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pHFI));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::SetEndOfFile
 * Purpose   : set end of file marker
 * Parameters: PHMHANDLEDATA              pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceHandler::SetEndOfFile(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetEndOfFile %s(%08xh)\n",
           lpHMDeviceName,
           pHMHandleData));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::SetFileTime
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

BOOL HMDeviceHandler::SetFileTime(PHMHANDLEDATA pHMHandleData,
                                      LPFILETIME pFT1,
                                      LPFILETIME pFT2,
                                      LPFILETIME pFT3)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetFileTime %s(%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pFT1,
           pFT2,
           pFT3));

  return(ERROR_INVALID_FUNCTION);
}

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::GetFileTime
 * Purpose   : get file time
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PFILETIME     pFT1
 *             PFILETIME     pFT2
 *             PFILETIME     pFT3
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMDeviceHandler::GetFileTime(PHMHANDLEDATA pHMHandleData,
                                      LPFILETIME pFT1,
                                      LPFILETIME pFT2,
                                      LPFILETIME pFT3)
{
  DebugInt3();
  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::GetFileSize
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

DWORD HMDeviceHandler::GetFileSize(PHMHANDLEDATA pHMHandleData,
                                       PDWORD        pSize)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetFileSize %s(%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           pSize));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::SetFilePointer
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

DWORD HMDeviceHandler::SetFilePointer(PHMHANDLEDATA pHMHandleData,
                                          LONG          lDistanceToMove,
                                          PLONG         lpDistanceToMoveHigh,
                                          DWORD         dwMoveMethod)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetFilePointer %s(%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           lDistanceToMove,
           lpDistanceToMoveHigh,
           dwMoveMethod));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::LockFile
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

DWORD HMDeviceHandler::LockFile(PHMHANDLEDATA pHMHandleData,
                                    DWORD         arg2,
                                    DWORD         arg3,
                                    DWORD         arg4,
                                    DWORD         arg5)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::LockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           arg2,
           arg3,
           arg4,
           arg5));

  return(ERROR_INVALID_FUNCTION);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::LockFileEx
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

DWORD HMDeviceHandler::LockFileEx(PHMHANDLEDATA pHMHandleData,
                                      DWORD         dwFlags,
                                      DWORD         dwReserved,
                                      DWORD         nNumberOfBytesToLockLow,
                                      DWORD         nNumberOfBytesToLockHigh,
                                      LPOVERLAPPED  lpOverlapped)
{

  dprintf(("KERNEL32: HandleManager::DeviceHandler::LockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwFlags,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::OpenFile
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

DWORD HMDeviceHandler::OpenFile (LPCSTR        lpFileName,
                                 PHMHANDLEDATA pHMHandleData,
                                 OFSTRUCT      *pOFStruct,
                                 UINT          arg3)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::OpenFile %s(%s,%08x,%08x,%08x) - stub?\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData,
           pOFStruct,
           arg3));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::UnlockFile
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

DWORD HMDeviceHandler::UnlockFile(PHMHANDLEDATA pHMHandleData,
                                      DWORD         arg2,
                                      DWORD         arg3,
                                      DWORD         arg4,
                                      DWORD         arg5)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::UnlockFile %s(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           arg2,
           arg3,
           arg4,
           arg5));

  return(ERROR_INVALID_FUNCTION);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::UnlockFileEx
 * Purpose   : file locking
 * Parameters: PHMHANDLEDATA pHMHandleData
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

DWORD HMDeviceHandler::UnlockFileEx(PHMHANDLEDATA pHMHandleData,
                                        DWORD         dwReserved,
                                        DWORD         nNumberOfBytesToLockLow,
                                        DWORD         nNumberOfBytesToLockHigh,
                                        LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::UnlockFileEx %s,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpHMDeviceName,
           pHMHandleData,
           dwReserved,
           nNumberOfBytesToLockLow,
           nNumberOfBytesToLockHigh,
           lpOverlapped));

  return(ERROR_INVALID_FUNCTION);
}



/*****************************************************************************
 * Name      : HMCreateSemaphore
 * Purpose   : router function for CreateSemaphore
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::CreateSemaphore(PHMHANDLEDATA         pHMHandleData,
                                       LPSECURITY_ATTRIBUTES lpsa,
                                       LONG                  lInitialCount,
                                       LONG                  lMaximumCount,
                                       LPCTSTR               lpszSemaphoreName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::CreateSemaphore(%08xh,%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           lInitialCount,
           lMaximumCount,
           lpszSemaphoreName));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMOpenSemaphore
 * Purpose   : router function for OpenSemaphore
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::OpenSemaphore(PHMHANDLEDATA         pHMHandleData,
                                     BOOL                  fInheritHandle,
                                     LPCTSTR               lpszSemaphoreName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::OpenSemaphore(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszSemaphoreName));

 return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMReleaseSemaphore
 * Purpose   : router function for ReleaseSemaphore
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

BOOL HMDeviceHandler::ReleaseSemaphore(PHMHANDLEDATA pHMHandleData,
                                       LONG          cReleaseCount,
                                       LPLONG        lpPreviousCount)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::ReleaseSemaphore(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           cReleaseCount,
           lpPreviousCount));

  return (ERROR_INVALID_FUNCTION);
}



/*****************************************************************************
 * Name      : HMCreateMutex
 * Purpose   : router function for CreateMutex
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::CreateMutex(PHMHANDLEDATA         pHMHandleData,
                                   LPSECURITY_ATTRIBUTES lpsa,
                                   BOOL                  fInitialOwner,
                                   LPCTSTR               lpszMutexName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::CreateMutex(%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           fInitialOwner,
           lpszMutexName));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMOpenMutex
 * Purpose   : router function for OpenMutex
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::OpenMutex(PHMHANDLEDATA         pHMHandleData,
                                 BOOL                  fInheritHandle,
                                 LPCTSTR               lpszMutexName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::OpenMutex(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszMutexName));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMReleaseMutex
 * Purpose   : router function for ReleaseMutex
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

BOOL HMDeviceHandler::ReleaseMutex(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::ReleaseMutex(%08xh)\n",
           pHMHandleData->hHMHandle));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMCreateEvent
 * Purpose   : router function for CreateEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::CreateEvent(PHMHANDLEDATA         pHMHandleData,
                                   LPSECURITY_ATTRIBUTES lpsa,
                                   BOOL                  fManualReset,
                                   BOOL                  fInitialState,
                                   LPCTSTR               lpszEventName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::CreateEvent(%08xh,%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           fManualReset,
           fInitialState,
           lpszEventName));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMOpenEvent
 * Purpose   : router function for OpenEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::OpenEvent(PHMHANDLEDATA         pHMHandleData,
                                 BOOL                  fInheritHandle,
                                 LPCTSTR               lpszEventName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::OpenEvent(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszEventName));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMSetEvent
 * Purpose   : router function for SetEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

BOOL HMDeviceHandler::SetEvent(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetEvent(%08xh)\n",
           pHMHandleData->hHMHandle));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMPulseEvent
 * Purpose   : router function for PulseEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

BOOL HMDeviceHandler::PulseEvent(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::PulseEvent(%08xh)\n",
           pHMHandleData->hHMHandle));

  return (ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : HMResetEvent
 * Purpose   : router function for ResetEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/06 20:44]
 *****************************************************************************/

BOOL HMDeviceHandler::ResetEvent(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::ResetEvent(%08xh)\n",
           pHMHandleData->hHMHandle));

  return (ERROR_INVALID_FUNCTION);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::WaitForSingleObject
 * Purpose   : object synchronization
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwTimeout
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::WaitForSingleObject(PHMHANDLEDATA pHMHandleData,
                                               DWORD         dwTimeout)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::WaitForSingleObject(%08xh,%08h)\n",
           pHMHandleData->hHMHandle,
           dwTimeout));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::WaitForSingleObjectEx
 * Purpose   : object synchronization
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwTimeout
 *             BOOL  fAlertable
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMDeviceHandler::WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                                 DWORD         dwTimeout,
                                                 BOOL          fAlertable)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::WaitForSingleObjectEx(%08xh,%08h,%08xh) not implemented correctly.\n",
           pHMHandleData->hHMHandle,
           dwTimeout,
           fAlertable));

  //@@@PH: WARNING mapped to WaitForSingleObject simply. fAlertable missing!
  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::FlushFileBuffers
 * Purpose   : flush the buffers of a file
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMDeviceHandler::FlushFileBuffers(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::Open32:FlushFileBuffers(%08xh)\n",
           pHMHandleData->hHMHandle));

  return(ERROR_INVALID_FUNCTION);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::GetOverlappedResult
 * Purpose   : asynchronus I/O
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

BOOL HMDeviceHandler::GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                              LPOVERLAPPED  arg2,
                                              LPDWORD       arg3,
                                              BOOL          arg4)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetOverlappedResult(%08xh,%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           arg2,
           arg3,
           arg4));

  return(ERROR_INVALID_FUNCTION);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::CreateFileMapping
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

DWORD HMDeviceHandler::CreateFileMapping(PHMHANDLEDATA              pHMHandleData,
                                         HANDLE                     hFile,
                                         LPSECURITY_ATTRIBUTES      lpFileMappingAttributes,
                                         DWORD                      flProtect,
                                         DWORD                      dwMaximumSizeHigh,
                                         DWORD                      dwMaximumSizeLow,
                                 LPCSTR lpName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::CreateFileMapping(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%s)\n",
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
 * Name      : DWORD HMDeviceHandler::OpenFileMapping
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

DWORD HMDeviceHandler::OpenFileMapping(PHMHANDLEDATA pHMHandleData,
                                      DWORD         fdwAccess,
                                      BOOL          fInherit,
                                      LPCTSTR       lpName)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::OpenFileMapping(%08xh,%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
     fdwAccess,
           fInherit,
           lpName));

  return(ERROR_INVALID_FUNCTION);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::MapViewOfFile
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

LPVOID HMDeviceHandler::MapViewOfFileEx(PHMHANDLEDATA pHMHandleData,
                                      DWORD         dwDesiredAccess,
                                      DWORD         dwFileOffsetHigh,
                                      DWORD         dwFileOffsetLow,
                                      DWORD         dwNumberOfBytesToMap,
                    LPVOID        lpBaseAddress)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::MapViewOfFileEx(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwDesiredAccess,
           dwFileOffsetHigh,
           dwFileOffsetLow,
           dwNumberOfBytesToMap, lpBaseAddress));

  return(NULL);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::DeviceIoControl
 * Purpose   : send command to device driver
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen
 *****************************************************************************/

BOOL HMDeviceHandler::DeviceIoControl(PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                      LPVOID lpInBuffer, DWORD nInBufferSize,
                                      LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                      LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::DeviceIoControl(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwIoControlCode,
           lpInBuffer,
           nInBufferSize,
           lpOutBuffer, nOutBufferSize));

  return(FALSE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::SetupComm
 * Purpose   : set com port parameters (queue)
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Achim Hasenmueller
 *****************************************************************************/

BOOL HMDeviceHandler::SetupComm(PHMHANDLEDATA pHMHandleData, DWORD dwInQueue, DWORD dwOutQueue)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetupComm(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwInQueue, dwOutQueue));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::GetCommState
 * Purpose   : query com port control block
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Achim Hasenmueller
 *****************************************************************************/
BOOL HMDeviceHandler::GetCommState(PHMHANDLEDATA pHMHandleData, LPDCB lpdcb)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetCommState(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpdcb));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::WaitCommEvent( PHMHANDLEDATA pHMHandleData,
                                     LPDWORD lpfdwEvtMask,
                                     LPOVERLAPPED lpo)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::WaitCommEvent(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpfdwEvtMask,
           lpo));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::GetCommProperties( PHMHANDLEDATA pHMHandleData,
                                         LPCOMMPROP lpcmmp)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetCommProperties(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpcmmp));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::GetCommMask( PHMHANDLEDATA pHMHandleData,
                                   LPDWORD lpfdwEvtMask)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetCommMask(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpfdwEvtMask));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::SetCommMask( PHMHANDLEDATA pHMHandleData,
                                   DWORD fdwEvtMask)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetCommMask(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           fdwEvtMask));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::PurgeComm( PHMHANDLEDATA pHMHandleData,
                                 DWORD fdwAction)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::PurgeComm(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           fdwAction));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::ClearCommError( PHMHANDLEDATA pHMHandleData,
                                      LPDWORD lpdwErrors,
                                      LPCOMSTAT lpcst)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::ClearCommError(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpdwErrors,
           lpcst));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::SetCommState( PHMHANDLEDATA pHMHandleData,
                                    LPDCB lpdcb)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetCommState(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpdcb));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::GetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                       LPCOMMTIMEOUTS lpctmo)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetCommTimeouts(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpctmo));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}
BOOL HMDeviceHandler::GetCommModemStatus( PHMHANDLEDATA pHMHandleData,
                                          LPDWORD lpModemStat )
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetCommModemStatus(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpModemStat));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::SetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                       LPCOMMTIMEOUTS lpctmo)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetCommTimeouts(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpctmo));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::TransmitCommChar( PHMHANDLEDATA pHMHandleData,
                                        CHAR cChar )
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::TransmitCommChar(%08xh,%02xh)\n",
           pHMHandleData->hHMHandle,
           cChar));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::SetCommConfig( PHMHANDLEDATA pHMHandleData,
                                     LPCOMMCONFIG lpCC,
                                     DWORD dwSize )
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetCommConfig(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpCC,
           dwSize));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::SetCommBreak( PHMHANDLEDATA pHMHandleData )
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetCommBreak(%08xh)\n",
           pHMHandleData->hHMHandle));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::GetCommConfig( PHMHANDLEDATA pHMHandleData,
                                     LPCOMMCONFIG lpCC,
                                     LPDWORD lpdwSize )
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetCommConfig(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpCC,
           lpdwSize));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::EscapeCommFunction( PHMHANDLEDATA pHMHandleData,
                                          UINT dwFunc )
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::EscapeCommFunction(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwFunc));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::ClearCommBreak( PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::ClearCommBreak(%08xh)\n",
           pHMHandleData->hHMHandle));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::SetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                            LPCOMMCONFIG lpCC,
                                            DWORD dwSize)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::SetDefaultCommConfig(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpCC,
           dwSize));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

BOOL HMDeviceHandler::GetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                            LPCOMMCONFIG lpCC,
                                            LPDWORD lpdwSize)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::GetDefaultCommConfig(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           lpCC,
           lpdwSize));
  SetLastError(ERROR_INVALID_HANDLE);
  return(FALSE);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::OpenThreadToken
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceHandler::OpenThreadToken(PHMHANDLEDATA pHMHandleData,
                                       HANDLE  ThreadHandle,
                                       BOOL    OpenAsSelf)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::OpenThreadToken(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           ThreadHandle));

  return ERROR_INVALID_HANDLE;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::OpenThreadToken
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceHandler::OpenProcessToken(PHMHANDLEDATA pHMHandleData,
                                        DWORD dwUserData,
                                        HANDLE  ProcessHandle)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::OpenProcessToken(%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           ProcessHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::CreateThread
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
HANDLE HMDeviceHandler::CreateThread(PHMHANDLEDATA pHMHandleData,
                                     LPSECURITY_ATTRIBUTES  lpsa,
                                     DWORD                  cbStack,
                                     LPTHREAD_START_ROUTINE lpStartAddr,
                                     LPVOID                 lpvThreadParm,
                                     DWORD                  fdwCreate,
                                     LPDWORD                lpIDThread,
                                     BOOL                   fFirstThread)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::CreateThread %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::GetThreadPriority
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
INT HMDeviceHandler::GetThreadPriority(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::GetThreadPriority %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::SuspendThread
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMDeviceHandler::SuspendThread(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::SuspendThread %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::SetThreadPriority
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceHandler::SetThreadPriority(PHMHANDLEDATA pHMHandleData, int priority)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::SetThreadPriority %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::GetThreadContext
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceHandler::GetThreadContext(PHMHANDLEDATA pHMHandleData, PCONTEXT lpContext)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::GetThreadContext %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::SetThreadContext
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceHandler::SetThreadContext(PHMHANDLEDATA pHMHandleData, const CONTEXT *lpContext)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::SetThreadContext %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::TerminateThread
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceHandler::TerminateThread(PHMHANDLEDATA pHMHandleData, DWORD exitcode)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::TerminateThread %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::ResumeThread
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD  HMDeviceHandler::ResumeThread(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::ResumeThread %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::GetExitCodeThread
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceHandler::GetExitCodeThread(PHMHANDLEDATA pHMHandleData, LPDWORD lpExitCode)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::GetExitCodeThread %08xh",
           pHMHandleData->hHMHandle));

  return ERROR_INVALID_HANDLE;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::SetThreadTerminated
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceHandler::SetThreadTerminated(PHMHANDLEDATA pHMHandleData)
{
    return FALSE;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::PeekNamedPipe
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDeviceHandler::PeekNamedPipe(PHMHANDLEDATA pHMHandleData,
                                      LPVOID lpvBuffer,
                                      DWORD   cbBuffer,
                                      LPDWORD lpcbRead,
                                      LPDWORD lpcbAvail,
                                      LPDWORD lpcbMessage)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::PeekNamedPipe (%08x)\n",
           pHMHandleData->hHMHandle));

  return (FALSE);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::CreateNamedPipe
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
DWORD HMDeviceHandler::CreateNamedPipe(PHMHANDLEDATA pHMHandleData,
                                         LPCTSTR lpName,
                                         DWORD  dwOpenMode,
                                         DWORD  dwPipeMode,
                                         DWORD  nMaxInstances,
                                         DWORD  nOutBufferSize,
                                         DWORD  nInBufferSize,
                                         DWORD  nDefaultTimeOut,
                                         LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::CreateNamedPipe (%s)\n",
           lpName));

  return ERROR_INVALID_HANDLE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::ConnectNamedPipe
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDeviceHandler::ConnectNamedPipe( PHMHANDLEDATA pHMHandleData,
                                          LPOVERLAPPED lpOverlapped)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::ConnectNamedPipe (%08x)\n",
           pHMHandleData->hHMHandle));

  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::DisconnectNamedPipe
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDeviceHandler::DisconnectNamedPipe(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::DisconnectNamedPipe (%08x)\n",
           pHMHandleData->hHMHandle));

  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::GetNamedPipeHandleState
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDeviceHandler::GetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                                LPDWORD lpState,
                                                LPDWORD lpCurInstances,
                                                LPDWORD lpMaxCollectionCount,
                                                LPDWORD lpCollectDataTimeout,
                                                LPTSTR  lpUserName,
                                                DWORD   nMaxUserNameSize)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::GetNamedPipeHandleState (%08x)\n",
           pHMHandleData->hHMHandle));

  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::GetNamedPipeInfo
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDeviceHandler::GetNamedPipeInfo(PHMHANDLEDATA pHMHandleData,
                                LPDWORD lpFlags,
                                LPDWORD lpOutBufferSize,
                                LPDWORD lpInBufferSize,
                                LPDWORD lpMaxInstances)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::GetNamedPipeInfo (%08x)\n",
           pHMHandleData->hHMHandle));

  return FALSE;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::TransactNamedPipe
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
DWORD HMDeviceHandler::TransactNamedPipe(PHMHANDLEDATA pHMHandleData,
                                           LPVOID        lpvWriteBuf,
                                           DWORD         cbWriteBuf,
                                           LPVOID        lpvReadBuf,
                                           DWORD         cbReadBuf,
                                           LPDWORD       lpcbRead,
                                           LPOVERLAPPED  lpo)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::TransactNamedPipe (%08x)\n",
           pHMHandleData->hHMHandle));

  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::SetNamedPipeHandleState
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDeviceHandler::SetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                                LPDWORD lpdwMode,
                                                LPDWORD lpcbMaxCollect,
                                                LPDWORD lpdwCollectDataTimeout)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::SetNamedPipeHandleState (%08x)\n",
           pHMHandleData->hHMHandle));

  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceHandler::CreatePipe
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDeviceHandler::CreatePipe(PHMHANDLEDATA pHMHandleDataRead,
                                 PHMHANDLEDATA pHMHandleDataWrite,
                                 LPSECURITY_ATTRIBUTES lpsa,
                                 DWORD         cbPipe)
{
  dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::CreatePipe (%08x,%08x)\n",
           pHMHandleDataRead->hHMHandle,pHMHandleDataWrite->hHMHandle));

    return(FALSE);
}
