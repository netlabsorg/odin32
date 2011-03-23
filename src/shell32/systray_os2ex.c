/*
 *  OS/2 System Tray support.
 *  A better implementaiton using the xsystray XCenter/eCenter widget API.
 *
 *  Author: Dmitriy Kuminov
 */

#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2wrap.h> // Odin32 OS/2 api wrappers

#include <win32api.h>

#include <string.h>

// declare function pointers for dynamic linking to xsystray DLL
#define XSTAPI_FPTRS_STATIC
#include <xsystray.h>

#include "systray_os2.h"

#include "dbglog.h"

#define WM_XST_MYNOTIFY (WM_USER + 1000)

static ULONG WM_XST_CREATED = 0;
static ULONG WM_TASKBARCREATED_W = 0;

static HWND hwndProxy = NULLHANDLE;
static ULONG hwndProxyRefs = 0;

static PFNWP OldProxyWndProc = NULL;

static MRESULT EXPENTRY ProxyWndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_XST_MYNOTIFY:
    {
        USHORT usIconID = SHORT1FROMMP(mp1);
        USHORT usNotifyCode = SHORT2FROMMP(mp1);

        SystrayItem *ptrayItem = SYSTRAY_FindItem(usIconID);
        if (!ptrayItem)
            return (MRESULT)FALSE;

        switch (usNotifyCode)
        {
        case XST_IN_MOUSE:
        {
            PXSTMOUSEMSG pmmsg = (PXSTMOUSEMSG)mp2;
            ULONG winMsg = 0;

            switch (pmmsg->ulMouseMsg)
            {
            case WM_BUTTON1DBLCLK: winMsg = WM_LBUTTONDBLCLK_W; break;
            case WM_BUTTON2DBLCLK: winMsg = WM_RBUTTONDBLCLK_W; break;
            case WM_BUTTON3DBLCLK: winMsg = WM_MBUTTONDBLCLK_W; break;
            case WM_BUTTON1UP: winMsg = WM_LBUTTONUP_W; break;
            case WM_BUTTON2UP: winMsg = WM_RBUTTONUP_W; break;
            case WM_BUTTON3UP: winMsg = WM_MBUTTONUP_W; break;
            case WM_BUTTON1DOWN: winMsg = WM_LBUTTONDOWN_W; break;
            case WM_BUTTON2DOWN: winMsg = WM_RBUTTONDOWN_W; break;
            case WM_BUTTON3DOWN: winMsg = WM_MBUTTONDOWN_W; break;
            default: break;
            }

            if (winMsg)
            {
                PostMessageA(ptrayItem->notifyIcon.hWnd,
                             ptrayItem->notifyIcon.uCallbackMessage,
                             (WPARAM)ptrayItem->notifyIcon.uID,
                             (LPARAM)winMsg);
                return (MRESULT)TRUE;
            }
            break;
        }
        case XST_IN_CONTEXT:
        {
            PostMessageA(ptrayItem->notifyIcon.hWnd,
                         ptrayItem->notifyIcon.uCallbackMessage,
                         (WPARAM)ptrayItem->notifyIcon.uID,
                         (LPARAM)WM_CONTEXTMENU_W);
            return (MRESULT)TRUE;
        }
        default:
            break;
        }

        return (MRESULT)FALSE;
    }
    default:
    {
        if (msg == WM_XST_CREATED)
        {
            // xCenter was restarted, no icons are shown; remove all items
            // and broadcast TaskbarCreated to let applications recreate them
            SYSTRAY_PruneAllItems();

            dprintf(("SHELL32: SYSTRAY: Broadcasting \"TaskbarCreated\" (%x)",
                     WM_TASKBARCREATED_W));
            PostMessageA(HWND_BROADCAST_W, WM_TASKBARCREATED_W,
                         (WPARAM)0, (LPARAM)0);
            return (MRESULT)TRUE;
        }
        break;
    }
    }

    return WinDefWindowProc(hWnd, msg, mp1, mp2);
}

static BOOL SYSTRAY_Ex_ItemInit(SystrayItem *ptrayItem)
{
    if (hwndProxyRefs == 0)
    {
        if (!WinRegisterClass(NULLHANDLE, "OdinXSysTrayProxy", ProxyWndProc,
                              0, 0))
        {
            dprintf(("SHELL32: SYSTRAY: WinRegisterClass() failed with %x",
                     WinGetLastError(0)));
            return FALSE;
        }

        hwndProxy = WinCreateWindow(HWND_DESKTOP, "OdinXSysTrayProxy", NULL,
                                    0, 0, 0, 0, 0, NULLHANDLE, HWND_BOTTOM, 0,
                                    NULL, NULL);

        if (hwndProxy == NULLHANDLE)
        {
            dprintf(("SHELL32: SYSTRAY: WinCreateWindow() failed with %x",
                     WinGetLastError(0)));
            return FALSE;
        }
    }

    ++ hwndProxyRefs;

    return TRUE;
}

static void SYSTRAY_Ex_ItemTerm(SystrayItem *ptrayItem)
{
    xstRemoveSysTrayIcon(hwndProxy, ptrayItem->uIdx);

    if (-- hwndProxyRefs == 0)
    {
        WinDestroyWindow(hwndProxy);
        hwndProxy = NULLHANDLE;
    }
}

static void SYSTRAY_Ex_ItemUpdate(SystrayItem *ptrayItem, ULONG uFlags)
{
    if (uFlags == 0 || (uFlags & (NIF_ICON | NIF_TIP) == NIF_ICON | NIF_TIP))
    {
        // uFlags = 0 means it's the first time so add the icon. The other case
        // is true when a bunch of the parameters is changed at once so use
        // xstAdd... too to save a few IPC calls.
        xstAddSysTrayIcon(hwndProxy, ptrayItem->uIdx,
                          ptrayItem->notifyIcon.hIcon,
                          ptrayItem->notifyIcon.szTip,
                          WM_XST_MYNOTIFY,
                          XST_IN_MOUSE | XST_IN_CONTEXT);
        return;
    }

    if (uFlags & NIF_ICON)
    {
        xstReplaceSysTrayIcon(hwndProxy, ptrayItem->uIdx,
                              ptrayItem->notifyIcon.hIcon);
    }
    if (uFlags & NIF_TIP)
    {
        xstSetSysTrayIconToolTip(hwndProxy, ptrayItem->uIdx,
                                 ptrayItem->notifyIcon.szTip);
    }
}

BOOL SYSTRAY_Ex_Init(void)
{
    static BOOL tried = FALSE;
    if (!tried) {
        char err[CCHMAXPATH];
        HMODULE hmod;

        tried = TRUE;

        // link to the xsystray DLL at runtime
        if (DosLoadModule(err, sizeof(err), "XSYSTRAY", &hmod) != NO_ERROR)
            return FALSE;

        #define R(f) if (DosQueryProcAddr(hmod, 0, #f, (PFN*)&f) != NO_ERROR) return FALSE

        R(xstQuerySysTrayVersion);
        R(xstAddSysTrayIcon);
        R(xstReplaceSysTrayIcon);
        R(xstRemoveSysTrayIcon);
        R(xstSetSysTrayIconToolTip);
        R(xstQuerySysTrayIconRect);
        R(xstGetSysTrayCreatedMsgId);
        R(xstGetSysTrayMaxTextLen);

        #undef R
    }

    // check if xsystray is there
    if (!xstQuerySysTrayVersion(NULL, NULL, NULL))
        return FALSE;

    // initialize some constants
    WM_XST_CREATED = xstGetSysTrayCreatedMsgId();

    WM_TASKBARCREATED_W = RegisterWindowMessageA("TaskbarCreated");

    SYSTRAY_ItemInit = SYSTRAY_Ex_ItemInit;
    SYSTRAY_ItemTerm = SYSTRAY_Ex_ItemTerm;
    SYSTRAY_ItemUpdate = SYSTRAY_Ex_ItemUpdate;

    return TRUE;
}

