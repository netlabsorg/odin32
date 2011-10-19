/* $Id: win32wbasepos.cpp,v 1.31 2003-05-15 12:40:20 sandervl Exp $ */
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
#include "win32wdesktop.h"
#include <win/hook.h>

#define DBG_LOCALLOG    DBG_win32wbasepos
#include "dbglocal.h"

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
    MinMax.ptMaxPosition.x = 0;
    MinMax.ptMaxPosition.y = 0;

    if(!(getStyle() & (WS_POPUP | WS_CHILD))) {
        RECT rect;
        OSLibWinGetMaxPosition(getOS2FrameWindowHandle(), &rect);
        MinMax.ptMaxPosition.x = rect.left;
        MinMax.ptMaxPosition.y = rect.top;

        MinMax.ptMaxSize.x = rect.right - rect.left;
        MinMax.ptMaxSize.y = rect.bottom - rect.top;
        MinMax.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
        MinMax.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
        MinMax.ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
        MinMax.ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);
    }
    else {
        MinMax.ptMaxSize.x = GetSystemMetrics(SM_CXSCREEN);
        MinMax.ptMaxSize.y = GetSystemMetrics(SM_CYSCREEN);
        MinMax.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
        MinMax.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
        MinMax.ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
        MinMax.ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);
    }

    if (HAS_DLGFRAME( dwStyle, dwExStyle ))
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
        //SvL: Wine has no 'else', but I'm seeing different behaviour in NT
        //     and it doesn't make much sense either as a window can have
        //     only one kind of border (see drawing code)
        else
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
        MinMax.ptMaxPosition.x -= xinc;
        MinMax.ptMaxPosition.y -= yinc;
//    }

    SendMessageA(getWindowHandle(), WM_GETMINMAXINFO, 0, (LPARAM)&MinMax );

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
   LONG result = 0;

   /* Send WM_NCCALCSIZE message to get new client area */
   params.rgrc[0] = *newWindowRect;
   if(calcValidRect)
   {
        winposCopy = *winpos;
        params.rgrc[1] = *oldWindowRect;
        params.rgrc[2] = *oldClientRect;
        //client rectangel must be in parent coordinates
        OffsetRect(&params.rgrc[2], rectWindow.left, rectWindow.top);

        params.lppos = &winposCopy;
   }
   result = SendMessageA(getWindowHandle(), WM_NCCALCSIZE, calcValidRect, (LPARAM)&params );

   /* If the application send back garbage, ignore it */
   if(params.rgrc[0].left <= params.rgrc[0].right && params.rgrc[0].top <= params.rgrc[0].bottom)
   {
        *newClientRect = params.rgrc[0];
        //client rectangle now in parent coordinates; convert to 'frame' coordinates
        OffsetRect(newClientRect, -rectWindow.left, -rectWindow.top);
   }

   /* FIXME: WVR_ALIGNxxx */
   if(newClientRect->left != rectClient.left || newClientRect->top  != rectClient.top)
        winpos->flags &= ~SWP_NOCLIENTMOVE;

   if((newClientRect->right - newClientRect->left != rectClient.right - rectClient.left) ||
       (newClientRect->bottom - newClientRect->top != rectClient.bottom - rectClient.top))
        winpos->flags &= ~SWP_NOCLIENTSIZE;

   return result;
}
/***********************************************************************
 *           WINPOS_HandleWindowPosChanging
 *
 * Default handling for a WM_WINDOWPOSCHANGING. Called from DefWindowProc().
 */
LONG Win32BaseWindow::HandleWindowPosChanging(WINDOWPOS *winpos)
{
    POINT maxSize, maxTrack, minTrack;
    if (winpos->flags & SWP_NOSIZE) return 0;

    if ((dwStyle & WS_THICKFRAME) ||
        ((dwStyle & (WS_POPUP | WS_CHILD)) == 0))
    {
        GetMinMaxInfo( &maxSize, NULL, &minTrack, &maxTrack );
        winpos->cx = MIN( winpos->cx, MAX( maxSize.x, maxTrack.x) );
        winpos->cy = MIN( winpos->cy, MAX( maxSize.y, maxTrack.y) );
	if (!(dwStyle & WS_MINIMIZE))
	{
	    if (winpos->cx < minTrack.x ) winpos->cx = minTrack.x;
	    if (winpos->cy < minTrack.y ) winpos->cy = minTrack.y;
	}
    }
    return 0;
}
/***********************************************************************
 *           WINPOS_FindIconPos
 *
 * Find a suitable place for an iconic window.
 */
static void WINPOS_FindIconPos( HWND hwnd, POINT &pt )
{
    RECT rectParent;
    int  x, y, xspacing, yspacing;
    HWND hwndChild, hwndParent;

    hwndParent = GetParent(hwnd);
    if(hwndParent == 0) {
        dprintf(("WINPOS_FindIconPos: no parent found for window %x", hwnd));
        return;
    }

    GetClientRect(hwndParent, &rectParent );
    if ((pt.x >= rectParent.left) && (pt.x + GetSystemMetrics(SM_CXICON) < rectParent.right) &&
        (pt.y >= rectParent.top) && (pt.y + GetSystemMetrics(SM_CYICON) < rectParent.bottom))
        return;  /* The icon already has a suitable position */

    xspacing = GetSystemMetrics(SM_CXICONSPACING);
    yspacing = GetSystemMetrics(SM_CYICONSPACING);

    y = rectParent.bottom;
    for (;;)
    {
        x = rectParent.left;
        do
        {
            /* Check if another icon already occupies this spot */
            hwndChild = GetWindow(hwndParent, GW_CHILD);

            while(hwndChild)
            {
                Win32BaseWindow *child = NULL;
                RECT *pRectWindow;

                child = Win32BaseWindow::GetWindowFromHandle(hwndChild);
                if(!child) {
                    dprintf(("ERROR: WINPOS_FindIconPos, child %x not found", hwndChild));
                    return;
                }
                if ((child->getStyle() & WS_MINIMIZE) && (child->getWindowHandle() != hwnd))
                {
                    pRectWindow = child->getWindowRect();
                    if ((pRectWindow->left   < x + xspacing) &&
                        (pRectWindow->right  >= x) &&
                        (pRectWindow->top    <= y) &&
                        (pRectWindow->bottom > y - yspacing))
                        break;  /* There's a window in there */
                }
                RELEASE_WNDOBJ(child);
                hwndChild = GetWindow(hwndChild, GW_HWNDNEXT);
            }

            if (!hwndChild) /* No window was found, so it's OK for us */
            {
                pt.x = x + (xspacing - GetSystemMetrics(SM_CXICON)) / 2;
                pt.y = y - (yspacing + GetSystemMetrics(SM_CYICON)) / 2;
                return;
            }
            x += xspacing;
        } while(x <= rectParent.right-xspacing);

        y -= yspacing;
    }
}
/******************************************************************************
 *           WINPOS_MinMaximize
 *
 * Fill in lpRect and return additional flags to be used with SetWindowPos().
 * This function assumes that 'cmd' is different from the current window
 * state.
 */
UINT Win32BaseWindow::MinMaximize(UINT cmd, LPRECT lpRect)
{
    UINT swpFlags = 0;
    POINT size, iconPos;

    size.x = rectWindow.left;
    size.y = rectWindow.top;

    if(IsRectEmpty(&windowpos.rcNormalPosition)) {
        CopyRect(&windowpos.rcNormalPosition, &rectWindow);
    }
    if(!HOOK_CallHooksA(WH_CBT, HCBT_MINMAX, getWindowHandle(), cmd))
    {
        if(getStyle() & WS_MINIMIZE )
        {
            if(!SendMessageA(getWindowHandle(), WM_QUERYOPEN, 0, 0L))
                return (SWP_NOSIZE | SWP_NOMOVE);
        }
        switch( cmd )
        {
        case SW_MINIMIZE:
            if( getStyle() & WS_MAXIMIZE)
            {
                 setFlags(getFlags() | WIN_RESTORE_MAX);
                 setStyle(getStyle() & ~WS_MAXIMIZE);
            }
            else setFlags(getFlags() & ~WIN_RESTORE_MAX);

            setStyle(getStyle() | WS_MINIMIZE);

            if(getParent() == NULL) {
// @@PF : for now disable windows style - it messes with WV minimize - will fix it soon
//                SetRect(lpRect, -32000, -32000, getWindowWidth(), getWindowHeight());
//                OSLibSetWindowStyle(getOS2FrameWindowHandle(), getOS2WindowHandle(), getStyle(), getExStyle());
            }
            else {
                iconPos.x = windowpos.ptMinPosition.x;
                iconPos.y = windowpos.ptMinPosition.y;
                WINPOS_FindIconPos(getWindowHandle(), iconPos);
                SetRect(lpRect, iconPos.x, iconPos.y, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON) );
            }
            break;

        case SW_MAXIMIZE:
            GetMinMaxInfo(&size, &windowpos.ptMaxPosition, NULL, NULL );

            if(getStyle() & WS_MINIMIZE )
            {
                setStyle(getStyle() & ~WS_MINIMIZE);
                OSLibSetWindowStyle(getOS2FrameWindowHandle(), getOS2WindowHandle(), getStyle(), getExStyle(), getStyle());
            }
            setStyle(getStyle() | WS_MAXIMIZE);

            SetRect(lpRect, windowpos.ptMaxPosition.x, windowpos.ptMaxPosition.y,
                    size.x, size.y );
            break;

        case SW_RESTORE:
            if(getStyle() & WS_MINIMIZE)
            {
                setStyle(getStyle() & ~WS_MINIMIZE);
                OSLibSetWindowStyle(getOS2FrameWindowHandle(), getOS2WindowHandle(), getStyle(), getExStyle(), getStyle());

                if( getFlags() & WIN_RESTORE_MAX)
                {
                    /* Restore to maximized position */
                    GetMinMaxInfo(&size, &windowpos.ptMaxPosition, NULL, NULL);
                    setStyle(getStyle() | WS_MAXIMIZE);
                    SetRect(lpRect, windowpos.ptMaxPosition.x, windowpos.ptMaxPosition.y, size.x, size.y);
                    break;
                }
            }
            else
            if( !(getStyle() & WS_MAXIMIZE) )
                 return 0;
            else setStyle(getStyle() & ~WS_MAXIMIZE);

            /* Restore to normal position */
            *lpRect = windowpos.rcNormalPosition;
            lpRect->right -= lpRect->left;
            lpRect->bottom -= lpRect->top;
            break;
        }
    }
    else swpFlags |= SWP_NOSIZE | SWP_NOMOVE;

    return swpFlags;
}
/***********************************************************************
 *           WIN_FixCoordinates
 *
 * Fix the coordinates - Helper for WIN_CreateWindowEx.
 * returns default show mode in sw.
 * Note: the feature presented as undocumented *is* in the MSDN since 1993.
 */
void Win32BaseWindow::FixCoordinates( CREATESTRUCTA *cs, INT *sw)
{
    if (cs->x == CW_USEDEFAULT || cs->x == CW_USEDEFAULT16 ||
        cs->cx == CW_USEDEFAULT || cs->cx == CW_USEDEFAULT16)
    {
        if (cs->style & (WS_CHILD | WS_POPUP))
        {
            if (cs->x == CW_USEDEFAULT || cs->x == CW_USEDEFAULT16) cs->x = cs->y = 0;
            if (cs->cx == CW_USEDEFAULT || cs->cx == CW_USEDEFAULT16) cs->cx = cs->cy = 0;
        }
        else  /* overlapped window */
        {
            STARTUPINFOA info;

            GetStartupInfoA( &info );

            if (cs->x == CW_USEDEFAULT || cs->x == CW_USEDEFAULT16)
            {
                /* Never believe Microsoft's documentation... CreateWindowEx doc says
                 * that if an overlapped window is created with WS_VISIBLE style bit
                 * set and the x parameter is set to CW_USEDEFAULT, the system ignores
                 * the y parameter. However, disassembling NT implementation (WIN32K.SYS)
                 * reveals that
                 *
                 * 1) not only it checks for CW_USEDEFAULT but also for CW_USEDEFAULT16
                 * 2) it does not ignore the y parameter as the docs claim; instead, it
                 *    uses it as second parameter to ShowWindow() unless y is either
                 *    CW_USEDEFAULT or CW_USEDEFAULT16.
                 *
                 * The fact that we didn't do 2) caused bogus windows pop up when wine
                 * was running apps that were using this obscure feature. Example -
                 * calc.exe that comes with Win98 (only Win98, it's different from
                 * the one that comes with Win95 and NT)
                 */
                if (cs->y != CW_USEDEFAULT && cs->y != CW_USEDEFAULT16) *sw = cs->y;
                cs->x = (info.dwFlags & STARTF_USEPOSITION) ? info.dwX : 0;
                cs->y = (info.dwFlags & STARTF_USEPOSITION) ? info.dwY : 0;
            }

            if (cs->cx == CW_USEDEFAULT || cs->cx == CW_USEDEFAULT16)
            {
                if (info.dwFlags & STARTF_USESIZE)
                {
                    cs->cx = info.dwXSize;
                    cs->cy = info.dwYSize;
                }
                else  /* if no other hint from the app, pick 3/4 of the screen real estate */
                {
                    RECT r;
                    SystemParametersInfoA( SPI_GETWORKAREA, 0, &r, 0);
                    cs->cx = (((r.right - r.left) * 3) / 4) - cs->x;
                    cs->cy = (((r.bottom - r.top) * 3) / 4) - cs->y;
                }
            }
        }
    }
    else
    {
	/* neither x nor cx are default. Check the y values .
	 * In the trace we see Outlook and Outlook Express using
	 * cy set to CW_USEDEFAULT when opening the address book.
	 */
	if (cs->cy == CW_USEDEFAULT || cs->cy == CW_USEDEFAULT16) {
	    RECT r;
	    dprintf(("Strange use of CW_USEDEFAULT in nHeight\n"));
	    SystemParametersInfoA( SPI_GETWORKAREA, 0, &r, 0);
	    cs->cy = (((r.bottom - r.top) * 3) / 4) - cs->y;
	}
    }
}
//******************************************************************************
//******************************************************************************
