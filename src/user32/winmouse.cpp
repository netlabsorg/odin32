/* $Id: winmouse.cpp,v 1.4 2000-01-18 20:11:19 sandervl Exp $ */
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
#include "winmouse.h"
#include "oslibmsg.h"
#include "pmwindow.h"

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
void DInputKeyBoardHandler(MSG *msg)
{
  if(keyboardHandler)
  	keyboardHandler(msg->hwnd, msg->message, msg->wParam, msg->lParam);
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
  if(msg == MOUSEMSG_BUTTON) 
  {
	switch(msg) 
        {
        case WM_LBUTTONDOWN:
		dwFlags |= MOUSEEVENTF_LEFTDOWN;
                break;
        case WM_LBUTTONUP:
		dwFlags |= MOUSEEVENTF_LEFTUP;
                break;
        case WM_RBUTTONUP:
		dwFlags |= MOUSEEVENTF_RIGHTUP;
                break;
        case WM_RBUTTONDOWN:
		dwFlags |= MOUSEEVENTF_RIGHTDOWN;
                break;
        case WM_MBUTTONUP:
		dwFlags |= MOUSEEVENTF_MIDDLEUP;
                break;
        case WM_MBUTTONDOWN:
		dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
                break;
	}
  }
  else  dwFlags |= MOUSEEVENTF_MOVE;

  x = (((long)x << 16) + ScreenWidth-1)  / ScreenWidth;
  y = (((long)y << 16) + ScreenHeight-1) / ScreenHeight;

  mouseHandler(dwFlags, x, y, 0, (DWORD)&mouseEvent);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
