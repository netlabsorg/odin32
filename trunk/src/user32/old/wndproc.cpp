/* $Id: wndproc.cpp,v 1.1 1999-09-15 23:04:30 sandervl Exp $ */

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
#include "wndmsg.h"

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
                        : hwnd(0), next(NULL), os2dlg(NULL), win32class(0), pOS2Callback(NULL)
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
Win32WindowProc::Win32WindowProc(WNDPROC pUserCallback, WNDPROC_O32 pOS2Callback)
                        : hwnd(0), next(NULL), os2dlg(NULL), win32class(0), pOS2Callback(NULL)
{
  //Insert it in front of the rest
  next       = windows;
  windows    = this;
  threadid   = (DWORD)GetCurrentThreadId();

  pCallback  = pUserCallback;	//can be NULL (to be set in SetWindowLong call)
  fIsWindow  = TRUE;
  if(pOS2Callback == NULL) {
	dprintf(("Win32WindowProc ctor: pOS2Callback == NULL"));
	DebugInt3();
  }
  this->pOS2Callback = pOS2Callback;
}
//******************************************************************************
//******************************************************************************
Win32WindowProc::Win32WindowProc(WNDPROC pUserCallback, DLGTEMPLATE *os2dlg)
                        : hwnd(0), next(NULL), os2dlg(NULL), win32class(0), pOS2Callback(NULL)
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
                        : hwnd(0), next(NULL), os2dlg(NULL), pOS2Callback(NULL)
{
  WNDCLASSA wc;
  BOOL rc;

  rc = GetClassInfoA(hinst, lpszClassName, &wc);
  if (rc == FALSE)
  {
    dprintf (("USER32:WNDPROC: GetClassInfoA(%s) failed.",
              lpszClassName));
  }
  else
  {
    //  pCallback = Win32WindowClass::GetClassCallback(hinst, (LPSTR)wc.lpszClassName);
    pCallback = Win32WindowClass::GetClassCallback((LPSTR)wc.lpszClassName);
    if(pCallback == NULL) //system class
        pCallback = (WNDPROC)wc.lpfnWndProc;
  }

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
  return(OS2ToWin32Callback);
}
//******************************************************************************
//******************************************************************************
WNDPROC Win32WindowProc::GetWin32ToOS2Callback()
{
  return(Win32ToOS2Callback);
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
  switch((UCHAR)(lParam >> 16)) {
        case 0x60: // VK_HOME
          lParam &= 0xFF00FFFF;
          lParam |= 0x00470000;
          break;
        case 0x61: // VK_UP
          lParam &= 0xFF00FFFF;
          lParam |= 0x00480000;
          break;
        case 0x62: // VK_PRIOR  //page up
          lParam &= 0xFF00FFFF;
          lParam |= 0x00490000;
          break;
        case 0x63: // VK_LEFT
          lParam &= 0xFF00FFFF;
          lParam |= 0x004B0000;
          break;
        case 0x64: // VK_RIGHT
          lParam &= 0xFF00FFFF;
          lParam |= 0x004D0000;
          break;
        case 0x65: // VK_END
          lParam &= 0xFF00FFFF;
          lParam |= 0x004F0000;
          break;
        case 0x66: // VK_DOWN:
          lParam &= 0xFF00FFFF;
          lParam |= 0x00500000;
          break;
        case 0x67: // VK_NEXT  //page down
          lParam &= 0xFF00FFFF;
          lParam |= 0x00510000;
          break;
        case 0x68: // VK_INSERT
          lParam &= 0xFF00FFFF;
          lParam |= 0x00520000;
          break;
        case 0x69: // VK_DELETE
          lParam &= 0xFF00FFFF;
          lParam |= 0x00530000;
          break;
  }
  return(lParam);
}
//******************************************************************************
//#undef DEBUG
//#define DEBUG1
//******************************************************************************
LRESULT EXPENTRY_O32 OS2ToWin32Callback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
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
                rc = OS2ToWin32Callback(hwnd, Msg, wParam, lParam);
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
LRESULT EXPENTRY Win32ToOS2Callback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;

  Win32WindowProc *curwnd = Win32WindowProc::FindProc(hwnd);
  if(curwnd && curwnd->pOS2Callback) {
	RestoreOS2TIB();	
	rc = curwnd->pOS2Callback(hwnd, Msg, wParam, lParam);
	SetWin32TIB();
	return rc;	
  }
  else 	DebugInt3();

  return 0;
}
//******************************************************************************
//******************************************************************************
Win32WindowProc *Win32WindowProc::windows = NULL;
