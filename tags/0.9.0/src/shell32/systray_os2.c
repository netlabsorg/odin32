/*
 *  OS/2 System Tray support. For usage with SysTray/WPS by MadInt
 *  or System Tray widget for XCenter/eCenter.
 *
 *  author: ErOs2
 *  date: 1 May 2004
 *
 */

#define  INCL_WIN
#include <os2wrap.h> // Odin32 OS/2 api wrappers

#include <win32api.h>

#include <string.h>

#include "systray_os2.h"

#define WM_TRAYADDME (WM_USER+1)
#define WM_TRAYDELME (WM_USER+2)
#define WM_TRAYICON  (WM_USER+3)
#define WM_TRAYEXIT  (0xCD20)

#define SZAPP       "SystrayServer"
#define SZTOPIC     "TRAY"

static HWND hwndTrayServer = 0;

static MRESULT EXPENTRY SYSTRAY_Old_WndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    SystrayItem *ptrayItem = (SystrayItem *)WinQueryWindowULong(hWnd, QWL_USER);

    switch ( msg )
    {
        case WM_CREATE:
            WinSetWindowULong(hWnd, QWL_USER, (ULONG)mp1);
            ptrayItem = (SystrayItem *)mp1;
            return (MRESULT)FALSE;
        case WM_DDE_INITIATEACK:
            hwndTrayServer = (HWND)mp1;
            if(hwndTrayServer)
                WinPostMsg(hwndTrayServer,WM_TRAYADDME,(MPARAM)hWnd,(MPARAM)NULL);
            return (MRESULT)TRUE;
        case WM_BUTTON1DBLCLK|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_LBUTTONDBLCLK_W);
            return (MRESULT)FALSE;
        case WM_BUTTON2DBLCLK|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_RBUTTONDBLCLK_W);
            return (MRESULT)FALSE;
        case WM_BUTTON3DBLCLK|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_MBUTTONDBLCLK_W);
            return (MRESULT)FALSE;
        case WM_BUTTON1UP|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_LBUTTONUP_W);
            return (MRESULT)FALSE;
        case WM_BUTTON2UP|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_RBUTTONUP_W);
            return (MRESULT)FALSE;
        case WM_BUTTON3UP|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_MBUTTONUP_W);
            return (MRESULT)FALSE;
        case WM_BUTTON1DOWN|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_LBUTTONDOWN_W);
            return (MRESULT)FALSE;
        case WM_BUTTON2DOWN|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_RBUTTONDOWN_W);
            return (MRESULT)FALSE;
        case WM_BUTTON3DOWN|0x2000:
            PostMessageA(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID, (LPARAM)WM_MBUTTONDOWN_W);
            return (MRESULT)FALSE;
    }
    return WinDefWindowProc( hWnd , msg , mp1 , mp2 );
}

static const char * const systraysupclass = "Odin32SystraySupport";

static BOOL SYSTRAY_Old_RegisterClass(void)
{
    //WinRegisterClass( hab, systraysupclass, SYSTRAY_Old_WndProc, 0, sizeof(ULONG) );
    WinRegisterClass(0, systraysupclass, SYSTRAY_Old_WndProc, 0, sizeof(ULONG) );
    return TRUE;
}


static BOOL SYSTRAY_Old_ItemInit(SystrayItem *ptrayItem)
{
    ULONG fcf = FCF_TITLEBAR;

    if (!SYSTRAY_Old_RegisterClass())
    {
        return FALSE;
    }

    ptrayItem->hWndFrame = WinCreateStdWindow(HWND_DESKTOP, 0, &fcf, NULL,
                                              "", 0, NULLHANDLE, 0, NULL);
    if (!ptrayItem->hWndFrame)
    {
        return FALSE;
    }
    ptrayItem->hWndClient = WinCreateWindow(ptrayItem->hWndFrame, systraysupclass, "",
                                            0, 0, 0, 0, 0, ptrayItem->hWndFrame,
                                            HWND_TOP, FID_CLIENT, ptrayItem, NULL);
    if (!ptrayItem->hWndClient)
    {
        return FALSE;
    }

    WinDdeInitiate(ptrayItem->hWndClient, SZAPP, SZTOPIC, NULL);
    if (hwndTrayServer)
        WinPostMsg(hwndTrayServer, WM_TRAYICON,
                   (MPARAM)ptrayItem->hWndClient, (MPARAM)NULL);

    return (hwndTrayServer != NULLHANDLE);
}

static void SYSTRAY_Old_ItemTerm(SystrayItem *ptrayItem)
{
    HPOINTER hIcon = (HPOINTER)WinSendMsg(ptrayItem->hWndFrame, WM_QUERYICON, NULL, NULL);
    if (hIcon != NULLHANDLE)
        WinDestroyPointer(hIcon);

    WinDestroyWindow(ptrayItem->hWndFrame);
}

static void SYSTRAY_Old_ItemUpdate(SystrayItem *ptrayItem, ULONG uFlags)
{
    // uFlags = 0 means it's the first time, add everything

    if (uFlags == 0 || (uFlags & NIF_ICON))
    {
        // Windows seems to make a copy of icons displayed in the tray area. At
        // least, calling DestroyIcon() after passing the icon handle to
        // Shell_NotifyIcon() doesn't harm the icon displayed in the tray.
        // Behave similarly on OS/2 (some applications do call DestroyIcon()).
        // The copy is deleted in SYSTRAY_Old_ItemTerm().

        POINTERINFO Info;
        HPOINTER hIcon = NULLHANDLE;
        BOOL brc;
        brc = WinQueryPointerInfo(ptrayItem->notifyIcon.hIcon, &Info);
        if (brc)
        {
            hIcon = WinCreatePointerIndirect(HWND_DESKTOP, &Info);
            if (hIcon != NULLHANDLE)
            {
                WinSendMsg(ptrayItem->hWndFrame, WM_SETICON,
                           MPFROMLONG(hIcon), MPVOID);
                if (hwndTrayServer)
                    WinPostMsg(hwndTrayServer, WM_TRAYICON,
                               (MPARAM)ptrayItem->hWndClient, (MPARAM)NULL);
            }
        }
    }
    if (uFlags == 0 || (uFlags & NIF_TIP))
    {
        WinSetWindowText(ptrayItem->hWndFrame, ptrayItem->notifyIcon.szTip);
        if (hwndTrayServer)
            WinPostMsg(hwndTrayServer, WM_TRAYICON,
                       (MPARAM)ptrayItem->hWndClient, (MPARAM)NULL);
    }
}

BOOL SYSTRAY_Init(void)
{
    // try the new xsystray API first
    if (SYSTRAY_Ex_Init())
        return TRUE;

    // fallback to the old API
    SYSTRAY_ItemInit = SYSTRAY_Old_ItemInit;
    SYSTRAY_ItemTerm = SYSTRAY_Old_ItemTerm;
    SYSTRAY_ItemUpdate = SYSTRAY_Old_ItemUpdate;

    return TRUE;
}

