/* $Id: oslibgdi.h,v 1.2 1999-11-26 17:06:07 cbratschi Exp $ */
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
#ifndef __OSLIBGDI_H__
#define __OSLIBGDI_H__

#ifdef OS2_INCLUDED
#include "win32type.h"
#endif
#include <win32wbase.h>

typedef struct
{
    LONG  xLeft;
    LONG  yBottom;
    LONG  xRight;
    LONG  yTop;
} RECTLOS2, *PRECTLOS2;

typedef struct
{
    LONG  x;
    LONG  y;
} OSLIBPOINT;

HDC   OSLibWinBeginPaint(HWND hwnd, RECT *pRectl);
BOOL  OSLibWinEndPaint(HDC hdc);

HDC   OSLibWinGetPS(HWND hwnd);
BOOL  OSLibWinReleasePS(HDC hdc);

BOOL  OSLibWinInvalidateRect(HWND hwnd, PRECT pRect, BOOL fIncludeChildren); //must be RECTL pointer!
BOOL  OSLibWinQueryUpdateRect(HWND hwnd, PRECT pRect);

//******************************************************************************
//Map win32 y coordinate (in window coordinates) to OS/2 y coord. (in window coordinates)
//******************************************************************************
inline ULONG MapOS2ToWin32Y(Win32BaseWindow *window, ULONG y)
{
    return window->getWindowHeight() - y - 1;
}

ULONG MapOS2ToWin32Y(HWND hwndParent, ULONG cy, ULONG y);
BOOL  MapOS2ToWin32Point(HWND hwndParent, HWND hwndChild, OSLIBPOINT *point);

//map os/2 rectangle to screen coordinates and convert to win32 rect
BOOL  MapOS2ToWin32Rectl(HWND hwndParent, HWND hwndChild, PRECTLOS2 rectOS2, PRECT rectWin32);

BOOL  MapOS2ToWin32Rectl(HWND hwnd,PRECTLOS2 rectOS2, PRECT rectWin32);
BOOL  MapWin32ToOS2Rectl(HWND hwnd,PRECT rectWin32, PRECTLOS2 rectOS2);

BOOL  MapOS2ToWin32WindowRect(PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL  MapWin32ToOS2WindowRect(PRECT rectWin32,PRECTLOS2 rectOS2);

#endif //__OSLIBGDI_H__
