/* $Id: win32wmisc.cpp,v 1.3 2003-05-27 09:46:30 sandervl Exp $ */
/*
 * Misc. functions for window management
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Parts based on Wine (windows\mdi.c; windows\win.c)
 *
 * Copyright 1994, Bob Amstadt
 *           1995,1996 Alex Korobka
 * Copyright 1993, 1994 Alexandre Julliard
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
#include <dbglog.h>
#include "win32wnd.h"
#include <heapstring.h>
#include <spy.h>
#include "wndmsg.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibdos.h"
#include "win32wndhandle.h"
#include "win32wmisc.h"
#include "ctrlconf.h"

//******************************************************************************
//******************************************************************************
/*******************************************************************
 *           WIN_ListChildren
 *
 * Build an array of the children of a given window. The array must be
 * freed with HeapFree. Returns NULL when no windows are found.
 */
HWND *WIN_ListChildren( HWND hwnd )
{
  Win32BaseWindow *parent = Win32BaseWindow::GetWindowFromHandle(hwnd), *win32wnd;
  HWND *list, *phwnd;
  UINT count = 0;

    if(parent == NULL) {
        dprintf(("ERROR: WIN_ListChildren invalid hwnd %x", hwnd));
        return 0;
    }

    /* First count the windows */
    win32wnd = (Win32BaseWindow*)parent->getFirstChild();
    while (win32wnd)
    {
        count++;
        win32wnd = (Win32BaseWindow*)win32wnd->getNextChild();
    }

    if( count )
    {
	/* Now build the list of all windows */

	if ((list = (HWND *)HeapAlloc( GetProcessHeap(), 0, sizeof(HWND) * (count + 1))) != NULL)
	{
            win32wnd = (Win32BaseWindow*)parent->getFirstChild();
	    phwnd = list;
            count = 0;
            while(win32wnd)
	    {
                *phwnd++ = win32wnd->getWindowHandle();
                count++;
                win32wnd = (Win32BaseWindow*)win32wnd->getNextChild();
	    }
            *phwnd = 0;
	}
	else count = 0;
    } else list = NULL;

    RELEASE_WNDOBJ(parent);
    return list;
}
//******************************************************************************
//******************************************************************************
MDICLIENTINFO *get_client_info( HWND hwndClient )
{
    MDICLIENTINFO *ret = NULL;
    Win32BaseWindow *client = Win32BaseWindow::GetWindowFromHandle(hwndClient);

    if (client)
    {
        if (client->getCBExtra() < sizeof(MDICLIENTINFO)) {
             dprintf(("WARNING: get_client_info %x is not an MDI client", hwndClient ));
        }
        else ret = (MDICLIENTINFO*)client->getExtraPtr();
        RELEASE_WNDOBJ(client);
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
void GetWindowRectParent(HWND hwnd, RECT *pRect)
{
    Win32BaseWindow *window = Win32BaseWindow::GetWindowFromHandle(hwnd);

    if (window)
    {
        *pRect = *window->getWindowRect();
        RELEASE_WNDOBJ(window);
    }
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API getSysMenu(HWND hwnd)
{
  Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

  if(win32wnd) {
      HMENU hmenu = win32wnd->GetSysMenu();
      RELEASE_WNDOBJ(win32wnd);
      return hmenu;
  }
  return (HMENU)0;
}
//******************************************************************************
//******************************************************************************
VOID setSysMenu(HWND hwnd,HMENU hMenu)
{
  Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

  if(win32wnd) {
      win32wnd->SetSysMenu(hMenu);
      RELEASE_WNDOBJ(win32wnd);
  }
}
/***********************************************************************
 *           NC_GetSysPopupPos
 */
void NC_GetSysPopupPos( HWND hwnd, RECT* rect )
{
    if (IsIconic(hwnd)) GetWindowRect( hwnd, rect );
    else
    {
#ifdef __WIN32OS2__
        Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if (!win32wnd) return;

        win32wnd->GetSysPopupPos(rect);

        RELEASE_WNDOBJ(win32wnd);
#else
        WND *wndPtr = WIN_FindWndPtr( hwnd );
        if (!wndPtr) return;

        NC_GetInsideRect( hwnd, rect );
        OffsetRect( rect, wndPtr->rectWindow.left, wndPtr->rectWindow.top);
        if (wndPtr->dwStyle & WS_CHILD)
            ClientToScreen( GetParent(hwnd), (POINT *)rect );
        if (TWEAK_WineLook == WIN31_LOOK) {
            rect->right = rect->left + GetSystemMetrics(SM_CXSIZE);
            rect->bottom = rect->top + GetSystemMetrics(SM_CYSIZE);
        }
        else {
            rect->right = rect->left + GetSystemMetrics(SM_CYCAPTION) - 1;
            rect->bottom = rect->top + GetSystemMetrics(SM_CYCAPTION) - 1;
        }
        WIN_ReleaseWndPtr( wndPtr );
#endif
    }
}
//*****************************************************************************
//*****************************************************************************
BOOL NC_DrawSysButton95 (HWND hwnd, HDC hdc, BOOL down)
{
    BOOL ret;

    Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if (!win32wnd) return FALSE;

    ret = win32wnd->DrawSysButton(hdc, NULL);

    RELEASE_WNDOBJ(win32wnd);

    return ret;
}
//*****************************************************************************
//*****************************************************************************
INT NC_HandleNCHitTest( HWND hwnd, POINT pt)
{
    INT ht;

    Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(win32wnd==NULL) {
        //SvL: This happens in Moraff's YourJongg 2.0, return here
        //TODO: Check if this is supposed to happen at all...
        return HTERROR;
    }

    ht = win32wnd->HandleNCHitTest(pt);
    RELEASE_WNDOBJ(win32wnd);

    return ht;
}
//*****************************************************************************
//*****************************************************************************
