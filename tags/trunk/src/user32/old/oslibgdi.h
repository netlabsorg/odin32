/* $Id: oslibgdi.h,v 1.1 2001-05-11 08:35:18 sandervl Exp $ */
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

//Mapping functions

INT  mapScreenY(INT screenPosY);
INT  mapScreenY(INT screenH,INT screenPosY);
INT inline mapY(INT height, int y)
{
   return height - 1 - y;
}

INT inline mapY(Win32BaseWindow *win32wnd,INT y)
{
   return win32wnd->getWindowHeight() - 1 - y;
}

INT inline mapOS2ToWin32Y(Win32BaseWindow *win32wnd,INT y)
{
   return win32wnd->getWindowHeight() - 1 - (y + win32wnd->getClientRectPtr()->top);
}

INT inline mapOS2ToWin32X(Win32BaseWindow *win32wnd,INT x)
{
   return x - win32wnd->getClientRectPtr()->left;
}

INT inline mapWin32ToOS2Y(Win32BaseWindow *win32wnd,INT y)
{
   if(win32wnd->getParent()) {
	return win32wnd->getWindowHeight() - 1 - (y - win32wnd->getParent()->getClientRectPtr()->top);
   }
   else	return y;
}

INT inline mapWin32ToOS2X(Win32BaseWindow *win32wnd,INT x)
{
   if(win32wnd->getParent()) {
	return x + win32wnd->getParent()->getClientRectPtr()->left;
   }
   else	return x;
}

INT  mapOS2ToWin32Y(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,INT fromPosY);
INT  mapWin32Y(Win32BaseWindow *wndFrom,Win32BaseWindow *wndTo,INT fromPosY);
BOOL mapScreenPoint(OSLIBPOINT *screenPt);
BOOL mapScreenPoint(INT screenH,OSLIBPOINT *screenPt);

BOOL mapOS2ToWin32Rect(int height, PRECTLOS2 rectOS2, PRECT rectWin32);
BOOL mapWin32ToOS2Rect(int height, PRECT rectWin32,PRECTLOS2 rectOS2);

BOOL mapWin32ToOS2RectClientToFrame(Win32BaseWindow *window, PRECT rectWin32,PRECTLOS2 rectOS2);
BOOL mapOS2ToWin32RectFrameToClient(Win32BaseWindow *window, PRECTLOS2 rectOS2, PRECT rectWin32);

#define mapWin32ToOS2RectFrame(window, rectWin32, rectOS2) \
        if(window->getParent()) { \
		mapWin32ToOS2Rect(window->getParent()->getWindowHeight(), rectWin32, rectOS2); \
        } \
        else    mapWin32ToOS2Rect(OSLibQueryScreenHeight(), rectWin32, rectOS2);

#define mapWin32ToOS2RectClient(window, rectWin32, rectOS2) \
        mapWin32ToOS2Rect(window->getClientHeight(), rectWin32, rectOS2)

#define mapWin32ToOS2ScreenRect(rectOS2, rectWin32) \
        mapWin32ToOS2Rect(OSLibQueryScreenHeight(), rectOS2, rectWin32)

#define mapOS2ToWin32RectFrame(window, rectOS2, rectWin32) \
        if(window->getParent()) { \
		mapOS2ToWin32Rect(window->getParent()->getWindowHeight(), rectOS2, rectWin32); \
        } \
        else    mapOS2ToWin32Rect(OSLibQueryScreenHeight(), rectOS2, rectWin32);
        

#define mapOS2ToWin32RectClient(window, rectOS2, rectWin32) \
        mapOS2ToWin32Rect(window->getClientHeight(), rectOS2, rectWin32)

#define mapOS2ToWin32ScreenRect(rectOS2, rectWin32) \
        mapOS2ToWin32Rect(OSLibQueryScreenHeight(), rectOS2, rectWin32)

BOOL copyOS2ToWin32Rect(PRECTLOS2 rectOS2,PRECT rectWin32);
BOOL copyWin32ToOS2WindowRect(PRECT rectWin32,PRECTLOS2 rectOS2);
INT  mapOS2ToWin32ChildOrigin(INT parentH,INT parentPosY,INT childH);

#define RECT_WIDTH(a)	((a).right - (a).left)
#define RECT_HEIGHT(a)	((a).bottom - (a).top)

#endif //__OSLIBGDI_H__
