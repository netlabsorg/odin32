/* $Id: clipboard.cpp,v 1.17 2003-07-31 16:07:56 sandervl Exp $ */

/*
 * Win32 Clipboard API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * NOTE: Use OS/2 frame handles for Open32 functions
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <string.h>
#include "win32wbase.h"
#include <unicode.h>

#define DBG_LOCALLOG    DBG_clipboard
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
BOOL WIN32API ChangeClipboardChain( HWND hwndRemove, HWND hwndNext)
{
    Win32BaseWindow *wndRemove, *wndNext;
    HWND hwndOS2Remove, hwndOS2Next = 0;

    wndRemove = Win32BaseWindow::GetWindowFromHandle(hwndRemove);
    if(!wndRemove) {
        dprintf(("ChangeClipboardChain, window %x not found", hwndRemove));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    hwndOS2Remove = wndRemove->getOS2WindowHandle();
    RELEASE_WNDOBJ(wndRemove);

    if(hwndNext) {
        wndNext = Win32BaseWindow::GetWindowFromHandle(hwndNext);
        if(!wndNext) {
            dprintf(("ChangeClipboardChain, window %x not found", hwndNext));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
        hwndOS2Next = wndNext->getOS2WindowHandle();
        RELEASE_WNDOBJ(wndNext);
    }

    dprintf(("USER32:  ChangeClipboardChain\n"));
    return O32_ChangeClipboardChain(hwndOS2Remove, hwndOS2Next);
}
//******************************************************************************
//******************************************************************************
int WIN32API CountClipboardFormats(void)
{
    return O32_CountClipboardFormats();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EmptyClipboard(void)
{
    return O32_EmptyClipboard();
}
//******************************************************************************
//******************************************************************************
UINT WIN32API EnumClipboardFormats(UINT uFormat)
{
    return O32_EnumClipboardFormats(uFormat);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameA(UINT format,LPSTR lpszFormatName,int cchMaxCount)
{
    dprintf(("USER32: GetClipboardFormatNameA %d\n",format));
    return O32_GetClipboardFormatName(format,lpszFormatName,cchMaxCount);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameW(UINT format,LPWSTR lpszFormatName,int cchMaxCount)
{
    int   rc;
    char *astring = (CHAR*)malloc(cchMaxCount);

    rc = GetClipboardFormatNameA(format,astring,cchMaxCount);
    if (rc) AsciiToUnicode(astring,lpszFormatName);
    free(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardOwner(void)
{
    HWND hwndOwner;
    Win32BaseWindow *window;

    dprintf(("USER32: GetClipboardOwner\n"));
    hwndOwner = O32_GetClipboardOwner();

    window = Win32BaseWindow::GetWindowFromOS2Handle(hwndOwner);
    if(!window) {
        //an OS/2 window probably owns the clipboard, we pretend nobody owns it
        return NULL;
    }
    hwndOwner = window->getWindowHandle();
    RELEASE_WNDOBJ(window);
    return hwndOwner;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardViewer(void)
{
    Win32BaseWindow *window;
    HWND hwndViewer;

    dprintf(("USER32: GetClipboardViewer"));
    hwndViewer = O32_GetClipboardViewer();

    window = Win32BaseWindow::GetWindowFromOS2Handle(hwndViewer);
    if(!window) {
        //probably an OS/2 window, we pretend it's nobody
        return NULL;
    }
    hwndViewer = window->getWindowHandle();
    RELEASE_WNDOBJ(window);
    return hwndViewer;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetClipboardViewer( HWND hwndNew)
{
    Win32BaseWindow *wndnew, *wndold;
    HWND hwndOld;
    HWND hwndOS2New;

    wndnew = Win32BaseWindow::GetWindowFromHandle(hwndNew);
    if(!wndnew) {
        dprintf(("SetClipboardViewer, window %x not found", hwndNew));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("USER32: SetClipboardViewer %x", hwndNew));
    hwndOS2New = wndnew->getOS2WindowHandle();
    RELEASE_WNDOBJ(wndnew);

    hwndOld = O32_SetClipboardViewer(hwndOS2New);

    wndold = Win32BaseWindow::GetWindowFromOS2Handle(hwndOld);
    if(!wndold) {
        //probably an OS/2 window, so pretend it's nobody
        return 0;
    }
    hwndOld = wndold->getWindowHandle();
    RELEASE_WNDOBJ(wndold);
    return hwndOld;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetOpenClipboardWindow(void)
{
    Win32BaseWindow *window;
    HWND hwnd;

    hwnd = O32_GetOpenClipboardWindow();

    window = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
    if(!window) {
        //probably an OS/2 window, we pretend it's nobody
        return NULL;
    }
    hwnd = window->getWindowHandle();
    RELEASE_WNDOBJ(window);
    return hwnd;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetPriorityClipboardFormat( PUINT arg1, int arg2)
{
    dprintf(("USER32: GetPriorityClipboardFormat"));
    return O32_GetPriorityClipboardFormat(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsClipboardFormatAvailable( UINT arg1)
{
    dprintf(("USER32: IsClipboardFormatAvailable %x", arg1));
    return O32_IsClipboardFormatAvailable(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OpenClipboard( HWND hwnd)
{
    Win32BaseWindow *window;

    if (hwnd) {
        window = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!window) {
            dprintf(("OpenClipboard, window %x not found", hwnd));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
        hwnd = window->getOS2WindowHandle();
        RELEASE_WNDOBJ(window);
    }
    dprintf(("USER32: OpenClipboard %x", hwnd));
    return O32_OpenClipboard(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseClipboard(void)
{
    return O32_CloseClipboard();
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterClipboardFormatA( LPCSTR arg1)
{
    dprintf(("USER32:  RegisterClipboardFormatA %s", arg1));
    return O32_RegisterClipboardFormat(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterClipboardFormatW(LPCWSTR arg1)
{
    UINT  rc;
    char *astring = UnicodeToAsciiString((LPWSTR)arg1);

    dprintf(("USER32:  RegisterClipboardFormatW %s\n", astring));
    rc = O32_RegisterClipboardFormat(astring);
    FreeAsciiString(astring);
    dprintf(("USER32:  RegisterClipboardFormatW returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetClipboardData(UINT uFormat)
{
    HANDLE hRet = O32_GetClipboardData(uFormat);
    dprintf(("GetClipboardData %x returned %x", uFormat, hRet));
#ifdef DEBUG
    if(hRet) {
        if(uFormat == CF_TEXT || uFormat == CF_DSPTEXT) {
            char *lpszText = (char *)GlobalLock(hRet);
            if(lpszText) dprintf(("GetClipboardData %s", lpszText));
            GlobalUnlock(hRet);
        }
        if(uFormat == CF_UNICODETEXT) {
            LPWSTR lpszText = (LPWSTR)GlobalLock(hRet);
            if(lpszText) dprintf(("GetClipboardData %ls", lpszText));
            GlobalUnlock(hRet);
        }
    }
#endif
    return hRet;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API SetClipboardData( UINT uFormat, HANDLE hClipObj)
{
    dprintf(("SetClipboardData %x %x", uFormat, hClipObj));
#ifdef DEBUG
    if(hClipObj) {
        if(uFormat == CF_TEXT || uFormat == CF_DSPTEXT) {
            char *lpszText = (char *)GlobalLock(hClipObj);
            if(lpszText) dprintf(("SetClipboardData %s", lpszText));
            GlobalUnlock(hClipObj);
        }
        if(uFormat == CF_UNICODETEXT) {
            LPWSTR lpszText = (LPWSTR)GlobalLock(hClipObj);
            if(lpszText) dprintf(("SetClipboardData %ls", lpszText));
            GlobalUnlock(hClipObj);
        }
    }
#endif
    return O32_SetClipboardData(uFormat, hClipObj);
}
//******************************************************************************
//******************************************************************************
