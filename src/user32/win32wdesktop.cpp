/* $Id: win32wdesktop.cpp,v 1.5 1999-10-24 22:56:10 sandervl Exp $ */
/*
 * Win32 Desktop Window for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <win32wbase.h>
#include <win32wdesktop.h>
#include "oslibwin.h"
#include "win32wndhandle.h"

Win32Desktop *windowDesktop = 0;

//******************************************************************************
//******************************************************************************
BOOL CreateWin32Desktop()
{
  windowDesktop = new Win32Desktop();
  if(windowDesktop == NULL) {
	dprintf(("Unable to create desktop window!!!"));
	return FALSE;
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void DestroyDesktopWindow()
{
  if(windowDesktop) {
	delete windowDesktop;
	windowDesktop = 0;
  }
}
//******************************************************************************
//******************************************************************************
Win32Desktop::Win32Desktop() : Win32BaseWindow(OBJTYPE_WINDOW)
{
  OSLibWinQueryWindowRect(OSLIB_HWND_DESKTOP, &rectWindow, RELATIVE_TO_SCREEN);
  OS2Hwnd      = OSLIB_HWND_DESKTOP;
  OS2HwndFrame = OSLIB_HWND_DESKTOP;
  rectClient   = rectWindow;
  Win32Hwnd    = 0;

  /* Find the window class */
  windowClass = Win32WndClass::FindClass(0, (LPSTR)DESKTOP_CLASS_ATOM);

  setWindowProc(windowClass->getWindowProc());
}
//******************************************************************************
//******************************************************************************
Win32Desktop::~Win32Desktop()
{
}
//******************************************************************************
//Disabling the desktop is not a good idea (mouse no longer responds)
//******************************************************************************
BOOL Win32Desktop::EnableWindow(BOOL fEnable)
{
  return TRUE; //of course we succeeded
}
//******************************************************************************
//Dummy window procedure. Does nothing.
//******************************************************************************
LRESULT WIN32API DesktopWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL DESKTOP_Register()
{
    WNDCLASSA wndClass;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)DesktopWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hCursor       = (HCURSOR)IDC_ARROWA;
    wndClass.hbrBackground = 0;
    wndClass.lpszClassName = DESKTOP_CLASS_NAMEA;

    return RegisterClassA(&wndClass);
}
//******************************************************************************
//******************************************************************************
BOOL DESKTOP_Unregister()
{
    if (GlobalFindAtomA(DESKTOP_CLASS_NAMEA))
            return UnregisterClassA(DESKTOP_CLASS_NAMEA,(HINSTANCE)NULL);
    else    return FALSE;
}
//******************************************************************************
//******************************************************************************
