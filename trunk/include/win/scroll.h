/* $Id: scroll.h,v 1.9 2003-11-14 13:43:08 sandervl Exp $ */

/*
 * Scroll-bar class extra info
 *
 * Copyright 1993 Martin Ayotte
 * Copyright 1994 Alexandre Julliard
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_SCROLL_H
#define __WINE_SCROLL_H

#define SCROLLBARCLASSNAME "ScrollBar"

typedef struct
{
    INT   CurVal;   /* Current scroll-bar value */
    INT   MinVal;   /* Minimum scroll-bar value */
    INT   MaxVal;   /* Maximum scroll-bar value */
    INT   Page;     /* Page size of scroll bar (Win32) */
    UINT  flags;    /* EnableScrollBar flags */
} SCROLLBAR_INFO;

VOID SCROLL_DrawScrollBar(HWND hwnd,HDC hdc,INT nBar,BOOL arrows,BOOL interior);
LRESULT SCROLL_HandleScrollEvent(HWND hwnd,WPARAM wParam,LPARAM lParam,INT nBar,UINT msg);

extern LRESULT WINAPI ScrollBarWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
extern LRESULT WINAPI HorzScrollBarWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam );
extern LRESULT WINAPI VertScrollBarWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam );

VOID SCROLL_SubclassScrollBars(HWND hwndHorz,HWND hwndVert);

BOOL SCROLLBAR_Register();
BOOL SCROLLBAR_Unregister();

#endif  /* __WINE_SCROLL_H */

