/* $Id: win32wmdichild.cpp,v 1.7 2000-01-02 19:30:45 cbratschi Exp $ */
/*
 * Win32 MDI Child Window Class for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Based on Wine (windows\mdi.c) (990815)
 *
 * Copyright 1994, Bob Amstadt
 *           1995,1996 Alex Korobka
 * Copyright 1993, 1994 Alexandre Julliard
 *
 *
 *
 * TODO: See #if 0's
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <win.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <heapstring.h>
#include <win32wnd.h>
#include <win32wmdiclient.h>
#include <win32wmdichild.h>
#include <spy.h>
#include "wndmsg.h"
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
Win32MDIChildWindow::Win32MDIChildWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL fUnicode)
                    : Win32BaseWindow(OBJTYPE_WINDOW)
{
    isUnicode = fUnicode;
    CreateWindowExA(lpCreateStructA, classAtom);
}
//******************************************************************************
//******************************************************************************
Win32MDIChildWindow::~Win32MDIChildWindow()
{
}
//******************************************************************************
//******************************************************************************
ULONG Win32MDIChildWindow::MsgActivate(BOOL fActivate, BOOL fMinimized, HWND hwnd)
{
 ULONG rc, curprocid, procidhwnd = -1, threadidhwnd = 0;

    //According to SDK docs, if app returns FALSE & window is being deactivated,
    //default processing is cancelled
    //TODO: According to Wine we should proceed anyway if window is sysmodal
    if(SendInternalMessageA(WM_NCACTIVATE, fActivate, 0) == FALSE && !fActivate)
    {
        return 0;
    }
    if(fActivate)
    {
         rc = SendInternalMessageA(WM_CHILDACTIVATE, MAKELONG((fActivate) ? WA_ACTIVE : WA_INACTIVE, fMinimized), hwnd);
         curprocid  = GetCurrentProcessId();
         if(hwnd) {
                threadidhwnd = GetWindowThreadProcessId(hwnd, &procidhwnd);
         }

         if(curprocid != procidhwnd && fActivate) {
            SendInternalMessageA(WM_ACTIVATEAPP, 1, threadidhwnd);
         }
         return rc;
    }
    else return 1;
}
//******************************************************************************
//******************************************************************************
BOOL Win32MDIChildWindow::isMDIChild()
{
    return TRUE;
}
//******************************************************************************
//******************************************************************************
LRESULT Win32MDIChildWindow::DefMDIChildProcA(UINT Msg, WPARAM wParam, LPARAM lParam)
{
 Win32MDIClientWindow *client = (Win32MDIClientWindow *)getParent();

    switch (Msg)
    {
    case WM_SETTEXT:
        DefWindowProcA(Msg, wParam, lParam);
        menuModifyItem();
        if( client->getMaximizedChild() == this )
                client->updateFrameText(MDI_REPAINTFRAME, NULL);
        return 0;

    case WM_GETMINMAXINFO:
    {
        childGetMinMaxInfo((MINMAXINFO *)lParam);
        return 0;
    }

    case WM_MENUCHAR:
        /* MDI children don't have menu bars */
        PostMessageA(client->getWindowHandle(), WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)LOWORD(wParam) );
        return 0x00010000L;

    case WM_CLOSE:
        client->SendMessageA(WM_MDIDESTROY,(WPARAM)getWindowHandle(), 0L);
        return 0;

    case WM_SETFOCUS:
        if(client->getActiveChild() != this )
            client->childActivate(this);
        break;

    case WM_CHILDACTIVATE:
        client->childActivate(this);
        return 0;

    case WM_NCPAINT:
        break;

    case WM_SYSCOMMAND:
        switch( wParam )
        {
        case SC_MOVE:
            if( client->getMaximizedChild() == this)
            {
                return 0;
            }
            break;
        case SC_RESTORE:
        case SC_MINIMIZE:
            setStyle(getStyle() | WS_SYSMENU);
            break;

        case SC_MAXIMIZE:
            if( client->getMaximizedChild() == this)
            {
                  return client->SendMessageA(Msg, wParam, lParam);
            }
            setStyle(getStyle() & ~WS_SYSMENU);
            break;

        case SC_NEXTWINDOW:
             client->SendMessageA(WM_MDINEXT, 0, 0);
             return 0;

        case SC_PREVWINDOW: //WM_MDINEXT??
             client->SendMessageA(WM_MDINEXT, 0, 0);
             return 0;
        }
        break;

    case WM_SETVISIBLE:
        if( client->getMaximizedChild()) {
                client->setMdiFlags(client->getMdiFlags() & ~MDIF_NEEDUPDATE);
        }
        else    client->postUpdate(SB_BOTH+1);
        break;

    case WM_SIZE:
        /* do not change */
        if( client->getActiveChild() == this && wParam != SIZE_MAXIMIZED )
        {
            client->setMaximizedChild(NULL);
            client->restoreFrameMenu(this);
            client->updateFrameText(MDI_REPAINTFRAME, NULL );
        }

        if( wParam == SIZE_MAXIMIZED )
        {
            Win32MDIChildWindow *maxChild = client->getMaximizedChild();

            if( maxChild == this ) break;

            if( maxChild)
            {
                maxChild->SendMessageA(WM_SETREDRAW, FALSE, 0L );
                client->restoreFrameMenu(maxChild);
                maxChild->ShowWindow(SW_SHOWNOACTIVATE);

                maxChild->SendMessageA(WM_SETREDRAW, TRUE, 0L );
            }

            client->setMaximizedChild(this);
            client->setActiveChild(this);

            client->augmentFrameMenu(this);

            client->updateFrameText(MDI_REPAINTFRAME, NULL );
        }

        if( wParam == SIZE_MINIMIZED )
        {
            Win32MDIChildWindow *switchTo = client->getWindow(this, TRUE, WS_MINIMIZE);

            if( switchTo )
                switchTo->SendMessageA(WM_CHILDACTIVATE, 0, 0L);
        }

        client->postUpdate(SB_BOTH+1);
        break;

#if 0
    case WM_NEXTMENU:
        if( wParam == VK_LEFT )     /* switch to frame system menu */
        {
            return MAKELONG( GetSubMenu(clientWnd->parent->hSysMenu, 0),
               clientWnd->parent->hwndSelf );
            goto END;
        }
        if( wParam == VK_RIGHT )    /* to frame menu bar */
        {
            retvalue = MAKELONG( clientWnd->parent->wIDmenu,
               clientWnd->parent->hwndSelf );
            goto END;
        }
#endif
    case WM_SYSCHAR:
        if (wParam == '-')
        {
            SendInternalMessageA(WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)(DWORD)VK_SPACE);
            return 0;
        }
    }
    return DefWindowProcA(Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT Win32MDIChildWindow::DefMDIChildProcW(UINT Msg, WPARAM wParam, LPARAM lParam)
{
 Win32MDIClientWindow *client = (Win32MDIClientWindow *)getParent();

    switch (Msg)
    {
    case WM_SETTEXT:
        DefWindowProcW(Msg, wParam, lParam);
        menuModifyItem();
        if( client->getMaximizedChild() == this )
                client->updateFrameText(MDI_REPAINTFRAME, NULL );

        return 0;

    case WM_GETMINMAXINFO:
    case WM_MENUCHAR:
    case WM_CLOSE:
    case WM_SETFOCUS:
    case WM_CHILDACTIVATE:
    case WM_NCPAINT:
    case WM_SYSCOMMAND:
    case WM_SETVISIBLE:
    case WM_SIZE:
    case WM_NEXTMENU:
        return DefMDIChildProcA(Msg, wParam, lParam );

    case WM_SYSCHAR:
        if (wParam == '-')
        {
            SendInternalMessageW(WM_SYSCOMMAND, SC_KEYMENU, (LPARAM)(DWORD)VK_SPACE);
            return 0;
        }
        break;
    }
    return DefWindowProcW(Msg, wParam, lParam);
}
/**********************************************************************
 *                  MDICreateChild
 */
HWND Win32MDIChildWindow::createChild(Win32MDIClientWindow *client, LPMDICREATESTRUCTA cs )
{
  POINT        pos[2];
  DWORD        style = cs->style | (WS_CHILD | WS_CLIPSIBLINGS);
  WORD         wIDmenu = client->getFirstChildId() + client->getNrOfChildren();
  char         lpstrDef[]="junk!";
  Win32MDIChildWindow *maximizedChild, *newchild;
  CREATESTRUCTA createstruct;
  ATOM         classAtom;
  char         tmpClassA[20] = "";
  WCHAR        tmpClassW[20];
  LPSTR        className;

    dprintf(("Win32MDIChildWindow::createChild %i,%i - dim %i,%i, style %08x\n",
             cs->x, cs->y, cs->cx, cs->cy, (unsigned)cs->style));

    /* calculate placement */
    calcDefaultChildPos(client, client->incTotalCreated()-1, pos, 0);

    if (cs->cx == CW_USEDEFAULT || !cs->cx) cs->cx = pos[1].x;
    if (cs->cy == CW_USEDEFAULT || !cs->cy) cs->cy = pos[1].y;

    if( cs->x == CW_USEDEFAULT )
    {
        cs->x = pos[0].x;
        cs->y = pos[0].y;
    }

    /* restore current maximized child */
    if( style & WS_VISIBLE && client->getMaximizedChild() )
    {
        if( style & WS_MAXIMIZE )
            client->SendMessageA(WM_SETREDRAW, FALSE, 0L );

        maximizedChild = client->getMaximizedChild();

        maximizedChild->ShowWindow( SW_SHOWNOACTIVATE );

        if( style & WS_MAXIMIZE )
            client->SendMessageA(WM_SETREDRAW, TRUE, 0L );
    }

    /* this menu is needed to set a check mark in MDI_ChildActivate */
    AppendMenuA(client->getMDIMenu(), MF_STRING ,wIDmenu, lpstrDef );

    client->incNrActiveChildren();

    /* fix window style */
    if( !(client->getStyle() & MDIS_ALLCHILDSTYLES) )
    {
        style &= (WS_CHILD | WS_CLIPSIBLINGS | WS_MINIMIZE | WS_MAXIMIZE |
                  WS_CLIPCHILDREN | WS_DISABLED | WS_VSCROLL | WS_HSCROLL );
        style |= (WS_VISIBLE | WS_OVERLAPPEDWINDOW);
    }

    createstruct.lpszName  = cs->szTitle;
    createstruct.style     = style;
    createstruct.dwExStyle = 0;
    createstruct.x         = cs->x;
    createstruct.y         = cs->y;
    createstruct.cx        = cs->cx;
    createstruct.cy        = cs->cy;
    createstruct.hInstance = cs->hOwner;
    createstruct.hMenu     = wIDmenu;
    createstruct.hwndParent= client->getWindowHandle();
    createstruct.lpCreateParams = (LPVOID)cs;

    className = (LPSTR)cs->szClass;
    /* Find the class atom */
    classAtom = GlobalFindAtomA(cs->szClass);
    if(classAtom == 0)
    {
        if (!HIWORD(cs->szClass))
        {
            sprintf(tmpClassA,"#%d", (int) className);
            classAtom = GlobalFindAtomA(tmpClassA);
            className = tmpClassA;
        }
        if (!classAtom)
        {
          if (!HIWORD(cs->szClass)) {
                  dprintf(("createChild: bad class name %04x\n", LOWORD(className)));
          }
          else    dprintf(("createChild: bad class name '%s'\n", tmpClassA ));

          SetLastError(ERROR_INVALID_PARAMETER);
          return 0;
        }
    }
    createstruct.lpszClass = className;

    newchild = new Win32MDIChildWindow(&createstruct, classAtom, FALSE);

    if(newchild && GetLastError() == 0)
    {
        newchild->menuModifyItem();

        if( newchild->getStyle() & WS_MINIMIZE && client->getActiveChild()) {
            newchild->ShowWindow(SW_SHOWMINNOACTIVE);
        }
        else
        {
            /* WS_VISIBLE is clear if a) the MDI client has
             * MDIS_ALLCHILDSTYLES style and 2) the flag is cleared in the
             * MDICreateStruct. If so the created window is not shown nor
             * activated.
             */
            int showflag = newchild->getStyle() & WS_VISIBLE;
            /* clear visible flag, otherwise SetWindoPos32 ignores
             * the SWP_SHOWWINDOW command.
             */
//SvL: Not here. This causes problems in OS/2
//            newchild->SetWindowLongA(GWL_STYLE, showflag & ~WS_VISIBLE);
            if(showflag){
                newchild->SetWindowPos(0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE );

                /* Set maximized state here in case hwnd didn't receive WM_SIZE
                 * during CreateWindow - bad!
                 */

                if((newchild->getStyle() & WS_MAXIMIZE) && !client->getMaximizedChild() )
                {
                    client->setMaximizedChild(newchild);

                    client->augmentFrameMenu(newchild);

                    client->updateFrameText(MDI_REPAINTFRAME, NULL );
                }
            }
            else
                /* needed, harmless ? */
                newchild->SetWindowPos(0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE );

        }
    }
    else
    {
        client->decNrActiveChildren();
        DeleteMenu(client->getMDIMenu(), wIDmenu,MF_BYCOMMAND);

        maximizedChild = client->getMaximizedChild();
        if( maximizedChild && maximizedChild->IsWindow() )
            maximizedChild->ShowWindow(SW_SHOWMAXIMIZED);

        dprintf(("MDI child creation failed!!"));
        return 0;
    }
    return newchild->getWindowHandle();
}
/**********************************************************************
 *          MDI_MenuModifyItem
 */
BOOL Win32MDIChildWindow::menuModifyItem()
{
    Win32MDIClientWindow *client = (Win32MDIClientWindow *)getParent();
    char   buffer[128];
    UINT   id   = getWindowId()-client->getFirstChildId()+1;
    UINT   n    = sprintf(buffer,(id > 9) ? "%d":"&%d ",id);
    BOOL   bRet = 0;

    if( !client->getMDIMenu() )
    {
        return FALSE;
    }

    if (getWindowNameA()) lstrcpynA(buffer + n, getWindowNameA(), sizeof(buffer) - n );

    n    = GetMenuState(client->getMDIMenu(), getWindowId() ,MF_BYCOMMAND);
    bRet = ModifyMenuA(client->getMDIMenu() , getWindowId(),
                       MF_BYCOMMAND | MF_STRING, getWindowId(), buffer );
    CheckMenuItem(client->getMDIMenu(), getWindowId() , n & MF_CHECKED);

    return bRet;
}

/**********************************************************************
 *          MDI_MenuDeleteItem
 */
BOOL Win32MDIChildWindow::menuDeleteItem()
{
    Win32MDIClientWindow *client = (Win32MDIClientWindow *)getParent();
    char    buffer[128];
    UINT    index      = 0,id,n;
    BOOL    retvalue;

    if( !client->getNrOfChildren() ||
        !client->getMDIMenu())
    {
        return FALSE;
    }

    id = getWindowId();
    DeleteMenu(client->getMDIMenu(),id,MF_BYCOMMAND);

    /* walk the rest of MDI children to prevent gaps in the id
     * sequence and in the menu child list */

    for( index = id+1; index <= client->getNrOfChildren() +
         client->getFirstChildId(); index++ )
    {
        Win32MDIChildWindow *tmpWnd = client->getChildByID(index);
        if( !tmpWnd )
        {
          dprintf(("no window for id=%i\n",index));
          continue;
        }

        /* set correct id */
        tmpWnd->setWindowId(tmpWnd->getWindowId()-1);

        n = sprintf(buffer, "%d ",index - client->getFirstChildId());
        if (tmpWnd->getWindowNameA())
            lstrcpynA(buffer + n, tmpWnd->getWindowNameA(), sizeof(buffer) - n );

        /* change menu */
        ModifyMenuA(client->getMDIMenu(), index ,MF_BYCOMMAND | MF_STRING,
                    index - 1 , buffer );
    }
    return TRUE;
}
/**********************************************************************
 *          MDI_CalcDefaultChildPos
 *
 *  It seems that the default height is about 2/3 of the client rect
 */
void Win32MDIChildWindow::calcDefaultChildPos(Win32MDIClientWindow *client, WORD n, LPPOINT lpPos, INT delta)
{
    INT  nstagger;
    RECT rect;
    INT  spacing = GetSystemMetrics(SM_CYCAPTION) +
                   GetSystemMetrics(SM_CYFRAME) - 1;

    client->getClientRect(&rect);
    if( rect.bottom - rect.top - delta >= spacing )
        rect.bottom -= delta;

    nstagger = (rect.bottom - rect.top)/(3 * spacing);
    lpPos[1].x = (rect.right - rect.left - nstagger * spacing);
    lpPos[1].y = (rect.bottom - rect.top - nstagger * spacing);
    lpPos[0].x = lpPos[0].y = spacing * (n%(nstagger+1));
}

/**********************************************************************
 *          MDI_ChildGetMinMaxInfo
 *
 * Note: The rule here is that client rect of the maximized MDI child
 *   is equal to the client rect of the MDI client window.
 */
void Win32MDIChildWindow::childGetMinMaxInfo(MINMAXINFO* lpMinMax )
{
    Win32MDIClientWindow *client = (Win32MDIClientWindow *)getParent();
    RECT rect;

    getClientRect(&rect);
    if(client->getParent() == NULL) {
        dprintf(("Win32MDIChildWindow::childGetMinMaxInfo:: client parent == NULL!!"));
        return;
    }
    MapWindowPoints(client->getParent()->getWindowHandle(), client->getWindowHandle(), (LPPOINT)&rect, 2);

    AdjustWindowRectEx( &rect, getStyle(), 0, getExStyle());

    lpMinMax->ptMaxSize.x = rect.right -= rect.left;
    lpMinMax->ptMaxSize.y = rect.bottom -= rect.top;

    lpMinMax->ptMaxPosition.x = rect.left;
    lpMinMax->ptMaxPosition.y = rect.top;
}

//******************************************************************************
//******************************************************************************

