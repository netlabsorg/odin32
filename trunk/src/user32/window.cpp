/* $Id: window.cpp,v 1.111 2001-10-24 09:53:14 sandervl Exp $ */
/*
 * Win32 window apis for OS/2
 *
 * Copyright 1999-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Parts based on Wine Windows code (windows\win.c, windows\property.c, windows\winpos.c)
 *
 * Copyright 1993, 1994, 1995 Alexandre Julliard
 *           1995, 1996, 1999 Alex Korobka
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 * TODO: Decide what to do about commands for OS/2 windows (non-Win32 apps)
 * TODO: ShowOwnedPopups needs to be tested
 *       GetLastActivePopup needs to be rewritten
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

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
#include "oslibmsg.h"
#include <win\winpos.h>
#include <win\win.h>
#include <heapstring.h>
#include <winuser32.h>
#include "hook.h"

#define DBG_LOCALLOG    DBG_window
#include "dbglocal.h"

ODINDEBUGCHANNEL(USER32-WINDOW)


//******************************************************************************
//******************************************************************************
ODINFUNCTION12(HWND,      CreateWindowExA,
               DWORD,     exStyle,
               LPCSTR,    className,
               LPCSTR,    windowName,
               DWORD,     style,
               INT,       x,
               INT,       y,
               INT,       width,
               INT,       height,
               HWND,      parent,
               HMENU,     menu,
               HINSTANCE, instance,
               LPVOID,    data )
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
        RELEASE_WNDOBJ(window);
        delete window;
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
ODINFUNCTION12(HWND,      CreateWindowExW,
               DWORD,     exStyle,
               LPCWSTR,   className,
               LPCWSTR,   windowName,
               DWORD,     style,
               INT,       x,
               INT,       y,
               INT,       width,
               INT,       height,
               HWND,      parent,
               HMENU,     menu,
               HINSTANCE, instance,
               LPVOID,    data )
{
  Win32BaseWindow *window;
  ATOM classAtom;
  CREATESTRUCTA cs;
  WCHAR tmpClassW[20];

    if(exStyle & WS_EX_MDICHILD)
        return CreateMDIWindowW(className, windowName, style, x, y, width, height, parent, instance, (LPARAM)data);

    /* Find the class atom */
    if (!(classAtom = GlobalFindAtomW(className)))
    {
       if (!HIWORD(className))
           dprintf(("CreateWindowEx32W: bad class name %04x",LOWORD(className)));
       else
           dprintf(("CreateWindowEx32W: bad class name '%ls'", className));

       SetLastError(ERROR_INVALID_PARAMETER);
       return 0;
    }
#ifdef DEBUG
    if(HIWORD(className)) {
         dprintf(("CreateWindowExW: class %ls name %ls parent %x (%d,%d) (%d,%d), %x %x", className, HIWORD(windowName) ? windowName : NULL, parent, x, y, width, height, style, exStyle));
    }
    else dprintf(("CreateWindowExW: class %d name %ls parent %x (%d,%d) (%d,%d), %x %x", className, HIWORD(windowName) ? windowName : NULL, parent, x, y, width, height, style, exStyle));
#endif
    if (!HIWORD(className))
    {
      wsprintfW(tmpClassW, (LPCWSTR)L"#%d", (int) className);
      className = tmpClassW;
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
        RELEASE_WNDOBJ(window);
        delete window;
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
ODINFUNCTION1(HWND, CreateFakeWindowEx,
              HWND, hwndOS2)
{
 Win32BaseWindow *window;

    window = new Win32BaseWindow(hwndOS2, 0);
    if(window == NULL)
    {
        dprintf(("Win32BaseWindow creation failed!!"));
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
ODINFUNCTION1(BOOL, DestroyWindow,
              HWND, hwnd)
{
  Win32BaseWindow *window;
  BOOL             ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("DestroyWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    ret = window->DestroyWindow();
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HWND, SetActiveWindow,
              HWND, hwnd)
{
  Win32BaseWindow *window;
  HWND             hwndActive;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetActiveWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    hwndActive = window->SetActiveWindow();
  
    // check last active popup window
    if (hwndActive)
    {
      // TODO:
      // set last active popup window to the ancestor window
      dprintf(("support for last active popup incorrectly implemented"));
    }
  
    RELEASE_WNDOBJ(window);
    return hwndActive;
}
//******************************************************************************
//Note: does not set last error if no parent (verified in NT4, SP6)
//******************************************************************************
HWND WIN32API GetParent(HWND hwnd)
{
  Win32BaseWindow *window;
  HWND             hwndParent;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetParent, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf2(("GetParent %x", hwnd));
    hwndParent = window->GetParent();
    RELEASE_WNDOBJ(window);
    return hwndParent;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HWND, SetParent,
              HWND, hwndChild,
              HWND, hwndNewParent)
{
  Win32BaseWindow *window;
  HWND             hwndOldParent;

    window = Win32BaseWindow::GetWindowFromHandle(hwndChild);
    if(!window) {
        dprintf(("SetParent, window %x not found", hwndChild));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    if(hwndNewParent == HWND_DESKTOP) {
        hwndNewParent = GetDesktopWindow();
    }
    else {
        if(!IsWindow(hwndNewParent)) {
            RELEASE_WNDOBJ(window);
            dprintf(("SetParent, parent %x not found", hwndNewParent));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
    }
    }
    dprintf(("SetParent %x %x", hwndChild, hwndNewParent));
    hwndOldParent = window->SetParent(hwndNewParent);
    RELEASE_WNDOBJ(window);
    return hwndOldParent;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, IsChild,
              HWND, hwndParent,
              HWND, hwnd)
{
  Win32BaseWindow *window;
  BOOL             fIsChild;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsChild %x, window %x not found", hwndParent, hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("IsChild %x %x", hwndParent, hwnd));
    fIsChild = window->IsChild(hwndParent);
    RELEASE_WNDOBJ(window);
    return fIsChild;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HWND, GetTopWindow,
              HWND, hwnd)
{
  Win32BaseWindow *window;
  HWND hwndTop;

    if(hwnd == HWND_DESKTOP) {
        windowDesktop->addRef();
        window = windowDesktop;
    }
    else {
        window = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!window) {
            dprintf(("GetTopWindow, window %x not found", hwnd));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
    }
    hwndTop = window->GetTopWindow();
    dprintf2(("GetTopWindow %x returned %x", hwnd, hwndTop));
    RELEASE_WNDOBJ(window);
    return hwndTop;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, IsIconic,
              HWND, hwnd)
{
  Win32BaseWindow *window;
  BOOL fIsIconic;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsIconic, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    fIsIconic = window->IsWindowIconic();
    dprintf(("IsIconic %x returned %d", hwnd, fIsIconic));
    RELEASE_WNDOBJ(window);
    return fIsIconic;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HWND, GetWindow,
              HWND, hwnd,
              UINT, uCmd)
{
  Win32BaseWindow *window;
  HWND hwndRelated;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    hwndRelated = window->GetWindow(uCmd);
    RELEASE_WNDOBJ(window);
    return hwndRelated;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, EnableWindow,
              HWND, hwnd,
              BOOL, fEnable)
{
  Win32BaseWindow *window;
  BOOL             fEnabled;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("EnableWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("EnableWindow %x %d", hwnd, fEnable));
    fEnabled = window->EnableWindow(fEnable);
    RELEASE_WNDOBJ(window);
    return fEnabled;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, BringWindowToTop,
              HWND, hwnd)
{
    dprintf(("BringWindowToTop %x", hwnd));
    return SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
}
/***********************************************************************
 *           SetInternalWindowPos   (USER32.483)
 */
ODINPROCEDURE4(SetInternalWindowPos,
               HWND,    hwnd,
               UINT,    showCmd,
               LPRECT,  lpRect,
               LPPOINT, lpPoint )
{
    if( IsWindow(hwnd) )
    {
        WINDOWPLACEMENT wndpl;
        UINT flags;

        GetWindowPlacement(hwnd, &wndpl);
        wndpl.length  = sizeof(wndpl);
        wndpl.showCmd = showCmd;
        wndpl.flags = 0;

        if(lpPoint)
        {
            wndpl.flags |= WPF_SETMINPOSITION;
            wndpl.ptMinPosition = *lpPoint;
        }
        if(lpRect)
        {
            wndpl.rcNormalPosition = *lpRect;
        }
        SetWindowPlacement( hwnd, &wndpl);
    }

}
/***********************************************************************
 *           GetInternalWindowPos   (USER32.245)
 */
ODINFUNCTION3(UINT,    GetInternalWindowPos,
              HWND,    hwnd,
              LPRECT,  rectWnd,
              LPPOINT, ptIcon )
{
    WINDOWPLACEMENT wndpl;

    if(GetWindowPlacement( hwnd, &wndpl ))
    {
        if (rectWnd) *rectWnd = wndpl.rcNormalPosition;
        if (ptIcon)  *ptIcon = wndpl.ptMinPosition;
        return wndpl.showCmd;
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HWND, GetActiveWindow)
{
    return Win32BaseWindow::GetActiveWindow();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, ShowWindow,
              HWND, hwnd,
              int,  nCmdShow)
{
  Win32BaseWindow *window;
  BOOL             ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("ShowWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    ret = window->ShowWindow(nCmdShow);
    RELEASE_WNDOBJ(window);
    return ret;
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
ODINFUNCTION2(BOOL, ShowWindowAsync,
              HWND, hwnd,
              int,  nCmdShow)
{
  dprintf(("USER32:ShowWindowAsync (%08xh,%08x) not correctly implemented.\n",
         hwnd,
         nCmdShow));

  return ShowWindow(hwnd, nCmdShow);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION7(BOOL, SetWindowPos,
              HWND, hwnd,
              HWND, hwndInsertAfter,
              int,  x,
              int,  y,
              int,  cx,
              int,  cy,
              UINT, fuFlags)
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
    BOOL ret = window->SetWindowPos(hwndInsertAfter, x, y, cx, cy, fuFlags);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//NOTE: length must equal structure size or else api fails (verified in NT4, SP6)
//******************************************************************************
ODINFUNCTION2(BOOL, SetWindowPlacement,
              HWND, hwnd,
              const WINDOWPLACEMENT *, winpos)
{
  Win32BaseWindow *window;

    if(!winpos || winpos->length != sizeof(WINDOWPLACEMENT)) {
        dprintf(("SetWindowPlacement %x invalid parameter", hwnd));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowPlacement, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    dprintf(("USER32: SetWindowPlacement %x %x", hwnd, winpos));
    BOOL ret = window->SetWindowPlacement((WINDOWPLACEMENT *)winpos);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//NOTE: Length does not need to be correct (even though the SDK docs claim otherwise)
//      (Verified in NT4, SP6)
//******************************************************************************
ODINFUNCTION2(BOOL, GetWindowPlacement,
              HWND, hwnd,
              LPWINDOWPLACEMENT, winpos)
{
  Win32BaseWindow *window;

    if(!winpos) {
        dprintf(("GetWindowPlacement %x invalid parameter", hwnd));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowPlacement, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    dprintf(("USER32: GetWindowPlacement %x %x", hwnd, winpos));
    BOOL ret = window->GetWindowPlacement(winpos);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindow(HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("IsWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    dprintf2(("IsWindow %x", hwnd));
    BOOL fIsWindow = window->IsWindow();
    RELEASE_WNDOBJ(window);
    return fIsWindow;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, IsWindowEnabled,
              HWND, hwnd)
{
  DWORD            dwStyle;

    if(!IsWindow(hwnd)) {
        dprintf(("IsWindowEnabled, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("IsWindowEnabled %x", hwnd));
    dwStyle = GetWindowLongA(hwnd, GWL_STYLE);
    if(dwStyle & WS_DISABLED) {
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowVisible(HWND hwnd)
{
  BOOL             ret;
  HWND             hwndParent;
  DWORD            dwStyle;

    if(hwnd == HWND_DESKTOP) {//TODO: verify in NT!
        dprintf(("IsWindowVisible DESKTOP returned TRUE"));
        return TRUE;    //desktop is always visible
    }
    if(!IsWindow(hwnd)) {
        dprintf(("IsWindowVisible, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    //check visibility of this window
    dwStyle = GetWindowLongA(hwnd, GWL_STYLE);
    if(!(dwStyle & WS_VISIBLE)) {
        ret = FALSE;
        goto end;
    }
    ret = TRUE;

    //check visibility of parents
    hwndParent = GetParent(hwnd);
    while(hwndParent) {
        dwStyle = GetWindowLongA(hwndParent, GWL_STYLE);
        if(!(dwStyle & WS_VISIBLE)) {
            dprintf(("IsWindowVisible %x returned FALSE (parent %x invisible)", hwnd, hwndParent));
            return FALSE;
        }
        if(!(dwStyle & WS_CHILD)) {
            break; //GetParent can also return the owner
        }
        hwndParent = GetParent(hwndParent);
    }

end:
    dprintf(("IsWindowVisible %x returned %d", hwnd, ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HWND, SetFocus,
              HWND, hwnd)
{
 Win32BaseWindow *window;
 Win32BaseWindow *oldfocuswnd;
 HWND lastFocus, lastFocus_W, hwnd_O, hwndTopParent;
 BOOL activate;
 TEB *teb;

    teb = GetThreadTEB();
    if(teb == NULL) {
        DebugInt3();
        return 0;
    }

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetFocus, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }

    hwnd_O = window->getOS2WindowHandle();
    if(teb->o.odin.hwndFocus) {
         lastFocus = teb->o.odin.hwndFocus;
    }
    else lastFocus = OSLibWinQueryFocus (OSLIB_HWND_DESKTOP);

    hwndTopParent = window->GetTopParent();
    activate = FALSE;
    lastFocus_W = OS2ToWin32Handle(lastFocus);
    if(lastFocus_W) {
         oldfocuswnd = Win32BaseWindow::GetWindowFromHandle(lastFocus_W);
         if(lastFocus_W != hwnd && hwndTopParent != oldfocuswnd->GetTopParent()) {
            activate = TRUE;
         }
         RELEASE_WNDOBJ(oldfocuswnd);
    }
    else activate = TRUE;

    dprintf(("SetFocus %x (%x) -> %x (%x) act %d", lastFocus_W, lastFocus, hwnd, hwnd_O, activate));

    if(HOOK_CallHooksA(WH_CBT, HCBT_SETFOCUS, hwnd, (LPARAM)lastFocus_W)) {
        dprintf(("hook cancelled SetFocus call!"));
        return 0;
    }

    //PM doesn't allow SetFocus calls during WM_SETFOCUS message processing;
    //must delay this function call
    if(teb->o.odin.fWM_SETFOCUS) {
        dprintf(("USER32: Delay SetFocus call!"));
        teb->o.odin.hwndFocus = hwnd;
        //mp1 = win32 window handle
        //mp2 = top parent if activation required
        OSLibPostMessageDirect(hwnd_O, WIN32APP_SETFOCUSMSG, hwnd, (activate) ? hwndTopParent : 0);
        return lastFocus_W;
    }
    teb->o.odin.hwndFocus = 0;
    if(!IsWindow(hwnd)) return FALSE;       //abort if window destroyed

    //NOTE: Don't always activate the window or else the z-order will be changed!!
    return (OSLibWinSetFocus(OSLIB_HWND_DESKTOP, hwnd_O, activate)) ? lastFocus_W : 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HWND, GetFocus)
{
 TEB *teb;
 HWND hwnd;

    teb = GetThreadTEB();
    if(teb == NULL) {
        DebugInt3();
        return 0;
    }
    //PM doesn't allow SetFocus calls during WM_SETFOCUS message processing;
    //If focus was changed during WM_SETFOCUS, the focus window handle is
    //stored in teb->o.odin.hwndFocus (set back to 0 when delayed SetFocus
    //is activated)
    if(teb->o.odin.hwndFocus) {
        dprintf(("USER32: GetFocus %x (DURING WM_SETFOCUS PROCESSING)", teb->o.odin.hwndFocus));
        return teb->o.odin.hwndFocus;
    }

    hwnd = OSLibWinQueryFocus(OSLIB_HWND_DESKTOP);
    hwnd = OS2ToWin32Handle(hwnd);
    dprintf(("USER32: GetFocus %x\n", hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, IsZoomed,
              HWND, hwnd)
{
 DWORD style;

    style = GetWindowLongA(hwnd, GWL_STYLE);
    dprintf(("USER32: IsZoomed %x returned %d", hwnd, ((style & WS_MAXIMIZE) != 0)));

    return (style & WS_MAXIMIZE) != 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, LockWindowUpdate,
              HWND, hwnd)
{
  return OSLibWinLockWindowUpdate(Win32ToOS2Handle(hwnd));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,  GetWindowRect,
              HWND,  hwnd,
              PRECT, pRect)
{
  Win32BaseWindow *window;

    if(pRect == NULL) {
        dprintf(("GetWindowRect %x invalid parameter!", hwnd));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if(hwnd == HWND_DESKTOP) {
         windowDesktop->addRef();
         window = windowDesktop;
    }     
    else window = Win32BaseWindow::GetWindowFromHandle(hwnd);
      
    if(!window) {
        dprintf(("GetWindowRect, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    *pRect = *window->getWindowRect();

    //convert from parent coordinates to screen (if necessary)
    if(window->getParent()) {
         MapWindowPoints(window->getParent()->getWindowHandle(), 0, (PPOINT)pRect, 2);
    }
    RELEASE_WNDOBJ(window);
    dprintf(("GetWindowRect %x (%d,%d) (%d,%d)", hwnd, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(int,  GetWindowTextLengthA,
              HWND, hwnd)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowTextLengthA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("GetWindowTextLengthA %x", hwnd));
    int ret = window->GetWindowTextLengthA();
    RELEASE_WNDOBJ(window);
    return ret;
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
    RELEASE_WNDOBJ(window);
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextLengthW( HWND hwnd)
{
   Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowTextLengthW, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("GetWindowTextLengthW %x", hwnd));
    int ret = window->GetWindowTextLengthW();
    RELEASE_WNDOBJ(window);
    return ret;
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
    int rc = window->GetWindowTextW(lpsz, cch);
    RELEASE_WNDOBJ(window);
    dprintf(("GetWindowTextW %x %ls", hwnd, lpsz));
    return rc;
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
    BOOL ret = window->SetWindowTextA((LPSTR)lpsz);
    RELEASE_WNDOBJ(window);
    return ret;
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
    dprintf(("SetWindowTextW %x %ls", hwnd, lpsz));
    BOOL ret = window->SetWindowTextW((LPWSTR)lpsz);
    RELEASE_WNDOBJ(window);
    return ret;
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

    return SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
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
    RELEASE_WNDOBJ(window);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AdjustWindowRect(PRECT rect, DWORD style, BOOL menu)
{
    return AdjustWindowRectEx(rect, style, menu, 0);
}
//******************************************************************************
//Calculate window rectangle based on given client rectangle, style, menu and extended style
//******************************************************************************
BOOL WIN32API AdjustWindowRectEx( PRECT rect, DWORD style, BOOL menu, DWORD exStyle)
{
    if(style == 0 && menu == FALSE && exStyle == 0) {
        dprintf(("AdjustWindowRectEx %x %x %d (%d,%d)(%d,%d) -> no change required", style, exStyle, menu, rect->left, rect->top, rect->right, rect->bottom));
        return TRUE;    //nothing needs to be changed (VERIFIED in NT 4)
    }
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
    if(!(style & WS_ICONIC)) {
        if (exStyle & WS_EX_CLIENTEDGE)
            InflateRect (rect, GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));

        if (exStyle & WS_EX_STATICEDGE)
            InflateRect (rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

        //SvL: scrollbars aren't checked *UNLESS* the style includes a border (any border)
        //     --> VERIFIED IN NT4, SP6 (fixes MFC apps with scrollbars + bar controls)
        if(style & (WS_THICKFRAME|WS_BORDER|WS_DLGFRAME)) {
            if (style & WS_VSCROLL) rect->right  += GetSystemMetrics(SM_CXVSCROLL);
            if (style & WS_HSCROLL) rect->bottom += GetSystemMetrics(SM_CYHSCROLL);
        }
    }

    dprintf(("AdjustWindowRectEx returned (%d,%d)(%d,%d)\n", rect->left, rect->top, rect->right, rect->bottom));

    return TRUE;
}
//******************************************************************************
/* Coordinate Space and Transformation Functions */
//******************************************************************************
/*******************************************************************
 *         WINPOS_GetWinOffset
 *
 * Calculate the offset between the origin of the two windows. Used
 * to implement MapWindowPoints.
 */
static void WINPOS_GetWinOffset( Win32BaseWindow *wndFrom, Win32BaseWindow *wndTo,
                                 POINT *offset )
{
 Win32BaseWindow *window;

    offset->x = offset->y = 0;

    /* Translate source window origin to screen coords */
    if(wndFrom != windowDesktop)
    {
    window = wndFrom;
        while(window)
        {
            offset->x += window->getClientRectPtr()->left + window->getWindowRect()->left;
            offset->y += window->getClientRectPtr()->top + window->getWindowRect()->top;
            window = window->getParent();
        }
    }

    /* Translate origin to destination window coords */
    if(wndTo != windowDesktop)
    {
    window = wndTo;
        while(window)
        {
            offset->x -= window->getClientRectPtr()->left + window->getWindowRect()->left;
            offset->y -= window->getClientRectPtr()->top + window->getWindowRect()->top;
            window = window->getParent();
        }
    }
}
//******************************************************************************
//******************************************************************************
int WIN32API MapWindowPoints(HWND hwndFrom, HWND hwndTo, LPPOINT lpPoints,
                             UINT cPoints)
{
 Win32BaseWindow *wndfrom, *wndto;
 int retval = 0;
 POINT offset;

    SetLastError(0);
    if(lpPoints == NULL || cPoints == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if(hwndTo == hwndFrom)
        return 0; //nothing to do

    if(hwndFrom == HWND_DESKTOP)
    {
        windowDesktop->addRef();
        wndfrom = windowDesktop;
    } 
    else {
        wndfrom = Win32BaseWindow::GetWindowFromHandle(hwndFrom);
        if(!wndfrom) {
            dprintf(("MapWindowPoints, window %x not found", hwndFrom));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
    }

    if(hwndTo == HWND_DESKTOP)
    {
        windowDesktop->addRef();
        wndto = windowDesktop;
    } 
    else {
        wndto = Win32BaseWindow::GetWindowFromHandle(hwndTo);
        if(!wndto) {
            dprintf(("MapWindowPoints, window %x not found", hwndTo));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
    }

    dprintf2(("USER32: MapWindowPoints %x to %x (%d,%d) (%d)", hwndFrom, hwndTo, lpPoints->x, lpPoints->y, cPoints));
    WINPOS_GetWinOffset(wndfrom, wndto, &offset);

    RELEASE_WNDOBJ(wndto);
    RELEASE_WNDOBJ(wndfrom);
    for(int i=0;i<cPoints;i++)
    {
        lpPoints[i].x += offset.x;
        lpPoints[i].y += offset.y;
    }
    retval = ((LONG)offset.y << 16) | offset.x;
    return retval;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScreenToClient(HWND hwnd, LPPOINT pt)
{
    PRECT rcl;
    BOOL rc;

    if(hwnd == HWND_DESKTOP) {
        return (TRUE); //nothing to do
    }
    if (!IsWindow(hwnd)) {
        dprintf(("warning: ScreenToClient: window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    SetLastError(0);
#ifdef DEBUG
    POINT tmp = *pt;
#endif
    MapWindowPoints(0, hwnd, pt, 1);
    dprintf2(("ScreenToClient %x (%d,%d) -> (%d,%d)", hwnd, tmp.x, tmp.y, pt->x, pt->y));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetDesktopWindow(void)
{
    HWND DesktopWindow = windowDesktop->getWindowHandle();
    dprintf2(("USER32: GetDesktopWindow, returned %x\n", DesktopWindow));
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
//    return OSLibWinFlashWindow(Win32ToOS2Handle(hwnd), fFlash);
    return 1;
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
    PRECT rcl;

    if(hwnd == HWND_DESKTOP) {
        return(TRUE); //nothing to do
    }
    if(!IsWindow(hwnd)) {
        dprintf(("warning: ClientToScreen window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return (FALSE);
    }
#ifdef DEBUG
    POINT tmp = *pt;
#endif
    MapWindowPoints(hwnd, 0, pt, 1);
    dprintf2(("ClientToScreen %x (%d,%d) -> (%d,%d)", hwnd, tmp.x, tmp.y, pt->x, pt->y));

    return TRUE;
}
//******************************************************************************
//Note: count 0 is a legal parameter (verified in NT4)
//******************************************************************************
HDWP WIN32API BeginDeferWindowPos(int count)
{
    HDWP handle;
    DWP *pDWP;

    if (count < 0)
    {
        dprintf(("USER32: BeginDeferWindowPos invalid param %d", count));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    dprintf(("USER32: BeginDeferWindowPos %d", count));
    if(count == 0)
        count = 8;  // change to any non-zero value

    handle = (HDWP)HeapAlloc(GetProcessHeap(), 0, sizeof(DWP) + (count-1)*sizeof(WINDOWPOS));
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
 *
 * TODO: SvL: Does this need to be thread safe?
 *
 */
HDWP WIN32API DeferWindowPos( HDWP hdwp, HWND hwnd, HWND hwndAfter,
                              INT x, INT y, INT cx, INT cy,
                              UINT flags )
{
  DWP *pDWP;
  int i;
  HDWP newhdwp = hdwp,retvalue;

    pDWP = (DWP *)hdwp;
    if (!pDWP) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (hwnd == GetDesktopWindow())
        return 0;

    if(!IsWindow(hwnd)) {
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
        //DWP structure already contains WINDOWPOS, allocated with (count-1)
        //in BeginDeferWindowPos; pDWP->suggestedCount alloc increases it by one
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
        dprintf(("**EndDeferWindowPos %x (%d,%d) (%d,%d) %x", winpos->hwnd, winpos->x, winpos->y, winpos->cx, winpos->cy, winpos->flags));
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
 * Status    : COMPLETELY IMPLEMENTED AND TESTED
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

        while (hWnd != NULL)
        {
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
    BOOL ret = window->CloseWindow();
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
static BOOL IsPointInWindow(HWND hwnd, POINT point)
{
    RECT  rectWindow;
    DWORD hittest, dwStyle, dwExStyle;

    dwStyle   = GetWindowLongA(hwnd, GWL_STYLE);
    dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);

    GetWindowRect(hwnd, &rectWindow);

    /* If point is in window, and window is visible, and it  */
    /* is enabled (or it's a top-level window), then explore */
    /* its children. Otherwise, go to the next window.       */

    if( (dwStyle & WS_VISIBLE) &&
        ((dwExStyle & (WS_EX_LAYERED | WS_EX_TRANSPARENT)) != (WS_EX_LAYERED | WS_EX_TRANSPARENT)) &&
        (!(dwStyle & WS_DISABLED) || ((dwStyle & (WS_POPUP | WS_CHILD)) != WS_CHILD)) &&
        ((point.x >= rectWindow.left) && (point.x <  rectWindow.right) &&
         (point.y >= rectWindow.top) && (point.y <  rectWindow.bottom))
#if 1
        )
#else
        &&
        (wndPtr->hrgnWnd ?  PtInRegion(wndPtr->hrgnWnd, 1))
#endif
    {
        hittest = SendMessageA(hwnd, WM_NCHITTEST, 0, MAKELONG(point.x, point.y));
        if(hittest != HTTRANSPARENT) {
            return TRUE;
        }
    }
    return FALSE;
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
        hwnd = OS2ToWin32Handle(hwndOS2);
        while(hwnd)
        {
                if(IsPointInWindow(hwnd, point)) {
                    dprintf(("WindowFromPoint (%d,%d) %x->%x\n", point.x, point.y, hwndOS2, hwnd));
                    return hwnd;
                }
                //try siblings
                HWND hwndSibling;
                HWND hwndParent = GetParent(hwnd);

                if(hwndParent) {
                    hwndSibling = GetWindow(hwndParent, GW_CHILD);
                    while(hwndSibling) {
                        if(hwndSibling != hwnd) {
                            if(IsPointInWindow(hwndSibling, point)) {
                                dprintf(("WindowFromPoint (%d,%d) %x->%x\n", point.x, point.y, hwndOS2, hwndSibling));
                                return hwndSibling;
                            }
                        }
                        hwndSibling = GetWindow(hwndSibling, GW_HWNDNEXT);
                    }
                }
                hwnd = hwndParent;
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
    BOOL ret = window->IsWindowUnicode();
    RELEASE_WNDOBJ(window);
    return ret;
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
    BOOL   ret = TRUE;
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
    ret = window->EnumChildWindows(lpfn, lParam);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumWindows(WNDENUMPROC lpfn, LPARAM lParam)
{
    return windowDesktop->EnumWindows(lpfn, lParam);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API ArrangeIconicWindows( HWND parent)
{
    RECT rectParent;
    HWND hwndChild;
    INT x, y, xspacing, yspacing;

    dprintf(("USER32: ArrangeIconicWindows %x", parent));
    dprintf(("USER32: TODO: icon title!!"));

    GetClientRect(parent, &rectParent);
    x = rectParent.left;
    y = rectParent.bottom;
    xspacing = GetSystemMetrics(SM_CXICONSPACING);
    yspacing = GetSystemMetrics(SM_CYICONSPACING);

    hwndChild = GetWindow( parent, GW_CHILD );
    while (hwndChild)
    {
        if( IsIconic( hwndChild ) )
        {
//            WND *wndPtr = WIN_FindWndPtr(hwndChild);
            
//            WINPOS_ShowIconTitle( wndPtr, FALSE );
               
            SetWindowPos( hwndChild, 0, x + (xspacing - GetSystemMetrics(SM_CXICON)) / 2,
                            y - yspacing - GetSystemMetrics(SM_CYICON)/2, 0, 0,
                            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
//	    if( IsWindow(hwndChild) )
//                WINPOS_ShowIconTitle(wndPtr , TRUE );
//            WIN_ReleaseWndPtr(wndPtr);

            if (x <= rectParent.right - xspacing) x += xspacing;
            else
            {
                x = rectParent.left;
                y -= yspacing;
            }
        }
        hwndChild = GetWindow( hwndChild, GW_HWNDNEXT );
    }
    return yspacing;
}
//******************************************************************************
//restores iconized window to previous size/position
//******************************************************************************
BOOL WIN32API OpenIcon(HWND hwnd)
{
    dprintf(("USER32: OpenIcon %x", hwnd));

    if(!IsIconic(hwnd))
        return FALSE;
    ShowWindow(hwnd, SW_SHOWNORMAL);
    return TRUE;
}
//******************************************************************************
//SDK: Windows can only be shown with ShowOwnedPopups if they were previously
//     hidden with the same api
//TODO: -> needs testing
//******************************************************************************
ODINFUNCTION2(BOOL, ShowOwnedPopups,
              HWND, hwndOwner,
              BOOL, fShow)
{
    Win32BaseWindow *window, *owner;
    HWND hwnd;

    owner = Win32BaseWindow::GetWindowFromHandle(hwndOwner);
    if(!owner) {
        dprintf(("ShowOwnedPopups, window %x not found", hwndOwner));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    dprintf(("USER32: ShowOwnedPopups %x %d", hwndOwner, fShow));

    hwnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    while(hwnd) {
        window = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(window) {
            if(window == owner && (window->getStyle() & WS_POPUP))
            {
                if(fShow) {
                    if(window->getFlags() & WIN_NEEDS_SHOW_OWNEDPOPUP)
                    {
                        /*
                        * In Windows, ShowOwnedPopups(TRUE) generates WM_SHOWWINDOW messages with SW_PARENTOPENING,
                        * regardless of the state of the owner
                        */
                        SendMessageA(hwnd, WM_SHOWWINDOW, SW_SHOW, SW_PARENTOPENING);
                        window->setFlags(window->getFlags() & ~WIN_NEEDS_SHOW_OWNEDPOPUP);
                    }
                }
                else
                {
                    if(IsWindowVisible(hwnd))
                    {
                        /*
                         * In Windows, ShowOwnedPopups(FALSE) generates WM_SHOWWINDOW messages with SW_PARENTCLOSING,
                         * regardless of the state of the owner
                         */
                        SendMessageA(hwnd, WM_SHOWWINDOW, SW_HIDE, SW_PARENTCLOSING);
                        window->setFlags(window->getFlags() | WIN_NEEDS_SHOW_OWNEDPOPUP);
                    }
                }
            }
            RELEASE_WNDOBJ(window);
        }
        else dprintf(("WARNING: window %x is not valid", hwnd));

        hwnd = GetWindow(hwnd, GW_HWNDNEXT);
    }
    RELEASE_WNDOBJ(owner);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HWND, GetForegroundWindow)
{
  HWND hwnd;

  hwnd = OS2ToWin32Handle(OSLibWinQueryActiveWindow());
  return hwnd;
}
//******************************************************************************

/******************************************************************************
 * The return value identifies the most recently active pop-up window.
 * The return value is the same as the hWnd parameter, if any of the
 * following conditions are met:
 *
 * - The window identified by hWnd was most recently active.
 * - The window identified by hWnd does not own any pop-up windows.
 * - The window identified by hWnd is not a top-level window or it is
 *   owned by another window.
 */
ODINFUNCTION1(HWND, GetLastActivePopup,
              HWND, hWnd)
{
  Win32BaseWindow *owner;

  owner = Win32BaseWindow::GetWindowFromHandle(hWnd);
  if(!owner) 
  {
    dprintf(("GetLastActivePopup, window %x not found", hWnd));
    SetLastError(ERROR_INVALID_WINDOW_HANDLE);
    return hWnd;
  }

  HWND hwndRetVal = owner->getLastActive();
  if (!IsWindow( hwndRetVal ))
    hwndRetVal = owner->getWindowHandle();
  
  RELEASE_WNDOBJ(owner);
  
  return hwndRetVal;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD,   GetWindowThreadProcessId,
              HWND,    hWnd,
              PDWORD,  lpdwProcessId)
{
  hWnd = Win32ToOS2Handle(hWnd);
  return O32_GetWindowThreadProcessId(hWnd,lpdwProcessId);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GetWindowContextHelpId,
              HWND,  hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetWindowContextHelpId, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("GetWindowContextHelpId %x", hwnd));
    DWORD ret = window->getWindowContextHelpId();
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,  SetWindowContextHelpId,
              HWND,  hwnd,
              DWORD, dwContextHelpId)
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
    RELEASE_WNDOBJ(window);
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HANDLE, GetPropA,
              HWND,   hwnd,
              LPCSTR, str )
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetPropA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    HANDLE ret = window->getProp(str);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HANDLE,  GetPropW,
              HWND,    hwnd,
              LPCWSTR, str )
{
  Win32BaseWindow *window;
  LPSTR strA;
  HANDLE ret;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetPropW, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }

    if(HIWORD(str)) {
         strA = HEAP_strdupWtoA( GetProcessHeap(), 0, str );
    }
    else strA = (LPSTR)str;

    ret = window->getProp(strA);

    RELEASE_WNDOBJ(window);
    if(HIWORD(str)) HeapFree( GetProcessHeap(), 0, strA );
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,   SetPropA,
              HWND,   hwnd,
              LPCSTR, str,
              HANDLE, handle )
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetPropA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    BOOL ret = window->setProp(str, handle);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,    SetPropW,
              HWND,    hwnd,
              LPCWSTR, str,
              HANDLE,  handle )
{
    BOOL ret;
    LPSTR strA;

    if (!HIWORD(str))
        return SetPropA( hwnd, (LPCSTR)(UINT)LOWORD(str), handle );
    strA = HEAP_strdupWtoA( GetProcessHeap(), 0, str );
    ret = SetPropA( hwnd, strA, handle );
    HeapFree( GetProcessHeap(), 0, strA );
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HANDLE, RemovePropA,
              HWND,   hwnd,
              LPCSTR, str )
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("RemovePropA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    HANDLE ret = window->removeProp(str);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HANDLE,  RemovePropW,
              HWND,    hwnd,
              LPCWSTR, str )
{
    LPSTR strA;
    HANDLE ret;

    if (!HIWORD(str))
        return RemovePropA( hwnd, (LPCSTR)(UINT)LOWORD(str) );
    strA = HEAP_strdupWtoA( GetProcessHeap(), 0, str );
    ret = RemovePropA( hwnd, strA );
    HeapFree( GetProcessHeap(), 0, strA );
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(INT, EnumPropsA,
              HWND, hwnd,
              PROPENUMPROCA, func )
{
    return EnumPropsExA( hwnd, (PROPENUMPROCEXA)func, 0 );
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(INT, EnumPropsW,
              HWND, hwnd,
              PROPENUMPROCW, func )
{
    return EnumPropsExW( hwnd, (PROPENUMPROCEXW)func, 0 );
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(INT, EnumPropsExA,
              HWND, hwnd,
              PROPENUMPROCEXA, func,
              LPARAM, lParam)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("EnumPropsExA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return -1;
    }
    INT ret = window->enumPropsExA(func, lParam);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(INT, EnumPropsExW,
              HWND, hwnd,
              PROPENUMPROCEXW, func,
              LPARAM, lParam)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("EnumPropsExA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return -1;
    }
    INT ret = window->enumPropsExW(func, lParam);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************


/*****************************************************************************
 * Name      : BOOL WIN32API AnyPopup
 * Purpose   : The AnyPopup function indicates whether an owned, visible,
 *             top-level pop-up, or overlapped window exists on the screen. The
 *             function searches the entire Windows screen, not just the calling
 *             application's client area.
 * Parameters: VOID
 * Variables :
 * Result    : If a pop-up window exists, the return value is TRUE even if the
 *             pop-up window is completely covered by other windows. Otherwise,
 *             it is FALSE.
 * Remark    : AnyPopup is a Windows version 1.x function and is retained for
 *             compatibility purposes. It is generally not useful.
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
ODINFUNCTION0(BOOL, AnyPopup)
{
  dprintf(("USER32:AnyPopup() not implemented.\n"));

  return (FALSE);
}
