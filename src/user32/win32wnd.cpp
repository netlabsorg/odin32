/* $Id: win32wnd.cpp,v 1.10 2001-06-09 14:50:23 sandervl Exp $ */
/*
 * Win32 Window Class for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 * Parts based on Wine (windows\mdi.c) (990815)
 *
 * Copyright 1994, Bob Amstadt
 *           1995,1996 Alex Korobka
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <win.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <win32wnd.h>
#include <heapstring.h>
#include <spy.h>
#include "wndmsg.h"
#include <oslibwin.h>
#include <oslibutil.h>
#include <oslibgdi.h>
#include <oslibres.h>
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "mdi.h"
#include "win32wmdiclient.h"

#define DBG_LOCALLOG	DBG_win32wnd
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
Win32Window::Win32Window(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                : Win32BaseWindow(lpCreateStructA, classAtom, isUnicode)
{
}
//******************************************************************************
//******************************************************************************
Win32Window::~Win32Window()
{
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Window::DefFrameProcA(HWND hwndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
 Win32MDIClientWindow *window = NULL;
 HWND                  hwndChild;

    if(hwndMDIClient)
        window = (Win32MDIClientWindow*)GetWindowFromHandle(hwndMDIClient);

    if (window && window->isMDIClient())
    {
        switch(Msg)
        {
        case WM_NCACTIVATE:
            window->SendMessageA(Msg, wParam, lParam);
            break;

#if 0
        case WM_SETTEXT:
            //CB: infinite loop
            window->updateFrameText(MDI_REPAINTFRAME,(LPCSTR)lParam);
            return 0;
#endif

        case WM_COMMAND:
            /* check for possible syscommands for maximized MDI child */
            if(wParam <  window->getFirstChildId() || wParam >= window->getFirstChildId()+window->getNrOfChildren())
            {
                if( (wParam - 0xF000) & 0xF00F ) break;
                switch( wParam )
                {
                case SC_SIZE:
                case SC_MOVE:
                case SC_MINIMIZE:
                case SC_MAXIMIZE:
                case SC_NEXTWINDOW:
                case SC_PREVWINDOW:
                case SC_CLOSE:
                case SC_RESTORE:
                    hwndChild = window->getMaximizedChild();
                    RELEASE_WNDOBJ(window);
                    if (hwndChild)
                        return ::SendMessageA(hwndChild, WM_SYSCOMMAND, wParam, lParam);
                }
            }
            else
            {
                hwndChild = window->getChildByID(wParam);
                if (hwndChild)
                    ::SendMessageA(window->getWindowHandle(),WM_MDIACTIVATE,(WPARAM)hwndChild,0L);
            }
            break;

        case WM_SETFOCUS:
            SetFocus(hwndMDIClient);
            break;

        case WM_SIZE:
            MoveWindow(hwndMDIClient, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            break;

#if 0
        case WM_NEXTMENU:
            ci     = (MDICLIENTINFO*)wndPtr->wExtra;

            if( !(wndPtr->parent->dwStyle & WS_MINIMIZE)
            && ci->hwndActiveChild && !ci->hwndChildMaximized )
            {
            /* control menu is between the frame system menu and
            * the first entry of menu bar */

                if( (wParam == VK_LEFT &&
                 wndPtr->parent->wIDmenu == LOWORD(lParam)) ||
                (wParam == VK_RIGHT &&
                GetSubMenu16(wndPtr->parent->hSysMenu, 0) == LOWORD(lParam)) )
                {
                    LRESULT retvalue;
                    wndPtr = WIN_FindWndPtr(ci->hwndActiveChild);
                    retvalue = MAKELONG( GetSubMenu16(wndPtr->hSysMenu, 0),
                                      ci->hwndActiveChild);
                    return retvalue;
                }
            }
            break;
#endif
        }
    }
    if(window) RELEASE_WNDOBJ(window);
    return DefWindowProcA(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Window::DefFrameProcW(HWND hwndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
        case WM_NCACTIVATE:
            ::SendMessageW(hwndMDIClient, Msg, wParam, lParam);
            break;

        case WM_SETTEXT:
        {
            LPSTR txt = HEAP_strdupWtoA(GetProcessHeap(),0,(LPWSTR)lParam);
            LRESULT ret = DefFrameProcA(hwndMDIClient, Msg, wParam, (DWORD)txt );
            HeapFree(GetProcessHeap(),0,txt);
            return ret;
        }
        case WM_NEXTMENU:
        case WM_SETFOCUS:
        case WM_SIZE:
            return DefFrameProcA(hwndMDIClient, Msg, wParam, lParam );
    }
    return DefWindowProcW(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
