/* $Id: win32wndhandle.h,v 1.1 1999-08-28 19:32:47 sandervl Exp $ */
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

BOOL  HwAllocateWindowHandle(HWND *hwnd, DWORD dwUserData);
void  HwFreeWindowHandle(HWND hwnd);
BOOL  HwGetWindowHandleData(HWND hwnd, DWORD *pdwUserData);

#endif //__WIN32WNDHANDLE_H__