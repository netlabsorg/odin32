/* $Id: hmevent.cpp,v 1.5 2001-06-19 10:50:24 sandervl Exp $ */

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

#include "HandleManager.H"
#include "HMEvent.h"
#include "oslibdos.h"

#define DBG_LOCALLOG	DBG_hmevent
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
#ifdef USE_OS2SEMAPHORES
  APIRET rc;
  HEV hev;
  char szSemName[CCHMAXPATH];

  dprintf(("KERNEL32: HandleManager::Event::CreateEvent(%08xh,%08xh,%08xh,%08xh,%s)\n",
           pHMHandleData,
           lpsa,
           fManualReset,
           fInitialState,
           lpszEventName));

  if(lpszEventName) {
      strcpy(szSemName, "\\SEM32\\");
      strcat(szSemName, lpszEventName);
      lpszEventName = szSemName;
  }
  //Manual reset means all threads waiting on the event semaphore will be
  //unblocked and the app must manually reset the event semaphore
  //Automatic reset -> only one waiting thread unblocked & state reset
  rc = DosCreateEventSem(lpszEventName, &hev, (fManualReset) ? 0 : DCE_POSTONE, fInitialState);

  if(rc) {
      dprintf(("DosCreateEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      return error2WinError(rc);
  }
  pHMHandleData->dwAccess  = EVENT_ALL_ACCESS_W;
  pHMHandleData->dwFlags   = fManualReset;
  pHMHandleData->hHMHandle = hev;
  return ERROR_SUCCESS_W;
#else
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
#endif
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
#ifdef USE_OS2SEMAPHORES
  HEV    hev;
  APIRET rc;
  char szSemName[CCHMAXPATH];

  dprintf(("KERNEL32: HandleManager::Event::OpenEvent(%08xh,%08xh,%s)\n",
           pHMHandleData,
           fInheritHandle,
           lpszEventName));

  if(lpszEventName == NULL) {
      pHMHandleData->hHMHandle = 0;
      return ERROR_INVALID_PARAMETER_W;
  }

  strcpy(szSemName, "\\SEM32\\");
  strcat(szSemName, lpszEventName);
  rc = DosOpenEventSem(szSemName, &hev);
  if(rc) {
      dprintf(("DosOpenEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      return error2WinError(rc);
  }
  pHMHandleData->hHMHandle = hev;
  return ERROR_SUCCESS_W;
#else
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
BOOL HMDeviceEventClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  APIRET rc;

  if(pHMHandleData->hHMHandle) {
      rc = DosCloseEventSem((HEV)pHMHandleData->hHMHandle);
      if(rc) {
          dprintf(("DosCloseEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
          SetLastError(error2WinError(rc));
          return FALSE;
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
BOOL HMDeviceEventClass::DuplicateHandle(PHMHANDLEDATA pHMHandleData, HANDLE  srcprocess,
                               PHMHANDLEDATA pHMSrcHandle,
                               HANDLE  destprocess,
                               PHANDLE desthandle,
                               DWORD   fdwAccess,
                               BOOL    fInherit,
                               DWORD   fdwOptions,
                               DWORD   fdwOdinOptions)
{
  APIRET rc; 
  HEV hev;
  
  dprintf(("KERNEL32:HandleManager::DuplicateHandle %s(%08x,%08x,%08x,%08x,%08x) - NOT IMPLEMENTED!!!!!!!!\n",
           lpHMDeviceName,
           pHMHandleData,
           srcprocess, pHMSrcHandle, destprocess, desthandle));

  if(srcprocess != destprocess) {
      DebugInt3();
      SetLastError(ERROR_ACCESS_DENIED_W);
      return FALSE;
  }
  hev = (HEV)pHMSrcHandle->hHMHandle;
  rc = DosOpenEventSem(NULL, &hev);
  if(rc) {
      dprintf(("DosOpenEventSem %x failed with rc %d", pHMSrcHandle->hHMHandle, rc));
      pHMHandleData->hHMHandle = 0;
      SetLastError(error2WinError(rc));
      return FALSE;
  }
  pHMHandleData->dwAccess  = fdwAccess;
  pHMHandleData->dwFlags   = pHMSrcHandle->dwFlags; //fManualReset
  pHMHandleData->hHMHandle = hev;
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
#endif

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
#ifdef USE_OS2SEMAPHORES
  APIRET rc;

  dprintf(("KERNEL32: HandleManager::Event::SetEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  if(!(pHMHandleData->dwAccess & EVENT_MODIFY_STATE_W) )
  {
      dprintf(("ERROR: Access denied!!"));
      SetLastError(ERROR_ACCESS_DENIED_W);
      return FALSE;
  }

  rc = DosPostEventSem(pHMHandleData->hHMHandle);
  if(rc) {
      dprintf(("DosPostEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      SetLastError(error2WinError(rc));
      return FALSE;
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
#else
  dprintf(("KERNEL32: HandleManager::Event::SetEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  return (O32_SetEvent(pHMHandleData->hHMHandle));
#endif
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
#ifdef USE_OS2SEMAPHORES
  APIRET rc;
  ULONG count;  

  dprintf2(("KERNEL32: HandleManager::Event::PulseEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  if(!(pHMHandleData->dwAccess & EVENT_MODIFY_STATE_W) )
  {
      dprintf(("ERROR: Access denied!!"));
      SetLastError(ERROR_ACCESS_DENIED_W);
      return FALSE;
  }

  rc = DosPostEventSem(pHMHandleData->hHMHandle);
  if(rc) {
      dprintf(("DosPostEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      SetLastError(error2WinError(rc));
      return FALSE;
  }
  if(pHMHandleData->dwFlags == TRUE) {//fManualReset
      rc = DosResetEventSem(pHMHandleData->hHMHandle, &count);
      if(rc) {
          dprintf(("DosResetEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
          SetLastError(error2WinError(rc));
          return FALSE;
      }
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
#else
  dprintf2(("KERNEL32: HandleManager::Event::PulseEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  return (O32_PulseEvent(pHMHandleData->hHMHandle));
#endif
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
#ifdef USE_OS2SEMAPHORES
  APIRET rc;
  ULONG count;  
  
  dprintf2(("KERNEL32: HandleManager::Event::ResetEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  if(!(pHMHandleData->dwAccess & EVENT_MODIFY_STATE_W) )
  {
      dprintf(("ERROR: Access denied!!"));
      SetLastError(ERROR_ACCESS_DENIED_W);
      return FALSE;
  }

  rc = DosResetEventSem(pHMHandleData->hHMHandle, &count);
  if(rc) {
      dprintf(("DosResetEventSem %x failed with rc %d", pHMHandleData->hHMHandle, rc));
      SetLastError(error2WinError(rc));
      return FALSE;
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
#else
  dprintf2(("KERNEL32: HandleManager::Event::ResetEvent(%08xh)\n",
            pHMHandleData->hHMHandle));

  return (O32_ResetEvent(pHMHandleData->hHMHandle));
#endif
}

