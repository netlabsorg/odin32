/* $Id: win32wmdiclient.cpp,v 1.1 1999-08-30 14:21:50 sandervl Exp $ */
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
#include <heapstring.h>
#include <win32wmdiclient.h>
#include <win32wnd.h>
#include <spy.h>
#include "wndmsg.h"
#include "hooks.h"
#include <oslibwin.h>
#include <oslibutil.h>
#include <oslibgdi.h>
#include <oslibres.h>
#include "oslibdos.h"
#include <winres.h>
#include "syscolor.h"
#include "win32wndhandle.h"
#include "heapshared.h"


//******************************************************************************
//******************************************************************************
Win32MDIClientWindow::Win32MDIClientWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                : Win32BaseWindow(lpCreateStructA, classAtom, isUnicode)
{
   memset(&ci, 0, sizeof(ci));
}
//******************************************************************************
//******************************************************************************
Win32MDIClientWindow::~Win32MDIClientWindow()
{
}
//******************************************************************************
//******************************************************************************
LRESULT Win32MDIClientWindow::MDIClientWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  LPCREATESTRUCTA  cs;
  RECT		   rect;
  INT 		   nItems;
  LRESULT          retvalue;
  Win32Window     *frameWnd;

    frameWnd = (Win32Window *)getParent();
    if(frameWnd == NULL) {
       return 0;
    }

#if 0
    switch (message)
    {
      case WM_CREATE:
	cs = (LPCREATESTRUCTA)lParam;

        ci->hWindowMenu    	= cs->lpCreateParams->hWindowMenu;
        ci->idFirstChild	= cs->lpCreateParams->idFirstChild;

	ci->hwndChildMaximized  = 0;
	ci->nActiveChildren	= 0;
	ci->nTotalCreated	= 0;
	ci->frameTitle		= NULL;
	ci->mdiFlags		= 0;
	ci->self		= hwnd;
	w->dwStyle             |= WS_CLIPCHILDREN;

	MDI_UpdateFrameText(frameWnd, hwnd, MDI_NOFRAMEREPAINT,frameWnd->text);

	AppendMenuA( ci->hWindowMenu, MF_SEPARATOR, 0, NULL );

	GetClientRect(frameWnd->hwndSelf, &rect);
	w->rectClient = rect;

	dprintf(("MDIClient created - hwnd = %04x, idFirst = %u\n",
			   hwnd, ci->idFirstChild ));

        retvalue = 0;
        goto END;
      
      case WM_DESTROY:
	if( ci->hwndChildMaximized ) MDI_RestoreFrameMenu(w, frameWnd->hwndSelf);
	if((nItems = GetMenuItemCount(ci->hWindowMenu)) > 0) 
	{
    	    ci->idFirstChild = nItems - 1;
	    ci->nActiveChildren++; 		/* to delete a separator */
	    while( ci->nActiveChildren-- )
	        DeleteMenu(ci->hWindowMenu,MF_BYPOSITION,ci->idFirstChild--);
	}
        retvalue = 0;
        goto END;

      case WM_MDIACTIVATE:
        if( ci->hwndActiveChild != (HWND)wParam )
	    SetWindowPos((HWND)wParam, 0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
        retvalue = 0;
        goto END;

      case WM_MDICASCADE:
        retvalue = MDICascade(w, ci);
        goto END;

      case WM_MDICREATE:
        if (lParam) retvalue = MDICreateChild( w, ci, hwnd,
                                           (MDICREATESTRUCTA*)lParam );
        else retvalue = 0;
        goto END;

      case WM_MDIDESTROY:
	retvalue = MDIDestroyChild( w, ci, hwnd, (HWND)wParam, TRUE );
        goto END;

      case WM_MDIGETACTIVE:
          if (lParam) *(BOOL *)lParam = (ci->hwndChildMaximized > 0);
          retvalue = ci->hwndActiveChild;
          goto END;

      case WM_MDIICONARRANGE:
	ci->mdiFlags |= MDIF_NEEDUPDATE;
        ArrangeIconicWindows(hwnd);
	ci->sbRecalc = SB_BOTH+1;
	SendMessageA(hwnd, WM_MDICALCCHILDSCROLL, 0, 0L);
        retvalue = 0;
        goto END;
	
      case WM_MDIMAXIMIZE:
	ShowWindow( (HWND)wParam, SW_MAXIMIZE );
        retvalue = 0;
        goto END;

      case WM_MDINEXT: /* lParam != 0 means previous window */
	MDI_SwitchActiveChild(hwnd, (HWND)wParam, (lParam)? FALSE : TRUE );
	break;
	
      case WM_MDIRESTORE:
        SendMessageA( (HWND)wParam, WM_SYSCOMMAND, SC_RESTORE, 0);
        retvalue = 0;
        goto END;

      case WM_MDISETMENU:
          retvalue = MDISetMenu( hwnd, (HMENU)wParam, (HMENU)lParam );
	  goto END;
      case WM_MDIREFRESHMENU:
          retvalue = MDIRefreshMenu( hwnd, (HMENU)wParam, (HMENU)lParam );
          goto END;

      case WM_MDITILE:
	ci->mdiFlags |= MDIF_NEEDUPDATE;
	ShowScrollBar(hwnd,SB_BOTH,FALSE);
	MDITile(w, ci, wParam);
        ci->mdiFlags &= ~MDIF_NEEDUPDATE;
        retvalue = 0;
        goto END;

      case WM_VSCROLL:
      case WM_HSCROLL:
	ci->mdiFlags |= MDIF_NEEDUPDATE;
        ScrollChildren(hwnd, message, wParam, lParam);
	ci->mdiFlags &= ~MDIF_NEEDUPDATE;
        retvalue = 0;
        goto END;

      case WM_SETFOCUS:
	if( ci->hwndActiveChild )
	{
	   WND*	pw = WIN_FindWndPtr( ci->hwndActiveChild );
	   if( !(pw->dwStyle & WS_MINIMIZE) )
	       SetFocus( ci->hwndActiveChild );
	   WIN_ReleaseWndPtr(pw);
	} 
        retvalue = 0;
        goto END;
	
      case WM_NCACTIVATE:
        if( ci->hwndActiveChild )
	     SendMessageA(ci->hwndActiveChild, message, wParam, lParam);
	break;
	
      case WM_PARENTNOTIFY:
        if (LOWORD(wParam) == WM_LBUTTONDOWN)
        {
            POINT  pt = MAKEPOINT(lParam);
            HWND child = ChildWindowFromPoint(hwnd, pt);

            if( child && child != hwnd && child != ci->hwndActiveChild )
                SetWindowPos(child, 0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
        }
        retvalue = 0;
        goto END;

      case WM_SIZE:
        if( IsWindow(ci->hwndChildMaximized) )
	{
	    WND*	child = WIN_FindWndPtr(ci->hwndChildMaximized);
	    RECT	rect;

	    rect.left = 0;
	    rect.top = 0;
	    rect.right = LOWORD(lParam);
	    rect.bottom = HIWORD(lParam);

	    AdjustWindowRectEx(&rect, child->dwStyle, 0, child->dwExStyle);
	    MoveWindow(ci->hwndChildMaximized, rect.left, rect.top,
			 rect.right - rect.left, rect.bottom - rect.top, 1);
            WIN_ReleaseWndPtr(child);
	}
	else
	    MDI_PostUpdate(hwnd, ci, SB_BOTH+1);

	break;

      case WM_MDICALCCHILDSCROLL:
	if( (ci->mdiFlags & MDIF_NEEDUPDATE) && ci->sbRecalc )
	{
	    CalcChildScroll(hwnd, ci->sbRecalc-1);
	    ci->sbRecalc = 0;
	    ci->mdiFlags &= ~MDIF_NEEDUPDATE;
	}
        retvalue = 0;
        goto END;
    }
#endif    
    retvalue = DefWindowProcA(message, wParam, lParam );
END:
    return retvalue;
}
/**********************************************************************
 *					MDIClientWndProc
 *
 * This function handles all MDI requests.
 */
LRESULT WINAPI MDIClientWndProc( HWND hwnd, UINT message, WPARAM wParam,
                                 LPARAM lParam )
{
  Win32MDIClientWindow *window;

    window = (Win32MDIClientWindow *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("MDIClientWndProc, window %x not found", hwnd));
        return 0;
    }
    return window->MDIClientWndProc(message, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
BOOL MDICLIENT_Register()
{
    WNDCLASSA wndClass;

    if (GlobalFindAtomA(MDICLIENTCLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW | CS_PARENTDC | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)MDIClientWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hCursor       = 0;
    wndClass.hbrBackground = (HBRUSH)0;
    wndClass.lpszClassName = MDICLIENTCLASSNAME;

    return RegisterClassA(&wndClass);
}
//******************************************************************************
//******************************************************************************
BOOL MDICLIENT_Unregister()
{
    if (GlobalFindAtomA(MDICLIENTCLASSNAME))
        return UnregisterClassA(MDICLIENTCLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}
//******************************************************************************
//******************************************************************************
