/* $Id: conin.cpp,v 1.12 2000-10-03 17:28:28 sandervl Exp $ */

/*
 * Win32 Console API Translation for OS/2
 * 1998/02/10 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 */


#ifdef DEBUG
#define DEBUG_LOCAL
#define DEBUG_LOCAL2
#endif


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
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

#include <win32api.h>
#include <misc.h>
#include <string.h>

#include "conwin.h"          // Windows Header for console only
#include "HandleManager.h"
#include "HMDevice.h"
#include "Conin.H"
#include "Console2.h"
#include <heapstring.h>

#define DBG_LOCALLOG	DBG_conin
#include "dbglocal.h"


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    : @@@PH CONIN$ handles should be exclusive
 *                   reject other requests to this device
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::CreateFile (LPCSTR        lpFileName,
                                          PHMHANDLEDATA pHMHandleData,
                                          PVOID         lpSecurityAttributes,
                                          PHMHANDLEDATA pHMHandleDataTemplate)
{
#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass::CreateFile %s(%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData->hHMHandle,
           lpSecurityAttributes,
           pHMHandleDataTemplate);
#endif

  pHMHandleData->dwType = FILE_TYPE_CHAR;        /* we're a character device */

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

BOOL HMDeviceConsoleInClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                       LPCVOID       lpBuffer,
                                       DWORD         nNumberOfBytesToRead,
                                       LPDWORD       lpNumberOfBytesRead,
                                       LPOVERLAPPED  lpOverlapped)
{
  ULONG  ulCounter;                  /* character counter for the queue loop */
  PSZ    pszTarget;                              /* pointer to target buffer */
  APIRET rc;                                               /* API returncode */
  INPUT_RECORD InputRecord;               /* buffer for the event to be read */
  ULONG  ulPostCounter;                            /* semaphore post counter */
  BOOL   fLoop = TRUE;      /* set to false if function may return to caller */

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped);
#endif

  ulCounter = 0;                              /* read ascii chars from queue */
  pszTarget = (PSZ)lpBuffer;

                                  /* block if no key events are in the queue */
  for (;fLoop;)                       /* until we got some characters */
  {
    if (iConsoleInputQueryEvents() == 0)      /* if queue is currently empty */
    {
      rc = DosWaitEventSem(pConsoleInput->hevInputQueue,    /* wait for input */
                           SEM_INDEFINITE_WAIT);
      DosResetEventSem(pConsoleInput->hevInputQueue,       /* reset semaphore */
                       &ulPostCounter);            /* post counter - ignored */
    }

    do
    {
      rc = iConsoleInputEventPop(&InputRecord);      /* get event from queue */
      if (rc == NO_ERROR)         /* if we've got a valid event in the queue */
      {
        //@@@PH other events are discarded!
        if ( (InputRecord.EventType == KEY_EVENT) &&     /* check event type */
             (InputRecord.Event.KeyEvent.bKeyDown == TRUE) )
        {
          // console in line input mode ?
          if (pConsoleInput->dwConsoleMode & ENABLE_LINE_INPUT)
          {
            // continue until buffer full or CR entered
            // Note: CRLF is actually returned at the end of the buffer!
            if (InputRecord.Event.KeyEvent.uChar.AsciiChar == 0x0d)
              fLoop = FALSE;
          }
          else
            // return on any single key in buffer :)
            // fLoop = FALSE;
            // @@@PH 2000/08/10 changed behaviour to return ALL input events
            // recorded in the console.
            fLoop = (iConsoleInputQueryEvents() != 0);

          // record key stroke
          if (pConsoleInput->dwConsoleMode & ENABLE_PROCESSED_INPUT)
          {
            // filter special characters first
            switch (InputRecord.Event.KeyEvent.uChar.AsciiChar)
            {
              case 0x00:
                // Ascii values of 0x00 are sent e.g. for SHIFT-DOWN
                // key events, etc.
                break;
              
              case 0x03: // ctrl-c is filtered!
                // @@@PH we're supposed to call a ctrl-c break handler here!
                break;
              
              case 0x0d: // CR
                // CR is automatically expanded to CRLF if in line input mode!
                if (pConsoleInput->dwConsoleMode & ENABLE_LINE_INPUT)
                {
                  *pszTarget = 0x0d; // CR
                  pszTarget++;
                  ulCounter++;
                  if (ulCounter < nNumberOfBytesToRead)  // check for room
                  {
                    *pszTarget = 0x0a; // LF
                    pszTarget++;
                    ulCounter++;
                  }
                  
                  if (pConsoleInput->dwConsoleMode & ENABLE_ECHO_INPUT)
                    HMWriteFile(pConsoleGlobals->hConsoleBuffer,
                        pszTarget-2,
                        2,
                        &ulPostCounter,                      /* dummy result */
                        NULL);
                  
                }
              
                break;

              case 0x08: // backspace
                if (ulCounter > 0)
                {
                  //@@@PH erase character on screen!
                  ulCounter--;
                  pszTarget--;
                                                     /* local echo enabled ? */
                  if (pConsoleInput->dwConsoleMode & ENABLE_ECHO_INPUT)
                    HMWriteFile(pConsoleGlobals->hConsoleBuffer,
                        &InputRecord.Event.KeyEvent.uChar.AsciiChar,
                        1,
                        &ulPostCounter,                      /* dummy result */
                        NULL);
                }
                break;

              default:
                // OK, for the rest ...
                *pszTarget = InputRecord.Event.KeyEvent.uChar.AsciiChar;
                dprintf(("KERNEL32:CONIN$: Debug: recorded key (%c - %02xh)\n",
                         *pszTarget,
                         *pszTarget));

                pszTarget++;
                ulCounter++;
                                                     /* local echo enabled ? */
                if (pConsoleInput->dwConsoleMode & ENABLE_ECHO_INPUT)
                  HMWriteFile(pConsoleGlobals->hConsoleBuffer,
                        &InputRecord.Event.KeyEvent.uChar.AsciiChar,
                        1,
                        &ulPostCounter,                      /* dummy result */
                        NULL);


            }
          }
          else
          {
            *pszTarget = InputRecord.Event.KeyEvent.uChar.AsciiChar;
            dprintf(("KERNEL32:CONIN$: Debug: recorded key (%c - %02xh)\n",
                     *pszTarget,
                     *pszTarget));

            pszTarget++;
            ulCounter++;

                                                     /* local echo enabled ? */
            if (pConsoleInput->dwConsoleMode & ENABLE_ECHO_INPUT)
              HMWriteFile(pConsoleGlobals->hConsoleBuffer,
                        &InputRecord.Event.KeyEvent.uChar.AsciiChar,
                        1,
                        &ulPostCounter,                      /* dummy result */
                        NULL);
          }

          // buffer filled?
          if (ulCounter >= nNumberOfBytesToRead)        /* at buffer's end ? */
            fLoop = FALSE;
        }
                                         /* Note: other events are discarded */
      }
    }
    while (rc == NO_ERROR);
  }

  *lpNumberOfBytesRead = ulCounter;                          /* write result */

  return(TRUE);                                                        /* OK */
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

BOOL HMDeviceConsoleInClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                        LPCVOID       lpBuffer,
                                        DWORD         nNumberOfBytesToWrite,
                                        LPDWORD       lpNumberOfBytesWritten,
                                        LPOVERLAPPED  lpOverlapped)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass:WriteFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
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

DWORD  HMDeviceConsoleInClass::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                               ULONG         ulRequestCode,
                                               ULONG         arg1,
                                               ULONG         arg2,
                                               ULONG         arg3,
                                               ULONG         arg4)
{
  switch (ulRequestCode)
  {
    case DRQ_FLUSHCONSOLEINPUTBUFFER:
      return (HMDeviceConsoleInClass::
              FlushConsoleInputBuffer(pHMHandleData));

    case DRQ_GETCONSOLEMODE:
      return (HMDeviceConsoleInClass
              ::GetConsoleMode(pHMHandleData,
                               (LPDWORD)arg1));

    case DRQ_GETNUMBEROFCONSOLEINPUTEVENTS:
      return (HMDeviceConsoleInClass::
              GetNumberOfConsoleInputEvents(pHMHandleData,
                                            (LPDWORD)arg1));

    case DRQ_PEEKCONSOLEINPUTA:
      return (HMDeviceConsoleInClass::
              PeekConsoleInputA(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)        arg2,
                                (LPDWORD)      arg3));

    case DRQ_PEEKCONSOLEINPUTW:
      return (HMDeviceConsoleInClass::
              PeekConsoleInputW(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)        arg2,
                                (LPDWORD)      arg3));


    case DRQ_READCONSOLEA:
      return (HMDeviceConsoleInClass::
              ReadConsoleA(pHMHandleData,
                           (CONST VOID*) arg1,
                           (DWORD)       arg2,
                           (LPDWORD)     arg3,
                           (LPVOID)      arg4));

    case DRQ_READCONSOLEW:
      return (HMDeviceConsoleInClass::
              ReadConsoleW(pHMHandleData,
                           (CONST VOID*) arg1,
                           (DWORD)       arg2,
                           (LPDWORD)     arg3,
                           (LPVOID)      arg4));

    case DRQ_READCONSOLEINPUTA:
      return (HMDeviceConsoleInClass::
              ReadConsoleInputA(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

    case DRQ_READCONSOLEINPUTW:
      return (HMDeviceConsoleInClass::
              ReadConsoleInputW(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

    case DRQ_SETCONSOLEMODE:
      return (HMDeviceConsoleInClass
              ::SetConsoleMode(pHMHandleData,
                               (DWORD)arg1));

    case DRQ_WRITECONSOLEINPUTA:
      return (HMDeviceConsoleInClass::
              WriteConsoleInputA(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

    case DRQ_WRITECONSOLEINPUTW:
      return (HMDeviceConsoleInClass::
              WriteConsoleInputW(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

  }

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass:_DeviceRequest %s(%08x,%08x,%08x,%08x,%08x,%08x) unknown request\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           ulRequestCode,
           arg1,
           arg2,
           arg3,
           arg4);
#endif

  SetLastError(ERROR_INVALID_FUNCTION_W);           /* request not implemented */
  return(FALSE);                 /* we assume this indicates API call failed */
}


/*****************************************************************************
 * Name      : BOOL HMDeviceConsoleInClass::FlushConsoleInputBuffer
 * Purpose   : flushes all events from the input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

BOOL HMDeviceConsoleInClass::FlushConsoleInputBuffer(PHMHANDLEDATA pHMHandleData)
{
  ULONG ulCounter;                                           /* loop counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::FlushConsoleInputBuffer(%08x).\n",
           pHMHandleData);
#endif

  pConsoleInput->ulIndexFree  = 0;
  pConsoleInput->ulIndexEvent = 0;
  pConsoleInput->ulEvents     = 0;

  for (ulCounter = 0;
       ulCounter < CONSOLE_INPUTQUEUESIZE;
       ulCounter++)
    pConsoleInput->arrInputRecord[ulCounter].EventType = 0x0000; /* free event */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::GetConsoleMode
 * Purpose   : queries the current console mode
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPDWORD lpMode
 * Variables :
 * Result    :

 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::GetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                             LPDWORD       lpMode)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::GetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           lpMode);
#endif

  *lpMode = pConsoleInput->dwConsoleMode;      /* return current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::GetNumberOfConsoleInputEvents
 * Purpose   : queries the current number of events in the input queue
 * Parameters: PHMHANDLEDATA pHMHandleData    - handle specific data
 *             LPDWORD       lpNumberOfEvents - return number of events
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

BOOL HMDeviceConsoleInClass::GetNumberOfConsoleInputEvents(PHMHANDLEDATA pHMHandleData,
                                                           LPDWORD       lpNumberOfEvents)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::GetNumberOfConsoleInputEvents(%08x,%08x).\n",
           pHMHandleData,
           lpNumberOfEvents);
#endif

  *lpNumberOfEvents = pConsoleInput->ulEvents;     /* return number of events */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::PeekConsoleInputA
 * Purpose   : peeks events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    : if queue is completely filled and no event is free,
 *             loop will scan over queue multiple times, until target
 *             buffer is filled. It does not check ulCounter to stop
 *             when one scan of the queue is complete.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::PeekConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG         ulCounter;                                   /* loop counter */
  ULONG         ulCurrentEvent;       /* index of current event in the queue */
  PINPUT_RECORD pirEvent;                /* pointer to current queue element */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::PeekConsoleInputA(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (iConsoleInputQueryEvents() == 0)        /* if queue is currently empty */
  {
    *lpcRead = 0;                               /* no events read from queue */
    return (TRUE);                                         /* OK, we're done */
  }


  for (ulCounter = 0,
       ulCurrentEvent = pConsoleInput->ulIndexEvent,
       pirEvent = &pConsoleInput->arrInputRecord[pConsoleInput->ulIndexEvent];

       ulCounter < cInRecords;

       ulCounter++,
       ulCurrentEvent++,
       pirEvent++,
       pirBuffer++)
  {
    if (ulCurrentEvent > CONSOLE_INPUTQUEUESIZE) /* reaching after end of que*/
    {
      ulCurrentEvent = 0;         /* then start over from beginning of queue */
      pirEvent       = pConsoleInput->arrInputRecord;
    }

    if (pirEvent->EventType == 0x0000)                   /* no more events ? */
      break;                                              /* leave loop then */

    memcpy(pirEvent,                                      /* copy event data */
           pirBuffer,
           sizeof(INPUT_RECORD));
  }

  *lpcRead = ulCounter;                      /* return number of events read */
  return (TRUE);                                           /* OK, we're done */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::PeekConsoleInputW
 * Purpose   : peeks events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    : if queue is completely filled and no event is free,
 *             loop will scan over queue multiple times, until target
 *             buffer is filled. It does not check ulCounter to stop
 *             when one scan of the queue is complete.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::PeekConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG         ulCounter;                                   /* loop counter */
  ULONG         ulCurrentEvent;       /* index of current event in the queue */
  PINPUT_RECORD pirEvent;                /* pointer to current queue element */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::PeekConsoleInputW(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (iConsoleInputQueryEvents() == 0)        /* if queue is currently empty */
  {
    *lpcRead = 0;                               /* no events read from queue */
    return (TRUE);                                         /* OK, we're done */
  }


  for (ulCounter = 0,
       ulCurrentEvent = pConsoleInput->ulIndexEvent,
       pirEvent = &pConsoleInput->arrInputRecord[pConsoleInput->ulIndexEvent];

       ulCounter < cInRecords;

       ulCounter++,
       ulCurrentEvent++,
       pirEvent++,
       pirBuffer++)
  {
    if (ulCurrentEvent > CONSOLE_INPUTQUEUESIZE) /* reaching after end of que*/
    {
      ulCurrentEvent = 0;         /* then start over from beginning of queue */
      pirEvent       = pConsoleInput->arrInputRecord;
    }

    if (pirEvent->EventType == 0x0000)                   /* no more events ? */
      break;                                              /* leave loop then */

    memcpy(pirEvent,                                      /* copy event data */
           pirBuffer,
           sizeof(INPUT_RECORD));
  }

  *lpcRead = ulCounter;                      /* return number of events read */
  return (TRUE);                                           /* OK, we're done */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleA
 * Purpose   : read a string from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cWriteCells
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleA(PHMHANDLEDATA pHMHandleData,
                                           CONST VOID*   lpvBuffer,
                                           DWORD         cchToRead,
                                           LPDWORD       lpcchRead,
                                           LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::ReadConsoleA(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToRead,
           lpcchRead,
           lpvReserved);
#endif

                               /* simply forward the request to that routine */
  return (HMDeviceConsoleInClass::ReadFile(pHMHandleData,
                                           lpvBuffer,
                                           cchToRead,
                                           lpcchRead,
                                           NULL));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleW
 * Purpose   : write a string to the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cWriteCells
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleW(PHMHANDLEDATA pHMHandleData,
                                           CONST VOID*   lpvBuffer,
                                           DWORD         cchToRead,
                                           LPDWORD       lpcchRead,
                                           LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  DWORD          dwResult;
  LPSTR          lpstrAscii;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::ReadConsoleW(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToRead,
           lpcchRead,
           lpvReserved);
#endif

  // create ascii buffer
  lpstrAscii = (LPSTR)HEAP_malloc(cchToRead);
  if (lpstrAscii == NULL)
     return ERROR_NOT_ENOUGH_MEMORY;

                               /* simply forward the request to that routine */
  dwResult = HMDeviceConsoleInClass::ReadFile(pHMHandleData,
                                              lpstrAscii,
                                              cchToRead,
                                              lpcchRead,
                                              NULL);
  /* Ascii -> unicode translation */
  if (dwResult == TRUE)
    lstrcpynAtoW((LPWSTR)lpvBuffer, lpstrAscii, *lpcchRead);

  HEAP_free(lpstrAscii);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleInputA
 * Purpose   : read events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG  ulPostCounter;                  /* semaphore post counter - ignored */
  APIRET rc;                                               /* API returncode */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::ReadConsoleInputA(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (iConsoleInputQueryEvents() == 0)        /* if queue is currently empty */
  {
    rc = DosWaitEventSem(pConsoleInput->hevInputQueue,     /* wait for input */
                         SEM_INDEFINITE_WAIT);
    DosResetEventSem(pConsoleInput->hevInputQueue,        /* reset semaphore */
                     &ulPostCounter);              /* post counter - ignored */
  }


  /* now read events into target buffer */
  for (ulPostCounter = 0;
       ulPostCounter < cInRecords;
       ulPostCounter++,
       pirBuffer++)
  {
    rc = iConsoleInputEventPop(pirBuffer);           /* get event from queue */
    if (rc != NO_ERROR)                  /* if read error occurs, break look */
      break;
  }

  *lpcRead = ulPostCounter;                 /* return number of records read */
  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleInputW
 * Purpose   : read events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG ulPostCounter;                   /* semaphore post counter - ignored */
  APIRET rc;                                               /* API returncode */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::ReadConsoleInputW(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (iConsoleInputQueryEvents() == 0)        /* if queue is currently empty */
  {
    rc = DosWaitEventSem(pConsoleInput->hevInputQueue,      /* wait for input */
                         SEM_INDEFINITE_WAIT);
    DosResetEventSem(pConsoleInput->hevInputQueue,         /* reset semaphore */
                     &ulPostCounter);              /* post counter - ignored */
  }


  /* now read events into target buffer */
  for (ulPostCounter = 0;
       ulPostCounter < cInRecords;
       ulPostCounter++,
       pirBuffer++)
  {
    rc = iConsoleInputEventPop(pirBuffer);           /* get event from queue */
    if (rc != NO_ERROR)                  /* if read error occurs, break look */
      break;
  }

  *lpcRead = ulPostCounter;                 /* return number of records read */
  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::SetConsoleMode
 * Purpose   : sets the current console mode
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             DWORD         dwMode        - console mode
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::SetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                             DWORD         dwMode)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::SetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           dwMode);
#endif

  pConsoleInput->dwConsoleMode = dwMode;          /* set current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::WriteConsoleInputA
 * Purpose   : this writes event records directly into the queue
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PINPUT_RECORD pirBuffer
 *             DWORD         cInRecords
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::WriteConsoleInputA (PHMHANDLEDATA pHMHandleData,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  APIRET         rc;                                       /* API returncode */
  ULONG          ulCounter;                                  /* loop counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::WriteConsoleInputA(%08x,%08x,%u,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcWritten);
#endif

  for (ulCounter = 0;
       ulCounter < cInRecords;
       ulCounter++,
       pirBuffer++)
  {
    rc = iConsoleInputEventPush(pirBuffer);            /* push current event */
    if (rc != NO_ERROR)                     /* oops ? queue full ? problem ? */
      break;
  }

  *lpcWritten = ulCounter;                /* return number of events written */
  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::WriteConsoleInputW
 * Purpose   : this writes event records directly into the queue
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PINPUT_RECORD pirBuffer
 *             DWORD         cInRecords
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::WriteConsoleInputW (PHMHANDLEDATA pHMHandleData,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  APIRET         rc;                                       /* API returncode */
  ULONG          ulCounter;                                  /* loop counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::WriteConsoleInputW(%08x,%08x,%u,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcWritten);
#endif

  for (ulCounter = 0;
       ulCounter < cInRecords;
       ulCounter++,
       pirBuffer++)
  {
    rc = iConsoleInputEventPush(pirBuffer);            /* push current event */
    if (rc != NO_ERROR)                     /* oops ? queue full ? problem ? */
      break;
  }

  *lpcWritten = ulCounter;                /* return number of events written */
  return (TRUE);                                                       /* OK */
}


DWORD HMDeviceConsoleInClass::GetFileType (PHMHANDLEDATA pHMHandleData)
{
   return FILE_TYPE_CHAR;
}
