/* $Id: oslibmsgtranslate.cpp,v 1.102 2003-02-16 18:29:26 sandervl Exp $ */
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
#include <winscan.h>
#include <winkeyboard.h>
#include "hook.h"
#include "user32api.h"

#define DBG_LOCALLOG    DBG_oslibmsgtranslate
#include "dbglocal.h"

static BOOL fGenerateDoubleClick = FALSE;
static MSG  doubleClickMsg = {0};

//For wheel mouse translation
#define WHEEL_DELTA  120
#define OS2_WHEEL_CORRECTION 1
//PF Correction is different for different mouse drivers. For now no correction
//is ok because lots of Odin controls rely on minimum delta. However in future
//we will possibly detect mouse driver and use correction if speed will be 
//too high or too low.
 
//******************************************************************************
//******************************************************************************
BOOL setThreadQueueExtraCharMessage(TEB* teb, MSG* pExtraMsg)
{
  // check if the single slot is occupied already
  if (teb->o.odin.fTranslated == TRUE)
    // there's still an already translated message to be processed
    return FALSE;
  teb->o.odin.fTranslated = TRUE;
  memcpy(&teb->o.odin.msgWCHAR, pExtraMsg, sizeof(MSG));
  return TRUE;
}

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
void OSLibSetMenuDoubleClick(BOOL fSet)
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
        if (fMsgRemoved && win32wnd && (ULONG)os2Msg->mp2 != 0) {
            // mp2 != 0 -> sent by window list; be nice and close
            // the window first
            win32wnd->MsgClose();
        }
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

    case WM_REALIZEPALETTE:
        winMsg->message = WINWM_PALETTECHANGED;
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
            //If SetFocus(0) was called, then the window has already received
            //a WM_KILLFOCUS; don't send another one
            if(!fIgnoreKeystrokes) {
                winMsg->message = WINWM_KILLFOCUS;
                winMsg->wParam  = (WPARAM)hwndFocus;
            }
            else {
                dprintf(("Window has already received a WM_KILLFOCUS (SetFocus(0)); ignore"));
                goto dummymessage;
            }
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

        //if a window is disabled, its parent receives the mouse messages
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
                 if(fMsgRemoved) doubleClickMsg.message = 0;
            }
            else {
                 dprintf(("save for double click"));
                 if(fMsgRemoved) {
                     doubleClickMsg = *winMsg;
                     if(doubleClickMsg.message >= WINWM_NCLBUTTONDOWN && doubleClickMsg.message <= WINWM_NCMBUTTONDOWN) {
                          doubleClickMsg.message += (WINWM_LBUTTONDOWN - WINWM_NCLBUTTONDOWN);
                     }
                 }
            }
        }
        EnableLogging();

        if(fMsgRemoved == MSG_REMOVE)
        {
            MSLLHOOKSTRUCT hook;
            ULONG          msg;

            if(winMsg->message >= WINWM_NCLBUTTONDOWN && winMsg->message <= WINWM_NCMBUTTONDBLCLK) {
                 msg = winMsg->message - WINWM_NCLBUTTONDOWN + WINWM_LBUTTONDOWN;
            }
            else msg = winMsg->message;
            
            if(msg == WINWM_LBUTTONDBLCLK) {
                msg = WINWM_LBUTTONDOWN;
            }
            else
            if(msg == WINWM_RBUTTONDBLCLK) {
                msg = WINWM_RBUTTONDOWN;
            }
            else
            if(msg == WINWM_MBUTTONDBLCLK) {
                msg = WINWM_MBUTTONDOWN;
            }

            hook.pt.x        = os2Msg->ptl.x & 0xFFFF;
            hook.pt.y        = mapScreenY(os2Msg->ptl.y);
            hook.mouseData   = 0;  //todo: XBUTTON1/2 (XP feature) or wheel data
            hook.flags       = 0;  //todo: injected (LLMHF_INJECTED)
            hook.time        = winMsg->time;
            hook.dwExtraInfo = 0;

            if(HOOK_CallHooksW( WH_MOUSE_LL, HC_ACTION, msg, (LPARAM)&hook)) {
                goto dummymessage; //hook swallowed message
            }
        }
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

        dprintf2(("WM_MOUSEMOVE (%d,%d)", winMsg->pt.x, winMsg->pt.y));
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

        //if a window is disabled, its parent receives the mouse messages
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
        if(fWasDisabled) {
            if(win32wnd) {
                winMsg->hwnd = win32wnd->getWindowHandle();
            }
            else {
                goto dummymessage; //don't send mouse messages to disabled windows
            }
        }
        if(fMsgRemoved == MSG_REMOVE)
        {
            MSLLHOOKSTRUCT hook;

            hook.pt.x        = os2Msg->ptl.x & 0xFFFF;
            hook.pt.y        = mapScreenY(os2Msg->ptl.y);
            hook.mouseData   = 0;
            hook.flags       = 0;  //todo: injected (LLMHF_INJECTED)
            hook.time        = winMsg->time;
            hook.dwExtraInfo = 0;

            if(HOOK_CallHooksW( WH_MOUSE_LL, HC_ACTION, winMsg->message, (LPARAM)&hook)) {
                goto dummymessage; //hook swallowed message
            }
        }
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

    case WM_CHAR_SPECIAL_ALTGRCONTROL:
    {
        // special char message from the keyboard hook
        dprintf(("PM: WM_CHAR_SPECIAL_ALTGRCONTROL"));
        
      // NO BREAK! FALLTHRU CASE!
    }
      
    case WM_CHAR_SPECIAL:
    {
        // @@@PH
        // special char message from the keyboard hook
        if(os2Msg->msg == WM_CHAR_SPECIAL) {
            dprintf(("PM: WM_CHAR_SPECIAL"));
        }
        
      // NO BREAK! FALLTHRU CASE!
    }
      
    case WM_CHAR:
    {
      ULONG  repeatCount=0;
      ULONG  virtualKey=0;
      ULONG  keyFlags=0;
      USHORT scanCode=0;
      ULONG  flags = SHORT1FROMMP(os2Msg->mp1);
      BOOL   keyWasPressed;
      char   c;
      USHORT usPMScanCode = CHAR4FROMMP(os2Msg->mp1);

        teb->o.odin.fTranslated = FALSE;
        repeatCount = CHAR3FROMMP(os2Msg->mp1);
        scanCode = CHAR4FROMMP(os2Msg->mp1);
        keyWasPressed = ((SHORT1FROMMP (os2Msg->mp1) & KC_PREVDOWN) == KC_PREVDOWN);

        dprintf(("PM: WM_CHAR: %x %x rep=%d scancode=%x", SHORT1FROMMP(os2Msg->mp2), SHORT2FROMMP(os2Msg->mp2), repeatCount, scanCode));
        dprintf(("PM: WM_CHAR: hwnd %x flags %x mp1 %x, mp2 %x, time=%08xh", win32wnd->getWindowHandle(), flags, os2Msg->mp1, os2Msg->mp2, os2Msg->time));
       
        BOOL  fWinExtended;
        BYTE  bWinVKey;
        WORD  wWinScan;

        if (scanCode==0) goto dummymessage;

        // Note: Numlock-state currently ignored, see below
        KeyTranslatePMScanToWinVKey(usPMScanCode,
                                    FALSE, 
                                    &bWinVKey,
                                    &wWinScan,
                                    &fWinExtended);
        winMsg->wParam = bWinVKey;
        winMsg->lParam  = repeatCount & 0x0FFFF;                 // bit 0-15, repeatcount
        winMsg->lParam |= (wWinScan & 0x1FF) << 16;  // bit 16-23, scancode + bit 15 extended
      
        // Set the extended bit when appropriate
        if (fWinExtended)
            winMsg->lParam = winMsg->lParam | WIN_KEY_EXTENDED;

#if 0
//TODO
        // Adjust VKEY value for pad digits if NumLock is on
        if ((scanCode >= 0x47) && (scanCode <= 0x53) &&
            (virtualKey >= 0x30) && (virtualKey >= 39))
            winMsg->wParam = virtualKey + 0x30;
#endif


        //@PF This looks ugly but this is just what we have in win32 both in win98/win2k
        //what happens is that lParam is tweaked in win32 to contain some illegal codes
        //I simply reproduce here all situation. Absolute values can be kept because
        //Break scancode can be acheived only by pressing Ctrl-Break combination
        if ((usPMScanCode == PMSCAN_BREAK) && !(flags & KC_KEYUP) && (flags & KC_CTRL)
                && (fMsgRemoved && !(teb->o.odin.fTranslated)))
        {
            MSG extramsg;
            memcpy(&extramsg, winMsg, sizeof(MSG));
            // adjust our WM_CHAR code
            extramsg.lParam = 0x01460001;

            //After SetFocus(0), all keystrokes are converted in WM_SYS*
            extramsg.message = (fIgnoreKeystrokes) ? WINWM_SYSCHAR : WINWM_CHAR;

            setThreadQueueExtraCharMessage(teb, &extramsg);
            // and finally adjust our WM_KEYDOWN code
            winMsg->lParam = 0x01460001;
        }
      
        if (!(flags & KC_ALT))
        {
          //
          // the Alt key is not pressed
          // or no more pressed
          //
          if (flags & KC_KEYUP)
          {
            // check for a lonesome ALT key ...
            // SvL: Only Left Alt; AltGr generates a WM_KEYUP when released
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
            winMsg->lParam |= WIN_KEY_PREVSTATE;                    // bit 30, previous state, always 1 for a WM_KEYUP message
            winMsg->lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP
          }
          else 
          { // send WM_KEYDOWN message
            winMsg->message = WINWM_KEYDOWN;

            if (keyWasPressed)
              winMsg->lParam |= WIN_KEY_PREVSTATE;                  // bit 30, previous state, 1 means key was pressed
             
            //Shift-Enter and possibly others need to have special handling
            if (flags & KC_SHIFT)
            {
                if(fMsgRemoved && !(teb->o.odin.fTranslated))
                {                    
                  dprintf(("PM: KC_SHIFT: %x",winMsg->wParam));
                  if (winMsg->wParam == VK_RETURN_W)
                  {
                    MSG extramsg;
                    memcpy(&extramsg, winMsg, sizeof(MSG));
    
                    //After SetFocus(0), all keystrokes are converted in WM_SYS*
                    extramsg.message = (fIgnoreKeystrokes) ? WINWM_SYSCHAR : WINWM_CHAR;

                    // insert message into the queue
                    setThreadQueueExtraCharMessage(teb, &extramsg);
                    winMsg->lParam &= 0x3FFFFFFF;
                  }
               } // else ???
            } // KC_SHIFT
            else
            {
              // in case we handle Enter directly through PMKBDHOOK
              if ((os2Msg->msg == WM_CHAR_SPECIAL) && (winMsg->wParam == VK_RETURN_W)
                   && (fMsgRemoved && !(teb->o.odin.fTranslated)))
              {
                    MSG extramsg;
                    memcpy(&extramsg, winMsg, sizeof(MSG));
    
                    //After SetFocus(0), all keystrokes are converted in WM_SYS*
                    extramsg.message = (fIgnoreKeystrokes) ? WINWM_SYSCHAR : WINWM_CHAR;

                    // insert message into the queue
                    setThreadQueueExtraCharMessage(teb, &extramsg);
              }
            }
          }
          // if right alt is down, then we need to set the alt down bit too
          // except for the fake Ctrl WM_CHAR sent for AltGr emulation
          if (os2Msg->msg != WM_CHAR_SPECIAL_ALTGRCONTROL && 
              (WinGetKeyState(HWND_DESKTOP, VK_ALTGRAF) & 0x8000)) 
          {
              winMsg->lParam |= WIN_KEY_ALTHELD;            
          }
        }
        else 
        {
          //
          // the Alt key is pressed
          //
          if (flags & KC_KEYUP)
          {
             //@@PF Note that without pmkbdhook there will not be correct message for Alt-Enter
              winMsg->message = WINWM_SYSKEYUP;
              winMsg->lParam |= WIN_KEY_PREVSTATE;            
              // No ALTHELD for Alt itself ;)
              winMsg->lParam |= WIN_KEY_ALTHELD;            
	      winMsg->lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP 
          }
          else 
          {
            // send WM_SYSKEYDOWN message
            winMsg->message = WINWM_SYSKEYDOWN;
            if (keyWasPressed)
              winMsg->lParam |= WIN_KEY_PREVSTATE;                  // bit 30, previous state, 1 means key was pressed

            // pressed ALT-key generates additional flag 0x2000000
            // if the current window has keyboard focus
            winMsg->lParam |= WIN_KEY_ALTHELD;
          }
        }
#if 0
        //
        // AltGr needs special handling
        //
        // AltGr -> WM_KEYDOWN (VK_CONTROL), WM_KEYDOWN (VK_MENU) 
        //          WM_SYSKEYUP (VK_CONTROL)
        //          WM_KEYUP (VK_MENU)
        //
        // Ctrl+AltGr -> WM_KEYDOWN (VK_CONTROL), WM_KEYUP (VK_CONTROL) 
        //               WM_KEYDOWN (VK_MENU) 
        //               WM_KEYUP (VK_MENU)
        //               WM_KEYUP (VK_CONTROL) 
        //
        // AltGr+Ctrl -> WM_KEYDOWN (VK_CONTROL), WM_KEYDOWN (VK_MENU) 
        //               WM_KEYDOWN (VK_CONTROL) 
        //               WM_SYSKEYUP (VK_CONTROL) 
        //               WM_SYSKEYUP (VK_CONTROL) 
        //               WM_KEYUP (VK_MENU)
        //
        // AltGr down -> if Ctrl down, send WM_KEYUP (VK_CONTROL)
        //               endif
        //               Send WM_KEYDOWN (VK_CONTROL)
        //               Send WM_KEYDOWN (VK_MENU)
        // AltGr up ->   if !(Ctrl down before AltGr was pressed || Ctrl up)
        //                   Send WM_SYSKEYUP (VK_CONTROL)
        //               endif
        //               Send WM_KEYDOWN (VK_MENU)
        // 
        if(winMsg->wParam == VK_MENU_W && (winMsg->lParam & WIN_KEY_EXTENDED)) 
        {//AltGr
            if(GetKeyState(VK_CONTROL_W) & 0x8000) 
            {//Ctrl key pressed, send WM_KEYUP
            
            }
        }
#endif

        //After SetFocus(0), all keystrokes are converted in WM_SYS*
        if(fIgnoreKeystrokes) {
            if(winMsg->message == WINWM_KEYDOWN) {
                winMsg->message = WINWM_SYSKEYDOWN;
            }
            else 
            if(winMsg->message == WINWM_KEYUP) {
                winMsg->message = WINWM_SYSKEYUP;
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

    case WM_DRAWCLIPBOARD:
        winMsg->message = WINWM_DRAWCLIPBOARD;
        break;

    case WM_HSCROLL:
    case WM_VSCROLL:
        //PF For win32 we support only vertical scrolling for WM_MOUSEWHEEL
        if (os2Msg->msg == WM_VSCROLL)
        {
	    POINT CursorPoint;
            winMsg->message = WINWM_MOUSEWHEEL;
            if (OSLibWinQueryPointerPos(&CursorPoint))
               mapScreenPoint((OSLIBPOINT*)&CursorPoint);
 
            if (SHORT2FROMMP(os2Msg->mp2) == SB_LINEDOWN)
                winMsg->wParam  = MAKELONG(GetMouseKeyState(), -WHEEL_DELTA/OS2_WHEEL_CORRECTION);  
            else
            if (SHORT2FROMMP(os2Msg->mp2) == SB_LINEUP)
                winMsg->wParam  = MAKELONG(GetMouseKeyState(), WHEEL_DELTA/OS2_WHEEL_CORRECTION);  
            else
                winMsg->wParam  = MAKELONG(GetMouseKeyState(), 0);  

            winMsg->lParam  = MAKELONG(CursorPoint.x, CursorPoint.y); 

            dprintf(("WM_MOUSEWHEEL message delta %d at (%d,%d)",HIWORD(winMsg->wParam),CursorPoint.x, CursorPoint.y));  
            if (fMsgRemoved == MSG_REMOVE)
            {
	            MSLLHOOKSTRUCT hook;

	            hook.pt.x       = os2Msg->ptl.x & 0xFFFF;
	            hook.pt.y       = mapScreenY(os2Msg->ptl.y);
	            if (SHORT2FROMMP(os2Msg->mp2) == SB_LINEDOWN)
	                hook.mouseData   = MAKELONG(GetMouseKeyState(), -WHEEL_DELTA/OS2_WHEEL_CORRECTION);  
	            else
	            if (SHORT2FROMMP(os2Msg->mp2) == SB_LINEUP)
	                hook.mouseData   = MAKELONG(GetMouseKeyState(), WHEEL_DELTA/OS2_WHEEL_CORRECTION);  
	            else goto dummymessage; // IBM driver produces other messages as well sometimes

	            hook.flags       = LLMHF_INJECTED;
	            hook.time        = winMsg->time;
	            hook.dwExtraInfo = 0;
	            if(HOOK_CallHooksW( WH_MOUSE_LL, HC_ACTION, WINWM_MOUSEWHEEL, (LPARAM)&hook))
	                goto dummymessage; //hook swallowed message
	   }
           break;
        } 
        goto dummymessage; //eat this message
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
  MSG extramsg;

  teb = GetThreadTEB();
  if(!teb)
    return FALSE;

  UCHAR ucPMScanCode = CHAR4FROMMP(teb->o.odin.os2msg.mp1);
  ULONG fl = SHORT1FROMMP(teb->o.odin.os2msg.mp1);
 
  
    //NOTE: These actually need to be posted so that the next message retrieved by GetMessage contains
    //      the newly generated WM_CHAR message.
    if(!teb->o.odin.fTranslated &&
       teb->o.odin.os2msg.msg == WM_CHAR && 
       !((SHORT1FROMMP(teb->o.odin.os2msg.mp1) & KC_KEYUP) == KC_KEYUP))
    {
      //TranslatedMessage was called before DispatchMessage, so queue WM_CHAR message
      memcpy(&extramsg, msg, sizeof(MSG));
      extramsg.wParam = SHORT1FROMMP(teb->o.odin.os2msg.mp2);
      extramsg.lParam = 0;

      // ESCAPE generates a WM_CHAR under windows, so take
      // special care for this here.
      switch (ucPMScanCode)
      {
        case PMSCAN_ESC:
          extramsg.wParam  = VK_ESCAPE_W;
          fl |= KC_CHAR;
        break;
      }

      if(!(fl & KC_CHAR) && msg->message < WINWM_SYSKEYDOWN) 
      {
        return FALSE;
      }

      //the KC_COMPOSITE flag might be set; in that case this key will
      //be combined with the previous dead key, so we must use the scancode
      //(e.g. ^ on german keyboards)
      if(fl & (KC_VIRTUALKEY|KC_COMPOSITE) == KC_VIRTUALKEY)
      {
        if(msg->wParam)
        {
          if ((msg->wParam >= VK_NUMPAD0_W) && 
              (msg->wParam <= VK_NUMPAD9_W))
            extramsg.wParam = msg->wParam - 0x30;
          else
            extramsg.wParam = msg->wParam;
        }
        else    
          extramsg.wParam = SHORT2FROMMP(teb->o.odin.os2msg.mp2);
      }


      //After SetFocus(0), all keystrokes are converted in WM_SYS*
      if(msg->message >= WINWM_SYSKEYDOWN || fIgnoreKeystrokes) 
        extramsg.message = WINWM_SYSCHAR;
      else    
        extramsg.message = WINWM_CHAR;

      if(fl & KC_DEADKEY)
        extramsg.message++;  //WM_DEADCHAR/WM_SYSDEADCHAR


      extramsg.lParam = msg->lParam & 0x00FFFFFF;
      if ((fl & KC_ALT) || (msg->lParam & WIN_KEY_ALTHELD))
        extramsg.lParam |= WIN_KEY_ALTHELD;
      if(fl & KC_PREVDOWN)
        extramsg.lParam |= WIN_KEY_PREVSTATE;
      if(fl & KC_KEYUP)
        extramsg.lParam |= (1<<31);

      // insert message into the queue
      setThreadQueueExtraCharMessage(teb, &extramsg);
      return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************

