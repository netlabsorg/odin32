/* $Id: pmwindow.h,v 1.9 2000-06-07 14:51:27 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PMWINDOW_H__
#define __PMWINDOW_H__

#define WIN32_STDCLASS      "Win32WindowClass"
#define WIN32_STDFRAMECLASS "Win32FrameClass"

BOOL InitPM();

void RegisterSystemClasses(ULONG hModule);
void UnregisterSystemClasses();

extern ULONG ScreenWidth;
extern ULONG ScreenHeight;
extern ULONG ScreenBitsPerPel;

#endif
