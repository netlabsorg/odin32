/* $Id: win32wbasepos.cpp,v 1.2 1999-10-16 14:51:43 sandervl Exp $ */
/*
 * Win32 Window Base Class for OS/2 (nonclient/position methods)
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 * Parts based on Wine Windows code (windows\win.c, windows\nonclient.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 * TODO: Not thread/process safe
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <win.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <heapstring.h>
#include <win32wbase.h>
#include <winres.h>
#include <spy.h>
#include "wndmsg.h"
#include "hooks.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibmenu.h"
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "heapshared.h"
#include "dc.h"
#include "pmframe.h"
#include "win32wdesktop.h"

#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_BORDER)))

#define HAS_THICKFRAME(style) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

#define HAS_3DFRAME(exStyle) \
    ((exStyle & WS_EX_CLIENTEDGE) || (exStyle & WS_EX_STATICEDGE) || (exStyle & WS_EX_WINDOWEDGE))

#define HAS_BORDER(style, exStyle) \
    ((style & WS_BORDER) || HAS_THICKFRAME(style) || HAS_DLGFRAME(style,exStyle))

#define IS_OVERLAPPED(style) \
    !(style & (WS_CHILD | WS_POPUP))

#define HAS_MENU()  (!(getStyle() & WS_CHILD) && (GetMenu() != 0))

#if 0
/***********************************************************************
 *           WINPOS_MinMaximize
 *
 * Fill in lpRect and return additional flags to be used with SetWindowPos().
 * This function assumes that 'cmd' is different from the current window
 * state.
 */
UINT Win32BaseWindow::MinMaximize(UINT cmd, LPRECT lpRect )
{
    UINT swpFlags = 0;
    POINT pt, size;
    LPINTERNALPOS lpPos;

    size.x = rectWindow.left; size.y = rectWindow.top;
    lpPos = WINPOS_InitInternalPos( wndPtr, size, &rectWindow );

    if (lpPos && !HOOK_CallHooks16(WH_CBT, HCBT_MINMAX, hwndSelf, cmd))
    {
    if( dwStyle & WS_MINIMIZE )
    {
        if( !SendMessageA(WM_QUERYOPEN, 0, 0L ) )
        return (SWP_NOSIZE | SWP_NOMOVE);
        swpFlags |= SWP_NOCOPYBITS;
    }
    switch( cmd )
    {
        case SW_MINIMIZE:
         if( dwStyle & WS_MAXIMIZE)
         {
             flags |= WIN_RESTORE_MAX;
             dwStyle &= ~WS_MAXIMIZE;
                 }
                 else
             flags &= ~WIN_RESTORE_MAX;
             dwStyle |= WS_MINIMIZE;

#if 0
         if( flags & WIN_NATIVE )
             if( pDriver->pSetHostAttr( wndPtr, HAK_ICONICSTATE, TRUE ) )
             swpFlags |= MINMAX_NOSWP;
#endif

         lpPos->ptIconPos = WINPOS_FindIconPos( wndPtr, lpPos->ptIconPos );

         SetRect(lpRect, lpPos->ptIconPos.x, lpPos->ptIconPos.y,
             GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON) );
         swpFlags |= SWP_NOCOPYBITS;
         break;

        case SW_MAXIMIZE:
                WINPOS_GetMinMaxInfo( wndPtr, &size, &pt, NULL, NULL );

         if( dwStyle & WS_MINIMIZE )
         {
             if( flags & WIN_NATIVE )
             if( pDriver->pSetHostAttr( wndPtr, HAK_ICONICSTATE, FALSE ) )
                 swpFlags |= MINMAX_NOSWP;

             WINPOS_ShowIconTitle( wndPtr, FALSE );
             dwStyle &= ~WS_MINIMIZE;
         }
                 dwStyle |= WS_MAXIMIZE;

         SetRect16( lpRect, lpPos->ptMaxPos.x, lpPos->ptMaxPos.y,
                    size.x, size.y );
         break;

        case SW_RESTORE:
         if( dwStyle & WS_MINIMIZE )
         {
             if( flags & WIN_NATIVE )
             if( pDriver->pSetHostAttr( wndPtr, HAK_ICONICSTATE, FALSE ) )
                 swpFlags |= MINMAX_NOSWP;

             dwStyle &= ~WS_MINIMIZE;
             WINPOS_ShowIconTitle( wndPtr, FALSE );

             if( flags & WIN_RESTORE_MAX)
             {
             /* Restore to maximized position */
                         CONV_POINT16TO32( &lpPos->ptMaxPos, &pt );
                         WINPOS_GetMinMaxInfo( wndPtr, &size, &pt, NULL, NULL);
                         CONV_POINT32TO16( &pt, &lpPos->ptMaxPos );
             dwStyle |= WS_MAXIMIZE;
             SetRect16( lpRect, lpPos->ptMaxPos.x, lpPos->ptMaxPos.y, size.x, size.y );
             break;
             }
         }
         else
             if( !(dwStyle & WS_MAXIMIZE) ) return (UINT16)(-1);
             else dwStyle &= ~WS_MAXIMIZE;

         /* Restore to normal position */

        *lpRect = lpPos->rectNormal;
         lpRect->right -= lpRect->left;
         lpRect->bottom -= lpRect->top;

         break;
    }
    } else swpFlags |= SWP_NOSIZE | SWP_NOMOVE;
    return swpFlags;
}
#endif
/*******************************************************************
 *           GetMinMaxInfo
 *
 * Get the minimized and maximized information for a window.
 */
void Win32BaseWindow::GetMinMaxInfo(POINT *maxSize, POINT *maxPos,
                                POINT *minTrack, POINT *maxTrack )
{
    MINMAXINFO MinMax;
    INT xinc, yinc;

    /* Compute default values */

    MinMax.ptMaxSize.x = GetSystemMetrics(SM_CXSCREEN);
    MinMax.ptMaxSize.y = GetSystemMetrics(SM_CYSCREEN);
    MinMax.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
    MinMax.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
    MinMax.ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
    MinMax.ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);

    if (flags & WIN_MANAGED) xinc = yinc = 0;
    else if (HAS_DLGFRAME( dwStyle, dwExStyle ))
    {
        xinc = GetSystemMetrics(SM_CXDLGFRAME);
        yinc = GetSystemMetrics(SM_CYDLGFRAME);
    }
    else
    {
        xinc = yinc = 0;
        if (HAS_THICKFRAME(dwStyle))
        {
            xinc += GetSystemMetrics(SM_CXFRAME);
            yinc += GetSystemMetrics(SM_CYFRAME);
        }
        if (dwStyle & WS_BORDER)
        {
            xinc += GetSystemMetrics(SM_CXBORDER);
            yinc += GetSystemMetrics(SM_CYBORDER);
        }
    }
    MinMax.ptMaxSize.x += 2 * xinc;
    MinMax.ptMaxSize.y += 2 * yinc;

#if 0
    lpPos = (LPINTERNALPOS)GetPropA( hwndSelf, atomInternalPos );
    if( lpPos && !EMPTYPOINT(lpPos->ptMaxPos) )
        CONV_POINT16TO32( &lpPos->ptMaxPos, &MinMax.ptMaxPosition );
    else
    {
#endif
        MinMax.ptMaxPosition.x = -xinc;
        MinMax.ptMaxPosition.y = -yinc;
//    }

    SendMessageA(WM_GETMINMAXINFO, 0, (LPARAM)&MinMax );

      /* Some sanity checks */

    dprintf(("GetMinMaxInfo: %ld %ld / %ld %ld / %ld %ld / %ld %ld\n",
                      MinMax.ptMaxSize.x, MinMax.ptMaxSize.y,
                      MinMax.ptMaxPosition.x, MinMax.ptMaxPosition.y,
                      MinMax.ptMaxTrackSize.x, MinMax.ptMaxTrackSize.y,
                      MinMax.ptMinTrackSize.x, MinMax.ptMinTrackSize.y));
    MinMax.ptMaxTrackSize.x = MAX( MinMax.ptMaxTrackSize.x,
                                   MinMax.ptMinTrackSize.x );
    MinMax.ptMaxTrackSize.y = MAX( MinMax.ptMaxTrackSize.y,
                                   MinMax.ptMinTrackSize.y );

    if (maxSize)    *maxSize  = MinMax.ptMaxSize;
    if (maxPos)     *maxPos   = MinMax.ptMaxPosition;
    if (minTrack)   *minTrack = MinMax.ptMinTrackSize;
    if (maxTrack)   *maxTrack = MinMax.ptMaxTrackSize;
}
/***********************************************************************
 *           WINPOS_SendNCCalcSize
 *
 * Send a WM_NCCALCSIZE message to a window.
 * All parameters are read-only except newClientRect.
 * oldWindowRect, oldClientRect and winpos must be non-NULL only
 * when calcValidRect is TRUE.
 */
LONG Win32BaseWindow::SendNCCalcSize(BOOL calcValidRect, RECT *newWindowRect,
                                     RECT *oldWindowRect,
                                     RECT *oldClientRect, WINDOWPOS *winpos,
                                     RECT *newClientRect )
{
   NCCALCSIZE_PARAMS params;
   WINDOWPOS winposCopy;
   LONG result;

   params.rgrc[0] = *newWindowRect;
   if (calcValidRect)
   {
        winposCopy = *winpos;
        params.rgrc[1] = *oldWindowRect;
        params.rgrc[2] = *oldClientRect;
        params.lppos = &winposCopy;
   }
   result = SendMessageA(WM_NCCALCSIZE, calcValidRect,
                         (LPARAM)&params );
   *newClientRect = params.rgrc[0];
   return result;
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::NCHandleCalcSize(WPARAM wParam, NCCALCSIZE_PARAMS *ncsize)
{
  LONG    result = 0;

        if (getStyle() & CS_VREDRAW) result |= WVR_VREDRAW;
        if (getStyle() & CS_HREDRAW) result |= WVR_HREDRAW;

//TODO: Wine calculates new size of client area even when window is iconic (client edges)
        if(!(getStyle() & (WS_MINIMIZE | WS_ICONIC)))
        {
            dprintf(("NCHandleCalcSize %x (%d,%d) (%d,%d)", getWindowHandle(), ncsize->rgrc[0].left, ncsize->rgrc[0].top, ncsize->rgrc[0].right, ncsize->rgrc[0].bottom));
            OSLibWinCalcFrameRect(getOS2FrameWindowHandle(), &ncsize->rgrc[0], TRUE); //frame -> client
            dprintf(("NCHandleCalcSize Adjusted client rect (%d,%d) (%d,%d)", ncsize->rgrc[0].left, ncsize->rgrc[0].top, ncsize->rgrc[0].right, ncsize->rgrc[0].bottom));

#if 0
            //relative to frame -> relative to itself
            ncsize->rgrc[0].right  -= ncsize->rgrc[0].left;
            ncsize->rgrc[0].left    = 0;
            ncsize->rgrc[0].bottom -= ncsize->rgrc[0].top;
            ncsize->rgrc[0].top     = 0;
#endif
        }
#if 0
//TODO: Docs say app should return 0 when fCalcValidRects == 0; Wine doesn't do this
        if(wParam == 0) //fCalcValidRects
            return 0;
#endif
        return result;
}
//******************************************************************************
//******************************************************************************

