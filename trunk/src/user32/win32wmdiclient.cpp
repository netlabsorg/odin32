/* $Id: win32wmdiclient.cpp,v 1.36 2001-06-11 15:05:44 sandervl Exp $ */
/*
 * Win32 MDI Client Window Class for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 * Parts based on Corel WINE (window\mdi.c: 20000317)
 * (Parts based on Wine (windows\mdi.c) (990815))
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
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <heapstring.h>
#include <win32wnd.h>
#include <win32wmdiclient.h>
#include <spy.h>
#include "wndmsg.h"
#include <oslibwin.h>
#include <oslibutil.h>
#include <oslibgdi.h>
#include <oslibres.h>
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"

#define DBG_LOCALLOG    DBG_win32wmdiclient
#include "dbglocal.h"


//******************************************************************************
//******************************************************************************
Win32MDIClientWindow::Win32MDIClientWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                : maximizedChild(0), activeChild(0), nActiveChildren(0), nTotalCreated(0),
                  frameTitle(NULL), mdiFlags(0), idFirstChild(0), hWindowMenu(0),
                  sbRecalc(0),
                  Win32BaseWindow()
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

        setClientRect(frameWnd->getClientRectPtr());

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
        if(activeChild != (HWND)wParam )
        {
            mdichild = (Win32MDIChildWindow *)GetWindowFromHandle((HWND)wParam);
            if(mdichild) {
                mdichild->SetWindowPos(0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
                RELEASE_WNDOBJ(mdichild);
            }
        }
        retvalue = 0;
        goto END;

    case WM_MDICASCADE:
        retvalue = cascade(wParam);
        goto END;

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
            RELEASE_WNDOBJ(mdichild);
        }
        goto END;

    case WM_MDIGETACTIVE:
        dprintf(("WM_MDIGETACTIVE: %x %x", this, activeChild));
        if (lParam)
            *(BOOL *)lParam = (maximizedChild != 0);

        retvalue = activeChild;
        goto END;

    case WM_MDIICONARRANGE:
        mdiFlags |= MDIF_NEEDUPDATE;
        ArrangeIconicWindows(Win32Hwnd);
        sbRecalc = SB_BOTH+1;
        SendMessageA(WM_MDICALCCHILDSCROLL,0,0L);
        retvalue = 0;
        goto END;

    case WM_MDIMAXIMIZE:
        ::ShowWindow( (HWND)wParam, SW_MAXIMIZE );
        retvalue = 0;
        goto END;

    case WM_MDINEXT: /* lParam != 0 means previous window */
        mdichild = (Win32MDIChildWindow *)GetWindowFromHandle((HWND)wParam);
        if(mdichild) {
            switchActiveChild(mdichild, (lParam)? FALSE : TRUE );
            RELEASE_WNDOBJ(mdichild);
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

    case WM_MDITILE:
        mdiFlags |= MDIF_NEEDUPDATE;
        ShowScrollBar(Win32Hwnd,SB_BOTH,FALSE);
        tile(wParam);
        mdiFlags &= ~MDIF_NEEDUPDATE;
        retvalue = 0;
        goto END;

    case WM_VSCROLL:
    case WM_HSCROLL:
        mdiFlags |= MDIF_NEEDUPDATE;
        ScrollChildren(Win32Hwnd, message, wParam, lParam);
        mdiFlags &= ~MDIF_NEEDUPDATE;
        retvalue = 0;
        goto END;

    case WM_SETFOCUS:
        if( activeChild )
        {
            if( !(GetWindowLongA(activeChild, GWL_STYLE) & WS_MINIMIZE) )
                ::SetFocus(activeChild);
        }
        retvalue = 0;
        goto END;

    case WM_NCACTIVATE:
        if( activeChild )
            ::SendMessageA(activeChild, message, wParam, lParam);
        break;

    case WM_PARENTNOTIFY:
        if (LOWORD(wParam) == WM_LBUTTONDOWN)
        {
            POINTS pt = MAKEPOINTS(lParam);
            POINT point;

            point.x = pt.x;
            point.y = pt.y;

            HWND child = ChildWindowFromPoint(getWindowHandle(), point);

            if( child && child != getWindowHandle() && (activeChild != child) )
                ::SetWindowPos(child, 0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
        }
        retvalue = 0;
        goto END;

    case WM_SIZE:
        if( ::IsWindow(maximizedChild) )
        {
            RECT    rect;

            rect.left = 0;
            rect.top = 0;
            rect.right = LOWORD(lParam);
            rect.bottom = HIWORD(lParam);

            AdjustWindowRectEx(&rect, GetWindowLongA(maximizedChild, GWL_STYLE), 0, GetWindowLongA(maximizedChild, GWL_EXSTYLE));
            ::MoveWindow(maximizedChild, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 1);
        }
        else postUpdate(SB_BOTH+1);
        break;

    case WM_MDICALCCHILDSCROLL:
        if( (mdiFlags & MDIF_NEEDUPDATE) && sbRecalc )
        {
            CalcChildScroll(Win32Hwnd, sbRecalc-1);
            sbRecalc = 0;
            mdiFlags &= ~MDIF_NEEDUPDATE;
        }
        retvalue = 0;
        goto END;
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
    LRESULT ret = window->MDIClientWndProc(message, wParam, lParam);
    RELEASE_WNDOBJ(window);
    return ret;
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

    if( !actchild ) {
         actchild = (Win32MDIChildWindow *)GetWindowFromHandle(getActiveChild());
    }
    else actchild->addRef();

    if( !actchild)  return 0;

    lock();
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
    unlock();
    RELEASE_WNDOBJ(actchild);
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
    Win32MDIChildWindow  *prevActive = (Win32MDIChildWindow *)GetWindowFromHandle(activeChild);

    if( child && child->getStyle() & WS_DISABLED )
    {
        if(prevActive) RELEASE_WNDOBJ(prevActive);
        return 0;
    }

    /* Don't activate if it is already active. Might happen 
       since ShowWindow DOES activate MDI children */
    if((child && activeChild == child->getWindowHandle()) ||
       (child == 0 && activeChild == 0))
    {
        if(prevActive) RELEASE_WNDOBJ(prevActive);
      	return 0;
    }

    if( GetActiveWindow() == getParent()->getWindowHandle())
        isActiveFrameWnd = TRUE;

    /* deactivate prev. active child */
    if( prevActive )
    {
        prevActive->setStyle(prevActive->getStyle() | WS_SYSMENU);
        prevActive->DeactivateChildWindow();
        prevActive->SendInternalMessageA( WM_NCACTIVATE, FALSE, 0L );
        prevActive->SendInternalMessageA( WM_MDIACTIVATE, (WPARAM)prevActive->getWindowHandle(), (LPARAM)(child) ? child->getWindowHandle() : 0);

        /* uncheck menu item */
        if( getMDIMenu() )
                CheckMenuItem(getMDIMenu(), prevActive->getWindowId(), 0);
    }

    /* set appearance */
    if( maximizedChild)
    {
        if( maximizedChild != child->getWindowHandle()) {
            if( child ) {
                activeChild = child->getWindowHandle();
                child->ShowWindow(SW_SHOWMAXIMIZED);
            }
            else
            if(activeChild) ::ShowWindow(activeChild, SW_SHOWNORMAL );
        }
    }

    dprintf(("childActivate: %x %x", this, (child) ? child->getWindowHandle() : 0));
    activeChild = (child) ? child->getWindowHandle() : 0;

    /* check if we have any children left */
    if( !activeChild )
    {
        if( isActiveFrameWnd )
            SetFocus(getWindowHandle());

        if(prevActive) RELEASE_WNDOBJ(prevActive);
        return 0;
    }

    /* check menu item */
    if( getMDIMenu() )
        CheckMenuItem(getMDIMenu(), child->getWindowId(), MF_CHECKED);

    /* bring active child to the top */
    child->SetWindowPos( 0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

    if( isActiveFrameWnd )
    {
        child->SendInternalMessageA( WM_NCACTIVATE, TRUE, 0L);
        if( GetFocus() == getWindowHandle())
            SendInternalMessageA( WM_SETFOCUS, (WPARAM)getWindowHandle(), 0L );
        else
            SetFocus( getWindowHandle() );
    }

    /* @@@PH prevActive may be NULL actually ?! */
    child->SendInternalMessageA( WM_MDIACTIVATE,
                         prevActive ? (WPARAM)prevActive->getWindowHandle() : 0,
                         child->getWindowHandle());

    if(prevActive) RELEASE_WNDOBJ(prevActive);
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
    HWND prevActiveChild  = 0;

    if ( !nextActiveChild) return; /* no window to switch to */

    prevActiveChild = getActiveChild();

    if ( prevActiveChild !=  nextActiveChild->getWindowHandle())
    {
        BOOL bOptimize = 0;

        if( getMaximizedChild() )
        {
            bOptimize = 1;
            nextActiveChild->setStyle(nextActiveChild->getStyle()& ~WS_VISIBLE);
        }

        nextActiveChild->SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

        if( bNextWindow && prevActiveChild )
            ::SetWindowPos(prevActiveChild, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );

        if( bOptimize )
            ShowWindow(SW_SHOW );
    }
}


/**********************************************************************
 *                                      MDIDestroyChild
 */
LRESULT Win32MDIClientWindow::destroyChild(Win32MDIChildWindow *child, BOOL flagDestroy )
{
    if( child->getWindowHandle() == getActiveChild())
    {
        switchActiveChild(child, TRUE);

        if( child->getWindowHandle() == getActiveChild() )
        {
            ::ShowWindow(child->getWindowHandle(),SW_HIDE);
            if( child->getWindowHandle() == getMaximizedChild() )
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
        postUpdate(SB_BOTH+1);
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
        Win32MDIChildWindow *childWnd = (Win32MDIChildWindow *)GetWindowFromHandle(getMaximizedChild());

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
        if(childWnd) RELEASE_WNDOBJ(childWnd);
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

    if (hmenuFrame && !IsMenu(hmenuFrame))
    {
	dprintf(("Win32MDIClientWindow::setMDIMenu: hmenuFrame is not a menu handle\n"));
	return 0L;
    }
	
    if (hmenuWindow && !IsMenu(hmenuWindow))
    {
	dprintf(("Win32MDIClientWindow::setMDIMenu: hmenuWindow is not a menu handle\n"));
	return 0L;
    }

    if( maximizedChild && hmenuFrame && hmenuFrame!=oldFrameMenu )
        restoreFrameMenu(maximizedChild);

    if( hmenuWindow && hmenuWindow != hWindowMenu )
    {
        /* delete menu items from ci->hWindowMenu
         * and add them to hmenuWindow */

        INT i = GetMenuItemCount(hWindowMenu) - 1;
        INT pos = GetMenuItemCount(hmenuWindow) + 1;

        AppendMenuA( hmenuWindow, MF_SEPARATOR, 0, NULL);

#if 1
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
#else
//doesn't work:
        if( nActiveChildren )
        {
            INT j;
            LPWSTR buffer = NULL;
	    MENUITEMINFOW mii;
            INT nbWindowsMenuItems; /* num of documents shown + "More Windows..." if present */

            if (nActiveChildren <= MDI_MOREWINDOWSLIMIT)
                nbWindowsMenuItems = nActiveChildren;
            else
                nbWindowsMenuItems = MDI_MOREWINDOWSLIMIT + 1;

            j = i - nbWindowsMenuItems + 1;

            for( ; i >= j ; i-- )
            {
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE
		  | MIIM_SUBMENU | MIIM_TYPE | MIIM_BITMAP;

		GetMenuItemInfoW(hWindowMenu, i, TRUE, &mii);
		if(mii.cch) { /* Menu is MFT_STRING */
		    mii.cch++; /* add room for '\0' */
		    buffer = (LPWSTR)HeapAlloc(GetProcessHeap(), 0,
				               mii.cch * sizeof(WCHAR));
		    mii.dwTypeData = buffer;
		    GetMenuItemInfoW(hWindowMenu, i, TRUE, &mii);
		}
                DeleteMenu(hWindowMenu, i, MF_BYPOSITION);
                InsertMenuItemW(hmenuWindow, pos, TRUE, &mii);
		if(buffer) {
		    HeapFree(GetProcessHeap(), 0, buffer);
		    buffer = NULL;
		}
            }
        }
#endif
        /* remove separator */
        DeleteMenu(hWindowMenu, i, MF_BYPOSITION);

        hWindowMenu = hmenuWindow;
    }

    if (hmenuFrame)
    {
        ::SetMenu(hwndFrame, hmenuFrame);

        if( hmenuFrame!=oldFrameMenu )
        {
          if (maximizedChild)
            augmentFrameMenu(maximizedChild);

          return oldFrameMenu;
        }
    }
    else
    {
	INT nItems = GetMenuItemCount(oldFrameMenu) - 1;
	UINT iId = GetMenuItemID(oldFrameMenu,nItems) ;

	if( !(iId == SC_RESTORE || iId == SC_CLOSE) )
	{
	    /* SetMenu() may already have been called, meaning that this window
	     * already has its menu. But they may have done a SetMenu() on
	     * an MDI window, and called MDISetMenu() after the fact, meaning
	     * that the "if" to this "else" wouldn't catch the need to
	     * augment the frame menu.
	     */
	    if( maximizedChild )
		augmentFrameMenu(maximizedChild);
	}
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
 *                  MDI_RestoreFrameMenu
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

Win32BaseWindow** Win32MDIClientWindow::buildWindowArray(UINT bwaFlags,PUINT total)
{
  Win32BaseWindow **list = NULL,*win32wnd,**pos;
  UINT skipHidden;
  DWORD skipFlags;

  skipHidden = bwaFlags & BWA_SKIPHIDDEN;
  skipFlags = (bwaFlags & BWA_SKIPDISABLED) ? WS_DISABLED : 0;
  if (bwaFlags & BWA_SKIPICONIC) skipFlags |= WS_MINIMIZE;

  /* First count the windows */
  *total = 0;
  win32wnd = (Win32BaseWindow*)this->getFirstChild();
  while (win32wnd)
  {
    if (!(win32wnd->getStyle() & skipFlags) && (!skipHidden || (win32wnd->getStyle() & WS_VISIBLE)))
          (*total)++;
    win32wnd = (Win32BaseWindow*)win32wnd->getNextChild();
  }

  if (*total)
  {
    /* Now build the list of all windows */
    list = (Win32BaseWindow**)HeapAlloc(GetProcessHeap(),0,sizeof(Win32BaseWindow*)*(*total+1));
    if (list)
    {
      for (win32wnd = (Win32BaseWindow*)this->getFirstChild(),pos = list;win32wnd;win32wnd = (Win32BaseWindow*)win32wnd->getNextChild())
      {
        if ((win32wnd->getStyle() & skipFlags));
        else if(!skipHidden || win32wnd->getStyle() & WS_VISIBLE)
          *pos++ = win32wnd;
      }
      *pos = NULL;
    }
  }

  return list;
}

void Win32MDIClientWindow::releaseWindowArray(Win32BaseWindow **wndArray)
{
  HeapFree(GetProcessHeap(),0,wndArray);
}

/**********************************************************************
 *                      MDI_CalcDefaultChildPos
 *
 *  It seems that the default height is about 2/3 of the client rect
 */
void Win32MDIClientWindow::calcDefaultChildPos(WORD n,LPPOINT lpPos,INT delta)
{
    INT  nstagger;
    RECT rect;
    INT  spacing = GetSystemMetrics(SM_CYCAPTION) +
                     GetSystemMetrics(SM_CYFRAME) - 1;

    getClientRect(&rect);
    if( rect.bottom - rect.top - delta >= spacing )
        rect.bottom -= delta;

    nstagger = (rect.bottom - rect.top)/(3 * spacing);
    lpPos[1].x = (rect.right - rect.left - nstagger * spacing);
    lpPos[1].y = (rect.bottom - rect.top - nstagger * spacing);
    lpPos[0].x = lpPos[0].y = spacing * (n%(nstagger+1));
}

/**********************************************************************
 *              MDICascade
 */
BOOL Win32MDIClientWindow::cascade(UINT fuCascade)
{
  Win32BaseWindow **list;
  UINT total = 0;

  if (getMaximizedChild())
    SendInternalMessageA(WM_MDIRESTORE, (WPARAM)getMaximizedChild(), 0);

  if (nActiveChildren == 0) return 0;

  list = buildWindowArray(BWA_SKIPHIDDEN | BWA_SKIPOWNED | BWA_SKIPICONIC,&total);
  if (list)
  {
    Win32BaseWindow** heapPtr = list;
    if (total)
    {
      INT delta = 0,n = 0;
      POINT pos[2];


      if (total < nActiveChildren)
        delta = GetSystemMetrics(SM_CYICONSPACING)+GetSystemMetrics(SM_CYICON);

      // walk the list (backwards) and move windows
      while (*list) list++;
      while (list != heapPtr)
      {
        list--;

        calcDefaultChildPos(n++,pos,delta);
        ::SetWindowPos((*list)->getWindowHandle(),0,pos[0].x,pos[0].y,pos[1].x,pos[1].y,SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOZORDER);
      }
    }
    releaseWindowArray(heapPtr);
  }

  if (total < nActiveChildren)
    ArrangeIconicWindows(Win32Hwnd);

  return TRUE;
}

/**********************************************************************
 *                  MDITile
 */
BOOL Win32MDIClientWindow::tile(UINT fuTile)
{
  Win32BaseWindow** list;
  UINT total = 0;

  if (getMaximizedChild())
    SendInternalMessageA(WM_MDIRESTORE, (WPARAM)getMaximizedChild(), 0);

  if (nActiveChildren == 0) return TRUE;

  list = buildWindowArray(BWA_SKIPHIDDEN | BWA_SKIPOWNED | BWA_SKIPICONIC |
          ((fuTile & MDITILE_SKIPDISABLED)? BWA_SKIPDISABLED : 0), &total );

  if (list)
  {
    Win32BaseWindow** heapPtr = list;

    if (total)
    {
      RECT    rect;
      int     x, y, xsize, ysize;
      int     rows, columns, r, c, i;

      GetClientRect(Win32Hwnd,&rect);
      rows    = (int) sqrt((double)total);
      columns = total / rows;

      if( fuTile & MDITILE_HORIZONTAL )  // version >= 3.1
      {
        i = rows;
        rows = columns;  // exchange r and c
        columns = i;
      }

      if( total != nActiveChildren)
      {
        y = rect.bottom - 2 * GetSystemMetrics(SM_CYICONSPACING) - GetSystemMetrics(SM_CYICON);
        rect.bottom = ( y - GetSystemMetrics(SM_CYICON) < rect.top )? rect.bottom: y;
      }

      ysize   = rect.bottom / rows;
      xsize   = rect.right  / columns;

      for (x = i = 0, c = 1; c <= columns && *list; c++)
      {
        if (c == columns)
        {
          rows  = total - i;
          ysize = rect.bottom / rows;
        }

        y = 0;
        for (r = 1; r <= rows && *list; r++, i++)
        {
          ::SetWindowPos((*list)->getWindowHandle(), 0, x, y, xsize, ysize,
                         SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOZORDER);
          y += ysize;
          list++;
        }
        x += xsize;
      }
    }
    releaseWindowArray(heapPtr);
  }

  if( total < nActiveChildren ) ArrangeIconicWindows(Win32Hwnd);

  return TRUE;
}

/* ----------------------- Frame window ---------------------------- */

/**********************************************************************
 *                  MDI_AugmentFrameMenu
 */
BOOL Win32MDIClientWindow::augmentFrameMenu(HWND hwndChild)
{
  Win32MDIChildWindow *child = (Win32MDIChildWindow *)GetWindowFromHandle(hwndChild);
  HMENU   hSysPopup = 0,hFrameMenu = ::GetMenu(getParent()->getWindowHandle()),hSysMenu = ::GetSystemMenu(child->getWindowHandle(),FALSE);
  HBITMAP hSysMenuBitmap = 0;

  if (!hFrameMenu || !hSysMenu) {
    RELEASE_WNDOBJ(child);
    return 0;
  }
  // create a copy of sysmenu popup and insert it into frame menu bar

  if (!(hSysPopup = LoadMenuA(GetModuleHandleA("USER32"), "SYSMENU"))) {
    RELEASE_WNDOBJ(child);
    return 0;
  }

  //TRACE("\tgot popup %04x in sysmenu %04x\n",
  //    hSysPopup, child->hSysMenu);

  AppendMenuA(hFrameMenu,MF_HELP | MF_BITMAP,
                 SC_MINIMIZE, (LPSTR)(DWORD)HBMMENU_MBAR_MINIMIZE ) ;
  AppendMenuA(hFrameMenu,MF_HELP | MF_BITMAP,
                 SC_RESTORE, (LPSTR)(DWORD)HBMMENU_MBAR_RESTORE );

  // In Win 95 look, the system menu is replaced by the child icon
 
  /* Find icon */
  HICON hIcon = child->IconForWindow(ICON_SMALL);

  if (hIcon)
  {
    HDC hMemDC;
    HBITMAP hBitmap, hOldBitmap;
    HBRUSH hBrush;
    HDC hdc = GetDC(child->getWindowHandle());

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
      ReleaseDC(child->getWindowHandle(), hdc);
      hSysMenuBitmap = hBitmap;
    }
  }
  RELEASE_WNDOBJ(child);

  if( !InsertMenuA(hFrameMenu,0,MF_BYPOSITION | MF_BITMAP | MF_POPUP,
                    hSysPopup, (LPSTR)(DWORD)hSysMenuBitmap))
  {
        //TRACE("not inserted\n");
    DestroyMenu(hSysPopup);
    return 0;
  }

  // The close button is only present in Win 95 look

  AppendMenuA(hFrameMenu,MF_HELP | MF_BITMAP,
                 SC_CLOSE, (LPSTR)(DWORD)HBMMENU_MBAR_CLOSE );

  EnableMenuItem(hSysPopup, SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
  EnableMenuItem(hSysPopup, SC_MOVE, MF_BYCOMMAND | MF_GRAYED);
  EnableMenuItem(hSysPopup, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
  SetMenuDefaultItem(hSysPopup, SC_CLOSE, FALSE);

  // redraw menu
  DrawMenuBar(getParent()->getWindowHandle());

  return 1;
}

/**********************************************************************
 *                  MDI_RestoreFrameMenu
 */
BOOL Win32MDIClientWindow::restoreFrameMenu(HWND hwndChild)
{
    MENUITEMINFOA menuInfo;
    HMENU hFrameMenu = ::GetMenu(getParent()->getWindowHandle());
    INT nItems = GetMenuItemCount(hFrameMenu) - 1;
    UINT iId = GetMenuItemID(hFrameMenu,nItems) ;

    //TRACE("frameWnd %p,child %04x\n",frameWnd,hChild);

    if(!(iId == SC_RESTORE || iId == SC_CLOSE) )
        return 0;

    /*
     * Remove the system menu, If that menu is the icon of the window
     * as it is in win95, we have to delete the bitmap.
     */

    menuInfo.cbSize = sizeof(MENUITEMINFOA);
    menuInfo.fMask  = MIIM_DATA | MIIM_TYPE;

    GetMenuItemInfoA(hFrameMenu,
             0,
             TRUE,
             &menuInfo);

    RemoveMenu(hFrameMenu,0,MF_BYPOSITION);

#if 0  //CB: hBmpClose not (yet) defined
    if ( (menuInfo.fType & MFT_BITMAP)           &&
     (LOWORD(menuInfo.dwTypeData)!=0)        &&
     (LOWORD(menuInfo.dwTypeData)!=hBmpClose) )
    {
      DeleteObject((HBITMAP)LOWORD(menuInfo.dwTypeData));
    }
#endif

    // close
    DeleteMenu(hFrameMenu,GetMenuItemCount(hFrameMenu) - 1,MF_BYPOSITION);

    // restore
    DeleteMenu(hFrameMenu,GetMenuItemCount(hFrameMenu) - 1,MF_BYPOSITION);
    // minimize
    DeleteMenu(hFrameMenu,GetMenuItemCount(hFrameMenu) - 1,MF_BYPOSITION);

    DrawMenuBar(getParent()->getWindowHandle());

    return 1;
}

/***********************************************************************
 *           CalcChildScroll   (USER.462)
 */
void WINAPI CalcChildScroll(HWND hwnd,WORD scroll)
{
    Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    Win32BaseWindow *child;
    SCROLLINFO info;
    RECT childRect, clientRect;
    INT  vmin, vmax, hmin, hmax, vpos, hpos;

    if (!win32wnd) return;

    GetClientRect( hwnd, &clientRect );
    SetRectEmpty( &childRect );

    //TODO: Check if this goes correctly
    win32wnd->lock();
    for (child = (Win32BaseWindow*)win32wnd->getFirstChild();child;child = (Win32BaseWindow*)child->getNextChild())
    {
          if( child->getStyle() & WS_MAXIMIZE )
          {
              win32wnd->unlock();
              RELEASE_WNDOBJ(win32wnd);
              ShowScrollBar(hwnd, SB_BOTH, FALSE);
              return;
          }
          UnionRect(&childRect,child->getWindowRect(),&childRect);
    }
    win32wnd->unlock();
    RELEASE_WNDOBJ(win32wnd);

    UnionRect( &childRect, &clientRect, &childRect );

    hmin = childRect.left; hmax = childRect.right - clientRect.right;
    hpos = clientRect.left - childRect.left;
    vmin = childRect.top; vmax = childRect.bottom - clientRect.bottom;
    vpos = clientRect.top - childRect.top;

    switch( scroll )
    {
        case SB_HORZ:
                        vpos = hpos; vmin = hmin; vmax = hmax;
        case SB_VERT:
                        info.cbSize = sizeof(info);
                        info.nMax = vmax; info.nMin = vmin; info.nPos = vpos;
                        info.fMask = SIF_POS | SIF_RANGE;
                        SetScrollInfo(hwnd, scroll, &info, TRUE);
                        break;
        case SB_BOTH:
        {
          SCROLLINFO vInfo, hInfo;

          vInfo.cbSize = hInfo.cbSize = sizeof(SCROLLINFO);
          vInfo.nMin   = vmin;
          hInfo.nMin   = hmin;
          vInfo.nMax   = vmax;
          hInfo.nMax   = hmax;
          vInfo.nPos   = vpos;
          hInfo.nPos   = hpos;
          vInfo.fMask  = hInfo.fMask = SIF_RANGE | SIF_POS;

          SetScrollInfo(hwnd,SB_VERT,&vInfo,TRUE);
          SetScrollInfo(hwnd,SB_HORZ,&hInfo,TRUE);
        }
    }
}

/***********************************************************************
 *           ScrollChildren32   (USER32.448)
 */
void WINAPI ScrollChildren(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hWnd);
  INT newPos = -1;
  INT curPos, length, minPos, maxPos, shift;

  if (!win32wnd) return;

  if (uMsg == WM_HSCROLL)
  {
    GetScrollRange(hWnd,SB_HORZ,&minPos,&maxPos);
    curPos = GetScrollPos(hWnd,SB_HORZ);
    length = win32wnd->getClientWidth()/2;
    shift = GetSystemMetrics(SM_CYHSCROLL);
  } 
  else if (uMsg == WM_VSCROLL)
  {
    GetScrollRange(hWnd,SB_VERT,&minPos,&maxPos);
    curPos = GetScrollPos(hWnd,SB_VERT);
    length = win32wnd->getClientHeight()/2;
    shift = GetSystemMetrics(SM_CXVSCROLL);
  } 
  else 
  { 
    RELEASE_WNDOBJ(win32wnd);
    return;
  }
  RELEASE_WNDOBJ(win32wnd);

  switch( wParam )
  {
    case SB_LINEUP:
      newPos = curPos - shift;
      break;

    case SB_LINEDOWN:
      newPos = curPos + shift;
      break;

    case SB_PAGEUP:
      newPos = curPos - length;
      break;

    case SB_PAGEDOWN:
      newPos = curPos + length;
      break;

    case SB_THUMBPOSITION:
      newPos = LOWORD(lParam);
      break;

    case SB_THUMBTRACK:
      return;

    case SB_TOP:
      newPos = minPos;
      break;

    case SB_BOTTOM:
      newPos = maxPos;
      break;

    case SB_ENDSCROLL:
      CalcChildScroll(hWnd,(uMsg == WM_VSCROLL)?SB_VERT:SB_HORZ);
      return;
  }

  if( newPos > maxPos )
    newPos = maxPos;
  else
    if( newPos < minPos )
      newPos = minPos;

  SetScrollPos(hWnd, (uMsg == WM_VSCROLL)?SB_VERT:SB_HORZ , newPos, TRUE);

  if( uMsg == WM_VSCROLL )
    ScrollWindowEx(hWnd ,0 ,curPos - newPos, NULL, NULL, 0, NULL,
                   SW_INVALIDATE | SW_ERASE | SW_SCROLLCHILDREN );
  else
    ScrollWindowEx(hWnd ,curPos - newPos, 0, NULL, NULL, 0, NULL,
                   SW_INVALIDATE | SW_ERASE | SW_SCROLLCHILDREN );
}

/*****************************************************************************
 * Name      : WORD WIN32API CascadeWindows
 * Purpose   : The CascadeWindows function cascades the specified windows or
 *             the child windows of the specified parent window.
 * Parameters: HWND hwndParent         handle of parent window
 *             UINT wHow               types of windows not to arrange
 *             CONST RECT * lpRect     rectangle to arrange windows in
 *             UINT cKids              number of windows to arrange
 *             const HWND FAR * lpKids array of window handles
 * Variables :
 * Result    : If the function succeeds, the return value is the number of windows arranged.
 *             If the function fails, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
WORD WIN32API CascadeWindows(HWND       hwndParent,
                             UINT       wHow,
                             CONST LPRECT lpRect,
                             UINT       cKids,
                             const HWND *lpKids)
{
  dprintf(("USER32:CascadeWindows(%08xh,%u,%08xh,%u,%08x) not implemented.\n",
         hwndParent,
         wHow,
         lpRect,
         cKids,
         lpKids));

  return (0);
}

/*****************************************************************************
 * Name      : BOOL WIN32API CascadeChildWindows
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API CascadeChildWindows(DWORD x1,
                                     DWORD x2)
{
  dprintf(("USER32: CascadeChildWindows(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}

/*****************************************************************************
 * Name      : WORD WIN32API TileWindows
 * Purpose   : The TileWindows function tiles the specified windows, or the child
 *             windows of the specified parent window.
 * Parameters: HWND       hwndParent     handle of parent window
 *             WORD       wFlags         types of windows not to arrange
 *             LPCRECT    lpRect         rectangle to arrange windows in
 *             WORD       cChildrenb     number of windows to arrange
 *             const HWND *ahwndChildren array of window handles
 * Variables :
 * Result    : If the function succeeds, the return value is the number of
 *               windows arranged.
 *             If the function fails, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
WORD WIN32API TileWindows(HWND       hwndParent,
                          UINT       wFlags,
                          const LPRECT lpRect,
                          UINT       cChildrenb,
                          const HWND *ahwndChildren)
{
  dprintf(("USER32:TileWindows (%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
           hwndParent,
           wFlags,
           lpRect,
           cChildrenb,
           ahwndChildren));

   return (0);
}

/*****************************************************************************
 * Name      : BOOL WIN32API TileChildWindows
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API TileChildWindows(DWORD x1,
                                  DWORD x2)
{
  dprintf(("USER32: TileChildWindows(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}

/* -------- Miscellaneous service functions ---------- */

void Win32MDIClientWindow::postUpdate(WORD recalc)
{
    if( !(mdiFlags & MDIF_NEEDUPDATE) )
    {
        mdiFlags |= MDIF_NEEDUPDATE;
        PostMessageA(getWindowHandle(), WM_MDICALCCHILDSCROLL, 0, 0);
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
    wndClass.hCursor       = LoadCursorA(0,IDC_ARROWA);;
    wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
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
