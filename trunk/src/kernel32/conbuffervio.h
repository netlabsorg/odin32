/* $Id: conbuffervio.h,v 1.1 2000-10-20 11:46:45 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Console Subsystem for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#ifndef _CONSOLE_BUFFERVIO_H_
#define _CONSOLE_BUFFERVIO_H_


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
#include "conbuffer.h"


/*****************************************************************************
 * ConsoleBuffer - does all the writing to an internal line buffer array     *
 *                 if this buffer is the active one, also does a screen updt *
 *****************************************************************************/

class HMDeviceConsoleVioBufferClass : public HMDeviceConsoleBufferClass
{
  public:
  HMDeviceConsoleVioBufferClass(LPCSTR           lpDeviceName,
                                PICONSOLEINPUT   piConsoleInput,
                                PICONSOLEGLOBALS piConsoleGlobals)
     : HMDeviceConsoleBufferClass(lpDeviceName, piConsoleInput, piConsoleGlobals)
  {
  };

  /* this is a handler method for calls to WriteFile() */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped);

};

#endif /* _CONSOLE_BUFFERVIO_H_ */

