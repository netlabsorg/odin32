/* $Id: pmwindow.cpp,v 1.116 2001-02-18 14:18:38 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_WIN
#define INCL_GPI
#define INCL_DEV                /* Device Function definitions  */
#define INCL_GPICONTROL         /* GPI control Functions        */
#define INCL_DOSPROCESS
#define INCL_WINTRACKRECT

#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <winuser32.h>
#include <wprocess.h>
#include <misc.h>
#include <win32wbase.h>
#include <win32dlg.h>
#include "win32wdesktop.h"
#include "pmwindow.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibmsg.h"
#include "dc.h"
#include <thread.h>
#include <wprocess.h>
#include "caret.h"
#include "timer.h"
#include <codepage.h>

#define DBG_LOCALLOG    DBG_pmwindow
#include "dbglocal.h"

HMQ  hmq = 0;                             /* Message queue handle         */
HAB  hab = 0;

RECTL desktopRectl = {0};
ULONG ScreenWidth  = 0;
ULONG ScreenHeight = 0;
ULONG ScreenBitsPerPel = 0;

static PFNWP pfnFrameWndProc = NULL;

MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY Win32FrameWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT ProcessPMMessage(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2, Win32BaseWindow *win32wnd,
                         MSG *pWinMsg, TEB *teb, BOOL isFrame);

//******************************************************************************
//Initialize PM; create hab, message queue and register special Win32 window classes
//******************************************************************************
BOOL InitPM()
{

    hab = WinInitialize(0);
    dprintf(("Winitialize returned %x", hab));
    hmq = WinCreateMsgQueue(hab, 0);

    if(!hab || !hmq)
    {
        UINT error;
        //CB: only fail on real error
        error = WinGetLastError(hab) & 0xFFFF; //error code
        if (!hab || (error != PMERR_MSG_QUEUE_ALREADY_EXISTS))
        {
            dprintf(("WinInitialize or WinCreateMsgQueue failed %x %x", hab, hmq));
            dprintf((" Error = %x",error));
            return(FALSE);
        }
        else
        {
            if(!hab) {
                hab = WinQueryAnchorBlock(HWND_DESKTOP);
                dprintf(("WinQueryAnchorBlock returned %x", hab));
            }
            if(!hmq) {
                hmq = HMQ_CURRENT;
            }
        }
    }
    SetThreadHAB(hab);
    dprintf(("InitPM: hmq = %x", hmq));
    SetThreadMessageQueue(hmq);

    BOOL rc = WinSetCp(hmq, GetDisplayCodepage());
    dprintf(("InitPM: WinSetCP was %sOK", rc ? "" : "not "));

    if(!WinRegisterClass(                 /* Register window class        */
        hab,                               /* Anchor block handle          */
        (PSZ)WIN32_STDCLASS,               /* Window class name            */
        (PFNWP)Win32WindowProc,            /* Address of window procedure  */
        CS_HITTEST,
        NROF_WIN32WNDBYTES))
    {
            dprintf(("WinRegisterClass Win32BaseWindow failed"));
            return(FALSE);
    }

    CLASSINFO FrameClassInfo;
    if(!WinQueryClassInfo (hab, WC_FRAME, &FrameClassInfo)) {
        dprintf (("WinQueryClassInfo WC_FRAME failed"));
        return (FALSE);
    }
    pfnFrameWndProc = FrameClassInfo.pfnWindowProc;

    dprintf(("WC_FRAME style %x", FrameClassInfo.flClassStyle));

    if(!WinRegisterClass(                 /* Register window class        */
        hab,                               /* Anchor block handle          */
        (PSZ)WIN32_STDFRAMECLASS,          /* Window class name            */
        (PFNWP)Win32FrameWindowProc,       /* Address of window procedure  */
        CS_HITTEST | CS_FRAME,
        FrameClassInfo.cbWindowData+NROF_WIN32WNDBYTES))
    {
        dprintf(("WinRegisterClass Win32BaseWindow failed %x", WinGetLastError(hab)));
        return(FALSE);
    }

    WinQueryWindowRect(HWND_DESKTOP, &desktopRectl);
    ScreenWidth  = desktopRectl.xRight;
    ScreenHeight = desktopRectl.yTop;

    HDC   hdc;              /* Device-context handle                */
    /* context data structure */
    DEVOPENSTRUC dop = {NULL, "DISPLAY", NULL, NULL, NULL, NULL,
                        NULL, NULL, NULL};

    /* create memory device context */
    hdc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);
    DevQueryCaps(hdc, CAPS_COLOR_BITCOUNT, 1, (PLONG)&ScreenBitsPerPel);
    DevCloseDC(hdc);

    dprintf(("InitPM: Desktop (%d,%d)", ScreenWidth, ScreenHeight));
    return TRUE;
} /* End of main */
//******************************************************************************
//Win32 window message handler
//******************************************************************************
MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 Win32BaseWindow *win32wnd;
 TEB             *teb;
 MSG              winMsg, *pWinMsg;
 MRESULT          rc = 0;

    //Restore our FS selector
    SetWin32TIB();

    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- BEGIN
    teb = GetThreadTEB();
    win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

    if(!teb || (msg != WM_CREATE && win32wnd == NULL)) {
        dprintf(("Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
        goto RunDefWndProc;
    }
////    if(teb->o.odin.fIgnoreMsgs) {
////        goto RunDefWndProc;
////    }

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
    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- END
    rc = ProcessPMMessage(hwnd, msg, mp1, mp2, win32wnd, pWinMsg, teb, FALSE);
    RestoreOS2TIB();
    return rc;

RunDefWndProc:
    RestoreOS2TIB();
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
//******************************************************************************
//******************************************************************************
MRESULT ProcessPMMessage(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2, Win32BaseWindow *win32wnd, MSG *pWinMsg, TEB *teb, BOOL isFrame)
{
 POSTMSG_PACKET  *postmsg;
 OSLIBPOINT       point, ClientPoint;
 MRESULT          rc = 0;

  if(msg == WIN32APP_POSTMSG) {
        //probably win32 app user message
        if((ULONG)mp1 == WIN32MSG_MAGICA) {
            return (MRESULT)win32wnd->DispatchMsgA(pWinMsg);
        }
        else
        if((ULONG)mp1 == WIN32MSG_MAGICW) {
            return (MRESULT)win32wnd->DispatchMsgW(pWinMsg);
        }
  }
  else
  if(msg == WIN32APP_SETFOCUSMSG) {
      //PM doesn't allow SetFocus calls during WM_SETFOCUS message processing;
      //must delay this function call
      //mp1 = win32 window handle
      //mp2 = activate flag
      dprintf(("USER32: Delayed SetFocus %x call!", mp1));
      teb->o.odin.hwndFocus = 0;
      WinFocusChange(HWND_DESKTOP, hwnd, mp2 ? FC_NOLOSEACTIVE : 0);
  }

  switch( msg )
  {
    //OS/2 msgs
    case WM_CREATE:
    {
        if(teb->o.odin.newWindow == 0)
            goto createfail;

        //Processing is done in after WinCreateWindow returns
        dprintf(("OS2: WM_CREATE %x", hwnd));
        win32wnd = (Win32BaseWindow *)teb->o.odin.newWindow;
        teb->o.odin.newWindow = 0;
        if(win32wnd->MsgCreate(hwnd) == FALSE)
        {
            return (MRESULT)TRUE; //discontinue window creation
        }
    createfail:
        return (MRESULT)FALSE;
    }

    case WM_QUIT:
        dprintf(("OS2: WM_QUIT %x", hwnd));
        win32wnd->MsgQuit();
        break;

    case WM_CLOSE:
        dprintf(("OS2: WM_CLOSE %x", hwnd));
        win32wnd->MsgClose();
        break;

    case WM_DESTROY:
        dprintf(("OS2: WM_DESTROY %x", hwnd));
        win32wnd->MsgDestroy();
        WinSetVisibleRegionNotify(hwnd, FALSE);
        goto RunDefWndProc;

    case WM_ENABLE:
        dprintf(("OS2: WM_ENABLE %x", hwnd));
        win32wnd->MsgEnable(SHORT1FROMMP(mp1));
        break;

    case WM_SHOW:
        dprintf(("OS2: WM_SHOW %x %d", hwnd, mp1));
        win32wnd->MsgShow((ULONG)mp1);
        break;

    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld;
      WINDOWPOS wp,wpOld;
      HWND      hParent = NULLHANDLE, hwndAfter;

        dprintf(("OS2: WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if(win32wnd->getParent() && win32wnd->getParent()->isOwnDC()) {
            dprintfOrigin(win32wnd->getParent()->getOwnDC());
            selectClientArea(win32wnd->getParent(), win32wnd->getParent()->getOwnDC());
        }

        if(pswp->fl & SWP_NOADJUST) {
            //ignore weird messages (TODO: why are they sent?)
            break;
        }
        //CB: show dialog in front of owner
        if (win32wnd->IsModalDialogOwner())
        {
            dprintf(("win32wnd->IsModalDialogOwner %x", win32wnd->getWindowHandle()));
            pswp->fl |= SWP_ZORDER;
            pswp->hwndInsertBehind = win32wnd->getOS2HwndModalDialog();
            if (pswp->fl & SWP_ACTIVATE)
            {
                pswp->fl &= ~SWP_ACTIVATE;
                WinSetWindowPos(win32wnd->getOS2HwndModalDialog(),0,0,0,0,0,SWP_ACTIVATE);
            }
        }

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
            goto RunDefWndProc;

        if(!win32wnd->CanReceiveSizeMsgs())
            break;

        WinQueryWindowPos(hwnd, &swpOld);
        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto RunDefWndProc;
            }
        }
        hwndAfter = pswp->hwndInsertBehind;
        if(win32wnd->getParent()) {
            OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getWindowHeight(),
                                   win32wnd->getParent()->getClientRectPtr()->left,
                                   win32wnd->getParent()->getClientRectPtr()->top,
                                   hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), 0, 0, hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           if(wndAfter) {
                wp.hwndInsertAfter = wndAfter->getWindowHandle();
           }
           else wp.hwndInsertAfter = HWND_TOP_W;
        }

        wpOld = wp;
        win32wnd->MsgPosChanging((LPARAM)&wp);

        if ((wp.hwndInsertAfter != wpOld.hwndInsertAfter) ||
            (wp.x != wpOld.x) || (wp.y != wpOld.y) || (wp.cx != wpOld.cx) || (wp.cy != wpOld.cy) || (wp.flags != wpOld.flags))
        {
            ULONG flags = pswp->fl;      //make a backup copy; OSLibMapWINDOWPOStoSWP will modify it

            dprintf(("OS2: WM_ADJUSTWINDOWPOS, app changed windowpos struct"));
            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

            if(win32wnd->getParent()) {
                  OSLibMapWINDOWPOStoSWP(&wp, pswp, &swpOld, win32wnd->getParent()->getWindowHeight(),
                                         win32wnd->getParent()->getClientRectPtr()->left,
                                         win32wnd->getParent()->getClientRectPtr()->top,
                                         hwnd);
            }
            else  OSLibMapWINDOWPOStoSWP(&wp, pswp, &swpOld, OSLibQueryScreenHeight(), 0, 0, hwnd);

            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

            //OSLibMapWINDOWPOStoSWP can add flags, but we must not let it remove flags!
            if(pswp->fl & SWP_SIZE)
                flags |= SWP_SIZE;

            if(pswp->fl & SWP_MOVE)
                flags |= SWP_MOVE;

            pswp->fl = flags;   //restore flags

            pswp->fl |= SWP_NOADJUST;
            pswp->hwndInsertBehind = hwndAfter;
            pswp->hwnd = hwnd;

            return (MRESULT)0xf;
        }
        return (MRESULT)0;
    }

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp   = (PSWP)mp1,pswpOld = pswp+1;
      SWP       swpOld = *(pswp + 1);
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE;
      RECTL rect;

        dprintf(("OS2: WM_WINDOWPOSCHANGED (%x) %x %x (%d,%d) (%d,%d)", mp2, win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
        {
            if(pswp->fl & SWP_ACTIVATE)
            {
                //Only send PM WM_ACTIVATE to top-level windows (frame windows)
                if(!(WinQueryWindowULong(hwnd, OFFSET_WIN32FLAGS) & FF_ACTIVE))
                {
                        WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
                }
            }
            goto RunDefWndProc;
        }

        if(pswp->fl & (SWP_MOVE | SWP_SIZE))
        {
            if(win32wnd->isChild())
            {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto PosChangedEnd; //parent has just been destroyed
            }
        }


        if(win32wnd->getParent()) {
          OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getWindowHeight(),
                                     win32wnd->getParent()->getClientRectPtr()->left,
                                     win32wnd->getParent()->getClientRectPtr()->top,
                                     hwnd);
        }
        else  OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), 0, 0, hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           if(wndAfter) {
                wp.hwndInsertAfter = wndAfter->getWindowHandle();
           }
           else wp.hwndInsertAfter = HWND_TOP_W;
        }

        if((pswp->fl & (SWP_MOVE | SWP_SIZE)) && !(win32wnd->getStyle() & WS_MINIMIZE_W))
        {
          //CB: todo: use result for WM_CALCVALIDRECTS
          //Get old client rectangle (for invalidation of frame window parts later on)
          mapWin32ToOS2Rect(win32wnd->getWindowHeight(), win32wnd->getClientRectPtr(), (PRECTLOS2)&rect);

          //Note: Also updates the new window rectangle
          win32wnd->MsgFormatFrame(&wp);

          if(win32wnd->CanReceiveSizeMsgs())
            win32wnd->MsgPosChanged((LPARAM)&wp);

          if((pswp->fl & SWP_SIZE) && ((pswp->cx != pswpOld->cx) || (pswp->cy != pswpOld->cy)))
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
          if(win32wnd->CanReceiveSizeMsgs())
            win32wnd->MsgPosChanged((LPARAM)&wp);
        }

        if(pswp->fl & SWP_ACTIVATE)
        {
             //Only send PM WM_ACTIVATE to top-level windows (frame windows)
             if(!(WinQueryWindowULong(hwnd, OFFSET_WIN32FLAGS) & FF_ACTIVE))
             {
                if(isFrame) {
                     WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
                }
                else
                if(win32wnd->IsWindowCreated()) {
                    win32wnd->MsgActivate(1, 0, win32wnd->getWindowHandle(), hwnd);
                }
             }
        }

PosChangedEnd:
        return (MRESULT)FALSE;
    }

    case WM_ACTIVATE:
    {
        USHORT flags = WinQueryWindowULong(hwnd, OFFSET_WIN32FLAGS);

        dprintf(("OS2: WM_ACTIVATE %x %x %x", hwnd, mp1, mp2));

        WinSetWindowULong(hwnd, OFFSET_WIN32FLAGS, SHORT1FROMMP(mp1) ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
        if(win32wnd->IsWindowCreated())
        {
            win32wnd->MsgActivate((LOWORD(pWinMsg->wParam) == WA_ACTIVE_W) ? 1 : 0, HIWORD(pWinMsg->wParam), pWinMsg->lParam, (HWND)mp2);

            //CB: show owner behind the dialog
            if(win32wnd->IsModalDialog())
            {
                Win32BaseWindow *topOwner = win32wnd->getOwner()->GetTopParent();

                if(topOwner) WinSetWindowPos(topOwner->getOS2WindowHandle(),hwnd,0,0,0,0,SWP_ZORDER);
            }
        }
        return 0;
    }

    case WM_SIZE:
    {
        dprintf(("OS2: WM_SIZE (%d,%d) (%d,%d)", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2), SHORT1FROMMP(mp1), SHORT2FROMMP(mp2)));
        goto RunDefWndProc;
    }

    case WM_CALCVALIDRECTS:
#if 0
    {
      PRECTL oldRect = (PRECTL)mp1,newRect = oldRect+1;
      UINT res = CVR_ALIGNLEFT | CVR_ALIGNTOP;

//CB: todo: use WM_NCCALCSIZE result
      if (win32wnd->getWindowClass())
      {
        DWORD dwStyle = win32wnd->getWindowClass()->getClassLongA(GCL_STYLE_W);

        if ((dwStyle & CS_HREDRAW_W) && (newRect->xRight-newRect->xLeft != oldRect->xRight-oldRect->xLeft))
            res |= CVR_REDRAW;
        else
        if ((dwStyle & CS_VREDRAW_W) && (newRect->yTop-newRect->yBottom != oldRect->yTop-oldRect->yBottom))
            res |= CVR_REDRAW;
      }
      else res |= CVR_REDRAW;

      return (MRESULT)res;
    }
#else
      dprintf(("PMWINDOW: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));
      return (MRESULT)(CVR_ALIGNLEFT | CVR_ALIGNTOP);
#endif

    case WM_VRNENABLED:
        dprintf(("OS2: WM_VRNENABLED %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        if(!win32wnd->isComingToTop() && ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == WS_EX_TOPMOST_W))
        {
                HWND hwndrelated;
                Win32BaseWindow *topwindow;

                win32wnd->setComingToTop(TRUE);

                hwndrelated = WinQueryWindow(hwnd, QW_PREV);
                dprintf(("WM_VRNENABLED hwndrelated = %x (hwnd=%x)", hwndrelated, hwnd));
                topwindow = Win32BaseWindow::GetWindowFromOS2Handle(hwndrelated);
                if(topwindow == NULL || ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == 0)) {
                        //put window at the top of z order
                        WinSetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
                }

                win32wnd->setComingToTop(FALSE);
                break;
        }
//test
        if(win32wnd->isOwnDC()) {
                dprintfOrigin(win32wnd->getOwnDC());
                selectClientArea(win32wnd, win32wnd->getOwnDC());
        }
//test
        goto RunDefWndProc;

    case WM_VRNDISABLED:
        dprintf(("OS2: WM_VRNDISABLED %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        if(win32wnd->isOwnDC()) {
                dprintfOrigin(win32wnd->getOwnDC());
        }
        goto RunDefWndProc;

    case WM_SETFOCUS:
    {
      HWND hwndFocus = (HWND)mp1;

        dprintf(("OS2: WM_SETFOCUS %x %x (%x) %d", win32wnd->getWindowHandle(), mp1, OS2ToWin32Handle(hwndFocus), mp2));

        //PM doesn't allow SetFocus calls during WM_SETFOCUS message processing;
        //must delay this function call

        teb->o.odin.fWM_SETFOCUS = TRUE;
        teb->o.odin.hwndFocus    = 0;
        if(WinQueryWindowULong(hwndFocus, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC) {
                //another (non-win32) application's window
                //set to NULL (allowed according to win32 SDK) to avoid problems
                hwndFocus = NULL;
        }
        if((ULONG)mp2 == TRUE) {
                HWND hwndFocusWin32 = OS2ToWin32Handle(hwndFocus);
                recreateCaret (hwndFocusWin32);
                win32wnd->MsgSetFocus(hwndFocusWin32);
        }
        else win32wnd->MsgKillFocus(OS2ToWin32Handle(hwndFocus));
        teb->o.odin.fWM_SETFOCUS = FALSE;

        break;
    }

#if 0
    //is sent to both windows gaining and loosing the focus
    case WM_FOCUSCHANGE:
    {
     HWND   hwndFocus = (HWND)mp1;
     HWND   hwndLoseFocus, hwndGainFocus;
     USHORT usSetFocus = SHORT1FROMMP(mp2);
     USHORT fsFocusChange = SHORT2FROMMP(mp2);

        rc = 0;
        dprintf(("OS2: WM_FOCUSCHANGE (start) %x %x %x %x", win32wnd->getWindowHandle(), hwndFocus, usSetFocus, fsFocusChange));
        if(usSetFocus) {
            hwndGainFocus = hwnd;
            hwndLoseFocus = hwndFocus;
        }
        else {
            hwndGainFocus = hwndFocus;
            hwndLoseFocus = hwnd;
        }

        if(usSetFocus)
        {
          Win32BaseWindow *winfocus = Win32BaseWindow::GetWindowFromOS2Handle(hwndLoseFocus);
          if(!(fsFocusChange & FC_NOSETACTIVE))
          {
            if(!winfocus || (winfocus->GetTopParent() != win32wnd->GetTopParent()))
            {
              if(winfocus)
                WinSendMsg(winfocus->GetTopParent()->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)0, (MPARAM)hwndGainFocus);
              else
                WinSendMsg(hwndLoseFocus, WM_ACTIVATE, (MPARAM)0, (MPARAM)hwndGainFocus);
            }
          }
          //SvL: Check if window is still valid
          win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
          if(win32wnd == NULL)
            return (MRESULT)rc;

          if(!(fsFocusChange & FC_NOSETACTIVE))
          {
            Win32BaseWindow *topparent = win32wnd->GetTopParent();
            if(!winfocus || (winfocus->GetTopParent() != topparent))
            {
              if(!(fsFocusChange & FC_NOBRINGTOTOP))
              {
                if(topparent) {
                  //put window at the top of z order
                  WinSetWindowPos(topparent->getOS2WindowHandle(), HWND_TOP, 0, 0, 0, 0, SWP_ZORDER);
                }
              }

              // PH 2000/09/01 Netscape 4.7
              // check if topparent is valid
              if (topparent)
                WinSendMsg(topparent->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)1, (MPARAM)hwndLoseFocus);
            }
          }
          //SvL: Check if window is still valid
          win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
          if(win32wnd == NULL)
            return (MRESULT)rc;

      //TODO: Don't send WM_SETSELECTION to child window if frame already has selection
      if(!(fsFocusChange & FC_NOSETSELECTION)) {
            WinSendMsg(hwndGainFocus, WM_SETSELECTION, (MPARAM)1, (MPARAM)0);
      }

      if(!(fsFocusChange & FC_NOSETFOCUS)) {
            WinSendMsg(hwndGainFocus, WM_SETFOCUS, (MPARAM)hwndLoseFocus, (MPARAM)1);
      }
        }
        else /* no usSetFocus */
        {
      if(!(fsFocusChange & FC_NOLOSEFOCUS)) {
            WinSendMsg(hwndLoseFocus, WM_SETFOCUS, (MPARAM)hwndGainFocus, (MPARAM)0);
      }
      //TODO: Don't send WM_SETSELECTION to child window if frame already has selection
      if(!(fsFocusChange & FC_NOLOSESELECTION)) {
            WinSendMsg(hwndLoseFocus, WM_SETSELECTION, (MPARAM)0, (MPARAM)0);
      }
          //SvL: Check if window is still valid
          win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
          if(win32wnd == NULL) {
                return (MRESULT)rc;
          }

          Win32BaseWindow *winfocus = Win32BaseWindow::GetWindowFromOS2Handle(hwndGainFocus);
          if(!(fsFocusChange & FC_NOLOSEACTIVE))
          {
            Win32BaseWindow *topparent = win32wnd->GetTopParent();

            if(!winfocus || (winfocus->GetTopParent() != topparent))
            {
              // PH 2000/09/01 Netscape 4.7
              // check if topparent is valid
              if (topparent)
                WinSendMsg(topparent->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)0, (MPARAM)hwndGainFocus);
            }
          }
          //SvL: Check if window is still valid
          win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
          if(win32wnd == NULL)
            return (MRESULT)rc;

          if(!(fsFocusChange & FC_NOSETACTIVE))
          {
            if(!winfocus || (winfocus->GetTopParent() != win32wnd->GetTopParent()))
            {
              if(winfocus)
              {
                // PH 2000/09/01 Netscape 4.7
                // check if topparent is valid
                Win32BaseWindow *topparent = winfocus->GetTopParent();
                if (topparent)
                  WinSendMsg(topparent->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)1, (MPARAM)hwndLoseFocus);
              }
              else
                WinSendMsg(hwndGainFocus, WM_ACTIVATE, (MPARAM)1, (MPARAM)hwndLoseFocus);
            }
          }
        }


#ifdef DEBUG
        SetWin32TIB();
        dprintf(("OS2: WM_FOCUSCHANGE (end) %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
#endif
        return (MRESULT)rc;
    }
#endif

    //**************************************************************************
    //Mouse messages (OS/2 Window coordinates -> Win32 coordinates relative to screen
    //**************************************************************************
    case WM_HITTEST:
    {
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd && win32wnd->IsWindowCreated())
        {
            MRESULT rc;

            rc = (MRESULT)win32wnd->MsgHitTest(pWinMsg);
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
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgButton(pWinMsg);

        rc = (MRESULT)TRUE;
        break;

    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:
    case WM_BUTTON2CLICK:
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:
    case WM_BUTTON1CLICK:
    case WM_BUTTON3MOTIONSTART:
    case WM_BUTTON3MOTIONEND:
    case WM_BUTTON3CLICK:
        rc = (MRESULT)TRUE;
        break;

    case WM_MOUSEMOVE:
    {
    if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
        win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
    }
        if(win32wnd)
            win32wnd->MsgMouseMove(pWinMsg);
        break;
    }

    case WM_CONTROL:
        goto RunDefWndProc;

    case WM_COMMAND:
        dprintf(("OS2: WM_COMMAND %x %x %x", hwnd, mp1, mp2));
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_SYSCOMMAND:
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_RENDERFMT:
    case WM_RENDERALLFMTS:
    case WM_DESTROYCLIPBOARD:
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_CHAR:
        win32wnd->MsgChar(pWinMsg);
        break;

    case WM_TIMER:
        win32wnd->DispatchMsgA(pWinMsg);
        goto RunDefWndProc;

    case WM_SETWINDOWPARAMS:
    {
        WNDPARAMS *wndParams = (WNDPARAMS *)mp1;

        dprintf(("OS2: WM_SETWINDOWPARAMS %x", hwnd));
        if(wndParams->fsStatus & WPM_TEXT) {
            win32wnd->MsgSetText(wndParams->pszText, wndParams->cchText);
        }
        goto RunDefWndProc;
    }

    case WM_QUERYWINDOWPARAMS:
    {
     PWNDPARAMS wndpars = (PWNDPARAMS)mp1;
     ULONG textlen;
     PSZ   wintext;

        if(wndpars->fsStatus & (WPM_CCHTEXT | WPM_TEXT))
        {
            if(wndpars->fsStatus & WPM_TEXT)
                win32wnd->MsgGetText(wndpars->pszText, wndpars->cchText);
            if(wndpars->fsStatus & WPM_CCHTEXT)
                wndpars->cchText = win32wnd->MsgGetTextLength();

            wndpars->fsStatus = 0;
            wndpars->cbCtlData = 0;
            wndpars->cbPresParams = 0;
            return (MRESULT)TRUE;
        }
        goto RunDefWndProc;
    }

    case WM_PAINT:
    {
      RECTL rectl;
      BOOL  rc;

        rc = WinQueryUpdateRect(hwnd, &rectl);
        dprintf(("OS2: WM_PAINT %x (%d,%d) (%d,%d) rc=%d", win32wnd->getWindowHandle(), rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop, rc));

        if(rc && win32wnd->IsWindowCreated() && (rectl.xLeft != rectl.xRight &&
           rectl.yBottom != rectl.yTop))
        {
            PRECT pClient = win32wnd->getClientRectPtr();
            PRECT pWindow = win32wnd->getWindowRect();

            if(!(pClient->left == 0 && pClient->top == 0 &&
               win32wnd->getClientHeight() == win32wnd->getWindowHeight() &&
               win32wnd->getClientWidth()  == win32wnd->getWindowWidth()))
            {
                win32wnd->MsgNCPaint();
            }
            win32wnd->DispatchMsgA(pWinMsg);
        }
        else    goto RunDefWndProc;

        //SvL: Not calling the default window procedure causes all sorts of
        //     strange problems (redraw & hanging app)
        //     -> check what WinBeginPaint does what we're forgetting in BeginPaint
//      WinQueryUpdateRect(hwnd, &rectl);
//        if(rectl.xLeft == 0 && rectl.yTop == 0 && rectl.xRight == 0 && rectl.yBottom == 0) {
//          RestoreOS2TIB();
//          return (MRESULT)FALSE;
//  }
//  dprintf(("Update rectangle (%d,%d)(%d,%d) not empty, msg %x", rectl.xLeft, rectl.yTop, rectl.xRight, rectl.yBottom, pWinMsg->message));
//  goto RunDefWndProc;
        break;
    }

    case WM_ERASEBACKGROUND:
    {
        dprintf(("OS2: WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        return (MRESULT)FALSE;
    }

#if 0
    case WM_CONTEXTMENU:
    {
        win32wnd->DispatchMsgA(pWinMsg);

        return (MRESULT)TRUE;
    }
#endif

    case WM_QUERYTRACKINFO:
    {
      PTRACKINFO trackInfo = (PTRACKINFO)mp2;

        dprintf(("OS2: WM_QUERYTRACKINFO %x", win32wnd->getWindowHandle()));
        trackInfo->cxBorder = 4;
        trackInfo->cyBorder = 4;
        win32wnd->AdjustTrackInfo((PPOINT)&trackInfo->ptlMinTrackSize,(PPOINT)&trackInfo->ptlMaxTrackSize);
        return (MRESULT)TRUE;
    }

    case WM_QUERYBORDERSIZE:
    {
      PWPOINT size = (PWPOINT)mp1;

      dprintf(("OS2: WM_QUERYBORDERSIZE %x", win32wnd->getWindowHandle()));

      size->x = 0;
      size->y = 0;
      return (MRESULT)TRUE;
    }

    case WM_REALIZEPALETTE:
    {
        dprintf(("OS2: WM_REALIZEPALETTE"));
        goto RunDefWndProc;
    }

    case WM_OWNERPOSCHANGE:
    {
        dprintf(("OS2: WM_OWNERPOSCHANGE"));
        goto RunDefWndProc;
    }

    case WM_INITMENU:
    case WM_MENUSELECT:
    case WM_MENUEND:
    case WM_NEXTMENU:
    case WM_SYSCOLORCHANGE:
    case WM_SYSVALUECHANGED:
    case WM_SETSELECTION:
    case WM_PPAINT:
    case WM_PSETFOCUS:
    case WM_PSYSCOLORCHANGE:
    case WM_PSIZE:
    case WM_PACTIVATE:
    case WM_PCONTROL:
    case WM_HELP:
    case WM_APPTERMINATENOTIFY:
    case WM_PRESPARAMCHANGED:
    case WM_DRAWITEM:
    case WM_MEASUREITEM:
    case WM_CONTROLPOINTER:
    case WM_QUERYDLGCODE:
    case WM_SUBSTITUTESTRING:
    case WM_MATCHMNEMONIC:
    case WM_SAVEAPPLICATION:
    case WM_SEMANTICEVENT:
    default:
        dprintf2(("OS2: RunDefWndProc hwnd %x msg %x mp1 %x mp2 %x", hwnd, msg, mp1, mp2));
        RestoreOS2TIB();
        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT)rc;

RunDefWndProc:
//  dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
  RestoreOS2TIB();
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
} /* End of Win32WindowProc */
//******************************************************************************
//******************************************************************************
MRESULT EXPENTRY Win32FrameWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 POSTMSG_PACKET  *postmsg;
 OSLIBPOINT       point, ClientPoint;
 Win32BaseWindow *win32wnd;
 TEB             *teb;
 MRESULT          rc = 0;
 MSG              winMsg, *pWinMsg;

    //Restore our FS selector
    SetWin32TIB();

    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- BEGIN
    teb = GetThreadTEB();
    win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

    if(!teb || (msg != WM_CREATE && win32wnd == NULL)) {
        dprintf(("Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
        goto RunDefFrameWndProc;
    }
////    if(teb->o.odin.fIgnoreMsgs) {
////        goto RunDefWndProc;
////    }

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
    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- END

    switch( msg )
    {
    case WM_CREATE:
    {
        RestoreOS2TIB();
        pfnFrameWndProc(hwnd, msg, mp1, mp2);
        SetWin32TIB();
        rc = ProcessPMMessage(hwnd, msg, mp1, mp2, win32wnd, pWinMsg, teb, TRUE);
        break;
    }

    case WM_CALCFRAMERECT:
        dprintf(("OS2: WM_CALCFRAMERECT %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)TRUE;
        break;

    case WM_QUERYCTLTYPE:
        // This is a frame window
        dprintf(("OS2: WM_QUERYCTLTYPE %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)CCT_FRAME;
        break;

    case WM_QUERYFOCUSCHAIN:
        dprintf(("OS2: WM_QUERYFOCUSCHAIN %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;

    case WM_FOCUSCHANGE:
    {
        HWND   hwndFocus = (HWND)mp1;
        HWND   hwndLoseFocus, hwndGainFocus;
        USHORT usSetFocus = SHORT1FROMMP(mp2);
        USHORT fsFocusChange = SHORT2FROMMP(mp2);

        dprintf(("OS2: WM_FOCUSCHANGE %x %x %x %x", win32wnd->getWindowHandle(), hwndFocus, usSetFocus, fsFocusChange));
        goto RunDefFrameWndProc;
    }

    case WM_ACTIVATE:
    case WM_SETFOCUS:
    {
        rc = ProcessPMMessage(hwnd, msg, mp1, mp2, win32wnd, pWinMsg, teb, TRUE);
        goto RunDefFrameWndProc;
    }

#if 0
//just a test
    case WM_ADJUSTWINDOWPOS:
    case WM_WINDOWPOSCHANGED:
    {
        rc = ProcessPMMessage(hwnd, msg, mp1, mp2, win32wnd, pWinMsg, teb, TRUE);
        goto RunDefFrameWndProc;
    }
#endif

    case WM_QUERYFRAMEINFO:
        dprintf(("OS2: WM_QUERYFRAMEINFO %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;

    case WM_FORMATFRAME:
        dprintf(("OS2: WM_FORMATFRAME %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;

    case WM_ADJUSTFRAMEPOS:
    {
        PSWP pswp   = (PSWP)mp1;

        dprintf(("OS2: WM_ADJUSTFRAMEPOS %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        goto RunDefFrameWndProc;
    }

    case WM_MINMAXFRAME:
    {
        PSWP swp = (PSWP)mp1;

        if (!win32wnd->IsWindowCreated()) goto RunDefWndProc;

        dprintf(("OS2: WM_MINMAXFRAME %x",hwnd));
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
        else
        if ((swp->fl & SWP_MINIMIZE) == SWP_MINIMIZE)
        {
            win32wnd->setStyle((win32wnd->getStyle() & ~WS_MAXIMIZE_W) | WS_MINIMIZE_W);
        }
        else
        if ((swp->fl & SWP_RESTORE) == SWP_RESTORE)
        {
            win32wnd->setStyle(win32wnd->getStyle() & ~(WS_MINIMIZE_W | WS_MAXIMIZE_W));
        }
        goto RunDefWndProc;
    }

    case WM_UPDATEFRAME:
        dprintf(("OS2: WM_UPDATEFRAME %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;

    default:
        rc = ProcessPMMessage(hwnd, msg, mp1, mp2, win32wnd, pWinMsg, teb, TRUE);
        break;
    }
    RestoreOS2TIB();
    return (MRESULT)rc;

RunDefFrameWndProc:
    RestoreOS2TIB();
    return pfnFrameWndProc(hwnd, msg, mp1, mp2);

RunDefWndProc:
    RestoreOS2TIB();
    return Win32WindowProc(hwnd, msg, mp1, mp2);
}
//******************************************************************************
//TODO: Quickly moving a window two times doesn't force a repaint (1st time)
//******************************************************************************
VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags)
{
  TRACKINFO track;
  RECTL     rcl;
  PRECT     pWindowRect, pClientRect;
  HWND      hwndTracking;
  HPS       hpsTrack;
  LONG      parentHeight, parentWidth;

    dprintf(("FrameTrackFrame: %x %x", win32wnd->getWindowHandle(), flags));
    track.cxBorder = 4;
    track.cyBorder = 4;  /* 4 pel wide lines used for rectangle */
    track.cxGrid = 1;
    track.cyGrid = 1;    /* smooth tracking with mouse */
    track.cxKeyboard = 8;
    track.cyKeyboard = 8; /* faster tracking using cursor keys */

    pWindowRect = win32wnd->getWindowRect();
    if(win32wnd->getParent()) {
        parentHeight = win32wnd->getParent()->getWindowHeight();
        parentWidth  = win32wnd->getParent()->getWindowWidth();
        hwndTracking = win32wnd->getParent()->getOS2WindowHandle();
        hpsTrack     = WinGetPS(hwndTracking);
    }
    else {
        parentHeight = OSLibQueryScreenHeight();
        parentWidth  = OSLibQueryScreenWidth();
        hwndTracking = HWND_DESKTOP;
        hpsTrack     = NULL;
    }

    mapWin32ToOS2Rect(parentHeight, pWindowRect, (PRECTLOS2)&track.rclTrack);
    WinQueryWindowRect(hwndTracking, &track.rclBoundary);

    track.ptlMinTrackSize.x = 10;
    track.ptlMinTrackSize.y = 10;  /* set smallest allowed size of rectangle */
    track.ptlMaxTrackSize.x = parentWidth;
    track.ptlMaxTrackSize.y = parentHeight; /* set largest allowed size of rectangle */

    win32wnd->AdjustTrackInfo((PPOINT)&track.ptlMinTrackSize, (PPOINT)&track.ptlMaxTrackSize);

    track.fs = flags;

    if(WinTrackRect(hwndTracking, NULL, &track) )
    {
        if(hpsTrack)    WinReleasePS(hpsTrack);

        /* if successful copy final position back */
        if(!WinEqualRect(0, &rcl, &track.rclTrack)) {
            dprintf(("FrameTrackFrame: new (os/2) window rect: (%d,%d)(%d,%d)", track.rclTrack.xLeft, track.rclTrack.yBottom, track.rclTrack.xRight - track.rclTrack.xLeft, track.rclTrack.yTop - track.rclTrack.yBottom));
        if(flags == TF_MOVE) {
            WinSetWindowPos(win32wnd->getOS2WindowHandle(),
                                        0, track.rclTrack.xLeft, track.rclTrack.yBottom,
                                        0, 0, SWP_MOVE);
        }
        else {
              SetWindowPos(win32wnd->getWindowHandle(), 0, track.rclTrack.xLeft,
                           parentHeight - track.rclTrack.yTop,
                           track.rclTrack.xRight - track.rclTrack.xLeft,
                           track.rclTrack.yTop - track.rclTrack.yBottom,
                           SWP_NOACTIVATE_W | SWP_NOZORDER_W | SWP_NOACTIVATE_W);
//            WinSetWindowPos(win32wnd->getOS2WindowHandle(),
//                                        0, track.rclTrack.xLeft, track.rclTrack.yBottom,
//                                        track.rclTrack.xRight - track.rclTrack.xLeft,
//                                        track.rclTrack.yTop - track.rclTrack.yBottom,
//                                        SWP_SIZE|SWP_MOVE);
        }
        }
        return;
   }
   if(hpsTrack) WinReleasePS(hpsTrack);
   return;
}
//******************************************************************************
//******************************************************************************
