/* $Id: windowclass.cpp,v 1.23 2001-08-31 13:37:39 phaller Exp $ */
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
#include <unicode.h>
#include <win32class.h>
#include <win32wbase.h>
#include <controls.h>

#define DBG_LOCALLOG    DBG_windowclass
#include "dbglocal.h"


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

ODINDEBUGCHANNEL(USER32-CLASS)


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

ODINFUNCTION1(ATOM,              RegisterClassA,
              CONST WNDCLASSA *, lpWndClass)
{
 WNDCLASSEXA wc;
 Win32WndClass *wclass;

    //CB: size new in ex structure
    wc.cbSize = sizeof(wc);
    memcpy(&wc.style, lpWndClass, sizeof(WNDCLASSA));
    wc.hIconSm = 0;
 
    wclass = Win32WndClass::FindClass(wc.hInstance, (LPSTR)wc.lpszClassName);
    if(wclass) {
        RELEASE_CLASSOBJ(wclass);
        if(HIWORD(wc.lpszClassName)) {
                dprintf(("RegisterClassA %x %s already exists", wc.hInstance, wc.lpszClassName));
        }
        else    dprintf(("RegisterClassA %x %x already exists", wc.hInstance, wc.lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    //TODO: not destroyed when class is unregistered (neither does Wine, but that might be a bug)
    dprintf(("RegisterClassA"));
    int iSmIconWidth  = GetSystemMetrics(SM_CXSMICON);
    int iSmIconHeight = GetSystemMetrics(SM_CYSMICON);

    if(wc.hIcon)
        wc.hIconSm = CopyImage(wc.hIcon, IMAGE_ICON, iSmIconWidth, iSmIconHeight,
                               LR_COPYFROMRESOURCE);

    wclass = new Win32WndClass(&wc,FALSE);
    if(wclass == NULL) {
        dprintf(("ERROR: RegisterClassA winclass == NULL!"));
        DebugInt3();
        return(0);
    }
    ATOM atom = wclass->getAtom();
    RELEASE_CLASSOBJ(wclass);
    return atom;
}
//******************************************************************************
//Note: RegisterClassA does NOT change the last error if successful! (verified in NT 4, SP6)
//******************************************************************************
ODINFUNCTION1(ATOM,                RegisterClassExA,
              CONST WNDCLASSEXA *, lpWndClass)
{
 Win32WndClass *wclass;

    wclass = Win32WndClass::FindClass(lpWndClass->hInstance, (LPSTR)lpWndClass->lpszClassName);
    if(wclass) {
        RELEASE_CLASSOBJ(wclass);
        if(HIWORD(lpWndClass->lpszClassName)) {
                dprintf(("RegisterClassExA %x %s already exists", lpWndClass->hInstance, lpWndClass->lpszClassName));
        }
        else    dprintf(("RegisterClassExA %x %x already exists", lpWndClass->hInstance, lpWndClass->lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    dprintf(("RegisterClassExA"));
    wclass = new Win32WndClass((WNDCLASSEXA *)lpWndClass,FALSE);
    if(wclass == NULL) {
        dprintf(("ERROR: RegisterClassExA winclass == NULL!"));
        DebugInt3();
        return(0);
    }
    ATOM atom = wclass->getAtom();
    RELEASE_CLASSOBJ(wclass);
    return atom;
}
//******************************************************************************
//Note: RegisterClassA does NOT change the last error if successful! (verified in NT 4, SP6)
//******************************************************************************
ODINFUNCTION1(ATOM,              RegisterClassW,
              CONST WNDCLASSW *, lpwc)
{
 ATOM rc;
 WNDCLASSEXA wc;
 Win32WndClass *winclass;

    //CB: size new in ex structure
    wc.cbSize = sizeof(wc);
    memcpy(&wc.style, lpwc, sizeof(WNDCLASSA));

    winclass = Win32WndClass::FindClass(wc.hInstance, (LPWSTR)wc.lpszClassName);
    if(winclass) {
        RELEASE_CLASSOBJ(winclass);
        if(HIWORD(wc.lpszClassName)) {
                dprintf(("RegisterClassW %x %ls already exists", wc.hInstance, wc.lpszClassName));
        }
        else    dprintf(("RegisterClassW %x %x already exists", wc.hInstance, wc.lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    //TODO: not destroyed when class is unregistered (neither does Wine, but that might be a bug)
    int iSmIconWidth  = GetSystemMetrics(SM_CXSMICON);
    int iSmIconHeight = GetSystemMetrics(SM_CYSMICON);

    if(wc.hIcon)
        wc.hIconSm = CopyImage(wc.hIcon, IMAGE_ICON, iSmIconWidth, iSmIconHeight,
                               LR_COPYFROMRESOURCE);

    dprintf(("RegisterClassW"));
    winclass = new Win32WndClass((WNDCLASSEXA *)&wc, TRUE);
    if(winclass == NULL) {
        dprintf(("ERROR: RegisterClassW winclass == NULL!"));
        DebugInt3();
        return 0;
    }
    rc = winclass->getAtom();
    RELEASE_CLASSOBJ(winclass);
    return(rc);
}
//******************************************************************************
//Note: RegisterClassA does NOT change the last error if successful! (verified in NT 4, SP6)
//******************************************************************************
ODINFUNCTION1(ATOM,                RegisterClassExW,
              CONST WNDCLASSEXW *, lpwc)
{
 ATOM rc;
 WNDCLASSEXA wc;
 Win32WndClass *winclass;

    memcpy(&wc, lpwc, sizeof(WNDCLASSEXA));

    winclass = Win32WndClass::FindClass(wc.hInstance, (LPWSTR)wc.lpszClassName);
    if(winclass) {
        RELEASE_CLASSOBJ(winclass);
        if(HIWORD(wc.lpszClassName)) {
                dprintf(("RegisterClassExW %x %ls already exists", wc.hInstance, wc.lpszClassName));
        }
        else    dprintf(("RegisterClassExW %x %x already exists", wc.hInstance, wc.lpszClassName));
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }

    dprintf(("RegisterClassExW"));
    winclass = new Win32WndClass((WNDCLASSEXA *)&wc, TRUE);
    if(winclass == NULL) {
        dprintf(("ERROR: RegisterClassExW winclass == NULL!"));
        DebugInt3();
        return(0);
    }
    rc = winclass->getAtom();
    RELEASE_CLASSOBJ(winclass);

    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,      UnregisterClassA,
              LPCSTR,    lpszClassName,
              HINSTANCE, hinst)
{
 BOOL ret;

   ret = Win32WndClass::UnregisterClassA(hinst, (LPSTR)lpszClassName);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,      UnregisterClassW,
              LPCWSTR,   lpszClassName,
              HINSTANCE, hinst)
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

  return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,        GetClassInfoA,
              HINSTANCE,   hInstance,
              LPCSTR,      lpszClass,
              WNDCLASSA *, lpwc)
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
        RELEASE_CLASSOBJ(wndclass);
        memcpy(lpwc, &wc.style, sizeof(WNDCLASSA));
        SetLastError(ERROR_SUCCESS);
        return(TRUE);
  }
  SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,        GetClassInfoW,
              HINSTANCE,   hinst,
              LPCWSTR,     lpszClass,
              WNDCLASSW *, lpwc)
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
        RELEASE_CLASSOBJ(wndclass);
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
ODINFUNCTION3(BOOL,          GetClassInfoExA,
              HINSTANCE,     hInstance,
              LPCTSTR,       lpszClass,
              LPWNDCLASSEXA, lpwcx)
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
        RELEASE_CLASSOBJ(wndclass);
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

ODINFUNCTION3(BOOL,          GetClassInfoExW,
              HINSTANCE,     hInstance,
              LPCWSTR,       lpszClass,
              LPWNDCLASSEXW, lpwcx)
{
 BOOL           rc;
 Win32WndClass *wndclass;
 char          *astring = NULL;

  if(HIWORD(lpszClass)) {
       dprintf(("USER32:GetClassInfoExW (%08xh,%ls,%08x)",
                 hInstance, lpszClass, lpwcx));
  }
  else dprintf(("USER32:GetClassInfoExW (%08xh,%x,%08x)",
                 hInstance, lpszClass, lpwcx));

  if(HIWORD(lpszClass) != 0) {
        astring = UnicodeToAsciiString((LPWSTR)lpszClass);
  }
  else  astring = (char *)lpszClass;

  wndclass = Win32WndClass::FindClass(hInstance, astring);
  if(HIWORD(astring) != 0)
        FreeAsciiString((char *)astring);

  if(wndclass) {
        wndclass->getClassInfo(lpwcx);
        RELEASE_CLASSOBJ(wndclass);
        lpwcx->cbSize = sizeof(WNDCLASSEXW);
        SetLastError(ERROR_SUCCESS);
        return(TRUE);
  }
  SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,   GetClassNameA,
              HWND,  hwnd,
              LPSTR, lpszClassName,
              int,   cchClassName)
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
    RELEASE_WNDOBJ(wnd);
    dprintf(("USER32: GetClassNameA %x %s (%d)", hwnd, lpszClassName, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,    GetClassNameW,
              HWND,   hwnd,
              LPWSTR, lpszClassName,
              int,    cchClassName)
{
 Win32BaseWindow *wnd;
 int              ret;

    dprintf(("USER32: GetClassNameW\n"));
    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassNameA wnd == NULL"));
        return(0);
    }
    ret = (wnd->getClass())->getClassName(lpszClassName, cchClassName);
    RELEASE_WNDOBJ(wnd);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, SetClassLongA,
			  HWND, hwnd, 
			  int,  nIndex, 
			  LONG, lNewVal)
{
 Win32BaseWindow *wnd;
 LONG             ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("SetClassLongA %x %d %x wnd == NULL", hwnd, nIndex, lNewVal));
        return(0);
    }
    ret = (wnd->getClass())->setClassLongA(nIndex, lNewVal);
    RELEASE_WNDOBJ(wnd);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG,   SetClassLongW,
              HWND,   hwnd,
              int,    nIndex,
              LONG,   lNewVal)
{
 Win32BaseWindow *wnd;
 LONG             ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("SetClassLongW %x %d %x wnd == NULL", hwnd, nIndex, lNewVal));
        return(0);
    }
    ret = (wnd->getClass())->setClassLongW(nIndex, lNewVal);
    RELEASE_WNDOBJ(wnd);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(WORD,  SetClassWord,
              HWND,  hwnd,
              int,   nIndex,
              WORD,  wNewVal)
{
 Win32BaseWindow *wnd;
 LONG             ret;

    dprintf(("USER32: SetClassWord %x %d %x", hwnd, nIndex, (ULONG)wNewVal));
    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("SetClassWordA %x %d %x wnd == NULL", hwnd, nIndex, wNewVal));
        return(0);
    }
    ret = (wnd->getClass())->setClassWord(nIndex, wNewVal);
    RELEASE_WNDOBJ(wnd);
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(WORD, GetClassWord,
              HWND, hwnd,
              int,  nIndex)
{
 Win32BaseWindow *wnd;
 WORD             ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassWordA %x %d wnd == NULL", hwnd, nIndex));
        return(0);
    }
    ret = (wnd->getClass())->getClassWord(nIndex);
    RELEASE_WNDOBJ(wnd);
    dprintf(("USER32: GetClassWord %x %d returned %x", hwnd, nIndex, (ULONG)ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, GetClassLongA,
              HWND, hwnd,
              int,  nIndex)
{
 Win32BaseWindow *wnd;
 LONG ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassLongA %x %d wnd == NULL", hwnd, nIndex));
        return(0);
    }
    ret = (wnd->getClass())->getClassLongA(nIndex);
    RELEASE_WNDOBJ(wnd);
    dprintf(("USER32: GetClassLongA %x %d returned %x", hwnd, nIndex, ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, GetClassLongW,
              HWND, hwnd,
              int,  nIndex)
{
 Win32BaseWindow *wnd;
 LONG ret;

    wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf(("GetClassLongW %x %d wnd == NULL", hwnd, nIndex));
        return(0);
    }
    ret = (wnd->getClass())->getClassLongW(nIndex);
    RELEASE_WNDOBJ(wnd);
    dprintf(("USER32: GetClassLongW %x %d returned %x", hwnd, nIndex, ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
