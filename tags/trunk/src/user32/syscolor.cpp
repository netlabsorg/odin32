/* $Id: syscolor.cpp,v 1.30 2001-10-15 17:09:04 sandervl Exp $ */

/*
 * Win32 system color API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (ported from Wine)
 * Copyright 1999 Christoph Bratschi
 * Copyright 1999 Achim Hasenmueller
 *
 * WINE:
 * Support for system colors
 *
 * Copyright  David W. Metcalfe, 1993
 * Copyright  Alexandre Julliard, 1994
 * Copyright 1997 Bertho A. Stultiens
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "user32.h"
#include "syscolor.h"
#include "options.h"

#define DBG_LOCALLOG    DBG_syscolor
#include "dbglocal.h"

#define CTLCOLOR_MAX   CTLCOLOR_STATIC

//SvL: Open32 colors are much better than those in the table below
#define NUM_OPEN32_SYSCOLORS 21

#define NUM_SYS_COLORS     (COLOR_GRADIENTINACTIVECAPTION+1)

BOOL USEWINCOLORS = 1;

/* Win98 colors */

static COLORREF SysColors[NUM_SYS_COLORS] =
{
  RGB(198,195,198), //COLOR_SCROLLBAR
  RGB(0,0,0),       //COLOR_BACKGROUND
  RGB(0,0,128),     //COLOR_ACTIVECAPTION
  RGB(128,128,128), //COLOR_INACTIVECAPTION
  RGB(198,195,198), //COLOR_MENU
  RGB(255,255,255), //COLOR_WINDOW
  RGB(0,0,0),       //COLOR_WINDOWFRAME
  RGB(0,0,0),       //COLOR_MENUTEXT
  RGB(0,0,0),       //COLOR_WINDOWTEXT
  RGB(255,255,255), //COLOR_CAPTIONTEXT
  RGB(198,195,198), //COLOR_ACTIVEBORDER
  RGB(198,195,198), //COLOR_INACTIVEBORDER
  RGB(128,128,128), //COLOR_APPWORKSPACE
  RGB(0,0,128),     //COLOR_HIGHLIGHT
  RGB(255,255,255), //COLOR_HIGHLIGHTTEXT
  RGB(198,195,198), //COLOR_BTNFACE
  RGB(128,128,128), //COLOR_BTNSHADOW
  RGB(128,128,128), //COLOR_GRAYTEXT
  RGB(0,0,0),       //COLOR_BTNTEXT
  RGB(198,195,198), //COLOR_INACTIVECAPTIONTEXT
  RGB(255,255,255), //COLOR_BTNHIGHLIGHT
  RGB(0,0,0),       //COLOR_3DDKSHADOW
  RGB(198,195,198), //COLOR_3DLIGHT
  RGB(0,0,0),       //COLOR_INFOTEXT
  RGB(255,255,255), //COLOR_INFOBK
  RGB(198,195,198), //COLOR_ALTERNATEBTNFACE
  RGB(0,0,255),     //COLOR_HOTLIGHT
  RGB(16,136,208),  //COLOR_GRADIENTACTIVECAPTION
//  RGB(16,136,208)   //COLOR_GRADIENTINACTIVECAPTION
  RGB(198,195,198)  //COLOR_GRADIENTINACTIVECAPTION
};

static char* ColorNames[NUM_SYS_COLORS] =
{
  "SCROLLBAR",
  "COLOR_BACKGROUND",
  "ACTIVECAPTION",
  "INACTIVECAPTION",
  "MENU",
  "WINDOW",
  "WINDOWFRAME",
  "MENUTEXT",
  "WINDOWTEXT",
  "CAPTIONTEXT",
  "ACTIVEBORDER",
  "INACTIVEBORDER",
  "APPWORKSPACE",
  "HIGHLIGHT",
  "HIGHLIGHTTEXT",
  "BTNFACE",
  "BTNSHADOW",
  "GRAYTEXT",
  "BTNTEXT",
  "INACTIVECAPTIONTEXT",
  "BTNHIGHLIGHT",
  "3DDKSHADOW",
  "3DLIGHT",
  "INFOTEXT",
  "INFOBK",
  "ALTERNATEBTNFACE",
  "COLOR_HOTLIGHT",
  "GRADIENTACTIVECAPTION",
  "GRADIENTINACTIVECAPTION"
};

static HPEN   SysColorPens[NUM_SYS_COLORS] = {0};
static HBRUSH SysColorBrushes[NUM_SYS_COLORS] = {0};
static BOOL   fColorInit = FALSE;

#define MAKE_SOLID(color) \
       (PALETTEINDEX(GetNearestPaletteIndex(STOCK_DEFAULT_PALETTE,(color))))

static const WORD wPattern55AA[] =
{
    0x5555, 0xaaaa, 0x5555, 0xaaaa,
    0x5555, 0xaaaa, 0x5555, 0xaaaa
};

static HBRUSH  hPattern55AABrush = 0;
static HBITMAP hPattern55AABitmap = 0;

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
  int i, r, g, b;
  const char * const *p;
  char buffer[100];

  if (PROFILE_GetOdinIniBool(ODINCOLORS,"SaveColors",FALSE))
  {
    for (i = 0,p = ColorNames; i < NUM_SYS_COLORS; i++, p++)
    {
      if (PROFILE_GetOdinIniString(ODINCOLORS,(LPCSTR)*p,"",buffer,sizeof(buffer)))
      {
        if (sscanf(buffer,"%d %d %d",&r,&g,&b) == 3) SysColors[i] = RGB(r,g,b);
      }
    }
  }
}
//******************************************************************************
//******************************************************************************
void SYSCOLOR_Save(void)
{
  INT x;
  const char * const *p;
  char buffer[100];

  if (!fColorInit) return;

  if (PROFILE_GetOdinIniBool(ODINCOLORS,"SaveColors",FALSE))
  {
    for (x = 0,p = ColorNames;x < NUM_SYS_COLORS;x++,p++)
    {
      INT color = SysColors[x];

      sprintf(buffer,"%d %d %d",color & 0xFF,(color & 0xFF00) >> 8,(color & 0xFF0000) >> 16);
      PROFILE_SetOdinIniString(ODINCOLORS,(LPCSTR)*p,(LPCSTR)buffer);
    }
  }
}
//******************************************************************************
//******************************************************************************
void SYSCOLOR_Init(int fOverride)
{
  INT x;

  if(fOverride == -1) {
       USEWINCOLORS = PROFILE_GetOdinIniBool(ODINCOLORS,"UseWinColors",TRUE);
  }
  else USEWINCOLORS = fOverride;

  SYSCOLOR_Load();
  if (USEWINCOLORS)
    for (x = 0;x < NUM_SYS_COLORS;x++) SYSCOLOR_SetColor(x,SysColors[x]);
  else
    for (x = 0;x < NUM_SYS_COLORS;x++) {
        if(x < NUM_OPEN32_SYSCOLORS)
                SYSCOLOR_SetColor(x,O32_GetSysColor(x));
        else    SYSCOLOR_SetColor(x,SysColors[x]);
  }
  fColorInit = TRUE;
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

  if ((nIndex >= 0) && (nIndex < NUM_SYS_COLORS))
  {
    if (USEWINCOLORS) return SysColors[nIndex];
    else
        if(nIndex < NUM_OPEN32_SYSCOLORS)
                return O32_GetSysColor(nIndex);
        else    return SysColors[nIndex];
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

    dprintf(("SetSysColors\n"));

    O32_SetSysColors(nChanges, lpSysColor, lpColorValues);

    for(i=0;i<nChanges;i++) {
        SYSCOLOR_SetColor(lpSysColor[i], lpColorValues[i]);
    }

//SvL: ???
////////    return SetSysColors(nChanges, lpSysColor, lpColorValues);

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
  if (!fColorInit)
  {
    SYSCOLOR_Init();
    fColorInit = TRUE;
  }
  dprintf(("GetSysColorBrush %d returned %x ", nIndex, ((nIndex >= 0) && (nIndex < NUM_SYS_COLORS)) ? SysColorBrushes[nIndex]:0));
  if( ((nIndex >= 0) && (nIndex < NUM_SYS_COLORS))  )
      return SysColorBrushes[nIndex];

  //TODO: is this still necessary (check in NT)
  dprintf2(("WARNING: Unknown index(%d)", nIndex ));
  return GetStockObject(LTGRAY_BRUSH);
}
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

    return ((index >= 0) && (index < NUM_SYS_COLORS)) ? SysColorPens[index]:0;
}
//******************************************************************************
//******************************************************************************
INT SYSCOLOR_GetLastColor(VOID)
{
  return NUM_SYS_COLORS-1;
}
//******************************************************************************
//******************************************************************************
INT SYSCOLOR_GetNumColors(VOID)
{
  return NUM_SYS_COLORS;
}
//******************************************************************************
//******************************************************************************
BOOL SYSCOLOR_GetUseWinColors(VOID)
{
  return USEWINCOLORS;
}
/*********************************************************************
 *      GetPattern55AABrush - doesn't exist in Win32
 */
HBRUSH WIN32API GetPattern55AABrush(void)
{
    if (!hPattern55AABrush)
        hPattern55AABrush = CreatePatternBrush(GetPattern55AABitmap());
    return hPattern55AABrush;
}
/*********************************************************************
 *      GetPattern55AABitmap - doesn't exist in Win32
 */
HBITMAP WIN32API GetPattern55AABitmap(void)
{
    if (!hPattern55AABitmap)
        hPattern55AABitmap = CreateBitmap( 8, 8, 1, 1, wPattern55AA );
    return hPattern55AABitmap;
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API GetControlBrush(HWND hwnd, HDC hdc, DWORD ctlType)
{
 HBRUSH bkgBrush = 0;

    if(ctlType <= CTLCOLOR_MAX)
    {
        bkgBrush = (HBRUSH)SendMessageA(GetParent(hwnd), WM_CTLCOLORMSGBOX + ctlType, 
                                        (WPARAM)hdc, (LPARAM)hwnd );

        if(GetObjectType(bkgBrush) != OBJ_BRUSH)
             bkgBrush = DefWindowProcA(hwnd, WM_CTLCOLORMSGBOX + ctlType, hdc, ctlType);
    }
    return bkgBrush;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsSystemPen(HPEN hPen)
{
    for(int i=0;i<NUM_SYS_COLORS;i++) {
        if(SysColorPens[i] == hPen) {
            return TRUE;
        }
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsSystemBrush(HBRUSH hBrush)
{
    for(int i=0;i<NUM_SYS_COLORS;i++) {
        if(SysColorBrushes[i] == hBrush) {
            return TRUE;
        }
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
