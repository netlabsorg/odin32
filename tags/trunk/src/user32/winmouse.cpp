/* $Id: winmouse.cpp,v 1.13 2001-04-15 09:47:21 sandervl Exp $ */
/*
 * Mouse handler for DINPUT
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include "win32wbase.h"
#include <winuser32.h>
#include <win\mouse.h>
#include "winmouse.h"
#include "oslibmsg.h"
#include "pmwindow.h"
#include "oslibwin.h"

#define DBG_LOCALLOG	DBG_winmouse
#include "dbglocal.h"

LPMOUSE_EVENT_PROC mouseHandler = NULL;
WNDPROC            keyboardHandler = NULL;

//******************************************************************************
//******************************************************************************
VOID WIN32API MOUSE_Enable(LPMOUSE_EVENT_PROC lpMouseEventProc)
{
  if(lpMouseEventProc == (LPMOUSE_EVENT_PROC)-1)
  {
	mouseHandler = NULL;
  }
  else	mouseHandler = lpMouseEventProc;
}
//******************************************************************************
//******************************************************************************
VOID WIN32API KEYBOARD_Enable(WNDPROC handler)
{
  keyboardHandler = handler;
}
//******************************************************************************
//******************************************************************************
BOOL DInputKeyBoardHandler(MSG *msg)
{
  if(!ISKDB_CAPTURED())
  	return FALSE;

  return keyboardHandler(msg->hwnd, msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
BOOL DInputMouseHandler(HWND hwnd, ULONG msg, ULONG x, ULONG y)
{
 WINE_MOUSEEVENT mouseEvent;
 DWORD dwFlags = MOUSEEVENTF_ABSOLUTE;

  if(!ISMOUSE_CAPTURED())
	return FALSE;

  mouseEvent.magic = WINE_MOUSEEVENT_MAGIC;
  mouseEvent.hWnd  = hwnd;
  mouseEvent.time  = OSLibWinQueryMsgTime();
  mouseEvent.keyState = 0; //not used in dinput right now
  switch(msg) 
  {
  case WM_NCLBUTTONDOWN:
  case WM_LBUTTONDOWN:
	dwFlags |= MOUSEEVENTF_LEFTDOWN;
        break;
  case WM_NCLBUTTONUP:
  case WM_LBUTTONUP:
	dwFlags |= MOUSEEVENTF_LEFTUP;
        break;
  case WM_NCRBUTTONUP:
  case WM_RBUTTONUP:
	dwFlags |= MOUSEEVENTF_RIGHTUP;
        break;
  case WM_NCRBUTTONDOWN:
  case WM_RBUTTONDOWN:
	dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        break;
  case WM_NCMBUTTONUP:
  case WM_MBUTTONUP:
	dwFlags |= MOUSEEVENTF_MIDDLEUP;
        break;
  case WM_NCMBUTTONDOWN:
  case WM_MBUTTONDOWN:
	dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        break;
  case WM_MOUSEMOVE:
  case WM_NCMOUSEMOVE:
	dwFlags |= MOUSEEVENTF_MOVE;
 	break;
  default:
	//TODO: handle double clicks???
	return FALSE;
  }

  x = (((long)x << 16) + ScreenWidth-1)  / ScreenWidth;
  y = (((long)y << 16) + ScreenHeight-1) / ScreenHeight;

  return mouseHandler(dwFlags, x, y, 0, (DWORD)&mouseEvent);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetCapture(void)
{
 HWND hwnd;

    hwnd = OS2ToWin32Handle(OSLibWinQueryCapture());
    dprintf(("USER32: GetCapture returned %x", hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetCapture(HWND hwnd)
{
 HWND hwndPrev = GetCapture();
 BOOL rc;

    if(hwnd == 0) {
	ReleaseCapture();
	return hwndPrev;
    }
    if(hwnd == hwndPrev) {
        dprintf(("USER32: SetCapture %x; already set to that window; ignore", hwnd));
        return hwndPrev;
    }
    if(hwndPrev != NULL) {
	//SvL: WinSetCapture returns an error if mouse is already captured
	OSLibWinSetCapture(0);
    }
    rc = OSLibWinSetCapture(Win32ToOS2Handle(hwnd));
    dprintf(("USER32: SetCapture %x (prev %x) returned %d", hwnd, hwndPrev, rc));
    if(hwndPrev) {
    	SendMessageA(hwndPrev, WM_CAPTURECHANGED, 0L, hwnd);
    }
    return hwndPrev;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReleaseCapture(void)
{
 HWND hwndPrev;
 BOOL ret;

    dprintf(("USER32:  ReleaseCapture"));
    hwndPrev = GetCapture();
    ret = OSLibWinSetCapture(0);
    if(hwndPrev) {
    	SendMessageA(hwndPrev, WM_CAPTURECHANGED, 0L, 0L);
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
