/* $Id: pmwindow.cpp,v 1.9 1999-09-26 10:09:59 sandervl Exp $ */
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
#include <win32wbase.h>
#include <win32dlg.h>
#include "pmwindow.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibmsg.h"
#include "dc.h"

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
     CS_SIZEREDRAW | CS_HITTEST | CS_MOVENOTIFY,
     NROF_WIN32WNDBYTES)) {
        dprintf(("WinRegisterClass Win32BaseWindow failed"));
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
 Win32BaseWindow    *win32wnd;
 APIRET          rc;

  //Restore our FS selector
  SetWin32TIB();

  win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

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
//        win32wnd->RemoveFakeOpen32();
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
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
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
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           wp.hwndInsertAfter = wndAfter->getWindowHandle();
        }
        classStyle = win32wnd->getClass()->getStyle();
        if ((yDelta != 0) && ((classStyle & CS_VREDRAW_W) ||
           ((classStyle & CS_HREDRAW_W) && (pswp->cx != pswpo->cx))))
        {
            HENUM henum = WinBeginEnumWindows(pswp->hwnd);
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

            if (i)
               WinSetMultWindowPos(GetThreadHAB(), swp, i);
        }
        win32wnd->MsgPosChanged((LPARAM)&wp);

        break;
    }

    case WM_ERASEBACKGROUND:
    {
        if (!win32wnd->isSupressErase()) {
            BOOL erased = sendEraseBkgnd (win32wnd);
            win32wnd->setEraseBkgnd (!erased, !erased);
        }
        break;
    }

    case WM_MOVE:
    {
        if (!win32wnd->isFrameWindow()) break;

        HWND      hFrame = win32wnd->getOS2FrameWindowHandle();
        SWP       swp, swpo;
        WINDOWPOS wp;
        ULONG     parentHeight = 0;
        RECTL     rcl;

        WinQueryWindowRect (hwnd, &rcl);
        WinMapWindowPoints (hwnd, hFrame, (PPOINTL)&rcl, 2);
        swp.x  = swpo.x  = rcl.xLeft;
        swp.y  = swpo.y  = rcl.yBottom;
        swp.cx = swpo.cx = rcl.xRight - rcl.xLeft;
        swp.cy = swpo.cy = rcl.yTop   - rcl.yBottom;
        swp.fl = SWP_MOVE | SWP_NOREDRAW;
        swp.hwnd             = hwnd;
        swp.hwndInsertBehind = NULLHANDLE;

        OSLibMapSWPtoWINDOWPOS(&swp, &wp, &swpo, NULLHANDLE, hFrame);

        wp.flags &= ~SWP_NOMOVE_W;
        wp.hwnd = win32wnd->getWindowHandle();
        win32wnd->setWindowRect(wp.x, wp.y, wp.x + wp.cx, wp.y + wp.cy);
        win32wnd->setClientRect(swpo.x, swpo.y, swpo.x + swpo.cx, swpo.y + swpo.cy);
        win32wnd->MsgPosChanged((LPARAM)&wp);
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
    //Only send this message when the mouse isn't captured
        if(WinQueryCapture(HWND_DESKTOP) != NULLHANDLE) {
                goto RunDefWndProc;
        }
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
	//NOTE: Do not call the default OS/2 window handler as that one changes
        //      the mousepointer!
        win32wnd->MsgMouseMove(keystate, SHORT1FROMMP(mp1), MapOS2ToWin32Y(win32wnd, SHORT2FROMMP(mp1)));
        break;
    }

    //**************************************************************************
    //Slider messages
    //**************************************************************************
    case WM_VSCROLL:
    case WM_HSCROLL:
    {
     ULONG scrollPos, scrollCode, scrollMsg;

	scrollCode = SHORT2FROMMP(mp2);
	scrollPos  = SHORT1FROMMP(mp2);
	scrollMsg  = msg;

	OSLibTranslateScrollCmdAndMsg(&scrollMsg, &scrollCode);

        if(win32wnd->MsgScroll(scrollMsg, scrollCode, scrollPos)) {
            goto RunDefWndProc;
        }
	break;
    }

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
        ULONG repeatCount=0, virtualKey=0, keyFlags=0, scanCode=0;
        ULONG flags = SHORT1FROMMP(mp1);
        BOOL keyWasPressed;
        char c;
        USHORT virtualKeyTable [66] = {
               0x00,    //   OS/2 VK         Win32 VK,    Entry 0 is not used
               0x01,    // VK_BUTTON1       VK_LBUTTON
               0x02,    // VK_BUTTON2       VK_RBUTTON
               0x04,    // VK_BUTTON3       VK_MBUTTON
               0x03,    // VK_BREAK         VK_CANCEL
               0x08,    // VK_BACKSPACE     VK_BACK
               0x09,    // VK_TAB           VK_TAB
               0x00,    // VK_BACKTAB       No equivalent!
               0x0D,    // VK_NEWLINE       VK_RETURN
               0x10,    // VK_SHIFT         VK_SHIFT
               0x11,    // VK_CTRL          VK_CONTROL
               0x12,    // VK_ALT           VK_MENU, best match I guess
               0x12,    // VK_ALTGRAF       VK_MENU, best match I guess
               0x13,    // VK_PAUSE         VK_PAUSE
               0x14,    // VK_CAPSLOCK      VK_CAPITAL
               0x1B,    // VK_ESC           VK_ESCAPE
               0x20,    // VK_SPACE         VK_SPACE
               0x00,    // VK_PAGEUP        No equivalent! At least, I think
               0x00,    // VK_PAGEDOWN      No equivalent! At least, I think
               0x23,    // VK_END           VK_END
               0x24,    // VK_HOME          VK_HOME
               0x25,    // VK_LEFT          VK_LEFT
               0x26,    // VK_UP            VK_UP
               0x27,    // VK_RIGHT         VK_RIGHT
               0x28,    // VK_DOWN          VK_DOWN
               0x2C,    // VK_PRINTSCRN     VK_SNAPSHOT
               0x2D,    // VK_INSERT        VK_INSERT
               0x2E,    // VK_DELETE        VK_DELETE
               0x91,    // VK_SCRLLOCK      VK_SCROLL
               0x90,    // VK_NUMLOCK       VK_NUMLOCK
               0x2B,    // VK_ENTER         VK_EXECUTE, best match I guess
               0x00,    // VK_SYSRQ         No equivalent!
               0x70,    // VK_F1            VK_F1
               0x71,    // VK_F2            VK_F2
               0x72,    // VK_F3            VK_F3
               0x73,    // VK_F4            VK_F4
               0x74,    // VK_F5            VK_F5
               0x75,    // VK_F6            VK_F6
               0x76,    // VK_F7            VK_F7
               0x77,    // VK_F8            VK_F8
               0x78,    // VK_F9            VK_F9
               0x79,    // VK_F10           VK_F10
               0x7A,    // VK_F11           VK_F11
               0x7B,    // VK_F12           VK_F12
               0x7C,    // VK_F13           VK_F13
               0x7D,    // VK_F14           VK_F14
               0x7E,    // VK_F15           VK_F15
               0x7F,    // VK_F16           VK_F16
               0x80,    // VK_F17           VK_F17
               0x81,    // VK_F18           VK_F18
               0x82,    // VK_F19           VK_F19
               0x83,    // VK_F20           VK_F20
               0x84,    // VK_F21           VK_F21
               0x85,    // VK_F22           VK_F22
               0x86,    // VK_F23           VK_F23
               0x87,    // VK_F24           VK_F24
               0x00,    // VK_ENDDRAG       No equivalent!
               0x0C,    // VK_CLEAR         VK_CLEAR
               0xF9,    // VK_EREOF         VK_EREOF
               0xFD,    // VK_PA1           VK_PA1
               0xF6,    // VK_ATTN          VK_ATTN
               0xF7,    // VK_CRSEL         VK_CRSEL
               0xF8,    // VK_EXSEL         VK_EXSEL
               0x00,    // VK_COPY          No equivalent!
               0x00,    // VK_BLK1          No equivalent!
               0x00};   // VK_BLK2          No equivalent!


        repeatCount = SHORT2FROMMP (mp1) >> 8;
        scanCode = SHORT2FROMMP (mp1) & 255;
        keyWasPressed = ((SHORT1FROMMP (mp1) & KC_PREVDOWN) == KC_PREVDOWN);

        // both WM_KEYUP & WM_KEYDOWN want a virtual key, find the right Win32 virtual key
        // given the OS/2 virtual key and OS/2 character

        if (((SHORT1FROMMP (mp1) & KC_CHAR) == KC_CHAR) ||
            ((SHORT1FROMMP (mp1) & KC_LONEKEY) == KC_LONEKEY))
        {
            c = SHORT1FROMMP (mp2);
            if ((c >= 'A') && (c <= 'Z')) {
                virtualKey = c;
                goto VirtualKeyFound;
            }
            if ((c >='a') && (c <= 'z')) {
                virtualKey = c - 32;   // make it uppercase
                goto VirtualKeyFound;
            }
            if ((c >= '0') && (c <= '9')) {
                virtualKey = c;
                goto VirtualKeyFound;
            }
        }

        // convert OS/2 virtual keys to Win32 virtual key
        if (SHORT2FROMMP (mp2) <= VK_BLK2)
            virtualKey = virtualKeyTable [SHORT2FROMMP (mp2)];

VirtualKeyFound:

        if ((WinGetKeyState (HWND_DESKTOP, VK_ALT) & 0x8000) == 0)
        {
            //
            // the Alt key is not pressed
            //
            if ((flags & KC_KEYUP) == KC_KEYUP) {
                // send WM_KEYUP message

                if(win32wnd->MsgKeyUp (repeatCount, scanCode, virtualKey)) {
                    goto RunDefWndProc;
                }
            }
            else {
                // send WM_KEYDOWN message
                if (win32wnd->MsgKeyDown (repeatCount, scanCode, virtualKey, keyWasPressed))
                    goto RunDefWndProc;
            }
        }
        else {
            //
            // the Alt key is pressed
            //
            if ((flags & KC_KEYUP) == KC_KEYUP) {
                // send WM_SYSKEYUP message

                if(win32wnd->MsgSysKeyUp (repeatCount, scanCode, virtualKey)) {
                    goto RunDefWndProc;
                }
            }
            else {
                // send WM_SYSKEYDOWN message
                if (win32wnd->MsgSysKeyDown (repeatCount, scanCode, virtualKey, keyWasPressed))
                    goto RunDefWndProc;
            }
        }

        break;
    }
#if 0
    case WM_CHAR:
    {
     ULONG keyflags = 0, vkey = 0;
     ULONG fl = SHORT1FROMMP(mp1);

        if(!(fl & KC_CHAR)) {
//            dprintf(("WM_CHAR: no valid character code"));
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
#endif
    case WM_INITMENU:
    case WM_MENUSELECT:
    case WM_MENUEND:
    case WM_NEXTMENU:
        goto RunDefWndProc;

    case WM_TIMER:
        win32wnd->MsgTimer((ULONG)mp1);
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

        if (WinQueryUpdateRect (hwnd, NULL)) {
            if (!win32wnd->isSupressErase()) {
                BOOL erased = sendEraseBkgnd (win32wnd);
                win32wnd->setEraseBkgnd (!erased, !erased);
            }
        }
        win32wnd->setSupressErase (FALSE);

        if(win32wnd->MsgPaint(0, 0)) {
                goto RunDefWndProc;
        }
        break;

    case WM_HITTEST:
    // Only send this message if the window is enabled
        if (WinIsWindowEnabled(hwnd))
    {
            if(win32wnd->MsgHitTest((*(POINTS *)&mp1).x, MapOS2ToWin32Y(OSLIB_HWND_DESKTOP, hwnd, (*(POINTS *)&mp1).y))) {
                    goto RunDefWndProc;
        }
        }
        else    goto RunDefWndProc;
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
