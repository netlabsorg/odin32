/* $Id: pmframe.h,v 1.4 1999-11-10 17:11:30 cbratschi Exp $ */
/*
 * Win32 Frame Managment Code for OS/2
 *
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 */
#ifndef __PMFRAME_H__
#define __PMFRAME_H__

PVOID FrameSubclassFrameWindow(Win32BaseWindow *win32wnd);
VOID  FrameSetBorderSize(Win32BaseWindow *win32wnd,BOOL resize);
UINT  FrameGetDefSizeBorderSize(VOID);
VOID  FrameTrackFrame(Win32BaseWindow *win32wnd,BOOL lefttop);
BOOL  FrameCreateScrollBars(Win32BaseWindow *win32wnd,BOOL createHorz,BOOL createVert,BOOL updateFrame,DWORD *flags = NULL);
VOID  FrameGetScrollBarHandles(Win32BaseWindow *win32wnd,BOOL getHorz,BOOL getVert);
BOOL  FrameShowScrollBars(Win32BaseWindow *win32wnd,BOOL changeHorz,BOOL changeVert,BOOL fShow,BOOL updateFrame,DWORD *flags = NULL);
VOID  FrameUpdateFrame(Win32BaseWindow *win32wnd,DWORD flags);

#endif
