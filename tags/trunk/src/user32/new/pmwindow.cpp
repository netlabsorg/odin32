/* $Id: pmwindow.cpp,v 1.13 1999-07-20 07:42:36 sandervl Exp $ */
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
#include "oslibgdi.h"
#include "oslibmsg.h"

HMQ  hmq = 0;                             /* Message queue handle         */
HAB  hab = 0;

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
        if (!hab || error != PMERR_MSG_QUEUE_ALREADY_EXISTS)
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

  if(!WinRegisterClass(                 /* Register window class        */
     hab,                               /* Anchor block handle          */
     (PSZ)WIN32_STDCLASS,               /* Window class name            */
     (PFNWP)Win32WindowProc,            /* Address of window procedure  */
     CS_SIZEREDRAW | CS_HITTEST,
     8)) {
        dprintf(("WinRegisterClass Win32Window failed"));
        return(FALSE);
   }

   return OSLibInitMsgQueue();
} /* End of main */
//******************************************************************************
//Win32 window message handler
//******************************************************************************
MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 POSTMSG_PACKET *postmsg;
 OSLIBPOINT      point;
 Win32Window    *win32wnd;
 APIRET          rc;

  //Restore our FS selector
  SetWin32TIB();

  win32wnd = Win32Window::GetWindowFromOS2Handle(hwnd);

  if(msg != WM_CREATE && win32wnd == NULL) {
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
        dprintf(("OS2: WM_CREATE %x", hwnd));
        RestoreOS2TIB();
        return (MRESULT)FALSE;

    case WM_QUIT:
        dprintf(("OS2: WM_QUIT %x", hwnd));
        if(win32wnd->MsgQuit()) {
                goto RunDefWndProc;
        }
        break;

    case WM_CLOSE:
        dprintf(("OS2: WM_CLOSE %x", hwnd));
        if(win32wnd->MsgClose()) {
                goto RunDefWndProc;
        }
        break;

    case WM_DESTROY:
        dprintf(("OS2: WM_DESTROY %x", hwnd));
        if(win32wnd->MsgDestroy()) {
                goto RunDefWndProc;
        }
        break;

    case WM_ENABLE:
        dprintf(("OS2: WM_ENABLE %x", hwnd));
        if(win32wnd->MsgEnable((ULONG)mp1)) {
                goto RunDefWndProc;
        }
        break;

    case WM_SHOW:
        dprintf(("OS2: WM_SHOW %x", hwnd));
        if(win32wnd->MsgShow((ULONG)mp1)) {
                goto RunDefWndProc;
        }
        break;

    case WM_ADJUSTWINDOWPOS:
    {
      ULONG    x, y;
      PSWP     pswp = (PSWP)mp1;

        dprintf(("OS2: WM_ADJUSTWINDOWPOS %x", hwnd));

        if(pswp->fl & SWP_MOVE) {
            if(win32wnd->isChild()) {
                x = pswp->x;
                y = pswp->cy - y - 1;
            }
            else {
                OSLIBPOINT point;

                point.x = pswp->x;
                point.y = pswp->y;

                MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
                x = point.x;
                y = point.y;
            }
            if(win32wnd->MsgMove(x, y)) {
                goto RunDefWndProc;
            }
            break;
        }
        goto RunDefWndProc;
    }

    case WM_WINDOWPOSCHANGED:
    {
        dprintf(("OS2: WM_WINDOWPOSCHANGED %x", hwnd));
        goto RunDefWndProc;
    }

    case WM_ERASEBACKGROUND:
    {
     HPS hps;

        dprintf(("OS2: WM_ERASEBACKGROUND %x", hwnd));
        hps = WinGetPS(hwnd);
        if(!win32wnd->MsgEraseBackGround((ULONG)hps))
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

        dprintf(("OS2: WM_SIZE %x", hwnd));
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

        dprintf(("OS2: WM_ACTIVATE %x", hwnd));
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
        dprintf(("OS2: WM_FOCUSCHANGE %x", hwnd));
        goto RunDefWndProc;

    case WM_SETFOCUS:
    {
      HWND hwndFocus = (HWND)mp1;

        dprintf(("OS2: WM_SETFOCUS %x", hwnd));
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
    //Mouse messages (OS/2 Window coordinates -> Win32 coordinates relative to screen
    //**************************************************************************
    case WM_BUTTON1DOWN:
        dprintf(("OS2: WM_BUTTON1DOWN %x", hwnd));
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_LEFTDOWN, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;

    case WM_BUTTON1UP:
        dprintf(("OS2: WM_BUTTON1UP %x", hwnd));
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_LEFTUP, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON1DBLCLK:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_LEFTDBLCLICK, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2DOWN:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_RIGHTDOWN, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2UP:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_RIGHTUP, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2DBLCLK:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_RIGHTDBLCLICK, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON3DOWN:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_MIDDLEDOWN, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON3UP:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_MIDDLEUP, point.x, point.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON3DBLCLK:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_MIDDLEDBLCLICK, point.x, point.y)) {
                goto RunDefWndProc;
        }
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
        ULONG keystate = 0;
        if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON1))
            keystate |= WMMOVE_LBUTTON;
        if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON2))
            keystate |= WMMOVE_MBUTTON;
        if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON3))
            keystate |= WMMOVE_RBUTTON;
        if(WinGetKeyState(HWND_DESKTOP, VK_SHIFT))
            keystate |= WMMOVE_SHIFT;
        if(WinGetKeyState(HWND_DESKTOP, VK_CTRL))
            keystate |= WMMOVE_CTRL;

        //OS/2 Window coordinates -> Win32 Window coordinates
        if(!win32wnd->MsgMouseMove(keystate, SHORT1FROMMP(mp1), MapOS2ToWin32Y(win32wnd, SHORT2FROMMP(mp1)))) {
                goto RunDefWndProc;
        }
        break;
    }

    //**************************************************************************
    //Slider messages
    //**************************************************************************
    case WM_VSCROLL:
    case WM_HSCROLL:

    case WM_CONTROL:

    case WM_COMMAND:
        if(SHORT1FROMMP(mp2) == CMDSRC_MENU) {
            win32wnd->MsgCommand(CMD_MENU, SHORT1FROMMP(mp1), 0);
        }
        if(SHORT1FROMMP(mp2) == CMDSRC_ACCELERATOR) {
            win32wnd->MsgCommand(CMD_ACCELERATOR, SHORT1FROMMP(mp1), 0);
        }
        //todo controls + accelerators
        break;

    case WM_SYSCOMMAND:
        dprintf(("WM_SYSCOMMAND"));
        break;

    case WM_CHAR:
        dprintf(("WM_CHAR"));

    case WM_INITMENU:
    case WM_MENUSELECT:
    case WM_MENUEND:
    case WM_NEXTMENU:

    case WM_TIMER:
        goto RunDefWndProc;

    case WM_SETWINDOWPARAMS:
    {
      WNDPARAMS *wndParams = (WNDPARAMS *)mp1;

        dprintf(("OS2: WM_SETWINDOWPARAMS %x", hwnd));
        if(wndParams->fsStatus & WPM_TEXT) {
            if(win32wnd->MsgSetText(wndParams->pszText, wndParams->cchText)) {
                    goto RunDefWndProc;
            }
        }
        goto RunDefWndProc;
    }

    case WM_PAINT:
        dprintf(("OS2: WM_PAINT %x", hwnd));
        if(win32wnd->MsgPaint(0, 0)) {
                goto RunDefWndProc;
        }
        break;

    case WM_HITTEST:
        if(win32wnd->MsgHitTest((*(POINTS *)&mp1).x, MapOS2ToWin32Y(OSLIB_HWND_DESKTOP, hwnd, (*(POINTS *)&mp1).y))) {
                goto RunDefWndProc;
        }
        break;

    case WM_SYSCOLORCHANGE:
    case WM_SYSVALUECHANGED:
    case WM_CALCVALIDRECTS:
    case WM_QUERYWINDOWPARAMS:
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
        dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
        RestoreOS2TIB();
        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  RestoreOS2TIB();
  return (MRESULT)FALSE;

RunDefWndProc:
  dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
  RestoreOS2TIB();
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
} /* End of Win32WindowProc */
//******************************************************************************
//******************************************************************************
