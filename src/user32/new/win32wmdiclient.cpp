/* $Id: win32wmdiclient.cpp,v 1.2 1999-08-31 10:36:23 sandervl Exp $ */
/*
 * Win32 MDI Client Window Class for OS/2
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
#include <win32wnd.h>
#include <win32wmdiclient.h>
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
    maximizedChild     = 0;
    activeChild        = 0;
    nActiveChildren    = 0;
    nTotalCreated      = 0;
    frameTitle         = NULL;
    mdiFlags           = 0;
    idFirstChild       = 0;
    hWindowMenu        = 0;
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
  LPCREATESTRUCTA   cs;
  LPCLIENTCREATESTRUCT ccs;
  RECT              rect;
  INT               nItems;
  LRESULT           retvalue;
  Win32Window       *frameWnd;
  Win32MDIChildWindow *mdichild;

    frameWnd = (Win32Window *)getParent();
    if(frameWnd == NULL) {
       return 0;
    }

    switch (message)
    {
    case WM_CREATE:
        cs = (LPCREATESTRUCTA)lParam;
        ccs = (LPCLIENTCREATESTRUCT)cs->lpCreateParams;

        hWindowMenu     = ccs->hWindowMenu;
        idFirstChild    = ccs->idFirstChild;

        maximizedChild  = 0;
        activeChild     = 0;
        nActiveChildren = 0;
        nTotalCreated   = 0;
        frameTitle      = NULL;
        mdiFlags        = 0;

        setStyle(getStyle() | WS_CLIPCHILDREN);

//        MDI_UpdateFrameText(frameWnd, hwnd, MDI_NOFRAMEREPAINT,frameWnd->text);

        AppendMenuA( hWindowMenu, MF_SEPARATOR, 0, NULL );

        setClientRect(frameWnd->getClientRect());

        dprintf(("MDIClient created - hwnd = %04x, idFirst = %u\n", getWindowHandle(), idFirstChild ));

        retvalue = 0;
        goto END;

    case WM_DESTROY:
//        if( maximizedChild ) MDI_RestoreFrameMenu(w, frameWnd->hwndSelf);

        if((nItems = GetMenuItemCount(hWindowMenu)) > 0)
        {
            idFirstChild = nItems - 1;
            nActiveChildren++;      /* to delete a separator */
            while( nActiveChildren-- )
                DeleteMenu(hWindowMenu,MF_BYPOSITION,idFirstChild--);
        }
        retvalue = 0;
        goto END;

    case WM_MDIACTIVATE:
        if( activeChild && activeChild->getWindowHandle() != (HWND)wParam )

        mdichild = (Win32MDIChildWindow *)GetWindowFromHandle((HWND)wParam);
        if(mdichild) {
            mdichild->SetWindowPos(0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
        }
        retvalue = 0;
        goto END;

#if 0
    case WM_MDICASCADE:
        retvalue = MDICascade(w, ci);
        goto END;
#endif

    case WM_MDICREATE:
        if (lParam) {
                retvalue = Win32MDIChildWindow::createChild( this, (MDICREATESTRUCTA*)lParam );
        }
        else    retvalue = 0;
        goto END;

    case WM_MDIDESTROY:
//        retvalue = MDIDestroyChild( w, ci, hwnd, (HWND)wParam, TRUE );
        goto END;

    case WM_MDIGETACTIVE:
        if (lParam)
            *(BOOL *)lParam = (maximizedChild != 0);

        retvalue = (activeChild) ? activeChild->getWindowHandle() : 0;
        goto END;

#if 0
    case WM_MDIICONARRANGE:
        mdiFlags |= MDIF_NEEDUPDATE;
        ArrangeIconicWindows(hwnd);
        sbRecalc = SB_BOTH+1;
        SendMessageA(hwnd, WM_MDICALCCHILDSCROLL, 0, 0L);
        retvalue = 0;
        goto END;
#endif

    case WM_MDIMAXIMIZE:
        ::ShowWindow( (HWND)wParam, SW_MAXIMIZE );
        retvalue = 0;
        goto END;

    case WM_MDINEXT: /* lParam != 0 means previous window */
//        MDI_SwitchActiveChild(hwnd, (HWND)wParam, (lParam)? FALSE : TRUE );
        break;

    case WM_MDIRESTORE:
        ::SendMessageA( (HWND)wParam, WM_SYSCOMMAND, SC_RESTORE, 0);
        retvalue = 0;
        goto END;
#if 0
    case WM_MDISETMENU:
        retvalue = MDISetMenu( hwnd, (HMENU)wParam, (HMENU)lParam );
        goto END;

    case WM_MDIREFRESHMENU:
        retvalue = MDIRefreshMenu( hwnd, (HMENU)wParam, (HMENU)lParam );
        goto END;

    case WM_MDITILE:
        mdiFlags |= MDIF_NEEDUPDATE;
        ShowScrollBar(hwnd,SB_BOTH,FALSE);
        MDITile(w, ci, wParam);
        mdiFlags &= ~MDIF_NEEDUPDATE;
        retvalue = 0;
        goto END;

    case WM_VSCROLL:
    case WM_HSCROLL:
        mdiFlags |= MDIF_NEEDUPDATE;
        ScrollChildren(hwnd, message, wParam, lParam);
        mdiFlags &= ~MDIF_NEEDUPDATE;
        retvalue = 0;
        goto END;
#endif

    case WM_SETFOCUS:
        if( activeChild )
        {
            if( !(activeChild->getStyle() & WS_MINIMIZE) )
                ::SetFocus(activeChild->getWindowHandle());
        }
        retvalue = 0;
        goto END;

    case WM_NCACTIVATE:
        if( activeChild )
            activeChild->SendMessageA(message, wParam, lParam);
        break;

    case WM_PARENTNOTIFY:
        if (LOWORD(wParam) == WM_LBUTTONDOWN)
        {
            POINTS pt = MAKEPOINTS(lParam);
            POINT point;

            point.x = pt.x;
            point.y = pt.y;

            HWND child = ChildWindowFromPoint(getWindowHandle(), point);

            if( child && child != getWindowHandle() && (!activeChild || activeChild->getWindowHandle() != child) )
                ::SetWindowPos(child, 0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
        }
        retvalue = 0;
        goto END;

    case WM_SIZE:
        if( maximizedChild && maximizedChild->IsWindow() )
        {
            RECT    rect;

            rect.left = 0;
            rect.top = 0;
            rect.right = LOWORD(lParam);
            rect.bottom = HIWORD(lParam);

            AdjustWindowRectEx(&rect, maximizedChild->getStyle(), 0, maximizedChild->getExStyle());
            ::MoveWindow(maximizedChild->getWindowHandle(), rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 1);
        }
#if 0
        else
            MDI_PostUpdate(hwnd, ci, SB_BOTH+1);
#endif
        break;

#if 0
    case WM_MDICALCCHILDSCROLL:
        if( (mdiFlags & MDIF_NEEDUPDATE) && sbRecalc )
        {
            CalcChildScroll(hwnd, sbRecalc-1);
            sbRecalc = 0;
            mdiFlags &= ~MDIF_NEEDUPDATE;
        }
        retvalue = 0;
        goto END;
#endif
    }
    retvalue = DefWindowProcA(message, wParam, lParam );
END:
    return retvalue;
}
/**********************************************************************
 *                  MDIClientWndProc
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
/**********************************************************************
 *                  MDI_ChildActivate
 *
 * Note: hWndChild is NULL when last child is being destroyed
 */
LONG Win32MDIClientWindow::childActivate(Win32MDIChildWindow *child)
{
    BOOL                  isActiveFrameWnd = 0;
    LONG                  retvalue;
    Win32MDIChildWindow  *prevActive = activeChild;

    if( child && child->getStyle() & WS_DISABLED )
    {
        return 0;
    }

    if( GetActiveWindow() == getParent()->getWindowHandle())
        isActiveFrameWnd = TRUE;

    /* deactivate prev. active child */
    if( prevActive )
    {
        prevActive->setStyle(prevActive->getStyle() | WS_SYSMENU);
        prevActive->SendMessageA( WM_NCACTIVATE, FALSE, 0L );
        prevActive->SendMessageA( WM_MDIACTIVATE, (WPARAM)prevActive->getWindowHandle(), (LPARAM)(child) ? child->getWindowHandle() : 0);

        /* uncheck menu item */
        if( getMDIMenu() )
                CheckMenuItem(getMDIMenu(), prevActive->getWindowId(), 0);
    }

    /* set appearance */
    if( maximizedChild)
    {
        if( maximizedChild != child) {
            if( child ) {
                activeChild = child;
                child->ShowWindow(SW_SHOWMAXIMIZED);
            }
            else
            if(activeChild) activeChild->ShowWindow( SW_SHOWNORMAL );
        }
    }

    activeChild = child;

    /* check if we have any children left */
    if( !activeChild )
    {
        if( isActiveFrameWnd )
            SetFocus(getWindowHandle());

        return 0;
    }

    /* check menu item */
    if( getMDIMenu() )
        CheckMenuItem(getMDIMenu(), child->getWindowId(), MF_CHECKED);

    /* bring active child to the top */
    child->SetWindowPos( 0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

    if( isActiveFrameWnd )
    {
        child->SendMessageA( WM_NCACTIVATE, TRUE, 0L);
        if( GetFocus() == getWindowHandle())
            SendMessageA( WM_SETFOCUS, (WPARAM)getWindowHandle(), 0L );
        else
            SetFocus( getWindowHandle() );
    }
    child->SendMessageA( WM_MDIACTIVATE, (WPARAM)prevActive->getWindowHandle(), (LPARAM)child->getWindowHandle());

    return TRUE;
}
/**********************************************************************
 *          MDI_SwitchActiveChild
 *
 * Note: SetWindowPos sends WM_CHILDACTIVATE to the child window that is
 *       being activated
 */
void Win32MDIClientWindow::switchActiveChild(Win32MDIChildWindow *nextActiveChild, BOOL bNextWindow )
{
    Win32MDIChildWindow *prevActiveChild  = 0;

    if ( !nextActiveChild) return; /* no window to switch to */

    prevActiveChild = getActiveChild();

    if ( prevActiveChild !=  nextActiveChild)
    {
        BOOL bOptimize = 0;

        if( getMaximizedChild() )
        {
            bOptimize = 1;
            nextActiveChild->setStyle(nextActiveChild->getStyle()& ~WS_VISIBLE);
        }

        nextActiveChild->SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

        if( bNextWindow && prevActiveChild )
            prevActiveChild->SetWindowPos(HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );

        if( bOptimize )
            ShowWindow(SW_SHOW );
    }
}


/**********************************************************************
 *                                      MDIDestroyChild
 */
LRESULT Win32MDIClientWindow::destroyChild(Win32MDIChildWindow *child, BOOL flagDestroy )
{
    if( child == getActiveChild())
    {
        switchActiveChild(child, TRUE);

        if( child == getActiveChild() )
        {
            ShowWindow(SW_HIDE);
            if( child == getMaximizedChild() )
            {
//                MDI_RestoreFrameMenu(w_parent->parent, child);
                  setMaximizedChild(NULL);
//                MDI_UpdateFrameText(w_parent->parent,parent,TRUE,NULL);
            }
            childActivate(0);
        }
    }
    child->menuDeleteItem();

    decNrActiveChildren();

    dprintf(("child destroyed - %04x\n", child->getWindowHandle()));

    if (flagDestroy)
    {
//        MDI_PostUpdate(GetParent(child), ci, SB_BOTH+1);
        ::DestroyWindow(child->getWindowHandle());
    }

    return 0;
}
/* -------- Miscellaneous service functions ----------
 *
 *			MDI_GetChildByID
 */
Win32MDIChildWindow *Win32MDIClientWindow::getChildByID(INT id)
{
 Win32MDIChildWindow *child;

    for (child = (Win32MDIChildWindow *)getFirstChild() ; child; child = (Win32MDIChildWindow *)child->getNextChild())
        if (child->getWindowId() == id) return child;

    return 0;
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
    else    return FALSE;
}
//******************************************************************************
//******************************************************************************
