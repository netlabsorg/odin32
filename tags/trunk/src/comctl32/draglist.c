/*
 * Drag List control
 *
 * Copyright 1999 Eric Kohl
 *
 * NOTES
 *   This is just a dummy control. An author is needed! Any volunteers?
 *   I will only improve this control once in a while.
 *     Eric <ekohl@abo.rhein-zeitung.de>
 *
 * TODO:
 *   - Everything.
 */

/* WINE 991212 level */

#include "commctrl.h"
#include <misc.h>

static DWORD dwLastScrollTime = 0;


BOOL WINAPI MakeDragList (HWND hwndLB)
{
    dprintf(("COMCTL32: MakeDragList - empty stub!"));

    return FALSE;
}


VOID WINAPI DrawInsert (HWND hwndParent, HWND hwndLB, INT nItem)
{
    dprintf(("COMCTL32: DrawInsert - empty stub!"));
}


INT WINAPI LBItemFromPt (HWND hwndLB, POINT pt, BOOL bAutoScroll)
{
    RECT rcClient;
    INT nIndex;
    DWORD dwScrollTime;

    dprintf(("COMCTL32: LBItemFromPt"));

    ScreenToClient (hwndLB, &pt);
    GetClientRect (hwndLB, &rcClient);
    nIndex = (INT)SendMessageA (hwndLB, LB_GETTOPINDEX, 0, 0);

    if (PtInRect (&rcClient, pt))
    {
        /* point is inside -- get the item index */
        while (TRUE)
        {
            if (SendMessageA (hwndLB, LB_GETITEMRECT, nIndex, (LPARAM)&rcClient) == LB_ERR)
                return -1;

            if (PtInRect (&rcClient, pt))
                return nIndex;

            nIndex++;
        }
    }
    else
    {
        /* point is outside */
        if (!bAutoScroll)
            return -1;

        if ((pt.x > rcClient.right) || (pt.x < rcClient.left))
            return -1;

        if (pt.y < 0)
            nIndex--;
        else
            nIndex++;

        dwScrollTime = GetTickCount ();

        if ((dwScrollTime - dwLastScrollTime) < 200)
            return -1;

        dwLastScrollTime = dwScrollTime;

        SendMessageA (hwndLB, LB_SETTOPINDEX, (WPARAM)nIndex, 0);
    }

    return -1;
}


static LRESULT CALLBACK
DRAGLIST_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    return FALSE;
}
