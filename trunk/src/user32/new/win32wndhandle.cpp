/* $Id: win32wndhandle.cpp,v 1.3 2000-01-01 14:57:33 cbratschi Exp $ */
/*
 * Win32 Handle Management Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * TODO: The table should be dynamically increased when necessary
 *       This is just a quick and dirty implementation 
 * TODO: Probably need to clean up the table when app closes
 *       (also store PID and remove all handles with PID of terminating app)
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <vmutex.h>
#include "win32wndhandle.h"

//******************************************************************************

//Global DLL Data
#pragma data_seg(_GLOBALDATA)
ULONG  WindowHandleTable[MAX_WINDOW_HANDLES] = {0};
VMutex tableMutex(TRUE);
ULONG  lowestFreeIndex = 0;
#pragma data_seg()

//******************************************************************************
//******************************************************************************
BOOL HwAllocateWindowHandle(HWND *hwnd, DWORD dwUserData)
{
  tableMutex.enter();
  if(lowestFreeIndex == -1) {
	//oops, out of handles
	dprintf(("USER32: HwAllocateWindowHandle OUT OF WINDOW HANDLES!!"));
	DebugInt3();
	tableMutex.leave();
	return FALSE;
  }
  *hwnd           = lowestFreeIndex;
  *hwnd          |= WNDHANDLE_MAGIC_HIGHWORD;
  WindowHandleTable[lowestFreeIndex] = dwUserData;

  lowestFreeIndex = -1;

  //find next free handle
  for(int i=0;i<MAX_WINDOW_HANDLES;i++) {
	if(WindowHandleTable[i] == 0) {
		lowestFreeIndex = i;
		break;
	}
  }
  tableMutex.leave();
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void HwFreeWindowHandle(HWND hwnd)
{
  hwnd &= WNDHANDLE_MAGIC_MASK;
  if(hwnd < MAX_WINDOW_HANDLES) {
	tableMutex.enter();
	WindowHandleTable[hwnd] = 0;
	if(lowestFreeIndex == -1 || hwnd < lowestFreeIndex) 
		lowestFreeIndex = hwnd;

	tableMutex.leave();
  }
}
//******************************************************************************
//******************************************************************************
BOOL HwGetWindowHandleData(HWND hwnd, DWORD *pdwUserData)
{
  if((hwnd & 0xFFFF0000) != WNDHANDLE_MAGIC_HIGHWORD) {
	return FALSE; //unknown window (PM?)
  }
  hwnd &= WNDHANDLE_MAGIC_MASK;
  if(hwnd < MAX_WINDOW_HANDLES) {
	*pdwUserData = WindowHandleTable[hwnd];
	return TRUE;
  }
  *pdwUserData = 0;
  return FALSE;
}
//******************************************************************************
//******************************************************************************
