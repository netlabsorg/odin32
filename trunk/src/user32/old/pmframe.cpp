/* $Id: pmframe.cpp,v 1.1 2001-05-11 08:38:38 sandervl Exp $ */
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

#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <winconst.h>
#include <misc.h>
#include <win32wbase.h>
#include "wprocess.h"
#include "pmframe.h"
#include "oslibutil.h"
#include "oslibwin.h"
#include "caret.h"
#include "oslibmsg.h"

#define DBG_LOCALLOG    DBG_pmframe
#include "dbglocal.h"

#define PMFRAMELOG

//******************************************************************************
//******************************************************************************
VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags)
{
  WinSendMsg(win32wnd->getOS2FrameWindowHandle(),WM_TRACKFRAME,(MPARAM)flags,(MPARAM)0);
}
//******************************************************************************
//******************************************************************************
VOID FrameUpdateChildPositions(HWND hwnd)
{
  HENUM henum;
  HWND hchild;
  RECTL rectl;

  henum = WinBeginEnumWindows(hwnd);
  while ((hchild = WinGetNextWindow(henum)) != NULLHANDLE)
  {
    Win32BaseWindow *child = Win32BaseWindow::GetWindowFromOS2FrameHandle(hchild);

    if (child)
    {
      WinQueryWindowRect(child->getOS2FrameWindowHandle(),&rectl);
      mapOS2ToWin32Rect(child->getOS2FrameWindowHandle(),OSLIB_HWND_DESKTOP,(PRECTLOS2)&rectl,child->getWindowRect());
      FrameUpdateChildPositions(child->getOS2WindowHandle());
    }
  }
  WinEndEnumWindows(henum);
}
//******************************************************************************
//Win32 frame message handler
//******************************************************************************
MRESULT EXPENTRY Win32FrameProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
 Win32BaseWindow *win32wnd;
 PFNWP            OldFrameProc;
 MRESULT          rc;
 TEB             *teb;
 MSG             *pWinMsg,winMsg;

  SetWin32TIB();

  teb = GetThreadTEB();
  win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwnd);

  if (!teb || (win32wnd == NULL) || !win32wnd->getOldFrameProc())
  {
    dprintf(("Invalid win32wnd pointer for frame %x!!", hwnd));
    goto RunDefWndProc;
  }

  if((teb->o.odin.msgstate & 1) == 0)
  {//message that was sent directly to our window proc handler; translate it here
        QMSG qmsg;

        qmsg.msg  = msg;
        qmsg.hwnd = hwnd;
        qmsg.mp1  = mp1;
        qmsg.mp2  = mp2;
        qmsg.time = WinQueryMsgTime(teb->o.odin.hab);
        WinQueryMsgPos(teb->o.odin.hab, &qmsg.ptl);
        qmsg.reserved = 0;

        if(OS2ToWinMsgTranslate((PVOID)teb, &qmsg, &winMsg, FALSE, MSG_REMOVE) == FALSE)
        {//message was not translated
            memset(&winMsg, 0, sizeof(MSG));
        }
        pWinMsg = &winMsg;
  }
  else {
        pWinMsg = &teb->o.odin.msg;
        teb->o.odin.msgstate++;
  }

  OldFrameProc = (PFNWP)win32wnd->getOldFrameProc();

  switch(msg)
  {
    case WM_FORMATFRAME:
        break;

    case WM_MINMAXFRAME:
    {
      PSWP swp = (PSWP)mp1;

      if (!win32wnd->IsWindowCreated()) goto RunDefFrameProc;
      dprintf(("PMFRAME: WM_MINMAXFRAME %x",hwnd));
      if ((swp->fl & SWP_MAXIMIZE) == SWP_MAXIMIZE)
      {
        win32wnd->setStyle((win32wnd->getStyle() & ~WS_MINIMIZE_W) | WS_MAXIMIZE_W);

        RECT rect;

        rect.left = rect.top = rect.right = rect.bottom = 0;
        win32wnd->AdjustMaximizedRect(&rect);
        swp->x += rect.left;
        swp->cx += rect.right-rect.left;
        swp->y -= rect.bottom;
        swp->cy += rect.bottom-rect.top;
      }
      else if ((swp->fl & SWP_MINIMIZE) == SWP_MINIMIZE)
      {
        win32wnd->setStyle((win32wnd->getStyle() & ~WS_MAXIMIZE_W) | WS_MINIMIZE_W);
      }
      else if ((swp->fl & SWP_RESTORE) == SWP_RESTORE)
      {
        win32wnd->setStyle(win32wnd->getStyle() & ~(WS_MINIMIZE_W | WS_MAXIMIZE_W));
      }
      goto RunDefFrameProc;
    }

    case WM_QUERYTRACKINFO:
    {
      PTRACKINFO trackInfo = (PTRACKINFO)mp2;

      RestoreOS2TIB();
      OldFrameProc(hwnd,msg,mp1,mp2);
      SetWin32TIB();
      trackInfo->cxBorder = 4;
      trackInfo->cyBorder = 4;
      win32wnd->AdjustTrackInfo((PPOINT)&trackInfo->ptlMinTrackSize,(PPOINT)&trackInfo->ptlMaxTrackSize);
      RestoreOS2TIB();
      return (MRESULT)TRUE;
    }

    case WM_QUERYBORDERSIZE:
    {
      PWPOINT size = (PWPOINT)mp1;

      size->x = 0;
      size->y = 0;
      RestoreOS2TIB();
      return (MRESULT)TRUE;
    }

    case WM_HITTEST:
    {
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
                win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd && win32wnd->IsWindowCreated())
        {
          MRESULT rc;

          rc = (MRESULT)win32wnd->MsgHitTest(pWinMsg);
          RestoreOS2TIB();
          return rc;
        }
        return (MRESULT)HT_NORMAL;
    }

    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON2DOWN:
    case WM_BUTTON2UP:
    case WM_BUTTON2DBLCLK:
    case WM_BUTTON3DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DBLCLK:
    {
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
                win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd && win32wnd->IsWindowCreated())
        {
          win32wnd->MsgButton(pWinMsg);
          RestoreOS2TIB();
        }
        return (MRESULT)TRUE;
    }

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
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
                win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd && win32wnd->IsWindowCreated())
                win32wnd->MsgMouseMove(pWinMsg);
        RestoreOS2TIB();
        return (MRESULT)TRUE;
    }

    case WM_PAINT:
    {
        dprintf(("PMFRAME: WM_PAINT"));
        if(win32wnd->getStyle() & WS_MINIMIZE_W)
                goto RunDefFrameProc;
        if(win32wnd->IsWindowCreated())
                win32wnd->MsgNCPaint();

        goto RunDefWndProc;
    }

    case WM_SIZE:
        dprintf(("PMFRAME: WM_SIZE"));
        goto RunDefFrameProc;

    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld;
      WINDOWPOS wp,wpOld;
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

        wpOld = wp;
        win32wnd->MsgPosChanging((LPARAM)&wp);

        if ((wp.hwndInsertAfter != wpOld.hwndInsertAfter) ||
            (wp.x != wpOld.x) || (wp.y != wpOld.y) || (wp.cx != wpOld.cx) || (wp.cy != wpOld.cy) || (wp.flags != wpOld.flags))
        {
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
        RestoreOS2TIB();
        return (MRESULT)0;
    }

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp   = (PSWP)mp1,pswpOld = pswp+1;
      SWP       swpOld = *(pswp + 1);
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE;
      RECTL rect;
      SWP swpClient = {0};

        dprintf(("PMFRAME: WM_WINDOWPOSCHANGED (%x) %x %x (%d,%d) (%d,%d)", mp2, win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
        {
                goto RunDefFrameProc;
        }

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto PosChangedEnd; //parent has just been destroyed
            }
        }

        OSLibMapSWPtoWINDOWPOSFrame(pswp, &wp, &swpOld, hParent, hwnd);

        if(pswp->fl & SWP_ACTIVATE)
        {
             WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
        }

        if((pswp->fl & (SWP_MOVE | SWP_SIZE)) && !(win32wnd->getStyle() & WS_MINIMIZE_W))
        {
          //Note: Also updates the new window rectangle
          win32wnd->MsgFormatFrame(&wp);

          //CB: todo: use result for WM_CALCVALIDRECTS
          mapWin32ToOS2Rect(win32wnd->getOS2FrameWindowHandle(), win32wnd->getClientRectPtr(), (PRECTLOS2)&rect);

          swpClient.hwnd = win32wnd->getOS2WindowHandle();
          swpClient.hwndInsertBehind = 0;
          swpClient.x  = rect.xLeft;
          swpClient.y  = rect.yBottom;
          swpClient.cx = rect.xRight-rect.xLeft;
          swpClient.cy = rect.yTop-rect.yBottom;
          //TODO: Get rid of SWP_SHOW; needed for winhlp32 button bar for now
          swpClient.fl = (pswp->fl & ~SWP_ZORDER) | SWP_MOVE | SWP_SHOW;
          WinSetMultWindowPos(teb->o.odin.hab, &swpClient, 1);

          //update child positions: rectWindow is in window coordinates
          if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
                  FrameUpdateChildPositions(win32wnd->getOS2WindowHandle());
          }

          if(win32wnd->CanReceiveSizeMsgs())
            win32wnd->MsgPosChanged((LPARAM)&wp);

          if ((pswp->fl & SWP_SIZE) && ((pswp->cx != pswpOld->cx) || (pswp->cy != pswpOld->cy)))
          {
            //redraw the frame (to prevent unnecessary client updates)
            BOOL redrawAll = FALSE;

            if (win32wnd->getWindowClass())
            {
              DWORD dwStyle = win32wnd->getWindowClass()->getClassLongA(GCL_STYLE_W);

              if ((dwStyle & CS_HREDRAW_W) && (pswp->cx != pswpOld->cx))
                redrawAll = TRUE;
              else if ((dwStyle & CS_VREDRAW_W) && (pswp->cy != pswpOld->cy))
                redrawAll = TRUE;
            } else redrawAll = TRUE;

            if (redrawAll)
            {
              //CB: redraw all children for now
              //    -> problems with update region if we don't do it
              //       todo: rewrite whole handling
              WinInvalidateRect(hwnd,NULL,TRUE);
            }
            else
            {
              HPS hps = WinGetPS(hwnd);
              RECTL frame,client,arcl[4];

              WinQueryWindowRect(hwnd,&frame);
               //top
              arcl[0].xLeft = 0;
              arcl[0].xRight = frame.xRight;
              arcl[0].yBottom = rect.yTop;
              arcl[0].yTop = frame.yTop;
               //right
              arcl[1].xLeft = rect.xRight;
              arcl[1].xRight = frame.xRight;
              arcl[1].yBottom = 0;
              arcl[1].yTop = frame.yTop;
               //left
              arcl[2].xLeft = 0;
              arcl[2].xRight = rect.xLeft;
              arcl[2].yBottom = 0;
              arcl[2].yTop = frame.yTop;
               //bottom
              arcl[3].xLeft = 0;
              arcl[3].xRight = frame.xRight;
              arcl[3].yBottom = 0;
              arcl[3].yTop = rect.yBottom;

              HRGN hrgn = GpiCreateRegion(hps,4,(PRECTL)&arcl);

              WinInvalidateRegion(hwnd,hrgn,FALSE);
              GpiDestroyRegion(hps,hrgn);
              WinReleasePS(hps);
            }
          }
        }
        else
        {
          //update child positions: rectWindow is in window coordinates
          if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
                  FrameUpdateChildPositions(win32wnd->getOS2WindowHandle());
          }

          if(win32wnd->CanReceiveSizeMsgs())
            win32wnd->MsgPosChanged((LPARAM)&wp);
        }

PosChangedEnd:
        RestoreOS2TIB();
        return (MRESULT)FALSE;
    }

    case WM_SHOW:
        dprintf(("PMFRAME: WM_SHOW"));
        break;

    case WM_ERASEBACKGROUND:
    {
        dprintf(("PMFRAME: WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        RestoreOS2TIB();
        return (MRESULT)FALSE;
    }

    case WM_CALCVALIDRECTS:
    {
      //don't redraw here or PM redraw the whole frame (done in WM_WINDOWPOSCHANGED)
      dprintf(("PMFRAME: WM_CALCVALIDRECTS"));
      RestoreOS2TIB();
      return (MRESULT)(CVR_ALIGNLEFT | CVR_ALIGNTOP);
    }

    case WM_ACTIVATE:
    {
        HWND hwndTitle;
        USHORT flags = WinQueryWindowUShort(hwnd,QWS_FLAGS);

        dprintf(("PMFRAME: WM_ACTIVATE %x %x %x", hwnd, mp1, mp2));
        if(win32wnd->IsWindowCreated())
          win32wnd->DispatchMsgA(pWinMsg);

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
        }
        else
        {
          WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
        }
        RestoreOS2TIB();
        return 0;
    }

    case WM_DESTROY:
      dprintf(("PMFRAME: WM_DESTROY %x",hwnd));
      WinSetVisibleRegionNotify(hwnd, FALSE);
      WinSubclassWindow(hwnd,OldFrameProc);
      win32wnd->setOldFrameProc(NULL);
      goto RunDefFrameProc;

    case WM_VRNENABLED:
        if(!win32wnd->isComingToTop() && ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == WS_EX_TOPMOST_W))
        {
                HWND hwndrelated;
                Win32BaseWindow *topwindow;

                win32wnd->setComingToTop(TRUE);

                hwndrelated = WinQueryWindow(hwnd, QW_PREV);
                dprintf(("WM_VRNENABLED hwndrelated = %x (hwnd=%x)", hwndrelated, hwnd));
                topwindow = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndrelated);
                if(topwindow == NULL || ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == 0)) {
                        //put window at the top of z order
                        WinSetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
                }

                win32wnd->setComingToTop(FALSE);
                break;
        }
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
//******************************************************************************
//******************************************************************************
PVOID FrameSubclassFrameWindow(Win32BaseWindow *win32wnd)
{
  return WinSubclassWindow(win32wnd->getOS2FrameWindowHandle(),PFNWP(Win32FrameProc));
}
//******************************************************************************
//******************************************************************************
VOID FrameUpdateClient(Win32BaseWindow *win32wnd)
{
  RECT rectOld, rectNew;
  RECTL rect;
  SWP swpClient = {0};

        rectOld = *win32wnd->getClientRectPtr();
        win32wnd->MsgFormatFrame(NULL);
        rectNew = *win32wnd->getClientRectPtr();
        if(WinEqualRect(0, (PRECTL)&rectOld, (PRECTL)&rectNew) == 1) {
                WinInvalidateRect(win32wnd->getOS2FrameWindowHandle(), NULL, FALSE);
                return;
        }
        //CB: todo: use result for WM_CALCVALIDRECTS
        mapWin32ToOS2Rect(win32wnd->getOS2FrameWindowHandle(), win32wnd->getClientRectPtr(), (PRECTLOS2)&rect);


        swpClient.hwnd = win32wnd->getOS2WindowHandle();
        swpClient.hwndInsertBehind = 0;
        swpClient.x  = rect.xLeft;
        swpClient.y  = rect.yBottom;
        swpClient.cx = rect.xRight-rect.xLeft;
        swpClient.cy = rect.yTop-rect.yBottom;
        swpClient.fl = SWP_MOVE | SWP_SIZE;
        WinSetMultWindowPos(GetThreadHAB(), &swpClient, 1);
}
//******************************************************************************
//******************************************************************************
