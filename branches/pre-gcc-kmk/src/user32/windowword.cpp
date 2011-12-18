/* $Id: windowword.cpp,v 1.13 2003-05-27 09:46:30 sandervl Exp $ */

/*
 * Win32 Get/SetWindowLong/Word user32 API functions for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <dbglog.h>
#include <ctrlconf.h>

#include <win32wbase.h>

#define DBG_LOCALLOG    DBG_windowword
#include "dbglocal.h"

//******************************************************************************
//Update the window style without sending WM_STYLECHANGING/ED
//******************************************************************************
void WIN_SetStyle(HWND hwnd, DWORD dwStyle)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        window->setStyle(dwStyle);
        RELEASE_WNDOBJ(window);
        return;
    }
    else {
        dprintf(("WIN_SetStyle window %x not found!", hwnd));
        return;
    }
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetWindowLongA(HWND hwnd, int nIndex, LONG  lNewLong)
{
 Win32BaseWindow *window;
 LONG ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        ret = window->SetWindowLong(nIndex,lNewLong, TYPE_ASCII);
        RELEASE_WNDOBJ(window);
        return ret;
    }
    else {
        dprintf(("SetWindowLongA %d %x; window %x not found!", nIndex, lNewLong, hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); //verified in NT4, SP6
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetWindowLongW(HWND hwnd, int nIndex, LONG  lNewLong)
{
 Win32BaseWindow *window;
 LONG ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        ret = window->SetWindowLong(nIndex,lNewLong, TYPE_UNICODE);
        RELEASE_WNDOBJ(window);
        return ret;
    }
    else {
        dprintf(("SetWindowLongW; window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); //verified in NT4, SP6
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
WORD WIN32API GetWindowWord(HWND hwnd, int nIndex)
{
 Win32BaseWindow *window;
 LONG ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        ret = window->GetWindowWord(nIndex);
        RELEASE_WNDOBJ(window);
        return ret;
    }
    else
    {
        dprintf(("GetWindowWord; window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); //verified in NT4, SP6
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
WORD WIN32API SetWindowWord(HWND hwnd, int nIndex, WORD arg3)
{
 Win32BaseWindow *window;
 LONG ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        ret = window->SetWindowWord(nIndex, arg3);
        RELEASE_WNDOBJ(window);
        return ret;
    }
    else
    {
        dprintf(("SetWindowWord; window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); //verified in NT4, SP6
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetWindowLongA(HWND hwnd, int nIndex)
{
 Win32BaseWindow *window;
 LONG ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        ret = window->GetWindowLong(nIndex, TYPE_ASCII);
        RELEASE_WNDOBJ(window);
        return ret;
    }
    else
    {
        dprintf(("GetWindowLongA; window %x (%d) not found!", hwnd, nIndex));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); //verified in NT4, SP6
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetWindowLongW(HWND hwnd, int nIndex)
{
 Win32BaseWindow *window;
 LONG ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        ret = window->GetWindowLong(nIndex, TYPE_UNICODE);
        RELEASE_WNDOBJ(window);
        return ret;
    }
    else
    {
        dprintf(("GetWindowLongW; window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); //verified in NT4, SP6
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
