/* $Id: win32wnd.h,v 1.1 1999-05-24 20:20:05 ktk Exp $ */

#ifndef __WIN32WND_H__
#define __WIN32WND_H__

#ifdef __cplusplus

#include "win32class.h"

#define WIN2OS2HWND(a)	(a^a)
#define OS22WINHWND(a)	(a^a)

//Win32 window message handler
typedef LRESULT  (* WIN32API WINWNDPROC) ( HWND, UINT, WPARAM, LPARAM );

class Win32Window
{
public:
    	     	Win32Window(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
virtual        ~Win32Window();

     MRESULT    ProcessMessage(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

virtual	 BOOL   SetWindowLong(int index, ULONG value);
virtual	 ULONG  GetWindowLong(int index);

	 BOOL   PostMessageA(HWND hwnd, ULONG msg, WPARAM wParam, LPARAM lParam);
	 BOOL   PostMessageW(HWND hwnd, ULONG msg, WPARAM wParam, LPARAM lParam);

Win32WndClass  *getClass()  { return windowClass; };

protected:
	HWND	OS2Hwnd;
	HWND	Win32Hwnd;
	BOOL    isUnicode;

	int	posx, posy, width, height;

	// values normally contained in the standard window words
	ULONG   ExtendedWindowStyle;	//GWL_EXSTYLE
	ULONG	WindowStyle;		//GWL_STYLE
   //ptr to WINWNDPROC in windowClass
   WINWNDPROC  *win32wndproc;		//GWL_WNDPROC
	ULONG   hInstance;		//GWL_HINSTANCE
	HWND	hwndParent;		//GWL_HWNDPARENT
	ULONG	windowId;		//GWL_ID
	ULONG	userData;		//GWL_USERDATA

	char   *windowText;
	ULONG   wndTextLength;
	
	ULONG  *UserWindowLong;
	ULONG	nrUserWindowLong;

Win32WndClass  *windowClass;

  Win32Window  *parent;
  Win32Window  *child;
  Win32Window  *nextchild;

private:


};


#endif //__cplusplus

#endif //__WIN32WND_H__