/* $Id: wndproc.h,v 1.3 1999-06-20 16:47:37 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Window procedure class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __WNDPROC_H__
#define __WNDPROC_H__

#ifdef _OS2WIN_H
#include <winos2def.h>
#endif

#include "wndclass.h"

//forward decl
class Win32WindowProc;

//used in COMDLG32.DLL
Win32WindowProc *SYSTEM CreateWindowProc(WNDPROC pUserCallback);
LRESULT EXPENTRY_O32 WndCallback(HWND, UINT, WPARAM, LPARAM);

//Notify parent window of creation/destruction and buttondown messages (if required)
void NotifyParent(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

class Win32WindowProc
{
public:
	Win32WindowProc(WNDPROC pUserCallback);
	Win32WindowProc(WNDPROC pUserCallback, DLGTEMPLATE *os2dlg);
	Win32WindowProc(HINSTANCE hinst, LPCSTR lpszClassName);
       ~Win32WindowProc();

 static WNDPROC_O32 GetOS2Callback();

 static void DeleteWindow(HWND hwnd);
 static Win32WindowProc *FindProc(HWND hwnd);
 static Win32WindowProc *FindProc(HWND hwnd, DWORD threadid);
	void SetWindowHandle(HWND hwndProc) { hwnd = hwndProc; };

 static BOOL FindWindowProc(Win32WindowProc *wndproc);

        WNDPROC GetWin32Callback(){ return pCallback; };
           void SetWin32Callback(WNDPROC pUserCallback) 
		{ 
			pCallback = pUserCallback; 
		};
	 int    IsWindow() { return fIsWindow; };

Win32WindowClass *GetWindowClass() { return win32class; };

private:

 WNDPROC        pCallback;
 HWND           hwnd;
 DWORD          threadid;
 int            fIsWindow;
 DLGTEMPLATE   *os2dlg;

 Win32WindowClass *win32class;

 static	        Win32WindowProc  *windows;
  	        Win32WindowProc  *next;

 friend LRESULT EXPENTRY_O32 WndCallback(HWND, UINT, WPARAM, LPARAM);
};

#endif