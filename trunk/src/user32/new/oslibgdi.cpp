/* $Id: oslibgdi.cpp,v 1.2 1999-07-18 14:39:35 sandervl Exp $ */
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

//******************************************************************************
//******************************************************************************
inline PRECTL Win32ToOS2Rect(PVOID pRectl)
{
 PRECTL pWinRect = (PRECTL)pRectl;
 ULONG  tmp;

  tmp = pWinRect->yBottom;
  pWinRect->yBottom = pWinRect->yTop;
  pWinRect->yTop = tmp;
  return pWinRect;
}
//******************************************************************************
//******************************************************************************
inline ULONG MAPWIN32POINT(RECTLOS2 *parent, RECTLOS2 *child, ULONG y)
{
  return (parent->yTop - parent->yBottom - (child->yTop - child->yBottom) - y - 1);
}
//******************************************************************************
//******************************************************************************
inline ULONG MAPWIN32POINT(RECTLOS2 *parent, ULONG cy, ULONG y)
{
  return (parent->yTop - parent->yBottom - cy - y - 1);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndChild)
{
 HWND     hwndParent;
 RECTLOS2 rectParent = {0}, rectChild = {0};

   WinQueryWindowRect(hwndChild, (PRECTL)&rectChild);
   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);
   return MAPWIN32POINT(&rectParent, &rectChild, rectChild.yBottom);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndChild, ULONG y)
{
 HWND     hwndParent;
 RECTLOS2 rectParent = {0}, rectChild = {0};

   WinQueryWindowRect(hwndChild, (PRECTL)&rectChild);
   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);
   return MAPWIN32POINT(&rectParent, &rectChild, y);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndChild, ULONG cy, ULONG y)
{
 HWND     hwndParent;
 RECTLOS2 rectParent = {0};

   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);
   return MAPWIN32POINT(&rectParent, cy, y);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(PRECTLOS2 rectParent, PRECTLOS2 rectChild, ULONG y)
{
   return MAPWIN32POINT(rectParent, rectChild, y);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(PRECTLOS2 rectParent, HWND hwndChild, ULONG y)
{
 RECTLOS2 rectChild = {0};

   WinQueryWindowRect(hwndChild, (PRECTL)&rectChild);
   return MAPWIN32POINT(rectParent, &rectChild, y);
}
//******************************************************************************
//******************************************************************************
ULONG MapOS2ToWin32Y(HWND hwndChild, PRECTLOS2 rectChild, ULONG y)
{
 HWND     hwndParent;
 RECTLOS2 rectParent = {0};

   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);
   return MAPWIN32POINT(&rectParent, rectChild, y);
}
//******************************************************************************
// MapOS2ToWin32Rectl
//   Convert OS/2 to Win32 RECTL structure
//
// Parameters:
//   hwndChild: Child window handle
//   rectChild: OS/2 child window RECTL
//
// Returns:
//   rectChild: Converted OS/2 rectange stored in Win32 RECTL (yTop & yBottom reversed)
//   TRUE:      Success
//   FALSE:     Failures
//******************************************************************************
BOOL MapOS2ToWin32Rectl(HWND hwndChild, PRECTLOS2 rectChild, PRECT rectWin32)
{
 HWND     hwndParent;
 RECTLOS2 rectParent = {0};

   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);

   rectWin32->yTop    = MAPWIN32POINT(&rectParent, rectChild->yTop - rectChild->yBottom, rectChild->yBottom);
   rectWin32->yBottom = MAPWIN32POINT(&rectParent, rectChild->yTop - rectChild->yBottom, rectChild->yTop);
   rectWin32->xLeft   = rectChild->xLeft;
   rectWin32->xRight  = rectChild->xRight;
   return TRUE;
}
//******************************************************************************
// MapOS2ToWin32Rectl
//   Convert OS/2 to Win32 RECTL structure
//
// Parameters:
//   rectParent: OS/2 Parent window RECTL
//   rectChild:  OS/2 Child window RECTL
//
// Returns:
//   rectChild:  Converted OS/2 rectange stored in Win32 RECTL (yTop & yBottom reversed)
//   TRUE:       Success
//   FALSE:      Failures
//******************************************************************************
BOOL MapOS2ToWin32Rectl(PRECTLOS2 rectParent, PRECTLOS2 rectChild, PRECT rectWin32)
{
   rectWin32->yTop    = MAPWIN32POINT(rectParent, rectChild->yTop - rectChild->yBottom, rectChild->yBottom);
   rectWin32->yBottom = MAPWIN32POINT(rectParent, rectChild->yTop - rectChild->yBottom, rectChild->yTop);
   rectWin32->xLeft   = rectChild->xLeft;
   rectWin32->xRight  = rectChild->xRight;
   return TRUE;
}
//******************************************************************************
// MapWin32ToOS2Rectl
//   Convert Win32 to OS/2 RECTL structure
//
// Parameters:
//   hwndChild:  OS/2  Child window handle (IN)
//   rectWin32:  Win32 Child window RECT   (IN)
//   rectChild:  OS/2  Child window RECTL  (OUT)
// Returns:
//   TRUE:       Success
//   FALSE:      Failures
//******************************************************************************
BOOL MapWin32ToOS2Rectl(HWND hwndChild, PRECT rectWin32, PRECTLOS2 rectChild)
{
 HWND     hwndParent;
 RECTLOS2 rectParent = {0};

   hwndParent = WinQueryWindow(hwndChild, QW_PARENT);
   WinQueryWindowRect(hwndParent, (PRECTL)&rectParent);

   rectChild->yTop    = MAPWIN32POINT(&rectParent, rectWin32->yBottom - rectWin32->yTop, rectWin32->yBottom);
   rectChild->yBottom = MAPWIN32POINT(&rectParent, rectWin32->yBottom - rectWin32->yTop, rectWin32->yTop);
   rectChild->xLeft   = rectWin32->xLeft;
   rectChild->xRight  = rectWin32->xRight;
   return TRUE;
}
//******************************************************************************
// MapWin32ToOS2Rectl
//   Convert Win32 to OS/2 RECTL structure
//
// Parameters:
//   rectParent: OS/2  Parent window RECTL (IN)
//   rectWin32:  Win32 Child window RECT   (IN)
//   rectChild:  OS/2  Child window RECTL  (OUT)
// Returns:
//   TRUE:       Success
//   FALSE:      Failures
//******************************************************************************
BOOL MapWin32ToOS2Rectl(PRECTLOS2 rectParent, PRECT rectWin32, PRECTLOS2 rectChild)
{
   rectChild->yTop    = MAPWIN32POINT(rectParent, rectWin32->yBottom - rectWin32->yTop, rectWin32->yBottom);
   rectChild->yBottom = MAPWIN32POINT(rectParent, rectWin32->yBottom - rectWin32->yTop, rectWin32->yTop);
   rectChild->xLeft   = rectWin32->xLeft;
   rectChild->xRight  = rectWin32->xRight;
   return TRUE;
}
//******************************************************************************
//******************************************************************************
HDC OSLibWinBeginPaint(HWND hwnd, PVOID pRectl)
{
 RECTLOS2 rectlOS2;

  MapWin32ToOS2Rectl(hwnd, (PRECT)pRectl, &rectlOS2);
  return WinBeginPaint(hwnd, NULLHANDLE, (PRECTL)&rectlOS2);
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
