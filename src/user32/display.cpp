/* $Id: display.cpp,v 1.11 2001-06-17 21:08:00 sandervl Exp $ */
/*
 * Display/Monitor Win32 apis
 *
 * Based on Wine code (991031)
 *
 * Copyright 1993 Robert J. Amstadt
 *           1996 Alex Korobka
 * Copyright 1998 Turchanov Sergey
 *
 * Copyright 1999 Christoph Bratschi
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <string.h>
#include <heapstring.h>
#include "pmwindow.h"
#include "monitor.h"
#include "windef.h"

#define DBG_LOCALLOG	DBG_display
#include "dbglocal.h"

#define NRMODES 5
#define NRDEPTHS 4
struct {
        int w,h;
} modes[NRMODES]={{512,384},{640,400},{640,480},{800,600},{1024,768}};
int depths[4] = {8,16,24,32};


/**********************************************************************/

#define xPRIMARY_MONITOR ((HMONITOR)0x12340042)

MONITOR MONITOR_PrimaryMonitor;

/* PM Monitor */

/***********************************************************************
 *              PMDRV_MONITOR_Initialize
 */
void PMDRV_MONITOR_Initialize(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_Initialize"));

  pMonitor->pDriverData = NULL;
}

/***********************************************************************
 *              PMDRV_MONITOR_Finalize
 */
void PMDRV_MONITOR_Finalize(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_Finalize"));
}

/***********************************************************************
 *              PMDRV_MONITOR_IsSingleWindow
 */
BOOL PMDRV_MONITOR_IsSingleWindow(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_IsSingleWindow"));

  return TRUE; //CB: right???
}

/***********************************************************************
 *              PMDRV_MONITOR_GetWidth
 *
 * Return the width of the monitor
 */
int PMDRV_MONITOR_GetWidth(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_GetWidth"));

  return GetSystemMetrics(SM_CXSCREEN);
}

/***********************************************************************
 *              PMDRV_MONITOR_GetHeight
 *
 * Return the height of the monitor
 */
int PMDRV_MONITOR_GetHeight(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_GetHeight"));

  return GetSystemMetrics(SM_CYSCREEN);
}

/***********************************************************************
 *              PMDRV_MONITOR_GetDepth
 *
 * Return the depth of the monitor
 */
int PMDRV_MONITOR_GetDepth(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_GetDepth"));

  return 24; //CB: change, right???
}

/***********************************************************************
 *              PMDRV_MONITOR_GetScreenSaveActive
 *
 * Returns the active status of the screen saver
 */
BOOL PMDRV_MONITOR_GetScreenSaveActive(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_GetScreenSaveActive"));

  return FALSE;
}

/***********************************************************************
 *              PMDRV_MONITOR_SetScreenSaveActive
 *
 * Activate/Deactivate the screen saver
 */
void PMDRV_MONITOR_SetScreenSaveActive(MONITOR *pMonitor, BOOL bActivate)
{
  dprintf(("MONITOR: PMDRV_SetScreenSaveActive"));
}

/***********************************************************************
 *              PMDRV_MONITOR_GetScreenSaveTimeout
 *
 * Return the screen saver timeout
 */
int PMDRV_MONITOR_GetScreenSaveTimeout(MONITOR *pMonitor)
{
  dprintf(("MONITOR: PMDRV_GetScreenSaveTimeout"));

  return 60*1000; //CB: stub
}

/***********************************************************************
 *              PMDRV_MONITOR_SetScreenSaveTimeout
 *
 * Set the screen saver timeout
 */
void PMDRV_MONITOR_SetScreenSaveTimeout(
  MONITOR *pMonitor, int nTimeout)
{
  dprintf(("MONITOR: PMDRV_SetScreenSaveTimeout"));
}

MONITOR_DRIVER PM_MONITOR_Driver =
{
  PMDRV_MONITOR_Initialize,
  PMDRV_MONITOR_Finalize,
  PMDRV_MONITOR_IsSingleWindow,
  PMDRV_MONITOR_GetWidth,
  PMDRV_MONITOR_GetHeight,
  PMDRV_MONITOR_GetDepth,
  PMDRV_MONITOR_GetScreenSaveActive,
  PMDRV_MONITOR_SetScreenSaveActive,
  PMDRV_MONITOR_GetScreenSaveTimeout,
  PMDRV_MONITOR_SetScreenSaveTimeout
};

MONITOR_DRIVER *MONITOR_Driver = &PM_MONITOR_Driver;

/***********************************************************************
 *              MONITOR_GetMonitor
 */
static MONITOR *MONITOR_GetMonitor(HMONITOR hMonitor)
{
  if(hMonitor == xPRIMARY_MONITOR)
    {
      return &MONITOR_PrimaryMonitor;
    }
  else
    {
      return NULL;
    }
}

/***********************************************************************
 *              MONITOR_Initialize
 */
void MONITOR_Initialize(MONITOR *pMonitor)
{
  MONITOR_Driver->pInitialize(pMonitor);
}

/***********************************************************************
 *              MONITOR_Finalize
 */
void MONITOR_Finalize(MONITOR *pMonitor)
{
  MONITOR_Driver->pFinalize(pMonitor);
}

/***********************************************************************
 *              MONITOR_IsSingleWindow
 */
BOOL MONITOR_IsSingleWindow(MONITOR *pMonitor)
{
  return MONITOR_Driver->pIsSingleWindow(pMonitor);
}

/***********************************************************************
 *              MONITOR_GetWidth
 */
int MONITOR_GetWidth(MONITOR *pMonitor)
{
  return MONITOR_Driver->pGetWidth(pMonitor);
}

/***********************************************************************
 *              MONITOR_GetHeight
 */
int MONITOR_GetHeight(MONITOR *pMonitor)
{
  return MONITOR_Driver->pGetHeight(pMonitor);
}

/***********************************************************************
 *              MONITOR_GetDepth
 */
int MONITOR_GetDepth(MONITOR *pMonitor)
{
  return MONITOR_Driver->pGetDepth(pMonitor);
}

/***********************************************************************
 *              MONITOR_GetScreenSaveActive
 */
BOOL MONITOR_GetScreenSaveActive(MONITOR *pMonitor)
{
  return MONITOR_Driver->pGetScreenSaveActive(pMonitor);
}

/***********************************************************************
 *              MONITOR_SetScreenSaveActive
 */
void MONITOR_SetScreenSaveActive(MONITOR *pMonitor, BOOL bActivate)
{
  MONITOR_Driver->pSetScreenSaveActive(pMonitor, bActivate);
}

/***********************************************************************
 *              MONITOR_GetScreenSaveTimeout
 */
int MONITOR_GetScreenSaveTimeout(MONITOR *pMonitor)
{
  return MONITOR_Driver->pGetScreenSaveTimeout(pMonitor);
}

/***********************************************************************
 *              MONITOR_SetScreenSaveTimeout
 */
void MONITOR_SetScreenSaveTimeout(MONITOR *pMonitor, int nTimeout)
{
  MONITOR_Driver->pSetScreenSaveTimeout(pMonitor, nTimeout);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumDisplaySettingsA
 * Purpose   : The EnumDisplaySettings function obtains information about one
 *             of a display device's graphics modes. You can obtain information
 *             for all of a display device's graphics modes by making a series
 *             of calls to this function.
 * Parameters: LPCTSTR   lpszDeviceName specifies the display device
 *             DWORD     iModeNum       specifies the graphics mode
 *             LPDEVMODE lpDevMode      points to structure to receive settings
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    : Wine Port (991031)
 * Status    :
 *
 * Author    :
 *****************************************************************************/
BOOL WIN32API EnumDisplaySettingsA(
        LPCSTR name,            /* [in] huh? */
        DWORD n,                /* [in] nth entry in display settings list*/
        LPDEVMODEA devmode)     /* [out] devmode for that setting */
{
        dprintf(("USER32: EnumDisplaySettingsA %s %d %x", name, n, devmode));
	if(devmode == NULL) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	//SvL: VMWare calls this with -1; valid in NT4, SP6
        //     Other negative numbers too. Don't know what they mean.
	if(n == 0xFFFFFFFF) {
		n = 0;
	}
	memset(devmode, 0, sizeof(*devmode));
	devmode->dmSize = sizeof(*devmode);
        devmode->dmDisplayFrequency = 70; //todo: get real refresh rate
        if(n==0) {
                devmode->dmBitsPerPel = ScreenBitsPerPel;
                devmode->dmPelsHeight = ScreenHeight;
                devmode->dmPelsWidth = ScreenWidth;
                return TRUE;
        }
        if ((n-1)<NRMODES*NRDEPTHS) {
                devmode->dmBitsPerPel   = depths[(n-1)/NRMODES];
                devmode->dmPelsHeight   = modes[(n-1)%NRMODES].h;
                devmode->dmPelsWidth    = modes[(n-1)%NRMODES].w;
                return TRUE;
        }
 	SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumDisplaySettingsW
 * Purpose   : The EnumDisplaySettings function obtains information about one
 *             of a display device's graphics modes. You can obtain information
 *             for all of a display device's graphics modes by making a series
 *             of calls to this function.
 * Parameters: LPCTSTR   lpszDeviceName specifies the display device
 *             DWORD     iModeNum       specifies the graphics mode
 *             LPDEVMODE lpDevMode      points to structure to receive settings
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    : Wine Port (991031)
 * Status    :
 *
 * Author    :
 *****************************************************************************/
BOOL WIN32API EnumDisplaySettingsW(LPCWSTR name,DWORD n,LPDEVMODEW devmode)
{
 LPSTR    nameA = NULL;
 DEVMODEA devmodeA;
 BOOL     ret;

        if(name) {
                nameA = HEAP_strdupWtoA(GetProcessHeap(),0,name);
        }
        ret = EnumDisplaySettingsA(nameA,n,&devmodeA);

        if (ret) {
                devmode->dmBitsPerPel   = devmodeA.dmBitsPerPel;
                devmode->dmPelsHeight   = devmodeA.dmPelsHeight;
                devmode->dmPelsWidth    = devmodeA.dmPelsWidth;
                /* FIXME: convert rest too, if they are ever returned */
        }
        if(name)
                HeapFree(GetProcessHeap(),0,nameA);
        return ret;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API ChangeDisplaySettingsA(LPDEVMODEA  lpDevMode, DWORD dwFlags)
{
    // lpDevMode might be NULL when change to default desktop mode
    // is being requested, this was the cause of trap
    if ( !lpDevMode )
    {
        return(DISP_CHANGE_SUCCESSFUL);
    }
    if(lpDevMode) {
        dprintf(("USER32:  ChangeDisplaySettingsA FAKED %X\n", dwFlags));
        dprintf(("USER32:  ChangeDisplaySettingsA lpDevMode->dmBitsPerPel %d\n", lpDevMode->dmBitsPerPel));
        dprintf(("USER32:  ChangeDisplaySettingsA lpDevMode->dmPelsWidth  %d\n", lpDevMode->dmPelsWidth));
        dprintf(("USER32:  ChangeDisplaySettingsA lpDevMode->dmPelsHeight %d\n", lpDevMode->dmPelsHeight));
    }
    return(DISP_CHANGE_SUCCESSFUL);
}
/*****************************************************************************
 * Name      : LONG WIN32API ChangeDisplaySettingsW
 * Purpose   : The ChangeDisplaySettings function changes the display settings
 *             to the specified graphics mode.
 * Parameters: LPDEVMODEW lpDevModeW
 *             DWORD      dwFlags
 * Variables :
 * Result    : DISP_CHANGE_SUCCESSFUL The settings change was successful.
 *             DISP_CHANGE_RESTART    The computer must be restarted in order for the graphics mode to work.
 *             DISP_CHANGE_BADFLAGS   An invalid set of flags was passed in.
 *             DISP_CHANGE_FAILED     The display driver failed the specified graphics mode.
 *             DISP_CHANGE_BADMODE    The graphics mode is not supported.
 *             DISP_CHANGE_NOTUPDATED Unable to write settings to the registry.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
LONG WIN32API ChangeDisplaySettingsW(LPDEVMODEW lpDevMode,
                                        DWORD      dwFlags)
{
  dprintf(("USER32:ChangeDisplaySettingsW(%08xh,%08x) not implemented.\n",
         lpDevMode,
         dwFlags));

  return (ChangeDisplaySettingsA((LPDEVMODEA)lpDevMode,
                                  dwFlags));
}
//******************************************************************************
//******************************************************************************
LONG WIN32API ChangeDisplaySettingsExA(LPCSTR devname, LPDEVMODEA lpDevMode,
                                       HWND hwnd, DWORD dwFlags, LPARAM lparam)
{
    // lpDevMode might be NULL when change to default desktop mode
    // is being requested, this was the cause of trap
    if ( !lpDevMode )
    {
        return(DISP_CHANGE_SUCCESSFUL);
    }
    if(lpDevMode) {
        dprintf(("USER32:  ChangeDisplaySettingsExA FAKED %X\n", dwFlags));
        dprintf(("USER32:  ChangeDisplaySettingsExA lpDevMode->dmBitsPerPel %d\n", lpDevMode->dmBitsPerPel));
        dprintf(("USER32:  ChangeDisplaySettingsExA lpDevMode->dmPelsWidth  %d\n", lpDevMode->dmPelsWidth));
        dprintf(("USER32:  ChangeDisplaySettingsExA lpDevMode->dmPelsHeight %d\n", lpDevMode->dmPelsHeight));
    }
    return(DISP_CHANGE_SUCCESSFUL);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API ChangeDisplaySettingsExW(LPCWSTR devname, LPDEVMODEW lpDevMode,
                                       HWND hwnd, DWORD dwFlags, LPARAM lparam)
{
  dprintf(("USER32:ChangeDisplaySettingsExW(%08xh,%08x) NOT implemented.\n",
         lpDevMode,
         dwFlags));

  //TODO: Need unicode translation
  return (ChangeDisplaySettingsExA((LPCSTR)devname, (LPDEVMODEA)lpDevMode,
                                   hwnd, dwFlags, lparam));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMonitorInfoA(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
{
    RECT rcWork;

    dprintf(("USER32: GetMonitorInfoA %x %x", hMonitor, lpMonitorInfo));

    if ((hMonitor == xPRIMARY_MONITOR) &&
        lpMonitorInfo &&
        (lpMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
        SystemParametersInfoA(SPI_GETWORKAREA, 0, &rcWork, 0))
    {
        lpMonitorInfo->rcMonitor.left = 0;
        lpMonitorInfo->rcMonitor.top  = 0;
        lpMonitorInfo->rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
        lpMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        lpMonitorInfo->rcWork = rcWork;
        lpMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;

	if (lpMonitorInfo->cbSize >= sizeof(MONITORINFOEXA))
            lstrcpyA(((MONITORINFOEXA*)lpMonitorInfo)->szDevice, "DISPLAY");

        return TRUE;
    }

    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMonitorInfoW(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
{
    RECT rcWork;

    dprintf(("USER32: GetMonitorInfoW %x %x", hMonitor, lpMonitorInfo));

    if ((hMonitor == xPRIMARY_MONITOR) &&
        lpMonitorInfo &&
        (lpMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0))
    {
        lpMonitorInfo->rcMonitor.left = 0;
        lpMonitorInfo->rcMonitor.top  = 0;
        lpMonitorInfo->rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
        lpMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        lpMonitorInfo->rcWork = rcWork;
        lpMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;

        if (lpMonitorInfo->cbSize >= sizeof(MONITORINFOEXW))
            lstrcpyW(((MONITORINFOEXW*)lpMonitorInfo)->szDevice, (LPCWSTR)"D\0I\0S\0P\0L\0A\0Y\0\0");

        return TRUE;
    }

    return FALSE;
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromWindow(HWND hWnd, DWORD dwFlags)
{
   WINDOWPLACEMENT wp;

    dprintf(("USER32: MonitorFromWindow %x %x", hWnd, dwFlags));

    if (dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST))
        return xPRIMARY_MONITOR;

    if (IsIconic(hWnd) ?
            GetWindowPlacement(hWnd, &wp) :
            GetWindowRect(hWnd, &wp.rcNormalPosition)) {

        return MonitorFromRect(&wp.rcNormalPosition, dwFlags);
    }

    dprintf(("USER32: MonitorFromWindow failed"));
    return NULL;
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromRect(LPRECT lprcScreenCoords, DWORD dwFlags)
{
    dprintf(("USER32: MonitorFromRect (%d,%d)(%d,%d) %x", lprcScreenCoords->left, lprcScreenCoords->top, lprcScreenCoords->right, lprcScreenCoords->bottom, dwFlags));

      if ((dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST)) ||
        ((lprcScreenCoords->right > 0) &&
        (lprcScreenCoords->bottom > 0) &&
        (lprcScreenCoords->left < GetSystemMetrics(SM_CXSCREEN)) &&
        (lprcScreenCoords->top < GetSystemMetrics(SM_CYSCREEN))))
    {
        return xPRIMARY_MONITOR;
    }
    dprintf(("USER32: MonitorFromRect failed"));
    return NULL;
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromPoint(POINT ptScreenCoords, DWORD dwFlags)
{
  dprintf(("USER32: MonitorFromPoint (%d,%d) %x", ptScreenCoords.x, ptScreenCoords.y, dwFlags));

  if ((dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST)) ||
      ((ptScreenCoords.x >= 0) &&
      (ptScreenCoords.x < GetSystemMetrics(SM_CXSCREEN)) &&
      (ptScreenCoords.y >= 0) &&
      (ptScreenCoords.y < GetSystemMetrics(SM_CYSCREEN))))
  {
    return xPRIMARY_MONITOR;
  }

  dprintf(("USER32: MonitorFromPoint failed"));
  return NULL;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumDisplayMonitors(
        HDC             hdcOptionalForPainting,
        LPRECT         lprcEnumMonitorsThatIntersect,
        MONITORENUMPROC lpfnEnumProc,
        LPARAM          dwData)
{
    RECT rcLimit;

    dprintf(("USER32: EnumDisplayMonitors %x %x %x %x", hdcOptionalForPainting, lprcEnumMonitorsThatIntersect, lpfnEnumProc, dwData));

    if (!lpfnEnumProc)
        return FALSE;

    rcLimit.left   = 0;
    rcLimit.top    = 0;
    rcLimit.right  = GetSystemMetrics(SM_CXSCREEN);
    rcLimit.bottom = GetSystemMetrics(SM_CYSCREEN);

    if (hdcOptionalForPainting)
    {
        RECT    rcClip;
        POINT   ptOrg;

        switch (GetClipBox(hdcOptionalForPainting, &rcClip))
        {
        default:
            if (!GetDCOrgEx(hdcOptionalForPainting, &ptOrg))
                return FALSE;

            OffsetRect(&rcLimit, -ptOrg.x, -ptOrg.y);
            if (IntersectRect(&rcLimit, &rcLimit, &rcClip) &&
                (!lprcEnumMonitorsThatIntersect ||
                     IntersectRect(&rcLimit, &rcLimit, lprcEnumMonitorsThatIntersect))) {

                break;
            }
            /*fall thru */
        case NULLREGION:
             return TRUE;
        case ERROR:
             return FALSE;
        }
    } else {
        if (    lprcEnumMonitorsThatIntersect &&
                !IntersectRect(&rcLimit, &rcLimit, lprcEnumMonitorsThatIntersect)) {

            return TRUE;
        }
    }

    return lpfnEnumProc(
            xPRIMARY_MONITOR,
            hdcOptionalForPainting,
            &rcLimit,
            dwData);
}
/***********************************************************************
 *           EnumDisplayDevicesA   (USER32.184)
 */
BOOL WINAPI EnumDisplayDevicesA(
	LPVOID unused,DWORD i,LPDISPLAY_DEVICEA lpDisplayDevice,DWORD dwFlags) 
{
        dprintf(("EnumDisplayDevicesA: %x %d %x %x", unused, i, lpDisplayDevice, dwFlags));
	if (i)
		return FALSE;
	
	strcpy((char *)lpDisplayDevice->DeviceName,"OS/2-PM");
	strcpy((char *)lpDisplayDevice->DeviceString,"OS/2 Presentation Manager Display");
	lpDisplayDevice->StateFlags =
			DISPLAY_DEVICE_ATTACHED_TO_DESKTOP	|
			DISPLAY_DEVICE_PRIMARY_DEVICE		|
			DISPLAY_DEVICE_VGA_COMPATIBLE;
	return TRUE;
}

/***********************************************************************
 *           EnumDisplayDevicesW   (USER32.185)
 */
BOOL WINAPI EnumDisplayDevicesW(
	LPVOID unused,DWORD i,LPDISPLAY_DEVICEW lpDisplayDevice,DWORD dwFlags) 
{
        dprintf(("EnumDisplayDevicesW: %x %d %x %x", unused, i, lpDisplayDevice, dwFlags));
	if (i)
		return FALSE;

	lstrcpyAtoW(lpDisplayDevice->DeviceName,"OS/2-PM");
	lstrcpyAtoW(lpDisplayDevice->DeviceString,"OS/2 Presentation Manager Display");
	lpDisplayDevice->StateFlags =
			DISPLAY_DEVICE_ATTACHED_TO_DESKTOP	|
			DISPLAY_DEVICE_PRIMARY_DEVICE		|
			DISPLAY_DEVICE_VGA_COMPATIBLE;
	return TRUE;
}
//******************************************************************************
//******************************************************************************
