/* $Id: wndclass.h,v 1.2 1999-05-27 15:17:57 phaller Exp $ */

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
extern "C" {
void RegisterSystemClasses(ULONG hModule);
}

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