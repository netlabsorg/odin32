/* $Id: oslibwin.cpp,v 1.9 1999-07-17 18:30:51 sandervl Exp $ */
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
#include <oslibwin.h>
#include "oslibstyle.h"
#include "oslibutil.h"
#include "oslibmsg.h"
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
 RECTL rectl;

  dprintf(("WinCreateWindow %x %x %x %s", hwndParent, dwWinStyle, dwFrameStyle, pszName));

  if(pszName && *pszName == 0) {
	pszName = NULL;
  }
  if(hwndParent == 0) {
        hwndParent = HWND_DESKTOP;
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
BOOL OSLibWinConvertStyle(ULONG dwStyle, ULONG *OSWinStyle, ULONG *OSFrameStyle)
{
  *OSWinStyle   = 0;
  *OSFrameStyle = 0;

  /* Window styles */
  if(dwStyle & WINWS_MINIMIZE)
        *OSWinStyle |= WS_MINIMIZED;
//Done explicitely in CreateWindowExA
#if 0
  if(dwStyle & WINWS_VISIBLE)
        *OSWinStyle |= WS_VISIBLE;
#endif
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
LONG OSLibWinQueryWindowTextLength(HWND hwnd)
{
  return WinQueryWindowTextLength(hwnd);
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinQueryWindowText(HWND hwnd,LONG lLength,char* pun)
{
  return WinQueryWindowText(hwnd,lLength,pun);
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
 RECTL rectl;
 HWND  hwndParent;

  if(fl & SWP_MOVE) {
	y = MapOS2ToWin32Y(hwnd, y);
  }
  return WinSetWindowPos(hwnd, hwndInsertBehind, x, y, cx, cy, fl);
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
  rc = WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, fl);
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
  else  return FALSE;
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
void OSLibWinPostQuitMessage(ULONG nExitCode)
{
  WinPostQueueMsg(GetThreadMessageQueue(), WM_QUIT, (MPARAM)nExitCode, 0);
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinDispatchMsg(MSG *msg, BOOL isUnicode)
{
 QMSG qmsg;

  WinToOS2MsgTranslate(msg, &qmsg, isUnicode);
  return (LONG)WinDispatchMsg(GetThreadHAB(), &qmsg);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax, BOOL isUnicode)
{
 QMSG qmsg;
 BOOL rc;

  rc = WinGetMsg(GetThreadHAB(), &qmsg, TranslateWinMsg(uMsgFilterMin), TranslateWinMsg(uMsgFilterMax), 0);
  OS2ToWinMsgTranslate(&qmsg, pMsg, isUnicode);
  return rc;
}
//******************************************************************************
//******************************************************************************
inline ULONG OS2TOWIN32POINT(RECTL *parent, RECTL *child, ULONG y)
{
  return (parent->yTop - parent->yBottom - (child->yTop - child->yBottom) - y - 1);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndChild)
{
 HWND  hwndParent;
 RECTL rectParent = {0}, rectChild = {0};

   WinQueryWindowRect(hwndChild, &rectChild);
   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, &rectParent);
   return OS2TOWIN32POINT(&rectParent, &rectChild, rectChild.yBottom);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndChild, ULONG y)
{
 HWND  hwndParent;
 RECTL rectParent = {0}, rectChild = {0};

   WinQueryWindowRect(hwndChild, &rectChild);
   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, &rectParent);
   return OS2TOWIN32POINT(&rectParent, &rectChild, y);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(PRECTL rectParent, PRECTL rectChild, ULONG y)
{
   return OS2TOWIN32POINT(rectParent, rectChild, y);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(PRECTL rectParent, HWND hwndChild, ULONG y)
{
 RECTL rectChild = {0};

   WinQueryWindowRect(hwndChild, &rectChild);
   return OS2TOWIN32POINT(rectParent, &rectChild, y);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndChild, PRECTL rectChild, ULONG y)
{
 HWND  hwndParent;
 RECTL rectParent = {0};

   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, &rectParent);
   return OS2TOWIN32POINT(&rectParent, rectChild, y);
}
//******************************************************************************
//******************************************************************************
