/* $Id: pmwindow.h,v 1.16 2002-02-18 10:14:57 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PMWINDOW_H__
#define __PMWINDOW_H__

#include <win32wbase.h>

#define WIN32_CDCLASS       "Win32CDWindowClass"
#define WIN32_STDCLASS      "Win32WindowClass"
#define WIN32_STDFRAMECLASS "Win32FrameClass"

#define TIMERID_DRIVEPOLL 	0x1717

BOOL InitPM();

void RegisterSystemClasses(ULONG hModule);
void UnregisterSystemClasses();

extern ULONG ScreenWidth;
extern ULONG ScreenHeight;
extern ULONG ScreenBitsPerPel;
extern BOOL  fOS2Look;
extern BOOL  fForceMonoCursor;
extern HWND  hwndCD;
extern LONG CapsCharHeight;

#define TFOS_LEFT                    0x0001
#define TFOS_TOP                     0x0002
#define TFOS_RIGHT                   0x0004
#define TFOS_BOTTOM                  0x0008
/* TF_MOVE = TF_LEFT | TF_TOP | TF_RIGHT | TF_BOTTOM */
#define TFOS_MOVE                    0x000F

VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags);
void Frame_SysCommandSizeMove(Win32BaseWindow *win32wnd, WPARAM wParam);

#endif
