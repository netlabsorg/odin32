/* $Id: driver.cpp,v 1.5 2000-02-17 14:09:30 sandervl Exp $ */

/*
 * Driver multimedia apis
 *
 * Copyright 1998 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <string.h>
#include <misc.h>
#include <unicode.h>

#include "winmm.h"
#include "os2timer.h"

#define DBG_LOCALLOG	DBG_driver
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-DRIVER)


/*****************************************************************************
 *             Sends the specifiedmessage to the installable driver
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

ODINFUNCTION4(LRESULT, SendDriverMessage,
              HDRVR, hdrvr,
              UINT, msg,
              LONG, lParam1,
              LONG, lParam2)
{
  dprintf(("WINMM: SendDriverMessage not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Opens an instance of an installable driver and initializes
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

ODINFUNCTION3(HDRVR, OpenDriver,
              LPCWSTR, lpDriverName,
              LPCWSTR, lpSectionName,
              LONG, lParam)
{
  dprintf(("WINMM: OpenDriver not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Opens an instance of an installable driver and initializes
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

ODINFUNCTION3(HDRVR, OpenDriverA,
              LPTSTR, lpDriverName,
              LPTSTR, lpSectionName,
              LONG, lParam)
{
  dprintf(("WINMM: OpenDriverA not implemented.\n"));
  return 0; /* unsuccessful return */
}



/*****************************************************************************
 *             Closes an installable driver.
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

ODINFUNCTION3(LRESULT, CloseDriver,
              HDRVR, hDrvr,
              LONG, lParam1,
              LONG, lParam2)
{
  dprintf(("WINMM: CloseDriver not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Provides default processing for anymessages not processed by
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

ODINFUNCTION5(LRESULT, DefDriverProc,
              DWORD, dwDriverID,
              HDRVR, hDrvr,
              UINT, msg,
              LONG, lParam1,
              LONG, lParam2)
{
  dprintf(("WINMM: DefDriverProc not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
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

ODINFUNCTION7(LRESULT, DriverCallback,
              DWORD, dwCallback,
              DWORD, dwFlags,
              HDRVR, hDrvr,
              DWORD, msg,
              DWORD, dwUser,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  dprintf(("WINMM: DriverCallback not implemented.\n"));
  return FALSE; /* unsuccessful return */
}


/*****************************************************************************
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

ODINFUNCTION1(HMODULE, DrvGetModuleHandle,
              HDRVR, hDriver)
{
  dprintf(("WINMM: DrvGetModuleHandle not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Sends the specified message to the installable driver
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

ODINFUNCTION4(LRESULT, DrvSendMessage,
              HDRVR, hdrvr,
              UINT, msg,
              LONG, lParam1,
              LONG, lParam2)
{
  dprintf(("WINMM: DrvSendMessage not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Retrieves the instance handle of the module that contains the
 *             installable driver.
 * Parameters: HDRVR hDriver
 * Variables :
 * Result    :
 * Remark    : What's the difference to DrvGetModuleHandle() ?
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION1(HMODULE, GetDriverModuleHandle,
              HDRVR, hDriver)
{
  dprintf(("WINMM: GetDriverModuleHandle not implemented.\n"));
  return 0; /* unsuccessful return */
}

