/* $Id: hmdevice.cpp,v 1.17 2000-03-16 19:20:37 sandervl Exp $ */

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
#include <misc.h>
#include "HandleManager.h"
#include "HMDevice.h"

#define DBG_LOCALLOG	DBG_hmdevice
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
                       		DWORD   fdwOptions)
{
  dprintf(("KERNEL32:HandleManager::DuplicateHandle %s(%08x,%08x,%08x,%08x,%08x) - NOT IMPLEMENTED!!!!!!!!\n",
           lpHMDeviceName,
           pHMHandleData,
           srcprocess, pHMSrcHandle, destprocess, desthandle));

  return(ERROR_INVALID_FUNCTION);
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
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));

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
           pHMHandleData,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

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

DWORD HMDeviceHandler::UnlockFileEx(PHMHANDLEDATA pHMHandleData,
                                        DWORD         dwFlags,
                                        DWORD         dwReserved,
                                        DWORD         nNumberOfBytesToLockLow,
                                        DWORD         nNumberOfBytesToLockHigh,
                                        LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32: HandleManager::DeviceHandler::UnlockFileEx %s(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
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
               		     	         LPCSTR lpName,   
				         HFILE *hOldMap)  // if create an existing memmap, return handle of old one
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
                                     LPDWORD                lpIDThread)
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
