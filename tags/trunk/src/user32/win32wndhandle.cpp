/* $Id: win32wndhandle.cpp,v 1.14 2002-06-13 14:02:50 sandervl Exp $ */
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
#include <custombuild.h>

#define DBG_LOCALLOG	DBG_win32wndhandle
#include "dbglocal.h"

//******************************************************************************

//Global DLL Data
#pragma data_seg(_GLOBALDATA)
ULONG                WindowHandleTable[MAX_WINDOW_HANDLES] = {0};
CRITICAL_SECTION_OS2 globalwhandlecritsect = {0};
ULONG                lastIndex = 0;
#pragma data_seg()

static char *pszWndHandleSemName = NULL;

//******************************************************************************
//******************************************************************************
void WIN32API SetCustomWndHandleSemName(LPSTR pszSemName) 
{
    pszWndHandleSemName = pszSemName;
}
//******************************************************************************
//******************************************************************************
void InitializeWindowHandles()
{
    if(globalwhandlecritsect.hmtxLock == 0) {
         DosInitializeCriticalSection(&globalwhandlecritsect, (pszWndHandleSemName) ? pszWndHandleSemName : WINHANDLE_CRITSECTION_NAME);
    }
    else {
         dprintf(("InitializeWindowHandles -> access shared critical section"));
         DosAccessCriticalSection(&globalwhandlecritsect, WINHANDLE_CRITSECTION_NAME);
    }
}
//******************************************************************************
//******************************************************************************
BOOL HwAllocateWindowHandle(HWND *hwnd, DWORD dwUserData)
{
  DosEnterCriticalSection(&globalwhandlecritsect);

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
	DosLeaveCriticalSection(&globalwhandlecritsect);
	dprintf(("ERROR: USER32: HwAllocateWindowHandle OUT OF WINDOW HANDLES!!"));
	DebugInt3();
	return FALSE;
  }
  *hwnd  = lastIndex+1;	//we skip handle 0x68000000 
  *hwnd |= WNDHANDLE_MAGIC_HIGHWORD;
  WindowHandleTable[lastIndex] = dwUserData;

  lastIndex++;
  DosLeaveCriticalSection(&globalwhandlecritsect);
  dprintf2(("HwAllocateWindowHandle %x", *hwnd));
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void HwFreeWindowHandle(HWND hwnd)
{
  hwnd = hwnd - 1; //we skip handle 0x68000000 
  hwnd &= WNDHANDLE_MAGIC_MASK;
  if(hwnd < MAX_WINDOW_HANDLES) {
	DosEnterCriticalSection(&globalwhandlecritsect);
	WindowHandleTable[hwnd] = 0;
	DosLeaveCriticalSection(&globalwhandlecritsect);
  }
  dprintf2(("HwFreeWindowHandle %x", hwnd));
}
//******************************************************************************
//******************************************************************************
BOOL HwGetWindowHandleData(HWND hwnd, DWORD *pdwUserData)
{
  hwnd = hwnd - 1; //we skip handle 0x68000000 
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
