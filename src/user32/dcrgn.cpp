/* $Id: dcrgn.cpp,v 1.1 2000-06-08 18:10:10 sandervl Exp $ */

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

#define INCLUDED_BY_DC
#include "dc.h"

#define DBG_LOCALLOG    DBG_dcrgn
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
int WIN32API GetClipBox( HDC hdc, PRECT lpRect)
{
 pDCData  pHps = (pDCData)GpiQueryDCData((HPS)hdc);
 RECTL    rectl;
 LONG     lComplexity;
 int      rc;

    if(!hdc || !lpRect || !pHps) {
    	dprintf(("GDI32: GetClipBox %x %x ERROR_INVALID_PARAMETER", hdc, lpRect));
	SetLastError(ERROR_INVALID_PARAMETER_W);
	return ERROR_W;
    }
    if(pHps->isPrinter)
    {
        lpRect->left   = 0;
        lpRect->top    = 0;
        lpRect->right  = GetDeviceCaps( hdc, HORZRES_W);
        lpRect->bottom = GetDeviceCaps( hdc, VERTRES_W);

        rc = SIMPLEREGION_W;
    }
    else {
    	lComplexity = GpiQueryClipBox(pHps->hps, &rectl);
    	if(lComplexity == RGN_ERROR) 
    	{
		rc = ERROR_W;
    	}
    	else
	if(lComplexity == RGN_NULL) 
    	{
		memset(lpRect, 0, sizeof(*lpRect));
		rc = NULLREGION_W;
	}
	else {
		lpRect->left   = rectl.xLeft;
		lpRect->right  = rectl.xRight;
#if 0
		lpRect->top    = pHps->height - rectl.yTop;
		lpRect->bottom = pHps->height - rectl.yBottom;
#else
		//No conversion required as GpiQueryClipBox is affected by
                //the y-inversion of the window
		lpRect->top    = rectl.yBottom;
		lpRect->bottom = rectl.yTop;
#endif
		//Convert including/including to including/excluding
		if(lpRect->left != lpRect->right) {
			lpRect->right++;
		}
		if(lpRect->top != lpRect->bottom) {
			lpRect->bottom++;
		}
		rc = (lComplexity == RGN_RECT) ? SIMPLEREGION_W : COMPLEXREGION_W;
    	}
    }
    dprintf(("GDI32: GetClipBox of %X returned %d\n", hdc, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipRgn( HDC hdc, HRGN hRgn)
{
 int rc;

    rc = O32_GetClipRgn(hdc, hRgn);
    dprintf(("GDI32: GetClipRgn %x %x returned %x", hdc, hRgn, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API ExcludeClipRect( HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: ExcludeClipRect"));
    return O32_ExcludeClipRect(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API IntersectClipRect(HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
 int rc;

  rc = O32_IntersectClipRect(arg1, arg2, arg3, arg4, arg5);
  dprintf(("GDI32: IntersectClipRect returned %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API ExtSelectClipRgn( HDC arg1, HRGN arg2, int  arg3)
{
    dprintf(("GDI32: ExtSelectClipRgn"));
    return O32_ExtSelectClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetClipRgn( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OffsetClipRgn"));
    return O32_OffsetClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API SelectClipRgn( HDC hdc, HRGN hRgn)
{
    dprintf(("GDI32: SelectClipRgn %x %x", hdc, hRgn));
    return O32_SelectClipRgn(hdc, hRgn);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetUpdateRect (HWND hwnd, LPRECT pRect, BOOL erase)
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

   BOOL updateRegionExists = WinQueryUpdateRect (wnd->getOS2WindowHandle(), pRect ? &rectl : NULL);
   if (!pRect) {
      return (updateRegionExists);
   }

   if(updateRegionExists)
   {
        //CB: for PM empty rect is valid
        if ((rectl.xLeft == rectl.xRight) || (rectl.yTop == rectl.yBottom)) {
                if(pRect) {
                        pRect->left = pRect->top = pRect->right = pRect->bottom = 0;
                }
                return FALSE;
        }
   	mapWin32ToOS2Rect(wnd->getWindowHeight(), wnd->getClientRectPtr(), (PRECTLOS2)&rectlClient);
   	WinIntersectRect(NULL, &rectl, &rectl, &rectlClient);
	mapOS2ToWin32RectFrameToClient(wnd, (PRECTLOS2)&rectl, pRect);

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

   return updateRegionExists;
}
//******************************************************************************
//******************************************************************************
BOOL GetOS2UpdateRect(Win32BaseWindow *window, LPRECT pRect)
{
   RECTL rectl;
   BOOL updateRegionExists = WinQueryUpdateRect(window->getOS2WindowHandle(),pRect ? &rectl:NULL);

   if (!pRect)
     return (updateRegionExists);

   if (updateRegionExists)
   {
     //CB: for PM empty rect is valid
     if ((rectl.xLeft == rectl.xRight) || (rectl.yTop == rectl.yBottom)) return FALSE;
     mapOS2ToWin32Rect(window->getWindowHeight(), (PRECTLOS2)&rectl,pRect);
   }
   else
       pRect->left = pRect->top = pRect->right = pRect->bottom = 0;

   return updateRegionExists;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetUpdateRgn (HWND hwnd, HRGN hrgn, BOOL erase)
{
   LONG Complexity;

   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if (!wnd)
   {
      SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
      return ERROR_W;
   }

   Complexity = O32_GetUpdateRgn (wnd->getOS2WindowHandle(), hrgn, FALSE);
   if (erase && (Complexity > NULLREGION_W)) sendEraseBkgnd(wnd);

   return Complexity;
}
//******************************************************************************
//******************************************************************************
INT WIN32API ExcludeUpdateRgn( HDC hDC, HWND  hWnd)
{
    dprintf(("USER32:  ExcludeUpdateRgn\n"));
    hWnd = Win32BaseWindow::Win32ToOS2Handle(hWnd);

    return O32_ExcludeUpdateRgn(hDC,hWnd);
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

    return O32_CombineRgn(hRgn, hWindowRegion, 0, RGN_COPY_W);
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
        O32_DeleteObject(window->GetWindowRegion());
    }
    window->SetWindowRegion(hRgn);
    if(bRedraw) {
        RedrawWindow(hwnd, 0, 0, RDW_UPDATENOW_W);
    }
//TODO:
//  When this function is called, the system sends the WM_WINDOWPOSCHANGING and
//  WM_WINDOWPOSCHANGED messages to the window.
    return 1;
}
//******************************************************************************
//******************************************************************************
