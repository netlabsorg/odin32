/* $Id: win32wfake.cpp,v 1.2 2003-03-27 10:42:42 sandervl Exp $ */
/*
 * Win32 Fake Window Class for OS/2
 *
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <dbglog.h>
#include <win\winproc.h>
#include <win32wbase.h>
#include <win32wfake.h>
#include "oslibwin.h"
#include "win32wndhandle.h"
#include "pmwindow.h"

#define DBG_LOCALLOG	DBG_win32wfake
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HWND WIN32API CreateFakeWindowEx(HWND  hwndOS2, ATOM  classAtom)
{
    Win32FakeWindow *window;

    window = new Win32FakeWindow(hwndOS2, classAtom);
    if(window == NULL)
    {
        dprintf(("Win32FakeWindow creation failed!!"));
        return 0;
    }
    HWND hwnd = window->getWindowHandle();
  
    // set myself as last active popup / window
    window->setLastActive( hwnd );
  
    RELEASE_WNDOBJ(window);
    return hwnd;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyFakeWindow(HWND hwnd)
{
    Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DestroyFakeWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    delete window;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32FakeWindow::Win32FakeWindow(HWND hwndOS2, ATOM classAtom)
                     : Win32BaseWindow()
{
    OS2Hwnd = OS2HwndFrame = hwndOS2;

    /* Find the window class */
    windowClass = Win32WndClass::FindClass(NULL, (LPSTR)classAtom);
    if (!windowClass)
    {
        char buffer[32];
        GlobalGetAtomNameA( classAtom, buffer, sizeof(buffer) );
        dprintf(("Bad class '%s'", buffer ));
        DebugInt3();
    }

    //Allocate window words
    nrUserWindowBytes = windowClass->getExtraWndBytes();
    if(nrUserWindowBytes) {
        userWindowBytes = (char *)_smalloc(nrUserWindowBytes);
        memset(userWindowBytes, 0, nrUserWindowBytes);
    }

    WINPROC_SetProc((HWINDOWPROC *)&win32wndproc, windowClass->getWindowProc((isUnicode) ? WNDPROC_UNICODE : WNDPROC_ASCII), WINPROC_GetProcType(windowClass->getWindowProc((isUnicode) ? WNDPROC_UNICODE : WNDPROC_ASCII)), WIN_PROC_WINDOW);
    hInstance  = NULL;
    dwStyle    = WS_VISIBLE;
    dwOldStyle = dwStyle;
    dwExStyle  = 0;

    //We pretend this window has no parent and its position is in screen coordinates
    OSLibWinQueryWindowClientRect(OS2Hwnd, &rectClient);
    OSLibQueryWindowRectAbsolute (OS2Hwnd, &rectWindow);

    setOldPMWindowProc(PMWinSubclassFakeWindow(OS2Hwnd));
}
//******************************************************************************
//******************************************************************************
Win32FakeWindow::~Win32FakeWindow()
{

}
//******************************************************************************
//******************************************************************************
PRECT Win32FakeWindow::getWindowRect()
{ 
    //the coordinates can change without us being notified, so recheck every time
    OSLibQueryWindowRectAbsolute (OS2Hwnd, &rectWindow);
    return &rectWindow; 
}
//******************************************************************************
//******************************************************************************
BOOL Win32FakeWindow::SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, 
                                   int cy, UINT fuFlags, BOOL fShowWindow)
{
    dprintf(("Win32FakeWindow::SetWindowPos; don't change window position, just update internal position"));

    //We pretend this window has no parent and its position is in screen coordinates
    OSLibWinQueryWindowClientRect(OS2Hwnd, &rectClient);
    OSLibQueryWindowRectAbsolute (OS2Hwnd, &rectWindow);

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32FakeWindow::isFakeWindow()
{
    return TRUE;
}
//******************************************************************************
//******************************************************************************
