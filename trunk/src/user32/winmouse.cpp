/* $Id: winmouse.cpp,v 1.1 1999-11-08 13:44:40 sandervl Exp $ */
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
BOOL DInputMouseHandler(HWND hwnd, ULONG msg, ULONG x, ULONG y, ULONG info)
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
	switch(info) 
        {
        case BUTTON_LEFTDOWN:
		dwFlags |= MOUSEEVENTF_LEFTDOWN;
                break;
        case BUTTON_LEFTUP:
		dwFlags |= MOUSEEVENTF_LEFTUP;
                break;
        case BUTTON_RIGHTUP:
		dwFlags |= MOUSEEVENTF_RIGHTUP;
                break;
        case BUTTON_RIGHTDOWN:
		dwFlags |= MOUSEEVENTF_RIGHTDOWN;
                break;
        case BUTTON_MIDDLEUP:
		dwFlags |= MOUSEEVENTF_MIDDLEUP;
                break;
        case BUTTON_MIDDLEDOWN:
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
