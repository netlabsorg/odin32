/* $Id: pmwindow.cpp,v 1.145 2001-09-15 15:23:12 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
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
#define INCL_DOSMODULEMGR
#define INCL_WINTRACKRECT

#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <winuser32.h>
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
#define INCLUDED_BY_DC
#include "dc.h"
#include <thread.h>
#include <wprocess.h>
#include "caret.h"
#include "timer.h"
#include <codepage.h>
#include "syscolor.h"
#include "options.h"


#define DBG_LOCALLOG    DBG_pmwindow
#include "dbglocal.h"

//define this to use the new code for WM_CALCVALIDRECT handling
//#define USE_CALCVALIDRECT

HMQ     hmq = 0;                             /* Message queue handle         */
HAB     hab = 0;
RECTL   desktopRectl = {0};
ULONG   ScreenWidth  = 0;
ULONG   ScreenHeight = 0;
ULONG   ScreenBitsPerPel = 0;
BOOL    fOS2Look = FALSE;
HBITMAP hbmFrameMenu[3] = {0};

static PFNWP pfnFrameWndProc = NULL;

MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY Win32FrameWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void FrameReplaceMenuItem(HWND hwndMenu, ULONG nIndex, ULONG idOld, ULONG   idNew,
                          HBITMAP hbmNew);

VOID APIENTRY DspInitSystemDriverName(PSZ pszDriverName, ULONG lenDriverName);

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
                PTIB ptib;
                PPIB ppib;

                DosGetInfoBlocks(&ptib, &ppib);

                hmq = WinQueueFromID(hab, ppib->pib_ulpid, ptib->tib_ptib2->tib2_ultid);
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
        0,
        NROF_WIN32WNDBYTES))
    {
            dprintf(("WinRegisterClass Win32BaseWindow failed"));
            return(FALSE);
    }

    CLASSINFO FrameClassInfo;
    if(!WinQueryClassInfo (hab, WC_FRAME, &FrameClassInfo)) {
        dprintf (("WinQueryClassInfo WC_FRAME failed"));
        return (FALSE);
    }
    pfnFrameWndProc = FrameClassInfo.pfnWindowProc;

    dprintf(("WC_FRAME style %x", FrameClassInfo.flClassStyle));

    if(!WinRegisterClass(                 /* Register window class        */
        hab,                               /* Anchor block handle          */
        (PSZ)WIN32_STDFRAMECLASS,          /* Window class name            */
        (PFNWP)Win32FrameWindowProc,       /* Address of window procedure  */
        CS_FRAME,
        FrameClassInfo.cbWindowData))
    {
        dprintf(("WinRegisterClass Win32BaseWindow failed %x", WinGetLastError(hab)));
        return(FALSE);
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

    fOS2Look = PROFILE_GetOdinIniBool(ODINSYSTEM_SECTION, "OS2Look", FALSE);
    if(fOS2Look) 
    {
        CHAR szDisplay[30];
        HMODULE hModDisplay;

        SYSCOLOR_Init(FALSE); //use OS/2 colors

        DspInitSystemDriverName(szDisplay, sizeof(szDisplay));
        DosQueryModuleHandle(szDisplay, &hModDisplay);

        hbmFrameMenu[0] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MINBUTTON, 0, 0);
        hbmFrameMenu[1] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MAXBUTTON, 0, 0);
        hbmFrameMenu[2] = GpiLoadBitmap(hdc, hModDisplay, SBMP_RESTOREBUTTON, 0, 0);
    }

    DevQueryCaps(hdc, CAPS_COLOR_BITCOUNT, 1, (PLONG)&ScreenBitsPerPel);
    DevCloseDC(hdc);

    dprintf(("InitPM: Desktop (%d,%d) bpp %d", ScreenWidth, ScreenHeight, ScreenBitsPerPel));
    return TRUE;
} /* End of main */
//******************************************************************************
//menu.cpp
BOOL MENU_Init();
//******************************************************************************
void WIN32API SetWindowAppearance(BOOL fLooks)
{
    if(fLooks) {
        CHAR szDisplay[30];
        HMODULE hModDisplay;

        SYSCOLOR_Init(FALSE); //use OS/2 colors

        if(hbmFrameMenu[0] == 0) 
        {
            CHAR szDisplay[30];
            HMODULE hModDisplay;
            HDC   hdc;              /* Device-context handle                */
            DEVOPENSTRUC dop = {NULL, "DISPLAY", NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL};

            /* create memory device context */
            hdc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);

            DspInitSystemDriverName(szDisplay, sizeof(szDisplay));
            DosQueryModuleHandle(szDisplay, &hModDisplay);

            hbmFrameMenu[0] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MINBUTTON, 0, 0);
            hbmFrameMenu[1] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MAXBUTTON, 0, 0);
            hbmFrameMenu[2] = GpiLoadBitmap(hdc, hModDisplay, SBMP_RESTOREBUTTON, 0, 0);
            DevCloseDC(hdc);
        }
    }
    fOS2Look = fLooks;
    MENU_Init();
}
//******************************************************************************
//Win32 window message handler
//******************************************************************************
MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 Win32BaseWindow *win32wnd;
 TEB             *teb;
 MSG              winMsg, *pWinMsg;
 MRESULT          rc = 0;
 POSTMSG_PACKET  *postmsg;
 OSLIBPOINT       point, ClientPoint;

    //Restore our FS selector
    SetWin32TIB();

    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- BEGIN
    teb = GetThreadTEB();
    win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

////    dprintf(("window %x msg %x", (win32wnd) ? win32wnd->getWindowHandle() : 0, msg));

    if(!teb || (msg != WM_CREATE && win32wnd == NULL)) {
        dprintf(("OS2: Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
        goto RunDefWndProc;
    }
////    if(teb->o.odin.fIgnoreMsgs) {
////        goto RunDefWndProc;
////    }

    if((teb->o.odin.msgstate & 1) == 0)
    {//message that was sent directly to our window proc handler; translate it here
        QMSG qmsg;

        qmsg.msg  = msg;
        qmsg.hwnd = hwnd;
        qmsg.mp1  = mp1;
        qmsg.mp2  = mp2;
        qmsg.time = WinQueryMsgTime(teb->o.odin.hab);
        WinQueryMsgPos(teb->o.odin.hab, &qmsg.ptl);
        qmsg.reserved = 0;

        if(OS2ToWinMsgTranslate((PVOID)teb, &qmsg, &winMsg, FALSE, MSG_REMOVE) == FALSE)
        {//message was not translated
            memset(&winMsg, 0, sizeof(MSG));
        }
        pWinMsg = &winMsg;
    }
    else {
        pWinMsg = &teb->o.odin.msg;
        teb->o.odin.msgstate++;
    }
    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- END

    if(msg >= WIN32APP_POSTMSG) {
        //probably win32 app user message
        dprintf2(("Posted message %x->%x", msg, msg-WIN32APP_POSTMSG));
        if((ULONG)mp1 == WIN32MSG_MAGICA) {
            rc = (MRESULT)win32wnd->DispatchMsgA(pWinMsg);
        }
        else
        if((ULONG)mp1 == WIN32MSG_MAGICW) {
            rc = (MRESULT)win32wnd->DispatchMsgW(pWinMsg);
        }
        else {//broadcasted message
            rc = (MRESULT)win32wnd->DispatchMsgA(pWinMsg);
        }
        RELEASE_WNDOBJ(win32wnd);
        RestoreOS2TIB();
        return rc;
    }

    switch( msg )
    {
    //OS/2 msgs
    case WM_CREATE:
    {
        if(teb->o.odin.newWindow == 0)
            goto createfail;

        //Processing is done in after WinCreateWindow returns
        dprintf(("OS2: WM_CREATE %x", hwnd));
        win32wnd = (Win32BaseWindow *)teb->o.odin.newWindow;
        win32wnd->addRef();
        teb->o.odin.newWindow = 0;
        if(win32wnd->MsgCreate(hwnd) == FALSE)
        {
            rc = (MRESULT)TRUE; //discontinue window creation
            break;
        }
    createfail:
        rc = (MRESULT)FALSE;
        break;
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
        WinSetVisibleRegionNotify(hwnd, FALSE);
        goto RunDefWndProc;

    case WM_ENABLE:
        dprintf(("OS2: WM_ENABLE %x", hwnd));
        break;

    case WM_SHOW:
        dprintf(("OS2: WM_SHOW %x %d", hwnd, mp1));
        win32wnd->MsgShow((ULONG)mp1);
        break;

    case WM_ACTIVATE:
    {
        ULONG flags = WinQueryWindowULong(hwnd, OFFSET_WIN32FLAGS);

        dprintf(("OS2: WM_ACTIVATE %x %x %x", hwnd, mp1, mp2));
        WinSetWindowULong(hwnd, OFFSET_WIN32FLAGS, SHORT1FROMMP(mp1) ? (flags | WINDOWFLAG_ACTIVE):(flags & ~WINDOWFLAG_ACTIVE));
        if(win32wnd->IsWindowCreated())
        {
            win32wnd->MsgActivate((LOWORD(pWinMsg->wParam) == WA_ACTIVE_W) ? 1 : 0, HIWORD(pWinMsg->wParam), pWinMsg->lParam, (HWND)mp2);
        }
        break;
    }

    case WM_SIZE:
    {
        dprintf(("OS2: WM_SIZE (%d,%d) (%d,%d)", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2), SHORT1FROMMP(mp1), SHORT2FROMMP(mp1)));
        win32wnd->SetVisibleRegionChanged(TRUE);
        goto RunDefWndProc;
    }


    case WM_VRNENABLED:
        dprintf(("OS2: WM_VRNENABLED %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        if(!win32wnd->isComingToTop() && ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == WS_EX_TOPMOST_W))
        {
            HWND hwndrelated;
            Win32BaseWindow *topwindow;

            win32wnd->setComingToTop(TRUE);

            hwndrelated = WinQueryWindow(hwnd, QW_PREV);
            dprintf(("WM_VRNENABLED hwndrelated = %x (hwnd=%x)", hwndrelated, hwnd));
            topwindow = Win32BaseWindow::GetWindowFromOS2Handle(hwndrelated);
            if(topwindow == NULL || ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == 0)) {
                //put window at the top of z order
                WinSetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
            }
            if(topwindow) RELEASE_WNDOBJ(topwindow);

            win32wnd->setComingToTop(FALSE);
            break;
        }
        goto RunDefWndProc;

    case WM_VRNDISABLED:
        dprintf(("OS2: WM_VRNDISABLED %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        goto RunDefWndProc;

    case WIN32APP_SETFOCUSMSG:
        //PM doesn't allow SetFocus calls during WM_SETFOCUS message processing;
        //must delay this function call
        //mp1 = win32 window handle
        //mp2 = top parent if activation required
        dprintf(("USER32: Delayed SetFocus %x %x %x call!", teb->o.odin.hwndFocus, mp1, mp2));
        if(teb->o.odin.hwndFocus) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(teb->o.odin.hwndFocus);
            if(win32wnd) {
                 if(mp2) {
                    SetActiveWindow((HWND)mp2);
                 }
                 if(!IsWindow(win32wnd->getWindowHandle())) break;       //abort if window destroyed
                 WinFocusChange(HWND_DESKTOP, win32wnd->getOS2WindowHandle(), FC_NOSETACTIVE);
            }
            else DebugInt3();
        }
        break;

    case WM_SETFOCUS:
    {
      HWND hwndFocus = (HWND)mp1;

        dprintf(("OS2: WM_SETFOCUS %x %x (%x) %d", win32wnd->getWindowHandle(), mp1, OS2ToWin32Handle(hwndFocus), mp2));

        //PM doesn't allow SetFocus calls during WM_SETFOCUS message processing;
        //must delay this function call

        teb->o.odin.fWM_SETFOCUS = TRUE;
        teb->o.odin.hwndFocus    = 0;
        if(WinQueryWindowULong(hwndFocus, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC)
        {
                //another (non-win32) application's window
                //set to NULL (allowed according to win32 SDK) to avoid problems
                hwndFocus = NULL;
        }
        if((ULONG)mp2 == TRUE) {
                HWND hwndFocusWin32 = OS2ToWin32Handle(hwndFocus);
                recreateCaret (hwndFocusWin32);
                win32wnd->MsgSetFocus(hwndFocusWin32);
        }
        else win32wnd->MsgKillFocus(OS2ToWin32Handle(hwndFocus));
        teb->o.odin.fWM_SETFOCUS = FALSE;

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
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgButton(pWinMsg);

        rc = (MRESULT)TRUE;
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
        rc = (MRESULT)TRUE;
        break;

    case WM_MOUSEMOVE:
    {
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
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
        dprintf(("OS2: WM_SYSCOMMAND %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_RENDERFMT:
    case WM_RENDERALLFMTS:
    case WM_DESTROYCLIPBOARD:
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_CHAR:
        dprintf(("OS2: WM_CHAR %x %x %x, %x %x", win32wnd->getWindowHandle(), mp1, mp2, pWinMsg->wParam, pWinMsg->lParam));
        win32wnd->MsgChar(pWinMsg);
        break;

    case WM_TIMER:
        dprintf(("OS2: WM_TIMER %x %x time %x", win32wnd->getWindowHandle(), pWinMsg->wParam, GetTickCount()));
        win32wnd->DispatchMsgA(pWinMsg);
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
            if(wndpars->fsStatus & WPM_TEXT)
                win32wnd->MsgGetText(wndpars->pszText, wndpars->cchText);
            if(wndpars->fsStatus & WPM_CCHTEXT)
                wndpars->cchText = win32wnd->MsgGetTextLength();

            wndpars->fsStatus = 0;
            wndpars->cbCtlData = 0;
            wndpars->cbPresParams = 0;
            rc = (MRESULT)TRUE;
            break;
        }
        goto RunDefWndProc;
    }

    case WM_PAINT:
    {
      RECTL rectl;
      BOOL  rc;

        rc = WinQueryUpdateRect(hwnd, &rectl);
        dprintf(("OS2: WM_PAINT %x (%d,%d) (%d,%d) rc=%d", win32wnd->getWindowHandle(), rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop, rc));

        if(rc && win32wnd->IsWindowCreated() && (rectl.xLeft != rectl.xRight &&
           rectl.yBottom != rectl.yTop))
        {
                win32wnd->DispatchMsgA(pWinMsg);
        }
        else    goto RunDefWndProc;
        break;
    }

    case WM_ERASEBACKGROUND:
    {
        dprintf(("OS2: WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)FALSE;
        break;
    }

    case WM_CALCVALIDRECTS:
        dprintf(("OS2: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)(CVR_ALIGNLEFT | CVR_ALIGNTOP);
        break;

    case WM_REALIZEPALETTE:
    {
        dprintf(("OS2: WM_REALIZEPALETTE"));
        goto RunDefWndProc;
    }

    case WM_HSCROLL:
    case WM_VSCROLL:
        dprintf(("OS2: %s %x %x %x", (msg == WM_HSCROLL) ? "WM_HSCROLL" : "WM_VSCROLL", win32wnd->getWindowHandle(), mp1, mp2));
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_DDE_INITIATE:
    case WM_DDE_INITIATEACK:
    case WM_DDE_REQUEST:
    case WM_DDE_ACK:
    case WM_DDE_DATA:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
    case WM_DDE_TERMINATE:
        dprintf(("OS2: WM_DDE %x %x", msg, win32wnd->getWindowHandle()));
        goto RunDefWndProc;

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
        dprintf2(("OS2: RunDefWndProc hwnd %x msg %x mp1 %x mp2 %x", hwnd, msg, mp1, mp2));
        goto RunDefWndProc;
    }
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    return (MRESULT)rc;

RunDefWndProc:
//  dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
} /* End of Win32WindowProc */
//******************************************************************************
//******************************************************************************
MRESULT EXPENTRY Win32FrameWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 POSTMSG_PACKET  *postmsg;
 OSLIBPOINT       point, ClientPoint;
 Win32BaseWindow *win32wnd;
 TEB             *teb;
 MRESULT          rc = 0;
 MSG              winMsg, *pWinMsg;

    //Restore our FS selector
    SetWin32TIB();

    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- BEGIN
    teb = GetThreadTEB();
    win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwnd);

    if(!teb || (msg != WM_CREATE && win32wnd == NULL)) {
        dprintf(("PMFRAME: Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
        goto RunDefFrameWndProc;
    }
////    if(teb->o.odin.fIgnoreMsgs) {
////        goto RunDefWndProc;
////    }

    if((teb->o.odin.msgstate & 1) == 0)
    {//message that was sent directly to our window proc handler; translate it here
        QMSG qmsg;

        qmsg.msg  = msg;
        qmsg.hwnd = hwnd;
        qmsg.mp1  = mp1;
        qmsg.mp2  = mp2;
        qmsg.time = WinQueryMsgTime(teb->o.odin.hab);
        WinQueryMsgPos(teb->o.odin.hab, &qmsg.ptl);
        qmsg.reserved = 0;

        if(OS2ToWinMsgTranslate((PVOID)teb, &qmsg, &winMsg, FALSE, MSG_REMOVE) == FALSE)
        {//message was not translated
            memset(&winMsg, 0, sizeof(MSG));
        }
        pWinMsg = &winMsg;
    }
    else {
        pWinMsg = &teb->o.odin.msg;
        teb->o.odin.msgstate++;
    }
    //NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- END

    switch( msg )
    {
    case WM_CREATE:
    {
        //WM_CREATE handled during client window creation
        dprintf(("PMFRAME: WM_CREATE %x"));
        goto RunDefFrameWndProc;
    }

    case WM_PAINT:
    {
      RECTL rectl;

        HPS hps = WinBeginPaint(hwnd, NULL, &rectl);
        dprintf(("PMFRAME: WM_PAINT %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop));

        if(win32wnd->IsWindowCreated() && (rectl.xLeft != rectl.xRight &&
           rectl.yBottom != rectl.yTop))
        {
            PRECT pClient = win32wnd->getClientRectPtr();
            PRECT pWindow = win32wnd->getWindowRect();

            if(!(pClient->left == 0 && pClient->top == 0 &&
               win32wnd->getClientHeight() == win32wnd->getWindowHeight() &&
               win32wnd->getClientWidth()  == win32wnd->getWindowWidth()))
            {
                RECT rectUpdate;

                mapOS2ToWin32Rect(win32wnd->getWindowHeight(), (PRECTLOS2)&rectl, &rectUpdate);
                win32wnd->MsgNCPaint(&rectUpdate);
            }
        }
        WinEndPaint(hps);
        break;
    }

    case WM_ERASEBACKGROUND:
    {
        dprintf(("PMFRAME:WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)FALSE;
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
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgButton(pWinMsg);

        rc = (MRESULT)TRUE;
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
        rc = (MRESULT)TRUE;
        break;

    case WM_MOUSEMOVE:
    {
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgMouseMove(pWinMsg);
        break;
    }

    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld;
      WINDOWPOS wp,wpOld;
      HWND      hParent = NULLHANDLE, hwndAfter;

        dprintf(("PMFRAME:WM_ADJUSTWINDOWPOS %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

        if(win32wnd->IsParentChanging()) {
            rc = 0;
            break;
        }

        if(pswp->fl & SWP_NOADJUST) {
            //ignore weird messages (TODO: why are they sent?)
            break;
        }
        //CB: show dialog in front of owner
        if (win32wnd->IsModalDialogOwner())
        {
            dprintf(("win32wnd->IsModalDialogOwner %x", win32wnd->getWindowHandle()));
            pswp->fl |= SWP_ZORDER;
            pswp->hwndInsertBehind = win32wnd->getOS2HwndModalDialog();
            if (pswp->fl & SWP_ACTIVATE)
            {
                pswp->fl &= ~SWP_ACTIVATE;
                WinSetWindowPos(win32wnd->getOS2HwndModalDialog(),0,0,0,0,0,SWP_ACTIVATE);
            }
        }

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
//            goto RunDefWndProc;
            goto RunDefFrameWndProc;

        if(!win32wnd->CanReceiveSizeMsgs())
            break;

        WinQueryWindowPos(hwnd, &swpOld);
        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto RunDefWndProc;
            }
        }
        hwndAfter = pswp->hwndInsertBehind;
        if(win32wnd->getParent()) {
             OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getClientHeight(), hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           dprintf2(("SWP_ZORDER: %x %x", pswp->hwndInsertBehind, (wndAfter) ? wndAfter->getWindowHandle() : 0));
           if(wndAfter) {
                wp.hwndInsertAfter = wndAfter->getWindowHandle();
                RELEASE_WNDOBJ(wndAfter);
           }
           else wp.hwndInsertAfter = HWND_TOP_W;
        }

        wpOld = wp;
        win32wnd->MsgPosChanging((LPARAM)&wp);

        if(win32wnd->getOldStyle() != win32wnd->getStyle()) {
             OSLibSetWindowStyle(win32wnd->getOS2FrameWindowHandle(), win32wnd->getOS2WindowHandle(), win32wnd->getStyle(), win32wnd->getExStyle());
             if(fOS2Look) {
                 DWORD dwOldStyle = win32wnd->getOldStyle();
                 DWORD dwStyle    = win32wnd->getStyle();
                 
                 if((dwOldStyle & WS_MINIMIZE_W) && !(dwStyle & WS_MINIMIZE_W)) {
                     //SC_RESTORE -> SC_MINIMIZE
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), 0, SC_RESTORE, SC_MINIMIZE, hbmFrameMenu[0]);
                 }
                 else
                 if((dwOldStyle & WS_MAXIMIZE_W) && !(dwStyle & WS_MAXIMIZE_W)) {
                     //SC_RESTORE -> SC_MAXIMIZE
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), MIT_END, SC_RESTORE, SC_MAXIMIZE, hbmFrameMenu[1]);
                 }
                 else
                 if(!(dwOldStyle & WS_MINIMIZE_W) && (dwStyle & WS_MINIMIZE_W)) {
                     //SC_MINIMIZE -> SC_RESTORE
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), 0, SC_MINIMIZE, SC_RESTORE, hbmFrameMenu[2]);
                 }
                 else
                 if(!(dwOldStyle & WS_MAXIMIZE_W) && (dwStyle & WS_MAXIMIZE_W)) {
                     //SC_MAXIMIZE -> SC_RESTORE
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), MIT_END, SC_MAXIMIZE, SC_RESTORE, hbmFrameMenu[2]);
                 }
             }
        }

        if ((wp.hwndInsertAfter != wpOld.hwndInsertAfter) ||
            (wp.x != wpOld.x) || (wp.y != wpOld.y) || (wp.cx != wpOld.cx) || (wp.cy != wpOld.cy) || (wp.flags != wpOld.flags))
        {
            ULONG flags = pswp->fl;      //make a backup copy; OSLibMapWINDOWPOStoSWP will modify it

            dprintf(("PMFRAME:WM_ADJUSTWINDOWPOS, app changed windowpos struct"));
            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

            if(win32wnd->getParent()) {
                  OSLibMapWINDOWPOStoSWP(&wp, pswp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                         hwnd);
            }
            else  OSLibMapWINDOWPOStoSWP(&wp, pswp, &swpOld, OSLibQueryScreenHeight(), hwnd);

            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

            //OSLibMapWINDOWPOStoSWP can add flags, but we must not let it remove flags!
            if(pswp->fl & SWP_SIZE)
                flags |= SWP_SIZE;

            if(pswp->fl & SWP_MOVE)
                flags |= SWP_MOVE;

            pswp->fl = flags;   //restore flags

            pswp->fl |= SWP_NOADJUST;
            pswp->hwndInsertBehind = hwndAfter;
            pswp->hwnd = hwnd;

            rc = (MRESULT)0xf;
            break;
        }
        rc = 0;
        break;
    }

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp   = (PSWP)mp1,pswpOld = pswp+1;
      SWP       swpOld = *(pswp + 1);
      WINDOWPOS wp;
      HWND      hParent = NULLHANDLE;
      RECTL     rect;

        dprintf(("PMFRAME:WM_WINDOWPOSCHANGED (%x) %x %x (%d,%d) (%d,%d)", mp2, win32wnd->getWindowHandle(), pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        if(win32wnd->IsParentChanging()) {
            goto PosChangedEnd;
        }

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
        {
            if(pswp->fl & SWP_RESTORE && win32wnd->getStyle() & WS_MINIMIZE_W) {
                dprintf(("Restoring minimized window %x", win32wnd->getWindowHandle()));
                win32wnd->ShowWindow(SW_RESTORE_W);
            }
            if(pswp->fl & SWP_SHOW) {
                WinShowWindow(win32wnd->getOS2WindowHandle(), 1);
            }
            else
            if(pswp->fl & SWP_HIDE) {
                WinShowWindow(win32wnd->getOS2WindowHandle(), 0);
            }
/*
            if(pswp->fl & SWP_ACTIVATE)
            {
                //Only send PM WM_ACTIVATE to top-level windows (frame windows)
                if(!(WinQueryWindowULong(WinWindowFromID(hwnd,FID_CLIENT), OFFSET_WIN32FLAGS) & WINDOWFLAG_ACTIVE))
                {
                    WinSendDlgItemMsg(hwnd, FID_CLIENT, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
//                    WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
                }
            }
            else
            if(pswp->fl & SWP_DEACTIVATE)
            {
                //Only send PM WM_ACTIVATE to top-level windows (frame windows)
                if(WinQueryWindowULong(WinWindowFromID(hwnd,FID_CLIENT), OFFSET_WIN32FLAGS) & WINDOWFLAG_ACTIVE)
                {
                    WinSendDlgItemMsg(hwnd, FID_CLIENT, WM_ACTIVATE, (MPARAM)FALSE, (MPARAM)hwnd);
//                    WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)FALSE, (MPARAM)hwnd);
                }
            }
*/
//            goto RunDefWndProc;
            goto RunDefFrameWndProc;
        }

        if(pswp->fl & (SWP_MOVE | SWP_SIZE))
        {
            if(win32wnd->isChild())
            {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto PosChangedEnd; //parent has just been destroyed
            }
        }


        if(win32wnd->getParent()) {
             OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                    hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
            Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
            dprintf2(("SWP_ZORDER: %x %x", pswp->hwndInsertBehind, (wndAfter) ? wndAfter->getWindowHandle() : 0));
            if(wndAfter) {
                 wp.hwndInsertAfter = wndAfter->getWindowHandle();
                 RELEASE_WNDOBJ(wndAfter);
            }
            else wp.hwndInsertAfter = HWND_TOP_W;
        }

        if(pswp->fl & SWP_SHOW) {
            WinShowWindow(win32wnd->getOS2WindowHandle(), 1);
        }
        else
        if(pswp->fl & SWP_HIDE) {
            WinShowWindow(win32wnd->getOS2WindowHandle(), 0);
        }

#ifndef USE_CALCVALIDRECT
        if((pswp->fl & (SWP_MOVE | SWP_SIZE)))
        {
            //CB: todo: use result for WM_CALCVALIDRECTS
            //Get old client rectangle (for invalidation of frame window parts later on)
            //Use new window height to calculate the client area
            mapWin32ToOS2Rect(pswp->cy, win32wnd->getClientRectPtr(), (PRECTLOS2)&rect);

            //Note: Also updates the new window rectangle
            win32wnd->MsgFormatFrame(&wp);

            if(win32wnd->isOwnDC()) {
                setPageXForm(win32wnd, (pDCData)GpiQueryDCData(win32wnd->getOwnDC()));
            }

            if(win32wnd->CanReceiveSizeMsgs())
                win32wnd->MsgPosChanged((LPARAM)&wp);

            if((pswp->fl & SWP_SIZE) && ((pswp->cx != pswpOld->cx) || (pswp->cy != pswpOld->cy)))
            {
                //redraw the frame (to prevent unnecessary client updates)
                BOOL redrawAll = FALSE;

                dprintf2(("WM_WINDOWPOSCHANGED: redraw frame"));
                if (win32wnd->getWindowClass())
                {
                    DWORD dwStyle = win32wnd->getWindowClass()->getClassLongA(GCL_STYLE_W);

                    if ((dwStyle & CS_HREDRAW_W) && (pswp->cx != pswpOld->cx))
                        redrawAll = TRUE;
                    else
                    if ((dwStyle & CS_VREDRAW_W) && (pswp->cy != pswpOld->cy))
                        redrawAll = TRUE;
                }
                else redrawAll = TRUE;

                if(win32wnd->IsMixMaxStateChanging()) {
                    dprintf(("WM_CALCVALIDRECT: window changed min/max/restore state, invalidate entire window"));
                    redrawAll = TRUE;
                }

                if (redrawAll)
                {
                    //CB: redraw all children for now
                    //    -> problems with update region if we don't do it
                    //       todo: rewrite whole handling
                    WinInvalidateRect(hwnd,NULL,TRUE);
                }
                else
                {
                    HPS hps = WinGetPS(hwnd);
                    RECTL frame,client,arcl[4];

                    WinQueryWindowRect(hwnd,&frame);

                    //top
                    arcl[0].xLeft = 0;
                    arcl[0].xRight = frame.xRight;
                    arcl[0].yBottom = rect.yTop;
                    arcl[0].yTop = frame.yTop;
                    //right
                    arcl[1].xLeft = rect.xRight;
                    arcl[1].xRight = frame.xRight;
                    arcl[1].yBottom = 0;
                    arcl[1].yTop = frame.yTop;
                    //left
                    arcl[2].xLeft = 0;
                    arcl[2].xRight = rect.xLeft;
                    arcl[2].yBottom = 0;
                    arcl[2].yTop = frame.yTop;
                    //bottom
                    arcl[3].xLeft = 0;
                    arcl[3].xRight = frame.xRight;
                    arcl[3].yBottom = 0;
                    arcl[3].yTop = rect.yBottom;

                    HRGN hrgn = GpiCreateRegion(hps,4,(PRECTL)&arcl);

                    WinInvalidateRegion(hwnd,hrgn,FALSE);
                    GpiDestroyRegion(hps,hrgn);
                    WinReleasePS(hps);
                }
            }
        }
        else
        {
#endif //USE_CALCVALIDRECT
            if(win32wnd->CanReceiveSizeMsgs())
                win32wnd->MsgPosChanged((LPARAM)&wp);
#ifndef USE_CALCVALIDRECT
        }
#endif

        if(pswp->fl & SWP_ACTIVATE)
        {
             //Only send PM WM_ACTIVATE to top-level windows (frame windows)
             if(!(WinQueryWindowULong(WinWindowFromID(hwnd,FID_CLIENT), OFFSET_WIN32FLAGS) & WINDOWFLAG_ACTIVE))
             {
//                WinSendDlgItemMsg(hwnd, FID_CLIENT, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
                WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)TRUE, (MPARAM)hwnd);
             }
        }
        else
        if(pswp->fl & SWP_DEACTIVATE)
        {
            //Only send PM WM_ACTIVATE to top-level windows (frame windows)
            if(WinQueryWindowULong(WinWindowFromID(hwnd,FID_CLIENT), OFFSET_WIN32FLAGS) & WINDOWFLAG_ACTIVE)
            {
//                WinSendDlgItemMsg(hwnd, FID_CLIENT, WM_ACTIVATE, (MPARAM)FALSE, (MPARAM)hwnd);
                    WinSendMsg(hwnd, WM_ACTIVATE, (MPARAM)FALSE, (MPARAM)hwnd);
            }
        }

PosChangedEnd:
        rc = (MRESULT)FALSE;
        break;
    }

    case WM_CALCVALIDRECTS:
#ifdef USE_CALCVALIDRECT
    {
        PRECTL    oldRect = (PRECTL)mp1, newRect = oldRect+1;
        PSWP      pswp = (PSWP)mp2;
        SWP       swpOld;
        WINDOWPOS wp;
        RECTL     newClientRect, oldClientRect;
        ULONG     nccalcret;
//        UINT      res = CVR_ALIGNLEFT | CVR_ALIGNTOP;
        UINT      res = 0;

        dprintf(("PMWINDOW: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));

        //Get old position info
        WinQueryWindowPos(hwnd, &swpOld);

        if(win32wnd->getParent()) {
             OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                    win32wnd->getParent()->getClientRectPtr()->left,
                                    win32wnd->getParent()->getClientRectPtr()->top,
                                    hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), 0, 0, hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
            Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
            if(wndAfter) {
                 wp.hwndInsertAfter = wndAfter->getWindowHandle();
                 RELEASE_WNDOBJ(wndAfter);
            }
            else wp.hwndInsertAfter = HWND_TOP_W;
        }

        //Get old client rectangle
        mapWin32ToOS2Rect(oldRect->yTop - oldRect->yBottom, win32wnd->getClientRectPtr(), (PRECTLOS2)&oldClientRect);

        //Note: Also updates the new window rectangle
        nccalcret = win32wnd->MsgFormatFrame(&wp);

        //Get new client rectangle
        mapWin32ToOS2Rect(pswp->cy, win32wnd->getClientRectPtr(), (PRECTLOS2)&newClientRect);

        if(nccalcret == 0) {
            res = CVR_ALIGNTOP | CVR_ALIGNLEFT;
        }
        else {
            if(nccalcret & WVR_ALIGNTOP_W) {
                res |= CVR_ALIGNTOP;
            }
            else
            if(nccalcret & WVR_ALIGNBOTTOM_W) {
                res |= CVR_ALIGNBOTTOM;
            }

            if(nccalcret & WVR_ALIGNLEFT_W) {
                res |= CVR_ALIGNLEFT;
            }
            else
            if(nccalcret & WVR_ALIGNRIGHT_W) {
                res |= CVR_ALIGNRIGHT;
            }

            if(nccalcret & WVR_REDRAW_W) {//WVR_REDRAW_W = (WVR_HREDRAW | WVR_VREDRAW)
                res |= CVR_REDRAW;
            }
            else
            if(nccalcret & WVR_VALIDRECTS_W) {
                //TODO:
                //res = 0;
            }
        }
        if(win32wnd->IsMixMaxStateChanging()) {
            dprintf(("WM_CALCVALIDRECT: window changed min/max/restore state, invalidate entire window"));
            res |= CVR_REDRAW;
        }
        if(res == (CVR_ALIGNTOP|CVR_ALIGNLEFT)) {
            oldRect->xRight  -= oldClientRect.xLeft;
            oldRect->yBottom += oldClientRect.yBottom;
            newRect->xRight  -= newClientRect.xLeft;
            newRect->yBottom += newClientRect.yBottom;
        }
        rc = res;
        break;
    }
#else
        dprintf(("PMWINDOW: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)(CVR_ALIGNLEFT | CVR_ALIGNTOP);
        break;
#endif

    case WM_CALCFRAMERECT:
        dprintf(("PMFRAME:WM_CALCFRAMERECT %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)TRUE;
        break;

    case WM_QUERYCTLTYPE:
        // This is a frame window
        dprintf(("PMFRAME:WM_QUERYCTLTYPE %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)CCT_FRAME;
        break;

#ifdef DEBUG
    case WM_QUERYFOCUSCHAIN:
        dprintf2(("PMFRAME:WM_QUERYFOCUSCHAIN %x fsCmd %x parent %x", win32wnd->getWindowHandle(), SHORT1FROMMP(mp1), (mp2) ? OS2ToWin32Handle((DWORD)mp2) : 0));

        RestoreOS2TIB();
        rc = pfnFrameWndProc(hwnd, msg, mp1, mp2);
        SetWin32TIB();
        dprintf2(("PMFRAME:WM_QUERYFOCUSCHAIN %x fsCmd %x parent %x returned %x", win32wnd->getWindowHandle(), SHORT1FROMMP(mp1), (mp2) ? OS2ToWin32Handle((DWORD)mp2) : 0, (rc) ? OS2ToWin32Handle((DWORD)rc) : 0));
        break;
//        goto RunDefFrameWndProc;
#endif

#ifdef DEBUG
    case WM_FOCUSCHANGE:
    {
        HWND   hwndFocus = (HWND)mp1;
        HWND   hwndLoseFocus, hwndGainFocus;
        USHORT usSetFocus = SHORT1FROMMP(mp2);
        USHORT fsFocusChange = SHORT2FROMMP(mp2);

        dprintf(("PMFRAME:WM_FOCUSCHANGE %x %x (%x) %x %x", win32wnd->getWindowHandle(), OS2ToWin32Handle(hwndFocus), hwndFocus, usSetFocus, fsFocusChange));
        goto RunDefFrameWndProc;
    }

    case WM_SETFOCUS:
    {
        dprintf(("PMFRAME: WM_SETFOCUS %x %x", win32wnd->getWindowHandle(), hwnd));
        goto RunDefFrameWndProc;
    }
#endif

    case WM_ACTIVATE:
    {
        HWND hwndTitle;
        USHORT flags = WinQueryWindowUShort(hwnd,QWS_FLAGS);

        dprintf(("PMFRAME: WM_ACTIVATE %x %x %x", win32wnd->getWindowHandle(), mp1, OS2ToWin32Handle((DWORD)mp2)));
        if (win32wnd->IsWindowCreated())
        {
            WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
            if(fOS2Look) {
                dprintf(("TBM_QUERYHILITE returned %d", WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_QUERYHILITE, 0, 0)));
                WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_SETHILITE, mp1, 0);
            }
            WinSendDlgItemMsg(hwnd, FID_CLIENT, WM_ACTIVATE, mp1, mp2);

            //CB: show owner behind the dialog
            if (win32wnd->IsModalDialog())
            {
                if(win32wnd->getOwner()) {
                    Win32BaseWindow *topOwner = Win32BaseWindow::GetWindowFromHandle(win32wnd->getOwner()->GetTopParent());
 
                    if (topOwner) {
                        WinSetWindowPos(topOwner->getOS2FrameWindowHandle(),hwnd,0,0,0,0,SWP_ZORDER);
                        RELEASE_WNDOBJ(topOwner);
                    }
                }
            }
        }
        else
        {
            WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
        }
        rc = 0;
        break;
    }

    case WM_ENABLE:
        dprintf(("PMFRAME: WM_ENABLE %x", hwnd));
        win32wnd->MsgEnable(SHORT1FROMMP(mp1));
        break;

    case WM_SHOW:
        dprintf(("PMFRAME: WM_SHOW %x %d", hwnd, mp1));
        //show client window
        WinShowWindow(win32wnd->getOS2WindowHandle(), (BOOL)mp1);
        break;

    case WM_QUERYTRACKINFO:
    {
        PTRACKINFO trackInfo = (PTRACKINFO)mp2;

        dprintf(("PMFRAME:WM_QUERYTRACKINFO %x", win32wnd->getWindowHandle()));
        trackInfo->cxBorder = 4;
        trackInfo->cyBorder = 4;
        win32wnd->AdjustTrackInfo((PPOINT)&trackInfo->ptlMinTrackSize,(PPOINT)&trackInfo->ptlMaxTrackSize);
        rc = (MRESULT)TRUE;
        break;
    }

    case WM_QUERYBORDERSIZE:
    {
        PWPOINT size = (PWPOINT)mp1;

        dprintf(("PMFRAME:WM_QUERYBORDERSIZE %x", win32wnd->getWindowHandle()));

        size->x = 0;
        size->y = 0;
        rc = (MRESULT)TRUE;
        break;
    }

#ifdef DEBUG
    case WM_QUERYFRAMEINFO:
        dprintf(("PMFRAME:WM_QUERYFRAMEINFO %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;
#endif

    case WM_FORMATFRAME:
        dprintf(("PMFRAME:WM_FORMATFRAME %x", win32wnd->getWindowHandle()));
        break;

#ifdef DEBUG
    case WM_ADJUSTFRAMEPOS:
    {
        PSWP pswp   = (PSWP)mp1;

        dprintf(("PMFRAME:WM_ADJUSTFRAMEPOS %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        goto RunDefFrameWndProc;
    }

    case WM_OWNERPOSCHANGE:
    {
        PSWP pswp   = (PSWP)mp1;

        dprintf(("PMFRAME:WM_OWNERPOSCHANGE %x %x %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));
        goto RunDefFrameWndProc;
    }
#endif

    case WM_MINMAXFRAME:
    {
        PSWP swp = (PSWP)mp1;

        if (!win32wnd->IsWindowCreated()) goto RunDefWndProc;

        dprintf(("PMFRAME:WM_MINMAXFRAME %x",hwnd));
        if ((swp->fl & SWP_MAXIMIZE) == SWP_MAXIMIZE)
        {
            win32wnd->setStyle((win32wnd->getStyle() & ~WS_MINIMIZE_W) | WS_MAXIMIZE_W);

            RECT rect;

            rect.left = rect.top = rect.right = rect.bottom = 0;
            win32wnd->AdjustMaximizedRect(&rect);
            swp->x += rect.left;
            swp->cx += rect.right-rect.left;
            swp->y -= rect.bottom;
            swp->cy += rect.bottom-rect.top;
        }
        else
        if ((swp->fl & SWP_MINIMIZE) == SWP_MINIMIZE)
        {
            win32wnd->setStyle((win32wnd->getStyle() & ~WS_MAXIMIZE_W) | WS_MINIMIZE_W);
        }
        else
        if ((swp->fl & SWP_RESTORE) == SWP_RESTORE)
        {
            win32wnd->setStyle(win32wnd->getStyle() & ~(WS_MINIMIZE_W | WS_MAXIMIZE_W));
        }
        goto RunDefWndProc;
    }

#ifdef DEBUG
    case WM_UPDATEFRAME:
        dprintf(("PMFRAME:WM_UPDATEFRAME %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;
#endif

    case WM_TRACKFRAME:
        dprintf(("PMFRAME: WM_TRACKFRAME %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        if(fOS2Look) {//sent by titlebar control
            FrameTrackFrame(win32wnd, TF_MOVE);
        }
        rc = 0;
        break;

    case WM_SYSCOMMAND:
        dprintf(("PMFRAME: WM_SYSCOMMAND %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->DispatchMsgA(pWinMsg);
        break;

#ifdef DEBUG
    case WM_DDE_INITIATE:
    case WM_DDE_INITIATEACK:
    case WM_DDE_REQUEST:
    case WM_DDE_ACK:
    case WM_DDE_DATA:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
    case WM_DDE_TERMINATE:
         dprintf(("PMFRAME: WM_DDE %x %x", msg, win32wnd->getWindowHandle()));
         break;
#endif

    default:
        goto RunDefFrameWndProc;
    }
    RestoreOS2TIB();
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    return (MRESULT)rc;

RunDefFrameWndProc:
    dprintf2(("RunDefFrameWndProc"));
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    return pfnFrameWndProc(hwnd, msg, mp1, mp2);

RunDefWndProc:
    dprintf2(("RunDefWndProc"));
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    //calling WinDefWindowProc here breaks Opera hotlist window (WM_ADJUSTWINDOWPOS)
//    return pfnFrameWndProc(hwnd, msg, mp1, mp2);
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
//******************************************************************************
//TODO: Quickly moving a window two times doesn't force a repaint (1st time)
//
//
BOOL (APIENTRY *WinTrackWindow)(HWND hwndTrack, PTRACKINFO pti) = NULL;
//
//******************************************************************************
VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags)
{
  TRACKINFO track;
  RECTL     rcl;
  PRECT     pWindowRect, pClientRect;
  HWND      hwndTracking;
  LONG      parentHeight, parentWidth;
  static    BOOL fInit = FALSE;
  APIRET    rc;
  BOOL      ret;

    if(!fInit) {
        HMODULE hModule;
        char    buf[CCHMAXPATH];
        rc = DosLoadModule(buf, sizeof(buf), "PMMERGE", &hModule);
        rc = DosQueryProcAddr(hModule, 5466, NULL, (PFN *)&WinTrackWindow);
        if(rc) WinTrackWindow = NULL;
        fInit = TRUE;
    }
    dprintf(("FrameTrackFrame: %x %x", win32wnd->getWindowHandle(), flags));
    track.cxBorder = 4;
    track.cyBorder = 4;  /* 4 pel wide lines used for rectangle */
    track.cxGrid = 1;
    track.cyGrid = 1;    /* smooth tracking with mouse */
    track.cxKeyboard = 8;
    track.cyKeyboard = 8; /* faster tracking using cursor keys */

    pWindowRect = win32wnd->getWindowRect();
    if(win32wnd->getParent()) {
        parentHeight = win32wnd->getParent()->getClientHeight();
        parentWidth  = win32wnd->getParent()->getClientWidth();
        hwndTracking = win32wnd->getParent()->getOS2WindowHandle();
    }
    else {
        parentHeight = OSLibQueryScreenHeight();
        parentWidth  = OSLibQueryScreenWidth();
        hwndTracking = HWND_DESKTOP;
    }

    mapWin32ToOS2Rect(parentHeight, pWindowRect, (PRECTLOS2)&track.rclTrack);
    rcl = track.rclTrack;
    WinQueryWindowRect(hwndTracking, &track.rclBoundary);

    track.ptlMinTrackSize.x = 10;
    track.ptlMinTrackSize.y = 10;  /* set smallest allowed size of rectangle */
    track.ptlMaxTrackSize.x = parentWidth;
    track.ptlMaxTrackSize.y = parentHeight; /* set largest allowed size of rectangle */

    win32wnd->AdjustTrackInfo((PPOINT)&track.ptlMinTrackSize, (PPOINT)&track.ptlMaxTrackSize);

    track.fs = flags;

    BOOL fDynamicDrag = WinQuerySysValue(HWND_DESKTOP, SVOS_DYNAMICDRAG);

    SEL sel = RestoreOS2FS();
    if(fDynamicDrag && WinTrackWindow) {
         ret = WinTrackWindow(win32wnd->getOS2FrameWindowHandle(), &track);
    }
    else ret = WinTrackRect(hwndTracking, NULL, &track);
    SetFS(sel);

    if(ret) {
        /* if successful copy final position back */
        if(!WinEqualRect(0, &rcl, &track.rclTrack)) {
            dprintf(("FrameTrackFrame: new (os/2) window rect: (%d,%d)(%d,%d)", track.rclTrack.xLeft, track.rclTrack.yBottom, track.rclTrack.xRight - track.rclTrack.xLeft, track.rclTrack.yTop - track.rclTrack.yBottom));
            if(flags == TF_MOVE) {
                WinSetWindowPos(win32wnd->getOS2FrameWindowHandle(),
                                0, track.rclTrack.xLeft, track.rclTrack.yBottom,
                                0, 0, SWP_MOVE);
            }
            else {
                WinSetWindowPos(win32wnd->getOS2FrameWindowHandle(),
                                0, track.rclTrack.xLeft, track.rclTrack.yBottom,
                                track.rclTrack.xRight - track.rclTrack.xLeft,
                                track.rclTrack.yTop - track.rclTrack.yBottom,
                                SWP_SIZE|SWP_MOVE);
            }
        }
        return;
   }
   return;
}
//******************************************************************************
//******************************************************************************
void FrameReplaceMenuItem(HWND hwndMenu, ULONG nIndex, ULONG idOld, ULONG   idNew,
                          HBITMAP hbmNew)
{
    MENUITEM mi;

    if (!hwndMenu)
        return;

    WinEnableWindowUpdate(hwndMenu, FALSE);

    if (WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(idOld, TRUE), MPFROMP(&mi)))
    {
        WinSendMsg(hwndMenu, MM_REMOVEITEM, (MPARAM)idOld, 0);
        mi.afStyle     = MIS_BITMAP | MIS_SYSCOMMAND;
        mi.afAttribute = 0;
        mi.hwndSubMenu = 0;
        mi.id    = idNew;
        mi.hItem = (ULONG)hbmNew;
        WinSendMsg(hwndMenu, MM_INSERTITEM, (MPARAM)&mi, 0);
    }
    WinEnableWindowUpdate(hwndMenu, TRUE);

    WinInvalidateRect(hwndMenu, NULL, TRUE);
}
//******************************************************************************
//******************************************************************************
