/* $Id: user32.cpp,v 1.110 2001-07-28 18:02:50 sandervl Exp $ */

/*
 * Win32 misc user32 API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1999 Christoph Bratschi
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
 *
 * Partly based on Wine code (windows\sysparams.c: SystemParametersInfoA)
 *
 * Copyright 1994 Alexandre Julliard
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*****************************************************************************
 * Name      : USER32.CPP
 * Purpose   : This module maps all Win32 functions contained in USER32.DLL
 *             to their OS/2-specific counterparts as far as possible.
 *****************************************************************************/

//Attention: many functions belong to other subsystems, move them to their
//           right place!

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <misc.h>
#include <winuser32.h>

#include "user32.h"
#include <winicon.h>
#include "syscolor.h"
#include "pmwindow.h"
#include "oslibgdi.h"
#include "oslibwin.h"
#include "oslibprf.h"

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
//#include <oslibwin.h>
#include <win32wnd.h>
#include <winuser.h>
#include "initterm.h"

#define DBG_LOCALLOG    DBG_user32
#include "dbglocal.h"

//undocumented stuff
// WIN32API CalcChildScroll
// WIN32API CascadeChildWindows
// WIN32API ClientThreadConnect
// WIN32API DragObject
// WIN32API DrawFrame
// WIN32API EditWndProc
// WIN32API EndTask
// WIN32API GetInputDesktop
// WIN32API GetNextQueueWindow
// WIN32API GetShellWindow
// WIN32API InitSharedTable
// WIN32API InitTask
// WIN32API IsHungThread
// WIN32API LockWindowStation
// WIN32API ModifyAccess
// WIN32API PlaySoundEvent
// WIN32API RegisterLogonProcess
// WIN32API RegisterNetworkCapabilities
// WIN32API RegisterSystemThread
// WIN32API SetDeskWallpaper
// WIN32API SetDesktopBitmap
// WIN32API SetInternalWindowPos
// WIN32API SetLogonNotifyWindow
// WIN32API SetShellWindow
// WIN32API SetSysColorsTemp
// WIN32API SetWindowFullScreenState
// WIN32API SwitchToThisWindow
// WIN32API SysErrorBox
// WIN32API TileChildWindows
// WIN32API UnlockWindowStation
// WIN32API UserClientDllInitialize
// WIN32API UserSignalProc
// WIN32API WinOldAppHackoMatic
// WIN32API WNDPROC_CALLBACK
// WIN32API YieldTask

ODINDEBUGCHANNEL(USER32-USER32)


/* Coordinate Transformation */

/* Rectangle Functions - parts from wine/windows/rect.c */

BOOL WIN32API CopyRect( PRECT lprcDst, const RECT * lprcSrc)
{
    dprintf2(("USER32:  CopyRect\n"));
    if (!lprcDst || !lprcSrc) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    memcpy(lprcDst,lprcSrc,sizeof(RECT));

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EqualRect( const RECT *lprc1, const RECT *lprc2)
{
    dprintf2(("USER32:  EqualRect\n"));
    if (!lprc1 || !lprc2)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    return (lprc1->left == lprc2->left &&
            lprc1->right == lprc2->right &&
            lprc1->top == lprc2->top &&
            lprc1->bottom == lprc2->bottom);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InflateRect( PRECT lprc, int dx, int  dy)
{
    dprintf2(("USER32: InflateRect (%d,%d)(%d,%d) %d,%d", lprc->left, lprc->top, lprc->right, lprc->bottom, dx, dy));
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lprc->left   -= dx;
    lprc->right  += dx;
    lprc->top    -= dy;
    lprc->bottom += dy;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IntersectRect( PRECT lprcDst, const RECT * lprcSrc1, const RECT * lprcSrc2)
{
    dprintf2(("USER32:  IntersectRect (%d,%d)(%d,%d) (%d,%d)(%d,%d)", lprcSrc1->left, lprcSrc1->top, lprcSrc1->right, lprcSrc1->bottom, lprcSrc2->left, lprcSrc2->top, lprcSrc2->right, lprcSrc2->bottom));
    if (!lprcSrc1 || !lprcSrc2)
    {
      	SetLastError(ERROR_INVALID_PARAMETER);
      	return FALSE;
    }

    if (IsRectEmpty(lprcSrc1) || IsRectEmpty(lprcSrc2) ||
       (lprcSrc1->left >= lprcSrc2->right) || (lprcSrc2->left >= lprcSrc1->right) ||
       (lprcSrc1->top >= lprcSrc2->bottom) || (lprcSrc2->top >= lprcSrc1->bottom))
    {
	//SvL: NT doesn't set the last error here
      	//SetLastError(ERROR_INVALID_PARAMETER);
      	if (lprcDst) SetRectEmpty(lprcDst);
      	return FALSE;
    }
    if (lprcDst)
    {
      lprcDst->left   = MAX(lprcSrc1->left,lprcSrc2->left);
      lprcDst->right  = MIN(lprcSrc1->right,lprcSrc2->right);
      lprcDst->top    = MAX(lprcSrc1->top,lprcSrc2->top);
      lprcDst->bottom = MIN(lprcSrc1->bottom,lprcSrc2->bottom);
    }

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsRectEmpty( const RECT * lprc)
{
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    return (lprc->left == lprc->right || lprc->top == lprc->bottom);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetRect( PRECT lprc, int x, int  y)
{
    dprintf2(("USER32: OffsetRect (%d,%d)(%d,%d) %d %d", lprc->left, lprc->top, lprc->right, lprc->bottom, x, y));
    if (!lprc)
    {
      	SetLastError(ERROR_INVALID_PARAMETER);
      	return FALSE;
    }

    lprc->left   += x;
    lprc->right  += x;
    lprc->top    += y;
    lprc->bottom += y;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtInRect( const RECT *lprc, POINT pt)
{
    dprintf2(("USER32: PtInRect (%d,%d)(%d,%d) (%d,%d)", lprc->left, lprc->top, lprc->right, lprc->bottom, pt.x, pt.y));
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    return (pt.x >= lprc->left &&
            pt.x < lprc->right &&
            pt.y >= lprc->top &&
            pt.y < lprc->bottom);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRect( PRECT lprc, int nLeft, int nTop, int nRight, int  nBottom)
{
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lprc->left   = nLeft;
    lprc->top    = nTop;
    lprc->right  = nRight;
    lprc->bottom = nBottom;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRectEmpty( PRECT lprc)
{
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lprc->left = lprc->right = lprc->top = lprc->bottom = 0;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SubtractRect( PRECT lprcDest, const RECT * lprcSrc1, const RECT * lprcSrc2)
{
    dprintf2(("USER32:  SubtractRect"));
    RECT tmp;

    if (!lprcDest || !lprcSrc1 || !lprcSrc2)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if (IsRectEmpty(lprcSrc1))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      SetRectEmpty(lprcDest);
      return FALSE;
    }
    *lprcDest = *lprcSrc1;
    if (IntersectRect(&tmp,lprcSrc1,lprcSrc2))
    {
      if (EqualRect(&tmp,lprcDest))
      {
        SetRectEmpty(lprcDest);
        return FALSE;
      }
      if ((tmp.top == lprcDest->top) && (tmp.bottom == lprcDest->bottom))
      {
        if (tmp.left == lprcDest->left) lprcDest->left = tmp.right;
        else if (tmp.right == lprcDest->right) lprcDest->right = tmp.left;
      }
      else if ((tmp.left == lprcDest->left) && (tmp.right == lprcDest->right))
      {
        if (tmp.top == lprcDest->top) lprcDest->top = tmp.bottom;
        else if (tmp.bottom == lprcDest->bottom) lprcDest->bottom = tmp.top;
      }
    }

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnionRect( PRECT lprcDst, const RECT *lprcSrc1, const RECT *lprcSrc2)
{
    dprintf2(("USER32:  UnionRect\n"));
    if (!lprcDst || !lprcSrc1 || !lprcSrc2)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if (IsRectEmpty(lprcSrc1))
    {
      if (IsRectEmpty(lprcSrc2))
      {
        SetLastError(ERROR_INVALID_PARAMETER);
        SetRectEmpty(lprcDst);
        return FALSE;
      }
      else *lprcDst = *lprcSrc2;
    }
    else
    {
      if (IsRectEmpty(lprcSrc2)) *lprcDst = *lprcSrc1;
      else
      {
        lprcDst->left   = MIN(lprcSrc1->left,lprcSrc2->left);
        lprcDst->right  = MAX(lprcSrc1->right,lprcSrc2->right);
        lprcDst->top    = MIN(lprcSrc1->top,lprcSrc2->top);
        lprcDst->bottom = MAX(lprcSrc1->bottom,lprcSrc2->bottom);
      }
    }

    return TRUE;
}

/* Mouse Input Functions */

/* Error Functions */

/*****************************************************************************
 * Name      : ExitWindowsEx
 * Purpose   : Shutdown System
 * Parameters: UINT  uFlags
 *             DWORD dwReserved
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/10/20 21:24]
 *****************************************************************************/

ODINFUNCTION2(BOOL, ExitWindowsEx, UINT,  uFlags,
                                   DWORD, dwReserved)
{
  int rc = MessageBoxA(HWND_DESKTOP,
                       "Are you sure you want to shutdown the OS/2 system?",
                       "Shutdown ...",
                       MB_YESNOCANCEL | MB_ICONQUESTION);
  switch (rc)
  {
    case IDCANCEL: return (FALSE);
    case IDYES:    break;
    case IDNO:
      dprintf(("no shutdown!\n"));
      return TRUE;
  }

  return O32_ExitWindowsEx(uFlags,dwReserved);
}


//******************************************************************************
//******************************************************************************
BOOL WIN32API MessageBeep( UINT uType)
{
    INT flStyle;

    dprintf(("USER32:  MessageBeep\n"));

    switch (uType)
    {
      case 0xFFFFFFFF:
        OSLibDosBeep(500,50);
        return TRUE;
      case MB_ICONASTERISK:
        flStyle = WAOS_NOTE;
        break;
      case MB_ICONEXCLAMATION:
        flStyle = WAOS_WARNING;
        break;
      case MB_ICONHAND:
      case MB_ICONQUESTION:
      case MB_OK:
        flStyle = WAOS_NOTE;
        break;
      default:
        flStyle = WAOS_ERROR; //CB: should be right
        break;
    }
    return OSLibWinAlarm(OSLIB_HWND_DESKTOP,flStyle);
}
//******************************************************************************
//2nd parameter not used according to SDK (yet?)
//******************************************************************************
VOID WIN32API SetLastErrorEx(DWORD dwErrCode, DWORD dwType)
{
  dprintf(("USER32: SetLastErrorEx %x %x", dwErrCode, dwType));
  SetLastError(dwErrCode);
}

/* Accessibility Functions */

int WIN32API GetSystemMetrics(int nIndex)
{
   int rc = 0;

   switch(nIndex) {
    case SM_CXSCREEN:
        rc = ScreenWidth;
        break;

    case SM_CYSCREEN:
        rc = ScreenHeight;
        break;

    case SM_CXVSCROLL:
        rc = OSLibWinQuerySysValue(SVOS_CXVSCROLL);
        break;

    case SM_CYHSCROLL:
        rc = OSLibWinQuerySysValue(SVOS_CYHSCROLL);
        break;

    case SM_CYCAPTION:
        if(fOS2Look) {
             rc = OSLibWinQuerySysValue(SVOS_CYTITLEBAR);
        }
        else rc = 19;
        break;

    case SM_CXBORDER:
    case SM_CYBORDER:
        rc = 1;
        break;

    case SM_CXDLGFRAME:
    case SM_CYDLGFRAME:
        rc = 3;
        break;

    case SM_CYMENU:
    case SM_CXMENUSIZE:
    case SM_CYMENUSIZE:
        if(fOS2Look) {
             rc = OSLibWinQuerySysValue(SVOS_CYMENU);
        }
        else rc = 19;
        break;

    case SM_CXSIZE:
    case SM_CYSIZE:
        rc = GetSystemMetrics(SM_CYCAPTION)-2;
        break;

    case SM_CXFRAME:
    case SM_CYFRAME:
        rc = 4;
        break;

    case SM_CXEDGE:
    case SM_CYEDGE:
        rc = 2;
        break;

    case SM_CXMINSPACING:
        rc = 160;
        break;

    case SM_CYMINSPACING:
        rc = 24;
        break;

    case SM_CXSMICON:
    case SM_CYSMICON:
        rc = 16;
        break;

    case SM_CYSMCAPTION:
        rc = 16;
        break;

    case SM_CXSMSIZE:
    case SM_CYSMSIZE:
        rc = 15;
        break;

//CB: todo: add missing metrics

    case SM_CXICONSPACING: //TODO: size of grid cell for large icons
        rc = OSLibWinQuerySysValue(SVOS_CXICON);
        //CB: return standard windows icon size?
        //rc = 32;
        break;
    case SM_CYICONSPACING:
        rc = OSLibWinQuerySysValue(SVOS_CYICON);
        //read SM_CXICONSPACING comment
        //rc = 32;
        break;
    case SM_PENWINDOWS:
        rc = FALSE;
        break;
    case SM_DBCSENABLED:
        rc = FALSE;
        break;
    case SM_CXICON:
    case SM_CYICON:
        rc = 32;  //CB: Win32: only 32x32, OS/2 32x32/40x40
                  //    we must implement 32x32 for all screen resolutions
        break;
    case SM_ARRANGE:
        rc = ARW_BOTTOMLEFT | ARW_LEFT;
        break;
    case SM_CXMINIMIZED:
        break;
    case SM_CYMINIMIZED:
        break;

    case SM_CXMINTRACK:
    case SM_CXMIN:
        rc = 112;
        break;

    case SM_CYMINTRACK:
    case SM_CYMIN:
        rc = 27;
        break;

    case SM_CXMAXTRACK: //max window size
    case SM_CXMAXIMIZED:    //max toplevel window size
        rc = OSLibWinQuerySysValue(SVOS_CXSCREEN);
        break;

    case SM_CYMAXTRACK:
    case SM_CYMAXIMIZED:
        rc = OSLibWinQuerySysValue(SVOS_CYSCREEN);
        break;

    case SM_NETWORK:
        rc = 0x01;  //TODO: default = yes
        break;
    case SM_CLEANBOOT:
        rc = 0;     //normal boot
        break;
    case SM_CXDRAG:     //nr of pixels before drag becomes a real one
        rc = 2;
        break;
    case SM_CYDRAG:
        rc = 2;
        break;
    case SM_SHOWSOUNDS: //show instead of play sound
        rc = FALSE;
        break;
    case SM_CXMENUCHECK:
        rc = 4;     //TODO
        break;
    case SM_CYMENUCHECK:
        rc = OSLibWinQuerySysValue(SVOS_CYMENU);
        break;
    case SM_SLOWMACHINE:
        rc = FALSE; //even a slow machine is fast with OS/2 :)
        break;
    case SM_MIDEASTENABLED:
        rc = FALSE;
        break;
    case SM_MOUSEWHEELPRESENT:
        rc = FALSE;
        break;
    case SM_XVIRTUALSCREEN:
        rc = 0;
        break;
    case SM_YVIRTUALSCREEN:
        rc = 0;
        break;

    case SM_CXVIRTUALSCREEN:
        rc = OSLibWinQuerySysValue(SVOS_CXSCREEN);
        break;
    case SM_CYVIRTUALSCREEN:
        rc = OSLibWinQuerySysValue(SVOS_CYSCREEN);
        break;
    case SM_CMONITORS:
        rc = 1;
        break;
    case SM_SAMEDISPLAYFORMAT:
        rc = TRUE;
        break;
    case SM_CMETRICS:
        rc = 81;
        //rc = O32_GetSystemMetrics(44);  //Open32 changed this one
        break;
    default:
        //better than nothing
        rc = O32_GetSystemMetrics(nIndex);
        break;
    }
    dprintf2(("USER32:  GetSystemMetrics %d returned %d\n", nIndex, rc));
    return(rc);
}
//******************************************************************************
/* Not support by Open32 (not included are the new win9x parameters):
      case SPI_GETFASTTASKSWITCH:
      case SPI_GETGRIDGRANULARITY:
      case SPI_GETICONTITLELOGFONT:
      case SPI_GETICONTITLEWRAP:
      case SPI_GETMENUDROPALIGNMENT:
      case SPI_ICONHORIZONTALSPACING:
      case SPI_ICONVERTICALSPACING:
      case SPI_LANGDRIVER:
      case SPI_SETFASTTASKSWITCH:
      case SPI_SETGRIDGRANULARITY:
      case SPI_SETICONTITLELOGFONT:
      case SPI_SETICONTITLEWRAP:
      case SPI_SETMENUDROPALIGNMENT:
      case SPI_GETSCREENSAVEACTIVE:
      case SPI_GETSCREENSAVETIMEOUT:
      case SPI_SETDESKPATTERN:
      case SPI_SETDESKWALLPAPER:
      case SPI_SETSCREENSAVEACTIVE:
      case SPI_SETSCREENSAVETIMEOUT:
*/
//******************************************************************************
BOOL WIN32API SystemParametersInfoA(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
 BOOL rc = TRUE;

  dprintf(("USER32:  SystemParametersInfoA uiAction: %d, uiParam: %d, pvParam: %d, fWinIni: %d\n",
           uiAction, uiParam, pvParam, fWinIni));

  switch(uiAction) {

    case SPI_GETBEEP:
        *(ULONG*)pvParam = OSLibWinQuerySysValue(SVOS_ALARM);
        break;

    case SPI_GETKEYBOARDDELAY:
        *(PULONG)pvParam = OSLibPrfQueryProfileInt(OSLIB_HINI_USER, "PM_ControlPanel",
                                                   "KeyRepeatDelay", 90);
        break;

    case SPI_GETKEYBOARDSPEED:
        *(PULONG)pvParam = OSLibPrfQueryProfileInt(OSLIB_HINI_USER, "PM_ControlPanel",
                                                   "KeyRepeatRate", 19);
        break;

#if 0
// TODO: Make OSLib a seperate DLL and use it everywhere?
    case SPI_GETMOUSE:
    {
        ULONG retCode;
        retCode = OSLibDosOpen(
        break;
    }
#endif

    case SPI_SETBEEP:
        // we don't do anything here. Win32 apps shouldn't change OS/2 settings
        dprintf(("USER32: SPI_SETBEEP is ignored!\n"));
        break;

    case SPI_SETBORDER:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETBORDER is ignored, implement!\n"));
        break;

    case SPI_SETDOUBLECLKHEIGHT:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETDOUBLECLICKHEIGHT is ignored, implement!\n"));
        break;

    case SPI_SETDOUBLECLKWIDTH:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETDOUBLECLICKWIDTH is ignored, implement!\n"));
        break;

    case SPI_SETDOUBLECLICKTIME:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETDOUBLECLICKTIME is ignored, implement!\n"));
        break;

    case SPI_SETKEYBOARDDELAY:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETKEYBOARDDELAY is ignored, implement!\n"));
        break;

    case SPI_SETKEYBOARDSPEED:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETKEYBOARDSPEED is ignored, implement!\n"));
        break;

    case SPI_SETMOUSE:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETMOUSE is ignored, implement!\n"));
        break;

    case SPI_SETMOUSEBUTTONSWAP:
        // TODO make this for Win32 apps only, Open32 changes OS/2 settings!
        dprintf(("USER32: SPI_SETMOUSEBUTTONSWAP is ignored, implement!\n"));
        break;

    case SPI_SCREENSAVERRUNNING:
        *(BOOL *)pvParam = FALSE;
        break;

    case SPI_GETDRAGFULLWINDOWS:
        *(BOOL *)pvParam = OSLibWinQuerySysValue(SVOS_DYNAMICDRAG);
        break;

    case SPI_GETNONCLIENTMETRICS:
    {
	LPNONCLIENTMETRICSA lpnm = (LPNONCLIENTMETRICSA)pvParam;
		
 	if (lpnm->cbSize == sizeof(NONCLIENTMETRICSA))
	{
            memset(lpnm, 0, sizeof(NONCLIENTMETRICSA));
            lpnm->cbSize = sizeof(NONCLIENTMETRICSA);

            SystemParametersInfoA(SPI_GETICONTITLELOGFONT, 0, (LPVOID)&(lpnm->lfCaptionFont),0);
            lpnm->lfCaptionFont.lfWeight = FW_BOLD;
            lpnm->iCaptionWidth    = 32; //TODO
            lpnm->iCaptionHeight   = 32; //TODO

            SystemParametersInfoA(SPI_GETICONTITLELOGFONT, 0, (LPVOID)&(lpnm->lfSmCaptionFont),0);
            lpnm->iSmCaptionWidth  = GetSystemMetrics(SM_CXSMSIZE);
            lpnm->iSmCaptionHeight = GetSystemMetrics(SM_CYSMSIZE);

            LPLOGFONTA lpLogFont = &(lpnm->lfMenuFont);
            if(fOS2Look) {
                char fontname[128];
                char *pszFontName;
                BOOL fFound = TRUE;

                if(OSLibPrfQueryProfileString(OSLIB_HINI_USER, "PM_SystemFonts", "Menus", "", fontname, sizeof(fontname)) == 1) {
                    if(OSLibPrfQueryProfileString(OSLIB_HINI_USER, "PM_SystemFonts", "DefaultFont", "", fontname, sizeof(fontname)) == 1) {
                        fFound = FALSE;
                    }
                }
                if(fFound) {
                    pszFontName = fontname;
                    while(*pszFontName != '.' && *pszFontName != 0) pszFontName++;
                    if(*pszFontName) {
                        *pszFontName++ = 0;

                        strncpy(lpLogFont->lfFaceName, pszFontName, sizeof(lpLogFont->lfFaceName));
                        lpLogFont->lfFaceName[sizeof(lpLogFont->lfFaceName)-1] = 0;
                        lpLogFont->lfWeight = FW_NORMAL;
                        lpLogFont->lfHeight = 16; //atoi(fontname);
                    }
                    else fFound = FALSE;
                }
                if(!fFound) {
                    GetProfileStringA("Desktop", "MenuFont", "WarpSans",
                                      lpLogFont->lfFaceName, LF_FACESIZE);
                    lpLogFont->lfWeight = FW_BOLD;
                    lpLogFont->lfHeight = -GetProfileIntA("Desktop","MenuFontSize", 16);
                }
                lpLogFont->lfWidth = 0;
                lpLogFont->lfEscapement = lpLogFont->lfOrientation = 0;
            }
            else {
                GetProfileStringA("Desktop", "MenuFont", "MS Sans Serif",
                                  lpLogFont->lfFaceName, LF_FACESIZE);
                lpLogFont->lfWeight = FW_NORMAL;
                lpLogFont->lfHeight = -GetProfileIntA("Desktop","MenuFontSize", 13);
                lpLogFont->lfWidth = 0;
                lpLogFont->lfEscapement = lpLogFont->lfOrientation = 0;
            }
            lpLogFont->lfItalic = FALSE;
            lpLogFont->lfStrikeOut = FALSE;
            lpLogFont->lfUnderline = FALSE;
            lpLogFont->lfCharSet = ANSI_CHARSET;
            lpLogFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
            lpLogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
            lpLogFont->lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;

            SystemParametersInfoA(SPI_GETICONTITLELOGFONT, 0,
     	 	  	      (LPVOID)&(lpnm->lfStatusFont),0);
            SystemParametersInfoA(SPI_GETICONTITLELOGFONT, 0,
     		  	      (LPVOID)&(lpnm->lfMessageFont),0);

            lpnm->iBorderWidth     = GetSystemMetrics(SM_CXBORDER);
            lpnm->iScrollWidth     = GetSystemMetrics(SM_CXHSCROLL);
            lpnm->iScrollHeight    = GetSystemMetrics(SM_CYHSCROLL);
            lpnm->iMenuHeight      = GetSystemMetrics(SM_CYMENU);
            lpnm->iMenuWidth       = lpnm->iMenuHeight; //TODO
          }
	else
 	{
	    dprintf(("SPI_GETNONCLIENTMETRICS: size mismatch !! (is %d; should be %d)\n", lpnm->cbSize, sizeof(NONCLIENTMETRICSA)));
	    /* FIXME: SetLastError? */
	    rc = FALSE;
	}
        break;
    }

    case SPI_GETICONMETRICS:			/*     45  WINVER >= 0x400 */
    {
	LPICONMETRICSA lpIcon = (LPICONMETRICSA)pvParam;
	if(lpIcon && lpIcon->cbSize == sizeof(*lpIcon))
	{
	    SystemParametersInfoA( SPI_ICONHORIZONTALSPACING, 0,
				   &lpIcon->iHorzSpacing, FALSE );
	    SystemParametersInfoA( SPI_ICONVERTICALSPACING, 0,
				   &lpIcon->iVertSpacing, FALSE );
	    SystemParametersInfoA( SPI_GETICONTITLEWRAP, 0,
				   &lpIcon->iTitleWrap, FALSE );
	    SystemParametersInfoA( SPI_GETICONTITLELOGFONT, 0,
				   &lpIcon->lfFont, FALSE );
	}
	else
	{
	    dprintf(("SPI_GETICONMETRICS: size mismatch !! (is %d; should be %d)\n", lpIcon->cbSize, sizeof(ICONMETRICSA)));
	    /* FIXME: SetLastError? */
	    rc = FALSE;
	}
	break;
    }

    case SPI_GETICONTITLELOGFONT:
    {
        LPLOGFONTA lpLogFont = (LPLOGFONTA)pvParam;

        /* from now on we always have an alias for MS Sans Serif */
        strcpy(lpLogFont->lfFaceName, "MS Sans Serif");
        lpLogFont->lfHeight = -GetProfileIntA("Desktop","IconTitleSize", /*8*/12); //CB: 8 is too small
        lpLogFont->lfWidth = 0;
        lpLogFont->lfEscapement = lpLogFont->lfOrientation = 0;
        lpLogFont->lfWeight = FW_NORMAL;
        lpLogFont->lfItalic = FALSE;
        lpLogFont->lfStrikeOut = FALSE;
        lpLogFont->lfUnderline = FALSE;
        lpLogFont->lfCharSet = ANSI_CHARSET;
        lpLogFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
        lpLogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
        lpLogFont->lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
        break;
    }
    case SPI_GETBORDER:
        *(INT *)pvParam = GetSystemMetrics( SM_CXFRAME );
        break;

    case SPI_GETWORKAREA:
        SetRect( (RECT *)pvParam, 0, 0,
                GetSystemMetrics( SM_CXSCREEN ),
                GetSystemMetrics( SM_CYSCREEN )
        );
        break;

    case SPI_GETWHEELSCROLLLINES: //TODO: Undocumented
        rc = 16;
        break;

    default:
        dprintf(("System parameter value is not supported!\n"));
        rc = FALSE;
        // AH: no longer call Open32
        //rc = O32_SystemParametersInfo(uiAction, uiParam, pvParam, fWinIni);
        break;
  }
  dprintf(("USER32:  SystemParametersInfoA %d, returned %d\n", uiAction, rc));
  return(rc);
}
//******************************************************************************
//TODO: Check for more options that have different structs for Unicode!!!!
//******************************************************************************
BOOL WIN32API SystemParametersInfoW(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
 BOOL rc = TRUE;
 NONCLIENTMETRICSW *clientMetricsW = (NONCLIENTMETRICSW *)pvParam;
 NONCLIENTMETRICSA  clientMetricsA = {0};
 PVOID  pvParamA;
 UINT   uiParamA;

    switch(uiAction) {
    case SPI_SETNONCLIENTMETRICS:
        clientMetricsA.cbSize = sizeof(NONCLIENTMETRICSA);
        clientMetricsA.iBorderWidth = clientMetricsW->iBorderWidth;
        clientMetricsA.iScrollWidth = clientMetricsW->iScrollWidth;
        clientMetricsA.iScrollHeight = clientMetricsW->iScrollHeight;
        clientMetricsA.iCaptionWidth = clientMetricsW->iCaptionWidth;
        clientMetricsA.iCaptionHeight = clientMetricsW->iCaptionHeight;
        ConvertFontWA(&clientMetricsW->lfCaptionFont, &clientMetricsA.lfCaptionFont);
        clientMetricsA.iSmCaptionWidth = clientMetricsW->iSmCaptionWidth;
        clientMetricsA.iSmCaptionHeight = clientMetricsW->iSmCaptionHeight;
        ConvertFontWA(&clientMetricsW->lfSmCaptionFont, &clientMetricsA.lfSmCaptionFont);
        clientMetricsA.iMenuWidth = clientMetricsW->iMenuWidth;
        clientMetricsA.iMenuHeight = clientMetricsW->iMenuHeight;
        ConvertFontWA(&clientMetricsW->lfMenuFont, &clientMetricsA.lfMenuFont);
        ConvertFontWA(&clientMetricsW->lfStatusFont, &clientMetricsA.lfStatusFont);
        ConvertFontWA(&clientMetricsW->lfMessageFont, &clientMetricsA.lfMessageFont);
        //no break
    case SPI_GETNONCLIENTMETRICS:
        uiParamA = sizeof(NONCLIENTMETRICSA);
        pvParamA = &clientMetricsA;
        break;

    case SPI_GETICONMETRICS:			/*     45  WINVER >= 0x400 */
    {
	LPICONMETRICSW lpIcon = (LPICONMETRICSW)pvParam;
	if(lpIcon && lpIcon->cbSize == sizeof(*lpIcon))
	{
	    SystemParametersInfoW( SPI_ICONHORIZONTALSPACING, 0,
				   &lpIcon->iHorzSpacing, FALSE );
	    SystemParametersInfoW( SPI_ICONVERTICALSPACING, 0,
				   &lpIcon->iVertSpacing, FALSE );
	    SystemParametersInfoW( SPI_GETICONTITLEWRAP, 0,
				   &lpIcon->iTitleWrap, FALSE );
	    SystemParametersInfoW( SPI_GETICONTITLELOGFONT, 0,
				   &lpIcon->lfFont, FALSE );
            return TRUE;
	}
	else
	{
	    dprintf(("SPI_GETICONMETRICS: size mismatch !! (is %d; should be %d)\n", lpIcon->cbSize, sizeof(ICONMETRICSA)));
	    /* FIXME: SetLastError? */
	    return FALSE;
	}
    }

    case SPI_GETICONTITLELOGFONT:
    {
        LPLOGFONTW lpLogFont = (LPLOGFONTW)pvParam;

        /* from now on we always have an alias for MS Sans Serif */
        lstrcpyW(lpLogFont->lfFaceName, (LPCWSTR)L"MS Sans Serif");
        lpLogFont->lfHeight = -GetProfileIntA("Desktop","IconTitleSize", /*8*/12); //CB: 8 is too small
        lpLogFont->lfWidth = 0;
        lpLogFont->lfEscapement = lpLogFont->lfOrientation = 0;
        lpLogFont->lfWeight = FW_NORMAL;
        lpLogFont->lfItalic = FALSE;
        lpLogFont->lfStrikeOut = FALSE;
        lpLogFont->lfUnderline = FALSE;
        lpLogFont->lfCharSet = ANSI_CHARSET;
        lpLogFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
        lpLogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
        lpLogFont->lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
        return TRUE;
    }
    default:
        pvParamA = pvParam;
        uiParamA = uiParam;
        break;
    }
    rc = SystemParametersInfoA(uiAction, uiParamA, pvParamA, fWinIni);

    switch(uiAction) {
    case SPI_GETNONCLIENTMETRICS:
        clientMetricsW->cbSize = sizeof(*clientMetricsW);
        clientMetricsW->iBorderWidth = clientMetricsA.iBorderWidth;
        clientMetricsW->iScrollWidth = clientMetricsA.iScrollWidth;
        clientMetricsW->iScrollHeight = clientMetricsA.iScrollHeight;
        clientMetricsW->iCaptionWidth = clientMetricsA.iCaptionWidth;
        clientMetricsW->iCaptionHeight = clientMetricsA.iCaptionHeight;
        ConvertFontAW(&clientMetricsA.lfCaptionFont, &clientMetricsW->lfCaptionFont);

        clientMetricsW->iSmCaptionWidth = clientMetricsA.iSmCaptionWidth;
        clientMetricsW->iSmCaptionHeight = clientMetricsA.iSmCaptionHeight;
        ConvertFontAW(&clientMetricsA.lfSmCaptionFont, &clientMetricsW->lfSmCaptionFont);

        clientMetricsW->iMenuWidth = clientMetricsA.iMenuWidth;
        clientMetricsW->iMenuHeight = clientMetricsA.iMenuHeight;
        ConvertFontAW(&clientMetricsA.lfMenuFont, &clientMetricsW->lfMenuFont);
        ConvertFontAW(&clientMetricsA.lfStatusFont, &clientMetricsW->lfStatusFont);
        ConvertFontAW(&clientMetricsA.lfMessageFont, &clientMetricsW->lfMessageFont);
        break;
    }
    dprintf(("USER32:  SystemParametersInfoW %d, returned %d\n", uiAction, rc));
    return(rc);
}

/* Help Functions */

BOOL WIN32API WinHelpA( HWND hwnd, LPCSTR lpszHelp, UINT uCommand, DWORD  dwData)
{
  static WORD WM_WINHELP = 0;
  HWND hDest;
  LPWINHELP lpwh;
  HINSTANCE winhelp;
  int size,dsize,nlen;
  BOOL ret;

  dprintf(("USER32: WinHelpA %x %s %d %x", hwnd, lpszHelp, uCommand, dwData));

  if(!WM_WINHELP)
  {
    WM_WINHELP=RegisterWindowMessageA("WM_WINHELP");
    if(!WM_WINHELP)
      return FALSE;
  }

  hDest = FindWindowA( "MS_WINHELP", NULL );
  if(!hDest)
  {
    if(uCommand == HELP_QUIT)
      return TRUE;
    else
      winhelp = WinExec ( "winhlp32.exe -x", SW_SHOWNORMAL );
    if ( winhelp <= 32 ) return FALSE;
    if ( ! ( hDest = FindWindowA ( "MS_WINHELP", NULL ) )) return FALSE;
  }

  switch(uCommand)
  {
    case HELP_CONTEXT:
    case HELP_SETCONTENTS:
    case HELP_CONTENTS:
    case HELP_CONTEXTPOPUP:
    case HELP_FORCEFILE:
    case HELP_HELPONHELP:
    case HELP_FINDER:
    case HELP_QUIT:
      dsize=0;
      break;

    case HELP_KEY:
    case HELP_PARTIALKEY:
    case HELP_COMMAND:
      dsize = strlen( (LPSTR)dwData )+1;
      break;

    case HELP_MULTIKEY:
      dsize = ((LPMULTIKEYHELP)dwData)->mkSize;
      break;

    case HELP_SETWINPOS:
      dsize = ((LPHELPWININFO)dwData)->wStructSize;
      break;

    default:
      //WARN("Unknown help command %d\n",wCommand);
      return FALSE;
  }
  if(lpszHelp)
    nlen = strlen(lpszHelp)+1;
  else
    nlen = 0;
  size = sizeof(WINHELP) + nlen + dsize;

  //allocate shared memory
  lpwh = (WINHELP*)_smalloc(size);
  lpwh->size = size;
  lpwh->command = uCommand;
  lpwh->data = dwData;
  if(nlen)
  {
    strcpy(((char*)lpwh) + sizeof(WINHELP),lpszHelp);
    lpwh->ofsFilename = sizeof(WINHELP);
  } else
      lpwh->ofsFilename = 0;
  if(dsize)
  {
    memcpy(((char*)lpwh)+sizeof(WINHELP)+nlen,(LPSTR)dwData,dsize);
    lpwh->ofsData = sizeof(WINHELP)+nlen;
  } else
      lpwh->ofsData = 0;

  ret = SendMessageA(hDest,WM_WINHELP,hwnd,(LPARAM)lpwh);
  free(lpwh);
  return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WinHelpW( HWND hwnd, LPCWSTR lpszHelp, UINT uCommand, DWORD  dwData)
{
  char *astring = UnicodeToAsciiString((LPWSTR)lpszHelp);
  BOOL  rc;

  dprintf(("USER32:  WinHelpW\n"));

  rc = WinHelpA(hwnd,astring,uCommand,dwData);
  FreeAsciiString(astring);

  return rc;
}


/* Window Functions */

/*****************************************************************************
 * Name      : BOOL WIN32API AnyPopup
 * Purpose   : The AnyPopup function indicates whether an owned, visible,
 *             top-level pop-up, or overlapped window exists on the screen. The
 *             function searches the entire Windows screen, not just the calling
 *             application's client area.
 * Parameters: VOID
 * Variables :
 * Result    : If a pop-up window exists, the return value is TRUE even if the
 *             pop-up window is completely covered by other windows. Otherwise,
 *             it is FALSE.
 * Remark    : AnyPopup is a Windows version 1.x function and is retained for
 *             compatibility purposes. It is generally not useful.
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API AnyPopup(VOID)
{
  dprintf(("USER32:AnyPopup() not implemented.\n"));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API PaintDesktop
 * Purpose   : The PaintDesktop function fills the clipping region in the
 *             specified device context with the desktop pattern or wallpaper.
 *             The function is provided primarily for shell desktops.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API PaintDesktop(HDC hdc)
{
  dprintf(("USER32:PaintDesktop (%08x) not implemented.\n",
         hdc));

  return (FALSE);
}

/* Filled Shape Functions */

  /* Last COLOR id */
#define COLOR_MAX   COLOR_GRADIENTINACTIVECAPTION

int WIN32API FillRect(HDC hDC, const RECT * lprc, HBRUSH hbr)
{
    if (hbr <= (HBRUSH) (COLOR_MAX + 1)) {
	hbr = GetSysColorBrush( (INT) hbr - 1 );
    }
    dprintf(("USER32:  FillRect %x (%d,%d)(%d,%d) brush %X", hDC, lprc->left, lprc->top, lprc->right, lprc->bottom, hbr));
    return O32_FillRect(hDC,lprc,hbr);
}
//******************************************************************************
//******************************************************************************
int WIN32API FrameRect( HDC hDC, const RECT * lprc, HBRUSH  hbr)
{
    dprintf(("USER32: FrameRect %x (%d,%d)(%d,%d) brush %x", hDC, lprc->top, lprc->left, lprc->bottom, lprc->right, hbr));
    return O32_FrameRect(hDC,lprc,hbr);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvertRect( HDC hDC, const RECT * lprc)
{
    if(lprc) {
         dprintf(("USER32: InvertRect %x (%d,%d)(%d,%d)", hDC, lprc->left, lprc->top, lprc->right, lprc->bottom));
    }
    else dprintf(("USER32: InvertRect %x NULL", hDC));
    return O32_InvertRect(hDC,lprc);
}

/* System Information Functions */

/* Window Station and Desktop Functions */

/*****************************************************************************
 * Name      : HDESK WIN32API GetThreadDesktop
 * Purpose   : The GetThreadDesktop function returns a handle to the desktop
 *             associated with a specified thread.
 * Parameters: DWORD dwThreadId thread identifier
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               desktop associated with the specified thread.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HDESK WIN32API GetThreadDesktop(DWORD dwThreadId)
{
  dprintf(("USER32:GetThreadDesktop (%u) not implemented.\n",
         dwThreadId));

  return (NULL);
}

/*****************************************************************************
 * Name      : BOOL WIN32API CloseDesktop
 * Purpose   : The CloseDesktop function closes an open handle of a desktop
 *             object. A desktop is a secure object contained within a window
 *             station object. A desktop has a logical display surface and
 *             contains windows, menus and hooks.
 * Parameters: HDESK hDesktop
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the functions fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API CloseDesktop(HDESK hDesktop)
{
  dprintf(("USER32:CloseDesktop(%08x) not implemented.\n",
         hDesktop));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API CloseWindowStation
 * Purpose   : The CloseWindowStation function closes an open window station handle.
 * Parameters: HWINSTA hWinSta
 * Variables :
 * Result    :
 * Remark    : If the function succeeds, the return value is TRUE.
 *             If the functions fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API CloseWindowStation(HWINSTA hWinSta)
{
  dprintf(("USER32:CloseWindowStation(%08x) not implemented.\n",
         hWinSta));

  return (FALSE);
}
/*****************************************************************************
 * Name      : HDESK WIN32API CreateDesktopA
 * Purpose   : The CreateDesktop function creates a new desktop on the window
 *             station associated with the calling process.
 * Parameters: LPCTSTR   lpszDesktop      name of the new desktop
 *             LPCTSTR   lpszDevice       name of display device to assign to the desktop
 *             LPDEVMODE pDevMode         reserved; must be NULL
 *             DWORD     dwFlags          flags to control interaction with other applications
 *             DWORD     dwDesiredAccess  specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the desktop
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               newly created desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HDESK WIN32API CreateDesktopA(LPCTSTR               lpszDesktop,
                              LPCTSTR               lpszDevice,
                              LPDEVMODEA            pDevMode,
                              DWORD                 dwFlags,
                              DWORD                 dwDesiredAccess,
                              LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateDesktopA(%s,%s,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktop,
         lpszDevice,
         pDevMode,
         dwFlags,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}
/*****************************************************************************
 * Name      : HDESK WIN32API CreateDesktopW
 * Purpose   : The CreateDesktop function creates a new desktop on the window
 *             station associated with the calling process.
 * Parameters: LPCTSTR   lpszDesktop      name of the new desktop
 *             LPCTSTR   lpszDevice       name of display device to assign to the desktop
 *             LPDEVMODE pDevMode         reserved; must be NULL
 *             DWORD     dwFlags          flags to control interaction with other applications
 *             DWORD     dwDesiredAccess  specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the desktop
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               newly created desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HDESK WIN32API CreateDesktopW(LPCTSTR               lpszDesktop,
                              LPCTSTR               lpszDevice,
                              LPDEVMODEW            pDevMode,
                              DWORD                 dwFlags,
                              DWORD                 dwDesiredAccess,
                              LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateDesktopW(%s,%s,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktop,
         lpszDevice,
         pDevMode,
         dwFlags,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}
/*****************************************************************************
 * Name      : HWINSTA WIN32API CreateWindowStationA
 * Purpose   : The CreateWindowStation function creates a window station object.
 *             It returns a handle that can be used to access the window station.
 *             A window station is a secure object that contains a set of global
 *             atoms, a clipboard, and a set of desktop objects.
 * Parameters: LPTSTR lpwinsta            name of the new window station
 *             DWORD dwReserved           reserved; must be NULL
 *             DWORD dwDesiredAccess      specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the window station
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               newly created window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HWINSTA WIN32API CreateWindowStationA(LPTSTR lpWinSta,
                                         DWORD  dwReserved,
                                         DWORD  dwDesiredAccess,
                                         LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateWindowStationA(%s,%08xh,%08xh,%08x) not implemented.\n",
         lpWinSta,
         dwReserved,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}
/*****************************************************************************
 * Name      : HWINSTA WIN32API CreateWindowStationW
 * Purpose   : The CreateWindowStation function creates a window station object.
 *             It returns a handle that can be used to access the window station.
 *             A window station is a secure object that contains a set of global
 *             atoms, a clipboard, and a set of desktop objects.
 * Parameters: LPTSTR lpwinsta            name of the new window station
 *             DWORD dwReserved           reserved; must be NULL
 *             DWORD dwDesiredAccess      specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the window station
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               newly created window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HWINSTA WIN32API CreateWindowStationW(LPWSTR lpWinSta,
                                         DWORD  dwReserved,
                                         DWORD  dwDesiredAccess,
                                         LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateWindowStationW(%s,%08xh,%08xh,%08x) not implemented.\n",
         lpWinSta,
         dwReserved,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}
/*****************************************************************************
 * Name      : BOOL WIN32API EnumDesktopWindows
 * Purpose   : The EnumDesktopWindows function enumerates all windows in a
 *             desktop by passing the handle of each window, in turn, to an
 *             application-defined callback function.
 * Parameters: HDESK       hDesktop handle of desktop to enumerate
 *             WNDENUMPROC lpfn     points to application's callback function
 *             LPARAM      lParam   32-bit value to pass to the callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API EnumDesktopWindows(HDESK       hDesktop,
                                    WNDENUMPROC lpfn,
                                    LPARAM      lParam)
{
  dprintf(("USER32:EnumDesktopWindows (%08xh,%08xh,%08x) not implemented.\n",
         hDesktop,
         lpfn,
         lParam));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API EnumDesktopsA
 * Purpose   : The EnumDesktops function enumerates all desktops in the window
 *             station assigned to the calling process. The function does so by
 *             passing the name of each desktop, in turn, to an application-
 *             defined callback function.
 * Parameters: HWINSTA         hwinsta    handle of window station to enumerate
 *             DESKTOPENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM          lParam     32-bit value to pass to the callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API EnumDesktopsA(HWINSTA          hWinSta,
                            DESKTOPENUMPROCA lpEnumFunc,
                            LPARAM           lParam)
{
  dprintf(("USER32:EnumDesktopsA (%08xh,%08xh,%08x) not implemented.\n",
         hWinSta,
         lpEnumFunc,
         lParam));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API EnumDesktopsW
 * Purpose   : The EnumDesktops function enumerates all desktops in the window
 *             station assigned to the calling process. The function does so by
 *             passing the name of each desktop, in turn, to an application-
 *             defined callback function.
 * Parameters: HWINSTA         hwinsta    handle of window station to enumerate
 *             DESKTOPENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM          lParam     32-bit value to pass to the callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API EnumDesktopsW(HWINSTA          hWinSta,
                            DESKTOPENUMPROCW lpEnumFunc,
                            LPARAM           lParam)
{
  dprintf(("USER32:EnumDesktopsW (%08xh,%08xh,%08x) not implemented.\n",
         hWinSta,
         lpEnumFunc,
         lParam));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API EnumWindowStationsA
 * Purpose   : The EnumWindowStations function enumerates all windowstations
 *             in the system by passing the name of each window station, in
 *             turn, to an application-defined callback function.
 * Parameters:
 * Variables : WINSTAENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM         lParam     32-bit value to pass to the callback function
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API EnumWindowStationsA(WINSTAENUMPROCA lpEnumFunc,
                                  LPARAM          lParam)
{
  dprintf(("USER32:EnumWindowStationsA (%08xh,%08x) not implemented.\n",
         lpEnumFunc,
         lParam));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API EnumWindowStationsW
 * Purpose   : The EnumWindowStations function enumerates all windowstations
 *             in the system by passing the name of each window station, in
 *             turn, to an application-defined callback function.
 * Parameters:
 * Variables : WINSTAENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM         lParam     32-bit value to pass to the callback function
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API EnumWindowStationsW(WINSTAENUMPROCW lpEnumFunc,
                                  LPARAM          lParam)
{
  dprintf(("USER32:EnumWindowStationsW (%08xh,%08x) not implemented.\n",
         lpEnumFunc,
         lParam));

  return (FALSE);
}
/*****************************************************************************
 * Name      : HWINSTA WIN32API GetProcessWindowStation
 * Purpose   : The GetProcessWindowStation function returns a handle of the
 *             window station associated with the calling process.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               window station associated with the calling process.
 *             If the function fails, the return value is NULL. This can occur
 *               if the calling process is not an application written for Windows
 *               NT. To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HWINSTA WIN32API GetProcessWindowStation(VOID)
{
  dprintf(("USER32:GetProcessWindowStation () not implemented.\n"));

  return (NULL);
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetUserObjectInformationA
 * Purpose   : The GetUserObjectInformation function returns information about
 *               a window station or desktop object.
 * Parameters: HANDLE  hObj            handle of object to get information for
 *             int     nIndex          type of information to get
 *             PVOID   pvInfo          points to buffer that receives the information
 *             DWORD   nLength         size, in bytes, of pvInfo buffer
 *             LPDWORD lpnLengthNeeded receives required size, in bytes, of pvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API GetUserObjectInformationA(HANDLE  hObj,
                                           int     nIndex,
                                           PVOID   pvInfo,
                                           DWORD   nLength,
                                           LPDWORD lpnLengthNeeded)
{
  dprintf(("USER32:GetUserObjectInformationA (%08xh,%08xh,%08xh,%u,%08x) not implemented.\n",
         hObj,
         nIndex,
         pvInfo,
         nLength,
         lpnLengthNeeded));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetUserObjectInformationW
 * Purpose   : The GetUserObjectInformation function returns information about
 *               a window station or desktop object.
 * Parameters: HANDLE  hObj            handle of object to get information for
 *             int     nIndex          type of information to get
 *             PVOID   pvInfo          points to buffer that receives the information
 *             DWORD   nLength         size, in bytes, of pvInfo buffer
 *             LPDWORD lpnLengthNeeded receives required size, in bytes, of pvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API GetUserObjectInformationW(HANDLE  hObj,
                                           int     nIndex,
                                           PVOID   pvInfo,
                                           DWORD   nLength,
                                           LPDWORD lpnLengthNeeded)
{
  dprintf(("USER32:GetUserObjectInformationW (%08xh,%08xh,%08xh,%u,%08x) not implemented.\n",
         hObj,
         nIndex,
         pvInfo,
         nLength,
         lpnLengthNeeded));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetUserObjectSecurity
 * Purpose   : The GetUserObjectSecurity function retrieves security information
 *             for the specified user object.
 * Parameters: HANDLE                hObj            handle of user object
 *             SECURITY_INFORMATION * pSIRequested    address of requested security information
 *             LPSECURITY_DESCRIPTOR  pSID            address of security descriptor
 *             DWORD                 nLength         size of buffer for security descriptor
 *             LPDWORD               lpnLengthNeeded address of required size of buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API GetUserObjectSecurity(HANDLE                hObj,
                                       PSECURITY_INFORMATION pSIRequested,
                                       PSECURITY_DESCRIPTOR  pSID,
                                       DWORD                 nLength,
                                       LPDWORD               lpnLengthNeeded)
{
  dprintf(("USER32:GetUserObjectSecurity (%08xh,%08xh,%08xh,%u,%08x) not implemented.\n",
         hObj,
         pSIRequested,
         pSID,
         nLength,
         lpnLengthNeeded));

  return (FALSE);
}
/*****************************************************************************
 * Name      : HDESK WIN32API OpenDesktopA
 * Purpose   : The OpenDesktop function returns a handle to an existing desktop.
 *             A desktop is a secure object contained within a window station
 *             object. A desktop has a logical display surface and contains
 *             windows, menus and hooks.
 * Parameters: LPCTSTR lpszDesktopName name of the desktop to open
 *             DWORD dwFlags           flags to control interaction with other applications
 *             BOOL fInherit           specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess   specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               opened desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HDESK WIN32API OpenDesktopA(LPCTSTR lpszDesktopName,
                               DWORD   dwFlags,
                               BOOL    fInherit,
                               DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenDesktopA (%s,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktopName,
         dwFlags,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}
/*****************************************************************************
 * Name      : HDESK WIN32API OpenDesktopW
 * Purpose   : The OpenDesktop function returns a handle to an existing desktop.
 *             A desktop is a secure object contained within a window station
 *             object. A desktop has a logical display surface and contains
 *             windows, menus and hooks.
 * Parameters: LPCTSTR lpszDesktopName name of the desktop to open
 *             DWORD dwFlags           flags to control interaction with other applications
 *             BOOL fInherit           specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess   specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               opened desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HDESK WIN32API OpenDesktopW(LPCTSTR lpszDesktopName,
                               DWORD   dwFlags,
                               BOOL    fInherit,
                               DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenDesktopW (%s,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktopName,
         dwFlags,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}
/*****************************************************************************
 * Name      : HDESK WIN32API OpenInputDesktop
 * Purpose   : The OpenInputDesktop function returns a handle to the desktop
 *             that receives user input. The input desktop is a desktop on the
 *             window station associated with the logged-on user.
 * Parameters: DWORD dwFlags         flags to control interaction with other applications
 *             BOOL  fInherit        specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               desktop that receives user input.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HDESK WIN32API OpenInputDesktop(DWORD dwFlags,
                                   BOOL  fInherit,
                                   DWORD dwDesiredAccess)
{
  dprintf(("USER32:OpenInputDesktop (%08xh,%08xh,%08x) not implemented.\n",
         dwFlags,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}
/*****************************************************************************
 * Name      : HWINSTA WIN32API OpenWindowStationA
 * Purpose   : The OpenWindowStation function returns a handle to an existing
 *               window station.
 * Parameters: LPCTSTR lpszWinStaName name of the window station to open
 *             BOOL fInherit          specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess  specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               specified window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HWINSTA WIN32API OpenWindowStationA(LPCTSTR lpszWinStaName,
                                       BOOL    fInherit,
                                       DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenWindowStatieonA (%s,%08xh,%08x) not implemented.\n",
         lpszWinStaName,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}
/*****************************************************************************
 * Name      : HWINSTA WIN32API OpenWindowStationW
 * Purpose   : The OpenWindowStation function returns a handle to an existing
 *               window station.
 * Parameters: LPCTSTR lpszWinStaName name of the window station to open
 *             BOOL fInherit          specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess  specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               specified window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.


 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HWINSTA WIN32API OpenWindowStationW(LPCTSTR lpszWinStaName,
                                       BOOL    fInherit,
                                       DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenWindowStatieonW (%s,%08xh,%08x) not implemented.\n",
         lpszWinStaName,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetProcessWindowStation
 * Purpose   : The SetProcessWindowStation function assigns a window station
 *             to the calling process. This enables the process to access
 *             objects in the window station such as desktops, the clipboard,
 *             and global atoms. All subsequent operations on the window station
 *             use the access rights granted to hWinSta.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API SetProcessWindowStation(HWINSTA hWinSta)
{
  dprintf(("USER32:SetProcessWindowStation (%08x) not implemented.\n",
         hWinSta));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetThreadDesktop
 * Purpose   : The SetThreadDesktop function assigns a desktop to the calling
 *             thread. All subsequent operations on the desktop use the access
 *             rights granted to hDesk.
 * Parameters: HDESK hDesk handle of the desktop to assign to this thread
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API SetThreadDesktop(HDESK hDesktop)
{
  dprintf(("USER32:SetThreadDesktop (%08x) not implemented.\n",
         hDesktop));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetUserObjectInformationA
 * Purpose   : The SetUserObjectInformation function sets information about a
 *             window station or desktop object.
 * Parameters: HANDLE hObject handle of the object for which to set information
 *             int    nIndex  type of information to set
 *             PVOID  lpvInfo points to a buffer that contains the information
 *             DWORD  cbInfo  size, in bytes, of lpvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API SetUserObjectInformationA(HANDLE hObject,
                                           int    nIndex,
                                           PVOID  lpvInfo,
                                           DWORD  cbInfo)
{
  dprintf(("USER32:SetUserObjectInformationA (%08xh,%u,%08xh,%08x) not implemented.\n",
           hObject,
           nIndex,
           lpvInfo,
           cbInfo));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetUserObjectInformationW
 * Purpose   : The SetUserObjectInformation function sets information about a
 *             window station or desktop object.
 * Parameters: HANDLE hObject handle of the object for which to set information
 *             int    nIndex  type of information to set
 *             PVOID  lpvInfo points to a buffer that contains the information
 *             DWORD  cbInfo  size, in bytes, of lpvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API SetUserObjectInformationW(HANDLE hObject,
                                           int    nIndex,
                                           PVOID  lpvInfo,
                                           DWORD  cbInfo)
{
  dprintf(("USER32:SetUserObjectInformationW (%08xh,%u,%08xh,%08x) not implemented.\n",
           hObject,
           nIndex,
           lpvInfo,
           cbInfo));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetUserObjectSecurity
 * Purpose   : The SetUserObjectSecurity function sets the security of a user
 *             object. This can be, for example, a window or a DDE conversation
 * Parameters: HANDLE  hObject           handle of user object
 *             SECURITY_INFORMATION * psi address of security information
 *             LPSECURITY_DESCRIPTOR  psd address of security descriptor
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API SetUserObjectSecurity(HANDLE hObject,
                                       PSECURITY_INFORMATION psi,
                                       PSECURITY_DESCRIPTOR  psd)
{
  dprintf(("USER32:SetUserObjectSecuroty (%08xh,%08xh,%08x) not implemented.\n",
           hObject,
           psi,
           psd));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SwitchDesktop
 * Purpose   : The SwitchDesktop function makes a desktop visible and activates
 *             it. This enables the desktop to receive input from the user. The
 *             calling process must have DESKTOP_SWITCHDESKTOP access to the
 *             desktop for the SwitchDesktop function to succeed.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API SwitchDesktop(HDESK hDesktop)
{
  dprintf(("USER32:SwitchDesktop (%08x) not implemented.\n",
         hDesktop));

  return (FALSE);
}

/* Debugging Functions */

/*****************************************************************************
 * Name      : VOID WIN32API SetDebugErrorLevel
 * Purpose   : The SetDebugErrorLevel function sets the minimum error level at
 *             which Windows will generate debugging events and pass them to a debugger.
 * Parameters: DWORD dwLevel debugging error level
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
VOID WIN32API SetDebugErrorLevel(DWORD dwLevel)
{
  dprintf(("USER32:SetDebugErrorLevel (%08x) not implemented.\n",
         dwLevel));
}

/* Drag'n'drop */

/*****************************************************************************
 * Name      : BOOL WIN32API DragObject
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
DWORD WIN32API DragObject(HWND x1,HWND x2,UINT x3,DWORD x4,HCURSOR x5)
{
  dprintf(("USER32: DragObject(%08x,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return (FALSE); /* default */
}

/* Unknown */

/*****************************************************************************
 * Name      : BOOL WIN32API SetShellWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API SetShellWindow(DWORD x1)
{
  dprintf(("USER32: SetShellWindow(%08x) not implemented.\n",
           x1));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API PlaySoundEvent
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API PlaySoundEvent(DWORD x1)
{
  dprintf(("USER32: PlaySoundEvent(%08x) not implemented.\n",
           x1));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetSysColorsTemp
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API SetSysColorsTemp(void)
{
  dprintf(("USER32: SetSysColorsTemp() not implemented.\n"));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API RegisterNetworkCapabilities
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API RegisterNetworkCapabilities(DWORD x1,
                                             DWORD x2)
{
  dprintf(("USER32: RegisterNetworkCapabilities(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API EndTask
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API EndTask(DWORD x1,
                         DWORD x2,
                         DWORD x3)
{
  dprintf(("USER32: EndTask(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetNextQueueWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API GetNextQueueWindow(DWORD x1,
                                    DWORD x2)
{
  dprintf(("USER32: GetNextQueueWindow(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API YieldTask
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API YieldTask(void)
{
  dprintf(("USER32: YieldTask() not implemented.\n"));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API WinOldAppHackoMatic
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API WinOldAppHackoMatic(DWORD x1)
{
  dprintf(("USER32: WinOldAppHackoMatic(%08x) not implemented.\n",
           x1));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API RegisterSystemThread
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API RegisterSystemThread(DWORD x1,
                                      DWORD x2)
{
  dprintf(("USER32: RegisterSystemThread(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API IsHungThread
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API IsHungThread(DWORD x1)
{
  dprintf(("USER32: IsHungThread(%08xh) not implemented.\n",
           x1));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API UserSignalProc
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API UserSignalProc(DWORD x1,
                                DWORD x2,
                                DWORD x3,
                                DWORD x4)
{
  dprintf(("USER32: SysErrorBox(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return (FALSE); /* default */
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetShellWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
HWND WIN32API GetShellWindow(void)
{
  dprintf(("USER32: GetShellWindow() not implemented.\n"));

  return (0); /* default */
}
/***********************************************************************
 *           RegisterTasklist32                [USER32.436]
 */
DWORD WIN32API RegisterTasklist (DWORD x)
{
    dprintf(("USER32: RegisterTasklist(%08xh) not implemented.\n",
             x));

    return TRUE;
}
/***********************************************************************
 *           SetLogonNotifyWindow   (USER32.486)
 */
DWORD WIN32API SetLogonNotifyWindow(HWINSTA hwinsta,HWND hwnd)
{
  dprintf(("USER32: SetLogonNotifyWindow - empty stub!"));

  return 1;
}


DWORD WIN32API NotifyWinEvent(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4)
{
  dprintf(("USER32: NotifyWinEvent %x %x %x %x - empty stub!!", arg1, arg2, arg3, arg4));

  return 0;
}

DWORD WIN32API UnhookWinEvent(DWORD arg1)
{
  dprintf(("USER32: UnhookWinEvent %x - empty stub!!", arg1));

  return 0;
}

DWORD WIN32API SetWinEventHook(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7)
{
  dprintf(("USER32: SetWinEventHook %x %x %x %x %x %x %x - empty stub!!", arg1, arg2, arg3, arg4, arg5, arg6, arg7));

  return 0;
}

DWORD WIN32API GetGUIThreadInfo(DWORD arg1, DWORD arg2)
{
  dprintf(("USER32: GetGUIThreadInfo %x %x - empty stub!!", arg1, arg2));

  return 0;
}

