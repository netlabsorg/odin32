/* $Id: display.cpp,v 1.1 1999-11-23 19:34:19 sandervl Exp $ */
/*
 * Display/Monitor Win32 apis
 *
 * Based on Wine code (991031)
 *
 * Copyright 1993 Robert J. Amstadt
 *	     1996 Alex Korobka 
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <heapstring.h>
#include "pmwindow.h"

#define NRMODES 5
#define NRDEPTHS 4
struct {
	int w,h;
} modes[NRMODES]={{512,384},{640,400},{640,480},{800,600},{1024,768}};
int depths[4] = {8,16,24,32};

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
 * Remark    :
 * Status    : 
 *
 * Author    : Wine Port (991031)
 *****************************************************************************/
BOOL WIN32API EnumDisplaySettingsA(
	LPCSTR name,		/* [in] huh? */
	DWORD n,		/* [in] nth entry in display settings list*/
	LPDEVMODEA devmode	/* [out] devmode for that setting */
) 
{
    	dprintf(("USER32:  EnumDisplaySettingsA %d", n));
	if (n==0) {
		devmode->dmBitsPerPel = ScreenBitsPerPel;
		devmode->dmPelsHeight = ScreenHeight;
		devmode->dmPelsWidth = ScreenWidth;
		return TRUE;
	}
	if ((n-1)<NRMODES*NRDEPTHS) {
		devmode->dmBitsPerPel	= depths[(n-1)/NRMODES];
		devmode->dmPelsHeight	= modes[(n-1)%NRMODES].h;
		devmode->dmPelsWidth	= modes[(n-1)%NRMODES].w;
		return TRUE;
	}
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
 * Remark    :
 * Status    : 
 *
 * Author    : Wine Port (991031)
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
		devmode->dmBitsPerPel	= devmodeA.dmBitsPerPel;
		devmode->dmPelsHeight	= devmodeA.dmPelsHeight;
		devmode->dmPelsWidth	= devmodeA.dmPelsWidth;
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
#ifdef DEBUG
    if(lpDevMode) {
        WriteLog("USER32:  ChangeDisplaySettingsA FAKED %X\n", dwFlags);
        WriteLog("USER32:  ChangeDisplaySettingsA lpDevMode->dmBitsPerPel %d\n", lpDevMode->dmBitsPerPel);
        WriteLog("USER32:  ChangeDisplaySettingsA lpDevMode->dmPelsWidth  %d\n", lpDevMode->dmPelsWidth);
        WriteLog("USER32:  ChangeDisplaySettingsA lpDevMode->dmPelsHeight %d\n", lpDevMode->dmPelsHeight);
    }
#endif
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
#ifdef DEBUG
    if(lpDevMode) {
        WriteLog("USER32:  ChangeDisplaySettingsExA FAKED %X\n", dwFlags);
        WriteLog("USER32:  ChangeDisplaySettingsExA lpDevMode->dmBitsPerPel %d\n", lpDevMode->dmBitsPerPel);
        WriteLog("USER32:  ChangeDisplaySettingsExA lpDevMode->dmPelsWidth  %d\n", lpDevMode->dmPelsWidth);
        WriteLog("USER32:  ChangeDisplaySettingsExA lpDevMode->dmPelsHeight %d\n", lpDevMode->dmPelsHeight);
    }
#endif
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
BOOL WIN32API GetMonitorInfoA(HMONITOR,LPMONITORINFO)
{
  dprintf(("USER32:  GetMonitorInfoA not supported!!\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMonitorInfoW(HMONITOR,LPMONITORINFO)
{
  dprintf(("USER32:  GetMonitorInfoW not supported!!\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromWindow(HWND hwnd, DWORD dwFlags)
{
  dprintf(("USER32:  MonitorFromWindow not correctly supported??\n"));
  //Attention: Win32 hwnd!

  return(0);
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromRect(LPRECT rect, DWORD dwFlags)
{
  dprintf(("USER32:  MonitorFromRect not correctly supported??\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromPoint(POINT point, DWORD dwflags)
{
  dprintf(("USER32:  MonitorFromPoint not correctly supported??\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumDisplayMonitors(HDC,LPRECT,MONITORENUMPROC,LPARAM)
{
  dprintf(("USER32:  EnumDisplayMonitors not supported??\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
