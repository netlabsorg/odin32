/* $Id: syscolor.h,v 1.1 1999-07-14 08:35:35 sandervl Exp $ */

/*
 * WINE:
 * System color objects
 *
 * Copyright  Alexandre Julliard, 1994
 */

#ifndef __WINE_SYSCOLOR_H
#define __WINE_SYSCOLOR_H

//GetSysColor extra colors
#define COLOR_3DDKSHADOW        21
#define COLOR_3DLIGHT           22
#define COLOR_INFOTEXT          23
#define COLOR_INFOBK            24
/* win98 colors */
#define COLOR_ALTERNATEBTNFACE         25  /* undocumented, constant's name unknown */
#define COLOR_HOTLIGHT                 26
#define COLOR_GRADIENTACTIVECAPTION    27
#define COLOR_GRADIENTINACTIVECAPTION  28

#define COLOR_DESKTOP           COLOR_BACKGROUND
#define COLOR_3DFACE            COLOR_BTNFACE
#define COLOR_3DSHADOW          COLOR_BTNSHADOW
#define COLOR_3DHIGHLIGHT       COLOR_BTNHIGHLIGHT
#define COLOR_3DHILIGHT         COLOR_BTNHIGHLIGHT
#define COLOR_BTNHILIGHT        COLOR_BTNHIGHLIGHT


BOOL   WIN32API OS2UpdateWindow(HWND hwnd);
HBRUSH WIN32API OS2GetSysColorBrush(int nIndex);

struct SysColorObjects
{
    HBRUSH hbrushScrollbar;        /* COLOR_SCROLLBAR           */
    HBRUSH hbrushBackground;       /* COLOR_BACKGROUND          */
    HBRUSH hbrushActiveCaption;    /* COLOR_ACTIVECAPTION       */
    HBRUSH hbrushInactiveCaption;  /* COLOR_INACTIVECAPTION     */
    HBRUSH hbrushMenu;             /* COLOR_MENU                */
    HBRUSH hbrushWindow;           /* COLOR_WINDOW              */
    HBRUSH hbrushWindowFrame;      /* COLOR_WINDOWFRAME         */
    HBRUSH hbrushMenuText;         /* COLOR_MENUTEXT            */
    HBRUSH hbrushWindowText;       /* COLOR_WINDOWTEXT          */
    HBRUSH hbrushCaptionText;      /* COLOR_CAPTIONTEXT         */
    HBRUSH hbrushActiveBorder;     /* COLOR_ACTIVEBORDER        */
    HBRUSH hbrushInactiveBorder;   /* COLOR_INACTIVEBORDER      */
    HBRUSH hbrushAppWorkspace;     /* COLOR_APPWORKSPACE        */
    HBRUSH hbrushHighlight;        /* COLOR_HIGHLIGHT           */
    HBRUSH hbrushHighlightText;    /* COLOR_HIGHLIGHTTEXT       */
    HBRUSH hbrushBtnFace;          /* COLOR_BTNFACE             */
    HBRUSH hbrushBtnShadow;        /* COLOR_BTNSHADOW           */
    HBRUSH hbrushGrayText;         /* COLOR_GRAYTEXT            */
    HBRUSH hbrushBtnText;          /* COLOR_BTNTEXT             */
    HBRUSH hbrushInactiveCaptionText; /* COLOR_INACTIVECAPTIONTEXT */
    HBRUSH hbrushBtnHighlight;     /* COLOR_BTNHIGHLIGHT        */
    HBRUSH hbrush3DDkShadow;       /* COLOR_3DDKSHADOW          */
    HBRUSH hbrush3DLight;          /* COLOR_3DLIGHT             */
    HBRUSH hbrushInfoText;         /* COLOR_INFOTEXT            */
    HBRUSH hbrushInfoBk;           /* COLOR_INFOBK              */
    HBRUSH hbrushAlternateBtnFace; /* COLOR_ALTERNATEBTNFACE */
    HBRUSH hbrushHotLight;         /* COLOR_HOTLIGHT */
    HBRUSH hbrushGradientActiveCaption; /* COLOR_GRADIENTACTIVECAPTION */
    HBRUSH hbrushGradientInactiveCaption; /* COLOR_GRADIENTINACTIVECAPTION */

    HPEN   hpenWindowFrame;        /* COLOR_WINDOWFRAME         */
    HPEN   hpenWindowText;         /* COLOR_WINDOWTEXT          */
};

extern void SYSCOLOR_Init(void);
extern struct SysColorObjects sysColorObjects;

#endif  /* __WINE_SYSCOLOR_H */
