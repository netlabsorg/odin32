/*
 * Listview class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 2000 Christoph Bratschi
 */

#ifndef __WINE_LISTVIEW_H
#define __WINE_LISTVIEW_H

/* Some definitions for inline edit control */
typedef BOOL (*EditlblCallback)(HWND, LPSTR, DWORD);

typedef struct tagEDITLABEL_ITEM
{
    WNDPROC EditWndProc;
    DWORD param;
    EditlblCallback EditLblCb;
} EDITLABEL_ITEM;

typedef struct tagLISTVIEW_SUBITEM
{
    LPWSTR pszText;
    INT iImage;
    INT iSubItem;

} LISTVIEW_SUBITEM;

typedef struct tagLISTVIEW_ITEM
{
  UINT state;
  LPWSTR pszText;
  INT iImage;
  LPARAM lParam;
  INT iIndent;
  POINT ptPosition;

} LISTVIEW_ITEM;


typedef struct tagLISTVIEW_INFO
{
    COMCTL32_HEADER header;

    COLORREF clrBk;
    COLORREF clrText;
    COLORREF clrTextBk;
    HIMAGELIST himlNormal;
    HIMAGELIST himlSmall;
    HIMAGELIST himlState;
    BOOL bLButtonDown;
    BOOL bRButtonDown;
    INT nFocusedItem;
    INT nItemHeight;
    INT nItemWidth;
    INT nSelectionMark;
    INT nHotItem;
    RECT rcList;
    RECT rcView;
    SIZE iconSize;
    SIZE iconSpacing;
    UINT uCallbackMask;
    HWND hwndHeader;
    HFONT hDefaultFont;
    HFONT hFont;
    BOOL bFocus;
    DWORD dwExStyle;    /* extended listview style */
    HDPA hdpaItems;
    PFNLVCOMPARE pfnCompare;
    LPARAM lParamSort;
    HWND hwndEdit;
    BOOL bDoEditLabel;
    EDITLABEL_ITEM *pedititem;

} LISTVIEW_INFO;


extern VOID LISTVIEW_Register (VOID);
extern VOID LISTVIEW_Unregister (VOID);

#endif  /* __WINE_LISTVIEW_H */
