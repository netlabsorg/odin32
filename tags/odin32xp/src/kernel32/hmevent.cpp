/* $Id: hmevent.cpp,v 1.9 2003-02-04 11:28:57 sandervl Exp $ */

/*
 * Win32 Event Semaphore implementation
 *
 * TODO: Inheritance
 * TODO: Does DCE_POSTONE work in Warp 3 or 4 with no FP applied?
 * TODO: No inheritance when CreateEvent is called for existing named event semaphore?
 *       (see HMCreateEvent in handlemanager.cpp)
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

#include "HandleManager.H"
#include "HMEvent.h"
#include "HMSemaphore.h"
#include "oslibdos.h"

#define DBG_LOCALLOG	DBG_hmevent
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

DWORD HMDeviceEventClass::CreateEvent(PHMHANDLEDATA         pHMHandleData,
                                      LPSECURITY_ATTRIBUTES lpsa,
                                      BOOL                  fManualReset,
                                      BOOL                  fInitialState,
                                      LPCTSTR               lpszEventName)
{
  HANDLE hOpen32;

  dprintf(("KERNEL32: HandleManager::Event::CreateEvent(%08xh,%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           fManualReset,
           fInitialState,
           lpszEventName));

  hOpen32 = O32_CreateEvent(lpsa,              // call Open32
                            fManualReset,
                            fInitialState,
                            lpszEventName);

  if (0 != hOpen32)                            // check success
  {
    pHMHandleData->hHMHandle = hOpen32;        // save handle
    return (NO_ERROR);
  }
  else
    return (GetLastError());
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

DWORD HMDeviceEventClass::OpenEvent(PHMHANDLEDATA         pHMHandleData,
                                    BOOL                  fInheritHandle,
                                    LPCTSTR               lpszEventName)
{
  HANDLE hOpen32;

  dprintf(("KERNEL32: HandleManager::Event::OpenEvent(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszEventName));

  hOpen32 = O32_OpenEvent(pHMHandleData->dwAccess,              // call Open32
                          fInheritHandle,
                          lpszEventName);

  if (0 != hOpen32)                            // check success
  {
    pHMHandleData->hHMHandle = hOpen32;        // save handle
    return (NO_ERROR);
  }
  else
    return (GetLastError());
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

BOOL HMDeviceEventClass::SetEvent(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HandleManager::Event::SetEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  return (O32_SetEvent(pHMHandleData->hHMHandle));
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

BOOL HMDeviceEventClass::PulseEvent(PHMHANDLEDATA pHMHandleData)
{
  dprintf2(("KERNEL32: HandleManager::Event::PulseEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  return (O32_PulseEvent(pHMHandleData->hHMHandle));
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

BOOL HMDeviceEventClass::ResetEvent(PHMHANDLEDATA pHMHandleData)
{
  dprintf2(("KERNEL32: HandleManager::Event::ResetEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  return (O32_ResetEvent(pHMHandleData->hHMHandle));
}

