/* $Id: pmframe.cpp,v 1.17 1999-11-02 17:07:25 cbratschi Exp $ */
/*
 * Win32 Frame Managment Code for OS/2
 *
 * Copyright 1999 by Christoph Bratschi (cbratschi@datacomm.ch)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_WIN
#define INCL_GPI

#include <os2.h>                        /* PM header file               */
#include <os2wrap.h>
#include <stdlib.h>
#include "win32type.h"
#include <misc.h>
#include <win32wbase.h>
#include "wprocess.h"
#include "pmframe.h"
#include "oslibutil.h"
#include "oslibwin.h"
#include "caret.h"

#define PMFRAMELOG

//******************************************************************************
//******************************************************************************
VOID Draw3DRect(HPS hps,RECTL rect,LONG colorBR,LONG colorTL)
{
  POINTL point;

  GpiSetColor(hps,colorBR);
  point.x = rect.xLeft;
  point.y = rect.yBottom;
  GpiMove(hps,&point);
  point.x = rect.xRight-1;
  GpiLine(hps,&point);
  point.y = rect.yTop-1;
  GpiLine(hps,&point);
  GpiSetColor(hps,colorTL);
  point.x--;
  GpiMove(hps,&point);
  point.x = rect.xLeft;
  GpiLine(hps,&point);
  point.y = rect.yBottom+1;
  GpiLine(hps,&point);
}
//******************************************************************************
//******************************************************************************
VOID DeflateRect(RECTL *rect)
{
  rect->xLeft++;
  rect->xRight--;
  rect->yTop--;
  rect->yBottom++;
}
//******************************************************************************
//******************************************************************************
VOID DrawFrame(HPS hps,RECTL *rect,Win32BaseWindow *win32wnd)
{
  LONG clrWhite,clrBlack,clrDark,clrLight;
  POINTL point;
  DWORD dwExStyle = win32wnd->getExStyle();
  DWORD dwStyle = win32wnd->getStyle();

  //CB: todo: switch to RGB mode and use win colors
  clrWhite = CLR_WHITE;
  clrBlack = CLR_BLACK;
  clrLight = CLR_PALEGRAY;
  clrDark  = CLR_DARKGRAY;

  if (dwExStyle & WS_EX_CLIENTEDGE_W)
  {
    Draw3DRect(hps,*rect,clrWhite,clrDark);
    DeflateRect(rect);
    Draw3DRect(hps,*rect,clrLight,clrBlack);
  }
  else if (dwExStyle & WS_EX_DLGMODALFRAME_W)
  {
    Draw3DRect(hps,*rect,clrBlack,clrLight);
    DeflateRect(rect);
    Draw3DRect(hps,*rect,clrDark,clrWhite);
    DeflateRect(rect);
    Draw3DRect(hps,*rect,clrLight,clrLight);
  }
  else if (dwExStyle & WS_EX_STATICEDGE_W)
  {
    Draw3DRect(hps,*rect,clrWhite,clrDark);
  }
  else if (dwExStyle & WS_EX_WINDOWEDGE_W);
  else if (dwStyle & WS_BORDER_W)
  {
    Draw3DRect(hps,*rect,clrBlack,clrBlack);
  }

  DeflateRect(rect);
}
//******************************************************************************
//******************************************************************************
BOOL CanDrawSizeBox(Win32BaseWindow *win32wnd)
{
  return (win32wnd->getStyle() & WS_SIZEBOX_W && WinQueryWindowULong(win32wnd->getOS2FrameWindowHandle(),QWL_STYLE) & FS_SIZEBORDER
          && win32wnd->getVertScrollHandle() && WinQueryWindow(win32wnd->getVertScrollHandle(),QW_PARENT) == win32wnd->getOS2FrameWindowHandle()
          && win32wnd->getHorzScrollHandle() && WinQueryWindow(win32wnd->getHorzScrollHandle(),QW_PARENT) == win32wnd->getOS2FrameWindowHandle());
}
//******************************************************************************
//******************************************************************************
VOID GetSizeBox(Win32BaseWindow *win32wnd,RECTL *rect)
{
  SWP swpHorz,swpVert;

  WinQueryWindowPos(win32wnd->getVertScrollHandle(),&swpVert);
  WinQueryWindowPos(win32wnd->getHorzScrollHandle(),&swpHorz);
  rect->xLeft = swpVert.x;
  rect->xRight = swpVert.x+swpVert.cx;
  rect->yTop = swpHorz.y+swpHorz.cy;
  rect->yBottom = swpHorz.y;
}
//******************************************************************************
//******************************************************************************
BOOL InSizeBox(Win32BaseWindow *win32wnd,POINTS *points)
{
  if (CanDrawSizeBox(win32wnd))
  {
    RECTL rect;
    POINTL point;

    point.x = points->x;
    point.y = points->y;
    GetSizeBox(win32wnd,&rect);
    return (WinPtInRect(GetThreadHAB(),&rect,&point));
  }

  return FALSE;
}
//******************************************************************************
//******************************************************************************
VOID DrawSizeBox(HPS hps,RECTL rect)
{
  POINTL p1,p2;
  LONG clrDark = CLR_DARKGRAY,clrWhite = CLR_WHITE;
  INT x;

  //CB: todo: switch to RGB mode and use win colors
  WinFillRect(hps,&rect,SYSCLR_DIALOGBACKGROUND);
  p1.x = rect.xRight-2;
  p1.y = rect.yBottom;
  p2.x = rect.xRight-1;
  p2.y = rect.yBottom+1;
  for (x = 0;x < 3;x++)
  {
    GpiSetColor(hps,clrDark);
    GpiMove(hps,&p1);
    GpiLine(hps,&p2);
    p1.x--;
    p2.y++;
    GpiMove(hps,&p1);
    GpiLine(hps,&p2);
    GpiSetColor(hps,clrWhite);
    p1.x--;
    p2.y++;
    GpiMove(hps,&p1);
    GpiLine(hps,&p2);
    p1.x -= 2;
    p2.y += 2;
  }
}
//******************************************************************************
//******************************************************************************
void DrawActivate(Win32BaseWindow *win32wnd, HWND hwnd)
{
    if (!win32wnd->isChild())
    {
        if (CanDrawSizeBox(win32wnd))
        {
          HPS hps;
          RECTL rect;

          GetSizeBox(win32wnd,&rect);
          hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN | PSF_CLIPSIBLINGS);
          DrawSizeBox(hps,rect);
          WinReleasePS(hps);

        }
    }
    else
    {
        HPS hps;
        RECTL rect;

        WinQueryWindowRect(hwnd,&rect);
        rect.xRight = rect.xRight-rect.xLeft;
        rect.yTop = rect.yTop-rect.yBottom;
        rect.xLeft = rect.yBottom = 0;
        hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN | PSF_CLIPSIBLINGS);
        DrawFrame(hps,&rect,win32wnd);
        WinReleasePS(hps);
    }
}
//******************************************************************************
//******************************************************************************
void FrameTrackFrame(Win32BaseWindow *win32wnd,BOOL lefttop)
{
  INT flags = lefttop ? (TF_LEFT | TF_TOP):(TF_RIGHT | TF_BOTTOM);

  WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_TRACKFRAME,(MPARAM)flags,(MPARAM)0);
}
//******************************************************************************
//Win32 frame message handler
//******************************************************************************
MRESULT EXPENTRY Win32FrameProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
 Win32BaseWindow *win32wnd;
 PFNWP            OldFrameProc;
 MRESULT          rc;

  SetWin32TIB();

  win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwnd);

  if(win32wnd == NULL || !win32wnd->getOldFrameProc())
  {
    dprintf(("Invalid win32wnd pointer for frame %x!!", hwnd));
    goto RunDefWndProc;
  }

  OldFrameProc = (PFNWP)win32wnd->getOldFrameProc();

  switch(msg)
  {
    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld;
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE, hwndAfter;

        dprintf(("PMFRAME: WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
            goto RunDefFrameProc;

        if(!win32wnd->CanReceiveSizeMsgs()) {
            goto RunDefFrameProc; //CB: must call def frame proc or frame control activation is broken
            break;
        }

        WinQueryWindowPos(hwnd, &swpOld);

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto RunDefFrameProc;
            }
        }
        hwndAfter = pswp->hwndInsertBehind;
        OSLibMapSWPtoWINDOWPOSFrame(pswp, &wp, &swpOld, hParent, hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           if(wndAfter) wp.hwndInsertAfter = wndAfter->getWindowHandle();
        }

        //CB: problems with profmine titlebar tracking
        if(win32wnd->MsgPosChanging((LPARAM)&wp) == 0)
        {//app or default window handler changed wp
            dprintf(("PMFRAME: WM_ADJUSTWINDOWPOS, app changed windowpos struct"));
            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

            OSLibMapWINDOWPOStoSWPFrame(&wp, pswp, &swpOld, hParent, hwnd);
            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
            pswp->fl |= SWP_NOADJUST;
            pswp->hwndInsertBehind = hwndAfter;
            pswp->hwnd = hwnd;

            goto RunDefFrameProc; //CB: must call def frame proc or frame control activation is broken
            //RestoreOS2TIB();
            //return (MRESULT)0xf;
        }
        goto RunDefFrameProc; //CB: must call def frame proc or frame control activation is broken
    }

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp   = (PSWP)mp1;
      SWP       swpOld = *(pswp + 1);
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE;
      LONG      yDelta = pswp->cy - swpOld.cy;
      LONG      xDelta = pswp->cx - swpOld.cx;

        dprintf(("PMFRAME: WM_WINDOWPOSCHANGED (%x) %x %x (%d,%d) (%d,%d)", mp2, win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        RestoreOS2TIB();
        rc = OldFrameProc(hwnd,msg,mp1,mp2);
        SetWin32TIB();

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
            goto PosChangedEnd;

        if(!win32wnd->CanReceiveSizeMsgs())
            goto PosChangedEnd;

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto PosChangedEnd; //parent has just been destroyed
            }
        }
        OSLibMapSWPtoWINDOWPOSFrame(pswp, &wp, &swpOld, hParent, hwnd);

        //delta is difference between old and new client height
        yDelta = swpOld.cy - win32wnd->getWindowHeight();

        win32wnd->setWindowRect(wp.x, wp.y, wp.x+wp.cx, wp.y+wp.cy);
        win32wnd->setClientRect(swpOld.x, swpOld.y, swpOld.x + swpOld.cx, swpOld.y + swpOld.cy);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           if(wndAfter) wp.hwndInsertAfter = wndAfter->getWindowHandle();
        }

        if (yDelta != 0 || xDelta != 0)
        {
            HENUM henum = WinBeginEnumWindows(WinWindowFromID(pswp->hwnd, FID_CLIENT));
            SWP swp[10];
            int i = 0;
            HWND hwnd;

            while ((hwnd = WinGetNextWindow(henum)) != NULLHANDLE)
            {
#if 0
                if (mdiClient )
                {
                  continue;
                }
#endif
                WinQueryWindowPos(hwnd, &(swp[i]));

#ifdef DEBUG
                Win32BaseWindow *window = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
                dprintf(("ENUMERATE %x delta %d (%d,%d) (%d,%d) %x", (window) ? window->getWindowHandle() : hwnd,
                         yDelta, swp[i].x, swp[i].y, swp[i].cx, swp[i].cy, swp[i].fl));
#endif

                if(swp[i].y != 0) {
                    //child window at offset <> 0 from client area -> offset now changes
                    swp[i].y  += yDelta;
                    swp[i].fl &= ~(SWP_NOREDRAW);
                }
                //else child window with the same start coorindates as the client area
                //The app should resize it.

               if (i == 9)
                {
                    WinSetMultWindowPos(GetThreadHAB(), swp, 10);
                    i = 0;
                }
                else
                {
                    i++;
                }
            }

            WinEndEnumWindows(henum);

            if (i)
               WinSetMultWindowPos(GetThreadHAB(), swp, i);
        }
        if (yDelta != 0)
        {
            POINT pt;
            if(GetCaretPos (&pt) == TRUE)
            {
                pt.y -= yDelta;
                SetCaretPos (pt.x, pt.y);
            }
        }
        win32wnd->MsgPosChanged((LPARAM)&wp);

PosChangedEnd:
        RestoreOS2TIB();
        return rc;
    }
    case WM_ACTIVATE:
    {
        HWND hwndTitle;
        USHORT flags = WinQueryWindowUShort(hwnd,QWS_FLAGS);

        //CB: emulate WM_ACTIVATE -> no flickering
        hwndTitle = WinWindowFromID(hwnd,FID_TITLEBAR);
        if (hwndTitle) WinSendMsg(hwndTitle,TBM_SETHILITE,mp1,MPVOID);

        WinSendMsg(WinWindowFromID(hwnd,FID_CLIENT),WM_ACTIVATE,mp1,mp2);
        WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));


        RestoreOS2TIB();
        return 0;
    }

    case WM_DESTROY:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_DESTROY"));
      #endif
      WinSubclassWindow(hwnd,OldFrameProc);
      win32wnd->setOldFrameProc(NULL);
      goto RunDefFrameProc;

    case WM_MOUSEMOVE:
      if (InSizeBox(win32wnd,(POINTS*)&mp1))
      {
        WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_SIZENWSE,FALSE));
        RestoreOS2TIB();
        return (MRESULT)TRUE;
      }
      else if (win32wnd->isChild()) goto RunDefWndProc;
      else goto RunDefFrameProc;

    case WM_BUTTON1DOWN:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_BUTTON1DOWN"));
      #endif
      if (InSizeBox(win32wnd,(POINTS*)&mp1))
      {
        WinSetActiveWindow(HWND_DESKTOP,hwnd);
        WinSendMsg(hwnd,WM_TRACKFRAME,(MPARAM)(TF_RIGHT | TF_BOTTOM),(MPARAM)0);
        RestoreOS2TIB();
        return (MRESULT)TRUE;
      }
      else if (win32wnd->isChild()) goto RunDefWndProc;
      else goto RunDefFrameProc;

    case WM_BUTTON2DOWN:
    case WM_BUTTON3DOWN:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_BUTTON2/3DOWN"));
      #endif
      if (win32wnd->isChild()) goto RunDefWndProc;
      else goto RunDefFrameProc;

    case WM_PAINT:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_PAINT"));
      #endif
      if (!win32wnd->isChild())
      {
        if (CanDrawSizeBox(win32wnd))
        {
          MRESULT res;
          HPS hps;
          RECTL rect;

          RestoreOS2TIB();
          res = OldFrameProc(hwnd,msg,mp1,mp2);
          SetWin32TIB();

          GetSizeBox(win32wnd,&rect);
          hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN | PSF_CLIPSIBLINGS);
          DrawSizeBox(hps,rect);
          WinReleasePS(hps);

          RestoreOS2TIB();
          return res;
        }
        else goto RunDefFrameProc;
      }
      else
      {
        RECTL rect;
        HPS hps;

        RestoreOS2TIB();
        OldFrameProc(hwnd,msg,mp1,mp2);
        SetWin32TIB();

        WinQueryWindowRect(hwnd,&rect);
        rect.xRight = rect.xRight-rect.xLeft;
        rect.yTop = rect.yTop-rect.yBottom;
        rect.xLeft = rect.yBottom = 0;
        hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN | PSF_CLIPSIBLINGS);
        DrawFrame(hps,&rect,win32wnd);
        WinReleasePS(hps);

        RestoreOS2TIB();
        return (MRESULT)0;
      }

    default:
      RestoreOS2TIB();
      return OldFrameProc(hwnd,msg,mp1,mp2);
  }

  RestoreOS2TIB();
  return (MRESULT)FALSE;

RunDefFrameProc:
  RestoreOS2TIB();
  return OldFrameProc(hwnd,msg,mp1,mp2);

RunDefWndProc:
  RestoreOS2TIB();
  return WinDefWindowProc(hwnd,msg,mp1,mp2);
}

PVOID FrameSubclassFrameWindow(Win32BaseWindow *win32wnd)
{
  return WinSubclassWindow(win32wnd->getOS2FrameWindowHandle(),PFNWP(Win32FrameProc));
}

VOID FrameGetBorderSize(Win32BaseWindow *win32wnd,PWPOINT pSize)
{
  WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_QUERYBORDERSIZE,(MPARAM)pSize,(MPARAM)0);
}

VOID FrameSetBorderSize(Win32BaseWindow *win32wnd,BOOL resize)
{
  POINTL point;

  if (!resize)
  {
    WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_SETBORDERSIZE,(MPARAM)win32wnd->getBorderWidth(),(MPARAM)win32wnd->getBorderHeight());

    return;
  }

  FrameGetBorderSize(win32wnd,&point);
  WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_SETBORDERSIZE,(MPARAM)win32wnd->getBorderWidth(),(MPARAM)win32wnd->getBorderHeight());
  if (point.x != win32wnd->getBorderWidth() || point.y != win32wnd->getBorderHeight())
  {
    INT xDiff = win32wnd->getBorderWidth()-point.x;
    INT yDiff = win32wnd->getBorderHeight()-point.y;
    SWP swp;

    WinQueryWindowPos(win32wnd->getOS2FrameWindowHandle(),&swp);
    swp.x  += xDiff;
    swp.y  += yDiff;
    swp.cx -= 2*xDiff;
    swp.cy -= 2*yDiff;
    WinSetWindowPos(win32wnd->getOS2FrameWindowHandle(),0,swp.x,swp.y,swp.cx,swp.cy,SWP_MOVE | SWP_SIZE);
  }
}

UINT FrameGetDefSizeBorderSize(VOID)
{
  return WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER);
}
