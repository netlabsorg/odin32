/* $Id: pmwindow.cpp,v 1.62 1999-12-05 00:31:47 sandervl Exp $ */
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

#include <os2wrap.h>
#include <stdlib.h>
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

HMQ  hmq = 0;                             /* Message queue handle         */
HAB  hab = 0;

RECTL desktopRectl = {0};
ULONG ScreenWidth  = 0;
ULONG ScreenHeight = 0;
ULONG ScreenBitsPerPel = 0;

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
               0x0A,    // VK_NEWLINE       0x0A (no VK_* def)
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

MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

//******************************************************************************
//Initialize PM; create hab, message queue and register special Win32 window classes
//******************************************************************************
BOOL InitPM()
{
  CLASSINFO FrameClassInfo;

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
//     CS_SIZEREDRAW | CS_HITTEST | CS_MOVENOTIFY,
     CS_SIZEREDRAW | CS_HITTEST,
     NROF_WIN32WNDBYTES)) {
        dprintf(("WinRegisterClass Win32BaseWindow failed"));
        return(FALSE);
   }
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
 APIRET           rc;

  //Restore our FS selector
  SetWin32TIB();

  win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

  if(msg != WM_CREATE && win32wnd == NULL) {
        dprintf(("Invalid win32wnd pointer for window %x!!", hwnd));
        goto RunDefWndProc;
  }
  if(msg == WIN32APP_POSTMSG && (ULONG)mp1 == WIN32PM_MAGIC) {
        //win32 app user message
        win32wnd->PostMessage((POSTMSG_PACKET *)mp2);
        return (MRESULT)0;
  }
  switch( msg )
  {
    //OS/2 msgs
    case WM_CREATE:
    {
     THDB *thdb = GetThreadTHDB();

        if(thdb == NULL || thdb->newWindow == 0)
            goto createfail;

        //Processing is done in after WinCreateWindow returns
        dprintf(("OS2: WM_CREATE %x", hwnd));
        win32wnd = (Win32BaseWindow *)thdb->newWindow;

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
        if(win32wnd->MsgEnable(SHORT1FROMMP(mp1))) {
                goto RunDefWndProc;
        }
        break;

    case WM_SHOW:
        dprintf(("OS2: WM_SHOW %x %d", hwnd, mp1));
        if(win32wnd->MsgShow((ULONG)mp1)) {
                goto RunDefWndProc;
        }
        break;

#if 1
    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;

        dprintf(("OS2: WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", hwnd, pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
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

#if 1
    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp  = (PSWP)mp1;
      SWP       swpOld = *(pswp + 1);
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE;
      LONG      yDelta = pswp->cy - swpOld.cy;
      LONG      xDelta = pswp->cx - swpOld.cx;

        dprintf(("OS2: WM_WINDOWPOSCHANGED %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0) goto RunDefWndProc;

        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto RunDefWndProc; //parent has just been destroyed
            }
        }
        OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, hParent, win32wnd->getOS2FrameWindowHandle());

        if (!win32wnd->CanReceiveSizeMsgs())    goto RunDefWndProc;

        dprintf(("Set client rectangle to (%d,%d)(%d,%d)", swpOld.x, swpOld.y, swpOld.x + swpOld.cx, swpOld.y + swpOld.cy));
        win32wnd->setClientRect(swpOld.x, swpOld.y, swpOld.x + swpOld.cx, swpOld.y + swpOld.cy);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           wp.hwndInsertAfter = wndAfter->getWindowHandle();
        }
        //SvL: Can be sent twice now (once in pmframe, once here); shouldn't really matter though...
        win32wnd->MsgPosChanged((LPARAM)&wp);

        goto RunDefWndProc;
    }
#endif

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

        if(win32wnd->MsgActivate(SHORT1FROMMP(mp1), fMinimized, Win32BaseWindow::OS2ToWin32Handle(hwndActivate))) {
                goto RunDefWndProc;
        }
        break;
    }

    case WM_SIZE:
    {
        dprintf(("OS2: WM_SIZE (%d,%d) (%d,%d)", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2), SHORT1FROMMP(mp1), SHORT2FROMMP(mp2)));
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

    case WM_FOCUSCHANGE:
        dprintf(("OS2: WM_FOCUSCHANGE %x", win32wnd->getWindowHandle()));
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
                rc = win32wnd->MsgSetFocus(hwndFocusWin32);
        }
        else    rc = win32wnd->MsgKillFocus(Win32BaseWindow::OS2ToWin32Handle(hwndFocus));
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
        win32wnd->MsgMouseMove(keystate, SHORT1FROMMP(mp1), MapOS2ToWin32Y(win32wnd, SHORT2FROMMP(mp1)));
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
        THDB *thdb;
        ULONG repeatCount=0, virtualKey=0, keyFlags=0, scanCode=0;
        ULONG flags = SHORT1FROMMP(mp1);
        BOOL keyWasPressed, fTranslated = FALSE, fRunDefWndProc = FALSE;

        char c;

        repeatCount = CHAR3FROMMP(mp1);
        scanCode = CHAR4FROMMP(mp1);
        keyWasPressed = ((SHORT1FROMMP (mp1) & KC_PREVDOWN) == KC_PREVDOWN);

        dprintf(("PM: WM_CHAR: %x %x %d %x", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2), repeatCount, scanCode));
        dprintf(("PM: WM_CHAR: %x", flags));

        // vitali add begin
        if ( ( SHORT1FROMMP(mp2) & 0x0FF ) == 0x0E0 )
        {
            // an extended key ( arrows, ins, del and so on )
            // get "virtual" scancode from character code cause
            // for "regular" keys they are equal
            scanCode = ( SHORT1FROMMP(mp2) >> 8) & 0x0FF;
        }
        // vitali add end

        // both WM_KEYUP & WM_KEYDOWN want a virtual key, find the right Win32 virtual key
        // given the OS/2 virtual key and OS/2 character

        //if (((SHORT1FROMMP (mp1) & KC_CHAR) == KC_CHAR) ||
        //    ((SHORT1FROMMP (mp1) & KC_LONEKEY) == KC_LONEKEY))
        c = 0;
        if ((SHORT1FROMMP (mp1) & 0xFF) != 0)
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
        dprintf (("VIRTUALKEYFOUND:(%x)", virtualKey));

        if(!(SHORT1FROMMP(mp1) & KC_ALT))
        {
            //
            // the Alt key is not pressed
            //
            if ((flags & KC_KEYUP) == KC_KEYUP) {
                // send WM_KEYUP message

                if(win32wnd->MsgKeyUp (repeatCount, scanCode, virtualKey)) {
                    fRunDefWndProc = TRUE;
                }
            }
            else {
                // send WM_KEYDOWN message
                if (win32wnd->MsgKeyDown (repeatCount, scanCode,
                                          virtualKey, keyWasPressed))
                    fRunDefWndProc = TRUE;
            }
        }
        else {
            //
            // the Alt key is pressed
            //
            if ((flags & KC_KEYUP) == KC_KEYUP) {
                // send WM_SYSKEYUP message

                dprintf(("PMWINDOW_WM_SYSKEYUP: vkey:(%x)", virtualKey));
                if(win32wnd->MsgSysKeyUp (repeatCount, scanCode, virtualKey)) {
                    fRunDefWndProc = TRUE;
                }
            }
            else {
                // send WM_SYSKEYDOWN message
                dprintf(("PMWINDOW_WM_SYSKEYDOWN: vkey:(%x)", virtualKey));
                if (win32wnd->MsgSysKeyDown (repeatCount, scanCode, virtualKey, keyWasPressed))
                    fRunDefWndProc = TRUE;
            }
        }

        thdb = GetThreadTHDB();
        if(thdb) {
            fTranslated = thdb->fMsgTranslated;
            thdb->fMsgTranslated = FALSE;  //reset flag
        }
        //NOTE: These actually need to be posted so that the next message retrieved by GetMessage contains
        //      the newly generated WM_CHAR message.
        if(fTranslated && !((flags & KC_KEYUP) == KC_KEYUP))
        {//TranslatedMessage was called before DispatchMessage, so send WM_CHAR messages
            ULONG keyflags = 0, vkey = 0;
            ULONG fl = SHORT1FROMMP(mp1);
            ULONG chCode = SHORT1FROMMP(mp2);

            if(!(fl & KC_CHAR)) {
//SvL: Test
                break;
//                goto RunDefWndProc;
            }
            if(fl & KC_VIRTUALKEY) {
                if(virtualKey)
                        vkey = virtualKey;
                else    vkey = SHORT2FROMMP(mp2);
                chCode = vkey;
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
            if(win32wnd->MsgChar(chCode, CHAR3FROMMP(mp1), CHAR4FROMMP(mp1), virtualKey, keyflags)) {
//SvL: Test
//                goto RunDefWndProc;

            }
        }
//SvL: Test
//        if(fRunDefWndProc) goto RunDefWndProc;
        break;
    }

    case WM_INITMENU:
    case WM_MENUSELECT:
    case WM_MENUEND:
    case WM_NEXTMENU:
        goto RunDefWndProc;

    case WM_TIMER:
        if (mp2)
        {
          BOOL sys;
          ULONG id;

          dprintf(("OS2: WM_TIMER %x %d", hwnd, mp1));
          if (TIMER_GetTimerInfo(hwnd,(ULONG)mp1,&sys,&id))
          {
            if (sys)
              win32wnd->MsgSysTimer(id);
            else
              win32wnd->MsgTimer(id);
          }
        }
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

    case WM_ERASEBACKGROUND:
    {
        dprintf(("OS2: WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
#if 1
        break;
#else

        if (WinQueryUpdateRect (hwnd, NULL) && !win32wnd->isSupressErase()) {
            BOOL erased = sendEraseBkgnd (win32wnd);
            win32wnd->setEraseBkgnd (!erased, !erased);
        }

        break;
#endif
    }

    case WM_PAINT:
    {
        dprintf(("OS2: WM_PAINT %x", hwnd));

#if 0
        if (WinQueryUpdateRect (hwnd, NULL)) {
            if (!win32wnd->isSupressErase() && win32wnd->isEraseBkgnd()) {
                BOOL erased = sendEraseBkgnd (win32wnd);
                win32wnd->setEraseBkgnd (!erased, !erased);
            }
        }

        win32wnd->setSupressErase (FALSE);
#endif

        win32wnd->MsgPaint(0, 0);

        //validate the update region, WM_PAINT has no return value
        goto RunDefWndProc;
    }

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

    case WM_CONTEXTMENU:
      {
        POINTL pt;

        dprintf(("OS2: WM_CONTEXTMENU %x", hwnd));
        pt.x = (*(POINTS *)&mp1).x;
        pt.y = (*(POINTS *)&mp1).y;
        WinMapWindowPoints(hwnd,HWND_DESKTOP,&pt,1);
        pt.y = WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN)-pt.y-1;
        win32wnd->MsgContextMenu(pt.x,pt.y);

        RestoreOS2TIB();
        return (MRESULT)TRUE;
      }

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
  return (MRESULT)FALSE;

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
