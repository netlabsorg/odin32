/* $Id: oslibgdi.cpp,v 1.1 1999-09-15 23:18:53 sandervl Exp $ */
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

    if(hwndParent == OSLIB_HWND_DESKTOP) {
        hwndParent = HWND_DESKTOP;
    }
    if(WinMapWindowPoints(hwndChild, hwndParent, (PPOINTL)rectOS2, 2) != TRUE) {
        dprintf(("MapOS2ToWin32Rect:WinMapWindowPoint %x %x returned false", hwndParent, hwndChild));
        return FALSE;
    }

    ULONG length = rectOS2->yTop - rectOS2->yBottom;

    rectWin32->bottom = length - rectOS2->yBottom;
    rectWin32->top    = length - rectOS2->yTop;
    rectWin32->left   = rectOS2->xLeft;
    rectWin32->right  = rectOS2->xRight;

    return TRUE;
}
//******************************************************************************
// MapOS2ToWin32Rectl
//   Convert OS/2 to Win32 RECTL structure
//
// Parameters:
//   rectOS2:    OS/2 child window RECTL
//   rectWin32:  Win32 Child window RECT   (IN)
//
// Returns:
//   TRUE:      Success
//   FALSE:     Failures
//******************************************************************************
BOOL MapOS2ToWin32Rectl(PRECTLOS2 rectOS2, PRECT rectWin32)
{
 ULONG length = rectOS2->yTop - rectOS2->yBottom;

   rectWin32->bottom = length - rectOS2->yBottom;
   rectWin32->top    = length - rectOS2->yTop;
   rectWin32->left   = rectOS2->xLeft;
   rectWin32->right  = rectOS2->xRight;
   return TRUE;
}
//******************************************************************************
// MapWin32ToOS2Rectl
//   Convert Win32 to OS/2 RECTL structure
//
// Parameters:
//   rectWin32:  Win32 Child window RECT   (IN)
//   rectOS2:    OS/2  Child window RECTL  (OUT)
// Returns:
//   TRUE:       Success
//   FALSE:      Failures
//******************************************************************************
BOOL MapWin32ToOS2Rectl(PRECT rectWin32, PRECTLOS2 rectOS2)
{
 ULONG length = rectWin32->top - rectWin32->bottom;

    rectOS2->yBottom = length - rectWin32->bottom;
    rectOS2->yTop    = length - rectWin32->top;
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
    MapOS2ToWin32Rectl((RECTLOS2 *)&rectl, rectWin32);
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
        MapWin32ToOS2Rectl(pRect, &rectl);
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
        MapOS2ToWin32Rectl(&rectl, pRect);
  }
  else  memset(pRect, 0, sizeof(RECT));
  return rc;
}
//******************************************************************************
//******************************************************************************

