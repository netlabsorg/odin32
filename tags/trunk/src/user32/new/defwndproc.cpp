/* $Id: defwndproc.cpp,v 1.9 1999-09-05 12:03:32 sandervl Exp $ */

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
#include "win32dlg.h"

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
  Win32Dialog *dialog;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog || !dialog->IsDialog()) {
        dprintf(("DefDlgProcA, window %x not found", hwnd));
        return 0;
    }
    return dialog->DefDlgProcA(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefDlgProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Dialog *dialog;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog || !dialog->IsDialog()) {
        dprintf(("DefDlgProcW, window %x not found", hwnd));
        return 0;
    }
    return dialog->DefDlgProcW(Msg, wParam, lParam);
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
