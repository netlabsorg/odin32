/* $Id: conout.cpp,v 1.9 2001-01-23 18:31:25 sandervl Exp $ */

/*
 * Win32 Console API Translation for OS/2
 * 1998/02/10 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 */


#ifdef DEBUG
#define DEBUG_LOCAL
#define DEBUG_LOCAL2
#endif

//#undef DEBUG_LOCAL
//#undef DEBUG_LOCAL2


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 - DWORD HandlerRoutine (DWORD dwCtrlType)
   basically an exception handler routine. handles a few signals / excpts.
   should be somewhere near the exception handling code ... :)

   Hmm, however as PM applications don't really get a ctrl-c signal,
   I'll have to do this on my own ...

 - supply unicode<->ascii conversions for all the _A and _W function pairs.

 - problem: we can't prevent thread1 from blocking the message queue ?
            what will happen if a WinTerminate() is issued there ?
            will the message queue be closed and provide smooth tasking ?
            how will open32 react on this ?

 - ECHO_LINE_INPUT / ReadFile blocks till CR

 - scrollbars
 * do some flowchart to exactly determine WHEN to use WHICH setting
   and perform WHAT action

 - clipboard support
*/


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define  INCL_WIN
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#define  INCL_DOSPROCESS
#define  INCL_DOSMODULEMGR
#define  INCL_VIO
#define  INCL_AVIO
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <builtin.h>
#include <stdlib.h>
#include <string.h>

#include <win32type.h>
#include <win32api.h>
#include <misc.h>

#include "conwin.h"          // Windows Header for console only
#include "HandleManager.h"
#include "HMDevice.h"
#include "ConOut.H"

#include "console2.h"
#include "conprop.h"
#include "unicode.h"

#define DBG_LOCALLOG	DBG_conout
#include "dbglocal.h"

/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleOutClass::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleOutClass::CreateFile (LPCSTR        lpFileName,
                                           PHMHANDLEDATA pHMHandleData,
                                           PVOID         lpSecurityAttributes,
                                           PHMHANDLEDATA pHMHandleDataTemplate)
{
  APIRET rc;
  BOOL   fResult;
  HANDLE hConsole;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass %s(%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData->hHMHandle,
           lpSecurityAttributes,
           pHMHandleDataTemplate);
#endif

  pHMHandleData->dwType = FILE_TYPE_CHAR;        /* we're a character device */


                 /* if no default buffer is available, then do default setup */
  if (pConsoleGlobals->hConsoleBuffer == INVALID_HANDLE_VALUE)
  {
                /* now we need a default screen buffer with the default size */
    hConsole = CreateConsoleScreenBuffer(0,
                                         0,
                                         NULL,
                                         CONSOLE_TEXTMODE_BUFFER,
                                         NULL);
    if (hConsole == INVALID_HANDLE_VALUE)
    {
#ifdef DEBUG_LOCAL
      WriteLog("KERNEL32/CONSOLE:OS2CreateConsoleScreenBuffer = %u.\n",
               GetLastError());
#endif
      return INVALID_HANDLE_VALUE;   /* abort further processing immediately */
    }

    fResult = SetConsoleTextAttribute(hConsole,
                                      pConsoleGlobals->Options.ucDefaultAttribute);
#ifdef DEBUG_LOCAL
    if (fResult == FALSE)                                    /* check errors */
      WriteLog("KERNEL32/CONSOLE:OS2SetConsoleTextAttribute=%u.\n",
               GetLastError());
#endif

    fResult = SetConsoleScreenBufferSize(hConsole,
                                         pConsoleGlobals->Options.coordDefaultSize);
    if (fResult == FALSE)
    {
#ifdef DEBUG_LOCAL
      WriteLog("KERNEL32/CONSOLE:OS2SetConsoleScreenBufferSize=%u.\n",
               GetLastError());
#endif
      HMCloseHandle(hConsole);                          /* free handle again */
      return (INVALID_HANDLE_VALUE);            /* abort further processing */
    }

    fResult = SetConsoleActiveScreenBuffer(hConsole);
    if (fResult == FALSE)
    {
#ifdef DEBUG_LOCAL
      WriteLog("KERNEL32/CONSOLE:OS2SetConsoleActiveScreenBuffer=%u.\n",
               GetLastError());
#endif
      HMCloseHandle(hConsole);                          /* free handle again */
      return (INVALID_HANDLE_VALUE);            /* abort further processing */
    }
    else
    {
      pConsoleGlobals->hConsoleBufferDefault = hConsole;        /* save handle */
      pConsoleGlobals->hConsoleBuffer        = hConsole;
    }
  }

  return(NO_ERROR);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceConsoleOutClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                       LPCVOID       lpBuffer,
                                       DWORD         nNumberOfBytesToRead,
                                       LPDWORD       lpNumberOfBytesRead,
                                       LPOVERLAPPED  lpOverlapped)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped);
#endif

  SetLastError(ERROR_ACCESS_DENIED_W);
  return FALSE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceConsoleOutClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                         LPCVOID       lpBuffer,
                                         DWORD         nNumberOfBytesToWrite,
                                         LPDWORD       lpNumberOfBytesWritten,
                                         LPOVERLAPPED  lpOverlapped)
{
  BOOL dwResult;                        /* result from subsequent WriteFile */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass:WriteFile %s(%08x,%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped);
#endif

        /* just prevent an endless loop, although this condition might never */
                                                                /* be true ! */
  if (pHMHandleData->hHMHandle != pConsoleGlobals->hConsoleBuffer)
  {
    dwResult = HMWriteFile(pConsoleGlobals->hConsoleBuffer,
                           lpBuffer,
                           nNumberOfBytesToWrite,
                           lpNumberOfBytesWritten,
                           lpOverlapped);

    return (dwResult);                                 /* return result code */
  }
  else {
    return (FALSE);                    /* raise error condition */
  }
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleOutClass::_DeviceRequest
 * Purpose   : we just forward those device requests to the console buffer
 *             currently associated with the console itself.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/03/35 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleOutClass::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                               ULONG         ulRequestCode,
                                               ULONG         arg1,
                                               ULONG         arg2,
                                               ULONG         arg3,
                                               ULONG         arg4)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass:_DeviceRequest %s(%08x,%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           ulRequestCode,
           arg1,
           arg2,
           arg3,
           arg4);
#endif
        /* just prevent an endless loop, although this condition might never */
                                                                /* be true ! */
  if (pHMHandleData->hHMHandle != pConsoleGlobals->hConsoleBuffer)
    return (HMDeviceRequest(pConsoleGlobals->hConsoleBuffer,
                            ulRequestCode,
                            arg1,
                            arg2,
                            arg3,
                            arg4));
  else
    return (ERROR_SYS_INTERNAL);                    /* raise error condition */
}


DWORD HMDeviceConsoleOutClass::GetFileType (PHMHANDLEDATA pHMHandleData)
{
   return FILE_TYPE_CHAR;
}
