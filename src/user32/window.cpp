/* $Id: window.cpp,v 1.7 1999-09-24 22:45:28 sandervl Exp $ */
/*
 * Win32 window apis for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 * Parts based on Wine Windows code (windows\win.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 * TODO: Decide what to do about commands for OS/2 windows (non-Win32 apps)
 *
 */

#include <os2win.h>
#include <misc.h>
#include <win32wbase.h>
#include <win32wmdiclient.h>
#include <win32wdesktop.h>
#include <oslibwin.h>
#include <oslibgdi.h>
#include "user32.h"
#include "winicon.h"

//******************************************************************************
//******************************************************************************
HWND WIN32API CreateWindowExA(DWORD exStyle, LPCSTR className,
                              LPCSTR windowName, DWORD style, INT x,
                              INT y, INT width, INT height,
                              HWND parent, HMENU menu,
                              HINSTANCE instance, LPVOID data )
{
  Win32BaseWindow *window;
  ATOM classAtom;
  CREATESTRUCTA cs;

    if(exStyle & WS_EX_MDICHILD)
        return CreateMDIWindowA(className, windowName, style, x, y, width, height, parent, instance, (LPARAM)data);

    //TODO: According to the docs className can be a 16 bits atom
    //      Wine seems to assume it's a string though...
    /* Find the class atom */
    if (!(classAtom = GlobalFindAtomA(className)))
    {
        dprintf(("CreateWindowEx32A: bad class name "));
        if (!HIWORD(className)) {
                dprintf(("CreateWindowEx32A: bad class name %04x\n", LOWORD(className)));
        }
        else    dprintf(("CreateWindowEx32A: bad class name '%s'\n", className ));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    /* Create the window */
    cs.lpCreateParams = data;
    cs.hInstance      = instance;
    cs.hMenu          = menu;
    cs.hwndParent     = parent;
    cs.x              = x;
    cs.y              = y;
    cs.cx             = width;
    cs.cy             = height;
    cs.style          = style;
    cs.lpszName       = windowName;
    cs.lpszClass      = className;
    cs.dwExStyle      = exStyle;
    dprintf(("CreateWindowExA: parent %x (%d,%d) (%d,%d), %x %x", parent, x, y, width, height, style, exStyle));

    //TODO: According to the docs className can be a 16 bits atom
    //      Wine seems to assume it's a string though...
    if(!stricmp(className, MDICLIENTCLASSNAMEA)) {
        window = (Win32BaseWindow *) new Win32MDIClientWindow(&cs, classAtom, FALSE);
    }
    else {
        window = new Win32BaseWindow( &cs, classAtom, FALSE );
    }
    if(window == NULL)
    {
        dprintf(("Win32BaseWindow creation failed!!"));
        return 0;
    }
    if(GetLastError() != 0)
    {
        dprintf(("Win32BaseWindow error found!!"));
        delete window;
        return 0;
    }
    return window->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateWindowExW(DWORD exStyle, LPCWSTR className,
                              LPCWSTR windowName, DWORD style, INT x,
                              INT y, INT width, INT height,
                              HWND parent, HMENU menu,
                              HINSTANCE instance, LPVOID data )
{
  Win32BaseWindow *window;
  ATOM classAtom;
  CREATESTRUCTA cs;

    if(exStyle & WS_EX_MDICHILD)
        return CreateMDIWindowW(className, windowName, style, x, y, width, height, parent, instance, (LPARAM)data);

    //TODO: According to the docs className can be a 16 bits atom
    //      Wine seems to assume it's a string though...
    /* Find the class atom */
    if (!(classAtom = GlobalFindAtomW(className)))
    {
        dprintf(("CreateWindowEx32A: bad class name "));
        if (!HIWORD(className)) {
                dprintf(("CreateWindowEx32A: bad class name %04x\n", LOWORD(className)));
        }
//        else    dprintf(("CreateWindowEx32A: bad class name '%s'\n", className ));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    /* Create the window */
    cs.lpCreateParams = data;
    cs.hInstance      = instance;
    cs.hMenu          = menu;
    cs.hwndParent     = parent;
    cs.x              = x;
    cs.y              = y;
    cs.cx             = width;
    cs.cy             = height;
    cs.style          = style;
    cs.lpszName       = (LPSTR)windowName;
    cs.lpszClass      = (LPSTR)className;
    cs.dwExStyle      = exStyle;

    //TODO: According to the docs className can be a 16 bits atom
    //      Wine seems to assume it's a string though...
    if(!lstrcmpW(className, (LPWSTR)MDICLIENTCLASSNAMEW)) {
        window = (Win32BaseWindow *) new Win32MDIClientWindow(&cs, classAtom, TRUE);
    }
    else {
        window = new Win32BaseWindow( &cs, classAtom, TRUE );
    }
    if(window == NULL)
    {
        dprintf(("Win32BaseWindow creation failed!!"));
        return 0;
    }
    if(GetLastError() != 0)
    {
        dprintf(("Win32BaseWindow error found!!"));
        delete window;
        return 0;
    }
    return window->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateMDIWindowA(LPCSTR lpszClassName, LPCSTR lpszWindowName,
                               DWORD dwStyle, int x, int y, int nWidth,
                               int nHeight, HWND hwndParent,
                               HINSTANCE hInstance, LPARAM lParam )
{
 HWND hwnd;
 MDICREATESTRUCTA cs;
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwndParent);
    if(!window) {
        dprintf(("CreateMDIWindowA, window %x not found", hwndParent));
        return 0;
    }

    dprintf(("USER32:  CreateMDIWindowA\n"));
    cs.szClass        = lpszClassName;
    cs.szTitle        = lpszWindowName;
    cs.hOwner         = hInstance;
    cs.x              = x;
    cs.y              = y;
    cs.cx             = nHeight;
    cs.cy             = nWidth;
    cs.style          = dwStyle;
    cs.lParam         = lParam;

    return window->SendMessageA(WM_MDICREATE, 0, (LPARAM)&cs);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateMDIWindowW(LPCWSTR lpszClassName, LPCWSTR lpszWindowName,
                               DWORD dwStyle, int x, int y, int nWidth,
                               int nHeight, HWND hwndParent,
                               HINSTANCE hInstance, LPARAM lParam )
{
 HWND hwnd;
 MDICREATESTRUCTW cs;
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwndParent);
    if(!window) {
        dprintf(("CreateMDIWindowW, window %x not found", hwndParent));
        return 0;
    }

    dprintf(("USER32:  CreateMDIWindowW\n"));
    cs.szClass        = lpszClassName;
    cs.szTitle        = lpszWindowName;
    cs.hOwner         = hInstance;
    cs.x              = x;
    cs.y              = y;
    cs.cx             = nHeight;
    cs.cy             = nWidth;
    cs.style          = dwStyle;
    cs.lParam         = lParam;

    return window->SendMessageW(WM_MDICREATE, 0, (LPARAM)&cs);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyWindow(HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DestroyWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("DestroyWindow %x", hwnd));
    return window->DestroyWindow();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetActiveWindow( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetActiveWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("SetActiveWindow %x", hwnd));
    return window->SetActiveWindow();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetParent( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetParent, window %x not found", hwnd));
        return 0;
    }
    dprintf(("GetParent %x", hwnd));
    return window->GetParent();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetParent( HWND hwndChild, HWND hwndNewParent)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwndChild);
    if(!window) {
        dprintf(("SetParent, window %x not found", hwndChild));
        return 0;
    }
    dprintf(("SetParent %x %x", hwndChild, hwndNewParent));
    return window->SetParent(hwndNewParent);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsChild( HWND hwndParent, HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsChild, window %x not found", hwnd));
        return 0;
    }
    dprintf(("IsChild %x %x", hwndParent, hwnd));
    return window->IsChild(hwndParent);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetTopWindow( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetTopWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("GetTopWindow %x", hwnd));
    return window->GetTopWindow();
}
//******************************************************************************
//******************************************************************************
#if 0
BOOL WIN32API UpdateWindow(HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("UpdateWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("UpdateWindow %x", hwnd));
    return window->UpdateWindow();
}
#endif
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsIconic( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsIconic, window %x not found", hwnd));
        return 0;
    }
    dprintf(("IsIconic %x", hwnd));
    return window->IsIconic();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetWindow(HWND hwnd, UINT uCmd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("GetWindow %x %d", hwnd, uCmd));
    return window->GetWindow(uCmd);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableWindow( HWND hwnd, BOOL fEnable)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("EnableWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("EnableWindow %x %d", hwnd, fEnable));
    return window->EnableWindow(fEnable);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BringWindowToTop(HWND hwnd)
{
    return SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetActiveWindow()
{
  return Win32BaseWindow::GetActiveWindow();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowWindow(HWND hwnd, int nCmdShow)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("ShowWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("ShowWindow %x", hwnd));
    return window->ShowWindow(nCmdShow);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowPos(HWND hwnd, HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowPos, window %x not found", hwnd));
        return 0;
    }
    dprintf(("SetWindowPos %x %x x=%d y=%d cx=%d cy=%d %x", hwnd, hwndInsertAfter, x, y, cx, cy, fuFlags));
    return window->SetWindowPos(hwndInsertAfter, x, y, cx, cy, fuFlags);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowPlacement( HWND arg1, const WINDOWPLACEMENT *  arg2)
{
    dprintf(("USER32:  SetWindowPlacement\n"));
    return O32_SetWindowPlacement(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowPlacement( HWND arg1, LPWINDOWPLACEMENT arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowPlacement\n");
#endif
    return O32_GetWindowPlacement(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindow( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsWindow, window %x not found", hwnd));
        return FALSE;
    }
    dprintf(("IsWindow %x", hwnd));
    return window->IsWindow();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowEnabled( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsWindowEnabled, window %x not found", hwnd));
        return 0;
    }
    dprintf(("IsWindowEnabled %x", hwnd));
    return window->IsWindowEnabled();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowVisible( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsWindowVisible, window %x not found", hwnd));
        return 0;
    }
    dprintf(("IsWindowVisible %x", hwnd));
    return window->IsWindowVisible();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetFocus (HWND hwnd)
{
    HWND lastFocus, lastFocus_W, hwnd_O;
    BOOL activate;

    hwnd_O    = Win32BaseWindow::Win32ToOS2Handle (hwnd);
    lastFocus = OSLibWinQueryFocus (OSLIB_HWND_DESKTOP);
    activate  = ((hwnd_O == lastFocus) || OSLibWinIsChild (lastFocus, hwnd_O));
    lastFocus_W = Win32BaseWindow::OS2ToWin32Handle (lastFocus);

    dprintf(("USER32:  SetFocus %x (%x) -> %x (%x)\n", lastFocus_W, lastFocus, hwnd, hwnd_O));

    return (OSLibWinSetFocus (OSLIB_HWND_DESKTOP, hwnd_O, activate)) ? lastFocus_W : 0;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetFocus(void)
{
    HWND hwnd;
//    dprintf(("USER32:  GetFocus\n"));

    hwnd = OSLibWinQueryFocus(OSLIB_HWND_DESKTOP);
    return Win32BaseWindow::OS2ToWin32Handle(hwnd);
}
//******************************************************************************
//******************************************************************************
/***********************************************************************
 *           GetInternalWindowPos   (USER32.245)
 */
UINT WIN32API GetInternalWindowPos(HWND    hwnd,
                                   LPRECT  rectWnd,
                                   LPPOINT ptIcon )
{
    WINDOWPLACEMENT wndpl;

    dprintf(("USER32: GetInternalWindowPos(%08xh,%08xh,%08xh)\n",
             hwnd,
             rectWnd,
             ptIcon));

    if (GetWindowPlacement( hwnd, &wndpl ))
    {
        if (rectWnd) *rectWnd = wndpl.rcNormalPosition;
        if (ptIcon)  *ptIcon = wndpl.ptMinPosition;
        return wndpl.showCmd;
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsZoomed( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsZoomed\n");
#endif
    return O32_IsZoomed(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LockWindowUpdate( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  LockWindowUpdate\n");
#endif
    return O32_LockWindowUpdate(arg1);
}
//******************************************************************************
//******************************************************************************

#if 0
BOOL WIN32API RedrawWindow( HWND arg1, const RECT * arg2, HRGN arg3, UINT arg4)
{
 BOOL rc;

  rc = O32_RedrawWindow(arg1, arg2, arg3, arg4);
#ifdef DEBUG
  WriteLog("USER32:  RedrawWindow %X , %X, %X, %X returned %d\n", arg1, arg2, arg3, arg4, rc);
#endif
  InvalidateRect(arg1, arg2, TRUE);
  UpdateWindow(arg1);
  SendMessageA(arg1, WM_PAINT, 0, 0);
  return(rc);
}
#endif
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowRect( HWND hwnd, PRECT pRect)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowRect, window %x not found", hwnd));
        return 0;
    }
    dprintf(("GetWindowRect %x", hwnd));
    return window->GetWindowRect(pRect);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextLengthA( HWND hwnd)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowTextLength, window %x not found", hwnd));
        return 0;
    }
    dprintf(("GetWindowTextLength %x", hwnd));
    return window->GetWindowTextLength();
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextA( HWND hwnd, LPSTR lpsz, int cch)
{
   Win32BaseWindow *window;
   int rc;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowTextA, window %x not found", hwnd));
        return 0;
    }
    rc = window->GetWindowTextA(lpsz, cch);
    dprintf(("GetWindowTextA %x %s", hwnd, lpsz));
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextLengthW( HWND hwnd)
{
    dprintf(("USER32:  GetWindowTextLengthW\n"));
    return GetWindowTextLengthA(hwnd);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextW(HWND hwnd, LPWSTR lpsz, int cch)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowTextW, window %x not found", hwnd));
        return 0;
    }
    dprintf(("GetWindowTextW %x", hwnd));
    return window->GetWindowTextW(lpsz, cch);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowTextA(HWND hwnd, LPCSTR lpsz)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowTextA, window %x not found", hwnd));
        return 0;
    }
    dprintf(("SetWindowTextA %x %s", hwnd, lpsz));
    return window->SetWindowTextA((LPSTR)lpsz);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowTextW( HWND hwnd, LPCWSTR lpsz)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowTextA, window %x not found", hwnd));
        return 0;
    }
    dprintf(("SetWindowTextW %x", hwnd));
    return window->SetWindowTextW((LPWSTR)lpsz);
}
/*******************************************************************
 *      InternalGetWindowText    (USER32.326)
 */
int WIN32API InternalGetWindowText(HWND   hwnd,
                                   LPWSTR lpString,
                                   INT    nMaxCount )
{
    dprintf(("USER32: InternalGetWindowText(%08xh,%08xh,%08xh) not properly implemented.\n",
             hwnd,
             lpString,
             nMaxCount));

    return GetWindowTextW(hwnd,lpString,nMaxCount);
}
//******************************************************************************
//TODO: Correct?
//******************************************************************************
BOOL WIN32API SetForegroundWindow(HWND hwnd)
{
    dprintf((" SetForegroundWindow %x", hwnd));

    return SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClientRect( HWND hwnd, PRECT pRect)
{
 BOOL rc;

    hwnd = Win32BaseWindow::Win32ToOS2Handle(hwnd);
    rc = OSLibWinQueryWindowRect(hwnd, pRect);
    dprintf(("USER32:  GetClientRect of %X returned (%d,%d) (%d,%d)\n", hwnd, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AdjustWindowRect( PRECT arg1, DWORD arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  AdjustWindowRect\n");
#endif
    return O32_AdjustWindowRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AdjustWindowRectEx( PRECT arg1, DWORD arg2, BOOL arg3, DWORD  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  AdjustWindowRectEx\n");
#endif
    return O32_AdjustWindowRectEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetDesktopWindow(void)
{
    dprintf(("USER32:  GetDesktopWindow\n"));
    return windowDesktop->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowA(LPCSTR lpszClass, LPCSTR lpszWindow)
{
    if(!lpszClass) {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
    }
    if(HIWORD(lpszClass)) {
         dprintf(("USER32:  FindWindow %s %s\n", lpszClass, lpszWindow));
    }
    else dprintf(("USER32:  FindWindow %x %s\n", lpszClass, lpszWindow));

    return Win32BaseWindow::FindWindowEx(OSLIB_HWND_DESKTOP, 0, (LPSTR)lpszClass, (LPSTR)lpszWindow);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowExA(HWND hwndParent, HWND hwndChildAfter, LPCSTR lpszClass, LPCSTR lpszWindow)
{
    if(!lpszClass) {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
    }
    if(HIWORD(lpszClass)) {
         dprintf(("USER32:  FindWindowExA (%x,%x) %s %s\n", hwndParent, hwndChildAfter, lpszClass, lpszWindow));
    }
    else dprintf(("USER32:  FindWindowExA (%x,%x)%x %s\n", hwndParent, hwndChildAfter, lpszClass, lpszWindow));

    return Win32BaseWindow::FindWindowEx(hwndParent, hwndChildAfter, (LPSTR)lpszClass, (LPSTR)lpszWindow);
}
/*****************************************************************************
 * Name      : HWND WIN32API FindWindowExW
 * Purpose   : The FindWindowEx function retrieves the handle of a window whose
 *             class name and window name match the specified strings. The
 *             function searches child windows, beginning with the one following
 *             the given child window.
 * Parameters: HWND    hwndParent     handle of parent window
 *             HWND    hwndChildAfter handle of a child window
 *             LPCTSTR lpszClass      address of class name
 *             LPCTSTR lpszWindow     address of window name
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               window that has the specified class and window names.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HWND WIN32API FindWindowExW(HWND    hwndParent,
                               HWND    hwndChildAfter,
                               LPCWSTR lpszClass,
                               LPCWSTR lpszWindow)
{
    if(!lpszClass) {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
    }
    dprintf(("USER32:  FindWindowExW (%x,%x) %x %s\n", hwndParent, hwndChildAfter, lpszClass, lpszWindow));

    return Win32BaseWindow::FindWindowEx(hwndParent, hwndChildAfter, (LPSTR)lpszClass, (LPSTR)lpszWindow);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlashWindow(HWND hwnd, BOOL fFlash)
{
    dprintf(("FlashWindow %x %d\n", hwnd, fFlash));
    return OSLibWinFlashWindow(Win32BaseWindow::Win32ToOS2Handle(hwnd), fFlash);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveWindow( HWND hwnd, INT x, INT y, INT cx, INT cy,
                          BOOL repaint )
{
    int flags = SWP_NOZORDER | SWP_NOACTIVATE;

    if (!repaint) flags |= SWP_NOREDRAW;
    dprintf(("MoveWindow: %04x %d,%d %dx%d %d\n", hwnd, x, y, cx, cy, repaint ));

    return SetWindowPos( hwnd, 0, x, y, cx, cy, flags );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ClientToScreen( HWND arg1, PPOINT  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  ClientToScreen\n");
#endif
    return O32_ClientToScreen(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDWP WIN32API BeginDeferWindowPos( int arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  BeginDeferWindowPos\n");
#endif
    return O32_BeginDeferWindowPos(arg1);
}
//******************************************************************************
//******************************************************************************
HDWP WIN32API DeferWindowPos( HDWP arg1, HWND arg2, HWND  arg3, int arg4, int arg5, int arg6, int arg7, UINT  arg8)
{
#ifdef DEBUG
    WriteLog("USER32:  DeferWindowPos\n");
#endif
    return O32_DeferWindowPos(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API ChildWindowFromPoint( HWND arg1, POINT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ChildWindowFromPoint\n");
#endif
    return O32_ChildWindowFromPoint(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
/*****************************************************************************
 * Name      : HWND WIN32API ChildWindowFromPointEx
 * Purpose   : The GetWindowRect function retrieves the dimensions of the
 *             bounding rectangle of the specified window. The dimensions are
 *             given in screen coordinates that are relative to the upper-left
 *             corner of the screen.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the window handle.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : FULLY IMPLEMENTED AND TESTED
 *
 * Author    : Rene Pronk [Sun, 1999/08/08 23:30]
 *****************************************************************************/

HWND WIN32API ChildWindowFromPointEx (HWND hwndParent, POINT pt, UINT uFlags)
{
        RECT rect;
        HWND hWnd;
        POINT absolutePt;

        dprintf(("USER32: ChildWindowFromPointEx(%08xh,%08xh,%08xh).\n",
                 hwndParent, pt, uFlags));

        if (GetWindowRect (hwndParent, &rect) == 0) {
                // oops, invalid handle
                return NULL;
        }

        // absolutePt has its top in the upper-left corner of the screen
        absolutePt = pt;
        ClientToScreen (hwndParent, &absolutePt);

        // make rect the size of the parent window
        GetWindowRect (hwndParent, &rect);
        rect.right = rect.right - rect.left;
        rect.bottom = rect.bottom - rect.top;
        rect.left = 0;
        rect.top = 0;

        if (PtInRect (&rect, pt) == 0) {
                // point is outside window
                return NULL;
        }

        // get first child
        hWnd = GetWindow (hwndParent, GW_CHILD);

        while (hWnd != NULL) {

                // do I need to skip this window?
                if (((uFlags & CWP_SKIPINVISIBLE) &&
                     (IsWindowVisible (hWnd) == FALSE)) ||
                    ((uFlags & CWP_SKIPDISABLED) &&
                     (IsWindowEnabled (hWnd) == FALSE)) ||
                    ((uFlags & CWP_SKIPTRANSPARENT) &&
                     (GetWindowLongA (hWnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT)))

                {
                        hWnd = GetWindow (hWnd, GW_HWNDNEXT);
                        continue;
                }

                // is the point in this window's rect?
                GetWindowRect (hWnd, &rect);
                if (PtInRect (&rect, absolutePt) == FALSE) {
                        hWnd = GetWindow (hWnd, GW_HWNDNEXT);
                        continue;
                }

                // found it!
                return hWnd;
        }

        // the point is in the parentwindow but the parentwindow has no child
        // at this coordinate
        return hwndParent;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseWindow(HWND hwnd)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("CloseWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("CloseWindow %x\n", hwnd));
    return window->CloseWindow();
}
//******************************************************************************
//TODO: Does this return handles of hidden or disabled windows?
//******************************************************************************
HWND WIN32API WindowFromPoint( POINT point)
{
 HWND hwnd;

    dprintf(("WindowFromPoint (%d,%d)\n", point.x, point.y));
    hwnd = OSLibWinWindowFromPoint(OSLIB_HWND_DESKTOP, (PVOID)&point);
    if(hwnd) {
        return Win32BaseWindow::OS2ToWin32Handle(hwnd);
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowUnicode(HWND hwnd)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsWindowUnicode, window %x not found", hwnd));
        return 0;
    }
    return window->IsUnicode();
}
/*****************************************************************************
 * Name      : WORD WIN32API CascadeWindows
 * Purpose   : The CascadeWindows function cascades the specified windows or
 *             the child windows of the specified parent window.
 * Parameters: HWND hwndParent         handle of parent window
 *             UINT wHow               types of windows not to arrange
 *             CONST RECT * lpRect     rectangle to arrange windows in
 *             UINT cKids              number of windows to arrange
 *             const HWND FAR * lpKids array of window handles
 * Variables :
 * Result    : If the function succeeds, the return value is the number of windows arranged.
 *             If the function fails, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

WORD WIN32API CascadeWindows(HWND       hwndParent,
                             UINT       wHow,
                             CONST LPRECT lpRect,
                             UINT       cKids,
                             const HWND *lpKids)
{
  dprintf(("USER32:CascadeWindows(%08xh,%u,%08xh,%u,%08x) not implemented.\n",
         hwndParent,
         wHow,
         lpRect,
         cKids,
         lpKids));

  return (0);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SwitchToThisWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API SwitchToThisWindow(HWND hwnd,
                                 BOOL x2)
{
  dprintf(("USER32: SwitchToThisWindow(%08xh,%08xh) not implemented.\n",
           hwnd,
           x2));

  return (FALSE); /* default */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumThreadWindows(DWORD dwThreadId, WNDENUMPROC lpfn, LPARAM lParam)
{
 Win32BaseWindow *window;
 BOOL   rc;
 ULONG  henum;
 HWND   hwndNext;
 ULONG  tid;
 ULONG  pid, curpid;

  dprintf(("EnumThreadWindows\n"));

  curpid = GetCurrentProcessId();

  henum = OSLibWinBeginEnumWindows(OSLIB_HWND_DESKTOP);
  while ((hwndNext = OSLibWinGetNextWindow(henum)) != 0)
  {
        OSLibWinQueryWindowProcess(hwndNext, &pid, &tid);
        if(!(curpid == pid && dwThreadId == tid))
                continue;

        window = Win32BaseWindow::GetWindowFromHandle(hwndNext);
        if(window == NULL) {
                //OS/2 window or non-frame window, so skip it
                continue;
        }
        if((rc = lpfn(window->getWindowHandle(), lParam)) == FALSE)
                break;
  }
  OSLibWinEndEnumWindows (henum);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumChildWindows(HWND hwnd, WNDENUMPROC lpfn, LPARAM lParam)
{
 Win32BaseWindow *window, *parentwindow;
 BOOL   rc = TRUE;
 ULONG  henum;
 HWND   hwndNext;

  dprintf(("EnumChildWindows %x %x\n", hwnd, lParam));

  parentwindow = Win32BaseWindow::GetWindowFromHandle(hwnd);
  if(!parentwindow) {
        dprintf(("EnumChildWindows, window %x not found", hwnd));
        return FALSE;
  }

  henum = OSLibWinBeginEnumWindows(OSLIB_HWND_DESKTOP);
  while ((hwndNext = OSLibWinGetNextWindow(henum)) != 0)
  {
        window = Win32BaseWindow::GetWindowFromHandle(hwndNext);
        if(window == NULL) {
                //OS/2 window or non-frame window, so skip it
                continue;
        }
        if((rc = lpfn(window->getWindowHandle(), lParam)) == FALSE)
        {
                rc = FALSE;
                break;
        }
  }
  OSLibWinEndEnumWindows(henum);
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumWindows(WNDENUMPROC lpfn, LPARAM lParam)
{
 Win32BaseWindow *window;
 BOOL   rc;
 ULONG  henum;
 HWND   hwndNext, hwndParent = OSLIB_HWND_DESKTOP;

  dprintf(("EnumThreadWindows\n"));

  do {
        henum = OSLibWinBeginEnumWindows(hwndParent);
        while ((hwndNext = OSLibWinGetNextWindow(henum)) != 0)
        {
                window = Win32BaseWindow::GetWindowFromHandle(hwndNext);
                if(window == NULL) {
                        //OS/2 window or non-frame window, so skip it
                        continue;
                }
                if((rc = lpfn(window->getWindowHandle(), lParam)) == FALSE) {
                        goto Abort;
                }
        }
        if(hwndParent == OSLIB_HWND_OBJECT)
                break;
        hwndParent = OSLIB_HWND_OBJECT;
        OSLibWinEndEnumWindows(henum);
  }
  while(TRUE);

Abort:
  OSLibWinEndEnumWindows(henum);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
#if 0
BOOL WIN32API GetUpdateRect( HWND hwnd, PRECT lpRect, BOOL  bErase)
{
    dprintf(("GetUpdateRect %x %d\n", hwnd, bErase));
    if (!lpRect) return FALSE;

    return OSLibWinQueryUpdateRect(Win32BaseWindow::Win32ToOS2Handle(hwnd), lpRect);
}
#endif
//******************************************************************************
//******************************************************************************
#if 0
BOOL WIN32API InvalidateRect(HWND hWnd, const RECT *lpRect, BOOL bErase)
{
#ifdef DEBUG
    if(lpRect)
         WriteLog("USER32:  InvalidateRect for window %X (%d,%d)(%d,%d) %d\n", hWnd, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, bErase);
    else WriteLog("USER32:  InvalidateRect for window %X NULL, %d\n", hWnd, bErase);
#endif

    //CB: bErase no quite the same
    hWnd = Win32BaseWindow::Win32ToOS2Handle(hWnd);
    if (lpRect)
    {
         return OSLibWinInvalidateRect(hWnd, (PRECT)lpRect, bErase);
    }
    else return OSLibWinInvalidateRect(hWnd,NULL,bErase);
}
#endif
//******************************************************************************
//******************************************************************************
UINT WIN32API ArrangeIconicWindows( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  ArrangeIconicWindows\n");
#endif
    return O32_ArrangeIconicWindows(arg1);
}
//******************************************************************************
//restores iconized window to previous size/position
//******************************************************************************
BOOL WIN32API OpenIcon(HWND hwnd)
{
#ifdef DEBUG
  WriteLog("USER32:  OpenIcon\n");
#endif
  if(!IsIconic(hwnd))
        return FALSE;
  ShowWindow(hwnd, SW_SHOWNORMAL);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowOwnedPopups( HWND arg1, BOOL  arg2)
{
    dprintf(("USER32:  ShowOwnedPopups\n"));
    return O32_ShowOwnedPopups(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
