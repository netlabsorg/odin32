/* $Id: wndproc.cpp,v 1.11 1999-06-26 18:25:08 sandervl Exp $ */

/*
 * Win32 window procedure class for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <dde.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <misc.h>
#include <wndproc.h>
#include <wndclass.h>
#include <spy.h>
#include <wprocess.h>
#include "dlgconvert.h"
#include "hooks.h"

#ifdef DEBUG
char *GetMsgText(int Msg);
#endif


void NotifyParent(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
 DWORD   dwStyle, dwExStyle;
 HWND    hwndParent = GetParent(hwnd);;

   while(hwndParent) {
	dwStyle   = GetWindowLongA(hwnd, GWL_STYLE);
	dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
	//SvL: Taken from Wine
	if(dwStyle & WS_CHILD && !(dwExStyle & WS_EX_NOPARENTNOTIFY) )
	{
		//NOTE: Open32's SendMessage swallows a WM_PARENTNOTIFY, so call
	 	//      the win32 callback handler directly!
		hwndParent = GetParent(hwnd);
		if(hwnd == hwndParent) {
			break;
		}

		dprintf(("%s Send WM_PARENTNOTIFY from child %x to parent %x", GetMsgText(Msg), hwnd, hwndParent));
		/* Notify the parent window only */
		Win32WindowProc *parentwnd = Win32WindowProc::FindProc(hwndParent);
		if(parentwnd) {
			if(Msg == WM_CREATE || Msg == WM_DESTROY) {
				//Todo: Set IdChild!!
				parentwnd->SendMessageA(hwndParent, WM_PARENTNOTIFY, MAKEWPARAM(Msg, 0), (LPARAM)hwnd );
			}
			else	parentwnd->SendMessageA(hwndParent, WM_PARENTNOTIFY, MAKEWPARAM(Msg, 0), lParam );
		}
	}
	else	break;

	hwnd = hwndParent;
   }
}
//******************************************************************************
//******************************************************************************
Win32WindowProc *SYSTEM CreateWindowProc(WNDPROC pUserCallback)
{
  return new Win32WindowProc(pUserCallback);
}
//******************************************************************************
//******************************************************************************
Win32WindowProc::Win32WindowProc(WNDPROC pUserCallback)
                        : hwnd(0), next(NULL), os2dlg(NULL), win32class(0)
{
  //Insert it in front of the rest
  next       = windows;
  windows    = this;
  threadid   = (DWORD)GetCurrentThreadId();

  pCallback  = pUserCallback;
  fIsWindow  = TRUE;
}
//******************************************************************************
//******************************************************************************
Win32WindowProc::Win32WindowProc(WNDPROC pUserCallback, DLGTEMPLATE *os2dlg)
                        : hwnd(0), next(NULL), os2dlg(NULL), win32class(0)
{
  //Insert it in front of the rest
  next       = windows;
  windows    = this;
  threadid   = (DWORD)GetCurrentThreadId();
  this->os2dlg = os2dlg;        //delete on destruction

  pCallback  = pUserCallback;
  fIsWindow  = FALSE;
}
//******************************************************************************
//******************************************************************************
Win32WindowProc::Win32WindowProc(HINSTANCE hinst, LPCSTR lpszClassName)
                        : hwnd(0), next(NULL), os2dlg(NULL)
{
 WNDCLASSA wc;
 BOOL rc;

  rc = GetClassInfoA(hinst, lpszClassName, &wc);
  assert(rc == TRUE);

//  pCallback = Win32WindowClass::GetClassCallback(hinst, (LPSTR)wc.lpszClassName);
  pCallback = Win32WindowClass::GetClassCallback((LPSTR)wc.lpszClassName);
//test (8nov)
  if(pCallback == NULL) {//system class
        pCallback = (WNDPROC)wc.lpfnWndProc;
  }
//  assert(pCallback != NULL);

  //Insert it in front of the rest
  next       = windows;
  windows    = this;
  threadid   = (DWORD)GetCurrentThreadId();

  fIsWindow  = TRUE;

  win32class = Win32WindowClass::FindClass((LPSTR)lpszClassName);

}
//******************************************************************************
//******************************************************************************
Win32WindowProc::~Win32WindowProc()
{
  Win32WindowProc *window = Win32WindowProc::windows;
  
  /* @@@PH 98/07/13 what's this whole code good for ? */
  if(window == this) 
  {
    windows = next;
  }
  else 
  {
    /* @@@PH 98/07/13 window can be NULL */
    if (window != NULL)
      while(window->next != NULL) 
      {
        if(window->next == this) 
        {
          window->next = next;
          break;
        }
        window = window->next;
      }
  }
  
  if(os2dlg) 
  {
    DeleteWin32DlgTemplate(os2dlg);
    os2dlg = NULL;
  }
}
//******************************************************************************
//******************************************************************************
BOOL Win32WindowProc::FindWindowProc(Win32WindowProc *wndproc)
{
  Win32WindowProc *window = Win32WindowProc::windows;

  while(window != NULL) {
        if(window == wndproc) {
                return(TRUE);
        }
        window = window->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
WNDPROC_O32 Win32WindowProc::GetOS2Callback()
{
  return(WndCallback);
}
//******************************************************************************
//******************************************************************************
Win32WindowProc *Win32WindowProc::FindProc(HWND hwnd)
{
  Win32WindowProc *window = Win32WindowProc::windows;

  while(window != NULL) {
        if(window->hwnd == hwnd) {
                return(window);
        }
        window = window->next;
  }
  dprintf(("Win32WindowProc::FindProc, can't find window %X!\n", hwnd));
  return(NULL);
}
//******************************************************************************
//Find newly created window
//******************************************************************************
Win32WindowProc *Win32WindowProc::FindProc(HWND hwnd, DWORD threadid)
{
  Win32WindowProc *window = Win32WindowProc::windows;

  while(window != NULL) {
        if(window->hwnd == 0 && window->threadid == threadid) {
                return(window);
        }
        window = window->next;
  }
  dprintf(("Win32WindowProc::FindProc, can't find window %X %d!\n", hwnd, threadid));
  return(NULL);
}
//******************************************************************************
//******************************************************************************
void Win32WindowProc::DeleteWindow(HWND hwnd)
{
  Win32WindowProc *window = FindProc(hwnd);

#ifdef DEBUG
  WriteLog("::DeleteWindow, destroy window/dialog %X!!\n", hwnd);
#endif
  if(window) {
        delete(window);
        return;
  }
#ifdef DEBUG
  WriteLog("::DeleteWindow, can't find window %X!!\n", hwnd);
#endif
}
//******************************************************************************
//******************************************************************************
LRESULT Win32WindowProc::SendMessageA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  PostSpyMessage(hwnd, Msg, wParam, lParam);
  return pCallback(hwnd, Msg, wParam, lParam);  
}
//******************************************************************************
//******************************************************************************
DWORD MapOEMToRealKey(DWORD wParam, DWORD lParam)
{
  switch(wParam) {
        case VK_PRIOR:  //page up
                lParam &= 0xFF00FFFF;
                lParam |= 0x00510000;
                break;
        case VK_NEXT:   //page down
                lParam &= 0xFF00FFFF;
                lParam |= 0x00490000;
                break;
        case VK_END:
                lParam &= 0xFF00FFFF;
                lParam |= 0x004F0000;
                break;
        case VK_HOME:
                lParam &= 0xFF00FFFF;
                lParam |= 0x00470000;
                break;
        case VK_UP:
                lParam &= 0xFF00FFFF;
                lParam |= 0x00480000;
                break;
        case VK_LEFT:
                lParam &= 0xFF00FFFF;
                lParam |= 0x004B0000;
                break;
        case VK_DOWN:
                lParam &= 0xFF00FFFF;
                lParam |= 0x00500000;
                break;
        case VK_RIGHT:
                lParam &= 0xFF00FFFF;
                lParam |= 0x004D0000;
                break;
        case VK_DELETE:
                lParam &= 0xFF00FFFF;
                lParam |= 0x00530000;
                break;
        case VK_INSERT:
                lParam &= 0xFF00FFFF;
                lParam |= 0x00520000;
                break;
  }
  return(lParam);
}
//******************************************************************************
//#undef DEBUG
//#define DEBUG1
//******************************************************************************
LRESULT EXPENTRY_O32 WndCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
 Win32WindowProc *curwnd;
 LRESULT rc;

  //Restore our FS selector
  SetWin32TIB();

  if(Msg == WM_MOUSEACTIVATE) 
  {
	//Open32 sends an OS/2 window message for a button click
	if(HIWORD(lParam) == 0x71)  //WM_BUTTONCLICKFIRST
	{
		lParam = (WM_LBUTTONDOWN << 16) | LOWORD(lParam);
	}
  }

  if(PostSpyMessage(hwnd, Msg, wParam, lParam) == FALSE)
       	dprintf(("Message %s for %X %x %x\n", GetMsgText(Msg), hwnd, wParam, lParam));

  if(HkCBT::OS2HkCBTProc(hwnd, Msg, wParam, lParam) == TRUE) {//hook swallowed msg
	RestoreOS2TIB();
        return(0);
  }
 
  curwnd = Win32WindowProc::FindProc(hwnd);
  if(!curwnd) {
	curwnd = Win32WindowProc::FindProc(0, GetCurrentThreadId());
	if(curwnd)	curwnd->SetWindowHandle(hwnd);
  }
  if(curwnd != NULL) {
	switch(Msg) 
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	        //SvL: Correct Open32 key mapping bug
                lParam = MapOEMToRealKey(wParam, lParam);
		break;
	case WM_CREATE:  //Open32 isn't sending WM_NCCREATE messages!!
                if(curwnd->SendMessageA(hwnd, WM_NCCREATE, 0, lParam) == 0) {
                       	dprintf(("WM_NCCREATE returned FALSE\n"));
			RestoreOS2TIB();
                       	return(-1); //don't create window
                }
	
		NotifyParent(hwnd, WM_CREATE, wParam, lParam);
//TODO
#if 0
                if(curwnd->SendMessageA(hwnd, WM_NCCALCSIZE, 0, lParam) == 0) {
			RestoreOS2TIB();
                        return(-1); //don't create window
                }
#endif
		break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_DESTROY: //nofity parent
		NotifyParent(hwnd, Msg, wParam, lParam);	
		break;

	case WM_ACTIVATE:
		if(LOWORD(wParam) != WA_INACTIVE)
        	{//SvL: Bugfix, Open32 is NOT sending this to the window (messes up Solitaire)
          	  HDC hdc = GetDC(hwnd);

                 	curwnd->SendMessageA(hwnd, WM_ERASEBKGND, hdc, 0);
                 	ReleaseDC(hwnd, hdc);
        	}
		break;
	}

        rc = curwnd->pCallback(hwnd, Msg, wParam, lParam);
        if(Msg == WM_NCDESTROY) {
                dprintf(("WM_NCDESTROY received for window/dialog %X\n", curwnd->hwnd));
                delete curwnd;
        }
	RestoreOS2TIB();
        return rc;
  }

  //Could be a dialog control using a registered class, so check this:
  char                  szClass[128];
  Win32WindowClass     *wclass;
  Win32WindowProc      *wnd;

  if(GetClassNameA(hwnd, szClass, sizeof(szClass))) {
        wclass = Win32WindowClass::FindClass(szClass);
        if(wclass) {
                wnd = new Win32WindowProc(wclass->GetClassCallback(szClass));
                wnd->SetWindowHandle(hwnd);
                rc = WndCallback(hwnd, Msg, wParam, lParam);
		RestoreOS2TIB();
		return rc;
        }
  }
  dprintf(("wnd Callback, can't find window %X %d!!!!\n", hwnd, Msg));
  RestoreOS2TIB();
  return 0;
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
typedef struct
{
    LPSTR pszMsg;
    UINT msg;
    UINT Flags;
} MSGDESC, *PMSGDESC;

//
// Message Table Flags
//
#define MTF_SELECTED                0x00000001

#define MTF_TYPE_MOUSE              0x00010000
#define MTF_TYPE_DDE                0x00020000
#define MTF_TYPE_CLIP               0x00040000
#define MTF_TYPE_NC                 0x00080000
#define MTF_TYPE_KEYBD              0x00100000
#define MTF_TYPE_LB                 0x00200000
#define MTF_TYPE_BM                 0x00400000
#define MTF_TYPE_STM                0x00800000
#define MTF_TYPE_CB                 0x01000000
#define MTF_TYPE_EM                 0x02000000
#ifdef FE_IME
#define MTF_TYPE_IME                0x04000000
#endif

//
// Message description table.  Describes each message that can be spied on.
// This table must be kept in sorted order.
//
MSGDESC gaMsgs[] =
{
    { "WM_NULL", WM_NULL,                                   // 0x0000
        0},
    { "WM_CREATE", WM_CREATE,                               // 0x0001
        0},
    { "WM_DESTROY", WM_DESTROY,                             // 0x0002
        0},
    { "WM_MOVE", WM_MOVE,                                   // 0x0003
        0},
    { "WM_SIZE", WM_SIZE,                                   // 0x0005
        0},
    { "WM_ACTIVATE", WM_ACTIVATE,                           // 0x0006
        0},
    { "WM_SETFOCUS", WM_SETFOCUS,                           // 0x0007
        0},
    { "WM_KILLFOCUS", WM_KILLFOCUS,                         // 0x0008
        0},
    { "WM_ENABLE", WM_ENABLE,                               // 0x000A
        0},
    { "WM_SETREDRAW", WM_SETREDRAW,                         // 0x000B
        0},
    { "WM_SETTEXT", WM_SETTEXT,                             // 0x000C
        0},
    { "WM_GETTEXT", WM_GETTEXT,                             // 0x000D
        0},
    { "WM_GETTEXTLENGTH", WM_GETTEXTLENGTH,                 // 0x000E
        0},
    { "WM_PAINT", WM_PAINT,                                 // 0x000F
        0},
    { "WM_CLOSE", WM_CLOSE,                                 // 0x0010
        0},
    { "WM_QUERYENDSESSION", WM_QUERYENDSESSION,             // 0x0011
        0},
    { "WM_QUIT", WM_QUIT,                                   // 0x0012
        0},
    { "WM_QUERYOPEN", WM_QUERYOPEN,                         // 0x0013
        0},
    { "WM_ERASEBKGND", WM_ERASEBKGND,                       // 0x0014
        0},
    { "WM_SYSCOLORCHANGE", WM_SYSCOLORCHANGE,               // 0x0015
        0},
    { "WM_ENDSESSION", WM_ENDSESSION,                       // 0x0016
        0},
    { "WM_SHOWWINDOW", WM_SHOWWINDOW,                       // 0x0018
        0},
    { "WM_WININICHANGE", WM_WININICHANGE,                   // 0x001A
        0},
    { "WM_DEVMODECHANGE", WM_DEVMODECHANGE,                 // 0x001B
        0},
    { "WM_ACTIVATEAPP", WM_ACTIVATEAPP,                     // 0x001C
        0},
    { "WM_FONTCHANGE", WM_FONTCHANGE,                       // 0x001D
        0},
    { "WM_TIMECHANGE", WM_TIMECHANGE,                       // 0x001E
        0},
    { "WM_CANCELMODE", WM_CANCELMODE,                       // 0x001F
        0},
    { "WM_SETCURSOR", WM_SETCURSOR,                         // 0x0020
        MTF_TYPE_MOUSE},
    { "WM_MOUSEACTIVATE", WM_MOUSEACTIVATE,                 // 0x0021
        MTF_TYPE_MOUSE},
    { "WM_CHILDACTIVATE", WM_CHILDACTIVATE,                 // 0x0022
        0},
    { "WM_QUEUESYNC", WM_QUEUESYNC,                         // 0x0023
        0},
    { "WM_GETMINMAXINFO", WM_GETMINMAXINFO,                 // 0x0024
        0},
    { "WM_PAINTICON", WM_PAINTICON,                         // 0x0026
        0},
    { "WM_ICONERASEBKGND", WM_ICONERASEBKGND,               // 0x0027
        0},
    { "WM_NEXTDLGCTL", WM_NEXTDLGCTL,                       // 0x0028
        0},
    { "WM_SPOOLERSTATUS", WM_SPOOLERSTATUS,                 // 0x002A
        0},
    { "WM_DRAWITEM", WM_DRAWITEM,                           // 0x002B
        0},
    { "WM_MEASUREITEM", WM_MEASUREITEM,                     // 0x002C
        0},
    { "WM_DELETEITEM", WM_DELETEITEM,                       // 0x002D
        0},
    { "WM_VKEYTOITEM", WM_VKEYTOITEM,                       // 0x002E
        MTF_TYPE_KEYBD},
    { "WM_CHARTOITEM", WM_CHARTOITEM,                       // 0x002F
        MTF_TYPE_KEYBD},
    { "WM_SETFONT", WM_SETFONT,                             // 0x0030
        0},
    { "WM_GETFONT", WM_GETFONT,                             // 0x0031
        0},
    { "WM_SETHOTKEY", WM_SETHOTKEY,                         // 0x0032
        MTF_TYPE_KEYBD},
    { "WM_GETHOTKEY", WM_GETHOTKEY,                         // 0x0033
        MTF_TYPE_KEYBD},
    { "WM_QUERYDRAGICON", WM_QUERYDRAGICON,                 // 0x0037
        0},
    { "WM_COMPAREITEM", WM_COMPAREITEM,                     // 0x0039
        0},
    { "WM_COMPACTING", WM_COMPACTING,                       // 0x0041
        0},
    { "WM_WINDOWPOSCHANGING", WM_WINDOWPOSCHANGING,         // 0x0046
        0},
    { "WM_WINDOWPOSCHANGED", WM_WINDOWPOSCHANGED,           // 0x0047
        0},
    { "WM_POWER", WM_POWER,                                 // 0x0048
        0},
    { "WM_COPYDATA", WM_COPYDATA,                           // 0x004A
        0},
    { "WM_CANCELJOURNAL", WM_CANCELJOURNAL,                 // 0x004B
        0},
    { "WM_NOTIFY", WM_NOTIFY, 0},                           // 0x4E
    { "WM_INPUTLANGCHANGEREQUEST", WM_INPUTLANGCHANGEREQUEST, 0},       //0x50
    { "WM_INPUTLANGCHANGE", WM_INPUTLANGCHANGE, 0},         // 0x51
    { "WM_TCARD", WM_TCARD, 0},                             // 0x52
    { "WM_HELP", WM_HELP, 0},                               // 0x53
    { "WM_USERCHANGED", WM_USERCHANGED, 0},                 // 0x54
    { "WM_NOTIFYFORMAT", WM_NOTIFYFORMAT, 0},               // 0x55
    { "WM_NCCREATE", WM_NCCREATE,                           // 0x0081
        MTF_TYPE_NC},
    { "WM_NCDESTROY", WM_NCDESTROY,                         // 0x0082
        MTF_TYPE_NC},
    { "WM_NCCALCSIZE", WM_NCCALCSIZE,                       // 0x0083
        MTF_TYPE_NC},
    { "WM_NCHITTEST", WM_NCHITTEST,                         // 0x0084
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCPAINT", WM_NCPAINT,                             // 0x0085
        MTF_TYPE_NC},
    { "WM_NCACTIVATE", WM_NCACTIVATE,                       // 0x0086
        MTF_TYPE_NC},
    { "WM_GETDLGCODE", WM_GETDLGCODE,                       // 0x0087
        0},
    { "WM_NCMOUSEMOVE", WM_NCMOUSEMOVE,                     // 0x00A0
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCLBUTTONDOWN", WM_NCLBUTTONDOWN,                 // 0x00A1
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCLBUTTONUP", WM_NCLBUTTONUP,                     // 0x00A2
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCLBUTTONDBLCLK", WM_NCLBUTTONDBLCLK,             // 0x00A3
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCRBUTTONDOWN", WM_NCRBUTTONDOWN,                 // 0x00A4
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCRBUTTONUP", WM_NCRBUTTONUP,                     // 0x00A5
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCRBUTTONDBLCLK", WM_NCRBUTTONDBLCLK,             // 0x00A6
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCMBUTTONDOWN", WM_NCMBUTTONDOWN,                 // 0x00A7
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCMBUTTONUP", WM_NCMBUTTONUP,                     // 0x00A8
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "WM_NCMBUTTONDBLCLK", WM_NCMBUTTONDBLCLK,             // 0x00A9
        MTF_TYPE_NC | MTF_TYPE_MOUSE},
    { "EM_GETSEL", EM_GETSEL,                               // 0x00B0
        MTF_TYPE_EM},
    { "EM_SETSEL", EM_SETSEL,                               // 0x00B1
        MTF_TYPE_EM},
    { "EM_GETRECT", EM_GETRECT,                             // 0x00B2
        MTF_TYPE_EM},
    { "EM_SETRECT", EM_SETRECT,                             // 0x00B3
        MTF_TYPE_EM},
    { "EM_SETRECTNP", EM_SETRECTNP,                         // 0x00B4
        MTF_TYPE_EM},
    { "EM_SCROLL", EM_SCROLL,                               // 0x00B5
        MTF_TYPE_EM},
    { "EM_LINESCROLL", EM_LINESCROLL,                       // 0x00B6
        MTF_TYPE_EM},
    { "EM_SCROLLCARET", EM_SCROLLCARET,                     // 0x00B7
        MTF_TYPE_EM},
    { "EM_GETMODIFY", EM_GETMODIFY,                         // 0x00B8
        MTF_TYPE_EM},
    { "EM_SETMODIFY", EM_SETMODIFY,                         // 0x00B9
        MTF_TYPE_EM},
    { "EM_GETLINECOUNT", EM_GETLINECOUNT,                   // 0x00BA
        MTF_TYPE_EM},
    { "EM_LINEINDEX", EM_LINEINDEX,                         // 0x00BB
        MTF_TYPE_EM},
    { "EM_SETHANDLE", EM_SETHANDLE,                         // 0x00BC
        MTF_TYPE_EM},
    { "EM_GETHANDLE", EM_GETHANDLE,                         // 0x00BD
        MTF_TYPE_EM},
    { "EM_GETTHUMB", EM_GETTHUMB,                           // 0x00BE
        MTF_TYPE_EM},
    { "EM_LINELENGTH", EM_LINELENGTH,                       // 0x00C1
        MTF_TYPE_EM},
    { "EM_REPLACESEL", EM_REPLACESEL,                       // 0x00C2
        MTF_TYPE_EM},
    { "EM_GETLINE", EM_GETLINE,                             // 0x00C4
        MTF_TYPE_EM},
    { "EM_LIMITTEXT", EM_LIMITTEXT,                         // 0x00C5
        MTF_TYPE_EM},
    { "EM_CANUNDO", EM_CANUNDO,                             // 0x00C6
        MTF_TYPE_EM},
    { "EM_UNDO", EM_UNDO,                                   // 0x00C7
        MTF_TYPE_EM},
    { "EM_FMTLINES", EM_FMTLINES,                           // 0x00C8
        MTF_TYPE_EM},
    { "EM_LINEFROMCHAR", EM_LINEFROMCHAR,                   // 0x00C9
        MTF_TYPE_EM},
    { "EM_SETTABSTOPS", EM_SETTABSTOPS,                     // 0x00CB
        MTF_TYPE_EM},
    { "EM_SETPASSWORDCHAR", EM_SETPASSWORDCHAR,             // 0x00CC
        MTF_TYPE_EM},
    { "EM_EMPTYUNDOBUFFER", EM_EMPTYUNDOBUFFER,             // 0x00CD
        MTF_TYPE_EM},
    { "EM_GETFIRSTVISIBLELINE", EM_GETFIRSTVISIBLELINE,     // 0x00CE
        MTF_TYPE_EM},
    { "EM_SETREADONLY", EM_SETREADONLY,                     // 0x00CF
        MTF_TYPE_EM},
    { "EM_SETWORDBREAKPROC", EM_SETWORDBREAKPROC,           // 0x00D0
        MTF_TYPE_EM},
    { "EM_GETWORDBREAKPROC", EM_GETWORDBREAKPROC,           // 0x00D1
        MTF_TYPE_EM},
    { "EM_GETPASSWORDCHAR", EM_GETPASSWORDCHAR,             // 0x00D2
        MTF_TYPE_EM},
    { "SBM_SETPOS", SBM_SETPOS,                             // 0x00E0
        0},
    { "SBM_GETPOS", SBM_GETPOS,                             // 0x00E1
        0},
    { "SBM_SETRANGE", SBM_SETRANGE,                         // 0x00E2
        0},
    { "SBM_GETRANGE", SBM_GETRANGE,                         // 0x00E3
        0},
    { "SBM_ENABLE_ARROWS", SBM_ENABLE_ARROWS,               // 0x00E4
        0},
    { "SBM_SETRANGEREDRAW", SBM_SETRANGEREDRAW,             // 0x00E6
        0},
    { "BM_GETCHECK", BM_GETCHECK,                           // 0x00F0
        MTF_TYPE_BM},
    { "BM_SETCHECK", BM_SETCHECK,                           // 0x00F1
        MTF_TYPE_BM},
    { "BM_GETSTATE", BM_GETSTATE,                           // 0x00F2
        MTF_TYPE_BM},
    { "BM_SETSTATE", BM_SETSTATE,                           // 0x00F3
        MTF_TYPE_BM},
    { "BM_SETSTYLE", BM_SETSTYLE,                           // 0x00F4
        MTF_TYPE_BM},
    { "WM_KEYDOWN", WM_KEYDOWN,                             // 0x0100
        MTF_TYPE_KEYBD},
    { "WM_KEYUP", WM_KEYUP,                                 // 0x0101
        MTF_TYPE_KEYBD},
    { "WM_CHAR", WM_CHAR,                                   // 0x0102
        MTF_TYPE_KEYBD},
    { "WM_DEADCHAR", WM_DEADCHAR,                           // 0x0103
        MTF_TYPE_KEYBD},
    { "WM_SYSKEYDOWN", WM_SYSKEYDOWN,                       // 0x0104
        MTF_TYPE_KEYBD},
    { "WM_SYSKEYUP", WM_SYSKEYUP,                           // 0x0105
        MTF_TYPE_KEYBD},
    { "WM_SYSCHAR", WM_SYSCHAR,                             // 0x0106
        MTF_TYPE_KEYBD},
    { "WM_SYSDEADCHAR", WM_SYSDEADCHAR,                     // 0x0107
        MTF_TYPE_KEYBD},
#ifdef  FE_IME
    { "WM_WNT_CONVERTREQUESTEX", WM_WNT_CONVERTREQUESTEX,   // 0x0109
        MTF_TYPE_IME},
    { "WM_CONVERTREQUEST", WM_CONVERTREQUEST,               // 0x010A
        MTF_TYPE_IME},
    { "WM_CONVERTRESULT", WM_CONVERTRESULT,                 // 0x010B
        MTF_TYPE_IME},
    { "WM_INTERIM", WM_INTERIM,                             // 0x010C
        MTF_TYPE_IME},
#   define TMP_MTF_TYPE_IME     MTF_TYPE_IME
#else // not FE_IME
#   define TMP_MTF_TYPE_IME     0
#endif
    { "WM_IME_STARTCOMPOSITION", WM_IME_STARTCOMPOSITION,   // 0x010D
        TMP_MTF_TYPE_IME},
    { "WM_IME_ENDCOMPOSITION",   WM_IME_ENDCOMPOSITION,     // 0x010E
        TMP_MTF_TYPE_IME},
    { "WM_IME_COMPOSITION",      WM_IME_COMPOSITION,        // 0x010F
        TMP_MTF_TYPE_IME},
    { "WM_INITDIALOG", WM_INITDIALOG,                       // 0x0110
        0},
    { "WM_COMMAND", WM_COMMAND,                             // 0x0111
        0},
    { "WM_SYSCOMMAND", WM_SYSCOMMAND,                       // 0x0112
        0},
    { "WM_TIMER", WM_TIMER,                                 // 0x0113
        0},
    { "WM_HSCROLL", WM_HSCROLL,                             // 0x0114
        0},
    { "WM_VSCROLL", WM_VSCROLL,                             // 0x0115
        0},
    { "WM_INITMENU", WM_INITMENU,                           // 0x0116
        0},
    { "WM_INITMENUPOPUP", WM_INITMENUPOPUP,                 // 0x0117
        0},
    { "WM_MENUSELECT", WM_MENUSELECT,                       // 0x011F
        0},
    { "WM_MENUCHAR", WM_MENUCHAR,                           // 0x0120
        0},
    { "WM_ENTERIDLE", WM_ENTERIDLE,                         // 0x0121
        0},
    { "WM_CTLCOLORMSGBOX", WM_CTLCOLORMSGBOX,               // 0x0132
        0},
    { "WM_CTLCOLOREDIT", WM_CTLCOLOREDIT,                   // 0x0133
        0},
    { "WM_CTLCOLORLISTBOX", WM_CTLCOLORLISTBOX,             // 0x0134
        0},
    { "WM_CTLCOLORBTN", WM_CTLCOLORBTN,                     // 0x0135
        0},
    { "WM_CTLCOLORDLG", WM_CTLCOLORDLG,                     // 0x0136
        0},
    { "WM_CTLCOLORSCROLLBAR", WM_CTLCOLORSCROLLBAR,         // 0x0137
        0},
    { "WM_CTLCOLORSTATIC", WM_CTLCOLORSTATIC,               // 0x0138
        0},
    { "CB_GETEDITSEL", CB_GETEDITSEL,                       // 0x0140
        MTF_TYPE_CB},
    { "CB_LIMITTEXT", CB_LIMITTEXT,                         // 0x0141
        MTF_TYPE_CB},
    { "CB_SETEDITSEL", CB_SETEDITSEL,                       // 0x0142
        MTF_TYPE_CB},
    { "CB_ADDSTRING", CB_ADDSTRING,                         // 0x0143
        MTF_TYPE_CB},
    { "CB_DELETESTRING", CB_DELETESTRING,                   // 0x0144
        MTF_TYPE_CB},
    { "CB_DIR", CB_DIR,                                     // 0x0145
        MTF_TYPE_CB},
    { "CB_GETCOUNT", CB_GETCOUNT,                           // 0x0146
        MTF_TYPE_CB},
    { "CB_GETCURSEL", CB_GETCURSEL,                         // 0x0147
        MTF_TYPE_CB},
    { "CB_GETLBTEXT", CB_GETLBTEXT,                         // 0x0148
        MTF_TYPE_CB},
    { "CB_GETLBTEXTLEN", CB_GETLBTEXTLEN,                   // 0x0149
        MTF_TYPE_CB},
    { "CB_INSERTSTRING", CB_INSERTSTRING,                   // 0x014A
        MTF_TYPE_CB},
    { "CB_RESETCONTENT", CB_RESETCONTENT,                   // 0x014B
        MTF_TYPE_CB},
    { "CB_FINDSTRING", CB_FINDSTRING,                       // 0x014C
        MTF_TYPE_CB},
    { "CB_SELECTSTRING", CB_SELECTSTRING,                   // 0x014D
        MTF_TYPE_CB},
    { "CB_SETCURSEL", CB_SETCURSEL,                         // 0x014E
        MTF_TYPE_CB},
    { "CB_SHOWDROPDOWN", CB_SHOWDROPDOWN,                   // 0x014F
        MTF_TYPE_CB},
    { "CB_GETITEMDATA", CB_GETITEMDATA,                     // 0x0150
        MTF_TYPE_CB},
    { "CB_SETITEMDATA", CB_SETITEMDATA,                     // 0x0151
        MTF_TYPE_CB},
    { "CB_GETDROPPEDCONTROLRECT", CB_GETDROPPEDCONTROLRECT, // 0x0152
        MTF_TYPE_CB},
    { "CB_SETITEMHEIGHT", CB_SETITEMHEIGHT,                 // 0x0153
        MTF_TYPE_CB},
    { "CB_GETITEMHEIGHT", CB_GETITEMHEIGHT,                 // 0x0154
        MTF_TYPE_CB},
    { "CB_SETEXTENDEDUI", CB_SETEXTENDEDUI,                 // 0x0155
        MTF_TYPE_CB},
    { "CB_GETEXTENDEDUI", CB_GETEXTENDEDUI,                 // 0x0156
        MTF_TYPE_CB},
    { "CB_GETDROPPEDSTATE", CB_GETDROPPEDSTATE,             // 0x0157
        MTF_TYPE_CB},
    { "CB_FINDSTRINGEXACT", CB_FINDSTRINGEXACT,             // 0x0158
        MTF_TYPE_CB},
    { "CB_SETLOCALE", CB_SETLOCALE,                         // 0x0159
        MTF_TYPE_CB},
    { "CB_GETLOCALE", CB_GETLOCALE,                         // 0x015A
        MTF_TYPE_CB},
    { "STM_SETICON", STM_SETICON,                           // 0x0170
        MTF_TYPE_STM},
    { "STM_GETICON", STM_GETICON,                           // 0x0171
        MTF_TYPE_STM},
    { "LB_ADDSTRING", LB_ADDSTRING,                         // 0x0180
        MTF_TYPE_LB},
    { "LB_INSERTSTRING", LB_INSERTSTRING,                   // 0x0181
        MTF_TYPE_LB},
    { "LB_DELETESTRING", LB_DELETESTRING,                   // 0x0182
        MTF_TYPE_LB},
    { "LB_SELITEMRANGEEX", LB_SELITEMRANGEEX,               // 0x0183
        MTF_TYPE_LB},
    { "LB_RESETCONTENT", LB_RESETCONTENT,                   // 0x0184
        MTF_TYPE_LB},
    { "LB_SETSEL", LB_SETSEL,                               // 0x0185
        MTF_TYPE_LB},
    { "LB_SETCURSEL", LB_SETCURSEL,                         // 0x0186
        MTF_TYPE_LB},
    { "LB_GETSEL", LB_GETSEL,                               // 0x0187
        MTF_TYPE_LB},
    { "LB_GETCURSEL", LB_GETCURSEL,                         // 0x0188
        MTF_TYPE_LB},
    { "LB_GETTEXT", LB_GETTEXT,                             // 0x0189
        MTF_TYPE_LB},
    { "LB_GETTEXTLEN", LB_GETTEXTLEN,                       // 0x018A
        MTF_TYPE_LB},
    { "LB_GETCOUNT", LB_GETCOUNT,                           // 0x018B
        MTF_TYPE_LB},
    { "LB_SELECTSTRING", LB_SELECTSTRING,                   // 0x018C
        MTF_TYPE_LB},
    { "LB_DIR", LB_DIR,                                     // 0x018D
        MTF_TYPE_LB},
    { "LB_GETTOPINDEX", LB_GETTOPINDEX,                     // 0x018E
        MTF_TYPE_LB},
    { "LB_FINDSTRING", LB_FINDSTRING,                       // 0x018F
        MTF_TYPE_LB},
    { "LB_GETSELCOUNT", LB_GETSELCOUNT,                     // 0x0190
        MTF_TYPE_LB},
    { "LB_GETSELITEMS", LB_GETSELITEMS,                     // 0x0191
        MTF_TYPE_LB},
    { "LB_SETTABSTOPS", LB_SETTABSTOPS,                     // 0x0192
        MTF_TYPE_LB},
    { "LB_GETHORIZONTALEXTENT", LB_GETHORIZONTALEXTENT,     // 0x0193
        MTF_TYPE_LB},
    { "LB_SETHORIZONTALEXTENT", LB_SETHORIZONTALEXTENT,     // 0x0194
        MTF_TYPE_LB},
    { "LB_SETCOLUMNWIDTH", LB_SETCOLUMNWIDTH,               // 0x0195
        MTF_TYPE_LB},
    { "LB_ADDFILE", LB_ADDFILE,                             // 0x0196
        MTF_TYPE_LB},
    { "LB_SETTOPINDEX", LB_SETTOPINDEX,                     // 0x0197
        MTF_TYPE_LB},
    { "LB_GETITEMRECT", LB_GETITEMRECT,                     // 0x0198
        MTF_TYPE_LB},
    { "LB_GETITEMDATA", LB_GETITEMDATA,                     // 0x0199
        MTF_TYPE_LB},
    { "LB_SETITEMDATA", LB_SETITEMDATA,                     // 0x019A
        MTF_TYPE_LB},
    { "LB_SELITEMRANGE", LB_SELITEMRANGE,                   // 0x019B
        MTF_TYPE_LB},
    { "LB_SETANCHORINDEX", LB_SETANCHORINDEX,               // 0x019C
        MTF_TYPE_LB},
    { "LB_GETANCHORINDEX", LB_GETANCHORINDEX,               // 0x019D
        MTF_TYPE_LB},
    { "LB_SETCARETINDEX", LB_SETCARETINDEX,                 // 0x019E
        MTF_TYPE_LB},
    { "LB_GETCARETINDEX", LB_GETCARETINDEX,                 // 0x019F
        MTF_TYPE_LB},
    { "LB_SETITEMHEIGHT", LB_SETITEMHEIGHT,                 // 0x01A0
        MTF_TYPE_LB},
    { "LB_GETITEMHEIGHT", LB_GETITEMHEIGHT,                 // 0x01A1
        MTF_TYPE_LB},
    { "LB_FINDSTRINGEXACT", LB_FINDSTRINGEXACT,             // 0x01A2
        MTF_TYPE_LB},
    { "LB_SETLOCALE", LB_SETLOCALE,                         // 0x01A5
        MTF_TYPE_LB},
    { "LB_GETLOCALE", LB_GETLOCALE,                         // 0x01A6
        MTF_TYPE_LB},
    { "LB_SETCOUNT", LB_SETCOUNT,                           // 0x01A7
        MTF_TYPE_LB},
    { "WM_MOUSEMOVE", WM_MOUSEMOVE,                         // 0x0200
        MTF_TYPE_MOUSE},
    { "WM_LBUTTONDOWN", WM_LBUTTONDOWN,                     // 0x0201
        MTF_TYPE_MOUSE},
    { "WM_LBUTTONUP", WM_LBUTTONUP,                         // 0x0202
        MTF_TYPE_MOUSE},
    { "WM_LBUTTONDBLCLK", WM_LBUTTONDBLCLK,                 // 0x0203
        MTF_TYPE_MOUSE},
    { "WM_RBUTTONDOWN", WM_RBUTTONDOWN,                     // 0x0204
        MTF_TYPE_MOUSE},
    { "WM_RBUTTONUP", WM_RBUTTONUP,                         // 0x0205
        MTF_TYPE_MOUSE},
    { "WM_RBUTTONDBLCLK", WM_RBUTTONDBLCLK,                 // 0x0206
        MTF_TYPE_MOUSE},
    { "WM_MBUTTONDOWN", WM_MBUTTONDOWN,                     // 0x0207
        MTF_TYPE_MOUSE},
    { "WM_MBUTTONUP", WM_MBUTTONUP,                         // 0x0208
        MTF_TYPE_MOUSE},
    { "WM_MBUTTONDBLCLK", WM_MBUTTONDBLCLK,                 // 0x0209
        MTF_TYPE_MOUSE},
    { "WM_PARENTNOTIFY", WM_PARENTNOTIFY,                   // 0x0210
        MTF_TYPE_MOUSE},
    { "WM_ENTERMENULOOP", WM_ENTERMENULOOP,                 // 0x0211
        0},
    { "WM_EXITMENULOOP", WM_EXITMENULOOP,                   // 0x0212
        0},
    { "WM_MDICREATE", WM_MDICREATE,                         // 0x0220
        0},
    { "WM_MDIDESTROY", WM_MDIDESTROY,                       // 0x0221
        0},
    { "WM_MDIACTIVATE", WM_MDIACTIVATE,                     // 0x0222
        0},
    { "WM_MDIRESTORE", WM_MDIRESTORE,                       // 0x0223
        0},
    { "WM_MDINEXT", WM_MDINEXT,                             // 0x0224
        0},
    { "WM_MDIMAXIMIZE", WM_MDIMAXIMIZE,                     // 0x0225
        0},
    { "WM_MDITILE", WM_MDITILE,                             // 0x0226
        0},
    { "WM_MDICASCADE", WM_MDICASCADE,                       // 0x0227
        0},
    { "WM_MDIICONARRANGE", WM_MDIICONARRANGE,               // 0x0228
        0},
    { "WM_MDIGETACTIVE", WM_MDIGETACTIVE,                   // 0x0229
        0},
    { "WM_MDISETMENU", WM_MDISETMENU,                       // 0x0230
        0},
    { "WM_ENTERSIZEMOVE", WM_ENTERSIZEMOVE,                 // 0x0231
        0},
    { "WM_EXITSIZEMOVE", WM_EXITSIZEMOVE,                   // 0x0232
        0},
    { "WM_DROPFILES", WM_DROPFILES,                         // 0x0233
        0},
    { "WM_MDIREFRESHMENU", WM_MDIREFRESHMENU,               // 0x0234
        0},
#ifdef  FE_IME
    { "WM_IME_REPORT", WM_IME_REPORT,                       // 0x0280
        MTF_TYPE_IME},
#endif
    { "WM_IME_SETCONTEXT",      WM_IME_SETCONTEXT,          // 0x0281
        TMP_MTF_TYPE_IME},
    { "WM_IME_NOTIFY",          WM_IME_NOTIFY,              // 0x0282
        TMP_MTF_TYPE_IME},
    { "WM_IME_CONTROL",         WM_IME_CONTROL,             // 0x0283
        TMP_MTF_TYPE_IME},
    { "WM_IME_COMPOSITIONFULL", WM_IME_COMPOSITIONFULL,     // 0x0284
        TMP_MTF_TYPE_IME},
    { "WM_IME_SELECT",          WM_IME_SELECT,              // 0x0285
        TMP_MTF_TYPE_IME},
    { "WM_IME_CHAR",            WM_IME_CHAR,                // 0x0286
        TMP_MTF_TYPE_IME},
#ifdef  FE_IME
    { "WM_IMEKEYDOWN", WM_IMEKEYDOWN,                       // 0x0290
        MTF_TYPE_IME},
    { "WM_IMEKEYUP", WM_IMEKEYUP,                           // 0x0291
        MTF_TYPE_IME},
#endif
    { "WM_CUT", WM_CUT,                                     // 0x0300
        MTF_TYPE_CLIP},
    { "WM_COPY", WM_COPY,                                   // 0x0301
        MTF_TYPE_CLIP},
    { "WM_PASTE", WM_PASTE,                                 // 0x0302
        MTF_TYPE_CLIP},
    { "WM_CLEAR", WM_CLEAR,                                 // 0x0303
        MTF_TYPE_CLIP},
    { "WM_UNDO", WM_UNDO,                                   // 0x0304
        MTF_TYPE_CLIP},
    { "WM_RENDERFORMAT", WM_RENDERFORMAT,                   // 0x0305
        MTF_TYPE_CLIP},
    { "WM_RENDERALLFORMATS", WM_RENDERALLFORMATS,           // 0x0306
        MTF_TYPE_CLIP},
    { "WM_DESTROYCLIPBOARD", WM_DESTROYCLIPBOARD,           // 0x0307
        MTF_TYPE_CLIP},
    { "WM_DRAWCLIPBOARD", WM_DRAWCLIPBOARD,                 // 0x0308
        MTF_TYPE_CLIP},
    { "WM_PAINTCLIPBOARD", WM_PAINTCLIPBOARD,               // 0x0309
        MTF_TYPE_CLIP},
    { "WM_VSCROLLCLIPBOARD", WM_VSCROLLCLIPBOARD,           // 0x030A
        MTF_TYPE_CLIP},
    { "WM_SIZECLIPBOARD", WM_SIZECLIPBOARD,                 // 0x030B
        MTF_TYPE_CLIP},
    { "WM_ASKCBFORMATNAME", WM_ASKCBFORMATNAME,             // 0x030C
        MTF_TYPE_CLIP},
    { "WM_CHANGECBCHAIN", WM_CHANGECBCHAIN,                 // 0x030D
        MTF_TYPE_CLIP},
    { "WM_HSCROLLCLIPBOARD", WM_HSCROLLCLIPBOARD,           // 0x030E
        MTF_TYPE_CLIP},
    { "WM_QUERYNEWPALETTE", WM_QUERYNEWPALETTE,             // 0x030F
        0},
    { "WM_PALETTEISCHANGING", WM_PALETTEISCHANGING,         // 0x0310
        0},
    { "WM_PALETTECHANGED", WM_PALETTECHANGED,               // 0x0311
        0},
    { "WM_HOTKEY", WM_HOTKEY,                               // 0x0312
        MTF_TYPE_KEYBD},
    { "WM_DDE_INITIATE", WM_DDE_INITIATE,                   // 0x03E0
        MTF_TYPE_DDE},
    { "WM_DDE_TERMINATE", WM_DDE_TERMINATE,                 // 0x03E1
        MTF_TYPE_DDE},
    { "WM_DDE_ADVISE", WM_DDE_ADVISE,                       // 0x03E2
        MTF_TYPE_DDE},
    { "WM_DDE_UNADVISE", WM_DDE_UNADVISE,                   // 0x03E3
        MTF_TYPE_DDE},
    { "WM_DDE_ACK", WM_DDE_ACK,                             // 0x03E4
        MTF_TYPE_DDE},
    { "WM_DDE_DATA", WM_DDE_DATA,                           // 0x03E5
        MTF_TYPE_DDE},
    { "WM_DDE_REQUEST", WM_DDE_REQUEST,                     // 0x03E6
        MTF_TYPE_DDE},
    { "WM_DDE_POKE", WM_DDE_POKE,                           // 0x03E7
        MTF_TYPE_DDE},
    { "WM_DDE_EXECUTE", WM_DDE_EXECUTE,                     // 0x03E8
        MTF_TYPE_DDE}
};

INT gcMessages = sizeof(gaMsgs) / sizeof(MSGDESC);


char *GetMsgText(int Msg)
{
 static char msgtxt[64];
 int i;

  for(i=0;i<gcMessages;i++) {
        if(gaMsgs[i].msg == Msg)
                return(gaMsgs[i].pszMsg);
  }
  sprintf(msgtxt, "%s %X ", "Unknown Message ", Msg);
  return(msgtxt);
}
#endif
//******************************************************************************
//******************************************************************************
Win32WindowProc *Win32WindowProc::windows = NULL;
