/* $Id: oslibwin.cpp,v 1.5 1999-07-17 09:17:58 sandervl Exp $ */
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
#include <misc.h>
#include <oslibwin.h>
#include "oslibstyle.h"
#include "oslibutil.h"
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
                          char *pszName, ULONG x, ULONG y, ULONG cx, ULONG cy,
                          HWND Owner, ULONG fHWND_BOTTOM, HWND *hwndFrame)
{
 HWND  hwndClient;
 RECTL rectl;

  if(hwndParent == 0) {
        hwndParent = HWND_DESKTOP;
  }
  if(WinQueryWindowRect(hwndParent, &rectl) == TRUE) {
        y = OS2TOWIN32POINT(rectl.yTop - rectl.yBottom, y);
  }
  if(dwFrameStyle) {
        dwWinStyle &= ~WS_CLIPCHILDREN; //invalid style according to docs
        *hwndFrame = WinCreateStdWindow(hwndParent, dwWinStyle,
                                       &dwFrameStyle, WIN32_STDCLASS,
                                       "", 0, 0, 0, &hwndClient) != 0;
        if(*hwndFrame) {
                if(pszName) {
                        WinSetWindowText(*hwndFrame, pszName);
                }
                WinSetWindowPos(*hwndFrame, (fHWND_BOTTOM) ? HWND_BOTTOM :HWND_TOP,
                                x, y, cx, cy, SWP_SIZE | SWP_MOVE);

                return hwndClient;
        }
	dprintf(("OSLibWinCreateWindow: WinCreateStdWindow failed (%x)", WinGetLastError(GetThreadHAB())));
        return 0;
  }
  hwndClient = WinCreateWindow(hwndParent, WIN32_STDCLASS, pszName, dwWinStyle, x, y, cx, cy,
                               Owner, (fHWND_BOTTOM) ? HWND_BOTTOM :HWND_TOP, 0, NULL,
                               NULL);
  *hwndFrame = hwndClient;
  return hwndClient;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinConvertStyle(ULONG dwStyle, ULONG *OSWinStyle, ULONG *OSFrameStyle)
{
  *OSWinStyle   = 0;
  *OSFrameStyle = 0;

  /* Window styles */
  if(dwStyle & WINWS_MINIMIZE)
        *OSWinStyle |= WS_MINIMIZED;
  if(dwStyle & WINWS_VISIBLE)
        *OSWinStyle |= WS_VISIBLE;
  if(dwStyle & WINWS_DISABLED)
        *OSWinStyle |= WS_DISABLED;
  if(dwStyle & WINWS_CLIPSIBLINGS)
        *OSWinStyle |= WS_CLIPSIBLINGS;
  if(dwStyle & WINWS_CLIPCHILDREN)
        *OSWinStyle |= WS_CLIPCHILDREN;
  if(dwStyle & WINWS_MAXIMIZE)
        *OSWinStyle |= WS_MAXIMIZED;
  if(dwStyle & WINWS_GROUP)
        *OSWinStyle |= WS_GROUP;
  if(dwStyle & WINWS_TABSTOP)
        *OSWinStyle |= WS_TABSTOP;

  if(dwStyle & WINWS_CAPTION)
        *OSFrameStyle |= FCF_TITLEBAR;
  if(dwStyle & WINWS_BORDER)
        *OSFrameStyle |= FCF_BORDER;
  if(dwStyle & WINWS_DLGFRAME)
        *OSFrameStyle |= FCF_DLGBORDER;
  if(dwStyle & WINWS_VSCROLL)
        *OSFrameStyle |= FCF_VERTSCROLL;
  if(dwStyle & WINWS_HSCROLL)
        *OSFrameStyle |= FCF_HORZSCROLL;
  if(dwStyle & WINWS_SYSMENU)
        *OSFrameStyle |= FCF_SYSMENU;
  if(dwStyle & WINWS_THICKFRAME)
        *OSFrameStyle |= FCF_SIZEBORDER;        //??
  if(dwStyle & WINWS_MINIMIZEBOX)
        *OSFrameStyle |= FCF_MINBUTTON;
  if(dwStyle & WINWS_MAXIMIZEBOX)
        *OSFrameStyle |= FCF_MAXBUTTON;

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
HWND OSLibWinCreateMenu(HWND hwndParent, PVOID menutemplate)
{
  return WinCreateMenu(hwndParent, menutemplate);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryTopMostChildWindow(HWND hwndParent)
{
  return WinQueryWindow(hwndParent, QW_TOP);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetWindowPos(HWND hwnd, HWND hwndInsertBehind, LONG x, LONG y, LONG cx, 
                          LONG cy, ULONG fl)
{
  return WinSetWindowPos(hwnd, hwndInsertBehind, x, y, cx, cy, fl);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinShowWindow(HWND hwnd, ULONG fl)
{
  return WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, fl);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinDestroyWindow(HWND hwnd)
{
  return WinDestroyWindow(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryUpdateRect(HWND hwnd, PVOID pRect)
{
  return WinQueryUpdateRect(hwnd, (RECTL *)pRect);
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
