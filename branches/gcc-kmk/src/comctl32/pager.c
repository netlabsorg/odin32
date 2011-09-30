/*
 * Pager control
 *
 * Copyright 1998, 1999 Eric Kohl
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
 *    Tested primarily with the controlspy Pager application.
 *       Susan Farley (susan@codeweavers.com)
 *
 * TODO:
 *    Implement repetitive button press.
 *    Adjust arrow size relative to size of button.
 *    Allow border size changes.
 *    Implement drag and drop style.
 */

#include <string.h>
#include "winbase.h"
#include "commctrl.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(pager);

typedef struct
{
    HWND   hwndChild;  /* handle of the contained wnd */
    BOOL   bNoResize;  /* set when created with CCS_NORESIZE */
    COLORREF clrBk;    /* background color */
    INT    nBorder;    /* border size for the control */
    INT    nButtonSize;/* size of the pager btns */
    INT    nPos;       /* scroll position */
    INT    nWidth;     /* from child wnd's response to PGN_CALCSIZE */
    INT    nHeight;    /* from child wnd's response to PGN_CALCSIZE */
    BOOL   bForward;   /* forward WM_MOUSEMOVE msgs to the contained wnd */
    BOOL   bCapture;   /* we have captured the mouse  */
    INT    TLbtnState; /* state of top or left btn */
    INT    BRbtnState; /* state of bottom or right btn */
    INT    direction;  /* direction of the scroll, (e.g. PGF_SCROLLUP) */
} PAGER_INFO;

#define PAGER_GetInfoPtr(hwnd) ((PAGER_INFO *)GetWindowLongA(hwnd, 0))
#define PAGER_IsHorizontal(hwnd) ((GetWindowLongA (hwnd, GWL_STYLE) & PGS_HORZ))

#define MIN_ARROW_WIDTH  8
#define MIN_ARROW_HEIGHT 5

#define TIMERID1         1
#define TIMERID2         2
#define INITIAL_DELAY    500
#define REPEAT_DELAY     50

/* the horizontal arrows are:
 *
 * 01234    01234
 * 1  *      *
 * 2 **      **
 * 3***      ***
 * 4***      ***
 * 5 **      **
 * 6  *      *
 * 7
 *
 */
static void
PAGER_DrawHorzArrow (HDC hdc, RECT r, INT colorRef, BOOL left)
{
    INT x, y, w, h;
    HPEN hPen, hOldPen;

    w = r.right - r.left + 1;
    h = r.bottom - r.top + 1;
    if ((h < MIN_ARROW_WIDTH) || (w < MIN_ARROW_HEIGHT))
        return;  /* refuse to draw partial arrow */

    if (!(hPen = CreatePen( PS_SOLID, 1, GetSysColor( colorRef )))) return;
    hOldPen = SelectObject ( hdc, hPen );
    if (left)
    {
        x = r.left + ((w - MIN_ARROW_HEIGHT) / 2) + 3;
        y = r.top + ((h - MIN_ARROW_WIDTH) / 2) + 1;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x--, y+5); y++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x--, y+3); y++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x, y+1);
    }
    else
    {
        x = r.left + ((w - MIN_ARROW_HEIGHT) / 2) + 1;
        y = r.top + ((h - MIN_ARROW_WIDTH) / 2) + 1;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x++, y+5); y++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x++, y+3); y++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x, y+1);
    }

    SelectObject( hdc, hOldPen );
    DeleteObject( hPen );
}

/* the vertical arrows are:
 *
 * 01234567    01234567
 * 1******        **
 * 2 ****        ****
 * 3  **        ******
 * 4
 *
 */
static void
PAGER_DrawVertArrow (HDC hdc, RECT r, INT colorRef, BOOL up)
{
    INT x, y, w, h;
    HPEN hPen, hOldPen;

    w = r.right - r.left + 1;
    h = r.bottom - r.top + 1;
    if ((h < MIN_ARROW_WIDTH) || (w < MIN_ARROW_HEIGHT))
        return;  /* refuse to draw partial arrow */

    if (!(hPen = CreatePen( PS_SOLID, 1, GetSysColor( colorRef )))) return;
    hOldPen = SelectObject ( hdc, hPen );
    if (up)
    {
        x = r.left + ((w - MIN_ARROW_HEIGHT) / 2) + 1;
        y = r.top + ((h - MIN_ARROW_WIDTH) / 2) + 3;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x+5, y--); x++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x+3, y--); x++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x+1, y);
    }
    else
    {
        x = r.left + ((w - MIN_ARROW_HEIGHT) / 2) + 1;
        y = r.top + ((h - MIN_ARROW_WIDTH) / 2) + 1;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x+5, y++); x++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x+3, y++); x++;
        MoveToEx (hdc, x, y, NULL);
        LineTo (hdc, x+1, y);
    }

    SelectObject( hdc, hOldPen );
    DeleteObject( hPen );
}

static void
PAGER_DrawButton(HDC hdc, COLORREF clrBk, RECT arrowRect,
                 BOOL horz, BOOL topLeft, INT btnState)
{
    HBRUSH   hBrush, hOldBrush;
    RECT     rc = arrowRect;

    if (!btnState) /* PGF_INVISIBLE */
        return;

    if ((rc.right - rc.left <= 0) || (rc.bottom - rc.top <= 0))
        return;

    hBrush = CreateSolidBrush(clrBk);
    hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    FillRect(hdc, &rc, hBrush);

    if (btnState == PGF_HOT)
    {
       DrawEdge( hdc, &rc, BDR_RAISEDINNER, BF_RECT);
       if (horz)
           PAGER_DrawHorzArrow(hdc, rc, COLOR_WINDOWFRAME, topLeft);
       else
           PAGER_DrawVertArrow(hdc, rc, COLOR_WINDOWFRAME, topLeft);
    }
    else if (btnState == PGF_NORMAL)
    {
       DrawEdge (hdc, &rc, BDR_OUTER, BF_FLAT);
       if (horz)
           PAGER_DrawHorzArrow(hdc, rc, COLOR_WINDOWFRAME, topLeft);
       else
           PAGER_DrawVertArrow(hdc, rc, COLOR_WINDOWFRAME, topLeft);
    }
    else if (btnState == PGF_DEPRESSED)
    {
       DrawEdge( hdc, &rc, BDR_SUNKENOUTER, BF_RECT);
       if (horz)
           PAGER_DrawHorzArrow(hdc, rc, COLOR_WINDOWFRAME, topLeft);
       else
           PAGER_DrawVertArrow(hdc, rc, COLOR_WINDOWFRAME, topLeft);
    }
    else if (btnState == PGF_GRAYED)
    {
       DrawEdge (hdc, &rc, BDR_OUTER, BF_FLAT);
       if (horz)
       {
           PAGER_DrawHorzArrow(hdc, rc, COLOR_3DHIGHLIGHT, topLeft);
           rc.left++, rc.top++; rc.right++, rc.bottom++;
           PAGER_DrawHorzArrow(hdc, rc, COLOR_3DSHADOW, topLeft);
       }
       else
       {
           PAGER_DrawVertArrow(hdc, rc, COLOR_3DHIGHLIGHT, topLeft);
           rc.left++, rc.top++; rc.right++, rc.bottom++;
           PAGER_DrawVertArrow(hdc, rc, COLOR_3DSHADOW, topLeft);
       }
    }

    SelectObject( hdc, hOldBrush );
    DeleteObject(hBrush);
}

static void PAGER_CaptureandTrack(PAGER_INFO *infoPtr, HWND hwnd)
{
    TRACKMOUSEEVENT trackinfo;

    TRACE("[%p] SetCapture\n", hwnd);
    SetCapture(hwnd);
    infoPtr->bCapture = TRUE;

    trackinfo.cbSize = sizeof(TRACKMOUSEEVENT);
    trackinfo.dwFlags = TME_QUERY;
    trackinfo.hwndTrack = hwnd;
    trackinfo.dwHoverTime = HOVER_DEFAULT;

    /* call _TrackMouseEvent to see if we are currently tracking for this hwnd */
    _TrackMouseEvent(&trackinfo);

    /* Make sure tracking is enabled so we receive a WM_MOUSELEAVE message */
    if(!(trackinfo.dwFlags & TME_LEAVE)) {
	trackinfo.dwFlags = TME_LEAVE; /* notify upon leaving */

	/* call TRACKMOUSEEVENT so we receive a WM_MOUSELEAVE message */
	/* and can properly deactivate the hot button */
	_TrackMouseEvent(&trackinfo);
    }
}


/* << PAGER_GetDropTarget >> */

static inline LRESULT
PAGER_ForwardMouse (HWND hwnd, WPARAM wParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    TRACE("[%p]\n", hwnd);

    infoPtr->bForward = (BOOL)wParam;

    return 0;
}

static inline LRESULT
PAGER_GetButtonState (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    LRESULT btnState = PGF_INVISIBLE;
    INT btn = (INT)lParam;
    TRACE("[%p]\n", hwnd);

    if (btn == PGB_TOPORLEFT)
        btnState = infoPtr->TLbtnState;
    else if (btn == PGB_BOTTOMORRIGHT)
        btnState = infoPtr->BRbtnState;

    return btnState;
}


static inline LRESULT
PAGER_GetPos(HWND hwnd)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    TRACE("[%p] returns %d\n", hwnd, infoPtr->nPos);
    return (LRESULT)infoPtr->nPos;
}

static inline LRESULT
PAGER_GetButtonSize(HWND hwnd)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    TRACE("[%p] returns %d\n", hwnd, infoPtr->nButtonSize);
    return (LRESULT)infoPtr->nButtonSize;
}

static inline LRESULT
PAGER_GetBorder(HWND hwnd)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    TRACE("[%p] returns %d\n", hwnd, infoPtr->nBorder);
    return (LRESULT)infoPtr->nBorder;
}

static inline LRESULT
PAGER_GetBkColor(HWND hwnd)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    TRACE("[%p] returns %06lx\n", hwnd, infoPtr->clrBk);
    return (LRESULT)infoPtr->clrBk;
}

static void
PAGER_CalcSize (HWND hwnd, INT* size, BOOL getWidth)
{
    NMPGCALCSIZE nmpgcs;
    ZeroMemory (&nmpgcs, sizeof (NMPGCALCSIZE));
    nmpgcs.hdr.hwndFrom = hwnd;
    nmpgcs.hdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
    nmpgcs.hdr.code = PGN_CALCSIZE;
    nmpgcs.dwFlag = getWidth ? PGF_CALCWIDTH : PGF_CALCHEIGHT;
    nmpgcs.iWidth = getWidth ? *size : 0;
    nmpgcs.iHeight = getWidth ? 0 : *size;
    SendMessageA (GetParent (hwnd), WM_NOTIFY,
                  (WPARAM)nmpgcs.hdr.idFrom, (LPARAM)&nmpgcs);

    *size = getWidth ? nmpgcs.iWidth : nmpgcs.iHeight;

    TRACE("[%p] PGN_CALCSIZE returns %s=%d\n", hwnd,
                  getWidth ? "width" : "height", *size);
}

static void
PAGER_PositionChildWnd(HWND hwnd, PAGER_INFO* infoPtr)
{
    if (infoPtr->hwndChild)
    {
        RECT rcClient;
        int nPos = infoPtr->nPos;

        /* compensate for a grayed btn, which will soon become invisible */
        if (infoPtr->TLbtnState == PGF_GRAYED)
            nPos += infoPtr->nButtonSize;

        GetClientRect(hwnd, &rcClient);

        if (PAGER_IsHorizontal(hwnd))
        {
            int wndSize = max(0, rcClient.right - rcClient.left);
            if (infoPtr->nWidth < wndSize)
                infoPtr->nWidth = wndSize;

            TRACE("[%p] SWP %dx%d at (%d,%d)\n", hwnd,
                         infoPtr->nWidth, infoPtr->nHeight,
                         -nPos, 0);
            SetWindowPos(infoPtr->hwndChild, 0,
                         -nPos, 0,
                         infoPtr->nWidth, infoPtr->nHeight,
                         SWP_NOZORDER);
        }
        else
        {
            int wndSize = max(0, rcClient.bottom - rcClient.top);
            if (infoPtr->nHeight < wndSize)
                infoPtr->nHeight = wndSize;

            TRACE("[%p] SWP %dx%d at (%d,%d)\n", hwnd,
                         infoPtr->nWidth, infoPtr->nHeight,
                         0, -nPos);
            SetWindowPos(infoPtr->hwndChild, 0,
                         0, -nPos,
                         infoPtr->nWidth, infoPtr->nHeight,
                         SWP_NOZORDER);
        }

        InvalidateRect(infoPtr->hwndChild, NULL, TRUE);
    }
}

static INT
PAGER_GetScrollRange(HWND hwnd, PAGER_INFO* infoPtr)
{
    INT scrollRange = 0;

    if (infoPtr->hwndChild)
    {
        INT wndSize, childSize;
        RECT wndRect;
        GetWindowRect(hwnd, &wndRect);

        if (PAGER_IsHorizontal(hwnd))
        {
            wndSize = wndRect.right - wndRect.left;
            PAGER_CalcSize(hwnd, &infoPtr->nWidth, TRUE);
            childSize = infoPtr->nWidth;
        }
        else
        {
            wndSize = wndRect.bottom - wndRect.top;
            PAGER_CalcSize(hwnd, &infoPtr->nHeight, FALSE);
            childSize = infoPtr->nHeight;
        }

        TRACE("childSize = %d,  wndSize = %d\n", childSize, wndSize);
        if (childSize > wndSize)
            scrollRange = childSize - wndSize + infoPtr->nButtonSize;
    }

    TRACE("[%p] returns %d\n", hwnd, scrollRange);
    return scrollRange;
}

static void
PAGER_GrayAndRestoreBtns(PAGER_INFO* infoPtr, INT scrollRange,
                         BOOL* needsResize, BOOL* needsRepaint)
{
    if (infoPtr->nPos > 0)
    {
        *needsResize |= !infoPtr->TLbtnState; /* PGF_INVISIBLE */
        if (infoPtr->TLbtnState != PGF_DEPRESSED)
            infoPtr->TLbtnState = PGF_NORMAL;
    }
    else
    {
        *needsRepaint |= (infoPtr->TLbtnState != PGF_GRAYED);
        infoPtr->TLbtnState = PGF_GRAYED;
    }

    if (scrollRange <= 0)
    {
        *needsRepaint |= (infoPtr->TLbtnState != PGF_GRAYED);
        infoPtr->TLbtnState = PGF_GRAYED;
        *needsRepaint |= (infoPtr->BRbtnState != PGF_GRAYED);
        infoPtr->BRbtnState = PGF_GRAYED;
    }
    else if (infoPtr->nPos < scrollRange)
    {
        *needsResize |= !infoPtr->BRbtnState; /* PGF_INVISIBLE */
        if (infoPtr->BRbtnState != PGF_DEPRESSED)
            infoPtr->BRbtnState = PGF_NORMAL;
    }
    else
    {
        *needsRepaint |= (infoPtr->BRbtnState != PGF_GRAYED);
        infoPtr->BRbtnState = PGF_GRAYED;
    }
}


static void
PAGER_NormalizeBtns(PAGER_INFO* infoPtr, BOOL* needsRepaint)
{
    if (infoPtr->TLbtnState & (PGF_HOT | PGF_DEPRESSED))
    {
        infoPtr->TLbtnState = PGF_NORMAL;
        *needsRepaint = TRUE;
    }

    if (infoPtr->BRbtnState & (PGF_HOT | PGF_DEPRESSED))
    {
        infoPtr->BRbtnState = PGF_NORMAL;
        *needsRepaint = TRUE;
    }
}

static void
PAGER_HideGrayBtns(PAGER_INFO* infoPtr, BOOL* needsResize)
{
    if (infoPtr->TLbtnState == PGF_GRAYED)
    {
        infoPtr->TLbtnState = PGF_INVISIBLE;
        *needsResize = TRUE;
    }

    if (infoPtr->BRbtnState == PGF_GRAYED)
    {
        infoPtr->BRbtnState = PGF_INVISIBLE;
        *needsResize = TRUE;
    }
}

static void
PAGER_UpdateBtns(HWND hwnd, PAGER_INFO *infoPtr,
                 INT scrollRange, BOOL hideGrayBtns)
{
    BOOL resizeClient = FALSE;
    BOOL repaintBtns = FALSE;

    if (scrollRange < 0)
        PAGER_NormalizeBtns(infoPtr, &repaintBtns);
    else
        PAGER_GrayAndRestoreBtns(infoPtr, scrollRange, &resizeClient, &repaintBtns);

    if (hideGrayBtns)
        PAGER_HideGrayBtns(infoPtr, &resizeClient);

    if (resizeClient) /* initiate NCCalcSize to resize client wnd */ {
        SetWindowPos(hwnd, 0,0,0,0,0,
                     SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE |
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (repaintBtns)
        SendMessageA(hwnd, WM_NCPAINT, 0, 0);
}

static LRESULT
PAGER_SetPos(HWND hwnd, INT newPos, BOOL fromBtnPress)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    INT scrollRange = PAGER_GetScrollRange(hwnd, infoPtr);
    INT oldPos = infoPtr->nPos;

    if ((scrollRange <= 0) || (newPos < 0))
        infoPtr->nPos = 0;
    else if (newPos > scrollRange)
        infoPtr->nPos = scrollRange;
    else
        infoPtr->nPos = newPos;

    TRACE("[%p] pos=%d, oldpos=%d\n", hwnd, infoPtr->nPos, oldPos);

    if (infoPtr->nPos != oldPos)
    {
        /* gray and restore btns, and if from WM_SETPOS, hide the gray btns */
        PAGER_UpdateBtns(hwnd, infoPtr, scrollRange, !fromBtnPress);
        PAGER_PositionChildWnd(hwnd, infoPtr);
    }

    return 0;
}

static LRESULT
PAGER_HandleWindowPosChanging(HWND hwnd, WPARAM wParam, WINDOWPOS *winpos)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);

    if (infoPtr->bNoResize && !(winpos->flags & SWP_NOSIZE))
    {
        /* don't let the app resize the nonscrollable dimension of a control
         * that was created with CCS_NORESIZE style
         * (i.e. height for a horizontal pager, or width for a vertical one) */

	/* except if the current dimension is 0 and app is setting for
	 * first time, then save amount as dimension. - GA 8/01 */

        if (PAGER_IsHorizontal(hwnd))
	    if (!infoPtr->nHeight && winpos->cy)
		infoPtr->nHeight = winpos->cy;
	    else
		winpos->cy = infoPtr->nHeight;
        else
	    if (!infoPtr->nWidth && winpos->cx)
		infoPtr->nWidth = winpos->cx;
	    else
		winpos->cx = infoPtr->nWidth;
	return 0;
    }

    DefWindowProcA (hwnd, WM_WINDOWPOSCHANGING, wParam, (LPARAM)winpos);

    return 1;
}

static INT
PAGER_SetFixedWidth(HWND hwnd, PAGER_INFO* infoPtr)
{
  /* Must set the non-scrollable dimension to be less than the full height/width
   * so that NCCalcSize is called.  The Msoft docs mention 3/4 factor for button
   * size, and experimentation shows that affect is almost right. */

    RECT wndRect;
    INT delta, h;
    GetWindowRect(hwnd, &wndRect);

    /* see what the app says for btn width */
    PAGER_CalcSize(hwnd, &infoPtr->nWidth, TRUE);

    if (infoPtr->bNoResize)
    {
        delta = wndRect.right - wndRect.left - infoPtr->nWidth;
        if (delta > infoPtr->nButtonSize)
            infoPtr->nWidth += 4 * infoPtr->nButtonSize / 3;
        else if (delta > 0)
            infoPtr->nWidth +=  infoPtr->nButtonSize / 3;
    }

    h = wndRect.bottom - wndRect.top + infoPtr->nButtonSize;

    TRACE("[%p] infoPtr->nWidth set to %d\n",
	       hwnd, infoPtr->nWidth);

    return h;
}

static INT
PAGER_SetFixedHeight(HWND hwnd, PAGER_INFO* infoPtr)
{
  /* Must set the non-scrollable dimension to be less than the full height/width
   * so that NCCalcSize is called.  The Msoft docs mention 3/4 factor for button
   * size, and experimentation shows that affect is almost right. */

    RECT wndRect;
    INT delta, w;
    GetWindowRect(hwnd, &wndRect);

    /* see what the app says for btn height */
    PAGER_CalcSize(hwnd, &infoPtr->nHeight, FALSE);

    if (infoPtr->bNoResize)
    {
        delta = wndRect.bottom - wndRect.top - infoPtr->nHeight;
        if (delta > infoPtr->nButtonSize)
            infoPtr->nHeight += infoPtr->nButtonSize;
        else if (delta > 0)
            infoPtr->nHeight +=  infoPtr->nButtonSize / 3;
    }

    w = wndRect.right - wndRect.left + infoPtr->nButtonSize;

    TRACE("[%p] infoPtr->nHeight set to %d\n",
	       hwnd, infoPtr->nHeight);

    return w;
}

/******************************************************************
 * For the PGM_RECALCSIZE message (but not the other uses in      *
 * this module), the native control does only the following:      *
 *                                                                *
 *    if (some condition)                                         *
 *          PostMessageA(hwnd, EM_FMTLINES, 0, 0);                *
 *    return DefWindowProcA(hwnd, PGM_RECALCSIZE, 0, 0);          *
 *                                                                *
 * When we figure out what the "some condition" is we will        *
 * implement that for the message processing.                     *
 ******************************************************************/

static LRESULT
PAGER_RecalcSize(HWND hwnd)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);

    TRACE("[%p]\n", hwnd);

    if (infoPtr->hwndChild)
    {
        INT scrollRange = PAGER_GetScrollRange(hwnd, infoPtr);

        if (scrollRange <= 0)
        {
            infoPtr->nPos = -1;
            PAGER_SetPos(hwnd, 0, FALSE);
        }
        else
        {
            PAGER_UpdateBtns(hwnd, infoPtr, scrollRange, TRUE);
            PAGER_PositionChildWnd(hwnd, infoPtr);
        }
    }

    return 1;
}


static LRESULT
PAGER_SetBkColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    COLORREF clrTemp = infoPtr->clrBk;

    infoPtr->clrBk = (COLORREF)lParam;
    TRACE("[%p] %06lx\n", hwnd, infoPtr->clrBk);

    /* the native control seems to do things this way */
    SetWindowPos(hwnd, 0,0,0,0,0,
		 SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE |
		 SWP_NOZORDER | SWP_NOACTIVATE);

    RedrawWindow(hwnd, 0, 0, RDW_ERASE | RDW_INVALIDATE);

    return (LRESULT)clrTemp;
}


static LRESULT
PAGER_SetBorder (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    INT nTemp = infoPtr->nBorder;

    infoPtr->nBorder = (INT)lParam;
    TRACE("[%p] %d\n", hwnd, infoPtr->nBorder);

    PAGER_RecalcSize(hwnd);

    return (LRESULT)nTemp;
}


static LRESULT
PAGER_SetButtonSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    INT nTemp = infoPtr->nButtonSize;

    infoPtr->nButtonSize = (INT)lParam;
    TRACE("[%p] %d\n", hwnd, infoPtr->nButtonSize);

    PAGER_RecalcSize(hwnd);

    return (LRESULT)nTemp;
}


static LRESULT
PAGER_SetChild (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    INT hw;

    infoPtr->hwndChild = IsWindow ((HWND)lParam) ? (HWND)lParam : 0;

    if (infoPtr->hwndChild)
    {
        TRACE("[%p] hwndChild=%p\n", hwnd, infoPtr->hwndChild);

        if (PAGER_IsHorizontal(hwnd)) {
            hw = PAGER_SetFixedHeight(hwnd, infoPtr);
	    /* adjust non-scrollable dimension to fit the child */
	    SetWindowPos(hwnd, 0, 0,0, hw, infoPtr->nHeight,
			 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER |
			 SWP_NOSIZE | SWP_NOACTIVATE);
	}
        else {
            hw = PAGER_SetFixedWidth(hwnd, infoPtr);
	    /* adjust non-scrollable dimension to fit the child */
	    SetWindowPos(hwnd, 0, 0,0, infoPtr->nWidth, hw,
			 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER |
			 SWP_NOSIZE | SWP_NOACTIVATE);
	}

        /* position child within the page scroller */
        SetWindowPos(infoPtr->hwndChild, HWND_TOP,
                     0,0,0,0,
                     SWP_SHOWWINDOW | SWP_NOSIZE);  /* native is 0 */

        infoPtr->nPos = -1;
        PAGER_SetPos(hwnd, 0, FALSE);
    }

    return 0;
}

static void
PAGER_Scroll(HWND hwnd, INT dir)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    NMPGSCROLL nmpgScroll;
    RECT rcWnd;

    if (infoPtr->hwndChild)
    {
        ZeroMemory (&nmpgScroll, sizeof (NMPGSCROLL));
        nmpgScroll.hdr.hwndFrom = hwnd;
        nmpgScroll.hdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
        nmpgScroll.hdr.code = PGN_SCROLL;

        GetWindowRect(hwnd, &rcWnd);
        GetClientRect(hwnd, &nmpgScroll.rcParent);
        nmpgScroll.iXpos = nmpgScroll.iYpos = 0;
        nmpgScroll.iDir = dir;

        if (PAGER_IsHorizontal(hwnd))
        {
            nmpgScroll.iScroll = rcWnd.right - rcWnd.left;
            nmpgScroll.iXpos = infoPtr->nPos;
        }
        else
        {
            nmpgScroll.iScroll = rcWnd.bottom - rcWnd.top;
            nmpgScroll.iYpos = infoPtr->nPos;
        }
        nmpgScroll.iScroll -= 2*infoPtr->nButtonSize;

        SendMessageA (GetParent(hwnd), WM_NOTIFY,
                    (WPARAM)nmpgScroll.hdr.idFrom, (LPARAM)&nmpgScroll);

        TRACE("[%p] PGN_SCROLL returns iScroll=%d\n", hwnd, nmpgScroll.iScroll);

        if (nmpgScroll.iScroll > 0)
        {
            infoPtr->direction = dir;

            if (dir == PGF_SCROLLLEFT || dir == PGF_SCROLLUP)
                PAGER_SetPos(hwnd, infoPtr->nPos - nmpgScroll.iScroll, TRUE);
            else
                PAGER_SetPos(hwnd, infoPtr->nPos + nmpgScroll.iScroll, TRUE);
        }
        else
            infoPtr->direction = -1;
    }
}

static LRESULT
PAGER_FmtLines(HWND hwnd)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);

    /* initiate NCCalcSize to resize client wnd and get size */
    SetWindowPos(hwnd, 0, 0,0,0,0,
		 SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE |
		 SWP_NOZORDER | SWP_NOACTIVATE);

    SetWindowPos(infoPtr->hwndChild, 0,
		 0,0,infoPtr->nWidth,infoPtr->nHeight,
		 0);

    return DefWindowProcA (hwnd, EM_FMTLINES, 0, 0);
}

static LRESULT
PAGER_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr;
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);

    /* allocate memory for info structure */
    infoPtr = (PAGER_INFO *)COMCTL32_Alloc (sizeof(PAGER_INFO));
    SetWindowLongA (hwnd, 0, (DWORD)infoPtr);

    /* set default settings */
    infoPtr->hwndChild = NULL;
    infoPtr->bNoResize = dwStyle & CCS_NORESIZE;
    infoPtr->clrBk = GetSysColor(COLOR_BTNFACE);
    infoPtr->nBorder = 0;
    infoPtr->nButtonSize = 12;
    infoPtr->nPos = 0;
    infoPtr->nWidth = 0;
    infoPtr->nHeight = 0;
    infoPtr->bForward = FALSE;
    infoPtr->bCapture = FALSE;
    infoPtr->TLbtnState = PGF_INVISIBLE;
    infoPtr->BRbtnState = PGF_INVISIBLE;
    infoPtr->direction = -1;

    if (dwStyle & PGS_DRAGNDROP)
        FIXME("[%p] Drag and Drop style is not implemented yet.\n", hwnd);
    /*
	 * If neither horizontal nor vertical style specified, default to vertical.
	 * This is probably not necessary, since the style may be set later on as
	 * the control is initialized, but just in case it isn't, set it here.
	 */
    if (!(dwStyle & PGS_HORZ) && !(dwStyle & PGS_VERT))
    {
        dwStyle |= PGS_VERT;
        SetWindowLongA(hwnd, GWL_STYLE, dwStyle);
    }

    return 0;
}


static LRESULT
PAGER_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    /* free pager info data */
    COMCTL32_Free (infoPtr);
    SetWindowLongA (hwnd, 0, 0);
    return 0;
}

static LRESULT
PAGER_NCCalcSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    LPRECT lpRect = (LPRECT)lParam;
    RECT rcChildw, rcmyw, wnrc, ltrc, rbrc;
    POINT cursor;
    BOOL resizeClient = FALSE;
    BOOL repaintBtns = FALSE;
    INT scrollRange;

    /*
     * lParam points to a RECT struct.  On entry, the struct
     * contains the proposed wnd rectangle for the window.
     * On exit, the struct should contain the screen
     * coordinates of the corresponding window's client area.
     */

    DefWindowProcA (hwnd, WM_NCCALCSIZE, wParam, lParam);

    TRACE("orig rect=(%ld,%ld)-(%ld,%ld)\n",
	  lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);

    if (PAGER_IsHorizontal(hwnd))
    {
	infoPtr->nWidth = lpRect->right - lpRect->left;
	PAGER_CalcSize (hwnd, &infoPtr->nWidth, TRUE);
	GetWindowRect (infoPtr->hwndChild, &rcChildw);
	MapWindowPoints (0, hwnd, (LPPOINT)&rcChildw, 2);
	GetCursorPos (&cursor);
	GetWindowRect (hwnd, &rcmyw);

	/* Reset buttons and hide any grey ones */
	scrollRange = infoPtr->nWidth - (rcmyw.right - rcmyw.left);

	TRACE("nPos=%d, scrollrange=%d, nHeigth=%d, myw=(%ld,%ld)-(%ld,%ld), cursor=(%ld,%ld)\n",
	      infoPtr->nPos, scrollRange, infoPtr->nHeight,
	      rcmyw.left, rcmyw.top,
	      rcmyw.right, rcmyw.bottom,
	      cursor.x, cursor.y);
	PAGER_GrayAndRestoreBtns(infoPtr, scrollRange, &resizeClient, &repaintBtns);
	PAGER_HideGrayBtns(infoPtr, &resizeClient);

	if (PtInRect (&rcmyw, cursor)) {
	    GetWindowRect (hwnd, &wnrc);
	    ltrc = wnrc;
	    ltrc.right = ltrc.left + infoPtr->nButtonSize;
	    rbrc = wnrc;
	    rbrc.left = rbrc.right - infoPtr->nButtonSize;
	    TRACE("horz lt rect=(%ld,%ld)-(%ld,%ld), rb rect=(%ld,%ld)-(%ld,%ld)\n",
		  ltrc.left, ltrc.top, ltrc.right, ltrc.bottom,
		  rbrc.left, rbrc.top, rbrc.right, rbrc.bottom);
	    if (PtInRect (&ltrc, cursor) && infoPtr->TLbtnState)
		RedrawWindow (hwnd, 0, 0, RDW_INVALIDATE | RDW_ERASE);
	    if (PtInRect (&rbrc, cursor) && infoPtr->BRbtnState)
		RedrawWindow (hwnd, 0, 0, RDW_INVALIDATE | RDW_ERASE);
	}
	if (infoPtr->TLbtnState && (lpRect->left + infoPtr->nButtonSize < lpRect->right))
	    lpRect->left += infoPtr->nButtonSize;
	if (infoPtr->BRbtnState && (lpRect->right - infoPtr->nButtonSize > lpRect->left))
	    lpRect->right -= infoPtr->nButtonSize;
    }
    else
    {
	/* native does: (from trace of IE4 opening "Favorites" frame)
	 *        DefWindowProc
	 *        WM_NOITFY  PGN_CALCSIZE w/ dwFlag=2
	 *        GetWindowRect (child, &rc)
         *        MapWindowPoints (0, syspager, &rc, 2)
         *        GetCursorPos( &cur )
         *        GetWindowRect (syspager, &rc2)
         *        PtInRect (&rc2, cur.x, cur.y) rtns 0
         *        returns with rect empty
	 */
	infoPtr->nHeight = lpRect->bottom - lpRect->top;
	PAGER_CalcSize (hwnd, &infoPtr->nHeight, FALSE);
	GetWindowRect (infoPtr->hwndChild, &rcChildw);
	MapWindowPoints (0, hwnd, (LPPOINT)&rcChildw, 2);
	GetCursorPos (&cursor);
	GetWindowRect (hwnd, &rcmyw);

	/* Reset buttons and hide any grey ones */
	scrollRange = infoPtr->nHeight - (rcmyw.bottom - rcmyw.top);

	TRACE("nPos=%d, scrollrange=%d, nHeigth=%d, myw=(%ld,%ld)-(%ld,%ld), cursor=(%ld,%ld)\n",
	      infoPtr->nPos, scrollRange, infoPtr->nHeight,
	      rcmyw.left, rcmyw.top,
	      rcmyw.right, rcmyw.bottom,
	      cursor.x, cursor.y);
	PAGER_GrayAndRestoreBtns(infoPtr, scrollRange, &resizeClient, &repaintBtns);
	PAGER_HideGrayBtns(infoPtr, &resizeClient);

	if (PtInRect (&rcmyw, cursor)) {

	    /* native does:
	     *    GetWindowRect(pager, &rc)
	     *    PtInRect(btn-left????, cur.x, cur.y)
	     *    if true -> ???
	     *    PtInRect(btn-right????, cur.x, cur.y)
	     *    if true
	     *      RedrawWindow(pager, 0, 0, 5)
	     *      return TRUE
	     */

	    GetWindowRect (hwnd, &wnrc);
	    ltrc = wnrc;
	    ltrc.right = ltrc.left + infoPtr->nButtonSize;
	    rbrc = wnrc;
	    rbrc.left = rbrc.right - infoPtr->nButtonSize;
	    TRACE("vert lt rect=(%ld,%ld)-(%ld,%ld), rb rect=(%ld,%ld)-(%ld,%ld)\n",
		  ltrc.left, ltrc.top, ltrc.right, ltrc.bottom,
		  rbrc.left, rbrc.top, rbrc.right, rbrc.bottom);
	    if (PtInRect (&ltrc, cursor) && infoPtr->TLbtnState)
		RedrawWindow (hwnd, 0, 0, RDW_INVALIDATE | RDW_ERASE);
	    if (PtInRect (&rbrc, cursor) && infoPtr->BRbtnState)
		RedrawWindow (hwnd, 0, 0, RDW_INVALIDATE | RDW_ERASE);
	}
	if (infoPtr->TLbtnState && (lpRect->top + infoPtr->nButtonSize < lpRect->bottom))
	    lpRect->top += infoPtr->nButtonSize;
	if (infoPtr->BRbtnState && (lpRect->bottom - infoPtr->nButtonSize > lpRect->top))
	    lpRect->bottom -= infoPtr->nButtonSize;
    }

    TRACE("[%p] client rect set to %ldx%ld at (%ld,%ld) BtnState[%d,%d]\n",
	  hwnd, lpRect->right-lpRect->left, lpRect->bottom-lpRect->top,
	  lpRect->left, lpRect->top,
	  infoPtr->TLbtnState, infoPtr->BRbtnState);

    return 0;
}

static LRESULT
PAGER_NCPaint (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO* infoPtr = PAGER_GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    RECT rcWindow, rcBottomRight, rcTopLeft;
    HDC hdc;
    BOOL bHorizontal = PAGER_IsHorizontal(hwnd);

    if (dwStyle & WS_MINIMIZE)
        return 0;

    DefWindowProcA (hwnd, WM_NCPAINT, wParam, lParam);

    if (!(hdc = GetDCEx (hwnd, 0, DCX_USESTYLE | DCX_WINDOW)))
        return 0;

    GetWindowRect (hwnd, &rcWindow);
    OffsetRect (&rcWindow, -rcWindow.left, -rcWindow.top);

    rcTopLeft = rcBottomRight = rcWindow;
    if (bHorizontal)
    {
        rcTopLeft.right = rcTopLeft.left + infoPtr->nButtonSize;
        rcBottomRight.left = rcBottomRight.right - infoPtr->nButtonSize;
    }
    else
    {
        rcTopLeft.bottom = rcTopLeft.top + infoPtr->nButtonSize;
        rcBottomRight.top = rcBottomRight.bottom - infoPtr->nButtonSize;
    }

    PAGER_DrawButton(hdc, infoPtr->clrBk, rcTopLeft,
                     bHorizontal, TRUE, infoPtr->TLbtnState);
    PAGER_DrawButton(hdc, infoPtr->clrBk, rcBottomRight,
                     bHorizontal, FALSE, infoPtr->BRbtnState);

    ReleaseDC( hwnd, hdc );
    return 0;
}

static INT
PAGER_HitTest (HWND hwnd, LPPOINT pt)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    RECT clientRect;
    BOOL bHorizontal = PAGER_IsHorizontal(hwnd);

    GetClientRect (hwnd, &clientRect);

    if (PtInRect(&clientRect, *pt))
    {
        TRACE("HTCLIENT\n");
        return HTCLIENT;
    }

    if (infoPtr->TLbtnState && infoPtr->TLbtnState != PGF_GRAYED)
    {
        if (bHorizontal)
        {
            if (pt->x < clientRect.left)
            {
                TRACE("HTLEFT\n");
                return HTLEFT;
            }
        }
        else
        {
            if (pt->y < clientRect.top)
            {
                TRACE("HTTOP\n");
                return HTTOP;
            }
        }
    }

    if (infoPtr->BRbtnState && infoPtr->BRbtnState != PGF_GRAYED)
    {
        if (bHorizontal)
        {
            if (pt->x > clientRect.right)
            {
                TRACE("HTRIGHT\n");
                return HTRIGHT;
            }
        }
        else
        {
            if (pt->y > clientRect.bottom)
            {
                TRACE("HTBOTTOM\n");
                return HTBOTTOM;
            }
        }
    }

    TRACE("HTNOWHERE\n");
    return HTNOWHERE;
}

static LRESULT
PAGER_NCHitTest (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt;

    pt.x = SLOWORD(lParam);
    pt.y = SHIWORD(lParam);

    ScreenToClient (hwnd, &pt);
    return PAGER_HitTest(hwnd, &pt);
}

static LRESULT
PAGER_SetCursor( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    BOOL notCaptured = FALSE;

    switch(LOWORD(lParam))
    {
        case HTLEFT:
        case HTTOP:
            if ((notCaptured = infoPtr->TLbtnState != PGF_HOT))
                infoPtr->TLbtnState = PGF_HOT;
            break;
        case HTRIGHT:
        case HTBOTTOM:
            if ((notCaptured = infoPtr->BRbtnState != PGF_HOT))
               infoPtr->BRbtnState = PGF_HOT;
            break;
        default:
            return FALSE;
    }

    if (notCaptured)
    {
	PAGER_CaptureandTrack(infoPtr, hwnd);

        SendMessageA(hwnd, WM_NCPAINT, 0, 0);
    }

    return TRUE;
}

static LRESULT
PAGER_MouseLeave (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);

    KillTimer (hwnd, TIMERID1);
    KillTimer (hwnd, TIMERID2);

    TRACE("[%p] ReleaseCapture\n", hwnd);
    ReleaseCapture();
    infoPtr->bCapture = FALSE;

    /* Notify parent of released mouse capture */
    {
        NMHDR nmhdr;
        ZeroMemory (&nmhdr, sizeof (NMHDR));
        nmhdr.hwndFrom = hwnd;
        nmhdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
        nmhdr.code = NM_RELEASEDCAPTURE;
        SendMessageA (GetParent(hwnd), WM_NOTIFY,
                        (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
    }

    /* make HOT btns NORMAL and hide gray btns */
    PAGER_UpdateBtns(hwnd, infoPtr, -1, TRUE);

    return TRUE;
}

static LRESULT
PAGER_MouseMove (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    POINT clpt, pt;
    RECT wnrect, TLbtnrect, BRbtnrect, *btnrect = NULL;
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    BOOL topLeft = FALSE;
    INT btnstate = 0;
    INT hit;
    HDC hdc;

    pt.x = SLOWORD(lParam);
    pt.y = SHIWORD(lParam);

    TRACE("[%p] to (%ld,%ld)\n", hwnd, pt.x, pt.y);
    ClientToScreen(hwnd, &pt);
    GetWindowRect(hwnd, &wnrect);
    if (PtInRect(&wnrect, pt)) {
	TLbtnrect = wnrect;
	BRbtnrect = wnrect;
	if (dwStyle & PGS_HORZ) {
	    TLbtnrect.right = TLbtnrect.left + infoPtr->nButtonSize;
	    BRbtnrect.left = BRbtnrect.right - infoPtr->nButtonSize;
	}
	else {
	    TLbtnrect.bottom = TLbtnrect.top + infoPtr->nButtonSize;
	    BRbtnrect.top = BRbtnrect.bottom - infoPtr->nButtonSize;
	}

	clpt = pt;
	MapWindowPoints(0, hwnd, &clpt, 1);
	hit = PAGER_HitTest(hwnd, &clpt);
	if (hit == HTLEFT || hit == HTTOP) {
	    topLeft = TRUE;
	    btnrect = &TLbtnrect;
	    infoPtr->TLbtnState = PGF_DEPRESSED;
	    btnstate = infoPtr->TLbtnState;
	}
	else if (hit == HTRIGHT || hit == HTBOTTOM) {
	    topLeft = FALSE;
	    btnrect = &BRbtnrect;
	    infoPtr->BRbtnState = PGF_DEPRESSED;
	    btnstate = infoPtr->BRbtnState;
	}

	/* If in one of the buttons the capture and draw buttons */
	if (btnrect) {
	    TRACE("[%p] draw btn (%ld,%ld)-(%ld,%ld), Capture %s, style %08lx\n",
		  hwnd, btnrect->left, btnrect->top,
		  btnrect->right, btnrect->bottom,
		  (infoPtr->bCapture) ? "TRUE" : "FALSE",
		  dwStyle);
	    if (!infoPtr->bCapture)
		PAGER_CaptureandTrack(infoPtr, hwnd);
	    if (dwStyle & PGS_AUTOSCROLL)
		SetTimer(hwnd, TIMERID1, 0x3e, 0);
	    MapWindowPoints(0, hwnd, (LPPOINT)btnrect, 2);
	    hdc = GetWindowDC(hwnd);
	    /* OffsetRect(wnrect, 0 | 1, 0 | 1) */
	    PAGER_DrawButton(hdc, infoPtr->clrBk, *btnrect,
			     PAGER_IsHorizontal(hwnd), topLeft, btnstate);
	    ReleaseDC(hwnd, hdc);
	    return DefWindowProcA (hwnd, WM_MOUSEMOVE, wParam, lParam);
	}
    }

    /* If we think we are captured, then do release */
    if (infoPtr->bCapture) {
	infoPtr->bCapture = FALSE;

        if (GetCapture() == hwnd) {
	    ReleaseCapture();
	    /* Notify parent of released mouse capture */
	    {
		NMHDR nmhdr;
		ZeroMemory (&nmhdr, sizeof (NMHDR));
		nmhdr.hwndFrom = hwnd;
		nmhdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
		nmhdr.code = NM_RELEASEDCAPTURE;
		SendMessageA (GetParent(hwnd), WM_NOTIFY,
			      (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
	    }
	}
	if (IsWindow(hwnd))
	    KillTimer(hwnd, TIMERID1);
    }
    return DefWindowProcA (hwnd, WM_MOUSEMOVE, wParam, lParam);
}

static LRESULT
PAGER_LButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    BOOL repaintBtns = FALSE;
    POINT pt;
    INT hit;

    pt.x = SLOWORD(lParam);
    pt.y = SHIWORD(lParam);

    TRACE("[%p] at (%d,%d)\n", hwnd, SLOWORD(lParam), SHIWORD(lParam));

    hit = PAGER_HitTest(hwnd, &pt);

    /* put btn in DEPRESSED state */
    if (hit == HTLEFT || hit == HTTOP)
    {
        repaintBtns = infoPtr->TLbtnState != PGF_DEPRESSED;
        infoPtr->TLbtnState = PGF_DEPRESSED;
        SetTimer(hwnd, TIMERID1, INITIAL_DELAY, 0);
    }
    else if (hit == HTRIGHT || hit == HTBOTTOM)
    {
        repaintBtns = infoPtr->BRbtnState != PGF_DEPRESSED;
        infoPtr->BRbtnState = PGF_DEPRESSED;
        SetTimer(hwnd, TIMERID1, INITIAL_DELAY, 0);
    }

    if (repaintBtns)
        SendMessageA(hwnd, WM_NCPAINT, 0, 0);

    switch(hit)
    {
    case HTLEFT:
        TRACE("[%p] PGF_SCROLLLEFT\n", hwnd);
        PAGER_Scroll(hwnd, PGF_SCROLLLEFT);
        break;
    case HTTOP:
        TRACE("[%p] PGF_SCROLLUP\n", hwnd);
        PAGER_Scroll(hwnd, PGF_SCROLLUP);
        break;
    case HTRIGHT:
        TRACE("[%p] PGF_SCROLLRIGHT\n", hwnd);
        PAGER_Scroll(hwnd, PGF_SCROLLRIGHT);
        break;
    case HTBOTTOM:
        TRACE("[%p] PGF_SCROLLDOWN\n", hwnd);
        PAGER_Scroll(hwnd, PGF_SCROLLDOWN);
        break;
    default:
        break;
    }

    return TRUE;
}

static LRESULT
PAGER_LButtonUp (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    TRACE("[%p]\n", hwnd);

    KillTimer (hwnd, TIMERID1);
    KillTimer (hwnd, TIMERID2);

    /* make PRESSED btns NORMAL but don't hide gray btns */
    PAGER_UpdateBtns(hwnd, infoPtr, -1, FALSE);

    return 0;
}

static LRESULT
PAGER_NCLButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt;

    pt.x = SLOWORD(lParam);
    pt.y = SHIWORD(lParam);

    TRACE("[%p] at (%d,%d)\n", hwnd, SLOWORD(lParam), SHIWORD(lParam));
    MapWindowPoints(0, hwnd, &pt, 1);
    lParam = MAKELONG(pt.x, pt.y);
    return PAGER_LButtonDown (hwnd, wParam, lParam);
}

static LRESULT
PAGER_Timer (HWND hwnd, WPARAM wParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    INT dir;

    /* if initial timer, kill it and start the repeat timer */
    if (wParam == TIMERID1) {
	if (PAGER_IsHorizontal(hwnd)) {
	    dir = (infoPtr->TLbtnState & PGF_DEPRESSED) ?
		PGF_SCROLLLEFT : PGF_SCROLLRIGHT;
	}
	else {
	    dir = (infoPtr->TLbtnState & PGF_DEPRESSED) ?
		PGF_SCROLLUP : PGF_SCROLLDOWN;
	}
	TRACE("[%p] TIMERID1: style=%08lx, dir=%d\n", hwnd, dwStyle, dir);
	KillTimer(hwnd, TIMERID1);
	SetTimer(hwnd, TIMERID1, REPEAT_DELAY, 0);
	if (dwStyle & PGS_AUTOSCROLL) {
	    PAGER_Scroll(hwnd, dir);
	    SetWindowPos(hwnd, 0,0,0,0,0,
			 SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE |
			 SWP_NOZORDER | SWP_NOACTIVATE);
	}
	return 0;

    }

    TRACE("[%p] TIMERID2: dir=%d\n", hwnd, infoPtr->direction);
    KillTimer(hwnd, TIMERID2);
    if (infoPtr->direction > 0) {
	PAGER_Scroll(hwnd, infoPtr->direction);
	SetTimer(hwnd, TIMERID2, REPEAT_DELAY, 0);
    }
    return 0;
}

static LRESULT
PAGER_EraseBackground (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt, ptorig;
    HDC hdc = (HDC)wParam;
    HWND parent;

    /* native does:
     *   parent = GetParent(pager)
     *   pt.x=0; pt.y=0; ?????
     *   MapWindowPoints(pager, parent, &pt, 1)
     *   OffsetWindowOrgEx(hdc, pt.x, pt.y, &ptorg)
     *   SendMessageA(parent, WM_ERASEBKGND, hdc, 0)
     *   SetWindowOrgEx(hdc, 0, 0, 0)
     */

    pt.x = 0;
    pt.y = 0;
    parent = GetParent(hwnd);
    MapWindowPoints(hwnd, parent, &pt, 1);
    OffsetWindowOrgEx (hdc, pt.x, pt.y, &ptorig);
    SendMessageA (parent, WM_ERASEBKGND, wParam, lParam);
    SetWindowOrgEx (hdc, ptorig.x, ptorig.y, 0);


#if 0
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    HBRUSH hBrush = CreateSolidBrush(infoPtr->clrBk);
    RECT rect;

    GetClientRect (hwnd, &rect);
    FillRect ((HDC)wParam, &rect, hBrush);

    /* background color of the child should be the same as the pager */
    if (infoPtr->hwndChild)
    {
        GetClientRect (infoPtr->hwndChild, &rect);
        FillRect ((HDC)wParam, &rect, hBrush);
    }

    DeleteObject (hBrush);
#endif

    return TRUE;
}


static LRESULT
PAGER_Size (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    /* note that WM_SIZE is sent whenever NCCalcSize resizes the client wnd */

    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);
    TRACE("[%p] %dx%d\n", hwnd, SLOWORD(lParam), SHIWORD(lParam));

    if (PAGER_IsHorizontal(hwnd))
        infoPtr->nHeight = SHIWORD(lParam);
    else
        infoPtr->nWidth = SLOWORD(lParam);

    return PAGER_RecalcSize(hwnd);
}


static LRESULT WINAPI
PAGER_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAGER_INFO *infoPtr = PAGER_GetInfoPtr (hwnd);

    if (!infoPtr && (uMsg != WM_CREATE))
	return DefWindowProcA (hwnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
        case EM_FMTLINES:
	    return PAGER_FmtLines(hwnd);

        case PGM_FORWARDMOUSE:
            return PAGER_ForwardMouse (hwnd, wParam);

        case PGM_GETBKCOLOR:
            return PAGER_GetBkColor(hwnd);

        case PGM_GETBORDER:
            return PAGER_GetBorder(hwnd);

        case PGM_GETBUTTONSIZE:
            return PAGER_GetButtonSize(hwnd);

        case PGM_GETPOS:
            return PAGER_GetPos(hwnd);

        case PGM_GETBUTTONSTATE:
            return PAGER_GetButtonState (hwnd, wParam, lParam);

/*      case PGM_GETDROPTARGET: */

        case PGM_RECALCSIZE:
            return PAGER_RecalcSize(hwnd);

        case PGM_SETBKCOLOR:
            return PAGER_SetBkColor (hwnd, wParam, lParam);

        case PGM_SETBORDER:
            return PAGER_SetBorder (hwnd, wParam, lParam);

        case PGM_SETBUTTONSIZE:
            return PAGER_SetButtonSize (hwnd, wParam, lParam);

        case PGM_SETCHILD:
            return PAGER_SetChild (hwnd, wParam, lParam);

        case PGM_SETPOS:
            return PAGER_SetPos(hwnd, (INT)lParam, FALSE);

        case WM_CREATE:
            return PAGER_Create (hwnd, wParam, lParam);

        case WM_DESTROY:
            return PAGER_Destroy (hwnd, wParam, lParam);

        case WM_SIZE:
            return PAGER_Size (hwnd, wParam, lParam);

        case WM_NCPAINT:
            return PAGER_NCPaint (hwnd, wParam, lParam);

        case WM_WINDOWPOSCHANGING:
            return PAGER_HandleWindowPosChanging (hwnd, wParam, (WINDOWPOS*)lParam);

        case WM_NCCALCSIZE:
            return PAGER_NCCalcSize (hwnd, wParam, lParam);

        case WM_NCHITTEST:
            return PAGER_NCHitTest (hwnd, wParam, lParam);

        case WM_SETCURSOR:
        {
            if (hwnd == (HWND)wParam)
                return PAGER_SetCursor(hwnd, wParam, lParam);
            else /* its for the child */
                return 0;
        }

        case WM_MOUSEMOVE:
            if (infoPtr->bForward && infoPtr->hwndChild)
                PostMessageA(infoPtr->hwndChild, WM_MOUSEMOVE, wParam, lParam);
            return PAGER_MouseMove (hwnd, wParam, lParam);

        case WM_MOUSELEAVE:
            return PAGER_MouseLeave (hwnd, wParam, lParam);

        case WM_NCLBUTTONDOWN:
            return PAGER_NCLButtonDown (hwnd, wParam, lParam);

        case WM_LBUTTONDOWN:
            return PAGER_LButtonDown (hwnd, wParam, lParam);

        case WM_NCLBUTTONUP:
        case WM_LBUTTONUP:
            return PAGER_LButtonUp (hwnd, wParam, lParam);

        case WM_ERASEBKGND:
            return PAGER_EraseBackground (hwnd, wParam, lParam);
/*
        case WM_PAINT:
            return PAGER_Paint (hwnd, wParam);
*/
        case WM_TIMER:
	    return PAGER_Timer (hwnd, wParam);

        case WM_NOTIFY:
        case WM_COMMAND:
            return SendMessageA (GetParent (hwnd), uMsg, wParam, lParam);

        default:
            return DefWindowProcA (hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


VOID
PAGER_Register (void)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS | CS_SAVEBITS;
    wndClass.lpfnWndProc   = (WNDPROC)PAGER_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(PAGER_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = 0;
    wndClass.lpszClassName = WC_PAGESCROLLERA;

    RegisterClassA (&wndClass);
}


VOID
PAGER_Unregister (void)
{
    UnregisterClassA (WC_PAGESCROLLERA, NULL);
}
