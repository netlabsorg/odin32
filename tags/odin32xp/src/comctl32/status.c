/*
 * Interface code to StatusWindow widget/control
 *
 * Copyright 1996 Bruce Milner
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 2002 Dimitrie O. Paun
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
 * of Comctl32.dll version 6.0 on Sep. 24, 2002, by Dimitrie O. Paun.
 * 
 * Unless otherwise noted, we believe this code to be complete, as per
 * the specification mentioned above.
 * If you discover missing features, or bugs, please note them below.
 * 
 * TODO:
 * 	-- CCS_BOTTOM (default)
 * 	-- CCS_LEFT
 * 	-- CCS_NODEVIDER
 * 	-- CCS_NOMOVEX
 * 	-- CCS_NOMOVEY
 * 	-- CCS_NOPARENTALIGN
 * 	-- CCS_RIGHT
 * 	-- CCS_TOP
 * 	-- CCS_VERT (defaults to RIGHT)
 */

#include <stdarg.h>
#include <string.h>

#include "windef.h"
#include "winbase.h"
#include "wine/unicode.h"
#include "wingdi.h"
#include "winuser.h"
#include "winnls.h"
#include "commctrl.h"
#include "comctl32.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(statusbar);

typedef struct
{
    INT 	x;
    INT 	style;
    RECT	bound;
    LPWSTR	text;
    HICON       hIcon;
} STATUSWINDOWPART;

typedef struct
{
    HWND              Self;
    HWND              Notify;
    WORD              numParts;
    UINT              height;
    BOOL              simple;
    HWND              hwndToolTip;
    HFONT             hFont;
    HFONT             hDefaultFont;
    COLORREF          clrBk;		/* background color */
    BOOL              bUnicode;		/* unicode flag */
    BOOL              NtfUnicode;	/* notify format */
    STATUSWINDOWPART  part0;		/* simple window */
    STATUSWINDOWPART* parts;
} STATUSWINDOWINFO;

/*
 * Run tests using Waite Group Windows95 API Bible Vol. 1&2
 * The second cdrom contains executables drawstat.exe, gettext.exe,
 * simple.exe, getparts.exe, setparts.exe, statwnd.exe
 */

#define HORZ_BORDER 0
#define VERT_BORDER 2
#define HORZ_GAP    2

#define STATUSBAR_GetInfoPtr(hwnd) ((STATUSWINDOWINFO *)GetWindowLongW (hwnd, 0))

/* prototype */
static void
STATUSBAR_SetPartBounds (STATUSWINDOWINFO *infoPtr);

static inline LPCSTR debugstr_t(LPCWSTR text, BOOL isW)
{
  return isW ? debugstr_w(text) : debugstr_a((LPCSTR)text);
}

static void
STATUSBAR_DrawSizeGrip (HDC hdc, LPRECT lpRect)
{
    HPEN hPenFace, hPenShadow, hPenHighlight, hOldPen;
    POINT pt;
    INT i;

    TRACE("draw size grip %ld,%ld - %ld,%ld\n", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);

    pt.x = lpRect->right - 1;
    pt.y = lpRect->bottom - 1;

    hPenFace = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DFACE ));
    hOldPen = SelectObject( hdc, hPenFace );
    MoveToEx (hdc, pt.x - 12, pt.y, NULL);
    LineTo (hdc, pt.x, pt.y);
    LineTo (hdc, pt.x, pt.y - 13);

    pt.x--;
    pt.y--;

    hPenShadow = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ));
    SelectObject( hdc, hPenShadow );
    for (i = 1; i < 11; i += 4) {
	MoveToEx (hdc, pt.x - i, pt.y, NULL);
	LineTo (hdc, pt.x + 1, pt.y - i - 1);

	MoveToEx (hdc, pt.x - i - 1, pt.y, NULL);
	LineTo (hdc, pt.x + 1, pt.y - i - 2);
    }

    hPenHighlight = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DHIGHLIGHT ));
    SelectObject( hdc, hPenHighlight );
    for (i = 3; i < 13; i += 4) {
	MoveToEx (hdc, pt.x - i, pt.y, NULL);
	LineTo (hdc, pt.x + 1, pt.y - i - 1);
    }

    SelectObject (hdc, hOldPen);
    DeleteObject( hPenFace );
    DeleteObject( hPenShadow );
    DeleteObject( hPenHighlight );
}


static void
STATUSBAR_DrawPart (HDC hdc, const STATUSWINDOWPART *part, const STATUSWINDOWINFO *infoPtr, int itemID)
{
    RECT r = part->bound;
    UINT border = BDR_SUNKENOUTER;

    TRACE("part bound %ld,%ld - %ld,%ld\n", r.left, r.top, r.right, r.bottom);
    if (part->style & SBT_POPOUT)
        border = BDR_RAISEDOUTER;
    else if (part->style & SBT_NOBORDERS)
        border = 0;

    DrawEdge(hdc, &r, border, BF_RECT|BF_ADJUST);

    if (part->style & SBT_OWNERDRAW)
	{
	    DRAWITEMSTRUCT dis;

	    dis.CtlID = GetWindowLongW (infoPtr->Self, GWL_ID);
	    dis.itemID = itemID;
	    dis.hwndItem = infoPtr->Self;
	    dis.hDC = hdc;
	    dis.rcItem = r;
	    dis.itemData = (INT)part->text;
	    SendMessageW (infoPtr->Notify, WM_DRAWITEM, (WPARAM)dis.CtlID, (LPARAM)&dis);
    }
    else
	{
	    if (part->hIcon)
		{
	        INT cy = r.bottom - r.top;

	        r.left += 2;
	        DrawIconEx (hdc, r.left, r.top, part->hIcon, cy, cy, 0, 0, DI_NORMAL);
	        r.left += cy;
		}
        DrawStatusTextW (hdc, &r, part->text, SBT_NOBORDERS);
    }
}


static void
STATUSBAR_RefreshPart (const STATUSWINDOWINFO *infoPtr, const STATUSWINDOWPART *part, HDC hdc, int itemID)
{
    HBRUSH hbrBk;
    HFONT  hOldFont;

    TRACE("item %d\n", itemID);
    if (!IsWindowVisible (infoPtr->Self))
        return;

    if (part->bound.right < part->bound.left) return;

    if (infoPtr->clrBk != CLR_DEFAULT)
	    hbrBk = CreateSolidBrush (infoPtr->clrBk);
    else
	    hbrBk = GetSysColorBrush (COLOR_3DFACE);
    FillRect(hdc, &part->bound, hbrBk);

    hOldFont = SelectObject (hdc, infoPtr->hFont ? infoPtr->hFont : infoPtr->hDefaultFont);

	STATUSBAR_DrawPart (hdc, part, infoPtr, itemID);

    SelectObject (hdc, hOldFont);

    if (infoPtr->clrBk != CLR_DEFAULT)
	    DeleteObject (hbrBk);

    if (GetWindowLongW (infoPtr->Self, GWL_STYLE) & SBARS_SIZEGRIP)
	{
	    RECT rect;

	    GetClientRect (infoPtr->Self, &rect);
	    STATUSBAR_DrawSizeGrip (hdc, &rect);
    }
}


static LRESULT
STATUSBAR_Refresh (STATUSWINDOWINFO *infoPtr, HDC hdc)
{
    int      i;
    RECT   rect;
    HBRUSH hbrBk;
    HFONT  hOldFont;

    TRACE("\n");
    if (!IsWindowVisible(infoPtr->Self))
        return 0;

    STATUSBAR_SetPartBounds(infoPtr);

    GetClientRect (infoPtr->Self, &rect);

    if (infoPtr->clrBk != CLR_DEFAULT)
	    hbrBk = CreateSolidBrush (infoPtr->clrBk);
    else
	    hbrBk = GetSysColorBrush (COLOR_3DFACE);
    FillRect(hdc, &rect, hbrBk);

    hOldFont = SelectObject (hdc, infoPtr->hFont ? infoPtr->hFont : infoPtr->hDefaultFont);

    if (infoPtr->simple) {
	    STATUSBAR_RefreshPart (infoPtr, &infoPtr->part0, hdc, 0);
    } else {
	    for (i = 0; i < infoPtr->numParts; i++) {
		    STATUSBAR_RefreshPart (infoPtr, &infoPtr->parts[i], hdc, i);
	    }
    }

    SelectObject (hdc, hOldFont);

    if (infoPtr->clrBk != CLR_DEFAULT)
	    DeleteObject (hbrBk);

    if (GetWindowLongW (infoPtr->Self, GWL_STYLE) & SBARS_SIZEGRIP)
	    STATUSBAR_DrawSizeGrip (hdc, &rect);

    return 0;
}


static void
STATUSBAR_SetPartBounds (STATUSWINDOWINFO *infoPtr)
{
    STATUSWINDOWPART *part;
    RECT rect, *r;
    int	i;

    /* get our window size */
    GetClientRect (infoPtr->Self, &rect);
    TRACE("client wnd size is %ld,%ld - %ld,%ld\n", rect.left, rect.top, rect.right, rect.bottom);

    rect.top += VERT_BORDER;

    /* set bounds for simple rectangle */
    infoPtr->part0.bound = rect;

    /* set bounds for non-simple rectangles */
    for (i = 0; i < infoPtr->numParts; i++) {
	part = &infoPtr->parts[i];
	r = &infoPtr->parts[i].bound;
	r->top = rect.top;
	r->bottom = rect.bottom;
	if (i == 0)
	    r->left = 0;
	else
	    r->left = infoPtr->parts[i-1].bound.right + HORZ_GAP;
	if (part->x == -1)
	    r->right = rect.right;
	else
	    r->right = part->x;

	if (infoPtr->hwndToolTip) {
	    TTTOOLINFOW ti;

	    ti.cbSize = sizeof(TTTOOLINFOW);
	    ti.hwnd = infoPtr->Self;
	    ti.uId = i;
	    ti.rect = *r;
	    SendMessageW (infoPtr->hwndToolTip, TTM_NEWTOOLRECTW,
			    0, (LPARAM)&ti);
	}
    }
}


static LRESULT
STATUSBAR_Relay2Tip (STATUSWINDOWINFO *infoPtr, UINT uMsg,
		     WPARAM wParam, LPARAM lParam)
{
    MSG msg;

    msg.hwnd = infoPtr->Self;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.time = GetMessageTime ();
    msg.pt.x = LOWORD(GetMessagePos ());
    msg.pt.y = HIWORD(GetMessagePos ());

    return SendMessageW (infoPtr->hwndToolTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
}


static BOOL
STATUSBAR_GetBorders (INT out[])
{
    TRACE("\n");
    out[0] = HORZ_BORDER; /* horizontal border width */
    out[1] = VERT_BORDER; /* vertical border width */
    out[2] = HORZ_GAP; /* width of border between rectangles */

    return TRUE;
}


static HICON
STATUSBAR_GetIcon (STATUSWINDOWINFO *infoPtr, INT nPart)
{
    TRACE("%d\n", nPart);
    /* MSDN says: "simple parts are indexed with -1" */
    if ((nPart < -1) || (nPart >= infoPtr->numParts))
	return 0;

    if (nPart == -1)
        return (infoPtr->part0.hIcon);
    else
        return (infoPtr->parts[nPart].hIcon);
}


static INT
STATUSBAR_GetParts (STATUSWINDOWINFO *infoPtr, INT num_parts, INT parts[])
{
    INT   i;

    TRACE("(%d)\n", num_parts);
    if (parts) {
	for (i = 0; i < num_parts; i++) {
	    parts[i] = infoPtr->parts[i].x;
	}
    }
    return infoPtr->numParts;
}


static BOOL
STATUSBAR_GetRect (STATUSWINDOWINFO *infoPtr, INT nPart, LPRECT rect)
{
    TRACE("part %d\n", nPart);
    if (infoPtr->simple)
	*rect = infoPtr->part0.bound;
    else
	*rect = infoPtr->parts[nPart].bound;
    return TRUE;
}


static LRESULT
STATUSBAR_GetTextA (STATUSWINDOWINFO *infoPtr, INT nPart, LPSTR buf)
{
    STATUSWINDOWPART *part;
    LRESULT result;

    TRACE("part %d\n", nPart);

    /* MSDN says: "simple parts use index of 0", so this check is ok. */
    if (nPart < 0 || nPart >= infoPtr->numParts) return 0;

    if (infoPtr->simple)
	part = &infoPtr->part0;
    else
	part = &infoPtr->parts[nPart];

    if (part->style & SBT_OWNERDRAW)
	result = (LRESULT)part->text;
    else {
        DWORD len = part->text ? WideCharToMultiByte( CP_ACP, 0, part->text, -1,
                                                      NULL, 0, NULL, NULL ) - 1 : 0;
        result = MAKELONG( len, part->style );
        if (part->text && buf)
            WideCharToMultiByte( CP_ACP, 0, part->text, -1, buf, len+1, NULL, NULL );
    }
    return result;
}


static LRESULT
STATUSBAR_GetTextW (STATUSWINDOWINFO *infoPtr, INT nPart, LPWSTR buf)
{
    STATUSWINDOWPART *part;
    LRESULT result;

    TRACE("part %d\n", nPart);
    if (nPart < 0 || nPart >= infoPtr->numParts) return 0;

    if (infoPtr->simple)
	part = &infoPtr->part0;
    else
	part = &infoPtr->parts[nPart];

    if (part->style & SBT_OWNERDRAW)
	result = (LRESULT)part->text;
    else {
	result = part->text ? strlenW (part->text) : 0;
	result |= (part->style << 16);
	if (part->text && buf)
	    strcpyW (buf, part->text);
    }
    return result;
}


static LRESULT
STATUSBAR_GetTextLength (STATUSWINDOWINFO *infoPtr, INT nPart)
{
    STATUSWINDOWPART *part;
    DWORD result;

    TRACE("part %d\n", nPart);

    /* MSDN says: "simple parts use index of 0", so this check is ok. */
    if (nPart < 0 || nPart >= infoPtr->numParts) return 0;

    if (infoPtr->simple)
	part = &infoPtr->part0;
    else
	part = &infoPtr->parts[nPart];

    if (part->text)
	result = strlenW(part->text);
    else
	result = 0;

    result |= (part->style << 16);
    return result;
}

static LRESULT
STATUSBAR_GetTipTextA (STATUSWINDOWINFO *infoPtr, INT id, LPSTR tip, INT size)
{
    TRACE("\n");
    if (tip) {
        CHAR buf[INFOTIPSIZE];
        buf[0]='\0';

        if (infoPtr->hwndToolTip) {
            TTTOOLINFOA ti;
            ti.cbSize = sizeof(TTTOOLINFOA);
            ti.hwnd = infoPtr->Self;
            ti.uId = id;
            ti.lpszText = buf;
            SendMessageA (infoPtr->hwndToolTip, TTM_GETTEXTA, 0, (LPARAM)&ti);
        }
        lstrcpynA (tip, buf, size);
    }
    return 0;
}


static LRESULT
STATUSBAR_GetTipTextW (STATUSWINDOWINFO *infoPtr, INT id, LPWSTR tip, INT size)
{
    TRACE("\n");
    if (tip) {
        WCHAR buf[INFOTIPSIZE];
        buf[0]=0;

	if (infoPtr->hwndToolTip) {
	    TTTOOLINFOW ti;
	    ti.cbSize = sizeof(TTTOOLINFOW);
	    ti.hwnd = infoPtr->Self;
	    ti.uId = id;
            ti.lpszText = buf;
	    SendMessageW(infoPtr->hwndToolTip, TTM_GETTEXTW, 0, (LPARAM)&ti);
	}
	lstrcpynW(tip, buf, size);
    }

    return 0;
}


static COLORREF
STATUSBAR_SetBkColor (STATUSWINDOWINFO *infoPtr, COLORREF color)
{
    COLORREF oldBkColor;

    oldBkColor = infoPtr->clrBk;
    infoPtr->clrBk = color;
    InvalidateRect(infoPtr->Self, NULL, FALSE);

    TRACE("CREF: %08lx -> %08lx\n", oldBkColor, infoPtr->clrBk);
    return oldBkColor;
}


static BOOL
STATUSBAR_SetIcon (STATUSWINDOWINFO *infoPtr, INT nPart, HICON hIcon)
{
    if ((nPart < -1) || (nPart >= infoPtr->numParts))
	return FALSE;

    TRACE("setting part %d\n", nPart);

    /* FIXME: MSDN says "if nPart is -1, the status bar is assumed simple" */
    if (nPart == -1) {
	if (infoPtr->part0.hIcon == hIcon) /* same as - no redraw */
	    return TRUE;
	infoPtr->part0.hIcon = hIcon;
	if (infoPtr->simple)
            InvalidateRect(infoPtr->Self, &infoPtr->part0.bound, FALSE);
    } else {
	if (infoPtr->parts[nPart].hIcon == hIcon) /* same as - no redraw */
	    return TRUE;

	infoPtr->parts[nPart].hIcon = hIcon;
	if (!(infoPtr->simple))
            InvalidateRect(infoPtr->Self, &infoPtr->parts[nPart].bound, FALSE);
    }
    return TRUE;
}


static BOOL
STATUSBAR_SetMinHeight (STATUSWINDOWINFO *infoPtr, INT height)
{

    TRACE("(height=%d)\n", height);
    if (IsWindowVisible (infoPtr->Self)) {
	INT  width, x, y;
	RECT parent_rect;

	GetClientRect (infoPtr->Notify, &parent_rect);
	infoPtr->height = height + VERT_BORDER;
	width = parent_rect.right - parent_rect.left;
	x = parent_rect.left;
	y = parent_rect.bottom - infoPtr->height;
	MoveWindow (infoPtr->Self, parent_rect.left,
		      parent_rect.bottom - infoPtr->height,
		      width, infoPtr->height, TRUE);
	STATUSBAR_SetPartBounds (infoPtr);
    }

    return TRUE;
}


static BOOL
STATUSBAR_SetParts (STATUSWINDOWINFO *infoPtr, INT count, LPINT parts)
{
    STATUSWINDOWPART *tmp;
    int	i, oldNumParts;

    TRACE("(%d,%p)\n", count, parts);

    oldNumParts = infoPtr->numParts;
    infoPtr->numParts = count;
    if (oldNumParts > infoPtr->numParts) {
	for (i = infoPtr->numParts ; i < oldNumParts; i++) {
	    if (infoPtr->parts[i].text && !(infoPtr->parts[i].style & SBT_OWNERDRAW))
		Free (infoPtr->parts[i].text);
	}
    } else if (oldNumParts < infoPtr->numParts) {
	tmp = Alloc (sizeof(STATUSWINDOWPART) * infoPtr->numParts);
	if (!tmp) return FALSE;
	for (i = 0; i < oldNumParts; i++) {
	    tmp[i] = infoPtr->parts[i];
	}
	if (infoPtr->parts)
	    Free (infoPtr->parts);
	infoPtr->parts = tmp;
    }
    if (oldNumParts == infoPtr->numParts) {
	for (i=0; i < oldNumParts; i++)
	    if (infoPtr->parts[i].x != parts[i])
		break;
	if (i==oldNumParts) /* Unchanged? no need to redraw! */
	    return TRUE;
    }

    for (i = 0; i < infoPtr->numParts; i++)
	infoPtr->parts[i].x = parts[i];

    if (infoPtr->hwndToolTip) {
	INT nTipCount, i;
	TTTOOLINFOW ti;

	ZeroMemory (&ti, sizeof(TTTOOLINFOW));
	ti.cbSize = sizeof(TTTOOLINFOW);
	ti.hwnd = infoPtr->Self;

	nTipCount = SendMessageW (infoPtr->hwndToolTip, TTM_GETTOOLCOUNT, 0, 0);
	if (nTipCount < infoPtr->numParts) {
	    /* add tools */
	    for (i = nTipCount; i < infoPtr->numParts; i++) {
		TRACE("add tool %d\n", i);
		ti.uId = i;
		SendMessageW (infoPtr->hwndToolTip, TTM_ADDTOOLW,
				0, (LPARAM)&ti);
	    }
	}
	else if (nTipCount > infoPtr->numParts) {
	    /* delete tools */
	    for (i = nTipCount - 1; i >= infoPtr->numParts; i--) {
		TRACE("delete tool %d\n", i);
		ti.uId = i;
		SendMessageW (infoPtr->hwndToolTip, TTM_DELTOOLW,
				0, (LPARAM)&ti);
	    }
	}
    }
    STATUSBAR_SetPartBounds (infoPtr);
    InvalidateRect(infoPtr->Self, NULL, FALSE);
    return TRUE;
}


static BOOL
STATUSBAR_SetTextT (STATUSWINDOWINFO *infoPtr, INT nPart, WORD style,
		    LPCWSTR text, BOOL isW)
{
    STATUSWINDOWPART *part=NULL;
    BOOL changed = FALSE;

    if (style & SBT_OWNERDRAW) {
         TRACE("part %d, text %p\n",nPart,text);
    }
    else TRACE("part %d, text %s\n", nPart, debugstr_t(text, isW));

    /* MSDN says: "If the parameter is set to SB_SIMPLEID (255), the status
     * window is assumed to be a simple window */

    if (nPart == 0x00ff) {
	part = &infoPtr->part0;
    } else {
	if (infoPtr->parts && nPart >= 0 && nPart < infoPtr->numParts) {
	    part = &infoPtr->parts[nPart];
	}
    }
    if (!part) return FALSE;

    if (part->style != style)
	changed = TRUE;

    part->style = style;
    if (style & SBT_OWNERDRAW) {
	if (part->text == text)
	    return TRUE;
	part->text = (LPWSTR)text;
    } else {
	LPWSTR ntext;

	if (text && !isW) {
	    LPCSTR atxt = (LPCSTR)text;
            DWORD len = MultiByteToWideChar( CP_ACP, 0, atxt, -1, NULL, 0 );
	    ntext = Alloc( (len + 1)*sizeof(WCHAR) );
	    if (!ntext) return FALSE;
            MultiByteToWideChar( CP_ACP, 0, atxt, -1, ntext, len );
	} else if (text) {
	    ntext = Alloc( (strlenW(text) + 1)*sizeof(WCHAR) );
	    if (!ntext) return FALSE;
	    strcpyW (ntext, text);
	} else ntext = 0;

	/* check if text is unchanged -> no need to redraw */
	if (text) {
	    if (!changed && part->text && !lstrcmpW(ntext, part->text)) {
		if (!isW) Free(ntext);
		return TRUE;
	    }
	} else {
	    if (!changed && !part->text)
		return TRUE;
	}

	if (part->text)
	    Free (part->text);
	part->text = ntext;
    }
    InvalidateRect(infoPtr->Self, &part->bound, FALSE);

    return TRUE;
}


static LRESULT
STATUSBAR_SetTipTextA (STATUSWINDOWINFO *infoPtr, INT id, LPSTR text)
{
    TRACE("part %d: \"%s\"\n", id, text);
    if (infoPtr->hwndToolTip) {
	TTTOOLINFOA ti;
	ti.cbSize = sizeof(TTTOOLINFOA);
	ti.hwnd = infoPtr->Self;
	ti.uId = id;
	ti.hinst = 0;
	ti.lpszText = text;
	SendMessageA (infoPtr->hwndToolTip, TTM_UPDATETIPTEXTA,
			0, (LPARAM)&ti);
    }

    return 0;
}


static LRESULT
STATUSBAR_SetTipTextW (STATUSWINDOWINFO *infoPtr, INT id, LPWSTR text)
{
    TRACE("part %d: \"%s\"\n", id, debugstr_w(text));
    if (infoPtr->hwndToolTip) {
	TTTOOLINFOW ti;
	ti.cbSize = sizeof(TTTOOLINFOW);
	ti.hwnd = infoPtr->Self;
	ti.uId = id;
	ti.hinst = 0;
	ti.lpszText = text;
	SendMessageW (infoPtr->hwndToolTip, TTM_UPDATETIPTEXTW,
			0, (LPARAM)&ti);
    }

    return 0;
}


inline static LRESULT
STATUSBAR_SetUnicodeFormat (STATUSWINDOWINFO *infoPtr, BOOL bUnicode)
{
    BOOL bOld = infoPtr->bUnicode;

    TRACE("(0x%x)\n", bUnicode);
    infoPtr->bUnicode = bUnicode;

    return bOld;
}


static BOOL
STATUSBAR_Simple (STATUSWINDOWINFO *infoPtr, BOOL simple)
{
    NMHDR  nmhdr;

    TRACE("(simple=%d)\n", simple);
    if (infoPtr->simple == simple) /* no need to change */
	return TRUE;

    infoPtr->simple = simple;

    /* send notification */
    nmhdr.hwndFrom = infoPtr->Self;
    nmhdr.idFrom = GetWindowLongW (infoPtr->Self, GWL_ID);
    nmhdr.code = SBN_SIMPLEMODECHANGE;
    SendMessageW (infoPtr->Notify, WM_NOTIFY, 0, (LPARAM)&nmhdr);
    InvalidateRect(infoPtr->Self, NULL, FALSE);
    return TRUE;
}


static LRESULT
STATUSBAR_WMDestroy (STATUSWINDOWINFO *infoPtr)
{
    int	i;

    TRACE("\n");
    for (i = 0; i < infoPtr->numParts; i++) {
	if (infoPtr->parts[i].text && !(infoPtr->parts[i].style & SBT_OWNERDRAW))
	    Free (infoPtr->parts[i].text);
    }
    if (infoPtr->part0.text && !(infoPtr->part0.style & SBT_OWNERDRAW))
	Free (infoPtr->part0.text);
    Free (infoPtr->parts);

    /* delete default font */
    if (infoPtr->hDefaultFont)
	DeleteObject (infoPtr->hDefaultFont);

    /* delete tool tip control */
    if (infoPtr->hwndToolTip)
	DestroyWindow (infoPtr->hwndToolTip);

    SetWindowLongW(infoPtr->Self, 0, 0);
    Free (infoPtr);
    return 0;
}


static LRESULT
STATUSBAR_WMCreate (HWND hwnd, LPCREATESTRUCTA lpCreate)
{
    STATUSWINDOWINFO *infoPtr;
    NONCLIENTMETRICSW nclm;
    DWORD dwStyle;
    RECT rect;
    int	i, width, len, textHeight = 0;
    HDC	hdc;

    TRACE("\n");
    infoPtr = (STATUSWINDOWINFO*)Alloc (sizeof(STATUSWINDOWINFO));
    if (!infoPtr) goto create_fail;
    SetWindowLongW (hwnd, 0, (DWORD)infoPtr);

    infoPtr->Self = hwnd;
    infoPtr->Notify = lpCreate->hwndParent;
    infoPtr->numParts = 1;
    infoPtr->parts = 0;
    infoPtr->simple = FALSE;
    infoPtr->clrBk = CLR_DEFAULT;
    infoPtr->hFont = 0;

    i = SendMessageW(infoPtr->Notify, WM_NOTIFYFORMAT, (WPARAM)hwnd, NF_QUERY);
    infoPtr->NtfUnicode = (i == NFR_UNICODE);

    GetClientRect (hwnd, &rect);
    InvalidateRect (hwnd, &rect, 0);
    UpdateWindow(hwnd);

    ZeroMemory (&nclm, sizeof(nclm));
    nclm.cbSize = sizeof(nclm);
    SystemParametersInfoW (SPI_GETNONCLIENTMETRICS, nclm.cbSize, &nclm, 0);
    infoPtr->hDefaultFont = CreateFontIndirectW (&nclm.lfStatusFont);

    /* initialize simple case */
    infoPtr->part0.bound = rect;
    infoPtr->part0.text = 0;
    infoPtr->part0.x = 0;
    infoPtr->part0.style = 0;
    infoPtr->part0.hIcon = 0;

    /* initialize first part */
    infoPtr->parts = Alloc (sizeof(STATUSWINDOWPART));
    if (!infoPtr->parts) goto create_fail;
    infoPtr->parts[0].bound = rect;
    infoPtr->parts[0].text = 0;
    infoPtr->parts[0].x = -1;
    infoPtr->parts[0].style = 0;
    infoPtr->parts[0].hIcon = 0;

    if (IsWindowUnicode (hwnd)) {
	infoPtr->bUnicode = TRUE;
	if (lpCreate->lpszName &&
	    (len = strlenW ((LPCWSTR)lpCreate->lpszName))) {
	    infoPtr->parts[0].text = Alloc ((len + 1)*sizeof(WCHAR));
	    if (!infoPtr->parts[0].text) goto create_fail;
	    strcpyW (infoPtr->parts[0].text, (LPCWSTR)lpCreate->lpszName);
	}
    }
    else {
	if (lpCreate->lpszName &&
	    (len = strlen((LPCSTR)lpCreate->lpszName))) {
            DWORD lenW = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)lpCreate->lpszName, -1, NULL, 0 );
	    infoPtr->parts[0].text = Alloc (lenW*sizeof(WCHAR));
	    if (!infoPtr->parts[0].text) goto create_fail;
            MultiByteToWideChar( CP_ACP, 0, (LPCSTR)lpCreate->lpszName, -1,
                                 infoPtr->parts[0].text, lenW );
	}
    }

    dwStyle = GetWindowLongW (hwnd, GWL_STYLE);

#ifndef __WIN32OS2__
    /* statusbars on managed windows should not have SIZEGRIP style */
    if ((dwStyle & SBARS_SIZEGRIP) && lpCreate->hwndParent)
        if (GetWindowLongW (lpCreate->hwndParent, GWL_EXSTYLE) & WS_EX_MANAGED)
            SetWindowLongW (hwnd, GWL_STYLE, dwStyle & ~SBARS_SIZEGRIP);
#endif

    if ((hdc = GetDC (hwnd))) {
	TEXTMETRICW tm;
	HFONT hOldFont;

	hOldFont = SelectObject (hdc, infoPtr->hDefaultFont);
	GetTextMetricsW (hdc, &tm);
	textHeight = tm.tmHeight;
	SelectObject (hdc, hOldFont);
	ReleaseDC (hwnd, hdc);
    }
    TRACE("    textHeight=%d\n", textHeight);

    if (dwStyle & SBT_TOOLTIPS) {
	infoPtr->hwndToolTip =
	    CreateWindowExW (0, TOOLTIPS_CLASSW, NULL, 0,
			     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			     CW_USEDEFAULT, hwnd, 0,
			     (HINSTANCE)GetWindowLongW(hwnd, GWL_HINSTANCE), NULL);

	if (infoPtr->hwndToolTip) {
	    NMTOOLTIPSCREATED nmttc;

	    nmttc.hdr.hwndFrom = hwnd;
	    nmttc.hdr.idFrom = GetWindowLongW (hwnd, GWL_ID);
	    nmttc.hdr.code = NM_TOOLTIPSCREATED;
	    nmttc.hwndToolTips = infoPtr->hwndToolTip;

	    SendMessageW (lpCreate->hwndParent, WM_NOTIFY,
			    (WPARAM)nmttc.hdr.idFrom, (LPARAM)&nmttc);
	}
    }

    if (!(dwStyle & CCS_NORESIZE)) { /* don't resize wnd if it doesn't want it ! */
        GetClientRect (infoPtr->Notify, &rect);
        width = rect.right - rect.left;
        infoPtr->height = textHeight + 4 + VERT_BORDER;
        SetWindowPos(hwnd, 0, lpCreate->x, lpCreate->y - 1,
			width, infoPtr->height, SWP_NOZORDER);
        STATUSBAR_SetPartBounds (infoPtr);
    }

    return 0;

create_fail:
    TRACE("    failed!\n");
    if (infoPtr) STATUSBAR_WMDestroy(infoPtr);
    return -1;
}


/* in contrast to SB_GETTEXT*, WM_GETTEXT handles the text
 * of the first part only (usual behaviour) */
static INT
STATUSBAR_WMGetText (STATUSWINDOWINFO *infoPtr, INT size, LPWSTR buf)
{
    INT len;

    TRACE("\n");
    if (!(infoPtr->parts[0].text))
        return 0;
    if (infoPtr->bUnicode)
        len = strlenW (infoPtr->parts[0].text);
    else
        len = WideCharToMultiByte( CP_ACP, 0, infoPtr->parts[0].text, -1, NULL, 0, NULL, NULL )-1;

    if (size > len) {
	if (infoPtr->bUnicode)
	    strcpyW (buf, infoPtr->parts[0].text);
	else
            WideCharToMultiByte( CP_ACP, 0, infoPtr->parts[0].text, -1,
                                 (LPSTR)buf, len+1, NULL, NULL );
	return len;
    }

    return -1;
}


static BOOL
STATUSBAR_WMNCHitTest (STATUSWINDOWINFO *infoPtr, INT x, INT y)
{
    if (GetWindowLongW (infoPtr->Self, GWL_STYLE) & SBARS_SIZEGRIP) {
	RECT  rect;
	POINT pt;

	GetClientRect (infoPtr->Self, &rect);

	pt.x = x;
	pt.y = y;
	ScreenToClient (infoPtr->Self, &pt);

	rect.left = rect.right - 13;
	rect.top += 2;

	if (PtInRect (&rect, pt))
	    return HTBOTTOMRIGHT;
    }

    return HTERROR;
}


static LRESULT
STATUSBAR_WMPaint (STATUSWINDOWINFO *infoPtr, HDC hdc)
{
    PAINTSTRUCT ps;

    TRACE("\n");
    if (hdc) return STATUSBAR_Refresh (infoPtr, hdc);
    hdc = BeginPaint (infoPtr->Self, &ps);
    STATUSBAR_Refresh (infoPtr, hdc);
    EndPaint (infoPtr->Self, &ps);

    return 0;
}


static LRESULT
STATUSBAR_WMSetFont (STATUSWINDOWINFO *infoPtr, HFONT font, BOOL redraw)
{
    infoPtr->hFont = font;
    TRACE("%p\n", infoPtr->hFont);
    if (redraw)
        InvalidateRect(infoPtr->Self, NULL, FALSE);

    return 0;
}


static BOOL
STATUSBAR_WMSetText (STATUSWINDOWINFO *infoPtr, LPCSTR text)
{
    STATUSWINDOWPART *part;
    int len;

    TRACE("\n");
    if (infoPtr->numParts == 0)
	return FALSE;

    part = &infoPtr->parts[0];
    /* duplicate string */
    if (part->text)
        Free (part->text);
    part->text = 0;
    if (infoPtr->bUnicode) {
	if (text && (len = strlenW((LPCWSTR)text))) {
	    part->text = Alloc ((len+1)*sizeof(WCHAR));
	    if (!part->text) return FALSE;
	    strcpyW (part->text, (LPCWSTR)text);
	}
    }
    else {
	if (text && (len = lstrlenA(text))) {
            DWORD lenW = MultiByteToWideChar( CP_ACP, 0, text, -1, NULL, 0 );
            part->text = Alloc (lenW*sizeof(WCHAR));
	    if (!part->text) return FALSE;
            MultiByteToWideChar( CP_ACP, 0, text, -1, part->text, lenW );
	}
    }

    InvalidateRect(infoPtr->Self, &part->bound, FALSE);

    return TRUE;
}


static BOOL
#ifdef __WIN32OS2__
STATUSBAR_WMSize (STATUSWINDOWINFO *infoPtr, WORD flags, LPARAM lParam)
#else
STATUSBAR_WMSize (STATUSWINDOWINFO *infoPtr, WORD flags)
#endif
{
    INT  width, x, y;
    RECT parent_rect;

    /* Need to resize width to match parent */
    TRACE("flags %04x\n", flags);

    if (flags != SIZE_RESTORED && flags != SIZE_MAXIMIZED)
    {
	WARN("flags MUST be SIZE_RESTORED or SIZE_MAXIMIZED\n");
	return FALSE;
    }

#ifdef __WIN32OS2__
    if (GetWindowLongW(infoPtr->Self, GWL_STYLE) & CCS_NORESIZE) {
        //@@PF At least do invalidate to erase garbage
        InvalidateRect(infoPtr->Self,NULL,TRUE);
        return FALSE;
    }

    /* Invalidate the window if we need to draw a size grip */
    /* TODO: The size grip area would be sufficient though */
    if (GetWindowLongW (infoPtr->Self, GWL_STYLE) & SBARS_SIZEGRIP) 
    {
        RECT rcWin;

        GetClientRect(infoPtr->Self, &rcWin);
        if(HIWORD(lParam) != (rcWin.bottom - rcWin.top) ||
           LOWORD(lParam) != (rcWin.right  - rcWin.left))
        {
            TRACE("SBARS_SIZEGRIP && height/width changed -> invalidate window");
            InvalidateRect(infoPtr->Self,NULL,TRUE);
        }
    }
#else
    if (GetWindowLongW(infoPtr->Self, GWL_STYLE) & CCS_NORESIZE) return FALSE;
#endif

    /* width and height don't apply */
    GetClientRect (infoPtr->Notify, &parent_rect);
    width = parent_rect.right - parent_rect.left;
    x = parent_rect.left;
    y = parent_rect.bottom - infoPtr->height;
    MoveWindow (infoPtr->Self, parent_rect.left,
		parent_rect.bottom - infoPtr->height,
		width, infoPtr->height, TRUE);
    STATUSBAR_SetPartBounds (infoPtr);
    return TRUE;
}


static LRESULT
STATUSBAR_NotifyFormat (STATUSWINDOWINFO *infoPtr, HWND from, INT cmd)
{
    if (cmd == NF_REQUERY) {
	INT i = SendMessageW(from, WM_NOTIFYFORMAT, (WPARAM)infoPtr->Self, NF_QUERY);
	infoPtr->NtfUnicode = (i == NFR_UNICODE);
    }
    return infoPtr->NtfUnicode ? NFR_UNICODE : NFR_ANSI;
}


static LRESULT
STATUSBAR_SendNotify (HWND hwnd, UINT code)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr(hwnd);
    NMHDR  nmhdr;

    TRACE("code %04x\n", code);
    nmhdr.hwndFrom = hwnd;
    nmhdr.idFrom = GetWindowLongW (hwnd, GWL_ID);
    nmhdr.code = code;
    SendMessageW (infoPtr->Notify, WM_NOTIFY, 0, (LPARAM)&nmhdr);
    return 0;
}



static LRESULT WINAPI
StatusWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr(hwnd);
    INT nPart = ((INT) wParam) & 0x00ff;
    LRESULT res;

    TRACE("hwnd=%p msg=%x wparam=%x lparam=%lx\n", hwnd, msg, wParam, lParam);
    if (!infoPtr && msg != WM_CREATE)
        return DefWindowProcW (hwnd, msg, wParam, lParam);

    switch (msg) {
	case SB_GETBORDERS:
	    return STATUSBAR_GetBorders ((INT *)lParam);

	case SB_GETICON:
	    return (LRESULT)STATUSBAR_GetIcon (infoPtr, nPart);

	case SB_GETPARTS:
	    return STATUSBAR_GetParts (infoPtr, (INT)wParam, (INT *)lParam);

	case SB_GETRECT:
	    return STATUSBAR_GetRect (infoPtr, nPart, (LPRECT)lParam);

	case SB_GETTEXTA:
	    return STATUSBAR_GetTextA (infoPtr, nPart, (LPSTR)lParam);

	case SB_GETTEXTW:
	    return STATUSBAR_GetTextW (infoPtr, nPart, (LPWSTR)lParam);

	case SB_GETTEXTLENGTHA:
	case SB_GETTEXTLENGTHW:
	    return STATUSBAR_GetTextLength (infoPtr, nPart);

	case SB_GETTIPTEXTA:
	    return STATUSBAR_GetTipTextA (infoPtr,  LOWORD(wParam), (LPSTR)lParam,  HIWORD(wParam));

	case SB_GETTIPTEXTW:
	    return STATUSBAR_GetTipTextW (infoPtr,  LOWORD(wParam), (LPWSTR)lParam,  HIWORD(wParam));

	case SB_GETUNICODEFORMAT:
	    return infoPtr->bUnicode;

	case SB_ISSIMPLE:
	    return infoPtr->simple;

	case SB_SETBKCOLOR:
	    return STATUSBAR_SetBkColor (infoPtr, (COLORREF)lParam);

	case SB_SETICON:
	    return STATUSBAR_SetIcon (infoPtr, nPart, (HICON)lParam);

	case SB_SETMINHEIGHT:
	    return STATUSBAR_SetMinHeight (infoPtr, (INT)wParam);

	case SB_SETPARTS:
	    return STATUSBAR_SetParts (infoPtr, (INT)wParam, (LPINT)lParam);

	case SB_SETTEXTA:
	    return STATUSBAR_SetTextT (infoPtr, nPart, wParam & 0xff00, (LPCWSTR)lParam, FALSE);

	case SB_SETTEXTW:
	    return STATUSBAR_SetTextT (infoPtr, nPart, wParam & 0xff00, (LPCWSTR)lParam, TRUE);

	case SB_SETTIPTEXTA:
	    return STATUSBAR_SetTipTextA (infoPtr, (INT)wParam, (LPSTR)lParam);

	case SB_SETTIPTEXTW:
	    return STATUSBAR_SetTipTextW (infoPtr, (INT)wParam, (LPWSTR)lParam);

	case SB_SETUNICODEFORMAT:
	    return STATUSBAR_SetUnicodeFormat (infoPtr, (BOOL)wParam);

	case SB_SIMPLE:
	    return STATUSBAR_Simple (infoPtr, (BOOL)wParam);

	case WM_CREATE:
	    return STATUSBAR_WMCreate (hwnd, (LPCREATESTRUCTA)lParam);

	case WM_DESTROY:
	    return STATUSBAR_WMDestroy (infoPtr);

	case WM_GETFONT:
	    return (LRESULT)(infoPtr->hFont? infoPtr->hFont : infoPtr->hDefaultFont);

	case WM_GETTEXT:
            return STATUSBAR_WMGetText (infoPtr, (INT)wParam, (LPWSTR)lParam);

	case WM_GETTEXTLENGTH:
	    return STATUSBAR_GetTextLength (infoPtr, 0);

	case WM_LBUTTONDBLCLK:
            return STATUSBAR_SendNotify (hwnd, NM_DBLCLK);

	case WM_LBUTTONUP:
	    return STATUSBAR_SendNotify (hwnd, NM_CLICK);

	case WM_MOUSEMOVE:
	    return STATUSBAR_Relay2Tip (infoPtr, msg, wParam, lParam);

	case WM_NCHITTEST:
	    res = STATUSBAR_WMNCHitTest(infoPtr, (INT)LOWORD(lParam),
			                (INT)HIWORD(lParam));
	    if (res != HTERROR) return res;
	    return DefWindowProcW (hwnd, msg, wParam, lParam);

	case WM_NCLBUTTONUP:
	case WM_NCLBUTTONDOWN:
    	    PostMessageW (infoPtr->Notify, msg, wParam, lParam);
	    return 0;

	case WM_NOTIFYFORMAT:
	    return STATUSBAR_NotifyFormat(infoPtr, (HWND)wParam, (INT)lParam);

	case WM_PAINT:
	    return STATUSBAR_WMPaint (infoPtr, (HDC)wParam);

	case WM_RBUTTONDBLCLK:
	    return STATUSBAR_SendNotify (hwnd, NM_RDBLCLK);

	case WM_RBUTTONUP:
	    return STATUSBAR_SendNotify (hwnd, NM_RCLICK);

	case WM_SETFONT:
	    return STATUSBAR_WMSetFont (infoPtr, (HFONT)wParam, LOWORD(lParam));

	case WM_SETTEXT:
	    return STATUSBAR_WMSetText (infoPtr, (LPCSTR)lParam);

	case WM_SIZE:
#ifdef __WIN32OS2__
	    if (STATUSBAR_WMSize (infoPtr, (WORD)wParam, lParam)) return 0;
#else
	    if (STATUSBAR_WMSize (infoPtr, (WORD)wParam)) return 0;
#endif
            return DefWindowProcW (hwnd, msg, wParam, lParam);

	default:
	    if ((msg >= WM_USER) && (msg < WM_APP))
		ERR("unknown msg %04x wp=%04x lp=%08lx\n",
		     msg, wParam, lParam);
	    return DefWindowProcW (hwnd, msg, wParam, lParam);
    }
    return 0;
}


/***********************************************************************
 * STATUS_Register [Internal]
 *
 * Registers the status window class.
 */

void
STATUS_Register (void)
{
    WNDCLASSW wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSW));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS | CS_VREDRAW;
    wndClass.lpfnWndProc   = (WNDPROC)StatusWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(STATUSWINDOWINFO *);
    wndClass.hCursor       = LoadCursorW (0, (LPWSTR)IDC_ARROWW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wndClass.lpszClassName = STATUSCLASSNAMEW;

    RegisterClassW (&wndClass);
}


/***********************************************************************
 * STATUS_Unregister [Internal]
 *
 * Unregisters the status window class.
 */

void
STATUS_Unregister (void)
{
    UnregisterClassW (STATUSCLASSNAMEW, NULL);
}
