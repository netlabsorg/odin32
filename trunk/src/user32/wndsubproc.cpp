/* $Id: wndsubproc.cpp,v 1.1 1999-05-24 20:20:00 ktk Exp $ */

/*
 * Win32 window subproc class for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <assert.h>
#include "misc.h"
#include "user32.h"
#include "wndsubproc.h"

static LRESULT WIN32API SubWndCallback(HWND, UINT, WPARAM, LPARAM);

//******************************************************************************
//******************************************************************************
Win32WindowSubProc::Win32WindowSubProc(HWND hwnd, WNDPROC_O32 pOpen32Callback)
{
#ifdef DEBUG
  WriteLog("ctr Win32WindowSubProc, %X %X\n", hwnd, pOpen32Callback);
#endif
  //Insert it in front of the rest
  next       = windows;
  windows    = this;

  pCallback  = pOpen32Callback;
  this->hwnd = hwnd;
}
//******************************************************************************
//******************************************************************************
Win32WindowSubProc::~Win32WindowSubProc()
{
  Win32WindowSubProc *window = Win32WindowSubProc::windows;

  if(window == this) {
	windows = next;
  }
  else {
	while(window->next != NULL) {
		if(window->next == this) {
			window->next = next;
			break;
		}
		window = window->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
WNDPROC Win32WindowSubProc::GetWin32Callback()
{
  return(SubWndCallback);
}
//******************************************************************************
//******************************************************************************
Win32WindowSubProc *Win32WindowSubProc::FindSubProc(WNDPROC_O32 pOrgCallback)
{
  Win32WindowSubProc *window = Win32WindowSubProc::windows;

  while(window != NULL) {
	if(window->pCallback == pOrgCallback) {
		return(window);
	}
	window = window->next;
  }
#ifdef DEBUG
////  WriteLog("Win32WindowSubProc::FindSubProc, can't find callback %X!\n", pOrgCallback);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
void Win32WindowSubProc::DeleteSubWindow(HWND hwnd)
{
  Win32WindowSubProc *window = windows;

#ifdef DEBUG
  WriteLog("::DeleteSubWindow, destroy window/dialog %X!!\n", hwnd);
#endif
  while(window != NULL) {
	if(window->hwnd == hwnd) {
		delete(window);
		return;
	}
	window = window->next;
  }
#ifdef DEBUG
  WriteLog("::DeleteSubWindow, can't find window %X!!\n", hwnd);
#endif
}
//******************************************************************************
//******************************************************************************
static LRESULT WIN32API SubWndCallback(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
 Win32WindowSubProc *curwnd = Win32WindowSubProc::windows;
 LRESULT rc;

  while(curwnd != NULL) {
	if(curwnd->hwnd == hwnd) {
		rc = curwnd->pCallback(hwnd, Msg, wParam, lParam);
		if(Msg == WM_NCDESTROY) {
#ifdef DEBUG
			WriteLog("WM_NCDESTROY received for subwindow %X\n", curwnd->hwnd);
#endif
			delete curwnd;
		}
		return rc;
	}
	curwnd = curwnd->next;
  }
#ifdef DEBUG
  WriteLog("SubWndCallback, can't find window %X %d!!!!\n", hwnd, Msg);
#endif
  return 0;
}
//******************************************************************************
//******************************************************************************
Win32WindowSubProc *Win32WindowSubProc::windows = NULL;
