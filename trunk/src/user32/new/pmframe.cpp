/* $Id: pmframe.cpp,v 1.5 2000-01-05 21:25:05 cbratschi Exp $ */
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
#include <string.h>
#include "win32type.h"
#include <misc.h>
#include <win32wbase.h>
#include "wprocess.h"
#include "pmframe.h"
#include "oslibutil.h"
#include "oslibwin.h"
#include "caret.h"
#include "oslibmsg.h"

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
inline VOID DeflateRect(RECTL *rect)
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
  return ((win32wnd->getStyle() & WS_SIZEBOX_W) && (WinQueryWindowULong(win32wnd->getOS2FrameWindowHandle(),QWL_STYLE) & FS_SIZEBORDER)
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
VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags)
{
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
 THDB            *thdb;
 MSG             *pWinMsg,winMsg;

  SetWin32TIB();

  thdb = GetThreadTHDB();
  win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwnd);

  if (!thdb || (win32wnd == NULL) || !win32wnd->getOldFrameProc())
  {
    dprintf(("Invalid win32wnd pointer for frame %x!!", hwnd));
    goto RunDefWndProc;
  }

  if((thdb->msgstate & 1) == 0)
  {//message that was sent directly to our window proc handler; translate it here
        QMSG qmsg;

        qmsg.msg  = msg;
        qmsg.hwnd = hwnd;
        qmsg.mp1  = mp1;
        qmsg.mp2  = mp2;
        qmsg.time = WinQueryMsgTime(thdb->hab);
        WinQueryMsgPos(thdb->hab, &qmsg.ptl);
        qmsg.reserved = 0;

        if(OS2ToWinMsgTranslate((PVOID)thdb, &qmsg, &winMsg, FALSE, MSG_REMOVE) == FALSE)
        {//message was not translated
            memset(&winMsg, 0, sizeof(MSG));
        }
        pWinMsg = &winMsg;
  }
  else {
        pWinMsg = &thdb->msg;
        thdb->msgstate++;
  }

  OldFrameProc = (PFNWP)win32wnd->getOldFrameProc();

  switch(msg)
  {
    case WM_FORMATFRAME:
    {
      PSWP pswp = (PSWP)mp1,swpClient;
      RECTL *client = (PRECTL)mp2,rect;
      RECT winRect;
      INT ccount;

      if (!win32wnd->IsWindowCreated()) goto RunDefFrameProc;
      dprintf(("PMFRAME: WM_FORMATFRAME %x",hwnd));
      RestoreOS2TIB();
      ccount = (INT)OldFrameProc(hwnd,msg,mp1,mp2);
      SetWin32TIB();
      dprintf(("Frame size: %d %d",win32wnd->getWindowWidth(),win32wnd->getWindowHeight()));
      win32wnd->MsgFormatFrame();
      //CB: todo: use result for WM_CALCVALIDRECTS
      mapWin32ToOS2Rect(WinQueryWindow(hwnd,QW_PARENT),win32wnd->getClientRectPtr(),(PRECTLOS2)&rect);
      WinMapWindowPoints(WinQueryWindow(hwnd,QW_PARENT),hwnd,(PPOINTL)&rect,2);
      dprintf(("New client position: %d %d %d %d",rect.xLeft,rect.yBottom,rect.xRight,rect.yTop));
      swpClient = &pswp[ccount-1];
      swpClient->x = rect.xLeft;
      swpClient->y = rect.yBottom;
      swpClient->cx = rect.xRight-rect.xLeft;
      swpClient->cy = rect.yTop-rect.yBottom;
      RestoreOS2TIB();
      return (MRESULT)ccount;
    }

    case WM_MINMAXFRAME:
      dprintf(("PMFRAME: WM_MINMAXFRAME %x",hwnd));
      //CB: todo
      goto RunDefFrameProc;

    case WM_QUERYBORDERSIZE:
      goto RunDefFrameProc;

    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON2DOWN:
    case WM_BUTTON2UP:
    case WM_BUTTON2DBLCLK:
    case WM_BUTTON3DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DBLCLK:
        if (win32wnd->IsWindowCreated())
        {
          win32wnd->MsgButton(pWinMsg);
          RestoreOS2TIB();
        }
        return (MRESULT)TRUE;

    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:
    case WM_BUTTON2CLICK:
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:
    case WM_BUTTON1CLICK:
    case WM_BUTTON3MOTIONSTART:
    case WM_BUTTON3MOTIONEND:
    case WM_BUTTON3CLICK:
        RestoreOS2TIB();
        return (MRESULT)TRUE;

    case WM_MOUSEMOVE:
    {
        //OS/2 Window coordinates -> Win32 Window coordinates
        if (win32wnd->IsWindowCreated())
          win32wnd->MsgMouseMove(pWinMsg);
        RestoreOS2TIB();
        return (MRESULT)TRUE;
    }

    case WM_HITTEST:
    {
      DWORD res;

      // Only send this message if the window is enabled
      if (!win32wnd->IsWindowCreated())
        res = HT_NORMAL;
      else if (!WinIsWindowEnabled(hwnd))
        res = HT_ERROR;
      else if (win32wnd->getIgnoreHitTest())
        res = HT_NORMAL;
      else
      {
        dprintf(("PMFRAME: WM_HITTEST %x (%d,%d)",hwnd,(*(POINTS *)&mp1).x,(*(POINTS *)&mp1).y));

        //CB: WinWindowFromPoint: PM sends WM_HITTEST -> loop -> stack overflow
        win32wnd->setIgnoreHitTest(TRUE);
        res = win32wnd->MsgHitTest(pWinMsg);
        win32wnd->setIgnoreHitTest(FALSE);
      }
      RestoreOS2TIB();
      return (MRESULT)res;
    }

    case WM_PAINT:
        //CB: todo: call defframe if minimized
        dprintf(("PMFRAME: WM_PAINT"));
        if (win32wnd->IsWindowCreated())
          win32wnd->MsgNCPaint();
        goto RunDefWndProc;

    case WM_SIZE:
        dprintf(("PMFRAME: WM_SIZE"));
        goto RunDefFrameProc;

    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld;
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE, hwndAfter;

        dprintf(("PMFRAME: WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        //CB: show dialog in front of owner
        if (win32wnd->IsModalDialogOwner())
        {
          pswp->fl |= SWP_ZORDER;
          pswp->hwndInsertBehind = win32wnd->getOS2HwndModalDialog();
          if (pswp->fl & SWP_ACTIVATE)
          {
            pswp->fl &= ~SWP_ACTIVATE;
            WinSetWindowPos(win32wnd->getOS2HwndModalDialog(),0,0,0,0,0,SWP_ACTIVATE);
          }
        }

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
            goto RunDefFrameProc;

        if(!win32wnd->CanReceiveSizeMsgs())
           break;

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

            RestoreOS2TIB();
            return (MRESULT)0xf;
        }
        goto RunDefFrameProc;
    }

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp   = (PSWP)mp1;
      SWP       swpOld = *(pswp + 1);
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE;

        dprintf(("PMFRAME: WM_WINDOWPOSCHANGED (%x) %x %x (%d,%d) (%d,%d)", mp2, win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
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

        win32wnd->setWindowRect(wp.x, wp.y, wp.x+wp.cx, wp.y+wp.cy);
dprintf(("CB: %d %d %d %d",wp.x,wp.y,wp.x+wp.cx,wp.y+wp.cy));
        if(win32wnd->CanReceiveSizeMsgs())
          win32wnd->MsgPosChanged((LPARAM)&wp);

PosChangedEnd:
        //calls WM_FORMATFRAME if SWP_SIZE is set
        RestoreOS2TIB();
        rc = OldFrameProc(hwnd,msg,mp1,mp2);
        SetWin32TIB();

        RestoreOS2TIB();
        return rc;
    }

    case WM_CALCVALIDRECTS:
    {
      PRECTL oldRect = (PRECTL)mp1,newRect = oldRect+1;
      UINT res = CVR_ALIGNLEFT | CVR_ALIGNTOP;

//CB: todo: use WM_NCCALCSIZE
      if (win32wnd->getWindowClass())
      {
        DWORD dwStyle = win32wnd->getWindowClass()->getClassLongA(GCL_STYLE_W);

        if ((dwStyle & CS_HREDRAW_W) && (newRect->xRight-newRect->xLeft != oldRect->xRight-oldRect->xLeft))
          res |= CVR_REDRAW;
        else if ((dwStyle & CS_VREDRAW_W) && (newRect->yTop-newRect->yBottom != oldRect->yTop-oldRect->yBottom))
          res |= CVR_REDRAW;
      } else res |= CVR_REDRAW;

      //CB: PM updates window frame (and unfortunately all other frame controls)
      RestoreOS2TIB();
      OldFrameProc(hwnd,msg,mp1,mp2);
      SetWin32TIB();

      RestoreOS2TIB();
      return (MRESULT)res;
    }

    case WM_ACTIVATE:
    {
        HWND hwndTitle;
        USHORT flags = WinQueryWindowUShort(hwnd,QWS_FLAGS);

        if (win32wnd->IsWindowCreated())
        {
          WinSendMsg(WinWindowFromID(hwnd,FID_CLIENT),WM_ACTIVATE,mp1,mp2);
          WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));

          //CB: show owner behind the dialog
          if (win32wnd->IsModalDialog())
          {
            Win32BaseWindow *topOwner = win32wnd->getOwner()->GetTopParent();

            if (topOwner) WinSetWindowPos(topOwner->getOS2FrameWindowHandle(),hwnd,0,0,0,0,SWP_ZORDER);
          }
        } else
        {
          WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
        }

        RestoreOS2TIB();
        return 0;
    }

    case WM_DESTROY:
      dprintf(("PMFRAME: WM_DESTROY %x",hwnd));
      WinSubclassWindow(hwnd,OldFrameProc);
      win32wnd->setOldFrameProc(NULL);
      goto RunDefFrameProc;

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
  if ((point.x != win32wnd->getBorderWidth()) || (point.y != win32wnd->getBorderHeight()))
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

BOOL FrameCreateScrollBars(Win32BaseWindow *win32wnd,BOOL createHorz,BOOL createVert,BOOL updateFrame,DWORD *flags)
{
  HWND hwndHScroll = 0,hwndVScroll = 0;
  ULONG updateFlags = 0;

  if (createHorz)
  {
    hwndHScroll = WinCreateWindow(win32wnd->getOS2FrameWindowHandle(),WC_SCROLLBAR,"",WS_VISIBLE | WS_PARENTCLIP | WS_SYNCPAINT | SBS_HORZ,0,0,0,0,win32wnd->getOS2FrameWindowHandle(),HWND_TOP,FID_HORZSCROLL,NULL,NULL);
    if (hwndHScroll) win32wnd->setHorzScrollHandle(hwndHScroll);
    else return FALSE;
    updateFlags = FCF_HORZSCROLL;
  }

  if (createVert)
  {
    hwndVScroll = WinCreateWindow(win32wnd->getOS2FrameWindowHandle(),WC_SCROLLBAR,"",WS_VISIBLE | WS_PARENTCLIP | WS_SYNCPAINT | SBS_VERT,0,0,0,0,win32wnd->getOS2FrameWindowHandle(),HWND_TOP,FID_VERTSCROLL,NULL,NULL);
    if (hwndVScroll) win32wnd->setVertScrollHandle(hwndVScroll); else
    {
      if (hwndHScroll) WinDestroyWindow(hwndHScroll);

      return FALSE;
    }
    updateFlags |= FCF_VERTSCROLL;
  }

  win32wnd->subclassScrollBars(hwndHScroll,hwndVScroll);

  if (updateFrame && updateFlags) WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_UPDATEFRAME,(MPARAM)0,(MPARAM)0);
  if (flags) *flags = updateFlags;

  return TRUE;
}

VOID FrameGetScrollBarHandles(Win32BaseWindow *win32wnd,BOOL getHorz,BOOL getVert)
{
  if (getHorz) win32wnd->setHorzScrollHandle(WinWindowFromID(win32wnd->getOS2FrameWindowHandle(),FID_HORZSCROLL));
  if (getVert) win32wnd->setVertScrollHandle(WinWindowFromID(win32wnd->getOS2FrameWindowHandle(),FID_VERTSCROLL));
}

BOOL FrameShowScrollBars(Win32BaseWindow *win32wnd,BOOL changeHorz,BOOL changeVert,BOOL fShow,BOOL updateFrame,DWORD *flags)
{
  HWND hwndObj = WinQueryObjectWindow(HWND_DESKTOP);
  ULONG updateFlags = 0;

  if (changeHorz)
  {
    HWND hwndCurPar = WinQueryWindow(win32wnd->getHorzScrollHandle(),QW_PARENT);

    if ((fShow && (hwndCurPar == hwndObj)) || (!fShow && (hwndCurPar != hwndObj)))
    {
      WinSetParent(win32wnd->getHorzScrollHandle(),fShow ? win32wnd->getOS2FrameWindowHandle():HWND_OBJECT,FALSE);
      updateFlags |= FCF_HORZSCROLL;
    }
  }

  if (changeVert)
  {
    HWND hwndCurPar = WinQueryWindow(win32wnd->getVertScrollHandle(),QW_PARENT);

    if ((fShow && (hwndCurPar == hwndObj)) || (!fShow && (hwndCurPar != hwndObj)))
    {
      WinSetParent(win32wnd->getVertScrollHandle(),fShow ? win32wnd->getOS2FrameWindowHandle():HWND_OBJECT,FALSE);
      updateFlags |= FCF_VERTSCROLL;
    }
  }

  if (updateFrame && updateFlags) WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_UPDATEFRAME,(MPARAM)updateFlags,(MPARAM)0);
  if (flags) *flags = updateFlags;

  return TRUE;
}

VOID FrameUpdateFrame(Win32BaseWindow *win32wnd,DWORD flags)
{
  WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_UPDATEFRAME,(MPARAM)flags,(MPARAM)0);
}

DWORD FrameHitTest(Win32BaseWindow *win32wnd,INT x,INT y)
{
  POINTL point;
  HWND hwnd = win32wnd->getOS2FrameWindowHandle(),child;

  if (hwnd == win32wnd->getOS2WindowHandle()) return HTCLIENT_W;
  if (win32wnd->getOS2WindowHandle() == WinQueryCapture(HWND_DESKTOP)) return HTCLIENT_W;
  point.x = x;
  point.y = mapScreenY(y);
  WinMapWindowPoints(HWND_DESKTOP,hwnd,&point,1);
  child = WinWindowFromPoint(hwnd,&point,FALSE);

  if (child == 0) return HTERROR_W;
  if (child == win32wnd->getOS2FrameWindowHandle())
  {
    RECTL client,frame;

    if (CanDrawSizeBox(win32wnd))
    {
      RECTL rect;

      GetSizeBox(win32wnd,&rect);
      if (WinPtInRect(GetThreadHAB(),&rect,&point)) return HTGROWBOX_W;
    }
    //somewhere in the border
    INT w = WinQuerySysValue(HWND_DESKTOP,SV_CXMINMAXBUTTON);
    INT h = WinQuerySysValue(HWND_DESKTOP,SV_CXMINMAXBUTTON);
    WinQueryWindowRect(hwnd,&frame);

    if (point.y < h)
    {
      if (point.x < w) return HTBOTTOMLEFT_W;
      if (point.x > frame.xRight-1-w) return HTBOTTOMRIGHT_W;
      return HTBOTTOM_W;
    }
    if (point.y > frame.yTop-1-h)
    {
      if (point.x < w) return HTTOPLEFT_W;
      if (point.x > frame.xRight-1-w) return HTTOPRIGHT_W;
      return HTTOP_W;
    }
    return HTBORDER_W;
  } else
  {
    if (child == WinWindowFromID(hwnd,FID_CLIENT)) return HTCLIENT_W;
    if (child == WinWindowFromID(hwnd,FID_VERTSCROLL)) return HTVSCROLL_W;
    if (child == WinWindowFromID(hwnd,FID_HORZSCROLL)) return HTHSCROLL_W;
    if (child == WinWindowFromID(hwnd,FID_SYSMENU)) return HTSYSMENU_W;
    if (child == WinWindowFromID(hwnd,FID_TITLEBAR)) return HTCAPTION_W;
    if (child == WinWindowFromID(hwnd,FID_MENU)) return HTMENU_W;
    if (child == WinWindowFromID(hwnd,FID_MINMAX))
    {
      //CB: close, reduce or zoom
      return HTZOOM_W;
    }

    return HTERROR_W;
  }
}
