/* $Id: nativefont.cpp,v 1.1 2000-02-23 17:09:44 cbratschi Exp $ */
/*
 * Native Font control
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
 */

#include "winbase.h"
#include "commctrl.h"
#include "nativefont.h"



#define NATIVEFONT_GetInfoPtr(hwnd) ((NATIVEFONT_INFO *)GetWindowLongA (hwnd, 0))




static LRESULT
NATIVEFONT_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    NATIVEFONT_INFO *infoPtr;

    /* allocate memory for info structure */
    infoPtr = (NATIVEFONT_INFO *)COMCTL32_Alloc (sizeof(NATIVEFONT_INFO));
    SetWindowLongA (hwnd, 0, (DWORD)infoPtr);


    /* initialize info structure */


    return 0;
}


static LRESULT
NATIVEFONT_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    NATIVEFONT_INFO *infoPtr = NATIVEFONT_GetInfoPtr (hwnd);




    /* free comboex info data */
    COMCTL32_Free (infoPtr);

    return 0;
}



static LRESULT WINAPI
NATIVEFONT_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

        case WM_CREATE:
            return NATIVEFONT_Create (hwnd, wParam, lParam);

        case WM_DESTROY:
            return NATIVEFONT_Destroy (hwnd, wParam, lParam);

        default:
//          ERR (nativefont, "unknown msg %04x wp=%08x lp=%08lx\n",
//                   uMsg, wParam, lParam);
            return DefWindowProcA (hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


VOID
NATIVEFONT_Register (VOID)
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA (WC_NATIVEFONTCTLA)) return;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)NATIVEFONT_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(NATIVEFONT_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = WC_NATIVEFONTCTLA;

    RegisterClassA (&wndClass);
}


VOID
NATIVEFONT_Unregister (VOID)
{
    if (GlobalFindAtomA (WC_NATIVEFONTCTLA))
        UnregisterClassA (WC_NATIVEFONTCTLA, (HINSTANCE)NULL);
}

