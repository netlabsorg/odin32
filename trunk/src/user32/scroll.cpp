/* $Id: scroll.cpp,v 1.44 2001-10-11 17:05:02 sandervl Exp $ */
/*
 * Scrollbar control
 *
 * Copyright 1999 Christoph Bratschi
 *
 * Copyright 1993 Martin Ayotte
 * Copyright 1994, 1996 Alexandre Julliard
 *
 * WINE version: 20000130
 *
 * Status:  complete
 * Version: 5.00
 */

#include <stdlib.h>
#include <os2win.h>
#include "controls.h"
#include "scroll.h"
#include "win32wbase.h"
#include "oslibwin.h"
#include "initterm.h"
#include "pmwindow.h"

#define DBG_LOCALLOG    DBG_scroll
#include "dbglocal.h"

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

 /* What to do in SetScrollInfo() */
#define SA_SSI_HIDE             0x0001
#define SA_SSI_SHOW             0x0002
#define SA_SSI_REPAINT_INTERIOR 0x0004
#define SA_SSI_REPAINT_ARROWS   0x0008
#define SA_SSI_MOVE_THUMB       0x0010
#define SA_SSI_REFRESH          0x0020

 /* Thumb-tracking info */
static HWND SCROLL_TrackingWin = 0;
static INT  SCROLL_TrackingBar = 0;
static INT  SCROLL_TrackingPos = 0;
static INT  SCROLL_TrackingVal = 0;
 /* Focus info */
static HWND SCROLL_FocusWin    = 0;
static BOOL SCROLL_HasFocus    = FALSE;
static BOOL SCROLL_Highlighted = FALSE;
static BOOL SCROLL_Scrolling   = FALSE;

 /* Hit test code of the last button-down event */
static enum SCROLL_HITTEST SCROLL_trackHitTest;
static enum SCROLL_HITTEST SCROLL_lastHitTest;
static BOOL SCROLL_trackVertical;

 /* Is the moving thumb being displayed? */
static BOOL SCROLL_MovingThumb = FALSE;

static SCROLLBAR_INFO *SCROLL_GetInfoPtr( HWND hwnd, INT nBar )
{
    Win32BaseWindow *win32wnd;

    switch(nBar)
    {
        case SB_HORZ:
        case SB_VERT:
        {
            SCROLLBAR_INFO *pInfo;
            win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

            if (!win32wnd) return NULL;
            pInfo = win32wnd->getScrollInfo(nBar);
            RELEASE_WNDOBJ(win32wnd);
            return pInfo;
        }

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
      {
        Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
        RECT rectClient;

        if (!win32wnd) return FALSE;
        rectClient = *win32wnd->getClientRectPtr();
        lprect->left   = rectClient.left;
        lprect->top    = rectClient.bottom;
        lprect->right  = rectClient.right;
        lprect->bottom = lprect->top+GetSystemMetrics(SM_CYHSCROLL);
        if (win32wnd->getStyle() & WS_BORDER)
        {
          lprect->left--;
          lprect->right++;
        } 
        else 
        if (win32wnd->getStyle() & WS_VSCROLL)
            lprect->right++;
        RELEASE_WNDOBJ(win32wnd);
        vertical = FALSE;
        break;
      }

      case SB_VERT:
      {
        Win32BaseWindow *win32wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
        RECT rectClient;

        if (!win32wnd) return FALSE;
        rectClient = *win32wnd->getClientRectPtr();
        lprect->left   = rectClient.right;
        lprect->top    = rectClient.top;
        lprect->right  = lprect->left+GetSystemMetrics(SM_CXVSCROLL);
        lprect->bottom = rectClient.bottom;
        if(win32wnd->getStyle() & WS_BORDER)
        {
          lprect->top--;
          lprect->bottom++;
        } 
        else 
        if (win32wnd->getStyle() & WS_HSCROLL)
          lprect->bottom++;
        RELEASE_WNDOBJ(win32wnd);
        vertical = TRUE;
        break;
      }

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
    //testestest
    dprintf(("SCROLL_GetScrollBarRect: thumbPos %d thumbSize %d", *thumbPos, *thumbSize));
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
      INT w = lpRect->right-lpRect->left;

      rect.left   -= w;
      rect.right  += w;
      rect.top    -= w;
      rect.bottom += w;
    } else
    {
      INT h = lpRect->bottom-lpRect->top;

      rect.top    -= h;
      rect.bottom += h;
      rect.left   -= h;
      rect.right  += h;
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
    SCROLLBAR_INFO *infoPtr = SCROLL_GetInfoPtr(hwnd,nBar);

    if (!infoPtr) return SCROLL_NOWHERE;

    BOOL vertical = SCROLL_GetScrollBarRect( hwnd, nBar, &rect,
                                           &arrowSize, &thumbSize, &thumbPos );

    if ( (bDragging && !SCROLL_PtInRectEx( &rect, pt, vertical )) ||
         (!PtInRect( &rect, pt )) ) return SCROLL_NOWHERE;

    if (vertical)
    {
        if (pt.y < rect.top + arrowSize) return (infoPtr->flags & ESB_DISABLE_LTUP) ? SCROLL_NOWHERE:SCROLL_TOP_ARROW;
        if (pt.y >= rect.bottom - arrowSize) return (infoPtr->flags & ESB_DISABLE_RTDN) ? SCROLL_NOWHERE:SCROLL_BOTTOM_ARROW;
        if (!thumbPos) return ((infoPtr->flags & ESB_DISABLE_BOTH) == ESB_DISABLE_BOTH) ? SCROLL_NOWHERE:SCROLL_TOP_RECT;
        pt.y -= rect.top;
        if (pt.y < thumbPos) return SCROLL_TOP_RECT;
        if (pt.y >= thumbPos + thumbSize) return SCROLL_BOTTOM_RECT;
    }
    else  /* horizontal */
    {
        if (pt.x < rect.left + arrowSize) return (infoPtr->flags & ESB_DISABLE_LTUP) ? SCROLL_NOWHERE:SCROLL_TOP_ARROW;
        if (pt.x >= rect.right - arrowSize) return (infoPtr->flags & ESB_DISABLE_RTDN) ? SCROLL_NOWHERE:SCROLL_BOTTOM_ARROW;
        if (!thumbPos) return ((infoPtr->flags & ESB_DISABLE_BOTH) == ESB_DISABLE_BOTH) ? SCROLL_NOWHERE:SCROLL_TOP_RECT;
        pt.x -= rect.left;
        if (pt.x < thumbPos) return SCROLL_TOP_RECT;
        if (pt.x >= thumbPos + thumbSize) return SCROLL_BOTTOM_RECT;
    }
    return SCROLL_THUMB;
}

static void SCROLL_DrawTopArrow(HDC hdc,SCROLLBAR_INFO *infoPtr,RECT *rect,INT arrowSize,BOOL vertical,BOOL top_pressed)
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
}

static void SCROLL_DrawBottomArrow(HDC hdc,SCROLLBAR_INFO *infoPtr,RECT *rect,INT arrowSize,BOOL vertical,BOOL bottom_pressed)
{
  RECT r;

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
 *           SCROLL_DrawArrows
 *
 * Draw the scroll bar arrows.
 */
static void SCROLL_DrawArrows( HDC hdc, SCROLLBAR_INFO *infoPtr,
                               RECT *rect, INT arrowSize, BOOL vertical,
                               BOOL top_pressed, BOOL bottom_pressed )
{
  SCROLL_DrawTopArrow(hdc,infoPtr,rect,arrowSize,vertical,top_pressed);
  SCROLL_DrawBottomArrow(hdc,infoPtr,rect,arrowSize,vertical,bottom_pressed);
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

    DrawEdge(hdc,&r,EDGE_RAISED,BF_RECT | BF_ADJUST);
    FillRect(hdc,&r,(SCROLL_FocusWin == hwnd && SCROLL_Highlighted && !SCROLL_Scrolling) ? GetSysColorBrush(COLOR_3DSHADOW):GetSysColorBrush(COLOR_BTNFACE));

    /* cleanup */
    SelectObject( hdc, hSavePen );
    SelectObject( hdc, hSaveBrush );
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

  SCROLL_DrawInterior( SCROLL_TrackingWin, hdc, SCROLL_TrackingBar,
              rect, arrowSize, thumbSize, pos,
              0, vertical, FALSE, FALSE );
}

/***********************************************************************
 *           SCROLL_ClipPos
 */
static POINT SCROLL_ClipPos( LPRECT lpRect, POINT pt )
{
    if( pt.x < lpRect->left )
        pt.x = lpRect->left;
    else
    if( pt.x >= lpRect->right )
        pt.x = lpRect->right-1;

    if( pt.y < lpRect->top )
        pt.y = lpRect->top;
    else
    if( pt.y >= lpRect->bottom )
        pt.y = lpRect->bottom-1;

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

void SCROLL_GetSizeBox(HWND hwnd,DWORD dwStyle,PRECT rect)
{
  RECT clientRect;
  INT cx = GetSystemMetrics(SM_CXVSCROLL);
  INT cy = GetSystemMetrics(SM_CYHSCROLL);

  GetClientRect(hwnd,&clientRect);

  if (dwStyle & SBS_SIZEBOXTOPLEFTALIGN)
  {
    rect->left   = 0;
    rect->right  = cx;
    rect->bottom = cy;
    rect->top    = 0;
  } else
  {
    rect->left   = clientRect.right-cx;
    rect->right  = clientRect.right;
    rect->bottom = clientRect.bottom;
    rect->top    = clientRect.bottom-cy;
  }
}

void SCROLL_DrawSizeBox(HDC hdc,RECT rect)
{
  POINT p1,p2;
  HPEN penDark = GetSysColorPen(COLOR_3DSHADOW);
  HPEN penWhite = GetSysColorPen(COLOR_3DHILIGHT);
  HPEN oldPen = SelectObject(hdc,penDark);
  INT x;

  p1.x = rect.right-1;
  p1.y = rect.bottom;
  p2.x = rect.right;
  p2.y = rect.bottom-1;
  for (x = 0;x < 3;x++)
  {
    SelectObject(hdc,penDark);
    MoveToEx(hdc,p1.x,p1.y,NULL);
    LineTo(hdc,p2.x,p2.y);
    p1.x--;
    p2.y--;
    MoveToEx(hdc,p1.x,p1.y,NULL);
    LineTo(hdc,p2.x,p2.y);
    SelectObject(hdc,penWhite);
    p1.x--;
    p2.y--;
    MoveToEx(hdc,p1.x,p1.y,NULL);
    LineTo(hdc,p2.x,p2.y);
    p1.x -= 2;
    p2.y -= 2;
  }

  SelectObject(hdc,oldPen);
}

/***********************************************************************
 *           SCROLL_DrawScrollBar
 *
 * Redraw the whole scrollbar.
 */
void SCROLL_DrawScrollBar(HWND hwnd,HDC hdc,INT nBar,BOOL arrows,BOOL interior)
{
    INT arrowSize, thumbSize, thumbPos;
    RECT rect;
    BOOL vertical;
    SCROLLBAR_INFO *infoPtr = SCROLL_GetInfoPtr( hwnd, nBar );

    if (!infoPtr) return;
    if (nBar == SB_CTL)
    {
      DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

      if (dwStyle & (SBS_SIZEBOX | SBS_SIZEGRIP))
      {
        RECT rect;
        HBRUSH hBrush;

        hdc = GetDCEx(hwnd,0,DCX_CACHE | ((nBar == SB_CTL) ? 0 : DCX_WINDOW));
        hBrush = GetSysColorBrush(COLOR_3DFACE);
        GetClientRect(hwnd,&rect);
        FillRect(hdc,&rect,hBrush);

        if (dwStyle & SBS_SIZEGRIP)
        {
          SCROLL_GetSizeBox(hwnd,dwStyle,&rect);
          SCROLL_DrawSizeBox(hdc,rect);
        }

        ReleaseDC(hwnd,hdc);

        return;
      }
    }

    vertical = SCROLL_GetScrollBarRect( hwnd, nBar, &rect,
                                        &arrowSize, &thumbSize, &thumbPos );

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

    if (SCROLL_MovingThumb &&
        (SCROLL_TrackingWin == hwnd) &&
        (SCROLL_TrackingBar == nBar))
        SCROLL_DrawMovingThumb( hdc, &rect, vertical, arrowSize, thumbSize );
    else if( interior )
      SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                           thumbPos, infoPtr->flags, vertical, SCROLL_trackHitTest == SCROLL_TOP_RECT && SCROLL_lastHitTest == SCROLL_TOP_RECT,SCROLL_trackHitTest == SCROLL_BOTTOM_RECT && SCROLL_lastHitTest == SCROLL_BOTTOM_RECT);
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
  HDC hdc = GetDCEx(hwnd,0,DCX_CACHE | ((nBar == SB_CTL) ? 0:DCX_WINDOW));

  if (!hdc) return;

  SCROLL_DrawScrollBar( hwnd, hdc, nBar, arrows, interior );
  ReleaseDC( hwnd, hdc );
}

/* Message Handler */

LRESULT SCROLL_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  SCROLLBAR_INFO *infoPtr = (SCROLLBAR_INFO*)malloc(sizeof(SCROLLBAR_INFO));

  infoPtr->MinVal = infoPtr->CurVal = infoPtr->Page = 0;
  infoPtr->MaxVal = 100;
  infoPtr->flags  = ESB_ENABLE_BOTH;

  SetInfoPtr(hwnd,(DWORD)infoPtr);

  return TRUE;
}

LRESULT SCROLL_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  CREATESTRUCTA *lpCreat = (CREATESTRUCTA *)lParam;

#ifdef __WIN32OS2__
  if (!((lpCreat->style & (SBS_SIZEBOX | SBS_SIZEGRIP)) && !(lpCreat->style & (SBS_SIZEBOXTOPLEFTALIGN | SBS_SIZEBOXBOTTOMRIGHTALIGN))))
#endif
  {
    if (lpCreat->style & SBS_VERT)
    {
      INT w,h;

      w = GetSystemMetrics(SM_CXVSCROLL);
#ifdef __WIN32OS2__
      if(lpCreat->style & (SBS_SIZEBOX | SBS_SIZEGRIP)) {
           h = GetSystemMetrics(SM_CYHSCROLL);
      }
      else h = lpCreat->cy;
#else
      h = lpCreat->cy;
#endif
      if (lpCreat->style & SBS_LEFTALIGN)
        MoveWindow(hwnd,lpCreat->x,lpCreat->y,w,h,FALSE);
      else if (lpCreat->style & SBS_RIGHTALIGN)
        MoveWindow(hwnd,lpCreat->x+lpCreat->cx-w,lpCreat->y,w,h,FALSE);
    } else /* SBS_HORZ */
    {
      INT w,h;

#ifdef __WIN32OS2__
      if(lpCreat->style & (SBS_SIZEBOX | SBS_SIZEGRIP)) {
           w = GetSystemMetrics(SM_CXVSCROLL);
      }
      else w = lpCreat->cx;
#else
      w = lpCreat->cx;
#endif

      h = GetSystemMetrics(SM_CYHSCROLL);

      if (lpCreat->style & SBS_TOPALIGN)
        MoveWindow(hwnd,lpCreat->x,lpCreat->y,w,h,FALSE);
      else if (lpCreat->style & SBS_BOTTOMALIGN)
        MoveWindow(hwnd,lpCreat->x,lpCreat->y+lpCreat->cy-h,w,h,FALSE);
    }
  }

  return 0;
}

static LRESULT SCROLL_Destroy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  SCROLLBAR_INFO* infoPtr = (SCROLLBAR_INFO*)GetInfoPtr(hwnd);

  free(infoPtr);

  return 0;
}

#ifdef __WIN32OS2__
static LRESULT SCROLL_Enable(HWND hwnd, WPARAM wParam)
{
  SCROLLBAR_INFO* infoPtr = (SCROLLBAR_INFO*)GetInfoPtr(hwnd);

  if (!infoPtr) return 0;

  EnableScrollBar(hwnd, SB_CTL, ESB_DISABLE_BOTH);

  return 0;
}
#endif

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
    static BOOL thumbTrackSent;
    static INT lastClickPos;   /* Position in the scroll-bar of the last button-down event. */
    static INT lastMousePos;   /* Position in the scroll-bar of the last mouse event. */
    static BOOL timerRunning;

    enum SCROLL_HITTEST hittest;
    HWND hwndOwner, hwndCtl;
    BOOL vertical;
    INT arrowSize, thumbSize, thumbPos;
    RECT rect;
    HDC hdc;
    POINT pt;
    LRESULT res = 0;

    SCROLLBAR_INFO *infoPtr = SCROLL_GetInfoPtr(hwnd,nBar);
    if (!infoPtr) return res;

    if (nBar == SB_CTL)
    {
      DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

      if ((dwStyle & (SBS_SIZEBOX | SBS_SIZEGRIP)))
      {
        if (!(dwStyle & SBS_SIZEGRIP)) return res;

        if (msg == WM_NCHITTEST)
        {
          if (dwStyle & SBS_SIZEGRIP)
          {
            RECT rect;

            pt.x = (SHORT)LOWORD(lParam);
            pt.y = (SHORT)HIWORD(lParam);
            ScreenToClient(hwnd,&pt);
            SCROLL_GetSizeBox(hwnd,dwStyle,&rect);
            if (PtInRect(&rect,pt))
            {
              if (dwStyle & SBS_SIZEBOXTOPLEFTALIGN)
                return HTTOPLEFT;
              else
                return HTBOTTOMRIGHT;
            }
          }
          return DefWindowProcA(hwnd,WM_NCHITTEST,wParam,lParam);
        } else if (msg == WM_LBUTTONDOWN)
        {
          return DefWindowProcA(hwnd,WM_LBUTTONDOWN,wParam,lParam);
        }

        return res;
      }
    }

    if (msg == WM_NCHITTEST) return DefWindowProcA(hwnd,WM_NCHITTEST,wParam,lParam);

    vertical = SCROLL_GetScrollBarRect(hwnd,nBar,&rect,&arrowSize,&thumbSize,&thumbPos);
    hwndOwner = (nBar == SB_CTL) ? GetParent(hwnd):hwnd;

    hwndCtl = (nBar == SB_CTL) ? hwnd:0;

    switch (msg)
    {
      case WM_LBUTTONDOWN:  /* Initialise mouse tracking */
        pt.x = (SHORT)LOWORD(lParam);
        pt.y = (SHORT)HIWORD(lParam);
        SCROLL_trackVertical = vertical;
        SCROLL_trackHitTest = hittest = SCROLL_HitTest(hwnd,nBar,pt,FALSE);
        if (SCROLL_trackHitTest == SCROLL_NOWHERE)
        {
          MessageBeep(MB_ICONEXCLAMATION);

          return res;
        }
        SCROLL_Scrolling = TRUE;
        timerRunning = FALSE;
        if ((SCROLL_FocusWin == hwnd) && SCROLL_Highlighted)
        {
          hdc = GetDCEx(hwnd,0,DCX_CACHE | ((nBar == SB_CTL) ? 0:DCX_WINDOW));
          SCROLL_DrawScrollBar(hwnd,hdc,nBar,FALSE,TRUE);
          ReleaseDC(hwnd,hdc);
        }
        lastClickPos  = vertical ? pt.y:pt.x;
        lastMousePos  = lastClickPos;
        trackThumbPos = thumbPos;
        prevPt = pt;
        if (nBar == SB_CTL) SetFocus(hwnd);
        SetCapture(hwnd);
        break;

      case WM_MOUSEMOVE:
        if (SCROLL_Scrolling)
        {
          pt.x = (SHORT)LOWORD(lParam);
          pt.y = (SHORT)HIWORD(lParam);
          hittest = SCROLL_HitTest(hwnd,nBar,pt,TRUE);
          prevPt = pt;
        } else return res;
        break;

      case WM_LBUTTONUP:
        if (SCROLL_Scrolling)
        {
          pt.x = (SHORT)LOWORD(lParam);
          pt.y = (SHORT)HIWORD(lParam);
          hittest = SCROLL_NOWHERE;
          ReleaseCapture();
          SCROLL_Scrolling = FALSE;
        } else return res;
        break;

      case WM_CAPTURECHANGED:
        if (SCROLL_Scrolling)
        {
          hittest = SCROLL_NOWHERE;
          SCROLL_Scrolling = FALSE;
        } else return res;
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
            hdc = GetDCEx(hwnd,0,DCX_CACHE | ((nBar == SB_CTL) ? 0:DCX_WINDOW));
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
          if (!SCROLL_Scrolling)
          {
            hdc = GetDCEx(hwnd,0,DCX_CACHE | ((nBar == SB_CTL) ? 0 : DCX_WINDOW));
            SCROLL_DrawScrollBar(hwnd,hdc,nBar,FALSE,TRUE);
            ReleaseDC(hwnd,hdc);
          }
          return res;
        } else return res;

      default:
        return res;  /* Should never happen */
    }

    hdc = GetDCEx(hwnd,0,DCX_CACHE | ((nBar == SB_CTL) ? 0 : DCX_WINDOW));

    switch(SCROLL_trackHitTest)
    {
      case SCROLL_NOWHERE:  /* No tracking in progress */
        break;

      case SCROLL_TOP_ARROW:
        if ((msg == WM_LBUTTONUP) || (msg == WM_CAPTURECHANGED))
          KillSystemTimer(hwnd,SCROLL_TIMER);
        else if ((msg == WM_LBUTTONDOWN) || (!timerRunning && msg == WM_SYSTIMER))
        {
          SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                          SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                          (TIMERPROC)0 );
          if (msg != WM_LBUTTONDOWN) timerRunning = TRUE;
        }

        if ((msg == WM_LBUTTONDOWN) || (SCROLL_lastHitTest != hittest))
        {
          SCROLL_DrawTopArrow(hdc,infoPtr,&rect,arrowSize,vertical,(hittest == SCROLL_trackHitTest));
          SCROLL_lastHitTest = hittest;
        }
        if ((hittest == SCROLL_trackHitTest) && ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)))
          SendMessageA(hwndOwner,vertical ? WM_VSCROLL:WM_HSCROLL,SB_LINEUP,hwndCtl);

        break;

      case SCROLL_TOP_RECT:
        if ((msg == WM_LBUTTONUP) || (msg == WM_CAPTURECHANGED))
          KillSystemTimer(hwnd,SCROLL_TIMER);
        else if ((msg == WM_LBUTTONDOWN) || (!timerRunning && (msg == WM_SYSTIMER)))
        {
          SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                          SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                          (TIMERPROC)0 );
          if (msg != WM_LBUTTONDOWN) timerRunning = TRUE;
        }

        if ((msg == WM_LBUTTONDOWN) || (SCROLL_lastHitTest != hittest))
        {
          SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                             thumbPos, infoPtr->flags, vertical,
                             (hittest == SCROLL_trackHitTest), FALSE );
          SCROLL_lastHitTest = hittest;
        }

        if ((hittest == SCROLL_trackHitTest) && ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)))
          SendMessageA(hwndOwner,vertical ? WM_VSCROLL:WM_HSCROLL,SB_PAGEUP,hwndCtl);

        break;

      case SCROLL_THUMB:
        if (msg == WM_LBUTTONDOWN)
        {
            SCROLL_TrackingWin = hwnd;
            SCROLL_TrackingBar = nBar;
            SCROLL_TrackingPos = trackThumbPos + lastMousePos - lastClickPos;
            SCROLL_TrackingVal = infoPtr->CurVal;
            SCROLL_MovingThumb = TRUE;
            thumbTrackSent = FALSE;
            SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
        } else if ((msg == WM_LBUTTONUP) || (msg == WM_CAPTURECHANGED))
        {
          UINT val;
          INT oldPos = infoPtr->CurVal;

          SCROLL_MovingThumb = FALSE;
          SCROLL_TrackingWin = 0;
          SCROLL_trackHitTest = SCROLL_NOWHERE;  /* Terminate tracking */
          val = SCROLL_GetThumbVal( infoPtr, &rect, vertical,
                                    trackThumbPos + lastMousePos - lastClickPos );

          if ((val != infoPtr->CurVal) || thumbTrackSent)
            SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                          MAKEWPARAM( SB_THUMBPOSITION, val ), hwndCtl );

          if (oldPos == infoPtr->CurVal)
          {
            vertical = SCROLL_GetScrollBarRect( hwnd, nBar, &rect,
                                        &arrowSize, &thumbSize, &thumbPos );
            SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                                 thumbPos, infoPtr->flags, vertical,
                                 FALSE, FALSE );
          }

          ReleaseDC(hwnd,hdc);
          return res;
        } else if (msg == WM_MOUSEMOVE)
        {
            UINT pos;

#ifdef __WIN32OS2__
            if (!SCROLL_PtInRectEx( &rect, pt, vertical ) && !fOS2Look) pos = lastClickPos;
#else
            if (!SCROLL_PtInRectEx( &rect, pt, vertical )) pos = lastClickPos;
#endif
            else
            {
                pt = SCROLL_ClipPos( &rect, pt );
                pos = vertical ? pt.y:pt.x;
            }
            if (pos != lastMousePos)
            {
                lastMousePos = pos;
                SCROLL_TrackingPos = trackThumbPos + pos - lastClickPos;
                SCROLL_TrackingVal = SCROLL_GetThumbVal( infoPtr, &rect,
                                                         vertical,
                                                         SCROLL_TrackingPos );
                SCROLL_DrawMovingThumb( hdc, &rect, vertical,
                                        arrowSize, thumbSize );
                SendMessageA( hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
                                MAKEWPARAM( SB_THUMBTRACK, SCROLL_TrackingVal),
                                hwndCtl );
                thumbTrackSent = TRUE;
            }
        }
        break;

      case SCROLL_BOTTOM_RECT:
        if ((msg == WM_LBUTTONUP) || (msg == WM_CAPTURECHANGED))
          KillSystemTimer(hwnd,SCROLL_TIMER);
        else if ((msg == WM_LBUTTONDOWN) || (!timerRunning && (msg == WM_SYSTIMER)))
        {
          SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                          SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                          (TIMERPROC)0 );
          if (msg != WM_LBUTTONDOWN) timerRunning = TRUE;
        }

        if ((msg == WM_LBUTTONDOWN) || (SCROLL_lastHitTest != hittest))
        {
          SCROLL_DrawInterior( hwnd, hdc, nBar, &rect, arrowSize, thumbSize,
                               thumbPos, infoPtr->flags, vertical,
                               FALSE, (hittest == SCROLL_trackHitTest) );
          SCROLL_lastHitTest = hittest;
        }

        if ((hittest == SCROLL_trackHitTest) && ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)))
          SendMessageA(hwndOwner,vertical ? WM_VSCROLL:WM_HSCROLL,SB_PAGEDOWN,hwndCtl);

        break;

      case SCROLL_BOTTOM_ARROW:
        if ((msg == WM_LBUTTONUP) || (msg == WM_CAPTURECHANGED))
          KillSystemTimer(hwnd,SCROLL_TIMER);
        else if ((msg == WM_LBUTTONDOWN) || (!timerRunning && (msg == WM_SYSTIMER)))
        {
          SetSystemTimer( hwnd, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
                          SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY,
                          (TIMERPROC)0 );
          if (msg != WM_LBUTTONDOWN) timerRunning = TRUE;
        }

        if ((msg == WM_LBUTTONDOWN) || (SCROLL_lastHitTest != hittest))
        {
          SCROLL_DrawBottomArrow(hdc,infoPtr,&rect,arrowSize,vertical,(hittest == SCROLL_trackHitTest));
          SCROLL_lastHitTest = hittest;
        }
        if ((hittest == SCROLL_trackHitTest) && ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)))
          SendMessageA(hwndOwner,vertical ? WM_VSCROLL:WM_HSCROLL,SB_LINEDOWN,hwndCtl);

        break;
    }

    if ((msg == WM_LBUTTONUP) || (msg == WM_CAPTURECHANGED))
    {
      SCROLL_trackHitTest = SCROLL_NOWHERE;  /* Terminate tracking */

      SendMessageA(hwndOwner,vertical ? WM_VSCROLL:WM_HSCROLL,SB_ENDSCROLL,hwndCtl);
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

    case WM_DESTROY:
      return SCROLL_Destroy(hwnd,wParam,lParam);

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCHITTEST:
    case WM_CAPTURECHANGED:
    case WM_MOUSEMOVE:
    case WM_SYSTIMER:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      return SCROLL_HandleScrollEvent(hwnd,wParam,lParam,SB_CTL,message);

    case WM_KEYDOWN:
      return SCROLL_KeyDown(hwnd,wParam,lParam);

#ifdef __WIN32OS2__
    case WM_ENABLE:
      return SCROLL_Enable(hwnd, wParam);

    case WM_SYSCOMMAND:
      //Not entirely sure this is 100% correct, but it seems the logical thing to do
      //(for scrollbar control windows with the size grip style)
      //Might need to send it to top parent
      return SendMessageA(GetParent(hwnd), message, wParam, lParam);
#endif

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
      return SetScrollInfo(hwnd,SB_CTL,(SCROLLINFO*)lParam,wParam);

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

/* Scrollbar API */

/*************************************************************************
 *           SetScrollInfo   (USER32.501)
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
INT WINAPI SetScrollInfo(HWND hwnd,INT nBar,const SCROLLINFO *info,BOOL bRedraw)
{
    /* Update the scrollbar state and set action flags according to
     * what has to be done graphics wise. */

    SCROLLBAR_INFO *infoPtr;
    UINT new_flags;
    INT action = 0;
    BOOL bChangeParams = FALSE; /* don't show/hide scrollbar if params don't change */

    dprintf(("USER32: SetScrollInfo %x %d",hwnd,nBar));

    if (!(infoPtr = SCROLL_GetInfoPtr(hwnd,nBar))) return 0;
    if (info->fMask & ~(SIF_ALL | SIF_DISABLENOSCROLL)) return 0;
    if ((info->cbSize != sizeof(*info)) &&
        (info->cbSize != sizeof(*info)-sizeof(info->nTrackPos))) return 0;

    /* Set the page size */
    if (info->fMask & SIF_PAGE)
    {
        if( infoPtr->Page != info->nPage )
        {
            infoPtr->Page = info->nPage;
            action |= SA_SSI_REPAINT_INTERIOR;
            bChangeParams = TRUE; 
        }
    }

    /* Set the scroll pos */
    if (info->fMask & SIF_POS)
    {
        //dsprintf(scroll, " pos=%d", info->nPos );
        if( infoPtr->CurVal != info->nPos )
        {
            infoPtr->CurVal = info->nPos;
            action |= SA_SSI_MOVE_THUMB;
        }
    }

    /* Set the scroll range */
    if (info->fMask & SIF_RANGE)
    {
        /* Invalid range -> range is set to (0,0) */
        if ((info->nMin > info->nMax) ||
            ((UINT)(info->nMax - info->nMin) >= 0x80000000))
        {
            //NOTE: This does not fail in NT4 (unlike SetScrollRange)
            infoPtr->MinVal = 0;
            infoPtr->MaxVal = 0;
            bChangeParams = TRUE; 
        }
        else
        {
            if( infoPtr->MinVal != info->nMin ||
                infoPtr->MaxVal != info->nMax )
            {
                action |= SA_SSI_REPAINT_INTERIOR;
                infoPtr->MinVal = info->nMin;
                infoPtr->MaxVal = info->nMax;
                bChangeParams = TRUE; 
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

    //testestest
    dprintf(("new values: page=%d pos=%d min=%d max=%d\n",
                 infoPtr->Page, infoPtr->CurVal,
                 infoPtr->MinVal, infoPtr->MaxVal ));

    /* don't change the scrollbar state if SetScrollInfo
     * is just called with SIF_DISABLENOSCROLL
     */
    if(!(info->fMask & SIF_ALL)) goto done;

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
          action |= SA_SSI_REFRESH;
        } 
        else 
        if (nBar != SB_CTL && bChangeParams)
        {
          action = SA_SSI_HIDE;
          infoPtr->flags = 0;
          goto done;
        }
      } 
      else  /* Show and enable scroll-bar */
      {
        new_flags = 0;
        if (nBar != SB_CTL && bChangeParams) action |= SA_SSI_SHOW;
        if (infoPtr->flags) action |= SA_SSI_REFRESH;
      }

      if (infoPtr->flags != new_flags) /* check arrow flags */
      {
        infoPtr->flags = new_flags;
        action |= SA_SSI_REPAINT_ARROWS;
      }
    }

done:
    /* Update scrollbar */

    if( action & SA_SSI_HIDE )
      ShowScrollBar(hwnd,nBar,FALSE);
    else
    {
        if(action & SA_SSI_SHOW)
          ShowScrollBar(hwnd,nBar,TRUE);

        if (bRedraw)
        {
          if (action & SA_SSI_REFRESH)
            SCROLL_RefreshScrollBar(hwnd,nBar,TRUE,TRUE);
          else
          {
            if (action & (SA_SSI_REPAINT_INTERIOR | SA_SSI_MOVE_THUMB))
              SCROLL_RefreshScrollBar(hwnd,nBar,FALSE,TRUE);
            if (action & SA_SSI_REPAINT_ARROWS)
              SCROLL_RefreshScrollBar(hwnd,nBar,TRUE,FALSE);
          }
        }
    }

    /* Return current position */

    return infoPtr->CurVal;
}
/*************************************************************************
 *           GetScrollInfo   (USER32.284)
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

    dprintf(("USER32: GetScrollInfo"));

    if (!(infoPtr = SCROLL_GetInfoPtr(hwnd,nBar))) return FALSE;
    if (info->fMask & ~(SIF_ALL | SIF_DISABLENOSCROLL)) return FALSE;
    if ((info->cbSize != sizeof(*info)) &&
        (info->cbSize != sizeof(*info)-sizeof(info->nTrackPos))) return FALSE;

    if (info->fMask & SIF_PAGE) info->nPage = infoPtr->Page;
    if (info->fMask & SIF_POS) info->nPos = infoPtr->CurVal;
    if ((info->fMask & SIF_TRACKPOS) && (info->cbSize == sizeof(*info)))
      info->nTrackPos = (SCROLL_MovingThumb && SCROLL_TrackingWin == hwnd && SCROLL_TrackingBar == nBar) ? SCROLL_TrackingVal:infoPtr->CurVal;

    if (info->fMask & SIF_RANGE)
    {
        info->nMin = infoPtr->MinVal;
        info->nMax = infoPtr->MaxVal;
    }
    return (info->fMask & SIF_ALL) != 0;
}
/*************************************************************************
 *           SetScrollPos   (USER32.502)
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
    if (!(infoPtr = SCROLL_GetInfoPtr(hwnd,nBar))) return 0;
    oldPos      = infoPtr->CurVal;
    info.cbSize = sizeof(info);
    info.nPos   = nPos;
    info.fMask  = SIF_POS;
    SetScrollInfo( hwnd, nBar, &info, bRedraw );
    return oldPos;
}
/*************************************************************************
 *           GetScrollPos   (USER32.285)
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

    infoPtr = SCROLL_GetInfoPtr(hwnd,nBar);
    if (!infoPtr) return 0;

    return infoPtr->CurVal;
}

/*************************************************************************
 *           SetScrollRange   (USER32.503)
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

    //This is what NT4 does
    if ((MinVal > MaxVal) ||
        ((UINT)(MaxVal - MinVal) >= 0x80000000))
    {
        dprintf(("Invalid range"));
        SetLastError(ERROR_INVALID_SCROLLBAR_RANGE);
        return FALSE;
    }

    info.cbSize = sizeof(info);
    info.nMin   = MinVal;
    info.nMax   = MaxVal;
    info.fMask  = SIF_RANGE;
    SetScrollInfo( hwnd, nBar, &info, bRedraw );
    return TRUE;
}

/*************************************************************************
 *           GetScrollRange   (USER32.286)
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

    infoPtr = SCROLL_GetInfoPtr(hwnd,nBar);
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
 *           ShowScrollBar   (USER32.532)
 *
 * RETURNS STD
 */
BOOL WINAPI ShowScrollBar(
                    HWND hwnd,  /* [I] Handle of window whose scrollbar(s) will be affected   */
                    INT nBar,   /* [I] One of SB_HORZ, SB_VERT, SB_BOTH or SB_CTL */
                    BOOL fShow  /* [I] TRUE = show, FALSE = hide  */)
{
    BOOL fShowH = (nBar == SB_HORZ) ? fShow : 0;
    BOOL fShowV = (nBar == SB_VERT) ? fShow : 0;
    DWORD dwStyle;

    dprintf(("ShowScrollBar %04x %d %d\n", hwnd, nBar, fShow));
    if (!IsWindow(hwnd)) return FALSE;

    dwStyle = GetWindowLongA(hwnd, GWL_STYLE);

    //CB: does Win32 send a WM_STYLECHANGED message?
    switch(nBar)
    {
      case SB_CTL:
       ShowWindow(hwnd,fShow ? SW_SHOW:SW_HIDE);
       return TRUE;

      case SB_BOTH:
      case SB_HORZ:
        if (fShow)
        {
            fShowH = !(dwStyle & WS_HSCROLL);
            SetWindowLongA(hwnd, GWL_STYLE, dwStyle | WS_HSCROLL);
        }
        else  /* hide it */
        {
            fShowH = (dwStyle & WS_HSCROLL);
            SetWindowLongA(hwnd, GWL_STYLE, dwStyle & ~WS_HSCROLL);
        }
        if( nBar == SB_HORZ )
        {
            fShowV = FALSE;
            break;
        }
        /* fall through */

    case SB_VERT:
        if (fShow)
        {
            fShowV = !(dwStyle & WS_VSCROLL);
            SetWindowLongA(hwnd, GWL_STYLE, dwStyle | WS_VSCROLL);
        }
        else  /* hide it */
        {
            fShowV = (dwStyle & WS_VSCROLL);
            SetWindowLongA(hwnd, GWL_STYLE, dwStyle & ~WS_VSCROLL);
        }
        if ( nBar == SB_VERT )
           fShowH = FALSE;
        break;

    default:
        return TRUE;  /* Nothing to do! */
    }

    if( fShowH || fShowV ) /* frame has been changed, let the window redraw itself */
    {
        SetWindowPos( hwnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE
                    | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED );
    }

    return TRUE;
}

/*************************************************************************
 *           EnableScrollBar   (USER32.171)
 */
BOOL WINAPI EnableScrollBar( HWND hwnd, INT nBar, UINT flags)
{
    BOOL bFineWithMe;
    SCROLLBAR_INFO *infoPtr;

    dprintf(("EnableScrollBar %04x %d %d\n", hwnd, nBar, flags));

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

BOOL WINAPI GetScrollBarInfo(HWND hwnd,LONG idObject,PSCROLLBARINFO psbi)
{
  if (!psbi || (psbi->cbSize != sizeof(SCROLLBARINFO)))
  {
    SetLastError(ERROR_INVALID_PARAMETER);

    return FALSE;
  }

  INT nBar,arrowSize;

  switch (idObject)
  {
    case OBJID_CLIENT:
      nBar = SB_CTL;
      break;

    case OBJID_HSCROLL:
      nBar = SB_HORZ;
      break;

    case OBJID_VSCROLL:
      nBar = SB_VERT;
      break;

    default:
      return FALSE;
  }

  SCROLL_GetScrollBarRect(hwnd,nBar,&psbi->rcScrollBar,&arrowSize,&psbi->dxyLineButton,&psbi->xyThumbTop);
  psbi->xyThumbBottom = psbi->xyThumbTop+psbi->dxyLineButton;
  psbi->bogus = 0; //CB: undocumented!
  psbi->rgstate[0] = IsWindowVisible(hwnd) ? STATE_SYSTEM_INVISIBLE:0;
  psbi->rgstate[1] = psbi->rgstate[2] = psbi->rgstate[3] = psbi->rgstate[4] = psbi->rgstate[5] = psbi->rgstate[0]; //CB: todo

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL SCROLLBAR_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(SCROLLBARCLASSNAME)) return FALSE;

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

