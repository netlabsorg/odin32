/* $Id: oslibgdi.cpp,v 1.5 1999-12-27 22:53:52 cbratschi Exp $ */
/*
 * Window GDI wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
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

//CB: new mapping infrastructure to avoid transformation bugs -> available soon

/*
All functions can be used to transform from Win32 to OS/2 and vice versa
First letter is lower case to avoid conflicts with Win32 API names

Single y mapping:
 mapScreenY()
 mapWindowY()

Single point mapping:
 mapScreenPoint()
 mapWindowPoint()

Single rect mapping:
 mapScreenRect()
 mapWindowRect()

Rect transformation:
 copyOS2Rect()
 copyWin32Rect()
*/

INT mapScreenY(INT y)
{
  return WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN)-1-y;
}
//******************************************************************************
//******************************************************************************
INT mapScreenY(INT screenH,INT y)
{
  return screenH-1-y;
}


//******************************************************************************
//******************************************************************************
inline ULONG MAPWIN32POINT(RECTLOS2 *parent, ULONG cy, ULONG y)
{
    return (parent->yTop - parent->yBottom - cy - y);
}
//******************************************************************************
//Map win32 y coordinate (in parent window coordinates) to OS/2 y coord. (in parent window coordinates)
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndParent, ULONG cy, ULONG y)
{
 RECTLOS2 rectParent = {0};

    if(hwndParent == OSLIB_HWND_DESKTOP) {
        hwndParent = HWND_DESKTOP;
    }
    WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);
    return MAPWIN32POINT(&rectParent, cy, y);
}
//******************************************************************************
//******************************************************************************
BOOL MapOS2ToWin32Point(HWND hwndParent, HWND hwndChild, OSLIBPOINT *point)
{
 RECTLOS2 rectParent = {0};

    if(hwndParent == OSLIB_HWND_DESKTOP) {
        hwndParent = HWND_DESKTOP;
    }
    if(WinMapWindowPoints(hwndChild, hwndParent, (POINTL *)point, 1) != TRUE) {
        dprintf(("MapOS2ToWin32Point:WinMapWindowPoint %x %x returned false", hwndParent, hwndChild));
        return FALSE;
    }
    WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);
    point->y = rectParent.yTop - point->y - 1;
    return TRUE;
}
//******************************************************************************
// MapOS2ToWin32Rect
//   Map os/2 rectangle to screen coordinates and convert to win32 rect
//
// Parameters:
//   hwndParent: Parent window handle
//   hwndChild:  Child window handle
//   rectOS2:    OS/2 child window RECTL
//   rectWin32:  Win32 Child window RECT   (IN)
//
// Returns:
//   TRUE:      Success
//   FALSE:     Failures
//******************************************************************************
BOOL MapOS2ToWin32Rectl(HWND hwndParent, HWND hwndChild, PRECTLOS2 rectOS2, PRECT rectWin32)
{
 RECTLOS2 rectParent = {0};
 Win32BaseWindow *window;
 LONG height;

    if(hwndParent == OSLIB_HWND_DESKTOP) {
        hwndParent = HWND_DESKTOP;
    }
    if(WinMapWindowPoints(hwndChild, hwndParent, (PPOINTL)rectOS2, 2) != TRUE) {
        dprintf(("MapOS2ToWin32Rect:WinMapWindowPoint %x %x returned false", hwndParent, hwndChild));
        return FALSE;
    }

    if(hwndParent != HWND_DESKTOP)
    {
         window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndParent);
         if(window == NULL)
                return FALSE;
         height = window->getWindowHeight();
    }
    else height = OSLibQueryScreenHeight();

    rectWin32->bottom = height - rectOS2->yBottom;
    rectWin32->top    = height - rectOS2->yTop;
    rectWin32->left   = rectOS2->xLeft;
    rectWin32->right  = rectOS2->xRight;

    return TRUE;
}
//******************************************************************************
// MapOS2ToWin32Rectl
//   Convert OS/2 to Win32 RECTL structure
//
// Parameters:
//   hwnd:       OS/2 window handle
//   rectOS2:    OS/2 child window RECTL
//   rectWin32:  Win32 Child window RECT   (IN)
//
// Returns:
//   TRUE:      Success
//   FALSE:     Failures
//******************************************************************************
BOOL MapOS2ToWin32Rectl(HWND hwnd,PRECTLOS2 rectOS2, PRECT rectWin32)
{
   RECTL rect;

   if (!WinQueryWindowRect(hwnd,&rect)) return FALSE;

   rectWin32->bottom = rect.yTop-rectOS2->yBottom;
   rectWin32->top    = rect.yTop-rectOS2->yTop;
   rectWin32->left   = rectOS2->xLeft;
   rectWin32->right  = rectOS2->xRight;

   return TRUE;
}
//******************************************************************************
// MapWin32ToOS2Rectl
//   Convert Win32 to OS/2 RECTL structure
//
// Parameters:
//   hwnd:       OS/2 window handle
//   rectWin32:  Win32 Child window RECT   (IN)
//   rectOS2:    OS/2  Child window RECTL  (OUT)
// Returns:
//   TRUE:       Success
//   FALSE:      Failures
//******************************************************************************
BOOL MapWin32ToOS2Rectl(HWND hwnd,PRECT rectWin32, PRECTLOS2 rectOS2)
{
    RECTL rect;

    if (!WinQueryWindowRect(hwnd,&rect)) return FALSE;

    rectOS2->yBottom = rect.yTop-rectWin32->bottom;
    rectOS2->yTop    = rect.yTop-rectWin32->top;
    rectOS2->xLeft   = rectWin32->left;
    rectOS2->xRight  = rectWin32->right;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL MapOS2ToWin32WindowRect(PRECTLOS2 rectOS2,PRECT rectWin32)
{
  rectWin32->bottom = rectOS2->yTop;
  rectWin32->top    = rectOS2->yBottom;
  rectWin32->left   = rectOS2->xLeft;
  rectWin32->right  = rectOS2->xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL MapWin32ToOS2WindowRect(PRECT rectWin32,PRECTLOS2 rectOS2)
{
  rectOS2->yBottom = rectWin32->top;
  rectOS2->yTop    = rectWin32->bottom;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;

  return TRUE;
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
    MapOS2ToWin32Rectl(hwnd,(RECTLOS2 *)&rectl, rectWin32);
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
        MapWin32ToOS2Rectl(hwnd,pRect, &rectl);
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
        MapOS2ToWin32Rectl(hwnd,&rectl, pRect);
  }
  else  memset(pRect, 0, sizeof(RECT));
  return rc;
}
//******************************************************************************
//******************************************************************************

