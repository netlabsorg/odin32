/* $Id: hooks.cpp,v 1.1 1999-07-14 08:35:34 sandervl Exp $ */

/*
 * Win32 hook class for OS/2
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
#include "hooks.h"

static LRESULT OS2HkWindowProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkCBTProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkDebugProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkMouseProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkKeyboardProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkGetMessageProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkMsgFilterProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkSysMsgFilterProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkShellProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkJrnlPlaybackProc(int code, WPARAM wParam, LPARAM lParam);
static LRESULT OS2HkJrnlRecordProc(int code, WPARAM wParam, LPARAM lParam);

//******************************************************************************
//HkWindow class
//******************************************************************************
HkWindow::HkWindow(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkWindow::~HkWindow()
{
 HkWindow *hook = HkWindow::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkWindow::GetOS2Hook()
{
  return(OS2HkWindowProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkWindow::UnHookWindowsHook(HHOOK hhook)
{
 HkWindow *hook = HkWindow::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkWindow::FindHookProc(HOOKPROC hkprc)
{
 HkWindow *hook = HkWindow::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkWindow::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkWindowProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkWindow *hook = HkWindow::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkWindow *HkWindow::hooks = NULL;

//******************************************************************************
//HkCBT class
//******************************************************************************
HkCBT::HkCBT(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = (HHOOK)this;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkCBT::~HkCBT()
{
 HkCBT *hook = HkCBT::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkCBT::GetOS2Hook()
{ //should never be called (Open32 CBT hook is broken)
  _interrupt(3);
  return 0;
//  return(OS2HkCBTProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkCBT::UnHookWindowsHook(HHOOK hhook)
{
 HkCBT *hook = HkCBT::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkCBT::FindHookProc(HOOKPROC hkprc)
{
 HkCBT *hook = HkCBT::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkCBT::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
LRESULT HkCBT::OS2HkCBTProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
 HkCBT *hook = HkCBT::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			//Save these for possible call to CallNextHookEx
			hook->orghwnd   = hwnd;
			hook->orgMsg    = Msg;
			hook->orgwParam = wParam;
			hook->orglParam = lParam;
			rc = hook->CBTCallback(hwnd, Msg, wParam, lParam);

			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
#ifdef DEBUG1
  WriteLog("HkCBT: No hooks found!\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
LRESULT HkCBT::CBTCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 CBT_CREATEWNDA    createwnd;
 CREATESTRUCTA     dlgcs = {0};
 CBTACTIVATESTRUCT activate;
 HWND              active, inactive;
 RECT              rect;

  switch(message) {
	case WM_CREATE:
	case WM_NCCREATE:
#ifdef DEBUG
		WriteLog("HkCBT: WM_CREATE\n");
#endif
		createwnd.lpcs            = (CREATESTRUCTA *)lParam;
		createwnd.hwndInsertAfter = createwnd.lpcs->hwndParent; //TODO: not correct
		return(pCallback(HCBT_CREATEWND, hwnd, (LPARAM)&createwnd));
	case WM_INITDIALOG:
#ifdef DEBUG
		WriteLog("HkCBT: WM_INITDIALOG\n");
#endif
		createwnd.lpcs            = &dlgcs;
		createwnd.hwndInsertAfter = GetParent(hwnd); //TODO: not correct
		return(pCallback(HCBT_CREATEWND, hwnd, (LPARAM)&createwnd));
	case WM_MDICREATE:
#ifdef DEBUG
		WriteLog("HkCBT: WM_MDICREATE\n");
#endif
		createwnd.lpcs            = (CREATESTRUCTA *)lParam; //==CLIENTCREATESTRUCT *
		createwnd.hwndInsertAfter = GetParent(hwnd); //TODO: not correct
		return(pCallback(HCBT_CREATEWND, hwnd, (LPARAM)&createwnd));
	case WM_DESTROY:
	case WM_NCDESTROY:
#ifdef DEBUG
		WriteLog("HkCBT: WM_DESTROY\n");
#endif
		return(pCallback(HCBT_DESTROYWND, hwnd, 0));
	case WM_ACTIVATE:
	case WM_NCACTIVATE:
#ifdef DEBUG
		WriteLog("HkCBT: WM_ACTIVATE\n");
#endif
		activate.fMouse     = (LOWORD(wParam) == WA_CLICKACTIVE);
		if(LOWORD(wParam) == WA_INACTIVE) {
			active   = lParam;
			inactive = hwnd;
		}
		else {
			active   = hwnd;
			inactive = lParam;
		}
		activate.hWndActive = active;
		return(pCallback(HCBT_ACTIVATE, inactive, (LPARAM)&activate));
	case WM_SETFOCUS:
#ifdef DEBUG
		WriteLog("HkCBT: WM_SETFOCUS\n");
#endif
		return(pCallback(HCBT_SETFOCUS, hwnd, wParam));
	case WM_MOVE:
#ifdef DEBUG
		WriteLog("HkCBT: WM_MOVE\n");
#endif
		GetWindowRect(hwnd, &rect);
		rect.left   = LOWORD(lParam);
		rect.top    = HIWORD(lParam);
		return(pCallback(HCBT_MOVESIZE, hwnd, (LPARAM)&rect));
	case WM_SIZE:
#ifdef DEBUG
		WriteLog("HkCBT: WM_SIZE\n");
#endif
		switch(wParam) {
			case SIZE_MINIMIZED:  //TODO:not correct
				return(pCallback(HCBT_MINMAX, hwnd, SW_SHOWMINIMIZED));	
			case SIZE_MAXIMIZED:  //TODO:not correct
				return(pCallback(HCBT_MINMAX, hwnd, SW_SHOWMAXIMIZED));
			case SIZE_RESTORED:
				GetWindowRect(hwnd, &rect);
				rect.right  = rect.left + LOWORD(lParam); //== new width
				rect.bottom = rect.top  - HIWORD(lParam); //== new height
				return(pCallback(HCBT_MOVESIZE, hwnd, (LPARAM)&rect));
		}
	case WM_QUEUESYNC:
#ifdef DEBUG
		WriteLog("HkCBT: WM_QUEUESYNC\n");
#endif
		return(pCallback(HCBT_QS, 0, 0));
	case WM_SYSCOMMAND:
#ifdef DEBUG
		WriteLog("HkCBT: WM_SYSCOMMAND\n");
#endif
		return(pCallback(HCBT_SYSCOMMAND, wParam, lParam));
#ifdef DEBUG1
	default:
		WriteLog("HkCBT: Other msg %X\n", message);
		break;
#endif

//TODO: HCBT_KEYSKIPPED & HCBT_CLICKSKIPPED
  }
  return(0);
}
//******************************************************************************
//******************************************************************************
HkCBT *HkCBT::FindHook(HHOOK hhook)
{
 HkCBT *hook = HkCBT::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		return(hook);
	}
	hook = hook->next;
  }
  return(NULL);
}
//******************************************************************************
//******************************************************************************
LRESULT HkCBT::CallNextHook(HHOOK hhook, int nCode, WPARAM wParam, LPARAM lParam)
{ 
 HkCBT *hkhook = (HkCBT *)hhook;

  if(hkhook->next != NULL) {
	return(hkhook->CBTCallback(hkhook->orghwnd, hkhook->orgMsg, hkhook->orgwParam, hkhook->orglParam));
  }
  else 	return(0);
}
//******************************************************************************
//******************************************************************************
HkCBT *HkCBT::hooks = NULL;

//******************************************************************************
//HkDebug class
//******************************************************************************
HkDebug::HkDebug(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkDebug::~HkDebug()
{
 HkDebug *hook = HkDebug::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkDebug::GetOS2Hook()
{
  return(OS2HkDebugProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkDebug::UnHookWindowsHook(HHOOK hhook)
{
 HkDebug *hook = HkDebug::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkDebug::FindHookProc(HOOKPROC hkprc)
{
 HkDebug *hook = HkDebug::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkDebug::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkDebugProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkDebug *hook = HkDebug::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkDebug *HkDebug::hooks = NULL;

//******************************************************************************
//HkGetMessage class
//******************************************************************************
HkGetMessage::HkGetMessage(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkGetMessage::~HkGetMessage()
{
 HkGetMessage *hook = HkGetMessage::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkGetMessage::GetOS2Hook()
{
  return(OS2HkGetMessageProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkGetMessage::UnHookWindowsHook(HHOOK hhook)
{
 HkGetMessage *hook = HkGetMessage::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkGetMessage::FindHookProc(HOOKPROC hkprc)
{
 HkGetMessage *hook = HkGetMessage::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkGetMessage::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkGetMessageProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkGetMessage *hook = HkGetMessage::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkGetMessage *HkGetMessage::hooks = NULL;

//******************************************************************************
//HkMouse class
//******************************************************************************
HkMouse::HkMouse(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkMouse::~HkMouse()
{
 HkMouse *hook = HkMouse::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkMouse::GetOS2Hook()
{
  return(OS2HkMouseProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkMouse::UnHookWindowsHook(HHOOK hhook)
{
 HkMouse *hook = HkMouse::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkMouse::FindHookProc(HOOKPROC hkprc)
{
 HkMouse *hook = HkMouse::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkMouse::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkMouseProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkMouse *hook = HkMouse::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkMouse *HkMouse::hooks = NULL;

//******************************************************************************
//HkKeyboard class
//******************************************************************************
HkKeyboard::HkKeyboard(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkKeyboard::~HkKeyboard()
{
 HkKeyboard *hook = HkKeyboard::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkKeyboard::GetOS2Hook()
{
  return(OS2HkKeyboardProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkKeyboard::UnHookWindowsHook(HHOOK hhook)
{
 HkKeyboard *hook = HkKeyboard::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkKeyboard::FindHookProc(HOOKPROC hkprc)
{
 HkKeyboard *hook = HkKeyboard::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkKeyboard::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkKeyboard *hook = HkKeyboard::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkKeyboard *HkKeyboard::hooks = NULL;

//******************************************************************************
//HkMsgFilter class
//******************************************************************************
HkMsgFilter::HkMsgFilter(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkMsgFilter::~HkMsgFilter()
{
 HkMsgFilter *hook = HkMsgFilter::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkMsgFilter::GetOS2Hook()
{
  return(OS2HkMsgFilterProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkMsgFilter::UnHookWindowsHook(HHOOK hhook)
{
 HkMsgFilter *hook = HkMsgFilter::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkMsgFilter::FindHookProc(HOOKPROC hkprc)
{
 HkMsgFilter *hook = HkMsgFilter::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkMsgFilter::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkMsgFilterProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkMsgFilter *hook = HkMsgFilter::hooks;
 LRESULT   rc;

#ifdef DEBUG
  WriteLog("MsgFilterProc\n");
#endif
  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkMsgFilter *HkMsgFilter::hooks = NULL;

//******************************************************************************
//HkSysMsgFilter class
//******************************************************************************
HkSysMsgFilter::HkSysMsgFilter(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkSysMsgFilter::~HkSysMsgFilter()
{
 HkSysMsgFilter *hook = HkSysMsgFilter::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkSysMsgFilter::GetOS2Hook()
{
  return(OS2HkSysMsgFilterProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkSysMsgFilter::UnHookWindowsHook(HHOOK hhook)
{
 HkSysMsgFilter *hook = HkSysMsgFilter::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkSysMsgFilter::FindHookProc(HOOKPROC hkprc)
{
 HkSysMsgFilter *hook = HkSysMsgFilter::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkSysMsgFilter::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkSysMsgFilterProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkSysMsgFilter *hook = HkSysMsgFilter::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkSysMsgFilter *HkSysMsgFilter::hooks = NULL;

//******************************************************************************
//HkShell class
//******************************************************************************
HkShell::HkShell(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkShell::~HkShell()
{
 HkShell *hook = HkShell::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkShell::GetOS2Hook()
{
  return(OS2HkShellProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkShell::UnHookWindowsHook(HHOOK hhook)
{
 HkShell *hook = HkShell::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkShell::FindHookProc(HOOKPROC hkprc)
{
 HkShell *hook = HkShell::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkShell::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkShellProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkShell *hook = HkShell::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkShell *HkShell::hooks = NULL;

//******************************************************************************
//HkJrnlPlayback class
//******************************************************************************
HkJrnlPlayback::HkJrnlPlayback(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkJrnlPlayback::~HkJrnlPlayback()
{
 HkJrnlPlayback *hook = HkJrnlPlayback::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkJrnlPlayback::GetOS2Hook()
{
  return(OS2HkJrnlPlaybackProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkJrnlPlayback::UnHookWindowsHook(HHOOK hhook)
{
 HkJrnlPlayback *hook = HkJrnlPlayback::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkJrnlPlayback::FindHookProc(HOOKPROC hkprc)
{
 HkJrnlPlayback *hook = HkJrnlPlayback::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkJrnlPlayback::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkJrnlPlaybackProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkJrnlPlayback *hook = HkJrnlPlayback::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkJrnlPlayback *HkJrnlPlayback::hooks = NULL;

//******************************************************************************
//HkJrnlRecord class
//******************************************************************************
HkJrnlRecord::HkJrnlRecord(HHOOK hook, HOOKPROC pUserCallback, HINSTANCE hinst, DWORD dwThreadId)
{
  //Insert it in front of the rest
  next  = hooks;
  hooks = this;

  pCallback         = pUserCallback;
  hhook             = hook;
  this->hinst       = hinst;
  this->dwThreadId  = dwThreadId;
  fRecursive        = FALSE;
}
//******************************************************************************
//******************************************************************************
HkJrnlRecord::~HkJrnlRecord()
{
 HkJrnlRecord *hook = HkJrnlRecord::hooks;

  if(hook == this) {
	hooks = next;
  }
  else {
	while(hook->next != NULL) {
		if(hook->next == this) {
			hook->next = next;
			break;
		}
		hook = hook->next;
	}
  }
}
//******************************************************************************
//******************************************************************************
HOOKPROC_O32 HkJrnlRecord::GetOS2Hook()
{
  return(OS2HkJrnlRecordProc);
}
//******************************************************************************
//******************************************************************************
BOOL HkJrnlRecord::UnHookWindowsHook(HHOOK hhook)
{
 HkJrnlRecord *hook = HkJrnlRecord::hooks;

  while(hook != NULL) {
	if(hook->hhook == hhook) {
		delete hook;
		return(TRUE);
	}
	hook = hook->next;
  }
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HHOOK HkJrnlRecord::FindHookProc(HOOKPROC hkprc)
{
 HkJrnlRecord *hook = HkJrnlRecord::hooks;

  while(hook != NULL) {
	if(hook->pCallback == hkprc) {
		return(hook->hhook);
	}
	hook = hook->next;
  } 
#ifdef DEBUG
  WriteLog("HkJrnlRecord::FindHookProc, couldn't find proc %X\n", hkprc);
#endif
  return(NULL);
}
//******************************************************************************
//******************************************************************************
static LRESULT OS2HkJrnlRecordProc(int code, WPARAM wParam, LPARAM lParam)
{
 HkJrnlRecord *hook = HkJrnlRecord::hooks;
 LRESULT   rc;

  while(hook != NULL) {
	if(hook->fRecursive == FALSE) {
		if(hook->dwThreadId == 0 || hook->dwThreadId == GetCurrentThreadId()) {
			hook->fRecursive = TRUE;
			rc = hook->pCallback(code, wParam, lParam);
			hook->fRecursive = FALSE;
			return(rc);
		}
	}
	hook = hook->next;
  } 
  return(0);
}
//******************************************************************************
//******************************************************************************
HkJrnlRecord *HkJrnlRecord::hooks = NULL;

