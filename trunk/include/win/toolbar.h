/* $Id: toolbar.h,v 1.9 2001-03-31 13:23:34 sandervl Exp $ */

/*
 * Toolbar class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_TOOLBAR_H
#define __WINE_TOOLBAR_H

#define MAXTOOLNAME 100 //max length of TBBUTTON_INFO.pszName

typedef struct tagTBUTTON_INFO
{
    INT iBitmap;
    INT idCommand;
    BYTE  fsState;
    BYTE  fsStyle;
    DWORD dwData;
    INT iString;

    //for our needs
    BOOL bHot;
    INT nRow;
    RECT rect;
    //Customize dialog
    BOOL   bDelete; //can delete
    LPWSTR pszName;
    INT    nCustomID;
} TBUTTON_INFO;


typedef struct tagTOOLBAR_INFO
{
    COMCTL32_HEADER header;

    DWORD    dwStructSize;   /* size of TBBUTTON struct */
    INT      nHeight;        /* height of the toolbar */
    INT      nWidth;         /* width of the toolbar */
    INT      nButtonHeight;
    INT      nButtonWidth;
    INT      nBitmapHeight;
    INT      nBitmapWidth;
    INT      nIndent;
    INT      nRows;           /* number of button rows */
    INT      nMaxTextRows;    /* maximum number of text rows */
    INT      cxMin;           /* minimum button width */
    INT      cxMax;           /* maximum button width */
    INT      nNumButtons;     /* number of buttons */
    INT      nNumBitmaps;     /* number of bitmaps */
    INT      nNumStrings;     /* number of strings */
    BOOL     bCaptured;       /* mouse captured? */
    INT      nButtonDown;
    INT      nOldHit;
    INT      nHotItem;        /* index of the "hot" item */
    HFONT    hFont;           /* text font */
    HIMAGELIST himlInt;         /* image list created internally */
    HIMAGELIST himlDef;         /* default image list for TB_SETIMAGELIST*/
    HIMAGELIST himlHot;         /* hot image list */
    HIMAGELIST himlDis;         /* disabled image list */
    HWND     hwndToolTip;     /* handle to tool tip control */
    BOOL     bTransparent;    /* background transparency flag */
    BOOL     bAutoSize;       /* auto size deadlock indicator */
    BOOL     bAnchor;         /* anchor highlight enabled */
    DWORD    dwExStyle;       /* extended toolbar style */
    DWORD    dwDTFlags;       /* DrawText flags */

    COLORREF clrInsertMark;   /* insert mark color */
    RECT     rcBound;         /* bounding rectangle */

    TBUTTON_INFO *buttons;      /* pointer to button array */
    LPWSTR       *strings;      /* pointer to string array */

    //Customize dialog
    HWND          hwndToolbar;
    TBUTTON_INFO* oldButtons;
    INT           nNumOldButtons;
    INT           nMaxCustomID;
    BOOL          changed;
} TOOLBAR_INFO;



#endif  /* __WINE_TOOLBAR_H */

