/* $Id: win32wnd.h,v 1.12 1999-07-18 17:12:03 sandervl Exp $ */
/*
 * Win32 Window Code for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WND_H__
#define __WIN32WND_H__

#ifdef __cplusplus

#include <win32class.h>
#include <gen_object.h>
#include <win32wndchild.h>

class Win32Window;

#define OFFSET_WIN32WNDPTR	0
#define OFFSET_WIN32PM_MAGIC	4

#define WIN32PM_MAGIC		0x12345678
#define CheckMagicDword(a)	(a==WIN32PM_MAGIC)

typedef struct {
	USHORT		cb;
	Win32Window    *win32wnd;
	ULONG           win32CreateStruct;	//or dialog create dword
} CUSTOMWNDDATA;

typedef struct
{
  	ULONG 		Msg;
  	ULONG 		wParam;
  	ULONG 		lParam;
} POSTMSG_PACKET;

#define WM_WIN32_POSTMESSAGEA	0x4000
#define WM_WIN32_POSTMESSAGEW	0x4001

class Win32Window : private GenericObject, private ChildWindow
{
public:
	DWORD	magic;

    	     	Win32Window(DWORD objType);
    	     	Win32Window(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
virtual        ~Win32Window();

virtual  ULONG  MsgCreate(HWND hwndOS2, ULONG initParam);
	 ULONG  MsgQuit();
	 ULONG  MsgClose();
         ULONG  MsgDestroy();
	 ULONG  MsgEnable(BOOL fEnable);
	 ULONG  MsgShow(BOOL fShow);
	 ULONG  MsgMove(ULONG x, ULONG y);
	 ULONG  MsgHitTest(ULONG x, ULONG y);
	 ULONG  MsgSize(ULONG width, ULONG height, BOOL fMinimize, BOOL fMaximize);
         ULONG  MsgActivate(BOOL fActivate, HWND hwnd);
	 ULONG  MsgSetFocus(HWND hwnd);
	 ULONG  MsgKillFocus(HWND hwnd);
	 ULONG  MsgButton(ULONG msg, ULONG x, ULONG y);
	 ULONG  MsgMouseMove(ULONG keystate, ULONG x, ULONG y);
	 ULONG  MsgPaint(ULONG tmp1, ULONG tmp2);
	 ULONG  MsgEraseBackGround(ULONG hps);
	 ULONG  MsgSetText(LPSTR lpsz, LONG cch);

virtual	 LONG   SetWindowLongA(int index, ULONG value);
virtual	 ULONG  GetWindowLongA(int index);
virtual	 WORD   SetWindowWord(int index, WORD value);
virtual	 WORD   GetWindowWord(int index);

	 DWORD  getStyle()			{ return dwStyle; };
	 DWORD  getExStyle()			{ return dwExStyle; };
	 HWND   getWindowHandle() 		{ return Win32Hwnd; };
	 HWND   getOS2WindowHandle() 		{ return OS2Hwnd; };
   Win32Window *getParent()			{ return (Win32Window *)ChildWindow::GetParent(); };
	 void   setParent(Win32Window *pwindow) { ChildWindow::SetParent((ChildWindow *)pwindow); };
       WNDPROC  getWindowProc()                 { return win32wndproc; };
         void   setWindowProc(WNDPROC newproc)  { win32wndproc = newproc; };
        DWORD   getWindowId()                   { return windowId; };
         void   setWindowId(DWORD id)           { windowId = id; };

	 DWORD  getFlags()			{ return flags; };
	 void   setFlags(DWORD newflags)	{ flags = newflags; };

	 BOOL   SetMenu(ULONG hMenu);
	 BOOL   ShowWindow(ULONG nCmdShow);
	 BOOL   SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags);
	 BOOL   DestroyWindow();
	 HWND   SetActiveWindow();
	 HWND   GetParent();
	 HWND   SetParent(HWND hwndNewParent);
	 BOOL   IsChild(HWND hwndParent);
	 HWND   GetTopWindow();
	 BOOL   UpdateWindow();
	 BOOL   IsIconic();
         HWND   GetWindow(UINT uCmd);
	 BOOL   EnableWindow(BOOL fEnable);
 	 BOOL   BringWindowToTop();
  static HWND   GetActiveWindow();
	 BOOL   IsWindow();
	 BOOL   IsWindowEnabled();
	 BOOL   IsWindowVisible();

	 BOOL   GetWindowRect(PRECT pRect);
	 int    GetWindowTextLengthA();
	 int    GetWindowTextA(LPSTR lpsz, int cch);
 	 BOOL   SetWindowTextA(LPCSTR lpsz);

       LRESULT  SendMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
       BOOL     PostMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       BOOL     PostMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefWindowProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefWindowProcW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

         void   NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam);

Win32WndClass  *getClass()  { return windowClass; };

static   HWND  Win32ToOS2Handle(HWND hwnd)
{
 Win32Window *window = GetWindowFromHandle(hwnd);

  if(window) {
	return window->getOS2WindowHandle();
  }
  else	return hwnd;	//OS/2 window handle
}

static   HWND  OS2ToWin32Handle(HWND hwnd)
{
 Win32Window *window = GetWindowFromOS2Handle(hwnd);

  if(window) {
	return window->getWindowHandle();
  }
  else	return hwnd;	//OS/2 window handle
}

static Win32Window *GetWindowFromHandle(HWND hwnd);
static Win32Window *GetWindowFromOS2Handle(HWND hwnd);

protected:
       LRESULT  SendInternalMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendInternalMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
	void    Init();

	HWND	OS2Hwnd;
	HWND	OS2HwndFrame;
	HWND    OS2HwndMenu;
	HWND	Win32Hwnd;
	BOOL    isUnicode;

	int	posx, posy, width, height;

	// values normally contained in the standard window words
	ULONG   dwExStyle;		//GWL_EXSTYLE
	ULONG	dwStyle;		//GWL_STYLE
      WNDPROC   win32wndproc;		//GWL_WNDPROC
	ULONG   hInstance;		//GWL_HINSTANCE
//Moved in ChildWindow class
/////   Win32Window *parent;			//GWL_HWNDPARENT
	ULONG	windowId;		//GWL_ID
	ULONG	userData;		//GWL_USERDATA

         HWND   hwndLinkAfter;
        DWORD   flags;
	DWORD   lastHitTestVal;		//Last value returned by WM_NCHITTEST handler

	BOOL    isIcon;

   Win32Window *owner;			

	char   *windowName;
	ULONG   wndNameLength;

	char   *windowText;
	ULONG   wndTextLength;
	
	ULONG  *userWindowLong;
	ULONG	nrUserWindowLong;

	RECT    rectWindow;
	RECT    rectClient;

Win32WndClass  *windowClass;

static GenericObject *windows;

private:
#ifndef OS2_INCLUDED
	BOOL  CreateWindowExA(CREATESTRUCTA *lpCreateStruct, ATOM classAtom);

	void  GetMinMaxInfo(POINT *maxSize, POINT *maxPos, POINT *minTrack, POINT *maxTrack );

	LONG  SendNCCalcSize(BOOL calcValidRect,
                             RECT *newWindowRect, RECT *oldWindowRect,
                             RECT *oldClientRect, WINDOWPOS *winpos,
                             RECT *newClientRect );

     LRESULT  SendInternalMessage(ULONG msg, WPARAM wParam, LPARAM lParam)
     {
	if(isUnicode)
		return SendInternalMessageW(msg, wParam, lParam);
	else	return SendInternalMessageA(msg, wParam, lParam);
     }
#endif
};


#define BUTTON_LEFTDOWN		0
#define BUTTON_LEFTUP		1
#define BUTTON_LEFTDBLCLICK	2
#define BUTTON_RIGHTUP		3
#define BUTTON_RIGHTDOWN	4
#define BUTTON_RIGHTDBLCLICK	5
#define BUTTON_MIDDLEUP		6
#define BUTTON_MIDDLEDOWN	7
#define BUTTON_MIDDLEDBLCLICK	8

#define WMMOVE_LBUTTON		1
#define WMMOVE_MBUTTON		2
#define WMMOVE_RBUTTON		4
#define WMMOVE_CTRL		8
#define WMMOVE_SHIFT		16


#endif //__cplusplus

#endif //__WIN32WND_H__