/* $Id: trackbar.h,v 1.2 1999-06-21 15:28:44 cbratschi Exp $ */

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
	HWND hwndNotify;
    HWND hwndToolTip;
    HWND hwndBuddyLA;
    HWND hwndBuddyRB;
    INT  fLocation;
	COLORREF clrBk;
	
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



extern VOID TRACKBAR_Register (VOID);
extern VOID TRACKBAR_Unregister (VOID);

#endif  /* __WINE_TRACKBAR_H */
