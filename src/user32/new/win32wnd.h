/* $Id: win32wnd.h,v 1.5 1999-07-17 09:17:58 sandervl Exp $ */
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

#define WIN32PM_MAGIC	0x12345678

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
	 ULONG  MsgMove(ULONG xScreen, ULONG yScreen, ULONG xParent, ULONG yParent);
	 ULONG  MsgSize(ULONG width, ULONG height, BOOL fMinimize, BOOL fMaximize);
         ULONG  MsgActivate(BOOL fActivate, HWND hwnd);
	 ULONG  MsgSetFocus(HWND hwnd);
	 ULONG  MsgKillFocus(HWND hwnd);
	 ULONG  MsgButton(ULONG msg, ULONG x, ULONG y);
	 ULONG  MsgPaint(ULONG tmp1, ULONG tmp2);
	 ULONG  MsgEraseBackGround(ULONG hps);

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

       LRESULT  SendMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  SendMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
       BOOL     PostMessageA(ULONG msg, WPARAM wParam, LPARAM lParam);
       BOOL     PostMessageW(ULONG msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefWindowProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
       LRESULT  DefWindowProcW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

         void   NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam);

Win32WndClass  *getClass()  { return windowClass; };

static Win32Window *GetWindowFromHandle(HWND hwnd);

protected:
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

     LRESULT  sendMessage(ULONG msg, WPARAM wParam, LPARAM lParam)
     {
	if(isUnicode)
		return SendMessageW(msg, wParam, lParam);
	else	return SendMessageA(msg, wParam, lParam);
     }
#endif
};


#define BUTTON_LEFTDOWN		0
#define BUTTON_LEFTUP		1
#define BUTTON_LEFTDBLCLICK	2
#define BUTTON_RIGHTUP		3
#define BUTTON_RIGHTDOWN	4
#define BUTTON_RIGHTDBLCLICK	5

#endif //__cplusplus

#endif //__WIN32WND_H__