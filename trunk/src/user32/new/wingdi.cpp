/* $Id: wingdi.cpp,v 1.6 1999-07-24 14:01:45 sandervl Exp $ */
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

#define OPEN32_GDI

//******************************************************************************
//TODO: Not complete
//******************************************************************************
HDC WIN32API BeginPaint(HWND hwnd, PPAINTSTRUCT lps)
{
  Win32Window *window;
  HDC hdc;

    window = Win32Window::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetDC, window %x not found", hwnd));
        return 0;
    }
#ifdef OPEN32_GDI
    hdc = O32_BeginPaint(window->getOS2WindowHandle(),lps);
    //CB: conflict with Open32 mechanism
#else
    hdc = OSLibWinBeginPaint(window->getOS2WindowHandle(), &lps->rcPaint);
    lps->hdc = hdc;
#endif
    dprintf(("BeginPaint %X returned %x\n", hwnd, hdc));
    return hdc;

//    return lps->hdc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndPaint(HWND hwnd, const PAINTSTRUCT *lps)
{
    dprintf(("EndPaint %x\n", hwnd));
#ifdef OPEN32_GDI
    return O32_EndPaint(hwnd,lps);
    //CB: dito
#else
    return OSLibWinEndPaint(lps->hdc);
#endif
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
#ifdef OPEN32_GDI
   return O32_GetDC(window->getOS2WindowHandle());
#else
   return OSLibWinGetPS(window->getOS2WindowHandle());
#endif
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
#ifdef OPEN32_GDI
    return O32_ReleaseDC(hwnd,hdc);
#else
    return OSLibWinReleasePS(hdc);
#endif
}
//******************************************************************************
//******************************************************************************
