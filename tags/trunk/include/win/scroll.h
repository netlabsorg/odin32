/* $Id: scroll.h,v 1.4 1999-09-29 08:26:39 sandervl Exp $ */

/*
 * Scroll-bar class extra info
 *
 * Copyright 1993 Martin Ayotte
 * Copyright 1994 Alexandre Julliard
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_SCROLL_H
#define __WINE_SCROLL_H

#define SCROLLBARCLASSNAME "SCROLLBAR"

typedef struct
{
    INT   CurVal;   /* Current scroll-bar value */
    INT   MinVal;   /* Minimum scroll-bar value */
    INT   MaxVal;   /* Maximum scroll-bar value */
    INT   Page;     /* Page size of scroll bar (Win32) */
    UINT  flags;    /* EnableScrollBar flags */
} SCROLLBAR_INFO;

extern LRESULT WINAPI ScrollBarWndProc( HWND hwnd, UINT uMsg,
                                        WPARAM wParam, LPARAM lParam );
extern void SCROLL_DrawScrollBar( HWND hwnd, HDC hdc, INT nBar,
                                  BOOL arrows, BOOL interior );
extern void SCROLL_HandleScrollEvent( HWND hwnd, INT nBar,
                                      UINT msg, POINT pt );

BOOL SCROLLBAR_Register();
BOOL SCROLLBAR_Unregister();

#endif  /* __WINE_SCROLL_H */

