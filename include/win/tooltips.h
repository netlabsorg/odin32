/* $Id: tooltips.h,v 1.6 2001-03-31 13:23:34 sandervl Exp $ */

/*
 * Tool tips class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_TOOLTIPS_H
#define __WINE_TOOLTIPS_H


typedef struct tagTT_SUBCLASS_INFO
{
    WNDPROC wpOrigProc;
    HWND    hwndToolTip;
    UINT    uRefCount;
} TT_SUBCLASS_INFO, *LPTT_SUBCLASS_INFO;


typedef struct tagTTTOOL_INFO
{
    UINT      uFlags;
    HWND      hwnd;
    UINT      uId;
    RECT      rect;
    HINSTANCE hinst;
    LPWSTR    lpszText;
    LPARAM    lParam;
} TTTOOL_INFO;


typedef struct tagTOOLTIPS_INFO
{
    COMCTL32_HEADER header;

    WCHAR    szTipText[INFOTIPSIZE];
    BOOL     bActive;
    BOOL     bTrackActive;
    UINT     uNumTools;
    COLORREF clrBk;
    COLORREF clrText;
    HFONT    hFont;
    INT      xTrackPos;
    INT      yTrackPos;
    INT      nMaxTipWidth;
    INT      nTool;
    INT      nOldTool;
    INT      nCurrentTool;
    INT      nTrackTool;
    INT      nAutomaticTime;
    INT      nReshowTime;
    INT      nAutoPopTime;
    INT      nInitialTime;
    RECT     rcMargin;

    TTTOOL_INFO *tools;
} TOOLTIPS_INFO;



#endif  /* __WINE_TOOLTIPS_H */
