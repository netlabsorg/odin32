/* $Id: win32wndhandle.cpp,v 1.1 2001-05-11 08:36:29 sandervl Exp $ */
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

#define DBG_LOCALLOG	DBG_win32wndhandle
#include "dbglocal.h"

//******************************************************************************

//NOTE: This must be in the local data segment -> if a shared semaphore was
//      created by a different process, the handle returned by DosOpenMutexSem
//      will be returned in hGlobalTableMutex
HMTX hGlobalTableMutex = 0;

//Global DLL Data
#pragma data_seg(_GLOBALDATA)
ULONG  WindowHandleTable[MAX_WINDOW_HANDLES] = {0};
VMutex tableMutex(VMUTEX_SHARED, &hGlobalTableMutex);
ULONG  lastIndex = 0;
#pragma data_seg()

//******************************************************************************
//******************************************************************************
BOOL HwAllocateWindowHandle(HWND *hwnd, DWORD dwUserData)
{
  tableMutex.enter(VMUTEX_WAIT_FOREVER, &hGlobalTableMutex);

  //find next free handle
  if(lastIndex >= MAX_WINDOW_HANDLES-1) {
        lastIndex = 0;
  }
  for(int i=lastIndex;i<MAX_WINDOW_HANDLES;i++) {
	if(WindowHandleTable[i] == 0) {
		lastIndex = i;
		break;
	}
  }
  if(i == MAX_WINDOW_HANDLES) {
	//oops, out of handles
	tableMutex.leave(&hGlobalTableMutex);
	dprintf(("ERROR: USER32: HwAllocateWindowHandle OUT OF WINDOW HANDLES!!"));
	DebugInt3();
	return FALSE;
  }
  *hwnd  = lastIndex;
  *hwnd |= WNDHANDLE_MAGIC_HIGHWORD;
  WindowHandleTable[lastIndex] = dwUserData;

  tableMutex.leave(&hGlobalTableMutex);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void HwFreeWindowHandle(HWND hwnd)
{
  hwnd &= WNDHANDLE_MAGIC_MASK;
  if(hwnd < MAX_WINDOW_HANDLES) {
	tableMutex.enter(VMUTEX_WAIT_FOREVER, &hGlobalTableMutex);
	WindowHandleTable[hwnd] = 0;
	tableMutex.leave(&hGlobalTableMutex);
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
