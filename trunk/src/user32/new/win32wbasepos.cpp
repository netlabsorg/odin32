/* $Id: win32wbasepos.cpp,v 1.6 2000-01-13 20:11:39 sandervl Exp $ */
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
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "dc.h"
#include "pmframe.h"
#include "win32wdesktop.h"

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
#if 0
        if(getParent()) {//in parent coordinates
            MapWindowPoints(getWindowHandle(), getParent()->getWindowHandle(), (POINT *)oldClientRect, 2);
        }
        else {//in screen coordinates (just add window rectangle origin (already in screen coordinates))
            OffsetRect(oldClientRect, rectWindow.left, rectWindow.top);
        }
#endif
        params.rgrc[2] = *oldClientRect;
        params.lppos = &winposCopy;
   }
   result = SendInternalMessageA(WM_NCCALCSIZE, calcValidRect, (LPARAM)&params );
   if (calcValidRect)
   {
      /* If the application send back garbage, ignore it */
      if (params.rgrc[2].left <= params.rgrc[2].right && params.rgrc[2].top <= params.rgrc[2].bottom)
      {
        *newClientRect = params.rgrc[2];
#if 0
        if(getParent()) {//in parent coordinates
            MapWindowPoints(getParent()->getWindowHandle(), getWindowHandle(), (POINT *)newClientRect, 2);
        }
        else {//in screen coordinates (just add window rectangle origin (already in screen coordinates))
            OffsetRect(newClientRect, -rectWindow.left, -rectWindow.top);
        }
#endif
      }
      else
        SetRectEmpty(newClientRect);
   }

   return result;
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

