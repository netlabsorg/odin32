/* $Id: oslibwin.cpp,v 1.76 2000-05-10 13:14:42 sandervl Exp $ */
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

#define DBG_LOCALLOG    DBG_oslibwin
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetParent(HWND hwnd, HWND hwndParent, ULONG fRedraw)
{
  if(hwndParent == OSLIB_HWND_DESKTOP)
  {
        hwndParent = HWND_DESKTOP;
  }
  else
  if(hwndParent == OSLIB_HWND_OBJECT) {
        hwndParent = HWND_OBJECT;
  }
  return (WinSetParent(hwnd, hwndParent, fRedraw) == 0);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetOwner(HWND hwnd, HWND hwndOwner)
{
  return WinSetOwner(hwnd, hwndOwner);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinCreateWindow(HWND hwndParent,ULONG dwWinStyle,
                          char *pszName, HWND Owner, ULONG fHWND_BOTTOM, HWND *hwndFrame,
                          ULONG id, BOOL fTaskList,BOOL fShellPosition,
                          int classStyle)
{
 HWND  hwndClient;

  dprintf(("WinCreateWindow %x %s %x task %d shell %d classstyle %x", hwndParent, pszName, id, fTaskList, fShellPosition, classStyle));

  if(pszName && *pszName == 0) {
        pszName = NULL;
  }
  if(hwndParent == OSLIB_HWND_DESKTOP) {
        hwndParent = HWND_DESKTOP;
  }
  if(Owner == OSLIB_HWND_DESKTOP) {
        Owner = HWND_DESKTOP;
  }
  ULONG dwClientStyle = 0;
  ULONG dwFrameStyle = 0;

  BOOL TopLevel = hwndParent == HWND_DESKTOP;

  FRAMECDATA FCData = {sizeof (FRAMECDATA), 0, 0, 0};

  if(classStyle & CS_SAVEBITS_W) dwWinStyle |= WS_SAVEBITS;
  if(classStyle & CS_PARENTDC_W) dwWinStyle |= WS_PARENTCLIP;

  dwClientStyle = dwWinStyle & ~(WS_TABSTOP | WS_GROUP | WS_CLIPSIBLINGS);

  dwFrameStyle |= FCF_NOBYTEALIGN;
  if(fTaskList)
  {
    dwFrameStyle |= FCF_TASKLIST | FCF_NOMOVEWITHOWNER;
  }
  if (fShellPosition) dwFrameStyle |= FCF_SHELLPOSITION;

  dwWinStyle &= ~WS_CLIPCHILDREN;
  FCData.flCreateFlags = dwFrameStyle;

  *hwndFrame = WinCreateWindow (hwndParent,
                                TopLevel ? WC_FRAME : WIN32_INNERFRAME,
                                pszName, dwWinStyle, 0, 0, 0, 0,
                                Owner, HWND_TOP,
                                id, &FCData, NULL);

  if (*hwndFrame) {
    hwndClient = WinCreateWindow (*hwndFrame, (classStyle & CS_SAVEBITS_W) ? WIN32_STDCLASS2:WIN32_STDCLASS,
                                  NULL, dwClientStyle, 0, 0, 0, 0,
                                  *hwndFrame, HWND_TOP, FID_CLIENT, NULL, NULL);
    return hwndClient;
  }
  dprintf(("OSLibWinCreateWindow: (FRAME) WinCreateStdWindow failed (%x)", WinGetLastError(GetThreadHAB())));
  return 0;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinConvertStyle(ULONG dwStyle, ULONG dwExStyle, ULONG *OSWinStyle)
{
  *OSWinStyle   = 0;

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

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetWindowULong(HWND hwnd, ULONG offset, ULONG value)
{
  if(offset == OSLIB_QWL_USER)
    offset = QWL_USER;

  return WinSetWindowULong(hwnd, offset, value);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinGetWindowULong(HWND hwnd, ULONG offset)
{
  if(offset == OSLIB_QWL_USER)
    offset = QWL_USER;

  return WinQueryWindowULong(hwnd, offset);
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
LONG OSLibWinQuerySysValue(LONG iSysValue)
{
  return WinQuerySysValue(HWND_DESKTOP,iSysValue);
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
BOOL OSLibWinQueryPointerPos(PPOINT pptlPoint)
{
  return WinQueryPointerPos(HWND_DESKTOP,(PPOINTL)pptlPoint);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryWindow(HWND hwnd, ULONG lCode)
{
  return WinQueryWindow(hwnd, lCode);
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
 BOOL rc = 1;

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
BOOL OSLibWinQueryWindowRect(HWND hwnd, PRECT pRect, int RelativeTo)
{
 BOOL     rc;
 RECTLOS2 rectl;

  rc = WinQueryWindowRect(hwnd, (PRECTL)&rectl);
  if(rc) {
        if(RelativeTo == RELATIVE_TO_SCREEN) {
                mapOS2ToWin32Rect(hwnd,OSLIB_HWND_DESKTOP,&rectl,pRect);
        }
        else    mapOS2ToWin32Rect(hwnd,&rectl,pRect);
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
 BOOL rc;
 HWND hwndClient;

  rc = WinEnableWindow(hwnd, fEnable);
  hwndClient = WinWindowFromID(hwnd, FID_CLIENT);
  if(hwndClient) {
        WinEnableWindow(hwndClient, fEnable);
  }
  return rc;
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
HWND OSLibWinQueryActiveWindow()
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

   HWND   hWinAfter;
   ULONG  flags = 0;
   SWP    swpFrame, swpClient;
   POINTL point;

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
    if (  fuFlags & SWP_NOADJUST)    flags |= SWP_NOSENDCHANGING_W;

    WinQueryWindowPos(hFrame, &swpFrame);

    if(fuFlags & (SWP_MOVE | SWP_SIZE))
    {
        point.x = swpFrame.x;
        point.y = swpFrame.y;

        if (hParent)
        {
          RECTL parentRect;

          WinQueryWindowRect(hParent,&parentRect);
          parentHeight = parentRect.yTop;
        } else
        {
          parentHeight = ScreenHeight;
        }

        point.y = parentHeight-point.y-swpFrame.cy;

        cy = swpFrame.cy;
        cx = swpFrame.cx;
        x  = point.x;
        y  = point.y;

        if ((pswp->x == pswpOld->x) && (pswp->y == pswpOld->y))
            flags |= SWP_NOMOVE_W;

        if ((pswp->cx == pswpOld->cx) && (pswp->cy == pswpOld->cy))
            flags |= SWP_NOSIZE_W;

        if (fuFlags & SWP_SIZE)
        {
            if (pswp->cy != pswpOld->cy)
            {
                flags &= ~SWP_NOMOVE_W;
            }
        }
    }

    pswpOld->x  = pswp->x;
    pswpOld->y  = swpFrame.cy-pswp->y-pswp->cy;
    pswpOld->cx = pswp->cx;
    pswpOld->cy = pswp->cy;

    dprintf(("window (%d,%d)(%d,%d)  client (%d,%d)(%d,%d)",
             x,y,cx,cy, pswpOld->x,pswpOld->y,pswpOld->cx,pswpOld->cy));

    pwpos->flags            = (UINT)flags;
    pwpos->cy               = cy;
    pwpos->cx               = cx;
    pwpos->x                = x;
    pwpos->y                = y;
    pwpos->hwndInsertAfter  = hWinAfter;
    pwpos->hwnd             = hWindow;
}
//******************************************************************************
//******************************************************************************
void OSLibMapSWPtoWINDOWPOSFrame(PSWP pswp, struct tagWINDOWPOS *pwpos, PSWP pswpOld, HWND hParent, HWND hFrame)
{
   HWND hWindow            = pswp->hwnd;
   HWND hWndInsertAfter    = pswp->hwndInsertBehind;
   long x                  = pswp->x;
   long y                  = pswp->y;
   long cx                 = pswp->cx;
   long cy                 = pswp->cy;
   UINT fuFlags            = (UINT)pswp->fl;
   ULONG parentHeight;

   HWND   hWinAfter;
   ULONG  flags = 0;
   SWP    swpClient;
   POINTL point;

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
    if (  fuFlags & SWP_NOADJUST)    flags |= SWP_NOSENDCHANGING_W;

    WinQueryWindowPos(WinWindowFromID(hFrame, FID_CLIENT), &swpClient);

    if(fuFlags & (SWP_MOVE | SWP_SIZE))
    {
        if (hParent)
        {
          RECTL parentRect;

          WinQueryWindowRect(hParent,&parentRect);
          parentHeight = parentRect.yTop;
        } else
        {
          parentHeight = ScreenHeight;
        }

        point.x = x;
        point.y = parentHeight-y-cy;

        x  = point.x;
        y  = point.y;

        if ((pswp->x == pswpOld->x) && (pswp->y == pswpOld->y))
            flags |= SWP_NOMOVE_W;

        if ((pswp->cx == pswpOld->cx) && (pswp->cy == pswpOld->cy))
            flags |= SWP_NOSIZE_W;

        if (fuFlags & SWP_SIZE)
        {
            if (pswp->cy != pswpOld->cy)
            {
                flags &= ~SWP_NOMOVE_W;
            }
        }
    }

    pswpOld->x  = swpClient.x;
    pswpOld->y  = pswp->cy-swpClient.y-swpClient.cy;
    pswpOld->cx = swpClient.cx;
    pswpOld->cy = swpClient.cy;

    dprintf(("window (%d,%d)(%d,%d)  client (%d,%d)(%d,%d)",
             x,y,cx,cy, pswpOld->x,pswpOld->y,pswpOld->cx,pswpOld->cy));

    pwpos->flags            = (UINT)flags;
    pwpos->cy               = cy;
    pwpos->cx               = cx;
    pwpos->x                = x;
    pwpos->y                = y;
    pwpos->hwndInsertAfter  = hWinAfter;
    pwpos->hwnd             = hWindow;
}
//******************************************************************************
//******************************************************************************
void OSLibMapWINDOWPOStoSWP(PWINDOWPOS pwpos, PSWP pswp, PSWP pswpOld, HWND hParent, HWND hFrame)
{
 BOOL fCvt = FALSE;

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
   if (  fuFlags & SWP_NOSENDCHANGING_W) flags |= SWP_NOADJUST;

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

         y = parentHeight-y-pswpOld->cy;
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
      y  = parentHeight-y-cy;

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
//Position in screen coordinates
//******************************************************************************
void OSLibMapWINDOWPOStoSWPFrame(PWINDOWPOS pwpos, PSWP pswp, PSWP pswpOld, HWND hParent, HWND hFrame)
{
 BOOL fCvt = FALSE;

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
   if (  fuFlags & SWP_NOSENDCHANGING_W) flags |= SWP_NOADJUST;

   if (flags & (SWP_MOVE | SWP_SIZE))
   {
      if (hParent)
      {
        RECTL parentRect;

        WinQueryWindowRect(hParent,&parentRect);
        parentHeight = parentRect.yTop;
      } else
      {
        parentHeight = ScreenHeight;
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
      y  = parentHeight-y-cy;

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
BOOL  OSLibWinCalcFrameRect(HWND hwndFrame, RECT *pRect, BOOL fClient)
{
 BOOL rc;

   WinMapWindowPoints(hwndFrame, HWND_DESKTOP, (PPOINTL)pRect, 2);

   rc = WinCalcFrameRect(hwndFrame, (PRECTL)pRect, fClient);
   WinMapWindowPoints(HWND_DESKTOP, hwndFrame, (PPOINTL)pRect, 2);

   return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibGetMinMaxInfo(HWND hwndFrame, MINMAXINFO *pMinMax)
{
 TRACKINFO tinfo;

   memset(&tinfo, 0, sizeof(TRACKINFO));
   WinSendMsg(hwndFrame, WM_QUERYTRACKINFO, (MPARAM)0,(MPARAM)&tinfo);

   pMinMax->ptMinTrackSize.x = tinfo.ptlMinTrackSize.x;
   pMinMax->ptMinTrackSize.y = tinfo.ptlMinTrackSize.y;
   pMinMax->ptMaxTrackSize.x = tinfo.ptlMaxTrackSize.x;
   pMinMax->ptMaxTrackSize.y = tinfo.ptlMaxTrackSize.y;
   return TRUE;
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
HWND OSLibWinQueryObjectWindow(VOID)
{
  return WinQueryObjectWindow(HWND_DESKTOP);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinObjectWindowFromID(HWND hwndOwner, ULONG ID)
{
  HWND   hwndNext, hwndFound=0;
  HENUM  henum;

  henum = WinBeginEnumWindows(HWND_OBJECT);
  while ((hwndNext = WinGetNextWindow(henum)) != 0)
  {
    if(WinQueryWindow(hwndNext, QW_OWNER) == hwndOwner &&
       WinQueryWindowUShort(hwndNext, QWS_ID) == ID)
    {
       hwndFound = hwndNext;
       break;
    }
  }
  WinEndEnumWindows(henum);
  return hwndFound;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSetWindowID(HWND hwnd, ULONG value)
{
  dprintf(("OSLibSetWindowID hwnd:%x ID:%x", hwnd, value));
  return WinSetWindowULong(hwnd, QWS_ID, value);
}
//******************************************************************************
//******************************************************************************
PVOID OSLibWinSubclassWindow(HWND hwnd,PVOID newWndProc)
{
  return WinSubclassWindow(hwnd,(PFNWP)newWndProc);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSetWindowRestoreRect(HWND hwnd, PRECT pRect)
{
 ULONG yHeight = OSLibGetWindowHeight(WinQueryWindow(hwnd, QW_PARENT));

  WinSetWindowUShort(hwnd, QWS_XRESTORE,  (USHORT)pRect->left );
  WinSetWindowUShort(hwnd, QWS_YRESTORE,  (USHORT)(yHeight - pRect->top -
                                                   (pRect->bottom - pRect->top)));
  WinSetWindowUShort(hwnd, QWS_CXRESTORE, (USHORT)(pRect->right - pRect->left));
  WinSetWindowUShort(hwnd, QWS_CYRESTORE, (USHORT)(pRect->bottom - pRect->top));
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSetWindowMinPos(HWND hwnd, ULONG x, ULONG y)
{
 ULONG yHeight = OSLibGetWindowHeight(WinQueryWindow(hwnd, QW_PARENT));

  WinSetWindowUShort(hwnd, QWS_XMINIMIZE, (USHORT)x );
  WinSetWindowUShort(hwnd, QWS_YMINIMIZE, (USHORT)(yHeight - y -
                    ( 2 * WinQuerySysValue( HWND_DESKTOP, SV_CYSIZEBORDER)) -
                      WinQuerySysValue( HWND_DESKTOP, SV_CYICON)));
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetKeyboardStateTable(unsigned char *PMKeyState)
{
  return WinSetKeyboardStateTable(HWND_DESKTOP, (PBYTE)&PMKeyState, FALSE );
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetKeyboardStateTable(unsigned char *PMKeyState)
{
  return WinSetKeyboardStateTable(HWND_DESKTOP, (PBYTE)&PMKeyState, TRUE );
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEnableWindowUpdate(HWND hwnd,BOOL fEnable)
{
  return WinEnableWindowUpdate(hwnd,fEnable);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinGetLastError()
{
  return WinGetLastError(GetThreadHAB()) & 0xFFFF;
}
//******************************************************************************
//******************************************************************************
void OSLibWinShowTaskList(HWND hwndFrame)
{
  //CB: don't know if this works on all machines
  WinSetActiveWindow(HWND_DESKTOP,0x8000000E);
}
//******************************************************************************
//******************************************************************************
void OSLibSetWindowStyle(HWND hwnd, ULONG dwStyle, ULONG dwExStyle, BOOL saveBits)
{
  ULONG dwWinStyle;

  OSLibWinConvertStyle(dwStyle, dwExStyle, &dwWinStyle);

  dwWinStyle = dwWinStyle & ~(WS_TABSTOP | WS_GROUP | WS_CLIPCHILDREN);
  if(saveBits) dwWinStyle |= WS_SAVEBITS;
  if(dwStyle & WS_VISIBLE_W)
        dwWinStyle |= WS_VISIBLE;

  WinSetWindowULong(hwnd, QWL_STYLE,
                    (WinQueryWindowULong(hwnd, QWL_STYLE) & ~0xffff0000) |
                     dwWinStyle);
}
//******************************************************************************
//******************************************************************************
void OSLibWinSetVisibleRegionNotify(HWND hwnd, BOOL fNotify)
{
  WinSetVisibleRegionNotify(hwnd, fNotify);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryCapture()
{
  return WinQueryCapture(HWND_DESKTOP);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetCapture(HWND hwnd)
{
  return WinSetCapture(HWND_DESKTOP, hwnd);
}
//******************************************************************************
//******************************************************************************
