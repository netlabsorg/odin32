/* $Id: oslibgdi.h,v 1.1 1999-07-18 10:39:51 sandervl Exp $ */
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

typedef struct
{
    LONG  xLeft;
    LONG  yBottom;
    LONG  xRight;
    LONG  yTop;
} RECTLOS2, *PRECTLOS2;

HDC   OSLibWinBeginPaint(HWND hwnd, PVOID pRectl);
BOOL  OSLibWinEndPaint(HDC hdc);

HDC   OSLibWinGetPS(HWND hwnd);
BOOL  OSLibWinReleasePS(HDC hdc);

ULONG MapOS2ToWin32Y(HWND hwndChild);
ULONG MapOS2ToWin32Y(HWND hwndChild, ULONG y);
ULONG MapOS2ToWin32Y(HWND hwndChild, ULONG cy, ULONG y);
ULONG MapOS2ToWin32Y(PRECTLOS2 rectParent, PRECTLOS2 rectChild, ULONG y);
ULONG MapOS2ToWin32Y(PRECTLOS2 rectParent, HWND hwndChild, ULONG y);
ULONG MapOS2ToWin32Y(HWND hwndChild, PRECTLOS2 rectChild, ULONG y);
BOOL  MapOS2ToWin32Rectl(HWND hwndChild, PRECTLOS2 rectChild, PRECT rectWin32);
BOOL  MapOS2ToWin32Rectl(PRECTLOS2 rectParent, PRECTLOS2 rectChild, PRECT rectWin32);
BOOL  MapWin32ToOS2Rectl(HWND hwndChild, PRECTLOS2 rectChild, PRECT rectWin32);
BOOL  MapWin32ToOS2Rectl(PRECTLOS2 rectParent, PRECTLOS2 rectChild, PRECT rectWin32);

#endif //__OSLIBGDI_H__
