/* $Id: comboex.cpp,v 1.4 2000-05-22 17:25:07 cbratschi Exp $ */
/*
 * ComboBoxEx control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 *
 * NOTES
 *   This is just a dummy control. An author is needed! Any volunteers?
 *   I will only improve this control once in a while.
 *     Eric <ekohl@abo.rhein-zeitung.de>
 *
 * TODO:
 *   - All messages.
 *   - All notifications.
 *
 * FIXME:
 *   - should include "combo.h"
 */

/*
 - Corel WINE 20000513 level
*/

#include "winbase.h"
#include "commctrl.h"
#include "comctl32.h"
#include "ccbase.h"
#include "comboex.h"


#define ID_CB_EDIT    1001

#define COMBOEX_GetInfoPtr(wndPtr) ((COMBOEX_INFO *)getInfoPtr(hwnd))


/* << COMBOEX_DeleteItem >> */


static LRESULT
COMBOEX_GetComboControl (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->hwndCombo;
}


static LRESULT
COMBOEX_GetEditControl (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);

    if ((GetWindowLongA (hwnd, GWL_STYLE) & CBS_DROPDOWNLIST) != CBS_DROPDOWN)
        return 0;

    return (LRESULT)GetDlgItem (infoPtr->hwndCombo, ID_CB_EDIT);
}


static LRESULT
COMBOEX_GetExtendedStyle (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->dwExtStyle;
}


static LRESULT
COMBOEX_GetImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->himl;
}




static LRESULT COMBOEX_InsertItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
    /* COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd); */

//    FIXME (comboex, "(0x%08x 0x%08lx)\n", wParam, lParam);

    return -1;
}



static LRESULT
COMBOEX_SetExtendedStyle (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);
    DWORD dwTemp;

    dwTemp = infoPtr->dwExtStyle;

    if ((DWORD)wParam) {
        infoPtr->dwExtStyle = (infoPtr->dwExtStyle & ~(DWORD)wParam) | (DWORD)lParam;
    }
    else
        infoPtr->dwExtStyle = (DWORD)lParam;

    /* FIXME: repaint?? */

    return (LRESULT)dwTemp;
}


static LRESULT
COMBOEX_SetImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);
    HIMAGELIST himlTemp;

    himlTemp = infoPtr->himl;
    infoPtr->himl = (HIMAGELIST)lParam;

    return (LRESULT)himlTemp;
}


static LRESULT COMBOEX_SetItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
    /* COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd); */

//    FIXME (comboex, "(%p): stub\n", (LPVOID)lParam);

    return TRUE;
}


static LRESULT
COMBOEX_Forward (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);

//    FIXME (comboex, "(0x%x 0x%x 0x%lx): stub\n", uMsg, wParam, lParam);

    if (infoPtr->hwndCombo)
        return SendMessageA (infoPtr->hwndCombo, uMsg, wParam, lParam);

    return 0;
}


static LRESULT
COMBOEX_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr;
    DWORD dwComboStyle;

    /* allocate memory for info structure */
    infoPtr = (COMBOEX_INFO*)initControl(hwnd,sizeof(COMBOEX_INFO));
    if (infoPtr == NULL) {
//      ERR (comboex, "could not allocate info memory!\n");
        return 0;
    }

    /* initialize info structure */


    /* create combo box */
    dwComboStyle = GetWindowLongA (hwnd, GWL_STYLE) &
                        (CBS_SIMPLE|CBS_DROPDOWN|CBS_DROPDOWNLIST|WS_CHILD);

    infoPtr->hwndCombo = CreateWindowA ("ComboBox", "",
                         WS_CHILD | WS_VISIBLE | CBS_OWNERDRAWFIXED | dwComboStyle,
                        0, 0, 0, 0, hwnd, (HMENU)1,
                        GetWindowLongA (hwnd, GWL_HINSTANCE), NULL);

    return 0;
}


static LRESULT
COMBOEX_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);

    /* free comboex info data */
    doneControl(hwnd);

    return 0;
}


static LRESULT
COMBOEX_Size (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    COMBOEX_INFO *infoPtr = COMBOEX_GetInfoPtr (hwnd);
    RECT rect;

    GetClientRect (hwnd, &rect);

    MoveWindow (infoPtr->hwndCombo, 0, 0, rect.right -rect.left,
                  rect.bottom - rect.top, TRUE);

    return 0;
}


static LRESULT WINAPI
COMBOEX_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
/*      case CBEM_DELETEITEM: */

        case CBEM_GETCOMBOCONTROL:
            return COMBOEX_GetComboControl (hwnd, wParam, lParam);

        case CBEM_GETEDITCONTROL:
            return COMBOEX_GetEditControl (hwnd, wParam, lParam);

        case CBEM_GETEXTENDEDSTYLE:
            return COMBOEX_GetExtendedStyle (hwnd, wParam, lParam);

        case CBEM_GETIMAGELIST:
            return COMBOEX_GetImageList (hwnd, wParam, lParam);

/*      case CBEM_GETITEM32A:
        case CBEM_GETITEM32W:
        case CBEM_HASEDITCHANGED:
*/

        case CBEM_INSERTITEMA:
            return COMBOEX_InsertItem(hwnd,wParam,lParam,FALSE);

        case CBEM_INSERTITEMW:
            return COMBOEX_InsertItem(hwnd,wParam,lParam,TRUE);

        case CBEM_SETEXTENDEDSTYLE:
            return COMBOEX_SetExtendedStyle (hwnd, wParam, lParam);

        case CBEM_SETIMAGELIST:
            return COMBOEX_SetImageList (hwnd, wParam, lParam);

        case CBEM_SETITEMA:
            return COMBOEX_SetItem(hwnd,wParam,lParam,FALSE);

        case CBEM_SETITEMW:
            return COMBOEX_SetItem(hwnd,wParam,lParam,TRUE);

        case CB_DELETESTRING:
        case CB_FINDSTRINGEXACT:
        case CB_GETCOUNT:
        case CB_GETCURSEL:
        case CB_GETDROPPEDCONTROLRECT:
        case CB_GETDROPPEDSTATE:
        case CB_GETITEMDATA:
        case CB_GETITEMHEIGHT:
        case CB_GETLBTEXT:
        case CB_GETLBTEXTLEN:
        case CB_GETEXTENDEDUI:
        case CB_LIMITTEXT:
        case CB_RESETCONTENT:
        case CB_SELECTSTRING:
        case CB_SETCURSEL:
        case CB_SETDROPPEDWIDTH:
        case CB_SETEXTENDEDUI:
        case CB_SETITEMDATA:
        case CB_SETITEMHEIGHT:
        case CB_SHOWDROPDOWN:
            return COMBOEX_Forward (hwnd, uMsg, wParam, lParam);


        case WM_CREATE:
            return COMBOEX_Create (hwnd, wParam, lParam);

        case WM_DESTROY:
            return COMBOEX_Destroy (hwnd, wParam, lParam);

        case WM_SIZE:
            return COMBOEX_Size (hwnd, wParam, lParam);

        default:
//          if (uMsg >= WM_USER)
//              ERR (comboex, "unknown msg %04x wp=%08x lp=%08lx\n",
//                   uMsg, wParam, lParam);
            return defComCtl32ProcA (hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


VOID
COMBOEX_Register (VOID)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)COMBOEX_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(COMBOEX_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = WC_COMBOBOXEXA;

    RegisterClassA (&wndClass);
}


VOID
COMBOEX_Unregister (VOID)
{
    UnregisterClassA (WC_COMBOBOXEXA, (HINSTANCE)NULL);
}

