/* $Id: defwndproc.cpp,v 1.16 2001-09-19 15:39:47 sandervl Exp $ */

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
