/* $Id: oslibmsgtranslate.cpp,v 1.125 2004-05-24 09:01:59 sandervl Exp $ */
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
#define  INCL_NLS
#define  INCL_GPI
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
#include <winnls.h>
#include <heapstring.h>
#include "hook.h"
#include "user32api.h"

#include <os2im.h>
#include <im32.h>

#define DBG_LOCALLOG    DBG_oslibmsgtranslate
#include "dbglocal.h"

static BOOL fGenerateDoubleClick = FALSE;
static MSG  doubleClickMsg = {0};

extern "C" UINT WINAPI GetACP(void); // from winnls.h
extern "C" UINT WIN32API clipboardPMToOdinFormat(ULONG ulPMFormat);

//For wheel mouse translation
#define WHEEL_DELTA  120
#define OS2_WHEEL_CORRECTION 1
//PF Correction is different for different mouse drivers. For now no correction
//is ok because lots of Odin controls rely on minimum delta. However in future
//we will possibly detect mouse driver and use correction if speed will be
//too high or too low.

//******************************************************************************
//
// setThreadQueueExtraCharMessage: queues WM_CHAR message so it is retrieved
//                                 by GetMessage & PeekMessage
//
// NOTE: WM_CHAR message always in ascii format
//
//******************************************************************************
BOOL setThreadQueueExtraCharMessage(TEB* teb, MSG* pExtraMsg)
{
  if (   teb->o.odin.tidAttachedInputThread
      && OSLibForwardMessageToAttachedThread(teb, pExtraMsg, NULL))
      return TRUE;

  // check if the single slot is occupied already
  if (teb->o.odin.fTranslated == TRUE) {
      // there's still an already translated message to be processed
      dprintf(("WARNING: translated message already pending!"));
      return FALSE;
  }

  teb->o.odin.fTranslated = TRUE;
  memcpy(&teb->o.odin.msgWCHAR, pExtraMsg, sizeof(MSG));
  return TRUE;
}

//******************************************************************************
//******************************************************************************
ULONG ConvertNumPadKey(ULONG pmScan)
{
 ULONG ret;
 BYTE winKey;

  switch (pmScan)
  {
   case PMSCAN_PAD7: ret = PMSCAN_HOME; break;
   case PMSCAN_PAD8: ret = PMSCAN_UP; break;
   case PMSCAN_PAD9: ret = PMSCAN_PAGEUP; break;
   case PMSCAN_PAD4: ret = PMSCAN_LEFT; break;
   case PMSCAN_PAD6: ret = PMSCAN_RIGHT; break;
   case PMSCAN_PAD1: ret = PMSCAN_END; break;
   case PMSCAN_PAD2: ret = PMSCAN_DOWN; break;
   case PMSCAN_PAD3: ret = PMSCAN_PAGEDOWN; break;
   case PMSCAN_PAD0: ret = PMSCAN_INSERT; break;
   case PMSCAN_PADPERIOD: ret = PMSCAN_DELETE; break;
   default:
           ret = pmScan;
  }

  KeyTranslatePMScanToWinVKey(ret, FALSE, (PBYTE)&winKey, NULL, NULL);
  return winKey;

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


/**
 * Inter process/thread packet cleanup.
 * See OSLibPackMessage() for details on the packing.
 *
 * @param   pTeb        Pointer to the thread environment block for the current thread.
 * @param   pPacket     Pointer to the packet in question.
 * @param   pWinMsg     Pointer to the window message corresponding to the packet.
 */
inline void OSLibCleanupPacket(TEB *pTeb, POSTMSG_PACKET *pPacket, MSG *pWinMsg)
{
    switch (pWinMsg->message)
    {
        /*
         * Place this in the TEB freeing any previous WM_COPYDATA packet.
         * Note! Nested WM_COPYDATA isn't working.
         */
        case WINWM_COPYDATA:
        {
            dprintf(("OSLibCleanupPacket: WM_COPYDATA: old %#p  new %#p", pTeb->o.odin.pWM_COPYDATA, pPacket));
            if (pTeb->o.odin.pWM_COPYDATA)
                _sfree(pTeb->o.odin.pWM_COPYDATA);
            pTeb->o.odin.pWM_COPYDATA = pPacket;
            break;
        }

        /*
         * Default packing - free the shared memory here.
         */
        default:
            _sfree(pPacket);
            break;
    }
}

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

    /*
     * Forwarded input (AttachThreadInput()).
     */
    if (    os2Msg->hwnd == NULLHANDLE
        &&  os2Msg->msg == WIN32APP_FORWARDEDPOSTMSG
        &&  os2Msg->mp2 == (MPARAM)WIN32APP_FORWARDEDPOSTMSG_MAGIC
        &&  os2Msg->mp1 != NULL)
    {
        *winMsg = *(MSG*)os2Msg->mp1;
        if (fMsgRemoved)
            _sfree(os2Msg->mp1);
        dprintf(("OS2ToWinMsgTranslate: Received forwarded messaged %x\n", os2Msg->msg));
        return TRUE;
    }

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
            if (fMsgRemoved == MSG_REMOVE)
            {
                /* avoid double free */
                if (os2Msg->mp2)
                {
                    OSLibCleanupPacket(teb, packet, winMsg);
                    os2Msg->mp2 = NULL;
                }
#ifdef DEBUG
                else
                    dprintf(("Trying to free NULL in WinMsgTranslate"));
#endif
            }
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

            // First the low-level mouse hook
            hook.pt          = winMsg->pt;
            hook.mouseData   = 0;  //todo: XBUTTON1/2 (XP feature) or wheel data
            hook.flags       = 0;  //todo: injected (LLMHF_INJECTED)
            hook.time        = winMsg->time;
            hook.dwExtraInfo = 0;

            if(HOOK_CallHooksW( WH_MOUSE_LL, HC_ACTION, msg, (LPARAM)&hook)) {
                goto dummymessage; //hook swallowed message
            }
        }
        MOUSEHOOKSTRUCT mousehk;

        // Now inform the WH_MOUSE hook
        mousehk.pt           = winMsg->pt;
        mousehk.hwnd         = winMsg->hwnd;
        mousehk.wHitTestCode = win32wnd->getLastHitTestVal();
        mousehk.dwExtraInfo  = 0;

        if(HOOK_CallHooksW( WH_MOUSE_W, (fMsgRemoved == MSG_REMOVE) ? HC_ACTION : HC_NOREMOVE, winMsg->message, (LPARAM)&mousehk)) {
            //TODO: WH_CBT HCBT_CLICKSKIPPED
            goto dummymessage; //hook swallowed message
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
        MSLLHOOKSTRUCT hook;
        if(fMsgRemoved == MSG_REMOVE)
        {
            hook.pt          = winMsg->pt;
            hook.mouseData   = 0;
            hook.flags       = 0;  //todo: injected (LLMHF_INJECTED)
            hook.time        = winMsg->time;
            hook.dwExtraInfo = 0;

            if(HOOK_CallHooksW( WH_MOUSE_LL, HC_ACTION, winMsg->message, (LPARAM)&hook)) {
                goto dummymessage; //hook swallowed message
            }
        }
        MOUSEHOOKSTRUCT mousehk;

        // Now inform the WH_MOUSE hook
        mousehk.pt           = winMsg->pt;
        mousehk.hwnd         = winMsg->hwnd;
        mousehk.wHitTestCode = win32wnd->getLastHitTestVal();
        mousehk.dwExtraInfo  = 0;

        if(HOOK_CallHooksW( WH_MOUSE_W, (fMsgRemoved == MSG_REMOVE) ? HC_ACTION : HC_NOREMOVE, winMsg->message, (LPARAM)&mousehk))
        {
            goto dummymessage; //hook swallowed message
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
        case SC_SYSMENU:
            win32sc = SC_KEYMENU_W; //??
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
      BOOL   numPressed = (BOOL)(WinGetKeyState(HWND_DESKTOP,VK_NUMLOCK) & 1);
      char   c;
      USHORT usPMScanCode = CHAR4FROMMP(os2Msg->mp1);

        teb->o.odin.fTranslated = FALSE;
        repeatCount = CHAR3FROMMP(os2Msg->mp1);
        scanCode = CHAR4FROMMP(os2Msg->mp1);
        keyWasPressed = ((SHORT1FROMMP (os2Msg->mp1) & KC_PREVDOWN) == KC_PREVDOWN);

        dprintf(("PM: WM_CHAR: %x %x rep=%d scancode=%x num=%d", SHORT1FROMMP(os2Msg->mp2), SHORT2FROMMP(os2Msg->mp2), repeatCount, scanCode, numPressed));
        dprintf(("PM: WM_CHAR: hwnd %x flags %x mp1 %x, mp2 %x, time=%08xh", win32wnd->getWindowHandle(), flags, os2Msg->mp1, os2Msg->mp2, os2Msg->time));

        BOOL  fWinExtended;
        BYTE  bWinVKey;
        WORD  wWinScan;

        if ( (!IsDBCSEnv() && scanCode == 0) ||
             (scanCode==0 ) && !( flags & KC_CHAR ) )
        {
            goto dummymessage;
        }

        if( scanCode != 0 )
        {
            switch( SHORT2FROMMP( os2Msg->mp2 ))
            {
                // for Korean
                case VK_DBE_HANJA :
                    bWinVKey = 0x19;
                    break;

                case VK_DBE_HANGEUL :
                    bWinVKey = 0x15;
                    break;

                case VK_DBE_JAMO :
                    bWinVKey = 0;
                    break;

                // for Japan
                case VK_DBE_KATAKANA :
                    bWinVKey = 0;
                    break;

                case VK_DBE_HIRAGANA :
                    bWinVKey = 0;
                    break;

                case VK_DBE_SBCSCHAR :
                    bWinVKey = 0;
                    break;

                case VK_DBE_DBCSCHAR :
                    bWinVKey = 0;
                    break;

                case VK_DBE_SBCSDBCSCHAR :
                    bWinVKey = 0;
                    break;

                case VK_DBE_ROMAN :
                    bWinVKey = 0;
                    break;

                // for PRC-Chinese
                case VK_DBE_HANZI :
                    bWinVKey = 0;
                    break;

                // for Taiwan
                case VK_DBE_TSANGJYE :
                    bWinVKey = 0;
                    break;

                case VK_DBE_PHONETIC :
                    bWinVKey = 0;
                    break;

                case VK_DBE_CONV :
                    bWinVKey = 0;
                    break;

                case VK_DBE_NOCONV :
                    bWinVKey = 0;
                    break;

                case VK_DBE_ALPHANUMERIC :
                    switch( GetACP())
                    {
                        case 949 :  // Korea
                        case 1361 :
                            bWinVKey = 0x15;
                            break;

                        case 932 :  // Japan
                        case 942 :
                        case 943 :

                        case 936 :  // PRC
                        case 1381 :

                        case 950 :  // Taiwan
                        default :
                            bWinVKey = 0;
                    }
                    break;

                default :
                    KeyTranslatePMScanToWinVKey(usPMScanCode,
                                                FALSE,
                                                &bWinVKey,
                                                &wWinScan,
                                                &fWinExtended);
            }

            winMsg->wParam = bWinVKey;
        }
        else
        {
            dprintf(("PM: WM_CHAR: DBCS processing "));

            winMsg->wParam = CHAR1FROMMP( os2Msg->mp2 );

            wWinScan = 0;
            fWinExtended = 0;

            if( CHAR2FROMMP( os2Msg->mp2 ))     // DBCS character
            {
                CHAR dbcsCh[ 2 ] = { CHAR1FROMMP( os2Msg->mp2 ), CHAR2FROMMP( os2Msg->mp2 )};

                if( isUnicode )
                {
                    WCHAR  uniChar;

                    MultiByteToWideChar( CP_ACP, 0, dbcsCh, 2, &uniChar, 1 );
                    winMsg->wParam = ( WPARAM )uniChar;
                }
                else
                    winMsg->wParam = ( dbcsCh[ 0 ] << 8 ) | dbcsCh[ 1 ];
            }
        }
        winMsg->lParam  = repeatCount & 0x0FFFF;                 // bit 0-15, repeatcount
        winMsg->lParam |= (wWinScan & 0x1FF) << 16;  // bit 16-23, scancode + bit 15 extended

        // Set the extended bit when appropriate
        if (fWinExtended)
            winMsg->lParam = winMsg->lParam | WIN_KEY_EXTENDED;

        //PF When we press shift we enable non-numeric functions of Numpad
        if ((!numPressed || (flags & KC_SHIFT)) && (scanCode >= PMSCAN_PAD7) && (scanCode <= PMSCAN_PADPERIOD))
             winMsg->wParam = ConvertNumPadKey(scanCode);

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
          else if( scanCode == 0 )
          {
            if( CHAR2FROMMP( os2Msg->mp2 ))
                winMsg->message = WINWM_IME_CHAR;
            else
            {
                //After SetFocus(0), all keystrokes are converted in WM_SYS*
                winMsg->message = (fIgnoreKeystrokes) ? WINWM_SYSCHAR :  WINWM_CHAR;
            }
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
          ULONG id, proc;

          if (TIMER_GetTimerInfo(os2Msg->hwnd,(ULONG)os2Msg->mp1,&sys,&id, &proc))
          {
                winMsg->wParam = (WPARAM)id;
                winMsg->lParam = (LPARAM)proc;
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
        winMsg->wParam  = (UINT)clipboardPMToOdinFormat((ULONG)os2Msg->mp1);
        if (!winMsg->wParam)
        {
            dprintf(("WM_RENDERFMT: failed to convert clipboard format (%d)!!!\n", os2Msg->mp1));
            DebugInt3();
        }
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
                    MOUSEHOOKSTRUCT mousehk;

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

    case WM_IMENOTIFY:
        dprintf(("WM_IMENOTIFY"));

        winMsg->wParam = 0;
        winMsg->lParam = 0;

        switch(( ULONG )os2Msg->mp1 )
        {
            case IMN_STARTCONVERSION :
                winMsg->message = WM_IME_STARTCOMPOSITION_W;
                break;

            case IMN_ENDCONVERSION :
                winMsg->message = WM_IME_ENDCOMPOSITION_W;
                break;

            case IMN_CONVERSIONFULL :
                winMsg->message = WM_IME_COMPOSITIONFULL_W;
                break;

            case IMN_IMECHANGED :
                winMsg->message = WM_IME_SELECT_W;
                // todo
                // fall through

            default :
                goto dummymessage;

        }
        break;

    case WM_IMEREQUEST:
        winMsg->wParam = 0;
        winMsg->lParam = 0;

        switch(( ULONG )os2Msg->mp1 )
        {
            case IMR_INSTANCEACTIVATE :
            {
                USHORT usIMR_IA = LOUSHORT( os2Msg->mp2 );

                winMsg->message = WM_IME_SETCONTEXT_W;
                winMsg->wParam = HIUSHORT( os2Msg->mp2 );

                if( usIMR_IA & IMR_IA_STATUS )
                    winMsg->lParam |= ISC_SHOWUIGUIDELINE_W;

                if( usIMR_IA & IMR_IA_CONVERSION )
                    winMsg->lParam |= ISC_SHOWUICOMPOSITIONWINDOW_W;

                if( usIMR_IA & IMR_IA_CANDIDATE )
                    winMsg->lParam |= ISC_SHOWUIALLCANDIDATEWINDOW_W;

                if( usIMR_IA & IMR_IA_INFOMSG )
                    winMsg->lParam |= 0; // todo

                if( usIMR_IA & IMR_IA_REGWORD )
                    winMsg->lParam |= 0; // todo
                break;
            }

            case IMR_STATUS :
                winMsg->message = WM_IME_NOTIFY_W;

                switch(( ULONG )os2Msg->mp2 )
                {
                    case IMR_STATUS_SHOW :
                        winMsg->wParam = IMN_OPENSTATUSWINDOW_W;
                        break;

                    case IMR_STATUS_HIDE :
                        winMsg->wParam = IMN_CLOSESTATUSWINDOW_W;
                        break;

                    case IMR_STATUS_INPUTMODE : // IMN_SETOPENSTATUS followed by IMN_SETCONVERSIONMODE
                        winMsg->wParam = IMN_SETOPENSTATUS_W;
                        break;

                    case IMR_STATUS_CONVERSIONMODE :
                        winMsg->wParam = IMN_SETSENTENCEMODE_W;
                        break;

                    case IMR_STATUS_STATUSPOS :
                        winMsg->wParam = IMN_SETSTATUSWINDOWPOS_W;
                        break;

                    case IMR_STATUS_STRING :
                    case IMR_STATUS_STRINGATTR :
                    case IMR_STATUS_CURSORPOS :
                    case IMR_STATUS_CURSORATTR :

                    default :
                        // todo
                        goto dummymessage;

                }
                break;

            case IMR_CONVRESULT :
            {
                ULONG ulIMR = ( ULONG )os2Msg->mp2;

                winMsg->message = WM_IME_COMPOSITION_W;
                winMsg->wParam = 0; // todo : current DBCS char

                if( ulIMR & IMR_CONV_CONVERSIONSTRING )
                    winMsg->lParam |= GCS_COMPSTR_W;

                if( ulIMR & IMR_CONV_CONVERSIONATTR )
                    winMsg->lParam |= GCS_COMPATTR_W;

                if( ulIMR & IMR_CONV_CONVERSIONCLAUSE )
                    winMsg->lParam |= GCS_COMPCLAUSE_W;

                if( ulIMR & IMR_CONV_READINGSTRING )
                    winMsg->lParam |= GCS_COMPREADSTR_W;

                if( ulIMR & IMR_CONV_READINGATTR )
                    winMsg->lParam |= GCS_COMPREADATTR_W;

                if( ulIMR & IMR_CONV_READINGCLAUSE )
                    winMsg->lParam |= GCS_COMPREADCLAUSE_W;

                if( ulIMR & IMR_CONV_CURSORPOS )
                    winMsg->lParam |= GCS_CURSORPOS_W;

                if( ulIMR & IMR_CONV_CURSORATTR )
                    winMsg->lParam |= 0; // todo

                if( ulIMR & IMR_CONV_CHANGESTART )
                    winMsg->lParam |= GCS_DELTASTART_W;

                if( ulIMR & IMR_CONV_INSERTCHAR )
                    winMsg->lParam |= CS_INSERTCHAR_W;

                if( ulIMR & IMR_CONV_NOMOVECARET )
                    winMsg->lParam |= CS_NOMOVECARET_W;

                if( ulIMR & IMR_CONV_CONVERSIONFONT )
                    winMsg->lParam |= 0; // todo

                if( ulIMR & IMR_CONV_CONVERSIONPOS )
                    winMsg->lParam |= 0; // todo

                if( ulIMR & IMR_RESULT_RESULTSTRING )
                    winMsg->lParam = GCS_RESULTSTR_W; // clear all composition info

                if( ulIMR & IMR_RESULT_RESULTATTR )
                    winMsg->lParam |= 0; // todo

                if( ulIMR & IMR_RESULT_RESULTCLAUSE )
                    winMsg->lParam |= GCS_RESULTCLAUSE_W;

                if( ulIMR & IMR_RESULT_READINGSTRING )
                    winMsg->lParam |= GCS_RESULTREADSTR_W;

                if( ulIMR & IMR_RESULT_READINGATTR )
                    winMsg->lParam |= 0; // todo

                if( ulIMR & IMR_RESULT_READINGCLAUSE )
                    winMsg->lParam |= GCS_RESULTREADCLAUSE_W;

                if( ulIMR && ( winMsg->lParam == 0 ))
                    goto dummymessage;
                break;
            }

            case IMR_CANDIDATE :
                winMsg->message = WM_IME_NOTIFY_W;
                switch(( ULONG )os2Msg->mp2 )
                {
                    case IMR_CANDIDATE_SHOW :
                    case IMR_CANDIDATE_HIDE :
                    case IMR_CANDIDATE_SELECT :
                    case IMR_CANDIDATE_CHANGE :
                    case IMR_CANDIDATE_CANDIDATEPOS :
                    default :
                        // todo
                        goto dummymessage;
                }
                break;

            case IMR_INFOMSG :
                winMsg->message = WM_IME_NOTIFY_W;
                winMsg->wParam = IMN_GUIDELINE_W;
                break;

            case IMR_REGWORD :
                goto dummymessage;

            case IMR_IMECHANGE :
                winMsg->message = WM_IME_SELECT_W;
                // todo
                goto dummymessage;

            case IMR_CONFIG :
                // todo
                goto dummymessage;

            case IMR_DICTIONARY :
                // todo
                goto dummymessage;

            case IMR_OTHERINFO :
                // todo
                goto dummymessage;
        }
        break;

#if 0 // application cannot receive this message
    case WM_IMECONTROL:
#endif
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
msgdone:
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinTranslateMessage(MSG *msg)
{
  TEB *teb;
  MSG extramsg;
  BOOL   numPressed = (BOOL)(WinGetKeyState(HWND_DESKTOP,VK_NUMLOCK) & 1);
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

      // PF With NumLock off do not generate any WM_CHAR messages at all
      // PADMINUS,PADPLUS fall in range of PAD7..PADPERIOD but they should generate WM_CHAR
      // other PAD(X) buttons miss that range at all and thus generate WM_CHAR
      if (!numPressed && (ucPMScanCode != PMSCAN_PADMINUS) && (ucPMScanCode != PMSCAN_PADPLUS) &&
         (ucPMScanCode >= PMSCAN_PAD7) && (ucPMScanCode <= PMSCAN_PADPERIOD))
        return FALSE;

      if(!(fl & KC_CHAR) && msg->message < WINWM_SYSKEYDOWN)
      {
         return FALSE;
      }

      //the KC_COMPOSITE flag might be set; in that case this key will
      //be combined with the previous dead key, so we must use the scancode
      //(e.g. ^ on german keyboards)
      if((fl & (KC_VIRTUALKEY|KC_COMPOSITE)) == KC_VIRTUALKEY)
      {
        if(!msg->wParam)
        {//TODO: Why is this here????
            DebugInt3();
            extramsg.wParam = SHORT2FROMMP(teb->o.odin.os2msg.mp2);
        }
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

/**
 * Checks if the message is one that should be forwarded.
 *
 * @returns True if the message should be forwarded.
 * @returns False if the message doesn't fall in to that group.
 * @param   pMsg    Message to examin.
 * @remark  Are there more messages???
 */
BOOL OSLibForwardableMessage(const MSG *pMsg)
{
    return (    (pMsg->message >= WINWM_KEYFIRST   && pMsg->message <= WINWM_KEYLAST)
            ||  (pMsg->message >= WINWM_MOUSEFIRST && pMsg->message <= WINWM_MOUSELAST) );
}

/**
 * Forwards this message to the attached thread if it's in the group of messages
 * which is supposed to be forwarded.
 *
 * @returns True if forwarded.
 * @returns False if not forwarded.
 * @param   pTeb    Pointer to the TEB of the current thread.
 * @param   pMsg    Message to forward.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
BOOL OSLibForwardMessageToAttachedThread(void *pvTeb, MSG *pMsg, void *hmm)
{
    TEB *pTeb = (TEB *)pvTeb;
    dprintf(("OSLibForwardMessageToAttachedThread: %p %p (msg=%x)\n", pvTeb, pMsg, pMsg->message));
    if (!OSLibForwardableMessage(pMsg))
        return FALSE;

    /*
     * Find the actual receiver thread.
     */
    int c = 100;
    TEB *pTebTo = pTeb;
    do
    {
        pTebTo = GetTEBFromThreadId(pTebTo->o.odin.tidAttachedInputThread);
    } while (c-- > 0 && !pTebTo && pTebTo->o.odin.tidAttachedInputThread);
    if (!c || !pTebTo)
    {
        if (c)  dprintf(("OSLibForwardMessageToAttachedThread: The receiver thread is dead or non existing.\n"));
        else    dprintf(("OSLibForwardMessageToAttachedThread: threads are attached in looooop.\n"));
        return FALSE; /* hmm.... */
    }
    dprintf(("OSLibForwardMessageToAttachedThread: Forwarding message %#x to %#x\n",
             pMsg->message, pTeb->o.odin.tidAttachedInputThread));

    /*
     * Pack down the message into shared memory.
     */
    MSG *pMsgCopy = (MSG *)_smalloc(sizeof(MSG));
    if (!pMsgCopy)
        return FALSE;
    *pMsgCopy = *pMsg;

    /*
     * Figure out how we should send the message.
     */
    if (WinInSendMsg(pTebTo->o.odin.hab))
    {
#if 0
        /*
         * Hmm what do we do here....
         */
        MRESULT rc = WinSendQueueMsg(pTebTo->o.odin.hmq, /*special! */, pMsgCopy, /*magic*/ );
        /* if (hmmSendMsgResult)
            *hmmSendMsgResult = (???)rc; */
#else
        dprintf(("OSLibForwardMessage: ERROR! %x in sendmsg!!!\n", pMsg->message));
        DebugInt3();

        _sfree(pMsgCopy);
        return FALSE;
#endif
    }
    else
    {
        if (!WinPostQueueMsg(pTebTo->o.odin.hmq, WIN32APP_FORWARDEDPOSTMSG, pMsgCopy, (MPARAM)WIN32APP_FORWARDEDPOSTMSG_MAGIC))
        {
            dprintf(("OSLibForwardMessage: Failed to post queue message to hmq=%#x\n", pTebTo->o.odin.hmq));
            _sfree(pMsgCopy);
        }
    }

    return TRUE;
}
