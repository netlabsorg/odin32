/* $Id: hmsemaphore.cpp,v 1.10 2003-02-04 11:29:00 sandervl Exp $ */

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
 * TODO: Use DosQueryEventSem to test the posted count against maximum count!!
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

#include <os2win.h>
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
    return (GetLastError());
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
    return (GetLastError());
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

//******************************************************************************
//******************************************************************************
