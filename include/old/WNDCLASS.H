/* $Id: WNDCLASS.H,v 1.1 1999-09-15 23:30:40 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 window class for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __WNDCLASS_H__
#define __WNDCLASS_H__

#ifdef _OS2WIN_H
#include <winos2def.h>
#endif

//SvL: 18-7-'98, Registers system window classes (button, listbox etc etc)
void WINAPI RegisterSystemClasses(HMODULE hModule);
void WINAPI UnregisterSystemClasses(void);

class Win32WindowClass
{
public:
	Win32WindowClass(WNDPROC pUserCallback, LPSTR id, HINSTANCE hinst);
       ~Win32WindowClass();

static WNDPROC_O32 GetOS2ClassCallback();
 static    WNDPROC GetClassCallback(HINSTANCE hinst, LPSTR id);
 static    WNDPROC GetClassCallback(LPSTR id);
 static       void UnregisterClass(HINSTANCE hinst, LPSTR id);

 static Win32WindowClass *FindClass(LPSTR id);

	   WNDPROC GetWinCallback() { return pWinCallback; };
	      void SetWinCallback(WNDPROC newcallback)
	      {
		pWinCallback = newcallback;
	      };

private:

 WNDPROC        pWinCallback;

 HINSTANCE      hinst;

 char           className[128];
 DWORD          classAtom;

 static	        Win32WindowClass  *wndclasses;
  	        Win32WindowClass  *next;
};

#endif
