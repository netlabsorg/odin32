/* $Id: pmwindow.h,v 1.3 2000-01-01 14:54:54 cbratschi Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PMWINDOW_H__
#define __PMWINDOW_H__

#define WIN32_STDCLASS   "Win32WindowClass"
#define WIN32_INNERFRAME "Win32InnerFrameClass"

BOOL InitPM();

PVOID SubclassWithDefHandler(HWND hwnd);

void RegisterSystemClasses(ULONG hModule);
void UnregisterSystemClasses();

extern ULONG ScreenWidth;
extern ULONG ScreenHeight;
extern ULONG ScreenBitsPerPel;

#endif
