/* $Id: rebar.h,v 1.2 1999-12-18 19:50:04 achimha Exp $ */

/*
 * Rebar class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_REBAR_H
#define __WINE_REBAR_H

typedef struct tagREBAR_BAND
{
    UINT    fStyle;
    COLORREF  clrFore;
    COLORREF  clrBack;
    INT     iImage;
    HWND    hwndChild;
    UINT    cxMinChild;
    UINT    cyMinChild;
    UINT    cx;
    HBITMAP hbmBack;
    UINT    wID;
    UINT    cyChild;
    UINT    cyMaxChild;
    UINT    cyIntegral;
    UINT    cxIdeal;
    LPARAM    lParam;
    UINT    cxHeader;

    UINT    uMinHeight;
    UINT    fDraw;          /* drawing flags */
    RECT    rcBand;         /* calculated band rectangle */
    RECT    rcGripper;      /* calculated gripper rectangle */
    RECT    rcCapImage;     /* calculated caption image rectangle */
    RECT    rcCapText;      /* calculated caption text rectangle */
    RECT    rcChild;        /* calculated child rectangle */

    LPWSTR    lpText;
    HWND    hwndPrevParent;
} REBAR_BAND;

typedef struct tagREBAR_INFO
{
    COLORREF   clrBk;       /* background color */
    COLORREF   clrText;     /* text color */
    HIMAGELIST himl;        /* handle to imagelist */
    UINT     uNumBands;   /* number of bands in the rebar */
    HWND     hwndToolTip; /* handle to the tool tip control */
    HWND     hwndNotify;  /* notification window (parent) */
    HFONT    hFont;       /* handle to the rebar's font */
    SIZE     imageSize;   /* image size (image list) */

    SIZE     calcSize;    /* calculated rebar size */
    BOOL     bAutoResize; /* auto resize deadlock flag */
    BOOL     bUnicode;    /* Unicode flag */
    HCURSOR  hcurArrow;   /* handle to the arrow cursor */
    HCURSOR  hcurHorz;    /* handle to the EW cursor */
    HCURSOR  hcurVert;    /* handle to the NS cursor */
    HCURSOR  hcurDrag;    /* handle to the drag cursor */
    INT      iVersion     /* version number */

    REBAR_BAND *bands;      /* pointer to the array of rebar bands */

} REBAR_INFO;


extern VOID REBAR_Register (VOID);
extern VOID REBAR_Unregister (VOID);

#endif  /* __WINE_REBAR_H */
