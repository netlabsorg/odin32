/* $Id: windowclass.cpp,v 1.16 2001-02-22 10:37:31 sandervl Exp $ */
/*
 * Win32 Window Class Code for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <win32class.h>
#include <win32wbase.h>
#include <controls.h>

#define DBG_LOCALLOG    DBG_windowclass
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
void RegisterSystemClasses(ULONG hModule)
{
    dprintf(("RegisterSystemClasses\n"));
    CONTROLS_Register();
}
//******************************************************************************
//******************************************************************************
void UnregisterSystemClasses()
{
    dprintf(("UnregisterSystemClasses\n"));
    CONTROLS_Unregister();
}
//******************************************************************************
//Note: RegisterClassA does NOT change the last error if successful! (verified in NT 4, SP6)
//******************************************************************************
ATOM WIN32API RegisterClassA(CONST WNDCLASSA *lpWndClass)
{
 WNDCLASSEXA wc;
 Win32WndClass *wclass;

    dprintf(("RegisterClassA\n"));
    //CB: size new in ex structure
    wc.cbSize = sizeof(wc);
    memcpy(&wc.style, lpWndClass, sizeof(WNDCLASSA));
    wc.hIconSm = 0;

    if(Win32WndClass::FindClass(wc.hInstance, (LPSTR)wc.lpszClassName)) {
        if(HIWORD(wc.lpszClassName)) {
                dprintf(("RegisterClassA %x %s already exists", wc.hInstance, wc.lpszClassName));
        }
        else    dprintf(("RegisterClassA %x %x already exists", wc.hInstance, wc.lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    wclass = new Win32WndClass(&wc,FALSE);
    if(wclass == NULL) {
        dprintf(("ERROR: RegisterClassA winclass == NULL!"));
        DebugInt3();
        return(0);
    }
    return(wclass->getAtom());
}
//******************************************************************************
//Note: RegisterClassA does NOT change the last error if successful! (verified in NT 4, SP6)
//******************************************************************************
ATOM WIN32API RegisterClassExA(CONST WNDCLASSEXA *lpWndClass)
{
 Win32WndClass *wclass;

    if(Win32WndClass::FindClass(lpWndClass->hInstance, (LPSTR)lpWndClass->lpszClassName)) {
        if(HIWORD(lpWndClass->lpszClassName)) {
                dprintf(("RegisterClassExA %x %s already exists", lpWndClass->hInstance, lpWndClass->lpszClassName));
        }
        else    dprintf(("RegisterClassExA %x %x already exists", lpWndClass->hInstance, lpWndClass->lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    dprintf(("RegisterClassExA\n"));
    wclass = new Win32WndClass((WNDCLASSEXA *)lpWndClass,FALSE);
    if(wclass == NULL) {
        dprintf(("ERROR: RegisterClassExA winclass == NULL!"));
        DebugInt3();
        return(0);
    }
    return(wclass->getAtom());
}
//******************************************************************************
//Note: RegisterClassA does NOT change the last error if successful! (verified in NT 4, SP6)
//******************************************************************************
WORD WIN32API RegisterClassW(CONST WNDCLASSW *lpwc)
{
 ATOM rc;
 WNDCLASSEXA wc;
 Win32WndClass *winclass;

    dprintf(("RegisterClassW\n"));
    //CB: size new in ex structure
    wc.cbSize = sizeof(wc);
    memcpy(&wc.style, lpwc, sizeof(WNDCLASSA));
    wc.hIconSm = 0;

    if(Win32WndClass::FindClass(wc.hInstance, (LPWSTR)wc.lpszClassName)) {
        dprintf(("RegisterClassW %x %x already exists", wc.hInstance, wc.lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    winclass = new Win32WndClass((WNDCLASSEXA *)&wc, TRUE);
    if(winclass == NULL) {
        dprintf(("ERROR: RegisterClassW winclass == NULL!"));
        DebugInt3();
        return 0;
    }
    rc = winclass->getAtom();

    return(rc);
}
//******************************************************************************
//Note: RegisterClassA does NOT change the last error if successful! (verified in NT 4, SP6)
//******************************************************************************
ATOM WIN32API RegisterClassExW(CONST WNDCLASSEXW *lpwc)
{
 ATOM rc;
 WNDCLASSEXA wc;
 Win32WndClass *winclass;

    dprintf(("RegisterClassExW\n"));
    memcpy(&wc, lpwc, sizeof(WNDCLASSEXA));

    if(Win32WndClass::FindClass(wc.hInstance, (LPWSTR)wc.lpszClassName)) {
        dprintf(("RegisterClassW %x %x already exists", wc.hInstance, wc.lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    winclass = new Win32WndClass((WNDCLASSEXA *)&wc, TRUE);
    if(winclass == NULL) {
        dprintf(("ERROR: RegisterClassExW winclass == NULL!"));
        DebugInt3();
        return(0);
    }
    rc = winclass->getAtom();

    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterClassA(LPCSTR lpszClassName, HINSTANCE hinst)
{
 BOOL ret;

   ret = Win32WndClass::UnregisterClassA(hinst, (LPSTR)lpszClassName);
#if 1
   return ret;
#else
  //Spintest returns FALSE in dll termination, so pretend it succeeded
//  return(TRUE);
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterClassW(LPCWSTR lpszClassName, HINSTANCE hinst)
{
 char *astring = NULL;
 BOOL  ret;

  dprintf(("USER32:  UnregisterClassW\n"));
  if(HIWORD(lpszClassName) != 0) {
        astring = UnicodeToAsciiString((LPWSTR)lpszClassName);
  }
  else  astring = (char *)lpszClassName;

  ret = Win32WndClass::UnregisterClassA(hinst, (LPSTR)astring);
  if(HIWORD(astring) != 0)
        FreeAsciiString((char *)astring);

#if 1
  return ret;
#else
  //Spintest returns FALSE in dll termination, so pretend it succeeded
//  return(TRUE);
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClassInfoA(HINSTANCE hInstance, LPCSTR lpszClass, WNDCLASSA *lpwc)
{
 WNDCLASSEXA    wc;
 BOOL           rc;
 Win32WndClass *wndclass;

  if(HIWORD(lpszClass) != 0) {
        dprintf(("USER32: GetClassInfoA %x %s %x", hInstance, lpszClass, lpwc));
  }
  else  dprintf(("USER32: GetClassInfoA %x %x %x", hInstance, lpszClass, lpwc));

  wndclass = Win32WndClass::FindClass(hInstance, (LPSTR)lpszClass);
  if(wndclass) {
        wndclass->getClassInfo(&wc);
        memcpy(lpwc, &wc.style, sizeof(WNDCLASSA));
        SetLastError(ERROR_SUCCESS);
        return(TRUE);
  }
  SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClassInfoW(HINSTANCE  hinst, LPCWSTR lpszClass, WNDCLASSW *lpwc)
{
 WNDCLASSEXW    wc;
 BOOL           rc;
 Win32WndClass *wndclass;
 char          *astring = NULL;

  dprintf(("USER32:  GetClassInfoW\n"));

  if(HIWORD(lpszClass) != 0) {
        astring = UnicodeToAsciiString((LPWSTR)lpszClass);
  }
  else  astring = (char *)lpszClass;

  wndclass = Win32WndClass::FindClass(hinst, astring);
  if(HIWORD(astring) != 0)
        FreeAsciiString((char *)astring);

  if(wndclass) {
        wndclass->getClassInfo(&wc);
        memcpy(lpwc, &wc.style, sizeof(WNDCLASSW));
        SetLastError(ERROR_SUCCESS);
        return(TRUE);
  }
  SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
  return(FALSE);
}
/****************************************************************************
 * Name      : BOOL WIN32API GetClassInfoExA
 * Purpose   : The GetClassInfoEx function retrieves information about a window
 *             class, including the handle of the small icon associated with the
 *             window class. GetClassInfo does not retrieve the handle of the small icon.
 * Parameters: HINSTANCE    hinst     handle of application instance
 *             LPCTSTR      lpszClass address of class name string
 *             LPWNDCLASSEX lpwcx     address of structure for class data
 * Variables :
 * Result    : If the function finds a matching class and successfully copies
 *             the data, the return value is TRUE;
 *             otherwise, it is FALSE.
 *             To get extended error information, call GetLastError.
 * Remark    : PH: does not obtain handle of the small icon
 *****************************************************************************/
BOOL WIN32API GetClassInfoExA(HINSTANCE     hInstance,
                              LPCTSTR       lpszClass,
                              LPWNDCLASSEXA lpwcx)
{
 BOOL           rc;
 Win32WndClass *wndclass;

  if(HIWORD(lpszClass)) {
       dprintf(("USER32:GetClassInfoExA (%08xh,%s,%08x)",
                 hInstance, lpszClass, lpwcx));
  }
  else dprintf(("USER32:GetClassInfoExA (%08xh,%x,%08x)",
                 hInstance, lpszClass, lpwcx));

  wndclass = Win32WndClass::FindClass(hInstance, (LPSTR)lpszClass);
  if(wndclass) {
        wndclass->getClassInfo(lpwcx);
        lpwcx->cbSize = sizeof(WNDCLASSEXA);
        SetLastError(ERROR_SUCCESS);
        return(TRUE);
  }
  SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
  return(FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetClassInfoExW
 * Purpose   : The GetClassInfoEx function retrieves information about a window
 *             class, including the handle of the small icon associated with the
 *             window class. GetClassInfo does not retrieve the handle of the small icon.
 * Parameters: HINSTANCE    hinst     handle of application instance
 *             LPCWSTR      lpszClass address of class name string
 *             LPWNDCLASSEX lpwcx     address of structure for class data
 * Variables :
 * Result    : If the function finds a matching class and successfully copies
 *               the data, the return value is TRUE;
 *             otherwise, it is FALSE.
 *             To get extended error information, call GetLastError.
 * Remark    : does not obtain handle of the small icon
 *
 *****************************************************************************/
BOOL WIN32API GetClassInfoExW(HINSTANCE     hInstance,
                                 LPCWSTR       lpszClass,
                                 LPWNDCLASSEXW lpwcx)
{
 BOOL           rc;
 Win32WndClass *wndclass;
 char          *astring = NULL;

  dprintf(("USER32: GetClassInfoExW\n"));

  if(HIWORD(lpszClass) != 0) {
        astring = UnicodeToAsciiString((LPWSTR)lpszClass);
  }
  else  astring = (char *)lpszClass;

  wndclass = Win32WndClass::FindClass(hInstance, astring);
  if(HIWORD(astring) != 0)
        FreeAsciiString((char *)astring);

  if(wndclass) {
        wndclass->getClassInfo(lpwcx);
        lpwcx->cbSize = sizeof(WNDCLASSEXW);
        SetLastError(ERROR_SUCCESS);
        return(TRUE);
  }
  SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClassNameA(HWND hwnd, LPSTR lpszClassName, int cchClassName)
{
 Win32BaseWindow *wnd;
 int rc;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassNameA wnd == NULL"));
        return(0);
    }
    *lpszClassName = 0;
    rc = (wnd->getClass())->getClassName(lpszClassName, cchClassName);
    dprintf(("USER32: GetClassNameA %x %s (%d)", hwnd, lpszClassName, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClassNameW(HWND hwnd, LPWSTR lpszClassName, int cchClassName)
{
 Win32BaseWindow *wnd;

    dprintf(("USER32: GetClassNameW\n"));
    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassNameA wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->getClassName(lpszClassName, cchClassName);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetClassLongA(HWND hwnd, int nIndex, LONG lNewVal)
{
 Win32BaseWindow *wnd;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("SetClassLongA %x %d %x wnd == NULL", hwnd, nIndex, lNewVal));
        return(0);
    }
    return (wnd->getClass())->setClassLongA(nIndex, lNewVal);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetClassLongW(HWND hwnd, int nIndex, LONG lNewVal)
{
 Win32BaseWindow *wnd;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("SetClassLongW %x %d %x wnd == NULL", hwnd, nIndex, lNewVal));
        return(0);
    }
    return (wnd->getClass())->setClassLongW(nIndex, lNewVal);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API SetClassWord(HWND hwnd, int nIndex, WORD  wNewVal)
{
 Win32BaseWindow *wnd;

    dprintf(("USER32: SetClassWord %x %d %x", hwnd, nIndex, (ULONG)wNewVal));
    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("SetClassWordA %x %d %x wnd == NULL", hwnd, nIndex, wNewVal));
        return(0);
    }
    return (wnd->getClass())->setClassWord(nIndex, wNewVal);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API GetClassWord(HWND hwnd, int nIndex)
{
 Win32BaseWindow *wnd;
 WORD             ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassWordA %x %d wnd == NULL", hwnd, nIndex));
        return(0);
    }
    ret = (wnd->getClass())->getClassWord(nIndex);
    dprintf(("USER32: GetClassWord %x %d returned %x", hwnd, nIndex, (ULONG)ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetClassLongA(HWND hwnd, int nIndex)
{
 Win32BaseWindow *wnd;
 LONG ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassLongA %x %d wnd == NULL", hwnd, nIndex));
        return(0);
    }
    ret = (wnd->getClass())->getClassLongA(nIndex);
    dprintf(("USER32: GetClassLongA %x %d returned %x", hwnd, nIndex, ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetClassLongW(HWND hwnd, int nIndex)
{
 Win32BaseWindow *wnd;
 LONG ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassLongW %x %d wnd == NULL", hwnd, nIndex));
        return(0);
    }
    ret = (wnd->getClass())->getClassLongW(nIndex);
    dprintf(("USER32: GetClassLongW %x %d returned %x", hwnd, nIndex, ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
