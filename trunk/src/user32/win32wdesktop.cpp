/* $Id: win32wdesktop.cpp,v 1.21 2004-01-11 12:03:21 sandervl Exp $ */
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
#include "win32wbase.h"
#include "win32wdesktop.h"
#include "oslibwin.h"
#include "win32wndhandle.h"

#define DBG_LOCALLOG	DBG_win32wdesktop
#include "dbglocal.h"

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
Win32Desktop::Win32Desktop() : Win32BaseWindow()
{
  rectWindow.left   = 0;
  rectWindow.top    = 0;
  rectWindow.right  = OSLibQueryScreenWidth();
  rectWindow.bottom = OSLibQueryScreenHeight();

  OS2Hwnd      = OSLIB_HWND_DESKTOP;
  OS2HwndFrame = OSLIB_HWND_DESKTOP;
  rectClient   = rectWindow;

  dprintf(("Desktop window %x", Win32Hwnd));

  /* Find the window class */
  windowClass = Win32WndClass::FindClass(0, (LPSTR)DESKTOP_CLASS_ATOM);

  setWindowProc(windowClass->getWindowProc(WNDPROC_ASCII));

  dwStyle |= WS_VISIBLE;
}
//******************************************************************************
//******************************************************************************
Win32Desktop::~Win32Desktop()
{
}
//******************************************************************************
//******************************************************************************
BOOL Win32Desktop::isDesktopWindow()
{
  return TRUE;
}
//******************************************************************************
//Disabling the desktop is not a good idea (mouse no longer responds)
//******************************************************************************
BOOL Win32Desktop::EnableWindow(BOOL fEnable)
{
  return TRUE; //of course we succeeded
}
//******************************************************************************
//******************************************************************************
BOOL Win32Desktop::DestroyWindow()
{
  dprintf(("WARNING: can't destroy desktop window!!"));
  return FALSE;
}
//******************************************************************************
//******************************************************************************
HWND Win32Desktop::GetWindow(UINT uCmd)
{
 HWND hwndRelated = 0;
 Win32BaseWindow *window;

    switch(uCmd)
    {
    case GW_CHILD:
        //special case for the desktop window. we need to find the first Odin
        //window in the z-order
        hwndRelated = OSLibWinQueryWindow(getOS2WindowHandle(), QWOS_TOP);
        while(hwndRelated) 
        {
            window = GetWindowFromOS2FrameHandle(hwndRelated);
            if(window) {
                 hwndRelated = window->getWindowHandle();
                 RELEASE_WNDOBJ(window); 
                 break;
            }
            hwndRelated = OSLibWinQueryWindow(hwndRelated, QWOS_NEXT);
        }
        break;

    default:
        return Win32BaseWindow::GetWindow(uCmd);
    }
end:
    dprintf(("GetWindow %x %d returned %x", getWindowHandle(), uCmd, hwndRelated));
    return hwndRelated;
}
//******************************************************************************
//Dummy window procedure. Does nothing.
//******************************************************************************
LRESULT WIN32API DesktopWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  switch (message)
  {
    case WM_GETTEXT:
      if (!lParam || !wParam) return 0;
      ((LPSTR)lParam)[0] = 0;
      return 0;
  }

  return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDeskWallPaper(LPCSTR filename)
{
  dprintf(("USER32: SetDeskWallPaper - empty stub!"));

  return TRUE;
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
    wndClass.hCursor       = LoadCursorA(0, IDC_ARROWA);
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
