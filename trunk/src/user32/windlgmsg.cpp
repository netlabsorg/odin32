/* $Id: windlgmsg.cpp,v 1.9 2001-06-09 14:50:25 sandervl Exp $ */
/*
 * Win32 dialog message APIs for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (OS/2 port & adaption)
 *
 * Based on Corel WINE code (20000317: window\dialog.c)
 * (Based on Wine code (990815: window\dialog.c))
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
 *
 * TODO: Dialog accelerator
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <string.h>
#include <ctype.h>
#include "win32wbase.h"
#include "win32dlg.h"

#define DBG_LOCALLOG    DBG_windlgmsg
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageA( HWND hwnd, int id, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Dialog *dialog;
  HWND hwndDlgItem;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog) {
        dprintf(("SendDlgItemMessageA, window %x not found", hwnd));
        return 0;
    }
    hwndDlgItem = dialog->getDlgItem(id);
    RELEASE_WNDOBJ(dialog);
    if(hwndDlgItem) {
        return SendMessageA(hwndDlgItem, Msg, wParam, lParam);
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageW( HWND hwnd, int id, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Dialog *dialog;
  HWND hwndDlgItem;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog) {
        dprintf(("SendDlgItemMessageW, window %x not found", hwnd));
        return 0;
    }
    hwndDlgItem = dialog->getDlgItem(id);
    RELEASE_WNDOBJ(dialog);
    if(hwndDlgItem) {
        return SendMessageW(hwndDlgItem, Msg, wParam, lParam);
    }
    return 0;
}
/***********************************************************************
 *           DIALOG_IsAccelerator
 */
static BOOL DIALOG_IsAccelerator( HWND hwnd, HWND hwndDlg, WPARAM vKey )
{
    HWND hwndControl = hwnd;
    HWND hwndNext;
    Win32BaseWindow *win32wnd;
    BOOL RetVal = FALSE;
    INT dlgCode;

    if (vKey == VK_SPACE)
    {
        dlgCode = SendMessageA( hwndControl, WM_GETDLGCODE, 0, 0 );
        if (dlgCode & DLGC_BUTTON)
        {
            SendMessageA( hwndControl, WM_LBUTTONDOWN, 0, 0);
            SendMessageA( hwndControl, WM_LBUTTONUP, 0, 0);
            RetVal = TRUE;
        }
    }
    else
    {
        do
        {
            win32wnd = Win32BaseWindow::GetWindowFromHandle(hwndControl);
            if ( (win32wnd != NULL) &&
                 ((win32wnd->getStyle() & (WS_VISIBLE | WS_DISABLED)) == WS_VISIBLE) )
            {
                dlgCode = SendMessageA( hwndControl, WM_GETDLGCODE, 0, 0 );
                if (dlgCode & (DLGC_BUTTON | DLGC_STATIC))
                {
                    INT textLen = win32wnd->GetWindowTextLengthA();

                    if (textLen > 0)
                    {
                        /* find the accelerator key */
                        char* text;
                        LPSTR p;

                        text = (char*)malloc(textLen+1);
                        win32wnd->GetWindowTextA(text,textLen);
                        p = text - 2;
                        do
                        {
                            p = strchr( p + 2, '&' );
                        }
                        while (p != NULL && p[1] == '&');

                        /* and check if it's the one we're looking for */
                        if (p != NULL && toupper( p[1] ) == toupper( vKey ) )
                        {
                            if ((dlgCode & DLGC_STATIC) ||
                                (win32wnd->getStyle() & 0x0f) == BS_GROUPBOX )
                            {
                                /* set focus to the control */
                                SendMessageA( hwndDlg, WM_NEXTDLGCTL,
                                              hwndControl, 1);
                                /* and bump it on to next */
                                SendMessageA( hwndDlg, WM_NEXTDLGCTL, 0, 0);
                            }
                            else
                            //TODO: this else part was removed in Wine and above if rules out this possibility (if (dlgCode & (DLGC_BUTTON | DLGC_STATIC)))
                            if (dlgCode & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))
                            {
                                /* send command message as from the control */
                                SendMessageA( hwndDlg, WM_COMMAND, MAKEWPARAM( LOWORD(win32wnd->getWindowId()), BN_CLICKED ),
                                              (LPARAM)hwndControl );
                            }
                            else if (dlgCode & DLGC_BUTTON)
                            {
                                /* send BM_CLICK message to the control */
                                SendMessageA( hwndControl, BM_CLICK, 0, 0 );
                            }
                            RetVal = TRUE;
                            free(text);
                            RELEASE_WNDOBJ(win32wnd);
                            break;
                        }
                        free(text);
                    }
                }
                hwndNext = GetWindow( hwndControl, GW_CHILD );
            }
            else
            {
                hwndNext = 0;
            }
            if(win32wnd) RELEASE_WNDOBJ(win32wnd);

            if (!hwndNext)
            {
                hwndNext = GetWindow( hwndControl, GW_HWNDNEXT );
            }
            while (!hwndNext)
            {
                hwndControl = GetParent( hwndControl );
                if (hwndControl == hwndDlg)
                {
                    if(hwnd==hwndDlg){  /* prevent endless loop */
                        hwndNext=hwnd;
                        break;
                    }
                    hwndNext = GetWindow( hwndDlg, GW_CHILD );
                }
                else
                {
                    hwndNext = GetWindow( hwndControl, GW_HWNDNEXT );
                }
            }
            hwndControl = hwndNext;
        }
        while (hwndControl != hwnd);
    }
    return RetVal;
}
/***********************************************************************
 *           DIALOG_FindMsgDestination
 *
 * The messages that IsDialogMessage send may not go to the dialog
 * calling IsDialogMessage if that dialog is a child, and it has the
 * DS_CONTROL style set.
 * We propagate up until we hit a that does not have DS_CONTROL, or
 * whose parent is not a dialog.
 *
 * This is undocumented behaviour.
 */
static HWND DIALOG_FindMsgDestination( HWND hwndDlg )
{
    while (GetWindowLongA(hwndDlg, GWL_STYLE) & DS_CONTROL)
    {
        Win32BaseWindow *pParent;
        HWND hParent = GetParent(hwndDlg);
        if (!hParent) break;

        pParent = Win32BaseWindow::GetWindowFromHandle(hParent);
        if (!pParent) break;

        if (!pParent->IsDialog()) {
            RELEASE_WNDOBJ(pParent);
            break;
        }
        RELEASE_WNDOBJ(pParent);
        hwndDlg = hParent;
    }

    return hwndDlg;
}

/***********************************************************************
 *           DIALOG_IsDialogMessage
 */
static BOOL DIALOG_IsDialogMessage( HWND hwndDlg, BOOL *translate, BOOL *dispatch, INT dlgCode, LPMSG msg )
{
    *translate = *dispatch = FALSE;

    if (msg->message == WM_PAINT)
    {
        /* Apparently, we have to handle this one as well */
        *dispatch = TRUE;
        return TRUE;
    }

      /* Only the key messages get special processing */
    if ((msg->message != WM_KEYDOWN) &&
        (msg->message != WM_SYSCHAR) &&
        (msg->message != WM_CHAR))
        return FALSE;

    if (dlgCode & DLGC_WANTMESSAGE)
    {
        *translate = *dispatch = TRUE;
        return TRUE;
    }

    hwndDlg = DIALOG_FindMsgDestination(hwndDlg);

    switch(msg->message)
    {
    case WM_KEYDOWN:
        switch(msg->wParam)
        {
        case VK_TAB:
            if (!(dlgCode & DLGC_WANTTAB))
            {
                SendMessageA( hwndDlg, WM_NEXTDLGCTL,
                             (GetKeyState(VK_SHIFT) & 0x8000), 0 );
                return TRUE;
            }
            break;

        case VK_RIGHT:
        case VK_DOWN:
        case VK_LEFT:
        case VK_UP:
            if (!(dlgCode & DLGC_WANTARROWS))
            {
                BOOL fPrevious = (msg->wParam == VK_LEFT || msg->wParam == VK_UP);
                HWND hwndNext =
                    GetNextDlgGroupItem (hwndDlg, GetFocus(), fPrevious );
                SendMessageA( hwndDlg, WM_NEXTDLGCTL, hwndNext, 1 );
                return TRUE;
            }
            break;

        case VK_ESCAPE:
            SendMessageA( hwndDlg, WM_COMMAND, IDCANCEL,
                            (LPARAM)GetDlgItem( hwndDlg, IDCANCEL ) );
            return TRUE;

        case VK_RETURN:
            {
                DWORD dw = SendMessageA( hwndDlg, DM_GETDEFID, 0, 0 );
                if (HIWORD(dw) == DC_HASDEFID)
                {
                    SendMessageA( hwndDlg, WM_COMMAND,
                                    MAKEWPARAM( LOWORD(dw), BN_CLICKED ),
                                    (LPARAM)GetDlgItem(hwndDlg, LOWORD(dw)));
                }
                else
                {
                    SendMessageA( hwndDlg, WM_COMMAND, IDOK,
                                    (LPARAM)GetDlgItem( hwndDlg, IDOK ) );

                }

                return TRUE;
            }
        }
        *translate = TRUE;
        break; /* case WM_KEYDOWN */

    case WM_CHAR:
        if (dlgCode & DLGC_WANTCHARS) break;
        /* drop through */

    case WM_SYSCHAR:
        if (DIALOG_IsAccelerator( msg->hwnd, hwndDlg, msg->wParam ))
        {
            /* don't translate or dispatch */
            return TRUE;
        }
        break;
    }

    /* If we get here, the message has not been treated specially */
    /* and can be sent to its destination window. */
    *dispatch = TRUE;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDialogMessageA( HWND hwndDlg, LPMSG msg)
{
    BOOL ret, translate, dispatch;
    INT dlgCode;

    if ((hwndDlg != msg->hwnd) && !IsChild( hwndDlg, msg->hwnd ))
        return FALSE;

    dlgCode = SendMessageA( msg->hwnd, WM_GETDLGCODE, 0, (LPARAM)msg);
    ret = DIALOG_IsDialogMessage(hwndDlg,&translate,&dispatch,dlgCode,msg);
    if (translate) TranslateMessage( msg );
    if (dispatch) DispatchMessageA( msg );

    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDialogMessageW(HWND hwndDlg, LPMSG msg)
{
    BOOL ret, translate, dispatch;
    INT dlgCode;

    if ((hwndDlg != msg->hwnd) && !IsChild( hwndDlg, msg->hwnd ))
        return FALSE;

    dlgCode = SendMessageW( msg->hwnd, WM_GETDLGCODE, 0, (LPARAM)msg);
    ret = DIALOG_IsDialogMessage(hwndDlg,&translate,&dispatch,dlgCode,msg);
    if (translate) TranslateMessage( msg );
    if (dispatch) DispatchMessageW( msg );
    return ret;
}
//******************************************************************************
//******************************************************************************
