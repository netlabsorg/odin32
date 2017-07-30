/* $Id: conin.h,v 1.5 2001-12-05 14:15:58 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Console Subsystem for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#ifndef _CONSOLE_IN_H_
#define _CONSOLE_IN_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Resources                                                                 *
 *****************************************************************************/


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

//#include <win32type.h>
#include "handlemanager.h"
#include "HMDevice.h"
#include "console2.h"


/*****************************************************************************
 * ConsoleInput - manages a queue of window events fro keyboard, mouse, etc. *
 *****************************************************************************/

class HMDeviceConsoleInClass : public HMDeviceHandler
{
  protected:
    /* input queue */
    DWORD dwNumberOfInputEvents;   /* number of unread events in input queue */
    PICONSOLEINPUT   pConsoleInput;         /* pointer to console input area */
    PICONSOLEGLOBALS pConsoleGlobals;      /* pointer to console global area */

  public:
    HMDeviceConsoleInClass(LPCSTR           lpDeviceName,
                           PICONSOLEINPUT   piConsoleInput,
                           PICONSOLEGLOBALS piConsoleGlobals)
      : HMDeviceHandler(lpDeviceName)
    {
      pConsoleInput   = piConsoleInput;          // save pointers to this data
      pConsoleGlobals = piConsoleGlobals;
    }

                       /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

                           /* this is a handler method for calls to ReadFile() */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

                        /* this is a handler method for calls to WriteFile() */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

  virtual DWORD  _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                 ULONG         ulRequestCode,
                                 ULONG         arg1,
                                 ULONG         arg2,
                                 ULONG         arg3,
                                 ULONG         arg4);

  /************************************
   * non-standard console I/O methods *
   ************************************/

  virtual BOOL  FlushConsoleInputBuffer(PHMHANDLEDATA pHMHandleData);

  virtual DWORD GetConsoleMode(PHMHANDLEDATA pHMHandleData,
                               LPDWORD       lpMode);

  virtual BOOL  GetNumberOfConsoleInputEvents(PHMHANDLEDATA pHMHandleData,
                                              LPDWORD       lpNumberOfEvents);

  virtual DWORD PeekConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);

  virtual DWORD PeekConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);

  virtual DWORD ReadConsoleA(PHMHANDLEDATA pHMHandleData,
                             CONST VOID*   lpvBuffer,
                             DWORD         cchToRead,
                             LPDWORD       lpcchRead,
                             LPVOID        lpvReserved);

  virtual DWORD ReadConsoleW(PHMHANDLEDATA pHMHandleData,
                             CONST VOID*   lpvBuffer,
                             DWORD         cchToRead,
                             LPDWORD       lpcchRead,
                             LPVOID        lpvReserved);

  virtual DWORD ReadConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);

  virtual DWORD ReadConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);

  virtual DWORD SetConsoleMode(PHMHANDLEDATA pHMHandleData,
                               DWORD         dwMode);

  virtual DWORD WriteConsoleInputA (PHMHANDLEDATA pHMHandleData,
                                    PINPUT_RECORD pirBuffer,
                                    DWORD         cInRecords,
                                    LPDWORD       lpcWritten);

  virtual DWORD WriteConsoleInputW (PHMHANDLEDATA pHMHandleData,
                                    PINPUT_RECORD pirBuffer,
                                    DWORD         cInRecords,
                                    LPDWORD       lpcWritten);

};

#endif /* _CONSOLE_IN_H_ */

