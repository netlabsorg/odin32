/* $Id: kobjects.cpp,v 1.16 2003-02-04 11:29:01 sandervl Exp $ */

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


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1







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

HANDLE WIN32API CreateMutexW(LPSECURITY_ATTRIBUTES arg1, BOOL arg2,
                             LPCWSTR arg3)
{
  HANDLE rc;
  char  *astring;

  if (arg3 != NULL) // support for unnamed mutexes
    astring = UnicodeToAsciiString((LPWSTR)arg3);
  else
    astring = NULL;

  rc = CreateMutexA(arg1,
                     arg2,
                     astring);

  if (astring != NULL)
    FreeAsciiString(astring);

  return(rc);
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

DWORD WIN32API WaitForSingleObject(HANDLE hObject, DWORD timeout)
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

DWORD WIN32API WaitForSingleObjectEx(HANDLE hObject, DWORD dwTimeout, BOOL fAlertable)
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

UINT WIN32API SetHandleCount(UINT cHandles)
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

BOOL WIN32API DuplicateHandle(HANDLE srcprocess, HANDLE srchandle,
                              HANDLE destprocess, PHANDLE desthandle,
                              DWORD arg5, BOOL arg6, DWORD arg7)
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

HANDLE WIN32API CreateSemaphoreW(LPSECURITY_ATTRIBUTES arg1, LONG arg2,
                                 LONG arg3, LPCWSTR arg4)
{
  HANDLE rc;
  char   *astring;

  if (arg4 != NULL) // support for unnamed semaphores
    astring = UnicodeToAsciiString((LPWSTR)arg4);
  else
    astring = NULL;

  rc = CreateSemaphoreA(arg1,
                         arg2,
                         arg3,
                         astring);

  if (astring != NULL)
    FreeAsciiString(astring);
  return(rc);
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

HANDLE WIN32API  OpenEventW(DWORD dwDesiredAccess, BOOL bInheritHandle,
                            LPCWSTR lpName)
{
  char  *asciiname;
  HANDLE rc;

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);

  rc = OpenEventA(dwDesiredAccess,
                   bInheritHandle,
                   asciiname);
  FreeAsciiString(asciiname);
  return(rc);
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

HANDLE WIN32API OpenMutexW(DWORD dwDesiredAccess, BOOL bInheritHandle,
                           LPCWSTR lpName)
{
  char  *asciiname;
  HANDLE rc;

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);

  rc = OpenMutexA(dwDesiredAccess,
                   bInheritHandle,
                   asciiname);
  FreeAsciiString(asciiname);
  return(rc);
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

HANDLE WIN32API OpenSemaphoreW(DWORD dwDesiredAccess, BOOL bInheritHandle,
                               LPCWSTR lpName)
{
  char  *asciiname;
  HANDLE rc;

  asciiname = UnicodeToAsciiString((LPWSTR)lpName);

  rc = OpenSemaphoreA(dwDesiredAccess,
                       bInheritHandle,
                       asciiname);
  FreeAsciiString(asciiname);
  return(rc);
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

DWORD WIN32API WaitForMultipleObjects(DWORD arg1, const HANDLE * arg2,
                                      BOOL arg3, DWORD arg4)
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

DWORD WIN32API WaitForMultipleObjectsEx(DWORD cObjects, CONST HANDLE *lphObjects,
                                        BOOL fWaitAll, DWORD dwTimeout,
                                        BOOL fAlertable)
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

HANDLE WIN32API ConvertToGlobalHandle(HANDLE hHandle)

//ODINFUNCTION2(HANDLE ConvertToGlobalHandle,
//              HANDLE hHandle,
//              BOOL   fInherit)
{
  return (hHandle);
}

//******************************************************************************
//******************************************************************************
