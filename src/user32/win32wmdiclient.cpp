/* $Id: win32wmdiclient.cpp,v 1.13 1999-12-09 10:59:05 sandervl Exp $ */
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


//******************************************************************************
//******************************************************************************
Win32MDIClientWindow::Win32MDIClientWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                : maximizedChild(0), activeChild(0), nActiveChildren(0), nTotalCreated(0),
                  frameTitle(NULL), mdiFlags(0), idFirstChild(0), hWindowMenu(0),
                  sbRecalc(0),
                  Win32BaseWindow(OBJTYPE_WINDOW)
{
  Init();
  this->isUnicode = isUnicode;
  CreateWindowExA(lpCreateStructA, classAtom);
}
//******************************************************************************
//******************************************************************************
Win32MDIClientWindow::~Win32MDIClientWindow()
{
    if(frameTitle)
        HeapFree(GetProcessHeap(), 0, frameTitle);
}
//******************************************************************************
//******************************************************************************
BOOL Win32MDIClientWindow::isMDIClient()
{
    return TRUE;
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

        updateFrameText(MDI_NOFRAMEREPAINT, getParent()->getWindowNameA());

        AppendMenuA( hWindowMenu, MF_SEPARATOR, 0, NULL );

        setClientRect(frameWnd->getClientRect());

        dprintf(("MDIClient created - hwnd = %04x, idFirst = %u\n", getWindowHandle(), idFirstChild ));

        retvalue = 0;
        goto END;

    case WM_DESTROY:
        if( maximizedChild ) restoreFrameMenu(getParent());

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
        {
            mdichild = (Win32MDIChildWindow *)GetWindowFromHandle((HWND)wParam);
            if(mdichild) {
                mdichild->SetWindowPos(0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
            }
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
        mdichild = (Win32MDIChildWindow *)GetWindowFromHandle((HWND)wParam);
        if(mdichild) {
            retvalue = destroyChild(mdichild, TRUE );
        }
        goto END;

    case WM_MDIGETACTIVE:
        dprintf(("WM_MDIGETACTIVE: %x %x", this, (activeChild) ? activeChild->getWindowHandle() : 0));
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
        mdichild = (Win32MDIChildWindow *)GetWindowFromHandle((HWND)wParam);
        if(mdichild) {
            switchActiveChild(mdichild, (lParam)? FALSE : TRUE );
        }
        break;

    case WM_MDIRESTORE:
        ::SendMessageA( (HWND)wParam, WM_SYSCOMMAND, SC_RESTORE, 0);
        retvalue = 0;
        goto END;
    case WM_MDISETMENU:
        retvalue = setMDIMenu((HMENU)wParam, (HMENU)lParam );
        goto END;

    case WM_MDIREFRESHMENU:
        retvalue = refreshMDIMenu((HMENU)wParam, (HMENU)lParam );
        goto END;

#if 0
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
        else postUpdate(SB_BOTH+1);
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
 *          MDI_GetWindow
 *
 * returns "activateable" child different from the current or zero
 */
Win32MDIChildWindow *Win32MDIClientWindow::getWindow(Win32MDIChildWindow *actchild, BOOL bNext,
                                                     DWORD dwStyleMask)
{
 Win32MDIChildWindow *lastchild = 0, *curchild;

    dwStyleMask |= WS_DISABLED | WS_VISIBLE;

    if( !actchild ) actchild = getActiveChild();
    if( !actchild)  return 0;

    for ( curchild = (Win32MDIChildWindow *)actchild->getNextChild(); ; curchild = (Win32MDIChildWindow *)curchild->getNextChild())
    {
        if (!curchild ) curchild = (Win32MDIChildWindow *)getFirstChild();

        if ( curchild == actchild ) break; /* went full circle */

        if (!curchild->getOwner() && (curchild->getStyle() & dwStyleMask) == WS_VISIBLE )
        {
            lastchild = curchild;
            if ( bNext ) break;
        }
    }
    return lastchild;
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

    dprintf(("childActivate: %x %x", this, (child) ? child->getWindowHandle() : 0));
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

    /* @@@PH prevActive may be NULL actually ?! */
    child->SendMessageA( WM_MDIACTIVATE,
                         prevActive ? (WPARAM)prevActive->getWindowHandle() : 0,
                         child->getWindowHandle());
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
                restoreFrameMenu(child);
                setMaximizedChild(NULL);
                updateFrameText(TRUE,NULL);
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
/**********************************************************************
 *                      MDI_UpdateFrameText
 *
 * used when child window is maximized/restored
 *
 * Note: lpTitle can be NULL
 */
void Win32MDIClientWindow::updateFrameText(BOOL repaint, LPCSTR lpTitle )
{
    char   lpBuffer[MDI_MAXTITLELENGTH+1];

    /* store new "default" title if lpTitle is not NULL */
    if (lpTitle)
    {
        if (frameTitle) HeapFree( GetProcessHeap(), 0, frameTitle );
        frameTitle = HEAP_strdupA( GetProcessHeap(), 0, lpTitle );
    }

    if (frameTitle)
    {
        Win32MDIChildWindow *childWnd = getMaximizedChild();

        if( childWnd && childWnd->getWindowNameA() )
        {
            /* combine frame title and child title if possible */

            LPCSTR lpBracket  = " - [";
            int i_frame_text_length = strlen(frameTitle);
            int i_child_text_length = strlen(childWnd->getWindowNameA());

            lstrcpynA( lpBuffer, frameTitle, MDI_MAXTITLELENGTH);

            if( i_frame_text_length + 6 < MDI_MAXTITLELENGTH )
            {
                strcat( lpBuffer, lpBracket );

                if( i_frame_text_length + i_child_text_length + 6 < MDI_MAXTITLELENGTH )
                {
                    strcat( lpBuffer, childWnd->getWindowNameA());
                    strcat( lpBuffer, "]" );
                }
                else
                {
                    lstrcpynA(lpBuffer + i_frame_text_length + 4,
                              childWnd->getWindowNameA(), MDI_MAXTITLELENGTH - i_frame_text_length - 5 );
                    strcat( lpBuffer, "]" );
                }
            }
        }
        else
        {
            lstrcpynA(lpBuffer, frameTitle, MDI_MAXTITLELENGTH );
            lpBuffer[MDI_MAXTITLELENGTH]='\0';
        }
    }
    else
    lpBuffer[0] = '\0';

    getParent()->SetWindowTextA(lpBuffer);
    if( repaint == MDI_REPAINTFRAME)
        getParent()->SetWindowPos(0,0,0,0,0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER );
}
/**********************************************************************
 *            MDISetMenu
 */
LRESULT Win32MDIClientWindow::setMDIMenu(HMENU hmenuFrame, HMENU hmenuWindow)
{
    HWND hwndFrame = ::GetParent(getWindowHandle());
    HMENU oldFrameMenu = ::GetMenu(hwndFrame);

    if( maximizedChild && hmenuFrame && hmenuFrame!=oldFrameMenu )
        restoreFrameMenu(maximizedChild);

    if( hmenuWindow && hmenuWindow != hWindowMenu )
    {
        /* delete menu items from ci->hWindowMenu
         * and add them to hmenuWindow */

        INT i = GetMenuItemCount(hWindowMenu) - 1;
        INT pos = GetMenuItemCount(hmenuWindow) + 1;

        AppendMenuA( hmenuWindow, MF_SEPARATOR, 0, NULL);

        if( nActiveChildren )
        {
            INT j = i - nActiveChildren + 1;
            char buffer[100];
            UINT id,state;

            for( ; i >= j ; i-- )
            {
                id = GetMenuItemID(hWindowMenu,i );
                state = GetMenuState(hWindowMenu,i,MF_BYPOSITION);

                GetMenuStringA(hWindowMenu, i, buffer, 100, MF_BYPOSITION);

                DeleteMenu(hWindowMenu, i , MF_BYPOSITION);
                InsertMenuA(hmenuWindow, pos, MF_BYPOSITION | MF_STRING,
                              id, buffer);
                CheckMenuItem(hmenuWindow ,pos , MF_BYPOSITION | (state & MF_CHECKED));
            }
        }

        /* remove separator */
        DeleteMenu(hWindowMenu, i, MF_BYPOSITION);

        hWindowMenu = hmenuWindow;
    }

    if( hmenuFrame && hmenuFrame!=oldFrameMenu)
    {
        ::SetMenu(hwndFrame, hmenuFrame);
#if 0
        if( ci->hwndChildMaximized )
            MDI_AugmentFrameMenu(ci, w->parent, ci->hwndChildMaximized );
#endif
        return oldFrameMenu;
    }
    return 0;
}

/**********************************************************************
 *            MDIRefreshMenu
 */
LRESULT Win32MDIClientWindow::refreshMDIMenu(HMENU hmenuFrame, HMENU hmenuWindow)
{
    HMENU oldFrameMenu = getParent()->GetMenu();

//    FIXME("partially function stub\n");

    return oldFrameMenu;
}
/**********************************************************************
 *					MDI_RestoreFrameMenu
 */
BOOL Win32MDIClientWindow::restoreFrameMenu(Win32BaseWindow *child)
{
    MENUITEMINFOA menuInfo;
    INT nItems = GetMenuItemCount(getParent()->GetMenu()) - 1;
    UINT iId = GetMenuItemID(getParent()->GetMenu(),nItems) ;

    if(!(iId == SC_RESTORE || iId == SC_CLOSE) )
	    return 0;

    /*
     * Remove the system menu, If that menu is the icon of the window
     * as it is in win95, we have to delete the bitmap.
     */
    menuInfo.cbSize = sizeof(MENUITEMINFOA);
    menuInfo.fMask  = MIIM_DATA | MIIM_TYPE;

    GetMenuItemInfoA(getParent()->GetMenu(),
        		     0,
		             TRUE,
		             &menuInfo);

    RemoveMenu(getParent()->GetMenu(),0,MF_BYPOSITION);

//TODO: See augmentframemenu
#if 0
    if ((menuInfo.fType & MFT_BITMAP)           &&
	    (LOWORD(menuInfo.dwTypeData)!=0)        &&
	    (LOWORD(menuInfo.dwTypeData)!=hBmpClose) )
    {
        DeleteObject((HBITMAP)LOWORD(menuInfo.dwTypeData));
    }
#endif

    /* close */
    DeleteMenu(getParent()->GetMenu(),GetMenuItemCount(getParent()->GetMenu()) - 1,MF_BYPOSITION);

    /* restore */
    DeleteMenu(getParent()->GetMenu(),GetMenuItemCount(getParent()->GetMenu()) - 1,MF_BYPOSITION);
    /* minimize */
    DeleteMenu(getParent()->GetMenu(),GetMenuItemCount(getParent()->GetMenu()) - 1,MF_BYPOSITION);

    DrawMenuBar(getParent()->getWindowHandle());

    return 1;
}

#if 0
/**********************************************************************
 *              MDICascade
 */
LONG Win32MDIClientWindow::cascade()
{
    WND**   ppWnd;
    UINT    total;

    if (getMaximizedChild())
        SendMessageA(WM_MDIRESTORE, (WPARAM)getMaximizedChild()->getWindowHandle(), 0);

    if (nActiveChildren == 0) return 0;

    if ((ppWnd = WIN_BuildWinArray(clientWnd, BWA_SKIPHIDDEN | BWA_SKIPOWNED |
                          BWA_SKIPICONIC, &total)))
    {
        WND**   heapPtr = ppWnd;
        if( total )
        {
            INT delta = 0, n = 0;
            POINT   pos[2];
            if( total < ci->nActiveChildren )
                delta = GetSystemMetrics(SM_CYICONSPACING) +
            GetSystemMetrics(SM_CYICON);

            /* walk the list (backwards) and move windows */
            while (*ppWnd) ppWnd++;
                while (ppWnd != heapPtr)
                {
                    ppWnd--;

                    MDI_CalcDefaultChildPos(clientWnd, n++, pos, delta);
                    SetWindowPos( (*ppWnd)->hwndSelf, 0, pos[0].x, pos[0].y,
                                pos[1].x, pos[1].y,
                                SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOZORDER);
                }
        }
        WIN_ReleaseWinArray(heapPtr);
    }

    if( total < ci->nActiveChildren )
        ArrangeIconicWindows( clientWnd->hwndSelf );

    return 0;
}

/**********************************************************************
 *                  MDITile
 */
void Win32MDIClientWindow::MDITile(WPARAM wParam )
{
    WND**   ppWnd;
    UINT    total = 0;

    if (getMaximizedChild())
        SendMessageA(WM_MDIRESTORE, (WPARAM)getMaximizedChild()->getWindowHandle(), 0);

    if (nActiveChildren == 0) return;

    ppWnd = WIN_BuildWinArray(wndClient, BWA_SKIPHIDDEN | BWA_SKIPOWNED | BWA_SKIPICONIC |
            ((wParam & MDITILE_SKIPDISABLED)? BWA_SKIPDISABLED : 0), &total );

    TRACE("%u windows to tile\n", total);

    if( ppWnd )
    {
        WND**   heapPtr = ppWnd;

        if( total )
        {
            RECT    rect;
            int     x, y, xsize, ysize;
            int     rows, columns, r, c, i;

            GetClientRect(wndClient->hwndSelf,&rect);
            rows    = (int) sqrt((double)total);
            columns = total / rows;

            if( wParam & MDITILE_HORIZONTAL )  /* version >= 3.1 */
            {
                i = rows;
                rows = columns;  /* exchange r and c */
                columns = i;
            }

            if( total != ci->nActiveChildren)
            {
                y = rect.bottom - 2 * GetSystemMetrics(SM_CYICONSPACING) - GetSystemMetrics(SM_CYICON);
                rect.bottom = ( y - GetSystemMetrics(SM_CYICON) < rect.top )? rect.bottom: y;
            }

            ysize   = rect.bottom / rows;
            xsize   = rect.right  / columns;

            for (x = i = 0, c = 1; c <= columns && *ppWnd; c++)
            {
                if (c == columns)
                {
                    rows  = total - i;
                    ysize = rect.bottom / rows;
                }

                y = 0;
                for (r = 1; r <= rows && *ppWnd; r++, i++)
                {
                    SetWindowPos((*ppWnd)->hwndSelf, 0, x, y, xsize, ysize,
                        SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOZORDER);
                            y += ysize;
                    ppWnd++;
                }
                x += xsize;
            }
    }
    WIN_ReleaseWinArray(heapPtr);
    }

    if( total < ci->nActiveChildren ) ArrangeIconicWindows( wndClient->hwndSelf );
}

/* ----------------------- Frame window ---------------------------- */

/**********************************************************************
 *                  MDI_AugmentFrameMenu
 */
BOOL Win32MDIClientWindow::augmentFrameMenu(HWND hChild )
{
    WND*    child = WIN_FindWndPtr(hChild);
    HMENU   hSysPopup = 0;
    HBITMAP hSysMenuBitmap = 0;

    if( !frame->wIDmenu || !child->hSysMenu )
    {
        WIN_ReleaseWndPtr(child);
        return 0;
    }
    WIN_ReleaseWndPtr(child);

    /* create a copy of sysmenu popup and insert it into frame menu bar */

    if (!(hSysPopup = LoadMenuA(GetModuleHandleA("USER32"), "SYSMENU")))
    return 0;

    TRACE("\tgot popup %04x in sysmenu %04x\n",
        hSysPopup, child->hSysMenu);

    AppendMenuA(frame->wIDmenu,MF_HELP | MF_BITMAP,
                   SC_MINIMIZE, (LPSTR)(DWORD)HBMMENU_MBAR_MINIMIZE ) ;
    AppendMenuA(frame->wIDmenu,MF_HELP | MF_BITMAP,
                   SC_RESTORE, (LPSTR)(DWORD)HBMMENU_MBAR_RESTORE );

  /* In Win 95 look, the system menu is replaced by the child icon */

  if(TWEAK_WineLook > WIN31_LOOK)
  {
    HICON hIcon = GetClassLongA(hChild, GCL_HICONSM);
    if (!hIcon)
      hIcon = GetClassLongA(hChild, GCL_HICON);
    if (hIcon)
    {
      HDC hMemDC;
      HBITMAP hBitmap, hOldBitmap;
      HBRUSH hBrush;
      HDC hdc = GetDC(hChild);

      if (hdc)
      {
        int cx, cy;
        cx = GetSystemMetrics(SM_CXSMICON);
        cy = GetSystemMetrics(SM_CYSMICON);
        hMemDC = CreateCompatibleDC(hdc);
        hBitmap = CreateCompatibleBitmap(hdc, cx, cy);
        hOldBitmap = SelectObject(hMemDC, hBitmap);
        SetMapMode(hMemDC, MM_TEXT);
        hBrush = CreateSolidBrush(GetSysColor(COLOR_MENU));
        DrawIconEx(hMemDC, 0, 0, hIcon, cx, cy, 0, hBrush, DI_NORMAL);
        SelectObject (hMemDC, hOldBitmap);
        DeleteObject(hBrush);
        DeleteDC(hMemDC);
        ReleaseDC(hChild, hdc);
        hSysMenuBitmap = hBitmap;
      }
    }
  }
  else
    hSysMenuBitmap = hBmpClose;

    if( !InsertMenuA(frame->wIDmenu,0,MF_BYPOSITION | MF_BITMAP | MF_POPUP,
                    hSysPopup, (LPSTR)(DWORD)hSysMenuBitmap))
    {
        TRACE("not inserted\n");
    DestroyMenu(hSysPopup);
    return 0;
    }

    /* The close button is only present in Win 95 look */
    if(TWEAK_WineLook > WIN31_LOOK)
    {
        AppendMenuA(frame->wIDmenu,MF_HELP | MF_BITMAP,
                       SC_CLOSE, (LPSTR)(DWORD)HBMMENU_MBAR_CLOSE );
    }

    EnableMenuItem(hSysPopup, SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSysPopup, SC_MOVE, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSysPopup, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
    SetMenuDefaultItem(hSysPopup, SC_CLOSE, FALSE);

    /* redraw menu */
    DrawMenuBar(frame->hwndSelf);

    return 1;
}

/**********************************************************************
 *                  MDI_RestoreFrameMenu
 */
static BOOL MDI_RestoreFrameMenu( WND *frameWnd, HWND hChild )
{
    MENUITEMINFOA menuInfo;
    INT nItems = GetMenuItemCount(frameWnd->wIDmenu) - 1;
    UINT iId = GetMenuItemID(frameWnd->wIDmenu,nItems) ;

    TRACE("frameWnd %p,child %04x\n",frameWnd,hChild);

    if(!(iId == SC_RESTORE || iId == SC_CLOSE) )
    return 0;

    /*
     * Remove the system menu, If that menu is the icon of the window
     * as it is in win95, we have to delete the bitmap.
     */
    menuInfo.cbSize = sizeof(MENUITEMINFOA);
    menuInfo.fMask  = MIIM_DATA | MIIM_TYPE;

    GetMenuItemInfoA(frameWnd->wIDmenu,
             0,
             TRUE,
             &menuInfo);

    RemoveMenu(frameWnd->wIDmenu,0,MF_BYPOSITION);

    if ( (menuInfo.fType & MFT_BITMAP)           &&
     (LOWORD(menuInfo.dwTypeData)!=0)        &&
     (LOWORD(menuInfo.dwTypeData)!=hBmpClose) )
    {
      DeleteObject((HBITMAP)LOWORD(menuInfo.dwTypeData));
    }

    if(TWEAK_WineLook > WIN31_LOOK)
    {
        /* close */
        DeleteMenu(frameWnd->wIDmenu,GetMenuItemCount(frameWnd->wIDmenu) - 1,MF_BYPOSITION);
    }
    /* restore */
    DeleteMenu(frameWnd->wIDmenu,GetMenuItemCount(frameWnd->wIDmenu) - 1,MF_BYPOSITION);
    /* minimize */
    DeleteMenu(frameWnd->wIDmenu,GetMenuItemCount(frameWnd->wIDmenu) - 1,MF_BYPOSITION);

    DrawMenuBar(frameWnd->hwndSelf);

    return 1;
}
#endif
/* -------- Miscellaneous service functions ----------
 *
 *          MDI_GetChildByID
 */
Win32MDIChildWindow *Win32MDIClientWindow::getChildByID(INT id)
{
 Win32MDIChildWindow *child;

    for (child = (Win32MDIChildWindow *)getFirstChild() ; child; child = (Win32MDIChildWindow *)child->getNextChild())
        if (child->getWindowId() == id) return child;

    return 0;
}

void Win32MDIClientWindow::postUpdate(WORD recalc)
{
    if( !(mdiFlags & MDIF_NEEDUPDATE) )
    {
        mdiFlags |= MDIF_NEEDUPDATE;
        PostMessageA(WM_MDICALCCHILDSCROLL, 0, 0);
    }
    sbRecalc = recalc;
}
//******************************************************************************
//******************************************************************************
BOOL MDICLIENT_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(MDICLIENTCLASSNAMEA)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)MDIClientWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hCursor       = 0;
    wndClass.hbrBackground = (HBRUSH)LTGRAY_BRUSH;
    wndClass.lpszClassName = MDICLIENTCLASSNAMEA;

    return RegisterClassA(&wndClass);
}
//******************************************************************************
//******************************************************************************
BOOL MDICLIENT_Unregister()
{
    if (GlobalFindAtomA(MDICLIENTCLASSNAMEA))
            return UnregisterClassA(MDICLIENTCLASSNAMEA,(HINSTANCE)NULL);
    else    return FALSE;
}
//******************************************************************************
//******************************************************************************
