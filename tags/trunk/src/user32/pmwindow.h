/* $Id: pmwindow.h,v 1.20 2003-04-01 09:58:38 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PMWINDOW_H__
#define __PMWINDOW_H__

#include <win32wbase.h>

extern char WIN32_CDCLASS[255];
extern char WIN32_STDCLASS[255];
extern char WIN32_STDFRAMECLASS[255];

#define TIMERID_DRIVEPOLL 	0x1717

BOOL InitPM();

void RegisterSystemClasses(ULONG hModule);
void UnregisterSystemClasses();

PVOID PMWinSubclassFakeWindow(HWND hwndOS2);

extern ULONG ScreenWidth;
extern ULONG ScreenHeight;
extern ULONG ScreenBitsPerPel;
extern BOOL  fOS2Look;
extern BOOL  fForceMonoCursor;
extern HWND  hwndCD;
extern LONG CapsCharHeight;
extern BOOL  fDragDropActive;

//Win32 bitmap handles of the OS/2 min, max and restore buttons
extern HBITMAP hBmpMinButton, hBmpMinButtonDown;
extern HBITMAP hBmpMaxButton, hBmpMaxButtonDown;
extern HBITMAP hBmpRestoreButton, hBmpRestoreButtonDown;
extern HBITMAP hBmpCloseButton, hBmpCloseButtonDown;

#define TFOS_LEFT                    0x0001
#define TFOS_TOP                     0x0002
#define TFOS_RIGHT                   0x0004
#define TFOS_BOTTOM                  0x0008
/* TF_MOVE = TF_LEFT | TF_TOP | TF_RIGHT | TF_BOTTOM */
#define TFOS_MOVE                    0x000F

#ifdef __cplusplus
VOID FrameTrackFrame(Win32BaseWindow *win32wnd,DWORD flags);
void Frame_SysCommandSizeMove(Win32BaseWindow *win32wnd, WPARAM wParam);
#endif

#endif
