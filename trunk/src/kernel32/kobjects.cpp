/* $Id: kobjects.cpp,v 1.2 1999-07-06 15:48:48 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 compatibility file functions for OS/2
 * Copyright 1998 Sander van Leeuven
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1998 Knut St. Osmundsen
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/
#include <os2win.h>
#include "misc.h"
#include "handlemanager.h"



// REMARK: THIS IS IN PREPARATION FOR HANDLEMANAGER SUPPORT (PH) !!
//#define HMCreateEvent              O32_CreateEvent
//#define HMCreateMutex              O32_CreateMutex
//#define HMCreateSemaphore          O32_CreateSemaphore
//#define HMSetEvent                 O32_SetEvent
//#define HMReleaseMutex             O32_ReleaseMutex
//#define HMWaitForSingleObject      O32_WaitForSingleObject
//#define HMWaitForSingleObjectEx    O32_WaitForSingleObjectEx
//#define HMGetOverlappedResult      O32_GetOverlappedResult
//#define HMOpenEvent                O32_OpenEvent
//#define HMOpenMutex                O32_OpenMutex
//#define HMOpenSemaphore            O32_OpenSemaphore
//#define HMPulseEvent               O32_PulseEvent
//#define HMReleaseSemaphore         O32_ReleaseSemaphore
//#define HMResetEvent               O32_ResetEvent
//#define HMWaitForMultipleObjects   O32_WaitForMultipleObjects
//#define HMWaitForMultipleObjectsEx O32_WaitForMultipleObjectsEx
//#define HMFlushFileBuffers         O32_FlushFileBuffers
#define HMSetHandleCount           O32_SetHandleCount
#define HMGetHandleCount           O32_GetHandleCount
#define HMDuplicateHandle          O32_DuplicateHandle



/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1



/*****************************************************************************
 * Name      : BOOL WIN32API CreateEventA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateEventA(LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fManualReset,
                             BOOL                  fInitialState,
                             LPCTSTR               lpszEventName)
{
  dprintf(("KERNEL32: CreateEventA(%08xh, %08xh, %08xh, %s)\n",
           lpsa,
           fManualReset,
           fInitialState,
           lpszEventName));

  return(HMCreateEvent(lpsa,                         /* create event semaphore */
                       fManualReset,
                       fInitialState,
                       lpszEventName));
}


/*****************************************************************************
 * Name      : BOOL WIN32API CreateEventW
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
                             BOOL                  arg2,
                             BOOL                  arg3,
                             LPCWSTR               arg4)
{
  HANDLE rc;
  char  *astring;

  dprintf(("KERNEL32: CreateEventW(%08xh, %08xh, %08xh, %s)\n",
           arg1,
           arg2,
           arg3,
           arg4));

  astring = UnicodeToAsciiString((LPWSTR)arg4);

  rc = HMCreateEvent(arg1,
                    arg2,
                    arg3,
                    astring);

  FreeAsciiString(astring);

  return(rc);
}


/*****************************************************************************
 * Name      : BOOL WIN32API CreateMutexA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateMutexA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateMutexA(LPSECURITY_ATTRIBUTES lpsa,
                              BOOL                  fInitialOwner,
                              LPCTSTR               lpszMutexName)
{
  dprintf(("KERNEL32: CreateMutexA(%08xh,%08xh,%s)\n",
           lpsa,
           fInitialOwner,
           lpszMutexName));

  return(HMCreateMutex(lpsa,
                       fInitialOwner,
                       lpszMutexName));
}


/*****************************************************************************
 * Name      : BOOL WIN32API CreateMutexW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateMutexW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateMutexW(PSECURITY_ATTRIBUTES arg1,
                             BOOL                 arg2,
                             LPCWSTR              arg3)
{
  HANDLE rc;
  char  *astring;

  dprintf(("KERNEL32: CreateMutexW(%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3));

  astring = UnicodeToAsciiString((LPWSTR)arg3);
  rc = HMCreateMutex(arg1,
                     arg2,
                     astring);
  FreeAsciiString(astring);

  return(rc);
}


/*****************************************************************************
 * Name      : BOOL WIN32API ReleaseMutex
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ReleaseMutex
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API ReleaseMutex(HANDLE mutex)
{
  dprintf(("KERNEL32: ReleaseMutex%08xh)\n",
           mutex));

  return(HMReleaseMutex(mutex));
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetEvent
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in SetEvent
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API SetEvent(HANDLE hEvent)
{
  dprintf(("KERNEL32: SetEvent(%08xh)\n",
           hEvent));

  return(HMSetEvent(hEvent));
}


/*****************************************************************************
 * Name      : BOOL WIN32API WaitForSingleObject
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in WaitForSingleObject
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

DWORD WIN32API WaitForSingleObject(HANDLE hObject,
                                   DWORD  timeout)
{
  dprintf(("KERNEL32: WaitForSingleObject (%08xh, %08xh)\n",
           hObject,
           timeout));

  return(HMWaitForSingleObject(hObject,
                               timeout));
}


/*****************************************************************************
 * Name      : DWORD OS2WaitForSingleObjectEx
 * Purpose   : The WaitForSingleObjectEx function is an extended wait function
 *             that can be used to perform an alertable wait. This enables the
 *             function to return when the system queues an I/O completion
 *             routine to be executed by the calling thread. The function also
 *             returns when the specified object is in the signaled state or
 *             when the time-out interval elapses.
 * Parameters: HANDLE hObject     handle of object to wait for
 *             DWORD  dwTimeout   time-out interval in milliseconds
 *             BOOL   fAlertable  alertable wait flag
 * Variables :
 * Result    : 0xFFFFFFFF in case of error
 * Remark    : only really required for async I/O
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API WaitForSingleObjectEx(HANDLE hObject,
                                        DWORD        dwTimeout,
                                        BOOL         fAlertable)
{
  dprintf(("Kernel32: WaitForSingleObjectEx(%08xh,%08xh,%08xh) not implemented correctly.\n",
           hObject,
           dwTimeout,
           fAlertable));

  return(HMWaitForSingleObjectEx(hObject,
                                 dwTimeout,
                                 fAlertable));
}


/*****************************************************************************
 * Name      : BOOL WIN32API FlushFileBuffers
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in FlushFileBuffers
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API FlushFileBuffers(HANDLE hFile)
{
  dprintf(("KERNEL32: FlushFileBuffers(%08xh)\n",
           hFile));

  return(HMFlushFileBuffers(hFile));
}


/*****************************************************************************
 * Name      : UINT WIN32API SetHandleCount
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in SetHandleCount
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

UINT WIN32API SetHandleCount(UINT cHandles)
{
  dprintf(("KERNEL32: SetHandleCount(%08xh)\n",
           cHandles));

  return(HMSetHandleCount(cHandles));
}


/*****************************************************************************
 * Name      : BOOL WIN32API DuplicateHandle
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in DuplicateHandle
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

//******************************************************************************
//SvL: 24-6-'97 - Added
//SvL: 28-6-'97: Fix for timeSetEvent in Red Alert Map Editor
//******************************************************************************
BOOL WIN32API DuplicateHandle(HANDLE  srcprocess,
                              HANDLE  srchandle,
                              HANDLE  destprocess,
                              PHANDLE desthandle,
                              DWORD   arg5,
                              BOOL    arg6,
                              DWORD   arg7)
{
  BOOL rc;

  dprintf(("KERNEL32: DuplicateHandle(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           srcprocess,
           srchandle,
           destprocess,
           desthandle,
           arg5,
           arg6,
           arg7));

  rc = HMDuplicateHandle(srcprocess,
                         srchandle,
                         destprocess,
                         desthandle,
                         arg5,
                         arg6,
                         arg7);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL WIN32API CreateSemaphoreA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateSemaphoreA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateSemaphoreA(LPSECURITY_ATTRIBUTES arg1,
                                 LONG                  arg2,
                                 LONG                  arg3,
                                 LPCSTR                arg4)
{
  dprintf(("KERNEL32: CreateSemaphoreA(%08xh,%08xh,%08xh,%s)\n",
           arg1,
           arg2,
           arg3,
           arg4));

  return HMCreateSemaphore(arg1,
                           arg2,
                           arg3,
                           (LPSTR)arg4);
}


/*****************************************************************************
 * Name      : BOOL WIN32API CreateSemaphoreW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateSemaphoreW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateSemaphoreW(PSECURITY_ATTRIBUTES arg1,
                                 LONG                 arg2,
                                 LONG                 arg3,
                                 LPCWSTR              arg4)
{
  HANDLE rc;
  char   *astring;

  dprintf(("KERNEL32: CreateSemaphoreW(%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3,
           arg4));

  astring = UnicodeToAsciiString((LPWSTR)arg4);
  rc = HMCreateSemaphore(arg1,
                         arg2,
                         arg3,
                         astring);
  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetOverlappedResult
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in GetOverlappedResult
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API GetOverlappedResult(HANDLE       arg1,
                                  LPOVERLAPPED arg2,
                                  LPDWORD      arg3,
                                  BOOL         arg4)
{
  dprintf(("KERNEL32: GetOverlappedResult (%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3,
           arg4));

  return HMGetOverlappedResult(arg1,
                               arg2,
                               arg3,
                               arg4);
}


/*****************************************************************************
 * Name      : BOOL WIN32API OpenEventA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenEventA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API OpenEventA(DWORD  arg1,
                           BOOL   arg2,
                           LPCSTR arg3)
{
  dprintf(("KERNEL32: OpenEventA(%08xh,%08xh,%s)\n",
           arg1,
           arg2,
           arg3));

  return HMOpenEvent(arg1,
                     arg2,
                     arg3);
}


/*****************************************************************************
 * Name      : BOOL WIN32API OpenEventW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenEventW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API OpenEventW(DWORD  dwDesiredAccess,
                           BOOL    bInheritHandle,
                           LPCWSTR lpName)
{
  char  *asciiname;
  HANDLE rc;

  dprintf(("KERNEL32: OpenEventW(%08xh,%08xh,%08xh\n",
           dwDesiredAccess,
           bInheritHandle,
           lpName));

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);
  rc = HMOpenEvent(dwDesiredAccess,
                   bInheritHandle,
                   asciiname);
  FreeAsciiString(asciiname);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL WIN32API OpenMutexA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenMutexA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API OpenMutexA(DWORD  arg1,
                           BOOL   arg2,
                           LPCSTR arg3)
{
  dprintf(("KERNEL32: OpenMutexA(%08xh,%08xh,%s)\n",
           arg1,
           arg2,
           arg3));

  return HMOpenMutex(arg1,
                     arg2,
                     arg3);
}


/*****************************************************************************
 * Name      : BOOL WIN32API OpenMutexW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenMutexW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API OpenMutexW(DWORD   dwDesiredAccess,
                           BOOL    bInheritHandle,
                           LPCWSTR lpName)
{
  char  *asciiname;
  HANDLE rc;

  dprintf(("KERNEL32: OpenMutexW(%08xh,%08xh,%08xh)\n",
           dwDesiredAccess,
           bInheritHandle,
           lpName));

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);
  rc = HMOpenMutex(dwDesiredAccess,
                   bInheritHandle,
                   asciiname);
  FreeAsciiString(asciiname);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL WIN32API OpenSemaphoreA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenSemaphoreA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API OpenSemaphoreA(DWORD   arg1,
                               BOOL    arg2,
                               LPCSTR  arg3)
{
  dprintf(("KERNEL32: OpenSemaphoreA(%08xh,%08xh,%s)\n",
           arg1,
           arg2,
           arg3));

  return HMOpenSemaphore(arg1,
                         arg2,
                         arg3);
}


/*****************************************************************************
 * Name      : BOOL WIN32API OpenSemaphoreW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenSemaphoreW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API OpenSemaphoreW(DWORD   dwDesiredAccess,
                               BOOL    bInheritHandle,
                               LPCWSTR lpName)
{
  char  *asciiname;
  HANDLE rc;

  dprintf(("KERNEL32: OpenSemaphoreW(%08xh,%08xh,%08xh)\n",
           dwDesiredAccess,
           bInheritHandle,
           lpName));

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);
  rc = HMOpenSemaphore(dwDesiredAccess,
                       bInheritHandle,
                       asciiname);
  FreeAsciiString(asciiname);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL WIN32API PulseEvent
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in PulseEvent
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API PulseEvent(HANDLE arg1)
{
  dprintf(("KERNEL32: PulseEvent(%08xh)\n",
           arg1));

  return HMPulseEvent(arg1);
}


/*****************************************************************************
 * Name      : BOOL WIN32API ReleaseSemaphore
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ReleaseSemaphore
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API ReleaseSemaphore(HANDLE arg1,
                               LONG   arg2,
                               PLONG  arg3)
{
  dprintf(("KERNEL32: ReleaseSemaphore(%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3));

  return HMReleaseSemaphore(arg1,
                            arg2,
                            arg3);
}


/*****************************************************************************
 * Name      : BOOL WIN32API ResetEvent
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ResetEvent
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

BOOL WIN32API ResetEvent(HANDLE arg1)
{
  dprintf(("KERNEL32: ResetEvent(%08xh)\n",
           arg1));

  return HMResetEvent(arg1);
}


/*****************************************************************************
 * Name      : BOOL WIN32API WaitForMultipleObjects
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in WaitForMultipleObjects
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

DWORD WIN32API WaitForMultipleObjects(DWORD        arg1,
                                      const HANDLE *arg2,
                                      BOOL         arg3,
                                      DWORD        arg4)
{
  dprintf(("KERNEL32:  OS2WaitForMultipleObjects(%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3,
           arg4));

  return HMWaitForMultipleObjects(arg1,
                                  (PHANDLE)arg2,
                                  arg3,
                                  arg4);
}


/*****************************************************************************
 * Name      : DWORD OS2WaitForMultipleObjectsEx
 * Purpose   : The WaitForMultipleObjectsEx function is an extended wait
 *             function that can be used to perform an alertable wait. This
 *             enables the function to return when the system queues an I/O
 *             completion routine to be executed by the calling thread. The
 *             function also returns either when any one or when all of the
 *             specified objects are in the signaled state, or when the
 *             time-out interval elapses.
 * Parameters: DWORD  cObjects    number of handles in handle array
 *             HANDLE *lphObjects address of object-handle array
 *             BOOL   fWaitAll    wait flag
 *             DWORD  dwTimeout   time-out interval in milliseconds
 *             BOOL   fAlertable  alertable wait flag
 * Variables :
 * Result    : 0xFFFFFFFF in case of error
 * Remark    : only really required for async I/O
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API WaitForMultipleObjectsEx(DWORD        cObjects,
                                           CONST HANDLE *lphObjects,
                                           BOOL         fWaitAll,
                                           DWORD        dwTimeout,
                                           BOOL         fAlertable)
{
  dprintf(("Kernel32: WaitForMultipleObjectsEx(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented correctly.\n",
           cObjects,
           lphObjects,
           fWaitAll,
           dwTimeout,
           fAlertable));

  return(HMWaitForMultipleObjectsEx(cObjects,
                                    (PHANDLE)lphObjects,
                                    fWaitAll,
                                    dwTimeout,
                                    fAlertable));
}


/*****************************************************************************
 * Name      : HANDLE WIN32API ConvertToGlobalHandle
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ConvertToGlobalHandle
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

HANDLE WIN32API ConvertToGlobalHandle(HANDLE hHandle)
//HANDLE WIN32API ConvertToGlobalHandle(HANDLE hHandle,
//                                      BOOL   fInherit)
{
  dprintf(("KERNEL32: ConvertToGlobalHandle(%08xh)\n",
           hHandle));

  return (hHandle);
}

