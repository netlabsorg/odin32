/* $Id: status.cpp,v 1.2 2000-03-18 16:17:29 cbratschi Exp $ */
/*
 * Interface code to StatusWindow widget/control
 *
 * Copyright 1996 Bruce Milner
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 */

/* WINE 990923 level */

#include "winbase.h"
#include "commctrl.h"
#include "comctl32.h"
#include "ccbase.h"
#include "status.h"
#include <string.h>

/*
 * Run tests using Waite Group Windows95 API Bible Vol. 1&2
 * The second cdrom contains executables drawstat.exe,gettext.exe,
 * simple.exe, getparts.exe, setparts.exe, statwnd.exe
 */

/*
 * Fixme/Todo
 * 1) Don't hard code bar to bottom of window, allow CCS_TOP also.
 * 2) Tooltip support (almost done).
 */

/* CB: Odin problems
 - DrawText: DT_VCENTER doesn't work (DT_SINGLELINE bug?)
*/

#define _MAX(a,b) (((a)>(b))?(a):(b))
#define _MIN(a,b) (((a)>(b))?(b):(a))

#define HORZ_BORDER 0
#define VERT_BORDER 2
#define VERT_SPACE  2 //space between border and text
#define HORZ_GAP    2
#define TOP_MARGIN  2
#define ICON_SPACE  2
#define TEXT_SPACE  3

#define STATUSBAR_GetInfoPtr(hwnd) ((STATUSWINDOWINFO*)getInfoPtr(hwnd))


static RECT STATUSBAR_GetSizeBox(HWND hwnd)
{
  RECT rect;

  GetClientRect(hwnd,&rect);
  rect.left = rect.right-GetSystemMetrics(SM_CXVSCROLL);
  rect.top  = rect.bottom-GetSystemMetrics(SM_CYHSCROLL);

  return rect;
}

static void
STATUSBAR_DrawSizeGrip (HDC hdc, LPRECT lpRect)
{
    HPEN hOldPen;
    POINT pt;
    INT i;

    pt.x = lpRect->right - 1;
    pt.y = lpRect->bottom - 1;

    hOldPen = SelectObject (hdc, GetSysColorPen (COLOR_3DFACE));
    MoveToEx (hdc, pt.x - 12, pt.y, NULL);
    LineTo (hdc, pt.x, pt.y);
    LineTo (hdc, pt.x, pt.y - 12);

    pt.x--;
    pt.y--;

    SelectObject (hdc, GetSysColorPen (COLOR_3DSHADOW));
    for (i = 1; i < 11; i += 4) {
        MoveToEx (hdc, pt.x - i, pt.y, NULL);
        LineTo (hdc, pt.x, pt.y - i);

        MoveToEx (hdc, pt.x - i-1, pt.y, NULL);
        LineTo (hdc, pt.x, pt.y - i-1);
    }

    SelectObject (hdc, GetSysColorPen (COLOR_3DHIGHLIGHT));
    for (i = 3; i < 13; i += 4) {
        MoveToEx (hdc, pt.x - i, pt.y, NULL);
        LineTo (hdc, pt.x, pt.y - i);
    }

    SelectObject (hdc, hOldPen);
}


static void
STATUSBAR_DrawPart (HDC hdc, STATUSWINDOWPART *part)
{
    RECT r = part->bound;
    UINT border = BDR_SUNKENOUTER;

    if (part->style & SBT_POPOUT)
      border = BDR_RAISEDOUTER;
    else if (part->style & SBT_NOBORDERS)
      border = 0;

    DrawEdge(hdc, &r, border, BF_RECT|BF_ADJUST);

    r.bottom -= VERT_SPACE;
    r.top += VERT_SPACE;

    /* draw the icon */
    if (part->hIcon) {
        INT cy = r.bottom - r.top;

        r.left += ICON_SPACE;
        DrawIconEx (hdc, r.left, r.top, part->hIcon, cy, cy, 0, 0, DI_NORMAL);
        r.left += cy;
    }

    /* now draw text */
    if (part->text) {
      int oldbkmode = SetBkMode(hdc, TRANSPARENT);
      LPWSTR p = (LPWSTR)part->text;
      UINT align = DT_LEFT;
      if (*p == L'\t') {
        p++;
        align = DT_CENTER;

        if (*p == L'\t') {
          p++;
          align = DT_RIGHT;
        }
      }
      r.left += TEXT_SPACE;
      DrawTextW (hdc, p, lstrlenW (p), &r, align|DT_VCENTER|DT_SINGLELINE);
      if (oldbkmode != TRANSPARENT)
        SetBkMode(hdc, oldbkmode);
    }
}


static VOID
STATUSBAR_RefreshPart (HWND hwnd, STATUSWINDOWPART *part, HDC hdc)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    HBRUSH hbrBk;
    HFONT  hOldFont;

    if (!IsWindowVisible (hwnd))
        return;

    if (self->clrBk != CLR_DEFAULT)
        hbrBk = CreateSolidBrush (self->clrBk);
    else
        hbrBk = GetSysColorBrush (COLOR_3DFACE);
    FillRect(hdc, &part->bound, hbrBk);

    hOldFont = SelectObject (hdc, self->hFont ? self->hFont : self->hDefaultFont);

    if (part->style & SBT_OWNERDRAW) {
        DRAWITEMSTRUCT dis;

        dis.CtlID = GetWindowLongA (hwnd, GWL_ID);
        dis.itemID = -1;
        dis.hwndItem = hwnd;
        dis.hDC = hdc;
        dis.rcItem = part->bound;
        dis.itemData = (INT)part->text;
        SendMessageA (GetParent (hwnd), WM_DRAWITEM,
                (WPARAM)dis.CtlID, (LPARAM)&dis);
    }
    else
        STATUSBAR_DrawPart (hdc, part);

    SelectObject (hdc, hOldFont);

    if (self->clrBk != CLR_DEFAULT)
        DeleteObject (hbrBk);

    if (GetWindowLongA (hwnd, GWL_STYLE) & SBARS_SIZEGRIP) {
        RECT rect;

        GetClientRect (hwnd, &rect);
        STATUSBAR_DrawSizeGrip (hdc, &rect);
    }
}


static BOOL
STATUSBAR_Refresh (HWND hwnd, HDC hdc)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    int      i;
    RECT   rect;
    HBRUSH hbrBk;
    HFONT  hOldFont;

    if (!IsWindowVisible(hwnd))
        return (TRUE);

    GetClientRect (hwnd, &rect);

    if (infoPtr->clrBk != CLR_DEFAULT)
        hbrBk = CreateSolidBrush (infoPtr->clrBk);
    else
        hbrBk = GetSysColorBrush (COLOR_3DFACE);
    FillRect(hdc, &rect, hbrBk);

    hOldFont = SelectObject (hdc, infoPtr->hFont ? infoPtr->hFont : infoPtr->hDefaultFont);

    if (infoPtr->simple) {
        STATUSBAR_DrawPart (hdc, &infoPtr->part0);
    }
    else {
        for (i = 0; i < infoPtr->numParts; i++) {
            if (infoPtr->parts[i].style & SBT_OWNERDRAW) {
                DRAWITEMSTRUCT dis;

                dis.CtlID = GetWindowLongA (hwnd, GWL_ID);
                dis.itemID = -1;
                dis.hwndItem = hwnd;
                dis.hDC = hdc;
                dis.rcItem = infoPtr->parts[i].bound;
                dis.itemData = (INT)infoPtr->parts[i].text;
                SendMessageA (GetParent (hwnd), WM_DRAWITEM,
                        (WPARAM)dis.CtlID, (LPARAM)&dis);
            }
            else
                STATUSBAR_DrawPart (hdc, &infoPtr->parts[i]);
        }
    }

    SelectObject (hdc, hOldFont);

    if (infoPtr->clrBk != CLR_DEFAULT)
        DeleteObject (hbrBk);

    if (GetWindowLongA(hwnd, GWL_STYLE) & SBARS_SIZEGRIP)
        STATUSBAR_DrawSizeGrip (hdc, &rect);

    return TRUE;
}


static void
STATUSBAR_SetPartBounds (HWND hwnd)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *part;
    RECT rect, *r;
    int i;

    /* get our window size */
    GetClientRect (hwnd, &rect);

    rect.top += TOP_MARGIN;

    /* set bounds for simple rectangle */
    self->part0.bound = rect;

    /* set bounds for non-simple rectangles */
    for (i = 0; i < self->numParts; i++) {
        part = &self->parts[i];
        r = &self->parts[i].bound;
        r->top = rect.top;
        r->bottom = rect.bottom;
        if (i == 0)
            r->left = 0;
        else
            r->left = self->parts[i-1].bound.right + HORZ_GAP;
        if (part->x == -1)
            r->right = rect.right;
        else
            r->right = part->x;

        if (self->hwndToolTip) {
            TTTOOLINFOA ti;

            ti.cbSize = sizeof(TTTOOLINFOA);
            ti.hwnd = hwnd;
            ti.uId = i;
            ti.rect = *r;
            SendMessageA (self->hwndToolTip, TTM_NEWTOOLRECTA,
                            0, (LPARAM)&ti);
        }
    }
}


static VOID
STATUSBAR_RelayEvent (HWND hwndTip, HWND hwndMsg, UINT uMsg,
                      WPARAM wParam, LPARAM lParam)
{
    MSG msg;

    msg.hwnd = hwndMsg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.time = GetMessageTime ();
    msg.pt.x = LOWORD(GetMessagePos ());
    msg.pt.y = HIWORD(GetMessagePos ());

    SendMessageA (hwndTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
}


static LRESULT
STATUSBAR_GetBorders (LPARAM lParam)
{
    LPINT out = (LPINT) lParam;

    out[0] = HORZ_BORDER; /* horizontal border width */
    out[1] = VERT_BORDER; /* vertical border width */
    out[2] = HORZ_GAP; /* width of border between rectangles */

    return TRUE;
}


static LRESULT
STATUSBAR_GetIcon (HWND hwnd, WPARAM wParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    INT nPart;

    nPart = (INT)wParam & 0x00ff;
    if ((nPart < -1) || (nPart >= self->numParts))
        return 0;

    if (nPart == -1)
        return (self->part0.hIcon);
    else
        return (self->parts[nPart].hIcon);
}


static LRESULT
STATUSBAR_GetParts (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    LPINT parts;
    INT   num_parts;
    INT   i;

    num_parts = (INT) wParam;
    parts = (LPINT) lParam;
    if (parts) {
        return (infoPtr->numParts);
        for (i = 0; i < num_parts; i++) {
            parts[i] = infoPtr->parts[i].x;
        }
    }
    return (infoPtr->numParts);
}


static LRESULT
STATUSBAR_GetRect (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    int part_num;
    LPRECT  rect;

    part_num = ((INT) wParam) & 0x00ff;
    rect = (LPRECT) lParam;
    if (infoPtr->simple)
        *rect = infoPtr->part0.bound;
    else
        *rect = infoPtr->parts[part_num].bound;
    return TRUE;
}


static LRESULT
STATUSBAR_GetTextA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *part;
    INT   nPart;
    LRESULT result;

    nPart = ((INT) wParam) & 0x00ff;
    if (self->simple)
        part = &self->part0;
    else
        part = &self->parts[nPart];

    if (part->style & SBT_OWNERDRAW)
        result = (LRESULT)part->text;
    else {
        result = part->text ? lstrlenW (part->text) : 0;
        result |= (part->style << 16);
        if (lParam && LOWORD(result))
            lstrcpyWtoA((LPSTR)lParam, part->text);
    }
    return result;
}


static LRESULT
STATUSBAR_GetTextW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *part;
    INT   nPart;
    LRESULT result;

    nPart = ((INT)wParam) & 0x00ff;
    if (infoPtr->simple)
        part = &infoPtr->part0;
    else
        part = &infoPtr->parts[nPart];

    if (part->style & SBT_OWNERDRAW)
        result = (LRESULT)part->text;
    else {
        result = part->text ? lstrlenW (part->text) : 0;
        result |= (part->style << 16);
        if (lParam)
            lstrcpyW ((LPWSTR)lParam, part->text);
    }
    return result;
}


static LRESULT
STATUSBAR_GetTextLength (HWND hwnd, WPARAM wParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *part;
    INT part_num;
    DWORD result;

    part_num = ((INT) wParam) & 0x00ff;

    if (infoPtr->simple)
        part = &infoPtr->part0;
    else
        part = &infoPtr->parts[part_num];

    if (part->text)
        result = lstrlenW(part->text);
    else
        result = 0;

    result |= (part->style << 16);
    return result;
}


static LRESULT
STATUSBAR_GetTipTextA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);

    if (infoPtr->hwndToolTip) {
        TTTOOLINFOA ti;
        ti.cbSize = sizeof(TTTOOLINFOA);
        ti.hwnd = hwnd;
        ti.uId = LOWORD(wParam);
        SendMessageA (infoPtr->hwndToolTip, TTM_GETTEXTA, 0, (LPARAM)&ti);

        if (ti.lpszText)
            lstrcpynA ((LPSTR)lParam, ti.lpszText, HIWORD(wParam));
    }

    return 0;
}


static LRESULT
STATUSBAR_GetTipTextW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);

    if (infoPtr->hwndToolTip) {
        TTTOOLINFOW ti;
        ti.cbSize = sizeof(TTTOOLINFOW);
        ti.hwnd = hwnd;
        ti.uId = LOWORD(wParam);
        SendMessageW (infoPtr->hwndToolTip, TTM_GETTEXTW, 0, (LPARAM)&ti);

        if (ti.lpszText)
            lstrcpynW ((LPWSTR)lParam, ti.lpszText, HIWORD(wParam));
    }

    return 0;
}

static LRESULT
STATUSBAR_IsSimple (HWND hwnd)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    return infoPtr->simple;
}


static LRESULT
STATUSBAR_SetBkColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    COLORREF oldBkColor;
    HDC    hdc;

    oldBkColor = self->clrBk;
    self->clrBk = (COLORREF)lParam;
    hdc = GetDC (hwnd);
    STATUSBAR_Refresh (hwnd, hdc);
    ReleaseDC (hwnd, hdc);

    return oldBkColor;
}


static LRESULT
STATUSBAR_SetIcon (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    INT nPart = (INT)wParam & 0x00ff;
    HDC hdc;

    if ((nPart < -1) || (nPart >= self->numParts))
        return FALSE;

    hdc = GetDC (hwnd);
    if (nPart == -1) {
        self->part0.hIcon = (HICON)lParam;
        if (self->simple)
            STATUSBAR_RefreshPart (hwnd, &self->part0, hdc);
    }
    else {
        self->parts[nPart].hIcon = (HICON)lParam;
        if (!(self->simple))
            STATUSBAR_RefreshPart (hwnd, &self->parts[nPart], hdc);
    }
    ReleaseDC (hwnd, hdc);

    return TRUE;
}


static LRESULT
STATUSBAR_SetMinHeight (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);

    if (IsWindowVisible (hwnd)) {
        HWND parent = GetParent (hwnd);
        INT  width, x, y;
        RECT parent_rect;

        GetClientRect (parent, &parent_rect);
        self->height = (INT)wParam + VERT_BORDER;
        width = parent_rect.right - parent_rect.left;
        x = parent_rect.left;
        y = parent_rect.bottom - self->height;
        MoveWindow (hwnd, parent_rect.left,
                      parent_rect.bottom - self->height,
                      width, self->height, TRUE);
        STATUSBAR_SetPartBounds (hwnd);
    }

    return TRUE;
}


static LRESULT
STATUSBAR_SetParts (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *tmp;
    HDC hdc;
    LPINT parts;
    int i;
    int oldNumParts;

    if (self->simple)
        self->simple = FALSE;

    oldNumParts = self->numParts;
    self->numParts = (INT) wParam;
    parts = (LPINT) lParam;
    if (oldNumParts > self->numParts) {
        for (i = self->numParts ; i < oldNumParts; i++) {
            if (self->parts[i].text && !(self->parts[i].style & SBT_OWNERDRAW))
                COMCTL32_Free (self->parts[i].text);
        }
    }
    else if (oldNumParts < self->numParts) {
        tmp = (STATUSWINDOWPART*)COMCTL32_Alloc (sizeof(STATUSWINDOWPART) * self->numParts);
        for (i = 0; i < oldNumParts; i++) {
            tmp[i] = self->parts[i];
        }
        if (self->parts)
            COMCTL32_Free (self->parts);
        self->parts = tmp;
    }

    for (i = 0; i < self->numParts; i++) {
        self->parts[i].x = parts[i];
    }

    if (self->hwndToolTip) {
        INT nTipCount =
            SendMessageA (self->hwndToolTip, TTM_GETTOOLCOUNT, 0, 0);

        if (nTipCount < self->numParts) {
            /* add tools */
            TTTOOLINFOA ti;
            INT i;

            ZeroMemory (&ti, sizeof(TTTOOLINFOA));
            ti.cbSize = sizeof(TTTOOLINFOA);
            ti.hwnd = hwnd;
            for (i = nTipCount; i < self->numParts; i++) {
//              TRACE (statusbar, "add tool %d\n", i);
                ti.uId = i;
                SendMessageA (self->hwndToolTip, TTM_ADDTOOLA,
                                0, (LPARAM)&ti);
            }
        }
        else if (nTipCount > self->numParts) {
            /* delete tools */
            INT i;

            for (i = nTipCount - 1; i >= self->numParts; i--) {

//              FIXME (statusbar, "delete tool %d\n", i);

            }
        }
    }

    STATUSBAR_SetPartBounds (hwnd);

    hdc = GetDC (hwnd);
    STATUSBAR_Refresh (hwnd, hdc);
    ReleaseDC (hwnd, hdc);

    return TRUE;
}


static LRESULT
STATUSBAR_SetTextA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *part;
    int part_num;
    int style;
    LPSTR text;
    LPWSTR newText;
    int len;
    HDC hdc;

    text = (LPSTR) lParam;
    part_num = ((INT) wParam) & 0x00ff;
    style = ((INT) wParam) & 0xff00;

    if ((self->simple) || (self->parts==NULL) || (part_num==255))
        part = &self->part0;
    else
        part = &self->parts[part_num];
    if (!part) return FALSE;

    if (style & SBT_OWNERDRAW)
    {
      if (!(part->style & SBT_OWNERDRAW) && part->text) COMCTL32_Free(part->text);
      part->text = (LPWSTR)text;
    } else
    {
      //compare
      if (text && (len = lstrlenA(text)))
      {
        newText = (WCHAR*)COMCTL32_Alloc((len+1)*sizeof(WCHAR));
        lstrcpyAtoW (newText,text);
      } else newText = NULL;
      if (lstrcmpW(part->text,newText) == 0)
      {
        COMCTL32_Free(newText);
        if (part->style != style)
        {
          hdc = GetDC(hwnd);
          STATUSBAR_RefreshPart(hwnd,part,hdc);
          ReleaseDC(hwnd, hdc);
        }
        return TRUE;
      }

      COMCTL32_Free(part->text);
      part->text = newText;
    }
    part->style = style;

    hdc = GetDC (hwnd);
    STATUSBAR_RefreshPart (hwnd, part, hdc);
    ReleaseDC (hwnd, hdc);

    return TRUE;
}


static LRESULT
STATUSBAR_SetTextW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *self = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *part;
    INT  part_num, style, len;
    LPWSTR text;
    HDC  hdc;

    text = (LPWSTR) lParam;
    part_num = ((INT) wParam) & 0x00ff;
    style = ((INT) wParam) & 0xff00;

    if ((self->simple) || (self->parts==NULL) || (part_num==255))
        part = &self->part0;
    else
        part = &self->parts[part_num];
    if (!part) return FALSE;

    if (style & SBT_OWNERDRAW)
    {
      if (!(part->style & SBT_OWNERDRAW) && part->text) COMCTL32_Free(part->text);
      part->text = text;
    } else
    {
      //compare
      if (lstrcmpW(part->text,text) == 0)
      {
        if (part->style != style)
        {
          hdc = GetDC(hwnd);
          STATUSBAR_RefreshPart(hwnd,part,hdc);
          ReleaseDC(hwnd, hdc);
        }
        return TRUE;
      }

      /* duplicate string */
      COMCTL32_Free(part->text);
      if (text && (len = lstrlenW(text)))
      {
        part->text = (WCHAR*)COMCTL32_Alloc((len+1)*sizeof(WCHAR));
        lstrcpyW(part->text,text);
      }
    }
    part->style = style;

    hdc = GetDC (hwnd);
    STATUSBAR_RefreshPart (hwnd, part, hdc);
    ReleaseDC (hwnd, hdc);

    return TRUE;
}


static LRESULT
STATUSBAR_SetTipTextA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);

//    TRACE (statusbar, "part %d: \"%s\"\n", (INT)wParam, (LPSTR)lParam);
    if (infoPtr->hwndToolTip) {
        TTTOOLINFOA ti;
        ti.cbSize = sizeof(TTTOOLINFOA);
        ti.hwnd = hwnd;
        ti.uId = (INT)wParam;
        ti.hinst = 0;
        ti.lpszText = (LPSTR)lParam;
        SendMessageA (infoPtr->hwndToolTip, TTM_UPDATETIPTEXTA,
                        0, (LPARAM)&ti);
    }

    return 0;
}


static LRESULT
STATUSBAR_SetTipTextW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);

    if (infoPtr->hwndToolTip) {
        TTTOOLINFOW ti;
        ti.cbSize = sizeof(TTTOOLINFOW);
        ti.hwnd = hwnd;
        ti.uId = (INT)wParam;
        ti.hinst = 0;
        ti.lpszText = (LPWSTR)lParam;
        SendMessageW (infoPtr->hwndToolTip, TTM_UPDATETIPTEXTW,
                        0, (LPARAM)&ti);
    }

    return 0;
}

static LRESULT
STATUSBAR_Simple (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    HDC  hdc;

    infoPtr->simple = (BOOL)wParam;

    /* send notification */
    sendNotify(hwnd,SBN_SIMPLEMODECHANGE);

    hdc = GetDC (hwnd);
    STATUSBAR_Refresh (hwnd, hdc);
    ReleaseDC (hwnd, hdc);

    return TRUE;
}


static LRESULT
STATUSBAR_WMCreate (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    LPCREATESTRUCTA lpCreate = (LPCREATESTRUCTA)lParam;
    NONCLIENTMETRICSA nclm;
    RECT        rect;
    int         width, len;
    HDC hdc;
    STATUSWINDOWINFO *infoPtr;

    infoPtr = (STATUSWINDOWINFO*)initControl(hwnd,sizeof(STATUSWINDOWINFO));

    infoPtr->numParts = 1;
    infoPtr->parts = 0;
    infoPtr->simple = FALSE;
    infoPtr->clrBk = CLR_DEFAULT;
    infoPtr->hFont = 0;
    GetClientRect (hwnd, &rect);

    nclm.cbSize = sizeof(NONCLIENTMETRICSA);
    SystemParametersInfoA (SPI_GETNONCLIENTMETRICS, 0, &nclm, 0);
    infoPtr->hDefaultFont = CreateFontIndirectA (&nclm.lfStatusFont);

    /* initialize simple case */
    infoPtr->part0.bound = rect;
    infoPtr->part0.text = 0;
    infoPtr->part0.x = 0;
    infoPtr->part0.style = 0;
    infoPtr->part0.hIcon = 0;

    /* initialize first part */
    infoPtr->parts = (STATUSWINDOWPART*)COMCTL32_Alloc (sizeof(STATUSWINDOWPART));
    infoPtr->parts[0].bound = rect;
    infoPtr->parts[0].text = 0;
    infoPtr->parts[0].x = -1;
    infoPtr->parts[0].style = 0;
    infoPtr->parts[0].hIcon = 0;

    if (infoPtr->header.fUnicode)
    {
      if (lpCreate->lpszName && (len = lstrlenW ((LPCWSTR)lpCreate->lpszName)))
      {
        infoPtr->parts[0].text = (WCHAR*)COMCTL32_Alloc ((len + 1)*sizeof(WCHAR));
        lstrcpyW (infoPtr->parts[0].text, (LPCWSTR)lpCreate->lpszName);
      }
    } else
    {
      if (lpCreate->lpszName && (len = lstrlenA ((LPCSTR)lpCreate->lpszName)))
      {
        infoPtr->parts[0].text = (WCHAR*)COMCTL32_Alloc ((len + 1)*sizeof(WCHAR));
        lstrcpyAtoW (infoPtr->parts[0].text, (char*)lpCreate->lpszName);
      }
    }

    hdc = GetDC(hwnd);
    if (hdc) {
        TEXTMETRICA tm;
        HFONT hOldFont;

        hOldFont = SelectObject (hdc,infoPtr->hDefaultFont);
        GetTextMetricsA(hdc, &tm);
        infoPtr->textHeight = tm.tmHeight+tm.tmExternalLeading;
        SelectObject (hdc, hOldFont);
        ReleaseDC(hwnd, hdc);
    }

    if (GetWindowLongA(hwnd,GWL_STYLE) & SBT_TOOLTIPS)
      infoPtr->hwndToolTip = createToolTip(hwnd,0,FALSE);

    GetClientRect (GetParent (hwnd), &rect);
    width = rect.right - rect.left;
    infoPtr->height = infoPtr->textHeight + 4 + VERT_BORDER;
//CB: todo: find bug in font handling!
infoPtr->height += 4;
    MoveWindow(hwnd,lpCreate->x,lpCreate->y-1,width,infoPtr->height,FALSE);
    STATUSBAR_SetPartBounds (hwnd);

    return 0;
}


static LRESULT
STATUSBAR_WMDestroy (HWND hwnd)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    int i;

    for (i = 0; i < infoPtr->numParts; i++) {
        if (infoPtr->parts[i].text && !(infoPtr->parts[i].style & SBT_OWNERDRAW))
            COMCTL32_Free (infoPtr->parts[i].text);
    }
    if (infoPtr->part0.text && !(infoPtr->part0.style & SBT_OWNERDRAW))
        COMCTL32_Free (infoPtr->part0.text);
    COMCTL32_Free (infoPtr->parts);

    /* delete default font */
    if (infoPtr->hDefaultFont)
        DeleteObject (infoPtr->hDefaultFont);

    /* delete tool tip control */
    destroyToolTip(infoPtr->hwndToolTip);

    doneControl(hwnd);

    return 0;
}


static LRESULT
STATUSBAR_WMGetFont (HWND hwnd)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    return infoPtr->hFont;
}


static LRESULT
STATUSBAR_WMGetText (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    INT len;

    if (!(infoPtr->parts[0].text))
        return 0;
    len = lstrlenW (infoPtr->parts[0].text);
    if (wParam > len)
    {
      if (infoPtr->header.fUnicode)
        lstrcpyW ((LPWSTR)lParam, infoPtr->parts[0].text);
      else
        lstrcpyWtoA ((LPSTR)lParam, infoPtr->parts[0].text);
      return len;
    }

    return -1;
}

static LRESULT
STATUSBAR_WMMouseMove (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr(hwnd);

    if (infoPtr->hwndToolTip)
        STATUSBAR_RelayEvent (infoPtr->hwndToolTip, hwnd,
                              WM_MOUSEMOVE, wParam, lParam);
    return 0;
}


static LRESULT
STATUSBAR_WMNCHitTest (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  if (GetWindowLongA (hwnd,GWL_STYLE) & SBARS_SIZEGRIP)
  {
    RECT rect = STATUSBAR_GetSizeBox(hwnd);
    POINT pt;

    pt.x = (SHORT)LOWORD(lParam);
    pt.y = (SHORT)HIWORD(lParam);
    ScreenToClient(hwnd,&pt);

    if (PtInRect(&rect,pt))
      return HTBOTTOMRIGHT;
  }

  return DefWindowProcA (hwnd, WM_NCHITTEST, wParam, lParam);
}


static LRESULT
STATUSBAR_WMNCLButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  PostMessageA (GetParent (hwnd), WM_NCLBUTTONDOWN, wParam, lParam);
  return 0;
}


static LRESULT
STATUSBAR_WMNCLButtonUp (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    PostMessageA (GetParent (hwnd), WM_NCLBUTTONUP, wParam, lParam);
    return 0;
}


static LRESULT
STATUSBAR_WMPaint (HWND hwnd, WPARAM wParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = wParam==0 ? BeginPaint (hwnd, &ps) : (HDC)wParam;
    STATUSBAR_Refresh (hwnd, hdc);
    if (!wParam)
        EndPaint (hwnd, &ps);

    return 0;
}


static LRESULT
STATUSBAR_WMSetFont (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);

    infoPtr->hFont = (HFONT)wParam;
    if (LOWORD(lParam) == TRUE)
    {
      HDC hdc = GetDC (hwnd);
      TEXTMETRICA tm;
      HFONT hOldFont;

      hOldFont = SelectObject(hdc,infoPtr->hFont);
      GetTextMetricsA(hdc,&tm);
      infoPtr->textHeight = tm.tmHeight+tm.tmExternalLeading;
      SelectObject(hdc,hOldFont);

      //CB: todo: move window

      STATUSBAR_Refresh (hwnd, hdc);
      ReleaseDC (hwnd, hdc);
    }

    return 0;
}


static LRESULT
STATUSBAR_WMSetText (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    STATUSWINDOWPART *part;
    int len;
    HDC hdc;

    if (!infoPtr) return DefWindowProcA(hwnd,WM_SETTEXT,wParam,lParam);

    if (infoPtr->numParts == 0)
        return FALSE;

    part = &infoPtr->parts[0];
    /* duplicate string */
    if (part->text)
        COMCTL32_Free (part->text);
    part->text = 0;
    if (infoPtr->header.fUnicode) {
        if (lParam && (len = lstrlenW((LPCWSTR)lParam))) {
            part->text = (WCHAR*)COMCTL32_Alloc ((len+1)*sizeof(WCHAR));
            lstrcpyW (part->text, (LPCWSTR)lParam);
        }
    }
    else {
        if (lParam && (len = lstrlenA((LPCSTR)lParam))) {
            part->text = (WCHAR*)COMCTL32_Alloc ((len+1)*sizeof(WCHAR));
            lstrcpyAtoW (part->text, (char*)lParam);
        }
    }

    hdc = GetDC (hwnd);
    STATUSBAR_RefreshPart (hwnd, part, hdc);
    ReleaseDC (hwnd, hdc);

    return TRUE;
}


static LRESULT
STATUSBAR_WMSize (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    STATUSWINDOWINFO *infoPtr = STATUSBAR_GetInfoPtr (hwnd);
    INT  width, x, y, flags;
    RECT parent_rect;
    HWND parent;

    /* Need to resize width to match parent */
    flags = (INT) wParam;

    /* FIXME for flags =
     * SIZE_MAXIMIZED, SIZE_MAXSHOW, SIZE_MINIMIZED, SIZE_RESTORED
     */

    if (flags == SIZE_RESTORED) {
        /* width and height don't apply */
        parent = GetParent (hwnd);
        GetClientRect (parent, &parent_rect);
        width = parent_rect.right - parent_rect.left;
        x = parent_rect.left;
        y = parent_rect.bottom - infoPtr->height;
        MoveWindow (hwnd, parent_rect.left,
                      parent_rect.bottom - infoPtr->height,
                      width, infoPtr->height, TRUE);
        STATUSBAR_SetPartBounds (hwnd);
    }
    return 0;
}

static LRESULT WINAPI
StatusWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case SB_GETBORDERS:
            return STATUSBAR_GetBorders (lParam);

        case SB_GETICON:
            return STATUSBAR_GetIcon (hwnd, wParam);

        case SB_GETPARTS:
            return STATUSBAR_GetParts (hwnd, wParam, lParam);

        case SB_GETRECT:
            return STATUSBAR_GetRect (hwnd, wParam, lParam);

        case SB_GETTEXTA:
            return STATUSBAR_GetTextA (hwnd, wParam, lParam);

        case SB_GETTEXTW:
            return STATUSBAR_GetTextW (hwnd, wParam, lParam);

        case SB_GETTEXTLENGTHA:
        case SB_GETTEXTLENGTHW:
            return STATUSBAR_GetTextLength (hwnd, wParam);

        case SB_GETTIPTEXTA:
            return STATUSBAR_GetTipTextA (hwnd, wParam, lParam);

        case SB_GETTIPTEXTW:
            return STATUSBAR_GetTipTextW (hwnd, wParam, lParam);

        case SB_ISSIMPLE:
            return STATUSBAR_IsSimple (hwnd);

        case SB_SETBKCOLOR:
            return STATUSBAR_SetBkColor (hwnd, wParam, lParam);

        case SB_SETICON:
            return STATUSBAR_SetIcon (hwnd, wParam, lParam);

        case SB_SETMINHEIGHT:
            return STATUSBAR_SetMinHeight (hwnd, wParam, lParam);

        case SB_SETPARTS:
            return STATUSBAR_SetParts (hwnd, wParam, lParam);

        case SB_SETTEXTA:
            return STATUSBAR_SetTextA (hwnd, wParam, lParam);

        case SB_SETTEXTW:
            return STATUSBAR_SetTextW (hwnd, wParam, lParam);

        case SB_SETTIPTEXTA:
            return STATUSBAR_SetTipTextA (hwnd, wParam, lParam);

        case SB_SETTIPTEXTW:
            return STATUSBAR_SetTipTextW (hwnd, wParam, lParam);

        case SB_SIMPLE:
            return STATUSBAR_Simple (hwnd, wParam, lParam);


        case WM_CREATE:
            return STATUSBAR_WMCreate (hwnd, wParam, lParam);

        case WM_DESTROY:
            return STATUSBAR_WMDestroy (hwnd);

        case WM_GETFONT:
            return STATUSBAR_WMGetFont (hwnd);

        case WM_GETTEXT:
            return STATUSBAR_WMGetText (hwnd, wParam, lParam);

        case WM_GETTEXTLENGTH:
            return STATUSBAR_GetTextLength (hwnd, 0);

        case WM_LBUTTONDBLCLK:
            return sendNotify(hwnd,NM_DBLCLK);

        case WM_LBUTTONUP:
            return sendNotify(hwnd,NM_CLICK);

        case WM_MOUSEMOVE:
            return STATUSBAR_WMMouseMove (hwnd, wParam, lParam);

        case WM_NCHITTEST:
            return STATUSBAR_WMNCHitTest (hwnd, wParam, lParam);

        case WM_NCLBUTTONDOWN:
            return STATUSBAR_WMNCLButtonDown (hwnd, wParam, lParam);

        case WM_NCLBUTTONUP:
            return STATUSBAR_WMNCLButtonUp (hwnd, wParam, lParam);

        case WM_PAINT:
            return STATUSBAR_WMPaint (hwnd, wParam);

        case WM_RBUTTONDBLCLK:
            return sendNotify(hwnd,NM_RDBLCLK);

        case WM_RBUTTONUP:
            return sendNotify(hwnd,NM_RCLICK);

        case WM_SETFONT:
            return STATUSBAR_WMSetFont (hwnd, wParam, lParam);

        case WM_SETTEXT:
            return STATUSBAR_WMSetText (hwnd, wParam, lParam);

        case WM_SIZE:
            return STATUSBAR_WMSize (hwnd, wParam, lParam);

        default:
//            if (msg >= WM_USER)
//              ERR (statusbar, "unknown msg %04x wp=%04x lp=%08lx\n",
//                   msg, wParam, lParam);
            return defComCtl32ProcA (hwnd, msg, wParam, lParam);
    }
    return 0;
}


/***********************************************************************
 * STATUS_Register [Internal]
 *
 * Registers the status window class.
 */

VOID
STATUS_Register (VOID)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW;
    wndClass.lpfnWndProc   = (WNDPROC)StatusWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(STATUSWINDOWINFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wndClass.lpszClassName = STATUSCLASSNAMEA;

    RegisterClassA (&wndClass);
}


/***********************************************************************
 * STATUS_Unregister [Internal]
 *
 * Unregisters the status window class.
 */

VOID
STATUS_Unregister (VOID)
{
    UnregisterClassA (STATUSCLASSNAMEA, (HINSTANCE)NULL);
}

