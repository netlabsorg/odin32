/* $Id: windlgmsg.cpp,v 1.2 1999-10-08 21:30:52 cbratschi Exp $ */
/*
 * Win32 dialog message APIs for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (OS/2 port & adaption)
 *
 * Based on Wine code (990815: window\dialog.c)
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
#include "win32wbase.h"
#include "win32dlg.h"

//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageA( HWND hwnd, int id, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Dialog *dialog;
  Win32BaseWindow *dlgcontrol;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog) {
        dprintf(("SendDlgItemMessageA, window %x not found", hwnd));
        return 0;
    }
    dlgcontrol = dialog->getDlgItem(id);
    if(dlgcontrol) {
        return dlgcontrol->SendMessageA(Msg, wParam, lParam);
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageW( HWND hwnd, int id, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  Win32Dialog *dialog;
  Win32BaseWindow *dlgcontrol;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog) {
        dprintf(("SendDlgItemMessageW, window %x not found", hwnd));
        return 0;
    }
    dlgcontrol = dialog->getDlgItem(id);
    if(dlgcontrol) {
        return dlgcontrol->SendMessageW(Msg, wParam, lParam);
    }
    return 0;
}
//TODO
#if 0
/***********************************************************************
 *           DIALOG_IsAccelerator
 */
static BOOL DIALOG_IsAccelerator( HWND hwnd, HWND hwndDlg, WPARAM vKey )
{
    HWND hwndControl = hwnd;
    HWND hwndNext;
    WND *wndPtr;
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
            wndPtr = WIN_FindWndPtr( hwndControl );
            if ( (wndPtr != NULL) &&
                 ((wndPtr->dwStyle & (WS_VISIBLE | WS_DISABLED)) == WS_VISIBLE) )
            {
                dlgCode = SendMessageA( hwndControl, WM_GETDLGCODE, 0, 0 );
                if ( (dlgCode & (DLGC_BUTTON | DLGC_STATIC)) &&
                     (wndPtr->text!=NULL))
                {
                    /* find the accelerator key */
                    LPSTR p = wndPtr->text - 2;
                    do
                    {
                        p = strchr( p + 2, '&' );
                    }
                    while (p != NULL && p[1] == '&');

                    /* and check if it's the one we're looking for */
                    if (p != NULL && toupper( p[1] ) == toupper( vKey ) )
                    {
                        if ((dlgCode & DLGC_STATIC) ||
                            (wndPtr->dwStyle & 0x0f) == BS_GROUPBOX )
                        {
                            /* set focus to the control */
                            SendMessageA( hwndDlg, WM_NEXTDLGCTL,
                                    hwndControl, 1);
                            /* and bump it on to next */
                            SendMessageA( hwndDlg, WM_NEXTDLGCTL, 0, 0);
                        }
                        else if (dlgCode &
                (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))
                        {
                            /* send command message as from the control */
                            SendMessageA( hwndDlg, WM_COMMAND,
                                MAKEWPARAM( LOWORD(wndPtr->wIDmenu),
                                    BN_CLICKED ),
                                (LPARAM)hwndControl );
                        }
                        else
                        {
                            /* click the control */
                            SendMessageA( hwndControl, WM_LBUTTONDOWN, 0, 0);
                            SendMessageA( hwndControl, WM_LBUTTONUP, 0, 0);
                        }
                        RetVal = TRUE;
            WIN_ReleaseWndPtr(wndPtr);
                        break;
                    }
                }
        hwndNext = GetWindow( hwndControl, GW_CHILD );
            }
        else
        {
        hwndNext = 0;
        }
            WIN_ReleaseWndPtr(wndPtr);
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
#endif
/***********************************************************************
 *           DIALOG_IsDialogMessage
 */
static BOOL DIALOG_IsDialogMessage( HWND hwnd, HWND hwndDlg,
                                    UINT message, WPARAM wParam,
                                    LPARAM lParam, BOOL *translate,
                                    BOOL *dispatch, INT dlgCode )
{
    *translate = *dispatch = FALSE;

    if (message == WM_PAINT)
    {
        /* Apparently, we have to handle this one as well */
        *dispatch = TRUE;
        return TRUE;
    }

      /* Only the key messages get special processing */
    if ((message != WM_KEYDOWN) &&
        (message != WM_SYSCHAR) &&
        (message != WM_CHAR))
        return FALSE;

    if (dlgCode & DLGC_WANTMESSAGE)
    {
        *translate = *dispatch = TRUE;
        return TRUE;
    }

    switch(message)
    {
    case WM_KEYDOWN:
        switch(wParam)
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
                BOOL fPrevious = (wParam == VK_LEFT || wParam == VK_UP);
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
            }
            return TRUE;
        }
        *translate = TRUE;
        break; /* case WM_KEYDOWN */

    case WM_CHAR:
        if (dlgCode & DLGC_WANTCHARS) break;
        /* drop through */

//TODO:
#if 0
    case WM_SYSCHAR:
        if (DIALOG_IsAccelerator( hwnd, hwndDlg, wParam ))
        {
            /* don't translate or dispatch */
            return TRUE;
        }
        break;
#endif
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
    ret = DIALOG_IsDialogMessage( msg->hwnd, hwndDlg, msg->message,
                                  msg->wParam, msg->lParam,
                                  &translate, &dispatch, dlgCode );
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
    ret = DIALOG_IsDialogMessage( msg->hwnd, hwndDlg, msg->message,
                                  msg->wParam, msg->lParam,
                                  &translate, &dispatch, dlgCode );
    if (translate) TranslateMessage( msg );
    if (dispatch) DispatchMessageW( msg );
    return ret;
}
//******************************************************************************
//******************************************************************************
