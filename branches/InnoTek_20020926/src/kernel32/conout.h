/* $Id: conout.h,v 1.4 2001-12-05 14:15:58 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Console Subsystem for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#ifndef _CONSOLE_OUT_H_
#define _CONSOLE_OUT_H_


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
#include "Console2.h"

/*****************************************************************************
 * ConsoleOutput - redirects requests to the currently active buffer         *
 *****************************************************************************/

class HMDeviceConsoleOutClass : public HMDeviceHandler
{
  protected:
    HANDLE hConsoleBuffer;            /* handle to the active console buffer */
    PICONSOLEINPUT   pConsoleInput;         /* pointer to console input area */
    PICONSOLEGLOBALS pConsoleGlobals;      /* pointer to console global area */

  public:
    HMDeviceConsoleOutClass(LPCSTR           lpDeviceName,
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

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

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


                                                /* handling non-standard I/O */
  virtual DWORD _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                ULONG         ulRequestCode,
                                ULONG         arg1,
                                ULONG         arg2,
                                ULONG         arg3,
                                ULONG         arg4);

};

#endif /* _CONSOLE_OUT_H_ */

