/* $Id: defwndproc.cpp,v 1.7 1999-08-31 17:14:51 sandervl Exp $ */

/*
 * Win32 default window API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * TODO: Incomplete default window handlers + incorrect handler (defframe)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include "user32.h"
#include "syscolor.h"
#include "win32wbase.h"
#include "win32wnd.h"
#include "win32wmdichild.h"

#ifdef DEBUG
char *GetMsgText(int Msg);
#endif

//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefWindowProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefWindowProcA, window %x not found", hwnd));
        return 0;
    }
    return window->DefWindowProcA(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefWindowProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefWindowProcW, window %x not found", hwnd));
        return 0;
    }
    return window->DefWindowProcW(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefDlgProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DDP*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

    case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
        dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
        return 0;

        default:
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//NOTE: Unicode msg translation!
//******************************************************************************
LRESULT WIN32API DefDlgProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DDPW*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

    case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
        dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
        return 0;

        default:
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefFrameProcA(HWND hwndFrame, HWND hwndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;

    window = (Win32Window *)Win32BaseWindow::GetWindowFromHandle(hwndFrame);
    if(!window) {
        dprintf(("DefFrameProcA, window %x not found", hwndFrame));
        return 0;
    }
    return window->DefFrameProcA(hwndMDIClient, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefFrameProcW(HWND hwndFrame, HWND hwndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;

    window = (Win32Window *)Win32BaseWindow::GetWindowFromHandle(hwndFrame);
    if(!window) {
        dprintf(("DefFrameProcW, window %x not found", hwndFrame));
        return 0;
    }
    return window->DefFrameProcW(hwndMDIClient, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefMDIChildProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32MDIChildWindow *window;

    window = (Win32MDIChildWindow *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefMDIProcA, window %x not found", hwnd));
        return 0;
    }
    return window->DefMDIChildProcA(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefMDIChildProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32MDIChildWindow *window;

    window = (Win32MDIChildWindow *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefMDIProcW, window %x not found", hwnd));
        return 0;
    }
    return window->DefMDIChildProcW(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
