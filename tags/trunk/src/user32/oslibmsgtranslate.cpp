/* $Id: oslibmsgtranslate.cpp,v 1.41 2001-02-21 20:51:06 sandervl Exp $ */
/*
 * Window message translation functions for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 * Copyright 1999      Rene Pronk (R.Pronk@twi.tudelft.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Extra msgs: which messages must be put into the queue and which can be sent directly?
 *       (According to the docs TranslateMessage really inserts a msg in the queue)
 * TODO: Filter translation isn't correct for posted messages
 *
 */
#define  INCL_WIN
#define  INCL_PM
#define  INCL_DOSPROCESS
#include <os2wrap.h>
#include <string.h>
#include <misc.h>
#include <winconst.h>
#include "oslibmsg.h"
#include <winuser32.h>
#include "win32wdesktop.h"
#include "oslibutil.h"
#include "timer.h"
#include <thread.h>
#include <wprocess.h>
#include "pmwindow.h"
#include "oslibwin.h"
#include "winmouse.h"

#define DBG_LOCALLOG    DBG_oslibmsgtranslate
#include "dbglocal.h"

//Used for key translation while processing WM_CHAR message
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
               0x21,    // VK_PAGEUP        VK_PRIOR
               0x22,    // VK_PAGEDOWN      VK_NEXT
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
               0x0D,    // VK_ENTER         VK_RETURN
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

//******************************************************************************
//******************************************************************************
ULONG GetMouseKeyState()
{
  ULONG keystate = 0;

  if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON1) & 0x8000)
    keystate |= MK_LBUTTON_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON2) & 0x8000)
    keystate |= MK_RBUTTON_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON3) & 0x8000)
    keystate |= MK_MBUTTON_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)
    keystate |= MK_SHIFT_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000)
    keystate |= MK_CONTROL_W;

  return keystate;
}
//******************************************************************************
//******************************************************************************
LONG IsNCMouseMsg(Win32BaseWindow *win32wnd)
{
  return ((win32wnd->getLastHitTestVal() != HTCLIENT_W) && (WinQueryCapture(HWND_DESKTOP) != win32wnd->getOS2WindowHandle()));
}
//******************************************************************************
//******************************************************************************
BOOL OS2ToWinMsgTranslate(void *pTeb, QMSG *os2Msg, MSG *winMsg, BOOL isUnicode, BOOL fMsgRemoved)
{
  Win32BaseWindow *win32wnd = 0;
  OSLIBPOINT       point, ClientPoint;
  POSTMSG_PACKET  *packet;
  TEB             *teb = (TEB *)pTeb;
  BOOL             fWasDisabled = FALSE;
  int i;

  memset(winMsg, 0, sizeof(MSG));
  win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(os2Msg->hwnd);

  //PostThreadMessage posts WIN32APP_POSTMSG msg without window handle
  //Realplayer starts a timer with hwnd 0 & proc 0; check this here
  if(win32wnd == 0 && (os2Msg->msg != WM_CREATE && os2Msg->msg != WM_QUIT && os2Msg->msg != WM_TIMER && os2Msg->msg != WIN32APP_POSTMSG))
  {
        goto dummymessage; //not a win32 client window
  }
  winMsg->time = os2Msg->time;
  //CB: PM bug or undocumented feature? ptl.x highword is set!
  winMsg->pt.x = os2Msg->ptl.x & 0xFFFF;
  winMsg->pt.y = mapScreenY(os2Msg->ptl.y);

  if(win32wnd) //==0 for WM_CREATE/WM_QUIT
    winMsg->hwnd = win32wnd->getWindowHandle();

  switch(os2Msg->msg)
  {
    case WIN32APP_POSTMSG:
    {
        packet = (POSTMSG_PACKET *)os2Msg->mp2;
        if(packet && ((ULONG)os2Msg->mp1 == WIN32MSG_MAGICA || (ULONG)os2Msg->mp1 == WIN32MSG_MAGICW)) {
            winMsg->message = packet->Msg;
            winMsg->wParam  = packet->wParam;
            winMsg->lParam  = packet->lParam;
            if(fMsgRemoved == MSG_REMOVE) free(packet); //free the shared memory here
            break;
        }
        goto dummymessage;
    }

    //OS/2 msgs
    case WM_CREATE:
    {
        if(teb->o.odin.newWindow == 0) {
            DebugInt3();
            goto dummymessage;
        }

        win32wnd = (Win32BaseWindow *)teb->o.odin.newWindow;

        winMsg->message = WINWM_CREATE;
        winMsg->hwnd    = win32wnd->getWindowHandle();
        winMsg->wParam  = 0;
        winMsg->lParam  = (LPARAM)win32wnd->tmpcs;
        break;
    }

    case WM_QUIT:
        winMsg->message = WINWM_QUIT;
        break;

    case WM_CLOSE:
        winMsg->message = WINWM_CLOSE;
        break;

    case WM_DESTROY:
        winMsg->message = WINWM_DESTROY;
        break;

    case WM_ENABLE:
        winMsg->message = WINWM_ENABLE;
        winMsg->wParam  = SHORT1FROMMP(os2Msg->mp1);
        break;

    case WM_SHOW:
        winMsg->message = WINWM_SHOWWINDOW;
        winMsg->wParam  = SHORT1FROMMP(os2Msg->mp1);
        break;

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp  = (PSWP)os2Msg->mp1;
      SWP       swpOld = *(pswp + 1);
      HWND      hParent = NULLHANDLE;
      LONG      yDelta = pswp->cy - swpOld.cy;
      LONG      xDelta = pswp->cx - swpOld.cx;

        dprintf(("OS2: WM_WINDOWPOSCHANGED %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0) goto dummymessage;

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto dummymessage; //parent has just been destroyed
            }
        }
        if(win32wnd->getParent()) {
   	      OSLibMapSWPtoWINDOWPOS(pswp, &teb->o.odin.wp, &swpOld, win32wnd->getParent()->getWindowHeight(), 
                                     win32wnd->getParent()->getClientRectPtr()->left,
                                     win32wnd->getParent()->getClientRectPtr()->top,
                                     win32wnd->getOS2WindowHandle());
        } 
        else  OSLibMapSWPtoWINDOWPOS(pswp, &teb->o.odin.wp, &swpOld, OSLibQueryScreenHeight(), 0, 0, win32wnd->getOS2WindowHandle());

        if (!win32wnd->CanReceiveSizeMsgs())    goto dummymessage;

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
/////                dprintf(("Set client rectangle to (%d,%d)(%d,%d)", swpOld.x, swpOld.y, swpOld.x + swpOld.cx, swpOld.y + swpOld.cy));
/////                win32wnd->setClientRect(swpOld.x, swpOld.y, swpOld.x + swpOld.cx, swpOld.y + swpOld.cy);

                teb->o.odin.wp.hwnd = win32wnd->getWindowHandle();
                if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
                {
                        Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
                        if(wndAfter)
                  	      teb->o.odin.wp.hwndInsertAfter = wndAfter->getWindowHandle();
			else  teb->o.odin.wp.hwndInsertAfter = HWND_TOP_W;
                }
        }
        winMsg->message = WINWM_WINDOWPOSCHANGED;
        winMsg->lParam  = (LPARAM)&teb->o.odin.wp;
        break;
    }

    case WM_ACTIVATE:
    {
      HWND hwndActivate = (HWND)os2Msg->mp2;
      BOOL fMinimized = FALSE;

        hwndActivate = OS2ToWin32Handle(hwndActivate);
	if(hwndActivate == 0) {
                //another (non-win32) application's window
                //set to desktop window handle
		hwndActivate = windowDesktop->getWindowHandle();
	}

        if(win32wnd->getStyle() & WS_MINIMIZE_W)
        {
           fMinimized = TRUE;
        }

        winMsg->message = WINWM_ACTIVATE;
        winMsg->wParam  = MAKELONG((SHORT1FROMMP(os2Msg->mp1)) ? WA_ACTIVE_W : WA_INACTIVE_W, fMinimized);
        winMsg->lParam  = (LPARAM)hwndActivate;
        break;
    }

    case WM_SETFOCUS:
    {
      HWND hwndFocus = (HWND)os2Msg->mp1;

        if(WinQueryWindowULong(hwndFocus, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC) {
                //another (non-win32) application's window
                //set to NULL (allowed according to win32 SDK) to avoid problems
                hwndFocus = NULL;
        }
        else    hwndFocus = OS2ToWin32Handle(hwndFocus);

        if((ULONG)os2Msg->mp2 == TRUE) {
                winMsg->message = WINWM_SETFOCUS;
                winMsg->wParam  = (WPARAM)hwndFocus;
        }
        else {
                winMsg->message = WINWM_KILLFOCUS;
                winMsg->wParam  = (WPARAM)hwndFocus;
        }
        break;
    }

    //**************************************************************************
    //Mouse messages (OS/2 Window coordinates -> Win32 coordinates relative to screen
    //**************************************************************************
    case WM_HITTEST:
      winMsg->message = WINWM_NCHITTEST;
      winMsg->wParam  = 0;
      winMsg->lParam  = MAKELONG(winMsg->pt.x,winMsg->pt.y);
      if(!win32wnd->IsWindowEnabled()) {
		if(win32wnd->getParent()) {
			winMsg->hwnd = win32wnd->getParent()->getWindowHandle();
		}
		else	goto dummymessage; //don't send mouse messages to disabled windows
      }
      break;

    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON2DOWN:
    case WM_BUTTON2UP:
    case WM_BUTTON2DBLCLK:
    case WM_BUTTON3DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DBLCLK:
        //WM_NC*BUTTON* is posted when the cursor is in a non-client area of the window

#ifdef ODIN_HITTEST
	//Send WM_HITTEST message
        win32wnd->sendHitTest(MAKELONG(winMsg->pt.x,winMsg->pt.y));
#endif
        win32wnd->sendHitTest(MAKELONG(winMsg->pt.x,winMsg->pt.y));

	//if a window is disabled, it's parent receives the mouse messages
	if(!win32wnd->IsWindowEnabled()) {
		if(win32wnd->getParent()) {
			win32wnd = win32wnd->getParent();
		}
		fWasDisabled = TRUE;
	}

        if(IsNCMouseMsg(win32wnd)) {
            winMsg->message = WINWM_NCLBUTTONDOWN + (os2Msg->msg - WM_BUTTON1DOWN);
            winMsg->wParam  = win32wnd->getLastHitTestVal();
            winMsg->lParam  = MAKELONG(winMsg->pt.x, winMsg->pt.y); //screen coordinates
        }
        else {
            point.x         = (*(POINTS *)&os2Msg->mp1).x;
            point.y         = (*(POINTS *)&os2Msg->mp1).y;
            ClientPoint.x   = mapOS2ToWin32X(win32wnd, point.x);
            ClientPoint.y   = mapOS2ToWin32Y(win32wnd, point.y);

            winMsg->message = WINWM_LBUTTONDOWN + (os2Msg->msg - WM_BUTTON1DOWN);
            winMsg->wParam  = GetMouseKeyState();
            winMsg->lParam  = MAKELONG(ClientPoint.x, ClientPoint.y); //client coordinates
        }
        if((fMsgRemoved == MSG_REMOVE) && ISMOUSE_CAPTURED())
        {
            if(DInputMouseHandler(win32wnd->getWindowHandle(), winMsg->message, winMsg->pt.x, winMsg->pt.y)) {
                goto dummymessage; //dinput swallowed message
            }
        }

	if(fWasDisabled) {
		if(win32wnd) {
			winMsg->hwnd = win32wnd->getWindowHandle();
		}
		else	goto dummymessage; //don't send mouse messages to disabled windows
	}
        break;

    case WM_BUTTON2CLICK:
    case WM_BUTTON1CLICK:
    case WM_BUTTON3CLICK:
        goto dummymessage;

    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:
    case WM_BUTTON3MOTIONSTART:
    case WM_BUTTON3MOTIONEND:
        //no break; translate to WM_MOUSEMOVE
        //Some applications (e.g. Unreal) retrieve all mouse messages
        //when a mouse button is pressed and don't expect WM_NULL

    case WM_MOUSEMOVE:
    {
        //WM_NCMOUSEMOVE is posted when the cursor moves into a non-client area of the window

#ifdef ODIN_HITTEST
	//Send WM_HITTEST message
        win32wnd->sendHitTest(MAKELONG(winMsg->pt.x,winMsg->pt.y));
#endif

	//if a window is disabled, it's parent receives the mouse messages
	if(!win32wnd->IsWindowEnabled()) {
		if(win32wnd->getParent()) {
			win32wnd = win32wnd->getParent();
		}
		fWasDisabled = TRUE;
	}
        if(IsNCMouseMsg(win32wnd))
        {
          winMsg->message = WINWM_NCMOUSEMOVE;
          winMsg->wParam  = (WPARAM)win32wnd->getLastHitTestVal();
          winMsg->lParam  = MAKELONG(winMsg->pt.x,winMsg->pt.y);
        }
        else
        {
          point.x         = (*(POINTS *)&os2Msg->mp1).x;
          point.y         = (*(POINTS *)&os2Msg->mp1).y;
          ClientPoint.x   = mapOS2ToWin32X(win32wnd, point.x);
          ClientPoint.y   = mapOS2ToWin32Y(win32wnd, point.y);

          winMsg->message = WINWM_MOUSEMOVE;
          winMsg->wParam  = GetMouseKeyState();
          winMsg->lParam  = MAKELONG(ClientPoint.x, ClientPoint.y); //client coordinates
        }
        if((fMsgRemoved == MSG_REMOVE) && ISMOUSE_CAPTURED())
        {
            if(DInputMouseHandler(win32wnd->getWindowHandle(), winMsg->message, winMsg->pt.x, winMsg->pt.y)) {
                goto dummymessage; //dinput swallowed message
            }
        }
	if(fWasDisabled) {
		if(win32wnd) {
			winMsg->hwnd = win32wnd->getWindowHandle();
		}
		else	goto dummymessage; //don't send mouse messages to disabled windows
	}
        //OS/2 Window coordinates -> Win32 Window coordinates
        break;
    }

    case WM_CONTROL:
      goto dummymessage;

    case WM_COMMAND:
        if(SHORT1FROMMP(os2Msg->mp2) == CMDSRC_MENU) {
            winMsg->message = WINWM_COMMAND;
            winMsg->wParam  = (WPARAM)SHORT1FROMMP(os2Msg->mp1); //id
            break;
        }
        //todo controls
        goto dummymessage;

    case WM_SYSCOMMAND:
    {
      ULONG x = 0, y = 0;
      ULONG win32sc;

        if(SHORT2FROMMP(os2Msg->mp2) == TRUE) {//syscommand caused by mouse action
            POINTL pointl;
            WinQueryPointerPos(HWND_DESKTOP, &pointl);
            x = pointl.x;
            y = mapScreenY(y);
        }
        switch(SHORT1FROMMP(os2Msg->mp1)) {
        case SC_MOVE:
            win32sc = SC_MOVE_W;
            break;
        case SC_CLOSE:
            win32sc = SC_CLOSE_W;
            break;
        case SC_MAXIMIZE:
            win32sc = SC_MAXIMIZE_W;
            break;
        case SC_MINIMIZE:
            win32sc = SC_MINIMIZE_W;
            break;
        case SC_NEXTFRAME:
        case SC_NEXTWINDOW:
            win32sc = SC_NEXTWINDOW_W;
            break;
        case SC_RESTORE:
            win32sc = SC_RESTORE_W;
            break;
        case SC_TASKMANAGER:
            win32sc = SC_TASKLIST_W;
            break;
        default:
            goto dummymessage;
        }
        winMsg->message    = WINWM_SYSCOMMAND;
        winMsg->wParam = (WPARAM)win32sc;
        winMsg->lParam = MAKELONG((USHORT)x, (USHORT)y);
        break;
    }
    case WM_CHAR:
    {
        ULONG repeatCount=0, virtualKey=0, keyFlags=0, scanCode=0;
        ULONG flags = SHORT1FROMMP(os2Msg->mp1);
        BOOL keyWasPressed;
        char c;

        teb->o.odin.fTranslated = FALSE;
        repeatCount = CHAR3FROMMP(os2Msg->mp1);
        scanCode = CHAR4FROMMP(os2Msg->mp1);
        keyWasPressed = ((SHORT1FROMMP (os2Msg->mp1) & KC_PREVDOWN) == KC_PREVDOWN);

        dprintf(("PM: WM_CHAR: %x %x %d %x", SHORT1FROMMP(os2Msg->mp2), SHORT2FROMMP(os2Msg->mp2), repeatCount, scanCode));
        dprintf(("PM: WM_CHAR: hwnd %x flags %x", win32wnd->getWindowHandle(), flags));

        // vitali add begin
        if ( ( SHORT1FROMMP(os2Msg->mp2) & 0x0FF ) == 0x0E0 )
        {
            // an extended key ( arrows, ins, del and so on )
            // get "virtual" scancode from character code cause
            // for "regular" keys they are equal
            scanCode = ( SHORT1FROMMP(os2Msg->mp2) >> 8) & 0x0FF;
        }
        // vitali add end

        // both WM_KEYUP & WM_KEYDOWN want a virtual key, find the right Win32 virtual key
        // given the OS/2 virtual key and OS/2 character

        //if (((SHORT1FROMMP (mp1) & KC_CHAR) == KC_CHAR) ||
        //    ((SHORT1FROMMP (mp1) & KC_LONEKEY) == KC_LONEKEY))
        c = 0;
        if ((SHORT1FROMMP (os2Msg->mp1) & 0xFF) != 0)
        {
            c = SHORT1FROMMP (os2Msg->mp2);
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
        if (SHORT2FROMMP (os2Msg->mp2) <= VK_BLK2)
            virtualKey = virtualKeyTable [SHORT2FROMMP (os2Msg->mp2)];

VirtualKeyFound:
        dprintf (("VIRTUALKEYFOUND:(%x)", virtualKey));

        winMsg->wParam  = virtualKey;
        winMsg->lParam  = repeatCount & 0x0FFFF;                 // bit 0-15, repeatcount
        winMsg->lParam |= (scanCode & 0x0FF) << 16;              // bit 16-23, scancode

        //TODO: Is this correct and complete? (how does PM differentiate between
        //      i.e numeric pad pgdn & 'normal' pgdn??)
        //Check if it's an extended key
        switch(virtualKey) {
        case VK_RETURN_W:
                //The enter key on the numeric keypad is an extended key
                if(SHORT2FROMMP(os2Msg->mp2) != VK_NEWLINE)
                        break;
                //no break
        case VK_LEFT_W:
        case VK_RIGHT_W:
        case VK_DOWN_W:
        case VK_UP_W:
        case VK_PRIOR_W:
        case VK_NEXT_W:
        case VK_END_W:
        case VK_DIVIDE_W:
        case VK_DELETE_W:
        case VK_HOME_W:
        case VK_INSERT_W:
        case VK_RCONTROL_W:
        case VK_RMENU_W: //is this the right alt???
                winMsg->lParam = winMsg->lParam | (1<<24);
                break;
        }

        if(!(SHORT1FROMMP(os2Msg->mp1) & KC_ALT))
        {
            //
            // the Alt key is not pressed
            //
            if ((flags & KC_KEYUP) == KC_KEYUP) {
                // send WM_KEYUP message

                winMsg->message = WINWM_KEYUP;
                winMsg->lParam |= 1 << 30;                              // bit 30, previous state, always 1 for a WM_KEYUP message
                winMsg->lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP
            }
            else {
                // send WM_KEYDOWN message
                winMsg->message = WINWM_KEYDOWN;
                if (keyWasPressed)
                    winMsg->lParam |= 1 << 30;                          // bit 30, previous state, 1 means key was pressed
            }
        }
        else {
            //
            // the Alt key is pressed
            //
            if ((flags & KC_KEYUP) == KC_KEYUP) {
                // send WM_SYSKEYUP message

                winMsg->message = WINWM_SYSKEYUP;
                winMsg->lParam |= 1 << 30;                              // bit 30, previous state, always 1 for a WM_KEYUP message
                winMsg->lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP
            }
            else {
                // send WM_SYSKEYDOWN message
                winMsg->message = WINWM_SYSKEYDOWN;
                if (keyWasPressed)
                    winMsg->lParam |= 1 << 30;                          // bit 30, previous state, 1 means key was pressed
            }
	    if(winMsg->wParam == VK_MENU_W) {
                winMsg->message = 0; //WM_SYS* already implies Alt
            }
        }
        if(ISKDB_CAPTURED())
        {
            if(DInputKeyBoardHandler(winMsg)) {
                goto dummymessage; //dinput swallowed message
            }
        }
        break;
    }

    case WM_TIMER:
        if (os2Msg->mp2)
        {
          BOOL sys;
          ULONG id;

          if (TIMER_GetTimerInfo(os2Msg->hwnd,(ULONG)os2Msg->mp1,&sys,&id))
          {
                winMsg->wParam = (WPARAM)id;
                winMsg->message= (sys) ? WINWM_SYSTIMER : WINWM_TIMER;
                break;
          }
        }
        goto dummymessage; //for caret blinking

    case WM_SETWINDOWPARAMS:
    {
      WNDPARAMS *wndParams = (WNDPARAMS *)os2Msg->mp1;

        if(wndParams->fsStatus & WPM_TEXT) {
            winMsg->message = WINWM_SETTEXT;
            winMsg->lParam  = (LPARAM)wndParams->pszText;
            break;
        }
        goto dummymessage;
    }

#if 0
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
            goto dummymessage;
      }
    }
#endif

    case WM_PAINT:
    {
        if(win32wnd->IsWindowIconic()) {
                winMsg->message = WINWM_PAINTICON;
        }
        else    winMsg->message = WINWM_PAINT;
        break;
    }

    case WM_CONTEXTMENU:
        winMsg->message = WINWM_CONTEXTMENU;
        winMsg->wParam  = win32wnd->getWindowHandle();
        winMsg->lParam  = MAKELONG(winMsg->pt.x,winMsg->pt.y);
        break;

    case WM_RENDERFMT:
        winMsg->message = WINWM_RENDERFORMAT;
        winMsg->wParam  = (UINT) os2Msg->mp1;
        break;

    case WM_RENDERALLFMTS:
        winMsg->message = WINWM_RENDERALLFORMATS;
        break;

    case WM_DESTROYCLIPBOARD:
        winMsg->message = WINWM_DESTROYCLIPBOARD;
        break;

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
dummymessage:
        winMsg->message = 0;
        winMsg->wParam  = 0;
        winMsg->lParam  = 0;
        return FALSE;
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinTranslateMessage(MSG *msg)
{
 TEB *teb;

    teb = GetThreadTEB();
    if(!teb) {
        return FALSE;
    }
    //NOTE: These actually need to be posted so that the next message retrieved by GetMessage contains
    //      the newly generated WM_CHAR message.
    if(!teb->o.odin.fTranslated && teb->o.odin.os2msg.msg == WM_CHAR && !((SHORT1FROMMP(teb->o.odin.os2msg.mp1) & KC_KEYUP) == KC_KEYUP))
    {//TranslatedMessage was called before DispatchMessage, so queue WM_CHAR message
            ULONG fl = SHORT1FROMMP(teb->o.odin.os2msg.mp1);
            MSG extramsg;

            memcpy(&extramsg, msg, sizeof(MSG));
            extramsg.wParam = SHORT1FROMMP(teb->o.odin.os2msg.mp2);
            extramsg.lParam = 0;

            if(!(fl & KC_CHAR) && msg->message < WINWM_SYSKEYDOWN) {
                return FALSE;
            }

            if(fl & KC_VIRTUALKEY) {
                if(msg->wParam)
                        extramsg.wParam = msg->wParam;
                else    extramsg.wParam = SHORT2FROMMP(teb->o.odin.os2msg.mp2);
            }


            if(msg->message >= WINWM_SYSKEYDOWN) {
                    extramsg.message = WINWM_SYSCHAR;
            }
            else    extramsg.message = WINWM_CHAR;

            if(fl & KC_DEADKEY) {
                extramsg.message++;  //WM_DEADCHAR/WM_SYSDEADCHAR
            }

            extramsg.lParam = msg->lParam & 0x00FFFFFF;
            if(fl & KC_ALT)
                extramsg.lParam |= (1<<29);
            if(fl & KC_PREVDOWN)
                extramsg.lParam |= (1<<30);
            if(fl & KC_KEYUP)
                extramsg.lParam |= (1<<31);

            teb->o.odin.fTranslated = TRUE;
            memcpy(&teb->o.odin.msgWCHAR, &extramsg, sizeof(MSG));
            return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************

