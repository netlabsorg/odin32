/* $Id: windowword.cpp,v 1.10 2001-02-03 18:52:03 sandervl Exp $ */

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
#include <misc.h>

#include <win32wbase.h>

#define DBG_LOCALLOG    DBG_windowword
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
LONG WIN32API SetWindowLongA(HWND hwnd, int nIndex, LONG  lNewLong)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->SetWindowLongA(nIndex,lNewLong);
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

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->SetWindowLongA(nIndex,lNewLong, TRUE);
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

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->GetWindowWord(nIndex);
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

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->SetWindowWord(nIndex, arg3);
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

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->GetWindowLongA(nIndex);
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

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(window)
    {
        return window->GetWindowLongA(nIndex, TRUE);
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
