/* $Id: hmopen32.cpp,v 1.29 2001-04-26 13:22:47 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 */


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */

#define DEBUG_LOCAL

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
#include "mmap.h"

#define DBG_LOCALLOG  DBG_hmopen32
#include "dbglocal.h"

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

BOOL HMDeviceOpen32Class::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  BOOL bRC;

  dprintfl(("KERNEL32: HandleManager::Open32::CloseHandle(%08x)\n",
           pHMHandleData->hHMHandle));

  bRC = O32_CloseHandle(pHMHandleData->hHMHandle);

  dprintfl(("KERNEL32: HandleManager::Open32::CloseHandle returned %08xh\n",
           bRC));

  return bRC;
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
BOOL HMDeviceOpen32Class::DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions)
{
 BOOL rc;

  dprintf(("KERNEL32:HandleManager::Open32::DuplicateHandle %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData,
           srcprocess, pHMSrcHandle->hHMHandle, destprocess, desthandle));

  rc = O32_DuplicateHandle(srcprocess, pHMSrcHandle->hHMHandle, destprocess, desthandle, fdwAccess, fInherit, fdwOptions);

  if(rc == TRUE) {
      pHMHandleData->hHMHandle = *desthandle;
      return TRUE;
  }
  else  return FALSE;
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

  return O32_GetFileType(pHMHandleData->hHMHandle);
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

  return O32_GetFileInformationByHandle(pHMHandleData->hHMHandle,
                                        pHFI);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::WaitForSingleObject
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

DWORD HMDeviceOpen32Class::WaitForSingleObject(PHMHANDLEDATA pHMHandleData,
                                               DWORD         dwTimeout)
{
 DWORD rc;
#ifdef DEBUG
 DWORD starttime, endtime;
#endif

  dprintfl(("KERNEL32: HandleManager::Open32::WaitForSingleObject(%08xh %08xh)\n",
            pHMHandleData->hHMHandle,
            dwTimeout));

#ifdef DEBUG
  if(dwTimeout) {
       starttime = O32_GetCurrentTime();
  }
#endif
  rc =  (O32_WaitForSingleObject(pHMHandleData->hHMHandle,
                                 dwTimeout));
#ifdef DEBUG
  if(dwTimeout) {
       endtime = O32_GetCurrentTime();
       dprintf2(("KERNEL32: HandleManager::WaitForSingleObject %x %x delta = %x (rc=%x)", pHMHandleData->hHMHandle, dwTimeout, endtime - starttime, rc));
  }
  else dprintf2(("KERNEL32: HandleManager::WaitForSingleObject %x 0 rc=%x", pHMHandleData->hHMHandle, rc));
#endif
  return rc;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceOpen32Class::WaitForSingleObjectEx
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

DWORD HMDeviceOpen32Class::WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                                 DWORD         dwTimeout,
                                                 BOOL          fAlertable)
{
  dprintfl(("KERNEL32: HandleManager::Open32::WaitForSingleObjectEx(%08xh,%08h,%08xh) not implemented correctly.\n",
            pHMHandleData->hHMHandle,
            dwTimeout,
            fAlertable));

  //@@@PH: WARNING mapped to WaitForSingleObject simply. fAlertable missing!
  return (O32_WaitForSingleObject(pHMHandleData->hHMHandle,
                                  dwTimeout));
}

