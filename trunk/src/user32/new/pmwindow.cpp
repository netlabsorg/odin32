/* $Id: pmwindow.cpp,v 1.5 1999-07-17 09:17:58 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <wprocess.h>
#include <misc.h>
#include <win32wnd.h>
#include <win32dlg.h>
#include "pmwindow.h"
#include "oslibwin.h"
#include "oslibutil.h"

HMQ  hmq = 0;                             /* Message queue handle         */
HAB  hab = 0;

MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

//******************************************************************************
//Initialize PM; create hab, message queue and register special Win32 window classes
//******************************************************************************
BOOL InitPM()
{
  hab = WinInitialize(0);
  hmq = WinCreateMsgQueue(hab, 0);

  if(!hab || !hmq) {
        UINT error;

        //CB: only fail on real error
        error = WinGetLastError(hab) & 0xFFFF; //error code
        if (!hab || error != PMERR_MSG_QUEUE_ALREADY_EXISTS)
        {
          dprintf(("WinInitialize or WinCreateMsgQueue failed %x %x", hab, hmq));
          dprintf((" Error = %x",error));
          return(FALSE);
        } else
        {
          //CB: get queue handle (todo)
        }
  }
  SetThreadHAB(hab);
  SetThreadMessageQueue(hmq);

  if(!WinRegisterClass(                 /* Register window class        */
     hab,                               /* Anchor block handle          */
     (PSZ)WIN32_STDCLASS,               /* Window class name            */
     (PFNWP)Win32WindowProc,            /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     8)) {
        dprintf(("WinRegisterClass Win32Window failed"));
        return(FALSE);
   }

   return(TRUE);
} /* End of main */
//******************************************************************************
//Win32 window message handler
//******************************************************************************
MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 POSTMSG_PACKET *postmsg;
 Win32Window  *win32wnd;
 ULONG         magic;
 APIRET        rc;

  //Restore our FS selector
  SetWin32TIB();

  win32wnd = (Win32Window *)WinQueryWindowULong(hwnd, OFFSET_WIN32WNDPTR);
  magic    = WinQueryWindowULong(hwnd, OFFSET_WIN32PM_MAGIC);

  if(msg != WM_CREATE && win32wnd == NULL && magic != WIN32PM_MAGIC) {
        dprintf(("Invalid win32wnd pointer for window %x!!", hwnd));
        goto RunDefWndProc;
  }
  switch( msg )
  {
    //internal messages
    case WM_WIN32_POSTMESSAGEA:
	postmsg = (POSTMSG_PACKET *)mp1;
	if(postmsg == NULL) {
		dprintf(("WM_WIN32_POSTMESSAGEA, postmsg NULL!!"));
		break;
	}
	win32wnd->SendMessageA(postmsg->Msg, postmsg->wParam, postmsg->lParam);
	free(postmsg);
	break;

    case WM_WIN32_POSTMESSAGEW:
	postmsg = (POSTMSG_PACKET *)mp1;
	if(postmsg == NULL) {
		dprintf(("WM_WIN32_POSTMESSAGEW, postmsg NULL!!"));
		break;
	}
	win32wnd->SendMessageW(postmsg->Msg, postmsg->wParam, postmsg->lParam);
	free(postmsg);
	break;

    //OS/2 msgs
    case WM_CREATE:
        //Processing is done in after WinCreateWindow returns
        break;

    case WM_QUIT:
        if(win32wnd->MsgQuit()) {
                goto RunDefWndProc;
        }
        break;

    case WM_CLOSE:
        if(win32wnd->MsgClose()) {
                goto RunDefWndProc;
        }
        break;

    case WM_DESTROY:
        if(win32wnd->MsgDestroy()) {
                goto RunDefWndProc;
        }
        break;

    case WM_ENABLE:
        if(win32wnd->MsgEnable((ULONG)mp1)) {
                goto RunDefWndProc;
        }
        break;

    case WM_SHOW:
        if(win32wnd->MsgShow((ULONG)mp1)) {
                goto RunDefWndProc;
        }
        break;

    case WM_MOVE:
    {
     RECTL  rectl, rectl2;
     POINTL point;
     HWND   hwndParent;
     ULONG  xScreen, yScreen, yParent, xParent;

        rc = WinQueryWindowRect(hwnd, &rectl);
        if(rc == TRUE) {
                point.x = rectl.xLeft;
                point.y = rectl.yBottom;

                //If the window has a parent, calculate position relative to that window
                if((hwndParent = WinQueryWindow(hwnd, QW_PARENT)) != WinQueryDesktopWindow(hab, NULLHANDLE)) {
                        rc = WinMapWindowPoints(hwnd, hwndParent, &point, 1);
                        if(rc == FALSE) {
                                dprintf(("WM_MOVE: WinMapWindowPoints (parent) failed!"));
                                break;
                        }
                        rc = WinQueryWindowRect(hwndParent, &rectl2);
                        if(rc == FALSE) {
                                dprintf(("WM_MOVE: WinQueryWindowRect(HWND_DESKTOP, &rectl) failed!"));
                                break;
                        }
                        yParent = point.x;
                        yParent = OS2TOWIN32POINT(rectl2.yTop - rectl2.yBottom, point.y);
                }
                else    xParent = yParent = -1;

                point.x = rectl.xLeft;
                point.y = rectl.yBottom;

                rc = WinMapWindowPoints(hwnd, HWND_DESKTOP, &point, 1);
                if(rc == FALSE) {
                        dprintf(("WM_MOVE: WinMapWindowPoints (desktop) failed!"));
                        break;
                }
                rc = WinQueryWindowRect(HWND_DESKTOP, &rectl);
                if(rc == FALSE) {
                        dprintf(("WM_MOVE: WinQueryWindowRect(HWND_DESKTOP, &rectl) failed!"));
                        break;
                }
                xScreen = point.x;
                yScreen = OS2TOWIN32POINT(rectl.yTop - rectl.yBottom, point.y);

                if(win32wnd->MsgMove(xScreen, yScreen, xParent, yParent)) {
                        goto RunDefWndProc;
                }
        }
        else {
                dprintf(("WM_MOVE: WinQueryWindowRect failed!"));
        }
        break;
    }

    case WM_WINDOWPOSCHANGED:
    {
        break;
    }

    case WM_ADJUSTWINDOWPOS:
    {
//      if(win32wnd->MsgWindowPosChanging(0, 0)) {
        break;
    }

    case WM_ERASEBACKGROUND:
    {
     HPS hps;

        hps = WinGetPS(hwnd);
        if(win32wnd->MsgEraseBackGround((ULONG)hps))
        {
                /*
                 * Return TRUE to request PM to paint the window background
                 * in SYSCLR_WINDOW.
                 */
                WinReleasePS(hps);
                return (MRESULT)( TRUE );
        }
        WinReleasePS(hps);
        return (MRESULT) FALSE;
    }
    case WM_SIZE:
    {
     SWP swp;

        rc = WinQueryWindowPos(hwnd, &swp);
        if(rc == FALSE) {
                dprintf(("WM_SIZE: WinQueryWindowPos failed!"));
                break;
        }
        if(win32wnd->MsgSize(SHORT1FROMMP(mp2), SHORT2FROMMP(mp2),
                                (swp.fl & SWP_MINIMIZE) != 0,
                                (swp.fl & SWP_MAXIMIZE) != 0))
        {
                goto RunDefWndProc;
        }

        break;
    }

    case WM_ACTIVATE:
    {
      HWND hwndActivate = (HWND)mp1;

        if(WinQueryWindowULong(hwndActivate, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC) {
                //another (non-win32) application's window
                //set to NULL (allowed according to win32 SDK) to avoid problems
                hwndActivate = NULL;
        }
        if(win32wnd->MsgActivate(1, hwndActivate)) {
                goto RunDefWndProc;
        }
        break;
    }
    case WM_FOCUSCHANGE:
        break;

    case WM_SETFOCUS:
    {
      HWND hwndFocus = (HWND)mp1;

        if(WinQueryWindowULong(hwndFocus, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC) {
                //another (non-win32) application's window
                //set to NULL (allowed according to win32 SDK) to avoid problems
                hwndFocus = NULL;
        }
        if((ULONG)mp2 == TRUE) {
                rc = win32wnd->MsgSetFocus(hwndFocus);
        }
        else    rc = win32wnd->MsgKillFocus(hwndFocus);
        if(rc) {
                goto RunDefWndProc;
        }
        break;
    }
    //**************************************************************************
    //Mouse messages
    //**************************************************************************
    case WM_BUTTON1DOWN:
        if(win32wnd->MsgButton(BUTTON_LEFTDOWN, (*(POINTS *)&mp1).x, (*(POINTS *)&mp1).y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON1UP:
        if(win32wnd->MsgButton(BUTTON_LEFTUP, (*(POINTS *)&mp1).x, (*(POINTS *)&mp1).y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON1DBLCLK:
        if(win32wnd->MsgButton(BUTTON_LEFTDBLCLICK, (*(POINTS *)&mp1).x, (*(POINTS *)&mp1).y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2DOWN:
        if(win32wnd->MsgButton(BUTTON_RIGHTDOWN, (*(POINTS *)&mp1).x, (*(POINTS *)&mp1).y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2UP:
        if(win32wnd->MsgButton(BUTTON_RIGHTUP, (*(POINTS *)&mp1).x, (*(POINTS *)&mp1).y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2DBLCLK:
        if(win32wnd->MsgButton(BUTTON_RIGHTDBLCLICK, (*(POINTS *)&mp1).x, (*(POINTS *)&mp1).y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:
    case WM_BUTTON2CLICK:
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:
    case WM_BUTTON1CLICK:
    case WM_BUTTON3DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DBLCLK:
    case WM_BUTTON3MOTIONSTART:
    case WM_BUTTON3MOTIONEND:
    case WM_BUTTON3CLICK:
        break;

    case WM_MOUSEMOVE:
        break;

    //**************************************************************************
    //Slider messages
    //**************************************************************************
    case WM_VSCROLL:
        break;
    case WM_HSCROLL:
        break;

    case WM_CONTROL:
        break;

    case WM_COMMAND:
    case WM_SYSCOMMAND:
        break;

    case WM_CHAR:
        break;

    case WM_INITMENU:
    case WM_MENUSELECT:
    case WM_MENUEND:
    case WM_NEXTMENU:
        break;

    case WM_TIMER:
        break;

    case WM_PAINT:
        if(win32wnd->MsgPaint(0, 0)) {
                goto RunDefWndProc;
        }
        break;

    case WM_SYSCOLORCHANGE:
    case WM_SYSVALUECHANGED:
        break;

    case WM_CALCVALIDRECTS:
    case WM_SETWINDOWPARAMS:
    case WM_QUERYWINDOWPARAMS:
    case WM_HITTEST:
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
        break;
    default:
        RestoreOS2TIB();
        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  RestoreOS2TIB();
  return (MRESULT)FALSE;

RunDefWndProc:
  RestoreOS2TIB();
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
} /* End of Win32WindowProc */
//******************************************************************************
//******************************************************************************
