/* $Id: hmsemaphore.cpp,v 1.7 2001-06-23 07:45:43 sandervl Exp $ */

/*
 * Win32 Semaphore implementation
 *
 * TODO: Inheritance
 * TODO: Does DCE_POSTONE work in Warp 3 or 4 with no FP applied?
 * TODO: No inheritance when CreateSemaphore is called for existing named event semaphore?
 *       (see HMCreateSemaphore in handlemanager.cpp)
 * TODO: OpenSemaphore does not work. (get SEM_INFO pointer)
 * TODO: Name collisions with files & mutex not allowed. Check if this can happen in OS/2
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
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

#ifdef USE_OS2SEMAPHORES
#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include <os2wrap.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#else
#include <os2win.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <heapshared.h>
#include "unicode.h"
#include "misc.h"

#include "HandleManager.H"
#include "HMSemaphore.h"
#include "oslibdos.h"

#define DBG_LOCALLOG	DBG_hmsemaphore
#include "dbglocal.h"

#ifndef DCE_AUTORESET
#define DCE_AUTORESET   0x1000  /* DosCreateEventSem option to auto-reset  */
                                /* event semaphore on post.                */
#define DCE_POSTONE     0x0800  /* DosCreateEventSem option to post only   */
                                /* waiter and auto-reset the semaphore when*/
                                /* there are multiple waiters.             */

#endif

#ifdef USE_OS2SEMAPHORES
typedef struct {
    LONG currentCount;
    LONG maximumCount;
    LONG refCount;
    HEV  hev;
} SEM_INFO, *PSEM_INFO;
#endif

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

DWORD HMDeviceSemaphoreClass::CreateSemaphore(PHMHANDLEDATA         pHMHandleData,
                                              LPSECURITY_ATTRIBUTES lpsa,
                                              LONG                  lInitialCount,
                                              LONG                  lMaximumCount,
                                              LPCTSTR               lpszSemaphoreName)
{
#ifdef USE_OS2SEMAPHORES
  APIRET rc;
  HEV hev;
  char szSemName[CCHMAXPATH];


  dprintf(("KERNEL32: HandleManager::Semaphore::CreateSemaphore(%08xh,%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           lInitialCount,
           lMaximumCount,
           lpszSemaphoreName));

  if(lMaximumCount <= 0 || lInitialCount < 0 || lInitialCount > lMaximumCount) {
      dprintf(("ERROR: invalid parameter"));
      return ERROR_INVALID_PARAMETER_W;
  }
  
  if(lpszSemaphoreName) {
      strcpy(szSemName, "\\SEM32\\");
      strcat(szSemName, lpszSemaphoreName);
      lpszSemaphoreName = szSemName;
  }
  rc = DosCreateEventSem(lpszSemaphoreName, &hev, DCE_POSTONE, lInitialCount);

  if(rc) {
      dprintf(("DosCreateEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      return error2WinError(rc);
  }
  pHMHandleData->dwAccess  = SEMAPHORE_ALL_ACCESS_W;
  PSEM_INFO pSemInfo       = (PSEM_INFO)_smalloc(sizeof(SEM_INFO));
  pSemInfo->refCount       = 1;
  pSemInfo->hev            = hev;
  pSemInfo->maximumCount   = lMaximumCount;
  pSemInfo->currentCount   = lInitialCount;
  pHMHandleData->hHMHandle = (DWORD)pSemInfo;
  pHMHandleData->dwInternalType = HMTYPE_SEMAPHORE;
  return ERROR_SUCCESS_W;
#else
  HANDLE hOpen32;

  dprintf(("KERNEL32: HandleManager::Semaphore::CreateSemaphore(%08xh,%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           lInitialCount,
           lMaximumCount,
           lpszSemaphoreName));

  hOpen32 = O32_CreateSemaphore(lpsa,              // call Open32
                                lInitialCount,
                                lMaximumCount,
                                (LPTSTR)lpszSemaphoreName);

  if (0 != hOpen32)                            // check success
  {
    pHMHandleData->hHMHandle = hOpen32;        // save handle
    return (NO_ERROR);
  }
  else
    return (O32_GetLastError());
#endif
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

DWORD HMDeviceSemaphoreClass::OpenSemaphore(PHMHANDLEDATA         pHMHandleData,
                                            BOOL                  fInheritHandle,
                                            LPCTSTR               lpszSemaphoreName)
{
#ifdef USE_OS2SEMAPHORES
  HEV    hev;
  APIRET rc;
  char szSemName[CCHMAXPATH];

  //TODO: NOT WORKING (initialcount/maximumcount)
  dprintf(("KERNEL32: HandleManager::Semaphore::OpenSemaphore(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszSemaphoreName));

  if(lpszSemaphoreName == NULL) {
      pHMHandleData->hHMHandle = 0;
      return ERROR_INVALID_PARAMETER_W;
  }

  strcpy(szSemName, "\\SEM32\\");
  strcat(szSemName, lpszSemaphoreName);
  rc = DosOpenEventSem(szSemName, &hev);
  if(rc) {
      dprintf(("DosOpenEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      return error2WinError(rc);
  }
  pHMHandleData->hHMHandle = hev;
  pHMHandleData->dwInternalType = HMTYPE_SEMAPHORE;
  return ERROR_SUCCESS_W;
#else
  HANDLE hOpen32;

  dprintf(("KERNEL32: HandleManager::Semaphore::OpenSemaphore(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszSemaphoreName));

  hOpen32 = O32_OpenSemaphore(pHMHandleData->dwAccess,              // call Open32
                              fInheritHandle,
                              lpszSemaphoreName);

  if (0 != hOpen32)                            // check success
  {
    pHMHandleData->hHMHandle = hOpen32;        // save handle
    return (NO_ERROR);
  }
  else
    return (O32_GetLastError());
#endif
}

/*****************************************************************************
 * Name      : HMDeviceEventClass::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : 
 *****************************************************************************/

#ifdef USE_OS2SEMAPHORES
BOOL HMDeviceSemaphoreClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  APIRET rc;
  PSEM_INFO pSemInfo  = (PSEM_INFO)pHMHandleData->hHMHandle;

  if(pSemInfo) {
      rc = DosCloseEventSem(pSemInfo->hev);
      if(rc) {
          dprintf(("DosCloseEventSem %x failed with rc %d", pSemInfo->hev, rc));
          SetLastError(error2WinError(rc));
          return FALSE;
      }
      if(InterlockedDecrement(&pSemInfo->refCount) == 0) {
          free(pSemInfo);
      }
  }
  return TRUE;
}
#endif


/*****************************************************************************
 * Name      : HMDeviceEventClass::DuplicateHandle
 * Purpose   : 
 * Parameters:
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : the standard behaviour is to return an error code for non-
 *             existant request codes
 * Status    :
 *
 * Author    : 
 *****************************************************************************/
#ifdef USE_OS2SEMAPHORES
BOOL HMDeviceSemaphoreClass::DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions)
{
  APIRET rc; 
  HEV    hev;
  PSEM_INFO pSemInfo  = (PSEM_INFO)pHMSrcHandle->hHMHandle;
  
  dprintf(("KERNEL32:HandleManager::DuplicateHandle %s(%08x,%08x,%08x,%08x,%08x)",
           lpHMDeviceName,
           pHMHandleData,
           srcprocess, pHMSrcHandle, destprocess, desthandle));

  if(srcprocess != destprocess) {
      DebugInt3();
      SetLastError(ERROR_ACCESS_DENIED_W);
      return FALSE;
  }
  InterlockedIncrement(&pSemInfo->refCount);
  pHMHandleData->dwAccess  = fdwAccess;
  pHMHandleData->hHMHandle = (DWORD)pSemInfo;
  pHMHandleData->dwInternalType = HMTYPE_SEMAPHORE;
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
#endif

#ifdef USE_OS2SEMAPHORES
/*****************************************************************************
 * Name      : DWORD HMDeviceSemaphoreClass::WaitForSingleObject
 * Purpose   : object synchronization
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwTimeout
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceSemaphoreClass::WaitForSingleObject(PHMHANDLEDATA pHMHandleData,
                                                  DWORD         dwTimeout)
{
 DWORD rc;

  dprintf2(("KERNEL32: HMDeviceSemaphoreClass::WaitForSingleObject(%08xh %08xh)",
            pHMHandleData->hHMHandle, dwTimeout));

  if(!(pHMHandleData->dwAccess & SYNCHRONIZE_W) )
  {
      dprintf(("ERROR: Access denied!!"));
      SetLastError(ERROR_ACCESS_DENIED_W);
      return WAIT_FAILED_W;
  }

  PSEM_INFO pSemInfo  = (PSEM_INFO)pHMHandleData->hHMHandle;

  if(InterlockedDecrement(&pSemInfo->currentCount) >= 0) {
      SetLastError(ERROR_SUCCESS_W);
      return WAIT_OBJECT_0_W;  
  }

  rc = DosWaitEventSem(pSemInfo->hev, dwTimeout);
  if(rc && rc != ERROR_INTERRUPT && rc != ERROR_TIMEOUT && rc != ERROR_SEM_OWNER_DIED) {
      dprintf(("DosWaitEventSem %x failed with rc %d", pSemInfo->hev, rc));
      SetLastError(error2WinError(rc));
      return WAIT_FAILED_W;
  }
  SetLastError(ERROR_SUCCESS_W);
  if(rc == ERROR_INTERRUPT || rc == ERROR_SEM_OWNER_DIED) {
      return WAIT_ABANDONED_W;
  }
  else 
  if(rc == ERROR_TIMEOUT) {
      return WAIT_TIMEOUT_W;
  }
  return WAIT_OBJECT_0_W;
}
#endif

#ifdef USE_OS2SEMAPHORES
/*****************************************************************************
 * Name      : DWORD HMDeviceSemaphoreClass::WaitForSingleObjectEx
 * Purpose   : object synchronization
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             DWORD dwTimeout
 *             BOOL  fAlertable
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceSemaphoreClass::WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                                 DWORD         dwTimeout,
                                                 BOOL          fAlertable)
{
    dprintf2(("KERNEL32: HMDeviceSemaphoreClass::WaitForSingleObjectEx(%08xh,%08h,%08xh) not implemented correctly.\n",
              pHMHandleData->hHMHandle, dwTimeout, fAlertable));

    if(!(pHMHandleData->dwAccess & SYNCHRONIZE_W) )
    {
        dprintf(("ERROR: Access denied!!"));
        SetLastError(ERROR_ACCESS_DENIED_W);
        return WAIT_FAILED_W;
    }

    return WaitForSingleObject(pHMHandleData, dwTimeout);
}
#endif

#ifdef USE_OS2SEMAPHORES
/*****************************************************************************
 * Name      : BOOL HMDeviceSemaphoreClass::MsgWaitForMultipleObjects
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMDeviceSemaphoreClass::MsgWaitForMultipleObjects(PHMHANDLEDATA pHMHandleData,
                                                        DWORD      nCount,
                                                        PHANDLE    pHandles,
                                                        BOOL       fWaitAll,
                                                        DWORD      dwMilliseconds,
                                                        DWORD      dwWakeMask)
{
    dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::MsgWaitForMultipleObjects %08x %d %x %d %d %x",
              pHMHandleData->hHMHandle, nCount, pHandles, fWaitAll, dwMilliseconds, dwWakeMask));

    if(!(pHMHandleData->dwAccess & SYNCHRONIZE_W) )
    {
        dprintf(("ERROR: Access denied!!"));
        SetLastError(ERROR_ACCESS_DENIED_W);
        return WAIT_FAILED_W;
    }

    return WAIT_FAILED_W;
}
#endif

#ifdef USE_OS2SEMAPHORES
/*****************************************************************************
 * Name      : BOOL HMDeviceSemaphoreClass::WaitForMultipleObjects
 * Purpose   :
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMDeviceSemaphoreClass::WaitForMultipleObjects(PHMHANDLEDATA pHMHandleData,
                                                 DWORD   cObjects,
                                                 PHANDLE lphObjects,
                                                 BOOL    fWaitAll,
                                                 DWORD   dwTimeout)
{
    dprintf(("KERNEL32: ERROR: HandleManager::DeviceHandler::WaitForMultipleObjects %08x %d %x %d %x",
              pHMHandleData->hHMHandle, cObjects, lphObjects, fWaitAll, dwTimeout));

    if(!(pHMHandleData->dwAccess & SYNCHRONIZE_W) )
    {
        dprintf(("ERROR: Access denied!!"));
        SetLastError(ERROR_ACCESS_DENIED_W);
        return WAIT_FAILED_W;
    }

    return WAIT_FAILED_W;
}
#endif

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

BOOL HMDeviceSemaphoreClass::ReleaseSemaphore(PHMHANDLEDATA pHMHandleData,
                                              LONG          cReleaseCount,
                                              LPLONG        lpPreviousCount)
{
#ifdef USE_OS2SEMAPHORES
  APIRET rc;
  ULONG count;  
  
  dprintf2(("KERNEL32: HandleManager::Semaphore::ReleaseSemaphore(%08xh,%08xh,%08xh)\n",
            pHMHandleData->hHMHandle,
            cReleaseCount,
            lpPreviousCount));

  if(!(pHMHandleData->dwAccess & SEMAPHORE_MODIFY_STATE_W) )
  {
      dprintf(("ERROR: Access denied!!"));
      SetLastError(ERROR_ACCESS_DENIED_W);
      return FALSE;
  }

  PSEM_INFO pSemInfo  = (PSEM_INFO)pHMHandleData->hHMHandle;

  if(InterlockedIncrement(&pSemInfo->currentCount) > 0) {
      //TODO: this is NOT thread safe:
      if(pSemInfo->currentCount > pSemInfo->maximumCount) { 
          pSemInfo->currentCount = pSemInfo->maximumCount;
      }
      SetLastError(ERROR_SUCCESS_W);
      return TRUE;  
  }

  rc = DosResetEventSem(pSemInfo->hev, &count);
  if(rc) {
      dprintf(("DosResetEventSem %x failed with rc %d", pSemInfo->hev, rc));
      SetLastError(error2WinError(rc));
      return FALSE;
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
#else
  dprintf(("KERNEL32: HandleManager::Semaphore::ReleaseSemaphore(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           cReleaseCount,
           lpPreviousCount));

  return (O32_ReleaseSemaphore(pHMHandleData->hHMHandle,
                               cReleaseCount,
                               lpPreviousCount));
#endif
}

