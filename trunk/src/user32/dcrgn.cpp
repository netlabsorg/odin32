/* $Id: dcrgn.cpp,v 1.6 2001-06-09 14:50:17 sandervl Exp $ */

/*
 * DC functions for USER32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>

#define INCL_WIN
#define INCL_GPI
#define INCL_GREALL
#define INCL_DEV
#include <os2wrap.h>
//#include <pmddi.h>
#include <stdlib.h>

#include <string.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <misc.h>
#include <win32wbase.h>
#include <math.h>
#include <limits.h>
#include "oslibwin.h"
#include "oslibmsg.h"
#include <dcdata.h>
#include <objhandle.h>
#include <wingdi32.h>

#define INCLUDED_BY_DC
#include "dc.h"

#define DBG_LOCALLOG    DBG_dcrgn
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
BOOL WIN32API GetUpdateRect(HWND hwnd, LPRECT pRect, BOOL erase)
{
    if (!hwnd)
    {
        dprintf(("GetUpdateRect %x %x %d -> invalid handle!!", hwnd, pRect, erase));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        return FALSE;
    }

    RECTL rectl, rectlClient;
    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

    if (!wnd)
    {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        dprintf(("GetUpdateRect %x %x %d -> invalid handle!!", hwnd, pRect, erase));
        return FALSE;
    }

    dprintf(("GetUpdateRect %x %x %d", hwnd, pRect, erase));

    BOOL updateRegionExists = WinQueryUpdateRect(wnd->getOS2WindowHandle(), pRect ? &rectl : NULL);
    if (!pRect) {
        dprintf(("GetUpdateRect returned %d", updateRegionExists));
        RELEASE_WNDOBJ(wnd);
        return (updateRegionExists);
    }

    if(updateRegionExists)
    {
        //CB: for PM empty rect is valid
        if ((rectl.xLeft == rectl.xRight) || (rectl.yTop == rectl.yBottom)) {
            if(pRect) {
                pRect->left = pRect->top = pRect->right = pRect->bottom = 0;
            }
            RELEASE_WNDOBJ(wnd);
            return FALSE;
        }
        mapOS2ToWin32Rect(wnd->getClientHeight(), (PRECTLOS2)&rectl, pRect);

        if(wnd->isOwnDC() && GetMapMode(wnd->getOwnDC()) != MM_TEXT_W)
        {
            DPtoLP(wnd->getOwnDC(), (LPPOINT)pRect, 2);
        }
        if (erase)
            sendEraseBkgnd (wnd);
   }
   else
   {
        if(pRect) {
            pRect->left = pRect->top = pRect->right = pRect->bottom = 0;
        }
   }

   dprintf(("GetUpdateRect returned (%d,%d)(%d,%d)", pRect->left, pRect->top, pRect->right, pRect->bottom));
   RELEASE_WNDOBJ(wnd);
   return updateRegionExists;
}
//******************************************************************************
//TODO: Seems to return region in window coordinates instead of client coordinates
//******************************************************************************
int WIN32API GetUpdateRgn(HWND hwnd, HRGN hrgn, BOOL erase)
{
    LONG lComplexity;
    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

    hrgn = ObjWinToOS2Region(hrgn);
    if(!wnd || !hrgn)
    {
        dprintf(("WARNING: GetUpdateRgn %x %x %d; invalid handle", hwnd, hrgn, erase));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        if(wnd) RELEASE_WNDOBJ(wnd);
        return ERROR_W;
    }
    lComplexity = WinQueryUpdateRegion(wnd->getOS2WindowHandle(), hrgn);
    if(lComplexity == RGN_ERROR) {
        dprintf(("WARNING: GetUpdateRgn %x %x %d; RGN_ERROR", hwnd, hrgn, erase));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        RELEASE_WNDOBJ(wnd);
        return ERROR_W;
    }

    if(lComplexity != RGN_NULL)
    {
        if(!setWinDeviceRegionFromPMDeviceRegion(hrgn, hrgn, NULL, wnd->getOS2WindowHandle()))
        {
            dprintf(("WARNING: GetUpdateRgn %x %x %d; setWinDeviceRegionFromPMDeviceRegion failed!", hwnd, hrgn, erase));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
            RELEASE_WNDOBJ(wnd);
            return ERROR_W;
        }
        if(erase) sendEraseBkgnd(wnd);
    }
    RELEASE_WNDOBJ(wnd);
    return lComplexity;
}
//******************************************************************************
//TODO: Check
//******************************************************************************
INT WIN32API ExcludeUpdateRgn(HDC hdc, HWND hwnd)
{
    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    pDCData          pHps = (pDCData)GpiQueryDCData((HPS)hdc);
    LONG             lComplexity;

    if(!wnd || !pHps)
    {
        dprintf(("WARNING: ExcludeUpdateRgn %x %x; invalid handle", hdc, hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        if(wnd) RELEASE_WNDOBJ(wnd);
        return ERROR_W;
    }
    dprintf(("USER32: ExcludeUpdateRgn %x %x", hdc, hwnd));

    lComplexity = WinExcludeUpdateRegion(pHps->hps, wnd->getOS2WindowHandle());
    if(lComplexity == RGN_ERROR) {
         SetLastError(ERROR_INVALID_HANDLE_W); //todo: correct error
    }
    else SetLastError(ERROR_SUCCESS_W);

    RELEASE_WNDOBJ(wnd);
    return lComplexity;      // windows and PM values are identical
}
/*****************************************************************************
 * Name      : int WIN32API GetWindowRgn
 * Purpose   : The GetWindowRgn function obtains a copy of the window region of a window.
 * Parameters: HWND hWnd handle to window whose window region is to be obtained
 *             HRGN hRgn handle to region that receives a copy of the window region
 * Variables :
 * Result    : NULLREGION, SIMPLEREGION, COMPLEXREGION, ERROR
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API GetWindowRgn(HWND hwnd, HRGN hRgn)
{
    Win32BaseWindow *window;
    HRGN hWindowRegion;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowContextHelpId, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        return 0;
    }
    dprintf(("USER32:GetWindowRgn (%x,%x)", hwnd, hRgn));
    hWindowRegion = window->GetWindowRegion();
    RELEASE_WNDOBJ(window);
    return CombineRgn(hRgn, hWindowRegion, 0, RGN_COPY_W);
}
/*****************************************************************************
 * Name      : int WIN32API SetWindowRgn
 * Purpose   : The SetWindowRgn function sets the window region of a window. The
 *             window region determines the area within the window where the
 *             operating system permits drawing. The operating system does not
 *             display any portion of a window that lies outside of the window region
 *             When this function is called, the system sends the WM_WINDOWPOSCHANGING and
 *             WM_WINDOWPOSCHANGED messages to the window.
 *
 * Parameters: HWND  hWnd    handle to window whose window region is to be set
 *             HRGN  hRgn    handle to region
 *             BOOL  bRedraw window redraw flag
 * Variables :
 * Result    : If the function succeeds, the return value is non-zero.
 *             If the function fails, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API SetWindowRgn(HWND hwnd,
                          HRGN hRgn,
                          BOOL bRedraw)
{
    Win32BaseWindow *window;
    HRGN hWindowRegion;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetWindowContextHelpId, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        return 0;
    }
    dprintf(("USER32:SetWindowRgn (%x,%x,%d)", hwnd, hRgn, bRedraw));
    if(window->GetWindowRegion()) {
        DeleteObject(window->GetWindowRegion());
    }
    window->SetWindowRegion(hRgn);
    if(bRedraw) {
        RedrawWindow(hwnd, 0, 0, RDW_UPDATENOW_W);
    }
    RELEASE_WNDOBJ(window);
//TODO:
//  When this function is called, the system sends the WM_WINDOWPOSCHANGING and
//  WM_WINDOWPOSCHANGED messages to the window.
    return 1;
}
//******************************************************************************
//******************************************************************************
