/* $Id: pmwindow.cpp,v 1.32 2000-01-01 17:07:42 cbratschi Exp $ */
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

#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include "win32type.h"
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
  if(!WinRegisterClass(                 /* Register window class        */
     hab,                               /* Anchor block handle          */
     (PSZ)WIN32_STDCLASS2,              /* Window class name            */
     (PFNWP)Win32WindowProc,            /* Address of window procedure  */
     CS_HITTEST | CS_SAVEBITS,
     NROF_WIN32WNDBYTES)) {
        dprintf(("WinRegisterClass Win32BaseWindow failed"));
        return(FALSE);
   }
/* //CB: perhaps reuse it for own frame
   if (!WinQueryClassInfo (hab, WC_FRAME, &FrameClassInfo)) {
     dprintf (("WinQueryClassInfo WC_FRAME failed"));
     return (FALSE);
   }
   FrameClassInfo.flClassStyle &= ~(CS_PUBLIC | CS_CLIPSIBLINGS);
   if (!WinRegisterClass (hab,
                          WIN32_INNERFRAME,
                          FrameClassInfo.pfnWindowProc,
                          FrameClassInfo.flClassStyle,
                          FrameClassInfo.cbWindowData)) {
     dprintf (("WinRegisterClass Win32InnerFrame failed"));
     return (FALSE);
   }
*/
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
   return OSLibInitMsgQueue();
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

        if(win32wnd->MsgCreate(WinQueryWindow(hwnd, QW_PARENT), hwnd) == FALSE)
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
        break;

    case WM_ENABLE:
        dprintf(("OS2: WM_ENABLE %x", hwnd));
        win32wnd->MsgEnable(SHORT1FROMMP(mp1));
        break;

    case WM_SHOW:
        dprintf(("OS2: WM_SHOW %x %d", hwnd, mp1));
        win32wnd->MsgShow((ULONG)mp1);
        break;

#if 1
    case WM_ADJUSTWINDOWPOS:
    {
//      PSWP     pswp = (PSWP)mp1;

//        dprintf(("OS2: WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", hwnd, pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        goto RunDefWndProc;
    }
#else
    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld, swpNew;
      WINDOWPOS wp;
      ULONG     parentHeight = 0;
      HWND      hParent = NULLHANDLE, hFrame = NULLHANDLE, hwndAfter;

        dprintf(("OS2: WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", hwnd, pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0) goto RunDefWndProc;;

        //SvL: TODO: Workaround. Why is this happening?
        //     When this flag is set the coordinates are 0, even though SWP_SIZE & SWP_MOVE are set.
//        if ((pswp->fl & SWP_NOADJUST)) goto RunDefWndProc;

        if(!win32wnd->CanReceiveSizeMsgs()) goto RunDefWndProc;;

        WinQueryWindowPos(hwnd, &swpOld);

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto RunDefWndProc;;
            }
        }
        hwndAfter = pswp->hwndInsertBehind;
        hFrame = win32wnd->getOS2FrameWindowHandle();
        OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, hParent, hFrame);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           if(wndAfter) wp.hwndInsertAfter = wndAfter->getWindowHandle();
        }
        if(win32wnd->MsgPosChanging((LPARAM)&wp) == 0)
        {//app or default window handler changed wp
            dprintf(("OS2: WM_ADJUSTWINDOWPOS, app changed windowpos struct"));
            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
            OSLibMapWINDOWPOStoSWP(&wp, &swpNew, &swpOld, hParent, hFrame);
            dprintf(("%x (%d,%d), (%d,%d)", swpNew.fl, swpNew.x, swpNew.y, swpNew.cx, swpNew.cy));
            swpNew.fl |= SWP_NOADJUST;
            swpNew.hwndInsertBehind = hwndAfter;
            swpNew.hwnd = hFrame;

            WinSetMultWindowPos(GetThreadHAB(), &swpNew, 1);
            return (MRESULT)0;
        }
        break;
    }
#endif

    case WM_WINDOWPOSCHANGED:
    {
        win32wnd->MsgPosChanged((LPARAM)&thdb->wp);
        goto RunDefWndProc;
    }

    case WM_ACTIVATE:
    {
      HWND hwndActivate = (HWND)mp2;
      BOOL fMinimized = FALSE;

        dprintf(("OS2: WM_ACTIVATE %x %x", hwnd, hwndActivate));
        if(WinQueryWindowULong(hwndActivate, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC) {
                //another (non-win32) application's window
                //set to NULL (allowed according to win32 SDK) to avoid problems
                hwndActivate = NULL;
        }
        if(WinQueryWindowULong(hwnd, QWL_STYLE) & WS_MINIMIZED)
        {
           fMinimized = TRUE;
        }

        win32wnd->MsgActivate(SHORT1FROMMP(mp1), fMinimized, Win32BaseWindow::OS2ToWin32Handle(hwndActivate));
        break;
    }

    case WM_SIZE:
    {
        dprintf(("OS2: WM_SIZE (%d,%d) (%d,%d)", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2), SHORT1FROMMP(mp1), SHORT2FROMMP(mp2)));
        break;
    }

    case WM_MINMAXFRAME:
    {
        dprintf(("OS2: WM_MINMAXFRAME"));
        break;
    }

    case WM_OWNERPOSCHANGE:
    {
        dprintf(("OS2: WM_OWNERPOSCHANGE"));
        goto RunDefWndProc;
    }

    case WM_CALCVALIDRECTS:
    {
        dprintf(("OS2: WM_CALCVALIDRECTS"));
        goto RunDefWndProc;
    }

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
    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON2DOWN:
    case WM_BUTTON2UP:
    case WM_BUTTON2DBLCLK:
    case WM_BUTTON3DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DBLCLK:
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
        goto RunDefWndProc;

    case WM_MOUSEMOVE:
    {
        //OS/2 Window coordinates -> Win32 Window coordinates
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
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_INITMENU:
        win32wnd->MsgInitMenu(pWinMsg);
        break;

    case WM_TIMER:
        win32wnd->DispatchMsgA(pWinMsg);
        goto RunDefWndProc;

    case WM_MENUSELECT:
    case WM_MENUEND:
    case WM_NEXTMENU:
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
            if(wndpars->fsStatus & WPM_CCHTEXT)
                wndpars->cchText = win32wnd->MsgGetTextLength();
            if(wndpars->fsStatus & WPM_TEXT)
                wndpars->pszText = win32wnd->MsgGetText();

            wndpars->fsStatus = 0;
            wndpars->cbCtlData = 0;
            wndpars->cbPresParams = 0;
            RestoreOS2TIB();
            return (MRESULT)TRUE;
        }
        goto RunDefWndProc;
    }

    case WM_PAINT:
        win32wnd->DispatchMsgA(pWinMsg);
        goto RunDefWndProc;

    case WM_HITTEST:
    {
      DWORD res;

      // Only send this message if the window is enabled
      if (!WinIsWindowEnabled(hwnd))
        res = HT_ERROR;
      else if (win32wnd->getIgnoreHitTest())
        res = HT_NORMAL;
      else
      {
        dprintf(("USER32: WM_HITTEST %x (%d,%d)",hwnd,(*(POINTS *)&mp1).x,(*(POINTS *)&mp1).y));

        //CB: WinWindowFromPoint: PM sends WM_HITTEST -> loop -> stack overflow
        win32wnd->setIgnoreHitTest(TRUE);
        res = win32wnd->MsgHitTest(pWinMsg);
        win32wnd->setIgnoreHitTest(FALSE);
      }
      RestoreOS2TIB();
      return (MRESULT)res;
    }

    case WM_CONTEXTMENU:
    {
        win32wnd->DispatchMsgA(pWinMsg);

        RestoreOS2TIB();
        return (MRESULT)TRUE;
    }

    case WM_ERASEBACKGROUND:
    {
        dprintf(("OS2: WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        break;
    }

    case WM_FOCUSCHANGE:
        dprintf(("OS2: WM_FOCUSCHANGE %x", win32wnd->getWindowHandle()));
        goto RunDefWndProc;

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
        //dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
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
//******************************************************************************
MRESULT EXPENTRY Win32SubclassWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  Win32BaseWindow* win32wnd;

  //Restore our FS selector
  SetWin32TIB();

  win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

  if (!win32wnd)
  {
    dprintf(("Invalid win32wnd pointer for subclassed window %x!!", hwnd));
    goto RunDefWndProc;
  }

  switch (msg)
  {
    case WM_WINDOWPOSCHANGED:
      {
        PSWP      pswp  = (PSWP)mp1;
        SWP       swpOld = *(pswp + 1);
        WINDOWPOS wp;
        HWND      hParent = NULLHANDLE, hFrame = NULLHANDLE;

        dprintf(("OS2Subclass: WM_WINDOWPOSCHANGED %x %x (%d,%d) (%d,%d)", hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0) break;

        hParent = hFrame = WinQueryWindow(hwnd, QW_PARENT);

        OSLibMapSWPtoWINDOWPOS(pswp,&wp, &swpOld,hParent,hFrame);

        win32wnd->setWindowRect(swpOld.x, swpOld.y, swpOld.x + swpOld.cx, swpOld.y + swpOld.cy);
        win32wnd->setClientRect(swpOld.x, swpOld.y, swpOld.x + swpOld.cx, swpOld.y + swpOld.cy);
        wp.x = swpOld.x;
        wp.y = swpOld.y;
        wp.cx = swpOld.cx;
        wp.cy = swpOld.cy;

        wp.hwnd = win32wnd->getWindowHandle();

        win32wnd->MsgPosChanged((LPARAM)&wp);

        goto RunOldWndProc;
      }

    default:
      goto RunDefHandler;
  }

RunDefWndProc:
  RestoreOS2TIB();
  return WinDefWindowProc(hwnd,msg,mp1,mp2);

RunOldWndProc:
  RestoreOS2TIB();
  return ((PFNWP)win32wnd->getOldWndProc())(hwnd,msg,mp1,mp2);

RunDefHandler:
  RestoreOS2TIB();
  return Win32WindowProc(hwnd,msg,mp1,mp2);
}

PVOID SubclassWithDefHandler(HWND hwnd)
{
  return WinSubclassWindow(hwnd,Win32SubclassWindowProc);
}
