/*
 * Listview class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 2000 Christoph Bratschi
 */

#ifndef __WINE_LISTVIEW_H
#define __WINE_LISTVIEW_H

/* Some definitions for inline edit control */
typedef BOOL (*EditlblCallback)(HWND,LPSTR,DWORD,BOOL);

typedef struct tagEDITLABEL_ITEM
{
    WNDPROC         EditWndProc;
    DWORD           param;
    EditlblCallback EditLblCb;
} EDITLABEL_ITEM;

typedef struct tagLISTVIEW_SUBITEM
{
    LPWSTR pszText;
    INT    iImage;
    INT    iSubItem;

} LISTVIEW_ITEMDATA;

typedef struct tagLISTVIEW_ITEM
{
  UINT   state;
  LPARAM lParam;
  INT    iIndent;
  POINT  ptPosition;
  INT    iWorkArea;
  HDPA   hdpaSubItems;

} LISTVIEW_ITEM;


typedef struct tagLISTVIEW_INFO
{
    COMCTL32_HEADER header;

    COLORREF       clrBk;
    COLORREF       clrText;
    COLORREF       clrTextBk;
    HIMAGELIST     himlNormal;
    HIMAGELIST     himlSmall;
    HIMAGELIST     himlState;
    HCURSOR        hHotCursor;
    BOOL           bLButtonDown;
    BOOL           bRButtonDown;
    INT            nFocusedItem;
    INT            nItemHeight;
    INT            nItemWidth;
    INT            nSelectionMark;
    INT            nHotItem;
    RECT           rcList;       //visible part of client
    RECT           rcView;       //bounding rectangle for icon view
    INT            nWorkAreas;
    RECT          *rcWorkAreas;
    SIZE           iconSize;
    SIZE           iconSpacing;
    UINT           uCallbackMask;
    HWND           hwndHeader;
    HFONT          hDefaultFont;
    HFONT          hFont;
    BOOL           bFocus;
    DWORD          dwStyle;      //window style
    UINT           uView;        //view
    DWORD          dwExStyle;    /* extended listview style */
    HDPA           hdpaItems;
    HDPA           hdpaSelItems; //selected items (if not single sel mode)
    PFNLVCOMPARE   pfnCompare;
    LPARAM         lParamSort;
    HWND           hwndEdit;
    HWND           hwndToolTip;
    BOOL           bDoEditLabel;
    EDITLABEL_ITEM *pedititem;
    POINT          lefttop;      //in scroll units
    POINT          maxScroll;    //in scroll units
    POINT          scrollPage;   //in scroll units
    POINT          scrollStep;   //in pixels
    DWORD          internalFlags;
    INT            hoverTime;

    LPWSTR         pszISearch;
    UINT           uISearchLen;
    DWORD          dwISearchTime;
} LISTVIEW_INFO;

typedef struct
{
  LVITEMW header;
  BOOL mustFree;
  BOOL unicode;
} LVINTERNALITEMW, *LPLVINTERNALITEMW;

#define IF_NOREDRAW     1


#endif  /* __WINE_LISTVIEW_H */
