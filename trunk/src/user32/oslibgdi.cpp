/* $Id: oslibgdi.cpp,v 1.9 2000-01-18 20:10:39 sandervl Exp $ */
/*
 * Window GDI wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#include <os2.h>
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <oslibgdi.h>
#include <oslibwin.h>
#include "win32wbase.h"

/*
First letter is lower case to avoid conflicts with Win32 API names
All transformations are for screen or client windows, for frame windows use OS/2 API's

Single y mapping:
 mapScreenY()
 mapY()
 mapOS2ToWin32Y()
 mapWin32ToOS2Y()
 mapWin32Y()

Single point mapping:
 mapScreenPoint()
 mapPoint()
 mapOS2ToWin32Point()
 mapWin32ToOS2Point()
 mapWin32Point()

Single rect mapping:
 mapOS2ToWin32ScreenRect()
 mapWin32ToOS2ScreenRect()
 mapOS2ToWin32Rect()
 mapWin32ToOS2Rect()
 mapWin32Rect()

Rect transformation:
 copyOS2ToWin32Rect()
 copyWin32ToOS2Rect()

Child origin:
 mapOS2ToWin32ChildOrigin()
*/

//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
INT mapScreenY(INT screenPosY)
{
  return ScreenHeight-1-screenPosY;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
INT mapScreenY(INT screenH,INT screenPosY)
{
  return screenH-1-screenPosY;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
INT mapY(HWND os2Client,INT clientPosY)
{
  RECTL rect;

  if (os2Client == OSLIB_HWND_DESKTOP) os2Client = HWND_DESKTOP; //client shouldn't be desktop
  if (!WinQueryWindowRect(os2Client,&rect)) return 0;

  return rect.yTop-1-clientPosY;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
INT mapY(Win32BaseWindow *win32wnd,INT clientPosY)
{
  if (!win32wnd) return 0;

  return win32wnd->getClientHeight()-1-clientPosY;
}
//******************************************************************************
//******************************************************************************
INT mapOS2ToWin32Y(HWND os2From,HWND os2To,INT fromPosY)
{
  POINTL pt;
  RECTL rect;

  if (os2From == OSLIB_HWND_DESKTOP) os2From = HWND_DESKTOP;
  if (os2To == OSLIB_HWND_DESKTOP) os2To = HWND_DESKTOP;
  if (os2From != os2To)
  {
    pt.x = 0;
    pt.y = fromPosY;
    if (!WinMapWindowPoints(os2From,os2To,&pt,1)) return 0;
  } else pt.y = fromPosY;
  if (!WinQueryWindowRect(os2To,&rect)) return 0;

  return rect.yTop-1-pt.y;
}
//******************************************************************************
//******************************************************************************
INT mapOS2ToWin32Y(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,INT fromPosY)
{
  POINTL pt;

  if (!wndFrom || !wndTo) return 0;
  if (wndFrom != wndTo)
  {
    pt.x = 0;
    pt.y = fromPosY;
    if (!WinMapWindowPoints(wndFrom->getOS2WindowHandle(),wndTo->getOS2WindowHandle(),&pt,1)) return 0;
  } else pt.y = fromPosY;

  return wndTo->getClientHeight()-1-pt.y;
}
//******************************************************************************
//******************************************************************************
INT mapWin32Y(HWND os2From,HWND os2To,INT fromPosY)
{
  POINTL pt;
  RECTL rect;

  if (os2From == OSLIB_HWND_DESKTOP) os2From = HWND_DESKTOP;
  if (os2To == OSLIB_HWND_DESKTOP) os2To = HWND_DESKTOP;
  if (os2From == os2To) return fromPosY;
  if (!WinQueryWindowRect(os2From,&rect)) return 0;
  pt.y = rect.yTop-1-fromPosY;
  pt.x = 0;
  if (!WinMapWindowPoints(os2From,os2To,&pt,1)) return 0;
  if (!WinQueryWindowRect(os2To,&rect)) return 0;

  return rect.yTop-1-pt.y;
}
//******************************************************************************
//******************************************************************************
INT mapWin32Y(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,INT fromPosY)
{
  POINTL pt;

  if (!wndFrom || !wndTo) return 0;
  if (wndFrom == wndTo) return fromPosY;
  pt.y = wndFrom->getClientHeight()-1-fromPosY;
  pt.x = 0;
  if (!WinMapWindowPoints(wndFrom->getOS2WindowHandle(),wndTo->getOS2WindowHandle(),&pt,1)) return 0;

  return wndTo->getClientHeight()-1-pt.y;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
BOOL mapScreenPoint(OSLIBPOINT *screenPt)
{
  if(!screenPt) return FALSE;
  screenPt->y = ScreenHeight-1-screenPt->y;

  return TRUE;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
BOOL mapScreenPoint(INT screenH,OSLIBPOINT *screenPt)
{
  if (!screenPt) return FALSE;
  screenPt->y = screenH-1-screenPt->y;

  return TRUE;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
BOOL mapPoint(HWND os2Client,OSLIBPOINT *clientPt)
{
  RECTL rect;

  if (!clientPt) return FALSE;
  if (os2Client == OSLIB_HWND_DESKTOP) os2Client = HWND_DESKTOP; //client shouldn't be desktop
  if (!WinQueryWindowRect(os2Client,&rect)) return 0;
  clientPt->y = rect.yTop-1-clientPt->y;

  return TRUE;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
BOOL mapPoint(Win32BaseWindow *win32wnd,OSLIBPOINT *clientPt)
{
  if (!win32wnd || !clientPt) return FALSE;
  clientPt->y = win32wnd->getClientHeight()-1-clientPt->y;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapOS2ToWin32Point(HWND os2From,HWND os2To,OSLIBPOINT *fromPt)
{
  RECTL rect;

  if (os2From == OSLIB_HWND_DESKTOP) os2From = HWND_DESKTOP;
  if (os2To == OSLIB_HWND_DESKTOP) os2To = HWND_DESKTOP;
  if (os2From != os2To)
  {
    if (!WinMapWindowPoints(os2From,os2To,(PPOINTL)fromPt,1)) return FALSE;
  }
  if (!WinQueryWindowRect(os2To,&rect)) return FALSE;
  fromPt->y = rect.yTop-1-fromPt->y;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapOS2ToWin32Point(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,OSLIBPOINT *fromPt)
{
  if (!wndFrom || !wndTo) return 0;
  if (wndFrom != wndTo)
  {
    if (!WinMapWindowPoints(wndFrom->getOS2WindowHandle(),wndTo->getOS2WindowHandle(),(PPOINTL)fromPt,1)) return FALSE;
  }
  fromPt->y = wndTo->getClientHeight()-1-fromPt->y;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32Point(HWND os2From,HWND os2To,OSLIBPOINT *fromPt)
{
  RECTL rect;

  if (os2From == OSLIB_HWND_DESKTOP) os2From = HWND_DESKTOP;
  if (os2To == OSLIB_HWND_DESKTOP) os2To = HWND_DESKTOP;
  if (os2From == os2To) return TRUE;
  if (!WinQueryWindowRect(os2From,&rect)) return 0;
  fromPt->y = rect.yTop-1-fromPt->y;
  if (!WinMapWindowPoints(os2From,os2To,(PPOINTL)fromPt,1)) return 0;
  if (!WinQueryWindowRect(os2To,&rect)) return 0;
  fromPt->y = rect.yTop-1-fromPt->y;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32Point(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,OSLIBPOINT *fromPt)
{
  if (!wndFrom || !wndTo) return FALSE;
  if (wndFrom == wndTo) return TRUE;
  fromPt->y = wndFrom->getClientHeight()-1-fromPt->y;
  if (!WinMapWindowPoints(wndFrom->getOS2WindowHandle(),wndTo->getOS2WindowHandle(),(PPOINTL)fromPt,1)) return 0;
  fromPt->y = wndTo->getClientHeight()-1-fromPt->y;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapOS2ToWin32ScreenRect(PRECTLOS2 rectOS2,PRECT rectWin32)
{
  if (!rectOS2 || !rectWin32) return FALSE;
  rectWin32->bottom = ScreenHeight-rectOS2->yBottom;
  rectWin32->top    = ScreenHeight-rectOS2->yTop;
  rectWin32->left   = rectOS2->xLeft;
  rectWin32->right  = rectOS2->xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32ToOS2ScreenRect(PRECT rectWin32,PRECTLOS2 rectOS2)
{
  if (!rectOS2 || !rectWin32) return FALSE;
  rectOS2->yBottom = ScreenHeight-rectWin32->bottom;
  rectOS2->yTop    = ScreenHeight-rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapOS2ToWin32Rect(HWND os2Client,PRECTLOS2 rectOS2,PRECT rectWin32)
{
  RECTL rect;

  if (!rectOS2 || !rectWin32) return FALSE;
  if (os2Client == OSLIB_HWND_DESKTOP) os2Client = HWND_DESKTOP; //shouldn't be the case
  if (!WinQueryWindowRect(os2Client,&rect)) return FALSE;
  rectWin32->bottom = rect.yTop-rectOS2->yBottom;
  rectWin32->top    = rect.yTop-rectOS2->yTop;
  rectWin32->left   = rectOS2->xLeft;
  rectWin32->right  = rectOS2->xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapOS2ToWin32Rect(Win32BaseWindow *win32wnd,PRECTLOS2 rectOS2,PRECT rectWin32)
{
  INT windowH;

  if (!win32wnd || !rectOS2 || !rectWin32) return FALSE;
  windowH = win32wnd->getClientHeight();
  rectWin32->bottom = windowH-rectOS2->yBottom;
  rectWin32->top    = windowH-rectOS2->yTop;
  rectWin32->left   = rectOS2->xLeft;
  rectWin32->right  = rectOS2->xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapOS2ToWin32Rect(HWND os2From,HWND os2To,PRECTLOS2 rectOS2,PRECT rectWin32)
{
  RECTL rect,temp;

  if (!rectOS2 || !rectWin32) return FALSE;
  if (os2From == OSLIB_HWND_DESKTOP) os2From = HWND_DESKTOP;
  if (os2To == OSLIB_HWND_DESKTOP) os2To = HWND_DESKTOP;
  temp = *((PRECTL)rectOS2);
  if (os2From != os2To)
  {
    if (!WinMapWindowPoints(os2From,os2To,(PPOINTL)&temp,2)) return FALSE;
  }
  if (!WinQueryWindowRect(os2To,&rect)) return FALSE;
  rectWin32->bottom = rect.yTop-temp.yBottom;
  rectWin32->top    = rect.yTop-temp.yTop;
  rectWin32->left   = temp.xLeft;
  rectWin32->right  = temp.xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapOS2ToWin32Rect(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,PRECTLOS2 rectOS2,PRECT rectWin32)
{
  RECTL temp;
  INT windowH;

  if (!wndFrom || !wndTo || !rectOS2 || !rectWin32) return FALSE;
  temp = *((PRECTL)rectOS2);
  if (wndFrom != wndTo)
  {
    if (!WinMapWindowPoints(wndFrom->getOS2WindowHandle(),wndTo->getOS2WindowHandle(),(PPOINTL)&temp,2)) return FALSE;
  }
  windowH = wndTo->getClientHeight();
  rectWin32->bottom = windowH-temp.yBottom;
  rectWin32->top    = windowH-temp.yTop;
  rectWin32->left   = temp.xLeft;
  rectWin32->right  = temp.xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32ToOS2Rect(HWND os2Client,PRECT rectWin32,PRECTLOS2 rectOS2)
{
  RECTL rect;

  if (!rectOS2 || !rectWin32) return FALSE;
  if (os2Client == OSLIB_HWND_DESKTOP) os2Client = HWND_DESKTOP; //shouldn't be the case
  if (!WinQueryWindowRect(os2Client,&rect)) return FALSE;
  rectOS2->yBottom = rect.yTop-rectWin32->bottom;
  rectOS2->yTop    = rect.yTop-rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32ToOS2Rect(Win32BaseWindow *win32wnd,PRECT rectWin32,PRECTLOS2 rectOS2)
{
  INT windowH;

  if (!win32wnd || !rectOS2 || !rectWin32) return FALSE;
  windowH = win32wnd->getClientHeight();
  rectOS2->yBottom = windowH-rectWin32->bottom;
  rectOS2->yTop    = windowH-rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32ToOS2Rect(HWND os2From,HWND os2To,PRECT rectWin32,PRECTLOS2 rectOS2)
{
  RECTL rect;

  if (!rectOS2 || !rectWin32) return FALSE;
  if (os2From == OSLIB_HWND_DESKTOP) os2From = HWND_DESKTOP;
  if (os2To == OSLIB_HWND_DESKTOP) os2To = HWND_DESKTOP;
  if (!WinQueryWindowRect(os2From,&rect)) return FALSE;
  rectOS2->yBottom = rect.yTop-rectWin32->bottom;
  rectOS2->yTop    = rect.yTop-rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;
  if (os2From != os2To)
  {
    if (!WinMapWindowPoints(os2From,os2To,(PPOINTL)rectOS2,2)) return FALSE;
  }

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32ToOS2Rect(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,PRECT rectWin32,PRECTLOS2 rectOS2)
{
  INT windowH;

  if (!wndFrom || !wndTo || !rectOS2 || !rectWin32) return FALSE;
  windowH = wndFrom->getClientHeight();
  rectOS2->yBottom = windowH-rectWin32->bottom;
  rectOS2->yTop    = windowH-rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;
  if (wndFrom != wndTo)
  {
    if (!WinMapWindowPoints(wndFrom->getOS2WindowHandle(),wndTo->getOS2WindowHandle(),(PPOINTL)rectOS2,2)) return FALSE;
  }

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32Rect(HWND os2From,HWND os2To,PRECT rectWin32)
{
  RECTL rect;

  mapWin32ToOS2Rect(os2From,rectWin32,(PRECTLOS2)&rect);
  WinMapWindowPoints((os2From == OSLIB_HWND_DESKTOP) ? HWND_DESKTOP:os2From,(os2To == OSLIB_HWND_DESKTOP) ? HWND_DESKTOP:os2To,(PPOINTL)&rect,2);
  mapOS2ToWin32Rect(os2To,(PRECTLOS2)&rect,rectWin32);

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL copyOS2ToWin32Rect(PRECTLOS2 rectOS2,PRECT rectWin32)
{
  rectWin32->bottom = rectOS2->yBottom;
  rectWin32->top    = rectOS2->yTop;
  rectWin32->left   = rectOS2->xLeft;
  rectWin32->right  = rectOS2->xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL copyWin32ToOS2WindowRect(PRECT rectWin32,PRECTLOS2 rectOS2)
{
  rectOS2->yBottom = rectWin32->bottom;
  rectOS2->yTop    = rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
INT mapOS2ToWin32ChildOrigin(INT parentH,INT parentPosY,INT childH)
{
  return parentH-parentPosY-childH;//Does: parentH-1-parentPosY-(childH-1)
}
//******************************************************************************
//******************************************************************************
HDC OSLibWinBeginPaint(HWND hwnd, RECT *rectWin32)
{
 RECTL rectl;

    if(WinQueryUpdateRect(hwnd, &rectl) == FALSE)
    {
        dprintf(("BeginPaint, NO update rectl"));
        return 0;
    }
    mapOS2ToWin32Rect(hwnd,(RECTLOS2 *)&rectl, rectWin32);
    return WinBeginPaint(hwnd, NULLHANDLE, &rectl);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEndPaint(HDC hdc)
{
  return WinEndPaint((HPS)hdc);
}
//******************************************************************************
//******************************************************************************
HDC OSLibWinGetPS(HWND hwnd)
{
  if(hwnd == OSLIB_HWND_DESKTOP)
        hwnd = HWND_DESKTOP;

  return (HDC)WinGetPS(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinReleasePS(HDC hdc)
{
  return WinReleasePS((HPS)hdc);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinInvalidateRect(HWND hwnd, PRECT pRect, BOOL fIncludeChildren)
{
 RECTLOS2 rectl;

    if(pRect) {
        mapWin32ToOS2Rect(hwnd,pRect, &rectl);
        return WinInvalidateRect(hwnd, (PRECTL)&rectl, fIncludeChildren);
    }
    return WinInvalidateRect(hwnd, NULL, fIncludeChildren);
}
//******************************************************************************
//Returns rectangle in Win32 window coordinates
//******************************************************************************
BOOL OSLibWinQueryUpdateRect(HWND hwnd, PRECT pRect)
{
 BOOL rc;
 RECTLOS2 rectl;

  rc = WinQueryUpdateRect(hwnd, (PRECTL)&rectl);
  if(rc) {
        mapOS2ToWin32Rect(hwnd,&rectl, pRect);
  }
  else  memset(pRect, 0, sizeof(RECT));
  return rc;
}
//******************************************************************************
//******************************************************************************

