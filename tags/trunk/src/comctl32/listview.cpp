/*$Id: listview.cpp,v 1.19 2000-05-22 17:25:08 cbratschi Exp $*/
/*
 * Listview control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Luc Tourangeau
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999-2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * NOTES
 *   Listview control implementation.
 *
 * TODO:
 *   - mouse selection
 *   - DrawText with end ellipsis
 *   - Hot item handling.
 *   - rcView and origin (need testcase!!!)
 *   - rcView not updated in LVM_SETITEMPOSITION and other messages
 *   - many position bugs
 *   - work area not used
 *   - custom draw
 *   - drag'n'drop (-> treeview (CCBase: need drag'n'dop framework))
 *   - edit timer
 *   - LISTVIEW_SetBkImage
 *   - LISTVIEW_GetBkImage
 *   - LVS_NOLABELWRAP
 *   - LVS_OWNERDATA
 *   - LVS_OWNERDRAWFIXED
 *   - LVS_EX_*
 *       LVS_EX_CHECKBOXES
 *       LVS_EX_TRACKSELECT
 *       LVS_EX_FULLROWSELECT
 *       LVS_EX_ONECLICKACTIVATE
 *       LVS_EX_TWOCLICKACTIVATE
 *       LVS_EX_FLATSB
 *       LVS_EX_REGIONAL
 *       LVS_EX_INFOTIP
 *       LVS_EX_UNDERLINEHOT
 *       LVS_EX_UNDERLINECOLD
 *       LVS_EX_MULTIWORKAREAS
 *   - many things untested!
 *
 * Data structure:
 *   LISTVIEW_SetItemCount : not implemented
 *
 * Status: all messages done, many styles not (yet) implemented
 * Version: 5.80
 */

/*
 Most identical with:
 - Corel 20000513 level
 - (WINE 20000130 level)
*/

#include <stdio.h>
#include "winbase.h"
#include "commctrl.h"
#include "ccbase.h"
#include "listview.h"
#include "comctl32.h"
#include "header.h"
#include "ctype.h"

/* constants */

#define DISP_TEXT_SIZE 128 // maximum size of a label

#define WIDTH_PADDING 12           // padding for items in list and small icon display modes
#define HEIGHT_PADDING 1           // padding for items in list, report and small icon display modes
#define REPORT_MARGINX 2           // offset of items in report display mode
#define ICON_TOP_PADDING 2         // padding for icon in large icon display mode
#define ICON_BOTTOM_PADDING 2
#define LABEL_VERT_OFFSET 2        // padding for label in large icon display mode
#define DEFAULT_LABEL_WIDTH 40     // default label width for items in list and small icon display modes
#define DEFAULT_COLUMN_WIDTH 96    // default column width for items in list display mode
#define LISTVIEW_SCROLL_DIV_SIZE 1 // Increment size of the horizontal scroll bar
#define DEFAULT_HOVERTIME    500   // hover delay
#define LV_ISEARCH_DELAY     1000  // documented in ListView_Message_Processing.htm

/* macros */

#define GETITEMCOUNT(infoPtr) ((infoPtr)->hdpaItems->nItemCount) // retrieve the number of items in the listview
#define LISTVIEW_GetInfoPtr(hwnd) ((LISTVIEW_INFO*)getInfoPtr(hwnd))

/* forward declarations */

HWND CreateEditLabel(LPCSTR text,DWORD style,INT x,INT y,INT width,INT height,HWND parent,HINSTANCE hinst,EditlblCallback EditLblCb,DWORD param);

static INT     LISTVIEW_HitTestItem(HWND, LPLVHITTESTINFO);
static INT     LISTVIEW_GetColumnCount(HWND);
static INT     LISTVIEW_GetCountPerRow(HWND);
static INT     LISTVIEW_GetCountPerColumn(HWND);
static LISTVIEW_ITEMDATA* LISTVIEW_GetItemData(HDPA hdpaSubItems,INT nSubItem);
static BOOL    LISTVIEW_GetItemPosition(HWND, INT, LPPOINT);
static LRESULT LISTVIEW_GetItemRect(HWND hwnd,INT nItem,LPRECT lprc,INT code);
static INT     LISTVIEW_GetLabelWidth(HWND,INT,INT);
static LRESULT LISTVIEW_GetOrigin(HWND, LPPOINT);
static LRESULT LISTVIEW_GetViewRect(HWND, LPRECT);
static VOID    LISTVIEW_RemoveSelections(HWND, INT, INT);
static BOOL    LISTVIEW_RemoveItemData(HDPA,INT);
static BOOL    LISTVIEW_SetItemFocus(HWND, INT);
static BOOL    LISTVIEW_SetItemPosition(HWND,INT,INT,INT,BOOL);
static VOID    LISTVIEW_UpdateSize(HWND);
static LRESULT LISTVIEW_SetViewRect(HWND, LPRECT);
static LRESULT LISTVIEW_GetItem(HWND hwnd,LPLVITEMW lpLVItem,BOOL unicode,BOOL internal);
static INT     LISTVIEW_GetTopIndex(HWND hwnd);
static LRESULT LISTVIEW_GetItemState(HWND hwnd, INT nItem, UINT uMask);
static LRESULT LISTVIEW_SetItemState(HWND hwnd,INT nItem,DWORD data,DWORD mask);
static LRESULT LISTVIEW_GetStringWidth(HWND hwnd,HDC hdc,LPWSTR lpszText,BOOL unicode);
static BOOL    LISTVIEW_EnsureVisible(HWND hwnd,INT nItem,BOOL bPartial);
static VOID    LISTVIEW_UpdateHeaderSize(HWND hwnd,INT nNewScrollPos,INT xScroll);

/* code */

static VOID LISTVIEW_Refresh(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  if (infoPtr->internalFlags & IF_NOREDRAW) return;

  InvalidateRect(hwnd,&infoPtr->rcList,TRUE);
}

static LRESULT LISTVIEW_Timer(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return DefWindowProcA(hwnd,WM_TIMER,wParam,lParam);
}

static VOID LISTVIEW_RefreshItem(HWND hwnd,INT nItem,BOOL wholeItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  RECT rect;

  if (infoPtr->internalFlags & IF_NOREDRAW) return;
  LISTVIEW_GetItemRect(hwnd,nItem,&rect,wholeItem ? LVIR_BOUNDS:LVIR_SELECTBOUNDS);

  if ((infoPtr->uView == LVS_REPORT) && !(infoPtr->dwStyle & LVS_NOCOLUMNHEADER))
  {
    RECT rect2;

    //clip header control
    if (nItem < LISTVIEW_GetTopIndex(hwnd)) return;

    //clip width
    HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)0,(LPARAM)&rect2);
    if (rect.right > rect2.right) rect.right = rect2.right;
  }

  InvalidateRect(hwnd,&rect,TRUE);
}

static VOID LISTVIEW_RefreshItems(HWND hwnd,INT nFirst,INT nLast)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  RECT rect;

  if (infoPtr->internalFlags & IF_NOREDRAW) return;

  if ((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON))
  {
    INT x;
    RECT rect;

    for (x = nFirst;x < nLast;x++)
    {
      LISTVIEW_GetItemRect(hwnd,x,&rect,LVIR_BOUNDS);
      InvalidateRect(hwnd,&rect,TRUE);
    }
  } else if (infoPtr->uView == LVS_REPORT)
  {
    INT nTop = LISTVIEW_GetTopIndex(hwnd),nBottom,xOffset = infoPtr->lefttop.x*infoPtr->scrollStep.x;
    RECT rcHeader,rect;

    if (nTop > nLast) return;
    if (nTop > nFirst) nFirst = nTop;

    nBottom = nTop+LISTVIEW_GetCountPerColumn(hwnd)+1;
    nBottom = min(nTop,GETITEMCOUNT(infoPtr));

    if (nFirst > nBottom) return;
    if (nLast > nBottom) nLast = nBottom;

    HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)0,(LPARAM)&rcHeader);
    rcHeader.left += REPORT_MARGINX;
    rcHeader.right = max(rcHeader.left,rcHeader.right-REPORT_MARGINX);

    if (infoPtr->dwStyle & WS_HSCROLL)
    {
      rcHeader.left -= xOffset;
      rcHeader.right -= xOffset;
    }

    rect.left = rcHeader.left;
    rect.right = rcHeader.right;
    rect.top = infoPtr->rcList.top+(nFirst-nTop)*infoPtr->nItemHeight;
    rect.bottom = rect.top+(nLast-nFirst+1)*infoPtr->nItemHeight;
    InvalidateRect(hwnd,&rect,TRUE);
  } else if (infoPtr->uView == LVS_LIST)
  {
    INT nColumnCount = LISTVIEW_GetColumnCount(hwnd);
    INT nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);
    INT nTop = LISTVIEW_GetTopIndex(hwnd);
    INT xStart = 0,yStart = 0,xEnd,yEnd,x;
    RECT rect;

    if (nLast < nTop) return;
    if (nFirst < nTop)
      nFirst = nTop;
    else
    {
      xStart = (nFirst-nTop)/nCountPerColumn;
      yStart = (nFirst-nTop)%nCountPerColumn;
      if (xStart >= nColumnCount) return;
    }
    xEnd = (nLast-nTop)/nCountPerColumn;
    yEnd = (nLast-nTop)%nCountPerColumn;
    if (xEnd >= nColumnCount)
    {
      xEnd = nColumnCount-1;
      yEnd = nCountPerColumn-1;
    }

    for (x = xStart;x <= xEnd;x++)
    {
      if (x == xStart)
        rect.top = yStart*infoPtr->nItemHeight;
      else
        rect.top = 0;
      rect.left = x*infoPtr->nItemWidth;
      if (x == xEnd)
        rect.bottom = rect.top+(yEnd+1)*infoPtr->nItemHeight;
      else
        rect.bottom = rect.top+nCountPerColumn*infoPtr->nItemHeight;
      rect.right = rect.left+infoPtr->nItemWidth;
      InvalidateRect(hwnd,&rect,TRUE);
    }
  }
}

static VOID LISTVIEW_RefreshSubItem(HWND hwnd,INT nItem,INT nSubItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  RECT rect,column;
  INT xOffset = infoPtr->lefttop.x*infoPtr->scrollStep.x;

  if (infoPtr->internalFlags & IF_NOREDRAW) return;

  if (infoPtr->uView != LVS_REPORT) return;
  if (nItem < LISTVIEW_GetTopIndex(hwnd)) return;
  LISTVIEW_GetItemRect(hwnd,nItem,&rect,LVIR_SELECTBOUNDS);

  //get column rect
  HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)nSubItem,(LPARAM)&column);
  rect.left = column.left+REPORT_MARGINX;
  rect.right = max(column.left,column.right-REPORT_MARGINX);

  /* Offset the Scroll Bar Pos */
  rect.left -= xOffset;
  rect.right -= xOffset;

  InvalidateRect(hwnd,&rect,TRUE);
}

static VOID LISTVIEW_ScrollWindow(HWND hwnd,INT xScroll,INT yScroll)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  if (infoPtr->internalFlags & IF_NOREDRAW) return;

  if ((infoPtr->uView == LVS_REPORT) && !(infoPtr->dwStyle & LVS_NOCOLUMNHEADER))
  {
    RECT rect = infoPtr->rcList;

    //clip header
    if (yScroll < 0)
    { //up
      rect.top -= yScroll;
      if (rect.top < rect.bottom)
      {
        INT h;

        ScrollWindowEx(hwnd,xScroll,yScroll,&rect,NULL,0,NULL,SW_INVALIDATE);
        h = rect.bottom-rect.top;
        rect.top = infoPtr->rcList.top+h;
        rect.bottom = infoPtr->rcList.bottom-h;
        if (rect.bottom > rect.top) InvalidateRect(hwnd,&rect,TRUE);
      } else
      {
        InvalidateRect(hwnd,&infoPtr->rcList,TRUE);
      }
    } else
    { //down
      ScrollWindowEx(hwnd,xScroll,yScroll,&rect,NULL,0,NULL,SW_INVALIDATE);
    }

    if (xScroll != 0) LISTVIEW_UpdateHeaderSize(hwnd,infoPtr->lefttop.x,xScroll);
  } else ScrollWindowEx(hwnd,xScroll,yScroll,NULL,NULL,0,NULL,SW_INVALIDATE);
}

/*************************************************************************
 * LISTVIEW_UpdateHeaderSize [Internal]
 *
 * Function to resize the header control
 *
 * PARAMS
 *     hwnd             [I] handle to a window
 *     nNewScrollPos    [I] Scroll Pos to Set
 *
 * RETURNS
 *     nothing
 *
 * NOTES
 */
static VOID LISTVIEW_UpdateHeaderSize(HWND hwnd, INT nNewScrollPos,INT xScroll)
{
    LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
    RECT winRect;
    POINT point[2];

    GetWindowRect(infoPtr->hwndHeader, &winRect);
    point[0].x = winRect.left;
    point[0].y = winRect.top;
    point[1].x = winRect.right;
    point[1].y = winRect.bottom;

    MapWindowPoints(HWND_DESKTOP, hwnd, point, 2);
    point[0].x = -nNewScrollPos*infoPtr->scrollStep.x;
    point[1].x += nNewScrollPos*infoPtr->scrollStep.x;

    //CB: todo: smoother scrolling (BTW: SWP_NOREDRAW doesn't work!)
    SetWindowPos(infoPtr->hwndHeader,0,point[0].x,point[0].y,point[1].x,point[1].y,SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
}

/***
 * DESCRIPTION:
 * Update the scrollbars. This functions should be called whenever
 * the content, size or view changes.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_UpdateScroll(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nListHeight = infoPtr->rcList.bottom - infoPtr->rcList.top;
  INT nListWidth = infoPtr->rcList.right - infoPtr->rcList.left;
  SCROLLINFO scrollInfo;

  if (infoPtr->dwStyle & LVS_NOSCROLL)
  {
    infoPtr->lefttop.x = 0;
    infoPtr->lefttop.y = 0;
    infoPtr->maxScroll = infoPtr->lefttop;
    infoPtr->scrollPage = infoPtr->lefttop;
    infoPtr->scrollStep = infoPtr->lefttop;
    ShowScrollBar(hwnd,SB_BOTH,FALSE);
    //update scroll flags
    infoPtr->dwStyle =  GetWindowLongA(hwnd,GWL_STYLE);
    return;
  }

  ZeroMemory(&scrollInfo, sizeof(SCROLLINFO));
  scrollInfo.cbSize = sizeof(SCROLLINFO);

  if (infoPtr->uView == LVS_LIST)
  {
    /* update horizontal scrollbar */
    INT nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);
    INT nCountPerRow = LISTVIEW_GetCountPerRow(hwnd);
    INT nNumOfItems = GETITEMCOUNT(infoPtr);

    infoPtr->maxScroll.x = nNumOfItems/nCountPerColumn;
    if (nNumOfItems % nCountPerColumn)
      infoPtr->maxScroll.x++;

    infoPtr->lefttop.x = LISTVIEW_GetTopIndex(hwnd)/nCountPerColumn;
    infoPtr->scrollPage.x = nCountPerRow;
    infoPtr->scrollStep.x = infoPtr->nItemWidth;

    scrollInfo.nMin  = 0;
    scrollInfo.nMax  = infoPtr->maxScroll.x-1;
    scrollInfo.nPos  = infoPtr->lefttop.x;
    scrollInfo.nPage = infoPtr->scrollPage.x;
    scrollInfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,TRUE);
  } else if (infoPtr->uView == LVS_REPORT)
  {
    /* update vertical scrollbar */
    infoPtr->maxScroll.y = GETITEMCOUNT(infoPtr);
    infoPtr->lefttop.y = LISTVIEW_GetTopIndex(hwnd);
    infoPtr->scrollPage.y = LISTVIEW_GetCountPerColumn(hwnd);
    infoPtr->scrollStep.y = infoPtr->nItemHeight;

    scrollInfo.nMin  = 0;
    scrollInfo.nMax  = infoPtr->maxScroll.y-1;
    scrollInfo.nPos  = infoPtr->lefttop.y;
    scrollInfo.nPage = infoPtr->scrollPage.y;
    scrollInfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    SetScrollInfo(hwnd,SB_VERT,&scrollInfo,TRUE);

    /* update horizontal scrollbar */
    nListWidth = infoPtr->rcList.right - infoPtr->rcList.left;
    if (!(infoPtr->dwStyle & WS_HSCROLL) || !GETITEMCOUNT(infoPtr))
      infoPtr->lefttop.x = 0;

    infoPtr->scrollPage.x = nListWidth / LISTVIEW_SCROLL_DIV_SIZE;
    infoPtr->maxScroll.x = max(infoPtr->nItemWidth / LISTVIEW_SCROLL_DIV_SIZE, 0);
    infoPtr->scrollStep.x = LISTVIEW_SCROLL_DIV_SIZE;

    scrollInfo.nMin  = 0;
    scrollInfo.nMax  = infoPtr->maxScroll.x-1;
    scrollInfo.nPos  = infoPtr->lefttop.x;
    scrollInfo.nPage = infoPtr->scrollPage.x;
    scrollInfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,TRUE);

    /* Update the Header Control */
    LISTVIEW_UpdateHeaderSize(hwnd,infoPtr->lefttop.x,0);
  } else
  {
    RECT rcView;

    if (LISTVIEW_GetViewRect(hwnd,&rcView))
    {
      INT nViewWidth = rcView.right - rcView.left;
      INT nViewHeight = rcView.bottom - rcView.top;

      /* Update Horizontal Scrollbar */
      if (!(infoPtr->dwStyle & WS_HSCROLL) || !GETITEMCOUNT(infoPtr))
        infoPtr->lefttop.x = 0;
      infoPtr->maxScroll.x = max(nViewWidth / LISTVIEW_SCROLL_DIV_SIZE, 0);
      infoPtr->scrollPage.x = nListWidth / LISTVIEW_SCROLL_DIV_SIZE;
      infoPtr->scrollStep.x = LISTVIEW_SCROLL_DIV_SIZE;

      scrollInfo.nMin  = 0;
      scrollInfo.nMax  = infoPtr->maxScroll.x-1;
      scrollInfo.nPos  = infoPtr->lefttop.x;
      scrollInfo.nPage = infoPtr->scrollPage.x;
      scrollInfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
      SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,TRUE);

      /* Update Vertical Scrollbar */
      nListHeight = infoPtr->rcList.bottom - infoPtr->rcList.top;
      if (!(infoPtr->dwStyle & WS_VSCROLL) || !GETITEMCOUNT(infoPtr))
        infoPtr->lefttop.x = 0;
      infoPtr->maxScroll.y = max(nViewHeight / LISTVIEW_SCROLL_DIV_SIZE,0);
      infoPtr->scrollPage.y = nListHeight / LISTVIEW_SCROLL_DIV_SIZE;
      infoPtr->scrollStep.y = LISTVIEW_SCROLL_DIV_SIZE;

      scrollInfo.nMin  = 0;
      scrollInfo.nMax  = infoPtr->maxScroll.y-1;
      scrollInfo.nPos  = infoPtr->lefttop.y;
      scrollInfo.nPage = infoPtr->scrollPage.y;
      scrollInfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
      SetScrollInfo(hwnd,SB_VERT,&scrollInfo,TRUE);
    }
  }
  //update scroll flags
  infoPtr->dwStyle =  GetWindowLongA(hwnd,GWL_STYLE);
}

/***
 * DESCRIPTION:
 * Aligns the items with the top edge of the window.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_AlignTop(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nListWidth = infoPtr->rcList.right - infoPtr->rcList.left;
  POINT ptItem;
  RECT rcView;
  INT i;

  if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
  {
    ZeroMemory(&ptItem, sizeof(POINT));
    ZeroMemory(&rcView, sizeof(RECT));

    if (nListWidth > infoPtr->nItemWidth)
    {
      for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
      {
        if (ptItem.x + infoPtr->nItemWidth > nListWidth)
        {
          ptItem.x = 0;
          ptItem.y += infoPtr->nItemHeight;
        }

        LISTVIEW_SetItemPosition(hwnd, i, ptItem.x, ptItem.y,TRUE);
        ptItem.x += infoPtr->nItemWidth;
        rcView.right = max(rcView.right, ptItem.x);
      }

      rcView.bottom = ptItem.y + infoPtr->nItemHeight;
    }
    else
    {
      for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
      {
        LISTVIEW_SetItemPosition(hwnd, i, ptItem.x, ptItem.y,TRUE);
        ptItem.y += infoPtr->nItemHeight;
      }

      rcView.right = infoPtr->nItemWidth;
      rcView.bottom = ptItem.y;
    }

    LISTVIEW_SetViewRect(hwnd, &rcView);
  }
}

/***
 * DESCRIPTION:
 * Aligns the items with the left edge of the window.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_AlignLeft(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nListHeight = infoPtr->rcList.bottom - infoPtr->rcList.top;
  POINT ptItem;
  RECT rcView;
  INT i;

  if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
  {
    ZeroMemory(&ptItem, sizeof(POINT));
    ZeroMemory(&rcView, sizeof(RECT));

    if (nListHeight > infoPtr->nItemHeight)
    {
      for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
      {
        if (ptItem.y + infoPtr->nItemHeight > nListHeight)
        {
          ptItem.y = 0;
          ptItem.x += infoPtr->nItemWidth;
        }

        LISTVIEW_SetItemPosition(hwnd, i, ptItem.x, ptItem.y,TRUE);
        ptItem.y += infoPtr->nItemHeight;
        rcView.bottom = max(rcView.bottom, ptItem.y);
      }

      rcView.right = ptItem.x + infoPtr->nItemWidth;
    }
    else
    {
      for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
      {
        LISTVIEW_SetItemPosition(hwnd, i, ptItem.x, ptItem.y,TRUE);
        ptItem.x += infoPtr->nItemWidth;
      }

      rcView.bottom = infoPtr->nItemHeight;
      rcView.right = ptItem.x;
    }

    LISTVIEW_SetViewRect(hwnd, &rcView);
  }
}

/***
 * DESCRIPTION:
 * Set the bounding rectangle of all the items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] LPRECT : bounding rectangle
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetViewRect(HWND hwnd, LPRECT lprcView)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;

//  TRACE("(hwnd=%x, left=%d, top=%d, right=%d, bottom=%d)\n", hwnd,
//        lprcView->left, lprcView->top, lprcView->right, lprcView->bottom);

  if (lprcView)
  {
    bResult = TRUE;
    infoPtr->rcView.left = lprcView->left;
    infoPtr->rcView.top = lprcView->top;
    infoPtr->rcView.right = lprcView->right;
    infoPtr->rcView.bottom = lprcView->bottom;
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Retrieves the bounding rectangle of all the items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [O] LPRECT : bounding rectangle
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_GetViewRect(HWND hwnd, LPRECT lprcView)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  POINT ptOrigin;

  if (lprcView)
  {
    bResult = LISTVIEW_GetOrigin(hwnd, &ptOrigin);
    if (bResult)
    {
      lprcView->left = infoPtr->rcView.left + ptOrigin.x;
      lprcView->top = infoPtr->rcView.top + ptOrigin.y;
      lprcView->right = infoPtr->rcView.right + ptOrigin.x;
      lprcView->bottom = infoPtr->rcView.bottom + ptOrigin.y;
    }
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Calculates the width of an item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] LONG : window style
 *
 * RETURN:
 * Returns item width.
 */
static INT LISTVIEW_GetItemWidth(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nHeaderItemCount;
  RECT rcHeaderItem;
  INT nItemWidth = 0;
  INT nLabelWidth;
  INT i;

  if (infoPtr->uView == LVS_ICON)
  {
    nItemWidth = infoPtr->iconSpacing.cx;
  }
  else if (infoPtr->uView == LVS_REPORT)
  {
    /* calculate width of header */
    nHeaderItemCount = HEADER_GetItemCount(infoPtr->hwndHeader);
    for (i = 0; i < nHeaderItemCount; i++)
    {
      if (HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)i,(LPARAM)&rcHeaderItem))
      {
        nItemWidth += (rcHeaderItem.right - rcHeaderItem.left);
      }
    }
  }
  else
  {
    for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
    {
      nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,i,0);
      nItemWidth = max(nItemWidth, nLabelWidth);
    }

    /* default label size */
    if (GETITEMCOUNT(infoPtr) == 0)
    {
      nItemWidth = DEFAULT_COLUMN_WIDTH;
    }
    else
    {
      if (nItemWidth == 0)
      {
        nItemWidth = DEFAULT_LABEL_WIDTH;
      }
      else
      {
        /* add padding */
        nItemWidth += WIDTH_PADDING;

        if (infoPtr->himlSmall)
        {
          nItemWidth += infoPtr->iconSize.cx;
        }

        if (infoPtr->himlState)
        {
          nItemWidth += infoPtr->iconSize.cx;
        }
      }
    }
  }

  if(nItemWidth == 0)
  {
      /* nItemWidth Cannot be Zero */
      nItemWidth = 1;
  }

  return nItemWidth;
}

/***
 * DESCRIPTION:
 * Calculates the width of a specific item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] LPSTR : string
 *
 * RETURN:
 * Returns the width of an item width a specified string.
 */
static INT LISTVIEW_CalculateWidth(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nHeaderItemCount;
  RECT rcHeaderItem;
  INT nItemWidth = 0;
  INT i;

  if (infoPtr->uView == LVS_ICON)
  {
    nItemWidth = infoPtr->iconSpacing.cx;
  }
  else if (infoPtr->uView == LVS_REPORT)
  {
    /* calculate width of header */
    nHeaderItemCount = HEADER_GetItemCount(infoPtr->hwndHeader);
    for (i = 0; i < nHeaderItemCount; i++)
    {
      if (HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)i,(LPARAM)&rcHeaderItem) != 0)
      {
        nItemWidth += (rcHeaderItem.right - rcHeaderItem.left);
      }
    }
  }
  else
  {
    /* get width of string */
    nItemWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,0);

    /* default label size */
    if (GETITEMCOUNT(infoPtr) == 0)
    {
      nItemWidth = DEFAULT_COLUMN_WIDTH;
    }
    else
    {
      if (nItemWidth == 0)
      {
        nItemWidth = DEFAULT_LABEL_WIDTH;
      }
      else
      {
        /* add padding */
        nItemWidth += WIDTH_PADDING;

        if (infoPtr->himlSmall != NULL)
        {
          nItemWidth += infoPtr->iconSize.cx;
        }

        if (infoPtr->himlState != NULL)
        {
          nItemWidth += infoPtr->iconSize.cx;
        }
      }
    }
  }

  return nItemWidth;
}

/***
 * DESCRIPTION:
 * Calculates the height of an item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] LONG : window style
 *
 * RETURN:
 * Returns item height.
 */
static INT LISTVIEW_GetItemHeight(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nItemHeight = 0;

  if (infoPtr->uView == LVS_ICON)
  {
    nItemHeight = infoPtr->iconSpacing.cy;
  }
  else
  {
    TEXTMETRICA tm;
    HDC hdc = GetDC(hwnd);
    HFONT hOldFont = SelectObject(hdc, infoPtr->hFont);
    GetTextMetricsA(hdc, &tm);
    nItemHeight = max(tm.tmHeight, infoPtr->iconSize.cy) + HEIGHT_PADDING;
    SelectObject(hdc, hOldFont);
    ReleaseDC(hwnd, hdc);
  }

  return nItemHeight;
}

static BOOL LISTVIEW_IsItemSelected(LISTVIEW_INFO *infoPtr,INT nItem)
{
  LISTVIEW_ITEM *lpItem;
  INT pos = 0;

  while ((lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaSelItems,pos)) != NULL)
  {
    if (DPA_GetPtrIndex(infoPtr->hdpaItems,lpItem) == nItem) return TRUE;
    pos++;
  }

  return FALSE;
}

/***
 * DESCRIPTION:
 * Adds a block of selections.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_AddGroupSelection(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nFirst = min(infoPtr->nSelectionMark, nItem);
  INT nLast = max(infoPtr->nSelectionMark, nItem);
  INT i;

  for (i = nFirst; i <= nLast; i++)
  {
    if (!LISTVIEW_IsItemSelected(infoPtr,nItem))
    {
      LISTVIEW_SetItemState(hwnd,i,LVIS_SELECTED,LVIS_SELECTED);
    }
  }

  LISTVIEW_SetItemFocus(hwnd, nItem);
}

/***
 * DESCRIPTION:
 * Adds a single selection.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_AddSelection(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (!LISTVIEW_IsItemSelected(infoPtr,nItem))
  {
    LISTVIEW_SetItemState(hwnd,nItem,LVIS_SELECTED,LVIS_SELECTED);
  }

  LISTVIEW_SetItemFocus(hwnd, nItem);
}

/***
 * DESCRIPTION:
 * Selects or unselects an item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 *   SELECT: TRUE
 *   UNSELECT : FALSE
 */
static BOOL LISTVIEW_ToggleSelection(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  DWORD state = 0,stateMask = 0;

  if (nItem != infoPtr->nFocusedItem)
  {
    stateMask = LVIS_FOCUSED;
    LISTVIEW_SetItemState(hwnd,infoPtr->nFocusedItem,state,stateMask);
  }

  stateMask = LVIS_SELECTED;
  if (!LISTVIEW_IsItemSelected(infoPtr,nItem))
  {
    bResult = TRUE;
    state = LVIS_SELECTED;
  }
  infoPtr->nSelectionMark = nItem;
  LISTVIEW_SetItemState(hwnd,nItem,state,stateMask);

  if (nItem != infoPtr->nFocusedItem)
  {
    state =  LVIS_FOCUSED;
    stateMask = LVIS_FOCUSED;
    infoPtr->nFocusedItem = nItem;
    LISTVIEW_SetItemState(hwnd,nItem,state,stateMask);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Selects items based on view coorddiantes.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] RECT : selection rectangle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_SetSelectionRect(HWND hwnd, RECT rcSelRect)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  POINT ptItem;
  INT i;

  for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
  {
    LISTVIEW_GetItemPosition(hwnd, i, &ptItem);

    if (PtInRect(&rcSelRect, ptItem) != FALSE)
    {
      LISTVIEW_SetItemState(hwnd,i,LVIS_SELECTED,LVIS_SELECTED);
    }
    else
    {
      LISTVIEW_SetItemState(hwnd,i,0,LVIS_SELECTED);
    }
  }
}

/***
 * DESCRIPTION:
 * Sets a single group selection.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_SetGroupSelection(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if ((infoPtr->uView == LVS_LIST) || (infoPtr->uView == LVS_REPORT))
  {
    INT i;
    INT nFirst = min(infoPtr->nSelectionMark, nItem);
    INT nLast = max(infoPtr->nSelectionMark, nItem);

    for (i = 0; i <= GETITEMCOUNT(infoPtr); i++)
    {
      if ((i < nFirst) || (i > nLast))
      {
        if (LISTVIEW_IsItemSelected(infoPtr,i))
        {
          LISTVIEW_SetItemState(hwnd,i,0,LVIS_SELECTED);
        }
      }
      else
      {
        if (!LISTVIEW_IsItemSelected(infoPtr,i))
        {
          LISTVIEW_SetItemState(hwnd,i,LVIS_SELECTED,LVIS_SELECTED);
        }
      }
    }
    LISTVIEW_SetItemState(hwnd,infoPtr->nFocusedItem,0,LVIS_FOCUSED);

    infoPtr->nFocusedItem = nItem;
    LISTVIEW_SetItemState(hwnd,nItem,LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);

  }
  else
  {
    POINT ptItem;
    POINT ptSelMark;
    RECT rcSel;

    LISTVIEW_GetItemPosition(hwnd, nItem, &ptItem);
    LISTVIEW_GetItemPosition(hwnd, infoPtr->nSelectionMark, &ptSelMark);
    rcSel.left = min(ptSelMark.x, ptItem.x);
    rcSel.top = min(ptSelMark.y, ptItem.y);
    rcSel.right = max(ptSelMark.x, ptItem.x) + infoPtr->nItemWidth;
    rcSel.bottom = max(ptSelMark.y, ptItem.y) + infoPtr->nItemHeight;
    LISTVIEW_SetSelectionRect(hwnd, rcSel);
    LISTVIEW_SetItemFocus(hwnd, nItem);
  }
}

/***
 * DESCRIPTION:
 * Manages the item focus.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 *   TRUE : focused item changed
 *   FALSE : focused item has NOT changed
 */
static BOOL LISTVIEW_SetItemFocus(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;

  if (infoPtr->nFocusedItem != nItem)
  {
    bResult = TRUE;

    LISTVIEW_SetItemState(hwnd,infoPtr->nFocusedItem,0,LVIS_FOCUSED);
    LISTVIEW_SetItemState(hwnd,nItem,LVIS_FOCUSED,LVIS_FOCUSED);

    infoPtr->nFocusedItem = nItem;
    LISTVIEW_EnsureVisible(hwnd, nItem, FALSE);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Sets a single selection.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_SetSelection(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  DWORD mask = LVIS_FOCUSED;

  if (!(infoPtr->dwStyle & LVS_SINGLESEL))
  {
    if (nItem > 0)
      LISTVIEW_RemoveSelections(hwnd,0,nItem);

    if (nItem < GETITEMCOUNT(infoPtr))
      LISTVIEW_RemoveSelections(hwnd,nItem+1,GETITEMCOUNT(infoPtr));
  } else mask |= LVIS_SELECTED;


  if (infoPtr->nFocusedItem != nItem) LISTVIEW_SetItemState(hwnd,infoPtr->nFocusedItem,0,mask);
  LISTVIEW_SetItemState(hwnd,nItem,LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);

  infoPtr->nFocusedItem = nItem;
  infoPtr->nSelectionMark = nItem;
}

/***
 * DESCRIPTION:
 * Set selection(s) with keyboard.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 *   SUCCESS : TRUE (needs to be repainted)
 *   FAILURE : FALSE (nothing has changed)
 */
static VOID LISTVIEW_KeySelection(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  WORD wShift = HIWORD(GetKeyState(VK_SHIFT));
  WORD wCtrl = HIWORD(GetKeyState(VK_CONTROL));

  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    if (infoPtr->dwStyle & LVS_SINGLESEL)
    {
      LISTVIEW_SetSelection(hwnd, nItem);
      LISTVIEW_EnsureVisible(hwnd, nItem, FALSE);
    }
    else
    {
      if (wShift)
      {
        LISTVIEW_SetGroupSelection(hwnd, nItem);
      }
      else if (wCtrl)
      {
        LISTVIEW_SetItemFocus(hwnd, nItem);
      }
      else
      {
        LISTVIEW_SetSelection(hwnd, nItem);
        LISTVIEW_EnsureVisible(hwnd, nItem, FALSE);
      }
    }
  }
}

/***
 * DESCRIPTION:
 * Selects an item based on coordinates.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] POINT : mouse click ccordinates
 *
 * RETURN:
 *   SUCCESS : item index
 *   FAILURE : -1
 */
static LRESULT LISTVIEW_MouseSelection(HWND hwnd, POINT pt)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  RECT rcItem;

  if (infoPtr->uView == LVS_REPORT)
  {
    INT nTop = LISTVIEW_GetTopIndex(hwnd),nItem;

    nItem = nTop+((pt.y-infoPtr->rcList.top)/infoPtr->nItemHeight);
    if (nItem >= GETITEMCOUNT(infoPtr)) return -1;

    if (LISTVIEW_GetItemRect(hwnd,nItem,&rcItem,LVIR_SELECTBOUNDS) && PtInRect(&rcItem,pt))
      return nItem;
  } else if (infoPtr->uView == LVS_LIST)
  {
    INT nTop = LISTVIEW_GetTopIndex(hwnd),nItem;
    INT nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);

    nItem = nTop+(pt.y/infoPtr->nItemHeight)+
            ((INT)(pt.x/infoPtr->nItemWidth))*nCountPerColumn;
    if (nItem >= GETITEMCOUNT(infoPtr)) return -1;

    if (LISTVIEW_GetItemRect(hwnd,nItem,&rcItem,LVIR_SELECTBOUNDS) && PtInRect(&rcItem,pt))
      return nItem;
  } else
  {
    INT i,nFirst = LISTVIEW_GetTopIndex(hwnd),nLast = GETITEMCOUNT(infoPtr);

    for (i = nFirst;i < nLast;i++)
    {
      if (LISTVIEW_GetItemRect(hwnd,i,&rcItem,LVIR_SELECTBOUNDS) && PtInRect(&rcItem,pt))
        return i;
    }
  }

  return -1;
}

/***
 * DESCRIPTION:
 * Removes all selection states.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 *   SUCCCESS : TRUE
 *   FAILURE : FALSE
 */
static VOID LISTVIEW_RemoveSelections(HWND hwnd, INT nFirst, INT nLast)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LISTVIEW_ITEM *lpItem;
  INT pos = 0;

  while ((lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaSelItems,pos)) != NULL)
  {
    INT nItem = DPA_GetPtrIndex(infoPtr->hdpaItems,lpItem);

    if ((nItem >= nFirst) && (nItem < nLast))
      LISTVIEW_SetItemState(hwnd,nItem,0,LVIS_SELECTED);
    else
      pos++;
  }
}

/***
 * DESCRIPTION:
 * Removes a column.
 *
 * PARAMETER(S):
 * [IO] HDPA : dynamic pointer array handle
 * [I] INT : column index (subitem index)
 *
 * RETURN:
 *   SUCCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_RemoveColumn(HDPA hdpaItems, INT nSubItem)
{
  BOOL bResult = TRUE;
  LISTVIEW_ITEM *lpItem;
  INT i;

  for (i = 0; i < hdpaItems->nItemCount; i++)
  {
    lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(hdpaItems,i);
    if (lpItem)
    {
      if (LISTVIEW_RemoveItemData(lpItem->hdpaSubItems,nSubItem) == FALSE)
      {
        bResult = FALSE;
      }
    }
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Removes a subitem at a given position.
 *
 * PARAMETER(S):
 * [IO] HDPA : dynamic pointer array handle
 * [I] INT : subitem index
 *
 * RETURN:
 *   SUCCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_RemoveItemData(HDPA hdpaSubItems,INT nSubItem)
{
  LISTVIEW_ITEMDATA *lpItemData = LISTVIEW_GetItemData(hdpaSubItems,nSubItem);
  INT i;

  if (lpItemData)
  {
    /* free string */
    if (lpItemData->pszText && (lpItemData->pszText != LPSTR_TEXTCALLBACKW))
    {
      COMCTL32_Free(lpItemData->pszText);
    }

    /* free item */
    COMCTL32_Free(lpItemData);

    /* free dpa memory */
    if (DPA_DeletePtr(hdpaSubItems,i) == NULL)
    {
      return FALSE;
    }
  }

  return TRUE;
}

/***
 * DESCRIPTION:
 * Compares the item information.
 *
 * PARAMETER(S):
 * [I] LISTVIEW_ITEM *: destination item
 * [I] LPLVITEM : source item
 *
 * RETURN:
 *   LVIF_* flags
 */
static UINT LISTVIEW_GetItemChanges(LISTVIEW_ITEM *lpItem,LISTVIEW_ITEMDATA *lpItemData,LPLVITEMW lpLVItem,BOOL unicode)
{
  UINT uChanged = 0;

  if (lpItem && lpItemData && lpLVItem)
  {
    if (lpLVItem->mask & LVIF_STATE)
    {
      if ((lpItem->state & lpLVItem->stateMask) !=
          (lpLVItem->state & lpLVItem->stateMask))
      {
        uChanged |= LVIF_STATE;
      }
    }

    if (lpLVItem->mask & LVIF_IMAGE)
    {
      if (lpItemData->iImage != lpLVItem->iImage)
      {
        uChanged |= LVIF_IMAGE;
      }
    }

    if (lpLVItem->mask & LVIF_PARAM)
    {
      if (lpItem->lParam != lpLVItem->lParam)
      {
        uChanged |= LVIF_PARAM;
      }
    }

    if (lpLVItem->mask & LVIF_INDENT)
    {
      if (lpItem->iIndent != lpLVItem->iIndent)
      {
        uChanged |= LVIF_INDENT;
      }
    }

    if (lpLVItem->mask & LVIF_TEXT)
    {
      if (unicode)
      {
        if (lpLVItem->pszText == LPSTR_TEXTCALLBACKW)
        {
          if (lpItemData->pszText != LPSTR_TEXTCALLBACKW)
          {
            uChanged |= LVIF_TEXT;
          }

        }
        else
        {
          if (lpItemData->pszText == LPSTR_TEXTCALLBACKW)
          {
            uChanged |= LVIF_TEXT;
          }
          else
          {
            if (lpLVItem->pszText)
            {
              if (lpItemData->pszText)
              {
                if (lstrcmpW(lpLVItem->pszText,lpItemData->pszText) != 0)
                {
                  uChanged |= LVIF_TEXT;
                }
              }
              else
              {
                uChanged |= LVIF_TEXT;
              }
            }
            else
            {
              if (lpItemData->pszText)
              {
                uChanged |= LVIF_TEXT;
              }
            }
          }
        }
      } else //!unicode
      {
        if ((LPSTR)lpLVItem->pszText == LPSTR_TEXTCALLBACKA)
        {
          if (lpItemData->pszText != LPSTR_TEXTCALLBACKW)
          {
            uChanged |= LVIF_TEXT;
          }

        }
        else
        {
          if (lpItemData->pszText == LPSTR_TEXTCALLBACKW)
          {
            uChanged |= LVIF_TEXT;
          }
          else
          {
            if (lpLVItem->pszText)
            {
              if (lpItemData->pszText)
              {
                if (lstrcmpAtoW((LPSTR)lpLVItem->pszText,lpItemData->pszText) != 0)
                {
                  uChanged |= LVIF_TEXT;
                }
              }
              else
              {
                uChanged |= LVIF_TEXT;
              }
            }
            else
            {
              if (lpItemData->pszText)
              {
                uChanged |= LVIF_TEXT;
              }
            }
          }
        }
      }
    }
  }

  return uChanged;
}

/***
 * DESCRIPTION:
 * Initializes item attributes.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [O] LISTVIEW_ITEM *: destination item
 * [I] LPLVITEM : source item
 *
 * RETURN:
 *   SUCCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_InitItem(HWND hwnd,LISTVIEW_ITEM *lpItem,LPLVITEMW lpLVItem,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;

  if (lpItem && lpLVItem)
  {
    bResult = TRUE;

    if (lpLVItem->mask & LVIF_STATE)
    {
      lpItem->state &= ~lpLVItem->stateMask;
      lpItem->state |= (lpLVItem->state & lpLVItem->stateMask);
    }

    if (lpLVItem->mask & LVIF_PARAM)
    {
      lpItem->lParam = lpLVItem->lParam;
    }

    if (lpLVItem->mask & LVIF_INDENT)
    {
      lpItem->iIndent = lpLVItem->iIndent;
    }
  }

  return bResult;
}

static BOOL LISTVIEW_InitItemData(HWND hwnd,LISTVIEW_ITEMDATA *lpItemData,LPLVITEMW lpLVItem,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;

  if (lpItemData && lpLVItem)
  {
    bResult = TRUE;

    lpItemData->iSubItem = lpLVItem->iSubItem;

    if (lpLVItem->mask & LVIF_IMAGE)
    {
      lpItemData->iImage = lpLVItem->iImage;
    }

    if (lpLVItem->mask & LVIF_TEXT)
    {
      if (unicode)
      {
        if (lpLVItem->pszText == LPSTR_TEXTCALLBACKW)
        {
          if (!lpItemData->iSubItem && ((infoPtr->dwStyle & LVS_SORTASCENDING) || (infoPtr->dwStyle & LVS_SORTDESCENDING)))
          {
            return FALSE;
          }

          if (lpItemData->pszText && (lpItemData->pszText != LPSTR_TEXTCALLBACKW))
          {
            COMCTL32_Free(lpItemData->pszText);
          }

          lpItemData->pszText = LPSTR_TEXTCALLBACKW;
        } else
        {
          if (lpItemData->pszText == LPSTR_TEXTCALLBACKW)
          {
            lpItemData->pszText = NULL;
          }

          bResult = Str_SetPtrW(&lpItemData->pszText,lpLVItem->pszText);
        }
      } else //!unicode
      {
        if ((LPSTR)lpLVItem->pszText == LPSTR_TEXTCALLBACKA)
        {
          if (!lpItemData->iSubItem && ((infoPtr->dwStyle & LVS_SORTASCENDING) || (infoPtr->dwStyle & LVS_SORTDESCENDING)))
          {
            return FALSE;
          }

          if (lpItemData->pszText && (lpItemData->pszText != LPSTR_TEXTCALLBACKW))
          {
            COMCTL32_Free(lpItemData->pszText);
          }

          lpItemData->pszText = LPSTR_TEXTCALLBACKW;
        } else
        {
          INT len;

          if (lpItemData->pszText != LPSTR_TEXTCALLBACKW) COMCTL32_Free(lpItemData->pszText);
          len = lpLVItem->pszText ? lstrlenA((LPSTR)lpLVItem->pszText):0;

          if (len > 0)
          {
            len++;
            lpItemData->pszText = (LPWSTR)COMCTL32_Alloc(len*sizeof(WCHAR));
            lstrcpyAtoW(lpItemData->pszText,(LPSTR)lpLVItem->pszText);
          } else lpItemData->pszText = NULL;

          bResult = TRUE;
        }
      }
    }
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Finds the dpa insert position (array index).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : subitem index
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static INT LISTVIEW_FindInsertPosition(HDPA hdpaSubItems,INT nSubItem)
{
  LISTVIEW_ITEMDATA *lpItemData;
  INT i;

  for (i = 1; i < hdpaSubItems->nItemCount; i++)
  {
    lpItemData = (LISTVIEW_ITEMDATA*)DPA_GetPtr(hdpaSubItems,i);
    if (lpItemData)
    {
      if (lpItemData->iSubItem > nSubItem)
      {
        return i;
      }
    }
  }

  return hdpaSubItems->nItemCount;
}

/***
 * DESCRIPTION:
 * Retrieves a listview subitem at a given position (column index).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : subitem index
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LISTVIEW_ITEMDATA* LISTVIEW_GetItemData(HDPA hdpaSubItems,INT nSubItem)
{
  LISTVIEW_ITEMDATA *lpItemData;
  INT i;

  for (i = 0;i < hdpaSubItems->nItemCount;i++)
  {
    lpItemData = (LISTVIEW_ITEMDATA*)DPA_GetPtr(hdpaSubItems,i);
    if (lpItemData)
    {
      if (lpItemData->iSubItem == nSubItem)
      {
        return lpItemData;
      } else if (lpItemData->iSubItem > nSubItem)
      {
        return NULL;
      }
    }
  }

  return NULL;
}

/***
 * DESCRIPTION:
 * Retrieves the index of the item at coordinate (0, 0) of the client area.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * item index
 */
static INT LISTVIEW_GetTopIndex(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nItem = 0;

  if (infoPtr->uView == LVS_LIST)
  {
    if (infoPtr->dwStyle & WS_HSCROLL)
      nItem = infoPtr->lefttop.x*LISTVIEW_GetCountPerColumn(hwnd);
  } else if (infoPtr->uView == LVS_REPORT)
  {
    if (infoPtr->dwStyle & WS_VSCROLL)
      nItem = infoPtr->lefttop.y;
  }

  return nItem;
}

/***
 * DESCRIPTION:
 * Draws a subitem.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 * [I] INT : item index
 * [I] INT : subitem index
 * [I] RECT * : clipping rectangle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_DrawSubItem(HWND hwnd,HDC hdc,INT nItem,INT nSubItem,RECT rcItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LVINTERNALITEMW lvItem;

  /* get information needed for drawing the item */
  ZeroMemory(&lvItem, sizeof(LVITEMW));
  lvItem.header.mask = LVIF_TEXT | LVIF_IMAGE;
  lvItem.header.iItem = nItem;
  lvItem.header.iSubItem = nSubItem;
  lvItem.header.cchTextMax = DISP_TEXT_SIZE;
  lvItem.header.pszText = NULL;
  lvItem.mustFree = FALSE;
  LISTVIEW_GetItem(hwnd,(LPLVITEMW)&lvItem,TRUE,TRUE);

  if (infoPtr->dwExStyle & LVS_EX_SUBITEMIMAGES)
  {
    /* small icons */
    if (infoPtr->himlSmall)
    {
      ImageList_SetBkColor(infoPtr->himlSmall,CLR_NONE);
      ImageList_Draw(infoPtr->himlSmall,lvItem.header.iImage,hdc,rcItem.left,rcItem.top,ILD_NORMAL);

      rcItem.left += infoPtr->iconSize.cx;
    }
  }

  /* set item colors */
  SetBkColor(hdc, infoPtr->clrTextBk);
  SetTextColor(hdc, infoPtr->clrText);

  if (lvItem.unicode)
    ExtTextOutW(hdc,rcItem.left,rcItem.top,ETO_OPAQUE | ETO_CLIPPED,&rcItem,lvItem.header.pszText,lstrlenW(lvItem.header.pszText),NULL);
  else
    ExtTextOutA(hdc,rcItem.left,rcItem.top,ETO_OPAQUE | ETO_CLIPPED,&rcItem,(LPSTR)lvItem.header.pszText,lstrlenA((LPSTR)lvItem.header.pszText),NULL);

  if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
}

static VOID LISTVIEW_DrawHottrackLine(HDC hdc,RECT *rect)
{
  HPEN hPen,hOldPen;
  INT rop;

  rop = SetROP2(hdc,R2_XORPEN);
  hPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
  hOldPen = SelectObject(hdc,hPen);

  MoveToEx(hdc,rect->left,rect->bottom-1,NULL);
  LineTo(hdc,rect->right,rect->bottom-1);

  DeleteObject(hPen);
  SelectObject(hdc,hOldPen);
  SetROP2(hdc,rop);
}

/***
 * DESCRIPTION:
 * Draws an item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 * [I] INT : item index
 * [I] RECT * : clipping rectangle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_DrawItem(HWND hwnd,HDC hdc,INT nItem,RECT rcItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nLabelWidth;
  LVINTERNALITEMW lvItem;
  INT nMixMode;
  DWORD dwBkColor;
  DWORD dwTextColor;

  /* get information needed for drawing the item */
  ZeroMemory(&lvItem, sizeof(LVITEMW));
  lvItem.header.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT;
  lvItem.header.stateMask = LVIS_SELECTED | LVIS_FOCUSED | LVIS_STATEIMAGEMASK;
  lvItem.header.iItem = nItem;
  lvItem.header.iSubItem = 0;
  lvItem.header.cchTextMax = DISP_TEXT_SIZE;
  lvItem.header.pszText = NULL;
  lvItem.mustFree = FALSE;
  LISTVIEW_GetItem(hwnd,(LPLVITEMW)&lvItem,TRUE,TRUE);

  /* state icons */
  if (infoPtr->himlState)
  {
     UINT uStateImage = (lvItem.header.state & LVIS_STATEIMAGEMASK) >> 12;

     if (uStateImage)
       ImageList_Draw(infoPtr->himlState,uStateImage-1,hdc,rcItem.left,rcItem.top,ILD_NORMAL);

     rcItem.left += infoPtr->iconSize.cx;
  }

  /* small icons */
  if (infoPtr->himlSmall)
  {
    ImageList_SetBkColor(infoPtr->himlSmall, CLR_NONE);
    ImageList_Draw(infoPtr->himlSmall,lvItem.header.iImage,hdc,rcItem.left,rcItem.top,((lvItem.header.state & LVIS_SELECTED) && !infoPtr->bFocus) ? ILD_SELECTED:ILD_NORMAL);

    rcItem.left += infoPtr->iconSize.cx;
  }

  /* Don't bother painting item being edited */
  if (infoPtr->hwndEdit && (lvItem.header.state & LVIS_FOCUSED))
  {
    if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
    return;
  }

  if ((lvItem.header.state & LVIS_SELECTED) && infoPtr->bFocus)
  {
    /* set item colors */
    dwBkColor = SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
    dwTextColor = SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
    /* set raster mode */
    nMixMode = SetROP2(hdc, R2_XORPEN);
  }
  else if ((infoPtr->dwStyle & LVS_SHOWSELALWAYS) &&
           (lvItem.header.state & LVIS_SELECTED) && !infoPtr->bFocus)
  {
    dwBkColor = SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
    dwTextColor = SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
    /* set raster mode */
    nMixMode = SetROP2(hdc, R2_COPYPEN);
  }
  else
  {
    /* set item colors */
    dwBkColor = SetBkColor(hdc, infoPtr->clrTextBk);
    dwTextColor = SetTextColor(hdc, infoPtr->clrText);
    /* set raster mode */
    nMixMode = SetROP2(hdc, R2_COPYPEN);
  }

  nLabelWidth = LISTVIEW_GetStringWidth(hwnd,hdc,lvItem.header.pszText,lvItem.unicode);
  if (rcItem.left + nLabelWidth < rcItem.right)
    rcItem.right = rcItem.left + nLabelWidth;

  /* draw label */
  if (lvItem.unicode)
    ExtTextOutW(hdc,rcItem.left,rcItem.top,ETO_OPAQUE | ETO_CLIPPED,&rcItem,lvItem.header.pszText,lstrlenW(lvItem.header.pszText),NULL);
  else
    ExtTextOutA(hdc,rcItem.left,rcItem.top,ETO_OPAQUE | ETO_CLIPPED,&rcItem,(LPSTR)lvItem.header.pszText,lstrlenA((LPSTR)lvItem.header.pszText),NULL);

  if ((lvItem.header.state & LVIS_FOCUSED) && infoPtr->bFocus)
    Rectangle(hdc, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);

  if (nMixMode)
  {
    SetROP2(hdc, R2_COPYPEN);
    SetBkColor(hdc, infoPtr->clrTextBk);
    SetTextColor(hdc, infoPtr->clrText);
  }
  if (nItem == infoPtr->nHotItem)
    LISTVIEW_DrawHottrackLine(hdc,&rcItem);

  if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
}

/***
 * DESCRIPTION:
 * Draws an item when in large icon display mode.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 * [I] LISTVIEW_ITEM * : item
 * [I] INT : item index
 * [I] RECT * : clipping rectangle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_DrawLargeItem(HWND hwnd, HDC hdc, INT nItem, RECT rcItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nDrawPosX = rcItem.left;
  INT nLabelWidth;
  TEXTMETRICA tm;
  LVINTERNALITEMW lvItem;

  /* get information needed for drawing the item */
  ZeroMemory(&lvItem, sizeof(LVITEMW));
  lvItem.header.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
  lvItem.header.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
  lvItem.header.iItem = nItem;
  lvItem.header.iSubItem = 0;
  lvItem.header.cchTextMax = DISP_TEXT_SIZE;
  lvItem.header.pszText = NULL;
  lvItem.mustFree = FALSE;
  LISTVIEW_GetItem(hwnd,(LPLVITEMW)&lvItem,TRUE,TRUE);

  if (lvItem.header.state & LVIS_SELECTED)
  {
    /* set item colors */
    SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
    SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
    /* set raster mode */
    SetROP2(hdc, R2_XORPEN);
  } else
  {
    /* set item colors */
    SetBkColor(hdc, infoPtr->clrTextBk);
    SetTextColor(hdc, infoPtr->clrText);
    /* set raster mode */
    SetROP2(hdc, R2_COPYPEN);
  }

  if (infoPtr->himlNormal)
  {
    rcItem.top += ICON_TOP_PADDING;
    nDrawPosX += (infoPtr->iconSpacing.cx - infoPtr->iconSize.cx) / 2;
    ImageList_Draw(infoPtr->himlNormal,lvItem.header.iImage,hdc,nDrawPosX,rcItem.top,(lvItem.header.state & LVIS_SELECTED) ? ILD_SELECTED:ILD_NORMAL);
  }

  /* Don't bother painting item being edited */
  if (infoPtr->hwndEdit && (lvItem.header.state & LVIS_FOCUSED))
  {
    if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
    return;
  }

  rcItem.top += infoPtr->iconSize.cy + ICON_BOTTOM_PADDING;
  nLabelWidth = LISTVIEW_GetStringWidth(hwnd,hdc,lvItem.header.pszText,lvItem.unicode);
  nDrawPosX = infoPtr->iconSpacing.cx - nLabelWidth;
  if (nDrawPosX > 1)
  {
    rcItem.left += nDrawPosX / 2;
    rcItem.right = rcItem.left + nLabelWidth;
  } else
  {
    rcItem.left += 1;
    rcItem.right = rcItem.left + infoPtr->iconSpacing.cx - 1;
  }

  /* draw label */
  GetTextMetricsA(hdc,&tm);
  rcItem.bottom = rcItem.top + tm.tmHeight + HEIGHT_PADDING;
  if (lvItem.unicode)
    ExtTextOutW(hdc,rcItem.left,rcItem.top,ETO_OPAQUE | ETO_CLIPPED,&rcItem,lvItem.header.pszText,lstrlenW(lvItem.header.pszText),NULL);
  else
    ExtTextOutA(hdc,rcItem.left,rcItem.top,ETO_OPAQUE | ETO_CLIPPED,&rcItem,(LPSTR)lvItem.header.pszText,lstrlenA((LPSTR)lvItem.header.pszText),NULL);

  if (lvItem.header.state & LVIS_FOCUSED)
    Rectangle(hdc, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
  if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
}

/***
 * DESCRIPTION:
 * Draws listview items when in report display mode.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_DrawReport(HWND hwnd,HDC hdc,RECT *updateRect)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nDrawPosY = infoPtr->rcList.top;
  INT nColumnCount = HEADER_GetItemCount(infoPtr->hwndHeader);
  RECT rcItem,*rcHeader;
  INT  j,nItem,nLast,xOffset = infoPtr->lefttop.x*infoPtr->scrollStep.x;

  nItem = LISTVIEW_GetTopIndex(hwnd);

  /* add 1 for displaying a partial item at the bottom */
  nLast = nItem+LISTVIEW_GetCountPerColumn(hwnd)+1;
  nLast = min(nLast,GETITEMCOUNT(infoPtr));

  /* send cache hint notification */
  if (infoPtr->dwStyle & LVS_OWNERDATA)
  {
    NMLVCACHEHINT nmlv;

    nmlv.iFrom = nItem;
    nmlv.iTo   = nLast;

    sendNotify(hwnd,LVN_ODCACHEHINT,&nmlv.hdr);
  }

  //get header rects
  rcHeader = (LPRECT)COMCTL32_Alloc(nColumnCount*sizeof(RECT));
  for (j = 0;j < nColumnCount;j++)
  {
    HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)j,(LPARAM)&rcHeader[j]);
    rcHeader[j].left += REPORT_MARGINX;
    rcHeader[j].right = max(rcHeader[j].left,rcHeader[j].right-REPORT_MARGINX);

    /* Offset the Scroll Bar Pos */
    if (infoPtr->dwStyle & WS_HSCROLL)
    {
      rcHeader[j].left -= xOffset;
      rcHeader[j].right -= xOffset;
    }
  }

  for (;nItem < nLast;nItem++)
  {
    for (j = 0;j < nColumnCount;j++)
    {
      rcItem = rcHeader[j];
      rcItem.top = nDrawPosY;
      rcItem.bottom = rcItem.top + infoPtr->nItemHeight;

      if (rcItem.top >= (updateRect ? updateRect->bottom:infoPtr->rcList.bottom)) goto DR_END;

      if (!updateRect || IntersectRect(NULL,&rcItem,updateRect))
      {
        if (j == 0)
          LISTVIEW_DrawItem(hwnd,hdc,nItem,rcItem);
        else
          LISTVIEW_DrawSubItem(hwnd,hdc,nItem,j,rcItem);
      }
    }

    nDrawPosY += infoPtr->nItemHeight;
  }

DR_END:
  if (infoPtr->dwExStyle & LVS_EX_GRIDLINES)
  {
    HPEN hOldPen;

    hOldPen = SelectObject(hdc,GetStockObject(BLACK_BRUSH));
    for (j = 0;j < nColumnCount-1;j++)
    {
      MoveToEx(hdc,rcHeader[j].right+REPORT_MARGINX-1,infoPtr->rcList.top,NULL);
      LineTo(hdc,rcHeader[j].right+REPORT_MARGINX-1,infoPtr->rcList.bottom);
    }
    SelectObject(hdc,hOldPen);
  }


  COMCTL32_Free(rcHeader);
}

/***
 * DESCRIPTION:
 * Retrieves the number of items that can fit vertically in the client area.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Number of items per row.
 */
static INT LISTVIEW_GetCountPerRow(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nListWidth = infoPtr->rcList.right - infoPtr->rcList.left;
  INT nCountPerRow = 1;

  if (nListWidth > 0)
  {
    if (infoPtr->uView == LVS_REPORT)
    {
      nCountPerRow = 1;
    }
    else
    {
      nCountPerRow =  nListWidth / infoPtr->nItemWidth;
      if (nCountPerRow == 0)
      {
        nCountPerRow = 1;
      }
    }
  }

  return nCountPerRow;
}

/***
 * DESCRIPTION:
 * Retrieves the number of items that can fit horizontally in the client
 * area.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Number of items per column.
 */
static INT LISTVIEW_GetCountPerColumn(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nListHeight = infoPtr->rcList.bottom - infoPtr->rcList.top;
  INT nCountPerColumn = 1;

  if (nListHeight > 0)
  {
    nCountPerColumn =  nListHeight / infoPtr->nItemHeight;
    if (nCountPerColumn == 0)
      nCountPerColumn = 1;
  }

  return nCountPerColumn;
}

/***
 * DESCRIPTION:
 * Retrieves the number of columns needed to display all the items when in
 * list display mode.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Number of columns.
 */
static INT LISTVIEW_GetColumnCount(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nColumnCount = 0;

  if (infoPtr->uView == LVS_LIST)
  {
    if (infoPtr->rcList.right % infoPtr->nItemWidth == 0)
    {
      nColumnCount = infoPtr->rcList.right / infoPtr->nItemWidth;
    }
    else
    {
      nColumnCount = infoPtr->rcList.right / infoPtr->nItemWidth + 1;
    }
  }

  return nColumnCount;
}


/***
 * DESCRIPTION:
 * Draws listview items when in list display mode.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_DrawList(HWND hwnd,HDC hdc,RECT *updateRect)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  RECT rcItem;
  INT i, j;
  INT nItem;
  INT nColumnCount;
  INT nCountPerColumn;
  INT nItemWidth = infoPtr->nItemWidth;
  INT nItemHeight = infoPtr->nItemHeight;

  /* get number of fully visible columns */
  nColumnCount = LISTVIEW_GetColumnCount(hwnd);
  nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);
  nItem = LISTVIEW_GetTopIndex(hwnd);

  for (i = 0; i < nColumnCount; i++)
  {
    for (j = 0; j < nCountPerColumn; j++, nItem++)
    {
      if (nItem >= GETITEMCOUNT(infoPtr))
        return;

      rcItem.top = j * nItemHeight;
      rcItem.left = i * nItemWidth;
      rcItem.bottom = rcItem.top + nItemHeight;
      rcItem.right = rcItem.left + nItemWidth;

      if (updateRect && (rcItem.left >= updateRect->right)) return;

      if (!updateRect || IntersectRect(NULL,&rcItem,updateRect))
        LISTVIEW_DrawItem(hwnd, hdc, nItem, rcItem);
    }
  }
}

/***
 * DESCRIPTION:
 * Draws listview items when in icon or small icon display mode.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 *
 * RETURN:
 * None
 */
static VOID LISTVIEW_DrawIcon(HWND hwnd,HDC hdc,BOOL bSmall,RECT *updateRect)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  POINT ptPosition;
  POINT ptOrigin;
  RECT rcItem;
  INT i;

  LISTVIEW_GetOrigin(hwnd, &ptOrigin);
  for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
  {
    LISTVIEW_GetItemPosition(hwnd, i, &ptPosition);
    ptPosition.x += ptOrigin.x;
    ptPosition.y += ptOrigin.y;

    if (ptPosition.y + infoPtr->nItemHeight > infoPtr->rcList.top)
    {
      if (ptPosition.x + infoPtr->nItemWidth > infoPtr->rcList.left)
      {
        if (ptPosition.y < infoPtr->rcList.bottom)
        {
          if (ptPosition.x < infoPtr->rcList.right)
          {
            rcItem.top = ptPosition.y;
            rcItem.left = ptPosition.x;
            rcItem.bottom = rcItem.top + infoPtr->nItemHeight;
            rcItem.right = rcItem.left + infoPtr->nItemWidth;
            if (!updateRect || IntersectRect(NULL,&rcItem,updateRect))
            {
              if (!bSmall)
                LISTVIEW_DrawLargeItem(hwnd, hdc, i, rcItem);
              else
                LISTVIEW_DrawItem(hwnd, hdc, i, rcItem);
            }
          }
        }
      }
    }
  }
}

/***
 * DESCRIPTION:
 * Draws listview items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 *
 * RETURN:
 * NoneX
 */
static VOID LISTVIEW_Draw(HWND hwnd,HDC hdc,RECT *updateRect)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  HFONT hOldFont;
  HPEN hPen, hOldPen;

  /* select font */
  hOldFont = SelectObject(hdc, infoPtr->hFont);

  /* select the doted pen (for drawing the focus box) */
  hPen = CreatePen(PS_DOT, 1, 0);
  hOldPen = SelectObject(hdc, hPen);

  /* select transparent brush (for drawing the focus box) */
  SelectObject(hdc, GetStockObject(NULL_BRUSH));

  switch (infoPtr->uView)
  {
    case LVS_LIST:
      LISTVIEW_DrawList(hwnd,hdc,updateRect);
      break;

    case LVS_REPORT:
      LISTVIEW_DrawReport(hwnd,hdc,updateRect);
      break;

    case LVS_SMALLICON:
      LISTVIEW_DrawIcon(hwnd,hdc,TRUE,updateRect);
      break;

    case LVS_ICON:
      LISTVIEW_DrawIcon(hwnd,hdc,FALSE,updateRect);
      break;
  }

  /* unselect objects */
  SelectObject(hdc, hOldFont);
  SelectObject(hdc, hOldPen);

  /* delete pen */
  DeleteObject(hPen);
}


/***
 * DESCRIPTION:
 * Calculates the approximate width and height of a given number of items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : number of items
 * [I] INT : width
 * [I] INT : height
 *
 * RETURN:
 * Returns a DWORD. The width in the low word and the height in high word.
 */
static LRESULT LISTVIEW_ApproximateViewRect(HWND hwnd,INT nItemCount,WORD wWidth,WORD wHeight)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nItemCountPerColumn = 1;
  INT nColumnCount = 0;
  DWORD dwViewRect = 0;

  if (nItemCount == -1)
  {
    nItemCount = GETITEMCOUNT(infoPtr);
  }

  if (infoPtr->uView == LVS_LIST)
  {
    if (wHeight == 0xFFFF)
    {
      /* use current height */
      wHeight = infoPtr->rcList.bottom - infoPtr->rcList.top;
    }

    if (wHeight < infoPtr->nItemHeight)
    {
      wHeight = infoPtr->nItemHeight;
    }

    if (nItemCount > 0)
    {
      if (infoPtr->nItemHeight > 0)
      {
        nItemCountPerColumn = wHeight / infoPtr->nItemHeight;
        if (nItemCountPerColumn == 0)
        {
          nItemCountPerColumn = 1;
        }

        if (nItemCount % nItemCountPerColumn != 0)
        {
          nColumnCount = nItemCount / nItemCountPerColumn;
        }
        else
        {
          nColumnCount = nItemCount / nItemCountPerColumn + 1;
        }
      }
    }

    /* Microsoft padding magic */
    wHeight = nItemCountPerColumn * infoPtr->nItemHeight + 2;
    wWidth = nColumnCount * infoPtr->nItemWidth + 2;

    dwViewRect = MAKELONG(wWidth, wHeight);
  } else if (infoPtr->uView == LVS_REPORT)
  {
    wHeight = infoPtr->nItemHeight;
    wWidth = max(infoPtr->nItemWidth,100);

    dwViewRect = MAKELONG(wWidth,infoPtr->rcList.top+nItemCount*wHeight);
  } else if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
  {
    INT lines,itemW,itemH;

    if (wWidth == 0xFFFF)
      wWidth = infoPtr->rcList.right-infoPtr->rcList.left;

    itemH = infoPtr->nItemHeight;
    itemW = max(infoPtr->nItemWidth,32);

    nColumnCount = wWidth/itemW;
    lines =  nItemCount/nColumnCount;
    if (nItemCount%nColumnCount) lines++;

    dwViewRect = MAKELONG(nColumnCount*itemW,lines*itemH);
  }

  return dwViewRect;
}

static VOID LISTVIEW_SnapToGrid(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
  {
    INT gridW = infoPtr->nItemWidth,gridH = infoPtr->nItemHeight,i;

    for (i = 0; i < GETITEMCOUNT(infoPtr);i++)
    {
      POINT point;

      LISTVIEW_GetItemPosition(hwnd,i,&point);
      if ((point.x % gridW) || (point.y % gridH))
      {
        LISTVIEW_SetItemPosition(hwnd,i,point.x-point.x%gridW,point.y-point.y%gridH,TRUE);
      }
    }
  }
}

/***
 * DESCRIPTION:
 * Arranges listview items in icon display mode.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : alignment code
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_Arrange(HWND hwnd, INT nAlignCode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  if ((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON))
  {
    if (nAlignCode == LVA_DEFAULT)
    {
      switch (infoPtr->dwStyle & LVS_ALIGNMASK)
      {
        case LVS_ALIGNLEFT:
          nAlignCode = LVA_ALIGNLEFT;
          break;

        case LVS_ALIGNTOP:
          nAlignCode = LVA_ALIGNLEFT;
          break;

        default:
          return FALSE;
      }
    }

    switch (nAlignCode)
    {
      case LVA_ALIGNLEFT:
        LISTVIEW_AlignLeft(hwnd);
        break;

      case LVA_ALIGNTOP:
        LISTVIEW_AlignTop(hwnd);
        break;

      case LVA_SNAPTOGRID:
        LISTVIEW_SnapToGrid(hwnd);
        break;

      default:
        return FALSE;
    }
  }

  return TRUE;
}

/***
 * DESCRIPTION:
 * Removes all listview items and subitems.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_DeleteAllItems(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  NMLISTVIEW nmlv;
  BOOL bSuppress;
  BOOL bResult = FALSE;
  INT i;
  INT j;

  if (GETITEMCOUNT(infoPtr) > 0)
  {
    /* initialize memory */
    ZeroMemory(&nmlv, sizeof(NMLISTVIEW));

    /* send LVN_DELETEALLITEMS notification */
    nmlv.iItem = -1;

    /* verify if subsequent LVN_DELETEITEM notifications should be
       suppressed */
    bSuppress = sendNotify(hwnd,LVN_DELETEALLITEMS,&nmlv.hdr);

    for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
    {
      LISTVIEW_ITEM *lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,i);

      if (lpItem)
      {
        for (j = 0;j < lpItem->hdpaSubItems->nItemCount;j++)
        {
          LISTVIEW_ITEMDATA *lpItemData = (LISTVIEW_ITEMDATA*)DPA_GetPtr(lpItem->hdpaSubItems,j);
          if (lpItemData)
          {
            /* free item string */
            if (lpItemData->pszText && (lpItemData->pszText != LPSTR_TEXTCALLBACKW))
            {
              COMCTL32_Free(lpItemData->pszText);
            }

            /* free subitem */
            COMCTL32_Free(lpItemData);
          }
        }

        if (bSuppress == FALSE)
        {
          /* send LVN_DELETEITEM notification */
          nmlv.iItem = i;
          nmlv.lParam = lpItem->lParam;
          sendNotify(hwnd,LVN_DELETEITEM,&nmlv.hdr);
        }

        /* free item */
        DPA_Destroy(lpItem->hdpaSubItems);
        COMCTL32_Free(lpItem);
      }
    }

    /* reinitialize listview memory */
    bResult = DPA_DeleteAllPtrs(infoPtr->hdpaItems);
    DPA_DeleteAllPtrs(infoPtr->hdpaSelItems);

    /* align items (set position of each item) */
    if ((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON))
    {
      if (infoPtr->dwStyle & LVS_ALIGNLEFT)
      {
        LISTVIEW_AlignLeft(hwnd);
      }
      else
      {
        LISTVIEW_AlignTop(hwnd);
      }
    }

    LISTVIEW_UpdateScroll(hwnd);

    /* invalidate client area (optimization needed) */
    LISTVIEW_Refresh(hwnd);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Removes a column from the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : column index
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_DeleteColumn(HWND hwnd, INT nColumn)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;

  if (HEADER_DeleteItem(infoPtr->hwndHeader, nColumn))
  {
    bResult = LISTVIEW_RemoveColumn(infoPtr->hdpaItems, nColumn);

    /* Need to reset the item width when deleting a column */
    infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);

    /* reset scroll parameters */
    if (infoPtr->uView == LVS_REPORT)
    {
      /* update scrollbar(s) */
      LISTVIEW_UpdateScroll(hwnd);

      /* refresh client area */
      LISTVIEW_Refresh(hwnd);
    }
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Removes an item from the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_DeleteItem(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  NMLISTVIEW nmlv;
  BOOL bResult = FALSE;
  LISTVIEW_ITEM *lpItem;
  INT i;

  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    /* initialize memory */
    ZeroMemory(&nmlv, sizeof(NMLISTVIEW));

    lpItem = (LISTVIEW_ITEM*)DPA_DeletePtr(infoPtr->hdpaItems,nItem);
    if (lpItem)
    {
      for (i = 0;i < lpItem->hdpaSubItems->nItemCount;i++)
      {
        LISTVIEW_ITEMDATA *lpItemData = (LISTVIEW_ITEMDATA*)DPA_GetPtr(lpItem->hdpaSubItems,i);

        if (lpItemData)
        {
          /* free item string */
          if (lpItemData->pszText && (lpItemData->pszText != LPSTR_TEXTCALLBACKW))
          {
            COMCTL32_Free(lpItemData->pszText);
          }

          /* free item */
          COMCTL32_Free(lpItemData);
        }
      }

      //remove from selection list
      DPA_DeletePtr(infoPtr->hdpaSelItems,DPA_GetPtrIndex(infoPtr->hdpaSelItems,lpItem));

      /* send LVN_DELETEITEM notification */
      nmlv.iItem = nItem;
      nmlv.lParam = lpItem->lParam;
      sendNotify(hwnd,LVN_DELETEITEM,&nmlv.hdr);

      /* free item */
      bResult = DPA_Destroy(lpItem->hdpaSubItems);
      COMCTL32_Free(lpItem);
    }

    /* align items (set position of each item) */
    if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
    {
      if (infoPtr->dwStyle & LVS_ALIGNLEFT)
      {
        LISTVIEW_AlignLeft(hwnd);
      }
      else
      {
        LISTVIEW_AlignTop(hwnd);
      }
    }

    /* If this item had focus change focus to next or previous item */
    if (GETITEMCOUNT(infoPtr) > 0)
    {
       int sItem = nItem < GETITEMCOUNT(infoPtr) ? nItem : nItem - 1;
       if (infoPtr->nFocusedItem == nItem)
           LISTVIEW_SetItemFocus(hwnd, sItem);
    }
    else
          infoPtr->nFocusedItem = -1;

    /* refresh client area */
    LISTVIEW_UpdateScroll(hwnd);
    LISTVIEW_Refresh(hwnd);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Return edit control handle of current edit label
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 *   SUCCESS : HWND
 *   FAILURE : 0
 */
static LRESULT LISTVIEW_GetEditControl(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->hwndEdit;
}


/***
 * DESCRIPTION:
 * Callback implementation for editlabel control
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] LPSTR : modified text
 * [I] DWORD : item index
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */

static BOOL LISTVIEW_EndEditLabel(HWND hwnd,LPSTR pszText,DWORD nItem,BOOL cancel)
{
  NMLVDISPINFOW dispInfo;
  LISTVIEW_ITEM *lpItem;
  LISTVIEW_ITEMDATA *lpItemData;
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bUpdateItemText;
  WCHAR* textW = NULL;

  ZeroMemory(&dispInfo, sizeof(NMLVDISPINFOW));

  if (!(lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,nItem)))
    return FALSE;

  if (!(lpItemData = LISTVIEW_GetItemData(lpItem->hdpaSubItems,0)))
    return FALSE;

  dispInfo.item.mask = 0;
  dispInfo.item.iItem = nItem;
  dispInfo.item.state = lpItem->state;
  dispInfo.item.stateMask = 0;
  if (cancel)
  {
    dispInfo.item.pszText = NULL;
    dispInfo.item.cchTextMax = 0;
  } else
  {
    if (isUnicodeNotify(&infoPtr->header))
    {
      textW = HEAP_strdupAtoW(GetProcessHeap(),0,pszText);

      dispInfo.item.pszText = textW;
      dispInfo.item.cchTextMax = textW ? lstrlenW(textW):0;
    } else
    {
      dispInfo.item.pszText = (LPWSTR)pszText;
      dispInfo.item.cchTextMax = pszText ? lstrlenA(pszText):0;
    }
  }
  dispInfo.item.iImage = lpItemData->iImage;
  dispInfo.item.lParam = lpItem->lParam;
  infoPtr->hwndEdit = 0;

  bUpdateItemText = sendNotify(hwnd,isUnicodeNotify(&infoPtr->header) ? LVN_ENDLABELEDITW:LVN_ENDLABELEDITA,&dispInfo.hdr);

  /* Do we need to update the Item Text */
  if (bUpdateItemText)
  {
    LVINTERNALITEMW lvItem;

    //get old text
    ZeroMemory(&lvItem, sizeof(LVITEMW));
    lvItem.header.mask = LVIF_TEXT;
    lvItem.header.stateMask = LVIS_SELECTED | LVIS_FOCUSED | LVIS_STATEIMAGEMASK;
    lvItem.header.iItem = nItem;
    lvItem.header.iSubItem = 0;
    lvItem.header.cchTextMax = DISP_TEXT_SIZE;
    lvItem.header.pszText = NULL;
    lvItem.mustFree = FALSE;
    LISTVIEW_GetItem(hwnd,(LPLVITEMW)&lvItem,TRUE,TRUE);

    if ((lvItem.unicode && (lstrcmpAtoW(pszText,lvItem.header.pszText) != 0)) || (!lvItem.unicode && (lstrcmpA((LPSTR)lvItem.header.pszText,pszText) != 0)))
    {
      if (lpItemData->pszText != LPSTR_TEXTCALLBACKW)
      {
        if (isUnicodeNotify(&infoPtr->header))
          Str_SetPtrW(&lpItemData->pszText,textW);
        else
        {
          COMCTL32_Free(lpItemData->pszText);
          lpItemData->pszText = HEAP_strdupAtoW(GetProcessHeap(),0,pszText);
        }
      } else
      {
        NMLVDISPINFOW lvdi;
        BOOL retval;

        lvdi.item.mask        = TVIF_TEXT;
        lvdi.item.iItem       = nItem;
        lvdi.item.state       = lpItem->state;
        lvdi.item.lParam      = lpItem->lParam;
        if (isUnicodeNotify(&infoPtr->header))
        {
          lvdi.item.pszText     = textW;
          lvdi.item.cchTextMax  = lstrlenW(textW)+1;
        } else
        {
          lvdi.item.pszText     = (LPWSTR)pszText;
          lvdi.item.cchTextMax  = lstrlenA(pszText)+1;
        }

        retval = (BOOL)sendNotify(hwnd,isUnicodeNotify(&infoPtr->header) ? LVN_SETDISPINFOW:LVN_SETDISPINFOA,&lvdi.hdr);
      }
    }
    if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
  }

  if (textW) HeapFree(GetProcessHeap(),0,textW);

  return TRUE;
}

/***
 * DESCRIPTION:
 * Begin in place editing of specified list view item
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static HWND LISTVIEW_EditLabel(HWND hwnd, INT nItem,BOOL unicode)
{
  NMLVDISPINFOW dispInfo;
  RECT rect;
  LISTVIEW_ITEM *lpItem;
  LISTVIEW_ITEMDATA *lpItemData;
  HWND hedit;
  HINSTANCE hinst = GetWindowLongA(hwnd, GWL_HINSTANCE);
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LVITEMW lvItem;
  CHAR* textA = NULL;
  WCHAR* textW = NULL;

  if (!(infoPtr->dwStyle & LVS_EDITLABELS))
    return FALSE;

  /* Is the EditBox still there, if so remove it */
  if (infoPtr->hwndEdit)
    SetFocus(hwnd);

  LISTVIEW_SetSelection(hwnd,nItem);
  LISTVIEW_SetItemFocus(hwnd,nItem);

  ZeroMemory(&dispInfo, sizeof(NMLVDISPINFOA));
  if (!(lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,nItem)))
    return 0;

  if (!(lpItemData = LISTVIEW_GetItemData(lpItem->hdpaSubItems,0)))
    return 0;

  /* get information needed for drawing the item */
  ZeroMemory(&lvItem, sizeof(LVITEMW));
  lvItem.mask = LVIF_TEXT;
  lvItem.iItem = nItem;
  lvItem.iSubItem = 0;
  lvItem.cchTextMax = DISP_TEXT_SIZE;
  if (isUnicodeNotify(&infoPtr->header))
  {
    textW = (WCHAR*)COMCTL32_Alloc(DISP_TEXT_SIZE*sizeof(WCHAR));
    lvItem.pszText = textW;
    LISTVIEW_GetItem(hwnd,&lvItem,TRUE,FALSE);
    dispInfo.item.cchTextMax = lstrlenW(lvItem.pszText);
  } else
  {
    textA = (CHAR*)COMCTL32_Alloc(DISP_TEXT_SIZE*sizeof(CHAR));
    lvItem.pszText = (LPWSTR)textA;
    LISTVIEW_GetItem(hwnd,&lvItem,FALSE,FALSE);
    dispInfo.item.cchTextMax = lstrlenA((LPSTR)lvItem.pszText);
  }

  dispInfo.item.mask = 0;
  dispInfo.item.iItem = nItem;
  dispInfo.item.state = lpItem->state;
  dispInfo.item.stateMask = 0;
  dispInfo.item.pszText = lvItem.pszText;
  dispInfo.item.iImage = lpItemData->iImage;
  dispInfo.item.lParam = lpItem->lParam;

  if (sendNotify(hwnd,isUnicodeNotify(&infoPtr->header) ? LVN_BEGINLABELEDITW:LVN_BEGINLABELEDITA,&dispInfo.hdr))
    return 0;

  if (!LISTVIEW_GetItemRect(hwnd, nItem, &rect,LVIR_LABEL))
    return 0;

  //Edit uses ANSI by default -> convert
  if (isUnicodeNotify(&infoPtr->header))
    textA = UnicodeToAsciiString(textW);

  if (!(hedit = CreateEditLabel(textA , WS_VISIBLE,
                 rect.left-2, rect.top-1, 0,
                 rect.bottom - rect.top+2,
                 hwnd, hinst, LISTVIEW_EndEditLabel, nItem)))
         return 0;

  infoPtr->hwndEdit = hedit;
  SetFocus(hedit);
  SendMessageA(hedit, EM_SETSEL, 0, -1);

  if (textA) COMCTL32_Free(textA);
  if (textW) COMCTL32_Free(textW);

  return hedit;
}

/***
 * DESCRIPTION:
 * Ensures the specified item is visible, scrolling into view if necessary.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [I] BOOL : partially or entirely visible
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_EnsureVisible(HWND hwnd,INT nItem,BOOL bPartialOk)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nScrollPosHeight = 0;
  INT nScrollPosWidth = 0;
  RECT rcItem;
  POINT oldlefttop = infoPtr->lefttop;

  if (LISTVIEW_GetItemRect(hwnd, nItem, &rcItem,LVIR_BOUNDS))
  {
    if (bPartialOk && IntersectRect(NULL,&rcItem,&infoPtr->rcList))
      return TRUE;

    if (rcItem.left < infoPtr->rcList.left)
    {
      if (infoPtr->dwStyle & WS_HSCROLL)
      {
        /* scroll left */
        if (infoPtr->uView == LVS_LIST)
        {
          nScrollPosWidth = infoPtr->scrollStep.x;
          rcItem.left += infoPtr->rcList.left;
        } else if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
        {
          nScrollPosWidth = infoPtr->scrollStep.x;
          rcItem.left += infoPtr->rcList.left;
        }

        /* When in LVS_REPORT view, the scroll position should
           not be updated. */
        if (nScrollPosWidth)
        {
          if (rcItem.left % nScrollPosWidth == 0)
            infoPtr->lefttop.x += rcItem.left / nScrollPosWidth;
          else
            infoPtr->lefttop.x += rcItem.left / nScrollPosWidth - 1;

          if (infoPtr->lefttop.x != oldlefttop.x)
          {
            SCROLLINFO scrollInfo;

            ZeroMemory(&scrollInfo,sizeof(SCROLLINFO));
            scrollInfo.cbSize = sizeof(SCROLLINFO);
            scrollInfo.fMask  = SIF_POS;
            scrollInfo.nPos = infoPtr->lefttop.x;
            SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,TRUE);
          }
        }
      }
    } else if (rcItem.right > infoPtr->rcList.right)
    {
      if (infoPtr->dwStyle & WS_HSCROLL)
      {
        /* scroll right */
        if (infoPtr->uView == LVS_LIST)
        {
          rcItem.right -= infoPtr->rcList.right;
          nScrollPosWidth = infoPtr->scrollStep.x;
        } else if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
        {
          rcItem.right -= infoPtr->rcList.right;
          nScrollPosWidth = infoPtr->scrollStep.x;
        }

        /* When in LVS_REPORT view, the scroll position should
           not be updated. */
        if (nScrollPosWidth)
        {
          SCROLLINFO scrollInfo;

          if (rcItem.right % nScrollPosWidth == 0)
            infoPtr->lefttop.x += rcItem.right / nScrollPosWidth;
          else
            infoPtr->lefttop.x += rcItem.right / nScrollPosWidth + 1;

          if (infoPtr->lefttop.x != oldlefttop.x)
          {
            SCROLLINFO scrollInfo;

            ZeroMemory(&scrollInfo,sizeof(SCROLLINFO));
            scrollInfo.cbSize = sizeof(SCROLLINFO);
            scrollInfo.fMask  = SIF_POS;
            scrollInfo.nPos   = infoPtr->lefttop.x;
            SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,TRUE);
          }
        }
      }
    }

    if (rcItem.top < infoPtr->rcList.top)
    {
      /* scroll up */
      if (infoPtr->dwStyle & WS_VSCROLL)
      {
        if (infoPtr->uView == LVS_REPORT)
        {
          rcItem.top -= infoPtr->rcList.top;
          nScrollPosHeight = infoPtr->scrollStep.y;
        }
        else if ((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON))
        {
          nScrollPosHeight = infoPtr->scrollStep.y;
          rcItem.top += infoPtr->rcList.top;
        }

        if (rcItem.top % nScrollPosHeight == 0)
          infoPtr->lefttop.y += rcItem.top / nScrollPosHeight;
        else
          infoPtr->lefttop.y += rcItem.top / nScrollPosHeight - 1;

        if (infoPtr->lefttop.y != oldlefttop.y)
        {
          SCROLLINFO scrollInfo;

          ZeroMemory(&scrollInfo,sizeof(SCROLLINFO));
          scrollInfo.cbSize = sizeof(SCROLLINFO);
          scrollInfo.fMask  = SIF_POS;
          scrollInfo.nPos = infoPtr->lefttop.y;
          SetScrollInfo(hwnd,SB_VERT,&scrollInfo,TRUE);
        }
      }
    } else if (rcItem.bottom > infoPtr->rcList.bottom)
    {
      /* scroll down */
      if (infoPtr->dwStyle & WS_VSCROLL)
      {
        if (infoPtr->uView == LVS_REPORT)
        {
          rcItem.bottom -= infoPtr->rcList.bottom;
          nScrollPosHeight = infoPtr->scrollStep.y;
        } else if ((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON))
        {
          nScrollPosHeight = infoPtr->scrollStep.x;
          rcItem.bottom -= infoPtr->rcList.bottom;
        }

        if (rcItem.bottom % nScrollPosHeight == 0)
          infoPtr->lefttop.y += rcItem.bottom / nScrollPosHeight;
        else
          infoPtr->lefttop.y += rcItem.bottom / nScrollPosHeight + 1;
        if (infoPtr->lefttop.y != oldlefttop.y)
        {
          SCROLLINFO scrollInfo;

          ZeroMemory(&scrollInfo,sizeof(SCROLLINFO));
          scrollInfo.cbSize = sizeof(SCROLLINFO);
          scrollInfo.fMask  = SIF_POS;
          scrollInfo.nPos = infoPtr->lefttop.y;
          SetScrollInfo(hwnd,SB_VERT,&scrollInfo,TRUE);
        }
      }
    }
  }

  if ((oldlefttop.x != infoPtr->lefttop.x) || (oldlefttop.y != infoPtr->lefttop.y))
    LISTVIEW_ScrollWindow(hwnd,(oldlefttop.x-infoPtr->lefttop.x)*infoPtr->scrollStep.x,(oldlefttop.y-infoPtr->lefttop.y)*infoPtr->scrollStep.y);

  return TRUE;
}

/***
 * DESCRIPTION:
 * Retrieves the nearest item, given a position and a direction.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] POINT : start position
 * [I] UINT : direction
 *
 * RETURN:
 * Item index if successdful, -1 otherwise.
 */
static INT LISTVIEW_GetNearestItem(HWND hwnd, POINT pt, UINT vkDirection)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  LVHITTESTINFO lvHitTestInfo;
  INT nItem = -1;
  RECT rcView;

  if (LISTVIEW_GetViewRect(hwnd, &rcView) != FALSE)
  {
    ZeroMemory(&lvHitTestInfo, sizeof(LVHITTESTINFO));
    LISTVIEW_GetOrigin(hwnd, &lvHitTestInfo.pt);
    lvHitTestInfo.pt.x += pt.x;
    lvHitTestInfo.pt.y += pt.y;

    do
    {
      if (vkDirection == VK_DOWN)
      {
        lvHitTestInfo.pt.y += infoPtr->nItemHeight;
      }
      else if (vkDirection == VK_UP)
      {
        lvHitTestInfo.pt.y -= infoPtr->nItemHeight;
      }
      else if (vkDirection == VK_LEFT)
      {
        lvHitTestInfo.pt.x -= infoPtr->nItemWidth;
      }
      else if (vkDirection == VK_RIGHT)
      {
        lvHitTestInfo.pt.x += infoPtr->nItemWidth;
      }

      if (PtInRect(&rcView, lvHitTestInfo.pt) == FALSE)
      {
        return -1;
      }
      else
      {
        nItem = LISTVIEW_HitTestItem(hwnd, &lvHitTestInfo);
      }

    }
    while (nItem == -1);
  }

  return nItem;
}

/***
 * DESCRIPTION:
 * Searches for an item with specific characteristics.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : base item index
 * [I] LPLVFINDINFO : item information to look for
 *
 * RETURN:
 *   SUCCESS : index of item
 *   FAILURE : -1
 */
static LRESULT LISTVIEW_FindItem(HWND hwnd,INT nStart,LPLVFINDINFOW lpFindInfo,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  POINT ptItem;
  LVINTERNALITEMW lvItem;
  BOOL bWrap = FALSE;
  INT nItem = nStart;
  INT nLast = GETITEMCOUNT(infoPtr);

  if ((nItem >= -1) && lpFindInfo)
  {
    ZeroMemory(&lvItem,sizeof(LVITEMW));

    if (lpFindInfo->flags & LVFI_PARAM)
    {
      lvItem.header.mask |= LVIF_PARAM;
    }

    if (lpFindInfo->flags & LVFI_STRING)
    {
      lvItem.header.mask |= LVIF_TEXT;
      lvItem.header.pszText = NULL;
      lvItem.header.cchTextMax = DISP_TEXT_SIZE;
    }

    if (lpFindInfo->flags & LVFI_PARTIAL)
    {
      lvItem.header.mask |= LVIF_TEXT;
      lvItem.header.pszText = NULL;
      lvItem.header.cchTextMax = DISP_TEXT_SIZE;
    }

    if (lpFindInfo->flags & LVFI_WRAP)
    {
      bWrap = TRUE;
    }

    if (lpFindInfo->flags & LVFI_NEARESTXY)
    {
      ptItem.x = lpFindInfo->pt.x;
      ptItem.y = lpFindInfo->pt.y;
    }

    while (TRUE)
    {
      while (nItem < nLast)
      {
        if (lpFindInfo->flags & LVFI_NEARESTXY)
        {
          nItem = LISTVIEW_GetNearestItem(hwnd,ptItem,lpFindInfo->vkDirection);
          if (nItem != -1)
          {
            /* get position of the new item index */
            if (LISTVIEW_GetItemPosition(hwnd, nItem, &ptItem) == FALSE)
              return -1;
          }
          else
            return -1;
        }
        else
        {
          nItem++;
        }

        lvItem.header.iItem = nItem;
        lvItem.header.iSubItem = 0;

        lvItem.mustFree = FALSE;
        lvItem.header.pszText = NULL;
        lvItem.header.cchTextMax = DISP_TEXT_SIZE;
        if (LISTVIEW_GetItem(hwnd,(LVITEMW*)&lvItem,TRUE,TRUE))
        {
          if (lvItem.header.mask & LVIF_TEXT)
          {
            if (lpFindInfo->flags & LVFI_PARTIAL)
            {
              if (lstrstrAW(lvItem.header.pszText,lvItem.unicode,(LPWSTR)lpFindInfo->psz,unicode))
              {
                if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
                continue;
              }
            } else
            {
              if (lstrcmpAW(lvItem.header.pszText,lvItem.unicode,(LPWSTR)lpFindInfo->psz,unicode) != 0)
              {
                if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
                continue;
              }
            }
            if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);
          }

          if (lvItem.header.mask & LVIF_PARAM)
          {
            if (lpFindInfo->lParam != lvItem.header.lParam)
              continue;
          }

          return nItem;
        }
      }

      if (bWrap)
      {
        nItem = -1;
        nLast = nStart + 1;
        bWrap = FALSE;
      }
      else
      {
        return -1;
      }
    }
  }

  return -1;
}

/***
 * DESCRIPTION:
 * Retrieves the background color of the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * COLORREF associated with the background.
 */
static LRESULT LISTVIEW_GetBkColor(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->clrBk;
}

/***
 * DESCRIPTION:
 * Retrieves the background image of the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [O] LPLVMKBIMAGE : background image attributes
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE`
 */
static LRESULT LISTVIEW_GetBkImage(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
  LPLVBKIMAGEW plvbki = (LPLVBKIMAGEW)lParam;

  if (!plvbki) return FALSE;

  //todo: use COM OLE interface

  return FALSE;
}

/***
 * DESCRIPTION:
 * Retrieves the callback mask.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 *  Value of mask
 */
static UINT LISTVIEW_GetCallbackMask(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->uCallbackMask;
}

/***
 * DESCRIPTION:
 * Retrieves column attributes.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT :  column index
 * [IO] LPLVCOLUMNA : column information
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_GetColumn(HWND hwnd,INT nItem,LPLVCOLUMNW lpColumn,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  HDITEMW hdi;
  BOOL bResult = FALSE;

  if (lpColumn)
  {
    /* initialize memory */
    ZeroMemory(&hdi,sizeof(HDITEMW));

    if (lpColumn->mask & LVCF_FMT)
    {
      hdi.mask |= HDI_FORMAT;
    }

    if (lpColumn->mask & LVCF_WIDTH)
    {
      hdi.mask |= HDI_WIDTH;
    }

    if (lpColumn->mask & LVCF_TEXT)
    {
      hdi.mask |= HDI_TEXT;
      hdi.cchTextMax = lpColumn->cchTextMax;
      hdi.pszText    = lpColumn->pszText;
    }

    if (lpColumn->mask & LVCF_IMAGE)
    {
      hdi.mask |= HDI_IMAGE;
    }

    if (lpColumn->mask & LVCF_ORDER)
    {
      hdi.mask |= HDI_ORDER;
    }

    bResult = HEADER_GetItem(infoPtr->hwndHeader,(WPARAM)nItem,(LPARAM)&hdi,unicode);
    if (bResult)
    {
      if (lpColumn->mask & LVCF_FMT)
      {
        lpColumn->fmt = 0;

        if (hdi.fmt & HDF_LEFT)
        {
          lpColumn->fmt |= LVCFMT_LEFT;
        }
        else if (hdi.fmt & HDF_RIGHT)
        {
          lpColumn->fmt |= LVCFMT_RIGHT;
        }
        else if (hdi.fmt & HDF_CENTER)
        {
          lpColumn->fmt |= LVCFMT_CENTER;
        }

        if (hdi.fmt & HDF_IMAGE)
        {
          lpColumn->fmt |= LVCFMT_COL_HAS_IMAGES;
        }
      }

      if (lpColumn->mask & LVCF_WIDTH)
      {
        lpColumn->cx = hdi.cxy;
      }

      if (lpColumn->mask & LVCF_IMAGE)
      {
        lpColumn->iImage = hdi.iImage;
      }

      if (lpColumn->mask & LVCF_ORDER)
      {
        lpColumn->iOrder = hdi.iOrder;
      }
    }
  }

  return bResult;
}

static LRESULT LISTVIEW_GetColumnOrderArray(HWND hwnd, INT iCount, LPINT lpiArray)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  return HEADER_GetOrderArray(infoPtr->hwndHeader,(WPARAM)iCount,(LPARAM)lpiArray);
}

/***
 * DESCRIPTION:
 * Retrieves the column width.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] int : column index
 *
 * RETURN:
 *   SUCCESS : column width
 *   FAILURE : zero
 */
static LRESULT LISTVIEW_GetColumnWidth(HWND hwnd, INT nColumn)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (infoPtr->uView == LVS_LIST)
  {
    return infoPtr->nItemWidth;
  } else if (infoPtr->uView == LVS_REPORT)
  {
    HDITEMW hdi;

    /* get column width from header */
    ZeroMemory(&hdi,sizeof(HDITEMW));
    hdi.mask = HDI_WIDTH;
    if (HEADER_GetItem(infoPtr->hwndHeader,(WPARAM)nColumn,(LPARAM)&hdi,TRUE))
    {
      return hdi.cxy;
    }
  }

  return 0;
}

/***
 * DESCRIPTION:
 * In list or report display mode, retrieves the number of items that can fit
 * vertically in the visible area. In icon or small icon display mode,
 * retrieves the total number of visible items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Number of fully visible items.
 */
static LRESULT LISTVIEW_GetCountPerPage(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nItemCount = 0;

  if (infoPtr->uView == LVS_LIST)
  {
    if (infoPtr->rcList.right > infoPtr->nItemWidth)
    {
      nItemCount = LISTVIEW_GetCountPerRow(hwnd) *
                   LISTVIEW_GetCountPerColumn(hwnd);
    }
  }
  else if (infoPtr->uView == LVS_REPORT)
  {
    nItemCount = LISTVIEW_GetCountPerColumn(hwnd);
  }
  else
  {
    nItemCount = GETITEMCOUNT(infoPtr);
  }

  return nItemCount;
}

/* LISTVIEW_GetEditControl */

/***
 * DESCRIPTION:
 * Retrieves the extended listview style.
 *
 * PARAMETERS:
 * [I] HWND  : window handle
 *
 * RETURN:
 *   SUCCESS : previous style
 *   FAILURE : 0
 */
static LRESULT LISTVIEW_GetExtendedListViewStyle(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return (infoPtr->dwExStyle);
}

/***
 * DESCRIPTION:
 * Retrieves the handle to the header control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Header handle.
 */
static LRESULT LISTVIEW_GetHeader(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->hwndHeader;
}

static LRESULT LISTVIEW_GetHotCursor(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return (LRESULT)infoPtr->hHotCursor;
}

static LRESULT LISTVIEW_GetHoverTime(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return (LRESULT)infoPtr->hoverTime;
}

static LRESULT LISTVIEW_SetHoverTime(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT oldTime = infoPtr->hoverTime;

  infoPtr->hoverTime = (INT)lParam;
  if (infoPtr->hoverTime == -1) infoPtr->hoverTime = DEFAULT_HOVERTIME;

  return (LRESULT)oldTime;
}

/***
 * DESCRIPTION:
 * Retrieves an image list handle.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : image list identifier
 *
 * RETURN:
 *   SUCCESS : image list handle
 *   FAILURE : NULL
 */
static LRESULT LISTVIEW_GetImageList(HWND hwnd, INT nImageList)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  HIMAGELIST himl = NULL;

  switch (nImageList)
  {
    case LVSIL_NORMAL:
      himl = infoPtr->himlNormal;
      break;

    case LVSIL_SMALL:
      himl = infoPtr->himlSmall;
      break;

    case LVSIL_STATE:
      himl = infoPtr->himlState;
      break;
  }

  return (LRESULT)himl;
}

/***
 * DESCRIPTION:
 * Retrieves item attributes.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [IO] LPLVITEMW : item info
 * [I] internal : if true then we will use tricks that avoid copies
 *                but are not compatible with the regular interface
 *                the unicode parameter is only used for NULL strings,
 *                the caller is responsible for ASCII/UNICODE translations
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_GetItem(HWND hwnd,LPLVITEMW lpLVItem,BOOL unicode,BOOL internal)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;

  if (lpLVItem)
  {
    if ((lpLVItem->iItem >= 0) && (lpLVItem->iItem < GETITEMCOUNT(infoPtr)))
    {
      LISTVIEW_ITEM *lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,lpLVItem->iItem);

      if (lpItem)
      {
        LISTVIEW_ITEMDATA *lpItemData = LISTVIEW_GetItemData(lpItem->hdpaSubItems,lpLVItem->iSubItem);

        //add subitem if not present
        if (!lpItemData && (lpLVItem->iSubItem > 0) && (lpLVItem->iSubItem < HEADER_GetItemCount(infoPtr->hwndHeader)))
        {
          INT nPosition;

          lpItemData = (LISTVIEW_ITEMDATA*)COMCTL32_Alloc(sizeof(LISTVIEW_ITEMDATA));
          ZeroMemory(lpItemData,sizeof(LISTVIEW_ITEMDATA));
          lpItemData->pszText = LPSTR_TEXTCALLBACKW;
          lpItemData->iImage = I_IMAGECALLBACK;
          lpItemData->iSubItem = lpLVItem->iSubItem;
          nPosition = LISTVIEW_FindInsertPosition(lpItem->hdpaSubItems,lpItemData->iSubItem);
          DPA_InsertPtr(lpItem->hdpaSubItems,nPosition,lpItemData);
        }

        if (lpItemData)
        {
          NMLVDISPINFOW dispInfo;
          LPSTR textA = NULL;
          LPWSTR textW = NULL;

          ZeroMemory(&dispInfo,sizeof(NMLVDISPINFOW));
          bResult = TRUE;

          if ((lpItemData->iImage == I_IMAGECALLBACK) && (lpLVItem->mask & LVIF_IMAGE))
            dispInfo.item.mask |= LVIF_IMAGE;

          if ((lpItemData->pszText == LPSTR_TEXTCALLBACKW) && (lpLVItem->mask & LVIF_TEXT))
          {
            dispInfo.item.mask |= LVIF_TEXT;
            dispInfo.item.cchTextMax = lpLVItem->cchTextMax;
            if (isUnicodeNotify(&infoPtr->header))
            {
              textW = (WCHAR*)COMCTL32_Alloc(dispInfo.item.cchTextMax*sizeof(WCHAR));
              if (textW) textW[0] = 0;
              dispInfo.item.pszText = textW;
            } else
            {
              textA = (CHAR*)COMCTL32_Alloc(dispInfo.item.cchTextMax*sizeof(CHAR));
              if (textA) textA[0] = 0;
              dispInfo.item.pszText = (LPWSTR)textA;
            }
          }

          if ((infoPtr->uCallbackMask != 0) && (lpLVItem->mask & LVIF_STATE))
          {
            dispInfo.item.mask |= LVIF_STATE;
            dispInfo.item.stateMask = infoPtr->uCallbackMask;
          }

          if (dispInfo.item.mask != 0)
          {
            dispInfo.item.iItem = lpLVItem->iItem;
            dispInfo.item.iSubItem = lpLVItem->iSubItem;
            dispInfo.item.lParam = lpItem->lParam;
            sendNotify(hwnd,isUnicodeNotify(&infoPtr->header) ? LVN_GETDISPINFOW:LVN_GETDISPINFOA,&dispInfo.hdr);
          }

          if (dispInfo.item.mask & LVIF_IMAGE)
          {
            lpLVItem->iImage = dispInfo.item.iImage;
            if (dispInfo.item.mask & LVIF_DI_SETITEM)
              lpItemData->iImage = dispInfo.item.iImage;
          } else if (lpLVItem->mask & LVIF_IMAGE)
              lpLVItem->iImage = lpItemData->iImage;

          if (dispInfo.item.mask & LVIF_TEXT)
          {
            if (dispInfo.item.mask & LVIF_DI_SETITEM)
            {
              if (isUnicodeNotify(&infoPtr->header))
              {
                Str_SetPtrW(&lpItemData->pszText,(dispInfo.item.pszText == LPSTR_TEXTCALLBACKW) ? NULL:dispInfo.item.pszText);
              } else
              {
                INT len = (dispInfo.item.pszText && (dispInfo.item.pszText != LPSTR_TEXTCALLBACKW)) ? lstrlenA((LPSTR)dispInfo.item.pszText):0;

                if (lpItemData->pszText != LPSTR_TEXTCALLBACKW) COMCTL32_Free(lpItemData->pszText);
                if (len > 0)
                {
                  len++;
                  lpItemData->pszText = (WCHAR*)COMCTL32_Alloc(len*sizeof(WCHAR));
                  lstrcpyAtoW(lpItemData->pszText,(LPSTR)dispInfo.item.pszText);
                } else lpItemData->pszText = NULL;
              }
            }
            /* Make sure the source string is valid */
            if (!dispInfo.item.pszText || (dispInfo.item.pszText == LPSTR_TEXTCALLBACKW))
            {
              if (!internal)
              {
                if (unicode)
                  lpLVItem->pszText[0] = L'\0';
                else
                  ((LPSTR)lpLVItem->pszText)[0] = '\0';
              } else
              {
                lpLVItem->pszText = NULL;
                ((LPLVINTERNALITEMW)lpLVItem)->unicode = unicode;
              }
            } else
            {
              if (internal)
              {
                lpLVItem->pszText = dispInfo.item.pszText;
                if (isUnicodeNotify(&infoPtr->header))
                {
                  ((LPLVINTERNALITEMW)lpLVItem)->unicode = TRUE;
                  if (textW == dispInfo.item.pszText)
                  {
                    ((LPLVINTERNALITEMW)lpLVItem)->mustFree = TRUE;
                    textW = NULL;
                  } else ((LPLVINTERNALITEMW)lpLVItem)->mustFree = FALSE;
                } else
                {
                  ((LPLVINTERNALITEMW)lpLVItem)->unicode = FALSE;
                  if (textA == (LPSTR)dispInfo.item.pszText)
                  {
                    ((LPLVINTERNALITEMW)lpLVItem)->mustFree = TRUE;
                    textA = NULL;
                  } else ((LPLVINTERNALITEMW)lpLVItem)->mustFree = FALSE;
                }
              } else
              {
                if (unicode)
                {
                  if (isUnicodeNotify(&infoPtr->header))
                    lstrcpynW(lpLVItem->pszText,dispInfo.item.pszText,lpLVItem->cchTextMax);
                  else
                    lstrcpynAtoW(lpLVItem->pszText,(LPSTR)dispInfo.item.pszText,lpLVItem->cchTextMax);
                  lpLVItem->pszText[lpLVItem->cchTextMax-1] = L'\0';
                } else
                {
                  if (isUnicodeNotify(&infoPtr->header))
                    lstrcpynWtoA((LPSTR)lpLVItem->pszText,dispInfo.item.pszText,lpLVItem->cchTextMax);
                  else
                    lstrcpynA((LPSTR)lpLVItem->pszText,(LPSTR)dispInfo.item.pszText,lpLVItem->cchTextMax);
                  ((LPSTR)lpLVItem->pszText)[lpLVItem->cchTextMax-1] = '\0';
                }
              }
            }
          } else if (lpLVItem->mask & LVIF_TEXT)
          {
            if (lpItemData->pszText)
            {
              if (internal)
              {
                lpLVItem->pszText = lpItemData->pszText;
                ((LPLVINTERNALITEMW)lpLVItem)->unicode = TRUE;
              } else
              {
                if (unicode)
                {
                  lstrcpynW(lpLVItem->pszText,lpItemData->pszText,lpLVItem->cchTextMax);
                  lpLVItem->pszText[lpLVItem->cchTextMax-1] = L'\0';
                } else
                {
                  lstrcpynWtoA((LPSTR)lpLVItem->pszText,lpItemData->pszText,lpLVItem->cchTextMax);
                  ((LPSTR)lpLVItem->pszText)[lpLVItem->cchTextMax-1] = '\0';
                }
              }
            } else
            {
              if (internal)
              {
                lpLVItem->pszText = NULL;
                ((LPLVINTERNALITEMW)lpLVItem)->unicode = unicode;
              } else
              {
                if (unicode)
                  lpLVItem->pszText[0] = 0;
                else
                  ((CHAR*)lpLVItem->pszText)[0] = 0;
              }
            }
          }

          if (dispInfo.item.mask & LVIF_STATE)
          {
            lpLVItem->state = lpItem->state;
            lpLVItem->state &= ~dispInfo.item.stateMask;
            lpLVItem->state |= (dispInfo.item.state & dispInfo.item.stateMask);
          } else if (lpLVItem->mask & LVIF_STATE)
              lpLVItem->state = lpItem->state & lpLVItem->stateMask;

          if (lpLVItem->mask & LVIF_PARAM)
            lpLVItem->lParam = lpItem->lParam;

          if (lpLVItem->mask & LVIF_INDENT)
            lpLVItem->iIndent = lpItem->iIndent;

          if (textA) COMCTL32_Free(textA);
          if (textW) COMCTL32_Free(textW);
        }
      }
    }
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Retrieves the index of the hot item.
 *
 * PARAMETERS:
 * [I] HWND  : window handle
 *
 * RETURN:
 *   SUCCESS : hot item index
 *   FAILURE : -1 (no hot item)
 */
static LRESULT LISTVIEW_GetHotItem(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return (infoPtr->nHotItem);
}

/***
 * DESCRIPTION:
 * Retrieves the number of items in the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Number of items.
 */
static LRESULT LISTVIEW_GetItemCount(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  return GETITEMCOUNT(infoPtr);
}

/***
 * DESCRIPTION:
 * Retrieves the position (upper-left) of the listview control item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [O] LPPOINT : coordinate information
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_GetItemPosition(HWND hwnd, INT nItem, LPPOINT lpptPosition)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  INT nCountPerColumn;
  INT nRow;

  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)) && lpptPosition)
  {
    if (infoPtr->uView == LVS_LIST)
    {
      bResult = TRUE;
      nItem = nItem - LISTVIEW_GetTopIndex(hwnd);
      nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);
      if (nItem < 0)
      {
        nRow = nItem % nCountPerColumn;
        if (nRow == 0)
        {
          lpptPosition->x = nItem / nCountPerColumn * infoPtr->nItemWidth;
          lpptPosition->y = 0;
        }
        else
        {
          lpptPosition->x = (nItem / nCountPerColumn -1) * infoPtr->nItemWidth;
          lpptPosition->y = (nRow + nCountPerColumn) * infoPtr->nItemHeight;
        }
      }
      else
      {
        lpptPosition->x = nItem / nCountPerColumn * infoPtr->nItemWidth;
        lpptPosition->y = nItem % nCountPerColumn * infoPtr->nItemHeight;
      }
    } else if (infoPtr->uView == LVS_REPORT)
    {
      bResult = TRUE;
      lpptPosition->x = REPORT_MARGINX;
      lpptPosition->y = ((nItem - LISTVIEW_GetTopIndex(hwnd)) *
                         infoPtr->nItemHeight) + infoPtr->rcList.top;
    } else
    {
      LISTVIEW_ITEM *lpItem;

      lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,nItem);
      if (lpItem)
      {
        bResult = TRUE;
        lpptPosition->x = lpItem->ptPosition.x;
        lpptPosition->y = lpItem->ptPosition.y;
      }
    }
  }

  return bResult;
}

static BOOL LISTVIEW_GetSubItemPosition(HWND hwnd,INT nItem,INT nSubItem,LPPOINT lpptPosition)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  RECT rect;

  if (HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)nSubItem,(LPARAM)&rect))
  {
    lpptPosition->x = rect.left+REPORT_MARGINX;
    lpptPosition->y = ((nItem-LISTVIEW_GetTopIndex(hwnd))*infoPtr->nItemHeight)+infoPtr->rcList.top;

    return TRUE;
  }

  return FALSE;
}

/***
 * DESCRIPTION:
 * Retrieves the bounding rectangle for a listview control item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [IO] LPRECT : bounding rectangle coordinates
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_GetItemRect(HWND hwnd,INT nItem,LPRECT lprc,INT code)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  POINT ptOrigin;
  POINT ptItem;
  HDC hdc;
  HFONT hOldFont;
  INT nLeftPos;
  INT nLabelWidth;
  TEXTMETRICA tm;

  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)) && lprc)
  {
    if (LISTVIEW_GetItemPosition(hwnd, nItem, &ptItem))
    {
      switch(code)
      {
        case LVIR_ICON:
          if (infoPtr->uView == LVS_ICON)
          {
            if (infoPtr->himlNormal)
            {
              if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
              {
                bResult = TRUE;
                lprc->left = ptItem.x + ptOrigin.x;
                lprc->top = ptItem.y + ptOrigin.y;
                lprc->right = lprc->left + infoPtr->iconSize.cx;
                lprc->bottom = (lprc->top + infoPtr->iconSize.cy +
                                ICON_BOTTOM_PADDING + ICON_TOP_PADDING);
              }
            }
          }
          else if (infoPtr->uView == LVS_SMALLICON)
          {
            if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
            {
              bResult = TRUE;
              lprc->left = ptItem.x + ptOrigin.x;
              lprc->top = ptItem.y + ptOrigin.y;
              lprc->bottom = lprc->top + infoPtr->nItemHeight;

              if (infoPtr->himlState)
                lprc->left += infoPtr->iconSize.cx;

              if (infoPtr->himlSmall)
                lprc->right = lprc->left + infoPtr->iconSize.cx;
              else
                lprc->right = lprc->left;
            }
          }
          else
          {
            bResult = TRUE;
            lprc->left = ptItem.x;
            lprc->top = ptItem.y;
            lprc->bottom = lprc->top + infoPtr->nItemHeight;

            if (infoPtr->himlState)
            {
              lprc->left += infoPtr->iconSize.cx;
            }

            if (infoPtr->himlSmall != NULL)
            {
              lprc->right = lprc->left + infoPtr->iconSize.cx;
            }
            else
            {
              lprc->right = lprc->left;
            }
          }
          break;

        case LVIR_LABEL:
          if (infoPtr->uView == LVS_ICON)
          {
            if (infoPtr->himlNormal)
            {
              if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
              {
                bResult = TRUE;
                lprc->left = ptItem.x + ptOrigin.x;
                lprc->top = (ptItem.y + ptOrigin.y + infoPtr->iconSize.cy +
                             ICON_BOTTOM_PADDING + ICON_TOP_PADDING);
                nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,0);
                if (infoPtr->iconSpacing.cx - nLabelWidth > 1)
                {
                  lprc->left += (infoPtr->iconSpacing.cx - nLabelWidth) / 2;
                  lprc->right = lprc->left + nLabelWidth;
                }
                else
                {
                  lprc->left += 1;
                  lprc->right = lprc->left + infoPtr->iconSpacing.cx - 1;
                }

                hdc = GetDC(hwnd);
                hOldFont = SelectObject(hdc, infoPtr->hFont);
                GetTextMetricsA(hdc, &tm);
                lprc->bottom = lprc->top + tm.tmHeight + HEIGHT_PADDING;
                SelectObject(hdc, hOldFont);
                ReleaseDC(hwnd, hdc);
              }
            }
          }
          else if (infoPtr->uView == LVS_SMALLICON)
          {
            if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
            {
              bResult = TRUE;
              nLeftPos = lprc->left = ptItem.x + ptOrigin.x;
              lprc->top = ptItem.y + ptOrigin.y;
              lprc->bottom = lprc->top + infoPtr->nItemHeight;

              if (infoPtr->himlState)
              {
                lprc->left += infoPtr->iconSize.cx;
              }

              if (infoPtr->himlSmall)
              {
                lprc->left += infoPtr->iconSize.cx;
              }

              nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,0);
              if (lprc->left + nLabelWidth < nLeftPos + infoPtr->nItemWidth)
              {
                lprc->right = lprc->left + nLabelWidth;
              }
              else
              {
                lprc->right = nLeftPos + infoPtr->nItemWidth;
              }
            }
          }
          else
          {
            bResult = TRUE;
            nLeftPos = lprc->left = ptItem.x;
            lprc->top = ptItem.y;
            lprc->bottom = lprc->top + infoPtr->nItemHeight;

            if (infoPtr->himlState)
            {
              lprc->left += infoPtr->iconSize.cx;
            }

            if (infoPtr->himlSmall)
            {
              lprc->left += infoPtr->iconSize.cx;
            }

            nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,0);
            if (lprc->left + nLabelWidth < nLeftPos + infoPtr->nItemWidth)
            {
              lprc->right = lprc->left + nLabelWidth;
            }
            else
            {
              lprc->right = nLeftPos + infoPtr->nItemWidth;
            }
          }
          break;

        case LVIR_BOUNDS:
          if (infoPtr->uView == LVS_ICON)
          {
            if (infoPtr->himlNormal)
            {
              if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
              {
                bResult = TRUE;
                hdc = GetDC(hwnd);
                hOldFont = SelectObject(hdc,infoPtr->hFont);
                GetTextMetricsA(hdc,&tm);
                SelectObject(hdc,hOldFont);
                ReleaseDC(hwnd,hdc);
                lprc->left = ptItem.x + ptOrigin.x;
                lprc->top = ptItem.y + ptOrigin.y;
                lprc->right = lprc->left + infoPtr->iconSpacing.cx;
                lprc->bottom = lprc->top+infoPtr->iconSpacing.cy+ICON_BOTTOM_PADDING+tm.tmHeight+HEIGHT_PADDING;
              }
            }
          }
          else if (infoPtr->uView == LVS_SMALLICON)
          {
            if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
            {
              bResult = TRUE;
              lprc->left = ptItem.x + ptOrigin.x;
              lprc->right = lprc->left;
              lprc->top = ptItem.y + ptOrigin.y;
              lprc->bottom = lprc->top + infoPtr->nItemHeight;
              if (infoPtr->himlState)
                lprc->right += infoPtr->iconSize.cx;
              if (infoPtr->himlSmall)
                lprc->right += infoPtr->iconSize.cx;
              lprc->right += LISTVIEW_GetLabelWidth(hwnd,nItem,0);
            }
          }
          else
          {
            bResult = TRUE;
            lprc->left = ptItem.x;
            lprc->right = lprc->left;
            lprc->top = ptItem.y;
            lprc->bottom = lprc->top + infoPtr->nItemHeight;

            if (infoPtr->himlState)
            {
              lprc->right += infoPtr->iconSize.cx;
            }

            if (infoPtr->himlSmall)
            {
              lprc->right += infoPtr->iconSize.cx;
            }

            nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,0);
            if (lprc->right + nLabelWidth < lprc->left + infoPtr->nItemWidth)
            {
              lprc->right += nLabelWidth;
            }
            else
            {
              lprc->right = lprc->left + infoPtr->nItemWidth;
            }
          }
          break;

        case LVIR_SELECTBOUNDS:
          if (infoPtr->uView == LVS_ICON)
          {
            if (infoPtr->himlNormal)
            {
              if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
              {
                bResult = TRUE;
                hdc = GetDC(hwnd);
                hOldFont = SelectObject(hdc,infoPtr->hFont);
                GetTextMetricsA(hdc,&tm);
                SelectObject(hdc,hOldFont);
                ReleaseDC(hwnd,hdc);
                lprc->left = ptItem.x + ptOrigin.x;
                lprc->top = ptItem.y + ptOrigin.y;
                lprc->right = lprc->left + infoPtr->iconSpacing.cx;
                lprc->bottom = lprc->top+infoPtr->iconSpacing.cy+ICON_BOTTOM_PADDING+tm.tmHeight+HEIGHT_PADDING;
              }
            }
          }
          else if (infoPtr->uView == LVS_SMALLICON)
          {
            if (LISTVIEW_GetOrigin(hwnd, &ptOrigin))
            {
              bResult = TRUE;
              nLeftPos= lprc->left = ptItem.x + ptOrigin.x;
              lprc->top = ptItem.y + ptOrigin.y;
              lprc->bottom = lprc->top + infoPtr->nItemHeight;

              if (infoPtr->himlState)
              {
                lprc->left += infoPtr->iconSize.cx;
              }

              lprc->right = lprc->left;

              if (infoPtr->himlSmall)
              {
                lprc->right += infoPtr->iconSize.cx;
              }

              nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,0);
              if (lprc->right + nLabelWidth < lprc->left + infoPtr->nItemWidth)
              {
                lprc->right += nLabelWidth;
              }
              else
              {
                lprc->right = lprc->left + infoPtr->nItemWidth;
              }
            }
          }
          else
          {
            bResult = TRUE;
            nLeftPos = lprc->left = ptItem.x;
            lprc->top = ptItem.y;
            lprc->bottom = lprc->top + infoPtr->nItemHeight;

            if (infoPtr->himlState)
            {
              lprc->left += infoPtr->iconSize.cx;
            }

            lprc->right = lprc->left;

            if (infoPtr->himlSmall)
            {
              lprc->right += infoPtr->iconSize.cx;
            }

            nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,0);
            if (lprc->right + nLabelWidth < nLeftPos + infoPtr->nItemWidth)
            {
              lprc->right += nLabelWidth;
            }
            else
            {
              lprc->right = nLeftPos + infoPtr->nItemWidth;
            }
          }
          break;
      }
    }
  }

  return bResult;
}

static LRESULT LISTVIEW_GetSubItemRect(HWND hwnd,INT nItem,LPRECT lprc)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nSubItem,code,nLabelWidth;
  POINT ptItem;

  if (!lprc || (infoPtr->uView != LVS_REPORT) || (nItem < 0) || (nItem >= GETITEMCOUNT(infoPtr))) return FALSE;

  nSubItem = lprc->top;
  if ((nSubItem < 0) || (nSubItem >= HEADER_GetItemCount(infoPtr->hwndHeader))) return FALSE;
  if (!LISTVIEW_GetSubItemPosition(hwnd,nItem,nSubItem,&ptItem)) return FALSE;

  code = lprc->left;

  switch (code)
  {
    case LVIR_BOUNDS:
      lprc->left = ptItem.x;
      lprc->right = lprc->left;
      lprc->top = ptItem.y;
      lprc->bottom = lprc->top+infoPtr->nItemHeight;

      if ((infoPtr->dwExStyle & LVS_EX_SUBITEMIMAGES) && infoPtr->himlSmall)
      {
        lprc->right += infoPtr->iconSize.cx;
      }

      nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,nSubItem);
      lprc->right += nLabelWidth;
      break;

    case LVIR_ICON:
      if (infoPtr->dwExStyle & LVS_EX_SUBITEMIMAGES)
      {
        lprc->left = ptItem.x;
        lprc->top = ptItem.y;
        lprc->bottom = lprc->top+infoPtr->nItemHeight;

        if (infoPtr->himlSmall)
        {
          lprc->right = lprc->left+infoPtr->iconSize.cx;
        } else
        {
          lprc->right = lprc->left;
        }
        break;
      } else return FALSE;

    case LVIR_LABEL:
      lprc->left = ptItem.x;
      lprc->right = lprc->left;
      lprc->top = ptItem.y;
      lprc->bottom = lprc->top+infoPtr->nItemHeight;

      nLabelWidth = LISTVIEW_GetLabelWidth(hwnd,nItem,nSubItem);
      lprc->right += nLabelWidth;
      break;

    default:
      return FALSE;
  }

  return TRUE;
}

/***
 * DESCRIPTION:
 * Retrieves the width of a label.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 *   SUCCESS : string width (in pixels)
 *   FAILURE : zero
 */
INT LISTVIEW_GetLabelWidth(HWND hwnd,INT nItem,INT nSubItem)
{
  INT nLabelWidth = 0;
  LVINTERNALITEMW lvItem;

  ZeroMemory(&lvItem, sizeof(LVITEMW));
  lvItem.header.mask = LVIF_TEXT;
  lvItem.header.iItem = nItem;
  lvItem.header.iSubItem = nSubItem;
  lvItem.header.cchTextMax = DISP_TEXT_SIZE;
  lvItem.header.pszText = NULL;
  lvItem.mustFree = FALSE;
  if (LISTVIEW_GetItem(hwnd,(LPLVITEMW)&lvItem,TRUE,TRUE))
    nLabelWidth = LISTVIEW_GetStringWidth(hwnd,0,lvItem.header.pszText,lvItem.unicode);
  if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);

  return nLabelWidth;
}

/***
 * DESCRIPTION:
 * Retrieves the spacing between listview control items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] BOOL : flag for small or large icon
 *
 * RETURN:
 * Horizontal + vertical spacing
 */
LRESULT LISTVIEW_GetItemSpacing(HWND hwnd, BOOL bSmall)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LONG lResult;

  if (bSmall == FALSE)
  {
    lResult = MAKELONG(infoPtr->iconSpacing.cx, infoPtr->iconSpacing.cy);
  }
  else
  {
    /* TODO: need to store width of smallicon item */
    lResult = MAKELONG(0, infoPtr->nItemHeight);
  }

  return lResult;
}

/***
 * DESCRIPTION:
 * Retrieves the state of a listview control item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [I] UINT : state mask
 *
 * RETURN:
 * State specified by the mask.
 */
LRESULT LISTVIEW_GetItemState(HWND hwnd, INT nItem, UINT uMask)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  LVITEMW lvItem;
  UINT uState = 0;

  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    ZeroMemory(&lvItem, sizeof(LVITEMW));
    lvItem.iItem = nItem;
    lvItem.stateMask = uMask;
    lvItem.mask = LVIF_STATE;
    if (LISTVIEW_GetItem(hwnd,&lvItem,TRUE,FALSE) != FALSE)
    {
      uState = lvItem.state;
    }
  }

  return uState;
}

/***
 * DESCRIPTION:
 * Retrieves the text of a listview control item or subitem.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [IO] LPLVITEMW : item information
 *
 * RETURN:
 *   SUCCESS : string length
 *   FAILURE : 0
 */
LRESULT LISTVIEW_GetItemText(HWND hwnd,INT nItem,LPLVITEMW lpLVItem,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nLength = 0;

  if (lpLVItem)
  {
    if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
    {
      lpLVItem->mask = LVIF_TEXT;
      lpLVItem->iItem = nItem;
      if (LISTVIEW_GetItem(hwnd,lpLVItem,unicode,FALSE))
      {
        nLength = unicode ? lstrlenW(lpLVItem->pszText):lstrlenA((LPSTR)lpLVItem->pszText);
      }
    }
  }

  return nLength;
}

/***
 * DESCRIPTION:
 * Searches for an item based on properties + relationships.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [I] INT : relationship flag
 *
 * RETURN:
 *   SUCCESS : item index
 *   FAILURE : -1
 */
static LRESULT LISTVIEW_GetNextItem(HWND hwnd, INT nItem, UINT uFlags)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  UINT uMask = 0;
  LVFINDINFOW lvFindInfo;
  INT nCountPerColumn;
  INT i;

  if ((nItem >= -1) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    ZeroMemory(&lvFindInfo, sizeof(LVFINDINFOW));

    if (uFlags & LVNI_CUT)
      uMask |= LVIS_CUT;

    if (uFlags & LVNI_DROPHILITED)
      uMask |= LVIS_DROPHILITED;

    if (uFlags & LVNI_FOCUSED)
      uMask |= LVIS_FOCUSED;

    if (uFlags & LVNI_SELECTED)
      uMask |= LVIS_SELECTED;

    if (uFlags & LVNI_ABOVE)
    {
      if ((infoPtr->uView == LVS_LIST) || (infoPtr->uView == LVS_REPORT))
      {
        while (nItem >= 0)
        {
          nItem--;
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
      else
      {
        lvFindInfo.flags = LVFI_NEARESTXY;
        lvFindInfo.vkDirection = VK_UP;
        LISTVIEW_GetItemPosition(hwnd, nItem, &lvFindInfo.pt);
        while ((nItem = LISTVIEW_FindItem(hwnd,nItem,&lvFindInfo,TRUE)) != -1)
        {
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
    }
    else if (uFlags & LVNI_BELOW)
    {
      if ((infoPtr->uView == LVS_LIST) || (infoPtr->uView == LVS_REPORT))
      {
        while (nItem < GETITEMCOUNT(infoPtr))
        {
          nItem++;
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
      else
      {
        lvFindInfo.flags = LVFI_NEARESTXY;
        lvFindInfo.vkDirection = VK_DOWN;
        LISTVIEW_GetItemPosition(hwnd, nItem, &lvFindInfo.pt);
        while ((nItem = LISTVIEW_FindItem(hwnd,nItem,&lvFindInfo,TRUE)) != -1)
        {
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
    }
    else if (uFlags & LVNI_TOLEFT)
    {
      if (infoPtr->uView == LVS_LIST)
      {
        nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);
        while (nItem - nCountPerColumn >= 0)
        {
          nItem -= nCountPerColumn;
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
      else if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
      {
        lvFindInfo.flags = LVFI_NEARESTXY;
        lvFindInfo.vkDirection = VK_LEFT;
        LISTVIEW_GetItemPosition(hwnd, nItem, &lvFindInfo.pt);
        while ((nItem = LISTVIEW_FindItem(hwnd,nItem,&lvFindInfo,TRUE)) != -1)
        {
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
    }
    else if (uFlags & LVNI_TORIGHT)
    {
      if (infoPtr->uView == LVS_LIST)
      {
        nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);
        while (nItem + nCountPerColumn < GETITEMCOUNT(infoPtr))
        {
          nItem += nCountPerColumn;
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
      else if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
      {
        lvFindInfo.flags = LVFI_NEARESTXY;
        lvFindInfo.vkDirection = VK_RIGHT;
        LISTVIEW_GetItemPosition(hwnd,nItem,&lvFindInfo.pt);
        while ((nItem = LISTVIEW_FindItem(hwnd,nItem,&lvFindInfo,TRUE)) != -1)
        {
          if ((LISTVIEW_GetItemState(hwnd, nItem, uMask) & uMask) == uMask)
            return nItem;
        }
      }
    }
    else
    {
      nItem++;

      /* search by index */
      for (i = nItem; i < GETITEMCOUNT(infoPtr); i++)
      {
        if ((LISTVIEW_GetItemState(hwnd, i, uMask) & uMask) == uMask)
          return i;
      }
    }
  }

  return -1;
}

static LRESULT LISTVIEW_GetNumberOfWorkAreas(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LPINT lpuWorkAreas = (LPINT)lParam;

  if (!lpuWorkAreas) return FALSE;

  *lpuWorkAreas = infoPtr->nWorkAreas;

  return TRUE;
}

static LRESULT LISTVIEW_GetWorkAreas(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nWorkAreas = (INT)wParam;
  LPRECT lprc = (LPRECT)lParam;

  if ((nWorkAreas <= 0) || (nWorkAreas > infoPtr->nWorkAreas) || !lprc) return FALSE;

  for (INT x = 0;x < nWorkAreas;x++)
    lprc[x] = infoPtr->rcWorkAreas[x];

  return TRUE;
}

static LRESULT LISTVIEW_SetWorkAreas(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nWorkAreas = (INT)wParam;
  LPRECT lprc = (LPRECT)lParam;

  if ((nWorkAreas < 0) || (nWorkAreas > LV_MAX_WORKAREAS)) return FALSE;

  COMCTL32_Free(infoPtr->rcWorkAreas);
  if ((nWorkAreas == 0) || !lprc)
  {
    infoPtr->nWorkAreas = 0;
    infoPtr->rcWorkAreas = NULL;
  } else
  {
    infoPtr->nWorkAreas = nWorkAreas;
    infoPtr->rcWorkAreas = (LPRECT)COMCTL32_Alloc(nWorkAreas*sizeof(RECT));
    for (INT x = 0;x < nWorkAreas;x++)
     infoPtr->rcWorkAreas[x] = lprc[x];
  }

  return TRUE;
}


/***
 * DESCRIPTION:
 * Retrieves the origin coordinates when in icon or small icon display mode.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [O] LPPOINT : coordinate information
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_GetOrigin(HWND hwnd, LPPOINT lpptOrigin)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
  {
    if (infoPtr->dwStyle & WS_HSCROLL)
      lpptOrigin->x = -infoPtr->lefttop.x*infoPtr->scrollStep.x;
    else
      lpptOrigin->x = 0;

    if (infoPtr->dwStyle & WS_VSCROLL)
      lpptOrigin->y = -infoPtr->lefttop.y*infoPtr->scrollStep.y;
    else
      lpptOrigin->y = 0;

    return TRUE;
  }

  return FALSE;
}

/***
 * DESCRIPTION:
 * Retrieves the number of items that are marked as selected.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Number of items selected.
 */
static LRESULT LISTVIEW_GetSelectedCount(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nSelectedCount = 0;

  return nSelectedCount = infoPtr->hdpaSelItems->nItemCount;
}

/***
 * DESCRIPTION:
 * Retrieves item index that marks the start of a multiple selection.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Index number or -1 if there is no selection mark.
 */
static LRESULT LISTVIEW_GetSelectionMark(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->nSelectionMark;
}

/***
 * DESCRIPTION:
 * Retrieves the width of a string.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 *   SUCCESS : string width (in pixels)
 *   FAILURE : zero
 */
static LRESULT LISTVIEW_GetStringWidth(HWND hwnd,HDC hdc,LPWSTR lpszText,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  HFONT hFont, hOldFont;
  SIZE stringSize;
  BOOL ownDC = FALSE;

  ZeroMemory(&stringSize,sizeof(SIZE));
  if (lpszText)
  {
    if (!hdc)
    {
      hFont = infoPtr->hFont ? infoPtr->hFont : infoPtr->hDefaultFont;
      hdc = GetDC(hwnd);
      hOldFont = SelectObject(hdc, hFont);
      ownDC = TRUE;
    }
    if (unicode)
      GetTextExtentPointW(hdc,lpszText,lstrlenW(lpszText),&stringSize);
    else
      GetTextExtentPointA(hdc,(LPSTR)lpszText,lstrlenA((LPSTR)lpszText),&stringSize);
    if (ownDC)
    {
      SelectObject(hdc, hOldFont);
      ReleaseDC(hwnd, hdc);
    }
  }

  return stringSize.cx;
}

/***
 * DESCRIPTION:
 * Retrieves the text backgound color.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * COLORREF associated with the the background.
 */
static LRESULT LISTVIEW_GetTextBkColor(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->clrTextBk;
}

/***
 * DESCRIPTION:
 * Retrieves the text color.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * COLORREF associated with the text.
 */
static LRESULT LISTVIEW_GetTextColor(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->clrText;
}

static BOOL LISTVIEW_InternalHitTestItem(HWND hwnd,LISTVIEW_INFO *infoPtr,INT nItem,LPLVHITTESTINFO lpHitTestInfo,BOOL checkSubItems)
{
  RECT rcItem;

  if (checkSubItems && (infoPtr->uView == LVS_REPORT))
  {
    INT nColumnCount = HEADER_GetItemCount(infoPtr->hwndHeader);
    INT xDiff = -infoPtr->lefttop.x*infoPtr->scrollStep.x;

    rcItem.top = infoPtr->rcList.top+(nItem-LISTVIEW_GetTopIndex(hwnd))*infoPtr->nItemHeight;
    rcItem.bottom = rcItem.top+infoPtr->nItemHeight;
    for (INT x = 0;x < nColumnCount;x++)
    {
      RECT rcColumn;

      HEADER_GetItemRect(infoPtr->hwndHeader,(WPARAM)x,(LPARAM)&rcColumn);
      rcItem.left = xDiff+REPORT_MARGINX+rcColumn.left;
      rcItem.right = xDiff+rcColumn.right-REPORT_MARGINX;
      if (PtInRect(&rcItem,lpHitTestInfo->pt))
      {
        rcItem.top = x;
        rcItem.left = LVIR_BOUNDS;
        if (LISTVIEW_GetSubItemRect(hwnd,nItem,&rcItem) && PtInRect(&rcItem,lpHitTestInfo->pt))
        {
          rcItem.top = x;
          rcItem.left = LVIR_ICON;
          if (LISTVIEW_GetSubItemRect(hwnd,nItem,&rcItem) && PtInRect(&rcItem,lpHitTestInfo->pt))
          {
            lpHitTestInfo->flags = LVHT_ONITEMICON;
            lpHitTestInfo->iItem = nItem;
            lpHitTestInfo->iSubItem = x;
            return TRUE;
          }

          lpHitTestInfo->flags = LVHT_ONITEMLABEL;
          lpHitTestInfo->iItem = nItem;
          lpHitTestInfo->iSubItem = x;

          return TRUE;
        }
      }
    }
    return FALSE;
  }

  if (LISTVIEW_GetItemRect(hwnd,nItem,&rcItem,LVIR_BOUNDS))
  {
    if (PtInRect(&rcItem,lpHitTestInfo->pt))
    {
      if (LISTVIEW_GetItemRect(hwnd,nItem,&rcItem,LVIR_ICON) && PtInRect(&rcItem,lpHitTestInfo->pt))
      {
        lpHitTestInfo->flags = LVHT_ONITEMICON;
        lpHitTestInfo->iItem = nItem;
        lpHitTestInfo->iSubItem = 0;
        return TRUE;
      }

      if (LISTVIEW_GetItemRect(hwnd,nItem,&rcItem,LVIR_LABEL) && PtInRect(&rcItem,lpHitTestInfo->pt))
      {
        lpHitTestInfo->flags = LVHT_ONITEMLABEL;
        lpHitTestInfo->iItem = nItem;
        lpHitTestInfo->iSubItem = 0;
        return TRUE;
      }

      lpHitTestInfo->flags = LVHT_ONITEMSTATEICON;
      lpHitTestInfo->iItem = nItem;
      lpHitTestInfo->iSubItem = 0;
      return TRUE;
    }
  }

  return FALSE;
}

/***
 * DESCRIPTION:
 * Determines which section of the item was selected (if any).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [IO] LPLVHITTESTINFO : hit test information
 *
 * RETURN:
 *   SUCCESS : item index
 *   FAILURE : -1
 */
static INT LISTVIEW_HitTestItem(HWND hwnd, LPLVHITTESTINFO lpHitTestInfo)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  RECT rcItem;

  if (!lpHitTestInfo) return -1;

  if (infoPtr->uView == LVS_REPORT)
  {
    INT nTop = LISTVIEW_GetTopIndex(hwnd),nItem;

    nItem = nTop+((lpHitTestInfo->pt.y-infoPtr->rcList.top)/infoPtr->nItemHeight);
    if (nItem >= GETITEMCOUNT(infoPtr)) return -1;

    if (LISTVIEW_InternalHitTestItem(hwnd,infoPtr,nItem,lpHitTestInfo,FALSE))
      return nItem;
  } else if (infoPtr->uView == LVS_LIST)
  {
    INT nTop = LISTVIEW_GetTopIndex(hwnd),nItem;
    INT nCountPerColumn = LISTVIEW_GetCountPerColumn(hwnd);

    nItem = nTop+(lpHitTestInfo->pt.y/infoPtr->nItemHeight)+
            ((INT)(lpHitTestInfo->pt.x/infoPtr->nItemWidth))*nCountPerColumn;
    if (nItem >= GETITEMCOUNT(infoPtr)) return -1;

    if (LISTVIEW_InternalHitTestItem(hwnd,infoPtr,nItem,lpHitTestInfo,FALSE))
      return nItem;
  } else
  {
    INT i;

    for (i = 0; i < GETITEMCOUNT(infoPtr); i++)
    {
      if (LISTVIEW_InternalHitTestItem(hwnd,infoPtr,i,lpHitTestInfo,FALSE))
        return i;
    }
  }

  lpHitTestInfo->flags = LVHT_NOWHERE;

  return -1;
}

/***
 * DESCRIPTION:
 * Determines which listview item is located at the specified position.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [IO} LPLVHITTESTINFO : hit test information
 *
 * RETURN:
 *   SUCCESS : item index
 *   FAILURE : -1
 */
static LRESULT LISTVIEW_HitTest(HWND hwnd, LPLVHITTESTINFO lpHitTestInfo)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nItem = -1;

  lpHitTestInfo->flags = 0;

  if (infoPtr->rcList.left > lpHitTestInfo->pt.x)
  {
    lpHitTestInfo->flags = LVHT_TOLEFT;
  } else if (infoPtr->rcList.right < lpHitTestInfo->pt.x)
  {
    lpHitTestInfo->flags = LVHT_TORIGHT;
  }
  if (infoPtr->rcList.top > lpHitTestInfo->pt.y)
  {
    lpHitTestInfo->flags |= LVHT_ABOVE;
  } else if (infoPtr->rcList.bottom < lpHitTestInfo->pt.y)
  {
    lpHitTestInfo->flags |= LVHT_BELOW;
  }

  if (lpHitTestInfo->flags == 0)
  {
    nItem = LISTVIEW_HitTestItem(hwnd, lpHitTestInfo);
  }

  return nItem;
}

static LRESULT LISTVIEW_SubItemHitTest(HWND hwnd,LPLVHITTESTINFO lpHitTestInfo)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT nItem = -1;
  INT nSubItem = -1;

  lpHitTestInfo->flags = 0;
  if (infoPtr->uView != LVS_REPORT) return -1;

  if (infoPtr->rcList.left > lpHitTestInfo->pt.x)
  {
    lpHitTestInfo->flags = LVHT_TOLEFT;
  } else if (infoPtr->rcList.right < lpHitTestInfo->pt.x)
  {
    lpHitTestInfo->flags = LVHT_TORIGHT;
  }
  if (infoPtr->rcList.top > lpHitTestInfo->pt.y)
  {
    lpHitTestInfo->flags |= LVHT_ABOVE;
  } else if (infoPtr->rcList.bottom < lpHitTestInfo->pt.y)
  {
    lpHitTestInfo->flags |= LVHT_BELOW;
  }

  if (lpHitTestInfo->flags == 0)
  {
    INT nTop = LISTVIEW_GetTopIndex(hwnd),nItem;

    nItem = nTop+((lpHitTestInfo->pt.y-infoPtr->rcList.top)/infoPtr->nItemHeight);
    if (nItem >= GETITEMCOUNT(infoPtr)) return -1;

    if (LISTVIEW_InternalHitTestItem(hwnd,infoPtr,nItem,lpHitTestInfo,TRUE))
      return lpHitTestInfo->iSubItem;
  }

  return nSubItem;
}


/***
 * DESCRIPTION:
 * Inserts a new column.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : column index
 * [I] LPLVCOLUMNA : column information
 *
 * RETURN:
 *   SUCCESS : new column index
 *   FAILURE : -1
 */
static LRESULT LISTVIEW_InsertColumn(HWND hwnd,INT nColumn,LPLVCOLUMNW lpColumn,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  HDITEMW hdi;
  INT nNewColumn = -1;

  if (lpColumn != NULL)
  {
    /* initialize memory */
    ZeroMemory(&hdi, sizeof(HDITEMW));

    if (lpColumn->mask & LVCF_FMT)
    {
      /* format member is valid */
      hdi.mask |= HDI_FORMAT;

      /* set text alignment (leftmost column must be left-aligned) */
      if (nColumn == 0)
      {
        hdi.fmt |= HDF_LEFT;
      } else
      {
        if (lpColumn->fmt & LVCFMT_LEFT)
          hdi.fmt |= HDF_LEFT;
        else if (lpColumn->fmt & LVCFMT_RIGHT)
          hdi.fmt |= HDF_RIGHT;
        else if (lpColumn->fmt & LVCFMT_CENTER)
          hdi.fmt |= HDF_CENTER;
      }

      if (lpColumn->fmt & LVCFMT_BITMAP_ON_RIGHT)
      {
        hdi.fmt |= HDF_BITMAP_ON_RIGHT;
      }

      if (lpColumn->fmt & LVCFMT_COL_HAS_IMAGES)
      {
        hdi.fmt |= HDF_IMAGE;
      }

      if (lpColumn->fmt & LVCFMT_IMAGE)
      {
        hdi.fmt |= HDF_IMAGE;
        hdi.iImage = I_IMAGECALLBACK;
      }
    }

    if (lpColumn->mask & LVCF_WIDTH)
    {
      hdi.mask |= HDI_WIDTH;
      hdi.cxy = lpColumn->cx;
    }

    if (lpColumn->mask & LVCF_TEXT)
    {
      hdi.mask |= HDI_TEXT | HDI_FORMAT;
      hdi.pszText = lpColumn->pszText;
      hdi.cchTextMax = unicode ? lstrlenW(lpColumn->pszText):lstrlenA((LPSTR)lpColumn->pszText);
      hdi.fmt |= HDF_STRING;
    }

    if (lpColumn->mask & LVCF_IMAGE)
    {
      hdi.mask |= HDI_IMAGE;
      hdi.iImage = lpColumn->iImage;
    }

    if (lpColumn->mask & LVCF_ORDER)
    {
      hdi.mask |= HDI_ORDER;
      hdi.iOrder = lpColumn->iOrder;
    }

    /* insert item in header control */
    nNewColumn = HEADER_InsertItem(infoPtr->hwndHeader,(WPARAM)nColumn,(LPARAM)&hdi,unicode);

    /* Need to reset the item width when inserting a new column */
    infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);

    LISTVIEW_UpdateScroll(hwnd);
    LISTVIEW_Refresh(hwnd);
  }

  return nNewColumn;
}

/* LISTVIEW_InsertCompare:  callback routine for comparing pszText members of the LV_ITEMS
   in a LISTVIEW on insert.  Passed to DPA_Sort in LISTVIEW_InsertItem.
   This function should only be used for inserting items into a sorted list (LVM_INSERTITEM)
   and not during the processing of a LVM_SORTITEMS message. Applications should provide
   their own sort proc. when sending LVM_SORTITEMS.
*/
/* Platform SDK:
    (remarks on LVITEM: LVM_INSERTITEM will insert the new item in the proper sort postion...
        if:
          LVS_SORTXXX must be specified,
          LVS_OWNERDRAW is not set,
          <item>.pszText is not LPSTR_TEXTCALLBACK.

    (LVS_SORT* flags): "For the LVS_SORTASCENDING... styles, item indices
    are sorted based on item text..."
*/
static INT WINAPI LISTVIEW_InsertCompare(LPVOID first,LPVOID second,LPARAM lParam)
{
  LISTVIEW_ITEM* lv_first = (LISTVIEW_ITEM*)first;
  LISTVIEW_ITEM* lv_second = (LISTVIEW_ITEM*)second;
  LISTVIEW_ITEMDATA* lv_firstData = (LISTVIEW_ITEMDATA*)DPA_GetPtr(lv_first->hdpaSubItems,0);
  LISTVIEW_ITEMDATA* lv_secondData = (LISTVIEW_ITEMDATA*)DPA_GetPtr(lv_second->hdpaSubItems,0);
  LONG dwStyle = GetWindowLongA((HWND)lParam,GWL_STYLE);
  INT  cmpv = lstrcmpiW(lv_firstData->pszText,lv_secondData->pszText);

  /* if we're sorting descending, negate the return value */
  return (dwStyle & LVS_SORTDESCENDING) ? -cmpv : cmpv;
}

/***
 * DESCRIPTION:
 * Inserts a new item in the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] LPLVITEMW : item information
 *
 * RETURN:
 *   SUCCESS : new item index
 *   FAILURE : -1
 */
static LRESULT LISTVIEW_InsertItem(HWND hwnd, LPLVITEMW lpLVItem,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  NMLISTVIEW nmlv;
  INT nItem = -1;
  INT nItemWidth = 0;
  LISTVIEW_ITEM *lpItem = NULL;
  LISTVIEW_ITEMDATA *lpItemData = NULL;

  if (lpLVItem)
  {
    /* make sure it's not a subitem; cannot insert a subitem */
    if (lpLVItem->iSubItem == 0)
    {
      lpItem = (LISTVIEW_ITEM*)COMCTL32_Alloc(sizeof(LISTVIEW_ITEM));
      lpItemData = (LISTVIEW_ITEMDATA*)COMCTL32_Alloc(sizeof(LISTVIEW_ITEMDATA));
      if (lpItem && lpItemData)
      {
        ZeroMemory(lpItem,sizeof(LISTVIEW_ITEM));
        ZeroMemory(lpItemData,sizeof(LISTVIEW_ITEMDATA));
        if (LISTVIEW_InitItem(hwnd,lpItem,lpLVItem,unicode) && LISTVIEW_InitItemData(hwnd,lpItemData,lpLVItem,unicode))
        {
          /* insert item in listview control data structure */
          lpItem->hdpaSubItems = DPA_Create(8);
          if (lpItem->hdpaSubItems)
          {
            nItem = DPA_InsertPtr(lpItem->hdpaSubItems,0,lpItemData);
            if (nItem != -1)
            {
              if ( ((infoPtr->dwStyle & LVS_SORTASCENDING) || (infoPtr->dwStyle & LVS_SORTDESCENDING))
                      && !(infoPtr->dwStyle & LVS_OWNERDRAWFIXED)
                      && (LPSTR_TEXTCALLBACKW != lpLVItem->pszText) )
              {
                /* Insert the item in the proper sort order based on the pszText
                  member. See comments for LISTVIEW_InsertCompare() for greater detail */
                nItem = DPA_InsertPtrSorted(infoPtr->hdpaItems,lpItem,LISTVIEW_InsertCompare,hwnd);
              } else
              {
                nItem = DPA_InsertPtr(infoPtr->hdpaItems,lpLVItem->iItem,lpItem);
              }

              if (nItem != -1)
              {
                /* manage item focus */
                if (lpLVItem->mask & LVIF_STATE)
                {
                  lpItem->state &= ~(LVIS_FOCUSED|LVIS_SELECTED);
                  if (lpLVItem->stateMask & LVIS_SELECTED)
                  {
                    LISTVIEW_SetSelection(hwnd, nItem);
                  }
                  else if (lpLVItem->stateMask & LVIS_FOCUSED)
                  {
                    LISTVIEW_SetItemFocus(hwnd, nItem);
                  }
                }

                /* send LVN_INSERTITEM notification */
                ZeroMemory(&nmlv, sizeof(NMLISTVIEW));
                nmlv.iItem = nItem;
                nmlv.lParam = lpItem->lParam;;
                sendNotify(hwnd,LVN_INSERTITEM,&nmlv.hdr);

                if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_LIST))
                {
                  nItemWidth = LISTVIEW_CalculateWidth(hwnd, lpLVItem->iItem);
                  if (nItemWidth > infoPtr->nItemWidth)
                  {
                    infoPtr->nItemWidth = nItemWidth;
                  }
                }

                /* align items (set position of each item) */
                if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
                {
                  if (infoPtr->dwStyle & LVS_ALIGNLEFT)
                  {
                    LISTVIEW_AlignLeft(hwnd);
                  }
                  else
                  {
                    LISTVIEW_AlignTop(hwnd);
                  }
                }

                /* refresh client area */
                LISTVIEW_UpdateScroll(hwnd);
                LISTVIEW_Refresh(hwnd);
              }
            }
          }
        }
      }
    }
  }

  /* free memory if unsuccessful */
  if (nItem == -1)
  {
    if (lpItem) COMCTL32_Free(lpItem);
    if (lpItemData) COMCTL32_Free(lpItemData);
  }

  return nItem;
}

/***
 * DESCRIPTION:
 * Redraws a range of items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : first item
 * [I] INT : last item
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_RedrawItems(HWND hwnd, INT nFirst, INT nLast)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (nFirst <= nLast)
  {
    if ((nFirst >= 0) && (nFirst < GETITEMCOUNT(infoPtr)))
    {
      if ((nLast >= 0) && (nLast < GETITEMCOUNT(infoPtr)))
      {
        if ((nFirst == 0) && (nLast == GETITEMCOUNT(infoPtr)))
        {
          InvalidateRect(hwnd,NULL,TRUE);

          return TRUE;
        }

        if (nFirst == nLast)
          LISTVIEW_RefreshItem(hwnd,nFirst,TRUE);
        else
          LISTVIEW_RefreshItems(hwnd,nFirst,nLast);

        return TRUE;
      }
    }
  }

  return FALSE;
}

static LRESULT LISTVIEW_Scroll(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT dx = (INT)wParam,dy = (INT)lParam,maxX,maxY;
  POINT oldlefttop = infoPtr->lefttop;

  //x units: (icon, report -> pixels; list -> columns)
  if (infoPtr->uView != LVS_LIST)
  {
    dx /= infoPtr->scrollStep.x;
  }

  //y units: (icon, list -> pixels; report -> lines)
  if (infoPtr->uView == LVS_LIST)
  {
    //no vertical scrolling (so far)
    dy = 0;
  } else if ((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON))
  {
    dy /= infoPtr->scrollStep.y;
  }

  infoPtr->lefttop.x += dx;
  infoPtr->lefttop.y += dy;
  maxX = infoPtr->maxScroll.x-infoPtr->scrollPage.x;
  maxY = infoPtr->maxScroll.y-infoPtr->scrollPage.y;

  if (infoPtr->lefttop.x > maxX)
    infoPtr->lefttop.x = maxX;

  if (infoPtr->lefttop.x < 0)
    infoPtr->lefttop.x = 0;

  if (infoPtr->lefttop.y > maxY)
    infoPtr->lefttop.y = maxY;

  if (infoPtr->lefttop.y < 0)
    infoPtr->lefttop.y = 0;

  if ((oldlefttop.x != infoPtr->lefttop.x) || (oldlefttop.y != infoPtr->lefttop.y))
  {
    SCROLLINFO scrollInfo;
    RECT rect;
    INT xScroll = (oldlefttop.x-infoPtr->lefttop.x)*infoPtr->scrollStep.x;
    INT yScroll = (oldlefttop.y-infoPtr->lefttop.y)*infoPtr->scrollStep.y;

    if (xScroll != 0)
    {
      ZeroMemory(&scrollInfo, sizeof(SCROLLINFO));
      scrollInfo.cbSize = sizeof(SCROLLINFO);
      scrollInfo.nPos   = infoPtr->lefttop.x;
      scrollInfo.fMask  = SIF_POS;
      SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,TRUE);
    }

    if (yScroll != 0)
    {
      ZeroMemory(&scrollInfo, sizeof(SCROLLINFO));
      scrollInfo.cbSize = sizeof(SCROLLINFO);
      scrollInfo.nPos   = infoPtr->lefttop.y;
      scrollInfo.fMask  = SIF_POS;
      SetScrollInfo(hwnd,SB_VERT,&scrollInfo,TRUE);
    }

    LISTVIEW_ScrollWindow(hwnd,xScroll,yScroll);

    return TRUE;
  }

  return FALSE;
}

/***
 * DESCRIPTION:
 * Sets the background color.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] COLORREF : background color
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetBkColor(HWND hwnd, COLORREF clrBk)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (infoPtr->clrBk != clrBk)
  {
    infoPtr->clrBk = clrBk;
    LISTVIEW_Refresh(hwnd);
  }

  return TRUE;
}

static LRESULT LISTVIEW_SetBkImage(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
  LPLVBKIMAGEW plvbki = (LPLVBKIMAGEW)lParam;

  if (!plvbki) return FALSE;

  //todo: use OLE COM interface

  return TRUE;
}

/***
 * DESCRIPTION:
 * Sets the callback mask. This mask will be used when the parent
 * window stores state information (some or all).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] UINT : state mask
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_SetCallbackMask(HWND hwnd, UINT uMask)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  infoPtr->uCallbackMask = uMask;

  return TRUE;
}

/***
 * DESCRIPTION:
 * Sets the attributes of a header item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : column index
 * [I] LPLVCOLUMNA : column attributes
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetColumn(HWND hwnd,INT nColumn,LPLVCOLUMNW lpColumn,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  HDITEMW hdi, hdiget;

  if ((lpColumn != NULL) && (nColumn >= 0) &&
      (nColumn < HEADER_GetItemCount(infoPtr->hwndHeader)))
  {
    /* initialize memory */
    ZeroMemory(&hdi, sizeof(HDITEMA));

    if (lpColumn->mask & LVCF_FMT)
    {
      /* format member is valid */
      hdi.mask |= HDI_FORMAT;

      /* get current format first */
      hdiget.mask = HDI_FORMAT;
      if (HEADER_GetItem(infoPtr->hwndHeader,(WPARAM)nColumn,(LPARAM)&hdiget,TRUE))
              /* preserve HDF_STRING if present */
              hdi.fmt = hdiget.fmt & HDF_STRING;

      /* set text alignment (leftmost column must be left-aligned) */
      if (nColumn == 0)
      {
        hdi.fmt |= HDF_LEFT;
      }
      else
      {
        if (lpColumn->fmt & LVCFMT_LEFT)
        {
          hdi.fmt |= HDF_LEFT;
        }
        else if (lpColumn->fmt & LVCFMT_RIGHT)
        {
          hdi.fmt |= HDF_RIGHT;
        }
        else if (lpColumn->fmt & LVCFMT_CENTER)
        {
          hdi.fmt |= HDF_CENTER;
        }
      }

      if (lpColumn->fmt & LVCFMT_BITMAP_ON_RIGHT)
      {
        hdi.fmt |= HDF_BITMAP_ON_RIGHT;
      }

      if (lpColumn->fmt & LVCFMT_COL_HAS_IMAGES)
      {
        hdi.fmt |= HDF_IMAGE;
      }

      if (lpColumn->fmt & LVCFMT_IMAGE)
      {
        hdi.fmt |= HDF_IMAGE;
        hdi.iImage = I_IMAGECALLBACK;
      }
    }

    if (lpColumn->mask & LVCF_WIDTH)
    {
      hdi.mask |= HDI_WIDTH;
      hdi.cxy = lpColumn->cx;
    }

    if (lpColumn->mask & LVCF_TEXT)
    {
      hdi.mask |= HDI_TEXT | HDI_FORMAT;
      hdi.pszText = lpColumn->pszText;
      hdi.cchTextMax = unicode ? lstrlenW(lpColumn->pszText):lstrlenA((LPSTR)lpColumn->pszText);
      hdi.fmt |= HDF_STRING;
    }

    if (lpColumn->mask & LVCF_IMAGE)
    {
      hdi.mask |= HDI_IMAGE;
      hdi.iImage = lpColumn->iImage;
    }

    if (lpColumn->mask & LVCF_ORDER)
    {
      hdi.mask |= HDI_ORDER;
      hdi.iOrder = lpColumn->iOrder;
    }

    /* set header item attributes */
    bResult = HEADER_SetItem(infoPtr->hwndHeader,(WPARAM)nColumn,(LPARAM)&hdi,unicode);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Sets the column order array
 *
 * PARAMETERS:
 * [I] HWND : window handle
 * [I] INT : number of elements in column order array
 * [I] INT : pointer to column order array
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetColumnOrderArray(HWND hwnd,INT iCount,LPINT lpiArray)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LPINT oldArray,diffList;
  INT nHeaderItemCount;
  BOOL rc,changed = FALSE;

  nHeaderItemCount = HEADER_GetItemCount(infoPtr->hwndHeader);
  if (iCount != nHeaderItemCount) return FALSE;

  oldArray = (LPINT)COMCTL32_Alloc(nHeaderItemCount*sizeof(INT));
  HEADER_GetOrderArray(infoPtr->hwndHeader,(WPARAM)nHeaderItemCount,(LPARAM)oldArray);
  rc = HEADER_SetOrderArray(infoPtr->hwndHeader,(WPARAM)iCount,(LPARAM)lpiArray);
  if (rc)
  {
    INT x,y;

    //build offset list
    diffList = (LPINT)COMCTL32_Alloc(nHeaderItemCount*sizeof(INT));
    for (x = 0;x < nHeaderItemCount;x++)
      for (y = 0;y < nHeaderItemCount;y++)
      {
        if (lpiArray[x] == oldArray[y])
        {
          diffList[x] = y;
          if (x != y) changed = TRUE;
          break;
        }
      }

    if (changed)
    {
      //change order
      for (x = 0;x < GETITEMCOUNT(infoPtr);x++)
      {
        LISTVIEW_ITEM *lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,x);
        HDPA newDPA;

        if (!lpItem) continue;
        newDPA = DPA_Create(8);
        for (y = 0;y < nHeaderItemCount;y++)
        {
          LISTVIEW_ITEMDATA *lpItemData;

          lpItemData = LISTVIEW_GetItemData(lpItem->hdpaSubItems,diffList[y]);
          if (!lpItemData) continue;
          lpItemData->iSubItem = y;
          DPA_InsertPtr(newDPA,newDPA->nItemCount,lpItemData);
        }
        DPA_Destroy(lpItem->hdpaSubItems);
        lpItem->hdpaSubItems = newDPA;
      }
      if (diffList) COMCTL32_Free(diffList);

      LISTVIEW_Refresh(hwnd);
    }
  }
  if (oldArray) COMCTL32_Free(oldArray);

  return rc;
}

/***
 * DESCRIPTION:
 * Sets the width of a column
 *
 * PARAMETERS:
 * [I] HWND : window handle
 * [I] INT : column index
 * [I] INT : column width
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetColumnWidth(HWND hwnd, INT iCol, INT cx)
{
    LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
    HDITEMA hdi;
    LRESULT lret;

    // make sure we can get the listview info
    if (!infoPtr)
      return (FALSE);

    // set column width only if in report mode
    if (infoPtr->uView != LVS_REPORT)
      return (FALSE);

    if (!infoPtr->hwndHeader) // make sure we have a header
      return (FALSE);

    if (cx == LVSCW_AUTOSIZE_USEHEADER)
    {
      cx = HEADER_GetExpandedColumnTextWidth(infoPtr->hwndHeader,iCol);
    } else if (cx == LVSCW_AUTOSIZE)
    {
      INT nItemWidth,i;

      cx = HEADER_GetExpandedColumnTextWidth(infoPtr->hwndHeader,iCol);

      for (i = 0;i < GETITEMCOUNT(infoPtr);i++)
      {
        nItemWidth = LISTVIEW_GetLabelWidth(hwnd,i,iCol)+2*REPORT_MARGINX;
        if (nItemWidth > cx) cx = nItemWidth;
      }
    }

    if (cx < 0)
        return (FALSE);

    hdi.mask = HDI_WIDTH;
    hdi.cxy = cx;

    // call header to update the column change
    lret = HEADER_SetItem(infoPtr->hwndHeader,(WPARAM)iCol,(LPARAM)&hdi,TRUE);

    infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);

    LISTVIEW_Refresh(hwnd); // force redraw of the listview

    return lret;
}

/***
 * DESCRIPTION:
 * Sets the extended listview style.
 *
 * PARAMETERS:
 * [I] HWND  : window handle
 * [I] DWORD : mask
 * [I] DWORD : style
 *
 * RETURN:
 *   SUCCESS : previous style
 *   FAILURE : 0
 */
static LRESULT LISTVIEW_SetExtendedListViewStyle(HWND hwnd, DWORD dwMask, DWORD dwStyle)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  DWORD dwOldStyle;

  /* store previous style */
  dwOldStyle = infoPtr->dwExStyle;

  /* set new style */
  infoPtr->dwExStyle = (dwOldStyle & ~dwMask) | (dwStyle & dwMask);

  //update report view
  if ((((dwOldStyle & LVS_EX_GRIDLINES) != (infoPtr->dwExStyle & LVS_EX_GRIDLINES)) ||
       ((dwOldStyle & LVS_EX_SUBITEMIMAGES) != (infoPtr->dwExStyle & LVS_EX_SUBITEMIMAGES))) &&
      (infoPtr->uView == LVS_REPORT))
  {
    LISTVIEW_Refresh(hwnd);
  }

  //update header
  if ((dwOldStyle & LVS_EX_HEADERDRAGDROP) != (infoPtr->dwExStyle & LVS_EX_HEADERDRAGDROP))
  {
    if (infoPtr->hwndHeader)
    {
      DWORD dwStyle = GetWindowLongA(infoPtr->hwndHeader,GWL_STYLE);

      if (infoPtr->dwExStyle & LVS_EX_HEADERDRAGDROP)
        dwStyle |= HDS_DRAGDROP;
      else
        dwStyle &= ~HDS_DRAGDROP;
      SetWindowLongA(infoPtr->hwndHeader,GWL_STYLE,dwStyle);
    }
  }

  return dwOldStyle;
}

static LRESULT LISTVIEW_SetHotCursor(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  HCURSOR hOldCursor = infoPtr->hHotCursor;

  infoPtr->hHotCursor = (HCURSOR)lParam;

  return hOldCursor;
}

/***
 * DESCRIPTION:
 * Sets the hot item index.
 *
 * PARAMETERS:
 * [I] HWND  : window handle
 * [I] INT   : index
 *
 * RETURN:
 *   SUCCESS : previous hot item index
 *   FAILURE : -1 (no hot item)
 */
static LRESULT LISTVIEW_SetHotItem(HWND hwnd, INT iIndex)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT iOldIndex;

  /* store previous index */
  iOldIndex = infoPtr->nHotItem;
  if (iOldIndex != infoPtr->nHotItem)
  {
    HDC hdc = 0;

    /* set new style */
    infoPtr->nHotItem = iIndex;

    if (iOldIndex != -1)
    {
      RECT rect;

      if (!hdc) hdc = GetDC(hwnd);
      LISTVIEW_GetItemRect(hwnd,iOldIndex,&rect,LVIR_LABEL);
      LISTVIEW_DrawHottrackLine(hdc,&rect);
    }
    if (iIndex != -1)
    {
      RECT rect;

      if (!hdc) hdc = GetDC(hwnd);
      LISTVIEW_GetItemRect(hwnd,iIndex,&rect,LVIR_LABEL);
      LISTVIEW_DrawHottrackLine(hdc,&rect);
    }
    if (hdc) ReleaseDC(hwnd,hdc);
  }

  return (iOldIndex);
}

static LRESULT LISTVIEW_SetIconSpacing(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  DWORD oldIS = MAKELONG(infoPtr->iconSpacing.cx,infoPtr->iconSpacing.cy);
  INT cx = LOWORD(lParam),cy = HIWORD(lParam);

  if (cx == -1) cx = GetSystemMetrics(SM_CXICONSPACING);
  if (cy == -1) cy = GetSystemMetrics(SM_CYICONSPACING);
  if ((cx != infoPtr->iconSpacing.cx) || (cy != infoPtr->iconSpacing.cy))
  {
    infoPtr->iconSpacing.cx = cx;
    infoPtr->iconSpacing.cy = cy;
    if (((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON)) && (infoPtr->dwStyle & LVS_AUTOARRANGE))
    {
      LISTVIEW_Arrange(hwnd,LVA_DEFAULT);
    }
  }

  return oldIS;
}

/***
 * DESCRIPTION:
 * Sets image lists.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : image list type
 * [I] HIMAGELIST : image list handle
 *
 * RETURN:
 *   SUCCESS : old image list
 *   FAILURE : NULL
 */
static LRESULT LISTVIEW_SetImageList(HWND hwnd, INT nType, HIMAGELIST himl)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  HIMAGELIST himlTemp = 0;

  switch (nType)
  {
    case LVSIL_NORMAL:
      himlTemp = infoPtr->himlNormal;
      infoPtr->himlNormal = himl;
      return (LRESULT)himlTemp;

    case LVSIL_SMALL:
      himlTemp = infoPtr->himlSmall;
      infoPtr->himlSmall = himl;
      return (LRESULT)himlTemp;

    case LVSIL_STATE:
      himlTemp = infoPtr->himlState;
      infoPtr->himlState = himl;
      ImageList_SetBkColor(infoPtr->himlState, CLR_NONE);
      return (LRESULT)himlTemp;
  }

  return (LRESULT)NULL;
}

/***
 * DESCRIPTION:
 * Sets the attributes of an item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] LPLVITEM : item information
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetItem(HWND hwnd,LPLVITEMW lpLVItem,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  NMLISTVIEW nmlv;

  if (lpLVItem)
  {
    if ((lpLVItem->iItem >= 0) && (lpLVItem->iItem < GETITEMCOUNT(infoPtr)))
    {
      LISTVIEW_ITEM *lpItem;

      lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,lpLVItem->iItem);
      if (lpItem)
      {
        LISTVIEW_ITEMDATA *lpItemData;

        lpItemData = LISTVIEW_GetItemData(lpItem->hdpaSubItems,lpLVItem->iSubItem);

        if (lpItemData)
        {
          UINT uChanged;

          ZeroMemory(&nmlv, sizeof(NMLISTVIEW));
          nmlv.lParam = lpItem->lParam;
          uChanged = LISTVIEW_GetItemChanges(lpItem,lpItemData,lpLVItem,unicode);
          if (uChanged != 0)
          {
            if (uChanged & LVIF_STATE)
            {
              nmlv.uNewState = lpLVItem->state & lpLVItem->stateMask;
              nmlv.uOldState = lpItem->state & lpLVItem->stateMask;
            }

            nmlv.uChanged = uChanged;
            nmlv.iItem = lpLVItem->iItem;
            nmlv.lParam = lpItem->lParam;
            /* send LVN_ITEMCHANGING notification */
            sendNotify(hwnd,LVN_ITEMCHANGING,&nmlv.hdr);

            /* copy information */
            if (uChanged & LVIF_STATE)
            {
              if ((nmlv.uNewState & LVIS_SELECTED) != (nmlv.uOldState & LVIS_SELECTED))
              {
                if (nmlv.uOldState & LVIS_SELECTED)
                  DPA_DeletePtr(infoPtr->hdpaSelItems,DPA_GetPtrIndex(infoPtr->hdpaSelItems,lpItem));
                else
                  DPA_InsertPtr(infoPtr->hdpaSelItems,0,lpItem);
              }
            }
            bResult = LISTVIEW_InitItem(hwnd,lpItem,lpLVItem,unicode) && LISTVIEW_InitItemData(hwnd,lpItemData,lpLVItem,unicode);

            /* send LVN_ITEMCHANGED notification */
            sendNotify(hwnd,LVN_ITEMCHANGED,&nmlv.hdr);

            if ((uChanged & LVIF_TEXT) && ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_LIST)))
            {
              INT nItemWidth = LISTVIEW_CalculateWidth(hwnd,lpLVItem->iItem);

              if (nItemWidth > infoPtr->nItemWidth)
              {
                infoPtr->nItemWidth = nItemWidth;
                LISTVIEW_Refresh(hwnd);

                return bResult;
              }
            }

            if (!lpLVItem->iSubItem)
              LISTVIEW_RefreshItem(hwnd,lpLVItem->iItem,TRUE);
            else
              LISTVIEW_RefreshSubItem(hwnd,lpLVItem->iItem,lpLVItem->iSubItem);
          } else
              bResult = TRUE;
        } else
        {
          /* set subitem only if column is present */
          if (HEADER_GetItemCount(infoPtr->hwndHeader) > lpLVItem->iSubItem)
          {
            lpItemData = (LISTVIEW_ITEMDATA*)COMCTL32_Alloc(sizeof(LISTVIEW_ITEMDATA));
            if (lpItemData)
            {
              ZeroMemory(lpItemData,sizeof(LISTVIEW_ITEMDATA));
              if (LISTVIEW_InitItem(hwnd,lpItem,lpLVItem,unicode) && LISTVIEW_InitItemData(hwnd,lpItemData,lpLVItem,unicode))
              {
                INT nPosition = LISTVIEW_FindInsertPosition(lpItem->hdpaSubItems,lpItemData->iSubItem);
                INT nItem = DPA_InsertPtr(lpItem->hdpaSubItems,nPosition,lpItemData);

                if (nItem != -1)
                {
                  bResult = TRUE;
                  LISTVIEW_RefreshSubItem(hwnd,lpLVItem->iItem,lpLVItem->iSubItem);
                }
              }
            }
            if (!bResult)
            {
              if (lpItemData) COMCTL32_Free(lpItemData);
            }
          }
        }
      }
    }
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Preallocates memory.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item count (projected number of items)
 * [I] DWORD : update flags
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_SetItemCount(HWND hwnd, INT nItems, DWORD dwFlags)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);


  if (nItems == 0)
    return LISTVIEW_DeleteAllItems (hwnd);

  if (infoPtr->dwStyle & LVS_OWNERDATA)
  {
    //FIXME("LVS_OWNERDATA is set!\n");
  } else
  {
    if (nItems > GETITEMCOUNT(infoPtr))
    {
      /* append items */

      //CB: todo: what does the Windows version?
    } else if (nItems < GETITEMCOUNT(infoPtr))
    {
      /* remove items */

      //CB: todo: what does the Windows version?
    }
  }

  return TRUE;
}

/***
 * DESCRIPTION:
 * Sets the position of an item.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [I] INT : x coordinate
 * [I] INT : y coordinate
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_SetItemPosition(HWND hwnd,INT nItem,INT nPosX,INT nPosY,BOOL ignoreAutoArrange)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;

  if ((nItem >= 0) || (nItem < GETITEMCOUNT(infoPtr)))
  {
    if ((infoPtr->uView == LVS_ICON) || (infoPtr->uView == LVS_SMALLICON))
    {
      LISTVIEW_ITEM *lpItem;

      lpItem = (LISTVIEW_ITEM*)DPA_GetPtr(infoPtr->hdpaItems,nItem);
      if (lpItem)
      {
        bResult = TRUE;

        if ((lpItem->ptPosition.x == nPosX) && (lpItem->ptPosition.y == nPosY))
          return bResult;

        if (infoPtr->dwStyle & LVS_AUTOARRANGE)
        {
          lpItem->ptPosition.x = nPosX;
          lpItem->ptPosition.y = nPosY;

          if (!ignoreAutoArrange) LISTVIEW_Arrange(hwnd,LVA_DEFAULT);
        } else
        {
          POINT point;
          RECT rect1,rect2;

          LISTVIEW_GetItemRect(hwnd,nItem,&rect1,LVIR_BOUNDS);
          lpItem->ptPosition.x = nPosX;
          lpItem->ptPosition.y = nPosY;
          LISTVIEW_GetItemRect(hwnd,nItem,&rect2,LVIR_BOUNDS);
          InvalidateRect(hwnd,&rect1,TRUE);
          InvalidateRect(hwnd,&rect2,TRUE);
        }
      }
    }
  }

  return bResult;
}

static BOOL LISTVIEW_SetItemPosition32(HWND hwnd,INT nItem,LPPOINT lpptNewPos)
{
  if (!lpptNewPos) return FALSE;

  return LISTVIEW_SetItemPosition(hwnd,nItem,lpptNewPos->x,lpptNewPos->y,FALSE);
}

//replaces ListView_SetItemState macro
static LRESULT LISTVIEW_SetItemState(HWND hwnd,INT nItem,DWORD data,DWORD mask)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LVITEMW lvItem;
  BOOL bResult;

  ZeroMemory(&lvItem, sizeof(LVITEMW));
  lvItem.mask = LVIF_STATE;
  lvItem.stateMask = mask;
  lvItem.state = data;

  if (nItem == -1)
  {
    bResult = TRUE;

    /* apply to all items */
    for (INT i = 0; i< GETITEMCOUNT(infoPtr); i++)
    {
      lvItem.iItem = i;
      if (!LISTVIEW_SetItem(hwnd,&lvItem,TRUE))
        bResult = FALSE;
    }
  } else
  {
    lvItem.iItem = nItem;
    bResult = LISTVIEW_SetItem(hwnd,&lvItem,TRUE);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Sets the state of one or many items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I]INT : item index
 * [I] LPLVITEM : item or subitem info
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetItemState(HWND hwnd,INT nItem,LPLVITEMW lpLVItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  LVITEMW lvItem;
  INT i;

  ZeroMemory(&lvItem, sizeof(LVITEMW));
  lvItem.mask = LVIF_STATE;
  lvItem.state = lpLVItem->state;
  lvItem.stateMask = lpLVItem->stateMask;

  if (nItem == -1)
  {
    bResult = TRUE;

    /* apply to all items */
    for (i = 0; i< GETITEMCOUNT(infoPtr); i++)
    {
      lvItem.iItem = i;
      if (!LISTVIEW_SetItem(hwnd,&lvItem,TRUE))
        bResult = FALSE;
    }
  } else
  {
    lvItem.iItem = nItem;
    bResult = LISTVIEW_SetItem(hwnd,&lvItem,TRUE);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Sets the text of an item or subitem.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 * [I] LPLVITEMW : item or subitem info
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static BOOL LISTVIEW_SetItemText(HWND hwnd, INT nItem, LPLVITEMW lpLVItem,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  LVITEMW lvItem;

  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    ZeroMemory(&lvItem, sizeof(LVITEMW));
    lvItem.mask = LVIF_TEXT;
    lvItem.pszText = lpLVItem->pszText;
    lvItem.iItem = nItem;
    lvItem.iSubItem = lpLVItem->iSubItem;
    bResult = LISTVIEW_SetItem(hwnd,&lvItem,unicode);
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Set item index that marks the start of a multiple selection.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT  : index
 *
 * RETURN:
 * Index number or -1 if there is no selection mark.
 */
static LRESULT LISTVIEW_SetSelectionMark(HWND hwnd, INT nIndex)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  INT nOldIndex = infoPtr->nSelectionMark;

  infoPtr->nSelectionMark = nIndex;

  return nOldIndex;
}

/***
 * DESCRIPTION:
 * Sets the text background color.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] COLORREF : text background color
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetTextBkColor(HWND hwnd, COLORREF clrTextBk)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (infoPtr->clrTextBk != clrTextBk)
  {
    infoPtr->clrTextBk = clrTextBk;
    LISTVIEW_Refresh(hwnd);
  }

  return TRUE;
}

/***
 * DESCRIPTION:
 * Sets the text foreground color.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] COLORREF : text color
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SetTextColor (HWND hwnd, COLORREF clrText)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (infoPtr->clrText != clrText)
  {
    infoPtr->clrText = clrText;
    LISTVIEW_Refresh(hwnd);
  }

  return TRUE;
}

/* LISTVIEW_SetToolTips */
/* LISTVIEW_SetUnicodeFormat */

/***
 * DESCRIPTION:
 * Callback internally used by LISTVIEW_SortItems()
 *
 * PARAMETER(S):
 * [I] LPVOID : first LISTVIEW_ITEM to compare
 * [I] LPVOID : second LISTVIEW_ITEM to compare
 * [I] LPARAM : HWND of control
 *
 * RETURN:
 *   if first comes before second : negative
 *   if first comes after second : positive
 *   if first and second are equivalent : zero
 */
static INT WINAPI LISTVIEW_CallBackCompare(
  LPVOID first,
  LPVOID second,
  LPARAM lParam)
{
  /* Forward the call to the client defined callback */
  INT rv;
  HWND hwnd = (HWND)lParam;
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (first == second)
    return 0;

  rv = (infoPtr->pfnCompare)( ((LISTVIEW_ITEM*) first)->lParam,
          ((LISTVIEW_ITEM*) second)->lParam, infoPtr->lParamSort );

  return rv;
}

/***
 * DESCRIPTION:
 * Sorts the listview items.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WPARAM : application-defined value
 * [I] LPARAM : pointer to comparision callback
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_SortItems(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
    LISTVIEW_ITEM *lpItem;
    int nCount, i;

    if (!infoPtr || !infoPtr->hdpaItems)
        return FALSE;

    nCount = GETITEMCOUNT(infoPtr);
    // if there are 0 or 1 items, there is no need to sort
    if (nCount > 1)
    {
      infoPtr->pfnCompare = (PFNLVCOMPARE)lParam;
      infoPtr->lParamSort = (LPARAM)wParam;

      // sort the sortList
      DPA_Sort(infoPtr->hdpaItems,LISTVIEW_CallBackCompare,hwnd);

      LISTVIEW_Refresh(hwnd);
    }

    return TRUE;
}

/* LISTVIEW_SubItemHitTest */

/***
 * DESCRIPTION:
 * Updates an items or rearranges the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : item index
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_Update(HWND hwnd, INT nItem)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult = FALSE;
  RECT rc;

  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    bResult = TRUE;

    /* rearrange with default alignment style */
    if ((infoPtr->dwStyle & LVS_AUTOARRANGE) && ((infoPtr->uView == LVS_ICON) ||
        (infoPtr->uView  == LVS_SMALLICON)))
    {
      LISTVIEW_Arrange(hwnd, 0);
    }
    else
    {
      /* get item bounding rectangle */

      LISTVIEW_GetItemRect(hwnd,nItem,&rc,LVIR_BOUNDS);
      InvalidateRect(hwnd,&rc,TRUE);
    }
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Creates the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_Create(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LPCREATESTRUCTA lpcs = (LPCREATESTRUCTA)lParam;
  UINT uView = lpcs->style & LVS_TYPEMASK;
  LOGFONTA logFont;
  DWORD headerStyle = WS_CHILD | HDS_HORZ;

  /* initialize color information  */
  infoPtr->clrBk = GetSysColor(COLOR_WINDOW);
  infoPtr->clrText = GetSysColor(COLOR_WINDOWTEXT);
  infoPtr->clrTextBk = GetSysColor(COLOR_WINDOW);

  infoPtr->hHotCursor = LoadCursorA(0,IDC_ARROWA);

  /* set default values */
  infoPtr->dwStyle = lpcs->style;
  infoPtr->uView = uView;
  infoPtr->uCallbackMask = 0;
  infoPtr->nFocusedItem = -1;
  infoPtr->nSelectionMark = -1;
  infoPtr->nHotItem = -1;
  infoPtr->iconSpacing.cx = GetSystemMetrics(SM_CXICONSPACING);
  infoPtr->iconSpacing.cy = GetSystemMetrics(SM_CYICONSPACING);
  ZeroMemory(&infoPtr->rcList, sizeof(RECT));
  infoPtr->hwndEdit = 0;
  infoPtr->pedititem = NULL;
  infoPtr->bDoEditLabel = FALSE;
  infoPtr->hoverTime = DEFAULT_HOVERTIME;

  /* get default font (icon title) */
  SystemParametersInfoA(SPI_GETICONTITLELOGFONT, 0, &logFont, 0);
  infoPtr->hDefaultFont = CreateFontIndirectA(&logFont);
  infoPtr->hFont = infoPtr->hDefaultFont;

  /* create header */
  if (!(lpcs->style & LVS_NOSORTHEADER)) headerStyle |= HDS_BUTTONS;
  infoPtr->hwndHeader = CreateWindowA(WC_HEADERA, (LPCSTR)NULL,
                                      headerStyle,
                                      0, 0, 0, 0, hwnd, (HMENU)0,
                                      lpcs->hInstance, NULL);

  /* set header font */
  SendMessageA(infoPtr->hwndHeader,WM_SETFONT,(WPARAM)infoPtr->hFont,(LPARAM)TRUE);

  if (uView == LVS_ICON)
  {
    infoPtr->iconSize.cx = GetSystemMetrics(SM_CXICON);
    infoPtr->iconSize.cy = GetSystemMetrics(SM_CYICON);
  } else if (uView == LVS_REPORT)
  {
    if (!(LVS_NOCOLUMNHEADER & lpcs->style))
    {
      ShowWindow(infoPtr->hwndHeader, SW_SHOWNORMAL);
    }

    infoPtr->iconSize.cx = GetSystemMetrics(SM_CXSMICON);
    infoPtr->iconSize.cy = GetSystemMetrics(SM_CYSMICON);
  } else
  {
    infoPtr->iconSize.cx = GetSystemMetrics(SM_CXSMICON);
    infoPtr->iconSize.cy = GetSystemMetrics(SM_CYSMICON);
  }

  /* allocate memory for the data structure */
  infoPtr->hdpaItems = DPA_Create(64);
  infoPtr->hdpaSelItems = DPA_Create(10);

  /* initialize size of items */
  infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);
  infoPtr->nItemHeight = LISTVIEW_GetItemHeight(hwnd);

  //create tooltip
  infoPtr->hwndToolTip = createToolTip(hwnd,TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT,TRUE);
  SendMessageA(infoPtr->hwndToolTip,WM_SETFONT,infoPtr->hFont,0);


  return 0;
}

/***
 * DESCRIPTION:
 * Erases the background of the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WPARAM : device context handle
 * [I] LPARAM : not used
 *
 * RETURN:
 *   SUCCESS : TRUE
 *   FAILURE : FALSE
 */
static LRESULT LISTVIEW_EraseBackground(HWND hwnd, WPARAM wParam,
                                        LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  BOOL bResult;

  if (infoPtr->clrBk == CLR_NONE)
  {
    bResult = SendMessageA(GetParent(hwnd), WM_ERASEBKGND, wParam, lParam);
  }
  else
  {
    RECT rc;
    HBRUSH hBrush = CreateSolidBrush(infoPtr->clrBk);

    GetClientRect(hwnd, &rc);
    FillRect((HDC)wParam, &rc, hBrush);
    DeleteObject(hBrush);
    bResult = TRUE;
  }

  return bResult;
}

/***
 * DESCRIPTION:
 * Retrieves the listview control font.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Font handle.
 */
static LRESULT LISTVIEW_GetFont(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->hFont;
}

/***
 * DESCRIPTION:
 * Performs vertical scrolling.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : scroll code
 * [I] SHORT : current scroll position if scroll code is SB_THUMBPOSITION
 *             or SB_THUMBTRACK.
 * [I] HWND : scrollbar control window handle
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_VScroll(HWND hwnd,INT nScrollCode,SHORT nCurrentPos,HWND hScrollWnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT maxY = infoPtr->maxScroll.y-infoPtr->scrollPage.y,oldY = infoPtr->lefttop.y;

  if (infoPtr->hwndEdit)
  {
    SendMessageA(infoPtr->hwndEdit, WM_KILLFOCUS, 0, 0);
  }

  switch (nScrollCode)
  {
    case SB_LINEUP:
      if (infoPtr->lefttop.y > 0)
        infoPtr->lefttop.y--;
      break;

    case SB_LINEDOWN:
      if (infoPtr->lefttop.y < maxY)
        infoPtr->lefttop.y++;
      break;

    case SB_PAGEUP:
      if (infoPtr->lefttop.y > 0)
      {
        if (infoPtr->lefttop.y >= infoPtr->scrollPage.y)
          infoPtr->lefttop.y -= infoPtr->scrollPage.y;
        else
          infoPtr->lefttop.y = 0;
      }
      break;

    case SB_PAGEDOWN:
      if (infoPtr->lefttop.y < maxY)
      {
        if (infoPtr->lefttop.y <= maxY-infoPtr->scrollPage.y)
          infoPtr->lefttop.y += infoPtr->scrollPage.y;
        else
          infoPtr->lefttop.y = maxY;
      }
      break;

    case SB_THUMBTRACK:
      infoPtr->lefttop.y = nCurrentPos;

      if (infoPtr->lefttop.y > maxY)
        infoPtr->lefttop.y = maxY;

      if (infoPtr->lefttop.y < 0)
        infoPtr->lefttop.y = 0;
      break;
  }

  if (oldY != infoPtr->lefttop.y)
  {
    SCROLLINFO scrollInfo;
    RECT rect;
    INT yScroll = (oldY-infoPtr->lefttop.y)*infoPtr->scrollStep.y;

    ZeroMemory(&scrollInfo, sizeof(SCROLLINFO));
    scrollInfo.cbSize = sizeof(SCROLLINFO);
    scrollInfo.nPos   = infoPtr->lefttop.y;
    scrollInfo.fMask  = SIF_POS;
    SetScrollInfo(hwnd,SB_VERT,&scrollInfo,TRUE);

    LISTVIEW_ScrollWindow(hwnd,0,yScroll);

    return TRUE;
  }

  return FALSE;
}

/***
 * DESCRIPTION:
 * Performs horizontal scrolling.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : scroll code
 * [I] SHORT : current scroll position if scroll code is SB_THIMBPOSITION
 *             or SB_THUMBTRACK.
 * [I] HWND : scrollbar control window handle
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_HScroll(HWND hwnd,INT nScrollCode,SHORT nCurrentPos,HWND hScrollWnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT maxX = infoPtr->maxScroll.x-infoPtr->scrollPage.x,oldX = infoPtr->lefttop.x;

  if (infoPtr->hwndEdit)
  {
    SendMessageA(infoPtr->hwndEdit, WM_KILLFOCUS, 0, 0);
  }

  switch (nScrollCode)
  {
    case SB_LINELEFT:
      if (infoPtr->lefttop.x > 0)
        infoPtr->lefttop.x--;
      break;

    case SB_LINERIGHT:
      if (infoPtr->lefttop.x < maxX)
        infoPtr->lefttop.x++;
      break;

    case SB_PAGELEFT:
      if (infoPtr->lefttop.x > 0)
      {
        if (infoPtr->lefttop.x >= infoPtr->scrollPage.x)
          infoPtr->lefttop.x -= infoPtr->scrollPage.x;
        else
          infoPtr->lefttop.x = 0;
      }
      break;

    case SB_PAGERIGHT:
      if (infoPtr->lefttop.x < maxX)
      {
        if (infoPtr->lefttop.x <= maxX-infoPtr->scrollPage.x)
          infoPtr->lefttop.x += infoPtr->scrollPage.x;
        else
          infoPtr->lefttop.x = maxX;
      }
      break;

    case SB_THUMBTRACK:
      infoPtr->lefttop.x = nCurrentPos;

      if (infoPtr->lefttop.x > maxX)
        infoPtr->lefttop.x = maxX;

      if (infoPtr->lefttop.x < 0)
        infoPtr->lefttop.x = 0;
      break;
  }

  if (oldX != infoPtr->lefttop.x)
  {
    SCROLLINFO scrollInfo;
    RECT rect;
    INT xScroll = (oldX-infoPtr->lefttop.x)*infoPtr->scrollStep.x;

    ZeroMemory(&scrollInfo, sizeof(SCROLLINFO));
    scrollInfo.cbSize = sizeof(SCROLLINFO);
    scrollInfo.nPos   = infoPtr->lefttop.x;
    scrollInfo.fMask  = SIF_POS;
    SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,TRUE);

    LISTVIEW_ScrollWindow(hwnd,xScroll,0);
  }

  return 0;
}

static LRESULT LISTVIEW_MouseWheel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
    LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
    INT gcWheelDelta = 0;
    UINT pulScrollLines = 3;
    INT wheelDelta = (short int)HIWORD(wParam);

   if (wParam & (MK_SHIFT | MK_CONTROL))
     return DefWindowProcA(hwnd,WM_MOUSEWHEEL,wParam,lParam);


    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES,0, &pulScrollLines, 0);
    gcWheelDelta -= wheelDelta;

    switch(infoPtr->uView)
    {
      case LVS_ICON:
      case LVS_SMALLICON:
       /*
        *  listview should be scrolled by a multiple of 37 dependently on its dimension or its visible item number
        *  should be fixed in the future.
        */
        if (infoPtr->dwStyle & WS_VSCROLL)
          LISTVIEW_VScroll(hwnd,SB_THUMBPOSITION,infoPtr->lefttop.y+(gcWheelDelta < 0) ? 37:-37,0);
        break;

      case LVS_REPORT:
        if (abs(gcWheelDelta) >= WHEEL_DELTA && pulScrollLines)
        {
          if (infoPtr->dwStyle & WS_VSCROLL)
          {
            int cLineScroll = min(LISTVIEW_GetCountPerColumn(hwnd), pulScrollLines);

            cLineScroll *= (gcWheelDelta / WHEEL_DELTA);
            LISTVIEW_VScroll(hwnd,SB_THUMBPOSITION,infoPtr->lefttop.y+cLineScroll,0);
          }
        }
        break;

      case LVS_LIST:
        LISTVIEW_HScroll(hwnd,(gcWheelDelta < 0) ? SB_LINELEFT:SB_LINERIGHT,0,0);
        break;
    }
    return 0;
}

/***
 * DESCRIPTION:
 * ???
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : virtual key
 * [I] LONG : key data
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_KeyDown(HWND hwnd, INT nVirtualKey, LONG lKeyData)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  NMLVKEYDOWN nmKeyDown;
  INT nItem = -1;
  BOOL bRedraw = FALSE;

  /* send LVN_KEYDOWN notification */
  ZeroMemory(&nmKeyDown, sizeof(NMLVKEYDOWN));
  nmKeyDown.wVKey = nVirtualKey;
  nmKeyDown.flags = 0;
  sendNotify(hwnd,LVN_KEYDOWN,&nmKeyDown.hdr);

  switch (nVirtualKey)
  {
  case VK_RETURN:
    if ((GETITEMCOUNT(infoPtr) > 0) && (infoPtr->nFocusedItem != -1))
    {
      /* send NM_RETURN notification */
      sendNotify(hwnd,NM_RETURN);

      /* send LVN_ITEMACTIVATE notification */
      sendNotify(hwnd,LVN_ITEMACTIVATE);
    }
    break;

  case VK_HOME:
    if (GETITEMCOUNT(infoPtr) > 0)
    {
      nItem = 0;
    }
    break;

  case VK_END:
    if (GETITEMCOUNT(infoPtr) > 0)
    {
      nItem = GETITEMCOUNT(infoPtr) - 1;
    }
    break;

  case VK_LEFT:
    nItem = LISTVIEW_GetNextItem(hwnd, infoPtr->nFocusedItem, LVNI_TOLEFT);
    break;

  case VK_UP:
    nItem = LISTVIEW_GetNextItem(hwnd, infoPtr->nFocusedItem, LVNI_ABOVE);
    break;

  case VK_RIGHT:
    nItem = LISTVIEW_GetNextItem(hwnd, infoPtr->nFocusedItem, LVNI_TORIGHT);
    break;

  case VK_DOWN:
    nItem = LISTVIEW_GetNextItem(hwnd, infoPtr->nFocusedItem, LVNI_BELOW);
    break;

  case VK_NEXT:
    if (infoPtr->uView == LVS_REPORT)
    {
      nItem = infoPtr->nFocusedItem + LISTVIEW_GetCountPerColumn(hwnd);
    }
    else
    {
      nItem = infoPtr->nFocusedItem + LISTVIEW_GetCountPerColumn(hwnd) * LISTVIEW_GetCountPerRow(hwnd);
    }
    if(nItem >= GETITEMCOUNT(infoPtr)) nItem = GETITEMCOUNT(infoPtr) - 1;
    break;

  case VK_PRIOR:
    if (infoPtr->uView == LVS_REPORT)
    {
      nItem = infoPtr->nFocusedItem - LISTVIEW_GetCountPerColumn(hwnd);
    }
    else
    {
      nItem = infoPtr->nFocusedItem - LISTVIEW_GetCountPerColumn(hwnd) * LISTVIEW_GetCountPerRow(hwnd);
    }
    if(nItem < 0) nItem = 0;
    break;
  }

  if ((nItem != -1) && (nItem != infoPtr->nFocusedItem))
    LISTVIEW_KeySelection(hwnd, nItem);

  return 0;
}

static BOOL LISTVIEW_Compare(HWND hwnd,INT nItem,LPWSTR text,INT textlen,BOOL *matchLast)
{
  BOOL res;
  INT itemlen;
  LVINTERNALITEMW lvItem;

  ZeroMemory(&lvItem,sizeof(LVITEMW));
  lvItem.header.mask = LVIF_TEXT;
  lvItem.header.iItem = nItem;
  lvItem.header.cchTextMax = DISP_TEXT_SIZE;
  lvItem.mustFree = FALSE;
  LISTVIEW_GetItem(hwnd,(LPLVITEMW)&lvItem,TRUE,TRUE);

  itemlen = lstrlenAW(lvItem.header.pszText,lvItem.unicode);
  if (itemlen < textlen)
  {
    res = FALSE;
  } else
  {
    res = (lstrcmpniAW(lvItem.header.pszText,lvItem.unicode,text,TRUE,textlen) == 0);
  }
  if (!res && matchLast)
  {
    textlen--;
    if ((textlen > 0) && (itemlen >= textlen))
      *matchLast = (lstrcmpniAW(lvItem.header.pszText,lvItem.unicode,text,TRUE,textlen) == 0);
    else
      *matchLast = FALSE;
  }
  if (lvItem.mustFree) COMCTL32_Free(lvItem.header.pszText);

  return res;
}

static INT LISTVIEW_Search(HWND hwnd,LISTVIEW_INFO *infoPtr,INT iItem,LPWSTR text,INT textlen,INT *nearest)
{
  INT start,item;
  BOOL bMatchLast;

  start = iItem;
  if (nearest) *nearest = -1;

  //search start to end
  for (item = start;item < GETITEMCOUNT(infoPtr);item++)
  {
    if (nearest)
    {
      if (LISTVIEW_Compare(hwnd,item,text,textlen,(*nearest == -1) ? &bMatchLast:NULL))
        return item;
      else if ((*nearest == -1) && bMatchLast)
        *nearest = item;
    } else
    {
      if (LISTVIEW_Compare(hwnd,item,text,textlen,NULL))
        return item;
    }
  }

  //search first to start
  for (item = 0;item < start;item++)
  {
    if (nearest)
    {
      if (LISTVIEW_Compare(hwnd,item,text,textlen,(*nearest == -1) ? &bMatchLast:NULL))
        return item;
      else if ((*nearest == -1) && bMatchLast)
        *nearest = item;
    } else
    {
      if (LISTVIEW_Compare(hwnd,item,text,textlen,NULL))
        return item;
    }
  }

  return -1;
}

//NOTE: sister function in treeview control -> sync changes

static VOID LISTVIEW_ISearch(HWND hwnd,CHAR ch)
{
  LISTVIEW_INFO *infoPtr = LISTVIEW_GetInfoPtr(hwnd);
  LPWSTR newString;
  INT len,item,nearest = -1;
  CHAR ch2[2];
  DWORD dwISearchTime;
  BOOL checkNearest = TRUE;

  //check timer
  dwISearchTime = GetTickCount();
  if ((infoPtr->uISearchLen > 0) && (TICKDIFF(infoPtr->dwISearchTime,dwISearchTime) > LV_ISEARCH_DELAY))
  {
    COMCTL32_Free(infoPtr->pszISearch);
    infoPtr->pszISearch = NULL;
    infoPtr->uISearchLen = 0;
  }

  //prepare new search string
  len = infoPtr->uISearchLen+1;
  newString = (WCHAR*)COMCTL32_Alloc((len+1)*sizeof(WCHAR));

  if (infoPtr->uISearchLen > 0) lstrcpyW(newString,infoPtr->pszISearch);

  ch2[0] = ch;
  ch2[1] = 0;
  lstrcpyAtoW((LPWSTR)&newString[len-1],(LPSTR)&ch2);
  for (INT x = 1;x < len;x++)
  {
    if (newString[0] != newString[x])
    {
      checkNearest = FALSE;
      break;
    }
  }

  //search

  //start with selected item or root
  if (infoPtr->nFocusedItem != -1)
  {
    item = infoPtr->nFocusedItem;

    //check if new string is valid for old selection
    if (LISTVIEW_Compare(hwnd,item,newString,len,NULL))
      goto ISearchDone;

    //no match, continue with next item
    item++;
  } else item = 0;

  //scan
  item = LISTVIEW_Search(hwnd,infoPtr,item,newString,len,checkNearest ? &nearest:NULL);

  if ((item == -1) && (nearest != -1))
  {
    LISTVIEW_SetSelection(hwnd,nearest);
    LISTVIEW_EnsureVisible(hwnd,nearest,FALSE);
    infoPtr->dwISearchTime = GetTickCount();

    COMCTL32_Free(newString);
    return;
  }

ISearchDone:
  //done
  if (item != -1)
  {
    COMCTL32_Free(infoPtr->pszISearch);
    infoPtr->pszISearch = newString;
    infoPtr->uISearchLen = len;
    LISTVIEW_SetSelection(hwnd,item);
    LISTVIEW_EnsureVisible(hwnd,item,FALSE);
    infoPtr->dwISearchTime = GetTickCount();
  } else
  {
    COMCTL32_Free(newString);
    MessageBeep(0xFFFFFFFF);
  }
}

static LRESULT LISTVIEW_Char(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  CHAR ch = (CHAR)wParam;

  LISTVIEW_ISearch(hwnd,ch);

  return 0;
}

/***
 * DESCRIPTION:
 * Kills the focus.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_KillFocus(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  /* send NM_KILLFOCUS notification */
  sendNotify(hwnd,NM_KILLFOCUS);

  /* set window focus flag */
  infoPtr->bFocus = FALSE;

  /* redraw the selected items */
  if (infoPtr->nFocusedItem != -1)
    LISTVIEW_RefreshItem(hwnd,infoPtr->nFocusedItem,FALSE);

  return 0;
}

/***
 * DESCRIPTION:
 * Processes double click messages (left mouse button).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WORD : key flag
 * [I] WORD : x coordinate
 * [I] WORD : y coordinate
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_LButtonDblClk(HWND hwnd, WORD wKey, WORD wPosX,
                                      WORD wPosY)
{
  LVHITTESTINFO htInfo;

  /* send NM_DBLCLK notification */
  sendNotify(hwnd,NM_DBLCLK);

  /* To send the LVN_ITEMACTIVATE, it must be on an Item */
  ZeroMemory(&htInfo, sizeof(LVHITTESTINFO));
  htInfo.pt.x = wPosX;
  htInfo.pt.y = wPosY;
  if(LISTVIEW_HitTest(hwnd, &htInfo) != -1)
  {
    /* send LVN_ITEMACTIVATE notification */
    sendNotify(hwnd,LVN_ITEMACTIVATE);
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Processes mouse down messages (left mouse button).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WORD : key flag
 * [I] WORD : x coordinate
 * [I] WORD : y coordinate
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_LButtonDown(HWND hwnd, WORD wKey, WORD wPosX, WORD wPosY)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  static BOOL bGroupSelect = TRUE;
  POINT ptPosition;
  INT nItem;

  /* send NM_RELEASEDCAPTURE notification */
  sendNotify(hwnd,NM_RELEASEDCAPTURE);

  if (!infoPtr->bFocus)
    SetFocus(hwnd);

  /* set left button down flag */
  infoPtr->bLButtonDown = TRUE;

  ptPosition.x = wPosX;
  ptPosition.y = wPosY;
  nItem = LISTVIEW_MouseSelection(hwnd, ptPosition);
  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    if (infoPtr->dwStyle & LVS_SINGLESEL)
    {
      if (LISTVIEW_IsItemSelected(infoPtr,nItem) && !infoPtr->bDoEditLabel)
        infoPtr->bDoEditLabel = TRUE;
      else
        LISTVIEW_SetSelection(hwnd, nItem);
    }
    else
    {
      if ((wKey & MK_CONTROL) && (wKey & MK_SHIFT))
      {
        if (bGroupSelect)
        {
          LISTVIEW_AddGroupSelection(hwnd, nItem);
        }
        else
        {
          LISTVIEW_AddSelection(hwnd, nItem);
        }
      }
      else if (wKey & MK_CONTROL)
      {
        bGroupSelect = LISTVIEW_ToggleSelection(hwnd, nItem);
      }
      else  if (wKey & MK_SHIFT)
      {
        LISTVIEW_SetGroupSelection(hwnd, nItem);
      }
      else
      {
        if (LISTVIEW_IsItemSelected(infoPtr,nItem) && !infoPtr->bDoEditLabel)
          infoPtr->bDoEditLabel = TRUE;
        else
          LISTVIEW_SetSelection(hwnd, nItem);
      }
    }
  }
  else
  {
    /* remove all selections */
    LISTVIEW_RemoveSelections(hwnd, 0, GETITEMCOUNT(infoPtr));
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Processes mouse up messages (left mouse button).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WORD : key flag
 * [I] WORD : x coordinate
 * [I] WORD : y coordinate
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_LButtonUp(HWND hwnd, WORD wKey, WORD wPosX,
                                  WORD wPosY)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

//  TRACE("(hwnd=%x,key=%hu,X=%hu,Y=%hu)\n", hwnd, wKey, wPosX, wPosY);

  if (infoPtr->bLButtonDown != FALSE)
  {
    /* send NM_CLICK notification */
    sendNotify(hwnd,NM_CLICK);

    /* set left button flag */
    infoPtr->bLButtonDown = FALSE;

    if(infoPtr->bDoEditLabel)
    {
      LVHITTESTINFO htInfo;
      int nItem;
      ZeroMemory(&htInfo, sizeof(LVHITTESTINFO));
      htInfo.pt.x = wPosX;
      htInfo.pt.y = wPosY;
      nItem = LISTVIEW_HitTest(hwnd, &htInfo);
      if(nItem != -1 && htInfo.flags == LVHT_ONITEMLABEL)
      {
        LISTVIEW_EditLabel(hwnd,nItem,TRUE);
      }
      infoPtr->bDoEditLabel = FALSE;
    }
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Creates the listview control (called before WM_CREATE).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WPARAM : unhandled
 * [I] LPARAM : widow creation info
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_NCCreate(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr;

  /* allocate memory for info structure */
  infoPtr = (LISTVIEW_INFO*)initControl(hwnd,sizeof(LISTVIEW_INFO));
  if (!infoPtr) return 0;

  return DefWindowProcA(hwnd,WM_NCCREATE,wParam,lParam);
}

/***
 * DESCRIPTION:
 * Destroys the listview control (called after WM_DESTROY).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_NCDestroy(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  //destroy tooltip
  destroyToolTip(infoPtr->hwndToolTip);

  /* delete all items */
  LISTVIEW_DeleteAllItems(hwnd);

  /* destroy data structure */
  DPA_Destroy(infoPtr->hdpaItems);
  DPA_Destroy(infoPtr->hdpaSelItems);

  /* destroy font */
  infoPtr->hFont = (HFONT)0;
  if (infoPtr->hDefaultFont)
  {
    DeleteObject(infoPtr->hDefaultFont);
  }

  //destroy image lists
  if (!(infoPtr->dwStyle & LVS_SHAREIMAGELISTS))
  {
    if (infoPtr->himlNormal) ImageList_Destroy(infoPtr->himlNormal);
    if (infoPtr->himlSmall) ImageList_Destroy(infoPtr->himlSmall);
    if (infoPtr->himlState) ImageList_Destroy(infoPtr->himlState);
  }

  /* free listview info pointer*/
  doneControl(hwnd);

  return 0;
}

/***
 * DESCRIPTION:
 * Handles notifications from children.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] INT : control identifier
 * [I] LPNMHDR : notification information
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_Notify(HWND hwnd, INT nCtrlId, LPNMHDR lpnmh)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (lpnmh->hwndFrom == infoPtr->hwndHeader)
  {
    /* handle notification from header control */
    if ((lpnmh->code == HDN_ITEMCHANGEDA) || (lpnmh->code == HDN_ITEMCHANGEDW))
    {
      INT width;

      width = LISTVIEW_GetItemWidth(hwnd);
      if (width != infoPtr->nItemWidth)
      {
        infoPtr->nItemWidth = width;
        LISTVIEW_UpdateScroll(hwnd);
        LISTVIEW_Refresh(hwnd);
      }
    } else if((lpnmh->code ==  HDN_ITEMCLICKA) || (lpnmh->code ==  HDN_ITEMCLICKW))
    {
      /* Handle sorting by Header Column */
      NMLISTVIEW nmlv;
      LPNMHEADERA pnmHeader = (LPNMHEADERA)lpnmh;

      ZeroMemory(&nmlv, sizeof(NMLISTVIEW));
      nmlv.iItem = -1;
      nmlv.iSubItem = pnmHeader->iItem;

      sendNotify(hwnd,LVN_COLUMNCLICK,&nmlv.hdr);
    }
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Paints/Repaints the listview control.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HDC : device context handle
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_Paint(HWND hwnd, HDC hdc)
{
  if (!hdc)
  {
    PAINTSTRUCT ps;

    hdc = BeginPaint(hwnd,&ps);
    LISTVIEW_Draw(hwnd, hdc,&ps.rcPaint);
    EndPaint(hwnd,&ps);
  } else
      LISTVIEW_Draw(hwnd,hdc,NULL);

  return 0;
}

/***
 * DESCRIPTION:
 * Processes double click messages (right mouse button).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WORD : key flag
 * [I] WORD : x coordinate
 * [I] WORD : y coordinate
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_RButtonDblClk(HWND hwnd, WORD wKey, WORD wPosX,
                                      WORD wPosY)
{
//  TRACE("(hwnd=%x,key=%hu,X=%hu,Y=%hu)\n", hwnd, wKey, wPosX, wPosY);

  /* send NM_RELEASEDCAPTURE notification */
  sendNotify(hwnd,NM_RELEASEDCAPTURE);

  /* send NM_RDBLCLK notification */
  sendNotify(hwnd,NM_RDBLCLK);

  return 0;
}

/***
 * DESCRIPTION:
 * Processes mouse down messages (right mouse button).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WORD : key flag
 * [I] WORD : x coordinate
 * [I] WORD : y coordinate
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_RButtonDown(HWND hwnd, WORD wKey, WORD wPosX,
                                    WORD wPosY)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);
  POINT ptPosition;
  INT nItem;

//  TRACE("(hwnd=%x,key=%hu,X=%hu,Y=%hu)\n", hwnd, wKey, wPosX, wPosY);

  /* send NM_RELEASEDCAPTURE notification */
  sendNotify(hwnd,NM_RELEASEDCAPTURE);

  /* make sure the listview control window has the focus */
  if (infoPtr->bFocus == FALSE)
  {
    SetFocus(hwnd);
  }

  /* set right button down flag */
  infoPtr->bRButtonDown = TRUE;

  /* determine the index of the selected item */
  ptPosition.x = wPosX;
  ptPosition.y = wPosY;
  nItem = LISTVIEW_MouseSelection(hwnd, ptPosition);
  if ((nItem >= 0) && (nItem < GETITEMCOUNT(infoPtr)))
  {
    if (!((wKey & MK_SHIFT) || (wKey & MK_CONTROL)))
    {
      LISTVIEW_SetSelection(hwnd, nItem);
    }
  }
  else
  {
    LISTVIEW_RemoveSelections(hwnd, 0, GETITEMCOUNT(infoPtr));
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Processes mouse up messages (right mouse button).
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WORD : key flag
 * [I] WORD : x coordinate
 * [I] WORD : y coordinate
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_RButtonUp(HWND hwnd, WORD wKey, WORD wPosX,
                                  WORD wPosY)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (infoPtr->bRButtonDown != FALSE)
  {
    LPARAM lParam;

    /* send NM_RClICK notification */
    sendNotify(hwnd,NM_RCLICK);

    /* set button flag */
    infoPtr->bRButtonDown = FALSE;

    lParam = MAKELPARAM(wPosX, wPosY);
    ClientToScreen(hwnd, (LPPOINT)&lParam);

    SendMessageA( hwnd, WM_CONTEXTMENU, (WPARAM) hwnd, lParam);
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Sets the focus.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HWND : window handle of previously focused window
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_SetFocus(HWND hwnd, HWND hwndLoseFocus)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  /* send NM_SETFOCUS notification */
  sendNotify(hwnd,NM_SETFOCUS);

  /* set window focus flag */
  infoPtr->bFocus = TRUE;

  if (infoPtr->nFocusedItem != -1)
    LISTVIEW_RefreshItem(hwnd,infoPtr->nFocusedItem,FALSE);

  return 0;
}

/***
 * DESCRIPTION:
 * Sets the font.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] HFONT : font handle
 * [I] WORD : redraw flag
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_SetFont(HWND hwnd, HFONT hFont, WORD fRedraw)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

  if (hFont == 0)
  {
    if (infoPtr->hFont == infoPtr->hDefaultFont) return 0;
    infoPtr->hFont = infoPtr->hDefaultFont;
  } else
  {
    if (infoPtr->hFont == hFont) return 0;
    infoPtr->hFont = hFont;
  }

  if (infoPtr->uView == LVS_REPORT)
  {
    /* set header font */
    SendMessageA(infoPtr->hwndHeader,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(fRedraw,0));
  }

  //set tooltip font
  SendMessageA(infoPtr->hwndToolTip,WM_SETFONT,(WPARAM)hFont,fRedraw);

  /* invalidate listview control client area */
  LISTVIEW_Refresh(hwnd);

  if (fRedraw)
    UpdateWindow(hwnd);

  return 0;
}

/***
 * DESCRIPTION:
 * Message handling for WM_SETREDRAW.
 * For the Listview, it invalidates the entire window (the doc specifies otherwise)
 *
 * PARAMETER(S):
 * [I] HWND   : window handle
 * [I] bRedraw: state of redraw flag
 *
 * RETURN:
 * DefWinProc return value
 */
static LRESULT LISTVIEW_SetRedraw(HWND hwnd,BOOL bRedraw)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  LRESULT lResult;

  if (bRedraw)
  {
    if (!(infoPtr->internalFlags & IF_NOREDRAW)) return 0;
    infoPtr->internalFlags &= ~IF_NOREDRAW;
    lResult = DefWindowProcA(hwnd, WM_SETREDRAW, bRedraw, 0);

    LISTVIEW_UpdateScroll(hwnd);
    RedrawWindow(hwnd,NULL,0,RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_ALLCHILDREN | RDW_ERASENOW);
  } else
  {
    infoPtr->internalFlags |= IF_NOREDRAW;
    lResult = DefWindowProcA(hwnd, WM_SETREDRAW, bRedraw, 0);
  }

  return lResult;
}

/***
 * DESCRIPTION:
 * Resizes the listview control. This function processes WM_SIZE
 * messages.  At this time, the width and height are not used.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WORD : new width
 * [I] WORD : new height
 *
 * RETURN:
 * Zero
 */
static LRESULT LISTVIEW_Size(HWND hwnd, int Width, int Height)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  LISTVIEW_UpdateSize(hwnd);
  LISTVIEW_UpdateScroll(hwnd);

  if ((infoPtr->uView == LVS_SMALLICON) || (infoPtr->uView == LVS_ICON))
  {
    if (infoPtr->dwStyle & LVS_ALIGNLEFT)
      LISTVIEW_AlignLeft(hwnd);
    else
      LISTVIEW_AlignTop(hwnd);
  }
  return 0;
}

/***
 * DESCRIPTION:
 * Sets the size information.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 *
 * RETURN:
 * Zero
 */
static VOID LISTVIEW_UpdateSize(HWND hwnd)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  RECT rcList;

  GetClientRect(hwnd, &rcList);
  infoPtr->rcList.left = 0;
  infoPtr->rcList.right = max(rcList.right - rcList.left, 1);
  infoPtr->rcList.top = 0;
  infoPtr->rcList.bottom = max(rcList.bottom - rcList.top, 1);

  if (infoPtr->uView == LVS_LIST)
  {
    if (!(infoPtr->dwStyle & WS_HSCROLL))
    {
      INT nHScrollHeight = GetSystemMetrics(SM_CYHSCROLL);
      if (infoPtr->rcList.bottom > nHScrollHeight)
      {
        infoPtr->rcList.bottom -= nHScrollHeight;
      }
    }
  } else if (infoPtr->uView == LVS_REPORT)
  {
    if (!(infoPtr->dwStyle & LVS_NOCOLUMNHEADER))
    {
      HDLAYOUT hl;
      WINDOWPOS wp;

      hl.prc = &rcList;
      hl.pwpos = &wp;
      HEADER_Layout(infoPtr->hwndHeader,(WPARAM)0,(LPARAM)&hl);
      SetWindowPos(infoPtr->hwndHeader,hwnd,wp.x,wp.y,wp.cx,wp.cy,wp.flags);
      infoPtr->rcList.top = max(wp.cy,0);
    }
  }
}

/***
 * DESCRIPTION:
 * Processes WM_STYLECHANGED messages.
 *
 * PARAMETER(S):
 * [I] HWND : window handle
 * [I] WPARAM : window style type (normal or extended)
 * [I] LPSTYLESTRUCT : window style information
 *
 * RETURN:
 * Zero
 */
static INT LISTVIEW_StyleChanged(HWND hwnd, WPARAM wStyleType,
                                 LPSTYLESTRUCT lpss)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  UINT uNewView = lpss->styleNew & LVS_TYPEMASK;
  UINT uOldView = lpss->styleOld & LVS_TYPEMASK;
  RECT rcList = infoPtr->rcList;

  if (wStyleType == GWL_STYLE)
  {
    infoPtr->dwStyle = lpss->styleNew;
    infoPtr->uView = lpss->styleNew & LVS_TYPEMASK;
    if (uOldView == LVS_REPORT)
    {
      ShowWindow(infoPtr->hwndHeader, SW_HIDE);
    }

    if ((lpss->styleOld & WS_HSCROLL) != 0)
    {
       ShowScrollBar(hwnd, SB_HORZ, FALSE);
    }

    if ((lpss->styleOld & WS_VSCROLL) != 0)
    {
       ShowScrollBar(hwnd, SB_VERT, FALSE);
    }

    if (uNewView == LVS_ICON)
    {
      infoPtr->iconSize.cx = GetSystemMetrics(SM_CXICON);
      infoPtr->iconSize.cy = GetSystemMetrics(SM_CYICON);
      infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);
      infoPtr->nItemHeight = LISTVIEW_GetItemHeight(hwnd);
      if (lpss->styleNew & LVS_ALIGNLEFT)
      {
        LISTVIEW_AlignLeft(hwnd);
      }
      else
      {
        LISTVIEW_AlignTop(hwnd);
      }
    }
    else if (uNewView == LVS_REPORT)
    {
      HDLAYOUT hl;
      WINDOWPOS wp;

      hl.prc = &rcList;
      hl.pwpos = &wp;
      HEADER_Layout(infoPtr->hwndHeader,(WPARAM)0,(LPARAM)&hl);
      SetWindowPos(infoPtr->hwndHeader, hwnd, wp.x, wp.y, wp.cx, wp.cy,
                   wp.flags);
      if (!(LVS_NOCOLUMNHEADER & lpss->styleNew))
      {
        ShowWindow(infoPtr->hwndHeader, SW_SHOWNORMAL);
      }
      infoPtr->iconSize.cx = GetSystemMetrics(SM_CXSMICON);
      infoPtr->iconSize.cy = GetSystemMetrics(SM_CYSMICON);
      infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);
      infoPtr->nItemHeight = LISTVIEW_GetItemHeight(hwnd);
    }
    else if (uNewView == LVS_LIST)
    {
      infoPtr->iconSize.cx = GetSystemMetrics(SM_CXSMICON);
      infoPtr->iconSize.cy = GetSystemMetrics(SM_CYSMICON);
      infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);
      infoPtr->nItemHeight = LISTVIEW_GetItemHeight(hwnd);
    }
    else
    {
      infoPtr->iconSize.cx = GetSystemMetrics(SM_CXSMICON);
      infoPtr->iconSize.cy = GetSystemMetrics(SM_CYSMICON);
      infoPtr->nItemWidth = LISTVIEW_GetItemWidth(hwnd);
      infoPtr->nItemHeight = LISTVIEW_GetItemHeight(hwnd);
      if (lpss->styleNew & LVS_ALIGNLEFT)
      {
        LISTVIEW_AlignLeft(hwnd);
      }
      else
      {
        LISTVIEW_AlignTop(hwnd);
      }
    }

    if ((lpss->styleOld & LVS_NOSORTHEADER) != (lpss->styleNew & LVS_NOSORTHEADER))
    {
      INT headerStyle = GetWindowLongA(infoPtr->hwndHeader,GWL_STYLE);

      if (lpss->styleNew & LVS_NOSORTHEADER)
        headerStyle &= ~HDS_BUTTONS;
      else
        headerStyle |= HDS_BUTTONS;
      SetWindowLongA(infoPtr->hwndHeader,GWL_STYLE,headerStyle);
    }

    /* update the size of the client area */
    LISTVIEW_UpdateSize(hwnd);

    /* add scrollbars if needed */
    LISTVIEW_UpdateScroll(hwnd);

    /* invalidate client area + erase background */
    LISTVIEW_Refresh(hwnd);
  }

  /* If they change the view and we have an active edit control
     we will need to kill the control since the redraw will
     misplace the edit control.
   */
  if (infoPtr->hwndEdit &&
        ((uNewView & (LVS_ICON|LVS_LIST|LVS_SMALLICON)) !=
        ((LVS_ICON|LVS_LIST|LVS_SMALLICON) & uOldView)))
  {
     SendMessageA(infoPtr->hwndEdit, WM_KILLFOCUS, 0, 0);
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Handle any WM_COMMAND messages
 *
 * PARAMETER(S):
 *
 * RETURN:
 */
static LRESULT LISTVIEW_Command(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(hwnd);

    switch (HIWORD(wParam))
    {
        case EN_UPDATE:
        {
            /*
             * Adjust the edit window size
             */
            char buffer[1024];
            HDC           hdc      = GetDC(infoPtr->hwndEdit);
            HFONT         hFont, hOldFont = 0;
            RECT          rect;
            SIZE          sz;
            int           len;

            len = GetWindowTextA(infoPtr->hwndEdit, buffer, 1023);
            GetWindowRect(infoPtr->hwndEdit, &rect);

            /* Select font to get the right dimension of the string */
            hFont = SendMessageA(infoPtr->hwndEdit, WM_GETFONT, 0, 0);
            if(hFont != 0)
            {
                hOldFont = SelectObject(hdc, hFont);
            }

            if (GetTextExtentPoint32A(hdc, buffer, lstrlenA(buffer), &sz))
            {
                TEXTMETRICA textMetric;

                /* Add Extra spacing for the next character */
                GetTextMetricsA(hdc, &textMetric);
                sz.cx += (textMetric.tmMaxCharWidth * 2);

                SetWindowPos (
                    infoPtr->hwndEdit,
                    HWND_TOP,
                    0,
                    0,
                    sz.cx,
                    rect.bottom - rect.top,
                    SWP_DRAWFRAME|SWP_NOMOVE);
            }
            if(hFont != 0)
            {
                SelectObject(hdc, hOldFont);
            }

            ReleaseDC(hwnd, hdc);

            break;
        }

        default:
          return SendMessageA (GetParent (hwnd), WM_COMMAND, wParam, lParam);
    }

    return 0;
}

/***
 * DESCRIPTION:
 * Subclassed edit control windproc function
 *
 * PARAMETER(S):
 *
 * RETURN:
 */
LRESULT CALLBACK EditLblWndProc(HWND hwnd, UINT uMsg,
        WPARAM wParam, LPARAM lParam)
{
    BOOL cancel = FALSE;
    LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO *)LISTVIEW_GetInfoPtr(GetParent(hwnd));
    EDITLABEL_ITEM *einfo = infoPtr->pedititem;
    static BOOL bIgnoreKillFocus = FALSE;

    switch (uMsg)
    {
        case WM_GETDLGCODE:
          return DLGC_WANTARROWS | DLGC_WANTALLKEYS;

        case WM_KILLFOCUS:
            if(bIgnoreKillFocus)
            {
                return TRUE;
            }
            break;

        case WM_DESTROY:
            {
                WNDPROC editProc = einfo->EditWndProc;
                SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)editProc);
                COMCTL32_Free(einfo);
                infoPtr->pedititem = NULL;
                return CallWindowProcA(editProc, hwnd, uMsg, wParam, lParam);
            }

        case WM_CHAR:
            if (VK_ESCAPE == (INT)wParam)
            {
                cancel = TRUE;
                break;
            }
            else if (VK_RETURN == (INT)wParam)
                break;

        default:
            return CallWindowProcA(einfo->EditWndProc, hwnd,
                        uMsg, wParam, lParam);
    }

    if (einfo->EditLblCb)
    {
        char *buffer  = NULL;

        if (!cancel)
        {
            int len = 1 + GetWindowTextLengthA(hwnd);

            if (len > 1)
            {
                if (NULL != (buffer = (char *)COMCTL32_Alloc(len*sizeof(char))))
                {
                    GetWindowTextA(hwnd, buffer, len);
                }
            }
        }
        /* Processing LVN_ENDLABELEDIT message could kill the focus       */
        /* eg. Using a messagebox                                         */
        bIgnoreKillFocus = TRUE;
        einfo->EditLblCb(GetParent(hwnd),buffer,einfo->param,cancel);

        if (buffer)
            COMCTL32_Free(buffer);

        einfo->EditLblCb = NULL;
        bIgnoreKillFocus = FALSE;
    }

    SendMessageA(hwnd, WM_CLOSE, 0, 0);
    return TRUE;
}

/***
 * DESCRIPTION:
 * Creates a subclassed edit cotrol
 *
 * PARAMETER(S):
 *
 * RETURN:
 */
HWND CreateEditLabel(LPCSTR text, DWORD style, INT x, INT y,
        INT width, INT height, HWND parent, HINSTANCE hinst,
        EditlblCallback EditLblCb, DWORD param)
{
    HWND hedit;
    SIZE sz;
    HDC hdc;
    HDC hOldFont=0;
    TEXTMETRICA textMetric;
    LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(parent);

    if (NULL == (infoPtr->pedititem = (EDITLABEL_ITEM*)COMCTL32_Alloc(sizeof(EDITLABEL_ITEM))))
        return 0;

    style |= WS_CHILDWINDOW|WS_CLIPSIBLINGS|ES_LEFT|WS_BORDER;
    hdc = GetDC(parent);

    /* Select the font to get appropriate metric dimensions */
    if(infoPtr->hFont != 0)
    {
        hOldFont = SelectObject(hdc, infoPtr->hFont);
    }

    /*Get String Lenght in pixels */
    GetTextExtentPoint32A(hdc, text, lstrlenA(text), &sz);

    /*Add Extra spacing for the next character */
    GetTextMetricsA(hdc, &textMetric);
    sz.cx += (textMetric.tmMaxCharWidth * 2);

    if(infoPtr->hFont != 0)
    {
        SelectObject(hdc, hOldFont);
    }

    ReleaseDC(parent, hdc);
    if (!(hedit = CreateWindowA("Edit", text, style, x, y, sz.cx, height,
                    parent, 0, hinst, 0)))
    {
        COMCTL32_Free(infoPtr->pedititem);
        return 0;
    }

    infoPtr->pedititem->param = param;
    infoPtr->pedititem->EditLblCb = EditLblCb;
    infoPtr->pedititem->EditWndProc = (WNDPROC)SetWindowLongA(hedit,
          GWL_WNDPROC, (LONG) EditLblWndProc);

    SendMessageA(hedit, WM_SETFONT, infoPtr->hFont, FALSE);

    return hedit;
}

static LRESULT LISTVIEW_CreateDragImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  INT iItem = (INT)wParam;
  LPPOINT lpptUpLeft = (LPPOINT)lParam;
  HWND hwtop;
  HDC htopdc,hdc;
  HFONT hOldFont;
  RECT rect;
  HBITMAP hbmp,hOldbmp;
  HIMAGELIST dragImage = NULL;

  LISTVIEW_GetOrigin(hwnd,lpptUpLeft);

  if ((iItem < 0) || (iItem > infoPtr->hdpaItems->nItemCount)) return 0;

  //get size
  LISTVIEW_GetItemRect(hwnd,iItem,&rect,LVIR_BOUNDS);
  rect.right -= rect.left;
  rect.bottom -= rect.top;
  rect.left = 0;
  rect.top = 0;

  hwtop = GetDesktopWindow();
  htopdc = GetDC(hwtop);
  hdc = CreateCompatibleDC(htopdc);

  hOldFont = SelectObject(hdc,infoPtr->hFont);
  hbmp = CreateCompatibleBitmap(htopdc,rect.right,rect.bottom);
  hOldbmp = SelectObject(hdc,hbmp);

  if (infoPtr->uView == LVS_ICON)
  {
    LISTVIEW_DrawLargeItem(hwnd,hdc,iItem,rect);
  } else
  {
    LISTVIEW_DrawItem(hwnd,hdc,iItem,rect);
  }

  SelectObject(hdc,hOldFont);
  SelectObject(hdc,hOldbmp);

  dragImage = ImageList_Create(rect.right,rect.bottom,ILC_COLOR,10,10);
  ImageList_Add(dragImage,hbmp,0);

  DeleteDC(hdc);
  DeleteObject(hbmp);
  ReleaseDC(hwtop,htopdc);

  return (LRESULT)dragImage;
}

static LRESULT LISTVIEW_GetISearchString(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
  LISTVIEW_INFO *infoPtr = LISTVIEW_GetInfoPtr(hwnd);
  LPWSTR lpsz = (LPWSTR)lParam;

  if (infoPtr->uISearchLen == 0) return 0;

  if (unicode)
    lstrcpyW(lpsz,infoPtr->pszISearch);
  else
    lstrcpyWtoA((LPSTR)lpsz,infoPtr->pszISearch);

  return infoPtr->uISearchLen;
}

static LRESULT LISTVIEW_GetToolTips(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);

  return infoPtr->hwndToolTip;
}

static LRESULT LISTVIEW_SetToolTips(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LISTVIEW_INFO *infoPtr = (LISTVIEW_INFO*)LISTVIEW_GetInfoPtr(hwnd);
  HWND oldToolTip = infoPtr->hwndToolTip;

  infoPtr->hwndToolTip = (HWND)lParam;

  return oldToolTip;
}

static LRESULT LISTVIEW_MouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  //CB: todo

  return DefWindowProcA(hwnd,WM_MOUSEMOVE,wParam,lParam);
}

/***
 * DESCRIPTION:
 * Window procedure of the listview control.
 *
 */
static LRESULT WINAPI LISTVIEW_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam)
{
  switch (uMsg)
  {
    case LVM_APPROXIMATEVIEWRECT:
      return LISTVIEW_ApproximateViewRect(hwnd,(INT)wParam,LOWORD(lParam),HIWORD(lParam));

    case LVM_ARRANGE:
      return LISTVIEW_Arrange(hwnd, (INT)wParam);

    case LVM_CREATEDRAGIMAGE:
      return LISTVIEW_CreateDragImage(hwnd,wParam,lParam);

    case LVM_DELETEALLITEMS:
      return LISTVIEW_DeleteAllItems(hwnd);

    case LVM_DELETECOLUMN:
      return LISTVIEW_DeleteColumn(hwnd, (INT)wParam);

    case LVM_DELETEITEM:
      return LISTVIEW_DeleteItem(hwnd, (INT)wParam);

    case LVM_EDITLABELA:
      return LISTVIEW_EditLabel(hwnd,(INT)wParam,FALSE);

    case LVM_EDITLABELW:
      return LISTVIEW_EditLabel(hwnd,(INT)wParam,TRUE);

    case LVM_ENSUREVISIBLE:
      return LISTVIEW_EnsureVisible(hwnd,(INT)wParam,(BOOL)lParam);

    case LVM_FINDITEMA:
      return LISTVIEW_FindItem(hwnd,(INT)wParam,(LPLVFINDINFOW)lParam,FALSE);

    case LVM_FINDITEMW:
      return LISTVIEW_FindItem(hwnd,(INT)wParam,(LPLVFINDINFOW)lParam,TRUE);

    case LVM_GETBKCOLOR:
      return LISTVIEW_GetBkColor(hwnd);

    case LVM_GETBKIMAGEA:
      return LISTVIEW_GetBkImage(hwnd,wParam,lParam,FALSE);

    case LVM_GETBKIMAGEW:
      return LISTVIEW_GetBkImage(hwnd,wParam,lParam,TRUE);

    case LVM_GETCALLBACKMASK:
      return LISTVIEW_GetCallbackMask(hwnd);

    case LVM_GETCOLUMNA:
      return LISTVIEW_GetColumn(hwnd,(INT)wParam,(LPLVCOLUMNW)lParam,FALSE);

    case LVM_GETCOLUMNW:
      return LISTVIEW_GetColumn(hwnd,(INT)wParam,(LPLVCOLUMNW)lParam,TRUE);

    case LVM_GETCOLUMNORDERARRAY:
      return LISTVIEW_GetColumnOrderArray(hwnd, (INT)wParam, (LPINT)lParam);

    case LVM_GETCOLUMNWIDTH:
      return LISTVIEW_GetColumnWidth(hwnd, (INT)wParam);

    case LVM_GETCOUNTPERPAGE:
      return LISTVIEW_GetCountPerPage(hwnd);

    case LVM_GETEDITCONTROL:
      return LISTVIEW_GetEditControl(hwnd);

    case LVM_GETEXTENDEDLISTVIEWSTYLE:
      return LISTVIEW_GetExtendedListViewStyle(hwnd);

    case LVM_GETHEADER:
      return LISTVIEW_GetHeader(hwnd);

    case LVM_GETHOTCURSOR:
      return LISTVIEW_GetHotCursor(hwnd,wParam,lParam);

    case LVM_GETHOTITEM:
      return LISTVIEW_GetHotItem(hwnd);

    case LVM_GETHOVERTIME:
      return LISTVIEW_GetHoverTime(hwnd,wParam,lParam);

    case LVM_GETIMAGELIST:
      return LISTVIEW_GetImageList(hwnd, (INT)wParam);

    case LVM_GETISEARCHSTRINGA:
      return LISTVIEW_GetISearchString(hwnd,wParam,lParam,FALSE);

    case LVM_GETISEARCHSTRINGW:
      return LISTVIEW_GetISearchString(hwnd,wParam,lParam,TRUE);

    case LVM_GETITEMA:
      return LISTVIEW_GetItem(hwnd,(LPLVITEMW)lParam,FALSE,FALSE);

    case LVM_GETITEMW:
      return LISTVIEW_GetItem(hwnd,(LPLVITEMW)lParam,TRUE,FALSE);

    case LVM_GETITEMCOUNT:
      return LISTVIEW_GetItemCount(hwnd);

    case LVM_GETITEMPOSITION:
      return LISTVIEW_GetItemPosition(hwnd, (INT)wParam, (LPPOINT)lParam);

    case LVM_GETITEMRECT:
      return LISTVIEW_GetItemRect(hwnd,(INT)wParam,(LPRECT)lParam,lParam ? ((LPRECT)lParam)->left:0);

    case LVM_GETITEMSPACING:
      return LISTVIEW_GetItemSpacing(hwnd, (BOOL)wParam);

    case LVM_GETITEMSTATE:
      return LISTVIEW_GetItemState(hwnd, (INT)wParam, (UINT)lParam);

    case LVM_GETITEMTEXTA:
      return LISTVIEW_GetItemText(hwnd,(INT)wParam,(LPLVITEMW)lParam,FALSE);

    case LVM_GETITEMTEXTW:
      return LISTVIEW_GetItemText(hwnd,(INT)wParam,(LPLVITEMW)lParam,TRUE);

    case LVM_GETNEXTITEM:
      return LISTVIEW_GetNextItem(hwnd, (INT)wParam, LOWORD(lParam));

    case LVM_GETNUMBEROFWORKAREAS:
      return LISTVIEW_GetNumberOfWorkAreas(hwnd,wParam,lParam);

    case LVM_GETORIGIN:
      return LISTVIEW_GetOrigin(hwnd, (LPPOINT)lParam);

    case LVM_GETSELECTEDCOUNT:
      return LISTVIEW_GetSelectedCount(hwnd);

    case LVM_GETSELECTIONMARK:
      return LISTVIEW_GetSelectionMark(hwnd);

    case LVM_GETSTRINGWIDTHA:
      return LISTVIEW_GetStringWidth(hwnd,0,(LPWSTR)lParam,FALSE);

    case LVM_GETSTRINGWIDTHW:
      return LISTVIEW_GetStringWidth(hwnd,0,(LPWSTR)lParam,TRUE);

    case LVM_GETSUBITEMRECT:
      return LISTVIEW_GetSubItemRect(hwnd,(INT)wParam,(LPRECT)lParam);

    case LVM_GETTEXTBKCOLOR:
      return LISTVIEW_GetTextBkColor(hwnd);

    case LVM_GETTEXTCOLOR:
      return LISTVIEW_GetTextColor(hwnd);

    case LVM_GETTOOLTIPS:
      return LISTVIEW_GetToolTips(hwnd,wParam,lParam);

    case LVM_GETTOPINDEX:
      return LISTVIEW_GetTopIndex(hwnd);

    case LVM_GETVIEWRECT:
      return LISTVIEW_GetViewRect(hwnd, (LPRECT)lParam);

    case LVM_GETWORKAREAS:
      return LISTVIEW_GetWorkAreas(hwnd,wParam,lParam);

    case LVM_HITTEST:
      return LISTVIEW_HitTest(hwnd,(LPLVHITTESTINFO)lParam);

    case LVM_INSERTCOLUMNA:
      return LISTVIEW_InsertColumn(hwnd,(INT)wParam,(LPLVCOLUMNW)lParam,FALSE);

    case LVM_INSERTCOLUMNW:
      return LISTVIEW_InsertColumn(hwnd,(INT)wParam,(LPLVCOLUMNW)lParam,TRUE);

    case LVM_INSERTITEMA:
      return LISTVIEW_InsertItem(hwnd,(LPLVITEMW)lParam,FALSE);

    case LVM_INSERTITEMW:
      return LISTVIEW_InsertItem(hwnd,(LPLVITEMW)lParam,TRUE);

    case LVM_REDRAWITEMS:
      return LISTVIEW_RedrawItems(hwnd, (INT)wParam, (INT)lParam);

    case LVM_SCROLL:
      return LISTVIEW_Scroll(hwnd,wParam,lParam);

    case LVM_SETBKCOLOR:
      return LISTVIEW_SetBkColor(hwnd, (COLORREF)lParam);

    case LVM_SETBKIMAGEA:
      return LISTVIEW_SetBkImage(hwnd,wParam,lParam,FALSE);

    case LVM_SETBKIMAGEW:
      return LISTVIEW_SetBkImage(hwnd,wParam,lParam,TRUE);

    case LVM_SETCALLBACKMASK:
      return LISTVIEW_SetCallbackMask(hwnd, (UINT)wParam);

    case LVM_SETCOLUMNA:
      return LISTVIEW_SetColumn(hwnd,(INT)wParam,(LPLVCOLUMNW)lParam,FALSE);

    case LVM_SETCOLUMNW:
      return LISTVIEW_SetColumn(hwnd,(INT)wParam,(LPLVCOLUMNW)lParam,TRUE);

    case LVM_SETCOLUMNORDERARRAY:
      return LISTVIEW_SetColumnOrderArray(hwnd, (INT)wParam, (LPINT)lParam);

    case LVM_SETCOLUMNWIDTH:
      return LISTVIEW_SetColumnWidth(hwnd, (INT)wParam, (INT)lParam);

    case LVM_SETEXTENDEDLISTVIEWSTYLE:
      return LISTVIEW_SetExtendedListViewStyle(hwnd, (DWORD)wParam, (DWORD)lParam);

    case LVM_SETHOTCURSOR:
      return LISTVIEW_SetHotCursor(hwnd,wParam,lParam);

    case LVM_SETHOTITEM:
      return LISTVIEW_SetHotItem(hwnd, (INT)wParam);

    case LVM_SETHOVERTIME:
      return LISTVIEW_SetHoverTime(hwnd,wParam,lParam);

    case LVM_SETICONSPACING:
      return LISTVIEW_SetIconSpacing(hwnd,wParam,lParam);

    case LVM_SETIMAGELIST:
      return LISTVIEW_SetImageList(hwnd, (INT)wParam, (HIMAGELIST)lParam);

    case LVM_SETITEMA:
      return LISTVIEW_SetItem(hwnd,(LPLVITEMW)lParam,FALSE);

    case LVM_SETITEMW:
      return LISTVIEW_SetItem(hwnd,(LPLVITEMW)lParam,TRUE);

    case LVM_SETITEMCOUNT:
      return LISTVIEW_SetItemCount(hwnd, (INT)wParam, (DWORD)lParam);

    case LVM_SETITEMPOSITION:
      return LISTVIEW_SetItemPosition(hwnd,(INT)wParam,(INT)LOWORD(lParam),(INT)HIWORD(lParam),FALSE);

    case LVM_SETITEMPOSITION32:
      return LISTVIEW_SetItemPosition32(hwnd,(INT)wParam,(LPPOINT)lParam);

    case LVM_SETITEMSTATE:
      return LISTVIEW_SetItemState(hwnd, (INT)wParam, (LPLVITEMW)lParam);

    case LVM_SETITEMTEXTA:
      return LISTVIEW_SetItemText(hwnd,(INT)wParam,(LPLVITEMW)lParam,FALSE);

    case LVM_SETITEMTEXTW:
      return LISTVIEW_SetItemText(hwnd,(INT)wParam,(LPLVITEMW)lParam,TRUE);

    case LVM_SETSELECTIONMARK:
      return LISTVIEW_SetSelectionMark(hwnd, (INT)lParam);

    case LVM_SETTEXTBKCOLOR:
      return LISTVIEW_SetTextBkColor(hwnd, (COLORREF)lParam);

    case LVM_SETTEXTCOLOR:
      return LISTVIEW_SetTextColor(hwnd, (COLORREF)lParam);

    case LVM_SETTOOLTIPS:
      return LISTVIEW_SetToolTips(hwnd,wParam,lParam);

    case LVM_SETWORKAREAS:
      return LISTVIEW_SetWorkAreas(hwnd,wParam,lParam);

    case LVM_SORTITEMS:
      return LISTVIEW_SortItems(hwnd, wParam, lParam);

    case LVM_SUBITEMHITTEST:
      return LISTVIEW_SubItemHitTest(hwnd,(LPLVHITTESTINFO)lParam);

    case LVM_UPDATE:
      return LISTVIEW_Update(hwnd, (INT)wParam);

    case WM_CHAR:
      return LISTVIEW_Char(hwnd,wParam,lParam);

    case WM_COMMAND:
      return LISTVIEW_Command(hwnd, wParam, lParam);

    case WM_CREATE:
      return LISTVIEW_Create(hwnd, wParam, lParam);

    case WM_ERASEBKGND:
      return LISTVIEW_EraseBackground(hwnd, wParam, lParam);

    case WM_GETDLGCODE:
      return DLGC_WANTCHARS | DLGC_WANTTAB | DLGC_WANTARROWS;

    case WM_GETFONT:
      return LISTVIEW_GetFont(hwnd);

    case WM_HSCROLL:
      return LISTVIEW_HScroll(hwnd,(INT)LOWORD(wParam),(INT)HIWORD(wParam),(HWND)lParam);

    case WM_KEYDOWN:
      return LISTVIEW_KeyDown(hwnd, (INT)wParam, (LONG)lParam);

    case WM_KILLFOCUS:
      return LISTVIEW_KillFocus(hwnd);

    case WM_LBUTTONDBLCLK:
      return LISTVIEW_LButtonDblClk(hwnd,(WORD)wParam,LOWORD(lParam),HIWORD(lParam));

    case WM_LBUTTONDOWN:
      return LISTVIEW_LButtonDown(hwnd,(WORD)wParam,LOWORD(lParam),HIWORD(lParam));

    case WM_LBUTTONUP:
      return LISTVIEW_LButtonUp(hwnd,(WORD)wParam,LOWORD(lParam),HIWORD(lParam));

    case WM_MOUSEMOVE:
      return LISTVIEW_MouseMove (hwnd,wParam,lParam);

    case WM_NCCREATE:
      return LISTVIEW_NCCreate(hwnd, wParam, lParam);

    case WM_NCDESTROY:
      return LISTVIEW_NCDestroy(hwnd);

    case WM_NOTIFY:
      return LISTVIEW_Notify(hwnd, (INT)wParam, (LPNMHDR)lParam);

    case WM_PAINT:
      return LISTVIEW_Paint(hwnd, (HDC)wParam);

    case WM_RBUTTONDBLCLK:
      return LISTVIEW_RButtonDblClk(hwnd,(WORD)wParam,LOWORD(lParam),HIWORD(lParam));

    case WM_RBUTTONDOWN:
      return LISTVIEW_RButtonDown(hwnd,(WORD)wParam,LOWORD(lParam),HIWORD(lParam));

    case WM_RBUTTONUP:
      return LISTVIEW_RButtonUp(hwnd,(WORD)wParam,LOWORD(lParam),HIWORD(lParam));

    case WM_SETFOCUS:
      return LISTVIEW_SetFocus(hwnd, (HWND)wParam);

    case WM_SETFONT:
      return LISTVIEW_SetFont(hwnd, (HFONT)wParam, (WORD)lParam);

    case WM_SETREDRAW:
      return LISTVIEW_SetRedraw(hwnd, (BOOL)wParam);

    case WM_SIZE:
      return LISTVIEW_Size(hwnd, (int)SLOWORD(lParam), (int)SHIWORD(lParam));

    case WM_STYLECHANGED:
      return LISTVIEW_StyleChanged(hwnd, wParam, (LPSTYLESTRUCT)lParam);

    case WM_TIMER:
      return LISTVIEW_Timer(hwnd,wParam,lParam);

    case WM_VSCROLL:
      return LISTVIEW_VScroll(hwnd,(INT)LOWORD(wParam),(INT)HIWORD(wParam),(HWND)lParam);

    case WM_MOUSEWHEEL:
      return LISTVIEW_MouseWheel(hwnd,wParam,lParam);

/*      case WM_WINDOWPOSCHANGED: */
/*      case WM_WININICHANGE: */

    default:
      //if (uMsg >= WM_USER)
      //{
      //  ERR("unknown msg %04x wp=%08x lp=%08lx\n", uMsg, wParam,
      //      lParam);
      //}

      /* call default window procedure */
      return defComCtl32ProcA(hwnd, uMsg, wParam, lParam);
  }

  return 0;
}

/***
 * DESCRIPTION:
 * Registers the window class.
 *
 * PARAMETER(S):
 * None
 *
 * RETURN:
 * None
 */
VOID LISTVIEW_Register(VOID)
{
  WNDCLASSA wndClass;

  ZeroMemory(&wndClass, sizeof(WNDCLASSA));
  wndClass.style = CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = (WNDPROC)LISTVIEW_WindowProc;
  wndClass.cbClsExtra = 0;
  wndClass.cbWndExtra = sizeof(LISTVIEW_INFO *);
  wndClass.hCursor = LoadCursorA(0, IDC_ARROWA);
  wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wndClass.lpszClassName = WC_LISTVIEWA;
  RegisterClassA(&wndClass);
}

/***
 * DESCRIPTION:
 * Unregisters the window class.
 *
 * PARAMETER(S):
 * None
 *
 * RETURN:
 * None
 */
VOID LISTVIEW_Unregister(VOID)
{
  UnregisterClassA(WC_LISTVIEWA, (HINSTANCE)NULL);
}
