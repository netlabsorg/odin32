/* $Id: pmwindow.h,v 1.4 2000-01-01 17:07:42 cbratschi Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PMWINDOW_H__
#define __PMWINDOW_H__

#define WIN32_STDCLASS   "Win32WindowClass"
#define WIN32_STDCLASS2  "Win32WindowClass2"

BOOL InitPM();

PVOID SubclassWithDefHandler(HWND hwnd);

void RegisterSystemClasses(ULONG hModule);
void UnregisterSystemClasses();

extern ULONG ScreenWidth;
extern ULONG ScreenHeight;
extern ULONG ScreenBitsPerPel;

#endif
