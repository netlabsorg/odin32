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

#include "hmhandle.h"


#define DBG_LOCALLOG	DBG_hmevent
#include "dbglocal.h"


/*****************************************************************************
 * Name      : HANDLE  HMCreateEvent
 * Purpose   : Wrapper for the CreateEvent() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE WIN32API CreateEventA(LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  bManualReset,
                             BOOL                  bInitialState,
                             LPCTSTR               lpName)
{
  PHMHANDLE pHandle;
  DWORD     rc;                                     /* API return code */


  if(lpName) { //check if shared event semaphore already exists
      dprintf(("Event semaphore name %s", lpName));
      //TODO: No inheritance??
      HANDLE handle = OpenEventA(EVENT_ALL_ACCESS, FALSE, lpName);
      if(handle) {
          dprintf(("CreateEvent: return handle of existing event semaphore %x", handle));
          SetLastError(ERROR_ALREADY_EXISTS);
          return handle;
      }
  }

  pHandle = HMHandleGetFreePtr(HMTYPE_EVENTSEM);                         /* get free handle */
  if (pHandle == NULL)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }

  /* call the device handler */
  rc = pHandle->pDeviceHandler->CreateEvent(&pHandle->hmHandleData,
                                   lpsa,
                                   bManualReset,
                                   bInitialState,
                                   lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    HMHandleFree(pHandle->hmHandleData.hWin32Handle);
    SetLastError(rc);         
    return 0;                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return pHandle->hmHandleData.hWin32Handle;                                   /* return valid handle */
}

/*****************************************************************************
 * Name      : BOOL CreateEventW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateEventA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateEventW(LPSECURITY_ATTRIBUTES arg1,
                             BOOL arg2, BOOL arg3,
                             LPCWSTR arg4)
{
  HANDLE rc;
  char  *astring;

  if (arg4 != NULL) // support for unnamed semaphores
    astring = UnicodeToAsciiString((LPWSTR)arg4);
  else
    astring = NULL;

  rc = CreateEventA(arg1,
                    arg2,
                    arg3,
                    astring);

  if (astring != NULL)
    FreeAsciiString(astring);

  return(rc);
}

/*****************************************************************************
 * Name      : HANDLE  HMOpenEvent
 * Purpose   : Wrapper for the OpenEvent() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE WIN32API OpenEventA(DWORD   fdwAccess,
                           BOOL    fInherit,
                           LPCTSTR lpName)
{
  PHMHANDLE pHandle;
  DWORD     rc;                                     /* API return code */

  if(lpName) {
      dprintf(("Event semaphore name %s", lpName));
  }

  pHandle = HMHandleGetFreePtr(HMTYPE_EVENTSEM);                         /* get free handle */
  if (pHandle == NULL)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }
  pHandle->hmHandleData.dwAccess   = fdwAccess;
                                                  /* call the device handler */
  rc = pHandle->pDeviceHandler->OpenEvent(&pHandle->hmHandleData,
                                 fInherit,
                                 lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    HMHandleFree(pHandle->hmHandleData.hWin32Handle);
    SetLastError(rc);
    return 0;                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return pHandle->hmHandleData.hWin32Handle;                                   /* return valid handle */
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
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL WIN32API SetEvent(HANDLE hEvent)
{
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hEvent);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  dwResult = pHMHandle->pDeviceHandler->SetEvent(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
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
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL WIN32API PulseEvent(HANDLE hEvent)
{
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hEvent);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  dwResult = pHMHandle->pDeviceHandler->PulseEvent(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
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
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL WIN32API ResetEvent(HANDLE hEvent)
{
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hEvent);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  dwResult = pHMHandle->pDeviceHandler->ResetEvent(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
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

  dprintf(("KERNEL32: HandleManager::Semaphore::CreateEvent hOpen32 = %p, pHMHandleData->hHMHandle = %p\n", hOpen32, pHMHandleData->hHMHandle));

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

