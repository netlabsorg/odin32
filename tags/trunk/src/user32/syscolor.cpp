/* $Id: syscolor.cpp,v 1.9 1999-10-09 11:03:23 sandervl Exp $ */

/*
 * Win32 system color API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (ported from Wine)
 * Copyright 1999 Christoph Bratschi
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

//SvL: Open32 colors are much better than those in the table below
#define NUM_OPEN32_SYSCOLORS 21

#define NUM_SYS_COLORS     (COLOR_GRADIENTINACTIVECAPTION+1)

/* Win98 colors */

static COLORREF SysColors[NUM_SYS_COLORS] =
{
  RGB(192,192,192), //COLOR_SCROLLBAR
  RGB(0,0,0),       //COLOR_BACKGROUND
  RGB(0,0,128),     //COLOR_ACTIVECAPTION
  RGB(128,128,128), //COLOR_INACTIVECAPTION
  RGB(192,192,192), //COLOR_MENU
  RGB(255,255,255), //COLOR_WINDOW
  RGB(0,0,0),       //COLOR_WINDOWFRAME
  RGB(0,0,0),       //COLOR_MENUTEXT
  RGB(0,0,0),       //COLOR_WINDOWTEXT
  RGB(255,255,255), //COLOR_CAPTIONTEXT
  RGB(192,192,192), //COLOR_ACTIVEBORDER
  RGB(192,192,192), //COLOR_INACTIVEBORDER
  RGB(128,128,128), //COLOR_APPWORKSPACE
  RGB(0,0,128),     //COLOR_HIGHLIGHT
  RGB(255,255,255), //COLOR_HIGHLIGHTTEXT
  RGB(192,192,192), //COLOR_BTNFACE
  RGB(128,128,128), //COLOR_BTNSHADOW
  RGB(128,128,128), //COLOR_GRAYTEXT
  RGB(0,0,0),       //COLOR_BTNTEXT
  RGB(192,192,192), //COLOR_INACTIVECAPTIONTEXT
  RGB(255,255,255), //COLOR_BTNHIGHLIGHT
  RGB(0,0,0),       //COLOR_3DDKSHADOW
  RGB(192,192,192), //COLOR_3DLIGHT
  RGB(0,0,0),       //COLOR_INFOTEXT
  RGB(255,255,255), //COLOR_INFOBK
  RGB(184,184,184), //COLOR_ALTERNATEBTNFACE
  RGB(0,0,255),     //COLOR_HOTLIGHT
  RGB(16,136,208),  //COLOR_GRADIENTACTIVECAPTION
  RGB(16,136,208)        //COLOR_GRADIENTINACTIVECAPTION
};

static HPEN   SysColorPens[NUM_SYS_COLORS] = {0};
static HBRUSH SysColorBrushes[NUM_SYS_COLORS] = {0};
static BOOL   fColorInit = FALSE;

#define MAKE_SOLID(color) \
       (PALETTEINDEX(GetNearestPaletteIndex(STOCK_DEFAULT_PALETTE,(color))))

static void SYSCOLOR_SetColor( int index, COLORREF color )
{
    if (index < 0 || index >= NUM_SYS_COLORS || (SysColors[index] == color && fColorInit)) return;
    SysColors[index] = color;
    /* set pen */
    if (SysColorPens[index]) DeleteObject(SysColorPens[index]);
    SysColorPens[index] = CreatePen(PS_SOLID, 1, color);
    /* set brush */
    if (SysColorBrushes[index]) DeleteObject(SysColorBrushes[index]);
    SysColorBrushes[index] = CreateSolidBrush(color);
}
//******************************************************************************
//******************************************************************************
void SYSCOLOR_Load(void)
{
/* CB: load colors from Odin ini file
    int i, r, g, b;
    const char * const *p;
    char buffer[100];

    for (i = 0, p = DefSysColors; i < NUM_SYS_COLORS; i++, p += 2)
    {
        if (sscanf(p[1], " %d %d %d", &r, &g, &b ) != 3) r = g = b = 0;
        SYSCOLOR_SetColor( i, RGB(r,g,b) );
    }
*/

}
//******************************************************************************
//******************************************************************************
void SYSCOLOR_Save(void)
{
  //CB: todo
}
//******************************************************************************
//******************************************************************************
void SYSCOLOR_Init(void)
{
  INT x;

  SYSCOLOR_Load();
  for (x = 0;x < NUM_SYS_COLORS;x++) {
	if(x < NUM_OPEN32_SYSCOLORS)
		SYSCOLOR_SetColor(x,O32_GetSysColor(x));
	else	SYSCOLOR_SetColor(x,SysColors[x]);
  }
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetSysColor(INT nIndex)
{
  if(fColorInit == FALSE)
  {
    SYSCOLOR_Init();
    fColorInit = TRUE;
  }

  if (nIndex >= 0 && nIndex < NUM_SYS_COLORS) 
  {
	if(nIndex < NUM_OPEN32_SYSCOLORS)
		return O32_GetSysColor(nIndex);
	else	return SysColors[nIndex];
  }
  else return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetSysColors(INT nChanges, const INT *lpSysColor,
                              const COLORREF *lpColorValues)
{
 int i;

    if(fColorInit == FALSE)
    {
      SYSCOLOR_Init();
      fColorInit = TRUE;
    }

#ifdef DEBUG
    WriteLog("OS2SetSysColors\n");
#endif
    
    O32_SetSysColors(nChanges, lpSysColor, lpColorValues);

    for(i=0;i<nChanges;i++) {
        SYSCOLOR_SetColor(lpSysColor[i], lpColorValues[i]);
    }

    return SetSysColors(nChanges, lpSysColor, lpColorValues);

    /* Send WM_SYSCOLORCHANGE message to all windows */

    SendMessageA( HWND_BROADCAST, WM_SYSCOLORCHANGE, 0, 0 );

    /* Repaint affected portions of all visible windows */

    RedrawWindow( GetDesktopWindow(), NULL, 0,
                RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN );
    return TRUE;
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API GetSysColorBrush(int nIndex)
{
#ifdef DEBUG
  WriteLog("OS2GetSysColorBrush %d returned", nIndex);
#endif
  if (!fColorInit)
  {
    SYSCOLOR_Init();
    fColorInit = TRUE;
  }

  if (nIndex >= 0 && nIndex < NUM_SYS_COLORS) return SysColorBrushes[nIndex];
  else return(GetStockObject(LTGRAY_BRUSH));

}
//******************************************************************************
//******************************************************************************
/***********************************************************************
 * This function is new to the Wine lib -- it does not exist in
 * Windows. However, it is a natural complement for GetSysColorBrush
 * in the Win32 API and is needed quite a bit inside Wine.
 */
HPEN WIN32API GetSysColorPen(INT index)
{
    if (!fColorInit)
    {
      SYSCOLOR_Init();
      fColorInit = TRUE;
    }

    if (index >= 0 && index < NUM_SYS_COLORS) return SysColorPens[index];
    else return 0;
}

