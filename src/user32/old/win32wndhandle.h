/* $Id: win32wndhandle.h,v 1.1 2001-05-11 08:35:22 sandervl Exp $ */
/*
 * Win32 Handle Management Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WNDHANDLE_H__
#define __WIN32WNDHANDLE_H__

#define MAX_WINDOW_HANDLES	 1024
#define WNDHANDLE_MAGIC_HIGHWORD 0x68000000
#define WNDHANDLE_MAGIC_MASK     0x0000FFFF

BOOL  HwAllocateWindowHandle(HWND *hwnd, DWORD dwUserData);
void  HwFreeWindowHandle(HWND hwnd);
BOOL  HwGetWindowHandleData(HWND hwnd, DWORD *pdwUserData);

#endif //__WIN32WNDHANDLE_H__