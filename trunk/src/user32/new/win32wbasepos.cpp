/* $Id: win32wbasepos.cpp,v 1.1 2000-01-01 14:57:31 cbratschi Exp $ */
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
 * Wine code based on build 990815
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
#include <win32wbase.h>
#include <winres.h>
#include <spy.h>
#include "wndmsg.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibmenu.h"
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "dc.h"
#include "pmframe.h"
#include "win32wdesktop.h"

  /* Some useful macros */
#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_THICKFRAME)))

#define HAS_THICKFRAME(style,exStyle) \
    (((style) & WS_THICKFRAME) && \
     !((exStyle) & WS_EX_DLGMODALFRAME))

#define HAS_THINFRAME(style) \
    (((style) & WS_BORDER) || !((style) & (WS_CHILD | WS_POPUP)))

#define HAS_BIGFRAME(style,exStyle) \
    (((style) & (WS_THICKFRAME | WS_DLGFRAME)) || \
     ((exStyle) & WS_EX_DLGMODALFRAME))

#define HAS_ANYFRAME(style,exStyle) \
    (((style) & (WS_THICKFRAME | WS_DLGFRAME | WS_BORDER)) || \
     ((exStyle) & WS_EX_DLGMODALFRAME) || \
     !((style) & (WS_CHILD | WS_POPUP)))

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
        if( !SendInternalMessageA(WM_QUERYOPEN, 0, 0L ) )
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
        if (HAS_THICKFRAME(dwStyle, dwExStyle))
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

    SendInternalMessageA(WM_GETMINMAXINFO, 0, (LPARAM)&MinMax );

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
   result = SendInternalMessageA(WM_NCCALCSIZE, calcValidRect,
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

            OffsetRect(&ncsize->rgrc[0], -ncsize->rgrc[0].left, -ncsize->rgrc[0].top);
        }
#if 0
//TODO: Docs say app should return 0 when fCalcValidRects == 0; Wine doesn't do this
        if(wParam == 0) //fCalcValidRects
            return 0;
#endif
        return result;
}
/***********************************************************************
 *           WIN_WindowNeedsWMBorder
 *
 * This method defines the rules for a window to have a WM border,
 * caption...  It is used for consitency purposes.
 */
BOOL Win32BaseWindow::WindowNeedsWMBorder( DWORD style, DWORD exStyle )
{
//    if (!(style & WS_CHILD) && Options.managed  &&
    if (!(style & WS_CHILD)  &&
        (((style & WS_CAPTION) == WS_CAPTION) ||
         (style & WS_THICKFRAME)))
        return TRUE;
    return FALSE;
}
/******************************************************************************
 * NC_AdjustRectOuter95
 *
 * Computes the size of the "outside" parts of the window based on the
 * parameters of the client area.
 *
 + PARAMS
 *     LPRECT16  rect
 *     DWORD  style
 *     BOOL32  menu
 *     DWORD  exStyle
 *
 * NOTES
 *     "Outer" parts of a window means the whole window frame, caption and
 *     menu bar. It does not include "inner" parts of the frame like client
 *     edge, static edge or scroll bars.
 *
 * Revision history
 *     05-Jul-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *        Original (NC_AdjustRect95) cut & paste from NC_AdjustRect
 *
 *     20-Jun-1998 Eric Kohl (ekohl@abo.rhein-zeitung.de)
 *        Split NC_AdjustRect95 into NC_AdjustRectOuter95 and
 *        NC_AdjustRectInner95 and added handling of Win95 styles.
 *
 *     28-Jul-1999 Ove Kåven (ovek@arcticnet.no)
 *        Streamlined window style checks.
 *
 *****************************************************************************/
void Win32BaseWindow::NC_AdjustRectOuter(LPRECT rect, DWORD style, BOOL menu, DWORD exStyle)
{
    if(style & WS_ICONIC) return;

    /* Decide if the window will be managed (see CreateWindowEx) */
//    if (!WindowNeedsWMBorder(style, exStyle))
//    {
       if (HAS_THICKFRAME( style, exStyle ))
            InflateRect( rect, GetSystemMetrics(SM_CXFRAME), GetSystemMetrics(SM_CYFRAME) );
        else
        if (HAS_DLGFRAME( style, exStyle ))
            InflateRect(rect, GetSystemMetrics(SM_CXDLGFRAME), GetSystemMetrics(SM_CYDLGFRAME) );
        else
        if (HAS_THINFRAME( style ))
            InflateRect( rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

        if ((style & WS_CAPTION) == WS_CAPTION)
        {
            if (exStyle & WS_EX_TOOLWINDOW)
                rect->top -= GetSystemMetrics(SM_CYSMCAPTION);
            else
                rect->top -= GetSystemMetrics(SM_CYCAPTION);
        }
//    }

    if (menu)
        rect->top -= GetSystemMetrics(SM_CYMENU);
}
/******************************************************************************
 * NC_AdjustRectInner95
 *
 * Computes the size of the "inside" part of the window based on the
 * parameters of the client area.
 *
 + PARAMS
 *     LPRECT16 rect
 *     DWORD    style
 *     DWORD    exStyle
 *
 * NOTES
 *     "Inner" part of a window means the window frame inside of the flat
 *     window frame. It includes the client edge, the static edge and the
 *     scroll bars.
 *
 * Revision history
 *     05-Jul-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *        Original (NC_AdjustRect95) cut & paste from NC_AdjustRect
 *
 *     20-Jun-1998 Eric Kohl (ekohl@abo.rhein-zeitung.de)
 *        Split NC_AdjustRect95 into NC_AdjustRectOuter95 and
 *        NC_AdjustRectInner95 and added handling of Win95 styles.
 *
 *****************************************************************************/
void Win32BaseWindow::NC_AdjustRectInner(LPRECT rect, DWORD style, DWORD exStyle)
{
    if(style & WS_ICONIC) return;

    if (exStyle & WS_EX_CLIENTEDGE)
        InflateRect(rect, GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));

    if (exStyle & WS_EX_STATICEDGE)
        InflateRect(rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

    if (style & WS_VSCROLL) rect->right  += GetSystemMetrics(SM_CXVSCROLL);
    if (style & WS_HSCROLL) rect->bottom += GetSystemMetrics(SM_CYHSCROLL);
}
/***********************************************************************
 *           WINPOS_HandleWindowPosChanging16
 *
 * Default handling for a WM_WINDOWPOSCHANGING. Called from DefWindowProc().
 */
LONG Win32BaseWindow::HandleWindowPosChanging(WINDOWPOS *winpos)
{
 POINT maxSize, minTrack;
 int   rc = 1;

    if (winpos->flags & SWP_NOSIZE)
        return 1;

    if ((getStyle() & WS_THICKFRAME) ||
        ((getStyle() & (WS_POPUP | WS_CHILD)) == 0))
    {
        GetMinMaxInfo(&maxSize, NULL, &minTrack, NULL );
        if (maxSize.x < winpos->cx) {
            winpos->cx = maxSize.x;
            rc = 0;
        }
        if (maxSize.y < winpos->cy) {
            winpos->cy = maxSize.y;
            rc = 0;
        }
        if (!(getStyle() & WS_MINIMIZE))
        {
            if (winpos->cx < minTrack.x ) {
                winpos->cx = minTrack.x;
                rc = 0;
            }
            if (winpos->cy < minTrack.y ) {
                winpos->cy = minTrack.y;
                rc = 0;
            }
        }
    }
    return rc;
}
//******************************************************************************
//******************************************************************************

