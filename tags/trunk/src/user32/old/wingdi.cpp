/* $Id: wingdi.cpp,v 1.1 2001-05-11 08:36:30 sandervl Exp $ */
/*
 * Win32 Window graphics apis for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>

#include <win32wbase.h>
#include <oslibwin.h>
#include <oslibgdi.h>

#define DBG_LOCALLOG	DBG_wingdi
#include "dbglocal.h"

#define OPEN32_GDI

#if 0
//******************************************************************************
//TODO: Not complete
//******************************************************************************
HDC WIN32API BeginPaint(HWND hwnd, PPAINTSTRUCT lps)
{
  Win32BaseWindow *window;
  HDC hdc;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
    dprintf(("GetDC, window %x not found", hwnd));
    return 0;
    }
// !! there is no HDC to paint on !!!    window->MsgEraseBackGround(hdc);
#ifdef OPEN32_GDI
    hdc = O32_BeginPaint(window->getOS2WindowHandle(),lps);
    //CB: conflict with Open32 mechanism
#else
    hdc = OSLibWinBeginPaint(window->getOS2WindowHandle(), &lps->rcPaint);
    lps->hdc = hdc;
#endif
    dprintf(("BeginPaint %X returned %x\n", hwnd, hdc));

    lps->fErase = !window->MsgEraseBackGround(hdc);
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
  Win32BaseWindow *window;

   window = Win32BaseWindow::GetWindowFromHandle(hwnd);
   if(!window) {
    dprintf(("GetDC, window %x not found", hwnd));
#ifdef OPEN32_GDI
    dprintf(("GetDC %x", hwnd));
    return O32_GetDC(0);
#else
    //desktop window?
    dprintf(("GetDC %x", hwnd));
    return OSLibWinGetPS(OSLIB_HWND_DESKTOP);
#endif
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
    dprintf(("ReleaseDC %x %x", hwnd, hdc));
#ifdef OPEN32_GDI
    return O32_ReleaseDC(hwnd,hdc);
#else
    return OSLibWinReleasePS(hdc);
#endif
}
//******************************************************************************
//******************************************************************************

HDC WIN32API GetWindowDC(HWND hwnd)
{
  Win32BaseWindow *window;

   window = Win32BaseWindow::GetWindowFromHandle(hwnd);
   if(!window) {
        dprintf(("GetWindowDC, window %x not found", hwnd));
        return 0;
   }
   dprintf(("GetWindowDC %x", hwnd));
#ifdef OPEN32_GDI
   return O32_GetWindowDC(window->getOS2FrameWindowHandle());
#else
   return OSLibWinGetPS(window->getOS2FrameWindowHandle());
#endif
}
#endif
//******************************************************************************
//******************************************************************************
