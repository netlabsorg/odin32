/* $Id: oslibmsgtranslate.cpp,v 1.66 2001-10-24 15:41:54 sandervl Exp $ */
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
#include <win32api.h>
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
#include <pmkbdhk.h>
#include <pmscan.h>

#define DBG_LOCALLOG    DBG_oslibmsgtranslate
#include "dbglocal.h"

// Formerly used method of translation based on OS/2 VKEY value didn't work
// right. We need to take a look at the scan code we get from PM and derive
// the Win32 VKEY and scancode from that because sometimes even the scancode
// used in Win32 is different from the PM scancode!
// The format is:
// Win VKEY, Win scancode, (PM scancode) (key description)
USHORT pmscan2winkey [][2] = {
    0, 0,           // 0x00
    0x1B, 0x01,     // 0x01 Esc
    0x31, 0x02,     // 0x02 1
    0x32, 0x03,     // 0x03 2
    0x33, 0x04,     // 0x04 3
    0x34, 0x05,     // 0x05 4
    0x35, 0x06,     // 0x06 5
    0x36, 0x07,     // 0x07 6
    0x37, 0x08,     // 0x08 7
    0x38, 0x09,     // 0x09 8
    0x39, 0x0A,     // 0x0A 9
    0x30, 0x0B,     // 0x0B 0
    0xBD, 0x0C,     // 0x0C -
    0xBB, 0x0D,     // 0x0D =
    0x08, 0x0E,     // 0x0E Bksp
    0x09, 0x0F,     // 0x0F Tab
    0x51, 0x10,     // 0x10 q
    0x57, 0x11,     // 0x11 w
    0x45, 0x12,     // 0x12 e
    0x52, 0x13,     // 0x13 r
    0x54, 0x14,     // 0x14 t
    0x59, 0x15,     // 0x15 y
    0x55, 0x16,     // 0x16 u
    0x49, 0x17,     // 0x17 i
    0x4F, 0x18,     // 0x18 o
    0x50, 0x19,     // 0x19 p
    0xDB, 0x1A,     // 0x1A [
    0xDD, 0x1B,     // 0x1B ]
    0x0D, 0x1C,     // 0x1C Enter
    0x11, 0x1D,     // 0x1D LCtrl
    0x41, 0x1E,     // 0x1E a
    0x53, 0x1F,     // 0x1F s
    0x44, 0x20,     // 0x20 d
    0x46, 0x21,     // 0x21 f
    0x47, 0x22,     // 0x22 g
    0x48, 0x23,     // 0x23 h
    0x4A, 0x24,     // 0x24 j
    0x4B, 0x25,     // 0x25 k
    0x4C, 0x26,     // 0x26 l
    0xBA, 0x27,     // 0x27 ;
    0xDE, 0x28,     // 0x28 '
    0xC0, 0x29,     // 0x29 `
    0x10, 0x2A,     // 0x2A LShift
    0xDC, 0x2B,     // 0x2B Bkslsh
    0x5A, 0x2C,     // 0x2C z
    0x58, 0x2D,     // 0x2D x
    0x43, 0x2E,     // 0x2E c
    0x56, 0x2F,     // 0x2F v
    0x42, 0x30,     // 0x30 b
    0x4E, 0x31,     // 0x31 n
    0x4D, 0x32,     // 0x32 m
    0xBC, 0x33,     // 0x33 ,
    0xBE, 0x34,     // 0x34 .
    0xBF, 0x35,     // 0x35 /
    0x10, 0x36,     // 0x36 RShift
    0x6A, 0x37,     // 0x37 * Pad
//  0x12, 0x38,     // 0x38 LAlt
    VK_LMENU_W, 0x38,     // 0x38 LAlt
    0x20, 0x39,     // 0x39 Space
    0x14, 0x3A,     // 0x3A CapsLk
    0x70, 0x3B,     // 0x3B F1
    0x71, 0x3C,     // 0x3C F2
    0x72, 0x3D,     // 0x3D F3
    0x73, 0x3E,     // 0x3E F4
    0x74, 0x3F,     // 0x3F F5
    0x75, 0x40,     // 0x40 F6
    0x76, 0x41,     // 0x41 F7
    0x77, 0x42,     // 0x42 F8
    0x78, 0x43,     // 0x43 F9
    0x79, 0x44,     // 0x44 F10 (?)
    0x90, 0x145,    // 0x45 NumLk
    0x91, 0x46,     // 0x46 ScrLk
    0x24, 0x47,     // 0x47 7 Pad
    0x26, 0x48,     // 0x48 8 Pad
    0x21, 0x49,     // 0x49 9 Pad
    0x6D, 0x4A,     // 0x4A - Pad
    VK_LEFT_W, 0x4B,  // 0x4B Left Arrow (depends on NumLock State)
    0x0C, 0x4C,     // 0x4C 5 Pad
    0x27, 0x4D,     // 0x4D 6 Pad
    0x6B, 0x4E,     // 0x4E + Pad
    0x23, 0x4F,     // 0x4F 1 Pad
    0x28, 0x50,     // 0x50 2 Pad
    0x22, 0x51,     // 0x51 3 Pad
    0x2D, 0x52,     // 0x52 0 Pad
    0x2E, 0x53,     // 0x53 . Pad
    0, 0,           // 0x54
    0, 0,           // 0x55
    0xe2, 0x56,     // 0x56 "<>|"
    0x7A, 0x57,     // 0x57 F11
    0x7B, 0x58,     // 0x58 F12
    0, 0,           // 0x59
    0x0D, 0x11C,    // 0x5A Enter Pad
    0x11, 0x11D,    // 0x5B RCtrl
    0x6F, 0x135,    // 0x5C / Pad
    0x2D, 0x152,    // 0x5D PrtSc
    0x12, 0x5E,     // 0x5E RAlt
    0x13, 0x45,     // 0x5F Pause
    VK_HOME_W,   0x60,           // 0x60
    VK_UP_W,     0x61,           // 0x61
    VK_PRIOR_W,  0x62,           // 0x62
    VK_LEFT_W,   0x63,           // 0x63
    VK_RIGHT_W,  0x64,           // 0x64
    VK_END_W,    0x65,           // 0x65
    VK_DOWN_W,   0x66,           // 0x66
    VK_NEXT_W,   0x67,           // 0x67
    VK_INSERT_W, 0x68,           // 0x68
    VK_DELETE_W, 0x69,           // 0x69
    VK_F23_W,    0x6A,           // 0x6A
    VK_F24_W,    0x6B,           // 0x6B
    0x5D, 0x15D,    // 0x6C RWin (PM scan 0x7C)
    0, 0,           // 0x6D
    0x5B, 0x15B,    // 0x6E LWin (PM scan 0x7E)
    0x5C, 0x15C     // 0x6F RMenu? (PM scan 0x7F)
};

static BOOL fGenerateDoubleClick = FALSE;
static MSG  doubleClickMsg = {0};

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
void SetMenuDoubleClick(BOOL fSet)
{
  fGenerateDoubleClick = fSet;
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
  BOOL             fIsFrame = FALSE;
  int i;

    memset(winMsg, 0, sizeof(MSG));
    win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(os2Msg->hwnd);
    if(!win32wnd) {
        win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(os2Msg->hwnd);
        if(win32wnd) {
            fIsFrame = TRUE;
        }
    }

    //PostThreadMessage posts WIN32APP_POSTMSG msg without window handle
    //Realplayer starts a timer with hwnd 0 & proc 0; check this here
    if(win32wnd == 0 && (os2Msg->msg != WM_CREATE && os2Msg->msg != WM_QUIT && os2Msg->msg != WM_TIMER && os2Msg->msg < WIN32APP_POSTMSG))
    {
        goto dummymessage; //not a win32 client window
    }
    winMsg->time = os2Msg->time;
    //CB: PM bug or undocumented feature? ptl.x highword is set!
    winMsg->pt.x = os2Msg->ptl.x & 0xFFFF;
    winMsg->pt.y = mapScreenY(os2Msg->ptl.y);

    if(win32wnd) //==0 for WM_CREATE/WM_QUIT
        winMsg->hwnd = win32wnd->getWindowHandle();

    if(os2Msg->msg >= WIN32APP_POSTMSG) {
        packet = (POSTMSG_PACKET *)os2Msg->mp2;
        if(packet && ((ULONG)os2Msg->mp1 == WIN32MSG_MAGICA || (ULONG)os2Msg->mp1 == WIN32MSG_MAGICW)) {
            winMsg->message = os2Msg->msg - WIN32APP_POSTMSG;
            winMsg->wParam  = packet->wParam;
            winMsg->lParam  = packet->lParam;
            if(fMsgRemoved == MSG_REMOVE) free(packet); //free the shared memory here
            if(win32wnd) RELEASE_WNDOBJ(win32wnd);
            return TRUE;
        }
        else {//broadcasted message (no packet present)
            winMsg->message = os2Msg->msg - WIN32APP_POSTMSG;
            winMsg->wParam  = (UINT)os2Msg->mp1;
            winMsg->lParam  = (DWORD)os2Msg->mp2;
            if(win32wnd) RELEASE_WNDOBJ(win32wnd);
            return TRUE;
        }
        goto dummymessage;
    }

    switch(os2Msg->msg)
    {
    //OS/2 msgs
    case WM_CREATE:
    {
        if(teb->o.odin.newWindow == 0) {
            DebugInt3();
            goto dummymessage;
        }

        win32wnd = (Win32BaseWindow *)teb->o.odin.newWindow;
        win32wnd->addRef();

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

        if(!fIsFrame) goto dummymessage;

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
              OSLibMapSWPtoWINDOWPOS(pswp, &teb->o.odin.wp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                     win32wnd->getOS2WindowHandle());
        }
        else  OSLibMapSWPtoWINDOWPOS(pswp, &teb->o.odin.wp, &swpOld, OSLibQueryScreenHeight(), win32wnd->getOS2WindowHandle());

        if (!win32wnd->CanReceiveSizeMsgs())    goto dummymessage;

        if(pswp->fl & (SWP_MOVE | SWP_SIZE))
        {
                teb->o.odin.wp.hwnd = win32wnd->getWindowHandle();
                if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
                {
                        Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
                        if(wndAfter) {
                              teb->o.odin.wp.hwndInsertAfter = wndAfter->getWindowHandle();
                              RELEASE_WNDOBJ(wndAfter);
                        }
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
        //WM_NC*BUTTON* is posted when the cursor is in a non-client area of the window

        dprintf(("MsgButton %x (%x) %d at (%d,%d) time %x", winMsg->hwnd, os2Msg->hwnd, WINWM_NCLBUTTONDOWN + (os2Msg->msg - WM_BUTTON1DOWN), winMsg->pt.x, winMsg->pt.y, winMsg->time));

        HWND hwnd;

        DisableLogging();
        if(GetCapture() != winMsg->hwnd)
        {
            hwnd = WindowFromPoint(winMsg->pt);
            if(win32wnd->getWindowHandle() != hwnd) {
                RELEASE_WNDOBJ(win32wnd);
                win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
                if(win32wnd == NULL) {
                    DebugInt3();
                    EnableLogging();
                    goto dummymessage;
                }
                winMsg->hwnd = hwnd;
            }
        }

        //if a window is disabled, it's parent receives the mouse messages
        if(!IsWindowEnabled(win32wnd->getWindowHandle())) {
            if(win32wnd->getParent()) {
                Win32BaseWindow *parent = win32wnd->getParent();;
                if(parent) parent->addRef();
                RELEASE_WNDOBJ(win32wnd);
                win32wnd = parent;
            }
            fWasDisabled = TRUE;
        }

        if(IsNCMouseMsg(win32wnd)) {
            winMsg->message = WINWM_NCLBUTTONDOWN + (os2Msg->msg - WM_BUTTON1DOWN);
            winMsg->wParam  = win32wnd->getLastHitTestVal();
            winMsg->lParam  = MAKELONG(winMsg->pt.x, winMsg->pt.y); //screen coordinates
        }
        else {
            ClientPoint.x = winMsg->pt.x;
            ClientPoint.y = winMsg->pt.y;
            MapWindowPoints(0, win32wnd->getWindowHandle(), (LPPOINT)&ClientPoint, 1);
            winMsg->message = WINWM_LBUTTONDOWN + (os2Msg->msg - WM_BUTTON1DOWN);
            winMsg->wParam  = GetMouseKeyState();
            winMsg->lParam  = MAKELONG(ClientPoint.x, ClientPoint.y); //client coordinates
        }
        EnableLogging();
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
            else goto dummymessage; //don't send mouse messages to disabled windows
        }

        DisableLogging();
        if ((winMsg->message == WINWM_LBUTTONDOWN) ||
            (winMsg->message == WINWM_RBUTTONDOWN) ||
            (winMsg->message == WINWM_MBUTTONDOWN) ||
            (winMsg->message == WINWM_NCLBUTTONDOWN) ||
            (winMsg->message == WINWM_NCRBUTTONDOWN) ||
            (winMsg->message == WINWM_NCMBUTTONDOWN))
        {
            if(fGenerateDoubleClick && doubleClickMsg.message == winMsg->message && 
               winMsg->time - doubleClickMsg.time < GetDoubleClickTime() &&
               (abs(winMsg->pt.x - doubleClickMsg.pt.x) < GetSystemMetrics(SM_CXDOUBLECLK_W)/2) &&
               (abs(winMsg->pt.y - doubleClickMsg.pt.y) < GetSystemMetrics(SM_CYDOUBLECLK_W)/2)) 
            {
                 dprintf(("single -> double click"));
                 if(winMsg->message >= WINWM_LBUTTONDOWN) {
                      winMsg->message += (WINWM_LBUTTONDBLCLK - WINWM_LBUTTONDOWN);
                 }
                 else winMsg->message += (WINWM_LBUTTONDBLCLK - WINWM_NCLBUTTONDOWN);
                 doubleClickMsg.message = 0;
            }
            else {
                 dprintf(("save for double click"));
                 doubleClickMsg = *winMsg;
                 if(doubleClickMsg.message >= WINWM_NCLBUTTONDOWN && doubleClickMsg.message <= WINWM_NCMBUTTONDOWN) {
                      doubleClickMsg.message += (WINWM_LBUTTONDOWN - WINWM_NCLBUTTONDOWN);
                 }
            }
        }
        EnableLogging();

        break;
    }

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

        HWND hwnd;

        dprintf2(("WM_NCMOUSEMOVE (%d,%d)", winMsg->pt.x, winMsg->pt.y));
        DisableLogging();
        if(GetCapture() != winMsg->hwnd)
        {
            hwnd = WindowFromPoint(winMsg->pt);
            if(win32wnd->getWindowHandle() != hwnd) {
                RELEASE_WNDOBJ(win32wnd);
                win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
                if(win32wnd == NULL) {
                    DebugInt3();
                    EnableLogging();
                    goto dummymessage;
                }
                winMsg->hwnd = hwnd;
            }
        }

        //if a window is disabled, it's parent receives the mouse messages
        if(!IsWindowEnabled(win32wnd->getWindowHandle())) {
            if(win32wnd->getParent()) {
                Win32BaseWindow *parent = win32wnd->getParent();;
                if(parent) parent->addRef();
                RELEASE_WNDOBJ(win32wnd);
                win32wnd = parent;
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
            ClientPoint.x = winMsg->pt.x;
            ClientPoint.y = winMsg->pt.y;
            MapWindowPoints(0, win32wnd->getWindowHandle(), (LPPOINT)&ClientPoint, 1);

            winMsg->message = WINWM_MOUSEMOVE;
            winMsg->wParam  = GetMouseKeyState();
            winMsg->lParam  = MAKELONG(ClientPoint.x, ClientPoint.y); //client coordinates
        }
        EnableLogging();
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
                else {
                    goto dummymessage; //don't send mouse messages to disabled windows
                }
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
        {
            //FALSE -> keyboard operation = user pressed Alt-F4 -> close app
            //TRUE  -> user clicked on close button -> close window
            if(SHORT2FROMMP(os2Msg->mp2) == FALSE) 
            {
                HWND hwnd = win32wnd->GetTopParent();
                if(win32wnd->getWindowHandle() != hwnd) {
                    RELEASE_WNDOBJ(win32wnd);
                    win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
                    if(win32wnd == NULL) {
                        DebugInt3();
                        goto dummymessage;
                    }
                    winMsg->hwnd = hwnd;
                }
            }
            win32sc = SC_CLOSE_W;
            break;
        }
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
            dprintf(("Unknown/unsupported SC command %d", SHORT1FROMMP(os2Msg->mp1)));
            goto dummymessage;
        }
        winMsg->message= WINWM_SYSCOMMAND;
        winMsg->wParam = (WPARAM)win32sc;
        winMsg->lParam = MAKELONG((USHORT)x, (USHORT)y);
        break;
    }
      
    case WM_CHAR_SPECIAL:
    {
        // @@@PH
        // special char message from the keyboard hook
        dprintf(("PM: WM_CHAR_SPECIAL\n"));
        
        // AltGr is a very, very strange key!
        UCHAR ucPMScanCode = CHAR4FROMMP(os2Msg->mp1);
        switch (ucPMScanCode)
        {
          case PMSCAN_ESC:
            // Note: ESC generates a WM_CHAR under Windows, not under PM
            // so we've got to post it to ourself here!
            // WM_CHAR(0x0000001bh, 000010001h)
            // @@@PH
            break;
          
          case PMSCAN_ALTRIGHT:
          {
            ULONG flags = SHORT1FROMMP(os2Msg->mp1);
            
            // we need very special treatment here for the
            // poor, crippled AltGr key
            
            if (flags & KC_KEYUP)
            {
              // key up
              // 1 - generate a virtual LCONTROL-keypress
              // 2 - send LMENU-keypress (NT emulates ALtGr w/ Ctrl-Alt!)
            }
            else
            {
              // key down:
              // 1 - generate a virtual LCONTROL-keypress
              // 2 - send LMENU-keypress (NT emulates ALtGr w/ Ctrl-AltGr!)
            }
          }
        } /* switch */
    } /* case */
        // NO BREAK! FALLTHRU CASE!
      
    case WM_CHAR:
    {
        ULONG repeatCount=0, virtualKey=0, keyFlags=0, scanCode=0;
        ULONG flags = SHORT1FROMMP(os2Msg->mp1);
        BOOL keyWasPressed, isExtended = FALSE;
        char c;

        teb->o.odin.fTranslated = FALSE;
        repeatCount = CHAR3FROMMP(os2Msg->mp1);
        scanCode = CHAR4FROMMP(os2Msg->mp1);
        keyWasPressed = ((SHORT1FROMMP (os2Msg->mp1) & KC_PREVDOWN) == KC_PREVDOWN);

        dprintf(("PM: WM_CHAR: %x %x rep=%d scancode=%x", SHORT1FROMMP(os2Msg->mp2), SHORT2FROMMP(os2Msg->mp2), repeatCount, scanCode));
        dprintf(("PM: WM_CHAR: hwnd %x flags %x mp1 %x, mp2 %x", win32wnd->getWindowHandle(), flags, os2Msg->mp1, os2Msg->mp2));

        // vitali add begin
        if ( ( SHORT1FROMMP(os2Msg->mp2) & 0x0FF ) == 0x0E0 )
        {
            // an extended key ( arrows, ins, del and so on )
            // get "virtual" scancode from character code because
            // for "regular" keys they are equal
            if(!(flags & (KC_SHIFT|KC_ALT|KC_CTRL))) {
                scanCode = ( SHORT1FROMMP(os2Msg->mp2) >> 8) & 0x0FF;
            }
            isExtended = TRUE;
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

VirtualKeyFound:
//        dprintf (("VIRTUALKEYFOUND:(%x)", virtualKey));

        // Adjust PM scancodes for Win* keys
        if (scanCode >= 0x70)
            scanCode -= 0x10;
        winMsg->wParam  = pmscan2winkey[scanCode][0];
        winMsg->lParam  = repeatCount & 0x0FFFF;                 // bit 0-15, repeatcount
        winMsg->lParam |= (pmscan2winkey[scanCode][1] & 0x1FF) << 16;  // bit 16-23, scancode + bit 15 extended

        // Adjust VKEY value for pad digits if NumLock is on
        if ((scanCode >= 0x47) && (scanCode <= 0x53) &&
            (virtualKey >= 0x30) && (virtualKey >= 39))
            winMsg->wParam = virtualKey + 0x30;

        // Set the extended bit when appropriate
        if (isExtended)
            winMsg->lParam = winMsg->lParam | (1<<24);

        if (!(flags & KC_ALT))
        {
          //
          // the Alt key is not pressed
          // or no more pressed
          //
          if (flags & KC_KEYUP)
          {
            // check for a lonesome ALT key ...
            if ( (flags & KC_LONEKEY) &&
                (winMsg->wParam == VK_LMENU_W) )
            {
              winMsg->message = WINWM_SYSKEYUP;
              
              // held ALT-key when current key is released
              // generates additional flag 0x2000000
              // Note: PM seems to do this differently, 
              // KC_ALT is already reset
            }
            else
            {
              // send WM_KEYUP message
              winMsg->message = WINWM_KEYUP;
            }
            
            winMsg->lParam |= 1 << 30;                              // bit 30, previous state, always 1 for a WM_KEYUP message
            winMsg->lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP
          }
          else 
          {
            // send WM_KEYDOWN message
            winMsg->message = WINWM_KEYDOWN;
            if (keyWasPressed)
              winMsg->lParam |= 1 << 30;                          // bit 30, previous state, 1 means key was pressed
          }
        }
        else 
        {
          //
          // the Alt key is pressed
          //
          if (flags & KC_KEYUP)
          {
            // send WM_SYSKEYUP message
            winMsg->message = WINWM_SYSKEYUP;
            winMsg->lParam |= 1 << 30;                              // bit 30, previous state, always 1 for a WM_KEYUP message
            winMsg->lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP
          }
          else 
          {
            // send WM_SYSKEYDOWN message
            winMsg->message = WINWM_SYSKEYDOWN;
            if (keyWasPressed)
              winMsg->lParam |= 1 << 30;                          // bit 30, previous state, 1 means key was pressed

            // pressed ALT-key generates additional flag 0x2000000
            // if the current window has keyboard focus
            winMsg->lParam |= 1 << 29;
          }
        }
      
      
        if (ISKDB_CAPTURED())
        {
            if (DInputKeyBoardHandler(winMsg)) {
                goto dummymessage; //dinput swallowed message
            }
        }
        break;
    }

    case WM_TIMER:
//Why was this check here????
//        if (os2Msg->mp2)
//        {
          BOOL sys;
          ULONG id;

          if (TIMER_GetTimerInfo(os2Msg->hwnd,(ULONG)os2Msg->mp1,&sys,&id))
          {
                winMsg->wParam = (WPARAM)id;
                winMsg->message= (sys) ? WINWM_SYSTIMER : WINWM_TIMER;
                break;
          }
//        }
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

    case WM_HSCROLL:
    case WM_VSCROLL:
        winMsg->message = (os2Msg->msg == WM_HSCROLL) ? WINWM_HSCROLL : WINWM_VSCROLL;
        winMsg->lParam  = 0;
	winMsg->wParam  = 0;
        switch(SHORT2FROMMP(os2Msg->mp2)) {
        case SB_LINERIGHT:
            winMsg->wParam = SB_LINERIGHT_W;
            break;
        case SB_LINELEFT:
            winMsg->wParam = SB_LINELEFT_W;
            break;
        case SB_PAGELEFT:
            winMsg->wParam = SB_PAGELEFT_W;
            break;
        case SB_PAGERIGHT:
            winMsg->wParam = SB_PAGERIGHT_W;
            break;
        default:
            dprintf(("Unsupported WM_H/VSCROLL message %x!!", SHORT2FROMMP(os2Msg->mp2)));
            goto dummymessage;
        }
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
        dprintf2(("dummy message %x %x %x %x", os2Msg->hwnd, os2Msg->msg, os2Msg->mp1, os2Msg->mp2));
        winMsg->message = 0;
        winMsg->wParam  = 0;
        winMsg->lParam  = 0;
        if(win32wnd) RELEASE_WNDOBJ(win32wnd);
        return FALSE;
    }
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
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
                if(msg->wParam) {
                    if ((msg->wParam >= VK_NUMPAD0_W) && (msg->wParam <= VK_NUMPAD9_W))
                        extramsg.wParam = msg->wParam - 0x30;
                    else
                        extramsg.wParam = msg->wParam;
                }
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

