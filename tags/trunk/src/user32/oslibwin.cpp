/* $Id: oslibwin.cpp,v 1.9 1999-09-26 10:09:59 sandervl Exp $ */
/*
 * Window API wrappers for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
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
#include "win32type.h"
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

        dwFrameStyle |= FCF_TASKLIST | FCF_NOMOVEWITHOWNER | FCF_NOBYTEALIGN;
        dwWinStyle   &= ~WS_CLIPCHILDREN;

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
  *OSWinStyle   = 0;
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

  if (dwStyle & WS_CHILD_W)
  {
    if (dwExStyle & WS_EX_CLIENTEDGE_W ||
        dwExStyle & WS_EX_STATICEDGE_W ||
        dwExStyle & WS_EX_WINDOWEDGE_W)
          *OSFrameStyle |= FCF_DLGBORDER;

    if(dwStyle & WS_VSCROLL_W)
          *OSFrameStyle |= FCF_VERTSCROLL;
    if(dwStyle & WS_HSCROLL_W)
          *OSFrameStyle |= FCF_HORZSCROLL;
  } else
  {
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
          *OSFrameStyle |= FCF_SIZEBORDER;        //??
    if(dwStyle & WS_MINIMIZEBOX_W)
          *OSFrameStyle |= FCF_MINBUTTON;
    if(dwStyle & WS_MAXIMIZEBOX_W)
          *OSFrameStyle |= FCF_MAXBUTTON;

    if(dwExStyle & WS_EX_DLGMODALFRAME_W)
          *OSFrameStyle |= FCF_DLGBORDER;
  }

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
ULONG OSLibSendMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam)
{
  return (ULONG)WinSendMsg(hwnd, msg, (MPARAM)wParam, (MPARAM)lParam);
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
BOOL OSLibWinSetFocus(HWND hwndDeskTop,HWND hwndNewFocus, BOOL activate)
{
  return WinFocusChange (hwndDeskTop, hwndNewFocus, activate ? FC_NOLOSEACTIVE : 0);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsChild (HWND hwnd, HWND hwndOf)
{
  return WinIsChild (hwnd, hwndOf);
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
BOOL OSLibWinSetMultWindowPos(PSWP pswp, ULONG num)
{
    return WinSetMultWindowPos(GetThreadHAB(), pswp, num);
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
 BOOL     rc;
 RECTLOS2 rectl;

  rc = WinQueryWindowRect(hwnd, (PRECTL)&rectl);
  if(rc) {
        if(RelativeTo == RELATIVE_TO_SCREEN) {
                MapOS2ToWin32Rectl(OSLIB_HWND_DESKTOP, hwnd, &rectl, pRect);
        }
        else    MapOS2ToWin32Rectl(&rectl, pRect);
  }
  else  memset(pRect, 0, sizeof(RECT));
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
BOOL OSLibWinQueryWindowPos (HWND hwnd, PSWP pswp)
{
  return WinQueryWindowPos(hwnd, pswp);
}
//******************************************************************************
//******************************************************************************
void OSLibMapSWPtoWINDOWPOS(PSWP pswp, PWINDOWPOS pwpos, PSWP pswpOld, HWND hParent, HWND hFrame)
{
   HWND hWindow            = pswp->hwnd;
   HWND hWndInsertAfter    = pswp->hwndInsertBehind;
   long x                  = pswp->x;
   long y                  = pswp->y;
   long cx                 = pswp->cx;
   long cy                 = pswp->cy;
   UINT fuFlags            = (UINT)pswp->fl;
   ULONG parentHeight;

   HWND  hWinAfter;
   ULONG flags = 0;

   HWND  hWnd = (hWindow == HWND_DESKTOP) ? HWND_DESKTOP_W: hWindow;

   if (hWndInsertAfter == HWND_TOP)
      hWinAfter = HWND_TOP_W;
   else if (hWndInsertAfter == HWND_BOTTOM)
      hWinAfter = HWND_BOTTOM_W;
   else
      hWinAfter = (HWND) hWndInsertAfter;

   //***********************************
   // convert PM flags to Windows flags
   //***********************************
   if (!(fuFlags & SWP_SIZE))       flags |= SWP_NOSIZE_W;
   if (!(fuFlags & SWP_MOVE))       flags |= SWP_NOMOVE_W;
   if (!(fuFlags & SWP_ZORDER))     flags |= SWP_NOZORDER_W;
   if (  fuFlags & SWP_NOREDRAW)    flags |= SWP_NOREDRAW_W;
   if (!(fuFlags & SWP_ACTIVATE))   flags |= SWP_NOACTIVATE_W;
   if (  fuFlags & SWP_SHOW)        flags |= SWP_SHOWWINDOW_W;
   if (  fuFlags & SWP_HIDE)        flags |= SWP_HIDEWINDOW_W;

   if (fuFlags & (SWP_MOVE | SWP_SIZE))
   {
      if (hParent == NULLHANDLE)
      {
        ULONG Offset;
        POINTL pt = {0, 0};

        Offset = OSLibGetWindowHeight(hFrame) - cy;
        parentHeight = ScreenHeight;

        cx += 2 * x;
        cy += Offset;
        WinMapWindowPoints (hFrame, HWND_DESKTOP, &pt, 1);
        x = pt.x;
        y = pt.y;

        pswpOld->cx += 2 * pswpOld->x;
        pswpOld->cy += Offset;
        pswpOld->x   = pt.x;
        pswpOld->y   = pt.y;
      }
      else
      {
        parentHeight = OSLibGetWindowHeight(hParent);
      }

      if (fuFlags & SWP_SIZE)
      {
         if (cy != pswpOld->cy)
         {
            flags &= ~SWP_NOMOVE_W;
         }
      }
      else
      {
         cx = pswpOld->cx;
         cy = pswpOld->cy;
      }

      if ((fuFlags & SWP_MOVE) == 0)
      {
         x = pswpOld->x;
         y = pswpOld->y;
      }

      y         = parentHeight - y - cy;
      LONG oldY = parentHeight - pswpOld->y - pswpOld->cy;

      if ((pswpOld->x == x) && (oldY == y))
         flags |= SWP_NOMOVE_W;

      if ((pswpOld->cx == cx) && (pswpOld->cy == cy))
         flags |= SWP_NOSIZE_W;
   }

   if (hParent == NULLHANDLE)
   {
       pswpOld->x  = x + pswp->x;
       pswpOld->y  = y + cy - pswp->y - pswp->cy;
   }
   else {
       pswpOld->x  = pswp->x;
       pswpOld->y  = parentHeight - pswp->y - cy;
   }
   pswpOld->cx = pswp->cx;
   pswpOld->cy = pswp->cy;

dprintf(("window (%d,%d)(%d,%d)  client (%d,%d)(%d,%d)",
         x,y,cx,cy, pswpOld->x,pswpOld->y,pswpOld->cx,pswpOld->cy));

   pwpos->flags            = (UINT)flags;
   pwpos->cy               = (int)cy;
   pwpos->cx               = (int)cx;
   pwpos->x                = (int)x;
   pwpos->y                = (int)y;
   pwpos->hwndInsertAfter  = hWinAfter;
   pwpos->hwnd             = hWindow;
}
//******************************************************************************
//******************************************************************************
void OSLibMapWINDOWPOStoSWP(PWINDOWPOS pwpos, PSWP pswp, PSWP pswpOld, HWND hParent, HWND hFrame)
{
   HWND hWnd            = pwpos->hwnd;
   HWND hWndInsertAfter = pwpos->hwndInsertAfter;
   long x               = pwpos->x;
   long y               = pwpos->y;
   long cx              = pwpos->cx;
   long cy              = pwpos->cy;
   UINT fuFlags         = pwpos->flags;
   ULONG parentHeight;

   HWND  hWinAfter;
   ULONG flags = 0;
   HWND  hWindow = hWnd ? (HWND)hWnd : HWND_DESKTOP;

   if (hWndInsertAfter == HWND_TOPMOST_W)
//      hWinAfter = HWND_TOPMOST;
      hWinAfter = HWND_TOP;
   else if (hWndInsertAfter == HWND_NOTOPMOST_W)
//      hWinAfter = HWND_NOTOPMOST;
      hWinAfter = HWND_TOP;
   else if (hWndInsertAfter == HWND_TOP_W)
      hWinAfter = HWND_TOP;
   else if (hWndInsertAfter == HWND_BOTTOM_W)
      hWinAfter = HWND_BOTTOM;
   else
      hWinAfter = (HWND) hWndInsertAfter;

   if (!(fuFlags & SWP_NOSIZE_W    )) flags |= SWP_SIZE;
   if (!(fuFlags & SWP_NOMOVE_W    )) flags |= SWP_MOVE;
   if (!(fuFlags & SWP_NOZORDER_W  )) flags |= SWP_ZORDER;
   if (  fuFlags & SWP_NOREDRAW_W  )  flags |= SWP_NOREDRAW;
   if (!(fuFlags & SWP_NOACTIVATE_W)) flags |= SWP_ACTIVATE;
   if (  fuFlags & SWP_SHOWWINDOW_W)  flags |= SWP_SHOW;
   if (  fuFlags & SWP_HIDEWINDOW_W)  flags |= SWP_HIDE;

   if (flags & (SWP_MOVE | SWP_SIZE))
   {
      if (hParent == NULLHANDLE)
        parentHeight = ScreenHeight;
      else
        parentHeight = OSLibGetWindowHeight(hParent);

      if ((flags & SWP_MOVE) == 0)
      {
         x = pswpOld->x;
         y = pswpOld->y;

         if (!((y == 0) && (pswpOld->cy == 0)))
         {
            y = parentHeight - y - pswpOld->cy;
         }
      }

      if (flags & SWP_SIZE)
      {
         if (cy != pswpOld->cy)
            flags |= SWP_MOVE;
      }
      else
      {
         cx = pswpOld->cx;
         cy = pswpOld->cy;
      }

      y = parentHeight - y - cy;

      if ((pswpOld->x == x) && (pswpOld->y == y))
         flags &= ~SWP_MOVE;

      if ((pswpOld->cx == cx) && (pswpOld->cy == cy))
         flags &= ~SWP_SIZE;
   }

   pswp->fl               = flags;
   pswp->cy               = cy;
   pswp->cx               = cx;
   pswp->x                = x;
   pswp->y                = y;
   pswp->hwndInsertBehind = hWinAfter;
   pswp->hwnd             = hWindow;
   pswp->ulReserved1      = 0;
   pswp->ulReserved2      = 0;
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinBeginEnumWindows(HWND hwnd)
{
   if(hwnd == OSLIB_HWND_DESKTOP)       hwnd = HWND_DESKTOP;
   else
   if(hwnd == OSLIB_HWND_OBJECT)        hwnd = HWND_OBJECT;

   return WinBeginEnumWindows(hwnd);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinGetNextWindow(HWND hwndEnum)
{
   return WinGetNextWindow(hwndEnum);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryClientWindow(HWND hwndFrame)
{
 HWND hwndClient = 0;

   if(((ULONG)WinSendMsg(hwndFrame, WM_QUERYFRAMEINFO, NULL, NULL)) & FI_FRAME)
          hwndClient = WinWindowFromID(hwndFrame, FID_CLIENT);

   return hwndClient;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEndEnumWindows(HWND hwndEnum)
{
   return WinEndEnumWindows(hwndEnum);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryWindowProcess(HWND hwnd, ULONG *pid, ULONG *tid)
{
   return WinQueryWindowProcess(hwnd, pid, tid);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinMapWindowPoints (HWND hwndFrom, HWND hwndTo, OSLIBPOINT *pptl, ULONG num)
{
   return WinMapWindowPoints (hwndFrom, hwndTo, (PPOINTL)pptl, num);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEnableScrollBar(HWND hwndParent, int scrollBar, BOOL fEnable)
{
 HWND hwndScroll;

   if(scrollBar == OSLIB_VSCROLL) {
	hwndScroll = WinWindowFromID(hwndParent, FID_VERTSCROLL);
   }
   else hwndScroll = WinWindowFromID(hwndParent, FID_HORZSCROLL);

   if(hwndScroll == NULL)
	return FALSE;

   return WinEnableWindow(hwndScroll, fEnable);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinShowScrollBar(HWND hwndParent, int scrollBar, BOOL fShow)
{
 HWND hwndScroll;

   if(scrollBar == OSLIB_VSCROLL) {
	hwndScroll = WinWindowFromID(hwndParent, FID_VERTSCROLL);
   }
   else hwndScroll = WinWindowFromID(hwndParent, FID_HORZSCROLL);

   if(hwndScroll == NULL)
	return FALSE;

   if(fShow != WinIsWindowVisible(hwndScroll)) {
	WinShowWindow(hwndScroll, fShow);
   }
   return TRUE;   
}
//******************************************************************************
//******************************************************************************
void OSLibTranslateScrollCmdAndMsg(ULONG *msg, ULONG *scrollcmd)
{
    switch(*scrollcmd)
    {
        case SB_LINEUP:
               *scrollcmd = SB_LINEUP_W;
               break;
        case SB_LINEDOWN:
               *scrollcmd = SB_LINEDOWN_W;
               break;
        case SB_PAGEUP:
               *scrollcmd = SB_PAGEUP_W;
               break;
        case SB_PAGEDOWN:
               *scrollcmd = SB_PAGEDOWN_W;
               break;
        case SB_SLIDERTRACK:
               *scrollcmd = SB_THUMBTRACK_W;
               break;
        case SB_SLIDERPOSITION:
               *scrollcmd = SB_THUMBPOSITION_W;
               break;
        case SB_ENDSCROLL:
               *scrollcmd = SB_ENDSCROLL_W;
               break;
    }
    *msg = (*msg == WM_HSCROLL) ? WM_HSCROLL_W : WM_VSCROLL_W;
    return;
}
//******************************************************************************
//******************************************************************************
