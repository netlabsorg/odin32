/* $Id: oslibwin.cpp,v 1.2 1999-07-14 21:05:58 cbratschi Exp $ */
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
#include <oslibwin.h>
#include "oslibstyle.h"
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
                          HWND Owner, ULONG fHWND_BOTTOM)
{
 HWND  hwndClient;
 HWND  hwndFrame;
 RECTL rectl;

  if(hwndParent == 0) {
        hwndParent = HWND_DESKTOP;
  }
  if(WinQueryWindowRect(hwndParent, &rectl) == 0) {
        y = OS2TOWIN32POINT(rectl.yTop - rectl.yBottom, y);
  }
  if(dwFrameStyle) {
        dwWinStyle &= ~WS_CLIPCHILDREN; //invalid style according to docs
        hwndFrame = WinCreateStdWindow(hwndParent, dwWinStyle,
                                       &dwFrameStyle, WIN32_STDCLASS,
                                       "", 0, 0, 0, &hwndClient) != 0;
        if(hwndFrame) {
                if(pszName) {
                        WinSetWindowText(hwndFrame, pszName);
                }
                WinSetWindowPos(hwndFrame, (fHWND_BOTTOM) ? HWND_BOTTOM :HWND_TOP,
                                x, y, cx, cy, SWP_SIZE | SWP_MOVE);

                return hwndFrame;
        }
        return 0;
  }
  else  return WinCreateWindow(hwndParent, WIN32_STDCLASS, pszName, dwWinStyle, x, y, cx, cy,
                               Owner, (fHWND_BOTTOM) ? HWND_BOTTOM :HWND_TOP, 0, NULL,
                               NULL);
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
