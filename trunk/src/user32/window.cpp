/* $Id: window.cpp,v 1.60 2000-04-13 18:50:44 sandervl Exp $ */
/*
 * Win32 window apis for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
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
#include <heapstring.h>

#define DBG_LOCALLOG    DBG_window
#include "dbglocal.h"

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

#if 1
    /* Find the class atom */
    if (!(classAtom = GlobalFindAtomA(className)))
    {
       if (!HIWORD(className))
           dprintf(("CreateWindowEx32A: bad class name %04x\n",LOWORD(className)));
       else
           dprintf(("CreateWindowEx32A: bad class name '%s'\n", className));

       SetLastError(ERROR_INVALID_PARAMETER);
       return 0;
    }

    if (!HIWORD(className))
    {
      sprintf(tmpClass,"#%d", (int) className);
      className = tmpClass;
    }
#else
    /* Find the class atom */
    if (!HIWORD(className) || !(classAtom = GlobalFindAtomA(className)))
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
#endif

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
         dprintf(("CreateWindowExA: class %s parent %x (%d,%d) (%d,%d), %x %x menu=%x", className, parent, x, y, width, height, style, exStyle, menu));
    }
    else dprintf(("CreateWindowExA: class %d parent %x (%d,%d) (%d,%d), %x %x menu=%x", className, parent, x, y, width, height, style, exStyle, menu));

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
    if (!HIWORD(className) || !(classAtom = GlobalFindAtomW(className)))
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
    if(HIWORD(className)) {
         dprintf(("CreateWindowExW: class %ls parent %x (%d,%d) (%d,%d), %x %x", className, parent, x, y, width, height, style, exStyle));
    }
    else dprintf(("CreateWindowExW: class %d parent %x (%d,%d) (%d,%d), %x %x", className, parent, x, y, width, height, style, exStyle));

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
//    dprintf(("GetParent %x", hwnd));
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
        dprintf(("IsChild %x, window %x not found", hwndParent, hwnd));
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
  BOOL rc;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsIconic, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    rc = window->IsWindowIconic();
    dprintf(("IsIconic %x returned %d", hwnd, rc));
    return rc;
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
/*****************************************************************************
 * Name      : BOOL WIN32API ShowWindowAsync
 * Purpose   : The ShowWindowAsync function sets the show state of a window
 *             created by a different thread.
 * Parameters: HWND hwnd     handle of window
 *             int  nCmdShow show state of window
 * Variables :
 * Result    : If the window was previously visible, the return value is TRUE.
 *             If the window was previously hidden, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API ShowWindowAsync (HWND hwnd,
                               int  nCmdShow)
{
  dprintf(("USER32:ShowWindowAsync (%08xh,%08x) not correctly implemented.\n",
         hwnd,
         nCmdShow));

  return ShowWindow(hwnd, nCmdShow);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowPos(HWND hwnd, HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags)
{
  Win32BaseWindow *window;

    if (!hwnd)
    {
      dprintf(("SetWindowPos: Can't move desktop!"));
      return TRUE;
    }
    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowPos, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
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

    if (hwnd)
      window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    else
      window = windowDesktop;
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

    if (hwnd)
      window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    else
      window = windowDesktop;


    if(!window) {
        dprintf(("GetWindowRect, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    if(pRect == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *pRect = *window->getWindowRect(); //always in screen coordinates

    if(hwnd == 0x68000034) {
        window = 0;
    }
    dprintf(("GetWindowRect %x (%d,%d) (%d,%d)", hwnd, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return TRUE;
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
 HWND hwndWin32 = hwnd;
 Win32BaseWindow *window;

    if (!pRect)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }
    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetClientRect, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    window->getClientRect(pRect);
    dprintf(("GetClientRect of %X returned (%d,%d) (%d,%d)\n", hwndWin32, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return TRUE;
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
    dprintf(("AdjustWindowRectEx %x %x %d (%d,%d)(%d,%d)\n", style, exStyle, menu, rect->left, rect->top, rect->right, rect->bottom));

    /* Correct the window style */
    if (!(style & (WS_POPUP | WS_CHILD)))  /* Overlapped window */
    	style |= WS_CAPTION;

    //SvL: Include WS_POPUP -> otherwise HAS_THINFRAME is true for popup windows
    //     Also include WS_CHILD -> otherwise HAS_THICKFRAME doesn't work correctly
    style &= (WS_DLGFRAME | WS_BORDER | WS_THICKFRAME | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_POPUP);
    exStyle &= (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_TOOLWINDOW);
    if (exStyle & WS_EX_DLGMODALFRAME) style &= ~WS_THICKFRAME;

    //Adjust rect outer (Win32BaseWindow::AdjustRectOuter)
    if (HAS_THICKFRAME(style,exStyle))
      InflateRect( rect, GetSystemMetrics(SM_CXFRAME), GetSystemMetrics(SM_CYFRAME) );
    else
      if (HAS_DLGFRAME(style,exStyle))
        InflateRect(rect, GetSystemMetrics(SM_CXDLGFRAME), GetSystemMetrics(SM_CYDLGFRAME) );
      else
        if (HAS_THINFRAME(style))
          InflateRect( rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

    if ((style & WS_CAPTION) == WS_CAPTION)
    {
      if (exStyle & WS_EX_TOOLWINDOW)
        rect->top -= GetSystemMetrics(SM_CYSMCAPTION);
      else
        rect->top -= GetSystemMetrics(SM_CYCAPTION);
    }

    if (menu)
      rect->top -= GetSystemMetrics(SM_CYMENU);

    //Adjust rect inner (Win32BaseWindow::AdjustRectInner)
    if (exStyle & WS_EX_CLIENTEDGE)
      InflateRect (rect, GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));

    if (exStyle & WS_EX_STATICEDGE)
      InflateRect (rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

    if (style & WS_VSCROLL) rect->right  += GetSystemMetrics(SM_CXVSCROLL);
    if (style & WS_HSCROLL) rect->bottom += GetSystemMetrics(SM_CYHSCROLL);

    dprintf(("AdjustWindowRectEx returned (%d,%d)(%d,%d)\n", rect->left, rect->top, rect->right, rect->bottom));

    return TRUE;
}
//******************************************************************************
/* Coordinate Space and Transformation Functions */
//******************************************************************************
int WIN32API MapWindowPoints(HWND hwndFrom, HWND hwndTo, LPPOINT lpPoints,
                             UINT cPoints)
{
 Win32BaseWindow *wndfrom, *wndto;
 int retval = 0;
 OSLIBPOINT point;

    if(lpPoints == NULL || cPoints == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if(hwndFrom)
    {
        wndfrom = Win32BaseWindow::GetWindowFromHandle(hwndFrom);
        if(!wndfrom) {
            dprintf(("MapWindowPoints, window %x not found", hwndFrom));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
    }
    else wndfrom = windowDesktop;

    if(hwndTo)
    {
        wndto = Win32BaseWindow::GetWindowFromHandle(hwndTo);
        if(!wndto) {
            dprintf(("MapWindowPoints, window %x not found", hwndTo));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
    }
    else wndto = windowDesktop;

    if(wndto == wndfrom)
        return 0; //nothing to do

    dprintf(("USER32: MapWindowPoints %x to %x (%d,%d) (%d)", hwndFrom, hwndTo, lpPoints->x, lpPoints->y, cPoints));
    point.x = lpPoints->x;
    point.y = lpPoints->y;
    if (!mapWin32Point(wndfrom,wndto,&point))
    {
      SetLastError(ERROR_INVALID_WINDOW_HANDLE);
      return 0;
    }

    short int xinc = point.x - lpPoints->x;
    short int yinc = point.y - lpPoints->y;

    for(int i=0;i<cPoints;i++)
    {
        lpPoints[i].x += xinc;
        lpPoints[i].y += yinc;
    }
    retval = ((LONG)yinc << 16) | xinc;
    return retval;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScreenToClient (HWND hwnd, LPPOINT pt)
{
    Win32BaseWindow *wnd;
    PRECT rcl;
    BOOL rc;

    if (!hwnd) return (TRUE);
    wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
    if (!wnd) {
        dprintf(("warning: ScreenToClient: window %x not found!", hwnd));
        return (TRUE);
    }
#ifdef DEBUG
    POINT tmp = *pt;
#endif
    rc = mapWin32Point(OSLIB_HWND_DESKTOP, wnd->getOS2WindowHandle(), (OSLIBPOINT*)pt);
    dprintf(("ScreenToClient %x (%d,%d) -> (%d,%d)", hwnd, tmp.x, tmp.y, pt->x, pt->y));
    return rc;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetDesktopWindow(void)
{
    HWND DesktopWindow = windowDesktop->getWindowHandle();
    dprintf2(("USER32: GetDesktopWindow, returned %d\n", DesktopWindow));
    return DesktopWindow;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowA(LPCSTR lpszClass, LPCSTR lpszWindow)
{
    return FindWindowExA( NULL, NULL, lpszClass, lpszWindow );
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowW( LPCWSTR lpClassName, LPCWSTR lpWindowName)
{
    return FindWindowExW( NULL, NULL, lpClassName, lpWindowName );
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowExA(HWND hwndParent, HWND hwndChildAfter, LPCSTR lpszClass, LPCSTR lpszWindow)
{
    ATOM atom = 0;

    if (lpszClass)
    {
        /* If the atom doesn't exist, then no class */
        /* with this name exists either. */
        if (!(atom = GlobalFindAtomA( lpszClass ))) 
        {
            SetLastError(ERROR_CANNOT_FIND_WND_CLASS);
            return 0;
        }
    }
    return Win32BaseWindow::FindWindowEx(hwndParent, hwndChildAfter, atom, (LPSTR)lpszWindow);
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
 *
 *****************************************************************************/

HWND WIN32API FindWindowExW(HWND    hwndParent,
                            HWND    hwndChildAfter,
                            LPCWSTR lpszClass,
                            LPCWSTR lpszWindow)
{
    ATOM atom = 0;
    char *buffer;
    HWND hwnd;

    if (lpszClass)
    {
        /* If the atom doesn't exist, then no class */
        /* with this name exists either. */
        if (!(atom = GlobalFindAtomW( lpszClass )))
        {
            SetLastError(ERROR_CANNOT_FIND_WND_CLASS);
            return 0;
        }
    }
    buffer = HEAP_strdupWtoA( GetProcessHeap(), 0, lpszWindow );
    hwnd = Win32BaseWindow::FindWindowEx(hwndParent, hwndChildAfter, atom, buffer);
    HeapFree( GetProcessHeap(), 0, buffer );
    return hwnd;
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
    dprintf(("MoveWindow: %x %d,%d %dx%d %d\n", hwnd, x, y, cx, cy, repaint ));

    return SetWindowPos( hwnd, 0, x, y, cx, cy, flags );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ClientToScreen (HWND hwnd, PPOINT pt)
{
    Win32BaseWindow *wnd;
    PRECT rcl;
    BOOL rc;

    if (!hwnd) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (FALSE);
    }
    wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
    if (!wnd) {
        dprintf(("warning: ClientToScreen window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return (FALSE);
    }
#ifdef DEBUG
    POINT tmp = *pt;
#endif
    rc = mapWin32Point(wnd->getOS2WindowHandle(),OSLIB_HWND_DESKTOP,(OSLIBPOINT*)pt);
    dprintf(("ClientToScreen %x (%d,%d) -> (%d,%d)", hwnd, tmp.x, tmp.y, pt->x, pt->y));

    return rc;
}
//******************************************************************************
//******************************************************************************
HDWP WIN32API BeginDeferWindowPos(int count)
{
    HDWP handle;
    DWP *pDWP;

    if (count <= 0)
    {
        dprintf(("USER32: BeginDeferWindowPos invalid param %d", count));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    dprintf(("USER32: BeginDeferWindowPos %d", count));
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

    dprintf(("USER32: DeferWindowPos hdwp %x hwnd %x hwndAfter %x (%d,%d)(%d,%d) %x", hdwp, hwnd, hwndAfter,
              x, y, cx, cy, flags));

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
    return ChildWindowFromPointEx(hwnd, pt, 0);
}
/*****************************************************************************
 * Name      : HWND WIN32API ChildWindowFromPointEx
 * Purpose   : pt: client coordinates
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

        dprintf(("ChildWindowFromPointEx(%08xh,%08xh,%08xh).\n",
                 hwndParent, pt, uFlags));

        if (GetWindowRect (hwndParent, &rect) == 0) {
                // oops, invalid handle
                SetLastError(ERROR_INVALID_WINDOW_HANDLE);
                return NULL;
        }

        ClientToScreen(hwndParent, &pt);
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
                if (PtInRect (&rect,pt) == FALSE) {
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
    HWND  hwndOS2, hwnd;
    POINT wPoint;

    wPoint.x = point.x;
    wPoint.y = mapScreenY(point.y);

    hwndOS2 = OSLibWinWindowFromPoint(OSLIB_HWND_DESKTOP, (PVOID)&wPoint);
    if(hwndOS2)
    {
      hwnd = Win32BaseWindow::OS2ToWin32Handle(hwndOS2);
      if(hwnd) {
              dprintf(("WindowFromPoint (%d,%d) %x->%x\n", point.x, point.y, hwndOS2, hwnd));
              return hwnd;
      }
    }
    dprintf(("WindowFromPoint (%d,%d) %x->1\n", point.x, point.y, hwndOS2));
    return windowDesktop->getWindowHandle();
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
    return window->IsWindowUnicode();
}
/***********************************************************************
 *             SwitchToThisWindow   (USER32.539)
 */
DWORD WINAPI SwitchToThisWindow( HWND hwnd, BOOL restore )
{
    return ShowWindow( hwnd, restore ? SW_RESTORE : SW_SHOWMINIMIZED );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumThreadWindows(DWORD dwThreadId, WNDENUMPROC lpfn, LPARAM lParam)
{
  return windowDesktop->EnumThreadWindows(dwThreadId, lpfn, lParam);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumChildWindows(HWND hwnd, WNDENUMPROC lpfn, LPARAM lParam)
{
 Win32BaseWindow *window;
 BOOL   rc = TRUE;
 ULONG  henum;
 HWND   hwndNext;

  if(lpfn == NULL) {
    dprintf(("EnumChildWindows invalid parameter %x %x\n", hwnd, lParam));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
  }
  window = Win32BaseWindow::GetWindowFromHandle(hwnd);
  if(!window) {
        dprintf(("EnumChildWindows, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
  }
  return window->EnumChildWindows(lpfn, lParam);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumWindows(WNDENUMPROC lpfn, LPARAM lParam)
{
  return windowDesktop->EnumWindows(lpfn, lParam);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API ArrangeIconicWindows( HWND hwnd)
{
    dprintf(("USER32:  ArrangeIconicWindows %x", hwnd));
    return O32_ArrangeIconicWindows(Win32BaseWindow::Win32ToOS2FrameHandle(hwnd));
}
//******************************************************************************
//restores iconized window to previous size/position
//******************************************************************************
BOOL WIN32API OpenIcon(HWND hwnd)
{
  dprintf(("USER32:  OpenIcon"));

  if(!IsIconic(hwnd))
        return FALSE;
  ShowWindow(hwnd, SW_SHOWNORMAL);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowOwnedPopups( HWND hwnd, BOOL  arg2)
{
    dprintf(("USER32:  ShowOwnedPopups %x", hwnd));
    return O32_ShowOwnedPopups(Win32BaseWindow::Win32ToOS2FrameHandle(hwnd), arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetForegroundWindow(void)
{
    dprintf(("USER32:  GetForegroundWindow"));
    return Win32BaseWindow::OS2ToWin32Handle(O32_GetForegroundWindow());
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetLastActivePopup( HWND hWnd)
{
 HWND hwnd;

    hWnd = Win32BaseWindow::Win32ToOS2Handle(hWnd);

    hwnd = Win32BaseWindow::OS2ToWin32Handle(O32_GetLastActivePopup(hWnd));

    dprintf(("GetLastActivePopup %x returned %x", hWnd, hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetWindowThreadProcessId(HWND hWnd, PDWORD  lpdwProcessId)
{
    dprintf2(("USER32:  GetWindowThreadProcessId"));
    hWnd = Win32BaseWindow::Win32ToOS2Handle(hWnd);

    return O32_GetWindowThreadProcessId(hWnd,lpdwProcessId);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetWindowContextHelpId(HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowContextHelpId, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("GetWindowContextHelpId %x", hwnd));
    return window->getWindowContextHelpId();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowContextHelpId(HWND hwnd, DWORD dwContextHelpId)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowContextHelpId, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("SetWindowContextHelpId %x %d", hwnd, dwContextHelpId));
    window->setWindowContextHelpId(dwContextHelpId);
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
