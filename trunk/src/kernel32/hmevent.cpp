/* $Id: hmevent.cpp,v 1.4 2001-03-19 19:27:13 sandervl Exp $ */

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
#include "HMEvent.h"

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
    return (O32_GetLastError());
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
    return (O32_GetLastError());
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
//testestest
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

