/* $Id: treeview.h,v 1.20 2001-03-31 13:23:34 sandervl Exp $ */
/*
 * Treeview class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1998 Alex Priem
 * Copyright 2000 Christoph Bratschi
 */

#ifndef __WINE_TREEVIEW_H
#define __WINE_TREEVIEW_H

#define ITEM_VSPACE            2
#define TV_REFRESH_DELAY     100     /* 100 ms delay between two refreshes */
#define TV_DEFAULTITEMHEIGHT  16
#define TVITEM_ALLOC          32      /* default nr of items to allocate at first try */
#define TV_ISEARCH_DELAY     1000 //documented in ListView_Message_Processing.htm
#define TV_INFOTIP_DELAY     500


/* internal structures */

typedef struct _TREEITEM    /* HTREEITEM is a _TREEINFO *. */
{
  UINT      callbackMask;
  UINT      state;
  UINT      stateMask;
  LPWSTR    pszText;
  int       cchTextMax;
  int       iImage;
  int       iSelectedImage;
  int       cChildren;
  LPARAM    lParam;
  int       iIntegral;      /* item height multiplier (1 is normal) */
  int       iLevel;         /* indentation level:0=root level */
  HTREEITEM parent;         /* handle to parent or 0 if at root*/
  HTREEITEM firstChild;     /* handle to first child or 0 if no child*/
  HTREEITEM lastChild;
  HTREEITEM prevSibling;    /* handle to prev item in list, 0 if first */
  HTREEITEM nextSibling;    /* handle to next item in list, 0 if last */
  RECT      rect;
  LONG      linesOffset;
  LONG      stateOffset;
  LONG      imageOffset;
  LONG      textOffset;
  LONG      textWidth;      /* horizontal text extent for pszText */
  LONG      textHeight;
  BOOL      inclient;       // displayed in client rect
  BOOL      displayed;      // item is displayed
  int       displayOrder;   /* display ordering, 0 is first display item */

  BOOL      calculated;     //rect is valid
} TREEVIEW_ITEM, *LPTREEVIEW_ITEM;

typedef struct tagTREEVIEW_INFO
{
  COMCTL32_HEADER header;

  HWND          hwnd;
  DWORD         dwStyle;
  HTREEITEM     root;
  UINT          uInternalStatus;
  INT           Timer;
  UINT          uNumItems;      /* number of valid TREEVIEW_ITEMs */
  INT           cdmode;         /* last custom draw setting */
  UINT          uScrollTime;    /* max. time for scrolling in milliseconds*/

  UINT          uItemHeight;    /* item height */
  BOOL          bHeightSet;

  LONG          clientWidth;    /* width of control window */
  LONG          clientHeight;   /* height of control window */

  LONG          treeWidth;      /* width of displayed tree items */
  LONG          treeHeight;     /* height of displayed tree items */

  UINT          uIndent;        /* indentation in pixels */
  HTREEITEM     selectedItem;   /* handle to selected item or 0 if none */
  HTREEITEM     focusItem;      /* handle to item that has focus, 0 if none */
  HTREEITEM     hotItem;        /* handle currently under cursor, 0 if none */
  HTREEITEM     firstVisible;   /* handle to first visible item */
  int           maxDisplayOrder;
  HTREEITEM     dropItem;       /* handle to item selected by drag cursor */
  HTREEITEM     insertMarkItem; /* item after which insertion mark is placed */
  BOOL          insertBeforeorAfter; /* flag used by TVM_SETINSERTMARK */
  HIMAGELIST    dragList;       /* Bitmap of dragged item */
  COLORREF      clrBk;
  COLORREF      clrText;
  COLORREF      clrLine;
  COLORREF      clrInsertMark;
  HFONT         hFont;
  HFONT         hBoldFont;
  HWND          hwndToolTip;

  HWND          hwndEdit;
  WNDPROC       wpEditOrig;     /* orig window proc for subclassing edit */
  BOOL          bIgnoreEditKillFocus;
  BOOL          bLabelChanged;

  HIMAGELIST    himlNormal;
  int           normalImageHeight;
  int           normalImageWidth;
  HIMAGELIST    himlState;
  int           stateImageHeight;
  int           stateImageWidth;
  HDPA          items;

  HTREEITEM       tipItem;        // item with tooltip
  POINT           lefttop;        //in pixels

  LPWSTR          pszISearch;
  UINT            uISearchLen;
  DWORD           dwISearchTime;
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
#define TV_INFOTIP_TIMER 3

#define TV_REFRESH_TIMER_SET 1
#define TV_EDIT_TIMER_SET    2
#define TV_INFOTIP_TIMER_SET 4

#endif  /* __WINE_TREEVIEW_H */
