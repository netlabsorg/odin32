/*
 * Win32 system color API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_WINSYS
#include <os2.h>
#include "os2native.h"
#include "misc.h"
#include "sysclr.h"

//******************************************************************************
//******************************************************************************
DWORD WIN32API GetSysColor(int winIndex)
{
 DWORD rgb;
 int   os2Index;

  switch(winIndex) {
	case COLOR_SCROLLBAR:
		os2Index = SYSCLR_SCROLLBAR;
		break;
	case COLOR_BACKGROUND:
		os2Index = SYSCLR_BACKGROUND;
		break;
	case COLOR_ACTIVECAPTION:
		os2Index = SYSCLR_ACTIVETITLETEXT;
		break;
	case COLOR_INACTIVECAPTION:
		os2Index = SYSCLR_INACTIVETITLETEXT;
		break;
	case COLOR_MENU:
		os2Index = SYSCLR_MENU;
		break;
	case COLOR_WINDOW:
		os2Index = SYSCLR_WINDOW;
		break;
	case COLOR_WINDOWFRAME:
		os2Index = SYSCLR_WINDOWFRAME;
		break;
	case COLOR_MENUTEXT:
		os2Index = SYSCLR_MENUTEXT;
		break;
	case COLOR_WINDOWTEXT:
		os2Index = SYSCLR_WINDOWTEXT;
		break;
	case COLOR_CAPTIONTEXT:
		os2Index = SYSCLR_TITLETEXT;
		break;
	case COLOR_ACTIVEBORDER:
		os2Index = SYSCLR_ACTIVEBORDER;
		break;
	case COLOR_INACTIVEBORDER:
		os2Index = SYSCLR_INACTIVEBORDER;
		break;
	case COLOR_APPWORKSPACE:
		os2Index = SYSCLR_APPWORKSPACE;
		break;
	case COLOR_HIGHLIGHT:
		os2Index = SYSCLR_HILITEBACKGROUND;
		break;
	case COLOR_HIGHLIGHTTEXT:
		os2Index = COLOR_WINDOWTEXT;
		break;
	case COLOR_BTNFACE:
		os2Index = SYSCLR_HILITEBACKGROUND;
		break;
	case COLOR_BTNSHADOW:
		os2Index = SYSCLR_SHADOW;
		break;
	case COLOR_GRAYTEXT:
		os2Index = SYSCLR_MENUDISABLEDTEXT;
		break;
	case COLOR_BTNTEXT:
		os2Index = SYSCLR_BUTTONDEFAULT;
		break;
	case COLOR_INACTIVECAPTIONTEXT:
		os2Index = SYSCLR_INACTIVETITLETEXT;
		break;
	case COLOR_BTNHIGHLIGHT:
		os2Index = SYSCLR_HILITEBACKGROUND;
		break;
	case COLOR_3DDKSHADOW:
		os2Index = SYSCLR_SHADOW;
		break;
	case COLOR_3DLIGHT:
		os2Index = SYSCLR_HILITEBACKGROUND;
		break;
	case COLOR_INFOTEXT:
		os2Index = SYSCLR_HELPTEXT;
		break;
	case COLOR_INFOBK:
		os2Index = SYSCLR_HELPBACKGROUND;
		break;
	default:
#ifdef DEBUG
		WriteLog("GetSysColor UNKNOWN index %d!!\n", winIndex);
#endif
		return(0);
  }
  rgb = WinQuerySysColor(HWND_DESKTOP, os2Index, 0);
#ifdef DEBUG
  WriteLog("GetSysColor %d returned %X\n", os2Index, rgb);
#endif
  return(rgb);
}
//******************************************************************************
//******************************************************************************
