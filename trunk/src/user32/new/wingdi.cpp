/* $Id: wingdi.cpp,v 1.4 1999-07-19 18:40:44 sandervl Exp $ */
/*
 * Win32 Window graphics apis for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>

#include <win32wnd.h>
#include <oslibwin.h>
#include <oslibgdi.h>

//******************************************************************************
//TODO: Not complete
//******************************************************************************
HDC WIN32API BeginPaint(HWND hwnd, PPAINTSTRUCT lps)
{
  Win32Window *window;

    window = Win32Window::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetDC, window %x not found", hwnd));
        return 0;
    }
    dprintf(("BeginPaint %X\n", hwnd));
    lps->hdc = OSLibWinBeginPaint(window->getOS2WindowHandle(), &lps->rcPaint);

    return lps->hdc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndPaint(HWND hwnd, const PAINTSTRUCT *lps)
{
    dprintf(("EndPaint %x\n", hwnd));
    return OSLibWinEndPaint(lps->hdc);
}
//******************************************************************************
//TODO: PARENT_DC flag
//******************************************************************************
HDC WIN32API GetDC(HWND hwnd)
{
  Win32Window *window;

   window = Win32Window::GetWindowFromHandle(hwnd);
   if(!window) {
    dprintf(("GetDC, window %x not found", hwnd));
    return 0;
   }
   dprintf(("GetDC %x", hwnd));
   return OSLibWinGetPS(window->getOS2WindowHandle());
}
//******************************************************************************
//TODO
//******************************************************************************
HDC WIN32API GetDCEx(HWND hwnd, HRGN arg2, DWORD arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetDCEx NOT CORRECT!\n");
#endif
    return GetDC(hwnd);
}
//******************************************************************************
//******************************************************************************
int WIN32API ReleaseDC(HWND hwnd, HDC hdc)
{
    return OSLibWinReleasePS(hdc);
}
//******************************************************************************
//******************************************************************************
