/* $Id: driver.cpp,v 1.3 1999-06-10 16:24:33 phaller Exp $ */

/*
 * Driver multimedia apis
 *
 * Copyright 1998 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>

#include <string.h>
#include <misc.h>
#include <unicode.h>

#include "winmm.h"
#include "os2timer.h"


/*****************************************************************************
 * Name      : LRESULT WIN32API SendDriverMessage
 * Purpose   : Sends the specified message to the installable driver
 * Parameters: HDRVR hDrvr
 *             UINT  msg
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

LRESULT WIN32API SendDriverMessage(HDRVR hdrvr,
                                      UINT msg,
                                      LONG lParam1,
                                      LONG lParam2)
{
  dprintf(("WINMM: SendDriverMessage(%08x,%08x,%08x,%08x) not implemented.\n",
           hdrvr,
           msg,
           lParam1,
           lParam2));

  return 0; /* unsuccessful return */
}


/*****************************************************************************
 * Name      : HDRVR WIN32API OpenDriver
 * Purpose   : Opens an instance of an installable driver and initializes
 *             the instance using either the driver's default settings or a
 *             driver-specific value.
 * Parameters: LPCWSTR lpDriverName
 *             LPCWSTR lpSectionName
 *             LONG    lParam
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

HDRVR WIN32API OpenDriver(LPCWSTR lpDriverName,
                             LPCWSTR lpSectionName,
                             LONG    lParam)
{
  dprintf(("WINMM: OpenDriver(%s,%s,%08x) not implemented.\n",
           lpDriverName,
           lpSectionName,
           lParam));

  return 0; /* unsuccessful return */
}


/*****************************************************************************
 * Name      : HDRVR WIN32API OpenDriverA
 * Purpose   : Opens an instance of an installable driver and initializes
 *             the instance using either the driver's default settings or a
 *             driver-specific value.
 * Parameters: LPCTSTR lpDriverName
 *             LPCTSTR lpSectionName
 *             LONG    lParam
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

HDRVR WIN32API OpenDriverA(LPTSTR lpDriverName,
                              LPTSTR lpSectionName,
                              LONG   lParam)
{
  dprintf(("WINMM: OpenDriverA(%s,%s,%08x) not implemented.\n",
           lpDriverName,
           lpSectionName,
           lParam));

  return 0; /* unsuccessful return */
}



/*****************************************************************************
 * Name      : HDRVR WIN32API CloseDriver
 * Purpose   : Closes an installable driver.
 * Parameters: HDRVR hDrvr
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

LRESULT WIN32API CloseDriver(HDRVR hDrvr,
                                LONG  lParam1,
                                LONG  lParam2)
{
  dprintf(("WINMM: CloseDriver(%08x,%08x,%08x) not implemented.\n",
           hDrvr,
           lParam1,
           lParam2));

  return 0; /* unsuccessful return */
}


/*****************************************************************************
 * Name      : LRESULT WIN32API DefDriverProc
 * Purpose   : Provides default processing for any messages not processed by
 *             an installable driver.
 * Parameters: DWORD dwDriverID
 *             HDRVR hDrvr
 *             UINT  msg
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

LRESULT WIN32API DefDriverProc(DWORD dwDriverID,
                                  HDRVR hDrvr,
                                  UINT  msg,
                                  LONG  lParam1,
                                  LONG  lParam2)
{
  dprintf(("WINMM: DefDriverProc(%08x,%08x,%08x,%08x,%08x) not implemented.\n",
           dwDriverID,
           hDrvr,
           msg,
           lParam1,
           lParam2));

  return 0; /* unsuccessful return */
}


/*****************************************************************************
 * Name      : LRESULT WIN32API DriverCallback
 * Purpose   : Calls a callback function, sends a message to a window, or
 *             unblocks a thread.
 * Parameters: DWORD dwCallback
 *             DWORD dwFlags
 *             HDRVR hDrvr
 *             DWORD msg
 *             DWORD dwUser
 *             DWORD dwParam1
 *             DWORD dwParam2
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

LRESULT WIN32API DriverCallback(DWORD dwCallback,
                                   DWORD dwFlags,
                                   HDRVR hDrvr,
                                   DWORD msg,
                                   DWORD dwUser,
                                   DWORD dwParam1,
                                   DWORD dwParam2)
{
  dprintf(("WINMM: DriverCallback(%08x,%08x,%08x,%08x,%08x,%08x,%08x) not implemented.\n",
           dwCallback,
           dwFlags,
           hDrvr,
           msg,
           dwUser,
           dwParam1,
           dwParam2));

  return FALSE; /* unsuccessful return */
}


/*****************************************************************************
 * Name      : HMODULE WIN32API DrvGetModuleHandle
 * Purpose   : Retrieves the instance handle of the module that contains the
 *             installable driver.
 * Parameters: HDRVR hDriver
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

HMODULE WIN32API DrvGetModuleHandle(HDRVR hDriver)
{
  dprintf(("WINMM: DrvGetModuleHandle(%08x) not implemented.\n",
           hDriver));

  return 0; /* unsuccessful return */
}


/*****************************************************************************
 * Name      : LRESULT WIN32API DrvSendMessage
 * Purpose   : Sends the specified message to the installable driver
 * Parameters: HDRVR hDrvr
 *             UINT  msg
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * Result    : API returncode
 * Remark    : Ehm, what's the difference to SendDriverMessage() ?
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

LRESULT WIN32API DrvSendMessage(HDRVR hdrvr,
                                   UINT msg,
                                   LONG lParam1,
                                   LONG lParam2)
{
  dprintf(("WINMM: DrvSendMessage(%08x,%08x,%08x,%08x) not implemented.\n",
           hdrvr,
           msg,
           lParam1,
           lParam2));

  return 0; /* unsuccessful return */
}


/*****************************************************************************
 * Name      : HMODULE WIN32API GetDriverModuleHandle
 * Purpose   : Retrieves the instance handle of the module that contains the
 *             installable driver.
 * Parameters: HDRVR hDriver
 * Variables :
 * Result    :
 * Remark    : What's the difference to DrvGetModuleHandle() ?
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

HMODULE WIN32API GetDriverModuleHandle(HDRVR hDriver)
{
  dprintf(("WINMM: GetDriverModuleHandle(%08x) not implemented.\n",
           hDriver));

  return 0; /* unsuccessful return */
}
