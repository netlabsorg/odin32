/* $Id: scroll.h,v 1.5 1999-10-17 12:18:50 cbratschi Exp $ */

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

VOID SCROLL_SubclassScrollBars(HWND hwndHorz,HWND hwndVert);

extern LRESULT WINAPI ScrollBarWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
extern LRESULT WINAPI HorzScrollBarWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam );
extern LRESULT WINAPI VertScrollBarWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam );

BOOL SCROLLBAR_Register();
BOOL SCROLLBAR_Unregister();

#endif  /* __WINE_SCROLL_H */

