/* $Id: hmmutex.cpp,v 1.4 2001-06-19 10:50:25 sandervl Exp $ */

/*
 * Win32 Mutex Semaphore implementation
 *
 * TODO: Inheritance
 * TODO: No inheritance when CreateMutex is called for existing named event semaphore?
 *       (see HMCreateMutex in handlemanager.cpp)
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
#include <os2wrap.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#else
#include <os2win.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "misc.h"
#include "oslibdos.h"

#include "HandleManager.H"
#include "HMMutex.h"

#define DBG_LOCALLOG	DBG_hmmutex
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

DWORD HMDeviceMutexClass::CreateMutex(PHMHANDLEDATA         pHMHandleData,
                                      LPSECURITY_ATTRIBUTES lpsa,
                                      BOOL                  fInitialOwner,
                                      LPCTSTR               lpszMutexName)
{
#ifdef USE_OS2SEMAPHORES
  APIRET rc;
  HMTX htmx;
  char szSemName[CCHMAXPATH];

  dprintf(("KERNEL32: HandleManager::Mutex::CreateMutex(%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           fInitialOwner,
           lpszMutexName));

  if(lpszMutexName) {
      strcpy(szSemName, "\\SEM32\\");
      strcat(szSemName, lpszMutexName);
      lpszMutexName = szSemName;
  }
  rc = DosCreateMutexSem(lpszMutexName, &htmx, 0, fInitialOwner);

  if(rc) {
      dprintf(("DosCreateMutexSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      return error2WinError(rc);
  }
  pHMHandleData->dwAccess  = MUTEX_ALL_ACCESS_W;
  pHMHandleData->hHMHandle = htmx;
  return ERROR_SUCCESS_W;
#else
  HANDLE hOpen32;

  dprintf(("KERNEL32: HandleManager::Mutex::CreateMutex(%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           fInitialOwner,
           lpszMutexName));

  hOpen32 = O32_CreateMutex(lpsa,              // call Open32
                            fInitialOwner,
                            lpszMutexName);

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

DWORD HMDeviceMutexClass::OpenMutex(PHMHANDLEDATA         pHMHandleData,
                                    BOOL                  fInheritHandle,
                                    LPCTSTR               lpszMutexName)
{
#ifdef USE_OS2SEMAPHORES
  HMTX   hmtx;
  APIRET rc;
  char szSemName[CCHMAXPATH];

  dprintf(("KERNEL32: HandleManager::Mutex::OpenMutex(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszMutexName));

  if(lpszMutexName == NULL) {
      pHMHandleData->hHMHandle = 0;
      return ERROR_INVALID_PARAMETER_W;
  }

  strcpy(szSemName, "\\SEM32\\");
  strcat(szSemName, lpszMutexName);
  rc = DosOpenMutexSem(szSemName, &hmtx);
  if(rc) {
      dprintf(("DosOpenMutexSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      return error2WinError(rc);
  }
  pHMHandleData->hHMHandle = hmtx;
  return ERROR_SUCCESS_W;
#else
  HANDLE hOpen32;

  dprintf(("KERNEL32: HandleManager::Mutex::OpenMutex(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszMutexName));

  hOpen32 = O32_OpenMutex(pHMHandleData->dwAccess,              // call Open32
                          fInheritHandle,
                          lpszMutexName);

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
 * Name      : HMDeviceMutexClass::CloseHandle
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
BOOL HMDeviceMutexClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  APIRET rc;

  if(pHMHandleData->hHMHandle) {
      rc = DosCloseMutexSem((HEV)pHMHandleData->hHMHandle);
      if(rc) {
          dprintf(("DosCloseMutexSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
          SetLastError(error2WinError(rc));
          return FALSE;
      }
  }
  return TRUE;
}
#endif

/*****************************************************************************
 * Name      : HMDeviceMutexClass::DuplicateHandle
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
BOOL HMDeviceMutexClass::DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions)
{
  APIRET rc; 
  HMTX hmtx;
  
  dprintf(("KERNEL32:HandleManager::DuplicateHandle %s(%08x,%08x,%08x,%08x,%08x) - NOT IMPLEMENTED!!!!!!!!\n",
           lpHMDeviceName,
           pHMHandleData,
           srcprocess, pHMSrcHandle, destprocess, desthandle));

  if(srcprocess != destprocess) {
      DebugInt3();
      SetLastError(ERROR_ACCESS_DENIED_W);
      return FALSE;
  }
  hmtx = (HMTX)pHMSrcHandle->hHMHandle;
  rc = DosOpenMutexSem(NULL, &hmtx);
  if(rc) {
      dprintf(("DosOpenMutexSem %x failed with rc %d", pHMSrcHandle->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      SetLastError(error2WinError(rc));
      return FALSE;
  }
  pHMHandleData->dwAccess  = fdwAccess;
  pHMHandleData->hHMHandle = hmtx;
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
#endif

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

BOOL HMDeviceMutexClass::ReleaseMutex(PHMHANDLEDATA pHMHandleData)
{
#ifdef USE_OS2SEMAPHORES
  APIRET rc;
  
  dprintf(("KERNEL32: HandleManager::Mutex::ReleaseMutex(%08xh)\n",
           pHMHandleData->hHMHandle));

  rc = DosReleaseMutexSem(pHMHandleData->hHMHandle);
  if(rc) {
      dprintf(("DosReleaseMutexSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      SetLastError(error2WinError(rc));
      return FALSE;
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
#else
  dprintf(("KERNEL32: HandleManager::Mutex::ReleaseMutex(%08xh)\n",
           pHMHandleData->hHMHandle));

  return (O32_ReleaseMutex(pHMHandleData->hHMHandle));
#endif
}

