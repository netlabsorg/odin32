/* $Id: hmmutex.cpp,v 1.8 2003-02-04 11:28:58 sandervl Exp $ */

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

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "misc.h"
#include "oslibdos.h"

#include "HandleManager.H"
#include "HMMutex.h"
#include "HMSemaphore.h"

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
    return (GetLastError());
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
    return (GetLastError());
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

BOOL HMDeviceMutexClass::ReleaseMutex(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::Mutex::ReleaseMutex(%08xh)\n",
           pHMHandleData->hHMHandle));

  return (O32_ReleaseMutex(pHMHandleData->hHMHandle));
}

