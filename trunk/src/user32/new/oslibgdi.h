/* $Id: oslibgdi.h,v 1.8 2000-01-05 21:25:03 cbratschi Exp $ */
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

//Mapping functions

INT  mapScreenY(INT screenPosY);
INT  mapScreenY(INT screenH,INT screenPosY);
INT  mapY(HWND os2Client,INT clientPosY);
INT  mapY(Win32BaseWindow *win32wnd,INT clientPosY);
INT  mapOS2ToWin32Y(HWND os2From,HWND os2To,INT fromPosY);
INT  mapOS2ToWin32Y(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,INT fromPosY);
INT  mapWin32Y(HWND os2From,HWND os2To,INT fromPosY);
INT  mapWin32Y(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,INT fromPosY);
BOOL mapScreenPoint(OSLIBPOINT *screenPt);
BOOL mapScreenPoint(INT screenH,OSLIBPOINT *screenPt);
BOOL mapPoint(HWND os2Client,OSLIBPOINT *clientPt);
BOOL mapPoint(Win32BaseWindow *win32wnd,OSLIBPOINT *clientPt);
BOOL mapOS2ToWin32Point(HWND os2From,HWND os2To,OSLIBPOINT *fromPt);
BOOL mapOS2ToWin32Point(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,OSLIBPOINT *fromPt);
BOOL mapWin32Point(HWND os2From,HWND os2To,OSLIBPOINT *fromPt);
BOOL mapWin32Point(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,OSLIBPOINT *fromPt);
BOOL mapOS2ToWin32ScreenRect(PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL mapWin32ToOS2ScreenRect(PRECT rectWin32,PRECTLOS2 rectOS2);
BOOL mapOS2ToWin32Rect(HWND os2Client,PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL mapOS2ToWin32Rect(Win32BaseWindow *win32wnd,PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL mapOS2ToWin32Rect(HWND os2From,HWND os2To,PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL mapOS2ToWin32Rect(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL mapWin32ToOS2Rect(HWND os2Client,PRECT rectWin32,PRECTLOS2 rectOS2);
BOOL mapWin32ToOS2Rect(Win32BaseWindow *win32wnd,PRECT rectWin32,PRECTLOS2 rectOS2);
BOOL mapWin32ToOS2Rect(HWND os2From,HWND os2To,PRECT rectWin32,PRECTLOS2 rectOS2);
BOOL mapWin32ToOS2Rect(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,PRECT rectWin32,PRECTLOS2 rectOS2);
BOOL mapWin32Rect(HWND os2From,HWND os2To,PRECT rectWin32);
BOOL copyOS2ToWin32Rect(PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL copyWin32ToOS2WindowRect(PRECT rectWin32,PRECTLOS2 rectOS2);
INT  mapOS2ToWin32ChildOrigin(INT parentH,INT parentPosY,INT childH);

#endif //__OSLIBGDI_H__
