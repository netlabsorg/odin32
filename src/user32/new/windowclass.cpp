/* $Id: windowclass.cpp,v 1.3 1999-07-15 08:18:11 sandervl Exp $ */
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
#include <win32wnd.h>
#include <handlemanager.h>

//******************************************************************************
//******************************************************************************
void RegisterSystemClasses(ULONG hModule)
{
}
//******************************************************************************
//******************************************************************************
void UnregisterSystemClasses()
{
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API RegisterClassA(CONST WNDCLASSA *lpWndClass)
{
 WNDCLASSEXA wc;
 Win32WndClass *wclass;

   //CB: size new in ex structure
   wc.cbSize = sizeof(wc);
   memcpy(&wc.style, lpWndClass, sizeof(WNDCLASSA));
   wc.hIconSm = 0;

   wclass = new Win32WndClass(&wc,FALSE);
   if(wclass == NULL) {
        dprintf(("RegisterClassA wclass == NULL!"));
        return(0);
   }
   return(wclass->getAtom());
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API RegisterClassExA(CONST WNDCLASSEXA *lpWndClass)
{
 Win32WndClass *wclass;

   wclass = new Win32WndClass((WNDCLASSEXA *)lpWndClass,FALSE);
   if(wclass == NULL) {
        dprintf(("RegisterClassExA wclass == NULL!"));
        return(0);
   }
   return(wclass->getAtom());
}
//******************************************************************************
//******************************************************************************
WORD WIN32API RegisterClassW(CONST WNDCLASSW *lpwc)
{
 ATOM rc;
 WNDCLASSEXA wclass;
 Win32WndClass *winclass;

    dprintf(("RegisterClassW\n"));
    //CB: size new in ex structure
    wclass.cbSize = sizeof(wclass);
    memcpy(&wclass.style, lpwc, sizeof(WNDCLASSA));
    if(wclass.lpszMenuName && (HIWORD(wclass.lpszMenuName) != 0)) {
        wclass.lpszMenuName = UnicodeToAsciiString((LPWSTR)lpwc->lpszMenuName);
    }
    if(wclass.lpszClassName && (HIWORD(wclass.lpszClassName) != 0)) {
        wclass.lpszClassName = UnicodeToAsciiString((LPWSTR)lpwc->lpszClassName);
    }
    winclass = new Win32WndClass((WNDCLASSEXA *)&wclass, TRUE);
    if(winclass == NULL) {
        dprintf(("RegisterClassW wclass == NULL!"));
        return(0);
    }
    rc = winclass->getAtom();

    if(lpwc->lpszMenuName && (HIWORD(lpwc->lpszMenuName) != 0)) {
        FreeAsciiString((char *)wclass.lpszMenuName);
    }
    if(lpwc->lpszClassName && (HIWORD(lpwc->lpszClassName) !=0)) {
        FreeAsciiString((char *)wclass.lpszClassName);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API RegisterClassExW(CONST WNDCLASSEXW *lpwc)
{
 ATOM rc;
 WNDCLASSEXA wclass;
 Win32WndClass *winclass;

    dprintf(("RegisterClassExW\n"));
    memcpy(&wclass, lpwc, sizeof(WNDCLASSEXA));
    if(wclass.lpszMenuName && (HIWORD(wclass.lpszMenuName) != 0)) {
        wclass.lpszMenuName = UnicodeToAsciiString((LPWSTR)lpwc->lpszMenuName);
    }
    if(wclass.lpszClassName && (HIWORD(wclass.lpszClassName) != 0)) {
        wclass.lpszClassName = UnicodeToAsciiString((LPWSTR)lpwc->lpszClassName);
    }

    winclass = new Win32WndClass((WNDCLASSEXA *)&wclass, TRUE);
    if(winclass == NULL) {
        dprintf(("RegisterClassExW wclass == NULL!"));
        return(0);
    }
    rc = winclass->getAtom();

    if(lpwc->lpszMenuName && (HIWORD(lpwc->lpszMenuName) != 0)) {
        FreeAsciiString((char *)wclass.lpszMenuName);
    }
    if(lpwc->lpszClassName && (HIWORD(lpwc->lpszClassName) != 0)) {
        FreeAsciiString((char *)wclass.lpszClassName);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterClassA(LPCSTR lpszClassName, HINSTANCE hinst)
{
   dprintf(("USER32:  UnregisterClassA\n"));
   Win32WndClass::UnregisterClassA(hinst, (LPSTR)lpszClassName);

   //Spintest returns FALSE in dll termination, so pretend it succeeded
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterClassW(LPCWSTR lpszClassName, HINSTANCE hinst)
{
 char *astring = NULL;

  dprintf(("USER32:  UnregisterClassW\n"));
  if(HIWORD(lpszClassName) != 0) {
        astring = UnicodeToAsciiString((LPWSTR)lpszClassName);
  }
  else  astring = (char *)lpszClassName;

  Win32WndClass::UnregisterClassA(hinst, (LPSTR)astring);
  if(HIWORD(astring) != 0)
        FreeAsciiString((char *)astring);

  //Spintest returns FALSE in dll termination, so pretend it succeeded
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClassInfoA(HINSTANCE hInstance, LPCSTR lpszClass, WNDCLASSA *lpwc)
{
 WNDCLASSEXA    wc;
 BOOL           rc;
 Win32WndClass *wndclass;

  dprintf(("USER32:  GetClassInfoA\n"));

  wndclass = Win32WndClass::FindClass(hInstance, (LPSTR)lpszClass);
  if(wndclass) {
        wndclass->getClassInfo(&wc);
        memcpy(lpwc, &wc, sizeof(WNDCLASSA));
        return(TRUE);
  }
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
        return(TRUE);
  }
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
 *               the data, the return value is TRUE;
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

  dprintf(("USER32:GetClassInfoExA (%08xh,%x,%08x).\n",
         hInstance,
         lpszClass,
         lpwcx));

  wndclass = Win32WndClass::FindClass(hInstance, (LPSTR)lpszClass);
  if(wndclass) {
        wndclass->getClassInfo(lpwcx);
        return(TRUE);
  }
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
        return(TRUE);
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClassNameA(HWND hwnd, LPSTR lpszClassName, int cchClassName)
{
 Win32Window *wnd;

    dprintf(("USER32: GetClassNameA\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("GetClassNameA wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->getClassName(lpszClassName, cchClassName);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClassNameW(HWND hwnd, LPWSTR lpszClassName, int cchClassName)
{
 Win32Window *wnd;

    dprintf(("USER32: GetClassNameW\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("GetClassNameA wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->getClassName(lpszClassName, cchClassName);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetClassLongA(HWND hwnd, int nIndex, LONG lNewVal)
{
 Win32Window *wnd;

    dprintf(("USER32: SetClassLongA\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("SetClassLongA wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->setClassLongA(nIndex, lNewVal);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetClassLongW(HWND hwnd, int nIndex, LONG lNewVal)
{
 Win32Window *wnd;

    dprintf(("USER32: SetClassLongW\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("SetClassLongW wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->setClassLongW(nIndex, lNewVal);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API SetClassWord(HWND hwnd, int nIndex, WORD  wNewVal)
{
 Win32Window *wnd;

    dprintf(("USER32: SetClassWordA\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("SetClassWordA wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->setClassWord(nIndex, wNewVal);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API GetClassWord(HWND hwnd, int nIndex)
{
 Win32Window *wnd;

    dprintf(("USER32: GetClassWordA\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("GetClassWordA wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->getClassWord(nIndex);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetClassLongA(HWND hwnd, int nIndex)
{
 Win32Window *wnd;

    dprintf(("USER32: GetClassLongA\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("GetClassLongA wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->getClassLongA(nIndex);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetClassLongW(HWND hwnd, int nIndex)
{
 Win32Window *wnd;

    dprintf(("USER32: GetClassLongW\n"));
    if(HMHandleTranslateToOS2(hwnd, (PULONG)&wnd) != NO_ERROR) {
        dprintf(("GetClassLongW wnd == NULL"));
        return(0);
    }
    return (wnd->getClass())->getClassLongW(nIndex);
}
//******************************************************************************
//******************************************************************************
