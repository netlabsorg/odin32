/*
 * Flat Scrollbar control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1998 Alex Priem
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 */

#include <string.h>
#include "winbase.h"
#include "winerror.h"
#include "commctrl.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(commctrl);

typedef struct
{
    DWORD dwDummy;  /* just to keep the compiler happy ;-) */
} FLATSB_INFO, *LPFLATSB_INFO;

#define FlatSB_GetInfoPtr(hwnd) ((FLATSB_INFO*)GetWindowLongA (hwnd, 0))


/***********************************************************************
 *		InitializeFlatSB (COMCTL32.@)
 *
 *	returns nonzero if successful, zero otherwise
 *
 */
BOOL WINAPI InitializeFlatSB(HWND hwnd)
{
    TRACE("[%p]\n", hwnd);
    FIXME("stub\n");
    return FALSE;
}

/***********************************************************************
 *		UninitializeFlatSB (COMCTL32.@)
 *
 *	returns:
 *	E_FAIL		if one of the scroll bars is currently in use
 *	S_FALSE		if InitializeFlatSB was never called on this hwnd
 *	S_OK		otherwise
 *
 */
HRESULT WINAPI UninitializeFlatSB(HWND hwnd)
{
    TRACE("[%p]\n", hwnd);
    FIXME("stub\n");
    return S_FALSE;
}

/***********************************************************************
 *		FlatSB_GetScrollProp (COMCTL32.@)
 *
 *	Returns nonzero if successful, or zero otherwise. If index is WSB_PROP_HSTYLE,
 *	the return is nonzero if InitializeFlatSB has been called for this window, or
 *	zero otherwise.
 *
 */
BOOL WINAPI
FlatSB_GetScrollProp(HWND hwnd, INT propIndex, LPINT prop)
{
    TRACE("[%p] propIndex=%d\n", hwnd, propIndex);
    FIXME("stub\n");
    return FALSE;
}

/***********************************************************************
 *		FlatSB_SetScrollProp (COMCTL32.@)
 */
BOOL WINAPI
FlatSB_SetScrollProp(HWND hwnd, UINT index, INT newValue, BOOL flag)
{
    TRACE("[%p] index=%u newValue=%d flag=%d\n", hwnd, index, newValue, flag);
    FIXME("stub\n");
    return FALSE;
}

/***********************************************************************
 * 	From the Microsoft docs:
 *	"If flat scroll bars haven't been initialized for the
 *	window, the flat scroll bar APIs will defer to the corresponding
 *	standard APIs.  This allows the developer to turn flat scroll
 *	bars on and off without having to write conditional code."
 *
 *	So, if we just call the standard functions until we implement
 *	the flat scroll bar functions, flat scroll bars will show up and
 *	behave properly, as though they had simply not been setup to
 *	have flat properties.
 *
 *	Susan <sfarley@codeweavers.com>
 *
 */

/***********************************************************************
 *		FlatSB_EnableScrollBar (COMCTL32.@)
 */
BOOL WINAPI
FlatSB_EnableScrollBar(HWND hwnd, int nBar, UINT flags)
{
    return EnableScrollBar(hwnd, nBar, flags);
}

/***********************************************************************
 *		FlatSB_ShowScrollBar (COMCTL32.@)
 */
BOOL WINAPI
FlatSB_ShowScrollBar(HWND hwnd, int nBar, BOOL fShow)
{
    return ShowScrollBar(hwnd, nBar, fShow);
}

/***********************************************************************
 *		FlatSB_GetScrollRange (COMCTL32.@)
 */
BOOL WINAPI
FlatSB_GetScrollRange(HWND hwnd, int nBar, LPINT min, LPINT max)
{
    return GetScrollRange(hwnd, nBar, min, max);
}

/***********************************************************************
 *		FlatSB_GetScrollInfo (COMCTL32.@)
 */
BOOL WINAPI
FlatSB_GetScrollInfo(HWND hwnd, int nBar, LPSCROLLINFO info)
{
    return GetScrollInfo(hwnd, nBar, info);
}

/***********************************************************************
 *		FlatSB_GetScrollPos (COMCTL32.@)
 */
INT WINAPI
FlatSB_GetScrollPos(HWND hwnd, int nBar)
{
    return GetScrollPos(hwnd, nBar);
}

/***********************************************************************
 *		FlatSB_SetScrollPos (COMCTL32.@)
 */
INT WINAPI
FlatSB_SetScrollPos(HWND hwnd, int nBar, INT pos, BOOL bRedraw)
{
    return SetScrollPos(hwnd, nBar, pos, bRedraw);
}

/***********************************************************************
 *		FlatSB_SetScrollInfo (COMCTL32.@)
 */
INT WINAPI
FlatSB_SetScrollInfo(HWND hwnd, int nBar, LPSCROLLINFO info, BOOL bRedraw)
{
    return SetScrollInfo(hwnd, nBar, info, bRedraw);
}

/***********************************************************************
 *		FlatSB_SetScrollRange (COMCTL32.@)
 */
INT WINAPI
FlatSB_SetScrollRange(HWND hwnd, int nBar, INT min, INT max, BOOL bRedraw)
{
    return SetScrollRange(hwnd, nBar, min, max, bRedraw);
}


static LRESULT
FlatSB_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACE("[%p] wParam=%04x lParam=%08lx\n", hwnd, wParam, lParam);
    return 0;
}


static LRESULT
FlatSB_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TRACE("[%p] wParam=%04x lParam=%08lx\n", hwnd, wParam, lParam);
    return 0;
}


static LRESULT WINAPI
FlatSB_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!FlatSB_GetInfoPtr(hwnd) && (uMsg != WM_CREATE))
	return DefWindowProcA( hwnd, uMsg, wParam, lParam );

    switch (uMsg)
    {
	case WM_CREATE:
	    return FlatSB_Create (hwnd, wParam, lParam);

	case WM_DESTROY:
	    return FlatSB_Destroy (hwnd, wParam, lParam);

	default:
	    if ((uMsg >= WM_USER) && (uMsg < WM_APP))
		ERR("unknown msg %04x wp=%08x lp=%08lx\n",
                    uMsg, wParam, lParam);
	    return DefWindowProcA (hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


VOID
FLATSB_Register (void)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)FlatSB_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(FLATSB_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = FLATSB_CLASSA;

    RegisterClassA (&wndClass);
}


VOID
FLATSB_Unregister (void)
{
    UnregisterClassA (FLATSB_CLASSA, NULL);
}
