/* $Id: window.cpp,v 1.2 1999-07-14 21:05:59 cbratschi Exp $ */
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
 */

#include <os2win.h>
#include <misc.h>
#include <win32wnd.h>

//******************************************************************************
//******************************************************************************
HWND WIN32API CreateWindowExA(DWORD exStyle, LPCSTR className,
                              LPCSTR windowName, DWORD style, INT x,
                              INT y, INT width, INT height,
                              HWND parent, HMENU menu,
                              HINSTANCE instance, LPVOID data )
{
  Win32Window *window;
  ATOM classAtom;
  CREATESTRUCTA cs;

    if(exStyle & WS_EX_MDICHILD)
        return CreateMDIWindowA(className, windowName, style, x, y, width, height, parent, instance, (LPARAM)data);

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
    window = new Win32Window( &cs, classAtom, FALSE );
    if(window == NULL)
    {
        dprintf(("Win32Window creation failed!!"));
        return 0;
    }
    if(GetLastError() != 0)
    {
        dprintf(("Win32Window error found!!"));
        delete window;
        return 0;
    }
    return window->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateMDIWindowA(LPCSTR arg1, LPCSTR arg2, DWORD arg3,
                               int arg4, int arg5, int arg6, int arg7,
                               HWND arg8, HINSTANCE arg9, LPARAM  arg10)
{
 HWND hwnd;

    dprintf(("USER32:  CreateMDIWindowA\n"));

    hwnd = O32_CreateMDIWindow((LPSTR)arg1, (LPSTR)arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

    dprintf(("USER32:  CreateMDIWindowA returned %X\n", hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateMDIWindowW(LPCWSTR arg1, LPCWSTR arg2, DWORD arg3, int arg4,
                               int arg5, int arg6, int arg7, HWND arg8, HINSTANCE arg9,
                               LPARAM arg10)
{
 HWND hwnd;
 char *astring1 = NULL, *astring2 = NULL;

    if((int)arg1 >> 16 != 0) {
        astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    }
    else    astring1 = (char *)arg2;

    astring2 = UnicodeToAsciiString((LPWSTR)arg2);

    hwnd = O32_CreateMDIWindow(astring1, astring2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

    if(astring1)    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    dprintf(("USER32:  CreateMDIWindowW hwnd = %X\n", hwnd));
    return(hwnd);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateWindowExW(DWORD     arg1,
                              LPCWSTR   arg2,
                              LPCWSTR   arg3,
                              DWORD     dwStyle,
                              int       arg5,
                              int       arg6,
                              int       arg7,
                              int       arg8,
                              HWND      arg9,
                              HMENU     arg10,
                              HINSTANCE arg11,
                              PVOID     arg12)
{
  HWND hwnd;
  char *astring1 = NULL,
       *astring2 = NULL;

  /* @@@PH 98/06/21 changed to call OS2CreateWindowExA */
  if(HIWORD(arg2) != 0)
    astring1 = UnicodeToAsciiString((LPWSTR)arg2);
  else
    astring1 = (char *)arg2;

  astring2 = UnicodeToAsciiString((LPWSTR)arg3);

#ifdef DEBUG
    WriteLog("USER32:  CreateWindowExW: dwExStyle = %X\n", arg1);
    if((int)arg2 >> 16 != 0)
         WriteLog("USER32:  CreateWindow: classname = %s\n", astring1);
    else WriteLog("USER32:  CreateWindow: classname = %X\n", arg2);
    WriteLog("USER32:  CreateWindow: windowname= %s\n", astring2);
    WriteLog("USER32:  CreateWindow: dwStyle   = %X\n", dwStyle);
    WriteLog("USER32:  CreateWindow: x         = %d\n", arg5);
    WriteLog("USER32:  CreateWindow: y         = %d\n", arg6);
    WriteLog("USER32:  CreateWindow: nWidth    = %d\n", arg7);
    WriteLog("USER32:  CreateWindow: nHeight   = %d\n", arg8);
    WriteLog("USER32:  CreateWindow: parent    = %X\n", arg9);
    WriteLog("USER32:  CreateWindow: hwmenu    = %X\n", arg10);
    WriteLog("USER32:  CreateWindow: hinstance = %X\n", arg11);
    WriteLog("USER32:  CreateWindow: param     = %X\n", arg12);
 #endif

  hwnd = CreateWindowExA(arg1,
                            astring1,
                            astring2,
                            dwStyle,
                            arg5,
                            arg6,
                            arg7,
                            arg8,
                            arg9,
                            arg10,
                            arg11,
                            arg12);

    if(HIWORD(arg1) != 0)
        FreeAsciiString(astring1);

    FreeAsciiString(astring2);

#ifdef DEBUG
    WriteLog("USER32:  ************CreateWindowExW hwnd = %X (%X)\n", hwnd, GetLastError());
#endif
    return(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyWindow(HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyWindow\n");
#endif
    return O32_DestroyWindow(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetActiveWindow( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetActiveWindow\n");
#endif
    return O32_SetActiveWindow(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetParent( HWND arg1)
{
#ifdef DEBUG
////    WriteLog("USER32:  GetParent\n");
#endif
    return O32_GetParent(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetParent( HWND arg1, HWND  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  SetParent\n");
#endif
    return O32_SetParent(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsChild( HWND arg1, HWND  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  IsChild\n");
#endif
    return O32_IsChild(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetTopWindow( HWND arg1)
{
#ifdef DEBUG
////    WriteLog("USER32:  GetTopWindow\n");
#endif
    return O32_GetTopWindow(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UpdateWindow(HWND hwnd)
{
  dprintf(("USER32:  UpdateWindow\n"));

  return O32_UpdateWindow(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsIconic( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsIconic\n");
#endif
    return O32_IsIconic(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetWindow(HWND arg1, UINT arg2)
{
 HWND rc;

    rc = O32_GetWindow(arg1, arg2);
#ifdef DEBUG
    WriteLog("USER32:  GetWindow %X %d returned %d\n", arg1, arg2, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API GetWindowDC(HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowDC\n");
#endif
    return O32_GetWindowDC(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableWindow( HWND arg1, BOOL  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnableWindow\n");
#endif
    return O32_EnableWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BringWindowToTop( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  BringWindowToTop\n");
#endif
    return O32_BringWindowToTop(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetActiveWindow()
{
  return(O32_GetActiveWindow());
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowWindow(HWND arg1, int arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ShowWindow %X %d\n", arg1, arg2);
#endif
    return O32_ShowWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowPos( HWND arg1, HWND arg2, int arg3, int arg4, int arg5, int arg6, UINT  arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  SetWindowPos\n");
#endif
    return O32_SetWindowPos(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
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

    if (O32_GetWindowPlacement( hwnd, &wndpl ))
    {
   if (rectWnd) *rectWnd = wndpl.rcNormalPosition;
   if (ptIcon)  *ptIcon = wndpl.ptMinPosition;
   return wndpl.showCmd;
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindow( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsWindow\n");
#endif
    return O32_IsWindow(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowEnabled( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsWindowEnabled\n");
#endif
    return O32_IsWindowEnabled(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowVisible( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsWindowVisible\n");
#endif
    return O32_IsWindowVisible(arg1);
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
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowRect( HWND arg1, PRECT  arg2)
{
 BOOL rc;

    rc = O32_GetWindowRect(arg1, arg2);
    dprintf(("USER32:  GetWindowRect %X returned %d\n", arg1, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowTextA(HWND arg1, LPCSTR  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  SetWindowText %s\n", arg2);
#endif
    return O32_SetWindowText(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowTextW( HWND arg1, LPCWSTR arg2)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 BOOL  rc;

   rc = SetWindowTextA(arg1, (LPCSTR)astring);
   dprintf(("USER32:  SetWindowTextW %X %s returned %d\n", arg1, astring, rc));
   FreeAsciiString(astring);
   return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetForegroundWindow(HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetForegroundWindow\n");
#endif
    return O32_SetForegroundWindow(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClientRect( HWND arg1, PRECT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClientRect of %X\n", arg1);
#endif

    return O32_GetClientRect(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowA(LPCSTR arg1, LPCSTR arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  FindWindow\n");
#endif
    return O32_FindWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowExA(HWND arg1, HWND arg2, LPCSTR arg3, LPCSTR arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  FindWindowExA, not completely implemented\n");
#endif
    return O32_FindWindow(arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlashWindow( HWND arg1, BOOL  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  FlashWindow\n");
#endif
    return O32_FlashWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveWindow(HWND arg1, int arg2, int arg3, int arg4, int arg5, BOOL arg6)
{
 BOOL rc;

    rc = O32_MoveWindow(arg1, arg2, arg3, arg4, arg5, arg6);
    dprintf(("USER32:  MoveWindow %X to (%d,%d) size (%d,%d), repaint = %d returned %d\n", arg1, arg2, arg3, arg4, arg5, arg6, rc));
    return(rc);
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
HWND WIN32API ChildWindowFromPointEx(HWND arg1, POINT arg2, UINT uFlags)
{
#ifdef DEBUG
    WriteLog("USER32:  ChildWindowFromPointEx, not completely supported!\n");
#endif
    return O32_ChildWindowFromPoint(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseWindow( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  CloseWindow\n");
#endif
    return O32_CloseWindow(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API WindowFromDC( HDC arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  WindowFromDC\n");
#endif
    return O32_WindowFromDC(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API WindowFromPoint( POINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  WindowFromPoint\n");
#endif
    return O32_WindowFromPoint(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowUnicode(HWND hwnd)
{
#ifdef DEBUG
  WriteLog("USER32:  IsWindowUnicode, not implemented\n");
#endif
  return(FALSE);
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
  dprintf(("USER32:FindWindowExW (%08xh,%08xh,%s,%s) not implemented.\n",
         hwndParent,
         hwndChildAfter,
         lpszClass,
         lpszWindow));

  return (NULL);
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
