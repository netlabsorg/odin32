/* $Id: kobjects.cpp,v 1.13 2001-04-05 05:54:51 sandervl Exp $ */

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

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <misc.h>
#include <unicode.h>
#include <handlemanager.h>

#define DBG_LOCALLOG	DBG_kobjects
#include "dbglocal.h"


ODINDEBUGCHANNEL(KERNEL32-KOBJECTS)


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
//#define HMDuplicateHandle          O32_DuplicateHandle



/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1



/*****************************************************************************
 * Name      : BOOL CreateEventA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

ODINFUNCTION4(HANDLE, CreateEventA,
              LPSECURITY_ATTRIBUTES, lpsa,
              BOOL, fManualReset,
              BOOL, fInitialState,
              LPCTSTR, lpszEventName)
{
  return(HMCreateEvent(lpsa,                         /* create event semaphore */
                       fManualReset,
                       fInitialState,
                       lpszEventName));
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

ODINFUNCTION4(HANDLE, CreateEventW,
              LPSECURITY_ATTRIBUTES, arg1,
              BOOL, arg2,
              BOOL, arg3,
              LPCWSTR, arg4)
{
  HANDLE rc;
  char  *astring;

  if (arg4 != NULL) // support for unnamed semaphores
    astring = UnicodeToAsciiString((LPWSTR)arg4);
  else
    astring = NULL;

  dprintf(("KERNEL32: CreateEventW(%s)\n",
           astring));

  rc = HMCreateEvent(arg1,
                    arg2,
                    arg3,
                    astring);

  if (astring != NULL)
    FreeAsciiString(astring);

  return(rc);
}


/*****************************************************************************
 * Name      : BOOL CreateMutexA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateMutexA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE, CreateMutexA,
              LPSECURITY_ATTRIBUTES, lpsa,
              BOOL, fInitialOwner,
              LPCTSTR, lpszMutexName)
{
  dprintf(("KERNEL32: CreateMutexA(%s)\n",
           lpszMutexName));

  return(HMCreateMutex(lpsa,
                       fInitialOwner,
                       lpszMutexName));
}


/*****************************************************************************
 * Name      : BOOL CreateMutexW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateMutexW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE, CreateMutexW,
              PSECURITY_ATTRIBUTES, arg1,
              BOOL, arg2,
              LPCWSTR, arg3)
{
  HANDLE rc;
  char  *astring;

  if (arg3 != NULL) // support for unnamed mutexes
    astring = UnicodeToAsciiString((LPWSTR)arg3);
  else
    astring = NULL;

  dprintf(("KERNEL32: CreateMutexW(%s)\n",
           astring));

  rc = HMCreateMutex(arg1,
                     arg2,
                     astring);

  if (astring != NULL)
    FreeAsciiString(astring);

  return(rc);
}


/*****************************************************************************
 * Name      : BOOL ReleaseMutex
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ReleaseMutex
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL, ReleaseMutex,
              HANDLE, mutex)
{
  return(HMReleaseMutex(mutex));
}


/*****************************************************************************
 * Name      : BOOL SetEvent
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in SetEvent
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL, SetEvent,
              HANDLE, hEvent)
{
  return(HMSetEvent(hEvent));
}


/*****************************************************************************
 * Name      : BOOL WaitForSingleObject
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in WaitForSingleObject
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION2(DWORD, WaitForSingleObject,
              HANDLE, hObject,
              DWORD, timeout)
{
  return(HMWaitForSingleObject(hObject,
                               timeout));
}


/*****************************************************************************
 * Name      : DWORD WaitForSingleObjectEx
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

ODINFUNCTION3(DWORD, WaitForSingleObjectEx,
              HANDLE, hObject,
              DWORD, dwTimeout,
              BOOL, fAlertable)
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
 * Name      : BOOL FlushFileBuffers
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in FlushFileBuffers
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL, FlushFileBuffers,
              HANDLE, hFile)
{
  return(HMFlushFileBuffers(hFile));
}


/*****************************************************************************
 * Name      : UINT SetHandleCount
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in SetHandleCount
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION1(UINT, SetHandleCount,
              UINT, cHandles)
{
  //SvL: Has no effect in NT; also ignore it
  return cHandles;
}


/*****************************************************************************
 * Name      : BOOL DuplicateHandle
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
ODINFUNCTION7(BOOL, DuplicateHandle,
              HANDLE, srcprocess,
              HANDLE, srchandle,
              HANDLE, destprocess,
              PHANDLE, desthandle,
              DWORD, arg5,
              BOOL, arg6,
              DWORD, arg7)
{
  BOOL rc;

  rc = HMDuplicateHandle(srcprocess,
                         srchandle,
                         destprocess,
                         desthandle,
                         arg5,
                         arg6,
                         arg7);
  //@@@PH: (temporary) fix for non-HandleManager handles
  if (rc == FALSE)
    rc = O32_DuplicateHandle(srcprocess,
                             srchandle,
                             destprocess,
                             desthandle,
                             arg5,
                             arg6,
                             arg7);

  return(rc);
}


/*****************************************************************************
 * Name      : BOOL CreateSemaphoreA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateSemaphoreA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION4(HANDLE, CreateSemaphoreA,
              LPSECURITY_ATTRIBUTES, arg1,
              LONG, arg2,
              LONG, arg3,
              LPCSTR, arg4)
{
  return HMCreateSemaphore(arg1,
                           arg2,
                           arg3,
                           (LPSTR)arg4);
}


/*****************************************************************************
 * Name      : BOOL CreateSemaphoreW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in CreateSemaphoreW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION4(HANDLE, CreateSemaphoreW,
              PSECURITY_ATTRIBUTES, arg1,
              LONG, arg2,
              LONG, arg3,
              LPCWSTR, arg4)
{
  HANDLE rc;
  char   *astring;

  if (arg4 != NULL) // support for unnamed semaphores
    astring = UnicodeToAsciiString((LPWSTR)arg4);
  else
    astring = NULL;

  dprintf(("KERNEL32: CreateSemaphoreW(%s)\n",
           astring));

  rc = HMCreateSemaphore(arg1,
                         arg2,
                         arg3,
                         astring);

  if (astring != NULL)
    FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL GetOverlappedResult
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in GetOverlappedResult
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION4(BOOL,         GetOverlappedResult,
              HANDLE,       arg1,
              LPOVERLAPPED, arg2,
              LPDWORD,      arg3,
              BOOL,         arg4)
{
  return HMGetOverlappedResult(arg1,
                               arg2,
                               arg3,
                               arg4);
}


/*****************************************************************************
 * Name      : BOOL OpenEventA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenEventA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE, OpenEventA,
              DWORD,  arg1,
              BOOL,   arg2,
              LPCSTR, arg3)
{
  dprintf(("KERNEL32: OpenEventA(%s)\n",
           arg3));

  return HMOpenEvent(arg1,
                     arg2,
                     arg3);
}


/*****************************************************************************
 * Name      : BOOL
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenEventW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE,  OpenEventW,
              DWORD,   dwDesiredAccess,
              BOOL,    bInheritHandle,
              LPCWSTR, lpName)
{
  char  *asciiname;
  HANDLE rc;

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);

  dprintf(("KERNEL32: OpenEventW(%s)\n",
           asciiname));

  rc = HMOpenEvent(dwDesiredAccess,
                   bInheritHandle,
                   asciiname);
  FreeAsciiString(asciiname);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL OpenMutexA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenMutexA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE, OpenMutexA,
              DWORD,  arg1,
              BOOL,   arg2,
              LPCSTR, arg3)
{
  dprintf(("KERNEL32: OpenMutexA(%s)\n",
           arg3));

  return HMOpenMutex(arg1,
                     arg2,
                     arg3);
}


/*****************************************************************************
 * Name      : BOOL OpenMutexW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenMutexW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE,  OpenMutexW,
              DWORD,   dwDesiredAccess,
              BOOL,    bInheritHandle,
              LPCWSTR, lpName)
{
  char  *asciiname;
  HANDLE rc;

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);

  dprintf(("KERNEL32: OpenMutexW(%s)\n",
           asciiname));

  rc = HMOpenMutex(dwDesiredAccess,
                   bInheritHandle,
                   asciiname);
  FreeAsciiString(asciiname);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL OpenSemaphoreA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenSemaphoreA
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE, OpenSemaphoreA,
              DWORD,  arg1,
              BOOL,   arg2,
              LPCSTR, arg3)
{
  dprintf(("KERNEL32: OpenSemaphoreA(%s)\n",
           arg3));

  return HMOpenSemaphore(arg1,
                         arg2,
                         arg3);
}


/*****************************************************************************
 * Name      : BOOL OpenSemaphoreW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in OpenSemaphoreW
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(HANDLE,  OpenSemaphoreW,
              DWORD,   dwDesiredAccess,
              BOOL,    bInheritHandle,
              LPCWSTR, lpName)
{
  char  *asciiname;
  HANDLE rc;

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);

  dprintf(("KERNEL32: OpenSemaphoreW(%s)\n",
           asciiname));

  rc = HMOpenSemaphore(dwDesiredAccess,
                       bInheritHandle,
                       asciiname);
  FreeAsciiString(asciiname);
  return(rc);
}


/*****************************************************************************
 * Name      : BOOL PulseEvent
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in PulseEvent
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PulseEvent,
              HANDLE, arg1)
{
  return HMPulseEvent(arg1);
}


/*****************************************************************************
 * Name      : BOOL ReleaseSemaphore
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ReleaseSemaphore
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION3(BOOL,   ReleaseSemaphore,
              HANDLE, arg1,
              LONG,   arg2,
              PLONG,  arg3)
{
  return HMReleaseSemaphore(arg1,
                            arg2,
                            arg3);
}


/*****************************************************************************
 * Name      : BOOL ResetEvent
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ResetEvent
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION1(BOOL,   ResetEvent,
              HANDLE, arg1)
{
  return HMResetEvent(arg1);
}


/*****************************************************************************
 * Name      : BOOL WaitForMultipleObjects
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in WaitForMultipleObjects
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION4(DWORD,          WaitForMultipleObjects,
              DWORD,          arg1,
              const HANDLE *, arg2,
              BOOL,           arg3,
              DWORD,          arg4)
{
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

ODINFUNCTION5(DWORD, WaitForMultipleObjectsEx,
              DWORD, cObjects,
              CONST HANDLE *, lphObjects,
              BOOL, fWaitAll,
              DWORD, dwTimeout,
              BOOL, fAlertable)
{
  return(HMWaitForMultipleObjectsEx(cObjects,
                                    (PHANDLE)lphObjects,
                                    fWaitAll,
                                    dwTimeout,
                                    fAlertable));
}


/*****************************************************************************
 * Name      : HANDLE ConvertToGlobalHandle
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : handle translation is done in ConvertToGlobalHandle
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1999/06/18 03:44]
 *****************************************************************************/

ODINFUNCTION1(HANDLE, ConvertToGlobalHandle,
              HANDLE, hHandle)

//ODINFUNCTION2(HANDLE, ConvertToGlobalHandle,
//              HANDLE, hHandle,
//              BOOL,   fInherit)
{
  return (hHandle);
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(HANDLE,CreateThread,LPSECURITY_ATTRIBUTES,  lpsa,
                                  DWORD,                  cbStack,
                                  LPTHREAD_START_ROUTINE, lpStartAddr,
                                  LPVOID,                 lpvThreadParm,
                                  DWORD,                  fdwCreate,
                                  LPDWORD,                lpIDThread)
{
    return HMCreateThread(lpsa, cbStack, lpStartAddr, lpvThreadParm, fdwCreate, lpIDThread);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetThreadPriority(HANDLE hThread)
{
    return HMGetThreadPriority(hThread);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SuspendThread(HANDLE hThread)
{
    return HMSuspendThread(hThread);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetThreadPriority(HANDLE hThread, int priority)
{
  return HMSetThreadPriority(hThread, priority);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetThreadContext(HANDLE hThread, PCONTEXT lpContext)
{
  return HMGetThreadContext(hThread, lpContext);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetThreadContext(HANDLE hThread, const CONTEXT *lpContext)
{
  return HMSetThreadContext(hThread, lpContext);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TerminateThread(HANDLE hThread, DWORD exitcode)
{
  return HMTerminateThread(hThread, exitcode);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ResumeThread(HANDLE hThread)
{
  return HMResumeThread(hThread);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetExitCodeThread(HANDLE hThread, LPDWORD arg2)
{
    return HMGetExitCodeThread(hThread, arg2);
}
//******************************************************************************
//******************************************************************************
