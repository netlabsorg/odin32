/* $Id: window.cpp,v 1.20 1999-10-17 20:18:46 sandervl Exp $ */
/*
 * Win32 window apis for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
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
#include <string.h>
#include <stdio.h>
#include <win32wbase.h>
#include <win32wmdiclient.h>
#include <win32wdesktop.h>
#include "win32dlg.h"
#include <oslibwin.h>
#include <oslibgdi.h>
#include "user32.h"
#include "winicon.h"
#include <win\winpos.h>

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
  char tmpClass[20];

    if(exStyle & WS_EX_MDICHILD)
        return CreateMDIWindowA(className, windowName, style, x, y, width, height, parent, instance, (LPARAM)data);

    /* Find the class atom */
    if (!(classAtom = GlobalFindAtomA(className)))
    {
        if (!HIWORD(className))
        {
          sprintf(tmpClass,"#%d", (int) className);
          classAtom = GlobalFindAtomA(tmpClass);
          className = tmpClass;
        }
        if (!classAtom)
        {
          if (!HIWORD(className)) {
                  dprintf(("CreateWindowEx32A: bad class name %04x\n", LOWORD(className)));
          }
          else    dprintf(("CreateWindowEx32A: bad class name '%s'\n", className ));

          SetLastError(ERROR_INVALID_PARAMETER);
          return 0;
        }
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
    if(HIWORD(className)) {
         dprintf(("CreateWindowExA: class %s parent %x (%d,%d) (%d,%d), %x %x", className, parent, x, y, width, height, style, exStyle));
    }
    else dprintf(("CreateWindowExA: class %d parent %x (%d,%d) (%d,%d), %x %x", className, parent, x, y, width, height, style, exStyle));

    if(!strcmpi(className, MDICLIENTCLASSNAMEA)) {
        window = (Win32BaseWindow *) new Win32MDIClientWindow(&cs, classAtom, FALSE);
    }
    else
    if(!strcmpi((char *) className, DIALOG_CLASS_NAMEA))
    {
      DLG_TEMPLATE dlgTemplate = {0};
      dlgTemplate.style = cs.style;
      dlgTemplate.exStyle = cs.dwExStyle;
      dlgTemplate.x = cs.x;
      dlgTemplate.y = cs.y;
      dlgTemplate.cx = cs.cx;
      dlgTemplate.cy = cs.cy;
      dlgTemplate.className = cs.lpszClass;
      dlgTemplate.caption = cs.lpszName;
      window = (Win32BaseWindow *) new Win32Dialog(cs.hInstance,
                                                   (LPCSTR) &dlgTemplate,
                                                   cs.hwndParent,
                                                   NULL,
                                                   (LPARAM) data,
                                                   FALSE);
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
  char tmpClassA[20];
  WCHAR tmpClassW[20];

    if(exStyle & WS_EX_MDICHILD)
        return CreateMDIWindowW(className, windowName, style, x, y, width, height, parent, instance, (LPARAM)data);

    /* Find the class atom */
    if (!(classAtom = GlobalFindAtomW(className)))
    {
        if (!HIWORD(className))
        {
          sprintf(tmpClassA,"#%d", (int) className);
          AsciiToUnicode(tmpClassA, tmpClassW);
          classAtom = GlobalFindAtomW(tmpClassW);
          className = (LPCWSTR)tmpClassW;
        }
        if (!classAtom)
        {
          if (!HIWORD(className)) {
                  dprintf(("CreateWindowEx32W: bad class name %04x\n", LOWORD(className)));
          }
      else    dprintf(("CreateWindowEx32W: bad class name "));

          SetLastError(ERROR_INVALID_PARAMETER);
          return 0;
        }
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

    if(!lstrcmpiW(className, (LPWSTR)MDICLIENTCLASSNAMEW)) {
        window = (Win32BaseWindow *) new Win32MDIClientWindow(&cs, classAtom, TRUE);
    }
    else
    if(!lstrcmpiW(className, (LPWSTR)DIALOG_CLASS_NAMEW))
    {
      DLG_TEMPLATE dlgTemplate = {0};
      dlgTemplate.style = cs.style;
      dlgTemplate.exStyle = cs.dwExStyle;
      dlgTemplate.x = cs.x;
      dlgTemplate.y = cs.y;
      dlgTemplate.cx = cs.cx;
      dlgTemplate.cy = cs.cy;
      dlgTemplate.className = cs.lpszClass;
      dlgTemplate.caption = cs.lpszName;
      window = (Win32BaseWindow *) new Win32Dialog(cs.hInstance,
                                                   (LPCSTR) &dlgTemplate,
                                                   cs.hwndParent,
                                                   NULL,
                                                   (LPARAM) data,
                                                   TRUE);
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
    SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    return window->GetTopWindow();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsIconic( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsIconic, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
  HWND rc;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    rc = window->GetWindow(uCmd);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableWindow( HWND hwnd, BOOL fEnable)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("EnableWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
    SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("SetWindowPos %x %x x=%d y=%d cx=%d cy=%d %x", hwnd, hwndInsertAfter, x, y, cx, cy, fuFlags));
    return window->SetWindowPos(hwndInsertAfter, x, y, cx, cy, fuFlags);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowPlacement(HWND hwnd, const WINDOWPLACEMENT *winpos)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowPlacement, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    return window->SetWindowPlacement((WINDOWPLACEMENT *)winpos);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowPlacement(HWND hwnd, LPWINDOWPLACEMENT arg2)
{
    dprintf(("USER32:  GetWindowPlacement\n"));
    return O32_GetWindowPlacement(Win32BaseWindow::Win32ToOS2FrameHandle(hwnd), arg2);
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
  BOOL rc;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsWindowVisible, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    rc = window->IsWindowVisible();
    dprintf(("IsWindowVisible %x returned %d", hwnd, rc));
    return rc;
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

    dprintf(("SetFocus %x (%x) -> %x (%x)\n", lastFocus_W, lastFocus, hwnd, hwnd_O));

    return (OSLibWinSetFocus (OSLIB_HWND_DESKTOP, hwnd_O, activate)) ? lastFocus_W : 0;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetFocus(void)
{
    HWND hwnd;

    hwnd = OSLibWinQueryFocus(OSLIB_HWND_DESKTOP);
    dprintf(("USER32: GetFocus %x\n", hwnd));
    hwnd = Win32BaseWindow::OS2ToWin32Handle(hwnd);
    return hwnd;
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
BOOL WIN32API IsZoomed(HWND hwnd)
{
    dprintf(("USER32:  IsZoomed\n"));
    return O32_IsZoomed(Win32BaseWindow::Win32ToOS2FrameHandle(hwnd));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LockWindowUpdate(HWND hwnd)
{
    dprintf(("USER32:  LockWindowUpdate\n"));
    return O32_LockWindowUpdate(Win32BaseWindow::Win32ToOS2FrameHandle(hwnd));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowRect( HWND hwnd, PRECT pRect)
{
  Win32BaseWindow *window;
  BOOL rc;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowRect, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    rc = window->GetWindowRect(pRect);
    dprintf(("GetWindowRect %x (%d,%d) (%d,%d)", hwnd, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextLengthA( HWND hwnd)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowTextLength, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
 HWND hwndWin32 = hwnd;

#if 1
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetClientRect, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    *pRect = *window->getClientRect();
    OffsetRect(pRect, -pRect->left, -pRect->top);
    dprintf(("GetClientRect of %X returned (%d,%d) (%d,%d)\n", hwndWin32, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return TRUE;
#else
    hwnd = Win32BaseWindow::Win32ToOS2Handle(hwnd);
    rc = OSLibWinQueryWindowRect(hwnd, pRect);
    dprintf(("GetClientRect of %X returned (%d,%d) (%d,%d)\n", hwndWin32, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return rc;
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AdjustWindowRect(PRECT rect, DWORD style, BOOL menu)
{
    return AdjustWindowRectEx(rect, style, menu, 0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AdjustWindowRectEx( PRECT rect, DWORD style, BOOL menu, DWORD exStyle)
{
    dprintf(("AdjustWindowRectEx %x %x %d (%d,%d)(%d,%d)\n", style, exStyle, menu, rect->right, rect->top, rect->left, rect->bottom));

#if 0
    O32_AdjustWindowRectEx(rect, style, menu, exStyle);
#else
    /* Correct the window style */
    if (!(style & (WS_POPUP | WS_CHILD)))  /* Overlapped window */
    style |= WS_CAPTION;

    style &= (WS_DLGFRAME | WS_BORDER | WS_THICKFRAME | WS_CHILD | WS_VSCROLL | WS_HSCROLL);
    exStyle &= (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE |
        WS_EX_STATICEDGE | WS_EX_TOOLWINDOW);
    if (exStyle & WS_EX_DLGMODALFRAME) style &= ~WS_THICKFRAME;

    Win32BaseWindow::NC_AdjustRectOuter( rect, style, menu, exStyle );
    Win32BaseWindow::NC_AdjustRectInner( rect, style, exStyle );
#endif

    dprintf(("AdjustWindowRectEx returned (%d,%d)(%d,%d)\n", rect->right, rect->top, rect->left, rect->bottom));
    return TRUE;
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
HWND WIN32API FindWindowW( LPCWSTR lpClassName, LPCWSTR lpWindowName)
{
 char *astring1 = UnicodeToAsciiString((LPWSTR)lpClassName);
 char *astring2 = UnicodeToAsciiString((LPWSTR)lpWindowName);
 HWND rc;

    rc = FindWindowA(astring1, astring2);
    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    return rc;
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
BOOL WIN32API ClientToScreen (HWND hwnd, PPOINT pt)
{
    Win32BaseWindow *wnd;
    PRECT rcl;

    dprintf(("USER32:  ClientToScreen\n"));
    if (!hwnd) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (FALSE);
    }
    wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
    if (!wnd) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return (FALSE);
    }

    rcl  = wnd->getClientRect();
    pt->y = (rcl->bottom - rcl->top) - pt->y;
    OSLibWinMapWindowPoints (wnd->getOS2WindowHandle(), OSLIB_HWND_DESKTOP, (OSLIBPOINT *)pt, 1);
    pt->y = ScreenHeight - pt->y;
    return (TRUE);
}
//******************************************************************************
//******************************************************************************
HDWP WIN32API BeginDeferWindowPos(int count)
{
    HDWP handle;
    DWP *pDWP;

    dprintf(("USER32:  BeginDeferWindowPos\n"));
    if (count <= 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    handle = (HDWP)HeapAlloc(GetProcessHeap(), 0, sizeof(DWP) + (count-1)*sizeof(WINDOWPOS) );
    if (!handle)
        return 0;

    pDWP = (DWP *) handle;
    pDWP->actualCount    = 0;
    pDWP->suggestedCount = count;
    pDWP->valid          = TRUE;
    pDWP->wMagic         = DWP_MAGIC;
    pDWP->hwndParent     = 0;
    return handle;
}
/***********************************************************************
 *           DeferWindowPos   (USER32.128)
 */
HDWP WIN32API DeferWindowPos( HDWP hdwp, HWND hwnd, HWND hwndAfter,
                              INT x, INT y, INT cx, INT cy,
                              UINT flags )
{
  DWP *pDWP;
  int i;
  HDWP newhdwp = hdwp,retvalue;
  Win32BaseWindow *window;

    pDWP = (DWP *)hdwp;
    if (!pDWP) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (hwnd == GetDesktopWindow())
        return 0;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DeferWindowPos, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        HeapFree(GetProcessHeap(), 0, (LPVOID)hdwp);
        return 0;
    }


/* Numega Bounds Checker Demo dislikes the following code.
   In fact, I've not been able to find any "same parent" requirement in any docu
   [AM 980509]
 */
#if 0
    /* All the windows of a DeferWindowPos() must have the same parent */
    parent = pWnd->parent->hwndSelf;
    if (pDWP->actualCount == 0) pDWP->hwndParent = parent;
    else if (parent != pDWP->hwndParent)
    {
        USER_HEAP_FREE( hdwp );
        retvalue = 0;
        goto END;
    }
#endif

    for (i = 0; i < pDWP->actualCount; i++)
    {
        if (pDWP->winPos[i].hwnd == hwnd)
        {
              /* Merge with the other changes */
            if (!(flags & SWP_NOZORDER))
            {
                pDWP->winPos[i].hwndInsertAfter = hwndAfter;
            }
            if (!(flags & SWP_NOMOVE))
            {
                pDWP->winPos[i].x = x;
                pDWP->winPos[i].y = y;
            }
            if (!(flags & SWP_NOSIZE))
            {
                pDWP->winPos[i].cx = cx;
                pDWP->winPos[i].cy = cy;
            }
            pDWP->winPos[i].flags &= flags | ~(SWP_NOSIZE | SWP_NOMOVE |
                                               SWP_NOZORDER | SWP_NOREDRAW |
                                               SWP_NOACTIVATE | SWP_NOCOPYBITS|
                                               SWP_NOOWNERZORDER);
            pDWP->winPos[i].flags |= flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW |
                                              SWP_FRAMECHANGED);
            retvalue = hdwp;
            goto END;
        }
    }
    if (pDWP->actualCount >= pDWP->suggestedCount)
    {
        newhdwp = (HDWP)HeapReAlloc(GetProcessHeap(), 0, (LPVOID)hdwp,
                                    sizeof(DWP) + pDWP->suggestedCount*sizeof(WINDOWPOS));
        if (!newhdwp)
        {
            retvalue = 0;
            goto END;
        }
        pDWP = (DWP *) newhdwp;
        pDWP->suggestedCount++;
    }
    pDWP->winPos[pDWP->actualCount].hwnd = hwnd;
    pDWP->winPos[pDWP->actualCount].hwndInsertAfter = hwndAfter;
    pDWP->winPos[pDWP->actualCount].x = x;
    pDWP->winPos[pDWP->actualCount].y = y;
    pDWP->winPos[pDWP->actualCount].cx = cx;
    pDWP->winPos[pDWP->actualCount].cy = cy;
    pDWP->winPos[pDWP->actualCount].flags = flags;
    pDWP->actualCount++;
    retvalue = newhdwp;
END:
    return retvalue;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndDeferWindowPos( HDWP hdwp)
{
    DWP *pDWP;
    WINDOWPOS *winpos;
    BOOL res = TRUE;
    int i;

    pDWP = (DWP *) hdwp;
    if (!pDWP) {
    dprintf(("**EndDeferWindowPos invalid parameter\n"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    dprintf(("**EndDeferWindowPos for %d windows", pDWP->actualCount));
    for (i = 0, winpos = pDWP->winPos; i < pDWP->actualCount; i++, winpos++)
    {
        dprintf(("**EndDeferWindowPos %x (%d,%d) (%d,%d) %x", winpos->hwnd, winpos->x, winpos->y, winpos->x, winpos->cy, winpos->flags));
        if (!(res = SetWindowPos(winpos->hwnd, winpos->hwndInsertAfter,
                                 winpos->x, winpos->y, winpos->cx,
                                 winpos->cy, winpos->flags )))
            break;
    }
    dprintf(("**EndDeferWindowPos DONE"));
    HeapFree(GetProcessHeap(), 0, (LPVOID)hdwp);
    return res;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API ChildWindowFromPoint( HWND hwnd, POINT pt)
{
    dprintf(("USER32:  ChildWindowFromPoint\n"));
//    return O32_ChildWindowFromPoint(arg1, arg2);
    return ChildWindowFromPointEx(hwnd, pt, 0);
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

        dprintf(("ChildWindowFromPointEx(%08xh,%08xh,%08xh).\n",
                 hwndParent, pt, uFlags));

        if (GetWindowRect (hwndParent, &rect) == 0) {
                // oops, invalid handle
                SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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

                dprintf(("ChildWindowFromPointEx returned %x", hWnd));
                // found it!
                return hWnd;
        }

        // the point is in the parentwindow but the parentwindow has no child
        // at this coordinate
            dprintf(("ChildWindowFromPointEx returned parent %x", hwndParent));
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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

        window = Win32BaseWindow::GetWindowFromOS2Handle(hwndNext);
        if(window == NULL) {
                window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndNext);
                if(!window) {
                        //OS/2 window or non-frame window, so skip it
                        continue;
                }
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
  }

  henum = OSLibWinBeginEnumWindows(OSLIB_HWND_DESKTOP);
  while ((hwndNext = OSLibWinGetNextWindow(henum)) != 0)
  {
        window = Win32BaseWindow::GetWindowFromOS2Handle(hwndNext);
        if(window == NULL) {
                window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndNext);
                if(!window) {
                        //OS/2 window or non-frame window, so skip it
                        continue;
                }
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
                window = Win32BaseWindow::GetWindowFromOS2Handle(hwndNext);
                if(window == NULL) {
                        window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndNext);
                        if(!window) {
                            //OS/2 window or non-frame window, so skip it
                            continue;
                        }
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

