/* $Id: defwndproc.cpp,v 1.15 2001-06-09 14:50:17 sandervl Exp $ */

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

#define DBG_LOCALLOG	DBG_defwndproc
#include "dbglocal.h"

#ifdef DEBUG
char *GetMsgText(int Msg);
#endif

//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefWindowProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;
  LRESULT result;

    dprintf2(("DefWindowProcA %x %x %x %x", hwnd, Msg, wParam, lParam));
    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefWindowProcA, window %x not found", hwnd));
        return 0;
    }
    result = window->DefWindowProcA(Msg, wParam, lParam);
    RELEASE_WNDOBJ(window);
    return result;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefWindowProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;
  LRESULT result;

    dprintf2(("DefWindowProcW %x %x %x %x", hwnd, Msg, wParam, lParam));
    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefWindowProcW, window %x not found", hwnd));
        return 0;
    }
    result = window->DefWindowProcW(Msg, wParam, lParam);
    RELEASE_WNDOBJ(window);
    return result;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefDlgProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Dialog *dialog;
  LRESULT result;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
//TODO: Wrong check?
//    if(!dialog || !dialog->IsDialog()) {
    if(!dialog) {
        dprintf(("DefDlgProcA, window %x not found", hwnd));
        return 0;
    }
    if(dialog->IsDialog()) 
    	 result = dialog->DefDlgProcA(Msg, wParam, lParam);
    else result = dialog->DefWindowProcA(Msg, wParam, lParam);
    RELEASE_WNDOBJ(dialog);
    return result;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefDlgProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Dialog *dialog;
  LRESULT result;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
//TODO: Wrong check?
//    if(!dialog || !dialog->IsDialog()) {
    if(!dialog) {
        dprintf(("DefDlgProcW, window %x not found", hwnd));
        return 0;
    }
    if(dialog->IsDialog()) 
    	 result = dialog->DefDlgProcW(Msg, wParam, lParam);
    else result = dialog->DefWindowProcW(Msg, wParam, lParam);
    RELEASE_WNDOBJ(dialog);
    return result;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefFrameProcA(HWND hwndFrame, HWND hwndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;
  LRESULT result;

    window = (Win32Window *)Win32BaseWindow::GetWindowFromHandle(hwndFrame);
    if(!window) {
        dprintf(("DefFrameProcA, window %x not found", hwndFrame));
        return 0;
    }
    result = window->DefFrameProcA(hwndMDIClient, Msg, wParam, lParam);
    RELEASE_WNDOBJ(window);
    return result;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefFrameProcW(HWND hwndFrame, HWND hwndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;
  LRESULT result;

    window = (Win32Window *)Win32BaseWindow::GetWindowFromHandle(hwndFrame);
    if(!window) {
        dprintf(("DefFrameProcW, window %x not found", hwndFrame));
        return 0;
    }
    result = window->DefFrameProcW(hwndMDIClient, Msg, wParam, lParam);
    RELEASE_WNDOBJ(window);
    return result;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefMDIChildProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32MDIChildWindow *window;
  LRESULT result;

    window = (Win32MDIChildWindow *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefMDIProcA, window %x not found", hwnd));
        return 0;
    }
    if(!window->isMDIChild()) {
	 dprintf(("App called DefMDIChildProcA for non-MDI window %x", hwnd));
 	 result = window->DefWindowProcA(Msg, wParam, lParam);
    }
    else result = window->DefMDIChildProcA(Msg, wParam, lParam);
    RELEASE_WNDOBJ(window);
    return result;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefMDIChildProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32MDIChildWindow *window;
  LRESULT result;

    window = (Win32MDIChildWindow *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DefMDIProcW, window %x not found", hwnd));
        return 0;
    }
    if(!window->isMDIChild()) {
	 dprintf(("App called DefMDIChildProcA for non-MDI window %x", hwnd));
 	 result = window->DefWindowProcW(Msg, wParam, lParam);
    }
    else result = window->DefMDIChildProcW(Msg, wParam, lParam);
    RELEASE_WNDOBJ(window);
    return result;
}
//******************************************************************************
//******************************************************************************
