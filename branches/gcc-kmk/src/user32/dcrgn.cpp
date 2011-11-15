/* $Id: dcrgn.cpp,v 1.9 2003-11-12 14:10:19 sandervl Exp $ */

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
#include <math.h>
#include <limits.h>
#include "win32wbase.h"
#include "oslibwin.h"
#include "oslibmsg.h"
#include <dcdata.h>
#include <objhandle.h>
#include <wingdi32.h>

#define INCLUDED_BY_DC
#include "dc.h"

#define DBG_LOCALLOG    DBG_dcrgn
#include "dbglocal.h"

#ifdef DEBUG
//#define dprintfRegion(a,b,c) if(DbgEnabledLvl2USER32[DBG_LOCALLOG] == 1) dprintfRegion1(a,b,c)
//#define dprintfRegion(a,b,c) dprintfRegion1(a,b,c)

static void dprintfRegion(HWND hWnd, HRGN hrgnClip)
{
 RGNRECT rgnRect = {0, 16, 0, RECTDIR_LFRT_TOPBOT};
 RECTL   rectRegion[16];
 APIRET  rc;
 HDC     hdc;
   
   hdc = GetDCEx(hWnd, NULL, DCX_CACHE_W|DCX_USESTYLE_W);
   dprintf(("dprintfRegion %x %x", hWnd, hdc));
   rc = GpiQueryRegionRects(hdc, hrgnClip, NULL, &rgnRect, &rectRegion[0]);
   for(int i=0;i<rgnRect.crcReturned;i++) {
        dprintf(("(%d,%d)(%d,%d)", rectRegion[i].xLeft, rectRegion[i].yBottom, rectRegion[i].xRight, rectRegion[i].yTop));
   }
   ReleaseDC(hWnd, hdc);
}
#else
#define dprintfRegion(b,c)
#endif

extern "C" {

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

    //check if the application previously didn't handle a WM_PAINT msg properly
    wnd->checkForDirtyUpdateRegion();

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
    //check if the application previously didn't handle a WM_PAINT msg properly
    wnd->checkForDirtyUpdateRegion();

    lComplexity = WinQueryUpdateRegion(wnd->getOS2WindowHandle(), hrgn);
    if(lComplexity == RGN_ERROR) {
        dprintf(("WARNING: GetUpdateRgn %x %x %d; RGN_ERROR", hwnd, hrgn, erase));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        RELEASE_WNDOBJ(wnd);
        return ERROR_W;
    }

    if(lComplexity != RGN_NULL)
    {
        dprintfRegion(hwnd, hrgn);
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
//TODO: Seems to return region in window coordinates instead of client coordinates
//******************************************************************************
int WIN32API GetUpdateRgnFrame(HWND hwnd, HRGN hrgn)
{
    LONG lComplexity;
    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

    hrgn = ObjWinToOS2Region(hrgn);
    if(!wnd || !hrgn)
    {
        dprintf(("WARNING: GetUpdateRgnFrame %x %x invalid handle", hwnd, hrgn));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        if(wnd) RELEASE_WNDOBJ(wnd);
        return ERROR_W;
    }
    lComplexity = WinQueryUpdateRegion(wnd->getOS2FrameWindowHandle(), hrgn);
    if(lComplexity == RGN_ERROR) {
        dprintf(("WARNING: GetUpdateRgnFrame %x %x RGN_ERROR", hwnd, hrgn));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        RELEASE_WNDOBJ(wnd);
        return ERROR_W;
    }

    if(lComplexity != RGN_NULL)
    {
        dprintfRegion(hwnd, hrgn);
        if(!setWinDeviceRegionFromPMDeviceRegion(hrgn, hrgn, NULL, wnd->getOS2FrameWindowHandle()))
        {
            dprintf(("WARNING: GetUpdateRgnFrame %x %x; setWinDeviceRegionFromPMDeviceRegion failed!", hwnd, hrgn));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
            RELEASE_WNDOBJ(wnd);
            return ERROR_W;
        }
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
// WinSetVisibleRgnNotifyProc
//   To set a notification procedure for visible region changes of a specific window.
//   The procedure will be called when a WM_VRNENABLED message is posted
//   with ffVisRgnChanged set to TRUE
//
// Parameters:
//   HWND hwnd				window handle
//   VISRGN_NOTIFY_PROC lpNotifyProc	notification proc or NULL to clear proc
//   DWORD dwUserData			value used as 3rd parameter during
//                                      visible region callback
//
// NOTE: Internal API
//******************************************************************************
BOOL WIN32API WinSetVisibleRgnNotifyProc(HWND hwnd, VISRGN_NOTIFY_PROC lpNotifyProc, 
                                         DWORD dwUserData)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("WinSetVisibleRgnNotifyProc, window %x not found", hwnd));
        return FALSE;
    }
    BOOL ret = window->setVisibleRgnNotifyProc(lpNotifyProc, dwUserData);
    RELEASE_WNDOBJ(window);
    return ret;
}
//******************************************************************************
//******************************************************************************

} // extern "C"
