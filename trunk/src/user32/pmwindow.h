/* $Id: pmwindow.h,v 1.2 1999-10-17 12:17:44 cbratschi Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PMWINDOW_H__
#define __PMWINDOW_H__

#define WIN32_STDCLASS  "Win32WindowClass"

BOOL InitPM();

PVOID SubclassWithDefHandler(HWND hwnd);

void RegisterSystemClasses(ULONG hModule);
void UnregisterSystemClasses();

#endif