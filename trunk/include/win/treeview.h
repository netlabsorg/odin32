/* $Id: treeview.h,v 1.16 2000-03-21 17:28:20 cbratschi Exp $ */
/*
 * Treeview class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1998 Alex Priem
 * Copyright 2000 Christoph Bratschi
 */

#ifndef __WINE_TREEVIEW_H
#define __WINE_TREEVIEW_H

#define MINIMUM_INDENT        10
#define ITEM_VSPACE            2
#define TV_REFRESH_DELAY     100     /* 100 ms delay between two refreshes */
#define TV_DEFAULTITEMHEIGHT  16
#define TVITEM_ALLOC          32      /* default nr of items to allocate at first try */
#define TV_ISEARCH_DELAY     500


/* internal structures */

typedef struct
{
  UINT      mask;
  HTREEITEM hItem;
  UINT      state;
  UINT      stateMask;
  LPWSTR    pszText;
  int       cchTextMax;
  int       iImage;
  int       iSelectedImage;
  int       cChildren;
  LPARAM    lParam;
  int       iIntegral;
  int       iLevel;         /* indentation level:0=root level */
  COLORREF  clrText;
  HTREEITEM parent;         /* handle to parent or 0 if at root*/
  HTREEITEM firstChild;     /* handle to first child or 0 if no child*/
  HTREEITEM sibling;        /* handle to next item in list, 0 if last */
  HTREEITEM upsibling;      /* handle to previous item in list, 0 if first */
  int       visible;
  RECT      rect;
  RECT      text;
  RECT      expandBox;      /* expand box (+/-) coordinate */
  RECT      bitmap;
  RECT      statebitmap;
  BOOL      calculated;
} TREEVIEW_ITEM, *LPTREEVIEW_ITEM;

typedef struct tagTREEVIEW_INFO
{
  COMCTL32_HEADER header;
  UINT            uInternalStatus;
  INT             Timer;
  UINT            uNumItems;      /* number of valid TREEVIEW_ITEMs */
  UINT            uNumPtrsAlloced;
  HTREEITEM       uMaxHandle;     /* needed for delete_item */
  HTREEITEM       TopRootItem;    /* handle to first item in treeview */
  INT             cdmode;         /* last custom draw setting */
  UINT            uScrollTime;    /* max. time for scrolling in milliseconds*/
  UINT            uItemHeight;    /* item height, -1 for default item height */
  UINT            uRealItemHeight;// current item height in pixels
  UINT            uVScrollStep;   // scroll step in pixels
  UINT            uVisibleHeight; /* visible height of treeview in pixels */
  UINT            uTotalHeight;   /* total height of treeview in pixels */
  UINT            uVisibleWidth;
  UINT            uTotalWidth;
  UINT            uIndent;        /* indentation in pixels */
  HTREEITEM       selectedItem;   /* handle to selected item or 0 if none */
  HTREEITEM       hotItem;        /* handle currently under cursor, 0 if none */
  HTREEITEM       tipItem;        // item with tooltip
  HTREEITEM       editItem;       /* handle to item currently editted, 0 if none */
  HTREEITEM       firstVisible;   /* handle to first visible item */
  HTREEITEM       dropItem;       /* handle to item selected by drag cursor */
  HTREEITEM       insertMarkItem; /* item after which insertion mark is placed */
  BOOL            insertBeforeorAfter; /* flag used by TVM_SETINSERTMARK */
  HIMAGELIST      dragList;       /* Bitmap of dragged item */
  POINT           lefttop;        //in pixels
  COLORREF        clrBk;
  COLORREF        clrText;
  COLORREF        clrLine;
  COLORREF        clrInsertMark;
  HFONT           hFont;
  HFONT           hBoldFont;
  HWND            hwndToolTip;
  HWND            hwndEdit;
  WNDPROC         wpEditOrig;     /* needed for subclassing edit control */
  HIMAGELIST      himlNormal;
  HIMAGELIST      himlState;
  LPTVSORTCB      pCallBackSort; /* ptr to TVSORTCB struct for callback sorting */
  TREEVIEW_ITEM   *items;        /* itemlist */
  INT             *freeList;     /* bitmap indicating which elements are valid */
                                 /* 1=valid, 0=free;   */
                                 /* size of list= uNumPtrsAlloced/32 */
  LPWSTR          pszISearch;
  UINT            uISearchLen;
} TREEVIEW_INFO, *LPTREEVIEW_INFO;

/* bitflags for infoPtr->uInternalStatus */

#define TV_HSCROLL      0x001    /* treeview too large to fit in window */
#define TV_VSCROLL      0x002    /* (horizontal/vertical) */
#define TV_LDRAG        0x004    /* Lbutton pushed to start drag */
#define TV_LDRAGGING    0x008    /* Lbutton pushed, mouse moved.  */
#define TV_RDRAG        0x010    /* dito Rbutton */
#define TV_RDRAGGING    0x020
#define TV_NOREDRAW     0x040
#define TV_CALCALL      0x080

/* bitflags for infoPtr->timer */

#define TV_REFRESH_TIMER 1
#define TV_EDIT_TIMER    2
#define TV_ISEARCH_TIMER 3

#define TV_REFRESH_TIMER_SET 1
#define TV_EDIT_TIMER_SET    2
#define TV_ISEARCH_TIMER_SET 3

extern VOID TREEVIEW_Register (VOID);
extern VOID TREEVIEW_Unregister (VOID);

#endif  /* __WINE_TREEVIEW_H */
