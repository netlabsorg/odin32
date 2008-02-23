/*
 * Updown control
 *
 * Copyright 1997, 2002 Dimitrie O. Paun
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
 * NOTE
 * 
 * This code was audited for completeness against the documented features
 * of Comctl32.dll version 6.0 on Sep. 9, 2002, by Dimitrie O. Paun.
 * 
 * Unless otherwise noted, we believe this code to be complete, as per
 * the specification mentioned above.
 * If you discover missing features, or bugs, please note them below.
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "commctrl.h"
#include "winnls.h"
#include "wine/unicode.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(updown);

typedef struct
{
    HWND      Self;            /* Handle to this up-down control */
    UINT      AccelCount;      /* Number of elements in AccelVect */
    UDACCEL*  AccelVect;       /* Vector containing AccelCount elements */
    INT       AccelIndex;      /* Current accel index, -1 if not accel'ing */
    INT       Base;            /* Base to display nr in the buddy window */
    INT       CurVal;          /* Current up-down value */
    INT       MinVal;          /* Minimum up-down value */
    INT       MaxVal;          /* Maximum up-down value */
    HWND      Buddy;           /* Handle to the buddy window */
    INT       BuddyType;       /* Remembers the buddy type BUDDY_TYPE_* */
    INT       Flags;           /* Internal Flags FLAG_* */
    BOOL      UnicodeFormat;   /* Marks the use of Unicode internally */
} UPDOWN_INFO;

/* Control configuration constants */

#define INITIAL_DELAY	500    /* initial timer until auto-inc kicks in */
#define AUTOPRESS_DELAY	250    /* time to keep arrow pressed on KEY_DOWN */
#define REPEAT_DELAY	50     /* delay between auto-increments */

#define DEFAULT_WIDTH	    14 /* default width of the ctrl */
#define DEFAULT_XSEP         0 /* default separation between buddy and ctrl */
#define DEFAULT_ADDTOP       0 /* amount to extend above the buddy window */
#define DEFAULT_ADDBOT       0 /* amount to extend below the buddy window */
#define DEFAULT_BUDDYBORDER  2 /* Width/height of the buddy border */
#define DEFAULT_BUDDYSPACER  2 /* Spacer between the buddy and the ctrl */


/* Work constants */

#define FLAG_INCR	0x01
#define FLAG_DECR	0x02
#define FLAG_MOUSEIN	0x04
#define FLAG_PRESSED	0x08
#define FLAG_ARROW	(FLAG_INCR | FLAG_DECR)

#define BUDDY_TYPE_UNKNOWN 0
#define BUDDY_TYPE_LISTBOX 1
#define BUDDY_TYPE_EDIT    2

#define TIMER_AUTOREPEAT   1
#define TIMER_ACCEL        2
#define TIMER_AUTOPRESS    3

#define BUDDY_UPDOWN_HWND        "buddyUpDownHWND"
#define BUDDY_SUPERCLASS_WNDPROC "buddySupperClassWndProc"

#define UPDOWN_GetInfoPtr(hwnd) ((UPDOWN_INFO *)GetWindowLongA (hwnd,0))
#define COUNT_OF(a) (sizeof(a)/sizeof(a[0]))

static void UPDOWN_DoAction (UPDOWN_INFO *infoPtr, int delta, int action);

/***********************************************************************
 *           UPDOWN_IsBuddyEdit
 * Tests if our buddy is an edit control.
 */
static inline BOOL UPDOWN_IsBuddyEdit(UPDOWN_INFO *infoPtr)
{
    return infoPtr->BuddyType == BUDDY_TYPE_EDIT;
}

/***********************************************************************
 *           UPDOWN_IsBuddyListbox
 * Tests if our buddy is a listbox control.
 */
static inline BOOL UPDOWN_IsBuddyListbox(UPDOWN_INFO *infoPtr)
{
    return infoPtr->BuddyType == BUDDY_TYPE_LISTBOX;
}

/***********************************************************************
 *           UPDOWN_InBounds
 * Tests if a given value 'val' is between the Min&Max limits
 */
static BOOL UPDOWN_InBounds(UPDOWN_INFO *infoPtr, int val)
{
    if(infoPtr->MaxVal > infoPtr->MinVal)
        return (infoPtr->MinVal <= val) && (val <= infoPtr->MaxVal);
    else
        return (infoPtr->MaxVal <= val) && (val <= infoPtr->MinVal);
}

/***********************************************************************
 *           UPDOWN_OffsetVal
 * Change the current value by delta.
 * It returns TRUE is the value was changed successfuly, or FALSE
 * if the value was not changed, as it would go out of bounds.
 */
static BOOL UPDOWN_OffsetVal(UPDOWN_INFO *infoPtr, int delta)
{
    /* check if we can do the modification first */
    if(!UPDOWN_InBounds (infoPtr, infoPtr->CurVal+delta)) {
        if (GetWindowLongW (infoPtr->Self, GWL_STYLE) & UDS_WRAP) {
            delta += (delta < 0 ? -1 : 1) *
		     (infoPtr->MaxVal < infoPtr->MinVal ? -1 : 1) *
		     (infoPtr->MinVal - infoPtr->MaxVal) +
		     (delta < 0 ? 1 : -1);
        } else return FALSE;
    }

    infoPtr->CurVal += delta;
    return TRUE;
}

/***********************************************************************
 * UPDOWN_HasBuddyBorder
 *
 * When we have a buddy set and that we are aligned on our buddy, we
 * want to draw a sunken edge to make like we are part of that control.
 */
static BOOL UPDOWN_HasBuddyBorder(UPDOWN_INFO* infoPtr)
{
    DWORD dwStyle = GetWindowLongW (infoPtr->Self, GWL_STYLE);

    return  ( ((dwStyle & (UDS_ALIGNLEFT | UDS_ALIGNRIGHT)) != 0) &&
	      UPDOWN_IsBuddyEdit(infoPtr) );
}

/***********************************************************************
 *           UPDOWN_GetArrowRect
 * wndPtr   - pointer to the up-down wnd
 * rect     - will hold the rectangle
 * arrow    - FLAG_INCR to get the "increment" rect (up or right)
 *            FLAG_DECR to get the "decrement" rect (down or left)
 *            If both flags are pressent, the envelope is returned.
 */
static void UPDOWN_GetArrowRect (UPDOWN_INFO* infoPtr, RECT *rect, int arrow)
{
    DWORD dwStyle = GetWindowLongW (infoPtr->Self, GWL_STYLE);

    GetClientRect (infoPtr->Self, rect);

    /*
     * Make sure we calculate the rectangle to fit even if we draw the
     * border.
     */
    if (UPDOWN_HasBuddyBorder(infoPtr)) {
        if (dwStyle & UDS_ALIGNLEFT)
            rect->left += DEFAULT_BUDDYBORDER;
        else
            rect->right -= DEFAULT_BUDDYBORDER;

        InflateRect(rect, 0, -DEFAULT_BUDDYBORDER);
    }

    /* now figure out if we need a space away from the buddy */
    if ( IsWindow(infoPtr->Buddy) ) {
	if (dwStyle & UDS_ALIGNLEFT) rect->right -= DEFAULT_BUDDYSPACER;
	else rect->left += DEFAULT_BUDDYSPACER;
    }

    /*
     * We're calculating the midpoint to figure-out where the
     * separation between the buttons will lay. We make sure that we
     * round the uneven numbers by adding 1.
     */
    if (dwStyle & UDS_HORZ) {
        int len = rect->right - rect->left + 1; /* compute the width */
        if (arrow & FLAG_INCR)
            rect->left = rect->left + len/2;
        if (arrow & FLAG_DECR)
            rect->right =  rect->left + len/2 - 1;
    } else {
        int len = rect->bottom - rect->top + 1; /* compute the height */
        if (arrow & FLAG_INCR)
            rect->bottom =  rect->top + len/2 - 1;
        if (arrow & FLAG_DECR)
            rect->top =  rect->top + len/2;
    }
}

/***********************************************************************
 *           UPDOWN_GetArrowFromPoint
 * Returns the rectagle (for the up or down arrow) that contains pt.
 * If it returns the up rect, it returns TRUE.
 * If it returns the down rect, it returns FALSE.
 */
static BOOL UPDOWN_GetArrowFromPoint (UPDOWN_INFO* infoPtr, RECT *rect, POINT pt)
{
    UPDOWN_GetArrowRect (infoPtr, rect, FLAG_INCR);
    if(PtInRect(rect, pt)) return FLAG_INCR;

    UPDOWN_GetArrowRect (infoPtr, rect, FLAG_DECR);
    if(PtInRect(rect, pt)) return FLAG_DECR;

    return 0;
}


/***********************************************************************
 *           UPDOWN_GetThousandSep
 * Returns the thousand sep. If an error occurs, it returns ','.
 */
static WCHAR UPDOWN_GetThousandSep()
{
    WCHAR sep[2];

    if(GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, sep, 2) != 1)
        sep[0] = ',';

    return sep[0];
}

/***********************************************************************
 *           UPDOWN_GetBuddyInt
 * Tries to read the pos from the buddy window and if it succeeds,
 * it stores it in the control's CurVal
 * returns:
 *   TRUE  - if it read the integer from the buddy successfully
 *   FALSE - if an error occurred
 */
static BOOL UPDOWN_GetBuddyInt (UPDOWN_INFO *infoPtr)
{
    WCHAR txt[20], sep, *src, *dst;
    int newVal;

    if (!IsWindow(infoPtr->Buddy))
        return FALSE;

    /*if the buddy is a list window, we must set curr index */
    if (UPDOWN_IsBuddyListbox(infoPtr)) {
        newVal = SendMessageW(infoPtr->Buddy, LB_GETCARETINDEX, 0, 0);
        if(newVal < 0) return FALSE;
    } else {
        /* we have a regular window, so will get the text */
        if (!GetWindowTextW(infoPtr->Buddy, txt, COUNT_OF(txt))) return FALSE;

        sep = UPDOWN_GetThousandSep();

        /* now get rid of the separators */
        for(src = dst = txt; *src; src++)
            if(*src != sep) *dst++ = *src;
        *dst = 0;

        /* try to convert the number and validate it */
        newVal = strtolW(txt, &src, infoPtr->Base);
        if(*src || !UPDOWN_InBounds (infoPtr, newVal)) return FALSE;
    }

    TRACE("new value(%d) from buddy (old=%d)\n", newVal, infoPtr->CurVal);
    infoPtr->CurVal = newVal;
    return TRUE;
}


/***********************************************************************
 *           UPDOWN_SetBuddyInt
 * Tries to set the pos to the buddy window based on current pos
 * returns:
 *   TRUE  - if it set the caption of the  buddy successfully
 *   FALSE - if an error occurred
 */
static BOOL UPDOWN_SetBuddyInt (UPDOWN_INFO *infoPtr)
{
    WCHAR fmt[3] = { '%', 'd', '\0' };
    WCHAR txt[20];
    int len;

    if (!IsWindow(infoPtr->Buddy)) return FALSE;

    TRACE("set new value(%d) to buddy.\n", infoPtr->CurVal);

    /*if the buddy is a list window, we must set curr index */
    if (UPDOWN_IsBuddyListbox(infoPtr)) {
        return SendMessageW(infoPtr->Buddy, LB_SETCURSEL, infoPtr->CurVal, 0) != LB_ERR;
    }

    /* Regular window, so set caption to the number */
    if (infoPtr->Base == 16) fmt[1] = 'X';
    len = wsprintfW(txt, fmt, infoPtr->CurVal);


    /* Do thousands seperation if necessary */
    if (!(GetWindowLongW (infoPtr->Self, GWL_STYLE) & UDS_NOTHOUSANDS) && (len > 3)) {
        WCHAR tmp[COUNT_OF(txt)], *src = tmp, *dst = txt;
        WCHAR sep = UPDOWN_GetThousandSep();
	int start = len % 3;

	memcpy(tmp, txt, sizeof(txt));
	if (start == 0) start = 3;
	dst += start;
	src += start;
        for (len=0; *src; len++) {
	    if (len % 3 == 0) *dst++ = sep;
	    *dst++ = *src++;
        }
        *dst = 0;
    }

    return SetWindowTextW(infoPtr->Buddy, txt);
}

/***********************************************************************
 * UPDOWN_Draw
 *
 * Draw the arrows. The background need not be erased.
 */
static LRESULT UPDOWN_Draw (UPDOWN_INFO *infoPtr, HDC hdc)
{
    DWORD dwStyle = GetWindowLongW (infoPtr->Self, GWL_STYLE);
    BOOL pressed, hot;
    RECT rect;

    /* Draw the common border between ourselves and our buddy */
    if (UPDOWN_HasBuddyBorder(infoPtr)) {
	GetClientRect(infoPtr->Self, &rect);
	DrawEdge(hdc, &rect, EDGE_SUNKEN,
		 BF_BOTTOM | BF_TOP |
		 (dwStyle & UDS_ALIGNLEFT ? BF_LEFT : BF_RIGHT));
    }

    /* Draw the incr button */
    UPDOWN_GetArrowRect (infoPtr, &rect, FLAG_INCR);
    pressed = (infoPtr->Flags & FLAG_PRESSED) && (infoPtr->Flags & FLAG_INCR);
    hot = (infoPtr->Flags & FLAG_INCR) && (infoPtr->Flags & FLAG_MOUSEIN);
    DrawFrameControl(hdc, &rect, DFC_SCROLL,
	(dwStyle & UDS_HORZ ? DFCS_SCROLLRIGHT : DFCS_SCROLLUP) |
        ((dwStyle & UDS_HOTTRACK) && hot ? DFCS_HOT : 0) |
	(pressed ? DFCS_PUSHED : 0) |
	(dwStyle & WS_DISABLED ? DFCS_INACTIVE : 0) );

    /* Draw the decr button */
    UPDOWN_GetArrowRect(infoPtr, &rect, FLAG_DECR);
    pressed = (infoPtr->Flags & FLAG_PRESSED) && (infoPtr->Flags & FLAG_DECR);
    hot = (infoPtr->Flags & FLAG_DECR) && (infoPtr->Flags & FLAG_MOUSEIN);
    DrawFrameControl(hdc, &rect, DFC_SCROLL,
	(dwStyle & UDS_HORZ ? DFCS_SCROLLLEFT : DFCS_SCROLLDOWN) |
        ((dwStyle & UDS_HOTTRACK) && hot ? DFCS_HOT : 0) |
	(pressed ? DFCS_PUSHED : 0) |
	(dwStyle & WS_DISABLED ? DFCS_INACTIVE : 0) );

    return 0;
}

/***********************************************************************
 * UPDOWN_Paint
 *
 * Asynchronous drawing (must ONLY be used in WM_PAINT).
 * Calls UPDOWN_Draw.
 */
static LRESULT UPDOWN_Paint (UPDOWN_INFO *infoPtr, HDC hdc)
{
    PAINTSTRUCT ps;
    if (hdc) return UPDOWN_Draw (infoPtr, hdc);
    hdc = BeginPaint (infoPtr->Self, &ps);
    UPDOWN_Draw (infoPtr, hdc);
    EndPaint (infoPtr->Self, &ps);
    return 0;
}

/***********************************************************************
 * UPDOWN_KeyPressed
 *
 * Handle key presses (up & down) when we have to do so
 */
static LRESULT UPDOWN_KeyPressed(UPDOWN_INFO *infoPtr, int key)
{
    int arrow;

    if (key == VK_UP) arrow = FLAG_INCR;
    else if (key == VK_DOWN) arrow = FLAG_DECR;
    else return 1;

    UPDOWN_GetBuddyInt (infoPtr);
    infoPtr->Flags &= ~FLAG_ARROW;
    infoPtr->Flags |= FLAG_PRESSED | arrow;
    InvalidateRect (infoPtr->Self, NULL, FALSE);
    SetTimer(infoPtr->Self, TIMER_AUTOPRESS, AUTOPRESS_DELAY, 0);
    UPDOWN_DoAction (infoPtr, 1, arrow);
    return 0;
}

/***********************************************************************
 * UPDOWN_Buddy_SubclassProc used to handle messages sent to the buddy
 *                           control.
 */
static LRESULT CALLBACK
UPDOWN_Buddy_SubclassProc(HWND  hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC superClassWndProc = (WNDPROC)GetPropA(hwnd, BUDDY_SUPERCLASS_WNDPROC);
    TRACE("hwnd=%p, wndProc=%d, uMsg=%04x, wParam=%d, lParam=%d\n",
	  hwnd, (INT)superClassWndProc, uMsg, wParam, (UINT)lParam);

    if (uMsg == WM_KEYDOWN) {
        HWND upDownHwnd = GetPropA(hwnd, BUDDY_UPDOWN_HWND);

	UPDOWN_KeyPressed(UPDOWN_GetInfoPtr(upDownHwnd), (int)wParam);
    }

    return CallWindowProcW( superClassWndProc, hwnd, uMsg, wParam, lParam);
}

/***********************************************************************
 *           UPDOWN_SetBuddy
 * Tests if 'bud' is a valid window handle. If not, returns FALSE.
 * Else, sets it as a new Buddy.
 * Then, it should subclass the buddy
 * If window has the UDS_ARROWKEYS, it subcalsses the buddy window to
 * process the UP/DOWN arrow keys.
 * If window has the UDS_ALIGNLEFT or UDS_ALIGNRIGHT style
 * the size/pos of the buddy and the control are adjusted accordingly.
 */
static BOOL UPDOWN_SetBuddy (UPDOWN_INFO* infoPtr, HWND bud)
{
    DWORD dwStyle = GetWindowLongW (infoPtr->Self, GWL_STYLE);
    RECT  budRect;  /* new coord for the buddy */
    int   x, width;  /* new x position and width for the up-down */
    WNDPROC baseWndProc;
    CHAR buddyClass[40];

    /* Is it a valid bud? */
    if(!IsWindow(bud)) return FALSE;

    TRACE("(hwnd=%p, bud=%p)\n", infoPtr->Self, bud);

    /* there is already a body assigned */
    if (infoPtr->Buddy)  RemovePropA(infoPtr->Buddy, BUDDY_UPDOWN_HWND);

    /* Store buddy window handle */
    infoPtr->Buddy = bud;

    /* keep upDown ctrl hwnd in a buddy property */
    SetPropA( bud, BUDDY_UPDOWN_HWND, infoPtr->Self);

    /* Store buddy window class type */
    infoPtr->BuddyType = BUDDY_TYPE_UNKNOWN;
    if (GetClassNameA(bud, buddyClass, COUNT_OF(buddyClass))) {
	if (lstrcmpiA(buddyClass, "Edit") == 0)
	    infoPtr->BuddyType = BUDDY_TYPE_EDIT;
	else if (lstrcmpiA(buddyClass, "Listbox") == 0)
	    infoPtr->BuddyType = BUDDY_TYPE_LISTBOX;
    }

    if(dwStyle & UDS_ARROWKEYS){
        /* Note that I don't clear the BUDDY_SUPERCLASS_WNDPROC property
           when we reset the upDown ctrl buddy to another buddy because it is not
           good to break the window proc chain. */
	if (!GetPropA(bud, BUDDY_SUPERCLASS_WNDPROC)) {
	    baseWndProc = (WNDPROC)SetWindowLongW(bud, GWL_WNDPROC, (LPARAM)UPDOWN_Buddy_SubclassProc);
 	    SetPropA(bud, BUDDY_SUPERCLASS_WNDPROC, (HANDLE)baseWndProc);
	}
    }

    /* Get the rect of the buddy relative to its parent */
    GetWindowRect(infoPtr->Buddy, &budRect);
    MapWindowPoints(HWND_DESKTOP, GetParent(infoPtr->Buddy), (POINT *)(&budRect.left), 2);

    /* now do the positioning */
    if  (dwStyle & UDS_ALIGNLEFT) {
        x  = budRect.left;
        budRect.left += DEFAULT_WIDTH + DEFAULT_XSEP;
    } else if (dwStyle & UDS_ALIGNRIGHT) {
        budRect.right -= DEFAULT_WIDTH + DEFAULT_XSEP;
        x  = budRect.right+DEFAULT_XSEP;
    } else {
        x  = budRect.right+DEFAULT_XSEP;
    }

    /* first adjust the buddy to accomodate the up/down */
    SetWindowPos(infoPtr->Buddy, 0, budRect.left, budRect.top,
	         budRect.right  - budRect.left, budRect.bottom - budRect.top,
	         SWP_NOACTIVATE|SWP_NOZORDER);

    /* now position the up/down */
    /* Since the UDS_ALIGN* flags were used, */
    /* we will pick the position and size of the window. */
    width = DEFAULT_WIDTH;

    /*
     * If the updown has a buddy border, it has to overlap with the buddy
     * to look as if it is integrated with the buddy control.
     * We nudge the control or change it size to overlap.
     */
    if (UPDOWN_HasBuddyBorder(infoPtr)) {
        if(dwStyle & UDS_ALIGNLEFT)
            width += DEFAULT_BUDDYBORDER;
        else
            x -= DEFAULT_BUDDYBORDER;
    }

    SetWindowPos(infoPtr->Self, infoPtr->Buddy, x,
		 budRect.top - DEFAULT_ADDTOP, width,
		 budRect.bottom - budRect.top + DEFAULT_ADDTOP + DEFAULT_ADDBOT,
		 SWP_NOACTIVATE);

    return TRUE;
}

/***********************************************************************
 *           UPDOWN_DoAction
 *
 * This function increments/decrements the CurVal by the
 * 'delta' amount according to the 'action' flag which can be a
 * combination of FLAG_INCR and FLAG_DECR
 * It notifies the parent as required.
 * It handles wraping and non-wraping correctly.
 * It is assumed that delta>0
 */
static void UPDOWN_DoAction (UPDOWN_INFO *infoPtr, int delta, int action)
{
    DWORD dwStyle = GetWindowLongW (infoPtr->Self, GWL_STYLE);
    NM_UPDOWN ni;

    TRACE("%d by %d\n", action, delta);

    /* check if we can do the modification first */
    delta *= (action & FLAG_INCR ? 1 : -1) * (infoPtr->MaxVal < infoPtr->MinVal ? -1 : 1);
    if ( (action & FLAG_INCR) && (action & FLAG_DECR) ) delta = 0;

    /* We must notify parent now to obtain permission */
    ni.iPos = infoPtr->CurVal;
    ni.iDelta = delta;
    ni.hdr.hwndFrom = infoPtr->Self;
    ni.hdr.idFrom   = GetWindowLongW (infoPtr->Self, GWL_ID);
    ni.hdr.code = UDN_DELTAPOS;
    if (!SendMessageW(GetParent (infoPtr->Self), WM_NOTIFY,
		   (WPARAM)ni.hdr.idFrom, (LPARAM)&ni)) {
        /* Parent said: OK to adjust */

        /* Now adjust value with (maybe new) delta */
        if (UPDOWN_OffsetVal (infoPtr, ni.iDelta)) {
            /* Now take care about our buddy */
            if (dwStyle & UDS_SETBUDDYINT) UPDOWN_SetBuddyInt (infoPtr);
        }
    }

    /* Also, notify it. This message is sent in any case. */
    SendMessageW( GetParent(infoPtr->Self),
		  dwStyle & UDS_HORZ ? WM_HSCROLL : WM_VSCROLL,
		  MAKELONG(SB_THUMBPOSITION, infoPtr->CurVal),
		  (LPARAM)infoPtr->Self);
}

/***********************************************************************
 *           UPDOWN_IsEnabled
 *
 * Returns TRUE if it is enabled as well as its buddy (if any)
 *         FALSE otherwise
 */
static BOOL UPDOWN_IsEnabled (UPDOWN_INFO *infoPtr)
{
    if(GetWindowLongW (infoPtr->Self, GWL_STYLE) & WS_DISABLED)
        return FALSE;
    if(infoPtr->Buddy)
        return IsWindowEnabled(infoPtr->Buddy);
    return TRUE;
}

/***********************************************************************
 *           UPDOWN_CancelMode
 *
 * Deletes any timers, releases the mouse and does  redraw if necessary.
 * If the control is not in "capture" mode, it does nothing.
 * If the control was not in cancel mode, it returns FALSE.
 * If the control was in cancel mode, it returns TRUE.
 */
static BOOL UPDOWN_CancelMode (UPDOWN_INFO *infoPtr)
{
    if (!(infoPtr->Flags & FLAG_PRESSED)) return FALSE;

    KillTimer (infoPtr->Self, TIMER_AUTOREPEAT);
    KillTimer (infoPtr->Self, TIMER_ACCEL);
    KillTimer (infoPtr->Self, TIMER_AUTOPRESS);

    if (GetCapture() == infoPtr->Self) {
	NMHDR hdr;
	hdr.hwndFrom = infoPtr->Self;
	hdr.idFrom   = GetWindowLongW (infoPtr->Self, GWL_ID);
	hdr.code = NM_RELEASEDCAPTURE;
	SendMessageW(GetParent (infoPtr->Self), WM_NOTIFY, hdr.idFrom, (LPARAM)&hdr);
	ReleaseCapture();
    }

    infoPtr->Flags &= ~FLAG_PRESSED;
    InvalidateRect (infoPtr->Self, NULL, FALSE);

    return TRUE;
}

/***********************************************************************
 *           UPDOWN_HandleMouseEvent
 *
 * Handle a mouse event for the updown.
 * 'pt' is the location of the mouse event in client or
 * windows coordinates.
 */
static void UPDOWN_HandleMouseEvent (UPDOWN_INFO *infoPtr, UINT msg, POINTS pts)
{
    DWORD dwStyle = GetWindowLongW (infoPtr->Self, GWL_STYLE);
    POINT pt = { pts.x, pts.y };
    RECT rect;
    int temp, arrow;

    switch(msg)
    {
        case WM_LBUTTONDOWN:  /* Initialise mouse tracking */
            /* If we are inside an arrow, then nothing to do */
            if(!(infoPtr->Flags & FLAG_MOUSEIN)) return;

            /* If the buddy is an edit, will set focus to it */
	    if (UPDOWN_IsBuddyEdit(infoPtr)) SetFocus(infoPtr->Buddy);

            /* Now see which one is the 'active' arrow */
	    if (infoPtr->Flags & FLAG_ARROW) {

            	/* Update the CurVal if necessary */
            	if (dwStyle & UDS_SETBUDDYINT) UPDOWN_GetBuddyInt (infoPtr);

            	/* Set up the correct flags */
            	infoPtr->Flags |= FLAG_PRESSED;

            	/* repaint the control */
	    	InvalidateRect (infoPtr->Self, NULL, FALSE);

            	/* process the click */
            	UPDOWN_DoAction (infoPtr, 1, infoPtr->Flags & FLAG_ARROW);

            	/* now capture all mouse messages */
            	SetCapture (infoPtr->Self);

            	/* and startup the first timer */
            	SetTimer(infoPtr->Self, TIMER_AUTOREPEAT, INITIAL_DELAY, 0);
	    }
            break;

	case WM_MOUSEMOVE:
            /* save the flags to see if any got modified */
            temp = infoPtr->Flags;

            /* Now see which one is the 'active' arrow */
            arrow = UPDOWN_GetArrowFromPoint (infoPtr, &rect, pt);

            /* Update the flags if we are in/out */
	    infoPtr->Flags &= ~(FLAG_MOUSEIN | FLAG_ARROW);
            if(arrow) {
	        infoPtr->Flags |=  FLAG_MOUSEIN | arrow;
            } else {
	        if(infoPtr->AccelIndex != -1) infoPtr->AccelIndex = 0;
            }

            /* If state changed, redraw the control */
            if(temp != infoPtr->Flags)
		 InvalidateRect (infoPtr->Self, &rect, FALSE);
            break;

	default:
	    ERR("Impossible case (msg=%x)!\n", msg);
    }

}

/***********************************************************************
 *           UpDownWndProc
 */
static LRESULT WINAPI UpDownWindowProc(HWND hwnd, UINT message, WPARAM wParam,
				LPARAM lParam)
{
    UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongW (hwnd, GWL_STYLE);
    int temp;

    if (!infoPtr && (message != WM_CREATE))
        return DefWindowProcW (hwnd, message, wParam, lParam);

    switch(message)
    {
        case WM_CREATE:
            SetWindowLongW (hwnd, GWL_STYLE, dwStyle & ~WS_BORDER);
            infoPtr = (UPDOWN_INFO*)COMCTL32_Alloc (sizeof(UPDOWN_INFO));
	    SetWindowLongW (hwnd, 0, (DWORD)infoPtr);

	    /* initialize the info struct */
	    infoPtr->Self = hwnd;
	    infoPtr->AccelCount = 0;
	    infoPtr->AccelVect = 0;
	    infoPtr->AccelIndex = -1;
	    infoPtr->CurVal = 0;
	    infoPtr->MinVal = 0;
	    infoPtr->MaxVal = 9999;
	    infoPtr->Base  = 10; /* Default to base 10  */
	    infoPtr->Buddy = 0;  /* No buddy window yet */
	    infoPtr->Flags = 0;  /* And no flags        */

            /* Do we pick the buddy win ourselves? */
	    if (dwStyle & UDS_AUTOBUDDY)
		UPDOWN_SetBuddy (infoPtr, GetWindow (hwnd, GW_HWNDPREV));

	    TRACE("UpDown Ctrl creation, hwnd=%p\n", hwnd);
	    break;

	case WM_DESTROY:
	    if(infoPtr->AccelVect) COMCTL32_Free (infoPtr->AccelVect);

	    if(infoPtr->Buddy) RemovePropA(infoPtr->Buddy, BUDDY_UPDOWN_HWND);

	    COMCTL32_Free (infoPtr);
	    SetWindowLongW (hwnd, 0, 0);
	    TRACE("UpDown Ctrl destruction, hwnd=%p\n", hwnd);
	    break;

	case WM_ENABLE:
	    if (dwStyle & WS_DISABLED) UPDOWN_CancelMode (infoPtr);
	    InvalidateRect (infoPtr->Self, NULL, FALSE);
	    break;

	case WM_TIMER:
	   /* is this the auto-press timer? */
	   if(wParam == TIMER_AUTOPRESS) {
		KillTimer(hwnd, TIMER_AUTOPRESS);
		infoPtr->Flags &= ~(FLAG_PRESSED | FLAG_ARROW);
		InvalidateRect(infoPtr->Self, NULL, FALSE);
	   }

	   /* if initial timer, kill it and start the repeat timer */
  	   if(wParam == TIMER_AUTOREPEAT) {
		KillTimer(hwnd, TIMER_AUTOREPEAT);
		/* if no accel info given, used default timer */
		if(infoPtr->AccelCount==0 || infoPtr->AccelVect==0) {
		    infoPtr->AccelIndex = -1;
		    temp = REPEAT_DELAY;
		} else {
		    infoPtr->AccelIndex = 0; /* otherwise, use it */
		    temp = infoPtr->AccelVect[infoPtr->AccelIndex].nSec * 1000 + 1;
		}
		SetTimer(hwnd, TIMER_ACCEL, temp, 0);
      	    }

	    /* now, if the mouse is above us, do the thing...*/
	    if(infoPtr->Flags & FLAG_MOUSEIN) {
		temp = infoPtr->AccelIndex == -1 ? 1 : infoPtr->AccelVect[infoPtr->AccelIndex].nInc;
		UPDOWN_DoAction(infoPtr, temp, infoPtr->Flags & FLAG_ARROW);

		if(infoPtr->AccelIndex != -1 && infoPtr->AccelIndex < infoPtr->AccelCount-1) {
		    KillTimer(hwnd, TIMER_ACCEL);
		    infoPtr->AccelIndex++; /* move to the next accel info */
		    temp = infoPtr->AccelVect[infoPtr->AccelIndex].nSec * 1000 + 1;
	  	    /* make sure we have at least 1ms intervals */
		    SetTimer(hwnd, TIMER_ACCEL, temp, 0);
		}
	    }
	    break;

	case WM_CANCELMODE:
	  return UPDOWN_CancelMode (infoPtr);

	case WM_LBUTTONUP:
	    if (GetCapture() != infoPtr->Self) break;

	    if ( (infoPtr->Flags & FLAG_MOUSEIN) &&
		 (infoPtr->Flags & FLAG_ARROW) ) {

	    	SendMessageW( GetParent(hwnd),
			      dwStyle & UDS_HORZ ? WM_HSCROLL : WM_VSCROLL,
                  	      MAKELONG(SB_ENDSCROLL, infoPtr->CurVal),
			      (LPARAM)hwnd);
		if (UPDOWN_IsBuddyEdit(infoPtr))
		    SendMessageW(infoPtr->Buddy, EM_SETSEL, 0, MAKELONG(0, -1));
	    }
	    UPDOWN_CancelMode(infoPtr);
	    break;

	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
	    if(UPDOWN_IsEnabled(infoPtr))
		UPDOWN_HandleMouseEvent (infoPtr, message, MAKEPOINTS(lParam));
	    break;

	case WM_KEYDOWN:
	    if((dwStyle & UDS_ARROWKEYS) && UPDOWN_IsEnabled(infoPtr))
		return UPDOWN_KeyPressed(infoPtr, (int)wParam);
	    break;

	case WM_PAINT:
	    return UPDOWN_Paint (infoPtr, (HDC)wParam);

	case UDM_GETACCEL:
	    if (wParam==0 && lParam==0) return infoPtr->AccelCount;
	    if (wParam && lParam) {
	        temp = min(infoPtr->AccelCount, wParam);
	        memcpy((void *)lParam, infoPtr->AccelVect, temp*sizeof(UDACCEL));
	        return temp;
      	    }
	    return 0;

	case UDM_SETACCEL:
	    TRACE("UpDown Ctrl new accel info, hwnd=%p\n", hwnd);
	    if(infoPtr->AccelVect) {
		COMCTL32_Free (infoPtr->AccelVect);
		infoPtr->AccelCount = 0;
		infoPtr->AccelVect  = 0;
      	    }
	    if(wParam==0) return TRUE;
	    infoPtr->AccelVect = COMCTL32_Alloc (wParam*sizeof(UDACCEL));
	    if(infoPtr->AccelVect == 0) return FALSE;
	    memcpy(infoPtr->AccelVect, (void*)lParam, wParam*sizeof(UDACCEL));
    	    return TRUE;

	case UDM_GETBASE:
	    return infoPtr->Base;

	case UDM_SETBASE:
	    TRACE("UpDown Ctrl new base(%d), hwnd=%p\n", wParam, hwnd);
	    if (wParam==10 || wParam==16) {
		temp = infoPtr->Base;
		infoPtr->Base = wParam;
		return temp;
	    }
	    break;

	case UDM_GETBUDDY:
	    return (LRESULT)infoPtr->Buddy;

	case UDM_SETBUDDY:
	    temp = (int)infoPtr->Buddy;
	    UPDOWN_SetBuddy (infoPtr, (HWND)wParam);
	    return temp;

	case UDM_GETPOS:
	    temp = UPDOWN_GetBuddyInt (infoPtr);
	    return MAKELONG(infoPtr->CurVal, temp ? 0 : 1);

	case UDM_SETPOS:
	    temp = SLOWORD(lParam);
	    TRACE("UpDown Ctrl new value(%d), hwnd=%p\n", temp, hwnd);
	    if(!UPDOWN_InBounds(infoPtr, temp)) {
		if(temp < infoPtr->MinVal) temp = infoPtr->MinVal;
		if(temp > infoPtr->MaxVal) temp = infoPtr->MaxVal;
	    }
	    wParam = infoPtr->CurVal;
	    infoPtr->CurVal = temp;
	    if(dwStyle & UDS_SETBUDDYINT) UPDOWN_SetBuddyInt (infoPtr);
	    return wParam;            /* return prev value */

	case UDM_GETRANGE:
	    return MAKELONG(infoPtr->MaxVal, infoPtr->MinVal);

	case UDM_SETRANGE:
		                               /* we must have:     */
	    infoPtr->MaxVal = SLOWORD(lParam); /* UD_MINVAL <= Max <= UD_MAXVAL */
	    infoPtr->MinVal = SHIWORD(lParam); /* UD_MINVAL <= Min <= UD_MAXVAL */
                	                       /* |Max-Min| <= UD_MAXVAL        */
	    TRACE("UpDown Ctrl new range(%d to %d), hwnd=%p\n",
		  infoPtr->MinVal, infoPtr->MaxVal, hwnd);
	    break;

	case UDM_GETRANGE32:
	    if (wParam) *(LPINT)wParam = infoPtr->MinVal;
	    if (lParam) *(LPINT)lParam = infoPtr->MaxVal;
	    break;

	case UDM_SETRANGE32:
	    infoPtr->MinVal = (INT)wParam;
	    infoPtr->MaxVal = (INT)lParam;
	    if (infoPtr->MaxVal <= infoPtr->MinVal)
		infoPtr->MaxVal = infoPtr->MinVal + 1;
	    TRACE("UpDown Ctrl new range(%d to %d), hwnd=%p\n",
		  infoPtr->MinVal, infoPtr->MaxVal, hwnd);
	    break;

	case UDM_GETPOS32:
	    if ((LPBOOL)lParam != NULL) *((LPBOOL)lParam) = TRUE;
	    return infoPtr->CurVal;

	case UDM_SETPOS32:
	    if(!UPDOWN_InBounds(infoPtr, (int)lParam)) {
		if((int)lParam < infoPtr->MinVal) lParam = infoPtr->MinVal;
		if((int)lParam > infoPtr->MaxVal) lParam = infoPtr->MaxVal;
	    }
	    temp = infoPtr->CurVal;         /* save prev value   */
	    infoPtr->CurVal = (int)lParam;  /* set the new value */
	    if(dwStyle & UDS_SETBUDDYINT) UPDOWN_SetBuddyInt (infoPtr);
	    return temp;                    /* return prev value */

	case UDM_GETUNICODEFORMAT:
	    /* we lie a bit here, we're always using Unicode internally */
	    return infoPtr->UnicodeFormat;

	case UDM_SETUNICODEFORMAT:
	    /* do we really need to honour this flag? */
	    temp = infoPtr->UnicodeFormat;
	    infoPtr->UnicodeFormat = (BOOL)wParam;
	    return temp;

	default:
	    if ((message >= WM_USER) && (message < WM_APP))
	     	ERR("unknown msg %04x wp=%04x lp=%08lx\n", message, wParam, lParam);
	    return DefWindowProcW (hwnd, message, wParam, lParam);
    }

    return 0;
}

/***********************************************************************
 *		UPDOWN_Register	[Internal]
 *
 * Registers the updown window class.
 */
void UPDOWN_Register(void)
{
    WNDCLASSW wndClass;

    ZeroMemory( &wndClass, sizeof( WNDCLASSW ) );
    wndClass.style         = CS_GLOBALCLASS | CS_VREDRAW;
    wndClass.lpfnWndProc   = (WNDPROC)UpDownWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(UPDOWN_INFO*);
    wndClass.hCursor       = LoadCursorW( 0, IDC_ARROWW );
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wndClass.lpszClassName = UPDOWN_CLASSW;

    RegisterClassW( &wndClass );
}


/***********************************************************************
 *		UPDOWN_Unregister	[Internal]
 *
 * Unregisters the updown window class.
 */
void UPDOWN_Unregister (void)
{
    UnregisterClassW (UPDOWN_CLASSW, (HINSTANCE)NULL);
}
