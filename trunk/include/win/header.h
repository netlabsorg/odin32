/* $Id: header.h,v 1.11 2001-03-31 13:23:33 sandervl Exp $ */

/*
 * Header window definitions
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Christoph Bratschi
 *
 */

#ifndef __WINE_HEADER_H_
#define __WINE_HEADER_H_

typedef struct
{
    INT     cxy;
    HBITMAP hbm;
    LPWSTR  pszText;
    INT     cchTextMax;
    INT     fmt;
    LPARAM  lParam;
    INT     iImage;
    INT     iOrder;             /* see documentation of HD_ITEM */
    UINT    type;
    LPVOID  pvFilter;

    BOOL    bDown;              /* is item pressed? (used for drawing) */
    RECT    rect;               /* bounding rectangle of the item */
} HEADER_ITEM;


typedef struct
{
    COMCTL32_HEADER header;

    UINT      uNumItem; /* number of items (columns) */
    INT       nHeight;  /* height of the header (pixels) */
    HFONT     hFont;            /* handle to the current font */
    HCURSOR   hcurArrow;        /* handle to the arrow cursor */
    HCURSOR   hcurDivider;      /* handle to a cursor (used over dividers) <-|-> */
    HCURSOR   hcurDivopen;      /* handle to a cursor (used over dividers) <-||-> */
    BOOL      bCaptured;        /* Is the mouse captured? */
    BOOL      bPressed; /* Is a header item pressed (down)? */
    BOOL      bTracking;        /* Is in tracking mode? */
    BOOL      bTrackOpen;
    BOOL      bDragDrop;
    BOOL      bTimer;
    INT       iMoveItem;        /* index of tracked item. (Tracking mode) */
    INT       xTrackOffset;     /* distance between the right side of the tracked item and the cursor */
    INT       xOldTrack;        /* track offset (see above) after the last WM_MOUSEMOVE */
    INT       nOldWidth;        /* width of a sizing item after the last WM_MOUSEMOVE */
    INT       iHotItem; /* index of hot item (cursor is over this item) */
    INT       xBmpMargin;

    HIMAGELIST  himl;           /* handle to a image list (may be 0) */
    HIMAGELIST  dragImage;
    POINT       dragStart;
    POINT       dragPos;
    INT         iDragItem;
    HEADER_ITEM *items;         /* pointer to array of HEADER_ITEM's */
} HEADER_INFO;

extern LRESULT HEADER_DeleteItem(HWND hwnd,WPARAM wParam);
extern DWORD   HEADER_GetExpandedColumnTextWidth(HWND hwnd,INT iItem);
extern LRESULT HEADER_GetItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode);
extern LRESULT HEADER_GetItemCount(HWND hwnd);
extern LRESULT HEADER_GetItemRect(HWND hwnd,WPARAM wParam,LPARAM lParam);
extern LRESULT HEADER_InsertItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode);
extern LRESULT HEADER_SetItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode);
extern LRESULT HEADER_SetOrderArray(HWND hwnd,WPARAM wParam,LPARAM lParam);
extern LRESULT HEADER_GetOrderArray(HWND hwnd,WPARAM wParam,LPARAM lParam);
extern LRESULT HEADER_Layout(HWND hwnd,WPARAM wParam,LPARAM lParam);


#endif /* __WINE_HEADER_H_ */
