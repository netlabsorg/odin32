/* $Id: hmsemaphore.cpp,v 1.3 2000-02-16 14:24:00 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
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

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "misc.h"

#include "HandleManager.H"
#include "HMSemaphore.h"

#define DBG_LOCALLOG	DBG_hmsemaphore
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

BOOL HMDeviceSemaphoreClass::ReleaseSemaphore(PHMHANDLEDATA pHMHandleData,
                                              LONG          cReleaseCount,
                                              LPLONG        lpPreviousCount)
{
  dprintf(("KERNEL32: HandleManager::Semaphore::ReleaseSemaphore(%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           cReleaseCount,
           lpPreviousCount));

  return (O32_ReleaseSemaphore(pHMHandleData->hHMHandle,
                               cReleaseCount,
                               lpPreviousCount));
}

