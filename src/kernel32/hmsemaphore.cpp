/* $Id: hmsemaphore.cpp,v 1.9 2001-06-23 19:43:50 sandervl Exp $ */

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
#define INCL_WIN
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
      FixSemName((char *)lpszSemaphoreName);
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
  FixSemName(szSemName);
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
    return HMSemMsgWaitForMultipleObjects(nCount, pHandles, fWaitAll, dwMilliseconds, dwWakeMask);
}

DWORD HMSemMsgWaitForMultipleObjects(DWORD   cObjects,
                                     PHANDLE lphObjects,
                                     BOOL    fWaitAll,
                                     DWORD   dwTimeout,
                                     DWORD   dwWakeMask)
{
    PHMHANDLEDATA *pHandles;
    HMUX hmux;
    PSEMRECORD pSemRec;
    int i, j;
    APIRET rc;
    ULONG  ulUser;
    static HMODULE hUser32 = 0;
    static BOOL (* WINAPI pfnPeekMessageA)(LPMSG,HWND,UINT,UINT,UINT);

    dprintf(("KERNEL32: WaitForMultipleObjects %d %x %d %x",
              cObjects, lphObjects, fWaitAll, dwTimeout));

    if(pfnPeekMessageA == NULL) {
          hUser32 = LoadLibraryA("USER32.DLL");
          *(FARPROC *)&pfnPeekMessageA = GetProcAddress(hUser32,"PeekMessageA");
    }   

    if(cObjects == 1) {
        //Can't use DosCreateMuxWaitSem here (will return error 292)
        PHMHANDLEDATA pHandle = HMQueryHandleData(lphObjects[0]);
        if(pHandle == NULL) {
            return WAIT_FAILED_W;
        }
        if(!(pHandle->dwAccess & SYNCHRONIZE_W) )
        {
            dprintf(("ERROR: Access denied (handle %x, index 0)!!", lphObjects[0]));
            SetLastError(ERROR_ACCESS_DENIED_W);
            return WAIT_FAILED_W;
        }

        switch(pHandle->dwInternalType) {
        case HMTYPE_SEMAPHORE:
        {
            PSEM_INFO pSemInfo  = (PSEM_INFO)pHandle->hHMHandle;

            dprintf(("KERNEL32: HMWaitForMultipleObjects(S): handle 0: ODIN-%08xh, OS/2-%08xh",
                     lphObjects[0], pSemInfo->hev));
            if(InterlockedDecrement(&pSemInfo->currentCount) >= 0) {
                return WAIT_OBJECT_0_W;
            }
            rc = WinWaitEventSem(pSemInfo->hev, dwTimeout);
            break;
        }
        case HMTYPE_EVENTSEM:
            dprintf(("KERNEL32: HMWaitForMultipleObjects(E): handle 0: ODIN-%08xh, OS/2-%08xh",
                     lphObjects[0], pHandle->hHMHandle));
            rc = WinWaitEventSem((HEV)pHandle->hHMHandle, dwTimeout);
            break;
        case HMTYPE_MUTEXSEM:
            dprintf(("KERNEL32: HMWaitForMultipleObjects(M): handle 0: ODIN-%08xh, OS/2-%08xh",
                     lphObjects[0], pHandle->hHMHandle));
            rc = WinRequestMutexSem((HMTX)pHandle->hHMHandle, dwTimeout);
            break;
        }
        if(rc && rc != ERROR_INTERRUPT && rc != ERROR_TIMEOUT && rc != ERROR_SEM_OWNER_DIED) {
            dprintf(("WinWaitEventSem/WinRequestMutexSem %x failed with rc %d", pHandle->hHMHandle, rc));
            SetLastError(error2WinError(rc));
            return WAIT_FAILED_W;
        }
        SetLastError(ERROR_SUCCESS_W);
        if(rc == ERROR_INTERRUPT || rc == ERROR_SEM_OWNER_DIED) {
            dprintf(("WAIT_ABANDONED_W (rc %d)", rc));
            return WAIT_ABANDONED_W;
        }
        else 
        if(rc == ERROR_TIMEOUT) {
            dprintf(("WAIT_TIMEOUT_W (rc %d)", rc));
            return WAIT_TIMEOUT_W;
        }
        MSG msg ;
   
        if(pfnPeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE_W) == TRUE) {
            dprintf(("WAIT_OBJECT_0_W+1 (rc %d)", rc));
            return WAIT_OBJECT_0_W + 1;
        }
        dprintf(("WAIT_OBJECT_0_W+1 (rc %d)", rc));
        return WAIT_OBJECT_0_W;
    }
    pHandles = (PHMHANDLEDATA *)alloca(cObjects * sizeof(PHMHANDLEDATA));
    pSemRec  = (PSEMRECORD)alloca(cObjects * sizeof(SEMRECORD));
    if(pHandles == NULL || pSemRec == NULL) {
        dprintf(("ERROR: out of memory!!"));
        SetLastError(ERROR_OUTOFMEMORY_W);
        return WAIT_FAILED_W;
    }
    for(i=0;i<cObjects;i++) {
        pHandles[i] = HMQueryHandleData(lphObjects[i]);
        if(pHandles[i] == NULL) {
            dprintf(("ERROR: handle %x not recognized", lphObjects[i]));
            SetLastError(ERROR_INVALID_HANDLE_W);
            return WAIT_FAILED_W;
        }
        if(!(pHandles[i]->dwAccess & SYNCHRONIZE_W) )
        {
            dprintf(("ERROR: Access denied (handle %x, index %d)!!", lphObjects[i], i));
            SetLastError(ERROR_ACCESS_DENIED_W);
            return WAIT_FAILED_W;
        }
        if(pHandles[i]->dwInternalType == HMTYPE_SEMAPHORE) {
            PSEM_INFO pSemInfo  = (PSEM_INFO)pHandles[i]->hHMHandle;
            dprintf(("KERNEL32: HMWaitForMultipleObjects: handle %3i: ODIN-%08xh, OS/2-%08xh",
                     i, lphObjects[i], pSemInfo->hev));
        }
        else 
            dprintf(("KERNEL32: HMWaitForMultipleObjects: handle %3i: ODIN-%08xh, OS/2-%08xh",
                     i, lphObjects[i], pHandles[i]->hHMHandle));

    }
    j = 0;
    for(i=0;i<cObjects;i++) {
        if(pHandles[i]->dwInternalType == HMTYPE_SEMAPHORE) {
            PSEM_INFO pSemInfo  = (PSEM_INFO)pHandles[i]->hHMHandle;

            if(InterlockedDecrement(&pSemInfo->currentCount) >= 0) {
                if(!fWaitAll) {
                    return WAIT_OBJECT_0_W + i;
                }
            }
            else {
                pSemRec[j].hsemCur = (HSEM)pSemInfo->hev;
                pSemRec[j].ulUser = j;
                j++;
            }
        }
        else {
            pSemRec[j].hsemCur = (HSEM)pHandles[i]->hHMHandle;
            pSemRec[j].ulUser = j;
            j++;
        }
    }
    rc = DosCreateMuxWaitSem(NULL, &hmux, j, pSemRec, (fWaitAll) ? DCMW_WAIT_ALL : DCMW_WAIT_ANY);
    if(rc) {
        dprintf(("DosCreateMuxWaitSem failed with rc %d", rc));
        SetLastError(error2WinError(rc));
        return WAIT_FAILED_W;
    }
    ulUser = -1;
    rc = WinWaitMuxWaitSem(hmux, dwTimeout, &ulUser);
    DosCloseMuxWaitSem(hmux);
    if(rc && rc != ERROR_INTERRUPT && rc != ERROR_TIMEOUT && rc != ERROR_SEM_OWNER_DIED) {
        dprintf(("DosWaitMuxWaitSem %x failed with rc %d", hmux, rc));
        SetLastError(error2WinError(rc));
        return WAIT_FAILED_W;
    }
    SetLastError(ERROR_SUCCESS_W);
    if(rc == ERROR_INTERRUPT || rc == ERROR_SEM_OWNER_DIED) {
        //TODO: add index of handle that caused the error....
        dprintf(("WAIT_ABANDONED_W"));
        return WAIT_ABANDONED_W;
    }
    else 
    if(rc == ERROR_TIMEOUT) {
        dprintf(("WAIT_TIMEOUT_W"));
        return WAIT_TIMEOUT_W;
    }
    if(ulUser == -1) {
        dprintf(("WAIT_OBJECT_0_W+%d", cObjects));
        return WAIT_OBJECT_0_W + cObjects; //message waiting
    }
    dprintf(("WAIT_OBJECT_0_W+%d", ulUser));
    return WAIT_OBJECT_0_W + ulUser;
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
    return HMSemWaitForMultipleObjects(cObjects, lphObjects, fWaitAll, dwTimeout);
}

DWORD HMSemWaitForMultipleObjects(DWORD   cObjects,
                                  PHANDLE lphObjects,
                                  BOOL    fWaitAll,
                                  DWORD   dwTimeout)
{
    PHMHANDLEDATA *pHandles;
    HMUX hmux;
    PSEMRECORD pSemRec;
    int i, j;
    APIRET rc;
    ULONG  ulUser;

    dprintf(("KERNEL32: WaitForMultipleObjects %d %x %d %x",
              cObjects, lphObjects, fWaitAll, dwTimeout));

    pHandles = (PHMHANDLEDATA *)alloca(cObjects * sizeof(PHMHANDLEDATA));
    pSemRec  = (PSEMRECORD)alloca(cObjects * sizeof(SEMRECORD));
    if(pHandles == NULL || pSemRec == NULL) {
        dprintf(("ERROR: out of memory!!"));
        SetLastError(ERROR_OUTOFMEMORY_W);
        return WAIT_FAILED_W;
    }
    for(i=0;i<cObjects;i++) {
        pHandles[i] = HMQueryHandleData(lphObjects[i]);
        if(pHandles[i] == NULL) {
            dprintf(("ERROR: handle %x not recognized", lphObjects[i]));
        }
        if(!(pHandles[i]->dwAccess & EVENT_MODIFY_STATE_W) )
        {
            dprintf(("ERROR: Access denied (handle %x, index %d)!!", lphObjects[i], i));
            SetLastError(ERROR_INVALID_HANDLE_W);
            return WAIT_FAILED_W;
        }
        if(pHandles[i]->dwInternalType == HMTYPE_SEMAPHORE) {
            PSEM_INFO pSemInfo  = (PSEM_INFO)pHandles[i]->hHMHandle;
            dprintf(("KERNEL32: HMWaitForMultipleObjects: handle %3i: ODIN-%08xh, OS/2-%08xh",
                     i, lphObjects[i], pSemInfo->hev));
        }
        else 
            dprintf(("KERNEL32: HMWaitForMultipleObjects: handle %3i: ODIN-%08xh, OS/2-%08xh",
                     i, lphObjects[i], pHandles[i]->hHMHandle));

    }
    j = 0;
    for(i=0;i<cObjects;i++) {
        if(pHandles[i]->dwInternalType == HMTYPE_SEMAPHORE) {
            PSEM_INFO pSemInfo  = (PSEM_INFO)pHandles[i]->hHMHandle;

            if(InterlockedDecrement(&pSemInfo->currentCount) >= 0) {
                if(!fWaitAll) {
                    return WAIT_OBJECT_0_W + i;
                }
            }
            else {
                pSemRec[j].hsemCur = (HSEM)pSemInfo->hev;
                pSemRec[j].ulUser = j;
                j++;
            }
        }
        else {
            pSemRec[j].hsemCur = (HSEM)pHandles[i]->hHMHandle;
            pSemRec[j].ulUser = j;
            j++;
        }
    }
    rc = DosCreateMuxWaitSem(NULL, &hmux, j, pSemRec, (fWaitAll) ? DCMW_WAIT_ALL : DCMW_WAIT_ANY);
    if(rc) {
        dprintf(("DosCreateMuxWaitSem failed with rc %d", rc));
        SetLastError(error2WinError(rc));
        return WAIT_FAILED_W;
    }
    rc = DosWaitMuxWaitSem(hmux, dwTimeout, &ulUser);
    DosCloseMuxWaitSem(hmux);
    if(rc && rc != ERROR_INTERRUPT && rc != ERROR_TIMEOUT && rc != ERROR_SEM_OWNER_DIED) {
        dprintf(("DosWaitMuxWaitSem %x failed with rc %d", hmux, rc));
        SetLastError(error2WinError(rc));
        return WAIT_FAILED_W;
    }
    SetLastError(ERROR_SUCCESS_W);
    if(rc == ERROR_INTERRUPT || rc == ERROR_SEM_OWNER_DIED) {
        //TODO: add index of handle that caused the error....
        return WAIT_ABANDONED_W;
    }
    else 
    if(rc == ERROR_TIMEOUT) {
        return WAIT_TIMEOUT_W;
    }
    return WAIT_OBJECT_0_W + ulUser;
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
  if(rc && rc != ERROR_ALREADY_RESET) {
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

//******************************************************************************
//Replaces illegal characters in semaphore name (or else OS/2 will return 
//ERROR_INVALID_NAME
//******************************************************************************
void FixSemName(char *lpszSemaphoreName)
{
    while(TRUE) {
        switch(*lpszSemaphoreName) {
        case 0:
            return;
        case '?':
        case ':':
            *lpszSemaphoreName = '_';
            break;
        }
        lpszSemaphoreName++;
    }
}
//******************************************************************************
//******************************************************************************
