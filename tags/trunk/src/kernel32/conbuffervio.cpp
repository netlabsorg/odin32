/* $Id: conbuffervio.cpp,v 1.3 2001-01-23 11:59:45 sandervl Exp $ */

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
#include <stdio.h>

#include "conwin.h"          // Windows Header for console only
#include "HandleManager.h"
#include "HMDevice.h"
#include "ConBuffervio.H"
#include "Console2.h"
#include <heapstring.h>

#define DBG_LOCALLOG    DBG_conbuffer
#include "dbglocal.h"

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

BOOL HMDeviceConsoleVioBufferClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                              LPCVOID       lpBuffer,
                                              DWORD         nNumberOfBytesToWrite,
                                              LPDWORD       lpNumberOfBytesWritten,
                                              LPOVERLAPPED  lpOverlapped)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
           ULONG ulCounter;                 /* counter for the byte transfer */
           PSZ   pszBuffer;
           char  filler[4] = {' ', 0x07, ' ', 0x07};
  register UCHAR ucChar;
          APIRET rc;
          ULONG  Row;
          USHORT Column;
          int    numchar;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleVioBufferClass:WriteFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped);
#endif

  /* check if we're called with non-existing line buffer */
  if (pConsoleBuffer->ppszLine == NULL) {
    SetLastError(ERROR_OUTOFMEMORY_W);
    return FALSE;
  }

  dprintf(("Current cursor position (%d,%d)", pConsoleBuffer->coordCursorPosition.X, pConsoleBuffer->coordCursorPosition.Y));

  if(nNumberOfBytesToWrite > 1024)
  {
    int  tmp = 0;
    BOOL retcode;

    while(nNumberOfBytesToWrite) {
        *lpNumberOfBytesWritten = 0;
        retcode = WriteFile(pHMHandleData, lpBuffer,
                            min(nNumberOfBytesToWrite, 512), lpNumberOfBytesWritten,
                            lpOverlapped);
        if(retcode != TRUE)     break;

        tmp                   += *lpNumberOfBytesWritten;
        nNumberOfBytesToWrite -= *lpNumberOfBytesWritten;
        lpBuffer               = (LPCVOID)((char *)lpBuffer + *lpNumberOfBytesWritten);
    }
    *lpNumberOfBytesWritten = tmp;
    return retcode;
  }
  pszBuffer = (PSZ)alloca(nNumberOfBytesToWrite);
  if(pszBuffer == NULL) {
    DebugInt3();
    return FALSE;
  }
  memcpy(pszBuffer, lpBuffer, nNumberOfBytesToWrite);

  ulCounter = 0;
  while(ulCounter < nNumberOfBytesToWrite)
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
#if 0
        case 8: /* Backspace */
          // not correct if deleting expanded tab character
            rc = VioGetCurPos(&Row, &Column, 0);
            if(!rc) {

            }
          if (pConsoleBuffer->coordCursorPosition.X > 0)
            pConsoleBuffer->coordCursorPosition.X--;

          //@@@PH overwrite old character
          *(pConsoleBuffer->ppszLine[pConsoleBuffer->coordCursorPosition.Y] +
            pConsoleBuffer->coordCursorPosition.X * 2) = 0x20;
          break;

        case 9: /* Tab */
        {
            rc = VioWrite
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
#endif
        case 13: /* CARRIAGE RETURN */
            dprintf(("CR"));
            pConsoleBuffer->coordCursorPosition.X = 0;
            VioSetCurPos(pConsoleBuffer->coordCursorPosition.Y, pConsoleBuffer->coordCursorPosition.X, 0);
            break;

        case 10: /* LINEFEED */
        {
            dprintf(("LF"));
            pConsoleBuffer->coordCursorPosition.Y++;
            pConsoleBuffer->coordCursorPosition.X = 0;
            if(pConsoleBuffer->coordCursorPosition.Y >= pConsoleBuffer->coordWindowSize.Y) {
                dprintf(("scrollup"));
                VioScrollUp(0, 0, pConsoleBuffer->coordWindowSize.Y-1, pConsoleBuffer->coordWindowSize.X-1,
                            1, &filler[0], 0);
                pConsoleBuffer->coordCursorPosition.Y = pConsoleBuffer->coordWindowSize.Y-1;
            }
            VioSetCurPos(pConsoleBuffer->coordCursorPosition.Y, pConsoleBuffer->coordCursorPosition.X, 0);
            break;
        }
        default:
          break;
      }
      ulCounter++;
    }
    else
    {
////        dprintf(("Current cursor position (%d,%d)", pConsoleBuffer->coordCursorPosition.X, pConsoleBuffer->coordCursorPosition.Y));
        numchar = ulCounter;
        while(pszBuffer[numchar] >= 32 && numchar < nNumberOfBytesToWrite) {
            numchar++;
        }
        numchar = numchar - ulCounter;

        if(pConsoleBuffer->coordCursorPosition.X + numchar > pConsoleBuffer->coordWindowSize.X)
        {
            int tmp = pConsoleBuffer->coordWindowSize.X - pConsoleBuffer->coordCursorPosition.X;

            VioWrtCharStr(&pszBuffer[ulCounter], tmp, pConsoleBuffer->coordCursorPosition.Y, pConsoleBuffer->coordCursorPosition.X, 0);
            ulCounter += tmp;
            numchar   -= tmp;

            pConsoleBuffer->coordCursorPosition.X = 0;
            pConsoleBuffer->coordCursorPosition.Y++;
            if(pConsoleBuffer->coordCursorPosition.Y >= pConsoleBuffer->coordWindowSize.Y) {
                dprintf(("scrollup"));
                VioScrollUp(0, 0, pConsoleBuffer->coordWindowSize.Y-1, pConsoleBuffer->coordWindowSize.X-1,
                            1, &filler[0], 0);
                pConsoleBuffer->coordCursorPosition.Y = pConsoleBuffer->coordWindowSize.Y-1;
            }
            VioWrtCharStr(&pszBuffer[ulCounter], numchar, pConsoleBuffer->coordCursorPosition.Y, pConsoleBuffer->coordCursorPosition.X, 0);
            pConsoleBuffer->coordCursorPosition.X += numchar;
            VioSetCurPos(pConsoleBuffer->coordCursorPosition.Y, pConsoleBuffer->coordCursorPosition.X, 0);
        }
        else {
            VioWrtCharStr(&pszBuffer[ulCounter], numchar, pConsoleBuffer->coordCursorPosition.Y, pConsoleBuffer->coordCursorPosition.X, 0);
            pConsoleBuffer->coordCursorPosition.X += numchar;
        }
        ulCounter += numchar;
    }
  }

  *lpNumberOfBytesWritten = ulCounter;

  return TRUE;
}


