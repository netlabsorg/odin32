/* $Id: trackbar.h,v 1.1 2002-10-29 13:47:32 sandervl Exp $ */

/*
 * Trackbar class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_TRACKBAR_H
#define __WINE_TRACKBAR_H

typedef struct tagTRACKBAR_INFO
{
    COMCTL32_HEADER header;

    HWND  hwnd;
    DWORD dwStyle;

    INT  nRangeMin;
    INT  nRangeMax;
    INT  nLineSize;
    INT  nPageSize;
    INT  nSelMin;
    INT  nSelMax;
    INT  nPos;
    UINT uThumbLen;
    UINT uNumTics;
    UINT  uTicFreq;
    HWND hwndToolTip;
    HWND hwndBuddyLA;
    HWND hwndBuddyRB;
    INT  fLocation;
    COLORREF clrBk;
    COLORREF clrHighlightedThumb;
    HBRUSH   hbrThumb;


    INT  flags;
    BOOL bFocus;
    RECT rcChannel;
    RECT rcSelection;
    RECT rcFullThumb;
    RECT rcThumb;
    INT  dragPos;
    LPLONG tics;
} TRACKBAR_INFO;

#define TB_REFRESH_TIMER       1
#define TB_REFRESH_DELAY       1


#endif  /* __WINE_TRACKBAR_H */
