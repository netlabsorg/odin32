/* $Id: oslibwin.cpp,v 1.23 1999-08-22 18:29:37 dengert Exp $ */
/*
 * Window API wrappers for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#include <os2.h>
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winconst.h>
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibmsg.h"
#include "oslibgdi.h"
#include "pmwindow.h"

//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetParent(HWND hwnd, HWND hwndParent, ULONG fRedraw)
{
  if(hwndParent == OSLIB_HWND_DESKTOP)
  {
	hwndParent = HWND_DESKTOP;
  }

  return (WinSetParent(hwnd, hwndParent, fRedraw) == 0);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinCreateWindow(HWND hwndParent, ULONG dwWinStyle, ULONG dwFrameStyle,
			  char *pszName, HWND Owner, ULONG fHWND_BOTTOM, HWND *hwndFrame)
{
 HWND  hwndClient;

  dprintf(("WinCreateWindow %x %x %x %s", hwndParent, dwWinStyle, dwFrameStyle, pszName));

  if(pszName && *pszName == 0) {
	pszName = NULL;
  }
  if(hwndParent == OSLIB_HWND_DESKTOP) {
	hwndParent = HWND_DESKTOP;
  }
  if(Owner == OSLIB_HWND_DESKTOP) {
	Owner = HWND_DESKTOP;
  }

  if(dwFrameStyle) {
	ULONG dwClientStyle;

	dwClientStyle = dwWinStyle & ~(WS_TABSTOP | WS_GROUP);
	if(pszName)
		dwFrameStyle |= FCF_TITLEBAR;

	dwFrameStyle |= FCF_TASKLIST;
	*hwndFrame = WinCreateStdWindow(hwndParent, dwWinStyle,
				       &dwFrameStyle, WIN32_STDCLASS,
				       "", dwClientStyle, 0, 0, &hwndClient);
	if(*hwndFrame) {
		if(pszName) {
			WinSetWindowText(*hwndFrame, pszName);
		}
		return hwndClient;
	}
	dprintf(("OSLibWinCreateWindow: WinCreateStdWindow failed (%x)", WinGetLastError(GetThreadHAB())));
	return 0;
  }
  hwndClient = WinCreateWindow(hwndParent, WIN32_STDCLASS, pszName, dwWinStyle, 0, 0, 0, 0,
			       Owner, (fHWND_BOTTOM) ? HWND_BOTTOM :HWND_TOP, 0, NULL,
			       NULL);
  *hwndFrame = hwndClient;
  return hwndClient;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinConvertStyle(ULONG dwStyle, ULONG dwExStyle, ULONG *OSWinStyle, ULONG *OSFrameStyle)
{
  *OSWinStyle	= 0;
  *OSFrameStyle = 0;

  /* Window styles */
  if(dwStyle & WS_MINIMIZE_W)
	*OSWinStyle |= WS_MINIMIZED;
//Done explicitely in CreateWindowExA
#if 0
  if(dwStyle & WS_VISIBLE_W)
	*OSWinStyle |= WS_VISIBLE;
#endif
  if(dwStyle & WS_DISABLED_W)
	*OSWinStyle |= WS_DISABLED;
  if(dwStyle & WS_CLIPSIBLINGS_W)
	*OSWinStyle |= WS_CLIPSIBLINGS;
  if(dwStyle & WS_CLIPCHILDREN_W)
	*OSWinStyle |= WS_CLIPCHILDREN;
  if(dwStyle & WS_MAXIMIZE_W)
	*OSWinStyle |= WS_MAXIMIZED;
  if(dwStyle & WS_GROUP_W)
	*OSWinStyle |= WS_GROUP;
  if(dwStyle & WS_TABSTOP_W)
	*OSWinStyle |= WS_TABSTOP;

  if(dwStyle & WS_CAPTION_W)
	*OSFrameStyle |= FCF_TITLEBAR;
  if(dwStyle & WS_DLGFRAME_W)
	*OSFrameStyle |= FCF_DLGBORDER;
  else
  if(dwStyle & WS_BORDER_W)
	*OSFrameStyle |= FCF_BORDER;

  if(dwStyle & WS_VSCROLL_W)
	*OSFrameStyle |= FCF_VERTSCROLL;
  if(dwStyle & WS_HSCROLL_W)
	*OSFrameStyle |= FCF_HORZSCROLL;
  if(dwStyle & WS_SYSMENU_W)
	*OSFrameStyle |= FCF_SYSMENU;
  if(dwStyle & WS_THICKFRAME_W)
	*OSFrameStyle |= FCF_SIZEBORDER;	//??
  if(dwStyle & WS_MINIMIZEBOX_W)
	*OSFrameStyle |= FCF_MINBUTTON;
  if(dwStyle & WS_MAXIMIZEBOX_W)
	*OSFrameStyle |= FCF_MAXBUTTON;

  if(dwExStyle & WS_EX_DLGMODALFRAME_W)
	*OSFrameStyle |= FCF_DLGBORDER;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetWindowULong(HWND hwnd, ULONG offset, ULONG value)
{
  return WinSetWindowULong(hwnd, offset, value);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinGetWindowULong(HWND hwnd, ULONG offset)
{
  return WinQueryWindowULong(hwnd, offset);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibPostMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam)
{
  return WinPostMsg(hwnd, msg, (MPARAM)wParam, (MPARAM)lParam);
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
BOOL OSLibWinAlarm(HWND hwndDeskTop,ULONG flStyle)
{
  return WinAlarm(hwndDeskTop,flStyle);
}
//******************************************************************************
//******************************************************************************
APIRET OSLibDosBeep(ULONG freg,ULONG dur)
{
  return DosBeep(freg,dur);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryFocus(HWND hwndDeskTop)
{
  return WinQueryFocus(hwndDeskTop);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinWindowFromID(HWND hwndParent,ULONG id)
{
  return WinWindowFromID(hwndParent,id);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetFocus(HWND hwndDeskTop,HWND hwndNewFocus)
{
  return WinSetFocus(hwndDeskTop,hwndNewFocus);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibGetWindowHeight(HWND hwnd)
{
  RECTL rect;

  return (WinQueryWindowRect(hwnd,&rect)) ? rect.yTop-rect.yBottom:0;
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinQuerySysValue(HWND hwndDeskTop,LONG iSysValue)
{
  return WinQuerySysValue(hwndDeskTop,iSysValue);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinQueryDlgItemText(HWND hwndDlg,ULONG idItem,LONG cchBufferMax,char* pchBuffer)
{
  return WinQueryDlgItemText(hwndDlg,idItem,cchBufferMax,pchBuffer);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetDlgItemText(HWND hwndDlg,ULONG idItem,char* pszText)
{
  return WinSetDlgItemText(hwndDlg,idItem,pszText);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryPointerPos(HWND hwndDeskTop,PPOINT pptlPoint)
{
  return WinQueryPointerPos(hwndDeskTop,(PPOINTL)pptlPoint);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryWindow(HWND hwnd, ULONG lCode)
{
  return WinQueryWindow(hwnd, lCode);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetWindowPos(HWND hwnd, HWND hwndInsertBehind, LONG x, LONG y, LONG cx,
			  LONG cy, ULONG fl)
{
 HWND hwndParent = hwndInsertBehind;
 BOOL rc;

    if(fl & SWP_MOVE) {
	switch(hwndParent)
	{
	    case HWNDOS_TOP:
	    case HWNDOS_BOTTOM:
		hwndParent = HWND_DESKTOP;
		break;
	}
	y = MapOS2ToWin32Y(hwndParent, cy, y);
    }
    rc = WinSetWindowPos(hwnd, hwndInsertBehind, x, y, cx, cy, fl);
    dprintf(("WinSetWindowPos %x %x %d %d %d %d %x returned %d (%x)", hwnd, hwndInsertBehind, x, y, cx, cy, fl, rc, WinGetLastError(GetThreadHAB())));
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinShowWindow(HWND hwnd, ULONG fl)
{
 BOOL rc;

  if(fl & SWP_SHOW) {
	 rc = WinShowWindow(hwnd, TRUE);
  }
  if(rc == 0)
	dprintf(("WinShowWindow %x failed %x", hwnd, WinGetLastError(GetThreadHAB())));
  rc = WinSetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, fl);
  if(rc == 0)
	dprintf(("WinShowWindow %x failed %x", hwnd, WinGetLastError(GetThreadHAB())));
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinDestroyWindow(HWND hwnd)
{
  return WinDestroyWindow(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryWindowRect(HWND hwnd, PRECT pRect, int RelativeTo)
{
 BOOL	  rc;
 RECTLOS2 rectl;

  rc = WinQueryWindowRect(hwnd, (PRECTL)&rectl);
  if(rc) {
	if(RelativeTo == RELATIVE_TO_SCREEN) {
		MapOS2ToWin32Rectl(OSLIB_HWND_DESKTOP, hwnd, &rectl, pRect);
	}
	else	MapOS2ToWin32Rectl(&rectl, pRect);
  }
  else	memset(pRect, 0, sizeof(RECT));
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsIconic(HWND hwnd)
{
 SWP  swp;
 BOOL rc;

  rc = WinQueryWindowPos(hwnd, &swp);
  if(rc == FALSE) {
	dprintf(("OSLibWinIsIconic: WinQueryWindowPos %x failed", hwnd));
	return FALSE;
  }

  if(swp.fl & SWP_MINIMIZE)
	return TRUE;
  else	return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetActiveWindow(HWND hwnd)
{
  return WinSetActiveWindow(HWND_DESKTOP, hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetFocus(HWND hwnd)
{
  return WinSetFocus(HWND_DESKTOP, hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEnableWindow(HWND hwnd, BOOL fEnable)
{
  return WinEnableWindow(hwnd, fEnable);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsWindowEnabled(HWND hwnd)
{
  return WinIsWindowEnabled(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsWindowVisible(HWND hwnd)
{
  return WinIsWindowVisible(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryActiveWindow()
{
  return WinQueryActiveWindow(HWND_DESKTOP);
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinQueryWindowTextLength(HWND hwnd)
{
  return WinQueryWindowTextLength(hwnd);
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinQueryWindowText(HWND hwnd, LONG length, LPSTR lpsz)
{
  return WinQueryWindowText(hwnd, length, lpsz);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetWindowText(HWND hwnd, LPSTR lpsz)
{
  return WinSetWindowText(hwnd, lpsz);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinFlashWindow(HWND hwnd, BOOL fFlash)
{
  return WinFlashWindow(hwnd, fFlash);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinWindowFromPoint(HWND hwnd, PVOID ppoint)
{
  return WinWindowFromPoint((hwnd == OSLIB_HWND_DESKTOP) ? HWND_DESKTOP : hwnd, (PPOINTL)ppoint, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinMinimizeWindow(HWND hwnd)
{
  return WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_MINIMIZE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetBorderSize(HWND hwnd, OSLIBPOINT *pointl)
{
  pointl->x = 0;
  pointl->y = 0;
  return (BOOL) WinSendMsg(hwnd, WM_QUERYBORDERSIZE, MPFROMP( &pointl), 0);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetIcon(HWND hwnd, HANDLE hIcon)
{
    return (BOOL) WinSendMsg(hwnd, WM_SETICON, (MPARAM)hIcon, 0);
}
//******************************************************************************
//******************************************************************************

