/* $Id: scroll.cpp,v 1.14 1999-10-23 23:04:38 sandervl Exp $ */
/*
 * Scrollbar control
 *
 * Copyright 1999 Christoph Bratschi
 *
 * Copyright 1993 Martin Ayotte
 * Copyright 1994, 1996 Alexandre Julliard
 *
 * WINE version: 990923
 */

#include <stdlib.h>
#include <os2win.h>
#include "controls.h"
#include "scroll.h"
#include "win32wbase.h"
#include "oslibwin.h"
#include "initterm.h"

static BOOL bitmapsLoaded = FALSE;

static HBITMAP hUpArrow = 0;
static HBITMAP hDnArrow = 0;
static HBITMAP hLfArrow = 0;
static HBITMAP hRgArrow = 0;
static HBITMAP hUpArrowD = 0;
static HBITMAP hDnArrowD = 0;
static HBITMAP hLfArrowD = 0;
static HBITMAP hRgArrowD = 0;
static HBITMAP hUpArrowI = 0;
static HBITMAP hDnArrowI = 0;
static HBITMAP hLfArrowI = 0;
static HBITMAP hRgArrowI = 0;

#define TOP_ARROW(flags,pressed) \
   (((flags)&ESB_DISABLE_UP) ? hUpArrowI : ((pressed) ? hUpArrowD:hUpArrow))
#define BOTTOM_ARROW(flags,pressed) \
   (((flags)&ESB_DISABLE_DOWN) ? hDnArrowI : ((pressed) ? hDnArrowD:hDnArrow))
#define LEFT_ARROW(flags,pressed) \
   (((flags)&ESB_DISABLE_LEFT) ? hLfArrowI : ((pressed) ? hLfArrowD:hLfArrow))
#define RIGHT_ARROW(flags,pressed) \
   (((flags)&ESB_DISABLE_RIGHT) ? hRgArrowI : ((pressed) ? hRgArrowD:hRgArrow))

#define SCROLL_MIN_RECT  4 /* Minimum size of the rectangle between the arrows */
#define SCROLL_MIN_THUMB 6 /* Minimum size of the thumb in pixels */

#define SCROLL_ARROW_THUMB_OVERLAP 0 /* Overlap between arrows and thumb */

#define SCROLL_FIRST_DELAY   200 /* Delay (in ms) before first repetition when holding the button down */
#define SCROLL_REPEAT_DELAY  50  /* Delay (in ms) between scroll repetitions */
#define SCROLL_BLINK_DELAY   1000

#define SCROLL_TIMER   0 /* Scroll timer id */
#define BLINK_TIMER    1

  /* Scroll-bar hit testing */
enum SCROLL_HITTEST
{
    SCROLL_NOWHERE,      /* Outside the scroll bar */
    SCROLL_TOP_ARROW,    /* Top or left arrow */
    SCROLL_TOP_RECT,     /* Rectangle between the top arrow and the thumb */
    SCROLL_THUMB,        /* Thumb rectangle */
    SCROLL_BOTTOM_RECT,  /* Rectangle between the thumb and the bottom arrow */
    SCROLL_BOTTOM_ARROW  /* Bottom or right arrow */
};

 /* What to do after SCROLL_SetScrollInfo() */
#define SA_SSI_HIDE             0x0001
#define SA_SSI_SHOW             0x0002
#define SA_SSI_REFRESH          0x0004
#define SA_SSI_REPAINT_ARROWS   0x0008

 /* Thumb-tracking info */
static HWND SCROLL_TrackingWin = 0;
static INT  SCROLL_TrackingBar = 0;
static INT  SCROLL_TrackingPos = 0;
static INT  SCROLL_TrackingVal = 0;
 /* Focus info */
static HWND SCROLL_FocusWin    = 0;
static BOOL SCROLL_HasFocus    = FALSE;
static BOOL SCROLL_Highlighted = FALSE;

 /* Hit test code of the last button-down event */
static enum SCROLL_HITTEST SCROLL_trackHitTest;
static BOOL SCROLL_trackVertical;

 /* Is the moving thumb being displayed? */
static BOOL SCROLL_MovingThumb = FALSE;

 /* Local functions */
static BOOL SCROLL_ShowScrollBar( HWND hwnd, INT nBar,
                                    BOOL fShowH, BOOL fShowV );
static INT SCROLL_SetScrollInfo( HWND hwnd, INT nBar,
                                   const SCROLLINFO *info, INT *action );
static void SCROLL_DrawInterior_9x( HWND hwnd, HDC hdc, INT nBar,
                    RECT *rect, INT arrowSize,
                    INT thumbSize, INT thumbPos,
                    UINT flags, BOOL vertical,
                    BOOL top_selected, BOOL bottom_selected );

/***********************************************************************
 *           SCROLL_LoadBitmaps
 */
static void SCROLL_LoadBitmaps(void)
{
    if (bitmapsLoaded) return;

    hUpArrow  = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_UPARROW) );
    hDnArrow  = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_DNARROW) );
    hLfArrow  = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_LFARROW) );
    hRgArrow  = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_RGARROW) );
    hUpArrowD = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_UPARROWD) );
    hDnArrowD = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_DNARROWD) );
    hLfArrowD = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_LFARROWD) );
    hRgArrowD = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_RGARROWD) );
    hUpArrowI = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_UPARROWI) );
    hDnArrowI = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_DNARROWI) );
    hLfArrowI = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_LFARROWI) );
    hRgArrowI = LoadBitmapA( hInstanceUser32, MAKEINTRESOURCEA(OBM_RGARROWI) );

    bitmapsLoaded = TRUE;
}

/***********************************************************************
 *           SCROLL_GetInfoPtr
 */
static SCROLLBAR_INFO *SCROLL_GetInfoPtr( HWND hwnd, INT nBar )
{
    Win32BaseWindow *win32wnd;
    HWND hwndFrame;

    switch(nBar)
    {
        case SB_HORZ:
        case SB_VERT:
          win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
          if (!win32wnd) return NULL;
          hwndFrame = OSLibWinQueryWindow(win32wnd->getOS2WindowHandle(),QWOS_PARENT);
          win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndFrame);
          if (!win32wnd) return NULL;
          return win32wnd->getScrollInfo(nBar);

        case SB_CTL:
          return (SCROLLBAR_INFO*)GetInfoPtr(hwnd);
    }

    return NULL;
}

/* Scrollbar Functions */

/***********************************************************************
 *           SCROLL_GetScrollBarRect
 *
 * Compute the scroll bar rectangle, in drawing coordinates (i.e. client
 * coords for SB_CTL, window coords for SB_VERT and SB_HORZ).
 * 'arrowSize' returns the width or height of an arrow (depending on
 * the orientation of the scrollbar), 'thumbSize' returns the size of
 * the thumb, and 'thumbPos' returns the position of the thumb
 * relative to the left or to the top.
 * Return TRUE if the scrollbar is vertical, FALSE if horizontal.
 */
static BOOL SCROLL_GetScrollBarRect( HWND hwnd, INT nBar, RECT *lprect,
                                       INT *arrowSize, INT *thumbSize,
                                       INT *thumbPos )
{
    INT pixels;
    BOOL vertical;
    RECT rectClient;

    switch(nBar)
    {
      case SB_HORZ:
      case SB_VERT:
        GetClientRect( hwnd, lprect );
        vertical = (nBar == SB_VERT);
        break;

      case SB_CTL:
        {
          DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

          GetClientRect( hwnd, lprect );
          vertical = ((dwStyle & SBS_VERT) != 0);
          break;
        }

    default:
        return FALSE;
    }

    if (vertical) pixels = lprect->bottom - lprect->top;
    else pixels = lprect->right - lprect->left;

    if (pixels <= 2*GetSystemMetrics(SM_CXVSCROLL) + SCROLL_MIN_RECT)
    {
        if (pixels > SCROLL_MIN_RECT)
            *arrowSize = (pixels - SCROLL_MIN_RECT) / 2;
        else
            *arrowSize = 0;
        *thumbPos = *thumbSize = 0;
    }
    else
    {
        SCROLLBAR_INFO *info = SCROLL_GetInfoPtr( hwnd, nBar );

        *arrowSize = GetSystemMetrics(SM_CXVSCROLL);
        pixels -= (2 * (GetSystemMetrics(SM_CXVSCROLL) - SCROLL_ARROW_THUMB_OVERLAP));

        if (info->Page)
        {
            *thumbSize = pixels * info->Page / (info->MaxVal-info->MinVal+1);
            if (*thumbSize < SCROLL_MIN_THUMB) *thumbSize = SCROLL_MIN_THUMB;
        }
        else *thumbSize = GetSystemMetrics(SM_CXVSCROLL);

        if (((pixels -= *thumbSize ) < 0) ||
            ((info->flags & ESB_DISABLE_BOTH) == ESB_DISABLE_BOTH))
        {
            /* Rectangle too small or scrollbar disabled -> no thumb */
            *thumbPos = *thumbSize = 0;
        }
        else
        {
            INT max = info->MaxVal - MAX( info->Page-1, 0 );
            if (info->MinVal >= max)
                *thumbPos = *arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
            else
                *thumbPos = *arrowSize - SCROLL_ARROW_THUMB_OVERLAP
                 + pixels * (info->CurVal-info->MinVal) / (max - info->MinVal);
        }
    }
    return vertical;
}

/***********************************************************************
 *           SCROLL_PtInRectEx
 */
static BOOL SCROLL_PtInRectEx( LPRECT lpRect, POINT pt, BOOL vertical )
{
    RECT rect = *lpRect;

    if (vertical)
    {
        rect.left -= lpRect->right - lpRect->left;
        rect.right += lpRect->right - lpRect->left;
    }
    else
    {
        rect.top -= lpRect->bottom - lpRect->top;
        rect.bottom += lpRect->bottom - lpRect->top;
    }
    return PtInRect( &rect, pt );
}

/***********************************************************************
 *           SCROLL_HitTest
 *
 * Scroll-bar hit testing (don't confuse this with WM_NCHITTEST!).
 */
static enum SCROLL_HITTEST SCROLL_HitTest( HWND hwnd, INT nBar,
                                           POINT pt, BOOL bDragging )
{
    INT arrowSize, thumbSize, thumbPos;
    RECT rect;

    BOOL vertical = SCROLL_GetScrollBarRect( hwnd, nBar, &rect,
                                           &arrowSize, &thumbSize, &thumbPos );

    if ( (bDragging && !SCROLL_PtInRectEx( &rect, pt, vertical )) ||
         (!PtInRect( &rect, pt )) ) return SCROLL_NOWHERE;

    if (vertical)
    {
        if (pt.y < rect.top + arrowSize) return SCROLL_TOP_ARROW;
        if (pt.y >= rect.bottom - arrowSize) return SCROLL_BOTTOM_ARROW;
        if (!thumbPos) return SCROLL_TOP_RECT;
        pt.y -= rect.top;
        if (pt.y < thumbPos) return SCROLL_TOP_RECT;
        if (pt.y >= thumbPos + thumbSize) return SCROLL_BOTTOM_RECT;
    }
    else  /* horizontal */
    {
        if (pt.x < rect.left + arrowSize) return SCROLL_TOP_ARROW;
        if (pt.x >= rect.right - arrowSize) return SCROLL_BOTTOM_ARROW;
        if (!thumbPos) return SCROLL_TOP_RECT;
        pt.x -= rect.left;
        if (pt.x < thumbPos) return SCROLL_TOP_RECT;
        if (pt.x >= thumbPos + thumbSize) return SCROLL_BOTTOM_RECT;
    }
    return SCROLL_THUMB;
}

/***********************************************************************
 *           SCROLL_DrawArrows
 *
 * Draw the scroll bar arrows.
 */
static void SCROLL_DrawArrows( HDC hdc, SCROLLBAR_INFO *infoPtr,
                               RECT *rect, INT arrowSize, BOOL vertical,
                               BOOL top_pressed, BOOL bottom_pressed )
{
  RECT r;

  r = *rect;
  if( vertical )
    r.bottom = r.top + arrowSize;
  else
    r.right = r.left + arrowSize;

  DrawFrameControl( hdc, &r, DFC_SCROLL,
            (vertical ? DFCS_SCROLLUP : DFCS_SCROLLLEFT)
            | (top_pressed ? (DFCS_PUSHED | DFCS_FLAT) : 0 )
            | (infoPtr->flags&ESB_DISABLE_LTUP ? DFCS_INACTIVE : 0 ) );

  r = *rect;
  if( vertical )
    r.top = r.bottom-arrowSize;
  else
    r.left = r.right-arrowSize;

  DrawFrameControl( hdc, &r, DFC_SCROLL,
            (vertical ? DFCS_SCROLLDOWN : DFCS_SCROLLRIGHT)
            | (bottom_pressed ? (DFCS_PUSHED | DFCS_FLAT) : 0 )
            | (infoPtr->flags&ESB_DISABLE_RTDN ? DFCS_INACTIVE : 0) );
}

/***********************************************************************
 *           SCROLL_DrawMovingThumb
 *
 * Draw the moving thumb rectangle.
 */
static void SCROLL_DrawMovingThumb( HDC hdc, RECT *rect, BOOL vertical,
                       INT arrowSize, INT thumbSize )
{
  INT pos = SCROLL_TrackingPos;
  INT max_size;

  if( vertical )
    max_size = rect->bottom - rect->top;
  else
    max_size = rect->right - rect->left;

  max_size -= (arrowSize-SCROLL_ARROW_THUMB_OVERLAP) + thumbSize;

  if( pos < (arrowSize-SCROLL_ARROW_THUMB_OVERLAP) )
    pos = (arrowSize-SCROLL_ARROW_THUMB_OVERLAP);
  else if( pos > max_size )
    pos = max_size;

  SCROLL_DrawInterior_9x( SCROLL_TrackingWin, hdc, SCROLL_TrackingBar,
              rect, arrowSize, thumbSize, pos,
              0, vertical, FALSE, FALSE );

  SCROLL_MovingThumb = !SCROLL_MovingThumb;
}

static void SCROLL_DrawInterior( HWND hwnd, HDC hdc, INT nBar,
                                 RECT *rect, INT arrowSize,
                                 INT thumbSize, INT thumbPos,
                                 UINT flags, BOOL vertical,
                                 BOOL top_selected, BOOL bottom_selected )
{
    RECT r;
    HPEN hSavePen;
    HBRUSH hSaveBrush,hBrush;
    BOOL Save_SCROLL_MovingThumb = SCROLL_MovingThumb;

//SCROLL_DrawInterior_9x(hwnd,hdc,nBar,rect,arrowSize,thumbSize,thumbPos,flags,vertical,top_selected,bottom_selected);
//return; //CB: improve!

    if (Save_SCROLL_MovingThumb &&
        (SCROLL_TrackingWin == hwnd) &&
        (SCROLL_TrackingBar == nBar))
        SCROLL_DrawMovingThumb( hdc, rect, vertical, arrowSize, thumbSize );

      /* Select the correct brush and pen */

    /* Only scrollbar controls send WM_CTLCOLORSCROLLBAR.
     * The window-owned scrollbars need to call DEFWND_ControlColor
     * to correctly setup default scrollbar colors
     */
    if (nBar == SB_CTL) {
        hBrush = (HBRUSH)SendMessageA( GetParent(hwnd), WM_CTLCOLORSCROLLBAR,
                                       (WPARAM)hdc,(LPARAM)hwnd);
    } else {
        hBrush = (HBRUSH)SendMessageA( hwnd, WM_CTLCOLORSCROLLBAR,
                                       (WPARAM)hdc,(LPARAM)hwnd);

    }

    hSavePen = SelectObject( hdc, GetSysColorPen(COLOR_WINDOWFRAME) );
    hSaveBrush = SelectObject( hdc, hBrush );

      /* Calculate the scroll rectangle */

    r = *rect;
    if (vertical)
    {
        r.top    += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
        r.bottom -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    }
    else
    {
        r.left  += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
        r.right -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    }

      /* Draw the scroll rectangles and thumb */

    if (!thumbPos)  /* No thumb to draw */
    {
        INT offset = 0;

        PatBlt( hdc, r.left+offset, r.top+offset,
                     r.right - r.left - 2*offset, r.bottom - r.top - 2*offset,
                     PATCOPY );

        /* cleanup and return */
        SelectObject( hdc, hSavePen );
        SelectObject( hdc, hSaveBrush );
        return;
    }

    if (vertical)
    {
        INT offset = 0;

        PatBlt( hdc, r.left + offset, r.top + offset,
                  r.right - r.left - offset*2,
                  thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP) - offset,
                  top_selected ? 0x0f0000 : PATCOPY );
        r.top += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
        PatBlt( hdc, r.left + offset, r.top + thumbSize,
                  r.right - r.left - offset*2,
                  r.bottom - r.top - thumbSize - offset,
                  bottom_selected ? 0x0f0000 : PATCOPY );
        r.bottom = r.top + thumbSize;
    }
    else  /* horizontal */
    {
        INT offset = 0;

        PatBlt( hdc, r.left + offset, r.top + offset,
                  thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
                  r.bottom - r.top - offset*2,
                  top_selected ? 0x0f0000 : PATCOPY );
        r.left += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
        PatBlt( hdc, r.left + thumbSize, r.top + offset,
                  r.right - r.left - thumbSize - offset,
                  r.bottom - r.top - offset*2,
                  bottom_selected ? 0x0f0000 : PATCOPY );
        r.right = r.left + thumbSize;
    }

      /* Draw the thumb */

    SelectObject(hdc,(SCROLL_FocusWin == hwnd && SCROLL_Highlighted) ? GetSysColorBrush(COLOR_3DHILIGHT):GetSysColorBrush(COLOR_BTNFACE));

    Rectangle(hdc,r.left+1,r.top+1,r.right-1,r.bottom-1);

    DrawEdge( hdc, &r, EDGE_RAISED, BF_RECT );

    if (Save_SCROLL_MovingThumb &&
        (SCROLL_TrackingWin == hwnd) &&
        (SCROLL_TrackingBar == nBar))
        SCROLL_DrawMovingThumb( hdc, rect, vertical, arrowSize, thumbSize );

    /* cleanup */
    SelectObject( hdc, hSavePen );
    SelectObject( hdc, hSaveBrush );
}

/***********************************************************************
 *           SCROLL_ClipPos
 */
static POINT SCROLL_ClipPos( LPRECT lpRect, POINT pt )
{
    if( pt.x < lpRect->left )
        pt.x = lpRect->left;
    else
    if( pt.x > lpRect->right )
        pt.x = lpRect->right;

    if( pt.y < lpRect->top )
        pt.y = lpRect->top;
    else
    if( pt.y > lpRect->bottom )
        pt.y = lpRect->bottom;

    return pt;
}

/***********************************************************************
 *           SCROLL_GetThumbVal
 *
 * Compute the current scroll position based on the thumb position in pixels
 * from the top of the scroll-bar.
 */
static UINT SCROLL_GetThumbVal( SCROLLBAR_INFO *infoPtr, RECT *rect,
                                  BOOL vertical, INT pos )
{
    INT thumbSize;
    INT pixels = vertical ? rect->bottom-rect->top : rect->right-rect->left;

    if ((pixels -= 2*(GetSystemMetrics(SM_CXVSCROLL) - SCROLL_ARROW_THUMB_OVERLAP)) <= 0)
        return infoPtr->MinVal;

    if (infoPtr->Page)
    {
        thumbSize = pixels * infoPtr->Page/(infoPtr->MaxVal-infoPtr->MinVal+1);
        if (thumbSize < SCROLL_MIN_THUMB) thumbSize = SCROLL_MIN_THUMB;
    }
    else thumbSize = GetSystemMetrics(SM_CXVSCROLL);

    if ((pixels -= thumbSize) <= 0) return infoPtr->MinVal;

    pos = MAX( 0, pos - (GetSystemMetrics(SM_CXVSCROLL) - SCROLL_ARROW_THUMB_OVERLAP) );
    if (pos > pixels) pos = pixels;

    if (!infoPtr->Page) pos *= infoPtr->MaxVal - infoPtr->MinVal;
    else pos *= infoPtr->MaxVal - infoPtr->MinVal - infoPtr->Page + 1;
    return infoPtr->MinVal + ((pos + pixels / 2) / pixels);
}

/***********************************************************************
 *           SCROLL_DrawScrollBar
 *
 * Redraw the whole scrollbar.
 */
void SCROLL_DrawScrollBar( HWND hwnd, HDC hdc, INT nBar,
                           BOOL arrows, BOOL interior )
{
    INT arrowSize, thumbSize, thumbPos;
    RECT rect;
    BOOL vertical;
    SCROLLBAR_INFO *infoPtr = SCROLL_GetInfoPtr( hwnd, nBar );
    BOOL Save_SCROLL_MovingThumb = SCROLL_MovingThumb;

    if (nBar == SB_CTL)
    {
      DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

      if (dwStyle & (SBS_SIZEBOX | SBS_SIZEGRIP))
      {
        //CB: todo

        return;
      }
    }

    vertical = SCROLL_GetScrollBarRect( hwnd, nBar, &rect,
                                        &arrowSize, &thumbSize, &thumbPos );

    if (Save_SCROLL_MovingThumb &&
        (SCROLL_TrackingWin == hwnd) &&
        (SCROLL_TrackingBar == nBar))
        SCROLL_DrawMovingThumb( hdc, &rect, vertical, arrowSize, thumbSize );

      /* Draw the arrows */

    if (arrows && arrowSize)
    {
        if( vertical == SCROLL_trackVertical && GetCapture() == hwnd )
            SCROLL_DrawArrows( hdc, infoPtr, &rect, arrowSize, vertical,
                               (SCROLL_trackHitTest == SCROLL_TOP_ARROW),
                               (SCROLL_trackHitTest == SCROLL_BOTTOM_ARROW) );
        else
            SCROLL_DrawArrows( hdc, infoPtr, &rect, arrowSize, vertical,
                                                               FALSE, FALSE );
    }
    if( interior )
        SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                         thumbPos, infoPtr->flags, vertical, FALSE, FALSE );

    if (Save_SCROLL_MovingThumb &&
        (SCROLL_TrackingWin == hwnd) &&
        (SCROLL_TrackingBar == nBar))
        SCROLL_DrawMovingThumb( hdc, &rect, vertical, arrowSize, thumbSize );

}

/***********************************************************************
 *           SCROLL_DrawInterior
 *
 * Draw the scroll bar interior (everything except the arrows).
 */
static void SCROLL_DrawInterior_9x( HWND hwnd, HDC hdc, INT nBar,
                    RECT *rect, INT arrowSize,
                    INT thumbSize, INT thumbPos,
                    UINT flags, BOOL vertical,
                    BOOL top_selected, BOOL bottom_selected )
{
    RECT r;
    HPEN hSavePen;
    HBRUSH hSaveBrush,hBrush;

    /* Only scrollbar controls send WM_CTLCOLORSCROLLBAR.
     * The window-owned scrollbars need to call DEFWND_ControlColor
     * to correctly setup default scrollbar colors
     */
    if (nBar == SB_CTL)
    {
      hBrush = (HBRUSH)SendMessageA( GetParent(hwnd), WM_CTLCOLORSCROLLBAR,
                     (WPARAM)hdc,(LPARAM)hwnd);
    }
    else
    {
      hBrush = (HBRUSH)SendMessageA( hwnd, WM_CTLCOLORSCROLLBAR,
                     (WPARAM)hdc,(LPARAM)hwnd);
    }

    hSavePen = SelectObject( hdc, GetSysColorPen(COLOR_WINDOWFRAME) );
    hSaveBrush = SelectObject( hdc, hBrush );

    /* Calculate the scroll rectangle */
    r = *rect;
    if (vertical)
    {
        r.top    += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
        r.bottom -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    }
    else
    {
        r.left  += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
        r.right -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    }

    /* Draw the scroll rectangles and thumb */
    if (!thumbPos)  /* No thumb to draw */
    {
        PatBlt( hdc, r.left, r.top,
                     r.right - r.left, r.bottom - r.top,
                     PATCOPY );

        /* cleanup and return */
        SelectObject( hdc, hSavePen );
        SelectObject( hdc, hSaveBrush );
        return;
    }

    if (vertical)
    {
        PatBlt( hdc, r.left, r.top,
                  r.right - r.left,
                  thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
                  top_selected ? 0x0f0000 : PATCOPY );
        r.top += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
        PatBlt( hdc, r.left, r.top + thumbSize,
                  r.right - r.left,
                  r.bottom - r.top - thumbSize,
                  bottom_selected ? 0x0f0000 : PATCOPY );
        r.bottom = r.top + thumbSize;
    }
    else  /* horizontal */
    {
        PatBlt( hdc, r.left, r.top,
                  thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
                  r.bottom - r.top,
                  top_selected ? 0x0f0000 : PATCOPY );
        r.left += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
        PatBlt( hdc, r.left + thumbSize, r.top,
                  r.right - r.left - thumbSize,
                  r.bottom - r.top,
                  bottom_selected ? 0x0f0000 : PATCOPY );
        r.right = r.left + thumbSize;
    }

    /* Draw the thumb */
    DrawEdge( hdc, &r, EDGE_RAISED, BF_RECT | BF_MIDDLE  );

    /* cleanup */
    SelectObject( hdc, hSavePen );
    SelectObject( hdc, hSaveBrush );
}

/***********************************************************************
 *           SCROLL_RefreshScrollBar
 *
 * Repaint the scroll bar interior after a SetScrollRange() or
 * SetScrollPos() call.
 */
static void SCROLL_RefreshScrollBar( HWND hwnd, INT nBar,
                                     BOOL arrows, BOOL interior )
{
  Win32BaseWindow *window;
  HWND hwndScroll;
  HDC hdc;

    //SvL: Get scrollbar handle from parent window
    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window && nBar != SB_CTL) {
        dprintf(("SCROLL_RefreshScrollBar window %x not found!", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return;
    }
    if(nBar == SB_VERT)
    {
        hwndScroll = Win32BaseWindow::OS2ToWin32Handle(window->getVertScrollHandle());
    }
    else
    if(nBar == SB_HORZ)
    {
        hwndScroll = Win32BaseWindow::OS2ToWin32Handle(window->getHorzScrollHandle());
    }
    else hwndScroll = hwnd;

    if(hwndScroll == 0)
        return;

    hdc = GetDCEx( hwndScroll, 0, DCX_CACHE | ((nBar == SB_CTL) ? 0 : DCX_WINDOW));

    if (!hdc) return;

    SCROLL_DrawScrollBar( hwndScroll, hdc, nBar, arrows, interior );
    ReleaseDC( hwndScroll, hdc );
}

/* Message Handler */

LRESULT SCROLL_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  SCROLLBAR_INFO *infoPtr = (SCROLLBAR_INFO*)malloc(sizeof(SCROLLBAR_INFO));

  infoPtr->MinVal = infoPtr->CurVal = infoPtr->Page = 0;
  infoPtr->MaxVal = 100;
  infoPtr->flags  = ESB_ENABLE_BOTH;

  SetInfoPtr(hwnd,(DWORD)infoPtr);

  if (!bitmapsLoaded) SCROLL_LoadBitmaps();

  return TRUE;
}

LRESULT SCROLL_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  CREATESTRUCTA *lpCreat = (CREATESTRUCTA *)lParam;

  if (lpCreat->style & SBS_VERT)
  {
    if (lpCreat->style & SBS_LEFTALIGN)
      MoveWindow( hwnd, lpCreat->x, lpCreat->y,
                                  GetSystemMetrics(SM_CXVSCROLL), lpCreat->cy, FALSE );
    else if (lpCreat->style & SBS_RIGHTALIGN)
      MoveWindow( hwnd,
                  lpCreat->x+lpCreat->cx-GetSystemMetrics(SM_CXVSCROLL)-1,
                  lpCreat->y,
                  GetSystemMetrics(SM_CXVSCROLL), lpCreat->cy, FALSE );
  } else  /* SBS_HORZ */
  {
    if (lpCreat->style & SBS_TOPALIGN)
      MoveWindow( hwnd, lpCreat->x, lpCreat->y,
                  lpCreat->cx, GetSystemMetrics(SM_CYHSCROLL), FALSE );
    else if (lpCreat->style & SBS_BOTTOMALIGN)
      MoveWindow( hwnd,
                  lpCreat->x,
                  lpCreat->y+lpCreat->cy-GetSystemMetrics(SM_CYHSCROLL)-1,
                  lpCreat->cx, GetSystemMetrics(SM_CYHSCROLL), FALSE );
  }

  return 0;
}

/***********************************************************************
 *           SCROLL_HandleScrollEvent
 *
 * Handle a mouse or timer event for the scrollbar.
 * 'pt' is the location of the mouse event in client (for SB_CTL) or
 * windows coordinates.
 */
LRESULT SCROLL_HandleScrollEvent(HWND hwnd,WPARAM wParam,LPARAM lParam,INT nBar,UINT msg)
{
    static POINT prevPt;       /* Previous mouse position for timer events */
    static UINT trackThumbPos; /* Thumb position when tracking started. */
    static INT lastClickPos;   /* Position in the scroll-bar of the last button-down event. */
    static INT lastMousePos;   /* Position in the scroll-bar of the last mouse event. */

    enum SCROLL_HITTEST hittest;
    HWND hwndOwner, hwndCtl;
    BOOL vertical;
    INT arrowSize, thumbSize, thumbPos;
    RECT rect;
    HDC hdc;
    POINT pt;
    LRESULT res = (msg == WM_MOUSEMOVE) ? 1:0;

    SCROLLBAR_INFO *infoPtr = SCROLL_GetInfoPtr( hwnd, nBar );
    if (!infoPtr) return res;

    if (nBar == SB_CTL)
    {
      DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

      if ((dwStyle & (SBS_SIZEBOX | SBS_SIZEGRIP)))
      {
        //CB: todo

        return res;
      }
    }

    if ((SCROLL_trackHitTest == SCROLL_NOWHERE) && (msg == WM_MOUSEMOVE || msg == WM_LBUTTONUP))
      return res;

    vertical = SCROLL_GetScrollBarRect( hwnd, nBar, &rect,
                                        &arrowSize, &thumbSize, &thumbPos );
    if (nBar == SB_CTL) hwndOwner = GetParent(hwnd); else
    {
      Win32BaseWindow *win32wnd;
      HWND hwndFrame;

      win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
      if (!win32wnd) return res;
      hwndFrame = OSLibWinQueryWindow(win32wnd->getOS2WindowHandle(),QWOS_PARENT);
      win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwndFrame);
      if (!win32wnd) return res;
      hwndOwner = win32wnd->getWindowHandle();
    }

    hwndCtl   = (nBar == SB_CTL) ? hwnd : 0;

    switch (msg)
    {
      case WM_LBUTTONDOWN:  /* Initialise mouse tracking */
        pt.x = (SHORT)LOWORD(lParam);
        pt.y = (SHORT)HIWORD(lParam);
        SCROLL_trackVertical = vertical;
        SCROLL_trackHitTest  = hittest = SCROLL_HitTest( hwnd, nBar, pt, FALSE );
        lastClickPos  = vertical ? (pt.y - rect.top) : (pt.x - rect.left);
        lastMousePos  = lastClickPos;
        trackThumbPos = thumbPos;
        prevPt = pt;
        SetCapture( hwnd );
        if (nBar == SB_CTL) SetFocus( hwnd );
        break;

      case WM_MOUSEMOVE:
        pt.x = (SHORT)LOWORD(lParam);
        pt.y = (SHORT)HIWORD(lParam);
        hittest = SCROLL_HitTest( hwnd, nBar, pt, TRUE );
        prevPt = pt;
        break;

      case WM_LBUTTONUP:
        pt.x = (SHORT)LOWORD(lParam);
        pt.y = (SHORT)HIWORD(lParam);
        hittest = SCROLL_NOWHERE;
        ReleaseCapture();
        break;

      case WM_SETFOCUS:
        if (nBar == SB_CTL)
        {
          SCROLL_FocusWin   = hwnd;
          SCROLL_HasFocus   = TRUE;
          SCROLL_Highlighted = FALSE;
          SetSystemTimer(hwnd,BLINK_TIMER,SCROLL_BLINK_DELAY,(TIMERPROC)0);
        }
        return res;

      case WM_KILLFOCUS:
        if (SCROLL_FocusWin == hwnd)
        {
          SCROLL_FocusWin    = 0;
          SCROLL_HasFocus    = FALSE;
          if (SCROLL_Highlighted)
          {
            SCROLL_Highlighted = FALSE;
            hdc = GetDCEx(hwnd,0,DCX_CACHE);
            SCROLL_DrawScrollBar(hwnd,hdc,nBar,FALSE,TRUE);
            ReleaseDC(hwnd,hdc);
          }
          KillSystemTimer(hwnd,BLINK_TIMER);
        }
        return res;

      case WM_SYSTIMER:
        if (wParam == SCROLL_TIMER)
        {
          pt = prevPt;
          hittest = SCROLL_HitTest( hwnd, nBar, pt, FALSE );
          break;
        } else if (wParam == BLINK_TIMER)
        {
          SCROLL_Highlighted = ~SCROLL_Highlighted;
          hdc = GetDCEx(hwnd,0,DCX_CACHE);
          SCROLL_DrawScrollBar(hwnd,hdc,nBar,FALSE,TRUE);
          ReleaseDC(hwnd,hdc);
          return res;
        } else return res;

      default:
        return res;  /* Should never happen */
    }

    hdc = GetDCEx(hwnd,0,DCX_CACHE | ((nBar == SB_CTL) ? 0:DCX_WINDOW));

    //TRACE("Event: hwnd=%04x bar=%d msg=%x pt=%ld,%ld hit=%d\n",
    //             hwnd, nBar, msg, pt.x, pt.y, hittest );

    switch(SCROLL_trackHitTest)
    {
      case SCROLL_NOWHERE:  /* No tracking in progress */
        break;

      case SCROLL_TOP_ARROW:
        SCROLL_DrawArrows( hdc, infoPtr, &rect, arrowSize, vertical,
                           (hittest == SCROLL_trackHitTest), FALSE );
        if (hittest == SCROLL_trackHitTest)
        {
            if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER))
            {
                SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                                SB_LINEUP, hwndCtl );
            }
            SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                            SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                            (TIMERPROC)0 );
        }
        else KillSystemTimer( hwnd, SCROLL_TIMER );
        break;

      case SCROLL_TOP_RECT:
        SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                             thumbPos, infoPtr->flags, vertical,
                             (hittest == SCROLL_trackHitTest), FALSE );
        if (hittest == SCROLL_trackHitTest)
        {
            if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER))
            {
                SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                                SB_PAGEUP, hwndCtl );
                SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                                SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                                (TIMERPROC)0 );
            }
        }
        else KillSystemTimer( hwnd, SCROLL_TIMER );
        break;

      case SCROLL_THUMB:
        if (msg == WM_LBUTTONDOWN)
        {
            SCROLL_TrackingWin = hwnd;
            SCROLL_TrackingBar = nBar;
            SCROLL_TrackingPos = trackThumbPos + lastMousePos - lastClickPos;
            if (!SCROLL_MovingThumb)
                SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
        }
        else if (msg == WM_LBUTTONUP)
        {
            if (SCROLL_MovingThumb)
                SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
            SCROLL_TrackingWin = 0;
            SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                                 thumbPos, infoPtr->flags, vertical,
                                 FALSE, FALSE );
        }
        else  /* WM_MOUSEMOVE */
        {
            UINT pos;

            if (!SCROLL_PtInRectEx( &rect, pt, vertical )) pos = lastClickPos;
            else
            {
                pt = SCROLL_ClipPos( &rect, pt );
                pos = vertical ? (pt.y - rect.top) : (pt.x - rect.left);
            }
            if ( (pos != lastMousePos) || (!SCROLL_MovingThumb) )
            {
                if (SCROLL_MovingThumb)
                    SCROLL_DrawMovingThumb( hdc, &rect, vertical,
                                        arrowSize, thumbSize );
                lastMousePos = pos;
                SCROLL_TrackingPos = trackThumbPos + pos - lastClickPos;
                SCROLL_TrackingVal = SCROLL_GetThumbVal( infoPtr, &rect,
                                                         vertical,
                                                         SCROLL_TrackingPos );
                SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                                MAKEWPARAM( SB_THUMBTRACK, SCROLL_TrackingVal),
                                hwndCtl );
                if (!SCROLL_MovingThumb)
                    SCROLL_DrawMovingThumb( hdc, &rect, vertical,
                                        arrowSize, thumbSize );
            }
        }
        break;

      case SCROLL_BOTTOM_RECT:
        SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                             thumbPos, infoPtr->flags, vertical,
                             FALSE, (hittest == SCROLL_trackHitTest) );
        if (hittest == SCROLL_trackHitTest)
        {
            if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER))
            {
                SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                                SB_PAGEDOWN, hwndCtl );
                SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                                SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                                (TIMERPROC)0 );
            }
        }
        else KillSystemTimer( hwnd, SCROLL_TIMER );
        break;

      case SCROLL_BOTTOM_ARROW:
        SCROLL_DrawArrows( hdc, infoPtr, &rect, arrowSize, vertical,
                           FALSE, (hittest == SCROLL_trackHitTest) );
        if (hittest == SCROLL_trackHitTest)
        {
            if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER))
            {
                SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                                SB_LINEDOWN, hwndCtl );
            }
            SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                            SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                            (TIMERPROC)0 );
        }
        else KillSystemTimer( hwnd, SCROLL_TIMER );
        break;
    }

    if (msg == WM_LBUTTONUP)
    {
        hittest = SCROLL_trackHitTest;
        SCROLL_trackHitTest = SCROLL_NOWHERE;  /* Terminate tracking */

        if (hittest == SCROLL_THUMB)
        {
            UINT val = SCROLL_GetThumbVal( infoPtr, &rect, vertical,
                                 trackThumbPos + lastMousePos - lastClickPos );
            SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                            MAKEWPARAM( SB_THUMBPOSITION, val ), hwndCtl );
        }
        else
            SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                            SB_ENDSCROLL, hwndCtl );
    }

    ReleaseDC( hwnd, hdc );

    return res;
}

/***********************************************************************
 *           SCROLL_HandleKbdEvent
 *
 * Handle a keyboard event (only for SB_CTL scrollbars).
 */
LRESULT SCROLL_KeyDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  UINT msg;

  if (dwStyle & (SBS_SIZEBOX | SBS_SIZEGRIP)) return 0;

  switch(wParam)
  {
    case VK_PRIOR: msg = SB_PAGEUP; break;
    case VK_NEXT:  msg = SB_PAGEDOWN; break;
    case VK_HOME:  msg = SB_TOP; break;
    case VK_END:   msg = SB_BOTTOM; break;
    case VK_UP:    msg = SB_LINEUP; break;
    case VK_DOWN:  msg = SB_LINEDOWN; break;
    default:
      return 0;
  }
  SendMessageA( GetParent(hwnd),
                (dwStyle & SBS_VERT) ? WM_VSCROLL : WM_HSCROLL,
                msg, hwnd );

  return 0;
}

LRESULT SCROLL_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam,INT nBar)
{
  PAINTSTRUCT ps;
  HDC hdc = wParam ? (HDC)wParam:BeginPaint( hwnd, &ps );

  SCROLL_DrawScrollBar( hwnd, hdc, nBar, TRUE, TRUE );
  if (!wParam) EndPaint( hwnd, &ps );

  return 0;
}

LRESULT SCROLL_SetRange(HWND hwnd,WPARAM wParam,LPARAM lParam,INT nBar,BOOL redraw)
{
  SCROLLBAR_INFO *infoPtr = SCROLL_GetInfoPtr(hwnd,nBar);
  INT oldPos = infoPtr->CurVal;

  SetScrollRange(hwnd,nBar,wParam,lParam,redraw);
  return (oldPos != infoPtr->CurVal) ? infoPtr->CurVal:0;
}

/* Window Procedures */

/***********************************************************************
 *           ScrollBarWndProc
 */
LRESULT WINAPI ScrollBarWndProc( HWND hwnd, UINT message, WPARAM wParam,
                                 LPARAM lParam )
{
  switch(message)
  {
    case WM_NCCREATE:
      return SCROLL_NCCreate(hwnd,wParam,lParam);

    case WM_CREATE:
      return SCROLL_Create(hwnd,wParam,lParam);

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_SYSTIMER:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      return SCROLL_HandleScrollEvent(hwnd,wParam,lParam,SB_CTL,message);

    case WM_KEYDOWN:
      return SCROLL_KeyDown(hwnd,wParam,lParam);

    case WM_ERASEBKGND:
      return 1;

    case WM_GETDLGCODE:
      return DLGC_WANTARROWS; /* Windows returns this value */

    case WM_PAINT:
      return SCROLL_Paint(hwnd,wParam,lParam,SB_CTL);

    case SBM_SETPOS:
      return SetScrollPos( hwnd, SB_CTL, wParam, (BOOL)lParam );

    case SBM_GETPOS:
      return GetScrollPos( hwnd, SB_CTL );

    case SBM_SETRANGE:
      return SCROLL_SetRange(hwnd,wParam,lParam,SB_CTL,FALSE);

    case SBM_GETRANGE:
      GetScrollRange( hwnd, SB_CTL, (LPINT)wParam, (LPINT)lParam );
      return 0;

    case SBM_ENABLE_ARROWS:
      return EnableScrollBar( hwnd, SB_CTL, wParam );

    case SBM_SETRANGEREDRAW:
      return SCROLL_SetRange(hwnd,wParam,lParam,SB_CTL,TRUE);

    case SBM_SETSCROLLINFO:
      return SetScrollInfo( hwnd, SB_CTL, (SCROLLINFO *)lParam, wParam );

    case SBM_GETSCROLLINFO:
      return GetScrollInfo( hwnd, SB_CTL, (SCROLLINFO *)lParam );

    case 0x00e5:
    case 0x00e7:
    case 0x00e8:
    case 0x00eb:
    case 0x00ec:
    case 0x00ed:
    case 0x00ee:
    case 0x00ef:
        //ERR("unknown Win32 msg %04x wp=%08x lp=%08lx\n",
        //            message, wParam, lParam );
        break;

    default:
      return DefWindowProcA( hwnd, message, wParam, lParam );
  }

  return 0;
}

/* frame handlers */

VOID SCROLL_SubclassScrollBars(HWND hwndHorz,HWND hwndVert)
{
  if (hwndHorz) SetWindowLongA(hwndHorz,GWL_WNDPROC,(ULONG)HorzScrollBarWndProc);
  if (hwndVert) SetWindowLongA(hwndVert,GWL_WNDPROC,(ULONG)VertScrollBarWndProc);

  if (!bitmapsLoaded) SCROLL_LoadBitmaps();
}

LRESULT WINAPI HorzScrollBarWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam )
{
  switch (message)
  {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_SYSTIMER:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      return SCROLL_HandleScrollEvent(hwnd,wParam,lParam,SB_HORZ,message);

    case WM_ERASEBKGND:
      return 1;

    case WM_GETDLGCODE:
      return DLGC_WANTARROWS; /* Windows returns this value */

    case WM_PAINT:
      return SCROLL_Paint(hwnd,wParam,lParam,SB_HORZ);

    case SBM_SETPOS:
      return SetScrollPos( hwnd, SB_HORZ, wParam, (BOOL)lParam );

    case SBM_GETPOS:
      return GetScrollPos( hwnd, SB_HORZ );

    case SBM_SETRANGE:
      return SCROLL_SetRange(hwnd,wParam,lParam,SB_HORZ,FALSE);

    case SBM_GETRANGE:
      GetScrollRange( hwnd, SB_HORZ, (LPINT)wParam, (LPINT)lParam );
      return 0;

    case SBM_ENABLE_ARROWS:
      return EnableScrollBar( hwnd, SB_HORZ, wParam );

    case SBM_SETRANGEREDRAW:
      return SCROLL_SetRange(hwnd,wParam,lParam,SB_HORZ,TRUE);

    case SBM_SETSCROLLINFO:
      return SetScrollInfo( hwnd, SB_HORZ, (SCROLLINFO *)lParam, wParam );

    case SBM_GETSCROLLINFO:
      return GetScrollInfo( hwnd, SB_HORZ, (SCROLLINFO *)lParam );

    default:
      return DefWindowProcA(hwnd,message,wParam,lParam);
  }

  return 0;
}

LRESULT WINAPI VertScrollBarWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam )
{
  switch (message)
  {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_SYSTIMER:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      return SCROLL_HandleScrollEvent(hwnd,wParam,lParam,SB_VERT,message);

    case WM_ERASEBKGND:
      return 1;

    case WM_GETDLGCODE:
      return DLGC_WANTARROWS; /* Windows returns this value */

    case WM_PAINT:
      return SCROLL_Paint(hwnd,wParam,lParam,SB_VERT);

    case SBM_SETPOS:
      return SetScrollPos( hwnd, SB_VERT, wParam, (BOOL)lParam );

    case SBM_GETPOS:
      return GetScrollPos( hwnd, SB_VERT );

    case SBM_SETRANGE:
      return SCROLL_SetRange(hwnd,wParam,lParam,SB_VERT,FALSE);

    case SBM_GETRANGE:
      GetScrollRange( hwnd, SB_VERT, (LPINT)wParam, (LPINT)lParam );
      return 0;

    case SBM_ENABLE_ARROWS:
      return EnableScrollBar( hwnd, SB_VERT, wParam );

    case SBM_SETRANGEREDRAW:
      return SCROLL_SetRange(hwnd,wParam,lParam,SB_VERT,TRUE);

    case SBM_SETSCROLLINFO:
      return SetScrollInfo( hwnd, SB_VERT, (SCROLLINFO *)lParam, wParam );

    case SBM_GETSCROLLINFO:
      return GetScrollInfo( hwnd, SB_VERT, (SCROLLINFO *)lParam );

    default:
      return DefWindowProcA(hwnd,message,wParam,lParam);
  }

  return 0;
}

/* Scrollbar API */

//CB: fix hide/show bugs

/*************************************************************************
 *           SetScrollInfo32   (USER32.501)
 * SetScrollInfo32 can be used to set the position, upper bound,
 * lower bound, and page size of a scrollbar control.
 *
 * RETURNS
 *    Scrollbar position
 *
 * NOTE
 *    For 100 lines of text to be displayed in a window of 25 lines,
 *  one would for instance use info->nMin=0, info->nMax=75
 *  (corresponding to the 76 different positions of the window on
 *  the text), and info->nPage=25.
 */
INT WINAPI SetScrollInfo(
                HWND hwnd /* [I] Handle of window whose scrollbar will be affected */,
                INT nBar /* [I] One of SB_HORZ, SB_VERT, or SB_CTL */,
                const SCROLLINFO *info /* [I] Specifies what to change and new values */,
                BOOL bRedraw /* [I] Should scrollbar be redrawn afterwards ? */)
{
  Win32BaseWindow *window;
  INT action;
  INT retVal;

    retVal = SCROLL_SetScrollInfo( hwnd, nBar, info, &action );
    if( action & SA_SSI_HIDE )
        SCROLL_ShowScrollBar( hwnd, nBar, FALSE, FALSE );
    else
    {
        if( action & SA_SSI_SHOW )
            if( SCROLL_ShowScrollBar( hwnd, nBar, TRUE, TRUE ) )
                return retVal; /* SetWindowPos() already did the painting */

        if( bRedraw && (action & SA_SSI_REFRESH))
            SCROLL_RefreshScrollBar( hwnd, nBar, TRUE, TRUE );
        else if( action & SA_SSI_REPAINT_ARROWS )
            SCROLL_RefreshScrollBar( hwnd, nBar, TRUE, FALSE );
    }
    return retVal;
}
//******************************************************************************
//******************************************************************************
INT SCROLL_SetScrollInfo( HWND hwnd, INT nBar,
                            const SCROLLINFO *info, INT *action  )
{
    /* Update the scrollbar state and set action flags according to
     * what has to be done graphics wise. */

    SCROLLBAR_INFO *infoPtr;
    UINT new_flags;

   *action = 0;

    if (!(infoPtr = SCROLL_GetInfoPtr(hwnd, nBar))) return 0;
    if (info->fMask & ~(SIF_ALL | SIF_DISABLENOSCROLL)) return 0;
    if ((info->cbSize != sizeof(*info)) &&
        (info->cbSize != sizeof(*info)-sizeof(info->nTrackPos))) return 0;

    /* Set the page size */
    if (info->fMask & SIF_PAGE)
    {
        if( infoPtr->Page != info->nPage )
        {
            infoPtr->Page = info->nPage;
           *action |= SA_SSI_REFRESH;
        }
    }

    /* Set the scroll pos */
    if (info->fMask & SIF_POS)
    {
        //dsprintf(scroll, " pos=%d", info->nPos );
        if( infoPtr->CurVal != info->nPos )
        {
            infoPtr->CurVal = info->nPos;
           *action |= SA_SSI_REFRESH;
        }
    }

    /* Set the scroll range */
    if (info->fMask & SIF_RANGE)
    {
        /* Invalid range -> range is set to (0,0) */
        if ((info->nMin > info->nMax) ||
            ((UINT)(info->nMax - info->nMin) >= 0x80000000))
        {
            infoPtr->MinVal = 0;
            infoPtr->MaxVal = 0;
        }
        else
        {
            if( infoPtr->MinVal != info->nMin ||
                infoPtr->MaxVal != info->nMax )
            {
               *action |= SA_SSI_REFRESH;
                infoPtr->MinVal = info->nMin;
                infoPtr->MaxVal = info->nMax;
            }
        }
    }

    /* Make sure the page size is valid */

    if (infoPtr->Page < 0) infoPtr->Page = 0;
    else if (infoPtr->Page > infoPtr->MaxVal - infoPtr->MinVal + 1 )
        infoPtr->Page = infoPtr->MaxVal - infoPtr->MinVal + 1;

    /* Make sure the pos is inside the range */

    if (infoPtr->CurVal < infoPtr->MinVal)
        infoPtr->CurVal = infoPtr->MinVal;
    else if (infoPtr->CurVal > infoPtr->MaxVal - MAX( infoPtr->Page-1, 0 ))
        infoPtr->CurVal = infoPtr->MaxVal - MAX( infoPtr->Page-1, 0 );

    //TRACE("    new values: page=%d pos=%d min=%d max=%d\n",
    //             infoPtr->Page, infoPtr->CurVal,
    //             infoPtr->MinVal, infoPtr->MaxVal );

    /* Check if the scrollbar should be hidden or disabled */
    if (info->fMask & (SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL))
    {
        new_flags = infoPtr->flags;
        if (infoPtr->MinVal >= infoPtr->MaxVal - MAX( infoPtr->Page-1, 0 ))
        {
            /* Hide or disable scroll-bar */
            if (info->fMask & SIF_DISABLENOSCROLL)
            {
                new_flags = ESB_DISABLE_BOTH;
               *action |= SA_SSI_REFRESH;
            }
            else if (nBar != SB_CTL)
            {
                *action = SA_SSI_HIDE;
                goto done;
            }
        }
        else  /* Show and enable scroll-bar */
        {
            new_flags = 0;
            if (nBar != SB_CTL)
                *action |= SA_SSI_SHOW;
        }

        if (infoPtr->flags != new_flags) /* check arrow flags */
        {
            infoPtr->flags = new_flags;
           *action |= SA_SSI_REPAINT_ARROWS;
        }
    }

done:
    /* Return current position */

    return infoPtr->CurVal;
}
/*************************************************************************
 *           GetScrollInfo32   (USER32.284)
 * GetScrollInfo32 can be used to retrieve the position, upper bound,
 * lower bound, and page size of a scrollbar control.
 *
 * RETURNS STD
 */
BOOL WINAPI GetScrollInfo(
                HWND hwnd /* [I] Handle of window */ ,
                INT nBar /* [I] One of SB_HORZ, SB_VERT, or SB_CTL */,
                LPSCROLLINFO info /* [IO] (info.fMask [I] specifies which values are to retrieve) */)
{
  SCROLLBAR_INFO *infoPtr;

    if (!(infoPtr = SCROLL_GetInfoPtr( hwnd, nBar ))) return FALSE;
    if (info->fMask & ~(SIF_ALL | SIF_DISABLENOSCROLL)) return FALSE;
    if ((info->cbSize != sizeof(*info)) &&
        (info->cbSize != sizeof(*info)-sizeof(info->nTrackPos))) return FALSE;

    if (info->fMask & SIF_PAGE) info->nPage = infoPtr->Page;
    if (info->fMask & SIF_POS) info->nPos = infoPtr->CurVal;
    if ((info->fMask & SIF_TRACKPOS) && (info->cbSize == sizeof(*info)))
        info->nTrackPos = (SCROLL_TrackingWin==hwnd) ? SCROLL_TrackingVal : infoPtr->CurVal;
    if (info->fMask & SIF_RANGE)
    {
        info->nMin = infoPtr->MinVal;
        info->nMax = infoPtr->MaxVal;
    }
    return (info->fMask & SIF_ALL) != 0;
}
/*************************************************************************
 *           SetScrollPos32   (USER32.502)
 *
 * RETURNS
 *    Success: Scrollbar position
 *    Failure: 0
 *
 * REMARKS
 *    Note the ambiguity when 0 is returned.  Use GetLastError
 *    to make sure there was an error (and to know which one).
 */
INT WINAPI SetScrollPos(
                HWND hwnd /* [I] Handle of window whose scrollbar will be affected */,
                INT nBar /* [I] One of SB_HORZ, SB_VERT, or SB_CTL */,
                INT nPos /* [I] New value */,
                BOOL bRedraw /* [I] Should scrollbar be redrawn afterwards ? */ )
{
  SCROLLINFO info;
  SCROLLBAR_INFO *infoPtr;
  INT oldPos;

    dprintf(("SetScrollPos %x %d %d %d", hwnd, nBar, nPos, bRedraw));
    if (!(infoPtr = SCROLL_GetInfoPtr( hwnd, nBar ))) return 0;
    oldPos      = infoPtr->CurVal;
    info.cbSize = sizeof(info);
    info.nPos   = nPos;
    info.fMask  = SIF_POS;
    SetScrollInfo( hwnd, nBar, &info, bRedraw );
    return oldPos;
}
/*************************************************************************
 *           GetScrollPos32   (USER32.285)
 *
 * RETURNS
 *    Success: Current position
 *    Failure: 0
 *
 * REMARKS
 *    Note the ambiguity when 0 is returned.  Use GetLastError
 *    to make sure there was an error (and to know which one).
 */
INT WINAPI GetScrollPos(
                HWND hwnd, /* [I] Handle of window */
                INT nBar /* [I] One of SB_HORZ, SB_VERT, or SB_CTL */)
{
  SCROLLBAR_INFO *infoPtr;

    dprintf(("GetScrollPos %x %d", hwnd, nBar));

    infoPtr = SCROLL_GetInfoPtr( hwnd, nBar );
    if (!infoPtr) return 0;

    return infoPtr->CurVal;
}
/*************************************************************************
 *           SetScrollRange32   (USER32.503)
 *
 * RETURNS STD
 */
BOOL WINAPI SetScrollRange(
                HWND hwnd,   /* [I] Handle of window whose scrollbar will be affected */
                INT nBar,    /* [I] One of SB_HORZ, SB_VERT, or SB_CTL */
                INT MinVal,  /* [I] New minimum value */
                INT MaxVal,  /* [I] New maximum value */
                BOOL bRedraw /* [I] Should scrollbar be redrawn afterwards ? */)
{
  SCROLLINFO info;

    dprintf(("SetScrollRange %x %x %d %d %d", hwnd, nBar, MinVal, MaxVal, bRedraw));
    info.cbSize = sizeof(info);
    info.nMin   = MinVal;
    info.nMax   = MaxVal;
    info.fMask  = SIF_RANGE;
    SetScrollInfo( hwnd, nBar, &info, bRedraw );
    return TRUE;
}
/*************************************************************************
 *           SCROLL_SetNCSbState
 *
 * Updates both scrollbars at the same time. Used by MDI CalcChildScroll().
 */
INT SCROLL_SetNCSbState(HWND hwnd, int vMin, int vMax, int vPos,
                                       int hMin, int hMax, int hPos)
{
    INT vA, hA;
    SCROLLINFO vInfo, hInfo;

    vInfo.cbSize = hInfo.cbSize = sizeof(SCROLLINFO);
    vInfo.nMin   = vMin;         hInfo.nMin   = hMin;
    vInfo.nMax   = vMax;         hInfo.nMax   = hMax;
    vInfo.nPos   = vPos;         hInfo.nPos   = hPos;
    vInfo.fMask  = hInfo.fMask = SIF_RANGE | SIF_POS;

    SCROLL_SetScrollInfo( hwnd, SB_VERT, &vInfo, &vA );
    SCROLL_SetScrollInfo( hwnd, SB_HORZ, &hInfo, &hA );

    if( !SCROLL_ShowScrollBar( hwnd, SB_BOTH,
                              (hA & SA_SSI_SHOW),(vA & SA_SSI_SHOW) ) )
    {
        /* SetWindowPos() wasn't called, just redraw the scrollbars if needed */
        if( vA & SA_SSI_REFRESH )
            SCROLL_RefreshScrollBar( hwnd, SB_VERT, FALSE, TRUE );

        if( hA & SA_SSI_REFRESH )
            SCROLL_RefreshScrollBar( hwnd, SB_HORZ, FALSE, TRUE );
    }
    return 0;
}


/*************************************************************************
 *           GetScrollRange32   (USER32.286)
 *
 * RETURNS STD
 */
BOOL WINAPI GetScrollRange(
                HWND hwnd,   /* [I] Handle of window */
                INT nBar,    /* [I] One of SB_HORZ, SB_VERT, or SB_CTL  */
                LPINT lpMin, /* [O] Where to store minimum value */
                LPINT lpMax  /* [O] Where to store maximum value */)
{
  SCROLLBAR_INFO *infoPtr;

    infoPtr = SCROLL_GetInfoPtr( hwnd, nBar );
    if (!infoPtr)
    {
        if (lpMin) lpMin = 0;
        if (lpMax) lpMax = 0;
        return FALSE;
    }
    if (lpMin) *lpMin = infoPtr->MinVal;
    if (lpMax) *lpMax = infoPtr->MaxVal;
    return TRUE;
}
/*************************************************************************
 *           SCROLL_ShowScrollBar()
 *
 * Back-end for ShowScrollBar(). Returns FALSE if no action was taken.
 * NOTE: fShowV/fShowH must be zero when nBar is SB_HORZ/SB_VERT.
 */
BOOL SCROLL_ShowScrollBar(HWND hwnd, INT nBar,
                          BOOL fShowH, BOOL fShowV )
{
    BOOL retvalue = FALSE;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    //TRACE("hwnd=%04x bar=%d horz=%d, vert=%d\n",
    //                hwnd, nBar, fShowH, fShowV );

    switch(nBar)
    {
    case SB_CTL:
        ShowWindow( hwnd, fShowH ? SW_SHOW : SW_HIDE );
        retvalue = TRUE;
        goto END;

    case SB_BOTH:
    case SB_HORZ:
        if (fShowH)
        {
            fShowH = !(dwStyle & WS_HSCROLL);
            dwStyle |= WS_HSCROLL;
        }
        else  /* hide it */
        {
            fShowH = (dwStyle & WS_HSCROLL);
            dwStyle &= ~WS_HSCROLL;
        }
        SetWindowLongA(hwnd,GWL_STYLE,dwStyle);
        if( nBar == SB_HORZ ) {
            fShowV = FALSE;
            break;
        }
        /* fall through */

    case SB_VERT:
        if (fShowV)
        {
            fShowV = !(dwStyle & WS_VSCROLL);
            dwStyle |= WS_VSCROLL;
        }
        else  /* hide it */
        {
            fShowV = (dwStyle & WS_VSCROLL);
            dwStyle &= ~WS_VSCROLL;
        }
        SetWindowLongA(hwnd,GWL_STYLE,dwStyle);
        if ( nBar == SB_VERT )
           fShowH = FALSE;
        break;

    default:
        retvalue = FALSE;  /* Nothing to do! */
        goto END;
    }

    if( fShowH || fShowV ) /* frame has been changed, let the window redraw itself */
    {
        SetWindowPos( hwnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE
                    | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED );
        retvalue = TRUE;
        goto END;
    }

    retvalue = FALSE; /* no frame changes */
END:
    return retvalue;
}


/*************************************************************************
 *           ShowScrollBar32   (USER32.532)
 *
 * RETURNS STD
 */
BOOL WINAPI ShowScrollBar(
                    HWND hwnd,  /* [I] Handle of window whose scrollbar(s) will be affected   */
                    INT nBar,   /* [I] One of SB_HORZ, SB_VERT, SB_BOTH or SB_CTL */
                    BOOL fShow  /* [I] TRUE = show, FALSE = hide  */)
{
  Win32BaseWindow *window;

    dprintf(("ShowScrollBar %04x %d %d\n", hwnd, nBar, fShow));

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window && nBar != SB_CTL) {
        dprintf(("ShowScrollBar window not found!"));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    if(nBar != SB_CTL || (window->getStyle() & (WM_HSCROLL|WM_VSCROLL) != 0))
    {
     BOOL rc;

        if(nBar == SB_HORZ || nBar == SB_BOTH)
            rc = OSLibWinShowScrollBar(window->getOS2FrameWindowHandle(),
                                       window->getHorzScrollHandle(),
                                       OSLIB_HSCROLL, fShow);

        if(nBar == SB_VERT || ( rc == TRUE && nBar == SB_BOTH))
            rc = OSLibWinShowScrollBar(window->getOS2FrameWindowHandle(),
                                       window->getVertScrollHandle(),
                                       OSLIB_VSCROLL, fShow);

        return rc;
    }

    SCROLL_ShowScrollBar( hwnd, nBar, (nBar == SB_VERT) ? 0 : fShow,
                                      (nBar == SB_HORZ) ? 0 : fShow );
    return TRUE;
}
/*************************************************************************
 *           EnableScrollBar32   (USER32.171)
 */
BOOL WINAPI EnableScrollBar( HWND hwnd, INT nBar, UINT flags)
{
  Win32BaseWindow *window;
  BOOL bFineWithMe;
  SCROLLBAR_INFO *infoPtr;

    dprintf(("EnableScrollBar %04x %d %d\n", hwnd, nBar, flags));

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window && nBar != SB_CTL) {
        dprintf(("EnableScrollBar window not found!"));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    if(nBar != SB_CTL || (window->getStyle() & (WM_HSCROLL|WM_VSCROLL) != 0))
    {
     BOOL fEnable, rc;

        //hwnd = parent window with default scrollbar(s)
        switch(flags) {
        case ESB_ENABLE_BOTH:
            fEnable = TRUE;
            break;
        case ESB_DISABLE_BOTH:
        case ESB_DISABLE_RTDN:
        case ESB_DISABLE_LTUP:
            fEnable = FALSE;
            break;
        default:
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }
        if(nBar == SB_HORZ || nBar == SB_BOTH)
            rc = OSLibWinEnableScrollBar(window->getOS2FrameWindowHandle(), OSLIB_HSCROLL, fEnable);

        if(nBar == SB_VERT || ( rc == TRUE && nBar == SB_BOTH))
            rc = OSLibWinEnableScrollBar(window->getOS2FrameWindowHandle(), OSLIB_VSCROLL, fEnable);

        return rc;
    }
    flags &= ESB_DISABLE_BOTH;

    if (nBar == SB_BOTH)
    {
        if (!(infoPtr = SCROLL_GetInfoPtr( hwnd, SB_VERT ))) return FALSE;
        if (!(bFineWithMe = (infoPtr->flags == flags)) )
        {
            infoPtr->flags = flags;
            SCROLL_RefreshScrollBar( hwnd, SB_VERT, TRUE, TRUE );
        }
        nBar = SB_HORZ;
    }
    else
        bFineWithMe = TRUE;

    if (!(infoPtr = SCROLL_GetInfoPtr( hwnd, nBar ))) return FALSE;
    if (bFineWithMe && infoPtr->flags == flags) return FALSE;
    infoPtr->flags = flags;

    SCROLL_RefreshScrollBar( hwnd, nBar, TRUE, TRUE );
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL SCROLLBAR_Register()
{
    WNDCLASSA wndClass;

    if (GlobalFindAtomA(SCROLLBARCLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
    wndClass.lpfnWndProc   = (WNDPROC)ScrollBarWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(SCROLLBAR_INFO);
    wndClass.hCursor       = LoadCursorA(0,IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)0;
    wndClass.lpszClassName = SCROLLBARCLASSNAME;

    return RegisterClassA(&wndClass);
}
//******************************************************************************
//******************************************************************************
BOOL SCROLLBAR_Unregister()
{
    if (GlobalFindAtomA(SCROLLBARCLASSNAME))
        return UnregisterClassA(SCROLLBARCLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}
//******************************************************************************
//******************************************************************************

