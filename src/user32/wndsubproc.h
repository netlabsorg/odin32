/* $Id: wndsubproc.h,v 1.1 1999-05-24 20:20:00 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 window subproc class for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __WNDSUBPROC_H__
#define __WNDSUBPROC_H__

#ifdef _OS2WIN_H
#include <winos2def.h>
#endif

#include "wndproc.h"

class Win32WindowSubProc
{
public:
	Win32WindowSubProc(HWND hwnd, WNDPROC_O32 pOpen32Callback);
       ~Win32WindowSubProc();

 static WNDPROC GetWin32Callback();
 static Win32WindowSubProc *FindSubProc(WNDPROC_O32 pOrgCallback);

 static void DeleteSubWindow(HWND hwnd);

private:

 WNDPROC_O32    pCallback;
 HWND           hwnd;

 static	        Win32WindowSubProc  *windows;
  	        Win32WindowSubProc  *next;

 friend static LRESULT WIN32API SubWndCallback(HWND, UINT, WPARAM, LPARAM);
};

#endif