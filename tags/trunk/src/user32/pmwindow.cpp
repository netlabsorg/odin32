/* $Id: pmwindow.cpp,v 1.96 2000-06-29 12:26:00 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <winconst.h>
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


MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

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
     NROF_WIN32WNDBYTES)) {
        dprintf(("WinRegisterClass Win32BaseWindow failed"));
        return(FALSE);
  }

  CLASSINFO FrameClassInfo;
  if(!WinQueryClassInfo (hab, WC_FRAME, &FrameClassInfo)) {
     dprintf (("WinQueryClassInfo WC_FRAME failed"));
     return (FALSE);
  }
  dprintf(("WC_FRAME style %x", FrameClassInfo.flClassStyle));

  if(!WinRegisterClass(                 /* Register window class        */
     hab,                               /* Anchor block handle          */
     (PSZ)WIN32_STDFRAMECLASS,          /* Window class name            */
     (PFNWP)Win32WindowProc,            /* Address of window procedure  */
     CS_HITTEST | CS_FRAME,
     FrameClassInfo.cbWindowData+NROF_WIN32WNDBYTES)) {
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
 POSTMSG_PACKET  *postmsg;
 OSLIBPOINT       point, ClientPoint;
 Win32BaseWindow *win32wnd;
 THDB            *thdb;
 APIRET           rc = 0;
 MSG              winMsg, *pWinMsg;

  //Restore our FS selector
  SetWin32TIB();

  thdb = GetThreadTHDB();
  win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

  if(!thdb || (msg != WM_CREATE && win32wnd == NULL)) {
        dprintf(("Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
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
  switch( msg )
  {
    //OS/2 msgs
    case WM_CREATE:
    {
        if(thdb->newWindow == 0)
            goto createfail;

        //Processing is done in after WinCreateWindow returns
        dprintf(("OS2: WM_CREATE %x", hwnd));
        win32wnd = (Win32BaseWindow *)thdb->newWindow;
        thdb->newWindow = 0;
        if(win32wnd->MsgCreate(hwnd) == FALSE)
        {
            RestoreOS2TIB();
            return (MRESULT)TRUE; //discontinue window creation
        }
    createfail:
        RestoreOS2TIB();
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
 
        if(pswp->fl & SWP_NOADJUST) {
		//ignore weird messages (TODO: why are they sent?)
		break;
	}
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
        else  OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), 0, 0, hwnd);

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

        dprintf(("OS2: WM_WINDOWPOSCHANGED (%x) %x %x (%d,%d) (%d,%d)", mp2, win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
        {
        	if(pswp->fl & SWP_ACTIVATE)
        	{
             		WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
        	}
                goto RunDefWndProc;
        }

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if(win32wnd->isChild()) {
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

        if(pswp->fl & SWP_ACTIVATE)
        {
             WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
        }

        if((pswp->fl & (SWP_MOVE | SWP_SIZE)) && !(win32wnd->getStyle() & WS_MINIMIZE_W))
        {
          //Note: Also updates the new window rectangle
          win32wnd->MsgFormatFrame(&wp);

          //CB: todo: use result for WM_CALCVALIDRECTS
          mapWin32ToOS2RectFrame(win32wnd, win32wnd->getClientRectPtr(), (PRECTLOS2)&rect);

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

PosChangedEnd:
        RestoreOS2TIB();
        return (MRESULT)FALSE;
    }

    case WM_ACTIVATE:
    {
        USHORT flags = WinQueryWindowUShort(hwnd,QWS_FLAGS);

        dprintf(("OS2: WM_ACTIVATE %x %x %x", hwnd, mp1, mp2));

        WinSetWindowUShort(hwnd,QWS_FLAGS, SHORT1FROMMP(mp1) ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
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
        RestoreOS2TIB();
        return 0;
    }

    case WM_SIZE:
    {
        dprintf(("OS2: WM_SIZE (%d,%d) (%d,%d)", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2), SHORT1FROMMP(mp1), SHORT2FROMMP(mp2)));
        goto RunDefWndProc;
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
      else if ((swp->fl & SWP_MINIMIZE) == SWP_MINIMIZE)
      {
        win32wnd->setStyle((win32wnd->getStyle() & ~WS_MAXIMIZE_W) | WS_MINIMIZE_W);
      }
      else if ((swp->fl & SWP_RESTORE) == SWP_RESTORE)
      {
        win32wnd->setStyle(win32wnd->getStyle() & ~(WS_MINIMIZE_W | WS_MAXIMIZE_W));
      }
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
        else if ((dwStyle & CS_VREDRAW_W) && (newRect->yTop-newRect->yBottom != oldRect->yTop-oldRect->yBottom))
          res |= CVR_REDRAW;
      } else res |= CVR_REDRAW;

      RestoreOS2TIB();
      return (MRESULT)res;
    }
#else
      dprintf(("PMWINDOW: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));
      RestoreOS2TIB();
      return (MRESULT)(CVR_ALIGNLEFT | CVR_ALIGNTOP);
#endif

    case WM_VRNENABLED:
	dprintf(("OS2: WM_VRNENABLED %x", win32wnd->getWindowHandle()));
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
        goto RunDefWndProc;

    case WM_SETFOCUS:
    {
      HWND hwndFocus = (HWND)mp1;

        dprintf(("OS2: WM_SETFOCUS %x %x %d", win32wnd->getWindowHandle(), mp1, mp2));
        if(WinQueryWindowULong(hwndFocus, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC) {
                //another (non-win32) application's window
                //set to NULL (allowed according to win32 SDK) to avoid problems
                hwndFocus = NULL;
        }
        if((ULONG)mp2 == TRUE) {
                HWND hwndFocusWin32 = Win32BaseWindow::OS2ToWin32Handle(hwndFocus);
                recreateCaret (hwndFocusWin32);
                win32wnd->MsgSetFocus(hwndFocusWin32);
        }
        else win32wnd->MsgKillFocus(Win32BaseWindow::OS2ToWin32Handle(hwndFocus));
        break;
    }

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
	if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
		win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
	}
        if(win32wnd)
	        win32wnd->MsgButton(pWinMsg);
        rc = TRUE;
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
        rc = TRUE;
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
            RestoreOS2TIB();
            return (MRESULT)TRUE;
        }
        goto RunDefWndProc;
    }

    case WM_PAINT:
    {
      RECTL rectl;
      BOOL  rc;

   	rc = WinQueryUpdateRect(hwnd, &rectl);
        dprintf(("OS2: WM_PAINT (%d,%d) (%d,%d)", rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop));
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
	else	goto RunDefWndProc;

	//SvL: Not calling the default window procedure causes all sorts of
        //     strange problems (redraw & hanging app)
        //     -> check what WinBeginPaint does what we're forgetting in BeginPaint
//   	WinQueryUpdateRect(hwnd, &rectl);
//        if(rectl.xLeft == 0 && rectl.yTop == 0 && rectl.xRight == 0 && rectl.yBottom == 0) {
//        	RestoreOS2TIB();
//        	return (MRESULT)FALSE;
//	}
//	dprintf(("Update rectangle (%d,%d)(%d,%d) not empty, msg %x", rectl.xLeft, rectl.yTop, rectl.xRight, rectl.yBottom, pWinMsg->message));
//	goto RunDefWndProc;
        break;
    }

    case WM_ERASEBACKGROUND:
    {
        dprintf(("OS2: WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        RestoreOS2TIB();
        return (MRESULT)FALSE;
    }

#if 0
    case WM_CONTEXTMENU:
    {
        win32wnd->DispatchMsgA(pWinMsg);

        RestoreOS2TIB();
        return (MRESULT)TRUE;
    }
#endif

    case WM_FOCUSCHANGE:
    {
     HWND   hwndFocus = (HWND)mp1;
     HWND   hwndLoseFocus, hwndGainFocus;
     USHORT usSetFocus = SHORT1FROMMP(mp2);
     USHORT fsFocusChange = SHORT2FROMMP(mp2);

        dprintf(("OS2: WM_FOCUSCHANGE (start) %x %x %x %x", win32wnd->getWindowHandle(), hwndFocus, usSetFocus, fsFocusChange));
        RestoreOS2TIB();
	if(usSetFocus) {
		hwndGainFocus = hwnd;
		hwndLoseFocus = hwndFocus;
	}
	else {
		hwndGainFocus = hwndFocus;
		hwndLoseFocus = hwnd;
	}
	if(!(fsFocusChange & FC_NOLOSEFOCUS)) {
       		WinSendMsg(hwndLoseFocus, WM_SETFOCUS, (MPARAM)hwndGainFocus, (MPARAM)0);
	}
	//TODO: Don't send WM_SETSELECTION to child window if frame already has selection
	if(!(fsFocusChange & FC_NOLOSESELECTION)) {
       		WinSendMsg(hwndLoseFocus, WM_SETSELECTION, (MPARAM)0, (MPARAM)0);
	}
	if(usSetFocus) 
	{
  	  Win32BaseWindow *winfocus = Win32BaseWindow::GetWindowFromOS2Handle(hwndLoseFocus);
	  if(!(fsFocusChange & FC_NOLOSEACTIVE)) {
		if(!winfocus || (winfocus->GetTopParent() != win32wnd->GetTopParent())) {
			if(winfocus) {
				WinSendMsg(winfocus->GetTopParent()->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)0, (MPARAM)hwndGainFocus);
			}
			else	WinSendMsg(hwndLoseFocus, WM_ACTIVATE, (MPARAM)0, (MPARAM)hwndGainFocus);
		}
	  }
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
			WinSendMsg(topparent->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)1, (MPARAM)hwndLoseFocus);
		}
	  }
	}
	else 
	{
 	  Win32BaseWindow *winfocus = Win32BaseWindow::GetWindowFromOS2Handle(hwndGainFocus);
	  if(!(fsFocusChange & FC_NOLOSEACTIVE)) {
		if(!winfocus || (winfocus->GetTopParent() != win32wnd->GetTopParent())) {
			WinSendMsg(win32wnd->GetTopParent()->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)0, (MPARAM)hwndGainFocus);
		}
	  }
  	  if(!(fsFocusChange & FC_NOSETACTIVE)) {
		if(!winfocus || (winfocus->GetTopParent() != win32wnd->GetTopParent())) {
			if(winfocus) {
				WinSendMsg(winfocus->GetTopParent()->getOS2WindowHandle(), WM_ACTIVATE, (MPARAM)1, (MPARAM)hwndLoseFocus);
			}
	       		else	WinSendMsg(hwndGainFocus, WM_ACTIVATE, (MPARAM)1, (MPARAM)hwndLoseFocus);
		}
	  }
	}
	//TODO: Don't send WM_SETSELECTION to child window if frame already has selection
	if(!(fsFocusChange & FC_NOSETSELECTION)) {
       		WinSendMsg(hwndGainFocus, WM_SETSELECTION, (MPARAM)1, (MPARAM)0);
	}

	if(!(fsFocusChange & FC_NOSETFOCUS)) {
       		WinSendMsg(hwndGainFocus, WM_SETFOCUS, (MPARAM)hwndLoseFocus, (MPARAM)1);
	}
#ifdef DEBUG
      	SetWin32TIB();
        dprintf(("OS2: WM_FOCUSCHANGE (end) %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
      	RestoreOS2TIB();
#endif
        return (MRESULT)rc;
    }

    case WM_QUERYTRACKINFO:
    {
      PTRACKINFO trackInfo = (PTRACKINFO)mp2;

        dprintf(("OS2: WM_QUERYTRACKINFO %x", win32wnd->getWindowHandle()));
      	trackInfo->cxBorder = 4;
      	trackInfo->cyBorder = 4;
      	win32wnd->AdjustTrackInfo((PPOINT)&trackInfo->ptlMinTrackSize,(PPOINT)&trackInfo->ptlMaxTrackSize);
      	RestoreOS2TIB();
      	return (MRESULT)TRUE;
    }

    case WM_QUERYBORDERSIZE:
    {
      PWPOINT size = (PWPOINT)mp1;

      dprintf(("OS2: WM_QUERYBORDERSIZE %x", win32wnd->getWindowHandle()));

      size->x = 0;
      size->y = 0;
      RestoreOS2TIB();
      return (MRESULT)TRUE;
    }

    case WM_OWNERPOSCHANGE:
    {
        dprintf(("OS2: WM_OWNERPOSCHANGE"));
        goto RunDefWndProc;
    }

    case WM_FORMATFRAME:
        dprintf(("OS2: WM_FORMATFRAME %x", win32wnd->getWindowHandle()));
//        goto RunDefWndProc;
	break;

    case WM_ADJUSTFRAMEPOS:
    {
      PSWP pswp   = (PSWP)mp1;

        dprintf(("OS2: WM_ADJUSTFRAMEPOS %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        break;
    }

    case WM_QUERYFRAMEINFO:
        dprintf(("OS2: WM_QUERYFRAMEINFO %x", win32wnd->getWindowHandle()));
	return (MRESULT)(FI_FRAME|FI_ACTIVATEOK);

    case WM_QUERYFOCUSCHAIN:
    {
      USHORT fsCmd = SHORT1FROMMP(mp1);

        dprintf(("OS2: WM_QUERYFOCUSCHAIN %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
#if 1
	goto RunDefWndProc;
#else
	switch(fsCmd) {
        case QFC_NEXTINCHAIN:
    		break;
	case QFC_ACTIVE:
	case QFC_FRAME:
		if(win32wnd->GetTopParent()) {
			dprintf(("Frame = %x", win32wnd->GetTopParent()->getOS2WindowHandle()));
			return (MRESULT)win32wnd->GetTopParent()->getOS2WindowHandle();
		}
		break;
	case QFC_SELECTACTIVE:
		break;
	case QFC_PARTOFCHAIN:
		return (MRESULT)TRUE;
	}
        break;
#endif
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
//        dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
        RestoreOS2TIB();
        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  RestoreOS2TIB();
  return (MRESULT)rc;

RunDefWndProc:
//  dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
  RestoreOS2TIB();
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
} /* End of Win32WindowProc */
//******************************************************************************
//TODO: Quickly moving a window two times doesn't force a repaint (1st time)
//******************************************************************************
VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags)
{
  TRACKINFO track;
  RECTL     rcl;
  PRECT     pWindowRect;
  HWND      hwndTracking;
  HPS       hpsTrack;
  LONG      parentHeight, parentWidth;

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

   rcl.xLeft   = pWindowRect->left;
   rcl.yTop    = OSLibQueryScreenHeight() - pWindowRect->top;
   rcl.xRight  = pWindowRect->right;
   rcl.yBottom = OSLibQueryScreenHeight() - pWindowRect->bottom;
   if(hwndTracking != HWND_DESKTOP) {
   	WinMapWindowPoints(win32wnd->getOS2WindowHandle(), HWND_DESKTOP, (PPOINTL)&rcl, 2);
   }
   WinCopyRect(hab, &track.rclTrack, &rcl);   /* starting point */
 
   WinSetRect(hab, &track.rclBoundary, 0, 0, parentWidth, parentHeight); /* bounding rectangle */
 
   track.ptlMinTrackSize.x = 10;
   track.ptlMinTrackSize.y = 10;  /* set smallest allowed size of rectangle */
   track.ptlMaxTrackSize.x = parentWidth;
   track.ptlMaxTrackSize.y = parentHeight; /* set largest allowed size of rectangle */

   win32wnd->AdjustTrackInfo((PPOINT)&track.ptlMinTrackSize, (PPOINT)&track.ptlMaxTrackSize);
 
   track.fs = flags;
 
   if(WinTrackRect(HWND_DESKTOP, NULL, &track) )
   {
   	if(hpsTrack)	WinReleasePS(hpsTrack);

     	/* if successful copy final position back */
	if(!WinEqualRect(0, &rcl, &track.rclTrack)) {
		if(flags == TF_MOVE) {
			WinSetWindowPos(win32wnd->getOS2WindowHandle(), 
                                        0, track.rclTrack.xLeft, track.rclTrack.yBottom, 
                                        0, 0, SWP_MOVE);
		}
		else {
			WinSetWindowPos(win32wnd->getOS2WindowHandle(), 
                                        0, track.rclTrack.xLeft, track.rclTrack.yBottom, 
                                        track.rclTrack.xRight - track.rclTrack.xLeft, 
                                        track.rclTrack.yTop - track.rclTrack.yBottom, 
                                        SWP_SIZE|SWP_MOVE);
		}
	}
	return;
   }
   if(hpsTrack)	WinReleasePS(hpsTrack);
   return;
}
//******************************************************************************
//******************************************************************************
