/* $Id: win32wmisc.cpp,v 1.1 2001-09-19 15:39:51 sandervl Exp $ */
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
#include "win32wndhandle.h"
#include "win32wmisc.h"

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

	if ((list = (HWND *)HeapAlloc( GetProcessHeap(), 0, sizeof(HWND) * (count + 1))))
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
   Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if (!win32wnd) return;

   win32wnd->GetSysPopupPos(rect);
   RELEASE_WNDOBJ(win32wnd);
}
//******************************************************************************
//******************************************************************************
