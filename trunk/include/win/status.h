/* $Id: status.h,v 1.2 2000-03-18 16:10:55 cbratschi Exp $ */

/*
 * Status window definitions
 *
 * Copyright 1996 Bruce Milner
 */

#ifndef __WINE_STATUS_H
#define __WINE_STATUS_H

typedef struct
{
    INT    x;
    INT    style;
    RECT   bound;
    LPWSTR text;
    HICON  hIcon;
} STATUSWINDOWPART;

typedef struct
{
    COMCTL32_HEADER header;

    UINT              numParts;
    UINT              textHeight;
    UINT              height;
    BOOL              simple;
    HWND              hwndToolTip;
    HFONT             hFont;
    HFONT             hDefaultFont;
    COLORREF          clrBk;     /* background color */
    STATUSWINDOWPART  part0;     /* simple window */
    STATUSWINDOWPART *parts;
} STATUSWINDOWINFO;


extern VOID STATUS_Register (VOID);
extern VOID STATUS_Unregister (VOID);

#endif  /* __WINE_STATUS_H */
