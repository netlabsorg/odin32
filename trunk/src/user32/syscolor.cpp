/* $Id: syscolor.cpp,v 1.1 1999-05-24 20:20:02 ktk Exp $ */

/*
 * Win32 system color API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (ported from Wine)
 *
 * WINE:
 * Support for system colors
 *
 * Copyright  David W. Metcalfe, 1993
 * Copyright  Alexandre Julliard, 1994
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "user32.h"
#include "syscolor.h"

struct SysColorObjects sysColorObjects;

static const char * const DefSysColors[] =
{
    "Scrollbar", "224 224 224",      /* COLOR_SCROLLBAR           */
    "Background", "192 192 192",     /* COLOR_BACKGROUND          */
    "ActiveTitle", "0 64 128",       /* COLOR_ACTIVECAPTION       */
    "InactiveTitle", "255 255 255",  /* COLOR_INACTIVECAPTION     */
    "Menu", "192 192 192",           /* COLOR_MENU                */
    "Window", "255 255 255",         /* COLOR_WINDOW              */
    "WindowFrame", "0 0 0",          /* COLOR_WINDOWFRAME         */
    "MenuText", "0 0 0",             /* COLOR_MENUTEXT            */
    "WindowText", "0 0 0",           /* COLOR_WINDOWTEXT          */
    "TitleText", "255 255 255",      /* COLOR_CAPTIONTEXT         */
    "ActiveBorder", "128 128 128",   /* COLOR_ACTIVEBORDER        */
    "InactiveBorder", "255 255 255", /* COLOR_INACTIVEBORDER      */
    "AppWorkspace", "255 255 232",   /* COLOR_APPWORKSPACE        */
    "Hilight", "166 202 240",        /* COLOR_HIGHLIGHT           */
    "HilightText", "0 0 0",          /* COLOR_HIGHLIGHTTEXT       */
    "ButtonFace", "192 192 192",     /* COLOR_BTNFACE             */
    "ButtonShadow", "128 128 128",   /* COLOR_BTNSHADOW           */
    "GrayText", "192 192 192",       /* COLOR_GRAYTEXT            */
    "ButtonText", "0 0 0",           /* COLOR_BTNTEXT             */
    "InactiveTitleText", "0 0 0",    /* COLOR_INACTIVECAPTIONTEXT */
    "ButtonHilight", "255 255 255",  /* COLOR_BTNHIGHLIGHT        */
    "3DDarkShadow", "32 32 32",      /* COLOR_3DDKSHADOW          */
    "3DLight", "192 192 192",        /* COLOR_3DLIGHT             */
    "InfoText", "0 0 0",             /* COLOR_INFOTEXT            */
    "InfoBackground", "255 255 192"  /* COLOR_INFOBK              */
};


#define NUM_SYS_COLORS     (COLOR_INFOBK+1)

static HBRUSH SysColors[NUM_SYS_COLORS] = {0};
static BOOL   fColorInit = FALSE;

#define MAKE_SOLID(color) \
       (PALETTEINDEX(GetNearestPaletteIndex(STOCK_DEFAULT_PALETTE,(color))))

//******************************************************************************
//******************************************************************************
static void SYSCOLOR_SetColor( int index, COLORREF color )
{
    if (index < 0 || index >= NUM_SYS_COLORS) return;
    SysColors[index] = color;
    switch(index)
    {
    case COLOR_SCROLLBAR:
	DeleteObject( sysColorObjects.hbrushScrollbar );
	sysColorObjects.hbrushScrollbar = CreateSolidBrush( color );
	break;
    case COLOR_BACKGROUND:
	DeleteObject( sysColorObjects.hbrushBackground );
	sysColorObjects.hbrushBackground = CreateSolidBrush( color );
	break;
    case COLOR_ACTIVECAPTION:
	DeleteObject( sysColorObjects.hbrushActiveCaption );
	sysColorObjects.hbrushActiveCaption = CreateSolidBrush( color );
	break;
    case COLOR_INACTIVECAPTION:
	DeleteObject( sysColorObjects.hbrushInactiveCaption );
	sysColorObjects.hbrushInactiveCaption = CreateSolidBrush( color );
	break;
    case COLOR_MENU:
	DeleteObject( sysColorObjects.hbrushMenu );
//	sysColorObjects.hbrushMenu = CreateSolidBrush( MAKE_SOLID(color) );
	sysColorObjects.hbrushMenu = CreateSolidBrush( color );
	break;
    case COLOR_WINDOW:
	DeleteObject( sysColorObjects.hbrushWindow );
	sysColorObjects.hbrushWindow = CreateSolidBrush( color );
	break;
    case COLOR_WINDOWFRAME:
	DeleteObject( sysColorObjects.hbrushWindowFrame );
	sysColorObjects.hbrushWindowFrame = CreateSolidBrush( color );
	/* FIXME: we should not need this pen */
	DeleteObject( sysColorObjects.hpenWindowFrame ); 
	sysColorObjects.hpenWindowFrame = CreatePen( PS_SOLID, 1, color );
	break;
    case COLOR_MENUTEXT:
	DeleteObject( sysColorObjects.hbrushMenuText );
	sysColorObjects.hbrushMenuText = CreateSolidBrush( color );
	break;
    case COLOR_WINDOWTEXT:
	DeleteObject( sysColorObjects.hbrushWindowText );
	sysColorObjects.hbrushWindowText = CreateSolidBrush( color );
	/* FIXME: we should not need this pen */
	DeleteObject( sysColorObjects.hpenWindowText );
	sysColorObjects.hpenWindowText = CreatePen( PS_DOT, 1, color );
	break;
    case COLOR_CAPTIONTEXT:
	DeleteObject( sysColorObjects.hbrushCaptionText );
	sysColorObjects.hbrushCaptionText = CreateSolidBrush( color );
	break;
    case COLOR_ACTIVEBORDER:
	DeleteObject( sysColorObjects.hbrushActiveBorder );
	sysColorObjects.hbrushActiveBorder = CreateSolidBrush( color );
	break;
    case COLOR_INACTIVEBORDER:
	DeleteObject( sysColorObjects.hbrushInactiveBorder );
	sysColorObjects.hbrushInactiveBorder = CreateSolidBrush( color );
	break;
    case COLOR_APPWORKSPACE:
	DeleteObject( sysColorObjects.hbrushAppWorkspace );
	sysColorObjects.hbrushAppWorkspace = CreateSolidBrush( color );
	break;
    case COLOR_HIGHLIGHT:
	DeleteObject( sysColorObjects.hbrushHighlight );
	sysColorObjects.hbrushHighlight = CreateSolidBrush( color );
	break;
    case COLOR_HIGHLIGHTTEXT:
	DeleteObject( sysColorObjects.hbrushHighlightText );
	sysColorObjects.hbrushHighlightText = CreateSolidBrush( color );
	break;
    case COLOR_BTNFACE:
	DeleteObject( sysColorObjects.hbrushBtnFace );
	sysColorObjects.hbrushBtnFace = CreateSolidBrush( color );
	break;
    case COLOR_BTNSHADOW:
	DeleteObject( sysColorObjects.hbrushBtnShadow );
	sysColorObjects.hbrushBtnShadow = CreateSolidBrush( color );
	break;
    case COLOR_GRAYTEXT:
	DeleteObject( sysColorObjects.hbrushGrayText );
	sysColorObjects.hbrushGrayText = CreateSolidBrush( color );
    case COLOR_BTNTEXT:
	DeleteObject( sysColorObjects.hbrushBtnText );
	sysColorObjects.hbrushBtnText = CreateSolidBrush( color );
	break;
    case COLOR_INACTIVECAPTIONTEXT:
	DeleteObject( sysColorObjects.hbrushInactiveCaptionText );
	sysColorObjects.hbrushInactiveCaptionText = CreateSolidBrush(color);
	break;
    case COLOR_BTNHIGHLIGHT:
	DeleteObject( sysColorObjects.hbrushBtnHighlight );
	sysColorObjects.hbrushBtnHighlight = CreateSolidBrush( color );
	break;
    case COLOR_3DDKSHADOW:
	DeleteObject( sysColorObjects.hbrush3DDkShadow );
	sysColorObjects.hbrush3DDkShadow = CreateSolidBrush( color );
	break;
    case COLOR_3DLIGHT:
	DeleteObject( sysColorObjects.hbrush3DLight );
	sysColorObjects.hbrush3DLight = CreateSolidBrush( color );
	break;
    case COLOR_INFOTEXT:
	DeleteObject( sysColorObjects.hbrushInfoText );
	sysColorObjects.hbrushInfoText = CreateSolidBrush( color );
	break;
    case COLOR_INFOBK:
	DeleteObject( sysColorObjects.hbrushInfoBk );
	sysColorObjects.hbrushInfoBk = CreateSolidBrush( color );
	break;
    }
}
//******************************************************************************
//******************************************************************************
void SYSCOLOR_Init(void)
{
    int i, r, g, b;
    const char * const *p;
    char buffer[100];

    for (i = 0, p = DefSysColors; i < NUM_SYS_COLORS; i++, p += 2)
    {
	if (sscanf(p[1], " %d %d %d", &r, &g, &b ) != 3) r = g = b = 0;
	SYSCOLOR_SetColor( i, RGB(r,g,b) );
    }
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetSysColor(INT nIndex)
{
  if(fColorInit == FALSE) {
	SYSCOLOR_Init();
	fColorInit = TRUE;
  }
#if 0
  if (nIndex >= 0 && nIndex < NUM_SYS_COLORS)
	return SysColors[nIndex];
  else  return 0;
#else
  if(nIndex < COLOR_3DDKSHADOW)
	return O32_GetSysColor(nIndex);
  else  return SysColors[nIndex];
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetSysColors(INT nChanges, const INT *lpSysColor,
                              const COLORREF *lpColorValues)
{
 int i;

    if(fColorInit == FALSE) {
	SYSCOLOR_Init();
	fColorInit = TRUE;
    }

#ifdef DEBUG
    WriteLog("OS2SetSysColors\n");
#endif

    for(i=0;i<nChanges;i++) {
	SYSCOLOR_SetColor(lpSysColor[i], lpColorValues[i]);
    }

    return SetSysColors(nChanges, lpSysColor, lpColorValues);
#if 0
    /* Send WM_SYSCOLORCHANGE message to all windows */

    SendMessage32A( HWND_BROADCAST, WM_SYSCOLORCHANGE, 0, 0 );

    /* Repaint affected portions of all visible windows */

    RedrawWindow32( GetDesktopWindow32(), NULL, 0,
                RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN );
    return TRUE;
#endif
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API GetSysColorBrush(int nIndex)
{
 static BOOL fFirst = TRUE;
 LOGBRUSH    lbrush;
 int         i;

#ifdef DEBUG
  WriteLog("OS2GetSysColorBrush %d returned", nIndex);
#endif
  if(fFirst == TRUE) {
	for(i=0;i<sizeof(SysColors)/sizeof(HBRUSH);i++) {
		lbrush.lbStyle = BS_SOLID;
		lbrush.lbHatch = 0;
		lbrush.lbColor = GetSysColor(i);
		SysColors[i] = CreateBrushIndirect(&lbrush);
	}
	fFirst = FALSE;
  }
  if(nIndex < sizeof(SysColors)/sizeof(HBRUSH)) {
#ifdef DEBUG
	WriteLog(" %X\n", SysColors[nIndex]);
#endif
	return(SysColors[nIndex]);
  }
#ifdef DEBUG
  WriteLog(" unknown index!\n");
#endif
  return(GetStockObject(LTGRAY_BRUSH));
}
//******************************************************************************
//******************************************************************************
