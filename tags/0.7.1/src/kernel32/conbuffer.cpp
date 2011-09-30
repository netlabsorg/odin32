/* $Id: conbuffer.cpp,v 1.20 2004-02-19 13:03:05 sandervl Exp $ */

/*
 * Win32 Console API Translation for OS/2
 *
 * 1998/02/10 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) console.cpp         1.0.0   1998/02/10 PH Start from scratch
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
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
#include <os2wrap.h>    //Odin32 OS/2 api wrappers

#include <win32api.h>
#include <misc.h>
#include <string.h>
#include <stdlib.h>

#include "conwin.h"          // Windows Header for console only
#include "HandleManager.h"
#include "HMDevice.h"
#include "ConBuffer.H"
#include "console.h"
#include "Console2.h"
#include <heapstring.h>

#define DBG_LOCALLOG    DBG_conbuffer
#include "dbglocal.h"


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::CreateFile
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

DWORD HMDeviceConsoleBufferClass::CreateFile (LPCSTR        lpFileName,
                                              PHMHANDLEDATA pHMHandleData,
                                              PVOID         lpSecurityAttributes,
                                              PHMHANDLEDATA pHMHandleDataTemplate)
{
  PCONSOLEBUFFER pConsoleBuffer;                 /* console buffer structure */

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass %s(%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData->hHMHandle,
           lpSecurityAttributes,
           pHMHandleDataTemplate);
#endif

  pHMHandleData->lpHandlerData = malloc ( sizeof(CONSOLEBUFFER) );

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:CheckPoint1: %s pHMHandleData=%08xh, lpHandlerData=%08xh\n",
           lpFileName,
           pHMHandleData,
           pHMHandleData->lpHandlerData);
#endif


  if (pHMHandleData->lpHandlerData == NULL)              /* check allocation */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY_W);          /* set error information */
    return (INVALID_HANDLE_VALUE);                  /* raise error condition */
  }
  else
  {
    pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

    memset(pHMHandleData->lpHandlerData,             /* initialize structure */
           0,
           sizeof (CONSOLEBUFFER) );

                                                      /* set buffer defaults */
    pConsoleBuffer->dwConsoleMode = ENABLE_PROCESSED_OUTPUT |
                                    ENABLE_WRAP_AT_EOL_OUTPUT;

    pConsoleBuffer->CursorInfo.dwSize   = 20;             /* 20% cell height */
    pConsoleBuffer->CursorInfo.bVisible = TRUE;
  }

  return(NO_ERROR);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HMDeviceConsoleBufferClass::GetFileType %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData));

  return FILE_TYPE_CHAR;
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

BOOL HMDeviceConsoleBufferClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass::CloseHandle %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData);
#endif

  if (pHMHandleData->lpHandlerData != NULL)                 /* check pointer */
  {
    PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;


    if (pConsoleBuffer->ppszLine != NULL)        /* free line buffer array ! */
      free (pConsoleBuffer->ppszLine);

    free (pHMHandleData->lpHandlerData);          /* free device object data */
    pHMHandleData->lpHandlerData = NULL;
  }

  return TRUE;
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

BOOL HMDeviceConsoleBufferClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                           LPCVOID       lpBuffer,
                                           DWORD         nNumberOfBytesToRead,
                                           LPDWORD       lpNumberOfBytesRead,
                                           LPOVERLAPPED lpOverlapped,
                                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)\n",
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

BOOL HMDeviceConsoleBufferClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                            LPCVOID       lpBuffer,
                                            DWORD         nNumberOfBytesToWrite,
                                            LPDWORD       lpNumberOfBytesWritten,
                                            LPOVERLAPPED lpOverlapped,
                                            LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
           ULONG ulCounter;                 /* counter for the byte transfer */
           PSZ   pszBuffer = (PSZ)lpBuffer;
  register UCHAR ucChar;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass:WriteFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped);
#endif

  if(lpCompletionRoutine) {
      dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
  }

  /* check if we're called with non-existing line buffer */
  if (pConsoleBuffer->ppszLine == NULL) {
    SetLastError(ERROR_OUTOFMEMORY_W);
    return FALSE;
  }
  for (ulCounter = 0;
       ulCounter < nNumberOfBytesToWrite;
       ulCounter++)
  {
    ucChar = pszBuffer[ulCounter];                        /* map to register */

    if ( (pConsoleBuffer->dwConsoleMode & ENABLE_PROCESSED_OUTPUT) &&
         (ucChar < 32) )     /* this is faster than a large switch statement */
    {
      switch (ucChar)
      {
        case 7: /* BEL */
          if (pConsoleGlobals->Options.fSpeakerEnabled == TRUE)
            DosBeep(pConsoleGlobals->Options.ulSpeakerFrequency,
                    pConsoleGlobals->Options.ulSpeakerDuration);
          break;

        case 8: /* Backspace */
          // not correct if deleting expanded tab character
          if (pConsoleBuffer->coordCursorPosition.X > 0)
            pConsoleBuffer->coordCursorPosition.X--;

          //@@@PH overwrite old character
          *(pConsoleBuffer->ppszLine[pConsoleBuffer->coordCursorPosition.Y] +
            pConsoleBuffer->coordCursorPosition.X * 2) = 0x20;
          break;

        case 9: /* Tab */
          pConsoleBuffer->coordCursorPosition.X =
            (pConsoleBuffer->coordCursorPosition.X
             / pConsoleGlobals->Options.ulTabSize
             + 1)
            * pConsoleGlobals->Options.ulTabSize;

          if (pConsoleBuffer->coordCursorPosition.X >=
              pConsoleBuffer->coordBufferSize.X)
          {
            pConsoleBuffer->coordCursorPosition.X = 0;
            pConsoleBuffer->coordCursorPosition.Y++;

            if (pConsoleBuffer->coordCursorPosition.Y >=
                pConsoleBuffer->coordBufferSize.Y)
            {
              if (pConsoleBuffer->dwConsoleMode & ENABLE_WRAP_AT_EOL_OUTPUT)
              {
                iConsoleBufferScrollUp(pConsoleBuffer,   /* scroll one line up */
                                       1);
                pConsoleBuffer->coordCursorPosition.Y--;
              }
            }
          }
          break;

        case 13: /* CARRIAGE RETURN */
          pConsoleBuffer->coordCursorPosition.X = 0;
          break;

        case 10: /* LINEFEED */
          pConsoleBuffer->coordCursorPosition.Y++;

          if (pConsoleBuffer->coordCursorPosition.Y >=
              pConsoleBuffer->coordBufferSize.Y)
          {
            iConsoleBufferScrollUp(pConsoleBuffer,     /* scroll one line up */
                                   1);
            pConsoleBuffer->coordCursorPosition.Y--;
          }
          break;

        default:
          break;
      }
    }
    else
    {
                                                          /* write character */
      *(pConsoleBuffer->ppszLine[pConsoleBuffer->coordCursorPosition.Y] +
        pConsoleBuffer->coordCursorPosition.X * 2) = ucChar;

      pConsoleBuffer->coordCursorPosition.X++;

      if (pConsoleBuffer->coordCursorPosition.X >=
          pConsoleBuffer->coordBufferSize.X)
      {
        pConsoleBuffer->coordCursorPosition.X = 0;
        pConsoleBuffer->coordCursorPosition.Y++;

        if (pConsoleBuffer->coordCursorPosition.Y >=
            pConsoleBuffer->coordBufferSize.Y)
        {
          if (pConsoleBuffer->dwConsoleMode & ENABLE_WRAP_AT_EOL_OUTPUT)
          {
            iConsoleBufferScrollUp(pConsoleBuffer,     /* scroll one line up */
                                   1);
            pConsoleBuffer->coordCursorPosition.Y--;
          }
          else
          {
                                              /* just stay on last character */
            pConsoleBuffer->coordCursorPosition.X = pConsoleBuffer->coordBufferSize.X - 1;
            pConsoleBuffer->coordCursorPosition.Y = pConsoleBuffer->coordBufferSize.Y - 1;
          }
        }
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  *lpNumberOfBytesWritten = ulCounter;

  return TRUE;
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

DWORD  HMDeviceConsoleBufferClass::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                                   ULONG         ulRequestCode,
                                                   ULONG         arg1,
                                                   ULONG         arg2,
                                                   ULONG         arg3,
                                                   ULONG         arg4)
{
  switch (ulRequestCode)
  {
    case DRQ_FILLCONSOLEOUTPUTATTRIBUTE:
    {
      COORD coordWrite;

      ULONG2COORD(coordWrite,arg3);

      return (FillConsoleOutputAttribute(pHMHandleData,
                                         (WORD)arg1,
                                         (DWORD)arg2,
                                         coordWrite,
                                         (LPDWORD)arg4));
    }


    case DRQ_FILLCONSOLEOUTPUTCHARACTERA:
    {
      COORD coordWrite;

      ULONG2COORD(coordWrite,arg3);

      return (FillConsoleOutputCharacterA(pHMHandleData,
                                          (UCHAR)arg1,
                                          (DWORD)arg2,
                                          coordWrite,
                                            (LPDWORD)arg4));
    }


    case DRQ_FILLCONSOLEOUTPUTCHARACTERW:
    {
      COORD coordWrite;

      ULONG2COORD(coordWrite,arg3);

      return (FillConsoleOutputCharacterW(pHMHandleData,
                                            (WCHAR)arg1,
                                            (DWORD)arg2,
                                            coordWrite,
                                            (LPDWORD)arg4));
    }


    case DRQ_GETCONSOLECURSORINFO:
      return (GetConsoleCursorInfo(pHMHandleData,
                                     (PCONSOLE_CURSOR_INFO)arg1));


    case DRQ_GETCONSOLEMODE:
      return (GetConsoleMode(pHMHandleData,
                               (LPDWORD)arg1));


    case DRQ_GETCONSOLESCREENBUFFERINFO:
      return (GetConsoleScreenBufferInfo(pHMHandleData,
                                           (PCONSOLE_SCREEN_BUFFER_INFO)arg1));


    case DRQ_GETLARGESTCONSOLEWINDOWSIZE:
      return (GetLargestConsoleWindowSize(pHMHandleData));


    case DRQ_READCONSOLEOUTPUTA:
    {
      COORD coordDestBufferSize;
      COORD coordDestBufferCoord;

      ULONG2COORD(coordDestBufferSize,  arg2);
      ULONG2COORD(coordDestBufferCoord, arg3);

      return (ReadConsoleOutputA(pHMHandleData,
                                   (PCHAR_INFO)arg1,
                                   coordDestBufferSize,
                                   coordDestBufferCoord,
                                   (PSMALL_RECT)arg4));
    }


    case DRQ_READCONSOLEOUTPUTW:
    {
      COORD coordDestBufferSize;
      COORD coordDestBufferCoord;

      ULONG2COORD(coordDestBufferSize,  arg2);
      ULONG2COORD(coordDestBufferCoord, arg3);

      return (ReadConsoleOutputW(pHMHandleData,
                                   (PCHAR_INFO)arg1,
                                   coordDestBufferSize,
                                   coordDestBufferCoord,
                                   (PSMALL_RECT)arg4));
    }


    case DRQ_READCONSOLEOUTPUTATTRIBUTE:
    {
      COORD coordReadCoord;

      ULONG2COORD(coordReadCoord, arg3);

      return (ReadConsoleOutputAttribute(pHMHandleData,
                                   (LPWORD)arg1,
                                   (DWORD)arg2,
                                   coordReadCoord,
                                   (LPDWORD)arg4));
    }


    case DRQ_READCONSOLEOUTPUTCHARACTERA:
    {
      COORD coordReadCoord;

      ULONG2COORD(coordReadCoord, arg3);

      return (ReadConsoleOutputCharacterA(pHMHandleData,
                                            (LPTSTR)arg1,
                                            (DWORD)arg2,
                                            coordReadCoord,
                                            (LPDWORD)arg4));
    }


    case DRQ_READCONSOLEOUTPUTCHARACTERW:
    {
      COORD coordReadCoord;

      ULONG2COORD(coordReadCoord, arg3);

      return (ReadConsoleOutputCharacterW(pHMHandleData,
                                            (LPWSTR)arg1,
                                            (DWORD)arg2,
                                            coordReadCoord,
                                            (LPDWORD)arg4));
    }


    case DRQ_SCROLLCONSOLESCREENBUFFERA:
    {
      COORD coordDestOrigin;

      ULONG2COORD(coordDestOrigin, arg3);

      return (ScrollConsoleScreenBufferA(pHMHandleData,
                                           (PSMALL_RECT)arg1,
                                           (PSMALL_RECT)arg2,
                                           coordDestOrigin,
                                           (PCHAR_INFO)arg4));
    }


    case DRQ_SCROLLCONSOLESCREENBUFFERW:
    {
      COORD coordDestOrigin;

      ULONG2COORD(coordDestOrigin, arg3);

      return (ScrollConsoleScreenBufferW(pHMHandleData,
                                           (PSMALL_RECT)arg1,
                                           (PSMALL_RECT)arg2,
                                           coordDestOrigin,
                                           (PCHAR_INFO)arg4));
    }


    case DRQ_SETCONSOLEACTIVESCREENBUFFER:
      return (SetConsoleActiveScreenBuffer(pHMHandleData));


    case DRQ_SETCONSOLECURSORINFO:
      return (SetConsoleCursorInfo(pHMHandleData,
                                     (PCONSOLE_CURSOR_INFO)arg1));


    case DRQ_SETCONSOLECURSORPOSITION:
    {
      COORD coordCursor;

      ULONG2COORD(coordCursor, arg1);

      return (SetConsoleCursorPosition(pHMHandleData,
                                         coordCursor));
    }


    case DRQ_SETCONSOLEMODE:
      return (SetConsoleMode(pHMHandleData,
                               (DWORD)arg1));


    case DRQ_SETCONSOLESCREENBUFFERSIZE:
    {
      COORD coordSize;

      ULONG2COORD(coordSize,arg1);

      return (SetConsoleScreenBufferSize(pHMHandleData,
                                           coordSize));
    }


    case DRQ_SETCONSOLETEXTATTRIBUTE:
      return (SetConsoleTextAttribute(pHMHandleData,
                                        (WORD)arg1));


    case DRQ_SETCONSOLEWINDOWINFO:
      return (SetConsoleWindowInfo(pHMHandleData,
                                     (BOOL)arg1,
                                     (PSMALL_RECT)arg2));


    case DRQ_WRITECONSOLEA:
      return (WriteConsoleA(pHMHandleData,
                              (CONST VOID*)arg1,
                              (DWORD)arg2,
                              (LPDWORD)arg3,
                              (LPVOID)arg4));


    case DRQ_WRITECONSOLEW:
      return (WriteConsoleW(pHMHandleData,
                              (CONST VOID*)arg1,
                              (DWORD)arg2,
                              (LPDWORD)arg3,
                              (LPVOID)arg4));


    case DRQ_WRITECONSOLEOUTPUTA:
    {
      COORD coordSrcBufferSize;
      COORD coordSrcBufferCoord;

      ULONG2COORD(coordSrcBufferSize,  arg2);
      ULONG2COORD(coordSrcBufferCoord, arg3);

      return (WriteConsoleOutputA(pHMHandleData,
                                    (PCHAR_INFO)arg1,
                                    coordSrcBufferSize,
                                    coordSrcBufferCoord,
                                    (PSMALL_RECT)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTW:
    {
      COORD coordSrcBufferSize;
      COORD coordSrcBufferCoord;

      ULONG2COORD(coordSrcBufferSize,  arg2);
      ULONG2COORD(coordSrcBufferCoord, arg3);

      return (WriteConsoleOutputA(pHMHandleData,
                                    (PCHAR_INFO)arg1,
                                    coordSrcBufferSize,
                                    coordSrcBufferCoord,
                                    (PSMALL_RECT)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTATTRIBUTE:
    {
      COORD coordWriteCoord;

      ULONG2COORD(coordWriteCoord,  arg3);

      return (WriteConsoleOutputAttribute(pHMHandleData,
                                            (LPWORD)arg1,
                                            (DWORD)arg2,
                                            coordWriteCoord,
                                            (LPDWORD)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTCHARACTERA:
    {
      COORD coordWriteCoord;

      ULONG2COORD(coordWriteCoord,  arg3);

      return (WriteConsoleOutputCharacterA(pHMHandleData,
                                             (LPTSTR)arg1,
                                             (DWORD)arg2,
                                             coordWriteCoord,
                                             (LPDWORD)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTCHARACTERW:
    {
      COORD coordWriteCoord;

      ULONG2COORD(coordWriteCoord,  arg3);

      return (WriteConsoleOutputCharacterW(pHMHandleData,
                                             (LPWSTR)arg1,
                                             (DWORD)arg2,
                                             coordWriteCoord,
                                             (LPDWORD)arg4));
    }


    case DRQ_INTERNAL_CONSOLEBUFFERMAP:
      iConsoleBufferMap((PCONSOLEBUFFER)pHMHandleData->lpHandlerData);
      return (NO_ERROR);


    case DRQ_INTERNAL_CONSOLECURSORSHOW:
      iConsoleCursorShow((PCONSOLEBUFFER)pHMHandleData->lpHandlerData,
                         (ULONG)arg1);
      return (NO_ERROR);


    case DRQ_INTERNAL_CONSOLEADJUSTWINDOW:
      iConsoleAdjustWindow((PCONSOLEBUFFER)pHMHandleData->lpHandlerData);
      return (NO_ERROR);
  }


#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass:_DeviceRequest %s(%08x,%08x,%08x,%08x,%08x,%08x) unknown request\n",
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
 * Name      : DWORD HMDeviceConsoleBufferClass::FillConsoleOutputAttribute
 * Purpose   : fills the console buffer with a specified attribute
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             WORD          wAttribute
 *             DWORD         nLength
 *             COORD         dwWriteCoord
 *             LPDWORD       lpNumberOfAttrsWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::FillConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                                             WORD    wAttribute,
                                                             DWORD   nLength,
                                                             COORD   dwWriteCoord,
                                                             LPDWORD lpNumberOfAttrsWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::FillConsoleOutputAttribute(%08x,attr=%04x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           wAttribute,
           nLength,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpNumberOfAttrsWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpNumberOfAttrsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfAttrsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpNumberOfAttrsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfAttrsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < nLength;
       ulCounter++)
  {
                                                /* write attribute into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2 + 1)
     ) = (UCHAR)(wAttribute & 0xFF);
                                 /* write attribute, don't change characters */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpNumberOfAttrsWritten != NULL)       /* ensure pointer is valid */
          *lpNumberOfAttrsWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
          pConsoleGlobals->fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpNumberOfAttrsWritten != NULL)             /* ensure pointer is valid */
    *lpNumberOfAttrsWritten = nLength;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterA
 * Purpose   : fills the console buffer with a specified ASCII character
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             UCHAR         ucCharacter
 *             DWORD         nLength
 *             COORD         dwWriteCoord
 *             LPDWORD       lpNumberOfCharsWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                                              UCHAR   ucCharacter,
                                                              DWORD   nLength,
                                                              COORD   dwWriteCoord,
                                                              LPDWORD lpNumberOfCharsWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::FillConsoleOutputCharacterA(%08x,char=%02x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           ucCharacter,
           nLength,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpNumberOfCharsWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < nLength;
       ulCounter++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = ucCharacter;

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpNumberOfCharsWritten != NULL)       /* ensure pointer is valid */
          *lpNumberOfCharsWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
          pConsoleGlobals->fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpNumberOfCharsWritten != NULL)             /* ensure pointer is valid */
    *lpNumberOfCharsWritten = nLength;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterW
 * Purpose   : fills the console buffer with a specified ASCII character
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             WCHAR         wcCharacter
 *             DWORD         nLength
 *             COORD         dwWriteCoord
 *             LPDWORD       lpNumberOfCharsWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                                              WCHAR   wcCharacter,
                                                              DWORD   nLength,
                                                              COORD   dwWriteCoord,
                                                              LPDWORD lpNumberOfCharsWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::FillConsoleOutputCharacterW(%08x,char=%02x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           wcCharacter,
           nLength,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpNumberOfCharsWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < nLength;
       ulCounter++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = (UCHAR)wcCharacter;          /* @@@PH unicode to ascii conversion ! */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpNumberOfCharsWritten != NULL)       /* ensure pointer is valid */
          *lpNumberOfCharsWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
          pConsoleGlobals->fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpNumberOfCharsWritten != NULL)             /* ensure pointer is valid */
    *lpNumberOfCharsWritten = nLength;

  return (TRUE);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetConsoleMode
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

DWORD HMDeviceConsoleBufferClass::GetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                                 LPDWORD       lpMode)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           lpMode);
#endif

  *lpMode = pConsoleBuffer->dwConsoleMode;    /* return current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetConsoleCursorInfo
 * Purpose   : queries the current console's cursor information
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCONSOLE_CURSOR_INFO pCCI
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetConsoleCursorInfo(PHMHANDLEDATA        pHMHandleData,
                                                       PCONSOLE_CURSOR_INFO pCCI)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetConsoleCursorInfo(%08x,%08x).\n",
           pHMHandleData,
           pCCI);
#endif

  memcpy(pCCI,                      /* just copy the whole information block */
         &pConsoleBuffer->CursorInfo,
         sizeof (pConsoleBuffer->CursorInfo) );

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetConsoleScreenBufferInfo
 * Purpose   : queries the current console screen buffer's info
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCONSOLE_SCREEN_BUFFER_INFO pCSBI
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetConsoleScreenBufferInfo(PHMHANDLEDATA               pHMHandleData,
                                                             PCONSOLE_SCREEN_BUFFER_INFO pCSBI)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetConsoleScreenBufferInfo(%08x,%08x).\n",
           pHMHandleData,
           pCSBI);
#endif

  pCSBI->dwSize           = pConsoleBuffer->coordBufferSize;
  pCSBI->dwCursorPosition = pConsoleBuffer->coordCursorPosition;
  pCSBI->wAttributes      = (USHORT)pConsoleBuffer->ucDefaultAttribute;

  /* @@@PH unsure, but should be OK */
  pCSBI->srWindow.Left   = pConsoleBuffer->coordWindowPosition.X;
  pCSBI->srWindow.Top    = pConsoleBuffer->coordWindowPosition.Y;
  pCSBI->srWindow.Right  = pConsoleBuffer->coordWindowPosition.X +
                           pConsoleBuffer->coordWindowSize.X - 1;
  pCSBI->srWindow.Bottom = pConsoleBuffer->coordWindowPosition.Y +
                           pConsoleBuffer->coordWindowSize.Y - 1;

  pCSBI->dwMaximumWindowSize = pConsoleBuffer->coordBufferSize;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetLargestConsoleWindowSize
 * Purpose   : Determine maximum AVIO size
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetLargestConsoleWindowSize(PHMHANDLEDATA pHMHandleData)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  COORD          coordSize;                             /* maximum avio size */
  LONG           lScreenCX;                   /* width and height of display */
  LONG           lScreenCY;
  APIRET         rc;                                       /* API returncode */


#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetLargestConsoleWindowSize(%08x).\n",
           pHMHandleData);
#endif

  /* @@@PH determine maximum console window size in characters
    based on display size and current avio font */

  lScreenCX = WinQuerySysValue(HWND_DESKTOP,            /* query PM for that */
                               SV_CXSCREEN);

  lScreenCY = WinQuerySysValue(HWND_DESKTOP,            /* query PM for that */
                               SV_CYFULLSCREEN);

  if (rc != NO_ERROR)
  {
    WriteLog("KERNEL32/CONSOLE: VioGetDeviceCellSize failed with #%u.\n",
             rc);

    return (FALSE);                                        /* say API failed */
  }

  if ( (pConsoleGlobals->sCellCX == 0) ||          /* prevent division by zero */
       (pConsoleGlobals->sCellCY == 0) )
  {
    WriteLog("KERNEL32/CONSOLE: VioGetDeviceCellSize returned 0 value.\n");

    return (FALSE);                                        /* say API failed */
  }

  coordSize.X = lScreenCX / pConsoleGlobals->sCellCX;                            /* calculate */
  coordSize.Y = lScreenCY / pConsoleGlobals->sCellCY;

                /* these limitations are due to OS/2's current VIO subsystem */
  coordSize.X = min(coordSize.X, MAX_OS2_COLUMNS);
  coordSize.Y = min(coordSize.Y, MAX_OS2_ROWS);

  return (COORD2ULONG(coordSize));                           /* return value */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputA
 * Purpose   : reads character and color attribute data from screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiDestBuffer
 *             COORD         coordDestBufferSize
 *             COORD         coordDestBufferCoord
 *             PSMALL_RECT   psrctSourceRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputA(PHMHANDLEDATA pHMHandleData,
                                                     PCHAR_INFO    pchiDestBuffer,
                                                     COORD         coordDestBufferSize,
                                                     COORD         coordDestBufferCoord,
                                                     PSMALL_RECT   psrctSourceRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,  ulY;                          /* current coordinate to be read */
  ULONG ulCX, ulCY;                       /* width and height of target area */
  ULONG ulReadX, ulReadY;                      /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputA(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiDestBuffer,
           coordDestBufferSize.X,
           coordDestBufferSize.Y,
           coordDestBufferCoord.X,
           coordDestBufferCoord.Y,
           psrctSourceRect);
#endif


  /* verify psrctSourceRect first */
  psrctSourceRect->Left  = max(psrctSourceRect->Left,  0);
  psrctSourceRect->Top   = max(psrctSourceRect->Top,   0);
  psrctSourceRect->Right = min(psrctSourceRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctSourceRect->Bottom= min(psrctSourceRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordDestBufferSize.X < coordDestBufferCoord.X) ||
       (coordDestBufferSize.Y < coordDestBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER_W);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordDestBufferSize.X - coordDestBufferCoord.X;
  ulCY = coordDestBufferSize.Y - coordDestBufferCoord.Y;

  ulCX = min(ulCX, (psrctSourceRect->Right  - psrctSourceRect->Left));
  ulCY = min(ulCY, (psrctSourceRect->Bottom - psrctSourceRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctSourceRect->Right  = psrctSourceRect->Left + ulCX;
  psrctSourceRect->Bottom = psrctSourceRect->Top  + ulCY;


  for (ulY = 0,
       ulReadY = psrctSourceRect->Top;

       ulY <= ulCY;

       ulY++,
       ulReadY++)
  {
    pchi = pchiDestBuffer + sizeof(CHAR_INFO) * coordDestBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordDestBufferCoord.Y + ulY)
                            * coordDestBufferSize.X;
    for (ulX = 0,
         ulReadX = psrctSourceRect->Left;

         ulX <= ulCX;

         ulX++,
         ulReadX++,
         pchi++)
    {
                                                           /* read character */
      wCell = *(pConsoleBuffer->ppszLine[ulReadY] + ulReadX * 2);

      pchi->Char.AsciiChar = (UCHAR)(wCell & 0x00FF);
      pchi->Attributes     = wCell >> 8;
    }
  }

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputW
 * Purpose   : reads character and color attribute data from screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiDestBuffer
 *             COORD         coordDestBufferSize
 *             COORD         coordDestBufferCoord
 *             PSMALL_RECT   psrctSourceRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputW(PHMHANDLEDATA pHMHandleData,
                                                     PCHAR_INFO    pchiDestBuffer,
                                                     COORD         coordDestBufferSize,
                                                     COORD         coordDestBufferCoord,
                                                     PSMALL_RECT   psrctSourceRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,  ulY;                          /* current coordinate to be read */
  ULONG ulCX, ulCY;                       /* width and height of target area */
  ULONG ulReadX, ulReadY;                      /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputW(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiDestBuffer,
           coordDestBufferSize.X,
           coordDestBufferSize.Y,
           coordDestBufferCoord.X,
           coordDestBufferCoord.Y,
           psrctSourceRect);
#endif


  /* verify psrctSourceRect first */
  psrctSourceRect->Left  = max(psrctSourceRect->Left,  0);
  psrctSourceRect->Top   = max(psrctSourceRect->Top,   0);
  psrctSourceRect->Right = min(psrctSourceRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctSourceRect->Bottom= min(psrctSourceRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordDestBufferSize.X < coordDestBufferCoord.X) ||
       (coordDestBufferSize.Y < coordDestBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER_W);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordDestBufferSize.X - coordDestBufferCoord.X;
  ulCY = coordDestBufferSize.Y - coordDestBufferCoord.Y;

  ulCX = min(ulCX, (psrctSourceRect->Right  - psrctSourceRect->Left));
  ulCY = min(ulCY, (psrctSourceRect->Bottom - psrctSourceRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctSourceRect->Right  = psrctSourceRect->Left + ulCX;
  psrctSourceRect->Bottom = psrctSourceRect->Top  + ulCY;


  for (ulY = 0,
       ulReadY = psrctSourceRect->Top;

       ulY <= ulCY;

       ulY++,
       ulReadY++)
  {
    pchi = pchiDestBuffer + sizeof(CHAR_INFO) * coordDestBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordDestBufferCoord.Y + ulY)
                            * coordDestBufferSize.X;
    for (ulX = 0,
         ulReadX = psrctSourceRect->Left;

         ulX <= ulCX;

         ulX++,
         ulReadX++,
         pchi++)
    {
                                                           /* read character */
      wCell = *(pConsoleBuffer->ppszLine[ulReadY] + ulReadX * 2);

                                                     /* @@@PH Ascii->Unicode */
      pchi->Char.UnicodeChar = (UCHAR)(wCell & 0x00FF);
      pchi->Attributes     = wCell >> 8;
    }
  }

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputAttribute
 * Purpose   : read an array with specified attributes from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cReadCells
 *             COORD         dwReadCoord
 *             LPDWORD       lpcNumberRead
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                                             LPWORD        lpwAttribute,
                                                             DWORD         cReadCells,
                                                             COORD         dwReadCoord,
                                                             LPDWORD       lpcNumberRead)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputAttribute(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwAttribute,
           cReadCells,
           dwReadCoord.X,
           dwReadCoord.Y,
           lpcNumberRead);
#endif

  if ( (dwReadCoord.X < 0) ||
       (dwReadCoord.Y < 0) )
  {
    if (lpcNumberRead != NULL)                       /* ensure pointer is valid */
      *lpcNumberRead = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }

                                    /* check if dwReadCoord is within specs */
  if ( (dwReadCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcNumberRead != NULL)                       /* ensure pointer is valid */
      *lpcNumberRead = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cReadCells;
       ulCounter++,
       lpwAttribute++)
  {
                                                /* write attribute into cell */
    *lpwAttribute = (UCHAR)
      *(pConsoleBuffer->ppszLine[dwReadCoord.Y] +
                                (dwReadCoord.X * 2 + 1));

    dwReadCoord.X++;                                 /* move write position */
    if (dwReadCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwReadCoord.X = 0;                               /* skip to next line */
      dwReadCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcNumberRead != NULL)                   /* ensure pointer is valid */
          *lpcNumberRead = ulCounter;

        return (TRUE);
      }
    }
  }

  if (lpcNumberRead != NULL)                         /* ensure pointer is valid */
    *lpcNumberRead = cReadCells;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterA
 * Purpose   : read an array with specified characters from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpReadBuffer
 *             DWORD         cRead
 *             COORD         coordReadCoord
 *             LPDWORD       lpcNumberRead
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                                              LPTSTR        lpwReadBuffer,
                                                              DWORD         cchRead,
                                                              COORD         coordReadCoord,
                                                              LPDWORD       lpcNumberRead)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputCharacterA(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwReadBuffer,
           cchRead,
           coordReadCoord.X,
           coordReadCoord.Y,
           lpcNumberRead);
#endif

  if ( (coordReadCoord.X < 0) ||
       (coordReadCoord.Y < 0) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }

                                  /* check if coordReadCoord is within specs */
  if ( (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cchRead;
       ulCounter++,
       lpwReadBuffer++)
  {
                                                /* write character into cell */
    *lpwReadBuffer =
      *(pConsoleBuffer->ppszLine[coordReadCoord.Y] +
                                (coordReadCoord.X * 2));

    coordReadCoord.X++;                               /* move write position */
    if (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      coordReadCoord.X = 0;                             /* skip to next line */
      coordReadCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcNumberRead != NULL)                /* ensure pointer is valid */
          *lpcNumberRead = ulCounter;

        return (TRUE);
      }
    }
  }

  if (lpcNumberRead != NULL)                         /* ensure pointer is valid */
    *lpcNumberRead = cchRead;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterW
 * Purpose   : read an array with specified characters from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpReadBuffer
 *             DWORD         cRead
 *             COORD         coordReadCoord
 *             LPDWORD       lpcNumberRead
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                                              LPWSTR        lpwReadBuffer,
                                                              DWORD         cchRead,
                                                              COORD         coordReadCoord,
                                                              LPDWORD       lpcNumberRead)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputCharacterW(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwReadBuffer,
           cchRead,
           coordReadCoord.X,
           coordReadCoord.Y,
           lpcNumberRead);
#endif

  if ( (coordReadCoord.X < 0) ||
       (coordReadCoord.Y < 0) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }

                                  /* check if coordReadCoord is within specs */
  if ( (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cchRead;
       ulCounter++,
       lpwReadBuffer++)
  {
    /* @@@PH Ascii -> Unicode translation */
                                                /* write character into cell */
    *lpwReadBuffer =
      *(pConsoleBuffer->ppszLine[coordReadCoord.Y] +
                                (coordReadCoord.X * 2));

    coordReadCoord.X++;                               /* move write position */
    if (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      coordReadCoord.X = 0;                             /* skip to next line */
      coordReadCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcNumberRead != NULL)                /* ensure pointer is valid */
          *lpcNumberRead = ulCounter;

        return (TRUE);
      }
    }
  }

  if (lpcNumberRead != NULL)                         /* ensure pointer is valid */
    *lpcNumberRead = cchRead;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferA
 * Purpose   : move a block of data within the screen buffer
 * Parameters: PHMHANDLEDATA pHMHandleData   - handle specific data
 *             PSMALL_RECT   psrctSourceRect - source rectangle
 *             PSMALL_RECT   psrctClipRect   - clipping rectangle
 *             COORD         coordDestOrigin - destination coordinate
 *             PCHAR_INFO    pchiFill        - fill character
 * Variables :
 * Result    :
 * Remark    : Routine is subject to optimizations.
 *             @@@PH rewrite -> faster, better handling of overlapped buffers
 *                   copy srctSource to buffer, fill it with fill character
 *                   copy buffer to srctDest ?
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferA(PHMHANDLEDATA pHMHandleData,
                                                             PSMALL_RECT   psrctSourceRect,
                                                             PSMALL_RECT   psrctClipRect,
                                                             COORD         coordDestOrigin,
                                                             PCHAR_INFO    pchiFill)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  SMALL_RECT     srctView;            /* the actual rectangle of "happening" */
  SMALL_RECT     srctSource;       /* the clipped source and dest rectangles */
  SMALL_RECT     srctDest;
  int            iX, iY;                             /* scan loop counters */
  PUSHORT        pusTarget, pusSource;      /* pointer to source, dest cells */
  WORD           wAttr;                      /* fill character and attribute */
  int            iBlitDirection;             /* to handle overlapped buffers */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ScrollConsoleScreenBufferA(%08x,%08x,%08x,x=%u y=%u,%08x).\n",
           pHMHandleData,
           psrctSourceRect,
           psrctClipRect,
           coordDestOrigin.X,
           coordDestOrigin.Y,
           pchiFill);
#endif

                                   /* calculate effective clipping rectangle */
  if (psrctClipRect != NULL)          /* if clipping rectangle was specified */
  {
    memcpy(&srctView,
           psrctClipRect,
           sizeof (SMALL_RECT) );

                                          /* check boundary with buffer size */
    srctView.Left   = max(0, srctView.Left);
    srctView.Top    = max(0, srctView.Top );
    srctView.Right  = min(srctView.Right,  pConsoleBuffer->coordBufferSize.X);
    srctView.Bottom = min(srctView.Bottom, pConsoleBuffer->coordBufferSize.Y);
  }
  else
  {
    srctView.Left   = 0;   /* buffer size is the maximum clipping rectangle */
    srctView.Top    = 0;
    srctView.Right  = pConsoleBuffer->coordBufferSize.X;
    srctView.Bottom = pConsoleBuffer->coordBufferSize.Y;
  }

  memcpy(&srctSource,                               /* copy source rectangle */
         psrctSourceRect,
         sizeof (srctSource) );
                                   /* check boundary with clipping rectangle */
  srctSource.Left   = max(srctSource.Left,  srctView.Left  );
  srctSource.Top    = max(srctSource.Top,   srctView.Top   );
  srctSource.Right  = min(srctSource.Right, srctView.Right );
  srctSource.Bottom = min(srctSource.Bottom,srctView.Bottom);

  srctDest.Left  = max(srctView.Left,   coordDestOrigin.X);
  srctDest.Top   = max(srctView.Top,    coordDestOrigin.Y);
  srctDest.Right = min(srctView.Right,  srctDest.Left + srctSource.Right  - srctSource.Left);
  srctDest.Bottom= min(srctView.Bottom, srctDest.Top  + srctSource.Bottom - srctSource.Top);

  /****************************
   * first copy the rectangle *
   ****************************/

  if (srctDest.Left >  srctSource.Left) iBlitDirection  = 0;
  else                                  iBlitDirection  = 1;
  if (srctDest.Top  >  srctSource.Top)  iBlitDirection += 2;

                               /* this leaves us with three different cases: */
                               /*                                            */
                               /* 0 - dest is to upper left of the source    */
                               /* 1 - dest is to upper right of the source   */
                               /* 2 - dest is to lower left of the source    */
                               /* 3 - dest is to lower right of the source   */

  switch (iBlitDirection)
  {
    /**************
     * upper left *
     **************/
    case 0:
      for (iY = 0;
           iY < srctDest.Bottom - srctDest.Top;
           iY++)
      {
                         /* calculate pointer to start of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               (srctDest.Left << 1) );

                         /* calculate pointer to start of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               (srctSource.Left << 1) );

        for (iX = srctDest.Left;
             iX <= srctDest.Right;
             iX++,
             pusTarget++,
             pusSource++)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;

    /***************
     * upper right *
     ***************/
    case 1:
      for (iY = 0;
           iY < srctDest.Bottom - srctDest.Top;
           iY++)
      {
                           /* calculate pointer to end of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               ( srctDest.Right << 1) );

                           /* calculate pointer to end of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               ( srctSource.Right << 1) );

        for (iX = srctDest.Right;
             iX >= srctDest.Left;
             iX--,
             pusTarget--,
             pusSource--)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;

    /***************
     * lower left  *
     ***************/
    case 2:
      for (iY = srctDest.Bottom - srctDest.Top - 1;
           iY >= 0;
           iY--)
      {
                         /* calculate pointer to start of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               (srctDest.Left << 1) );

                         /* calculate pointer to start of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               (srctSource.Left << 1) );

        for (iX = srctDest.Left;
             iX <= srctDest.Right;
             iX++,
             pusTarget++,
             pusSource++)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;

    /****************
     * lower right  *
     ****************/
    case 3:
      for (iY = srctDest.Bottom - srctDest.Top - 1;
           iY >= 0;
           iY--)
      {
                           /* calculate pointer to end of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               ( srctDest.Right << 1) );

                           /* calculate pointer to end of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               (srctSource.Right << 1) );

        for (iX = srctDest.Right;
             iX >= srctDest.Left;
             iX--,
             pusTarget--,
             pusSource--)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;
  }


              /* this is the character and attribute for the uncovered cells */
  wAttr = (pchiFill->Char.AsciiChar) + (pchiFill->Attributes << 8);

  for (iY = srctSource.Top;            /* now fill uncovered area with pchi */
       iY < srctSource.Bottom;
       iY++)
  {
    pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY] + srctSource.Left);

    for (iX = srctSource.Left;
         iX < srctSource.Right;
         iX++,
         pusTarget++)
         /* check if covered by srctDest or not */
      if ( (iY >= srctDest.Top)    &&
           (iY <= srctDest.Bottom) &&
           (iX >= srctDest.Left)   &&
           (iX <= srctDest.Right)
         )
        ;                              /* should be faster for the optimizer */
      else
        *pusTarget = wAttr;            /* write fill character and attribute */
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferW
 * Purpose   : move a block of data within the screen buffer


 * Parameters: PHMHANDLEDATA pHMHandleData   - handle specific data
 *             PSMALL_RECT   psrctSourceRect - source rectangle
 *             PSMALL_RECT   psrctClipRect   - clipping rectangle
 *             COORD         coordDestOrigin - destination coordinate
 *             PCHAR_INFO    pchiFill        - fill character
 * Variables :
 * Result    :
 * Remark    : Routine is subject to optimizations.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferW(PHMHANDLEDATA pHMHandleData,
                                                             PSMALL_RECT   psrctSourceRect,
                                                             PSMALL_RECT   psrctClipRect,
                                                             COORD         coordDestOrigin,
                                                             PCHAR_INFO    pchiFill)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  SMALL_RECT     srctView;            /* the actual rectangle of "happening" */
  SMALL_RECT     srctSource;       /* the clipped source and dest rectangles */
  SMALL_RECT     srctDest;
  ULONG          ulX, ulY;                             /* scan loop counters */
  PUSHORT        pusTarget, pusSource;      /* pointer to source, dest cells */
  WORD           wAttr;                      /* fill character and attribute */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ScrollConsoleScreenBufferW(%08x,%08x,%08x,x=%u y=%u,%08x).\n",
           pHMHandleData,
           psrctSourceRect,
           psrctClipRect,
           coordDestOrigin.X,
           coordDestOrigin.Y,
           pchiFill);
#endif

                                   /* calculate effective clipping rectangle */
  if (psrctClipRect != NULL)          /* if clipping rectangle was specified */
  {
    memcpy(&srctView,
           psrctClipRect,
           sizeof (SMALL_RECT) );

                                          /* check boundary with buffer size */
    srctView.Left   = max(0, srctView.Left);
    srctView.Top    = max(0, srctView.Top );
    srctView.Right  = min(srctView.Right,  pConsoleBuffer->coordBufferSize.X);
    srctView.Bottom = min(srctView.Bottom, pConsoleBuffer->coordBufferSize.Y);
  }
  else
  {
    srctView.Left   = 0;   /* buffer size is the maximum clipping rectangle */
    srctView.Top    = 0;
    srctView.Right  = pConsoleBuffer->coordBufferSize.X;
    srctView.Bottom = pConsoleBuffer->coordBufferSize.Y;
  }

  memcpy(&srctSource,                               /* copy source rectangle */
         psrctSourceRect,
         sizeof (srctSource) );
                                   /* check boundary with clipping rectangle */
  srctSource.Left   = max(srctSource.Left,  srctView.Left  );
  srctSource.Top    = max(srctSource.Top,   srctView.Top   );
  srctSource.Right  = min(srctSource.Right, srctView.Right );
  srctSource.Bottom = min(srctSource.Bottom,srctView.Bottom);

  srctDest.Left  = max(srctView.Left,   coordDestOrigin.X);
  srctDest.Top   = max(srctView.Top,    coordDestOrigin.Y);
  srctDest.Right = min(srctView.Right,  srctDest.Left + srctSource.Right  - srctSource.Left);
  srctDest.Bottom= min(srctView.Bottom, srctDest.Top  + srctSource.Bottom - srctSource.Top);

  /* first copy the rectangle */
  for (ulY = 0;
       ulY < srctDest.Bottom - srctDest.Top;
       ulY++)
  {
                         /* calculate pointer to start of target screen line */
    pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[ulY + srctDest.Top] +
                           srctDest.Left);

                         /* calculate pointer to start of source screen line */
    pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[ulY + srctSource.Top] +
                           srctSource.Left);

    for (ulX = srctDest.Left;
         ulX < srctDest.Right;
         ulX++,
         pusTarget++,
         pusSource++)
      *pusTarget = *pusSource;                             /* copy character */
  }


              /* this is the character and attribute for the uncovered cells */
  /* @@@PH Unicode->Ascii translation */
  wAttr = (pchiFill->Char.UnicodeChar) + (pchiFill->Attributes << 8);

  for (ulY = srctSource.Top;            /* now fill uncovered area with pchi */
       ulY < srctSource.Bottom;
       ulY++)
  {
    pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[ulY] + srctSource.Left);

    for (ulX = srctSource.Left;
         ulX < srctSource.Right;
         ulX++,
         pusTarget++)
      *pusTarget = wAttr;              /* write fill character and attribute */
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleCursorInfo
 * Purpose   : sets the current console's cursor information
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCONSOLE_CURSOR_INFO pCCI
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleCursorInfo(PHMHANDLEDATA        pHMHandleData,
                                                       PCONSOLE_CURSOR_INFO pCCI)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleCursorInfo(%08x,%08x).\n",
           pHMHandleData,
           pCCI);
#endif

                                                       /* validate structure */
  if ( (pCCI->dwSize < 1) ||
       (pCCI->dwSize > 100) )
  {
    SetLastError(ERROR_INVALID_PARAMETER_W);        /* set extended error info */
    return (FALSE);                                            /* API failed */
  }

              /* if we're the active buffer, remove cursor from screen first */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    iConsoleCursorShow(pConsoleBuffer,
                       CONSOLECURSOR_HIDE);

  memcpy(&pConsoleBuffer->CursorInfo,    /* copy the whole information block */
         pCCI,
         sizeof (pConsoleBuffer->CursorInfo) );

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleCursorPosition
 * Purpose   : sets the current console's cursor position
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             COORD         coordCursorPosition
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleCursorPosition(PHMHANDLEDATA pHMHandleData,
                                                           COORD         coordCursorPosition)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleCursorPosition(%08x,x=%u.y=%u).\n",
           pHMHandleData,
           coordCursorPosition.X,
           coordCursorPosition.Y);
#endif

                                  /* @@@PH remove cursor from screen first ! */
  pConsoleBuffer->coordCursorPosition = coordCursorPosition;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleMode
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

DWORD HMDeviceConsoleBufferClass::SetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                                 DWORD         dwMode)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           dwMode);
#endif

  pConsoleBuffer->dwConsoleMode = dwMode;        /* set current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleScreenBufferSize
 * Purpose   : allocate or re-allocate the screenbuffer and transform the
 *             old buffer as required
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             COORD         coordSize     - the new buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleScreenBufferSize (PHMHANDLEDATA pHMHandleData,
                                                              COORD         coordSize)
{
  ULONG ulSize;                         /* calculated size of the new buffer */
  PSZ  *ppszNew;                                 /* pointer to the new array */
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulLine;                                        /* line index counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleScreenBufferSize(%u,%u).\n",
           coordSize.X,
           coordSize.Y);
#endif


                                 /* re-allocate the whole line-pointer array */
  ulSize = coordSize.Y * (coordSize.X * 2 + sizeof (PSZ) );
  if (ulSize == 0)                          /* set new buffer size to zero ? */
  {
    if (pConsoleBuffer->ppszLine != NULL)        /* if old buffer is present */
      free (pConsoleBuffer->ppszLine);                    /* free old buffer */

    pConsoleBuffer->ppszLine          = NULL;
    pConsoleBuffer->coordBufferSize.X = 0;
    pConsoleBuffer->coordBufferSize.Y = 0;
    pConsoleBuffer->coordWindowSize.X = 0;
    pConsoleBuffer->coordWindowSize.Y = 0;
    pConsoleBuffer->coordWindowPosition.X = 0;
    pConsoleBuffer->coordWindowPosition.Y = 0;

    return (TRUE);                                                     /* OK */
  }


  ppszNew = (PSZ *) malloc(ulSize);                        /* allocate array */
  if (ppszNew == NULL)                            /* check proper allocation */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);          /* set error information */
    return (FALSE);                                 /* raise error condition */
  }


  for (ulLine = 0;                               /* setup line pointer array */
       ulLine < coordSize.Y;
       ulLine++)
  {
                                                   /* calculate line pointer */
    ulSize = (ULONG)ppszNew + (coordSize.Y * sizeof(PSZ) )
                            + (coordSize.X * ulLine * 2);
    ppszNew[ulLine] = (PSZ)ulSize;                /* OK, write index pointer */
  }

  ulSize     = ( ((ULONG)(pConsoleBuffer->ucDefaultAttribute) << 8) +
                 ((ULONG)' ') +
                 ((ULONG)(pConsoleBuffer->ucDefaultAttribute) << 24) +
                 ((ULONG)' ' << 16) );

                                                    /* scroll the line index */
  for (ulLine = 0;
       ulLine < coordSize.Y;
       ulLine++)
    iConsoleBufferFillLine(ulSize,
                           (PUSHORT)(ppszNew[ulLine]),
                           coordSize.X);



                                                   /* copy lines as required */
  if (pConsoleBuffer->ppszLine != NULL)         /* previous buffer present ? */
  {
    ULONG x, y;

                                          /* copy old characters as required */
    x = min(pConsoleBuffer->coordBufferSize.X, coordSize.X);
    y = min(pConsoleBuffer->coordBufferSize.Y, coordSize.Y);

    for (ulLine = 0;                                    /* copy line by line */
         ulLine < y;
         ulLine++)
      memcpy(ppszNew[ulLine],
             pConsoleBuffer->ppszLine[ulLine],
             x * 2);

    free (pConsoleBuffer->ppszLine);    /* free previous screen buffer array */
  }


  pConsoleBuffer->ppszLine          = ppszNew;     /* poke in the new values */
  pConsoleBuffer->coordBufferSize.X = coordSize.X;
  pConsoleBuffer->coordBufferSize.Y = coordSize.Y;
  if (flVioConsole) {
        USHORT Row = 0;
        USHORT Column = 0;
        APIRET rc;

        rc = VioGetCurPos(&Row, &Column, 0);
        dprintf(("Current cursor position (%d,%d)", Column, Row));
        pConsoleBuffer->coordCursorPosition.Y = Row;
        pConsoleBuffer->coordCursorPosition.X = Column;
  }
  else {
        pConsoleBuffer->coordCursorPosition.X = 0;
        pConsoleBuffer->coordCursorPosition.Y = 0;
  }

  /* @@@PH to be changed ! */
  pConsoleBuffer->coordWindowSize.X = coordSize.X;                /* default */
  pConsoleBuffer->coordWindowSize.Y = coordSize.Y;
  pConsoleBuffer->coordWindowPosition.X = 0;
  pConsoleBuffer->coordWindowPosition.Y = 0;

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return TRUE;
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
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleTextAttribute(PHMHANDLEDATA pHMHandleData,
                                                          WORD          wAttr)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleTextAttribute(%u).\n",
           wAttr);
#endif

  pConsoleBuffer->ucDefaultAttribute = (UCHAR)wAttr;
  return (TRUE);
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
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleActiveScreenBuffer(PHMHANDLEDATA pHMHandleData)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleActiveScreenBuffer().\n");
#endif

                              /* set new buffer handle to the global console */
  pConsoleGlobals->hConsoleBuffer  = pHMHandleData->hHMHandle;
  pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceConsoleBufferClass::SetConsoleWindowInfo
 * Purpose   : set a new size to the console window
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             BOOL          fAbsolute
 *             PSMALL_RECT   psrctWindowRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

BOOL HMDeviceConsoleBufferClass::SetConsoleWindowInfo(PHMHANDLEDATA pHMHandleData,
                                                      BOOL          fAbsolute,
                                                      PSMALL_RECT   psrctWindowRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleWindowInfo(%08x,%u,%08x).\n",
           pHMHandleData,
           fAbsolute,
           psrctWindowRect);
#endif

  if (fAbsolute == TRUE)                  /* absolute coordinates provided ? */
  {
    if ( (psrctWindowRect->Left   < 0) ||          /* check parameters first */
         (psrctWindowRect->Top    < 0) ||
         (psrctWindowRect->Right  <= psrctWindowRect->Left) ||
         (psrctWindowRect->Bottom <= psrctWindowRect->Top)
       )
    {
      SetLastError(ERROR_INVALID_PARAMETER_W);        /* set error information */
      return (FALSE);                                               /* error */
    }

                                 /* check we don't go beyond screen buffer ! */
    if ( ((psrctWindowRect->Right  - psrctWindowRect->Left) > pConsoleBuffer->coordBufferSize.X) ||
         ((psrctWindowRect->Bottom - psrctWindowRect->Top ) > pConsoleBuffer->coordBufferSize.Y) ||
         (psrctWindowRect->Left >= pConsoleBuffer->coordBufferSize.X) ||
         (psrctWindowRect->Top  >= pConsoleBuffer->coordBufferSize.Y)
       )
    {
      SetLastError(ERROR_INVALID_PARAMETER_W);        /* set error information */
      return (FALSE);                                               /* error */
    }

    pConsoleBuffer->coordWindowSize.X     = psrctWindowRect->Right -
                                            psrctWindowRect->Left;
    pConsoleBuffer->coordWindowSize.Y     = psrctWindowRect->Bottom -
                                            psrctWindowRect->Top;

    pConsoleBuffer->coordWindowPosition.X = psrctWindowRect->Left;
    pConsoleBuffer->coordWindowPosition.Y = psrctWindowRect->Top;
  }
  else
  {
    int iSizeX;                                      /* relative coordinates */
    int iSizeY;
    int iPosX;
    int iPosY;

    iSizeX = pConsoleBuffer->coordWindowSize.X + psrctWindowRect->Left + psrctWindowRect->Right;
    iSizeY = pConsoleBuffer->coordWindowSize.Y + psrctWindowRect->Top  + psrctWindowRect->Bottom;
    iPosX  = pConsoleBuffer->coordWindowPosition.X  + psrctWindowRect->Left;
    iPosY  = pConsoleBuffer->coordWindowPosition.Y  + psrctWindowRect->Top;

                                 /* check we don't go beyond screen buffer ! */
    if ( (iSizeX > pConsoleBuffer->coordBufferSize.X) ||
         (iSizeY > pConsoleBuffer->coordBufferSize.Y) ||
         (iPosX >= pConsoleBuffer->coordBufferSize.X) ||
         (iPosY >= pConsoleBuffer->coordBufferSize.Y) ||
         (iSizeX < 0) ||
         (iSizeY < 0) ||
         (iPosX  < 0) ||
         (iPosY  < 0)
       )
    {
      SetLastError(ERROR_INVALID_PARAMETER_W);        /* set error information */
      return (FALSE);                                               /* error */
    }

                                       /* Values are verified for being OK ! */
    pConsoleBuffer->coordWindowPosition.X  = iPosX;
    pConsoleBuffer->coordWindowPosition.Y  = iPosY;
    pConsoleBuffer->coordWindowSize.X = iSizeX;
    pConsoleBuffer->coordWindowSize.Y = iSizeY;
  }

  /* update window */
  /* @@@PH

   ConsoleWindowResize(COORD coordWindowSize,
                       COORD coordWindowPos,

   */

                                          /* update window contents (scroll) */
                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleA
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

DWORD HMDeviceConsoleBufferClass::WriteConsoleA(PHMHANDLEDATA pHMHandleData,
                                                CONST VOID*   lpvBuffer,
                                                DWORD         cchToWrite,
                                                LPDWORD       lpcchWritten,
                                                LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleA(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToWrite,
           lpcchWritten,
           lpvReserved);
#endif

  /* simply forward the request to that routine */
  return (WriteFile(pHMHandleData, lpvBuffer, cchToWrite, lpcchWritten, NULL, NULL));
}

/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleW
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

DWORD HMDeviceConsoleBufferClass::WriteConsoleW(PHMHANDLEDATA pHMHandleData,
                                                CONST VOID*   lpvBuffer,
                                                DWORD         cchToWrite,
                                                LPDWORD       lpcchWritten,
                                                LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  DWORD          rc;
  LPSTR          pszAscii;
  int            alen;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleW(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToWrite,
           lpcchWritten,
           lpvReserved);
#endif

  alen = WideCharToMultiByte( GetConsoleCP(), 0, (LPCWSTR)lpvBuffer, cchToWrite, 0, 0, 0, 0 );
  /* Ascii -> unicode translation */
  pszAscii = (LPSTR)HEAP_malloc(( alen + 1 ) * sizeof( WCHAR ));
  if (pszAscii == NULL)
     return ERROR_NOT_ENOUGH_MEMORY;

  WideCharToMultiByte( GetConsoleCP(), 0, (LPWSTR)lpvBuffer, cchToWrite, pszAscii, alen, 0, 0 );
  pszAscii[ alen ] = 0;

  /* simply forward the request to that routine */
  rc = WriteFile(pHMHandleData, pszAscii, alen, lpcchWritten, NULL, NULL);

  *lpcchWritten = MultiByteToWideChar( GetConsoleCP(), 0, pszAscii, *lpcchWritten, 0, 0 );
  // free memory again
  HEAP_free(pszAscii);
  return (rc);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputA
 * Purpose   : write character and color attribute data to screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiSrcBuffer
 *             COORD         coordSrcBufferSize
 *             COORD         coordSrcBufferCoord
 *             PSMALL_RECT   psrctDestRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputA(PHMHANDLEDATA pHMHandleData,
                                                      PCHAR_INFO    pchiSrcBuffer,
                                                      COORD         coordSrcBufferSize,
                                                      COORD         coordSrcBufferCoord,
                                                      PSMALL_RECT   psrctDestRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,      ulY;                      /* current coordinate to be read */
  ULONG ulCX,     ulCY;                   /* width and height of target area */
  ULONG ulWriteX, ulWriteY;                    /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;
  PSZ        pszTarget;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputA(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiSrcBuffer,
           coordSrcBufferSize.X,
           coordSrcBufferSize.Y,
           coordSrcBufferCoord.X,
           coordSrcBufferCoord.Y,
           psrctDestRect);
#endif


  /* verify psrctDestRect first */
  psrctDestRect->Left  = max(psrctDestRect->Left,  0);
  psrctDestRect->Top   = max(psrctDestRect->Top,   0);
  psrctDestRect->Right = min(psrctDestRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctDestRect->Bottom= min(psrctDestRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordSrcBufferSize.X < coordSrcBufferCoord.X) ||
       (coordSrcBufferSize.Y < coordSrcBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER_W);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordSrcBufferSize.X - coordSrcBufferCoord.X;
  ulCY = coordSrcBufferSize.Y - coordSrcBufferCoord.Y;

  ulCX = min(ulCX, (psrctDestRect->Right  - psrctDestRect->Left));
  ulCY = min(ulCY, (psrctDestRect->Bottom - psrctDestRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctDestRect->Right  = psrctDestRect->Left + ulCX;
  psrctDestRect->Bottom = psrctDestRect->Top  + ulCY;


  for (ulY = 0,
       ulWriteY = psrctDestRect->Top;

       ulY <= ulCY;

       ulY++,
       ulWriteY++)
  {
    pchi = pchiSrcBuffer + sizeof(CHAR_INFO) * coordSrcBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordSrcBufferCoord.Y + ulY)
                            * coordSrcBufferSize.X;

                                /* calculate pointer to start of screen line */
    pszTarget = pConsoleBuffer->ppszLine[ulWriteY] + ((int)psrctDestRect->Left << 1);

    for (ulX = 0,
         ulWriteX = psrctDestRect->Left;

         ulX <= ulCX;

         ulX++,
         ulWriteX++,
         pchi++)
    {
                                            /* write character and attribute */
      *pszTarget++ = (UCHAR)pchi->Char.AsciiChar;
      *pszTarget++ = (UCHAR)pchi->Attributes;
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputW
 * Purpose   : write character and color attribute data to screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiSrcBuffer
 *             COORD         coordSrcBufferSize
 *             COORD         coordSrcBufferCoord
 *             PSMALL_RECT   psrctDestRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputW(PHMHANDLEDATA pHMHandleData,
                                                      PCHAR_INFO    pchiSrcBuffer,
                                                      COORD         coordSrcBufferSize,
                                                      COORD         coordSrcBufferCoord,
                                                      PSMALL_RECT   psrctDestRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,      ulY;                      /* current coordinate to be read */
  ULONG ulCX,     ulCY;                   /* width and height of target area */
  ULONG ulWriteX, ulWriteY;                    /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;
  PSZ        pszTarget;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputW(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiSrcBuffer,
           coordSrcBufferSize.X,
           coordSrcBufferSize.Y,
           coordSrcBufferCoord.X,
           coordSrcBufferCoord.Y,
           psrctDestRect);
#endif


  /* verify psrctDestRect first */
  psrctDestRect->Left  = max(psrctDestRect->Left,  0);
  psrctDestRect->Top   = max(psrctDestRect->Top,   0);
  psrctDestRect->Right = min(psrctDestRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctDestRect->Bottom= min(psrctDestRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordSrcBufferSize.X < coordSrcBufferCoord.X) ||
       (coordSrcBufferSize.Y < coordSrcBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER_W);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordSrcBufferSize.X - coordSrcBufferCoord.X;
  ulCY = coordSrcBufferSize.Y - coordSrcBufferCoord.Y;

  ulCX = min(ulCX, (psrctDestRect->Right  - psrctDestRect->Left));
  ulCY = min(ulCY, (psrctDestRect->Bottom - psrctDestRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctDestRect->Right  = psrctDestRect->Left + ulCX;
  psrctDestRect->Bottom = psrctDestRect->Top  + ulCY;


  for (ulY = 0,
       ulWriteY = psrctDestRect->Top;

       ulY <= ulCY;

       ulY++,
       ulWriteY++)
  {
    pchi = pchiSrcBuffer + sizeof(CHAR_INFO) * coordSrcBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordSrcBufferCoord.Y + ulY)
                            * coordSrcBufferSize.X;

                                /* calculate pointer to start of screen line */
    pszTarget = pConsoleBuffer->ppszLine[ulWriteY] + ((int)psrctDestRect->Left << 1);

    for (ulX = 0,
         ulWriteX = psrctDestRect->Left;

         ulX <= ulCX;

         ulX++,
         ulWriteX++,
         pchi++)
    {
                                            /* write character and attribute */
      *pszTarget++ = (UCHAR)pchi->Char.UnicodeChar;  /* @@@PH unicode->ascii */
      *pszTarget++ = (UCHAR)pchi->Attributes;
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputAttribute
 * Purpose   : write an array with specified attributes to the console
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

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                                              LPWORD        lpwAttribute,
                                                              DWORD         cWriteCells,
                                                              COORD         dwWriteCoord,
                                                              LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputAttribute(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwAttribute,


           cWriteCells,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpcWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cWriteCells;
       ulCounter++,
       lpwAttribute++)
  {
                                                /* write attribute into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2 + 1)
     ) = (UCHAR)*lpwAttribute;           /* write attribute and skip to next */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcWritten != NULL)                   /* ensure pointer is valid */
          *lpcWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
          pConsoleGlobals->fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpcWritten != NULL)                         /* ensure pointer is valid */
    *lpcWritten = cWriteCells;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterA
 * Purpose   : fills the console buffer with a specified attribute
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPTSTR        lpWriteBuffer
 *             DWORD         cchWrite
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                                               LPTSTR        lpWriteBuffer,
                                                               DWORD         cchWrite,
                                                               COORD         dwWriteCoord,
                                                               LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputCharacterA(%08x,pstr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpWriteBuffer,
           cchWrite,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpcWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the character lines */
  for (ulCounter = 0;
       ulCounter < cchWrite;
       ulCounter++,
       lpWriteBuffer++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = (UCHAR)*lpWriteBuffer;          /* write character and skip to next */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcWritten != NULL)                   /* ensure pointer is valid */
          *lpcWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
          pConsoleGlobals->fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpcWritten != NULL)                         /* ensure pointer is valid */
    *lpcWritten = cchWrite;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterW
 * Purpose   : fills the console buffer with a specified attribute
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWSTR        lpWriteBuffer
 *             DWORD         cchWrite
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                                               LPWSTR        lpWriteBuffer,
                                                               DWORD         cchWrite,
                                                               COORD         dwWriteCoord,
                                                               LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputCharacterW(%08x,pstr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpWriteBuffer,
           cchWrite,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpcWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER_W);
    return (FALSE);
  }


                                        /* OK, now write the character lines */
  for (ulCounter = 0;
       ulCounter < cchWrite;
       ulCounter++,
       lpWriteBuffer++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = (UCHAR)*lpWriteBuffer;          /* write character and skip to next */
                                       /* @@@PH unicode to ascii translation */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcWritten != NULL)                   /* ensure pointer is valid */
          *lpcWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
          pConsoleGlobals->fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHMHandle == pConsoleGlobals->hConsoleBuffer)
    pConsoleGlobals->fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpcWritten != NULL)                         /* ensure pointer is valid */
    *lpcWritten = cchWrite;

  return (TRUE);
}

