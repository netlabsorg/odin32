/* $Id: pmwindow.cpp,v 1.24 1999-08-28 19:32:46 sandervl Exp $ */
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

#include <os2.h>                        /* PM header file               */
#include <os2wrap.h>
#include <stdlib.h>
#include "win32type.h"
#include <winconst.h>
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

RECTL desktopRectl = {0};
ULONG ScreenWidth  = 0;
ULONG ScreenHeight = 0;

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
     NROF_WIN32WNDBYTES)) {
        dprintf(("WinRegisterClass Win32Window failed"));
        return(FALSE);
   }

   WinQueryWindowRect(HWND_DESKTOP, &desktopRectl);
   ScreenWidth  = desktopRectl.xRight;
   ScreenHeight = desktopRectl.yTop;

   dprintf(("InitPM: Desktop (%d,%d)", ScreenWidth, ScreenHeight));
   return OSLibInitMsgQueue();
} /* End of main */
//******************************************************************************
//Win32 window message handler
//******************************************************************************
MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 POSTMSG_PACKET *postmsg;
 OSLIBPOINT      point, ClientPoint;
 Win32Window    *win32wnd;
 APIRET          rc;

  //Restore our FS selector
  SetWin32TIB();

  win32wnd = Win32Window::GetWindowFromOS2Handle(hwnd);

  if(msg != WM_CREATE && win32wnd == NULL) {
        dprintf(("Invalid win32wnd pointer for window %x!!", hwnd));
        goto RunDefWndProc;
  }
  if(msg > WIN32APP_USERMSGBASE) {
	//win32 app user message
	dprintf(("PMWINDOW: Message %x (%x,%x) posted to window %x", (ULONG)msg-WIN32APP_USERMSGBASE, mp1, mp2, hwnd));
        win32wnd->SendMessageA((ULONG)msg-WIN32APP_USERMSGBASE, (ULONG)mp1, (ULONG)mp2);
  }
  switch( msg )
  {
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
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld;
      WINDOWPOS wp;
      ULONG    parentHeight = 0;
      HWND      hParent = NULLHANDLE, hFrame = NULLHANDLE;

        dprintf(("OS2: WM_ADJUSTWINDOWPOS %x %x (%d,%d) (%d,%d)", hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0) break;

        WinQueryWindowPos(hwnd, &swpOld);

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild())
                hParent = win32wnd->getParent()->getOS2WindowHandle();
            else
                hFrame = win32wnd->getOS2FrameWindowHandle();
        }
        OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, hParent, hFrame);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32Window *wndAfter = Win32Window::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           if(wndAfter) wp.hwndInsertAfter = wndAfter->getWindowHandle();
        }
        win32wnd->MsgPosChanging((LPARAM)&wp);
        break;
    }

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp  = (PSWP)mp1;
      PSWP      pswpo = pswp + 1;
      WINDOWPOS wp;
      ULONG     parentHeight = 0;
      HWND      hParent = NULLHANDLE, hFrame = NULLHANDLE;
      LONG      yDelta = pswp->cy - pswpo->cy;
      ULONG     classStyle;

        dprintf(("OS2: WM_WINDOWPOSCHANGED %x %x (%d,%d) (%d,%d)", hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0) break;

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild())
                hParent = win32wnd->getParent()->getOS2WindowHandle();
            else
                hFrame = win32wnd->getOS2FrameWindowHandle();
        }
        OSLibMapSWPtoWINDOWPOS(pswp, &wp, pswpo, hParent, hFrame);

        win32wnd->setWindowRect(wp.x, wp.y, wp.x + wp.cx, wp.y + wp.cy);
        win32wnd->setClientRect(pswpo->x, pswpo->y, pswpo->x + pswpo->cx, pswpo->y + pswpo->cy);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32Window *wndAfter = Win32Window::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           wp.hwndInsertAfter = wndAfter->getWindowHandle();
        }
        classStyle = win32wnd->getClass()->getStyle();
//        if (yDelta != 0)
        if ((yDelta != 0) && ((classStyle & CS_VREDRAW_W) ||
           ((classStyle & CS_HREDRAW_W) && (pswp->cx != pswpo->cx))))
        {
            HENUM henum = WinBeginEnumWindows(pswp->hwnd);
            SWP swp[10];
            int i = 0;
            HWND hwnd;

            dprintf(("move children"));
            while ((hwnd = WinGetNextWindow(henum)) != NULLHANDLE)
            {
#if 0
               /* Do not move MDI clients.  MDI clients are a special case,
                * even though they are bottom aligned they are always supposed
                * to be the same size as their parent.  This code is an
                * optimization and will not work correctly if the this
                * assumption is incorrect.
                */
               WinBase *pBase = (WinBase *)WinQueryDAXData( hwnd );
               if ( pBase && pBase->typeOf() == mdiClient )
               {
                  continue;
               }
#endif
               // We don't know how many children we have so we'll move ten
               // at a time.  Not very nice, I know.
               WinQueryWindowPos(hwnd, &(swp[i]));

               // The SWP flags are all set but PM will ignore any
               // superflous ones, so keep them as they contain useful
               // minimise and maximise flags.
               swp[i].y += yDelta;

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

            // Any remaining windows?
            if (i)
               WinSetMultWindowPos(GetThreadHAB(), swp, i);
        }
        win32wnd->MsgPosChanged((LPARAM)&wp);

        break;
    }

    case WM_ERASEBACKGROUND:
    {
        break;
    }
    case WM_SIZE:
    {
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

        dprintf(("OS2: WM_SETFOCUS %x %d", hwnd, mp2));
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
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_LEFTDOWN, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;

    case WM_BUTTON1UP:
        dprintf(("OS2: WM_BUTTON1UP %x", hwnd));
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_LEFTUP, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON1DBLCLK:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_LEFTDBLCLICK, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2DOWN:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_RIGHTDOWN, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2UP:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_RIGHTUP, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON2DBLCLK:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_RIGHTDBLCLICK, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON3DOWN:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_MIDDLEDOWN, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON3UP:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_MIDDLEUP, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
                goto RunDefWndProc;
        }
        break;
    case WM_BUTTON3DBLCLK:
        point.x = (*(POINTS *)&mp1).x;
        point.y = (*(POINTS *)&mp1).y;
        ClientPoint.x = point.x;
        ClientPoint.y = MapOS2ToWin32Y(hwnd, 1, point.y);
        MapOS2ToWin32Point(OSLIB_HWND_DESKTOP, hwnd, &point);
        if(win32wnd->MsgButton(BUTTON_MIDDLEDBLCLICK, point.x, point.y, ClientPoint.x, ClientPoint.y)) {
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
        //TODO: What do windows apps that handle this messages return?
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
    {
      ULONG x = 0, y = 0;
      ULONG win32sc;

        if(SHORT2FROMMP(mp2) == TRUE) {//syscommand caused by mouse action
            POINTL pointl;
            WinQueryPointerPos(HWND_DESKTOP, &pointl);
            x = pointl.x;
            y = ScreenHeight - y;
        }
        switch(SHORT1FROMMP(mp1)) {
        case SC_MOVE:
            win32sc = WIN32SC_MOVE;
            break;
        case SC_CLOSE:
            win32sc = WIN32SC_CLOSE;
            break;
        case SC_MAXIMIZE:
            win32sc = WIN32SC_MAXIMIZE;
            break;
        case SC_MINIMIZE:
            win32sc = WIN32SC_MINIMIZE;
            break;
        case SC_NEXTFRAME:
        case SC_NEXTWINDOW:
            win32sc = WIN32SC_NEXTWINDOW;
            break;
        case SC_RESTORE:
            win32sc = WIN32SC_RESTORE;
            break;
        case SC_TASKMANAGER:
            win32sc = WIN32SC_TASKLIST;
            break;
        default:
            goto RunDefWndProc;
        }
        dprintf(("WM_SYSCOMMAND %x %x (%d,%d)", hwnd, win32sc, x, y));
        if(win32wnd->MsgSysCommand(win32sc, x, y)) {
            goto RunDefWndProc;
        }
        break;
    }
    case WM_CHAR:
    {
     ULONG keyflags = 0, vkey = 0;
     ULONG fl = SHORT1FROMMP(mp1);

        if(!(fl & KC_CHAR)) {
            dprintf(("WM_CHAR: no valid character code"));
            goto RunDefWndProc;
        }
        if(fl & KC_VIRTUALKEY) {
            vkey = SHORT2FROMMP(mp2);
        }
        if(fl & KC_KEYUP) {
            keyflags |= KEY_UP;
        }
        if(fl & KC_ALT) {
            keyflags |= KEY_ALTDOWN;
        }
        if(fl & KC_PREVDOWN) {
            keyflags |= KEY_PREVDOWN;
        }
        if(fl & KC_DEADKEY) {
            keyflags |= KEY_DEADKEY;
        }
        if(win32wnd->MsgChar(SHORT1FROMMP(mp2), CHAR3FROMMP(mp1), CHAR4FROMMP(mp1), vkey, keyflags)) {
            goto RunDefWndProc;
        }
        break;
    }
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

    case WM_QUERYWINDOWPARAMS:
    {
     PWNDPARAMS wndpars = (PWNDPARAMS)mp1;
     ULONG textlen;
     PSZ   wintext;

        if(wndpars->fsStatus & (WPM_CCHTEXT | WPM_TEXT)) {
            if(wndpars->fsStatus & WPM_CCHTEXT)
                wndpars->cchText = win32wnd->MsgGetTextLength();
            if(wndpars->fsStatus & WPM_TEXT)
                wndpars->pszText = win32wnd->MsgGetText();
            return (MRESULT)TRUE;
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
  return (MRESULT)FALSE;

RunDefWndProc:
//  dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
  RestoreOS2TIB();
  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
} /* End of Win32WindowProc */
//******************************************************************************
//******************************************************************************
