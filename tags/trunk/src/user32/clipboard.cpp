/* $Id: clipboard.cpp,v 1.5 2000-02-16 14:34:06 sandervl Exp $ */

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
#include "win32wbase.h"

#define DBG_LOCALLOG	DBG_clipboard
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
BOOL WIN32API ChangeClipboardChain( HWND hwndRemove, HWND hwndNext)
{
  Win32BaseWindow *wndRemove, *wndNext;

    wndRemove = Win32BaseWindow::GetWindowFromHandle(hwndRemove);
    if(!wndRemove) {
        dprintf(("ChangeClipboardChain, window %x not found", hwndRemove));
	SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    wndNext = Win32BaseWindow::GetWindowFromHandle(hwndNext);
    if(!wndNext) {
        dprintf(("ChangeClipboardChain, window %x not found", hwndNext));
	SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("USER32:  ChangeClipboardChain\n"));
    return O32_ChangeClipboardChain(wndRemove->getOS2FrameWindowHandle(), 
                                    wndNext->getOS2FrameWindowHandle());
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseClipboard(void)
{
    dprintf(("USER32:  CloseClipboard\n"));
    return O32_CloseClipboard();
}
//******************************************************************************
//******************************************************************************
int WIN32API CountClipboardFormats(void)
{
    dprintf(("USER32:  CountClipboardFormats\n"));
    return O32_CountClipboardFormats();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EmptyClipboard(void)
{
    dprintf(("USER32:  EmptyClipboard\n"));
    return O32_EmptyClipboard();
}
//******************************************************************************
//******************************************************************************
UINT WIN32API EnumClipboardFormats(UINT arg1)
{
    dprintf(("USER32:  EnumClipboardFormats\n"));
    return O32_EnumClipboardFormats(arg1);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetClipboardData( UINT arg1)
{
    dprintf(("USER32:  GetClipboardData\n"));
    return O32_GetClipboardData(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameA( UINT arg1, LPSTR arg2, int  arg3)
{
    dprintf(("USER32:  GetClipboardFormatNameA %s\n", arg2));
    return O32_GetClipboardFormatName(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameW(UINT arg1, LPWSTR arg2, int arg3)
{
 int   rc;
 char *astring = UnicodeToAsciiString(arg2);

    dprintf(("USER32:  GetClipboardFormatNameW %s\n", astring));
    rc = O32_GetClipboardFormatName(arg1, astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardOwner(void)
{
 HWND hwndOwner;
 Win32BaseWindow *window;

    dprintf(("USER32:  GetClipboardOwner\n"));
    hwndOwner = O32_GetClipboardOwner();

    window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndOwner);
    if(!window) {
	//an OS/2 window probably owns the clipboard, we pretend nobody owns it
	return NULL;
    }
    return window->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardViewer(void)
{
 Win32BaseWindow *window;
 HWND hwndViewer;

    dprintf(("USER32:  GetClipboardViewer\n"));
    hwndViewer = O32_GetClipboardViewer();

    window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndViewer);
    if(!window) {
	//probably an OS/2 window, we pretend it's nobody
	return NULL;
    }
    return window->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetOpenClipboardWindow(void)
{
 Win32BaseWindow *window;
 HWND hwnd;

    dprintf(("USER32:  GetOpenClipboardWindow\n"));
    hwnd = O32_GetOpenClipboardWindow();

    window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwnd);
    if(!window) {
	//probably an OS/2 window, we pretend it's nobody
	return NULL;
    }
    return window->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
int WIN32API GetPriorityClipboardFormat( PUINT arg1, int  arg2)
{
    dprintf(("USER32:  GetPriorityClipboardFormat\n"));
    return O32_GetPriorityClipboardFormat(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsClipboardFormatAvailable( UINT arg1)
{
    dprintf(("USER32:  IsClipboardFormatAvailable\n"));
    return O32_IsClipboardFormatAvailable(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OpenClipboard( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("OpenClipboard, window %x not found", hwnd));
	SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("USER32:  OpenClipboard\n"));
    return O32_OpenClipboard(window->getOS2FrameWindowHandle());
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterClipboardFormatA( LPCSTR arg1)
{
    dprintf(("USER32:  RegisterClipboardFormatA\n"));
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
HANDLE WIN32API SetClipboardData( UINT arg1, HANDLE  arg2)
{
    dprintf(("USER32:  SetClipboardData\n"));
    return O32_SetClipboardData(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetClipboardViewer( HWND hwndNew)
{
  Win32BaseWindow *wndnew, *wndold;
  HWND hwndOld;

    wndnew = Win32BaseWindow::GetWindowFromHandle(hwndNew);
    if(!wndnew) {
        dprintf(("OpenClipboard, window %x not found", hwndNew));
	SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("USER32:  SetClipboardViewer\n"));
    hwndOld = O32_SetClipboardViewer(wndnew->getOS2FrameWindowHandle());

    wndold = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndOld);
    if(!wndold) {
	//probably an OS/2 window, so pretend it's nobody
        return 0;
    }
    return wndold->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
