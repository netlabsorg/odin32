/* $Id: winnonclient.cpp,v 1.3 2000-01-18 20:11:20 sandervl Exp $ */
/*
 * Win32 menu API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (Port from Wine)
 *
 * Parts ported from Wine: (window\nonclient.c)
 * Copyright 1994 Alexandre Julliard
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>

  /* Some useful macros */
#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_BORDER)))

#define HAS_THICKFRAME(style) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

#define HAS_FIXEDFRAME(style,exStyle) \
   (((((exStyle) & WS_EX_DLGMODALFRAME) || \
      ((style) & WS_DLGFRAME)) && ((style) & WS_BORDER)) && \
     !((style) & WS_THICKFRAME))

/* win31 style (simple border) in win95 look */
#define HAS_CLASSICBORDER(style,exStyle) \
    (!((exStyle) & WS_EX_STATICEDGE) && !((exStyle) & WS_EX_CLIENTEDGE) && \
    !((style) & WS_THICKFRAME) && !((style) & WS_DLGFRAME) && \
    ((style) & WS_BORDER))

#define HAS_SIZEFRAME(style) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

#define HAS_MENU(w)  (!((w)->dwStyle & WS_CHILD) && ((w)->wIDmenu != 0))

#define ON_LEFT_BORDER(hit) \
 (((hit) == HTLEFT) || ((hit) == HTTOPLEFT) || ((hit) == HTBOTTOMLEFT))
#define ON_RIGHT_BORDER(hit) \
 (((hit) == HTRIGHT) || ((hit) == HTTOPRIGHT) || ((hit) == HTBOTTOMRIGHT))
#define ON_TOP_BORDER(hit) \
 (((hit) == HTTOP) || ((hit) == HTTOPLEFT) || ((hit) == HTTOPRIGHT))
#define ON_BOTTOM_BORDER(hit) \
 (((hit) == HTBOTTOM) || ((hit) == HTBOTTOMLEFT) || ((hit) == HTBOTTOMRIGHT))

/******************************************************************************
 * NC_AdjustRectOuter95
 *
 * Computes the size of the "outside" parts of the window based on the
 * parameters of the client area.
 *
 + PARAMS
 *     LPRECT16  rect
 *     DWORD  style
 *     BOOL32  menu
 *     DWORD  exStyle
 *
 * NOTES
 *     "Outer" parts of a window means the whole window frame, caption and
 *     menu bar. It does not include "inner" parts of the frame like client
 *     edge, static edge or scroll bars.
 *
 * Revision history
 *     05-Jul-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *        Original (NC_AdjustRect95) cut & paste from NC_AdjustRect
 *
 *     20-Jun-1998 Eric Kohl (ekohl@abo.rhein-zeitung.de)
 *        Split NC_AdjustRect95 into NC_AdjustRectOuter95 and
 *        NC_AdjustRectInner95 and added handling of Win95 styles.
 *
 *****************************************************************************/

static void
NC_AdjustRectOuter95 (LPRECT rect, DWORD style, BOOL menu, DWORD exStyle)
{
    if(style & WS_ICONIC) return;

    /* Decide if the window will be managed (see CreateWindowEx) */
#if 0
    if (!(Options.managed && !(style & WS_CHILD) &&
          ((style & (WS_DLGFRAME | WS_THICKFRAME)) ||
           (exStyle & WS_EX_DLGMODALFRAME))))
#else
    if (!(style & WS_CHILD) &&
          ((style & (WS_DLGFRAME | WS_THICKFRAME)) ||
           (exStyle & WS_EX_DLGMODALFRAME)))
#endif
    {
        if (HAS_FIXEDFRAME( style, exStyle ))
            InflateRect(rect, GetSystemMetrics(SM_CXDLGFRAME), GetSystemMetrics(SM_CYDLGFRAME) );
        else
        {
            if (HAS_SIZEFRAME(style))
                InflateRect( rect, GetSystemMetrics(SM_CXFRAME), GetSystemMetrics(SM_CYFRAME) );
#if 0
            if (style & WS_BORDER)
                InflateRect( rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));
#endif
        }

        if ((style & WS_CAPTION) == WS_CAPTION)
        {
	    if (exStyle & WS_EX_TOOLWINDOW)
		rect->top -= GetSystemMetrics(SM_CYSMCAPTION);
	    else
		rect->top -= GetSystemMetrics(SM_CYCAPTION);
        }
    }

    if (menu)
	rect->top -= GetSystemMetrics(SM_CYMENU);
}


/******************************************************************************
 * NC_AdjustRectInner95
 *
 * Computes the size of the "inside" part of the window based on the
 * parameters of the client area.
 *
 + PARAMS
 *     LPRECT16 rect
 *     DWORD    style
 *     DWORD    exStyle
 *
 * NOTES
 *     "Inner" part of a window means the window frame inside of the flat
 *     window frame. It includes the client edge, the static edge and the
 *     scroll bars.
 *
 * Revision history
 *     05-Jul-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *        Original (NC_AdjustRect95) cut & paste from NC_AdjustRect
 *
 *     20-Jun-1998 Eric Kohl (ekohl@abo.rhein-zeitung.de)
 *        Split NC_AdjustRect95 into NC_AdjustRectOuter95 and
 *        NC_AdjustRectInner95 and added handling of Win95 styles.
 *
 *****************************************************************************/

static void
NC_AdjustRectInner95 (LPRECT rect, DWORD style, DWORD exStyle)
{
    if(style & WS_ICONIC) return;

    if (exStyle & WS_EX_CLIENTEDGE)
	InflateRect (rect, GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));

    if (exStyle & WS_EX_STATICEDGE)
	InflateRect (rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

    if (HAS_CLASSICBORDER(style, exStyle))
        InflateRect( rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER ));

    if (style & WS_VSCROLL) rect->right  += GetSystemMetrics(SM_CXVSCROLL);
    if (style & WS_HSCROLL) rect->bottom += GetSystemMetrics(SM_CYHSCROLL);
}

/***********************************************************************
 *           AdjustWindowRectEx    (USER.454)
 */
BOOL WIN32API AdjustWindowRectEx(LPRECT rect, DWORD style,
                                 BOOL menu, DWORD exStyle )
{
      /* Correct the window style */
    if (!(style & (WS_POPUP | WS_CHILD)))  /* Overlapped window */
	style |= WS_CAPTION;
    style &= (WS_DLGFRAME | WS_BORDER | WS_THICKFRAME | WS_CHILD);
    exStyle &= (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE |
		WS_EX_STATICEDGE | WS_EX_TOOLWINDOW);
    if (exStyle & WS_EX_DLGMODALFRAME) style &= ~WS_THICKFRAME;

    NC_AdjustRectOuter95( rect, style, menu, exStyle );
    NC_AdjustRectInner95( rect, style, exStyle );

    return TRUE;
}

/***********************************************************************
 *           AdjustWindowRect    (USER32.2)
 */
BOOL WIN32API AdjustWindowRect( LPRECT rect, DWORD style, BOOL menu )
{
    return AdjustWindowRectEx( rect, style, menu, 0 );
}
