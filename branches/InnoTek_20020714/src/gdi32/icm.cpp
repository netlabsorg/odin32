/* $Id: icm.cpp,v 1.2 2001-09-09 12:24:13 sandervl Exp $ */

/*
 * GDI32 Color Management apis
 *
 * Copyright 1998-2001 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>
#include "callback.h"
#include "unicode.h"
#include "dibsect.h"
#include <codepage.h>
#include "oslibgpi.h"
#include "oslibgdi.h"
#include <dcdata.h>
#include <winuser32.h>

#define DBG_LOCALLOG    DBG_gdi32
#include "dbglocal.h"

ODINDEBUGCHANNEL(GDI32-ICM)


//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteColorSpace(HCOLORSPACE hColorSpace)
{
        dprintf(("GDI32: DeleteColorSpace - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetColorSpace(HDC hdc, HCOLORSPACE hColorSpace)
{
        dprintf(("GDI32: SetColorSpace - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
HCOLORSPACE WIN32API CreateColorSpaceA(LPLOGCOLORSPACEA lpLogColorSpace)
{
        dprintf(("GDI32: CreateColorSpaceA - stub\n"));
        return 0;
}
//******************************************************************************
//******************************************************************************
HCOLORSPACE WIN32API CreateColorSpaceW(LPLOGCOLORSPACEW lpwLogColorSpace)
{
        dprintf(("GDI32: CreateColorSpaceW - stub\n"));
        return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetColorSpace(HDC hdc)
{
        dprintf(("GDI32: GetColorSpace - stub\n"));
        return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API SetICMMode(HDC hdc, int mode)
{
        dprintf(("GDI32: SetICMMode - stub\n"));
        return 0;
}
//******************************************************************************



/*****************************************************************************
 * Name      : BOOL CheckColorsInGamut
 * Purpose   : The CheckColorsInGamut function indicates whether the specified
 *             color values are within the gamut of the specified device.
 * Parameters: HDC    hdc        handle of device context
 *             LPVOID lpaRGBQuad
 *             LPVOID lpResult
 *             DWORD  dwResult
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CheckColorsInGamut(HDC    hdc,
                                    LPVOID lpaRGBQuad,
                                    LPVOID lpResult,
                                    DWORD  dwResult)
{
  dprintf(("GDI32: CheckColorsInGamut(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hdc,
           lpaRGBQuad,
           lpResult,
           dwResult));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL ColorMatchToTarget
 * Purpose   : The ColorMatchToTarget function enables or disables preview for
 *             the specified device context. When preview is enabled, colors
 *             in subsequent output to the specified device context are
 *             displayed as they would appear on the target device. This is
 *             useful for checking how well the target maps the specified
 *             colors in an image. To enable preview, image color matching
 *             must be enabled for both the target and the preview device context.
 * Parameters: HDC    hdc        handle of device context
 *             HDC    hdcTarget  handle of target device context
 *             DWORD  uiAction
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API ColorMatchToTarget(HDC   hdc,
                                    HDC   hdcTarget,
                                    DWORD uiAction)
{
  dprintf(("GDI32: ColorMatchToTarget(%08xh,%08xh,%08xh) not implemented.\n",
           hdc,
           hdcTarget,
           uiAction));

  return (FALSE);
}


/*****************************************************************************
 * Name      : int EnumICMProfilesA
 * Purpose   : The EnumICMProfilesA function enumerates the different color
 *             profiles that the system supports for the specified device context.
 * Parameters: HDC         hdc
 *             ICMENUMPROC lpICMEnumFunc
 *             LPARAM      lParam
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API EnumICMProfilesA(HDC         hdc,
                                 ICMENUMPROCA lpICMEnumProc,
                                 LPARAM      lParam)
{
  dprintf(("GDI32: EnumICMProfilesA(%08xh, %08xh, %08xh) not implemented(-1).\n",
           hdc,
           lpICMEnumProc,
           lParam));

  return (-1);
}


/*****************************************************************************
 * Name      : int EnumICMProfilesW
 * Purpose   : The EnumICMProfilesW function enumerates the different color
 *             profiles that the system supports for the specified device context.
 * Parameters: HDC         hdc
 *             ICMENUMPROC lpICMEnumFunc
 *             LPARAM      lParam
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API EnumICMProfilesW(HDC          hdc,
                              ICMENUMPROCW lpICMEnumProc,
                              LPARAM       lParam)
{
  dprintf(("GDI32: EnumICMProfilesW(%08xh, %08xh, %08xh) not implemented (-1).\n",
           hdc,
           lpICMEnumProc,
           lParam));

  return (-1);
}



/*****************************************************************************
 * Name      : DWORD GetDeviceGammaRamp
 * Purpose   : The GetDeviceGammaRamp function retrieves the gamma ramp on
 *             direct color display boards.
 * Parameters: HDC     hdc        handle to device context
 *             LPVOID  lpRamp     Gamma ramp array
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetDeviceGammaRamp(HDC    hdc,
                                     LPVOID lpRamp)
{
  dprintf(("GDI32: GetDeviceGammaRamp(%08xh, %08xh) not implemented.\n",
           hdc,
           lpRamp));

  return (FALSE);
}




/*****************************************************************************
 * Name      : BOOL GetICMProfileA
 * Purpose   : The GetICMProfileA function retrieves the name of the color
 *             profile file for the device associated with the specified device
 *             context.
 * Parameters: HDC     hdc        handle to device context
 *             DWORD   cbName
 *             LPTSTR  lpszFilename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetICMProfileA(HDC    hdc,
                                DWORD  cbName,
                                LPTSTR lpszFilename)
{
  dprintf(("GDI32: GetICMProfileA(%08xh, %08xh, %08xh) not implemented.\n",
           hdc,
           cbName,
           lpszFilename));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetICMProfileW
 * Purpose   : The GetICMProfileW function retrieves the name of the color
 *             profile file for the device associated with the specified device
 *             context.
 * Parameters: HDC     hdc        handle to device context
 *             DWORD   cbName
 *             LPWSTR  lpszFilename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetICMProfileW(HDC    hdc,
                                DWORD  cbName,
                                LPTSTR lpszFilename)
{
  dprintf(("GDI32: GetICMProfileW(%08xh, %08xh, %08xh) not implemented.\n",
           hdc,
           cbName,
           lpszFilename));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetLogColorSpaceA
 * Purpose   : The GetLogColorSpace function retrieves information about the
 *             logical color space identified by the specified handle.
 * Parameters: HCOLORSPACE     hColorSpace
 *             LPLOGCOLORSPACE lpbuffer
 *             DWORD           nSize
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

#define LPLOGCOLORSPACE LPVOID
BOOL WIN32API GetLogColorSpaceA(HCOLORSPACE     hColorSpace,
                                   LPLOGCOLORSPACE lpBuffer,
                                   DWORD           nSize)
{
  dprintf(("GDI32: GetLogColorSpaceA(%08xh, %08xh, %08xh) not implemented.\n",
           hColorSpace,
           lpBuffer,
           nSize));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetLogColorSpaceW
 * Purpose   : The GetLogColorSpace function retrieves information about the
 *             logical color space identified by the specified handle.
 * Parameters: HCOLORSPACE     hColorSpace
 *             LPLOGCOLORSPACE lpbuffer
 *             DWORD           nSize
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetLogColorSpaceW(HCOLORSPACE     hColorSpace,
                                   LPLOGCOLORSPACE lpBuffer,
                                   DWORD           nSize)
{
  dprintf(("GDI32: GetLogColorSpaceW(%08xh, %08xh, %08xh) not implemented.\n",
           hColorSpace,
           lpBuffer,
           nSize));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetDeviceGammaRamp
 * Purpose   : The SetDeviceGammaRamp function sets the gamma ramp on direct
 *             color display boards.
 * Parameters: HDC    hdc   handle of device context
 *             LPVOID lpRamp
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetDeviceGammaRamp(HDC    hdc,
                                    LPVOID lpRamp)
{
  dprintf(("GDI32: SetDeviceGammaRamp(%08xh, %08xh) not implemented.\n",
           hdc,
           lpRamp));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetICMProfileA
 * Purpose   : The SetICMProfileA function sets the color profile for the
 *             specified device context.
 * Parameters: HDC    hdc   handle of device context
 *             LPTSTR lpFileName
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetICMProfileA(HDC    hdc,
                                LPTSTR lpFileName)
{
  dprintf(("GDI32: SetICMProfileA(%08xh, %s) not implemented.\n",
           hdc,
           lpFileName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetICMProfileW
 * Purpose   : The SetICMProfileW function sets the color profile for the
 *             specified device context.
 * Parameters: HDC    hdc   handle of device context
 *             LPTSTR lpFileName
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetICMProfileW(HDC    hdc,
                                LPWSTR lpFileName)
{
  dprintf(("GDI32: SetICMProfileW(%08xh, %s) not implemented.\n",
           hdc,
           lpFileName));

  return (FALSE);
}



/*****************************************************************************
 * Name      : BOOL UpdateICMRegKeyA
 * Purpose   : The UpdateICMRegKeyA function installs, removes, or queries
 *             registry entries that identify ICC color profiles or color-matching
 *             DLLs. The function carries out the action specified by the nCommand
 *             parameter.
 * Parameters: DWORD   dwReserved
 *             DWORD   CMID
 *             LPTSTR  lpszFileName
 *             UINT    nCommand
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UpdateICMRegKeyA(DWORD  dwReserved,
                                  DWORD  CMID,
                                  LPTSTR lpszFileName,
                                  UINT   nCommand)
{
  dprintf(("GDI32: UpdateICMRegKeyA(%08xh, %08xh, %08xh, %08xh) not implemented.\n",
           dwReserved,
           CMID,
           lpszFileName,
           nCommand));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL UpdateICMRegKeyW
 * Purpose   : The UpdateICMRegKeyW function installs, removes, or queries
 *             registry entries that identify ICC color profiles or color-matching
 *             DLLs. The function carries out the action specified by the nCommand
 *             parameter.
 * Parameters: DWORD   dwReserved
 *             DWORD   CMID
 *             LPWSTR  lpszFileName
 *             UINT    nCommand
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UpdateICMRegKeyW(DWORD  dwReserved,
                                  DWORD  CMID,
                                  LPWSTR lpszFileName,
                                  UINT   nCommand)
{
  dprintf(("GDI32: UpdateICMRegKeyW(%08xh, %08xh, %08xh, %08xh) not implemented.\n",
           dwReserved,
           CMID,
           lpszFileName,
           nCommand));

  return (FALSE);
}



