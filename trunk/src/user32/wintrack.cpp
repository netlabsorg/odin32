/*
 * Window tracking related functions.
 *
 * Copyright 2001 Sander van Leeuwen
 *
 * Changes from Wine code: (20011004)
 * - Only draw changed track frame instead of clearing the old one and
 *   drawing the new one (less flickering)
 * - Send WM_MOVING when moving a window (not done in Wine)
 * - Send WM_SIZING only when sizing a window 
 * - Fixed handling of rectangles changed by WM_SIZING/MOVING
 *
 * Based on Wine code: (dlls\x11drv\winpos.c)
 *
 * Copyright 1993, 1994, 1995, 2001 Alexandre Julliard
 * Copyright 1995, 1996, 1999 Alex Korobka
 *
 */
#include <os2win.h>
#include <string.h>

#include "win32wbase.h"
#include "hook.h"

#define ON_LEFT_BORDER(hit) \
 (((hit) == HTLEFT) || ((hit) == HTTOPLEFT) || ((hit) == HTBOTTOMLEFT))
#define ON_RIGHT_BORDER(hit) \
 (((hit) == HTRIGHT) || ((hit) == HTTOPRIGHT) || ((hit) == HTBOTTOMRIGHT))
#define ON_TOP_BORDER(hit) \
 (((hit) == HTTOP) || ((hit) == HTTOPLEFT) || ((hit) == HTTOPRIGHT))
#define ON_BOTTOM_BORDER(hit) \
 (((hit) == HTBOTTOM) || ((hit) == HTBOTTOMLEFT) || ((hit) == HTBOTTOMRIGHT))

#define RECT_WIDTH(a)	((a)->right - (a)->left)
#define RECT_HEIGHT(a)  ((a)->bottom - (a)->top) 
#define RECT_EQUAL(a,b) (memcmp(a, b, sizeof(RECT)) == 0)

#ifdef CUSTOM_TRACKFRAME 

/***********************************************************************
 *           draw_moving_frame
 *
 * Draw the frame used when moving or resizing window.
 *
 * FIXME:  This causes problems in Win95 mode.  (why?)
 */
static void draw_moving_frame( HDC hdc, RECT *rect, BOOL thickframe, DWORD hittest, BOOL fRedraw)
{
    if (thickframe)
    {
        const int width = GetSystemMetrics(SM_CXFRAME);
        const int height = GetSystemMetrics(SM_CYFRAME);

        static RECT oldRect = {0};

        if(fRedraw && EqualRect(&oldRect, rect)) {
            return;
        }

        HBRUSH hbrush = SelectObject( hdc, GetStockObject( GRAY_BRUSH ) );
        if(fRedraw && hittest != HTCAPTION) 
        {
             int x, y, linewidth, lineheight;

             //This should be done in a better way (less code), but at least
             //it works.
             switch(hittest) {
             case HTLEFT:
                 //clear old edge
                 PatBlt( hdc, oldRect.left, oldRect.top + height,
                         width, RECT_HEIGHT(&oldRect) - 2*height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left, rect->top + height,
                         width, RECT_HEIGHT(rect) - 2*height, PATINVERT );

                 if(oldRect.left > rect->left) {
                     x = rect->left;
                     linewidth = oldRect.left - rect->left;
                 }
                 else {
                     x = oldRect.left;
                     linewidth = rect->left - oldRect.left;
                 }
                 PatBlt(hdc, x, oldRect.top,      linewidth, height,  PATINVERT);
                 PatBlt(hdc, x, oldRect.bottom-1, linewidth, -height, PATINVERT);
                 break;
             case HTRIGHT:
                 //clear old edge
                 PatBlt( hdc, oldRect.right-1, oldRect.top + height,
                         -width, RECT_HEIGHT(&oldRect) - 2*height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->right-1, rect->top + height,
                         -width, RECT_HEIGHT(rect) - 2*height, PATINVERT );

                 if(oldRect.right > rect->right) {
                     x = rect->right;
                     linewidth = oldRect.right - rect->right;
                 }
                 else {
                     x = oldRect.right;
                     linewidth = rect->right - oldRect.right;
                 }
                 PatBlt( hdc, x, oldRect.top,      linewidth,  height, PATINVERT );
                 PatBlt( hdc, x, oldRect.bottom-1, linewidth, -height, PATINVERT );
                 break;

             case HTTOP:
                 //clear old edge
                 PatBlt( hdc, oldRect.left + width, oldRect.top,
                         RECT_WIDTH(&oldRect) - 2*width, height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left + width, rect->top,
                         RECT_WIDTH(rect) - 2*width, height, PATINVERT );

                 if(oldRect.top > rect->top) {
                     y = rect->top;
                     lineheight = oldRect.top - rect->top;
                 }
                 else {
                     y = oldRect.top;
                     lineheight = rect->top - oldRect.top;
                 }

                 PatBlt( hdc, oldRect.left,      y,  width, lineheight, PATINVERT );
                 PatBlt( hdc, oldRect.right - 1, y, -width, lineheight, PATINVERT );
                 break;

             case HTBOTTOM:
                 //clear old edge
                 PatBlt( hdc, oldRect.left + width, oldRect.bottom-1,
                         RECT_WIDTH(&oldRect) - 2*width, -height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left + width, rect->bottom-1,
                         RECT_WIDTH(rect) - 2*width, -height, PATINVERT );

                 if(oldRect.bottom > rect->bottom) {
                     y = rect->bottom;
                     lineheight = oldRect.bottom - rect->bottom;
                 }
                 else {
                     y = oldRect.bottom;
                     lineheight = rect->bottom - oldRect.bottom;
                 }

                 PatBlt( hdc, oldRect.left,      y,  width, lineheight, PATINVERT );
                 PatBlt( hdc, oldRect.right - 1, y, -width, lineheight, PATINVERT );
                 break;

             case HTBOTTOMLEFT:
                 //clear old edge (bottom)
                 PatBlt( hdc, oldRect.left, oldRect.bottom-1,
                         RECT_WIDTH(&oldRect) - width, -height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left, rect->bottom-1,
                         RECT_WIDTH(rect) - width, -height, PATINVERT );

                 //clear old edge (left)
                 PatBlt( hdc, oldRect.left, oldRect.top + height,
                         width, RECT_HEIGHT(&oldRect) - 2*height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left, rect->top + height,
                         width, RECT_HEIGHT(rect) - 2*height, PATINVERT );

                 //right
                 if(oldRect.bottom > rect->bottom) {
                     y = rect->bottom;
                     lineheight = oldRect.bottom - rect->bottom;
                 }
                 else {
                     y = oldRect.bottom;
                     lineheight = rect->bottom - oldRect.bottom;
                 }
                 PatBlt( hdc, oldRect.right-1, y,  -width, lineheight, PATINVERT );

                 //top
                 if(oldRect.left > rect->left) {
                     x = rect->left;
                     linewidth = oldRect.left - rect->left;
                 }
                 else {
                     x = oldRect.left;
                     linewidth = rect->left - oldRect.left;
                 }
                 PatBlt(hdc, x, oldRect.top, linewidth, height,  PATINVERT);
                 break;

             case HTBOTTOMRIGHT:
                 //clear old edge (bottom)
                 PatBlt( hdc, oldRect.left + width, oldRect.bottom-1,
                         RECT_WIDTH(&oldRect) - width, -height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left + width, rect->bottom-1,
                         RECT_WIDTH(rect) - width, -height, PATINVERT );

                 //clear old edge (right)
                 PatBlt( hdc, oldRect.right-1, oldRect.top + height,
                         -width, RECT_HEIGHT(&oldRect) - 2*height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->right-1, rect->top + height,
                         -width, RECT_HEIGHT(rect) - 2*height, PATINVERT );

                 //left
                 if(oldRect.bottom > rect->bottom) {
                     y = rect->bottom;
                     lineheight = oldRect.bottom - rect->bottom;
                 }
                 else {
                     y = oldRect.bottom;
                     lineheight = rect->bottom - oldRect.bottom;
                 }
                 PatBlt( hdc, oldRect.left, y, width, lineheight, PATINVERT );

                 //top
                 if(oldRect.right > rect->right) {
                     x = rect->right;
                     linewidth = oldRect.right - rect->right;
                 }
                 else {
                     x = oldRect.right;
                     linewidth = rect->right - oldRect.right;
                 }
                 PatBlt(hdc, x, oldRect.top, linewidth, height,  PATINVERT);
                 break;

             case HTTOPLEFT:
                 //clear old edge (top)
                 PatBlt( hdc, oldRect.left, oldRect.top,
                         RECT_WIDTH(&oldRect) - width, height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left, rect->top,
                         RECT_WIDTH(rect) - width, height, PATINVERT );

                 //clear old edge (left)
                 PatBlt( hdc, oldRect.left, oldRect.top + height,
                         width, RECT_HEIGHT(&oldRect) - 2*height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left, rect->top + height,
                         width, RECT_HEIGHT(rect) - 2*height, PATINVERT );

                 //right
                 if(oldRect.top > rect->top) {
                     y = rect->top;
                     lineheight = oldRect.top - rect->top;
                 }
                 else {
                     y = oldRect.top;
                     lineheight = rect->top - oldRect.top;
                 }
                 PatBlt( hdc, oldRect.right-1, y,  -width, lineheight, PATINVERT );

                 //bottom
                 if(oldRect.left > rect->left) {
                     x = rect->left;
                     linewidth = oldRect.left - rect->left;
                 }
                 else {
                     x = oldRect.left;
                     linewidth = rect->left - oldRect.left;
                 }
                 PatBlt(hdc, x, oldRect.bottom-1, linewidth, -height,  PATINVERT);
                 break;

             case HTTOPRIGHT:
                 //clear old edge (top)
                 PatBlt( hdc, oldRect.left+width, oldRect.top,
                         RECT_WIDTH(&oldRect) - width, height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->left+width, rect->top,
                         RECT_WIDTH(rect) - width, height, PATINVERT );

                 //clear old edge (right)
                 PatBlt( hdc, oldRect.right-1, oldRect.top + height,
                         -width, RECT_HEIGHT(&oldRect) - 2*height, PATINVERT );
                 //and draw new one
                 PatBlt( hdc, rect->right-1, rect->top + height,
                         -width, RECT_HEIGHT(rect) - 2*height, PATINVERT );

                 //left
                 if(oldRect.top > rect->top) {
                     y = rect->top;
                     lineheight = oldRect.top - rect->top;
                 }
                 else {
                     y = oldRect.top;
                     lineheight = rect->top - oldRect.top;
                 }
                 PatBlt( hdc, oldRect.left, y,  width, lineheight, PATINVERT);

                 //bottom
                 if(oldRect.right > rect->right) {
                     x = rect->right;
                     linewidth = oldRect.right - rect->right;
                 }
                 else {
                     x = oldRect.right;
                     linewidth = rect->right - oldRect.right;
                 }
                 PatBlt(hdc, x, oldRect.bottom-1, linewidth, -height,  PATINVERT);
                 break;
             }
             oldRect = *rect;
        }
        else { 
             if(fRedraw) {
                 PatBlt( hdc, oldRect.left, oldRect.top,
                         RECT_WIDTH(&oldRect) - width, height, PATINVERT );
                 PatBlt( hdc, oldRect.left, oldRect.top + height, width,
                         RECT_HEIGHT(&oldRect) - height, PATINVERT );
                 PatBlt( hdc, oldRect.left + width, oldRect.bottom - 1,
                         RECT_WIDTH(&oldRect) - width, -height, PATINVERT );
                 PatBlt( hdc, oldRect.right - 1, oldRect.top, -width,
                         RECT_HEIGHT(&oldRect) - height, PATINVERT );
             }
             oldRect = *rect;
             PatBlt( hdc, rect->left, rect->top,
                     rect->right - rect->left - width, height, PATINVERT );
             PatBlt( hdc, rect->left, rect->top + height, width,
                     rect->bottom - rect->top - height, PATINVERT );
             PatBlt( hdc, rect->left + width, rect->bottom - 1,
                     rect->right - rect->left - width, -height, PATINVERT );
             PatBlt( hdc, rect->right - 1, rect->top, -width,
                     rect->bottom - rect->top - height, PATINVERT );
        }
        SelectObject( hdc, hbrush );
    }
    else DrawFocusRect( hdc, rect );
}


/***********************************************************************
 *           start_size_move
 *
 * Initialisation of a move or resize, when initiatied from a menu choice.
 * Return hit test code for caption or sizing border.
 */
static LONG start_size_move( Win32BaseWindow *win32wnd, WPARAM wParam, POINT *capturePoint, LONG style )
{
    HWND hwnd = win32wnd->getWindowHandle();
    LONG hittest = 0;
    POINT pt;
    MSG msg;
    RECT rectWindow;

    GetWindowRect( hwnd, &rectWindow );

    if ((wParam & 0xfff0) == SC_MOVE)
    {
        /* Move pointer at the center of the caption */
        RECT rect;
        win32wnd->GetInsideRect( &rect );
        if (style & WS_SYSMENU)
            rect.left += GetSystemMetrics(SM_CXSIZE) + 1;
        if (style & WS_MINIMIZEBOX)
            rect.right -= GetSystemMetrics(SM_CXSIZE) + 1;
        if (style & WS_MAXIMIZEBOX)
            rect.right -= GetSystemMetrics(SM_CXSIZE) + 1;
        pt.x = rectWindow.left + (rect.right - rect.left) / 2;
        pt.y = rectWindow.top + rect.top + GetSystemMetrics(SM_CYSIZE)/2;
        hittest = HTCAPTION;
        *capturePoint = pt;
    }
    else  /* SC_SIZE */
    {
        while(!hittest)
        {
//            GetMessageW( &msg, 0, WM_KEYFIRST, WM_MOUSELAST );
            GetMessageW( &msg, 0, 0, 0 );
            if (CallMsgFilterW( &msg, MSGF_SIZE )) continue;

            switch(msg.message)
            {
            case WM_MOUSEMOVE:
                hittest = win32wnd->HandleNCHitTest( msg.pt );
//                hittest = NC_HandleNCHitTest( hwnd, msg.pt );
                if ((hittest < HTLEFT) || (hittest > HTBOTTOMRIGHT))
                    hittest = 0;
                break;

            case WM_LBUTTONUP:
                return 0;

            case WM_KEYDOWN:
                switch(msg.wParam)
                {
                case VK_UP:
                    hittest = HTTOP;
                    pt.x =(rectWindow.left+rectWindow.right)/2;
                    pt.y = rectWindow.top + GetSystemMetrics(SM_CYFRAME) / 2;
                    break;
                case VK_DOWN:
                    hittest = HTBOTTOM;
                    pt.x =(rectWindow.left+rectWindow.right)/2;
                    pt.y = rectWindow.bottom - GetSystemMetrics(SM_CYFRAME) / 2;
                    break;
                case VK_LEFT:
                    hittest = HTLEFT;
                    pt.x = rectWindow.left + GetSystemMetrics(SM_CXFRAME) / 2;
                    pt.y =(rectWindow.top+rectWindow.bottom)/2;
                    break;
                case VK_RIGHT:
                    hittest = HTRIGHT;
                    pt.x = rectWindow.right - GetSystemMetrics(SM_CXFRAME) / 2;
                    pt.y =(rectWindow.top+rectWindow.bottom)/2;
                    break;
                case VK_RETURN:
                case VK_ESCAPE: return 0;
                }
            }
        }
        *capturePoint = pt;
    }
    SetCursorPos( pt.x, pt.y );
    win32wnd->DefWindowProcA(WM_SETCURSOR, (WPARAM)hwnd, MAKELONG( hittest, WM_MOUSEMOVE ));
//    NC_HandleSetCursor( hwnd, (WPARAM)hwnd, MAKELONG( hittest, WM_MOUSEMOVE ));
    dprintf(("start_size_move: hittest %d", hittest));
    return hittest;
}


/***********************************************************************
 *           SysCommandSizeMove   (X11DRV.@)
 *
 * Perform SC_MOVE and SC_SIZE commands.
 */
void Frame_SysCommandSizeMove(Win32BaseWindow *win32wnd, WPARAM wParam )
{
    HWND hwnd = win32wnd->getWindowHandle();
    MSG msg;
    RECT sizingRect, mouseRect, origRect, lastsizingRect;
    HDC hdc;
    HWND parent;
    LONG hittest = (LONG)(wParam & 0x0f);
    HCURSOR16 hDragCursor = 0, hOldCursor = 0;
    POINT minTrack, maxTrack;
    POINT capturePoint, pt;
    LONG style = GetWindowLongA( hwnd, GWL_STYLE );
    LONG exstyle = GetWindowLongA( hwnd, GWL_EXSTYLE );
    BOOL    thickframe = HAS_THICKFRAME( style, exstyle );
    BOOL    iconic = style & WS_MINIMIZE;
    BOOL    moved = FALSE;
    DWORD     dwPoint = GetMessagePos ();
    BOOL DragFullWindows = FALSE;
    BOOL grab;
    int iWndsLocks;

    dprintf(("FrameTrackFrame %x", wParam));

    SystemParametersInfoA(SPI_GETDRAGFULLWINDOWS, 0, &DragFullWindows, 0);
  
    pt.x = SLOWORD(dwPoint);
    pt.y = SHIWORD(dwPoint);
    capturePoint = pt;

//    if (IsZoomed(hwnd) || !IsWindowVisible(hwnd) || (exstyle & WS_EX_MANAGED)) return;
    if (IsZoomed(hwnd) || !IsWindowVisible(hwnd)) return;

    if ((wParam & 0xfff0) == SC_MOVE)
    {
        if (!hittest) hittest = start_size_move( win32wnd, wParam, &capturePoint, style );
        if (!hittest) return;
    }
    else  /* SC_SIZE */
    {
        if (!thickframe) return;
        if ( hittest && hittest != HTSYSMENU ) hittest += 2;
        else
        {
            SetCapture(hwnd);
            hittest = start_size_move( win32wnd, wParam, &capturePoint, style );
            if (!hittest)
            {
                ReleaseCapture();
                return;
            }
        }
    }

      /* Get min/max info */

//    WINPOS_GetMinMaxInfo( hwnd, NULL, NULL, &minTrack, &maxTrack );
    win32wnd->AdjustTrackInfo(&minTrack, &maxTrack);
    GetWindowRect( hwnd, &sizingRect );
    if (style & WS_CHILD)
    {
        parent = GetParent(hwnd);
        /* make sizing rect relative to parent */
        MapWindowPoints( 0, parent, (POINT*)&sizingRect, 2 );
        GetClientRect( parent, &mouseRect );
    }
    else
    {
        parent = GetDesktopWindow();
        SetRect(&mouseRect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }
    origRect = sizingRect;

    if (ON_LEFT_BORDER(hittest))
    {
        mouseRect.left  = max( mouseRect.left, sizingRect.right-maxTrack.x );
        mouseRect.right = min( mouseRect.right, sizingRect.right-minTrack.x );
    }
    else if (ON_RIGHT_BORDER(hittest))
    {
        mouseRect.left  = max( mouseRect.left, sizingRect.left+minTrack.x );
        mouseRect.right = min( mouseRect.right, sizingRect.left+maxTrack.x );
    }
    if (ON_TOP_BORDER(hittest))
    {
        mouseRect.top    = max( mouseRect.top, sizingRect.bottom-maxTrack.y );
        mouseRect.bottom = min( mouseRect.bottom,sizingRect.bottom-minTrack.y);
    }
    else if (ON_BOTTOM_BORDER(hittest))
    {
        mouseRect.top    = max( mouseRect.top, sizingRect.top+minTrack.y );
        mouseRect.bottom = min( mouseRect.bottom, sizingRect.top+maxTrack.y );
    }
    if (parent) MapWindowPoints( parent, 0, (LPPOINT)&mouseRect, 2 );

    /* Retrieve a default cache DC (without using the window style) */
    hdc = GetDCEx( parent, 0, DCX_CACHE);

    if( iconic ) /* create a cursor for dragging */
    {
        HICON hIcon = GetClassLongA( hwnd, GCL_HICON);
        if(!hIcon) hIcon = (HICON)SendMessageA( hwnd, WM_QUERYDRAGICON, 0, 0L);
        if( hIcon ) hDragCursor = hIcon;
// CURSORICON_IconToCursor( hIcon, TRUE );
        if( !hDragCursor ) iconic = FALSE;
    }

    /* repaint the window before moving it around */
    RedrawWindow( hwnd, NULL, 0, RDW_UPDATENOW | RDW_ALLCHILDREN );

    SendMessageA( hwnd, WM_ENTERSIZEMOVE, 0, 0 );
    SetCapture( hwnd );

    //prevent the app from drawing to this window (or its children)    
    if(!DragFullWindows) 
        LockWindowUpdate(hwnd);

    while(1)
    {
        int dx = 0, dy = 0;

//        if (!GetMessageW( &msg, 0, WM_KEYFIRST, WM_MOUSELAST )) break;
        if (!GetMessageW( &msg, 0, 0, 0 )) break;
        if (CallMsgFilterW( &msg, MSGF_SIZE )) continue;

        /* Exit on button-up, Return, or Esc */
        if ((msg.message == WM_LBUTTONUP) ||
            ((msg.message == WM_KEYDOWN) &&
             ((msg.wParam == VK_RETURN) || (msg.wParam == VK_ESCAPE)))) break;

        if ((msg.message != WM_KEYDOWN) && (msg.message != WM_MOUSEMOVE)) {
            DispatchMessageA(&msg);
            continue;  /* We are not interested in other messages */
        }

        pt = msg.pt;

        if (msg.message == WM_KEYDOWN) switch(msg.wParam)
        {
        case VK_UP:    pt.y -= 8; break;
        case VK_DOWN:  pt.y += 8; break;
        case VK_LEFT:  pt.x -= 8; break;
        case VK_RIGHT: pt.x += 8; break;
        }

        pt.x = max( pt.x, mouseRect.left );
        pt.x = min( pt.x, mouseRect.right );
        pt.y = max( pt.y, mouseRect.top );
        pt.y = min( pt.y, mouseRect.bottom );

        dprintf(("mouseRect (%d,%d)(%d,%d)", mouseRect.left, mouseRect.top, mouseRect.right, mouseRect.bottom));
        dprintf(("capturePoint (%d,%d)", capturePoint.x, capturePoint.y));
        dx = pt.x - capturePoint.x;
        dy = pt.y - capturePoint.y;

        if (dx || dy)
        {
            if( !moved )
            {
                moved = TRUE;

                if( iconic ) /* ok, no system popup tracking */
                {
                    hOldCursor = SetCursor(hDragCursor);
                    ShowCursor( TRUE );
//                    WINPOS_ShowIconTitle( hwnd, FALSE );
                }
                else if(!DragFullWindows)
                    draw_moving_frame( hdc, &sizingRect, thickframe, hittest, FALSE );
            }

            if (msg.message == WM_KEYDOWN) SetCursorPos( pt.x, pt.y );
            else
            {
                RECT newRect = sizingRect, tempRect;
                WPARAM wpSizingHit = 0;

                if (hittest == HTCAPTION) OffsetRect( &newRect, dx, dy );

                if (ON_LEFT_BORDER(hittest)) newRect.left += dx;
                else 
                if (ON_RIGHT_BORDER(hittest)) newRect.right += dx;

                if (ON_TOP_BORDER(hittest)) newRect.top += dy;
                else 
                if (ON_BOTTOM_BORDER(hittest)) newRect.bottom += dy;

////                if(!iconic && !DragFullWindows) draw_moving_frame( hdc, &sizingRect, thickframe, hittest, TRUE);

                /* determine the hit location */
                if (hittest >= HTLEFT && hittest <= HTBOTTOMRIGHT)
                    wpSizingHit = WMSZ_LEFT + (hittest - HTLEFT);

                tempRect = newRect;

                dprintf(("WM_SIZING rect (%d,%d)(%d,%d)", newRect.left, newRect.top, newRect.right, newRect.bottom));

                //Although the docs say the app should return TRUE when processing
                //this message, experiments show that NT checks the rectangle
                //regardless of the return value
                if ((wParam & 0xfff0) == SC_MOVE) {
                     SendMessageA( hwnd, WM_MOVING, wpSizingHit, (LPARAM)&newRect );
                }
                else SendMessageA( hwnd, WM_SIZING, wpSizingHit, (LPARAM)&newRect );

                dprintf(("WM_SIZING rect (%d,%d)(%d,%d)", newRect.left, newRect.top, newRect.right, newRect.bottom));

                dprintf(("update capture point dx %d dy %d", dx, dy));
                capturePoint   = pt;
                sizingRect     = tempRect;
                lastsizingRect = newRect;

                if (!iconic)
                {
                    if(!DragFullWindows)
                        draw_moving_frame( hdc, &newRect, thickframe, hittest, TRUE );
                    else {
                        /* To avoid any deadlocks, all the locks on the windows
			   structures must be suspended before the SetWindowPos */
//                        iWndsLocks = WIN_SuspendWndsLock();
                        SetWindowPos( hwnd, 0, newRect.left, newRect.top,
                                      newRect.right - newRect.left,
                                      newRect.bottom - newRect.top,
                                      ( hittest == HTCAPTION ) ? SWP_NOSIZE : 0 );
//                        WIN_RestoreWndsLock(iWndsLocks);
                    }
                }
            }
        }
    }

    //Enable window update
    if(!DragFullWindows) 
        LockWindowUpdate(NULL);

    ReleaseCapture();
    if( iconic )
    {
        if( moved ) /* restore cursors, show icon title later on */
        {
            ShowCursor( FALSE );
            SetCursor( hOldCursor );
        }
        DestroyCursor( hDragCursor );
    }
    else if (moved && !DragFullWindows)
        draw_moving_frame( hdc, &lastsizingRect, thickframe, hittest, FALSE);

    ReleaseDC( parent, hdc );

//    wine_tsx11_lock();
//    XUngrabPointer( display, CurrentTime );
//    if (grab)
//    {
//        XSync( display, False );
//        XUngrabServer( display );
//        XSync( display, False );
//        gdi_display = old_gdi_display;
//    }
//    wine_tsx11_unlock();

    if (HOOK_CallHooksA( WH_CBT, HCBT_MOVESIZE, (WPARAM)hwnd, (LPARAM)&lastsizingRect )) moved = FALSE;

    SendMessageA( hwnd, WM_EXITSIZEMOVE, 0, 0 );
    SendMessageA( hwnd, WM_SETVISIBLE, !IsIconic(hwnd), 0L);

    /* window moved or resized */
    if (moved)
    {
        /* To avoid any deadlocks, all the locks on the windows
	   structures must be suspended before the SetWindowPos */
//        iWndsLocks = WIN_SuspendWndsLock();

        /* if the moving/resizing isn't canceled call SetWindowPos
         * with the new position or the new size of the window
         */
        if (!((msg.message == WM_KEYDOWN) && (msg.wParam == VK_ESCAPE)) )
        {
            /* NOTE: SWP_NOACTIVATE prevents document window activation in Word 6 */
            if(!DragFullWindows)
                SetWindowPos( hwnd, 0, lastsizingRect.left, lastsizingRect.top,
                              lastsizingRect.right - lastsizingRect.left,
                              lastsizingRect.bottom - lastsizingRect.top,
                              ( hittest == HTCAPTION ) ? SWP_NOSIZE : 0 );
        }
        else
        { /* restore previous size/position */
            if(DragFullWindows)
                SetWindowPos( hwnd, 0, origRect.left, origRect.top,
                              origRect.right - origRect.left,
                              origRect.bottom - origRect.top,
                              ( hittest == HTCAPTION ) ? SWP_NOSIZE : 0 );
        }

//        WIN_RestoreWndsLock(iWndsLocks);
    }

    if (IsIconic(hwnd))
    {
        /* Single click brings up the system menu when iconized */

        if( !moved )
        {
            if(style & WS_SYSMENU )
                SendMessageA( hwnd, WM_SYSCOMMAND,
                              SC_MOUSEMENU + HTSYSMENU, MAKELONG(pt.x,pt.y));
        }
//        else WINPOS_ShowIconTitle( hwnd, TRUE );
    }
}

#endif //#ifdef CUSTOM_TRACKFRAME 
