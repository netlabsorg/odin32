/*
 *  OS/2 System Tray support.
 *  A better implementaiton using the xsystray XCenter/eCenter widget API.
 *
 *  Author: Dmitriy Kuminov
 */

#define  INCL_WIN
#define  INCL_DOS
#define  INCL_DOSERRORS
#include <os2wrap.h>

#include <string.h>

#include <odin.h>
#include <winconst.h>

// declare function pointers for dynamic linking to xsystray DLL
#define XSTAPI_FPTRS_STATIC
#include <xsystray.h>

#include "systray_os2.h"

#define WM_XST_MYNOTIFY (WM_USER + 1000)

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
                DoWin32PostMessage(ptrayItem->notifyIcon.hWnd,
                                   ptrayItem->notifyIcon.uCallbackMessage,
                                   (MPARAM)ptrayItem->notifyIcon.uID,
                                   (MPARAM)winMsg);
            }

            return (MRESULT)FALSE;
        }
        default:
            break;
        }
    }

    default:
        break;
    }

    return OldProxyWndProc(hWnd, msg, mp1, mp2);
}

static BOOL SYSTRAY_Ex_ItemInit(SystrayItem *ptrayItem)
{
    if (hwndProxyRefs == 0)
    {
        ULONG fcf = 0;
        hwndProxy = WinCreateStdWindow(HWND_DESKTOP, 0, &fcf, NULL,
                                       NULL, 0, NULLHANDLE, 0, NULL);
        if (hwndProxy == NULLHANDLE)
            return FALSE;

        OldProxyWndProc = WinSubclassWindow(hwndProxy, ProxyWndProc);
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

    SYSTRAY_ItemInit = SYSTRAY_Ex_ItemInit;
    SYSTRAY_ItemTerm = SYSTRAY_Ex_ItemTerm;
    SYSTRAY_ItemUpdate = SYSTRAY_Ex_ItemUpdate;

    return TRUE;
}

