/* $Id: pmframe.cpp,v 1.7 1999-10-17 12:17:44 cbratschi Exp $ */
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

#define PMFRAMELOG

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

VOID DeflateRect(RECTL *rect)
{
  rect->xLeft++;
  rect->xRight--;
  rect->yTop--;
  rect->yBottom++;
}

VOID DrawFrame(HPS hps,RECTL *rect,Win32BaseWindow *win32wnd)
{
  LONG clrWhite,clrBlack,clrDark,clrLight;
  POINTL point;
  DWORD dwExStyle = win32wnd->getExStyle();
  DWORD dwStyle = win32wnd->getStyle();

  clrWhite = CLR_WHITE;
  clrBlack = CLR_BLACK;
  clrLight = CLR_PALEGRAY;
  clrDark  = CLR_DARKGRAY;

  if (dwExStyle & WS_EX_CLIENTEDGE_W)
  {
    Draw3DRect(hps,*rect,clrWhite,clrDark);
    DeflateRect(rect);
    Draw3DRect(hps,*rect,clrLight,clrBlack);
  } else if (dwExStyle & WS_EX_DLGMODALFRAME_W)
  {
    Draw3DRect(hps,*rect,clrBlack,clrLight);
    DeflateRect(rect);
    Draw3DRect(hps,*rect,clrDark,clrWhite);
    DeflateRect(rect);
    Draw3DRect(hps,*rect,clrLight,clrLight);
  } else if (dwExStyle & WS_EX_STATICEDGE_W)
  {
    Draw3DRect(hps,*rect,clrWhite,clrDark);
  } else if (dwExStyle & WS_EX_WINDOWEDGE_W);
  else if (dwStyle & WS_BORDER_W)
  {
    Draw3DRect(hps,*rect,clrBlack,clrBlack);
  }

  DeflateRect(rect);
}

BOOL CanDrawSizeBox(Win32BaseWindow *win32wnd)
{
  return (!win32wnd->isChild() && WinQueryWindowULong(win32wnd->getOS2FrameWindowHandle(),QWL_STYLE) & FS_SIZEBORDER
          && win32wnd->getVertScrollHandle() && WinQueryWindow(win32wnd->getVertScrollHandle(),QW_PARENT) == win32wnd->getOS2FrameWindowHandle()
          && win32wnd->getHorzScrollHandle() && WinQueryWindow(win32wnd->getHorzScrollHandle(),QW_PARENT) == win32wnd->getOS2FrameWindowHandle());
}

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

VOID DrawSizeBox(HPS hps,RECTL rect)
{
  POINTL p1,p2;
  LONG clrDark = CLR_DARKGRAY,clrWhite = CLR_WHITE;
  INT x;

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
//Win32 frame message handler
//******************************************************************************
MRESULT EXPENTRY Win32FrameProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
  Win32BaseWindow *win32wnd;
  PFNWP OldFrameProc;

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
      Win32BaseWindow *wndchild;

      wndchild = Win32BaseWindow::GetWindowFromOS2FrameHandle(pswp->hwnd);
      if(wndchild && wndchild->isChild())
      {
#if 0
       SWP swp = *pswp;

        MRESULT rc = OldFrameProc(hwnd, msg, mp1, mp2);
        pswp->x = swp.x;
        pswp->y = swp.y;
        pswp->fl = swp.fl;
#endif
        dprintf(("PMFRAME: WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", hwnd, pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        RestoreOS2TIB();
        return (MRESULT)0;
      }
      goto RunDefFrameProc;
    }
#if 0
    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp  = (PSWP)mp1;
      dprintf(("PMFRAME: WM_WINDOWPOSCHANGED %x %x %x (%d,%d) (%d,%d)", hwnd, pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
      goto RunDefFrameProc;
    }

    case WM_FORMATFRAME:
    {
      PSWP      pswp  = (PSWP)mp1;
      dprintf(("PMFRAME: WM_FORMATFRAME %x %x %x (%d,%d) (%d,%d)", hwnd, pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
      goto RunDefFrameProc;
    }
#endif

    case WM_DESTROY:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_DESTROY"));
      #endif
      WinSubclassWindow(hwnd,OldFrameProc);
      win32wnd->setOldFrameProc(NULL);
      goto RunDefFrameProc;

    case WM_MOUSEMOVE:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_MOUSEMOVE"));
      #endif
      if (InSizeBox(win32wnd,(POINTS*)&mp1))
      {
        WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_SIZENWSE,FALSE));
        RestoreOS2TIB();
        return (MRESULT)TRUE;
      } else if (win32wnd->isChild()) goto RunDefWndProc;
      else goto RunDefFrameProc;

    case WM_BUTTON1DOWN:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_BUTTON1DOWN"));
      #endif
      if (InSizeBox(win32wnd,(POINTS*)&mp1))
      {
        WinSendMsg(hwnd,WM_TRACKFRAME,(MPARAM)(TF_RIGHT | TF_BOTTOM),(MPARAM)0);
        RestoreOS2TIB();
        return (MRESULT)TRUE;
      } else if (win32wnd->isChild()) goto RunDefWndProc;
      else goto RunDefFrameProc;

    case WM_BUTTON2DOWN:
    case WM_BUTTON3DOWN:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_BUTTON2/3DOWN"));
      #endif
      if (win32wnd->isChild()) goto RunDefWndProc;
      else goto RunDefFrameProc;

    case WM_ACTIVATE:
      #ifdef PMFRAMELOG
       dprintf(("PMFRAME: WM_ACTIVATE"));
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
          hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN);
          DrawSizeBox(hps,rect);
          WinReleasePS(hps);

          RestoreOS2TIB();
          return res;
        } else goto RunDefFrameProc;
      } else
      {
        MRESULT res;
        HPS hps;
        RECTL rect;

        RestoreOS2TIB();
        res = OldFrameProc(hwnd,msg,mp1,mp2);
        SetWin32TIB();

        WinQueryWindowRect(hwnd,&rect);
        rect.xRight = rect.xRight-rect.xLeft;
        rect.yTop = rect.yTop-rect.yBottom;
        rect.xLeft = rect.yBottom = 0;
        hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN);
        DrawFrame(hps,&rect,win32wnd);
        WinReleasePS(hps);

        RestoreOS2TIB();
        return res;
      }

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
          hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN);
          DrawSizeBox(hps,rect);
          WinReleasePS(hps);

          RestoreOS2TIB();
          return res;
        } else goto RunDefFrameProc;
      } else
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
        hps = WinGetClipPS(hwnd,0,PSF_CLIPCHILDREN);
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
