/* $Id: treeview.cpp,v 1.3 2000-03-18 16:17:34 cbratschi Exp $ */
/* Treeview control
 *
 * Copyright 1998 Eric Kohl <ekohl@abo.rhein-zeitung.de>
 * Copyright 1998,1999 Alex Priem <alexp@sci.kun.nl>
 * Copyright 1999 Sylvain St-Germain
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999-2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 *
 * TODO:
 *   Using DPA to store the item ptr would be good.
 *  -check custom draw
                        uHotItem
 *   use separate routine to get item text/image.
 *
 * FIXMEs  (for personal use)
    Expand:             -ctlmacro expands twice ->toggle.
   -DblClick:   ctlmacro.exe's NM_DBLCLK seems to go wrong (returns FALSE).
   -treehelper: stack corruption makes big window.
 *
 * Status: in progress
 * Version: 5.80
 */

/*
 Most identical with:
 - Corel 20000212 level
 - (WINE 991212 level)
*/

/* CB: todo
 - bug in SetScrollInfo/ShowScrollBar: WM_SIZE and WM_NCPAINT problems (i.e. RegEdit)
 - VK_LEFT in WinHlp32 displays expanded icon
 - tooltips not finished
 - expand not finished
 - TVS_FULLROWSELECT
 - TVS_TRACKSELECT (called hottrack)
 - WM_ENABLE: draw disabled control
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "winbase.h"
#include "wingdi.h"
#include "winnls.h"
#include "commctrl.h"
#include "comctl32.h"
#include "ccbase.h"
#include "treeview.h"

/* ffs should be in <string.h>. */

//#define OS2LINEHACK //CB: too slow, but looks good

/* Defines, since they do not need to return previous state, and nr
 * has no side effects in this file.
 */
#define tv_test_bit(nr,bf)      (((LPBYTE)bf)[nr>>3]&(1<<(nr&7)))
#define tv_set_bit(nr,bf)       ((LPBYTE)bf)[nr>>3]|=(1<<(nr&7))
#define tv_clear_bit(nr,bf)     ((LPBYTE)bf)[nr>>3]&=~(1<<(nr&7))

#define TREEVIEW_GetInfoPtr(hwnd) ((TREEVIEW_INFO *)getInfoPtr(hwnd))

static BOOL    TREEVIEW_SendTreeviewNotify (HWND hwnd, UINT code, UINT action, HTREEITEM oldItem, HTREEITEM newItem);
static BOOL    TREEVIEW_SendTreeviewDnDNotify (HWND hwnd, UINT code, HTREEITEM dragItem, POINT pt);
static BOOL    TREEVIEW_SendDispInfoNotify (HWND hwnd, TREEVIEW_ITEM *wineItem, UINT code, UINT what);
static BOOL    TREEVIEW_SendCustomDrawNotify (HWND hwnd, DWORD dwDrawStage, HDC hdc, RECT rc);
static BOOL    TREEVIEW_SendCustomDrawItemNotify (HWND hwnd, HDC hdc, TREEVIEW_ITEM *tvItem, UINT uItemDrawState);
static LRESULT TREEVIEW_RButtonUp (HWND hwnd, LPPOINT pPt);
static LRESULT TREEVIEW_SelectItem (HWND hwnd, WPARAM wParam, LPARAM lParam);
static LRESULT TREEVIEW_DoSelectItem(HWND hwnd,INT action,HTREEITEM newSelect,INT cause);
static void    TREEVIEW_Refresh(HWND hwnd);
static void    TREEVIEW_Draw(HWND hwnd,HDC hdc,RECT *updateRect);
static BOOL    TREEVIEW_UnqueueRefresh(HWND hwnd,BOOL calc,BOOL refresh);
static void    TREEVIEW_QueueRefresh(HWND hwnd);
static void    TREEVIEW_CalcItem(HWND hwnd,HDC hdc,DWORD dwStyle,TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item);
static BOOL    TREEVIEW_CalcItems(HWND hwnd,HDC hdc,TREEVIEW_INFO *infoPtr);
static LRESULT TREEVIEW_EnsureVisible(HWND hwnd,HTREEITEM hItem);
static VOID    TREEVIEW_ISearch(HWND hwnd,CHAR ch);

static LRESULT CALLBACK TREEVIEW_Edit_SubclassProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI TREEVIEW_EndEditLabelNow (HWND hwnd, BOOL bCancel);

HWND TREEVIEW_EditLabel(HWND hwnd, HTREEITEM hItem,BOOL unicode);

/* helper functions. Work with the assumption that validity of operands
   is checked beforehand, and that tree state is valid.  */

/* FIXME: MS documentation says `GetNextVisibleItem' returns NULL
   if not succesfull'. Probably only applies to derefencing infoPtr
   (ie we are offered a valid treeview structure)
   and not whether there is a next `visible' child.
   FIXME: check other failures.
 */

/***************************************************************************
 * This method returns the TREEVIEW_ITEM object given the handle
 */
static TREEVIEW_ITEM* TREEVIEW_ValidItem(
  TREEVIEW_INFO *infoPtr,
  HTREEITEM  handle)
{
  if ((!handle) || (handle>infoPtr->uMaxHandle))
    return NULL;

  if (tv_test_bit ((INT)handle, infoPtr->freeList))
    return NULL;

  return &infoPtr->items[(INT)handle];
}

/***************************************************************************
 * This function uses cChildren field to decide whether item has children
 * or not.
 * Note: return value doesn't reflect physical presence of children.
 */
static INT TREEVIEW_HasChildren(
  HWND hwnd,
  TREEVIEW_ITEM *wineItem)
{
   INT cChildren = 0;

   if ( wineItem->mask & TVIF_CHILDREN )
   {
      if (wineItem->cChildren == I_CHILDRENCALLBACK)
      {
         TREEVIEW_ITEM tempItem;

         tempItem.hItem  = wineItem->hItem;
         tempItem.state  = wineItem->state;
         tempItem.lParam = wineItem->lParam;

         TREEVIEW_SendDispInfoNotify(hwnd, &tempItem, TVN_GETDISPINFO, TVIF_CHILDREN);
         cChildren = tempItem.cChildren;
      }
      else
         cChildren = wineItem->cChildren;
   }
   else if ( wineItem->firstChild )
      cChildren = 1;

   return cChildren;
}

/***************************************************************************
 * This method returns the last expanded child item of a tree node
 */
static TREEVIEW_ITEM *TREEVIEW_GetLastListItem(
  HWND hwnd,
  TREEVIEW_INFO *infoPtr,
  TREEVIEW_ITEM *tvItem)
{
  TREEVIEW_ITEM *wineItem = tvItem;

  /*
   * Get this item last sibling
   */
  while (wineItem->sibling)
          wineItem=& infoPtr->items [(INT)wineItem->sibling];

  /*
   * If the last sibling has expanded children, restart.
   */
  if ((wineItem->state & TVIS_EXPANDED) &&
       TREEVIEW_HasChildren(hwnd, wineItem))
  {
       return TREEVIEW_GetLastListItem(
             hwnd,
             infoPtr,
             &(infoPtr->items[(INT)wineItem->firstChild]));
  }

  return wineItem;
}

/***************************************************************************
 * This method returns the previous physical item in the list not
 * considering the tree hierarchy.
 */
static TREEVIEW_ITEM *TREEVIEW_GetPrevListItem(
  HWND hwnd,
  TREEVIEW_INFO *infoPtr,
  TREEVIEW_ITEM *tvItem)
{
  if (tvItem->upsibling)
  {
    /*
     * This item has a upsibling, get the last item.  Since, GetLastListItem
     * first looks at siblings, we must feed it with the first child.
     */
    TREEVIEW_ITEM *upItem = &infoPtr->items[(INT)tvItem->upsibling];

    if ( (upItem->state & TVIS_EXPANDED) &&
          TREEVIEW_HasChildren(hwnd, upItem) )
    {
       return TREEVIEW_GetLastListItem(
               hwnd,
               infoPtr,
               &infoPtr->items[(INT)upItem->firstChild]);
    }
    else
      return upItem;
  }
  else
  {
    /*
     * this item does not have a upsibling, get the parent
     */
    if (tvItem->parent)
      return &infoPtr->items[(INT)tvItem->parent];
  }

  return NULL;
}


/***************************************************************************
 * This method returns the next physical item in the treeview not
 * considering the tree hierarchy.
 */
static TREEVIEW_ITEM *TREEVIEW_GetNextListItem(
  HWND hwnd,
  TREEVIEW_INFO *infoPtr,
  TREEVIEW_ITEM *tvItem)
{
  TREEVIEW_ITEM *wineItem = NULL;

  /*
   * If this item has children and is expanded, return the first child
   */
  if ( (tvItem->state & TVIS_EXPANDED) &&
        TREEVIEW_HasChildren(hwnd, tvItem) )
  {
     return (& infoPtr->items[(INT)tvItem->firstChild]);
  }


  /*
   * try to get the sibling
   */
  if (tvItem->sibling)
                return (& infoPtr->items[(INT)tvItem->sibling]);

  /*
   * Otherwise, get the parent's sibling.
   */
  wineItem=tvItem;
  while (wineItem->parent) {
    wineItem=& infoPtr->items [(INT)wineItem->parent];
        if (wineItem->sibling)
      return (& infoPtr->items [(INT)wineItem->sibling]);
  }

  return NULL;
}

/***************************************************************************
 * This method returns the nth item starting at the given item.  It returns
 * the last item (or first) we we run out of items.
 *
 * Will scroll backward if count is <0.
 *             forward if count is >0.
 */
static TREEVIEW_ITEM *TREEVIEW_GetListItem(
  HWND hwnd,
  TREEVIEW_INFO *infoPtr,
  TREEVIEW_ITEM *tvItem,
  LONG          count)
{
  TREEVIEW_ITEM *previousItem = NULL;
  TREEVIEW_ITEM *wineItem     = tvItem;
  LONG          iter          = 0;

  if      (count > 0)
  {
    /* Find count item downward */
    while ((iter++ < count) && (wineItem != NULL))
    {
      /* Keep a pointer to the previous in case we ask for more than we got */
      previousItem = wineItem;
      wineItem     = TREEVIEW_GetNextListItem(hwnd,infoPtr,wineItem);
    }

    if (wineItem == NULL)
      wineItem = previousItem;
  }
  else if (count < 0)
  {
    /* Find count item upward */
    while ((iter-- > count) && (wineItem != NULL))
    {
      /* Keep a pointer to the previous in case we ask for more than we got */
      previousItem = wineItem;
      wineItem = TREEVIEW_GetPrevListItem(hwnd,infoPtr,wineItem);
    }

    if (wineItem == NULL)
      wineItem = previousItem;
  }
  else
    wineItem = NULL;

  return wineItem;
}


/***************************************************************************
 * This method
 */
static void TREEVIEW_RemoveAllChildren(
  HWND hwnd,
  TREEVIEW_ITEM *parentItem)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *killItem;
  INT    kill;

  kill=(INT)parentItem->firstChild;
  while (kill)
  {
    tv_set_bit ( kill, infoPtr->freeList);
    infoPtr->uNumItems--;
    killItem=& infoPtr->items[kill];
    if (killItem->pszText != LPSTR_TEXTCALLBACKW)
      COMCTL32_Free (killItem->pszText);
    killItem->pszText = NULL;
    TREEVIEW_SendTreeviewNotify (hwnd, TVN_DELETEITEM, 0, (HTREEITEM)kill, 0);
    if (killItem->firstChild)
      TREEVIEW_RemoveAllChildren (hwnd, killItem);
    kill=(INT)killItem->sibling;
  }

  parentItem->firstChild = 0;
}


static void
TREEVIEW_RemoveItem (HWND hwnd, TREEVIEW_ITEM *wineItem)

{
 TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
 TREEVIEW_ITEM *parentItem, *upsiblingItem, *siblingItem;
 INT iItem;

 if (infoPtr->hwndEdit) SetFocus(hwnd);

 iItem=(INT)wineItem->hItem;
 tv_set_bit(iItem,infoPtr->freeList);
 infoPtr->uNumItems--;
 parentItem=NULL;
 if (wineItem->pszText != LPSTR_TEXTCALLBACKW)
   COMCTL32_Free (wineItem->pszText);
 wineItem->pszText = NULL;

 TREEVIEW_SendTreeviewNotify (hwnd, TVN_DELETEITEM, 0, (HTREEITEM)iItem, 0);

 if (wineItem->firstChild)
   TREEVIEW_RemoveAllChildren (hwnd,wineItem);

 if (wineItem->parent) {
   parentItem=& infoPtr->items [(INT)wineItem->parent];
   if ((INT)parentItem->firstChild==iItem)
     parentItem->firstChild=wineItem->sibling;
 }

 if (iItem==(INT)infoPtr->TopRootItem)
        infoPtr->TopRootItem=(HTREEITEM)wineItem->sibling;
 if (wineItem->upsibling) {
        upsiblingItem=& infoPtr->items [(INT)wineItem->upsibling];
        upsiblingItem->sibling=wineItem->sibling;
 }
 if (wineItem->sibling) {
        siblingItem=& infoPtr->items [(INT)wineItem->sibling];
        siblingItem->upsibling=wineItem->upsibling;
 }

 if (iItem==(INT)infoPtr->selectedItem) {
   if (!wineItem->upsibling)
     infoPtr->selectedItem = 0;
   else
     TREEVIEW_DoSelectItem(hwnd, TVGN_CARET, wineItem->upsibling, TVC_UNKNOWN);
 }
}

/* Note:TREEVIEW_RemoveTree doesn't remove infoPtr itself */

static void TREEVIEW_RemoveTree (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *killItem;
  int i;

  TREEVIEW_EndEditLabelNow(hwnd,TRUE);

  for (i=1; i<=(INT)infoPtr->uMaxHandle; i++)
        if (!tv_test_bit (i, infoPtr->freeList)) {
                killItem=& infoPtr->items [i];
                if (killItem->pszText != LPSTR_TEXTCALLBACKW)
                  COMCTL32_Free (killItem->pszText);
                killItem->pszText = NULL;

                TREEVIEW_SendTreeviewNotify
                                        (hwnd, TVN_DELETEITEM, 0, killItem->hItem, 0);
                }

  if (infoPtr->uNumPtrsAlloced) {
        COMCTL32_Free (infoPtr->items);
        COMCTL32_Free (infoPtr->freeList);
        infoPtr->uNumItems=0;
        infoPtr->uNumPtrsAlloced=0;
        infoPtr->uMaxHandle=0;
        infoPtr->lefttop.x = infoPtr->lefttop.y = 0;
    }
}


static LRESULT
TREEVIEW_GetImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  if ((INT)wParam == TVSIL_NORMAL)
        return (LRESULT) infoPtr->himlNormal;
  if ((INT)wParam == TVSIL_STATE)
        return (LRESULT) infoPtr->himlState;

  return 0;
}

static LRESULT
TREEVIEW_SetImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
    HIMAGELIST himlTemp;

    switch ((INT)wParam) {
        case TVSIL_NORMAL:
            himlTemp = infoPtr->himlNormal;
            infoPtr->himlNormal = (HIMAGELIST)lParam;
            return (LRESULT)himlTemp;

        case TVSIL_STATE:
            himlTemp = infoPtr->himlState;
            infoPtr->himlState = (HIMAGELIST)lParam;
            return (LRESULT)himlTemp;
    }

    return (LRESULT)NULL;
}



static LRESULT
TREEVIEW_SetItemHeight (HWND hwnd, WPARAM wParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  INT cx,cy,prevHeight = infoPtr->uItemHeight;

  if (wParam == -1)
  {
    infoPtr->uItemHeight = -1;

    return prevHeight;
  }

  ImageList_GetIconSize (infoPtr->himlNormal, &cx, &cy);

  if (wParam > cy) cy = wParam;
  infoPtr->uItemHeight = cy;

  if (!(GetWindowLongA(hwnd, GWL_STYLE) & TVS_NONEVENHEIGHT))
    if (infoPtr->uItemHeight & 0x1) infoPtr->uItemHeight++;

  return prevHeight;
}

static LRESULT
TREEVIEW_GetItemHeight (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return infoPtr->uItemHeight;
}

static LRESULT
TREEVIEW_GetLineColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  //TRACE("\n");
  return (LRESULT) infoPtr->clrLine;
}

static LRESULT
TREEVIEW_SetLineColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  COLORREF prevColor=infoPtr->clrLine;

  //TRACE("\n");
  infoPtr->clrLine=(COLORREF) lParam;
  return (LRESULT) prevColor;
}

static LRESULT
TREEVIEW_GetInsertMarkColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  //TRACE("\n");
  return (LRESULT) infoPtr->clrInsertMark;
}

static LRESULT
TREEVIEW_SetInsertMarkColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  COLORREF prevColor=infoPtr->clrInsertMark;

  //TRACE("%d %ld\n",wParam,lParam);
  infoPtr->clrInsertMark=(COLORREF) lParam;
  return (LRESULT) prevColor;
}

static LRESULT
TREEVIEW_SetInsertMark (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  //FIXME("%d %ld\n",wParam,lParam);
  if (!TREEVIEW_ValidItem (infoPtr, (HTREEITEM)lParam)) return 0;
  //FIXME("%d %ld\n",wParam,lParam);

  infoPtr->insertBeforeorAfter=(BOOL) wParam;
  infoPtr->insertMarkItem=(HTREEITEM) lParam;

  TREEVIEW_Refresh(hwnd);

  return 1;
}

static LRESULT
TREEVIEW_SetTextColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  COLORREF prevColor=infoPtr->clrText;

  infoPtr->clrText=(COLORREF) lParam;
  if (infoPtr->clrText != prevColor)
    TREEVIEW_Refresh(hwnd);

  return (LRESULT) prevColor;
}

static LRESULT
TREEVIEW_GetBkColor (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return (LRESULT) infoPtr->clrBk;
}

static LRESULT
TREEVIEW_SetBkColor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  COLORREF prevColor=infoPtr->clrBk;

  infoPtr->clrBk=(COLORREF) lParam;
  if (infoPtr->clrBk != prevColor)
    TREEVIEW_Refresh(hwnd);


  return (LRESULT) prevColor;
}

static LRESULT
TREEVIEW_GetTextColor (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return (LRESULT) infoPtr->clrText;
}


/* cdmode: custom draw mode as received from app. in first NMCUSTOMDRAW
           notification */

#define TREEVIEW_LEFT_MARGIN 8

#ifdef OS2LINEHACK
//CB: hack for PS_DOT bug in Open32 pen handling

BOOL drawPixel;
HDC  drawDC;

VOID CALLBACK TREEVIEW_DDAProc(int x,int y,LPARAM lpData)
{
  if (drawPixel) SetPixel(drawDC,x,y,(COLORREF)lpData);
  drawPixel = !drawPixel;
}

static void TREEVIEW_Polyline(HDC hdc,const POINT *lppt,int cPoints,COLORREF color)
{
  INT x;

  drawPixel = TRUE;
  drawDC = hdc;
  for (x = 0;x < cPoints-1;x++)
    LineDDA(lppt[x].x,lppt[x].y,lppt[x+1].x,lppt[x+1].y,TREEVIEW_DDAProc,color);
}
#endif

//pen must be selected!

static void TREEVIEW_DrawVLines(HDC hdc,TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item)
{
  POINT points[2];
  INT center;

  center = (item->rect.top+item->rect.bottom)/2;
//INT cChildren = TREEVIEW_HasChildren(hwnd,wineItem);
  if ((item->iLevel == 0) && !item->upsibling && item->sibling)
  {
    TREEVIEW_ITEM *lastItem = &infoPtr->items[(INT)item->sibling];

    while (lastItem->sibling) lastItem = &infoPtr->items[(INT)lastItem->sibling];

    points[0].y = center;
    points[1].x = points[0].x = 8;
    points[1].y = (lastItem->rect.top+lastItem->rect.bottom)/2;
#ifdef OS2LINEHACK
    TREEVIEW_Polyline(hdc,points,2,infoPtr->clrLine);
#else
    Polyline(hdc,points,2);
#endif
  }

  if (item->firstChild && (item->state & TVIS_EXPANDED))
  {
    TREEVIEW_ITEM *lastItem = &infoPtr->items[(INT)item->firstChild];

    while (lastItem->sibling) lastItem = &infoPtr->items[(INT)lastItem->sibling];

    points[0].y = (lastItem->upsibling != NULL) ?
                   item->rect.bottom-3:  /* is linked to an icon       */
                   item->rect.bottom+1;  /* is linked to a +/- box     */
    points[1].x = points[0].x = 28+20*item->iLevel-infoPtr->lefttop.x;
    points[1].y = (lastItem->rect.top+lastItem->rect.bottom)/2;  /* is linked to a +/- box     */
#ifdef OS2LINEHACK
    TREEVIEW_Polyline(hdc,points,2,infoPtr->clrLine);
#else
    Polyline(hdc,points,2);
#endif
  }
}

static void
TREEVIEW_DrawItem(HWND hwnd,HDC hdc,TREEVIEW_ITEM *item,DWORD dwStyle,TREEVIEW_INFO *infoPtr)
{
  INT   center,cditem;
  HFONT hOldFont;

  if (!item->calculated) TREEVIEW_CalcItem(hwnd,hdc,dwStyle,infoPtr,item);

  if (item->state & TVIS_BOLD)
    hOldFont = SelectObject(hdc,infoPtr->hBoldFont);
  else
    hOldFont = SelectObject(hdc,infoPtr->hFont);

  cditem = 0;

  if (infoPtr->cdmode & CDRF_NOTIFYITEMDRAW)
  {
    cditem = TREEVIEW_SendCustomDrawItemNotify(hwnd, hdc, item, CDDS_ITEMPREPAINT);

    if (cditem & CDRF_SKIPDEFAULT) return;
  }

  //CDRF_NEWFONT: CCM_SETVERSION < 5 clips text (what we do now)
  //                             > 5 ??? recalc?

  /*
   * Set drawing starting points
   */
  center = (item->rect.top+item->rect.bottom)/2; /* this item vertical center */

  /*
   * Display the tree hierarchy
   */
  if (dwStyle & TVS_HASLINES)
  {
    /*
     * Write links to parent node
     * we draw the L starting from the child to the parent
     *
     * points[0] is attached to the current item
     * points[1] is the L corner
     * points[2] is attached to the parent or the up sibling
     */
    if (dwStyle & TVS_LINESATROOT)
    {
      TREEVIEW_ITEM *upNode    = NULL;
      RECT  upRect             = {0,0,0,0};
      HPEN  hOldPen, hnewPen;
      POINT points[2];

      hnewPen = CreatePen(PS_DOT, 0, infoPtr->clrLine);
      hOldPen = SelectObject(hdc,hnewPen);

      TREEVIEW_DrawVLines(hdc,infoPtr,item);

      /*
       * determine the target location of the line at root, either be linked
       * to the up sibling or to the parent node.
       */
      if (item->upsibling)
        upNode  = TREEVIEW_ValidItem (infoPtr, item->upsibling);
      else if (item->parent)
        upNode  = TREEVIEW_ValidItem (infoPtr, item->parent);

      if (upNode) upRect = upNode->rect;

      if (item->iLevel == 0)
      {
        points[1].x = upRect.left+8;
        points[0].x = points[1].x + 10;
        points[1].y = points[0].y = center;
      } else
      {
        points[1].x = 8+(20*item->iLevel)-infoPtr->lefttop.x;
        points[1].y = points[0].y = center;
        points[0].x = points[1].x + 10;
      }

      /*
       * Get a dotted pen
       */
#ifdef OS2LINEHACK
      TREEVIEW_Polyline(hdc,points,2,infoPtr->clrLine);
#else
      Polyline(hdc,points,2);
#endif
      DeleteObject(hnewPen);
      SelectObject(hdc, hOldPen);
    }
  }

  /*
   * Display the (+/-) signs
   */
  if ((dwStyle & TVS_HASBUTTONS) && (dwStyle & TVS_HASLINES))
  {
    if (TREEVIEW_HasChildren(hwnd,item))
    {
      INT vCenter = (item->expandBox.top+item->expandBox.bottom)/2;
      INT hCenter = (item->expandBox.right+item->expandBox.left)/2;

      Rectangle(hdc,item->expandBox.left,item->expandBox.top,item->expandBox.right,item->expandBox.bottom);

      MoveToEx(hdc,item->expandBox.left+2,vCenter,NULL);
      LineTo(hdc,item->expandBox.right-2,vCenter);

      if (!(item->state & TVIS_EXPANDED))
      {
        MoveToEx(hdc,hCenter,item->expandBox.top+2,NULL);
        LineTo(hdc,hCenter,item->expandBox.bottom-2);
      }
    }
  }

  /*
   * Display the image associated with this item
   */
  if ((item->mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE)) || (dwStyle & TVS_CHECKBOXES))
  {
    INT        imageIndex;
    HIMAGELIST *himlp = NULL;

    /* State images are displayed to the left of the Normal image
     * image number is in state; zero should be `display no image'.
     * FIXME: that last sentence looks like it needs some checking.
     */
    if (infoPtr->himlState)
      himlp =& infoPtr->himlState;
    imageIndex = item->state >> 12;

    if (himlp && imageIndex)
    {
      imageIndex--;       /* see FIXME */
      ImageList_Draw(*himlp,imageIndex,hdc,item->statebitmap.left,item->statebitmap.top,ILD_NORMAL);
    }

    /* Now, draw the normal image; can be either selected or
     * non-selected image.
     */

    himlp = NULL;
    if (infoPtr->himlNormal)
      himlp = &infoPtr->himlNormal; /* get the image list */

    imageIndex = item->iImage;
    if ((item->state & TVIS_SELECTED) && item->iSelectedImage)
    {
      /* The item is curently selected */
      if (item->iSelectedImage == I_IMAGECALLBACK)
        TREEVIEW_SendDispInfoNotify(hwnd, item, TVN_GETDISPINFO, TVIF_SELECTEDIMAGE);

      imageIndex = item->iSelectedImage;
    } else
    {
      /* The item is not selected */
      if (item->iImage == I_IMAGECALLBACK)
        TREEVIEW_SendDispInfoNotify(hwnd, item, TVN_GETDISPINFO, TVIF_IMAGE);

      imageIndex = item->iImage;
    }

    if (himlp && (imageIndex != I_IMAGENONE))
    {
      int ovlIdx = 0;

      if(item->stateMask & TVIS_OVERLAYMASK)
        ovlIdx = item->state & TVIS_OVERLAYMASK;

      ImageList_Draw(*himlp,imageIndex,hdc,item->bitmap.left,item->bitmap.top,ILD_NORMAL | ovlIdx);
    }
  }

  /*
   * Display the text associated with this item
   */
  /* Don't paint item's text if it's being edited */
  if (!infoPtr->hwndEdit || (infoPtr->editItem != item->hItem))
  {
    if ((item->mask & TVIF_TEXT) && (item->pszText))
    {
      UINT  uTextJustify = DT_LEFT;
      COLORREF oldTextColor = 0;
      INT      oldBkMode;
      HBRUSH   hbrBk = 0;
      BOOL     inFocus = GetFocus() == hwnd;

      oldBkMode = SetBkMode(hdc, TRANSPARENT);

      /* - If item is drop target or it is selected and window is in focus -
       * use blue background (COLOR_HIGHLIGHT).
       * - If item is selected, window is not in focus, but it has style
       * TVS_SHOWSELALWAYS - use grey background (COLOR_BTNFACE)
       * - Otherwise - don't fill background
       */
      if ((item->state & TVIS_DROPHILITED) ||
          ((item->state & TVIS_SELECTED) &&
          (inFocus || (GetWindowLongA( hwnd, GWL_STYLE) & TVS_SHOWSELALWAYS))))
      {
        if ((item->state & TVIS_DROPHILITED) || inFocus)
        {
          hbrBk = CreateSolidBrush(GetSysColor( COLOR_HIGHLIGHT));
          oldTextColor = SetTextColor(hdc, GetSysColor( COLOR_HIGHLIGHTTEXT));
        } else
        {
          hbrBk = CreateSolidBrush(GetSysColor( COLOR_BTNFACE));

          if (infoPtr->clrText == -1)
            oldTextColor = SetTextColor(hdc, GetSysColor( COLOR_WINDOWTEXT));
          else
            oldTextColor = SetTextColor(hdc, infoPtr->clrText);
        }
      } else
      {
        if (infoPtr->clrText == -1)
          oldTextColor = SetTextColor(hdc, GetSysColor( COLOR_WINDOWTEXT));
        else
          oldTextColor = SetTextColor(hdc, infoPtr->clrText);
      }

      if (item->pszText == LPSTR_TEXTCALLBACKW)
        TREEVIEW_SendDispInfoNotify (hwnd, item, TVN_GETDISPINFO, TVIF_TEXT);

      if (hbrBk)
      {
        FillRect(hdc, &item->text, hbrBk);
        DeleteObject(hbrBk);
      }

      item->text.left += 2;

      /* Draw it */
      DrawTextW ( hdc,
        item->pszText,
        lstrlenW(item->pszText),
        &item->text,
        uTextJustify | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

      item->text.left -= 2;

      /* Restore the hdc state */
      SetTextColor( hdc, oldTextColor);

      /* Draw the box arround the selected item */
      if (item->state & TVIS_SELECTED)
      {
        HPEN  hNewPen     = CreatePen(PS_DOT, 0, GetSysColor(COLOR_WINDOWTEXT) );
        HPEN  hOldPen     = SelectObject( hdc, hNewPen );
        INT   rop         = SetROP2(hdc, R2_XORPEN);
        POINT points[5];

        points[4].x = points[0].x = item->text.left;
        points[4].y = points[0].y = item->text.top;
        points[1].x = item->text.right-1 ;
        points[1].y = item->text.top;
        points[2].x = item->text.right-1;
        points[2].y = item->text.bottom-1;
        points[3].x = item->text.left;
        points[3].y = item->text.bottom-1;

        Polyline (hdc,points,5);

        SetROP2(hdc, rop);
        DeleteObject(hNewPen);
        SelectObject(hdc, hOldPen);
      }

      if (oldBkMode != TRANSPARENT)
        SetBkMode(hdc, oldBkMode);
    }
  }

  /* Draw insertion mark if necessary */

  //if (infoPtr->insertMarkItem)
  //              TRACE ("item:%d,mark:%d\n", (int)wineItem->hItem,
  //                             (int) infoPtr->insertMarkItem);
  if (item->hItem == infoPtr->insertMarkItem)
  {
    HPEN hNewPen, hOldPen;
    int offset;

    hNewPen = CreatePen(PS_SOLID, 2, infoPtr->clrInsertMark);
    hOldPen = SelectObject( hdc, hNewPen );

    if (infoPtr->insertBeforeorAfter)
      offset = item->text.top+1;
    else
      offset = item->text.bottom-1;

    MoveToEx(hdc,item->text.left, offset-3, NULL);
    LineTo(hdc,item->text.left, offset+3);

    MoveToEx(hdc,item->text.left,offset,NULL);
    LineTo(hdc,item->text.right-2,offset);

    MoveToEx(hdc,item->text.right-2, offset+3, NULL);
    LineTo(hdc,item->text.right-2, offset-3);

    DeleteObject(hNewPen);

    SelectObject(hdc, hOldPen);
  }

  if (cditem & CDRF_NOTIFYPOSTPAINT)
    cditem = TREEVIEW_SendCustomDrawItemNotify(hwnd, hdc, item, CDDS_ITEMPOSTPAINT);

  SelectObject (hdc, hOldFont);
}

static LRESULT
TREEVIEW_GetItemRect (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *wineItem;
  HTREEITEM     *iItem;
  LPRECT        lpRect   = (LPRECT)lParam;

  /*
   * validate parameters
   */
  if ((infoPtr == NULL) || (lpRect == NULL))
    return FALSE;

  TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE);

  /*
   * retrieve the item ptr
   */
  iItem = (HTREEITEM *) lParam;
  wineItem = TREEVIEW_ValidItem (infoPtr, *iItem);
  if (!wineItem || !wineItem->visible)
    return FALSE;

  /*
   * If wParam is TRUE return the text size otherwise return
   * the whole item size
   */
  if (wParam)
  {
    lpRect->left   = wineItem->text.left;
    lpRect->right  = wineItem->text.right;
    lpRect->bottom = wineItem->text.bottom;
    lpRect->top    = wineItem->text.top;
  } else
  {
    lpRect->left   = wineItem->rect.left;
    lpRect->right  = wineItem->rect.right;
    lpRect->bottom = wineItem->rect.bottom;
    lpRect->top    = wineItem->rect.top;
  }

  return TRUE;
}

static LRESULT
TREEVIEW_GetVisibleCount (HWND hwnd,  WPARAM wParam, LPARAM lParam)

{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return (LRESULT)infoPtr->uVisibleHeight/infoPtr->uRealItemHeight;
}

static LRESULT TREEVIEW_SetItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *wineItem;
  TVITEMEXW *tvItem;
  INT iItem,len;

  if (infoPtr->hwndEdit) SetFocus(hwnd);

  tvItem = (LPTVITEMEXW)lParam;
  iItem = (INT)tvItem->hItem;

  wineItem = TREEVIEW_ValidItem (infoPtr, (HTREEITEM)iItem);
  if (!wineItem) return FALSE;

  if (tvItem->mask & TVIF_CHILDREN)
    wineItem->cChildren = tvItem->cChildren;

  if (tvItem->mask & TVIF_IMAGE)
    wineItem->iImage = tvItem->iImage;

  if (tvItem->mask & TVIF_INTEGRAL)
    wineItem->iIntegral = tvItem->iIntegral;

  if (tvItem->mask & TVIF_PARAM)
    wineItem->lParam = tvItem->lParam;

  if (tvItem->mask & TVIF_SELECTEDIMAGE)
    wineItem->iSelectedImage = tvItem->iSelectedImage;

  if (tvItem->mask & TVIF_STATE)
  {
    wineItem->state &= ~tvItem->stateMask;
    wineItem->state |= (tvItem->state & tvItem->stateMask);
    wineItem->stateMask |= tvItem->stateMask;
  }

  if (tvItem->mask & TVIF_TEXT)
  {
    if (unicode)
    {
      if (tvItem->pszText != LPSTR_TEXTCALLBACKW)
      {
        len = lstrlenW(tvItem->pszText)+1;
        if (len > wineItem->cchTextMax)
        {
          wineItem->pszText= (WCHAR*)COMCTL32_ReAlloc(wineItem->pszText,len*sizeof(WCHAR));
          wineItem->cchTextMax = len;
        }
        lstrcpynW(wineItem->pszText,tvItem->pszText,len);
      } else
      {
        if (wineItem->cchTextMax)
        {
          COMCTL32_Free (wineItem->pszText);
          wineItem->cchTextMax = 0;
        }
        wineItem->pszText = LPSTR_TEXTCALLBACKW;
      }
    } else
    {
      LPTVITEMEXA tvItem;

      tvItem = (LPTVITEMEXA)lParam;

      if (tvItem->pszText != LPSTR_TEXTCALLBACKA)
      {
        len = lstrlenA (tvItem->pszText) + 1;
        if (len > wineItem->cchTextMax)
        {
          wineItem->pszText = (WCHAR*)COMCTL32_ReAlloc(wineItem->pszText,len*sizeof(WCHAR));
          wineItem->cchTextMax = len;
        }
        lstrcpynAtoW(wineItem->pszText,tvItem->pszText,len);
      } else
      {
        if (wineItem->cchTextMax)
        {
          COMCTL32_Free (wineItem->pszText);
          wineItem->cchTextMax = 0;
        }
        wineItem->pszText = LPSTR_TEXTCALLBACKW;
      }
    }
  }

  wineItem->mask |= tvItem->mask;
  wineItem->calculated = FALSE;
  TREEVIEW_QueueRefresh(hwnd);

  return TRUE;
}

static LRESULT
TREEVIEW_GetItemState (HWND hwnd, WPARAM wParam, LPARAM lParam)

{
    TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
    TREEVIEW_ITEM *wineItem;

    wineItem = TREEVIEW_ValidItem (infoPtr, (HTREEITEM)wParam);
    if (!wineItem) return 0;

    return (wineItem->state & lParam);
}

static void TREEVIEW_Refresh(HWND hwnd)
{
  TREEVIEW_UnqueueRefresh(hwnd,TRUE,FALSE);

  InvalidateRect(hwnd,NULL,TRUE);
}

static void TREEVIEW_RefreshItem(HWND hwnd,TREEVIEW_ITEM *item,BOOL wholeLine)
{
  if (item && item->visible)
  {
    RECT rect = item->rect;

    if (wholeLine)
    {
      RECT client;

      GetClientRect(hwnd,&client);
      rect.left = 0;
      rect.right = client.right;
    } else
    {
      DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

      //redraw text and image
      if (((item->mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE)) || (dwStyle & TVS_CHECKBOXES)) && (item->iSelectedImage != item->iImage))
      {
        rect.left += TREEVIEW_LEFT_MARGIN;
        if (item->iLevel != 0) rect.left += (5*item->iLevel);
        rect.left += 15;
      } else rect = item->text;
    }

    InvalidateRect(hwnd,&rect,TRUE);
  }
}

//HDC parameter is optional

static void TREEVIEW_CalcItem(HWND hwnd,HDC hdc,DWORD dwStyle,TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item)
{
  RECT r;
  INT center,xpos;
  BOOL ownDC = FALSE;

  item->calculated = TRUE;

  r      = item->rect;               /* this item rectangle */
  center = (r.top+r.bottom)/2;           /* this item vertical center */
  xpos   = r.left + TREEVIEW_LEFT_MARGIN;/* horizontal starting point */

  if (item->iLevel != 0)/*  update position only for non root node */
    xpos += 5*item->iLevel;

  if ((dwStyle & TVS_HASBUTTONS) && (dwStyle & TVS_HASLINES))
  {
    if (TREEVIEW_HasChildren(hwnd,item))
    {
      /* Setup expand box coordinate to facilitate the LMBClick handling */
      item->expandBox.left   = xpos-4;
      item->expandBox.top    = center-4;
      item->expandBox.right  = xpos+5;
      item->expandBox.bottom = center+5;
    } else SetRectEmpty(&item->expandBox);
  } else SetRectEmpty(&item->expandBox);

  xpos += 13; /* update position */

  if ((item->mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE)) || (dwStyle & TVS_CHECKBOXES))
  {
    INT        imageIndex,cx,cy;
    HIMAGELIST *himlp = NULL;

    /* State images are displayed to the left of the Normal image
     * image number is in state; zero should be `display no image'.
     * FIXME: that last sentence looks like it needs some checking.
     */
    if (infoPtr->himlState)
      himlp = &infoPtr->himlState;
    imageIndex = item->state >> 12;

    if (himlp && imageIndex)
    {
      if (!hdc)
      {
        ownDC = TRUE;
        hdc = GetDC(hwnd);
      }

      imageIndex--;       /* see FIXME */
      ImageList_GetIconSize(*himlp,&cx,&cy);
      item->statebitmap.left = xpos-2;
      item->statebitmap.right = xpos-2+cx;
      item->statebitmap.top = r.top+1;
      item->statebitmap.bottom = r.top+1+cy;
      xpos += cx;
    } else SetRectEmpty(&item->statebitmap);

    /* Now, draw the normal image; can be either selected or
     * non-selected image.
     */

    himlp = NULL;
    if (infoPtr->himlNormal)
      himlp = &infoPtr->himlNormal; /* get the image list */

    if (himlp)
    {
      int ovlIdx = 0;

      if(item->stateMask & TVIS_OVERLAYMASK)
        ovlIdx = item->state & TVIS_OVERLAYMASK;

      if (!hdc)
      {
        ownDC = TRUE;
        hdc = GetDC(hwnd);
      }

      ImageList_GetIconSize(*himlp,&cx,&cy);
      item->bitmap.left = xpos-2;
      item->bitmap.right = xpos-2+cx;
      item->bitmap.top = r.top+1;
      item->bitmap.bottom = r.top+1+cy;
      xpos += cx;
    } else SetRectEmpty(&item->bitmap);
  } else
  {
    SetRectEmpty(&item->statebitmap);
    SetRectEmpty(&item->bitmap);
  }

  r.left = xpos;
  if ((item->mask & TVIF_TEXT) && (item->pszText))
  {
    UINT  uTextJustify = DT_LEFT;
    HFONT hOldFont;

    r.left += 3;
    r.right -= 3;

    item->text.left  = r.left;
    item->text.right = r.right;
    item->text.top   = r.top;
    item->text.bottom= r.bottom;

    if (item->pszText == LPSTR_TEXTCALLBACKW)
      TREEVIEW_SendDispInfoNotify (hwnd, item, TVN_GETDISPINFO, TVIF_TEXT);

    if (!hdc)
    {
      ownDC = TRUE;
      hdc = GetDC(hwnd);
    }

    if (item->state & TVIS_BOLD)
      hOldFont = SelectObject (hdc, infoPtr->hBoldFont);
    else
      hOldFont = SelectObject (hdc, infoPtr->hFont);

    /* Obtain the text coordinate */
    DrawTextW (
            hdc,
            item->pszText,
            lstrlenW(item->pszText),
            &item->text,
            uTextJustify | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);

    SelectObject(hdc,hOldFont);

    /* We need to reset it to items height */
    item->text.top = r.top;
    item->text.bottom = r.bottom;
    item->text.right += 4; /* This is extra for focus rectangle */

    xpos = item->text.right;
  } else SetRectEmpty(&item->text);

  item->rect.right = xpos;

  if (ownDC) ReleaseDC(hwnd,hdc);
}

//HDC parameter is optional

static BOOL TREEVIEW_CalcItems(HWND hwnd,HDC hdc,TREEVIEW_INFO *infoPtr)
{
  TREEVIEW_ITEM *item;
  INT iItem, indent,x,y,height,itemHeight;
  TEXTMETRICA tm;
  RECT rect,view;
  BOOL ownDC = FALSE,changedLeftTop = FALSE;
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  INT maxXScroll,maxYScroll;
  INT xDiff,yDiff;
  HFONT oldFont;

  GetClientRect(hwnd,&rect);
  infoPtr->uVisibleHeight = rect.bottom-rect.top;
  infoPtr->uVisibleWidth = rect.right-rect.left;

  if (infoPtr->uInternalStatus & TV_CALCALL)
  {
    itemHeight = 0;
    ImageList_GetIconSize (infoPtr->himlNormal, &x, &itemHeight);
    itemHeight = MAX(infoPtr->uItemHeight,itemHeight);

    if (!hdc)
    {
      ownDC = TRUE;
      hdc = GetDC(hwnd);
    }

    oldFont = SelectObject(hdc,infoPtr->hFont);
    GetTextMetricsA(hdc,&tm);
    itemHeight = MAX(tm.tmHeight+tm.tmExternalLeading,itemHeight);
    SelectObject(hdc,infoPtr->hBoldFont);
    GetTextMetricsA(hdc,&tm);
    itemHeight = MAX(tm.tmHeight+tm.tmExternalLeading,itemHeight);
    SelectObject(hdc,oldFont);
    if (!(dwStyle & TVS_NONEVENHEIGHT))
      if (itemHeight & 0x1) itemHeight++; //important for PS_DOT pen!
    infoPtr->uRealItemHeight = itemHeight;
    infoPtr->uVScrollStep = itemHeight+ITEM_VSPACE;
  } else
  {
    itemHeight = infoPtr->uRealItemHeight;
  }
  infoPtr->uTotalWidth = 0;

  view = rect;
  OffsetRect(&view,infoPtr->lefttop.x,infoPtr->lefttop.y);

  iItem = (INT)infoPtr->TopRootItem;
  infoPtr->firstVisible = 0;
  item = NULL;
  indent = x = y = 0;

  while (iItem)
  {
    item = &infoPtr->items[iItem];
    item->iLevel = indent;

    height = itemHeight*item->iIntegral+ITEM_VSPACE;

    //calculate size and fill rects
    if ((infoPtr->uInternalStatus & TV_CALCALL) || !item->calculated)
    {
      item->rect.top    = y-view.top;
      item->rect.bottom = item->rect.top+height;
      item->rect.left   = x-view.left;
      item->rect.right  = rect.right; //dummy
      if (!hdc)
      {
        ownDC = TRUE;
        hdc = GetDC(hwnd);
      }
      TREEVIEW_CalcItem(hwnd,hdc,dwStyle,infoPtr,item);
    } else
    {
      INT xOffset,yOffset;

      xOffset = (x-view.left)-item->rect.left;
      yOffset = (y-view.top)-item->rect.top;
      OffsetRect(&item->rect,xOffset,yOffset);
      OffsetRect(&item->text,xOffset,yOffset);
      OffsetRect(&item->expandBox,xOffset,yOffset);
      OffsetRect(&item->bitmap,xOffset,yOffset);
      OffsetRect(&item->statebitmap,xOffset,yOffset);
    }
    infoPtr->uTotalWidth = MAX(infoPtr->uTotalWidth,item->rect.right+infoPtr->lefttop.x);

    if (((y >= view.top) && (y < view.bottom)) || ((y+height > view.top) && (y+height <= view.bottom)))
    {
      item->visible = TRUE;
      if (!infoPtr->firstVisible)
        infoPtr->firstVisible = item->hItem;
    } else item->visible = FALSE;

    /* look up next item */

    if ((item->firstChild) && (item->state & TVIS_EXPANDED))
    {
      iItem = (INT)item->firstChild;
      indent++;
      x += infoPtr->uIndent;
    } else
    {
      iItem = (INT)item->sibling;
      while ((!iItem) && (indent > 0))
      {
        indent--;
        x -= infoPtr->uIndent;
        item = &infoPtr->items[(INT)item->parent];
        iItem = (INT)item->sibling;
      }
    }
    y += height;
  } /* while */

  if (ownDC) ReleaseDC(hwnd,hdc);

  infoPtr->uInternalStatus &= ~TV_CALCALL;
  infoPtr->uTotalHeight = y;

  //check cx and cy
  yDiff = xDiff = 0;

  maxYScroll = infoPtr->uTotalHeight-infoPtr->uVisibleHeight;
  if (maxYScroll < 0) maxYScroll = 0;
  if (infoPtr->lefttop.y > maxYScroll)
  {
    INT mod = maxYScroll % infoPtr->uVScrollStep;

    if (mod > 0) maxYScroll += infoPtr->uVScrollStep-mod;
    yDiff = infoPtr->lefttop.y-maxYScroll;
    infoPtr->lefttop.y = maxYScroll;
  }

  maxXScroll = infoPtr->uTotalWidth-infoPtr->uVisibleWidth;
  if (maxXScroll < 0) maxXScroll = 0;
  if (infoPtr->lefttop.x > maxXScroll)
  {
    xDiff = infoPtr->lefttop.x-maxXScroll;
    infoPtr->lefttop.x = maxXScroll;
  }

  changedLeftTop = xDiff || yDiff;
  if (changedLeftTop)
  {
    iItem = (INT)infoPtr->TopRootItem;
    item = NULL;
    indent = 0;
    infoPtr->firstVisible = 0;

    while (iItem)
    {
      item = &infoPtr->items[iItem];
      OffsetRect(&item->rect,xDiff,yDiff);
      OffsetRect(&item->text,xDiff,yDiff);
      OffsetRect(&item->expandBox,xDiff,yDiff);
      OffsetRect(&item->bitmap,xDiff,yDiff);
      OffsetRect(&item->statebitmap,xDiff,yDiff);

      y = view.top+item->rect.top;
      if (((y >= view.top) && (y < view.bottom)) || ((y+height > view.top) && (y+height <= view.bottom)))
      {
        item->visible = TRUE;
        if (!infoPtr->firstVisible)
          infoPtr->firstVisible = item->hItem;
      } else item->visible = FALSE;

      if ((item->firstChild) && (item->state & TVIS_EXPANDED))
      {
        iItem = (INT)item->firstChild;
        indent++;
      } else
      {
        iItem = (INT)item->sibling;
        while ((!iItem) && (indent > 0))
        {
          indent--;
          item = &infoPtr->items[(INT)item->parent];
          iItem = (INT)item->sibling;
        }
      }
    } /* while */
  }

  if (!(dwStyle & TVS_NOSCROLL) && (infoPtr->uVisibleHeight > 0) && (infoPtr->uVisibleWidth > 0))
  {
    if (infoPtr->uTotalHeight > infoPtr->uVisibleHeight)
    {
      SCROLLINFO info;
      INT visH = ((INT)(infoPtr->uVisibleHeight/infoPtr->uVScrollStep))*infoPtr->uVScrollStep;

      info.cbSize = sizeof(info);
      info.nMin   = 0;
      info.nMax   = infoPtr->uTotalHeight-1;
      info.nPos   = infoPtr->lefttop.y;
      info.nPage  = visH;
      info.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
      infoPtr->uInternalStatus |= TV_VSCROLL;
      SetScrollInfo(hwnd,SB_VERT,&info,TRUE);
    } else
    {
      if (infoPtr->uInternalStatus & TV_VSCROLL)
        ShowScrollBar(hwnd,SB_VERT,FALSE);
      infoPtr->uInternalStatus &= ~TV_VSCROLL;
    }
    if (!(dwStyle & TVS_NOHSCROLL) && (infoPtr->uTotalWidth > infoPtr->uVisibleWidth))
    {
      SCROLLINFO info;

      info.cbSize = sizeof(info);
      info.nMin   = 0;
      info.nMax   = infoPtr->uTotalWidth-1;
      info.nPos   = infoPtr->lefttop.x;
      info.nPage  = MAX(infoPtr->uVisibleWidth,1);
      info.fMask  = SIF_RANGE | SIF_POS | SIF_PAGE;
      infoPtr->uInternalStatus |= TV_HSCROLL;
      SetScrollInfo(hwnd,SB_HORZ,&info,TRUE);
    } else
    {
      if (infoPtr->uInternalStatus & TV_HSCROLL)
        ShowScrollBar(hwnd,SB_HORZ,FALSE);
      infoPtr->uInternalStatus &= ~TV_HSCROLL;
    }
  } else
  {
    if (infoPtr->uInternalStatus & (TV_VSCROLL | TV_HSCROLL))
      ShowScrollBar(hwnd,SB_BOTH,FALSE);
    infoPtr->uInternalStatus &= ~(TV_VSCROLL | TV_HSCROLL);
  }

  return changedLeftTop;
}

static void
TREEVIEW_Draw(HWND hwnd,HDC hdc,RECT *updateRect)
{
    TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    HBRUSH hbrBk;
    RECT rect;
    TREEVIEW_ITEM *item;
    INT iItem,indent;
    BOOL visFound = FALSE;
    HPEN hNewPen,hOldPen;

    TREEVIEW_UnqueueRefresh(hwnd,TRUE,FALSE);

    GetClientRect (hwnd, &rect);
    if ((rect.left == rect.right) || (rect.top == rect.bottom)) return;

    infoPtr->cdmode = TREEVIEW_SendCustomDrawNotify(hwnd, CDDS_PREPAINT, hdc, rect);

    if (infoPtr->cdmode == CDRF_SKIPDEFAULT) return;

    //draw items

    hNewPen = CreatePen(PS_DOT,0,infoPtr->clrLine);

    iItem = (INT)infoPtr->TopRootItem;
    indent = 0;

    while (iItem)
    {
      item = &infoPtr->items[iItem];
      if (item->visible)
      {
        if (updateRect && IntersectRect(NULL,&item->rect,updateRect))
        {
          TREEVIEW_DrawItem(hwnd,hdc,item,dwStyle,infoPtr);
          visFound = TRUE;
        }
      } else if (visFound) break;
      if (!visFound && (dwStyle & TVS_HASLINES) && (dwStyle & TVS_LINESATROOT))
      {
        //draw vertical connections
        hOldPen = SelectObject(hdc,hNewPen);
        TREEVIEW_DrawVLines(hdc,infoPtr,item);
        SelectObject(hdc,hOldPen);
      }
      if (item->firstChild && (item->state & TVIS_EXPANDED))
      {
        iItem = (INT)item->firstChild;
        indent++;
      } else
      {
        iItem = (INT)item->sibling;
        while (!iItem && (indent > 0))
        {
          item = &infoPtr->items[(INT)item->parent];
          iItem = (INT)item->sibling;
          indent--;
        }
      }
    }

    DeleteObject(hNewPen);

    if (infoPtr->cdmode & CDRF_NOTIFYPOSTPAINT)
      infoPtr->cdmode = TREEVIEW_SendCustomDrawNotify(hwnd, CDDS_POSTPAINT, hdc, rect);
}

static LRESULT TREEVIEW_SetRedraw(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  if (wParam)
  {
    if (!(infoPtr->uInternalStatus & TV_NOREDRAW)) return 0;
    infoPtr->uInternalStatus &= ~TV_NOREDRAW;
    TREEVIEW_CalcItems(hwnd,0,infoPtr);
    TREEVIEW_Refresh(hwnd);
  } else
  {
    infoPtr->uInternalStatus |= TV_NOREDRAW;
  }

  return 0;
}


static LRESULT
TREEVIEW_HandleTimer (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  switch (wParam)
  {
    case TV_REFRESH_TIMER:
      KillTimer (hwnd, TV_REFRESH_TIMER);
      infoPtr->Timer &= ~TV_REFRESH_TIMER_SET;
      TREEVIEW_CalcItems(hwnd,0,infoPtr);
      TREEVIEW_Refresh(hwnd);
      return 0;

    case TV_EDIT_TIMER:
      KillTimer (hwnd, TV_EDIT_TIMER);
      infoPtr->Timer &= ~TV_EDIT_TIMER_SET;
      if (infoPtr->editItem)
        TREEVIEW_EditLabel(hwnd,infoPtr->editItem,TRUE);
      return 0;

    case TV_ISEARCH_TIMER:
      KillTimer(hwnd,TV_ISEARCH_TIMER);
      infoPtr->Timer &= ~TV_ISEARCH_TIMER_SET;
      return 0;
 }

 return 1;
}

static void TREEVIEW_QueueRefresh(HWND hwnd)

{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  if (infoPtr->Timer & TV_REFRESH_TIMER_SET)
    KillTimer (hwnd, TV_REFRESH_TIMER);

  if (infoPtr->uInternalStatus & TV_NOREDRAW)
  {
    infoPtr->Timer &= ~TV_REFRESH_TIMER_SET;

    return;
  }

  SetTimer (hwnd, TV_REFRESH_TIMER, TV_REFRESH_DELAY, 0);
  infoPtr->Timer |= TV_REFRESH_TIMER_SET;
}

static BOOL TREEVIEW_UnqueueRefresh(HWND hwnd,BOOL calc,BOOL refresh)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  if (infoPtr->Timer & TV_REFRESH_TIMER_SET)
  {
    KillTimer (hwnd, TV_REFRESH_TIMER);
    infoPtr->Timer &= ~TV_REFRESH_TIMER_SET;
    if (calc) TREEVIEW_CalcItems(hwnd,0,infoPtr);
    if (refresh) TREEVIEW_Refresh(hwnd);

    return TRUE;
  }

  return FALSE;
}

static LRESULT
TREEVIEW_GetItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  LPTVITEMEXW    tvItem;
  TREEVIEW_ITEM *wineItem;
  INT         iItem;

  tvItem = (LPTVITEMEXW)lParam;
  iItem = (INT)tvItem->hItem;

  wineItem = TREEVIEW_ValidItem (infoPtr, (HTREEITEM)iItem);
  if (!wineItem) return FALSE;

  if (tvItem->mask & TVIF_CHILDREN)
    tvItem->cChildren = TREEVIEW_HasChildren(hwnd, wineItem);

  if (tvItem->mask & TVIF_HANDLE)
    tvItem->hItem = wineItem->hItem;

  if (tvItem->mask & TVIF_IMAGE)
    tvItem->iImage = wineItem->iImage;

  if (tvItem->mask & TVIF_INTEGRAL)
    tvItem->iIntegral = wineItem->iIntegral;

  // undocumented: windows ignores TVIF_PARAM and
  // always sets lParam
  tvItem->lParam = wineItem->lParam;

  if (tvItem->mask & TVIF_SELECTEDIMAGE)
    tvItem->iSelectedImage = wineItem->iSelectedImage;

  if (tvItem->mask & TVIF_STATE)
    tvItem->state = wineItem->state & tvItem->stateMask;

  if (tvItem->mask & TVIF_TEXT)
  {
    if (wineItem->pszText == LPSTR_TEXTCALLBACKW)
      TREEVIEW_SendDispInfoNotify(hwnd,wineItem,TVN_GETDISPINFO,TVIF_TEXT);
    if (unicode)
    {
      if (wineItem->pszText == LPSTR_TEXTCALLBACKW)
        tvItem->pszText = LPSTR_TEXTCALLBACKW;
      else if (wineItem->pszText)
        lstrcpynW(tvItem->pszText,wineItem->pszText,tvItem->cchTextMax);
    } else
    {
      LPTVITEMEXA tvItem = (LPTVITEMEXA)lParam;

      if (wineItem->pszText == LPSTR_TEXTCALLBACKW)
        tvItem->pszText = LPSTR_TEXTCALLBACKA;
      else if (wineItem->pszText)
        lstrcpynWtoA(tvItem->pszText, wineItem->pszText,tvItem->cchTextMax);
    }
  }

  return TRUE;
}



/* FIXME: check implementation of TVGN_NEXT/TVGN_NEXTVISIBLE */

static LRESULT
TREEVIEW_GetNextItem (HWND hwnd, WPARAM wParam, LPARAM lParam)

{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *wineItem, *returnItem;
  INT iItem, retval, flag;

  flag  = (INT) wParam;
  iItem = (INT) lParam;
  retval = 0;
  switch (flag)
  {
    case TVGN_CHILD: /* Special case: child of 0 is root */
      if (iItem) break;

    case TVGN_ROOT:
      retval = (INT)infoPtr->TopRootItem;
      break;

    case TVGN_CARET:
      retval = (INT)infoPtr->selectedItem;
      break;

    case TVGN_FIRSTVISIBLE:
      TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE);
      retval = (INT)infoPtr->firstVisible;
      break;

    case TVGN_DROPHILITE:
      retval = (INT)infoPtr->dropItem;
      break;
  }

  if (retval) return retval;

  wineItem = TREEVIEW_ValidItem (infoPtr, (HTREEITEM)iItem);
  returnItem = NULL;
  if (!wineItem) return FALSE;

  switch (flag)
  {
    case TVGN_NEXT:
      retval = (INT)wineItem->sibling;
      break;

    case TVGN_PREVIOUS:
      retval = (INT)wineItem->upsibling;
      break;

    case TVGN_PARENT:
      retval = (INT)wineItem->parent;
      break;

    case TVGN_CHILD:
      retval = (INT)wineItem->firstChild;
      break;

    case TVGN_LASTVISIBLE:
      returnItem = TREEVIEW_GetLastListItem (hwnd,infoPtr,wineItem);
      break;

    case TVGN_NEXTVISIBLE:
      returnItem = TREEVIEW_GetNextListItem (hwnd,infoPtr,wineItem);
      break;

    case TVGN_PREVIOUSVISIBLE:
      returnItem = TREEVIEW_GetPrevListItem (hwnd,infoPtr, wineItem);
      break;

    default:
      break;
  }

  if (returnItem) return (INT)returnItem->hItem;

  return retval;
}


static LRESULT
TREEVIEW_GetCount (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return (LRESULT) infoPtr->uNumItems;
}

/***************************************************************************
 * This method does the chaining of the insertion of a treeview item
 * before an item.
 * If parent is NULL, we're inserting at the root of the list.
 */
static void TREEVIEW_InsertBefore(
    TREEVIEW_INFO *infoPtr,
    TREEVIEW_ITEM *newItem,
    TREEVIEW_ITEM *sibling,
    TREEVIEW_ITEM *parent)
{
  HTREEITEM     siblingHandle   = 0;
  HTREEITEM     upSiblingHandle = 0;
  TREEVIEW_ITEM *upSibling      = NULL;

//  if (newItem == NULL)
//    ERR(treeview, "NULL newItem, impossible condition\n");

//  if (parent == NULL)
//    ERR(treeview, "NULL parent, impossible condition\n");

  if (sibling != NULL) /* Insert before this sibling for this parent */
  {
    /* Store the new item sibling up sibling and sibling tem handle */
    siblingHandle   = sibling->hItem;
    upSiblingHandle = sibling->upsibling;
    /* As well as a pointer to the upsibling sibling object */
    if ( (INT)sibling->upsibling != 0 )
      upSibling = &infoPtr->items[(INT)sibling->upsibling];

    /* Adjust the sibling pointer */
    sibling->upsibling = newItem->hItem;

    /* Adjust the new item pointers */
    newItem->upsibling = upSiblingHandle;
    newItem->sibling   = siblingHandle;

    /* Adjust the up sibling pointer */
    if ( upSibling != NULL )
      upSibling->sibling = newItem->hItem;
    else
    if (parent)
      /* this item is the first child of this parent, adjust parent pointers */
      parent->firstChild = newItem->hItem;
    else infoPtr->TopRootItem = newItem->hItem;
  }
  else /* Insert as first child of this parent */
  if (parent)
    parent->firstChild = newItem->hItem;
}

/***************************************************************************
 * This method does the chaining of the insertion of a treeview item
 * If parent is NULL, we're inserting at the root of the list.
 * after an item.
 */
static void TREEVIEW_InsertAfter(
    TREEVIEW_INFO *infoPtr,
    TREEVIEW_ITEM *newItem,
    TREEVIEW_ITEM *upSibling,
    TREEVIEW_ITEM *parent)
{
  HTREEITEM     upSiblingHandle = 0;
  HTREEITEM     siblingHandle   = 0;
  TREEVIEW_ITEM *sibling        = NULL;

//  if (newItem == NULL)
//    ERR(treeview, "NULL newItem, impossible condition\n");

//  if (parent == NULL)
//    ERR(treeview, "NULL parent, impossible condition\n");

  if (upSibling != NULL) /* Insert after this upsibling for this parent */
  {
    /* Store the new item up sibling and sibling item handle */
    upSiblingHandle = upSibling->hItem;
    siblingHandle   = upSibling->sibling;
    /* As well as a pointer to the upsibling sibling object */
    if ( (INT)upSibling->sibling != 0 )
      sibling = &infoPtr->items[(INT)upSibling->sibling];

    /* Adjust the up sibling pointer */
    upSibling->sibling = newItem->hItem;

    /* Adjust the new item pointers */
    newItem->upsibling = upSiblingHandle;
    newItem->sibling   = siblingHandle;

    /* Adjust the sibling pointer */
    if ( sibling != NULL )
      sibling->upsibling = newItem->hItem;
    /*
    else
      newItem is the last of the level, nothing else to do
    */
  }
  else /* Insert as first child of this parent */
  if (parent)
    parent->firstChild = newItem->hItem;
}

/***************************************************************************
 * Forward the DPA local callback to the treeview owner callback
 */
static INT WINAPI TREEVIEW_CallBackCompare(LPVOID first,LPVOID second,LPARAM tvInfoPtr)
{
  /* Forward the call to the client define callback */
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr((HWND)tvInfoPtr);

  return (infoPtr->pCallBackSort->lpfnCompare)(
    ((TREEVIEW_ITEM*)first)->lParam,
    ((TREEVIEW_ITEM*)second)->lParam,
    infoPtr->pCallBackSort->lParam);
}

/***************************************************************************
 * Treeview native sort routine: sort on item text.
 */
static INT WINAPI TREEVIEW_SortOnName(LPVOID first,LPVOID second,LPARAM tvInfoPtr)
{
  HWND hwnd = (HWND)tvInfoPtr;
  WCHAR *txt1,*txt2;
  TREEVIEW_ITEM *item;

  item = (TREEVIEW_ITEM *) first;
  if (item->pszText == LPSTR_TEXTCALLBACKW)
    TREEVIEW_SendDispInfoNotify (hwnd, item, TVN_GETDISPINFO, TVIF_TEXT);

  txt1 = item->pszText;

  item = (TREEVIEW_ITEM *) second;
  if (item->pszText == LPSTR_TEXTCALLBACKW)
    TREEVIEW_SendDispInfoNotify (hwnd, item, TVN_GETDISPINFO, TVIF_TEXT);

  txt2 = item->pszText;

  return lstrcmpiW(txt1,txt2); //CB: or lstrcmpW?
}

/***************************************************************************
 * Setup the treeview structure with regards of the sort method
 * and sort the children of the TV item specified in lParam
 * fRecurse: currently unused. Should be zero.
 * parent: if pSort!=NULL, should equal pSort->hParent.
 *         otherwise, item which child items are to be sorted.
 * pSort:  sort method info. if NULL, sort on item text.
 *         if non-NULL, sort on item's lParam content, and let the
 *         application decide what that means. See also TVM_SORTCHILDRENCB.
 */

static LRESULT WINAPI TREEVIEW_Sort(HWND hwnd,BOOL fRecurse,HTREEITEM parent,LPTVSORTCB pSort)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *sortMe  = NULL; /* Node for which we sort the children */
  INT            cChildren;
  HTREEITEM      hti;
  BOOL root = FALSE;

  /* Obtain the TVSORTBC struct */
  infoPtr->pCallBackSort = pSort;

  /* undocumented feature: TVI_ROOT means `sort the whole tree' */

  if ((parent == TVI_ROOT) || (parent == 0))
  {
    root = TRUE;
    parent = infoPtr->TopRootItem;
  }

  /* Check for a valid handle to the parent item */
  sortMe = TREEVIEW_ValidItem(infoPtr,parent);
  if (!sortMe)
    return FALSE;

  cChildren = 0;
  for(hti = root ? sortMe->hItem:sortMe->firstChild; hti; hti = infoPtr->items[(INT)hti].sibling)
      cChildren++;

  /* Make sure there is something to sort */
  if (cChildren > 1)
  {
    /* pointer organization */
    HDPA          sortList   = DPA_Create(cChildren);
    HTREEITEM     itemHandle = root ? sortMe->hItem:sortMe->firstChild;
    TREEVIEW_ITEM *itemPtr   = &infoPtr->items[(INT)itemHandle];

    /* TREEVIEW_ITEM rechaining */
    INT  count     = 0;
    VOID *item     = 0;
    VOID *nextItem = 0;
    VOID *prevItem = 0;

    /* Build the list of item to sort */
    do
    {
      itemPtr    = &infoPtr->items[(INT)itemHandle];
      DPA_InsertPtr(
        sortList,      /* the list */
        cChildren+1,   /* force the insertion to be an append */
        itemPtr);      /* the ptr to store */

      /* Get the next sibling */
      itemHandle = itemPtr->sibling;
    } while (itemHandle != NULL);

    /* let DPA perform the sort activity */
        if (pSort)
        DPA_Sort(
                sortList,                  /* what  */
                TREEVIEW_CallBackCompare,  /* how   */
                hwnd);                     /* owner */
        else
                DPA_Sort (
                        sortList,            /* what  */
                        TREEVIEW_SortOnName, /* how   */
                        hwnd);               /* owner */

    /*
     * Reorganized TREEVIEW_ITEM structures.
     * Note that we know we have at least two elements.
     */

    /* Get the first item and get ready to start... */
    item = DPA_GetPtr(sortList, count++);
    while ((nextItem = DPA_GetPtr(sortList, count++)) != NULL)
    {
      /* link the two current item toghether */
      ((TREEVIEW_ITEM*)item)->sibling       = ((TREEVIEW_ITEM*)nextItem)->hItem;
      ((TREEVIEW_ITEM*)nextItem)->upsibling = ((TREEVIEW_ITEM*)item)->hItem;

      if (prevItem == NULL) /* this is the first item, update the parent */
      {
        if (root)
          infoPtr->TopRootItem = ((TREEVIEW_ITEM*)item)->hItem;
        else
          sortMe->firstChild = ((TREEVIEW_ITEM*)item)->hItem;
        ((TREEVIEW_ITEM*)item)->upsibling = NULL;
      }
      else                  /* fix the back chaining */
      {
        ((TREEVIEW_ITEM*)item)->upsibling = ((TREEVIEW_ITEM*)prevItem)->hItem;
      }

      /* get ready for the next one */
      prevItem = item;
      item     = nextItem;
    }

    /* the last item is pointed to by item and never has a sibling */
    ((TREEVIEW_ITEM*)item)->sibling = NULL;

    DPA_Destroy(sortList);
    TREEVIEW_QueueRefresh(hwnd);

    return TRUE;
  }
  return FALSE;
}

/***************************************************************************
 * Setup the treeview structure with regards of the sort method
 * and sort the children of the TV item specified in lParam
 */
static LRESULT WINAPI TREEVIEW_SortChildrenCB(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LPTVSORTCB pSort = (LPTVSORTCB) lParam;

  return TREEVIEW_Sort (hwnd, wParam, pSort->hParent, pSort);
}


/***************************************************************************
 * Sort the children of the TV item specified in lParam.
 */
static LRESULT WINAPI TREEVIEW_SortChildren(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return TREEVIEW_Sort (hwnd, (BOOL) wParam, (HTREEITEM) lParam, NULL);
}


int ffs(int mask)
{
        int bit;

        if (mask == 0)
                return(0);
        for (bit = 1; !(mask & 1); bit++)
                mask >>= 1;
        return(bit);
}

/* the method used below isn't the most memory-friendly, but it avoids
   a lot of memory reallocations */

/* BTW: we waste handle 0; 0 is not an allowed handle. */

static LRESULT
TREEVIEW_InsertItem(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)

{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TVINSERTSTRUCTW *ptdi;
  TVITEMEXW     *tvItem;
  TREEVIEW_ITEM *wineItem, *parentItem, *prevsib, *sibItem;
  INT           iItem,i,len;

  if (infoPtr->hwndEdit) SetFocus(hwnd);

  /* Item to insert */
  ptdi = (LPTVINSERTSTRUCTW)lParam;

  /* check if memory is available */

  if (infoPtr->uNumPtrsAlloced==0) {
        infoPtr->items = (TREEVIEW_ITEM*)COMCTL32_Alloc (TVITEM_ALLOC*sizeof (TREEVIEW_ITEM));
        infoPtr->freeList= (INT*)COMCTL32_Alloc (((TVITEM_ALLOC>>5)) * sizeof (INT));
        infoPtr->uNumPtrsAlloced=TVITEM_ALLOC;
        infoPtr->TopRootItem=(HTREEITEM)1;
   }

  /*
   * Reallocate contiguous space for items
   */
  if (infoPtr->uNumItems == (infoPtr->uNumPtrsAlloced-1) ) {
        TREEVIEW_ITEM *oldItems = infoPtr->items;
        INT *oldfreeList = infoPtr->freeList;

        infoPtr->uNumPtrsAlloced*=2;
    infoPtr->items = (TREEVIEW_ITEM*)COMCTL32_Alloc (infoPtr->uNumPtrsAlloced*sizeof (TREEVIEW_ITEM));
    infoPtr->freeList= (INT*)COMCTL32_Alloc (((infoPtr->uNumPtrsAlloced>>5))*sizeof (INT));

    memcpy (&infoPtr->items[0], &oldItems[0],
                    infoPtr->uNumPtrsAlloced/2 * sizeof(TREEVIEW_ITEM));
    memcpy (&infoPtr->freeList[0], &oldfreeList[0],
                    (infoPtr->uNumPtrsAlloced>>6) * sizeof(INT));

    COMCTL32_Free (oldItems);
    COMCTL32_Free (oldfreeList);
   }

  /*
   * Reset infoPtr structure with new stat according to current TV picture
   */
  iItem=0;
  infoPtr->uNumItems++;
  if ((INT)infoPtr->uMaxHandle==(infoPtr->uNumItems-1))  {
        iItem=infoPtr->uNumItems;
        infoPtr->uMaxHandle = (HTREEITEM)((INT)infoPtr->uMaxHandle + 1);
  } else {                                       /* check freelist */
        for (i=0; i<infoPtr->uNumPtrsAlloced>>5; i++) {
                if (infoPtr->freeList[i]) {
                        for(iItem = 0; iItem < 32; iItem++)
                        {
                           if(tv_test_bit(iItem, &infoPtr->freeList[i]))
                              break;
                        }
                        tv_clear_bit(iItem,&infoPtr->freeList[i]);
                        iItem+=i<<5;
                        break;
                }
    }
  }

  /*
   * Find the parent item of the new item
   */
  tvItem = &ptdi->itemex;
  wineItem=& infoPtr->items[iItem];

  if ((ptdi->hParent==TVI_ROOT) || (ptdi->hParent==0)) {
    parentItem       = NULL;
    wineItem->parent = 0;
    sibItem          = &infoPtr->items [(INT)infoPtr->TopRootItem];
  }
  else  {
        parentItem = &infoPtr->items[(INT)ptdi->hParent];

    /* Do the insertion here it if it's the only item of this parent */
        if (!parentItem->firstChild)
                parentItem->firstChild=(HTREEITEM)iItem;

        wineItem->parent = ptdi->hParent;
        sibItem          = &infoPtr->items [(INT)parentItem->firstChild];
  }


  /* NOTE: I am moving some setup of the wineItem object that was initialy
   *       done at the end of the function since some of the values are
   *       required by the Callback sorting
   */

  if (tvItem->mask & TVIF_TEXT)
  {
    /*
     * Setup the item text stuff here since it's required by the Sort method
     * when the insertion are ordered
     */
    if (unicode)
    {
      if (tvItem->pszText != LPSTR_TEXTCALLBACKW)
      {
        //TRACE (treeview,"(%p,%s)\n", &tvItem->pszText, tvItem->pszText);
        len = lstrlenW(tvItem->pszText)+1;
        wineItem->pszText = (WCHAR*)COMCTL32_Alloc(len*sizeof(WCHAR));
        lstrcpyW (wineItem->pszText, tvItem->pszText);
        wineItem->cchTextMax = len;
      } else
      {
        //TRACE (treeview,"LPSTR_TEXTCALLBACK\n");
        wineItem->pszText = LPSTR_TEXTCALLBACKW;
        wineItem->cchTextMax = 0;
      }
    } else
    {
      TVITEMEXA *tvItemA = (LPTVITEMEXA)tvItem;

      if (tvItemA->pszText != LPSTR_TEXTCALLBACKA)
      {
        //TRACE (treeview,"(%p,%s)\n", &tvItem->pszText, tvItem->pszText);
        len = lstrlenA(tvItemA->pszText)+1;
        wineItem->pszText= (WCHAR*)COMCTL32_Alloc (len*sizeof(WCHAR));
        lstrcpyAtoW (wineItem->pszText, tvItemA->pszText);
        wineItem->cchTextMax = len;
      } else
      {
        //TRACE (treeview,"LPSTR_TEXTCALLBACK\n");
        wineItem->pszText = LPSTR_TEXTCALLBACKW;
        wineItem->cchTextMax = 0;
      }
    }
  }

  if (tvItem->mask & TVIF_PARAM)
    wineItem->lParam=tvItem->lParam;


  wineItem->upsibling=0;  /* needed in case we're the first item in a list */
  wineItem->sibling=0;
  wineItem->firstChild=0;
  wineItem->hItem=(HTREEITEM)iItem;

  if (sibItem != wineItem)
  {
    prevsib=NULL;

    if (ptdi->hInsertAfter == TVI_FIRST)
    {
      if (wineItem->parent) {
        wineItem->sibling=parentItem->firstChild;
        parentItem->firstChild=(HTREEITEM)iItem;
      } else {
        wineItem->sibling=infoPtr->TopRootItem;
        infoPtr->TopRootItem=(HTREEITEM)iItem;
      }
      sibItem->upsibling=(HTREEITEM)iItem;
    } else if (ptdi->hInsertAfter == TVI_SORT)
    {
      TREEVIEW_ITEM *aChild;

      TREEVIEW_ITEM *previousChild = NULL;
      BOOL bItemInserted           = FALSE;

      if (parentItem)
        aChild = &infoPtr->items[(INT)parentItem->firstChild];
      else
        aChild = &infoPtr->items[(INT)infoPtr->TopRootItem];

      /* lookup the text if using LPSTR_TEXTCALLBACKs */
      if (wineItem->pszText==LPSTR_TEXTCALLBACKW) {
        TREEVIEW_SendDispInfoNotify (hwnd, wineItem, TVN_GETDISPINFO, TVIF_TEXT);
      }

      /* Iterate the parent children to see where we fit in */
      while ( aChild != NULL )
      {
        INT comp;

        /* lookup the text if using LPSTR_TEXTCALLBACKs */
        if (aChild->pszText==LPSTR_TEXTCALLBACKW) {
          TREEVIEW_SendDispInfoNotify (hwnd, aChild, TVN_GETDISPINFO, TVIF_TEXT);
        }

        comp = lstrcmpW(wineItem->pszText, aChild->pszText);
        if ( comp < 0 )  /* we are smaller than the current one */
        {
          TREEVIEW_InsertBefore(infoPtr, wineItem, aChild, parentItem);
          bItemInserted = TRUE;
          break;
        }
        else if ( comp > 0 )  /* we are bigger than the current one */
        {
          previousChild = aChild;
          aChild = (aChild->sibling == 0)  /* This will help us to exit   */
                      ? NULL               /* if there is no more sibling */
                      : &infoPtr->items[(INT)aChild->sibling];

          /* Look at the next item */
          continue;
        }
        else if ( comp == 0 )
        {
          /*
           * An item with this name is already existing, therefore,
           * we add after the one we found
           */
          TREEVIEW_InsertAfter(infoPtr, wineItem, aChild, parentItem);
          bItemInserted = TRUE;
          break;
        }
      }

      /*
       * we reach the end of the child list and the item as not
       * yet been inserted, therefore, insert it after the last child.
       */
      if ( (! bItemInserted ) && (aChild == NULL) )
        TREEVIEW_InsertAfter(infoPtr, wineItem, previousChild, parentItem);
    } else if (ptdi->hInsertAfter == TVI_LAST)
    {
TVI_LAST_CASE:
      while (sibItem->sibling) {
        prevsib=sibItem;
        sibItem=&infoPtr->items [(INT)sibItem->sibling];
      }
      sibItem->sibling=(HTREEITEM)iItem;
      wineItem->upsibling=sibItem->hItem;
    } else
    {
      while ((sibItem->sibling) && (sibItem->hItem!=ptdi->hInsertAfter))
      {
        prevsib=sibItem;
        sibItem=&infoPtr->items [(INT)sibItem->sibling];
      }
      if (sibItem->hItem!=ptdi->hInsertAfter) {
        goto TVI_LAST_CASE;
      }
      prevsib=sibItem;
      if (sibItem->sibling) {
        sibItem=&infoPtr->items [(INT)sibItem->sibling];
        sibItem->upsibling=(HTREEITEM)iItem;
        wineItem->sibling=sibItem->hItem;
      }
      prevsib->sibling=(HTREEITEM)iItem;
      wineItem->upsibling=prevsib->hItem;
    }
  }

  /* Fill in info structure */

  wineItem->mask = tvItem->mask;
  wineItem->iIntegral = 1;

  if (tvItem->mask & TVIF_CHILDREN)
    wineItem->cChildren=tvItem->cChildren;

  wineItem->expandBox.left   = 0; /* Initialize the expandBox */
  wineItem->expandBox.top    = 0;
  wineItem->expandBox.right  = 0;
  wineItem->expandBox.bottom = 0;

  if (tvItem->mask & TVIF_IMAGE)
   wineItem->iImage = tvItem->iImage;

  /* If the application sets TVIF_INTEGRAL without
     supplying a TVITEMEX structure, it's toast */

  if (tvItem->mask & TVIF_INTEGRAL)
    wineItem->iIntegral = tvItem->iIntegral;

  if (tvItem->mask & TVIF_SELECTEDIMAGE)
    wineItem->iSelectedImage = tvItem->iSelectedImage;

  if (tvItem->mask & TVIF_STATE)
  {
    wineItem->state = tvItem->state;
    wineItem->stateMask = tvItem->stateMask;
  }

  wineItem->calculated = FALSE;
  //TREEVIEW_Sort(hwnd,0,NULL,NULL); //CB: the Corel people do this, I think it's wrong
  TREEVIEW_QueueRefresh(hwnd);

  return (LRESULT) iItem;
}


static LRESULT
TREEVIEW_DeleteItem (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  INT iItem;
  TREEVIEW_ITEM *wineItem;

  if (lParam == (INT)TVI_ROOT)
  {
    TREEVIEW_RemoveTree (hwnd);
  } else
  {
    iItem = (INT)lParam;
    wineItem = TREEVIEW_ValidItem (infoPtr, (HTREEITEM)iItem);
    if (!wineItem) return FALSE;

    TREEVIEW_RemoveItem (hwnd, wineItem);
  }

  TREEVIEW_QueueRefresh(hwnd);

  return TRUE;
}



static LRESULT
TREEVIEW_GetIndent (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return infoPtr->uIndent;
}

static LRESULT
TREEVIEW_SetIndent (HWND hwnd, WPARAM wParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  INT newIndent;

  newIndent = (INT)wParam;
  if (newIndent < MINIMUM_INDENT) newIndent = MINIMUM_INDENT;
  if (newIndent != infoPtr->uIndent)
  {
    infoPtr->uIndent=newIndent;
    infoPtr->uInternalStatus |= TV_CALCALL;
    TREEVIEW_QueueRefresh(hwnd);
  }

  return 0;
}

static LRESULT
TREEVIEW_GetToolTips (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return infoPtr->hwndToolTip;
}


static LRESULT
TREEVIEW_SetToolTips (HWND hwnd, WPARAM wParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  HWND prevToolTip;

  prevToolTip = infoPtr->hwndToolTip;
  infoPtr->hwndToolTip = (HWND)wParam;

  return prevToolTip;
}


static LRESULT CALLBACK
TREEVIEW_GetEditControl (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return infoPtr->hwndEdit;
}


//@@@PH: Note - this SubclassProc is sometimes called with the
//       wrong window handle. Therefore, infoPtr points to anything
//       but the expected structure.
LRESULT CALLBACK
TREEVIEW_Edit_SubclassProc (HWND hwnd, UINT uMsg, WPARAM wParam,
                                                        LPARAM lParam)
{
  BOOL bCancel = FALSE;
  static BOOL bIgnoreKillFocus = FALSE;

  switch (uMsg)
  {
    case WM_PAINT:
        {
          LRESULT rc;
          TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(GetParent(hwnd));
          //TRACE("WM_PAINT start\n");
          rc = CallWindowProcA( infoPtr->wpEditOrig, hwnd, uMsg, wParam, lParam);
          //TRACE("WM_PAINT done\n");
          return rc;
        }

    case WM_KILLFOCUS:
        if(bIgnoreKillFocus)
        {
            return TRUE;
        }
        break;

    case WM_GETDLGCODE:
      return DLGC_WANTARROWS | DLGC_WANTALLKEYS;

    case WM_KEYDOWN:
        if (VK_ESCAPE == (INT)wParam)
        {
            bCancel = TRUE;
            break;
        }
        else if (VK_RETURN == (INT)wParam)
        {
            break;
        }

    default:
    {
      TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(GetParent(hwnd));

      //@@@PH 1999/11/05 method called with freed infoPtr memory object
      if (infoPtr != NULL)
        return CallWindowProcA( infoPtr->wpEditOrig, hwnd, uMsg, wParam, lParam);
      else
        break;
    }
  }

  /* Processing LVN_ENDLABELEDIT message could kill the focus       */
  /* eg. Using a messagebox                                         */
  bIgnoreKillFocus = TRUE;
  TREEVIEW_EndEditLabelNow(GetParent(hwnd), bCancel);
  bIgnoreKillFocus = FALSE;

  return 0;
}

/* should handle edit control messages here */

static LRESULT
TREEVIEW_Command (HWND hwnd, WPARAM wParam, LPARAM lParam)

{
  switch (HIWORD(wParam))
  {
    case EN_UPDATE:
    {
      /*
       * Adjust the edit window size
       */
      char buffer[1024];
      TREEVIEW_INFO *infoPtr  = TREEVIEW_GetInfoPtr(hwnd);
      TREEVIEW_ITEM *editItem = TREEVIEW_ValidItem(infoPtr, infoPtr->editItem);
      HDC           hdc       = GetDC(infoPtr->hwndEdit);
      SIZE          sz;
      int           len;
      HFONT         hFont, hOldFont=0;

      len = GetWindowTextA(infoPtr->hwndEdit, buffer, 1024);

      /* Select font to get the right dimension of the string */
      hFont = SendMessageA(infoPtr->hwndEdit, WM_GETFONT, 0, 0);
      if(hFont != 0)
      {
          hOldFont = SelectObject(hdc, hFont);
      }

      if (GetTextExtentPoint32A(hdc, buffer, strlen(buffer), &sz))
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
            editItem->text.bottom - editItem->text.top + 3,
            SWP_NOMOVE|SWP_DRAWFRAME);
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

static LRESULT TREEVIEW_Size (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  if (wParam == SIZE_RESTORED)
    if (TREEVIEW_CalcItems(hwnd,0,infoPtr))
      TREEVIEW_Refresh(hwnd);

  return TRUE;
}

static LRESULT
TREEVIEW_StyleChanged (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  if (infoPtr->hwndEdit) SetFocus(hwnd);

  infoPtr->uInternalStatus |= TV_CALCALL;
  TREEVIEW_QueueRefresh(hwnd);

  return 0;
}

static LRESULT
TREEVIEW_SysColorChange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  TREEVIEW_Refresh(hwnd);

  return DefWindowProcA(hwnd,WM_SYSCOLORCHANGE,wParam,lParam);
}

static LRESULT
TREEVIEW_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  LPCREATESTRUCTA lpcs = (LPCREATESTRUCTA)lParam;
  TREEVIEW_INFO *infoPtr;
  DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
  LOGFONTA logFont;
  TEXTMETRICA tm;
  HDC hdc;

  /* allocate memory for info structure */
  infoPtr = (TREEVIEW_INFO*)initControl(hwnd,sizeof(TREEVIEW_INFO));

  if (infoPtr == NULL) return 0;

  hdc = GetDC(hwnd);

  /* set default settings */
  infoPtr->uInternalStatus = TV_CALCALL;
  infoPtr->uNumItems = 0;
  infoPtr->clrBk   = GetSysColor (COLOR_WINDOW);
  infoPtr->clrLine = GetSysColor (COLOR_WINDOWTEXT);
  infoPtr->clrInsertMark = GetSysColor (COLOR_BTNTEXT);
  infoPtr->lefttop.y = 0;
  infoPtr->lefttop.x = 0;
  infoPtr->uIndent = 15;
  infoPtr->himlNormal = NULL;
  infoPtr->himlState = NULL;
  infoPtr->uItemHeight = -1;
  infoPtr->uRealItemHeight = 0;
  infoPtr->uVScrollStep = 1;
  infoPtr->uVisibleHeight = lpcs->cy;
  infoPtr->uTotalHeight = 0;
  infoPtr->uVisibleWidth = lpcs->cx;
  infoPtr->uTotalWidth = 0;

  GetTextMetricsA (hdc, &tm);
  infoPtr->hFont = GetStockObject (DEFAULT_GUI_FONT);
  GetObjectA (infoPtr->hFont, sizeof (LOGFONTA), &logFont);
  logFont.lfWeight = FW_BOLD;
  infoPtr->hBoldFont = CreateFontIndirectA (&logFont);

  infoPtr->items = NULL;
  infoPtr->selectedItem = 0;
  infoPtr->clrText = -1;        /* use system color */
  infoPtr->dropItem = 0;
  infoPtr->insertMarkItem = 0;
  infoPtr->insertBeforeorAfter = 0;
  infoPtr->pCallBackSort = NULL;
  infoPtr->uScrollTime = 300;  /* milliseconds */
  infoPtr->hwndEdit = 0;
  infoPtr->pszISearch = NULL;
  infoPtr->uISearchLen = 0;

  infoPtr->tipItem = 0;
  infoPtr->hwndToolTip = 0;
  if (!(dwStyle & TVS_NOTOOLTIPS))
    infoPtr->hwndToolTip = createToolTip(hwnd,TTF_TRACK | TTF_ABSOLUTE,TRUE);

  if (dwStyle & TVS_CHECKBOXES)
  {
    HBITMAP hbmLoad;
    int nIndex;

    infoPtr->himlState = ImageList_Create (16, 16,ILC_COLOR | ILC_MASK, 15, 1);

    //MSDN docu says: uses DrawFrameControl but never believe what they write
    hbmLoad = LoadBitmapA (COMCTL32_hModule, MAKEINTRESOURCEA(IDT_CHECK));
    nIndex = ImageList_AddMasked (infoPtr->himlState, hbmLoad, CLR_DEFAULT);
    DeleteObject (hbmLoad);
  }
  ReleaseDC (hwnd, hdc);

  return 0;
}

static LRESULT
TREEVIEW_Destroy (HWND hwnd)
{
   TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

   TREEVIEW_RemoveTree (hwnd);
   if (infoPtr->Timer & TV_REFRESH_TIMER_SET)
     KillTimer (hwnd, TV_REFRESH_TIMER);
   destroyToolTip(infoPtr->hwndToolTip);

   /* Restore original windproc so that we can free infoPtr */
   if (infoPtr->hwndEdit)
     SetWindowLongA (infoPtr->hwndEdit, GWL_WNDPROC, (DWORD)infoPtr->wpEditOrig);

   DeleteObject(infoPtr->hBoldFont);
   COMCTL32_Free(infoPtr->pszISearch);
   doneControl(hwnd);

   return 0;
}


static LRESULT
TREEVIEW_Paint (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = wParam==0 ? BeginPaint (hwnd, &ps) : (HDC)wParam;
    TREEVIEW_Draw(hwnd,hdc,&ps.rcPaint);
    if(!wParam)
        EndPaint (hwnd, &ps);

    return 0;
}

static LRESULT
TREEVIEW_SetFocus (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  sendNotify(hwnd,NM_SETFOCUS);

  if (!(dwStyle & TVS_SHOWSELALWAYS))
  {
    if (infoPtr->selectedItem)
      TREEVIEW_RefreshItem(hwnd,TREEVIEW_ValidItem(infoPtr,infoPtr->selectedItem),FALSE);
    else if (infoPtr->firstVisible)
      TREEVIEW_DoSelectItem(hwnd,TVGN_CARET,infoPtr->firstVisible,TVC_UNKNOWN);
  }

  return 0;
}

static LRESULT
TREEVIEW_KillFocus (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  sendNotify(hwnd,NM_KILLFOCUS);

  if (!(dwStyle & TVS_SHOWSELALWAYS) && infoPtr->selectedItem)
    TREEVIEW_RefreshItem(hwnd,TREEVIEW_ValidItem(infoPtr,infoPtr->selectedItem),FALSE);

  return 0;
}

static LRESULT TREEVIEW_Enable(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  TREEVIEW_Refresh(hwnd);

  return DefWindowProcA(hwnd,WM_ENABLE,wParam,lParam);
}

static LRESULT
TREEVIEW_EraseBackground (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
    HBRUSH hBrush = CreateSolidBrush (infoPtr->clrBk);
    RECT rect;

    GetClientRect (hwnd, &rect);
    FillRect ((HDC)wParam, &rect, hBrush);
    DeleteObject (hBrush);

    return TRUE;
}

static LRESULT TREEVIEW_GetDlgCode(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

/* Notifications */

static BOOL
TREEVIEW_SendTreeviewNotify (HWND hwnd, UINT code, UINT action,
                        HTREEITEM oldItem, HTREEITEM newItem)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  NMTREEVIEWW nmhdr;
  TREEVIEW_ITEM  *wineItem;
  CHAR *oldText = NULL,*newText = NULL;
  BOOL rc;

  ZeroMemory(&nmhdr,sizeof(NMTREEVIEWW));

  nmhdr.action = action;
  if (oldItem)
  {
    wineItem = &infoPtr->items[(INT)oldItem];
    nmhdr.itemOld.mask      = wineItem->mask;
    nmhdr.itemOld.hItem     = wineItem->hItem;
    nmhdr.itemOld.state     = wineItem->state;
    nmhdr.itemOld.stateMask = wineItem->stateMask;
    nmhdr.itemOld.iImage    = wineItem->iImage;
    if (!isUnicodeNotify(&infoPtr->header))
    {
      if (!wineItem->pszText) nmhdr.itemOld.pszText = NULL; else
      {
        INT len = lstrlenW(wineItem->pszText)+1;

        oldText = (CHAR*)COMCTL32_Alloc(len);
        lstrcpyWtoA(oldText,wineItem->pszText);
        nmhdr.itemOld.pszText = (WCHAR*)oldText;
      }
    } else nmhdr.itemOld.pszText   = wineItem->pszText;
    nmhdr.itemOld.cchTextMax= wineItem->cchTextMax;
    nmhdr.itemOld.iImage    = wineItem->iImage;
    nmhdr.itemOld.iSelectedImage    = wineItem->iSelectedImage;
    nmhdr.itemOld.cChildren = wineItem->cChildren;
    nmhdr.itemOld.lParam    = wineItem->lParam;
  }

  if (newItem)
  {
    wineItem = &infoPtr->items[(INT)newItem];
    nmhdr.itemNew.mask              = wineItem->mask;
    nmhdr.itemNew.hItem             = wineItem->hItem;
    nmhdr.itemNew.state             = wineItem->state;
    nmhdr.itemNew.stateMask = wineItem->stateMask;
    nmhdr.itemNew.iImage    = wineItem->iImage;
    if (!isUnicodeNotify(&infoPtr->header))
    {
      if (!wineItem->pszText) nmhdr.itemNew.pszText = NULL; else
      {
        INT len = lstrlenW(wineItem->pszText)+1;

        newText = (CHAR*)COMCTL32_Alloc(len);
        lstrcpyWtoA(newText,wineItem->pszText);
        nmhdr.itemNew.pszText = (WCHAR*)newText;
      }
    } else nmhdr.itemNew.pszText   = wineItem->pszText;
    nmhdr.itemNew.cchTextMax= wineItem->cchTextMax;
    nmhdr.itemNew.iImage    = wineItem->iImage;
    nmhdr.itemNew.iSelectedImage    = wineItem->iSelectedImage;
    nmhdr.itemNew.cChildren = wineItem->cChildren;
    nmhdr.itemNew.lParam    = wineItem->lParam;
  }

  nmhdr.ptDrag.x = 0;
  nmhdr.ptDrag.y = 0;

  rc = (BOOL)sendNotify(hwnd,code,&nmhdr.hdr);

  if (oldText) COMCTL32_Free(oldText);
  if (newText) COMCTL32_Free(newText);

  return rc;
}

static BOOL
TREEVIEW_SendTreeviewDnDNotify (HWND hwnd, UINT code, HTREEITEM dragItem,
                                                                POINT pt)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  NMTREEVIEWA nmhdr;
  TREEVIEW_ITEM  *wineItem;

  nmhdr.action = 0;
  wineItem=& infoPtr->items[(INT)dragItem];
  nmhdr.itemNew.mask    = wineItem->mask;
  nmhdr.itemNew.hItem   = wineItem->hItem;
  nmhdr.itemNew.state   = wineItem->state;
  nmhdr.itemNew.lParam  = wineItem->lParam;

  nmhdr.ptDrag.x = pt.x;
  nmhdr.ptDrag.y = pt.y;

  return (BOOL)sendNotify(hwnd,code,&nmhdr.hdr);

}

static BOOL
TREEVIEW_SendDispInfoNotify (HWND hwnd, TREEVIEW_ITEM *wineItem, UINT code, UINT what)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  NMTVDISPINFOW tvdi;
  BOOL retval;
  WCHAR *buf;

  tvdi.item.mask        = what;
  tvdi.item.hItem       = wineItem->hItem;
  tvdi.item.state       = wineItem->state;
  tvdi.item.lParam      = wineItem->lParam;
  tvdi.item.pszText     = (WCHAR*)COMCTL32_Alloc(128*(isUnicodeNotify(&infoPtr->header) ? sizeof(WCHAR):sizeof(char)));
  tvdi.item.cchTextMax = 128;
  buf = tvdi.item.pszText;

  retval = (BOOL)sendNotify(hwnd,code,&tvdi.hdr);

  /* Ignore posible changes */
  if (code == TVN_BEGINLABELEDIT)
      return retval;

  if (what & TVIF_TEXT)
  {
    if (isUnicodeNotify(&infoPtr->header))
    {
      wineItem->pszText = tvdi.item.pszText;
      if (buf == tvdi.item.pszText)
      {
        wineItem->cchTextMax = 128;
      } else
      {
        //TRACE (treeview,"user-supplied buffer\n");
        COMCTL32_Free(buf);
        wineItem->cchTextMax = 0;
      }
    } else
    {
      if (buf == tvdi.item.pszText)
      {
        COMCTL32_Free(wineItem->pszText);
        wineItem->cchTextMax = 128;
        wineItem->pszText = (WCHAR*)COMCTL32_Alloc(128*sizeof(WCHAR));
        lstrcpynAtoW(wineItem->pszText,(CHAR*)tvdi.item.pszText,wineItem->cchTextMax);
        COMCTL32_Free(buf);
      } else
      {
        //TRACE (treeview,"user-supplied buffer\n");
        COMCTL32_Free(buf);
        wineItem->cchTextMax = 0;
      }
    }
  }

  if (what & TVIF_SELECTEDIMAGE)
    wineItem->iSelectedImage = tvdi.item.iSelectedImage;
  if (what & TVIF_IMAGE)
    wineItem->iImage         = tvdi.item.iImage;
  if (what & TVIF_CHILDREN)
    wineItem->cChildren      = tvdi.item.cChildren;

  return retval;
}

static BOOL
TREEVIEW_SendCustomDrawNotify (HWND hwnd, DWORD dwDrawStage, HDC hdc,
                        RECT rc)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  NMTVCUSTOMDRAW nmcdhdr;
  LPNMCUSTOMDRAW nmcd;

  nmcd = &nmcdhdr.nmcd;
  nmcd->dwDrawStage= dwDrawStage;
  nmcd->hdc        = hdc;
  nmcd->rc.left    = rc.left;
  nmcd->rc.right   = rc.right;
  nmcd->rc.bottom  = rc.bottom;
  nmcd->rc.top     = rc.top;
  nmcd->dwItemSpec = 0;
  nmcd->uItemState = 0;
  nmcd->lItemlParam= 0;
  nmcdhdr.clrText  = infoPtr->clrText;
  nmcdhdr.clrTextBk= infoPtr->clrBk;
  nmcdhdr.iLevel   = 0;

  return (BOOL)sendNotify(hwnd,NM_CUSTOMDRAW,&nmcdhdr.nmcd.hdr);
}

/* FIXME: need to find out when the flags in uItemState need to be set */

static BOOL
TREEVIEW_SendCustomDrawItemNotify (HWND hwnd, HDC hdc,
                        TREEVIEW_ITEM *wineItem, UINT uItemDrawState)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  NMTVCUSTOMDRAW nmcdhdr;
  LPNMCUSTOMDRAW nmcd;
  DWORD dwDrawStage,dwItemSpec;
  UINT uItemState;
  INT retval;

  dwDrawStage=CDDS_ITEM | uItemDrawState;
  dwItemSpec=(DWORD)wineItem->hItem;
  uItemState=0;
  if (wineItem->hItem == infoPtr->selectedItem)
  {
    uItemState|=CDIS_SELECTED;
    if (GetFocus() == hwnd) uItemState |= CDIS_FOCUS;
  }
  if (wineItem->hItem==infoPtr->hotItem)      uItemState|=CDIS_HOT;

  nmcd= & nmcdhdr.nmcd;
  nmcd->dwDrawStage= dwDrawStage;
  nmcd->hdc                = hdc;
  nmcd->rc.left    = wineItem->rect.left;
  nmcd->rc.right   = wineItem->rect.right;
  nmcd->rc.bottom  = wineItem->rect.bottom;
  nmcd->rc.top     = wineItem->rect.top;
  nmcd->dwItemSpec = dwItemSpec;
  nmcd->uItemState = uItemState;
  nmcd->lItemlParam= wineItem->lParam;

  nmcdhdr.clrText  = infoPtr->clrText;
  nmcdhdr.clrTextBk= infoPtr->clrBk;
  nmcdhdr.iLevel   = wineItem->iLevel;

  //TRACE (treeview,"drawstage:%lx hdc:%x item:%lx, itemstate:%x\n",
  //               dwDrawStage, hdc, dwItemSpec, uItemState);

  retval = sendNotify(hwnd,NM_CUSTOMDRAW,&nmcdhdr.nmcd.hdr);

  infoPtr->clrText=nmcdhdr.clrText;
  infoPtr->clrBk  =nmcdhdr.clrTextBk;

  return (BOOL) retval;
}

static VOID TREEVIEW_SendKeyDownNotify(HWND hwnd,UINT code,WORD wVKey)
{
  NMTVKEYDOWN nmkdhdr;

  nmkdhdr.wVKey = wVKey;
  nmkdhdr.flags = 0;

  sendNotify(hwnd,code,&nmkdhdr.hdr);
}

/* Note:If the specified item is the child of a collapsed parent item,
   the parent's list of child items is (recursively) expanded to reveal the
   specified item. This is mentioned for TREEVIEW_SelectItem; don't
   know if it also applies here.
*/

static LRESULT
TREEVIEW_Expand (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *wineItem;
  UINT flag;
  INT expand;

  flag = (UINT)wParam;
  expand = (INT)lParam;

  wineItem = TREEVIEW_ValidItem (infoPtr,(HTREEITEM)expand);

  if (!wineItem)
    return 0;

  if (!TREEVIEW_HasChildren(hwnd, wineItem))
    return 0;

  if ((flag & 0xF) == TVE_TOGGLE)
  {
    if (wineItem->state & TVIS_EXPANDED)
      flag = (flag & ~0xF)+TVE_COLLAPSE;
    else
      flag = (flag & ~0xF)+TVE_EXPAND;
  }

  switch (flag & 0xF)
  {
    case TVE_COLLAPSE:
      if (!wineItem->state & TVIS_EXPANDED)
        return 0;

      if (flag & TVE_COLLAPSERESET)
      {
        wineItem->state &= ~(TVIS_EXPANDEDONCE | TVIS_EXPANDED);
        TREEVIEW_RemoveAllChildren (hwnd, wineItem);
      } else wineItem->state &= ~TVIS_EXPANDED;
      break;

    case TVE_EXPAND:
      if (wineItem->state & TVIS_EXPANDED)
        return 0;

      if (flag & TVE_EXPANDPARTIAL)
      {
return FALSE; //CB: to check
        wineItem->state ^=TVIS_EXPANDED;
        wineItem->state |=TVIS_EXPANDEDONCE;
        break;
      }

      if (!(wineItem->state & TVIS_EXPANDEDONCE))
      {
        //TRACE(treeview, "  and has never been expanded...\n");
        wineItem->state |= TVIS_EXPANDED;

        /* this item has never been expanded */
        if (TREEVIEW_SendTreeviewNotify(hwnd,TVN_ITEMEXPANDING,TVE_EXPAND,0,(HTREEITEM)expand))
          return FALSE;


        /* FIXME
         * Since the TVN_ITEMEXPANDING message may has caused the parent to
         * insert new items which in turn may have cause items placeholder
         * reallocation, I reassign the current item pointer so we have
         * something valid to work with...
         * However, this should not be necessary,
         * investigation required in TREEVIEW_InsertItemA
         */
        wineItem = TREEVIEW_ValidItem (infoPtr, (HTREEITEM)expand);
        if (!wineItem)
        {
          //ERR(treeview,
          //  "Catastropic situation, cannot retreive item #%d\n",
          //  expand);
          return FALSE;
        }

        wineItem->state |= TVIS_EXPANDEDONCE;
        //TRACE(treeview, "  TVN_ITEMEXPANDING sent...\n");

        TREEVIEW_SendTreeviewNotify (
          hwnd,
          TVN_ITEMEXPANDED,
          TVE_EXPAND,
          0,
          (HTREEITEM)expand);

        //TRACE(treeview, "  TVN_ITEMEXPANDED sent...\n");

      } else
      {
        /* this item has already been expanded */
        wineItem->state |= TVIS_EXPANDED;
      }
      break;

    default:
      return FALSE;
  }

  /* If item was collapsed we probably need to change selection */
  if (flag & TVE_COLLAPSE)
  {
     HTREEITEM hItem = infoPtr->selectedItem;

     if (!TREEVIEW_ValidItem (infoPtr, hItem))
        hItem = wineItem->hItem;
     else
     {
        while ( hItem )
        {
           hItem = infoPtr->items[(INT)hItem].parent;

           if (hItem == wineItem->hItem)
              break;
        }
     }

     if (hItem)
        TREEVIEW_DoSelectItem(hwnd, TVGN_CARET, hItem, TVC_UNKNOWN);
  }

  //CB: todo: optimize!
  TREEVIEW_UnqueueRefresh(hwnd,FALSE,FALSE);
  //CB: todo: precalc expanded items here
  infoPtr->uInternalStatus |= TV_CALCALL;
  TREEVIEW_CalcItems(hwnd,0,infoPtr);
  TREEVIEW_Refresh(hwnd);
  //CB: todo: check cx and cy to fit ranges!

  return TRUE;
}

static LRESULT TREEVIEW_HitTest(HWND hwnd,LPTVHITTESTINFO lpht)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *wineItem;
  RECT rect;
  UINT status;
  INT x,y;

  if (!lpht) return 0;
  GetClientRect (hwnd, &rect);
  status = 0;
  x = lpht->pt.x;
  y = lpht->pt.y;
  if (x < rect.left)  status |= TVHT_TOLEFT;
  if (x > rect.right) status |= TVHT_TORIGHT;
  if (y < rect.top )  status |= TVHT_ABOVE;
  if (y > rect.bottom) status |= TVHT_BELOW;

  if (status)
  {
    lpht->flags = status;
    lpht->hItem = 0;

    return 0;
  }

  if (infoPtr->firstVisible)
  {
   wineItem = &infoPtr->items [(INT)infoPtr->firstVisible];

   while ((wineItem != NULL) && (y > wineItem->rect.bottom))
     wineItem = TREEVIEW_GetNextListItem(hwnd,infoPtr,wineItem);
  } else wineItem = NULL;

  if (!wineItem)
  {
    lpht->flags = TVHT_NOWHERE;
    lpht->hItem = 0;

    return 0;
  }

  lpht->flags = 0;

  if (x < wineItem->expandBox.left) {
    lpht->flags |= TVHT_ONITEMINDENT;
        goto done;
  }
  if ( PtInRect ( &wineItem->expandBox, lpht->pt)) {
    lpht->flags |= TVHT_ONITEMBUTTON;
        goto done;
  }
  if ( PtInRect ( &wineItem->bitmap, lpht->pt)) {
        lpht->flags |= TVHT_ONITEMICON;
    goto done;
  }
  if ( PtInRect ( &wineItem->statebitmap, lpht->pt)) {
        lpht->flags |= TVHT_ONITEMSTATEICON;
    goto done;
  }
  if ( PtInRect ( &wineItem->text, lpht->pt)) {
    lpht->flags |= TVHT_ONITEMLABEL;
        goto done;
  }

  lpht->flags |= TVHT_ONITEMRIGHT;

done:
  lpht->hItem = wineItem->hItem;

  return (LRESULT) wineItem->hItem;
}

HWND TREEVIEW_EditLabel(HWND hwnd,HTREEITEM hItem,BOOL unicode)
{
  SIZE sz;
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *editItem = TREEVIEW_ValidItem(infoPtr, (HTREEITEM)hItem);
  HINSTANCE hinst = GetWindowLongA(hwnd, GWL_HINSTANCE);
  HDC hdc;
  HFONT hOldFont = 0;
  TEXTMETRICA textMetric;

  if (!editItem)
    return FALSE;

  if(infoPtr->hwndEdit)
    return infoPtr->hwndEdit;

  /* Make shure that edit item is selected */

  TREEVIEW_DoSelectItem ( hwnd, TVGN_CARET, hItem, TVC_UNKNOWN);

  if (editItem->pszText == LPSTR_TEXTCALLBACKW)
    TREEVIEW_SendDispInfoNotify (hwnd, editItem, TVN_GETDISPINFO, TVIF_TEXT);

  hdc = GetDC(hwnd);
  /* Select the font to get appropriate metric dimensions */
  if(infoPtr->hFont != 0)
  {
      hOldFont = SelectObject(hdc, infoPtr->hFont);
  }

  /*Get String Lenght in pixels */
  GetTextExtentPoint32W(hdc, editItem->pszText, lstrlenW(editItem->pszText), &sz);

  /*Add Extra spacing for the next character */
  GetTextMetricsA(hdc, &textMetric);
  sz.cx += (textMetric.tmMaxCharWidth * 2);

  if(infoPtr->hFont != 0)
  {
      SelectObject(hdc, hOldFont);
  }

  ReleaseDC(hwnd, hdc);
  infoPtr->hwndEdit = CreateWindowExA (
                         WS_EX_LEFT,
                         "EDIT",
                          0,
                         WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_CLIPSIBLINGS |
                         ES_WANTRETURN | ES_LEFT,
                         editItem->text.left - 2, editItem->text.top  - 1,
                         sz.cx+3, editItem->text.bottom - editItem->text.top  + 3,
                         hwnd,
                         0,hinst,0); /* FIXME: (HMENU)IDTVEDIT,pcs->hInstance,0);*/

  SendMessageA ( infoPtr->hwndEdit, WM_SETFONT, infoPtr->hFont, FALSE);
  infoPtr->wpEditOrig = (WNDPROC)SetWindowLongA (
                                   infoPtr->hwndEdit,
                                   GWL_WNDPROC,
                  (DWORD) TREEVIEW_Edit_SubclassProc);

  if (TREEVIEW_SendDispInfoNotify (hwnd, editItem, TVN_BEGINLABELEDIT,
           editItem->mask & (TVIF_HANDLE|TVIF_TEXT|TVIF_STATE|TVIF_PARAM)))
  {
     DestroyWindow(infoPtr->hwndEdit);
     infoPtr->hwndEdit = 0;

     return (HWND)0;
  }

  infoPtr->editItem = hItem;
  SetWindowTextW(infoPtr->hwndEdit, editItem->pszText);
  SetFocus(infoPtr->hwndEdit);
  SendMessageA(infoPtr->hwndEdit, EM_SETSEL, 0, -1);
  ShowWindow(infoPtr->hwndEdit, SW_SHOW);

  return infoPtr->hwndEdit;
}


LRESULT WINAPI
TREEVIEW_EndEditLabelNow (HWND hwnd, BOOL bCancel)
{
  TREEVIEW_INFO *infoPtr    = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *editedItem = TREEVIEW_ValidItem (infoPtr, infoPtr->editItem);
  NMTVDISPINFOW tvdi;
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  BOOL bCommit;
  WCHAR *textW = NULL;
  CHAR *textA = NULL;
  int  iLength = 0;

  if (!infoPtr->hwndEdit)
     return FALSE;

  tvdi.item.mask        = 0;
  tvdi.item.hItem       = editedItem->hItem;
  tvdi.item.state       = editedItem->state;
  tvdi.item.lParam      = editedItem->lParam;

  if (!bCancel)
  {
    textW = (WCHAR*)COMCTL32_Alloc(1024*sizeof(WCHAR));
    iLength = GetWindowTextW(infoPtr->hwndEdit,textW,1023);

    //if (iLength >= 1023)
    //{
    //  ERR("Insuficient space to retrieve new item label.");
    //}

    if (isUnicodeNotify(&infoPtr->header)) tvdi.item.pszText = textW; else
    {
      INT len = iLength+1;

      textA = (CHAR*)COMCTL32_Alloc(len);
      lstrcpynWtoA(textA,textW,len);
      tvdi.item.pszText = (WCHAR*)textA;
    }
    tvdi.item.cchTextMax  = iLength + 1;
  }
  else
  {
      tvdi.item.pszText = NULL;
      tvdi.item.cchTextMax  = 0;
  }

  bCommit = (BOOL)sendNotify(hwnd,TVN_ENDLABELEDIT,&tvdi.hdr);

  if (!bCancel && bCommit) /* Apply the changes */
  {
    if (!isUnicodeNotify(&infoPtr->header))
      lstrcpynAtoW(textW,textA,iLength+1);
    if (lstrcmpW(textW,editedItem->pszText) != 0)
    {
      if(NULL == COMCTL32_ReAlloc(editedItem->pszText,(iLength+1)*sizeof(WCHAR)))
      {
        //ERR("OutOfMemory, cannot allocate space for label");
        DestroyWindow(infoPtr->hwndEdit);
        infoPtr->hwndEdit = 0;
        if (textA) COMCTL32_Free(textA);
        if (textW) COMCTL32_Free(textW);

        return FALSE;
      }
      else
      {
        editedItem->cchTextMax = iLength + 1;
        lstrcpyW( editedItem->pszText,textW);
      }
    }
  }

  ShowWindow(infoPtr->hwndEdit, SW_HIDE);
  DestroyWindow(infoPtr->hwndEdit);
  infoPtr->hwndEdit = 0;
  infoPtr->editItem = 0;
  if (textA) COMCTL32_Free(textA);
  if (textW) COMCTL32_Free(textW);

  editedItem->calculated = FALSE;
  TREEVIEW_CalcItem(hwnd,0,dwStyle,infoPtr,editedItem);
  TREEVIEW_RefreshItem(hwnd,editedItem,TRUE);

  return TRUE;
}


/***************************************************************************
 * This is quite unusual piece of code, but that's how it's implemented in
 * Windows.
 */
static LRESULT TREEVIEW_TrackMouse(HWND hwnd, POINT pt)
{
   INT cxDrag = GetSystemMetrics(SM_CXDRAG);
   INT cyDrag = GetSystemMetrics(SM_CYDRAG);
   RECT r;
   MSG msg;

   r.top    = pt.y - cyDrag;
   r.left   = pt.x - cxDrag;
   r.bottom = pt.y + cyDrag;
   r.right  = pt.x + cxDrag;

   SetCapture(hwnd);

   while(1)
   {
      if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE|PM_NOYIELD))
      {
         if (msg.message == WM_MOUSEMOVE)
         {
            pt.x = (INT)LOWORD(msg.lParam);
            pt.y = (INT)HIWORD(msg.lParam);
            if (PtInRect(&r, pt))
               continue;
            else
            {
               ReleaseCapture();
               return 1;
            }
         }
         else if (msg.message >= WM_LBUTTONDOWN &&
                  msg.message <= WM_RBUTTONDBLCLK)
         {
            if (msg.message == WM_RBUTTONUP)
              TREEVIEW_RButtonUp (hwnd, &pt);
            break;
         }

         DispatchMessageA( &msg );
      }

      if (GetCapture() != hwnd)
         return 0;
   }

   ReleaseCapture();
   return 0;
}

static LRESULT
TREEVIEW_LButtonDoubleClick (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *wineItem;
  TV_HITTESTINFO hitinfo;

  hitinfo.pt.x = (INT)LOWORD(lParam);
  hitinfo.pt.y = (INT)HIWORD(lParam);
  SetFocus (hwnd);

  if (infoPtr->Timer & TV_EDIT_TIMER_SET)
  {
     /* If there is pending 'edit label' event - kill it now */
     infoPtr->editItem = 0;
     KillTimer (hwnd, TV_EDIT_TIMER);
  }

  if (!TREEVIEW_HitTest(hwnd,&hitinfo) || !(hitinfo.flags & TVHT_ONITEM)) return FALSE;
  wineItem = &infoPtr->items [(INT)hitinfo.hItem];

  //MSDN documentation says: stop on non zero. but this isn't true in this case
  if (!sendNotify(hwnd,NM_DBLCLK))
    TREEVIEW_Expand (hwnd, (WPARAM) TVE_TOGGLE, (LPARAM) wineItem->hItem);

 return TRUE;
}

static LRESULT TREEVIEW_MouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TVHITTESTINFO ht;
  TREEVIEW_ITEM *item;

  ht.pt.x = (INT)LOWORD(lParam);
  ht.pt.y = (INT)HIWORD(lParam);

  TREEVIEW_HitTest(hwnd,&ht);

  if (ht.hItem)
  {
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    item = &infoPtr->items[(INT)ht.hItem];

    if (infoPtr->hwndToolTip && ((item->text.left < 0) || (item->text.right > infoPtr->uVisibleWidth)) && (infoPtr->tipItem != item->hItem))
    {
      DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
      LPWSTR text;
      TTTOOLINFOW ti;
      POINT pt;

      if(dwStyle & TVS_INFOTIP)
      {
        NMTVGETINFOTIPW tvgit;

        tvgit.pszText    = (WCHAR*)COMCTL32_Alloc(isUnicodeNotify(&infoPtr->header) ? INFOTIPSIZE*sizeof(WCHAR):INFOTIPSIZE*sizeof(CHAR));
        tvgit.cchTextMax = INFOTIPSIZE;
        tvgit.hItem      = item->hItem;
        tvgit.lParam     = item->lParam;

        sendNotify(hwnd,isUnicodeNotify(&infoPtr->header) ? TVN_GETINFOTIPW:TVN_GETINFOTIPA,&tvgit.hdr);
        if (isUnicodeNotify(&infoPtr->header))
        {
          text = tvgit.pszText;
        } else
        {
          text = (WCHAR*)COMCTL32_Alloc(tvgit.cchTextMax*sizeof(WCHAR));
          lstrcpyAtoW(text,(LPSTR)tvgit.pszText);
        }
      } else
      {
        if (item->pszText == LPSTR_TEXTCALLBACKW)
          TREEVIEW_SendDispInfoNotify(hwnd,item,TVN_GETDISPINFO,TVIF_TEXT);

        text = item->pszText;
      }
#if 0 //CB: not yet
      infoPtr->tipItem = item->hItem;
      ti.cbSize   = sizeof(ti);
      ti.uId      = 0;
      ti.hwnd     = hwnd;
      ti.hinst    = 0;
      ti.lpszText = text;
      pt.x = ti.rect.left;
      pt.y = ti.rect.top;
      ClientToScreen(hwnd,&pt);
      SendMessageA(infoPtr->hwndToolTip,TTM_TRACKPOSITION,0,(LPARAM)MAKELPARAM(pt.x,pt.y));
      SendMessageA(infoPtr->hwndToolTip,TTM_UPDATETIPTEXTW,0,(LPARAM)&ti);
      SendMessageA(infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&ti);
#endif
      if (text != item->pszText) COMCTL32_Free(text);
    }

    if (dwStyle & TVS_TRACKSELECT)
    {
      //CB: todo: hottracking
    }
  }

  return 0;
}

static LRESULT
TREEVIEW_LButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TVHITTESTINFO ht;
  BOOL bTrack;
  DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);

  /* If Edit control is active - kill it and return.
   * The best way to do it is to set focus to itself.
   * Edit control subclassed procedure will automatically call
   * EndEditLabelNow.
   */
  if (infoPtr->hwndEdit)
  {
     SetFocus (hwnd);
     return 0;
  }

  ht.pt.x = (INT)LOWORD(lParam);
  ht.pt.y = (INT)HIWORD(lParam);

  TREEVIEW_HitTest (hwnd,&ht);

  bTrack = (ht.flags & TVHT_ONITEM) && !(dwStyle & TVS_DISABLEDRAGDROP);

  /* Send NM_CLICK right away */
  if (!bTrack)
     if (sendNotify(hwnd,NM_CLICK))
        goto setfocus;

  if (ht.flags & TVHT_ONITEMBUTTON)
  {
    TREEVIEW_Expand (hwnd, (WPARAM) TVE_TOGGLE, (LPARAM) ht.hItem);
    goto setfocus;
  } else if (bTrack)
  {
    if (TREEVIEW_TrackMouse(hwnd, ht.pt))
    {
      TREEVIEW_SendTreeviewDnDNotify (hwnd, TVN_BEGINDRAG, ht.hItem, ht.pt);
      infoPtr->dropItem = ht.hItem;
      return 0;
    }
  }

  if (sendNotify(hwnd,NM_CLICK))
     goto setfocus;

  /*
   * If the style allow editing and the node is already selected
   * and the click occured on the item label...
   */
  if ((dwStyle & TVS_EDITLABELS) && (ht.flags & TVHT_ONITEMLABEL) && (infoPtr->selectedItem == ht.hItem))
  {
    if (infoPtr->Timer & TV_EDIT_TIMER_SET)
      KillTimer (hwnd, TV_EDIT_TIMER);

    infoPtr->editItem = ht.hItem;
    SetTimer (hwnd, TV_EDIT_TIMER, GetDoubleClickTime(), 0);
    infoPtr->Timer|=TV_EDIT_TIMER_SET;
  } else if (ht.flags & (TVHT_ONITEMLABEL | TVHT_ONITEMICON))
  {
    TREEVIEW_DoSelectItem(hwnd,TVGN_CARET,ht.hItem,TVC_BYMOUSE);
  } else if (ht.flags & TVHT_ONITEMSTATEICON)
  {
    if (dwStyle & TVS_CHECKBOXES)  /* TVS_CHECKBOXES requires _us_ */
    {                              /* to toggle the current state  */
      int state;
      TREEVIEW_ITEM *wineItem;

      wineItem = TREEVIEW_ValidItem(infoPtr, ht.hItem);

      state = 1-(wineItem->state>>12);
      wineItem->state &= ~TVIS_STATEIMAGEMASK;
      wineItem->state |= state<<12;
      if (wineItem->visible)
      {
        TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE);
        InvalidateRect(hwnd,&wineItem->statebitmap,TRUE);
      }
    }
  }

setfocus:
  SetFocus (hwnd);

  return 0;
}


static LRESULT
TREEVIEW_RButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
 TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
   TVHITTESTINFO ht;

   if (infoPtr->hwndEdit)
   {
      SetFocus(hwnd);
      return 0;
   }

   ht.pt.x = (INT)LOWORD(lParam);
   ht.pt.y = (INT)HIWORD(lParam);

   TREEVIEW_HitTest(hwnd,&ht);

   if (TREEVIEW_TrackMouse(hwnd, ht.pt))
   {
      if (ht.hItem)
      {
         TREEVIEW_SendTreeviewDnDNotify (hwnd, TVN_BEGINRDRAG, ht.hItem, ht.pt);
         infoPtr->dropItem = ht.hItem;
      }
   }
   else
   {
      SetFocus(hwnd);
      sendNotify(hwnd,NM_RCLICK);
   }

   return 0;
}

static LRESULT TREEVIEW_RButtonDoubleClick(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  sendNotify(hwnd,NM_RDBLCLK);

  return DefWindowProcA(hwnd,WM_RBUTTONDBLCLK,wParam,lParam);
}

static LRESULT
TREEVIEW_RButtonUp (HWND hwnd, LPPOINT pPt)
{
    POINT pt;

    pt.x = pPt->x;
    pt.y = pPt->y;

    /* Change to screen coordinate for WM_CONTEXTMENU */
    ClientToScreen(hwnd, &pt);

    /* Send the WM_CONTEXTMENU on a right click */
    SendMessageA( hwnd, WM_CONTEXTMENU, (WPARAM) hwnd, MAKELPARAM(pt.x, pt.y));
    return 0;
}

static LRESULT
TREEVIEW_CreateDragImage (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
 TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
 TREEVIEW_ITEM *dragItem;
 INT cx,cy;
 HDC    hdc,htopdc;
 HWND hwtop;
 HBITMAP hbmp,hOldbmp;
 SIZE  size;
 RECT  rc;
 HFONT hOldFont;
 WCHAR    *itemtxt;

// TRACE (treeview,"\n");
 if (!(infoPtr->himlNormal))  return 0;
 dragItem=TREEVIEW_ValidItem (infoPtr, (HTREEITEM) lParam);

 if (!dragItem) return 0;

 if (dragItem->pszText==LPSTR_TEXTCALLBACKW) {
     TREEVIEW_SendDispInfoNotify (hwnd, dragItem, TVN_GETDISPINFO, TVIF_TEXT);
 }
 itemtxt=dragItem->pszText;

 hwtop=GetDesktopWindow ();
 htopdc= GetDC (hwtop);
 hdc=CreateCompatibleDC (htopdc);

 hOldFont=SelectObject (hdc, infoPtr->hFont);
 GetTextExtentPoint32W (hdc, itemtxt, lstrlenW (itemtxt), &size);

 hbmp=CreateCompatibleBitmap (htopdc, size.cx, size.cy);
 hOldbmp=SelectObject (hdc, hbmp);

 ImageList_GetIconSize (infoPtr->himlNormal, &cx, &cy);
 size.cx+=cx;
 if (cy>size.cy) size.cy=cy;

 infoPtr->dragList=ImageList_Create (size.cx, size.cy, ILC_COLOR, 10, 10);
 ImageList_Draw (infoPtr->himlNormal, dragItem->iImage, hdc, 0, 0, ILD_NORMAL);

/*
 ImageList_GetImageInfo (infoPtr->himlNormal, dragItem->hItem, &iminfo);
 ImageList_AddMasked (infoPtr->dragList, iminfo.hbmImage, CLR_DEFAULT);
*/

/* draw item text */

 SetRect (&rc, cx, 0, size.cx,size.cy);
 DrawTextW (hdc, itemtxt, lstrlenW (itemtxt), &rc, DT_LEFT);
 SelectObject (hdc, hOldFont);
 SelectObject (hdc, hOldbmp);

 ImageList_Add (infoPtr->dragList, hbmp, 0);

 DeleteDC (hdc);
 DeleteObject (hbmp);
 ReleaseDC (hwtop, htopdc);

 return (LRESULT)infoPtr->dragList;
}


static LRESULT
TREEVIEW_DoSelectItem (HWND hwnd, INT action, HTREEITEM newSelect, INT cause)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *prevItem,*wineItem;
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  INT prevSelect;

  wineItem = TREEVIEW_ValidItem (infoPtr, (HTREEITEM)newSelect);

  if (wineItem && wineItem->parent)
  {
    TREEVIEW_ITEM *parent = TREEVIEW_ValidItem(infoPtr,wineItem->parent);

    /*
     * If the item has a collapse parent expand the parent so he
     * can expose the item
     */
    while (parent && !(parent->state & TVIS_EXPANDED))
    {
      TREEVIEW_Expand(hwnd,TVE_EXPAND,(LPARAM)parent->hItem);
      parent = TREEVIEW_ValidItem(infoPtr,parent->parent);
    }
  }

  switch (action)
  {
    case TVGN_CARET:
      prevSelect = (INT)infoPtr->selectedItem;

      if ((HTREEITEM)prevSelect == newSelect)
        return FALSE;

      prevItem= TREEVIEW_ValidItem (infoPtr, (HTREEITEM)prevSelect);

      if (newSelect)
        if (TREEVIEW_SendTreeviewNotify(hwnd,TVN_SELCHANGING,cause,(HTREEITEM)prevSelect,(HTREEITEM)newSelect))
          return FALSE;       /* FIXME: OK? */

      if (prevItem)
        prevItem->state &= ~TVIS_SELECTED;
      if (wineItem)
        wineItem->state |=  TVIS_SELECTED;

      infoPtr->selectedItem = (HTREEITEM)newSelect;

      if ((cause == TVC_BYMOUSE) && (dwStyle & TVS_SINGLEEXPAND))
      {
        BOOL control = GetKeyState(VK_CONTROL) & 0x8000;
        UINT rc = TREEVIEW_SendTreeviewNotify(hwnd,TVN_SINGLEEXPAND,0,prevItem->hItem,wineItem->hItem);

        if (!(rc & TVNRET_SKIPOLD) && !control && prevItem->state & TVIS_EXPANDED)
          TREEVIEW_Expand(hwnd,TVE_COLLAPSE,(LPARAM)prevItem->hItem);
        if (!(rc & TVNRET_SKIPNEW) && TREEVIEW_HasChildren(hwnd,wineItem))
          TREEVIEW_Expand(hwnd,TVE_TOGGLE,(LPARAM)wineItem->hItem);
      }

      TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE);
      TREEVIEW_RefreshItem(hwnd,prevItem,FALSE);
      TREEVIEW_RefreshItem(hwnd,wineItem,FALSE);

      TREEVIEW_SendTreeviewNotify(hwnd,TVN_SELCHANGED,cause,(HTREEITEM)prevSelect,(HTREEITEM)newSelect);

      break;

    case TVGN_DROPHILITE:
      prevItem= TREEVIEW_ValidItem (infoPtr, infoPtr->dropItem);

      if (prevItem)
        prevItem->state &= ~TVIS_DROPHILITED;

      infoPtr->dropItem = (HTREEITEM)newSelect;

      if (wineItem)
        wineItem->state |=TVIS_DROPHILITED;

      TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE);
      TREEVIEW_RefreshItem(hwnd,prevItem,FALSE);
      TREEVIEW_RefreshItem(hwnd,wineItem,FALSE);

      break;

    case TVGN_FIRSTVISIBLE:
    {
      TREEVIEW_ITEM *firstVis;
      INT scrollY;

      if (!infoPtr->firstVisible) return FALSE;

      firstVis = &infoPtr->items[(INT)infoPtr->firstVisible];

      if (wineItem->rect.top < 0)
        scrollY = wineItem->rect.top;
      else
      {
        scrollY = MIN(wineItem->rect.top,(infoPtr->uTotalHeight-infoPtr->uVisibleHeight)-infoPtr->lefttop.y);
        scrollY -= scrollY % infoPtr->uVScrollStep;
      }

      if (scrollY != 0)
      {
        infoPtr->lefttop.y += scrollY;
        if (!TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE))
          TREEVIEW_CalcItems(hwnd,0,infoPtr);

        ScrollWindowEx(hwnd,0,-scrollY,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));
      }

      break;
    }
 }

 return TRUE;
}

static LRESULT
TREEVIEW_SelectItem (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
 return TREEVIEW_DoSelectItem (hwnd, wParam, (HTREEITEM) lParam, TVC_UNKNOWN);
}

static LRESULT
TREEVIEW_GetFont (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
 TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

// TRACE (treeview,"%x\n",infoPtr->hFont);
 return infoPtr->hFont;
}

static LRESULT
TREEVIEW_SetFont (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TEXTMETRICA tm;
  LOGFONTA logFont;
  HFONT hFont, hOldFont;
  INT height;
  HDC hdc;

  infoPtr->hFont = (HFONT)wParam;

  hFont = infoPtr->hFont ? infoPtr->hFont : GetStockObject (SYSTEM_FONT);

  GetObjectA (infoPtr->hFont, sizeof (LOGFONTA), &logFont);
  logFont.lfWeight = FW_BOLD;
  DeleteObject(infoPtr->hBoldFont);
  infoPtr->hBoldFont = CreateFontIndirectA (&logFont);

  infoPtr->uInternalStatus |= TV_CALCALL;
  TREEVIEW_CalcItems(hwnd,0,infoPtr);

  if (lParam)
  {
    TREEVIEW_UnqueueRefresh(hwnd,FALSE,FALSE);
    TREEVIEW_Refresh(hwnd);
  }

  return 0;
}

static LRESULT
TREEVIEW_VScroll (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  INT newY,mod;
  INT maxY = infoPtr->uTotalHeight-infoPtr->uVisibleHeight;

  if (!(infoPtr->uInternalStatus & TV_VSCROLL)) return FALSE;

  mod = maxY % infoPtr->uVScrollStep;
  if (mod > 0) maxY += infoPtr->uVScrollStep-mod;

  switch (LOWORD (wParam))
  {
    case SB_LINEUP:
      newY = infoPtr->lefttop.y-infoPtr->uVScrollStep;
      if (newY < 0) newY = 0;
      break;

    case SB_LINEDOWN:
      newY = infoPtr->lefttop.y+infoPtr->uVScrollStep;
      if (newY > maxY) newY = maxY;
      break;

    case SB_PAGEUP:
      newY = infoPtr->lefttop.y-MAX(((INT)(infoPtr->uVisibleHeight/infoPtr->uVScrollStep))*infoPtr->uVScrollStep,infoPtr->uVScrollStep);
      if (newY < 0) newY = 0;
      break;

    case SB_PAGEDOWN:
      newY = infoPtr->lefttop.y+MAX(((INT)(infoPtr->uVisibleHeight/infoPtr->uVScrollStep))*infoPtr->uVScrollStep,infoPtr->uVScrollStep);
      if (newY > maxY) newY = maxY;
      break;

    case SB_THUMBTRACK:
      newY = HIWORD(wParam);
      mod = newY % infoPtr->uVScrollStep;
      if (mod > 0) newY -= mod;
      break;

    default:
      return FALSE;
  }

  if (newY != infoPtr->lefttop.y)
  {
    INT scrollY = infoPtr->lefttop.y-newY;

    infoPtr->lefttop.y = newY;
    if (!TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE))
      TREEVIEW_CalcItems(hwnd,0,infoPtr);
    ScrollWindowEx(hwnd,0,scrollY,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));

    return TRUE;
  }

  return FALSE;
}

static LRESULT
TREEVIEW_HScroll (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  int maxWidth;
  int lastPos = infoPtr->lefttop.x;

  if (!(infoPtr->uInternalStatus & TV_HSCROLL)) return FALSE;

  switch (LOWORD (wParam))
  {
    case SB_LINEUP:
      if (!infoPtr->lefttop.x) return FALSE;
      infoPtr->lefttop.x -= infoPtr->uRealItemHeight;
      if (infoPtr->lefttop.x < 0) infoPtr->lefttop.x = 0;
      break;

    case SB_LINEDOWN:
      maxWidth = infoPtr->uTotalWidth-infoPtr->uVisibleWidth;
      if (maxWidth <= 0) return FALSE;
      if (infoPtr->lefttop.x == maxWidth) return FALSE;
      infoPtr->lefttop.x += infoPtr->uRealItemHeight; /*FIXME */
      if (infoPtr->lefttop.x > maxWidth)
        infoPtr->lefttop.x = maxWidth;
      break;

    case SB_PAGEUP:
      if (!infoPtr->lefttop.x) return FALSE;
      infoPtr->lefttop.x -= infoPtr->uVisibleWidth;
      if (infoPtr->lefttop.x < 0) infoPtr->lefttop.x = 0;
      break;

    case SB_PAGEDOWN:
      maxWidth = infoPtr->uTotalWidth-infoPtr->uVisibleWidth;
      if (maxWidth <= 0) return FALSE;
      if (infoPtr->lefttop.x == maxWidth) return FALSE;
      infoPtr->lefttop.x += infoPtr->uVisibleWidth;
      if (infoPtr->lefttop.x > maxWidth)
        infoPtr->lefttop.x = maxWidth;
      break;

    case SB_THUMBTRACK:
      maxWidth = infoPtr->uTotalWidth-infoPtr->uVisibleWidth;
      if (maxWidth <= 0) return FALSE;
      infoPtr->lefttop.x = HIWORD(wParam);
      if (infoPtr->lefttop.x < 0) infoPtr->lefttop.x = 0;
      if (infoPtr->lefttop.x > maxWidth) infoPtr->lefttop.x = maxWidth;
      break;

    default:
      return FALSE;
  }

  if (lastPos != infoPtr->lefttop.x)
  {
    if (!TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE))
      TREEVIEW_CalcItems(hwnd,0,infoPtr);
    ScrollWindowEx(hwnd,lastPos-infoPtr->lefttop.x,0,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));

    return TRUE;
  }

  return FALSE;
}

static LRESULT TREEVIEW_MouseWheel (HWND hwnd, WPARAM wParam, LPARAM lParam)
{

    TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
    short gcWheelDelta = 0;
    UINT pulScrollLines = 3;

    if (wParam & (MK_SHIFT | MK_CONTROL))
      return DefWindowProcA( hwnd, WM_MOUSEWHEEL, wParam, lParam );


    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES,0, &pulScrollLines, 0);

    gcWheelDelta -= (short) HIWORD(wParam);
    pulScrollLines *= (gcWheelDelta / WHEEL_DELTA);

    if (abs(gcWheelDelta) >= WHEEL_DELTA && pulScrollLines)
    {
        int wheelDy = pulScrollLines * infoPtr->uRealItemHeight;
        int newDy = infoPtr->lefttop.y + wheelDy;
        int maxDy = infoPtr->uTotalHeight-infoPtr->uVisibleHeight;

        if (newDy > maxDy) newDy = maxDy;
        if (newDy < 0) newDy = 0;

        if (newDy == infoPtr->lefttop.y) return TRUE;

        TREEVIEW_VScroll(hwnd, MAKEWPARAM(SB_THUMBTRACK,newDy),0);
    }
  return TRUE;
}

static LRESULT
TREEVIEW_KeyDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_INFO *infoPtr        = TREEVIEW_GetInfoPtr(hwnd);
  HTREEITEM     hNewSelection   = 0;
  INT           prevSelect      = (INT)infoPtr->selectedItem;
  TREEVIEW_ITEM *prevItem       =
    (prevSelect != 0 ) ?
      TREEVIEW_ValidItem (infoPtr, (HTREEITEM)prevSelect) :
      NULL;
  TREEVIEW_ITEM *newItem        = NULL;
  BOOL control;

  TREEVIEW_SendKeyDownNotify(hwnd,TVN_KEYDOWN,wParam);

  control = GetKeyState(VK_CONTROL) & 0x8000;

  if (control)
  {
    INT scrollMsg = -1;
    BOOL horz = FALSE;

    switch (wParam)
    {
      case VK_UP:
        scrollMsg = SB_LINELEFT;
        break;

      case VK_DOWN:
        scrollMsg = SB_LINERIGHT;
        break;

      case VK_LEFT:
        scrollMsg = SB_LINELEFT;
        horz = TRUE;
        break;

      case VK_RIGHT:
        scrollMsg = SB_LINERIGHT;
        horz = TRUE;
        break;

      case VK_PRIOR:
        scrollMsg = SB_PAGELEFT;
        break;

      case VK_NEXT:
        scrollMsg = SB_PAGERIGHT;
        break;
    }
    if (scrollMsg != -1)
    {
      SendMessageA(hwnd,horz ? WM_HSCROLL:WM_VSCROLL,MAKELONG(scrollMsg,0),0);
      return TRUE;
    }
  }

  if (prevSelect == 0)
    return FALSE;

  switch (wParam)
  {
    case VK_UP:
      newItem = TREEVIEW_GetPrevListItem(hwnd,infoPtr, prevItem);

      if (!newItem)
        newItem = &infoPtr->items[(INT)infoPtr->TopRootItem];

      hNewSelection = newItem->hItem;

      break;

    case VK_DOWN:
      newItem = TREEVIEW_GetNextListItem (hwnd,infoPtr, prevItem);

      if (!newItem)
        newItem = prevItem;

      hNewSelection = newItem->hItem;

      break;

    case VK_HOME:
      newItem = &infoPtr->items[(INT)infoPtr->TopRootItem];
      hNewSelection = newItem->hItem;

      break;

    case VK_END:
      newItem       = &infoPtr->items[(INT)infoPtr->TopRootItem];
      newItem       = TREEVIEW_GetLastListItem (hwnd,infoPtr, newItem);
      hNewSelection = newItem->hItem;

      break;

    case VK_LEFT:
      if ( (prevItem->state & TVIS_EXPANDED) &&
         TREEVIEW_HasChildren(hwnd, prevItem))
      {
        TREEVIEW_Expand(hwnd, TVE_COLLAPSE, prevSelect );
      } else if ((INT)prevItem->parent)
      {
        newItem = (& infoPtr->items[(INT)prevItem->parent]);

        hNewSelection = newItem->hItem; //CB: what does Win32??? Please not the Corel hack!!!
      }

      break;

    case VK_RIGHT:
      if (TREEVIEW_HasChildren(hwnd, prevItem))
      {
        if (!(prevItem->state & TVIS_EXPANDED))
          TREEVIEW_Expand(hwnd, TVE_EXPAND, prevSelect );
         else
         {
           newItem = (& infoPtr->items[(INT)prevItem->firstChild]);
          hNewSelection = newItem->hItem;
         }
      }

      break;

    case VK_ADD:
      if (!(prevItem->state & TVIS_EXPANDED))
        TREEVIEW_Expand(hwnd, TVE_EXPAND, prevSelect );
      break;

    case VK_SUBTRACT:
      if (prevItem->state & TVIS_EXPANDED)
      TREEVIEW_Expand(hwnd, TVE_COLLAPSE, prevSelect );
    break;

    case VK_PRIOR:
      newItem=TREEVIEW_GetListItem(
              hwnd,
              infoPtr,
              prevItem,
              -1*(TREEVIEW_GetVisibleCount(hwnd,0,0)-3));
      if (!newItem)
        newItem = prevItem;

      hNewSelection = newItem->hItem;

      break;

    case VK_NEXT:
      newItem=TREEVIEW_GetListItem(
              hwnd,
              infoPtr,
              prevItem,
              TREEVIEW_GetVisibleCount(hwnd,0,0)-3);

      if (!newItem)
        newItem = prevItem;

      hNewSelection = newItem->hItem;

      break;

    case VK_RETURN:
      sendNotify(hwnd,NM_RETURN);
      break;

    case VK_BACK:

    default:
      break;
  }

  if (hNewSelection)
  {
    if (TREEVIEW_DoSelectItem(hwnd,TVGN_CARET,(HTREEITEM)hNewSelection,TVC_BYKEYBOARD))
    {
      TREEVIEW_EnsureVisible(hwnd,hNewSelection);
    }
  }

  return FALSE;
}

static LRESULT TREEVIEW_Char(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  CHAR ch = (CHAR)wParam;

  TREEVIEW_ISearch(hwnd,ch);

  return 0;
}

static LRESULT
TREEVIEW_GetScrollTime (HWND hwnd)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  return infoPtr->uScrollTime;
}

static LRESULT
TREEVIEW_SetScrollTime (HWND hwnd, UINT uScrollTime)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  UINT uOldScrollTime = infoPtr->uScrollTime;

  infoPtr->uScrollTime = min (uScrollTime, 100);

  return uOldScrollTime;
}

static LRESULT TREEVIEW_EnsureVisible(HWND hwnd,HTREEITEM hItem)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  TREEVIEW_ITEM *item;
  RECT rect;
  INT scrollY;

  item = TREEVIEW_ValidItem(infoPtr,hItem);

  if (!item) return FALSE;

  if (item && item->parent)
  {
    TREEVIEW_ITEM *parent = TREEVIEW_ValidItem(infoPtr,item->parent);

    while (parent && !(parent->state & TVIS_EXPANDED))
    {
      TREEVIEW_Expand(hwnd,TVE_EXPAND,(LPARAM)parent->hItem);
      parent = TREEVIEW_ValidItem(infoPtr,parent->parent);
    }
  }

  TREEVIEW_UnqueueRefresh(hwnd,TRUE,TRUE);
  GetClientRect(hwnd,&rect);
  if (item->rect.top < 0)
    scrollY = item->rect.top;
  else if (item->rect.bottom > rect.bottom)
  {
    INT mod;

    scrollY = item->rect.bottom-rect.bottom;
    mod = scrollY % infoPtr->uVScrollStep;
    if (mod) scrollY += infoPtr->uVScrollStep-mod;
  } else return FALSE;

  if (scrollY != 0)
  {
    infoPtr->lefttop.y += scrollY;
    TREEVIEW_CalcItems(hwnd,0,infoPtr);
    ScrollWindowEx(hwnd,0,-scrollY,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));

    return TRUE;
  }

  return FALSE;
}

static BOOL TREEVIEW_Compare(HWND hwnd,TREEVIEW_ITEM *item,LPWSTR text,INT textlen)
{
  if (item->pszText == LPSTR_TEXTCALLBACKW)
    TREEVIEW_SendDispInfoNotify(hwnd,item,TVN_GETDISPINFO,TVIF_TEXT);
  if (item->pszText == LPSTR_TEXTCALLBACKW) return FALSE;

  //simulate lstrcmpniW
  return CompareStringW(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,item->pszText,MIN(lstrlenW(item->pszText),textlen),text,textlen) == 2;
}

static TREEVIEW_ITEM* TREEVIEW_Search(HWND hwnd,TREEVIEW_INFO *infoPtr,INT iItem,LPWSTR text,INT textlen)
{
  TREEVIEW_ITEM *item,*start;
  BOOL found = FALSE;

  item = start = &infoPtr->items[iItem];

  //search start to end
  while (item)
  {
    if (TREEVIEW_Compare(hwnd,item,text,textlen)) return item;
    item = TREEVIEW_GetNextListItem(hwnd,infoPtr,item);
  }
  if (!found)
  {
    iItem = (INT)infoPtr->TopRootItem;
    item = &infoPtr->items[iItem];

    //search first to start
    while (item != start)
    {
      if (TREEVIEW_Compare(hwnd,item,text,textlen)) return item;
      item = TREEVIEW_GetNextListItem(hwnd,infoPtr,item);
    }
  }

  return NULL;
}

//CB: this method is CPU intense: optimize it and use a secondary thread

static VOID TREEVIEW_ISearch(HWND hwnd,CHAR ch)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);
  LPWSTR newString;
  INT len,iItem;
  CHAR ch2[2];
  TREEVIEW_ITEM *item;

  //check timer
  if (infoPtr->Timer & TV_ISEARCH_TIMER_SET)
  {
    KillTimer(hwnd,TV_ISEARCH_TIMER);
    infoPtr->Timer &= ~TV_ISEARCH_TIMER_SET;
  } else if (infoPtr->uISearchLen > 0)
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

  //search

  iItem = infoPtr->selectedItem ? (INT)infoPtr->selectedItem:(INT)infoPtr->TopRootItem;

  if (infoPtr->selectedItem)
  {
    item = &infoPtr->items[iItem];
    item = TREEVIEW_GetNextListItem(hwnd,infoPtr,item);
    if (!item) item = &infoPtr->items[(INT)infoPtr->TopRootItem];
    iItem = (INT)item->hItem;
  } else iItem = (INT)infoPtr->TopRootItem;

  item = TREEVIEW_Search(hwnd,infoPtr,iItem,newString,len);
  if (!item && infoPtr->selectedItem && infoPtr->pszISearch)
  {
    TREEVIEW_ITEM *next;

    item = &infoPtr->items[iItem];
    next = TREEVIEW_Search(hwnd,infoPtr,(INT)item->hItem,infoPtr->pszISearch,infoPtr->uISearchLen);
    if (next)
    {
      TREEVIEW_SelectItem(hwnd,(WPARAM)TVGN_CARET,(LPARAM)next->hItem);
      TREEVIEW_EnsureVisible(hwnd,next->hItem);
    }
    COMCTL32_Free(newString);
    return;
  }

  //done
  if (item)
  {
    COMCTL32_Free(infoPtr->pszISearch);
    infoPtr->pszISearch = newString;
    infoPtr->uISearchLen = len;
    TREEVIEW_SelectItem(hwnd,(WPARAM)TVGN_CARET,(LPARAM)item->hItem);
    TREEVIEW_EnsureVisible(hwnd,item->hItem);
    SetTimer(hwnd,TV_ISEARCH_TIMER,TV_ISEARCH_DELAY,0);
    infoPtr->Timer |= TV_ISEARCH_TIMER_SET;
  } else
  {
    COMCTL32_Free(newString);
    MessageBeep(0xFFFFFFFF);
  }
}

static LRESULT TREEVIEW_GetISearchString(HWND hwnd,LPWSTR lpsz,BOOL unicode)
{
  TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(hwnd);

  if (infoPtr->uISearchLen == 0) return 0;

  if (unicode)
    lstrcpyW(lpsz,infoPtr->pszISearch);
  else
    lstrcpyWtoA((LPSTR)lpsz,infoPtr->pszISearch);

  return infoPtr->uISearchLen;
}

static LRESULT TREEVIEW_SetCursor(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  sendNotify(hwnd,NM_SETCURSOR); //CB: todo: result

  return DefWindowProcA(hwnd,WM_SETCURSOR,wParam,lParam);
}

static LRESULT WINAPI
TREEVIEW_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_CREATE)
     return TREEVIEW_Create (hwnd, wParam, lParam);

   if (!TREEVIEW_GetInfoPtr(hwnd))
     return DefWindowProcA (hwnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
        case TVM_INSERTITEMA:
          return TREEVIEW_InsertItem(hwnd,wParam,lParam,FALSE);

        case TVM_INSERTITEMW:
          return TREEVIEW_InsertItem(hwnd,wParam,lParam,TRUE);

        case TVM_DELETEITEM:
                return TREEVIEW_DeleteItem (hwnd, wParam, lParam);

        case TVM_EXPAND:
                return TREEVIEW_Expand (hwnd, wParam, lParam);

        case TVM_GETITEMRECT:
                return TREEVIEW_GetItemRect (hwnd, wParam, lParam);

        case TVM_GETCOUNT:
                return TREEVIEW_GetCount (hwnd, wParam, lParam);

        case TVM_GETINDENT:
                return TREEVIEW_GetIndent (hwnd);

        case TVM_SETINDENT:
                return TREEVIEW_SetIndent (hwnd, wParam);

        case TVM_GETIMAGELIST:
                return TREEVIEW_GetImageList (hwnd, wParam, lParam);

        case TVM_SETIMAGELIST:
                return TREEVIEW_SetImageList (hwnd, wParam, lParam);

        case TVM_GETNEXTITEM:
                return TREEVIEW_GetNextItem (hwnd, wParam, lParam);

        case TVM_SELECTITEM:
                return TREEVIEW_SelectItem (hwnd, wParam, lParam);

        case TVM_GETITEMA:
                return TREEVIEW_GetItem(hwnd,wParam,lParam,FALSE);

        case TVM_GETITEMW:
                return TREEVIEW_GetItem(hwnd,wParam,lParam,TRUE);

        case TVM_SETITEMA:
                return TREEVIEW_SetItem(hwnd,wParam,lParam,FALSE);

        case TVM_SETITEMW:
                return TREEVIEW_SetItem(hwnd,wParam,lParam,TRUE);

        case TVM_EDITLABELA:
                return TREEVIEW_EditLabel(hwnd,(HTREEITEM)lParam,FALSE);

        case TVM_EDITLABELW:
                return TREEVIEW_EditLabel(hwnd,(HTREEITEM)lParam,TRUE);

        case TVM_GETEDITCONTROL:
                return TREEVIEW_GetEditControl (hwnd);

        case TVM_GETVISIBLECOUNT:
                return TREEVIEW_GetVisibleCount (hwnd, wParam, lParam);

        case TVM_HITTEST:
                return TREEVIEW_HitTest(hwnd,(LPTVHITTESTINFO)lParam);

        case TVM_CREATEDRAGIMAGE:
                return TREEVIEW_CreateDragImage (hwnd, wParam, lParam);

        case TVM_SORTCHILDREN:
                return TREEVIEW_SortChildren(hwnd, wParam, lParam);

        case TVM_ENSUREVISIBLE:
                return TREEVIEW_EnsureVisible(hwnd,(HTREEITEM)lParam);

        case TVM_SORTCHILDRENCB:
                return TREEVIEW_SortChildrenCB(hwnd, wParam, lParam);

        case TVM_ENDEDITLABELNOW:
                return TREEVIEW_EndEditLabelNow (hwnd,(BOOL)wParam);

        case TVM_GETISEARCHSTRINGA:
                return TREEVIEW_GetISearchString(hwnd,(LPWSTR)lParam,FALSE);

        case TVM_GETISEARCHSTRINGW:
                return TREEVIEW_GetISearchString(hwnd,(LPWSTR)lParam,TRUE);

        case TVM_GETTOOLTIPS:
                return TREEVIEW_GetToolTips (hwnd);

        case TVM_SETTOOLTIPS:
                return TREEVIEW_SetToolTips (hwnd, wParam);

        case TVM_SETINSERTMARK:
                return TREEVIEW_SetInsertMark (hwnd,wParam, lParam);

        case TVM_SETITEMHEIGHT:
                return TREEVIEW_SetItemHeight (hwnd, wParam);

        case TVM_GETITEMHEIGHT:
                return TREEVIEW_GetItemHeight (hwnd);

        case TVM_SETBKCOLOR:
                return TREEVIEW_SetBkColor (hwnd, wParam, lParam);

        case TVM_SETTEXTCOLOR:
                return TREEVIEW_SetTextColor (hwnd, wParam, lParam);

        case TVM_GETBKCOLOR:
                return TREEVIEW_GetBkColor (hwnd);

        case TVM_GETTEXTCOLOR:
                return TREEVIEW_GetTextColor (hwnd);

        case TVM_SETSCROLLTIME:
                return TREEVIEW_SetScrollTime (hwnd, (UINT)wParam);

        case TVM_GETSCROLLTIME:
                return TREEVIEW_GetScrollTime (hwnd);

        case TVM_GETITEMSTATE:
                return TREEVIEW_GetItemState (hwnd,wParam, lParam);

        case TVM_GETLINECOLOR:
                return TREEVIEW_GetLineColor (hwnd,wParam, lParam);

        case TVM_SETLINECOLOR:
                return TREEVIEW_SetLineColor (hwnd,wParam, lParam);

        case TVM_SETINSERTMARKCOLOR:
                return TREEVIEW_SetInsertMarkColor (hwnd,wParam, lParam);

        case TVM_GETINSERTMARKCOLOR:
                return TREEVIEW_GetInsertMarkColor (hwnd,wParam, lParam);

        case WM_COMMAND:
                return TREEVIEW_Command (hwnd, wParam, lParam);

        case WM_DESTROY:
                return TREEVIEW_Destroy (hwnd);

        case WM_ENABLE:
                return TREEVIEW_Enable(hwnd,wParam,lParam);

        case WM_ERASEBKGND:
                return TREEVIEW_EraseBackground (hwnd, wParam, lParam);

        case WM_GETDLGCODE:
                return TREEVIEW_GetDlgCode(hwnd,wParam,lParam);

        case WM_PAINT:
                return TREEVIEW_Paint (hwnd, wParam, lParam);

        case WM_GETFONT:
                return TREEVIEW_GetFont (hwnd, wParam, lParam);

        case WM_SETFONT:
                return TREEVIEW_SetFont (hwnd, wParam, lParam);

        case WM_KEYDOWN:
                return TREEVIEW_KeyDown (hwnd, wParam, lParam);

        case WM_CHAR:
                return TREEVIEW_Char(hwnd,wParam,lParam);

        case WM_SETFOCUS:
                return TREEVIEW_SetFocus (hwnd, wParam, lParam);

        case WM_KILLFOCUS:
                return TREEVIEW_KillFocus (hwnd, wParam, lParam);

        case WM_MOUSEMOVE:
                return TREEVIEW_MouseMove(hwnd,wParam,lParam);

        case WM_LBUTTONDOWN:
                return TREEVIEW_LButtonDown (hwnd, wParam, lParam);

        case WM_LBUTTONDBLCLK:
                return TREEVIEW_LButtonDoubleClick (hwnd, wParam, lParam);

        case WM_RBUTTONDOWN:
                return TREEVIEW_RButtonDown (hwnd, wParam, lParam);

        case WM_RBUTTONDBLCLK:
                return TREEVIEW_RButtonDoubleClick(hwnd,wParam,lParam);

        case WM_STYLECHANGED:
                return TREEVIEW_StyleChanged (hwnd, wParam, lParam);

        case WM_SYSCOLORCHANGE:
                return TREEVIEW_SysColorChange(hwnd,wParam,lParam);

        case WM_SETCURSOR:
                return TREEVIEW_SetCursor(hwnd,wParam,lParam);

        case WM_SETREDRAW:
                return TREEVIEW_SetRedraw(hwnd,wParam,lParam);

        case WM_TIMER:
                return TREEVIEW_HandleTimer (hwnd, wParam, lParam);

        case WM_SIZE:
                return TREEVIEW_Size (hwnd, wParam,lParam);

        case WM_HSCROLL:
                return TREEVIEW_HScroll (hwnd, wParam, lParam);

        case WM_VSCROLL:
                return TREEVIEW_VScroll (hwnd, wParam, lParam);

        case WM_MOUSEWHEEL:
                return TREEVIEW_MouseWheel (hwnd, wParam, lParam);

        case WM_DRAWITEM:
                //printf ("drawItem\n");
                return DefWindowProcA (hwnd, uMsg, wParam, lParam);

        default:
              //if (uMsg >= WM_USER)
              //  FIXME (treeview, "Unknown msg %04x wp=%08x lp=%08lx\n",
              //     uMsg, wParam, lParam);
            return defComCtl32ProcA(hwnd,uMsg,wParam,lParam);
      }
    return 0;
}


VOID TREEVIEW_Register (VOID)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)TREEVIEW_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(TREEVIEW_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = 0;
    wndClass.lpszClassName = WC_TREEVIEWA;

    RegisterClassA (&wndClass);
}


VOID TREEVIEW_Unregister (VOID)
{
    UnregisterClassA (WC_TREEVIEWA, (HINSTANCE)NULL);
}

