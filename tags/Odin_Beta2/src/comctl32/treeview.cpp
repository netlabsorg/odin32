/* $Id: treeview.cpp,v 1.19 2001-09-15 09:13:31 sandervl Exp $ */
/* Treeview control
 *
 * Copyright 1998 Eric Kohl <ekohl@abo.rhein-zeitung.de>
 * Copyright 1998,1999 Alex Priem <alexp@sci.kun.nl>
 * Copyright 1999 Sylvain St-Germain
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999-2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Note that TREEVIEW_INFO * and HTREEITEM are the same thing.
 *
 * Note2: All items always! have valid (allocated) pszText field.
 *      If item's text == LPSTR_TEXTCALLBACKA/W we allocate buffer
 *      of size TEXT_CALLBACK_SIZE in DoSetItem.
 *      We use callbackMask to keep track of fields to be updated.
 *
 *
 * Status: complete (many things untested)
 * Version: 5.80
 */

/*
 Most identical with:
 - Corel 20000807 level
 - (WINE 991212 level)
*/

/* CB: todo/bugs
 - startup: missing WM_SIZE messages -> i.e. wrong info tip calculation
 - bug in SetScrollInfo/ShowScrollBar: WM_SIZE and WM_NCPAINT problems (i.e. RegEdit)
 - VK_LEFT in WinHlp32 displays expanded icon
 - expand not finished
 - WM_ENABLE: draw disabled control
 - AMD cpuid: text not drawn (> treeview.cpp,v 1.13 2000/05/10 19:50:33)
  t1: GDI32: SetBkColor to FFFFFF
  t1: GDI32: GetTextMetricsA returned 1
  t1: GDI32: SetTextAlign
-->
  t1: GDI32: RestoreDC
  t1: SendInternalMessageA WM_NOTIFY for 68000003 65 1312e4
  t1: USER32:  GetDlgCtrlID

  missing lines at -->:
  t1: GDI32: GetTextExtentPoint32A 10001a1 Processor Speed Test 20 returned 1 (93,14)
  t1: GDI32: ExtTextOutA 10001a1 Processor Speed Test
  t1: GDI32: GetTextAlign
  t1: GDI32: GetTextExtentPoint32A 10001a1 454 Mhz 7 returned 1 (37,14)
  t1: GDI32: ExtTextOutA 10001a1 454 Mhz
  t1: GDI32: GetTextAlign

  don't know why it fails
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "winbase.h"
#include "wingdi.h"
#include "winnls.h"
#include "commctrl.h"
#include "comctl32.h"
#include "ccbase.h"
#include "treeview.h"

/* ffs should be in <string.h>. */

//#define NDEBUG
#define NDEBUG_TEXT

#define TEXT_CALLBACK_SIZE 260

#define TREEVIEW_LEFT_MARGIN 8

#define MINIMUM_INDENT 19

#define CALLBACK_MASK_ALL (TVIF_TEXT|TVIF_CHILDREN|TVIF_IMAGE|TVIF_SELECTEDIMAGE)

#define STATEIMAGEINDEX(x)   (((x) >> 12) & 0x0f)
#define OVERLAYIMAGEINDEX(x) (((x) >> 8) & 0x0f)
#define ISVISIBLE(x)         ((x)->displayed)
#define INCLIENT(x)          ((x)->inclient)

typedef VOID (*TREEVIEW_ItemEnumFunc)(TREEVIEW_INFO *, TREEVIEW_ITEM *,LPVOID);

static BOOL    TREEVIEW_UnqueueRefresh(TREEVIEW_INFO *,BOOL calc,BOOL refresh);
static void    TREEVIEW_QueueRefresh(TREEVIEW_INFO *);
static void    TREEVIEW_Refresh(TREEVIEW_INFO *infoPtr);
static void    TREEVIEW_RefreshItem(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item,DWORD changed);

static VOID    TREEVIEW_HideInfoTip(TREEVIEW_INFO *infoPtr);
static LRESULT TREEVIEW_DoSelectItem(TREEVIEW_INFO *, INT, HTREEITEM, INT);
static LRESULT TREEVIEW_EnsureVisible(TREEVIEW_INFO *,HTREEITEM,BOOL);
static LRESULT TREEVIEW_RButtonUp(TREEVIEW_INFO *, LPPOINT);
static LRESULT TREEVIEW_EndEditLabelNow(TREEVIEW_INFO *infoPtr, BOOL bCancel);
static VOID    TREEVIEW_CheckInfoTip(TREEVIEW_INFO *infoPtr);
static VOID    TREEVIEW_ISearch(TREEVIEW_INFO *infoPtr,CHAR ch);

/* Random Utilities *****************************************************/

#ifdef NDEBUG
static inline void
TREEVIEW_VerifyTree(TREEVIEW_INFO *infoPtr)
{
    (void)infoPtr;
}

#else
/* The definition is at the end of the file. */
static void TREEVIEW_VerifyTree(TREEVIEW_INFO *infoPtr);
#ifndef NDEBUG_TEXT
static void TREEVIEW_WriteVerify(CHAR *text)
{
  dprintf((text));
}
#else
static inline void TREEVIEW_WriteVerify(CHAR * text) {}
#endif
#endif

/* Returns the treeview private data if hwnd is a treeview.
 * Otherwise returns an undefined value. */
#define TREEVIEW_GetInfoPtr(hwnd) ((TREEVIEW_INFO *)getInfoPtr(hwnd))

/* Don't call this. Nothing wants an item index. */
static inline int
TREEVIEW_GetItemIndex(TREEVIEW_INFO *infoPtr, HTREEITEM handle)
{
    assert(infoPtr != NULL);

    return DPA_GetPtrIndex(infoPtr->items,handle);
}

/***************************************************************************
 * This method checks that handle is an item for this tree.
 */
static BOOL
TREEVIEW_ValidItem(TREEVIEW_INFO *infoPtr,HTREEITEM handle)
{
    if (TREEVIEW_GetItemIndex(infoPtr, handle) == -1)
    {
        //TRACE("invalid item %p\n", handle);
        return FALSE;
    }
    else
        return TRUE;
}

static HFONT
TREEVIEW_CreateBoldFont(HFONT hOrigFont)
{
    LOGFONTA font;

    GetObjectA(hOrigFont, sizeof(font), &font);
    font.lfWeight = FW_BOLD;

    return CreateFontIndirectA(&font);
}

static inline HFONT
TREEVIEW_FontForItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    return (item->state & TVIS_BOLD) ? infoPtr->hBoldFont : infoPtr->hFont;
}

/* for trace/debugging purposes only */
static const WCHAR *
TREEVIEW_ItemName(TREEVIEW_ITEM *item)
{
    if (item == NULL) return (WCHAR*)L"<null item>";
    if (item->pszText == LPSTR_TEXTCALLBACKW) return (WCHAR*)L"<callback>";
    if (item->pszText == NULL) return (WCHAR*)L"<null>";

    return item->pszText;
}

/* An item is not a child of itself. */
static BOOL
TREEVIEW_IsChildOf(TREEVIEW_ITEM *parent, TREEVIEW_ITEM *child)
{
    do
    {
        child = child->parent;
        if (child == parent) return TRUE;
    } while (child != NULL);

    return FALSE;
}

/* Tree Traversal *******************************************************/

/***************************************************************************
 * This method returns the last expanded sibling or child child item
 * of a tree node
 */
static TREEVIEW_ITEM *
TREEVIEW_GetLastListItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem)
{
    if (!wineItem)
       return NULL;

    while (wineItem->lastChild)
    {
       if (wineItem->state & TVIS_EXPANDED)
          wineItem = wineItem->lastChild;
       else
          break;
    }

    if (wineItem == infoPtr->root)
        return NULL;

    return wineItem;
}

/***************************************************************************
 * This method returns the previous non-hidden item in the list not
 * considering the tree hierarchy.
 */
static TREEVIEW_ITEM *
TREEVIEW_GetPrevListItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *tvItem)
{
    if (tvItem->prevSibling)
    {
        /* This item has a prevSibling, get the last item in the sibling's tree. */
        TREEVIEW_ITEM *upItem = tvItem->prevSibling;

        if ((upItem->state & TVIS_EXPANDED) && upItem->lastChild != NULL)
            return TREEVIEW_GetLastListItem(infoPtr, upItem->lastChild);
        else
            return upItem;
    }
    else
    {
        /* this item does not have a prevSibling, get the parent */
        return (tvItem->parent != infoPtr->root) ? tvItem->parent : NULL;
    }
}


/***************************************************************************
 * This method returns the next physical item in the treeview not
 * considering the tree hierarchy.
 */
static TREEVIEW_ITEM *
TREEVIEW_GetNextListItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *tvItem)
{
    assert(tvItem != NULL);

    /*
     * If this item has children and is expanded, return the first child
     */
    if ((tvItem->state & TVIS_EXPANDED) && tvItem->firstChild != NULL)
    {
        return tvItem->firstChild;
    }


    /*
     * try to get the sibling
     */
    if (tvItem->nextSibling)
        return tvItem->nextSibling;

    /*
     * Otherwise, get the parent's sibling.
     */
    while (tvItem->parent)
    {
        tvItem = tvItem->parent;

        if (tvItem->nextSibling)
            return tvItem->nextSibling;
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
static TREEVIEW_ITEM *
TREEVIEW_GetListItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem,
                     LONG count)
{
    TREEVIEW_ITEM *(*next_item)(TREEVIEW_INFO *, TREEVIEW_ITEM *);
    TREEVIEW_ITEM *previousItem;

    assert(wineItem != NULL);

    if (count > 0)
    {
        next_item = TREEVIEW_GetNextListItem;
    }
    else if (count < 0)
    {
        count = -count;
        next_item = TREEVIEW_GetPrevListItem;
    }
    else
        return wineItem;

    do
    {
        previousItem = wineItem;
        wineItem = next_item(infoPtr, wineItem);

    } while (--count && wineItem != NULL);


    return wineItem ? wineItem : previousItem;
}

static VOID
TREEVIEW_ForEachDisplayed(TREEVIEW_INFO *infoPtr,TREEVIEW_ItemEnumFunc func,LPVOID data)
{
    TREEVIEW_ITEM *item;

    for (item = infoPtr->root->firstChild; item != NULL;
         item = TREEVIEW_GetNextListItem(infoPtr, item))
    {
        func(infoPtr, item, data);
    }
}

/* Notifications */

static VOID
TREEVIEW_TVItemFromItem(UINT mask,TVITEMW *tvItem,TREEVIEW_ITEM *item)
{
    tvItem->mask = mask;
    tvItem->hItem = item;
    tvItem->state = item->state;
    tvItem->stateMask = 0;
    tvItem->iImage = item->iImage;
    tvItem->pszText = item->pszText;
    tvItem->cchTextMax = item->cchTextMax;
    tvItem->iImage = item->iImage;
    tvItem->iSelectedImage = item->iSelectedImage;
    tvItem->cChildren = item->cChildren;
    tvItem->lParam = item->lParam;
}

static BOOL TREEVIEW_SendTreeviewNotify(TREEVIEW_INFO *infoPtr,UINT code,UINT action,UINT mask,HTREEITEM oldItem,HTREEITEM newItem)
{
  NMTREEVIEWW nmhdr;
  CHAR *oldText = NULL,*newText = NULL;
  BOOL rc;

  ZeroMemory(&nmhdr,sizeof(NMTREEVIEWW));

  nmhdr.action = action;
  if (oldItem)
  {
    TREEVIEW_TVItemFromItem(mask, &nmhdr.itemOld, oldItem);
    if (!isUnicodeNotify(&infoPtr->header))
    {
      if (!oldItem->pszText || (oldItem->pszText == LPSTR_TEXTCALLBACKW)) nmhdr.itemOld.pszText = NULL; else
      {
        INT len = lstrlenW(oldItem->pszText)+1;

        oldText = (CHAR*)COMCTL32_Alloc(len);
        lstrcpyWtoA(oldText,oldItem->pszText);
        nmhdr.itemOld.pszText = (WCHAR*)oldText;
        nmhdr.itemOld.cchTextMax = len;
      }
    }
  }

  if (newItem)
  {
    TREEVIEW_TVItemFromItem(mask, &nmhdr.itemNew, newItem);
    if (!isUnicodeNotify(&infoPtr->header))
    {
      if (!newItem->pszText || (newItem->pszText == LPSTR_TEXTCALLBACKW)) nmhdr.itemNew.pszText = NULL; else
      {
        INT len = lstrlenW(newItem->pszText)+1;

        newText = (CHAR*)COMCTL32_Alloc(len);
        lstrcpyWtoA(newText,newItem->pszText);
        nmhdr.itemNew.pszText = (WCHAR*)newText;
        nmhdr.itemNew.cchTextMax = len;
      }
    }
  }

  nmhdr.ptDrag.x = 0;
  nmhdr.ptDrag.y = 0;

  rc = (BOOL)sendNotify(infoPtr->hwnd,code,&nmhdr.hdr);

  if (oldText) COMCTL32_Free(oldText);
  if (newText) COMCTL32_Free(newText);

  return rc;
}

static BOOL
TREEVIEW_SendTreeviewDnDNotify (TREEVIEW_INFO *infoPtr,UINT code,HTREEITEM dragItem,POINT pt)
{
  NMTREEVIEWA nmhdr;

  nmhdr.action = 0;
  nmhdr.itemNew.mask = TVIF_STATE | TVIF_PARAM | TVIF_HANDLE;
  nmhdr.itemNew.hItem = dragItem;
  nmhdr.itemNew.state = dragItem->state;
  nmhdr.itemNew.lParam = dragItem->lParam;

  nmhdr.ptDrag.x = pt.x;
  nmhdr.ptDrag.y = pt.y;

  return (BOOL)sendNotify(infoPtr->hwnd,code,&nmhdr.hdr);
}

static BOOL
TREEVIEW_SendCustomDrawNotify(TREEVIEW_INFO *infoPtr,DWORD dwDrawStage,HDC hdc,RECT rc)
{
  NMTVCUSTOMDRAW nmcdhdr;
  LPNMCUSTOMDRAW nmcd;

  nmcd = &nmcdhdr.nmcd;
  nmcd->dwDrawStage = dwDrawStage;
  nmcd->hdc         = hdc;
  nmcd->rc          = rc;
  nmcd->dwItemSpec  = 0;
  nmcd->uItemState  = 0;
  nmcd->lItemlParam = 0;
  nmcdhdr.clrText   = infoPtr->clrText;
  nmcdhdr.clrTextBk = infoPtr->clrBk;
  nmcdhdr.iLevel    = 0;

  return (BOOL)sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&nmcdhdr.nmcd.hdr);
}

/* FIXME: need to find out when the flags in uItemState need to be set */

static BOOL
TREEVIEW_SendCustomDrawItemNotify(TREEVIEW_INFO *infoPtr,HDC hdc,TREEVIEW_ITEM *wineItem,UINT uItemDrawState)
{
  NMTVCUSTOMDRAW nmcdhdr;
  LPNMCUSTOMDRAW nmcd;
  DWORD dwDrawStage,dwItemSpec;
  UINT uItemState;
  INT retval;

  dwDrawStage = CDDS_ITEM | uItemDrawState;
  dwItemSpec = (DWORD)wineItem;
  uItemState = 0;
  if (wineItem->state & TVIS_SELECTED)
  {
    uItemState |= CDIS_SELECTED;
    if (GetFocus() == infoPtr->hwnd) uItemState |= CDIS_FOCUS;
  }
  if (wineItem == infoPtr->selectedItem)
    uItemState |= CDIS_FOCUS;
  if (wineItem == infoPtr->hotItem)
    uItemState |= CDIS_HOT;

  nmcd = &nmcdhdr.nmcd;
  nmcd->dwDrawStage = dwDrawStage;
  nmcd->hdc         = hdc;
  nmcd->rc          = wineItem->rect;
  nmcd->dwItemSpec  = dwItemSpec;
  nmcd->uItemState  = uItemState;
  nmcd->lItemlParam = wineItem->lParam;
  nmcdhdr.clrText   = infoPtr->clrText;
  nmcdhdr.clrTextBk = infoPtr->clrBk;
  nmcdhdr.iLevel    = wineItem->iLevel;

  retval = sendNotify(infoPtr->hwnd,NM_CUSTOMDRAW,&nmcdhdr.nmcd.hdr);

  infoPtr->clrText = nmcdhdr.clrText;
  infoPtr->clrBk   = nmcdhdr.clrTextBk;

  return (BOOL)retval;
}

static void
TREEVIEW_UpdateDispInfo(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *wineItem,UINT mask)
{
  NMTVDISPINFOW callback;

  mask &= wineItem->callbackMask;

  if (mask == 0) return;

  /* 'state' always contains valid value, as well as 'lParam'.
   * All other parameters are uninitialized.
   */

  callback.item.pszText         = wineItem->pszText;
  callback.item.cchTextMax      = wineItem->cchTextMax;
  callback.item.mask            = mask;
  callback.item.hItem           = wineItem;
  callback.item.state           = wineItem->state;
  callback.item.lParam = wineItem->lParam;

  /*
  callback.item.stateMask       = 0;
  callback.item.iImage          = wineItem->iImage;
  callback.item.iSelectedImage  = wineItem->iSelectedImage;
  callback.item.cChildren       = wineItem->cChildren;
  */

  sendNotify(infoPtr->hwnd,isUnicodeNotify(&infoPtr->header) ? TVN_GETDISPINFOW:TVN_GETDISPINFOA,&callback.hdr);

  /* It may have changed due to a call to SetItem. */
  mask &= wineItem->callbackMask;

  if (mask & TVIF_TEXT)
  {
    if (callback.item.pszText != wineItem->pszText)
    {
      INT len = MAX((isUnicodeNotify(&infoPtr->header) ? lstrlenW(callback.item.pszText):lstrlenA((CHAR*)callback.item.pszText))+1,TEXT_CALLBACK_SIZE);

      wineItem->pszText = (WCHAR*)COMCTL32_ReAlloc(wineItem->pszText,len*sizeof(WCHAR));
      if (isUnicodeNotify(&infoPtr->header))
        lstrcpyW(wineItem->pszText,callback.item.pszText);
      else
        lstrcpyAtoW(wineItem->pszText,(CHAR*)callback.item.pszText);
      wineItem->cchTextMax = len;
    } else if (!isUnicodeNotify(&infoPtr->header))
    {
      WCHAR *newText;
      INT len = MAX(lstrlenA((CHAR*)callback.item.pszText)+1,TEXT_CALLBACK_SIZE);

      newText = (WCHAR*)COMCTL32_Alloc(len*sizeof(WCHAR));
      lstrcpyAtoW(newText,(LPSTR)callback.item.pszText);
      if (wineItem->pszText) COMCTL32_Free(wineItem->pszText);
      wineItem->pszText = newText;
      wineItem->cchTextMax = len;
    }
  }

  if (mask & TVIF_IMAGE)
    wineItem->iImage = callback.item.iImage;

  if (mask & TVIF_SELECTEDIMAGE)
    wineItem->iSelectedImage = callback.item.iSelectedImage;

  if (mask & TVIF_CHILDREN)
    wineItem->cChildren = callback.item.cChildren;

  /* These members are now permanently set. */
  if (callback.item.mask & TVIF_DI_SETITEM)
    wineItem->callbackMask &= ~callback.item.mask;
}

static VOID TREEVIEW_SendKeyDownNotify(TREEVIEW_INFO *infoPtr,UINT code,WORD wVKey)
{
  NMTVKEYDOWN nmkdhdr;

  nmkdhdr.wVKey = wVKey;
  nmkdhdr.flags = 0;

  sendNotify(infoPtr->hwnd,code,&nmkdhdr.hdr);
}

/***************************************************************************
 * This function uses cChildren field to decide whether the item has
 * children or not.
 * Note: if this returns TRUE, the child items may not actually exist,
 * they could be virtual.
 *
 * Just use wineItem->firstChild to check for physical children.
 */
static INT
TREEVIEW_HasChildren(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *wineItem)
{
  TREEVIEW_UpdateDispInfo(infoPtr, wineItem, TVIF_CHILDREN);

  return wineItem->cChildren > 0;
}

/* Item Position ********************************************************/

static VOID
TREEVIEW_ComputeTextWidth(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item,HDC hDC,BOOL useFont)
{
    HDC hdc;
    HFONT hOldFont = 0;
    RECT rect;

    /* DRAW's OM docker creates items like this */
    if (item->pszText == NULL)
    {
        item->textWidth = 0;
        return;
    }

    hdc = hDC ? hDC:GetDC(infoPtr->hwnd);
    if (!hDC || !useFont) hOldFont = SelectObject(hdc, TREEVIEW_FontForItem(infoPtr, item));

    SetRectEmpty(&rect);
    DrawTextW (hdc,item->pszText,lstrlenW(item->pszText),&rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);
    item->textWidth = rect.right-rect.left;
    item->textHeight = rect.bottom-rect.top;
    item->rect.right = item->textOffset+item->textWidth+4;

    if (!hDC || !useFont) SelectObject(hdc, hOldFont);
    if (!hDC) ReleaseDC(infoPtr->hwnd,hdc);
}

static INT CALLBACK
TREEVIEW_ResetItemOrderVisible(LPVOID pItem, DWORD unused)
{
    TREEVIEW_ITEM *item = (TREEVIEW_ITEM *)pItem;
    (void)unused;

    item->displayOrder = -1;
    item->displayed = FALSE;
    item->inclient = FALSE;

    return 1;
}

static VOID TREEVIEW_CalcItem(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item,HDC hdc)
{
  TREEVIEW_UpdateDispInfo(infoPtr,item,TVIF_TEXT);
  TREEVIEW_ComputeTextWidth(infoPtr,item,hdc,FALSE);
  item->calculated = TRUE;
}

static BOOL TREEVIEW_CalcItems(TREEVIEW_INFO *infoPtr)
{
  TREEVIEW_ITEM *item;
  int order = 0;
  BOOL lar = ((infoPtr->dwStyle & (TVS_LINESATROOT|TVS_HASLINES|TVS_HASBUTTONS)) > TVS_LINESATROOT);
  HDC hdc = 0;
  INT maxXScroll,maxYScroll;
  INT xDiff,yDiff;
  BOOL changedLeftTop = FALSE;
  RECT rc;

  TREEVIEW_HideInfoTip(infoPtr);

  // 1) reset
  infoPtr->treeWidth = 0;
  infoPtr->treeHeight = 0;
  GetClientRect(infoPtr->hwnd,&rc);
  infoPtr->clientWidth = rc.right;
  infoPtr->clientHeight = rc.bottom;
  DPA_EnumCallback(infoPtr->items,TREEVIEW_ResetItemOrderVisible,(LPARAM)NULL);

  // 2) get order and tree height
  infoPtr->firstVisible = NULL;
  for (item = infoPtr->root->firstChild;item != NULL;item = TREEVIEW_GetNextListItem(infoPtr, item))
  {
    INT itemW;

    item->displayOrder = order;

    if ((infoPtr->uInternalStatus & TV_CALCALL) || !item->calculated)
    {
      if (!hdc) hdc = GetDC(infoPtr->hwnd);
      TREEVIEW_CalcItem(infoPtr,item,hdc);
    }

    //set internal metrics
    item->linesOffset = infoPtr->uIndent * (item->iLevel + lar - 1) - infoPtr->lefttop.x;
    item->stateOffset = item->linesOffset + infoPtr->uIndent;
    item->imageOffset = item->stateOffset + (STATEIMAGEINDEX(item->state) ? infoPtr->stateImageWidth : 0);
    item->textOffset  = item->imageOffset + infoPtr->normalImageWidth;

    //set item rect
    item->rect.top = infoPtr->uItemHeight*item->displayOrder-infoPtr->lefttop.y;
    item->rect.bottom = item->rect.top+infoPtr->uItemHeight*item->iIntegral;
    item->rect.left = item->stateOffset;
    item->rect.right = item->textOffset+item->textWidth+4;
    item->displayed = TRUE;
    item->inclient = (((item->rect.top >= 0) || (item->rect.bottom >= 0)) && (item->rect.top < infoPtr->clientHeight));
    if (!infoPtr->firstVisible && item->inclient) infoPtr->firstVisible = item;

    itemW = item->textOffset+item->textWidth+4+infoPtr->lefttop.x;
    if (itemW > infoPtr->treeWidth)
      infoPtr->treeWidth = itemW;

    infoPtr->treeHeight += item->rect.bottom-item->rect.top;

    order += item->iIntegral;
  }
  infoPtr->maxDisplayOrder = order;

  if (hdc) ReleaseDC(infoPtr->hwnd,hdc);
  infoPtr->uInternalStatus &= ~TV_CALCALL;

  // 3) check lefttop
  yDiff = xDiff = 0;

  maxYScroll = infoPtr->treeHeight-infoPtr->clientHeight;
  if (maxYScroll < 0) maxYScroll = 0;
  if (infoPtr->lefttop.y > maxYScroll)
  {
    INT mod = maxYScroll % infoPtr->uItemHeight;

    if (mod > 0) maxYScroll += infoPtr->uItemHeight-mod;
    yDiff = infoPtr->lefttop.y-maxYScroll;
    infoPtr->lefttop.y = maxYScroll;
  }

  maxXScroll = infoPtr->treeWidth-infoPtr->clientWidth;
  if (maxXScroll < 0) maxXScroll = 0;
  if (infoPtr->lefttop.x > maxXScroll)
  {
    xDiff = infoPtr->lefttop.x-maxXScroll;
    infoPtr->lefttop.x = maxXScroll;
  }

  changedLeftTop = xDiff || yDiff;
  if (changedLeftTop)
  {
    //update visible flag and position
    infoPtr->firstVisible = NULL;
    for (item = infoPtr->root->firstChild;item != NULL;item = TREEVIEW_GetNextListItem(infoPtr, item))
    {
      item->rect.top += yDiff;
      item->rect.bottom += yDiff;
      item->rect.left += xDiff;
      item->rect.right += xDiff;
      item->inclient = (((item->rect.top >= 0) || (item->rect.bottom >= 0)) && (item->rect.top < infoPtr->clientHeight));
      if (!infoPtr->firstVisible && item->inclient) infoPtr->firstVisible = item;
      item->linesOffset += xDiff;
      item->stateOffset += xDiff;
      item->imageOffset += xDiff;
      item->textOffset  += xDiff;
    }
  }

  // 4) set scrollbars
  if (!(infoPtr->dwStyle & TVS_NOSCROLL) && (infoPtr->clientHeight > 0) && (infoPtr->clientWidth > 0))
  {
    if (infoPtr->treeHeight > infoPtr->clientHeight)
    {
      SCROLLINFO info;
      INT visH = ((INT)(infoPtr->clientHeight/infoPtr->uItemHeight))*infoPtr->uItemHeight;

      info.cbSize = sizeof(info);
      info.nMin   = 0;
      info.nMax   = infoPtr->treeHeight-1;
      info.nPos   = infoPtr->lefttop.y;
      info.nPage  = visH;
      info.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
      infoPtr->uInternalStatus |= TV_VSCROLL;
      SetScrollInfo(infoPtr->hwnd,SB_VERT,&info,TRUE);
    } else
    {
      if (infoPtr->uInternalStatus & TV_VSCROLL)
        ShowScrollBar(infoPtr->hwnd,SB_VERT,FALSE);
      infoPtr->uInternalStatus &= ~TV_VSCROLL;
    }
    if (!(infoPtr->dwStyle & TVS_NOHSCROLL) && (infoPtr->treeWidth > infoPtr->clientWidth))
    {
      SCROLLINFO info;

      info.cbSize = sizeof(info);
      info.nMin   = 0;
      info.nMax   = infoPtr->treeWidth-1;
      info.nPos   = infoPtr->lefttop.x;
      info.nPage  = MAX(infoPtr->clientWidth,1);
      info.fMask  = SIF_RANGE | SIF_POS | SIF_PAGE;
      infoPtr->uInternalStatus |= TV_HSCROLL;
      SetScrollInfo(infoPtr->hwnd,SB_HORZ,&info,TRUE);
    } else
    {
      if (infoPtr->uInternalStatus & TV_HSCROLL)
        ShowScrollBar(infoPtr->hwnd,SB_HORZ,FALSE);
      infoPtr->uInternalStatus &= ~TV_HSCROLL;
    }
  } else
  {
    if (infoPtr->uInternalStatus & (TV_VSCROLL | TV_HSCROLL))
      ShowScrollBar(infoPtr->hwnd,SB_BOTH,FALSE);
    infoPtr->uInternalStatus &= ~(TV_VSCROLL | TV_HSCROLL);
  }

  infoPtr->dwStyle = GetWindowLongA(infoPtr->hwnd,GWL_STYLE);

  return changedLeftTop;
}

static VOID TREEVIEW_MoveItems(TREEVIEW_INFO *infoPtr,INT xScroll,INT yScroll)
{
  TREEVIEW_ITEM *item;

  infoPtr->firstVisible = NULL;
  for (item = infoPtr->root->firstChild;item;item = TREEVIEW_GetNextListItem(infoPtr,item))
  {
    item->rect.top += yScroll;
    item->rect.bottom += yScroll;
    item->rect.left += xScroll;
    item->rect.right += xScroll;
    item->inclient = (((item->rect.top >= 0) || (item->rect.bottom >= 0)) && (item->rect.top < infoPtr->clientHeight));
    if (!infoPtr->firstVisible && item->inclient) infoPtr->firstVisible = item;

    //set internal metrics
    item->linesOffset += xScroll;
    item->stateOffset += xScroll;
    item->imageOffset += xScroll;
    item->textOffset  += xScroll;
  }
}

/* Item Allocation **********************************************************/

static TREEVIEW_ITEM *
TREEVIEW_AllocateItem(TREEVIEW_INFO *infoPtr)
{
    TREEVIEW_ITEM *newItem = (TREEVIEW_ITEM*)COMCTL32_Alloc(sizeof(TREEVIEW_ITEM));

    if (!newItem)
        return NULL;

    ZeroMemory(newItem,sizeof(TREEVIEW_ITEM));
    if (DPA_InsertPtr(infoPtr->items, INT_MAX, newItem) == -1)
    {
        COMCTL32_Free(newItem);
        return NULL;
    }

    return newItem;
}

/* Exact opposite of TREEVIEW_AllocateItem. In particular, it does not
 * free item->pszText. */
static void
TREEVIEW_FreeItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    DPA_DeletePtr(infoPtr->items, DPA_GetPtrIndex(infoPtr->items, item));
    COMCTL32_Free(item);
}


/* Item Insertion *******************************************************/

/***************************************************************************
 * This method inserts newItem before sibling as a child of parent.
 * sibling can be NULL, but only if parent has no children.
 */
static void
TREEVIEW_InsertBefore(TREEVIEW_ITEM *newItem, TREEVIEW_ITEM *sibling,
                      TREEVIEW_ITEM *parent)
{
    assert(newItem != NULL);
    assert(parent != NULL);

    if (sibling != NULL)
    {
        assert(sibling->parent == parent);

        if (sibling->prevSibling != NULL)
            sibling->prevSibling->nextSibling = newItem;

        newItem->prevSibling = sibling->prevSibling;
        sibling->prevSibling = newItem;
    } else newItem->prevSibling = NULL;

    newItem->nextSibling = sibling;

    if (parent->firstChild == sibling)
        parent->firstChild = newItem;

    if (parent->lastChild == NULL)
        parent->lastChild = newItem;
}

/***************************************************************************
 * This method inserts newItem after sibling as a child of parent.
 * sibling can be NULL, but only if parent has no children.
 */
static void
TREEVIEW_InsertAfter(TREEVIEW_ITEM *newItem, TREEVIEW_ITEM *sibling,
                     TREEVIEW_ITEM *parent)
{
    assert(newItem != NULL);
    assert(parent != NULL);

    if (sibling != NULL)
    {
        assert(sibling->parent == parent);

        if (sibling->nextSibling != NULL)
            sibling->nextSibling->prevSibling = newItem;

        newItem->nextSibling = sibling->nextSibling;
        sibling->nextSibling = newItem;
    } else newItem->nextSibling = NULL;

    newItem->prevSibling = sibling;

    if (parent->lastChild == sibling)
        parent->lastChild = newItem;

    if (parent->firstChild == NULL)
        parent->firstChild = newItem;
}

static BOOL
TREEVIEW_DoSetItem(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *wineItem,const TVITEMEXW *tvItem,BOOL unicode,DWORD *changed = NULL)
{
  UINT callbackClear = 0;
  UINT callbackSet = 0;

  if (changed) *changed = 0;

  /* Do this first in case it fails. */

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
        INT len = lstrlenW(tvItem->pszText)+1;

        callbackClear |= TVIF_TEXT;

        if (changed && ((wineItem->callbackMask & LVIF_TEXT) || (lstrcmpW(wineItem->pszText,tvItem->pszText) != 0))) *changed |= TVIF_TEXT;
        if (len > wineItem->cchTextMax)
        {
          LPWSTR newText = (WCHAR*)COMCTL32_ReAlloc(wineItem->pszText,len*sizeof(WCHAR));
          if (newText == NULL) return FALSE;

          wineItem->pszText = newText;
          wineItem->cchTextMax = len;
        }
        lstrcpyW (wineItem->pszText,tvItem->pszText);
      } else
      {
        callbackSet |= TVIF_TEXT;

        wineItem->pszText = (WCHAR*)COMCTL32_ReAlloc(wineItem->pszText,TEXT_CALLBACK_SIZE*sizeof(WCHAR));
        wineItem->cchTextMax = TEXT_CALLBACK_SIZE;
        if (changed) *changed |= TVIF_TEXT;
      }
    } else
    {
      if ((LPSTR)tvItem->pszText != LPSTR_TEXTCALLBACKA)
      {
        INT len = lstrlenA((LPSTR)tvItem->pszText)+1;

        callbackClear |= TVIF_TEXT;

        if (changed && ((wineItem->callbackMask & LVIF_TEXT) || (lstrcmpAtoW((CHAR*)tvItem->pszText,wineItem->pszText) != 0))) *changed |= TVIF_TEXT;
        if (len > wineItem->cchTextMax)
        {
          LPWSTR newText = (WCHAR*)COMCTL32_ReAlloc(wineItem->pszText,len*sizeof(WCHAR));
          if (newText == NULL) return FALSE;

          wineItem->pszText = newText;
          wineItem->cchTextMax = len;
        }
        lstrcpyAtoW (wineItem->pszText,(LPSTR)tvItem->pszText);
      } else
      {
        callbackSet |= TVIF_TEXT;

        wineItem->pszText = (WCHAR*)COMCTL32_ReAlloc(wineItem->pszText,TEXT_CALLBACK_SIZE*sizeof(WCHAR));
        wineItem->cchTextMax = TEXT_CALLBACK_SIZE;
        if (changed) *changed |= TVIF_TEXT;
      }
    }
  }

  if (tvItem->mask & TVIF_CHILDREN)
  {
    if (changed && (wineItem->cChildren != tvItem->cChildren)) *changed |= TVIF_CHILDREN;
    wineItem->cChildren = tvItem->cChildren;

    if (wineItem->cChildren == I_CHILDRENCALLBACK)
      callbackSet |= TVIF_CHILDREN;
    else
      callbackClear |= TVIF_CHILDREN;
  }

  if (tvItem->mask & TVIF_IMAGE)
  {
    if (changed && (wineItem->iImage != tvItem->iImage)) *changed |= TVIF_IMAGE;
    wineItem->iImage = tvItem->iImage;

    if (wineItem->iImage == I_IMAGECALLBACK)
      callbackSet |= TVIF_IMAGE;
    else
      callbackClear |= TVIF_IMAGE;
  }

  if (tvItem->mask & TVIF_SELECTEDIMAGE)
  {
    if (changed && (wineItem->iSelectedImage != tvItem->iSelectedImage)) *changed |= TVIF_SELECTEDIMAGE;
    wineItem->iSelectedImage = tvItem->iSelectedImage;

    if (wineItem->iSelectedImage == I_IMAGECALLBACK)
      callbackSet |= TVIF_SELECTEDIMAGE;
    else
      callbackClear |= TVIF_SELECTEDIMAGE;
  }

  if (tvItem->mask & TVIF_PARAM)
  {
    if (changed && (wineItem->lParam != tvItem->lParam)) *changed |= TVIF_PARAM;
    wineItem->lParam = tvItem->lParam;
  }

  /* If the application sets TVIF_INTEGRAL without
   * supplying a TVITEMEX structure, it's toast. */
  if (tvItem->mask & TVIF_INTEGRAL)
  {
    if (changed && (wineItem->iIntegral != tvItem->iIntegral)) *changed |= TVIF_INTEGRAL;
    wineItem->iIntegral = tvItem->iIntegral;
  }

  if (tvItem->mask & TVIF_STATE)
  {
    DWORD oldState = wineItem->state;

    wineItem->state &= ~tvItem->stateMask;
    wineItem->state |= (tvItem->state & tvItem->stateMask);
    if (changed && (wineItem->state != oldState)) *changed |= TVIF_STATE;
  }

  wineItem->callbackMask |= callbackSet;
  wineItem->callbackMask &= ~callbackClear;

  return TRUE;
}

/* Note that the new item is pre-zeroed. */
static LRESULT
TREEVIEW_InsertItem(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
  TVINSERTSTRUCTW *ptdi = (LPTVINSERTSTRUCTW)lParam;
  TVITEMEXW *tvItem;
  HTREEITEM insertAfter;
  INT x;
  TREEVIEW_ITEM *newItem, *parentItem;
  BOOL bTextUpdated = FALSE;

  if (!ptdi) return NULL;
  tvItem = &ptdi->itemex;

  if (infoPtr->hwndEdit) SetFocus(infoPtr->hwnd);

  if ((ptdi->hParent == TVI_ROOT) || (ptdi->hParent == 0))
  {
    parentItem = infoPtr->root;
  } else
  {
    parentItem = ptdi->hParent;

    if (!TREEVIEW_ValidItem(infoPtr,parentItem))
    {
      //dprintf(("invalid parent %p\n",parentItem));

      return (LRESULT)(HTREEITEM)NULL;
    }
  }

  insertAfter = ptdi->hInsertAfter;

  /* Validate this now for convenience. */
  if ((insertAfter != TVI_FIRST) && (insertAfter != TVI_LAST) && (insertAfter != TVI_SORT))
  {
    if (!TREEVIEW_ValidItem(infoPtr,insertAfter))
    {
      //dprintf(("invalid insert after %p\n",insertAfter));
      insertAfter = TVI_LAST;
    } else if (parentItem != insertAfter->parent)
    {
      //dprintf(("invalid insert after parent %p %p %p",insertAfter,insertAfter->parent,parentItem));
      //CB: Verified with AMD cpuid 2.04 (there are other strange things...)
      insertAfter = parentItem->lastChild;
    }
  }

  //TRACE("parent %p position %p: %s\n", parentItem, insertAfter,
  //      (tvItem->mask & TVIF_TEXT)
  //      ? ((tvItem->pszText == LPSTR_TEXTCALLBACKA) ? "<callback>"
  //      : tvItem->pszText)
  //      : "<no label>");

  newItem = TREEVIEW_AllocateItem(infoPtr);
  if (newItem == NULL)
    return (LRESULT)(HTREEITEM)NULL;

  newItem->parent = parentItem;
  newItem->iIntegral = 1;

  if (!TREEVIEW_DoSetItem(infoPtr,newItem,tvItem,unicode))
    return (LRESULT)(HTREEITEM)NULL;

  /* After this point, nothing can fail. (Except for TVI_SORT.) */

  infoPtr->uNumItems++;

  if (insertAfter == TVI_FIRST)
  {
    TREEVIEW_InsertBefore(newItem, parentItem->firstChild, parentItem);
  } else if (insertAfter == TVI_LAST)
  {
    TREEVIEW_InsertAfter(newItem, parentItem->lastChild, parentItem);
  } else if (insertAfter == TVI_SORT)
  {
    TREEVIEW_ITEM *aChild;
    TREEVIEW_ITEM *previousChild = NULL;
    BOOL bItemInserted = FALSE;

    aChild = parentItem->firstChild;

    bTextUpdated = TRUE;
    TREEVIEW_UpdateDispInfo(infoPtr, newItem, TVIF_TEXT);

    /* Iterate the parent children to see where we fit in */
    while (aChild)
    {
      INT comp;

      TREEVIEW_UpdateDispInfo(infoPtr,aChild,TVIF_TEXT);
      comp = lstrcmpW(newItem->pszText,aChild->pszText);

      if (comp < 0)     /* we are smaller than the current one */
      {
        TREEVIEW_InsertBefore(newItem, aChild, parentItem);
        bItemInserted = TRUE;
        break;
      }
      else if (comp > 0)        /* we are bigger than the current one */
      {
        previousChild = aChild;

         /* This will help us to exit if there is no more sibling */
        aChild = (aChild->nextSibling == 0)
                  ? NULL
                  : aChild->nextSibling;

        /* Look at the next item */
        continue;
      }
      else if (comp == 0)
      {
        /*
         * An item with this name is already existing, therefore,
         * we add after the one we found
         */
        TREEVIEW_InsertAfter(newItem, aChild, parentItem);
        bItemInserted = TRUE;
        break;
      }
    }

    /*
     * we reach the end of the child list and the item as not
     * yet been inserted, therefore, insert it after the last child.
     */
    if ((!bItemInserted) && (aChild == NULL))
      TREEVIEW_InsertAfter(newItem, previousChild, parentItem);
  } else
  {
    /* hInsertAfter names a specific item we want to insert after */
    TREEVIEW_InsertAfter(newItem,insertAfter,parentItem);
  }

  //TRACE("new item %p; parent %p, mask %x\n", newItem,
  // newItem->parent, tvItem->mask);

  newItem->iLevel = newItem->parent->iLevel + 1;

  if (newItem->parent->cChildren == 0)
    newItem->parent->cChildren = 1;

  if (infoPtr->dwStyle & TVS_CHECKBOXES)
  {
    if (STATEIMAGEINDEX(newItem->state) == 0)
      newItem->state |= INDEXTOSTATEIMAGEMASK(1);
  }

  TREEVIEW_VerifyTree(infoPtr);

  newItem->calculated = FALSE;
  TREEVIEW_QueueRefresh(infoPtr);

  return (LRESULT)newItem;
}

/* Item Deletion ************************************************************/
static void
TREEVIEW_RemoveItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem);

static void
TREEVIEW_RemoveAllChildren(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *parentItem)
{
    TREEVIEW_ITEM *kill = parentItem->firstChild;

    while (kill != NULL)
    {
        TREEVIEW_ITEM *next = kill->nextSibling;

        TREEVIEW_RemoveItem(infoPtr, kill);

        kill = next;
    }

    assert(parentItem->cChildren <= 0); /* I_CHILDRENCALLBACK or 0 */
    assert(parentItem->firstChild == NULL);
    assert(parentItem->lastChild == NULL);
}

static void
TREEVIEW_UnlinkItem(TREEVIEW_ITEM *item)
{
    TREEVIEW_ITEM *parentItem = item->parent;

    assert(item != NULL);
    assert(item->parent != NULL); /* i.e. it must not be the root */

    if (parentItem->firstChild == item)
        parentItem->firstChild = item->nextSibling;

    if (parentItem->lastChild == item)
        parentItem->lastChild = item->prevSibling;

    if (parentItem->firstChild == NULL && parentItem->lastChild == NULL
        && parentItem->cChildren > 0)
        parentItem->cChildren = 0;

    if (item->prevSibling)
        item->prevSibling->nextSibling = item->nextSibling;

    if (item->nextSibling)
        item->nextSibling->prevSibling = item->prevSibling;
}

static void
TREEVIEW_RemoveItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem)
{
    //TRACE("%p, (%s)\n", wineItem, TREEVIEW_ItemName(wineItem));

    TREEVIEW_SendTreeviewNotify(infoPtr,isUnicodeNotify(&infoPtr->header) ? TVN_DELETEITEMW:TVN_DELETEITEMA,TVIF_HANDLE | TVIF_PARAM,0,wineItem,0);

    if (wineItem->firstChild)
        TREEVIEW_RemoveAllChildren(infoPtr, wineItem);

    /* Selection is handled in TREEVIEW_DeleteItem. */

    if (infoPtr->insertMarkItem == wineItem)
        infoPtr->insertMarkItem = NULL;

    /* XXX focusItem, hotItem, dropItem */

    TREEVIEW_UnlinkItem(wineItem);

    infoPtr->uNumItems--;

    if (wineItem->pszText != LPSTR_TEXTCALLBACKW)
        COMCTL32_Free(wineItem->pszText);

    TREEVIEW_FreeItem(infoPtr, wineItem);
}


/* Empty out the tree. */
static void
TREEVIEW_RemoveTree(TREEVIEW_INFO *infoPtr)
{
    TREEVIEW_RemoveAllChildren(infoPtr, infoPtr->root);

    assert(infoPtr->uNumItems == 0);    /* root isn't counted in uNumItems */
}

static LRESULT
TREEVIEW_DeleteItem(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_ITEM *oldSelection = infoPtr->selectedItem;
    TREEVIEW_ITEM *newSelection = NULL;
    TREEVIEW_ITEM *newFirstVisible = NULL;
    TREEVIEW_ITEM *parent, *prev = NULL;
    BOOL visible = FALSE;

    if (lParam == (LPARAM)TVI_ROOT)
    {
        parent = infoPtr->root;
        newSelection = NULL;
        visible = TRUE;
        TREEVIEW_RemoveTree(infoPtr);
    }
    else
    {
        TREEVIEW_ITEM *wineItem = (TREEVIEW_ITEM *)lParam;

        if (!TREEVIEW_ValidItem(infoPtr, wineItem))
            return FALSE;

        //TRACE("%p (%s)\n", wineItem, TREEVIEW_ItemName(wineItem));
        parent = wineItem->parent;

        if (ISVISIBLE(wineItem))
        {
            prev = TREEVIEW_GetPrevListItem(infoPtr, wineItem);
            visible = TRUE;
        }

        if (infoPtr->selectedItem != NULL
            && (wineItem == infoPtr->selectedItem
                || TREEVIEW_IsChildOf(wineItem, infoPtr->selectedItem)))
        {
            if (wineItem->nextSibling)
                newSelection = wineItem->nextSibling;
            else if (wineItem->parent != infoPtr->root)
                newSelection = wineItem->parent;
        }

        if (infoPtr->firstVisible == wineItem)
        {
            if (wineItem->nextSibling)
               newFirstVisible = wineItem->nextSibling;
            else if (wineItem->prevSibling)
               newFirstVisible = wineItem->prevSibling;
            else if (wineItem->parent != infoPtr->root)
               newFirstVisible = wineItem->parent;
        }
        else
            newFirstVisible = infoPtr->firstVisible;

        TREEVIEW_RemoveItem(infoPtr, wineItem);
    }

    /* Don't change if somebody else already has. */
    if (oldSelection == infoPtr->selectedItem)
    {
#ifdef __WIN32OS2__
      //Crash in UltraEdit when opening file, because the old selected
      //item is already freed
      if (oldSelection == (TREEVIEW_ITEM *)lParam) {
          infoPtr->selectedItem = 0;
      }
#endif
      if (TREEVIEW_ValidItem(infoPtr, newSelection))
        TREEVIEW_DoSelectItem(infoPtr, TVGN_CARET, newSelection, TVC_UNKNOWN);
      else
        infoPtr->selectedItem = 0;
    }

    /* Validate insertMark dropItem.
     * hotItem ??? - used for comparision only.
     */
    if (!TREEVIEW_ValidItem(infoPtr, infoPtr->insertMarkItem))
        infoPtr->insertMarkItem = 0;

    if (!TREEVIEW_ValidItem(infoPtr, infoPtr->dropItem))
        infoPtr->dropItem = 0;

    if (!TREEVIEW_ValidItem(infoPtr, newFirstVisible))
        newFirstVisible = infoPtr->root->firstChild;

    TREEVIEW_VerifyTree(infoPtr);

    if (visible)
      TREEVIEW_QueueRefresh(infoPtr);
    else if (INCLIENT(parent) && !TREEVIEW_HasChildren(infoPtr,parent))
      TREEVIEW_RefreshItem(infoPtr,parent,TVIF_CHILDREN);

    return TRUE;
}

/* Get/Set Messages *********************************************************/

static LRESULT
TREEVIEW_GetIndent(TREEVIEW_INFO *infoPtr)
{
  return infoPtr->uIndent;
}

static LRESULT
TREEVIEW_SetIndent(TREEVIEW_INFO *infoPtr, WPARAM wParam)
{
  INT newIndent;

  newIndent = (INT)wParam;
  if (newIndent < MINIMUM_INDENT)
    newIndent = MINIMUM_INDENT;

  if (infoPtr->uIndent != newIndent)
  {
    infoPtr->uIndent = newIndent;

    infoPtr->uInternalStatus |= TV_CALCALL;
    TREEVIEW_QueueRefresh(infoPtr);
  }

  return 0;
}

static LRESULT
TREEVIEW_GetToolTips(TREEVIEW_INFO *infoPtr)
{
    return infoPtr->hwndToolTip;
}

static LRESULT
TREEVIEW_SetToolTips(TREEVIEW_INFO *infoPtr, WPARAM wParam)
{
    HWND prevToolTip;

    prevToolTip = infoPtr->hwndToolTip;
    infoPtr->hwndToolTip = (HWND)wParam;

    return prevToolTip;
}

static LRESULT
TREEVIEW_GetScrollTime(TREEVIEW_INFO *infoPtr)
{
    return infoPtr->uScrollTime;
}

static LRESULT
TREEVIEW_SetScrollTime(TREEVIEW_INFO *infoPtr, UINT uScrollTime)
{
    UINT uOldScrollTime = infoPtr->uScrollTime;

    infoPtr->uScrollTime = min(uScrollTime, 100);

    return uOldScrollTime;
}


static LRESULT
TREEVIEW_GetImageList(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case (WPARAM)TVSIL_NORMAL:
        return (LRESULT)infoPtr->himlNormal;

    case (WPARAM)TVSIL_STATE:
        return (LRESULT)infoPtr->himlState;

    default:
        return 0;
    }
}

static LRESULT
TREEVIEW_SetImageList(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    HIMAGELIST himlNew = (HIMAGELIST)lParam;
    HIMAGELIST himlOld = 0;
    int oldWidth  = infoPtr->normalImageWidth;
    int oldHeight = infoPtr->normalImageHeight;

    switch (wParam)
    {
      case (WPARAM)TVSIL_NORMAL:
        himlOld = infoPtr->himlNormal;
        infoPtr->himlNormal = himlNew;

        if (himlNew != NULL)
            ImageList_GetIconSize(himlNew, &infoPtr->normalImageWidth,
                                  &infoPtr->normalImageHeight);
        else
        {
            infoPtr->normalImageWidth = 0;
            infoPtr->normalImageHeight = 0;
        }

        break;

      case (WPARAM)TVSIL_STATE:
        himlOld = infoPtr->himlState;
        infoPtr->himlState = himlNew;

        if (himlNew != NULL)
            ImageList_GetIconSize(himlNew, &infoPtr->stateImageWidth,
                                  &infoPtr->stateImageHeight);
        else
        {
            infoPtr->stateImageWidth = 0;
            infoPtr->stateImageHeight = 0;
        }

        break;
    }

    if ((oldWidth != infoPtr->normalImageWidth) || (oldHeight != infoPtr->normalImageHeight))
      infoPtr->uInternalStatus |= TV_CALCALL;
    TREEVIEW_QueueRefresh(infoPtr);

    return (LRESULT)himlOld;
}

/* Compute the natural height (based on the font size) for items. */
static UINT
TREEVIEW_NaturalHeight(TREEVIEW_INFO *infoPtr)
{
  TEXTMETRICA tm;
  HDC hdc = GetDC(infoPtr->hwnd);
  HFONT hOldFont = SelectObject(hdc, infoPtr->hFont);
  INT height;

  GetTextMetricsA(hdc, &tm);

  SelectObject(hdc, hOldFont);
  ReleaseDC(infoPtr->hwnd, hdc);

  /* The 16 is a hack because our fonts are tiny. */
  height = MAX(16, tm.tmHeight + tm.tmExternalLeading);
  if (!(infoPtr->dwStyle & TVS_NONEVENHEIGHT))
    if (height & 0x1) height++; //important for PS_DOT pen!

  return height;
}

static LRESULT
TREEVIEW_SetItemHeight(TREEVIEW_INFO *infoPtr, WPARAM wParam)
{
    INT prevHeight = infoPtr->uItemHeight;

    if (wParam == (WPARAM)(SHORT)-1)
    {
        infoPtr->uItemHeight = TREEVIEW_NaturalHeight(infoPtr);
        infoPtr->bHeightSet = FALSE;
    }
    else
    {
        infoPtr->uItemHeight = wParam;
        infoPtr->bHeightSet = TRUE;
    }

    if (!(infoPtr->dwStyle & TVS_NONEVENHEIGHT))
      if (infoPtr->uItemHeight & 0x1) infoPtr->uItemHeight++;

    if (infoPtr->uItemHeight != prevHeight)
    {
        infoPtr->uInternalStatus |= TV_CALCALL;
        TREEVIEW_QueueRefresh(infoPtr);
    }

    return prevHeight;
}

static LRESULT
TREEVIEW_GetItemHeight(TREEVIEW_INFO *infoPtr)
{
    return infoPtr->uItemHeight;
}


static LRESULT
TREEVIEW_GetFont(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    return infoPtr->hFont;
}

static LRESULT
TREEVIEW_SetFont(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  UINT uHeight = infoPtr->uItemHeight;

  infoPtr->hFont = wParam ? (HFONT)wParam : GetStockObject(SYSTEM_FONT);

  DeleteObject(infoPtr->hBoldFont);
  infoPtr->hBoldFont = TREEVIEW_CreateBoldFont(infoPtr->hFont);

  if (!infoPtr->bHeightSet)
    infoPtr->uItemHeight = TREEVIEW_NaturalHeight(infoPtr);

  SendMessageA(infoPtr->hwndToolTip,WM_SETFONT,infoPtr->hFont,1);

  if (uHeight != infoPtr->uItemHeight)
  {
    infoPtr->uInternalStatus |= TV_CALCALL;
    TREEVIEW_CalcItems(infoPtr);
  }

  if (lParam)
  {
    TREEVIEW_UnqueueRefresh(infoPtr,FALSE,FALSE);
    TREEVIEW_Refresh(infoPtr);
  }

  return 0;
}


static LRESULT
TREEVIEW_GetLineColor(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)infoPtr->clrLine;
}

static LRESULT
TREEVIEW_SetLineColor(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    COLORREF prevColor = infoPtr->clrLine;

    infoPtr->clrLine = (COLORREF)lParam;

    return (LRESULT)prevColor;
}


static LRESULT
TREEVIEW_GetTextColor(TREEVIEW_INFO *infoPtr)
{
    return (LRESULT)infoPtr->clrText;
}

static LRESULT
TREEVIEW_SetTextColor(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    COLORREF prevColor = infoPtr->clrText;

    infoPtr->clrText = (COLORREF)lParam;

    if (infoPtr->clrText != prevColor)
        TREEVIEW_QueueRefresh(infoPtr);

    return (LRESULT)prevColor;
}


static LRESULT
TREEVIEW_GetBkColor(TREEVIEW_INFO *infoPtr)
{
    return (LRESULT)infoPtr->clrBk;
}

static LRESULT
TREEVIEW_SetBkColor(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    COLORREF prevColor = infoPtr->clrBk;

    infoPtr->clrBk = (COLORREF)lParam;

    if (infoPtr->clrBk != prevColor)
        TREEVIEW_QueueRefresh(infoPtr);

    return (LRESULT)prevColor;
}


static LRESULT
TREEVIEW_GetInsertMarkColor(TREEVIEW_INFO *infoPtr, WPARAM wParam,
                            LPARAM lParam)
{
    return (LRESULT)infoPtr->clrInsertMark;
}

static LRESULT
TREEVIEW_SetInsertMarkColor(TREEVIEW_INFO *infoPtr, WPARAM wParam,
                            LPARAM lParam)
{
    COLORREF prevColor = infoPtr->clrInsertMark;

    infoPtr->clrInsertMark = (COLORREF)lParam;

    return (LRESULT)prevColor;
}


static LRESULT
TREEVIEW_SetInsertMark(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    if (!TREEVIEW_ValidItem(infoPtr, (HTREEITEM)lParam))
        return 0;

    infoPtr->insertBeforeorAfter = (BOOL)wParam;
    infoPtr->insertMarkItem = (HTREEITEM)lParam;

    TREEVIEW_QueueRefresh(infoPtr);

    return 1;
}

/************************************************************************
 * Some serious braindamage here. lParam is a pointer to both the
 * input HTREEITEM and the output RECT.
 */
static LRESULT
TREEVIEW_GetItemRect(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_ITEM *wineItem;
    const HTREEITEM *pItem = (HTREEITEM*)lParam;
    LPRECT lpRect = (LPRECT)lParam;

    /*
     * validate parameters
     */
    if (pItem == NULL)
        return FALSE;

    TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE);

    wineItem = *pItem;
    if (!TREEVIEW_ValidItem(infoPtr, *pItem) || !wineItem->displayed)
        return FALSE;

    if (!wineItem->calculated)
      TREEVIEW_CalcItem(infoPtr,wineItem,(HDC)0);

    /*
     * If wParam is TRUE return the text size otherwise return
     * the whole item size
     */
    if (wParam)
    {
        /* Windows does not send TVN_GETDISPINFO here. */

        lpRect->top = wineItem->rect.top;
        lpRect->bottom = wineItem->rect.bottom;

        lpRect->left = wineItem->textOffset;
        lpRect->right = wineItem->textOffset+wineItem->textWidth+4;
    }
    else
    {
        *lpRect = wineItem->rect;
    }

    return TRUE;
}

static inline LRESULT
TREEVIEW_GetVisibleCount(TREEVIEW_INFO *infoPtr)
{
    /* Suprise! This does not take integral height into account. */
    return infoPtr->clientHeight / infoPtr->uItemHeight;
}

static LRESULT
TREEVIEW_GetItem(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam,BOOL unicode)
{
    LPTVITEMEXW tvItem;
    TREEVIEW_ITEM *wineItem;

    tvItem = (LPTVITEMEXW)lParam;

    wineItem = tvItem->hItem;

    if (!TREEVIEW_ValidItem(infoPtr, wineItem))
        return FALSE;

    TREEVIEW_UpdateDispInfo(infoPtr, wineItem, tvItem->mask);

    if (tvItem->mask & TVIF_CHILDREN)
        tvItem->cChildren = wineItem->cChildren;

    if (tvItem->mask & TVIF_HANDLE)
        tvItem->hItem = wineItem;

    if (tvItem->mask & TVIF_IMAGE)
        tvItem->iImage = wineItem->iImage;

    if (tvItem->mask & TVIF_INTEGRAL)
        tvItem->iIntegral = wineItem->iIntegral;

    /* undocumented: windows ignores TVIF_PARAM and
     * * always sets lParam
     */
    tvItem->lParam = wineItem->lParam;

    if (tvItem->mask & TVIF_SELECTEDIMAGE)
        tvItem->iSelectedImage = wineItem->iSelectedImage;

    if (tvItem->mask & TVIF_STATE)
        tvItem->state = wineItem->state & tvItem->stateMask;

    if (tvItem->mask & TVIF_TEXT)
    {
      if (unicode)
        lstrcpynW(tvItem->pszText,wineItem->pszText,tvItem->cchTextMax);
      else
        lstrcpynWtoA((CHAR*)tvItem->pszText,wineItem->pszText,tvItem->cchTextMax);
    }

    return TRUE;
}

/* Beware MSDN Library Visual Studio 6.0. It says -1 on failure, 0 on success,
 * which is wrong. */
static LRESULT
TREEVIEW_SetItem(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam,BOOL unicode)
{
    TREEVIEW_ITEM *wineItem;
    TVITEMEXW *tvItem;
    DWORD changed = 0;

    if (infoPtr->hwndEdit) SetFocus(infoPtr->hwnd);

    tvItem = (LPTVITEMEXW)lParam;
    wineItem = tvItem->hItem;

    if (!TREEVIEW_ValidItem(infoPtr, wineItem))
        return FALSE;

    if (!TREEVIEW_DoSetItem(infoPtr,wineItem,tvItem,unicode,&changed))
        return FALSE;

    /* If the text or TVIS_BOLD was changed, and it is displayed, recalculate. */
    if (((changed & TVIF_TEXT) || ((tvItem->mask & TVIF_STATE) && (tvItem->stateMask & TVIS_BOLD))) && wineItem->displayed)
    {
      TREEVIEW_ComputeTextWidth(infoPtr,wineItem,0,FALSE);
    }

    if (changed & (TVIF_INTEGRAL | TVIF_CHILDREN))
      TREEVIEW_QueueRefresh(infoPtr);
    else if (changed)
      TREEVIEW_RefreshItem(infoPtr,wineItem,changed);

    return TRUE;
}

static LRESULT
TREEVIEW_GetItemState(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_ITEM *wineItem = (HTREEITEM)wParam;

    if (!wineItem || !TREEVIEW_ValidItem(infoPtr, wineItem))
        return 0;

    return (wineItem->state & lParam);
}

static LRESULT
TREEVIEW_GetNextItem(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_ITEM *wineItem;
    TREEVIEW_ITEM *retval;
    WPARAM which;

    which = (INT)wParam;
    wineItem = (TREEVIEW_ITEM *)lParam;
    retval = 0;

    /* handle all the global data here */
    switch (which)
    {
      case TVGN_CHILD:            /* Special case: child of 0 is root */
        if (wineItem)
            break;
        /* fall through */
      case TVGN_ROOT:
        retval = infoPtr->root->firstChild;
        break;

      case TVGN_CARET:
        retval = infoPtr->selectedItem;
        break;

      case TVGN_FIRSTVISIBLE:
        TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE);
        retval = infoPtr->firstVisible;
        break;

      case TVGN_DROPHILITE:
        retval = infoPtr->dropItem;
        break;

      case TVGN_LASTVISIBLE:
        retval = TREEVIEW_GetLastListItem(infoPtr,infoPtr->root);
        break;
    }

    if (retval)
    {
        //TRACE("flags:%x, returns %p\n", which, retval);
        return (LRESULT)retval;
    }

    if (!TREEVIEW_ValidItem(infoPtr, wineItem))
        return FALSE;

    switch (which)
    {
      case TVGN_NEXT:
        retval = wineItem->nextSibling;
        break;

      case TVGN_PREVIOUS:
        retval = wineItem->prevSibling;
        break;

      case TVGN_PARENT:
        retval = (wineItem->parent != infoPtr->root) ? wineItem->parent : NULL;
        break;

      case TVGN_CHILD:
        retval = wineItem->firstChild;
        break;

      case TVGN_NEXTVISIBLE:
        retval = TREEVIEW_GetNextListItem(infoPtr, wineItem);
        break;

      case TVGN_PREVIOUSVISIBLE:
        retval = TREEVIEW_GetPrevListItem(infoPtr, wineItem);
        break;

      default:
        //TRACE("Unknown msg %x,item %p\n", which, wineItem);
        break;
    }

    //TRACE("flags:%x, item %p;returns %p\n", which, wineItem, retval);
    return (LRESULT)retval;
}


static LRESULT
TREEVIEW_GetCount(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)infoPtr->uNumItems;
}

static VOID
TREEVIEW_ToggleItemState(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    if (infoPtr->dwStyle & TVS_CHECKBOXES)
    {
        static const unsigned int state_table[] = { 0, 2, 1 };

        unsigned int state;

        state = STATEIMAGEINDEX(item->state);
        //TRACE("state:%x\n", state);
        item->state &= ~TVIS_STATEIMAGEMASK;

        if (state < 3)
            state = state_table[state];

        item->state |= INDEXTOSTATEIMAGEMASK(state);

        //TRACE("state:%x\n", state);
        TREEVIEW_RefreshItem(infoPtr,item,TVIF_IMAGE);
    }
}


/* Painting *************************************************************/

/* Draw the lines and expand button for an item. Also draws one section
 * of the line from item's parent to item's parent's next sibling. */
static void
TREEVIEW_DrawItemLines(TREEVIEW_INFO *infoPtr, HDC hdc, TREEVIEW_ITEM *item)
{
    LONG centerx, centery;
    BOOL lar = ((infoPtr->dwStyle
                 & (TVS_LINESATROOT|TVS_HASLINES|TVS_HASBUTTONS))
                > TVS_LINESATROOT);

    if (!lar && item->iLevel == 0)
        return;

    centerx = (item->linesOffset + item->stateOffset) / 2;
    centery = (item->rect.top + item->rect.bottom) / 2;

    if (infoPtr->dwStyle & TVS_HASLINES)
    {
        HPEN hOldPen, hNewPen;
        HTREEITEM parent;

        /*
         * Get a dotted grey pen
         */
        hNewPen = CreatePen(PS_DOT, 0, infoPtr->clrLine);
        hOldPen = SelectObject(hdc, hNewPen);

        MoveToEx(hdc, item->stateOffset, centery, NULL);
        LineTo(hdc, centerx - 1, centery);

        if (item->prevSibling || item->parent != infoPtr->root)
        {
            MoveToEx(hdc, centerx, item->rect.top, NULL);
            LineTo(hdc, centerx, centery);
        }

        if (item->nextSibling)
        {
            MoveToEx(hdc, centerx, centery, NULL);
            LineTo(hdc, centerx, item->rect.bottom + 1);
        }

        /* Draw the line from our parent to its next sibling. */
        parent = item->parent;
        while (parent != infoPtr->root)
        {
            int pcenterx = (parent->linesOffset + parent->stateOffset) / 2;

            if (parent->nextSibling
                /* skip top-levels unless TVS_LINESATROOT */
                && parent->stateOffset > parent->linesOffset)
            {
                MoveToEx(hdc, pcenterx, item->rect.top, NULL);
                LineTo(hdc, pcenterx, item->rect.bottom + 1);
            }

            parent = parent->parent;
        }

        SelectObject(hdc, hOldPen);
        DeleteObject(hNewPen);
    }

    /*
     * Display the (+/-) signs
     */

    if (infoPtr->dwStyle & TVS_HASBUTTONS)
    {
        if (item->cChildren)
        {
            LONG height = item->rect.bottom - item->rect.top;
            LONG width  = item->stateOffset - item->linesOffset;
            LONG rectsize = MIN(height, width) / 4;
            /* plussize = ceil(rectsize * 3/4) */
            LONG plussize = (rectsize + 1) * 3 / 4;

            HPEN hNewPen  = CreatePen(PS_SOLID, 0, infoPtr->clrLine);
            HPEN hOldPen  = SelectObject(hdc, hNewPen);
            HBRUSH hbr    = CreateSolidBrush(infoPtr->clrBk);
            HBRUSH hbrOld = SelectObject(hdc, hbr);

            Rectangle(hdc, centerx - rectsize, centery - rectsize,
                      centerx + rectsize + 1, centery + rectsize + 1);

            SelectObject(hdc, hbrOld);
            DeleteObject(hbr);

            SelectObject(hdc, hOldPen);
            DeleteObject(hNewPen);

            MoveToEx(hdc, centerx - plussize + 1, centery, NULL);
            LineTo(hdc, centerx + plussize, centery);

            if (!(item->state & TVIS_EXPANDED))
            {
                MoveToEx(hdc, centerx, centery - plussize + 1, NULL);
                LineTo(hdc, centerx, centery + plussize);
            }
        }
    }
}

static VOID TREEVIEW_DrawHottrackLine(HDC hdc,TREEVIEW_ITEM *item)
{
  HPEN hPen,hOldPen;
  INT rop;

  if (!item->inclient) return;

  rop = SetROP2(hdc,R2_XORPEN);
  hPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
  hOldPen = SelectObject(hdc,hPen);

  MoveToEx(hdc,item->textOffset,item->rect.bottom-1,NULL);
  LineTo(hdc,item->textOffset+item->textWidth,item->rect.bottom-1);

  DeleteObject(hPen);
  SelectObject(hdc,hOldPen);
  SetROP2(hdc,rop);
}

static void
TREEVIEW_DrawItem(TREEVIEW_INFO *infoPtr, HDC hdc, TREEVIEW_ITEM *wineItem)
{
    INT cditem;
    HFONT hOldFont;
    int centery;

    if (!wineItem->calculated) TREEVIEW_CalcItem(infoPtr,wineItem,hdc);

    hOldFont = SelectObject(hdc, TREEVIEW_FontForItem(infoPtr, wineItem));

    TREEVIEW_UpdateDispInfo(infoPtr, wineItem, CALLBACK_MASK_ALL);

    /* The custom draw handler can query the text rectangle,
     * so get ready. */
    TREEVIEW_ComputeTextWidth(infoPtr,wineItem,hdc,TRUE);

    cditem = 0;

    if (infoPtr->cdmode & CDRF_NOTIFYITEMDRAW)
    {
        cditem = TREEVIEW_SendCustomDrawItemNotify(infoPtr, hdc, wineItem, CDDS_ITEMPREPAINT);

        if (cditem & CDRF_SKIPDEFAULT)
        {
            SelectObject(hdc, hOldFont);
            return;
        }
    }

    if (cditem & CDRF_NEWFONT)
        TREEVIEW_ComputeTextWidth(infoPtr,wineItem,hdc,TRUE);

    TREEVIEW_DrawItemLines(infoPtr, hdc, wineItem);

    centery = (wineItem->rect.top + wineItem->rect.bottom) / 2;

    /*
     * Display the images associated with this item
     */
    {
        INT imageIndex;

        /* State images are displayed to the left of the Normal image
         * image number is in state; zero should be `display no image'.
         */
        imageIndex = STATEIMAGEINDEX(wineItem->state);

        if (infoPtr->himlState && imageIndex)
        {
            ImageList_Draw(infoPtr->himlState, imageIndex, hdc,
                           wineItem->stateOffset,
                           centery - infoPtr->stateImageHeight / 2,
                           ILD_NORMAL);
        }

        /* Now, draw the normal image; can be either selected or
         * non-selected image.
         */

        if ((wineItem->state & TVIS_SELECTED) && wineItem->iSelectedImage)
        {
            /* The item is curently selected */
            imageIndex = wineItem->iSelectedImage;
        }
        else
        {
            /* The item is not selected */
            imageIndex = wineItem->iImage;
        }

        if (infoPtr->himlNormal && (imageIndex != I_IMAGENONE))
        {
            int ovlIdx = wineItem->state & TVIS_OVERLAYMASK;

            ImageList_Draw(infoPtr->himlNormal, imageIndex, hdc,
                           wineItem->imageOffset,
                           centery - infoPtr->normalImageHeight / 2,
                           ILD_NORMAL | ovlIdx);
        }
    }


    /*
     * Display the text associated with this item
     */

    /* Don't paint item's text if it's being edited */
    if (!infoPtr->hwndEdit)
    {
        if (wineItem->pszText && (wineItem->pszText[0] != 0))
        {
            COLORREF oldTextColor = 0;
            INT oldBkMode;
            HBRUSH hbrBk = 0;
            BOOL inFocus = (GetFocus() == infoPtr->hwnd);
            RECT rcText;

            oldBkMode = SetBkMode(hdc, TRANSPARENT);

            /* - If item is drop target or it is selected and window is in focus -
             * use blue background (COLOR_HIGHLIGHT).
             * - If item is selected, window is not in focus, but it has style
             * TVS_SHOWSELALWAYS - use grey background (COLOR_BTNFACE)
             * - Otherwise - don't fill background
             */
            if ((wineItem->state & TVIS_DROPHILITED) ||
                ((wineItem->state & TVIS_SELECTED) &&
                 (inFocus || (infoPtr->dwStyle & TVS_SHOWSELALWAYS))))
            {
                if ((wineItem->state & TVIS_DROPHILITED) || inFocus)
                {
                    hbrBk = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
                    oldTextColor =
                        SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
                }
                else
                {
                    hbrBk = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

                    if (infoPtr->clrText == -1)
                        oldTextColor =
                            SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
                    else
                        oldTextColor = SetTextColor(hdc, infoPtr->clrText);
                }
            }
            else
            {
                if (infoPtr->clrText == -1)
                    oldTextColor =
                        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
                else
                    oldTextColor = SetTextColor(hdc, infoPtr->clrText);
            }

            rcText.top = wineItem->rect.top;
            rcText.bottom = wineItem->rect.bottom;
            rcText.left = wineItem->textOffset;
            rcText.right = rcText.left + wineItem->textWidth + 4;

            if (hbrBk)
            {
                FillRect(hdc, &rcText, hbrBk);
                DeleteObject(hbrBk);
            }

            /* Draw the box arround the selected item */
            if ((wineItem == infoPtr->selectedItem) && inFocus)
            {
                HPEN hNewPen = CreatePen(PS_DOT, 0,
                                         GetSysColor(COLOR_WINDOWTEXT));
                HPEN hOldPen = SelectObject(hdc, hNewPen);
                INT rop = SetROP2(hdc, R2_XORPEN);
                POINT points[5];

                points[4].x = points[0].x = rcText.left;
                points[4].y = points[0].y = rcText.top;
                points[1].x = rcText.right - 1;
                points[1].y = rcText.top;
                points[2].x = rcText.right - 1;
                points[2].y = rcText.bottom - 1;
                points[3].x = rcText.left;
                points[3].y = rcText.bottom - 1;

                Polyline(hdc, points, 5);

                SetROP2(hdc, rop);
                SelectObject(hdc, hOldPen);
                DeleteObject(hNewPen);
            }

            rcText.left += 2;
            rcText.right -= 2;

            /* Draw it */
            DrawTextW(hdc,
                      wineItem->pszText,
                      lstrlenW(wineItem->pszText),
                      &rcText,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

            /* Restore the hdc state */
            SetTextColor(hdc, oldTextColor);

            if (oldBkMode != TRANSPARENT)
                SetBkMode(hdc, oldBkMode);
        }
    }

    /* Draw insertion mark if necessary */

    if (wineItem == infoPtr->insertMarkItem)
    {
        HPEN hNewPen, hOldPen;
        int offset;
        int left, right;

        hNewPen = CreatePen(PS_SOLID, 2, infoPtr->clrInsertMark);
        hOldPen = SelectObject(hdc, hNewPen);

        if (infoPtr->insertBeforeorAfter)
            offset = wineItem->rect.bottom - 1;
        else
            offset = wineItem->rect.top + 1;

        left = wineItem->textOffset - 2;
        right = wineItem->textOffset + wineItem->textWidth + 2;

        MoveToEx(hdc, left, offset - 3, NULL);
        LineTo(hdc, left, offset + 4);

        MoveToEx(hdc, left, offset, NULL);
        LineTo(hdc, right + 1, offset);

        MoveToEx(hdc, right, offset + 3, NULL);
        LineTo(hdc, right, offset - 4);

        SelectObject(hdc, hOldPen);
        DeleteObject(hNewPen);
    }

    //draw hot item if necessary
    if (wineItem == infoPtr->hotItem)
      TREEVIEW_DrawHottrackLine(hdc,wineItem);

    if (cditem & CDRF_NOTIFYPOSTPAINT)
    {
        cditem = TREEVIEW_SendCustomDrawItemNotify(infoPtr, hdc, wineItem, CDDS_ITEMPOSTPAINT);
    }

    SelectObject(hdc, hOldFont);
}

static void TREEVIEW_QueueRefresh(TREEVIEW_INFO *infoPtr)
{
  if (infoPtr->Timer & TV_REFRESH_TIMER_SET)
    KillTimer(infoPtr->hwnd,TV_REFRESH_TIMER);

  if (infoPtr->uInternalStatus & TV_NOREDRAW)
  {
    infoPtr->Timer &= ~TV_REFRESH_TIMER_SET;

    return;
  }

  SetTimer(infoPtr->hwnd,TV_REFRESH_TIMER,TV_REFRESH_DELAY,0);
  infoPtr->Timer |= TV_REFRESH_TIMER_SET;
}

static BOOL TREEVIEW_UnqueueRefresh(TREEVIEW_INFO *infoPtr,BOOL calc,BOOL refresh)
{
  if (infoPtr->Timer & TV_REFRESH_TIMER_SET)
  {
    KillTimer(infoPtr->hwnd,TV_REFRESH_TIMER);
    infoPtr->Timer &= ~TV_REFRESH_TIMER_SET;

    if (calc) TREEVIEW_CalcItems(infoPtr);
    if (refresh)
    {
      TREEVIEW_Refresh(infoPtr);
      UpdateWindow(infoPtr->hwnd);
    }

    return TRUE;
  }

  return FALSE;
}

static void
TREEVIEW_Draw(TREEVIEW_INFO *infoPtr,HDC hdc,RECT *updateRect)
{
    RECT rect;
    TREEVIEW_ITEM *item;
    INT iItem;

    TREEVIEW_UnqueueRefresh(infoPtr,TRUE,FALSE);

    GetClientRect(infoPtr->hwnd,&rect);
    if ((rect.left == rect.right) || (rect.top == rect.bottom)) return;

    infoPtr->cdmode = TREEVIEW_SendCustomDrawNotify(infoPtr,CDDS_PREPAINT,hdc,rect);

    if (infoPtr->cdmode == CDRF_SKIPDEFAULT) return;

    /* We iterate through all visible items in order */
    item = infoPtr->firstVisible;

    while ((item != NULL) && item->inclient)
    {
      RECT itemRect = item->rect;

      if (!item->calculated) TREEVIEW_CalcItem(infoPtr,item,hdc);
      if (updateRect && IntersectRect(NULL,&itemRect,updateRect))
        TREEVIEW_DrawItem(infoPtr,hdc,item);
      else
      {
        if (updateRect && (item->rect.top >= updateRect->bottom)) break;
        itemRect.left = 0;
        if (updateRect && IntersectRect(NULL,&itemRect,updateRect))
          TREEVIEW_DrawItemLines(infoPtr,hdc,item);
      }
      item = TREEVIEW_GetNextListItem(infoPtr,item);
    }

    if (infoPtr->cdmode & CDRF_NOTIFYPOSTPAINT)
        infoPtr->cdmode = TREEVIEW_SendCustomDrawNotify(infoPtr, CDDS_POSTPAINT, hdc, rect);
}

static void TREEVIEW_Refresh(TREEVIEW_INFO *infoPtr)
{
  TREEVIEW_UnqueueRefresh(infoPtr,TRUE,FALSE);

  InvalidateRect(infoPtr->hwnd,NULL,TRUE);
}

static void TREEVIEW_RefreshItem(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item,DWORD changed)
{
  if (item && item->inclient)
  {
    RECT rect = item->rect;

    //left
    if (changed & TVIF_STATE) rect.left = item->stateOffset;
    else if (changed & (TVIF_IMAGE | TVIF_SELECTEDIMAGE)) rect.left = item->imageOffset;
    else if (changed & TVIF_TEXT) rect.left = item->textOffset;

    //right
    if (changed & TVIF_TEXT);
    else if (changed & (TVIF_IMAGE | TVIF_SELECTEDIMAGE)) rect.right = item->imageOffset-infoPtr->normalImageWidth;
    else if (changed & TVIF_STATE) rect.right = item->stateOffset-infoPtr->stateImageWidth;

    InvalidateRect(infoPtr->hwnd,&rect,TRUE);
  }
}

static LRESULT
TREEVIEW_Paint(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  HDC hdc = (HDC)wParam;

  if (!hdc)
  {
    PAINTSTRUCT ps;

    hdc = BeginPaint(infoPtr->hwnd,&ps);
    TREEVIEW_Draw(infoPtr,hdc,&ps.rcPaint);
    EndPaint(infoPtr->hwnd,&ps);
  } else
      TREEVIEW_Draw(infoPtr,hdc,NULL);

  return 0;
}

/* Sorting **************************************************************/

/***************************************************************************
 * Forward the DPA local callback to the treeview owner callback
 */
static INT WINAPI
TREEVIEW_CallBackCompare(LPVOID pvFirst, LPVOID pvSecond, LPARAM callback)
{
    LPTVSORTCB pCallBackSort = (LPTVSORTCB)callback;

    TREEVIEW_ITEM *first = (TREEVIEW_ITEM *)pvFirst;
    TREEVIEW_ITEM *second = (TREEVIEW_ITEM *)pvSecond;

    /* Forward the call to the client-defined callback */
    return pCallBackSort->lpfnCompare(first->lParam,
                                      second->lParam,
                                      pCallBackSort->lParam);
}

/***************************************************************************
 * Treeview native sort routine: sort on item text.
 */
static INT WINAPI
TREEVIEW_SortOnName(LPVOID pvFirst, LPVOID pvSecond, LPARAM tvInfoPtr)
{
    TREEVIEW_INFO *infoPtr = (TREEVIEW_INFO *)tvInfoPtr;

    TREEVIEW_ITEM *first = (TREEVIEW_ITEM *)pvFirst;
    TREEVIEW_ITEM *second = (TREEVIEW_ITEM *)pvSecond;

    TREEVIEW_UpdateDispInfo(infoPtr, first, TVIF_TEXT);
    TREEVIEW_UpdateDispInfo(infoPtr, second, TVIF_TEXT);

    return lstrcmpiW(first->pszText, second->pszText);
}

/* Returns the number of physical children belonging to item. */
static INT
TREEVIEW_CountChildren(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    INT cChildren = 0;
    HTREEITEM hti;

    for (hti = item->firstChild; hti != NULL; hti = hti->nextSibling)
        cChildren++;

    return cChildren;
}

/* Returns a DPA containing a pointer to each physical child of item in
 * sibling order. If item has no children, an empty DPA is returned. */
static HDPA
TREEVIEW_BuildChildDPA(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    HTREEITEM child = item->firstChild;

    HDPA list = DPA_Create(8);
    if (list == 0) return NULL;

    for (child = item->firstChild; child != NULL; child = child->nextSibling)
    {
        if (DPA_InsertPtr(list, INT_MAX, child) == -1)
        {
            DPA_Destroy(list);
            return NULL;
        }
    }

    return list;
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

static LRESULT
TREEVIEW_Sort(TREEVIEW_INFO *infoPtr, BOOL fRecurse, HTREEITEM parent,
              LPTVSORTCB pSort)
{
    INT cChildren;
    PFNDPACOMPARE pfnCompare;
    LPARAM lpCompare;
    BOOL root = FALSE;

    /* undocumented feature: TVI_ROOT means `sort the whole tree' */
    if ((parent == TVI_ROOT) || (parent == 0))
    {
      root = TRUE;
      parent = infoPtr->root;
    }

    /* Check for a valid handle to the parent item */
    if (!TREEVIEW_ValidItem(infoPtr, parent))
    {
        //ERR("invalid item hParent=%x\n", (INT)parent);
        return FALSE;
    }

    if (pSort)
    {
        pfnCompare = (PFNDPACOMPARE)TREEVIEW_CallBackCompare;
        lpCompare = (LPARAM)pSort;
    }
    else
    {
        pfnCompare = (PFNDPACOMPARE)TREEVIEW_SortOnName;
        lpCompare = (LPARAM)infoPtr;
    }

    cChildren = TREEVIEW_CountChildren(infoPtr,parent);

    /* Make sure there is something to sort */
    if (cChildren > 1)
    {
        /* TREEVIEW_ITEM rechaining */
        INT count = 0;
        HTREEITEM item = 0;
        HTREEITEM nextItem = 0;
        HTREEITEM prevItem = 0;

        HDPA sortList = TREEVIEW_BuildChildDPA(infoPtr, parent);

        if (sortList == NULL)
            return FALSE;

        /* let DPA sort the list */
        DPA_Sort(sortList, pfnCompare, lpCompare);

        /* The order of DPA entries has been changed, so fixup the
         * nextSibling and prevSibling pointers. */

        item = (HTREEITEM)DPA_GetPtr(sortList, count++);
        while ((nextItem = (HTREEITEM)DPA_GetPtr(sortList, count++)) != NULL)
        {
            /* link the two current item toghether */
            item->nextSibling = nextItem;
            nextItem->prevSibling = item;

            if (prevItem == NULL)
            {
                /* this is the first item, update the parent */
                parent->firstChild = item;
                item->prevSibling = NULL;
            }
            else
            {
                /* fix the back chaining */
                item->prevSibling = prevItem;
            }

            /* get ready for the next one */
            prevItem = item;
            item = nextItem;
        }

        /* the last item is pointed to by item and never has a sibling */
        item->nextSibling = NULL;
        parent->lastChild = item;

        DPA_Destroy(sortList);

        TREEVIEW_VerifyTree(infoPtr);

        TREEVIEW_QueueRefresh(infoPtr);

        return TRUE;
    }
    return FALSE;
}

/***************************************************************************
 * Setup the treeview structure with regards of the sort method
 * and sort the children of the TV item specified in lParam
 */
static LRESULT
TREEVIEW_SortChildrenCB(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    LPTVSORTCB pSort = (LPTVSORTCB)lParam;

    return TREEVIEW_Sort(infoPtr, wParam, pSort->hParent, pSort);
}


/***************************************************************************
 * Sort the children of the TV item specified in lParam.
 */
static LRESULT
TREEVIEW_SortChildren(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    return TREEVIEW_Sort(infoPtr, (BOOL)wParam, (HTREEITEM)lParam, NULL);
}


/* Expansion/Collapse ***************************************************/

static BOOL
TREEVIEW_SendExpanding(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem,
                       UINT action)
{
    return !TREEVIEW_SendTreeviewNotify(infoPtr,isUnicodeNotify(&infoPtr->header) ? TVN_ITEMEXPANDINGW:TVN_ITEMEXPANDINGA, action,
                                        TVIF_HANDLE | TVIF_STATE | TVIF_PARAM
                                        | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
                                        0, wineItem);
}

static VOID
TREEVIEW_SendExpanded(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem,
                      UINT action)
{
    TREEVIEW_SendTreeviewNotify(infoPtr,isUnicodeNotify(&infoPtr->header) ? TVN_ITEMEXPANDEDW:TVN_ITEMEXPANDEDA, action,
                                TVIF_HANDLE | TVIF_STATE | TVIF_PARAM
                                | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
                                0, wineItem);
}


/* This corresponds to TVM_EXPAND with TVE_COLLAPSE.
 * bRemoveChildren corresponds to TVE_COLLAPSERESET. */
static BOOL
TREEVIEW_Collapse(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem,
                  BOOL bRemoveChildren, BOOL bUser)
{
    UINT action = TVE_COLLAPSE | (bRemoveChildren ? TVE_COLLAPSERESET : 0);
    BOOL bSetSelection, bSetFirstVisible;

    //TRACE("TVE_COLLAPSE %p %s\n", wineItem, TREEVIEW_ItemName(wineItem));

    if (!(wineItem->state & TVIS_EXPANDED) || wineItem->firstChild == NULL)
        return FALSE;

    if (bUser)
        TREEVIEW_SendExpanding(infoPtr, wineItem, action);

    wineItem->state &= ~TVIS_EXPANDED;

    if (bUser)
        TREEVIEW_SendExpanded(infoPtr, wineItem, action);

    bSetSelection = (infoPtr->selectedItem != NULL
                     && TREEVIEW_IsChildOf(wineItem, infoPtr->selectedItem));

    if (TREEVIEW_IsChildOf(wineItem,infoPtr->firstVisible))
    {
      infoPtr->lefttop.y += wineItem->rect.top;
    }

    if (bRemoveChildren)
    {
        //TRACE("TVE_COLLAPSERESET\n");
        wineItem->state &= ~TVIS_EXPANDEDONCE;
        TREEVIEW_RemoveAllChildren(infoPtr, wineItem);
    }

    //update window
    //CB: todo: optimize!
    TREEVIEW_UnqueueRefresh(infoPtr,FALSE,FALSE);
    //CB: todo: precalc expanded items here
    TREEVIEW_CalcItems(infoPtr);
    TREEVIEW_Refresh(infoPtr);

    if (bSetSelection)
    {
        /* Don't call DoSelectItem, it sends notifications. */
        if (infoPtr->selectedItem)
            infoPtr->selectedItem->state &= ~TVIS_SELECTED;
        wineItem->state |= TVIS_SELECTED;
        infoPtr->selectedItem = wineItem;

        TREEVIEW_EnsureVisible(infoPtr,wineItem,TRUE);
    }

    return TRUE;
}

static BOOL
TREEVIEW_Expand(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem,
                BOOL bExpandPartial, BOOL bUser)
{
    //TRACE("TVE_EXPAND\n");

    if (!TREEVIEW_HasChildren(infoPtr, wineItem)
        || (wineItem->state & TVIS_EXPANDED))
        return FALSE;

    //TRACE("  is not expanded...\n");

    if (bUser || !(wineItem->state & TVIS_EXPANDEDONCE))
    {
        //TRACE("  and has never been expanded...\n");

        if (bExpandPartial)
        {
return FALSE; //CB: how does this work??? (only display one level? nonsense in MSDN docu)
          wineItem->state ^=TVIS_EXPANDED;
          wineItem->state |=TVIS_EXPANDEDONCE;
        }

        wineItem->state |= TVIS_EXPANDED;
        //TRACE("  TVN_ITEMEXPANDING sent...\n");

        if (!TREEVIEW_SendExpanding(infoPtr, wineItem, TVE_EXPAND))
        {
            //TRACE("  TVN_ITEMEXPANDING returned TRUE, exiting...\n");
            return FALSE;
        }

        TREEVIEW_SendExpanded(infoPtr, wineItem, TVE_EXPAND);

        wineItem->state |= TVIS_EXPANDEDONCE;
    }
    else
    {
        /* this item has already been expanded */
        wineItem->state |= TVIS_EXPANDED;
    }

    //if (bExpandPartial)
    //  FIXME("TVE_EXPANDPARTIAL not implemented\n");

    //update window
    //CB: todo: optimize!
    TREEVIEW_UnqueueRefresh(infoPtr,TRUE,FALSE);

    /* Scroll up so that as many children as possible are visible.
     * This looses when expanding causes an HScroll bar to appear, but we
     * don't know that yet, so the last item is obscured. */
    if (wineItem->firstChild && wineItem->inclient && (infoPtr->firstVisible != wineItem) && (wineItem->lastChild->rect.bottom > infoPtr->clientHeight))
    {
      INT childrenH = wineItem->lastChild->rect.bottom-wineItem->firstChild->rect.top;
      INT itemH = wineItem->rect.bottom-wineItem->rect.top;
      INT clientH = ((INT)(infoPtr->clientHeight/infoPtr->uItemHeight))*infoPtr->uItemHeight;

      if (itemH+childrenH > clientH)
      {
        infoPtr->lefttop.y += wineItem->rect.top;
      } else
      {
        INT diff = wineItem->lastChild->rect.bottom-clientH;

        infoPtr->lefttop.y += diff;
      }
    }

    TREEVIEW_CalcItems(infoPtr);
    TREEVIEW_Refresh(infoPtr);
    //CB: todo: check cx and cy to fit ranges!

    return TRUE;
}

static BOOL
TREEVIEW_Toggle(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *wineItem, BOOL bUser)
{
    if (wineItem->state & TVIS_EXPANDED)
        return TREEVIEW_Collapse(infoPtr, wineItem, FALSE, bUser);
    else
        return TREEVIEW_Expand(infoPtr, wineItem, FALSE, bUser);
}

static VOID
TREEVIEW_ExpandAll(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    TREEVIEW_Expand(infoPtr, item, FALSE, TRUE);

    for (item = item->firstChild; item != NULL; item = item->nextSibling)
    {
        if (TREEVIEW_HasChildren(infoPtr, item))
            TREEVIEW_ExpandAll(infoPtr, item);
    }
}

/* Note:If the specified item is the child of a collapsed parent item,
   the parent's list of child items is (recursively) expanded to reveal the
   specified item. This is mentioned for TREEVIEW_SelectItem; don't
   know if it also applies here.
*/

static LRESULT
TREEVIEW_ExpandMsg(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_ITEM *wineItem = (HTREEITEM)lParam;
    UINT flag = wParam;

    if (!TREEVIEW_ValidItem(infoPtr, wineItem))
        return 0;

    switch (flag & TVE_TOGGLE)
    {
    case TVE_COLLAPSE:
        return TREEVIEW_Collapse(infoPtr, wineItem, flag & TVE_COLLAPSERESET,
                                 FALSE);

    case TVE_EXPAND:
        return TREEVIEW_Expand(infoPtr, wineItem, flag & TVE_EXPANDPARTIAL,
                               FALSE);

    case TVE_TOGGLE:
        return TREEVIEW_Toggle(infoPtr, wineItem, TRUE);

    default:
        return 0;
    }

}

/* Hit-Testing **********************************************************/

static TREEVIEW_ITEM *
TREEVIEW_HitTestPoint(TREEVIEW_INFO *infoPtr,POINT pt)
{
  TREEVIEW_ITEM *wineItem;

  if (!infoPtr->firstVisible)
    return NULL;

  wineItem = infoPtr->firstVisible;

  while (wineItem && (pt.y > wineItem->rect.bottom))
    wineItem = TREEVIEW_GetNextListItem(infoPtr,wineItem);

  return wineItem;
}

static LRESULT
TREEVIEW_HitTest(TREEVIEW_INFO *infoPtr,LPTVHITTESTINFO lpht,BOOL ignoreClientRect)
{
    TREEVIEW_ITEM *wineItem;
    RECT rect;
    UINT status;
    LONG x, y;

    if (!lpht) return 0;

    status = 0;
    x = lpht->pt.x;
    y = lpht->pt.y;
    lpht->hItem = 0;

    if (!ignoreClientRect)
    {
      GetClientRect(infoPtr->hwnd,&rect);

      if (x < rect.left) status |= TVHT_TOLEFT;
      else if (x > rect.right) status |= TVHT_TORIGHT;
      if (y < rect.top) status |= TVHT_ABOVE;
      else if (y > rect.bottom) status |= TVHT_BELOW;

      if (status)
      {
        lpht->flags = status;
        return (LRESULT)(HTREEITEM)NULL;
      }
    }

    wineItem = TREEVIEW_HitTestPoint(infoPtr,lpht->pt);
    if (!wineItem)
    {
        lpht->flags = TVHT_NOWHERE;
        return (LRESULT)(HTREEITEM)NULL;
    }

    if (!wineItem->calculated)
      TREEVIEW_CalcItem(infoPtr,wineItem,0);

    if (x >= wineItem->textOffset + wineItem->textWidth)
    {
        lpht->flags = TVHT_ONITEMRIGHT;
    }
    else if (x >= wineItem->textOffset)
    {
        lpht->flags = TVHT_ONITEMLABEL;
    }
    else if (x >= wineItem->imageOffset)
    {
        lpht->flags = TVHT_ONITEMICON;
    }
    else if (x >= wineItem->stateOffset)
    {
        lpht->flags = TVHT_ONITEMSTATEICON;
    }
    else if ((x >= wineItem->linesOffset) && (infoPtr->dwStyle & TVS_HASBUTTONS))
    {
        lpht->flags = TVHT_ONITEMBUTTON;
    }
    else
    {
        lpht->flags = TVHT_ONITEMINDENT;
    }

    lpht->hItem = wineItem;
    //TRACE("(%ld,%ld):result %x\n", lpht->pt.x, lpht->pt.y, lpht->flags);

    return (LRESULT)wineItem;
}

/* Item Label Editing ***************************************************/

static LRESULT
TREEVIEW_GetEditControl(TREEVIEW_INFO *infoPtr)
{
    return infoPtr->hwndEdit;
}

static LRESULT CALLBACK
TREEVIEW_Edit_SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_INFO *infoPtr;
    BOOL bCancel = FALSE;

    switch (uMsg)
    {
    case WM_PAINT:
        {
            LRESULT rc;
            TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(GetParent(hwnd));

            //TRACE("WM_PAINT start\n");
            rc = CallWindowProcA(infoPtr->wpEditOrig, hwnd, uMsg, wParam,
                                 lParam);
            //TRACE("WM_PAINT done\n");
            return rc;
        }

    case WM_KILLFOCUS:
    {
        TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(GetParent(hwnd));
        if (infoPtr->bIgnoreEditKillFocus)
            return TRUE;

        break;
    }

    case WM_GETDLGCODE:
        return DLGC_WANTARROWS | DLGC_WANTALLKEYS;

    case WM_KEYDOWN:
        if (wParam == (WPARAM)VK_ESCAPE)
        {
            bCancel = TRUE;
            break;
        }
        else if (wParam == (WPARAM)VK_RETURN)
        {
            break;
        }

        /* fall through */
    default:
        {
            TREEVIEW_INFO *infoPtr = TREEVIEW_GetInfoPtr(GetParent(hwnd));

            //@@@PH 1999/11/05 method called with freed infoPtr memory object
            if (infoPtr != NULL)
              return CallWindowProcA(infoPtr->wpEditOrig, hwnd, uMsg, wParam, lParam);
            else
              break;
        }
    }

    /* Processing TVN_ENDLABELEDIT message could kill the focus       */
    /* eg. Using a messagebox                                         */

    infoPtr = TREEVIEW_GetInfoPtr(GetParent(hwnd));
    infoPtr->bIgnoreEditKillFocus = TRUE;
    TREEVIEW_EndEditLabelNow(infoPtr, bCancel || !infoPtr->bLabelChanged);
    infoPtr->bIgnoreEditKillFocus = FALSE;

    return 0;
}


/* should handle edit control messages here */

static LRESULT
TREEVIEW_Command(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    //TRACE("%x %ld\n", wParam, lParam);

    switch (HIWORD(wParam))
    {
    case EN_UPDATE:
        {
            /*
             * Adjust the edit window size
             */
            char buffer[1024];
            TREEVIEW_ITEM *editItem = infoPtr->selectedItem;
            HDC hdc = GetDC(infoPtr->hwndEdit);
            SIZE sz;
            int len;
            HFONT hFont, hOldFont = 0;

            infoPtr->bLabelChanged = TRUE;

            len = GetWindowTextA(infoPtr->hwndEdit, buffer, sizeof(buffer));

            /* Select font to get the right dimension of the string */
            hFont = SendMessageA(infoPtr->hwndEdit, WM_GETFONT, 0, 0);
            if (hFont != 0)
            {
                hOldFont = SelectObject(hdc, hFont);
            }

            if (GetTextExtentPoint32A(hdc, buffer, strlen(buffer), &sz))
            {
                TEXTMETRICA textMetric;

                /* Add Extra spacing for the next character */
                GetTextMetricsA(hdc, &textMetric);
                sz.cx += (textMetric.tmMaxCharWidth * 2);

                sz.cx = MAX(sz.cx, textMetric.tmMaxCharWidth * 3);
                sz.cx = MIN(sz.cx,
                            infoPtr->clientWidth - editItem->textOffset + 2);

                SetWindowPos(infoPtr->hwndEdit,
                             HWND_TOP,
                             0,
                             0,
                             sz.cx,
                             editItem->rect.bottom - editItem->rect.top + 3,
                             SWP_NOMOVE | SWP_DRAWFRAME);
            }

            if (hFont != 0)
            {
                SelectObject(hdc, hOldFont);
            }

            ReleaseDC(infoPtr->hwnd, hdc);
            break;
        }

    default:
        return SendMessageA(GetParent(infoPtr->hwnd), WM_COMMAND, wParam, lParam);
    }

    return 0;
}

HWND TREEVIEW_EditLabel(TREEVIEW_INFO *infoPtr,HTREEITEM hItem,BOOL unicode)
{
  SIZE sz;
  TREEVIEW_ITEM *editItem = hItem;
  HINSTANCE hinst = GetWindowLongA(infoPtr->hwnd,GWL_HINSTANCE);
  HDC hdc;
  HFONT hOldFont = 0;
  TEXTMETRICA textMetric;
  CHAR* textA = NULL;
  NMTVDISPINFOW tvdi;

  if (!TREEVIEW_ValidItem(infoPtr, editItem))
    return (HWND)NULL;

  if(infoPtr->hwndEdit)
    return infoPtr->hwndEdit;

  infoPtr->bLabelChanged = FALSE;

  /* Make shure that edit item is selected */

  TREEVIEW_DoSelectItem(infoPtr,TVGN_CARET,hItem,TVC_UNKNOWN);
  TREEVIEW_EnsureVisible(infoPtr,hItem,TRUE);

  TREEVIEW_UpdateDispInfo(infoPtr, editItem, TVIF_TEXT);

  hdc = GetDC(infoPtr->hwnd);
  /* Select the font to get appropriate metric dimensions */
  if(infoPtr->hFont != 0)
  {
      hOldFont = SelectObject(hdc, infoPtr->hFont);
  }

  /*Get String Lenght in pixels */
  GetTextExtentPoint32W(hdc,editItem->pszText,lstrlenW(editItem->pszText),&sz);

  /*Add Extra spacing for the next character */
  GetTextMetricsA(hdc, &textMetric);
  sz.cx += (textMetric.tmMaxCharWidth * 2);

  sz.cx = MAX(sz.cx, textMetric.tmMaxCharWidth * 3);
  sz.cx = MIN(sz.cx, infoPtr->clientWidth - editItem->textOffset + 2);

  if(infoPtr->hFont != 0)
  {
      SelectObject(hdc, hOldFont);
  }

  ReleaseDC(infoPtr->hwnd, hdc);
  infoPtr->hwndEdit = CreateWindowExA (
                         WS_EX_LEFT,
                         "EDIT",
                          0,
                         WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_CLIPSIBLINGS |
                         ES_WANTRETURN | ES_LEFT,
                         editItem->textOffset - 2, editItem->rect.top  - 1,
                         sz.cx+3, editItem->rect.bottom - editItem->rect.top  + 3,
                         infoPtr->hwnd,
                         0,hinst,0); /* FIXME: (HMENU)IDTVEDIT,pcs->hInstance,0);*/

    /* Get a 2D border. */
  SetWindowLongA(infoPtr->hwndEdit, GWL_EXSTYLE,GetWindowLongA(infoPtr->hwndEdit, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);
  SetWindowLongA(infoPtr->hwndEdit, GWL_STYLE,GetWindowLongA(infoPtr->hwndEdit, GWL_STYLE) | WS_BORDER);

  SendMessageA(infoPtr->hwndEdit, WM_SETFONT, TREEVIEW_FontForItem(infoPtr, editItem),FALSE);

  infoPtr->wpEditOrig = (WNDPROC)SetWindowLongA (infoPtr->hwndEdit,GWL_WNDPROC,(DWORD)TREEVIEW_Edit_SubclassProc);

  tvdi.item.mask        = TVIF_HANDLE | TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
  tvdi.item.hItem       = editItem;
  tvdi.item.state       = editItem->state;
  tvdi.item.lParam      = editItem->lParam;
  if (isUnicodeNotify(&infoPtr->header))
  {
    tvdi.item.pszText     = editItem->pszText;
  } else
  {
    textA = HEAP_strdupWtoA(GetProcessHeap(),0,editItem->pszText);
    tvdi.item.pszText = (LPWSTR)textA;
  }

  if (sendNotify(infoPtr->hwnd,isUnicodeNotify(&infoPtr->header) ? TVN_BEGINLABELEDITW:TVN_BEGINLABELEDITA,&tvdi.hdr))
  {
     DestroyWindow(infoPtr->hwndEdit);
     infoPtr->hwndEdit = 0;
     if (textA) HeapFree(GetProcessHeap(),0,textA);

     return (HWND)0;
  }
  if (textA) HeapFree(GetProcessHeap(),0,textA);

  infoPtr->selectedItem = hItem;
  SetWindowTextW(infoPtr->hwndEdit,editItem->pszText);
  SetFocus(infoPtr->hwndEdit);
  SendMessageA(infoPtr->hwndEdit, EM_SETSEL, 0, -1);
  ShowWindow(infoPtr->hwndEdit, SW_SHOW);

  return infoPtr->hwndEdit;
}


static LRESULT
TREEVIEW_EndEditLabelNow(TREEVIEW_INFO *infoPtr,BOOL bCancel)
{
  TREEVIEW_ITEM *editedItem = infoPtr->selectedItem;
  NMTVDISPINFOW tvdi;
  DWORD dwStyle = GetWindowLongA(infoPtr->hwnd,GWL_STYLE);
  BOOL bCommit;
  WCHAR *textW = NULL;
  CHAR *textA = NULL;
  int  iLength = 0;

  if (!infoPtr->hwndEdit)
     return FALSE;

  tvdi.item.mask        = 0;
  tvdi.item.hItem       = editedItem;
  tvdi.item.state       = editedItem->state;
  tvdi.item.lParam      = editedItem->lParam;

  if (!bCancel)
  {
    textW = (WCHAR*)COMCTL32_Alloc(1024*sizeof(WCHAR));
    iLength = GetWindowTextW(infoPtr->hwndEdit,textW,1023);

    if (isUnicodeNotify(&infoPtr->header)) tvdi.item.pszText = textW; else
    {
      INT len = iLength+1;

      textA = (CHAR*)COMCTL32_Alloc(1024*sizeof(CHAR));
      lstrcpynWtoA(textA,textW,len);
      tvdi.item.pszText = (WCHAR*)textA;
    }
    tvdi.item.cchTextMax  = iLength + 1;
  } else
  {
    tvdi.item.pszText = NULL;
    tvdi.item.cchTextMax  = 0;
  }

  bCommit = (BOOL)sendNotify(infoPtr->hwnd,isUnicodeNotify(&infoPtr->header) ? TVN_ENDLABELEDITW:TVN_ENDLABELEDITA,&tvdi.hdr);

  if (!bCancel && bCommit) /* Apply the changes */
  {
    BOOL mustFree = FALSE;

    if (!isUnicodeNotify(&infoPtr->header))
      lstrcpynAtoW(textW,textA,iLength+1);

    if (lstrcmpW(textW,editedItem->pszText) != 0)
    {
      if (editedItem->callbackMask & TVIF_TEXT)
      {
        NMTVDISPINFOW tvdi;
        BOOL retval;

        tvdi.item.mask        = TVIF_TEXT;
        tvdi.item.hItem       = editedItem;
        tvdi.item.state       = editedItem->state;
        tvdi.item.lParam      = editedItem->lParam;
        if (isUnicodeNotify(&infoPtr->header))
        {
          tvdi.item.pszText     = textW;
          tvdi.item.cchTextMax  = lstrlenW(textW)+1;
        } else
        {
          tvdi.item.pszText     = (LPWSTR)textA;
          tvdi.item.cchTextMax  = lstrlenA(textA)+1;
        }

        retval = (BOOL)sendNotify(infoPtr->hwnd,isUnicodeNotify(&infoPtr->header) ? TVN_SETDISPINFOW:TVN_SETDISPINFOA,&tvdi.hdr);
      } else
      {
        if (!COMCTL32_ReAlloc(editedItem->pszText,(iLength+1)*sizeof(WCHAR)))
        {
          //ERR("OutOfMemory, cannot allocate space for label");
          DestroyWindow(infoPtr->hwndEdit);
          infoPtr->hwndEdit = 0;
          if (textA) COMCTL32_Free(textA);
          if (textW) COMCTL32_Free(textW);

          return FALSE;
        } else
        {
          editedItem->cchTextMax = iLength + 1;
          lstrcpyW(editedItem->pszText,textW);
        }
      }
    }
  }

  ShowWindow(infoPtr->hwndEdit, SW_HIDE);
  DestroyWindow(infoPtr->hwndEdit);
  infoPtr->hwndEdit = 0;
  if (textA) COMCTL32_Free(textA);
  if (textW) COMCTL32_Free(textW);

  editedItem->calculated = FALSE;
  TREEVIEW_RefreshItem(infoPtr,editedItem,0);

  return TRUE;
}

static LRESULT
TREEVIEW_HandleTimer (TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
    case TV_REFRESH_TIMER:
      KillTimer(infoPtr->hwnd,TV_REFRESH_TIMER);
      infoPtr->Timer &= ~TV_REFRESH_TIMER_SET;
      TREEVIEW_CalcItems(infoPtr);
      TREEVIEW_Refresh(infoPtr);
      return 0;

    case TV_EDIT_TIMER:
      KillTimer(infoPtr->hwnd,TV_EDIT_TIMER);
      infoPtr->Timer &= ~TV_EDIT_TIMER_SET;
      TREEVIEW_EditLabel(infoPtr,infoPtr->selectedItem,TRUE);
      return 0;

    case TV_INFOTIP_TIMER:
      TREEVIEW_CheckInfoTip(infoPtr);
      return 0;
 }

 return 1;
}

/* Mouse Tracking/Drag **************************************************/

/***************************************************************************
 * This is quite unusual piece of code, but that's how it's implemented in
 * Windows.
 */
static LRESULT
TREEVIEW_TrackMouse(TREEVIEW_INFO *infoPtr, POINT pt)
{
    INT cxDrag = GetSystemMetrics(SM_CXDRAG);
    INT cyDrag = GetSystemMetrics(SM_CYDRAG);
    RECT r;
    MSG msg;

    r.top = pt.y - cyDrag;
    r.left = pt.x - cxDrag;
    r.bottom = pt.y + cyDrag;
    r.right = pt.x + cxDrag;

    SetCapture(infoPtr->hwnd);

    while (1)
    {
        if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE | PM_NOYIELD))
        {
            if (msg.message == WM_MOUSEMOVE)
            {
                pt.x = (LONG)(INT16)LOWORD(msg.lParam);
                pt.y = (LONG)(INT16)HIWORD(msg.lParam);
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
                    TREEVIEW_RButtonUp(infoPtr, &pt);
                break;
            }

            DispatchMessageA(&msg);
        }

        if (GetCapture() != infoPtr->hwnd)
            return 0;
    }

    ReleaseCapture();
    return 0;
}

static LRESULT
TREEVIEW_LButtonDoubleClick(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_ITEM *wineItem;
  TV_HITTESTINFO hit;

  hit.pt.x = (LONG)(INT16)LOWORD(lParam);
  hit.pt.y = (LONG)(INT16)HIWORD(lParam);

  SetFocus (infoPtr->hwnd);

  if (infoPtr->Timer & TV_EDIT_TIMER_SET)
  {
     /* If there is pending 'edit label' event - kill it now */
     KillTimer(infoPtr->hwnd,TV_EDIT_TIMER);
  }

  wineItem = (TREEVIEW_ITEM *)TREEVIEW_HitTest(infoPtr,&hit,FALSE);
  if (!wineItem)
    return 0;

  //MSDN documentation says: stop on non zero. but this isn't true in this case
  if (!sendNotify(infoPtr->hwnd,NM_DBLCLK))
  {                             /* FIXME! */
    switch (hit.flags)
    {
      case TVHT_ONITEMRIGHT:
        /* FIXME: we should not have send NM_DBLCLK in this case. */
        break;

      case TVHT_ONITEMINDENT:
        if (!(infoPtr->dwStyle & TVS_HASLINES))
        {
          break;
        } else
        {
          int level = hit.pt.x / infoPtr->uIndent;

          if (!(infoPtr->dwStyle & TVS_LINESATROOT)) level++;

          while (wineItem->iLevel > level)
          {
            wineItem = wineItem->parent;
          }

          /* fall through */
        }

      case TVHT_ONITEMLABEL:
      case TVHT_ONITEMICON:
      case TVHT_ONITEMBUTTON:
        TREEVIEW_Toggle(infoPtr, wineItem, TRUE);
        break;

      case TVHT_ONITEMSTATEICON:
        if (infoPtr->dwStyle & TVS_CHECKBOXES)
          TREEVIEW_ToggleItemState(infoPtr, wineItem);
        else
          TREEVIEW_Toggle(infoPtr, wineItem, TRUE);
        break;
    }
 }

 return TRUE;
}

static LRESULT
TREEVIEW_LButtonDown(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  TVHITTESTINFO ht;
  BOOL bTrack;

  /* If Edit control is active - kill it and return.
   * The best way to do it is to set focus to itself.
   * Edit control subclassed procedure will automatically call
   * EndEditLabelNow.
   */
  if (infoPtr->hwndEdit)
  {
     SetFocus(infoPtr->hwnd);
     return 0;
  }

  ht.pt.x = (LONG)(INT16)LOWORD(lParam);
  ht.pt.y = (LONG)(INT16)HIWORD(lParam);

  TREEVIEW_HitTest(infoPtr,&ht,FALSE);

  bTrack = (ht.flags & TVHT_ONITEM) && !(infoPtr->dwStyle & TVS_DISABLEDRAGDROP);

  /* Send NM_CLICK right away */
  if (!bTrack)
     if (sendNotify(infoPtr->hwnd,NM_CLICK))
        goto setfocus;

  if (ht.flags & TVHT_ONITEMBUTTON)
  {
    TREEVIEW_Toggle(infoPtr,ht.hItem,TRUE);
    goto setfocus;
  } else if (bTrack)
  {
    if (TREEVIEW_TrackMouse(infoPtr,ht.pt))
    {
      TREEVIEW_SendTreeviewDnDNotify(infoPtr,isUnicodeNotify(&infoPtr->header) ? TVN_BEGINDRAGW:TVN_BEGINDRAGA, ht.hItem, ht.pt);
      infoPtr->dropItem = ht.hItem;
      return 0;
    }
  }

  if (sendNotify(infoPtr->hwnd,NM_CLICK))
     goto setfocus;

  /*
   * If the style allow editing and the node is already selected
   * and the click occured on the item label...
   */
  if ((infoPtr->dwStyle & TVS_EDITLABELS) && (ht.flags & TVHT_ONITEMLABEL) && (infoPtr->selectedItem == ht.hItem))
  {
    if (infoPtr->Timer & TV_EDIT_TIMER_SET)
      KillTimer(infoPtr->hwnd,TV_EDIT_TIMER);

    SetTimer(infoPtr->hwnd,TV_EDIT_TIMER,GetDoubleClickTime(),0);
    infoPtr->Timer |= TV_EDIT_TIMER_SET;
  } else if (ht.flags & (TVHT_ONITEMLABEL | TVHT_ONITEMICON))
  {
    TREEVIEW_DoSelectItem(infoPtr,TVGN_CARET,ht.hItem,TVC_BYMOUSE);
  } else if (ht.flags & TVHT_ONITEMSTATEICON)
  {
    if (infoPtr->dwStyle & TVS_CHECKBOXES)
    {
      /* TVS_CHECKBOXES requires us to toggle the current state */
      if (infoPtr->dwStyle & TVS_CHECKBOXES)
        TREEVIEW_ToggleItemState(infoPtr,ht.hItem);
    }
  }

setfocus:
  SetFocus(infoPtr->hwnd);

  return 0;
}

static LRESULT
TREEVIEW_RButtonDown(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
   TVHITTESTINFO ht;

   if (infoPtr->hwndEdit)
   {
      SetFocus(infoPtr->hwnd);
      return 0;
   }

   ht.pt.x = (LONG)(INT16)LOWORD(lParam);
   ht.pt.y = (LONG)(INT16)HIWORD(lParam);

   TREEVIEW_HitTest(infoPtr,&ht,FALSE);

   if (TREEVIEW_TrackMouse(infoPtr,ht.pt))
   {
      if (ht.hItem)
      {
         TREEVIEW_SendTreeviewDnDNotify(infoPtr,isUnicodeNotify(&infoPtr->header) ? TVN_BEGINRDRAGW:TVN_BEGINDRAGA, ht.hItem, ht.pt);
         infoPtr->dropItem = ht.hItem;
      }
   }
   else
   {
      SetFocus(infoPtr->hwnd);
      sendNotify(infoPtr->hwnd,NM_RCLICK);
   }

   return 0;
}

static LRESULT
TREEVIEW_RButtonUp(TREEVIEW_INFO *infoPtr, LPPOINT pPt)
{
    POINT pt = *pPt;

    /* Change to screen coordinate for WM_CONTEXTMENU */
    ClientToScreen(infoPtr->hwnd, &pt);

    /* Send the WM_CONTEXTMENU on a right click */
    SendMessageA( infoPtr->hwnd, WM_CONTEXTMENU, (WPARAM)infoPtr->hwnd, MAKELPARAM(pt.x, pt.y));
    return 0;
}

static LRESULT TREEVIEW_CreateDragImage (TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  TREEVIEW_ITEM *dragItem;
  INT cx,cy;
  HDC    hdc,htopdc;
  HWND hwtop;
  HBITMAP hbmp,hOldbmp;
  SIZE  size;
  RECT  rc;
  HFONT hOldFont;

  if (!(infoPtr->himlNormal)) return 0;
  if (!dragItem || !TREEVIEW_ValidItem(infoPtr, dragItem))
    return 0;

  TREEVIEW_UpdateDispInfo(infoPtr, dragItem, TVIF_TEXT);

  hwtop = GetDesktopWindow();
  htopdc = GetDC(hwtop);
  hdc = CreateCompatibleDC(htopdc);

  hOldFont = SelectObject (hdc, infoPtr->hFont);
  GetTextExtentPoint32W (hdc, dragItem->pszText, lstrlenW (dragItem->pszText), &size);

  hbmp = CreateCompatibleBitmap (htopdc, size.cx, size.cy);
  hOldbmp = SelectObject (hdc, hbmp);

  ImageList_GetIconSize (infoPtr->himlNormal, &cx, &cy);
  size.cx += cx;
  if (cy > size.cy) size.cy = cy;

  infoPtr->dragList = ImageList_Create (size.cx, size.cy, ILC_COLOR, 10, 10);
  ImageList_Draw (infoPtr->himlNormal, dragItem->iImage, hdc, 0, 0, ILD_NORMAL);

/*
 ImageList_GetImageInfo (infoPtr->himlNormal, dragItem->hItem, &iminfo);
 ImageList_AddMasked (infoPtr->dragList, iminfo.hbmImage, CLR_DEFAULT);
*/

  /* draw item text */

  SetRect (&rc, cx, 0, size.cx,size.cy);
  DrawTextW (hdc, dragItem->pszText, lstrlenW(dragItem->pszText), &rc, DT_LEFT);
  SelectObject (hdc, hOldFont);
  SelectObject (hdc, hOldbmp);

  ImageList_Add (infoPtr->dragList, hbmp, 0);

  DeleteDC (hdc);
  DeleteObject (hbmp);
  ReleaseDC (hwtop, htopdc);

  return (LRESULT)infoPtr->dragList;
}

/* Selection ************************************************************/

static LRESULT
TREEVIEW_DoSelectItem(TREEVIEW_INFO *infoPtr, INT action, HTREEITEM newSelect,
                      INT cause)
{
  TREEVIEW_ITEM *prevSelect;
  BOOL refreshPrev = FALSE,refreshNew = FALSE;

  assert(newSelect == NULL || TREEVIEW_ValidItem(infoPtr, newSelect));

  //TRACE("Entering item %p (%s), flag %x, cause %x, state %d\n",
  //    newSelect, TREEVIEW_ItemName(newSelect), action, cause,
  //    newSelect ? newSelect->state : 0);

  if (newSelect && newSelect->parent)
  {
    TREEVIEW_ITEM *parent = newSelect->parent;

    /*
     * If the item has a collapse parent expand the parent so he
     * can expose the item
     */
    while (parent && !(parent->state & TVIS_EXPANDED))
    {
      TREEVIEW_Expand(infoPtr,parent,FALSE,FALSE);
      parent = parent->parent;
    }
  }


  switch (action)
  {
    case TVGN_CARET:
      prevSelect = infoPtr->selectedItem;

      if (prevSelect == newSelect)
          return FALSE;

      if (TREEVIEW_SendTreeviewNotify(infoPtr,
                                      isUnicodeNotify(&infoPtr->header) ? TVN_SELCHANGINGW:TVN_SELCHANGINGA,
                                      cause,
                                      TVIF_HANDLE | TVIF_STATE | TVIF_PARAM,
                                      prevSelect,
                                      newSelect))
        return FALSE;

      if (prevSelect)
      {
        refreshPrev = prevSelect->state & TVIS_SELECTED;
        prevSelect->state &= ~TVIS_SELECTED;
      }

      if (newSelect)
      {
        refreshNew = !(newSelect->state & TVIS_SELECTED);
        newSelect->state |= TVIS_SELECTED;
      }

      infoPtr->selectedItem = newSelect;

      if ((cause == TVC_BYMOUSE) && (infoPtr->dwStyle & TVS_SINGLEEXPAND))
      {
        BOOL control = GetKeyState(VK_CONTROL) & 0x8000;
        UINT rc = TREEVIEW_SendTreeviewNotify(infoPtr,TVN_SINGLEEXPAND,0,TVIF_HANDLE | TVIF_STATE | TVIF_PARAM,prevSelect,newSelect);

        if (!(rc & TVNRET_SKIPOLD) && !control && (prevSelect->state & TVIS_EXPANDED))
          TREEVIEW_Collapse(infoPtr,prevSelect,FALSE,FALSE);
        if (!(rc & TVNRET_SKIPNEW) && TREEVIEW_HasChildren(infoPtr,newSelect))
          TREEVIEW_Toggle(infoPtr,newSelect,FALSE);
      }

      TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE);
      TREEVIEW_EnsureVisible(infoPtr,infoPtr->selectedItem,FALSE);
      if (infoPtr->dwStyle & TVS_FULLROWSELECT)
      {
        TREEVIEW_ITEM *item;

        //deselect last selected row
        if (prevSelect)
        {
          if (refreshPrev) TREEVIEW_RefreshItem(infoPtr,prevSelect,TVIF_IMAGE | TVIF_TEXT);
          if (prevSelect->prevSibling)
          {
            item = prevSelect->prevSibling;
            while (item)
            {
              if (item->state & TVIS_SELECTED)
              {
                item->state &= ~TVIS_SELECTED;
                TREEVIEW_RefreshItem(infoPtr,item,TVIF_IMAGE | TVIF_TEXT);
              }
              item = item->prevSibling;
            }
          }
          if (prevSelect->nextSibling)
          {
            item = prevSelect->nextSibling;
            while (item)
            {
              if (item->state & TVIS_SELECTED)
              {
                item->state &= ~TVIS_SELECTED;
                TREEVIEW_RefreshItem(infoPtr,item,TVIF_IMAGE | TVIF_TEXT);
              }
              item = item->nextSibling;
            }
          }
        }

        //select new row
        if (newSelect)
        {
          if (refreshNew) TREEVIEW_RefreshItem(infoPtr,newSelect,TVIF_IMAGE | TVIF_TEXT);
          if (newSelect->prevSibling)
          {
            item = newSelect->prevSibling;
            while (item)
            {
              if (!(item->state & TVIS_SELECTED))
              {
                item->state |= TVIS_SELECTED;
                TREEVIEW_RefreshItem(infoPtr,item,TVIF_IMAGE | TVIF_TEXT);
              }
              item = item->prevSibling;
            }
          }
          if (newSelect->nextSibling)
          {
            item =  newSelect->nextSibling;
            while (item)
            {
              if (!(item->state & TVIS_SELECTED))
              {
                item->state |= TVIS_SELECTED;
                TREEVIEW_RefreshItem(infoPtr,item,TVIF_IMAGE | TVIF_TEXT);
              }
              item = item->nextSibling;
            }
          }
        }
      } else
      {
        if (refreshPrev) TREEVIEW_RefreshItem(infoPtr,prevSelect,TVIF_IMAGE | TVIF_TEXT);
        if (refreshNew) TREEVIEW_RefreshItem(infoPtr,newSelect,TVIF_IMAGE | TVIF_TEXT);
      }

      TREEVIEW_SendTreeviewNotify(infoPtr,
                                  isUnicodeNotify(&infoPtr->header) ? TVN_SELCHANGEDW:TVN_SELCHANGEDA,
                                  cause,
                                  TVIF_HANDLE | TVIF_STATE | TVIF_PARAM,
                                  prevSelect,
                                  newSelect);
      break;

    case TVGN_DROPHILITE:
      prevSelect = infoPtr->dropItem;

      if (prevSelect)
      {
        refreshPrev = prevSelect->state & TVIS_DROPHILITED;
        prevSelect->state &= ~TVIS_DROPHILITED;
      }

      infoPtr->dropItem = newSelect;

      if (newSelect)
      {
        refreshNew = !(newSelect->state & TVIS_DROPHILITED);
        newSelect->state |= TVIS_DROPHILITED;
      }

      TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE);
      if (refreshPrev) TREEVIEW_RefreshItem(infoPtr,prevSelect,TVIF_IMAGE | TVIF_TEXT);
      if (refreshNew) TREEVIEW_RefreshItem(infoPtr,newSelect,TVIF_IMAGE | TVIF_TEXT);

      break;

    case TVGN_FIRSTVISIBLE:
    {
      TREEVIEW_ITEM *firstVis;
      INT scrollY;

      if (!infoPtr->firstVisible) return FALSE;

      firstVis = infoPtr->firstVisible;

      if (newSelect->rect.top < 0)
        scrollY = newSelect->rect.top;
      else
      {
        scrollY = MIN(newSelect->rect.top,(infoPtr->treeHeight-infoPtr->clientHeight)-infoPtr->lefttop.y);
        scrollY -= scrollY % infoPtr->uItemHeight;
      }

      if (scrollY != 0)
      {
        infoPtr->lefttop.y += scrollY;
        if (!TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE))
          TREEVIEW_CalcItems(infoPtr);

        ScrollWindowEx(infoPtr->hwnd,0,-scrollY,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));
      }

      break;
    }
  }

  //TRACE("Leaving state %d\n", newSelect ? newSelect->state : 0);
  return TRUE;
}

/* FIXME: handle NM_KILLFOCUS etc */
static LRESULT
TREEVIEW_SelectItem(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    HTREEITEM item = (HTREEITEM)lParam;

    if (item != NULL && !TREEVIEW_ValidItem(infoPtr, item))
        return FALSE;

    //TRACE("%p (%s) %d\n", item, TREEVIEW_ItemName(item), (int)wParam);

    if (!TREEVIEW_DoSelectItem(infoPtr, wParam, item, TVC_UNKNOWN))
        return FALSE;

    return TRUE;
}

/* Scrolling ************************************************************/

static LRESULT TREEVIEW_EnsureVisible(TREEVIEW_INFO *infoPtr,HTREEITEM item,BOOL bHScroll)
{
  RECT rect;
  INT scrollY = 0,scrollX = 0;

  if (!item) return FALSE;

  if (!TREEVIEW_ValidItem(infoPtr,item))
    return FALSE;

  if (item && item->parent)
  {
    TREEVIEW_ITEM *parent = item->parent;

    while (parent && !(parent->state & TVIS_EXPANDED))
    {
      TREEVIEW_Expand(infoPtr,parent,FALSE,FALSE);
      parent = parent->parent;
    }
  }

  TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE);
  GetClientRect(infoPtr->hwnd,&rect);
  if (item->rect.top < 0)
    scrollY = item->rect.top;
  else if (item->rect.bottom > rect.bottom)
  {
    INT mod;

    scrollY = item->rect.bottom-rect.bottom;
    mod = scrollY % infoPtr->uItemHeight;
    if (mod) scrollY += infoPtr->uItemHeight-mod;
  }

  if (bHScroll)
  {
    if (!item->calculated) TREEVIEW_CalcItem(infoPtr,item,0);

    if (item->textOffset < rect.left)
      scrollX = item->textOffset-rect.left;
    else if (item->textOffset+item->textWidth > rect.right)
    {
      scrollX = item->textOffset+item->textWidth-rect.right;
      if (scrollX > item->textOffset-rect.left) scrollX = item->textOffset-rect.left;
    }
  }

  if ((scrollY != 0) || (scrollX != 0))
  {
    infoPtr->lefttop.y += scrollY;
    infoPtr->lefttop.x += scrollX;
    TREEVIEW_CalcItems(infoPtr);
    ScrollWindowEx(infoPtr->hwnd,-scrollX,-scrollY,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));

    return TRUE;
  }

  return FALSE;
}

static LRESULT
TREEVIEW_VScroll(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  INT newY,mod;
  INT maxY = infoPtr->treeHeight-infoPtr->clientHeight;

  if (!(infoPtr->uInternalStatus & TV_VSCROLL)) return FALSE;

  mod = maxY % infoPtr->uItemHeight;
  if (mod > 0) maxY += infoPtr->uItemHeight-mod;

  switch (LOWORD (wParam))
  {
    case SB_LINEUP:
      newY = infoPtr->lefttop.y-infoPtr->uItemHeight;
      if (newY < 0) newY = 0;
      break;

    case SB_LINEDOWN:
      newY = infoPtr->lefttop.y+infoPtr->uItemHeight;
      if (newY > maxY) newY = maxY;
      break;

    case SB_PAGEUP:
      newY = infoPtr->lefttop.y-MAX(((INT)(infoPtr->clientHeight/infoPtr->uItemHeight))*infoPtr->uItemHeight,infoPtr->uItemHeight);
      if (newY < 0) newY = 0;
      break;

    case SB_PAGEDOWN:
      newY = infoPtr->lefttop.y+MAX(((INT)(infoPtr->clientHeight/infoPtr->uItemHeight))*infoPtr->uItemHeight,infoPtr->uItemHeight);
      if (newY > maxY) newY = maxY;
      break;

    case SB_THUMBTRACK:
      newY = HIWORD(wParam);
      mod = newY % infoPtr->uItemHeight;
      if (mod > 0) newY -= mod;
      break;

    default:
      return FALSE;
  }

  if (newY != infoPtr->lefttop.y)
  {
    INT scrollY = infoPtr->lefttop.y-newY;
    SCROLLINFO info;

    TREEVIEW_HideInfoTip(infoPtr);
    infoPtr->lefttop.y = newY;
    if (!TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE))
      TREEVIEW_MoveItems(infoPtr,0,scrollY);

    info.cbSize = sizeof(info);
    info.nPos   = infoPtr->lefttop.y;
    info.fMask  = SIF_POS;
    SetScrollInfo(infoPtr->hwnd,SB_VERT,&info,TRUE);

    ScrollWindowEx(infoPtr->hwnd,0,scrollY,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));

    return TRUE;
  }

  return FALSE;
}

static LRESULT
TREEVIEW_HScroll(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  int maxWidth;
  int lastPos = infoPtr->lefttop.x;
  SCROLLINFO info;

  if (!(infoPtr->uInternalStatus & TV_HSCROLL)) return FALSE;

  switch (LOWORD (wParam))
  {
    case SB_LINEUP:
      if (!infoPtr->lefttop.x) return FALSE;
      infoPtr->lefttop.x -= infoPtr->uItemHeight;
      if (infoPtr->lefttop.x < 0) infoPtr->lefttop.x = 0;
      break;

    case SB_LINEDOWN:
      maxWidth = infoPtr->treeWidth-infoPtr->clientWidth;
      if (maxWidth <= 0) return FALSE;
      if (infoPtr->lefttop.x == maxWidth) return FALSE;
      infoPtr->lefttop.x += infoPtr->uItemHeight; /*FIXME */
      if (infoPtr->lefttop.x > maxWidth)
        infoPtr->lefttop.x = maxWidth;
      break;

    case SB_PAGEUP:
      if (!infoPtr->lefttop.x) return FALSE;
      infoPtr->lefttop.x -= infoPtr->clientWidth;
      if (infoPtr->lefttop.x < 0) infoPtr->lefttop.x = 0;
      break;

    case SB_PAGEDOWN:
      maxWidth = infoPtr->treeWidth-infoPtr->clientWidth;
      if (maxWidth <= 0) return FALSE;
      if (infoPtr->lefttop.x == maxWidth) return FALSE;
      infoPtr->lefttop.x += infoPtr->clientWidth;
      if (infoPtr->lefttop.x > maxWidth)
        infoPtr->lefttop.x = maxWidth;
      break;

    case SB_THUMBTRACK:
      maxWidth = infoPtr->treeWidth-infoPtr->clientWidth;
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
    TREEVIEW_HideInfoTip(infoPtr);
    if (!TREEVIEW_UnqueueRefresh(infoPtr,TRUE,TRUE))
      TREEVIEW_MoveItems(infoPtr,lastPos-infoPtr->lefttop.x,0);

    info.cbSize = sizeof(info);
    info.nPos   = infoPtr->lefttop.x;
    info.fMask  = SIF_POS;
    SetScrollInfo(infoPtr->hwnd,SB_HORZ,&info,TRUE);

    ScrollWindowEx(infoPtr->hwnd,lastPos-infoPtr->lefttop.x,0,NULL,NULL,0,NULL,SW_INVALIDATE | SW_SCROLLCHILDREN | (infoPtr->uScrollTime << 16));

    return TRUE;
  }

  return FALSE;
}

static LRESULT
TREEVIEW_MouseWheel(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    short gcWheelDelta;
    UINT pulScrollLines = 3;

    if (infoPtr->firstVisible == NULL)
        return TRUE;

    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &pulScrollLines, 0);

    gcWheelDelta = -(short)HIWORD(wParam);
    pulScrollLines *= (gcWheelDelta / WHEEL_DELTA);

    if (abs(gcWheelDelta) >= WHEEL_DELTA && pulScrollLines)
    {
        int wheelDy = pulScrollLines * infoPtr->uItemHeight;
        int newDy = infoPtr->lefttop.y + wheelDy;
        int maxDy = infoPtr->treeHeight-infoPtr->clientHeight;

        if (newDy > maxDy) newDy = maxDy;

        if (newDy < 0) newDy = 0;

        TREEVIEW_VScroll(infoPtr, MAKEWPARAM(SB_THUMBTRACK, newDy), 0);
    }
    return TRUE;
}

/* Create/Destroy *******************************************************/

static LRESULT
TREEVIEW_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  LPCREATESTRUCTA lpcs = (LPCREATESTRUCTA)lParam;
  TREEVIEW_INFO *infoPtr;
  RECT rcClient;

  /* allocate memory for info structure */
  infoPtr = (TREEVIEW_INFO*)initControl(hwnd,sizeof(TREEVIEW_INFO));

  if (!infoPtr) return 0;

  /* set default settings */
  infoPtr->hwnd = hwnd;
  infoPtr->dwStyle = lpcs->style;
  infoPtr->uInternalStatus = TV_CALCALL;
  infoPtr->Timer = 0;
  infoPtr->uNumItems = 0;
  infoPtr->cdmode = 0;
  infoPtr->uScrollTime = 300;   /* milliseconds */

  GetClientRect(hwnd,&rcClient);

  /* No scroll bars yet. */
  infoPtr->clientWidth = rcClient.right;
  infoPtr->clientHeight = rcClient.bottom;

  infoPtr->treeWidth = 0;
  infoPtr->treeHeight = 0;

  infoPtr->uIndent = 19;
  infoPtr->selectedItem = 0;
  infoPtr->firstVisible = 0;
  infoPtr->maxDisplayOrder = 0;
  infoPtr->dropItem = 0;
  infoPtr->insertMarkItem = 0;
  infoPtr->insertBeforeorAfter = 0;

  infoPtr->clrBk = GetSysColor(COLOR_WINDOW);
  infoPtr->clrText = -1;        /* use system color */
  infoPtr->clrLine = RGB(128, 128, 128);
  infoPtr->clrInsertMark = GetSysColor(COLOR_BTNTEXT);

  infoPtr->lefttop.y = 0;
  infoPtr->lefttop.x = 0;

  infoPtr->hwndEdit = 0;
  infoPtr->wpEditOrig = NULL;
  infoPtr->bIgnoreEditKillFocus = FALSE;
  infoPtr->bLabelChanged = FALSE;

  infoPtr->himlNormal = NULL;
  infoPtr->himlState = NULL;
  infoPtr->normalImageWidth = 0;
  infoPtr->normalImageHeight = 0;
  infoPtr->stateImageWidth = 0;
  infoPtr->stateImageHeight = 0;

  infoPtr->items = DPA_Create(16);

  infoPtr->hFont = GetStockObject(DEFAULT_GUI_FONT);
  infoPtr->hBoldFont = TREEVIEW_CreateBoldFont(infoPtr->hFont);

  infoPtr->uItemHeight = TREEVIEW_NaturalHeight(infoPtr);

  infoPtr->root = TREEVIEW_AllocateItem(infoPtr);
  infoPtr->root->state = TVIS_EXPANDED;
  infoPtr->root->iLevel = -1;
  infoPtr->root->displayOrder = -1;

  infoPtr->pszISearch = NULL;
  infoPtr->uISearchLen = 0;

  infoPtr->tipItem = 0;
  infoPtr->hwndToolTip = 0;
  if (!(infoPtr->dwStyle & TVS_NOTOOLTIPS))
  {
    infoPtr->hwndToolTip = createToolTip(hwnd,TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT,TRUE);
    SendMessageA(infoPtr->hwndToolTip,WM_SETFONT,infoPtr->hFont,0);
  }

  if (infoPtr->dwStyle & TVS_CHECKBOXES)
  {
    RECT rc;
    HBITMAP hbm, hbmOld;
    HDC hdc;
    int nIndex;

    infoPtr->himlState = ImageList_Create(16,16,ILC_COLOR | ILC_MASK,3,0);

    hdc = CreateCompatibleDC(0);
    hbm = CreateCompatibleBitmap(hdc, 48, 16);
    hbmOld = SelectObject(hdc, hbm);

    rc.left  = 0;   rc.top    = 0;
    rc.right = 48;  rc.bottom = 16;
    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));

    rc.left  = 18;   rc.top    = 2;
    rc.right = 30;   rc.bottom = 14;
    DrawFrameControl(hdc, &rc, DFC_BUTTON,
                     DFCS_BUTTONCHECK|DFCS_FLAT);

    rc.left  = 34;   rc.right  = 46;
    DrawFrameControl(hdc, &rc, DFC_BUTTON,
                     DFCS_BUTTONCHECK|DFCS_FLAT|DFCS_CHECKED);

    nIndex = ImageList_AddMasked(infoPtr->himlState, hbm,
                                 GetSysColor(COLOR_WINDOW));
    //TRACE("chckbox index %d\n", nIndex);
    SelectObject(hdc, hbmOld);
    DeleteObject(hbm);
    DeleteDC(hdc);

    infoPtr->stateImageWidth = 16;
    infoPtr->stateImageHeight = 16;
  }

  return 0;
}

static LRESULT
TREEVIEW_Destroy(TREEVIEW_INFO *infoPtr)
{
   TREEVIEW_RemoveTree(infoPtr);

   /* tool tip is automatically destroyed: we are its owner */

   /* Restore original windproc. */
   if (infoPtr->hwndEdit)
     SetWindowLongA(infoPtr->hwndEdit, GWL_WNDPROC,(LONG)infoPtr->wpEditOrig);

   if (infoPtr->Timer & TV_REFRESH_TIMER_SET)
     KillTimer(infoPtr->hwnd, TV_REFRESH_TIMER);

   DeleteObject(infoPtr->hBoldFont);
   COMCTL32_Free(infoPtr->pszISearch);
   doneControl(infoPtr->hwnd);

   return 0;
}

/* Miscellaneous Messages ***********************************************/

static LRESULT
TREEVIEW_ScrollKeyDown(TREEVIEW_INFO *infoPtr, WPARAM key)
{
    static const struct
    {
        unsigned char code;
    }
    scroll[] =
    {
#define SCROLL_ENTRY(dir, code) { ((dir) << 7) | (code) }
        SCROLL_ENTRY(SB_VERT, SB_PAGEUP),       /* VK_PRIOR */
        SCROLL_ENTRY(SB_VERT, SB_PAGEDOWN),     /* VK_NEXT */
        SCROLL_ENTRY(SB_VERT, SB_BOTTOM),       /* VK_END */
        SCROLL_ENTRY(SB_VERT, SB_TOP),          /* VK_HOME */
        SCROLL_ENTRY(SB_HORZ, SB_LINEUP),       /* VK_LEFT */
        SCROLL_ENTRY(SB_VERT, SB_LINEUP),       /* VK_UP */
        SCROLL_ENTRY(SB_HORZ, SB_LINEDOWN),     /* VK_RIGHT */
        SCROLL_ENTRY(SB_VERT, SB_LINEDOWN)      /* VK_DOWN */
#undef SCROLL_ENTRY
    };

    if (key >= VK_PRIOR && key <= VK_DOWN)
    {
        unsigned char code = scroll[key - VK_PRIOR].code;

        (((code & (1 << 7)) == (SB_HORZ << 7))
         ? TREEVIEW_HScroll
         : TREEVIEW_VScroll)(infoPtr, code & 0x7F, 0);
    }

    return 0;
}

/************************************************************************
 *        TREEVIEW_KeyDown
 *
 * VK_UP       Move selection to the previous non-hidden item.
 * VK_DOWN     Move selection to the next non-hidden item.
 * VK_HOME     Move selection to the first item.
 * VK_END      Move selection to the last item.
 * VK_LEFT     If expanded then collapse, otherwise move to parent.
 * VK_RIGHT    If collapsed then expand, otherwise move to first child.
 * VK_ADD      Expand.
 * VK_SUBTRACT Collapse.
 * VK_MULTIPLY Expand all.
 * VK_PRIOR    Move up GetVisibleCount items.
 * VK_NEXT     Move down GetVisibleCount items.
 * VK_BACK     Move to parent.
 * CTRL-Left,Right,Up,Down,PgUp,PgDown,Home,End: Scroll without changing selection
 */
static LRESULT
TREEVIEW_KeyDown(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  /* If it is non-NULL and different, it will be selected and visible. */
  TREEVIEW_ITEM *newSelection = NULL;
  TREEVIEW_ITEM *prevItem = infoPtr->selectedItem;

  TREEVIEW_SendKeyDownNotify(infoPtr,TVN_KEYDOWN,wParam);

  //TRACE("%x %lx\n", wParam, lParam);

  if (prevItem == NULL)
      return FALSE;

  if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
      return TREEVIEW_ScrollKeyDown(infoPtr, wParam);

  switch (wParam)
  {
    case VK_UP:
        newSelection = TREEVIEW_GetPrevListItem(infoPtr, prevItem);
        if (!newSelection)
            newSelection = infoPtr->root->firstChild;
        break;

    case VK_DOWN:
        newSelection = TREEVIEW_GetNextListItem(infoPtr, prevItem);
        break;

    case VK_HOME:
        newSelection = infoPtr->root->firstChild;
        break;

    case VK_END:
        newSelection = TREEVIEW_GetLastListItem(infoPtr,infoPtr->root);
        break;

    case VK_LEFT:
        if (prevItem->state & TVIS_EXPANDED)
        {
            TREEVIEW_Collapse(infoPtr, prevItem, FALSE, TRUE);
        }
        else if (prevItem->parent != infoPtr->root)
        {
            newSelection = prevItem->parent;
        }
        break;

    case VK_RIGHT:
        if (TREEVIEW_HasChildren(infoPtr, prevItem))
        {
            if (!(prevItem->state & TVIS_EXPANDED))
                TREEVIEW_Expand(infoPtr, prevItem, FALSE, TRUE);
            else
            {
                newSelection = prevItem->firstChild;
            }
        }

        break;

    case VK_MULTIPLY:
        TREEVIEW_ExpandAll(infoPtr, prevItem);
        break;

    case VK_ADD:
        if (!(prevItem->state & TVIS_EXPANDED))
            TREEVIEW_Expand(infoPtr, prevItem, FALSE, TRUE);
        break;

    case VK_SUBTRACT:
        if (prevItem->state & TVIS_EXPANDED)
            TREEVIEW_Collapse(infoPtr, prevItem, FALSE, TRUE);
        break;

    case VK_PRIOR:
        newSelection
            = TREEVIEW_GetListItem(infoPtr, prevItem,
                                   -TREEVIEW_GetVisibleCount(infoPtr));
        break;

    case VK_NEXT:
        newSelection
            = TREEVIEW_GetListItem(infoPtr, prevItem,
                                   TREEVIEW_GetVisibleCount(infoPtr));
        break;

    case VK_RETURN:
      sendNotify(infoPtr->hwnd,NM_RETURN);
      break;

    case VK_BACK:
        newSelection = prevItem->parent;
        if (newSelection == infoPtr->root)
            newSelection = NULL;
        break;

    case VK_SPACE:
        if (infoPtr->dwStyle & TVS_CHECKBOXES)
            TREEVIEW_ToggleItemState(infoPtr, prevItem);
        break;
  }

  if (newSelection && newSelection != prevItem)
  {
    if (TREEVIEW_DoSelectItem(infoPtr,TVGN_CARET,newSelection,TVC_BYKEYBOARD))
    {
      TREEVIEW_EnsureVisible(infoPtr,newSelection,FALSE);
    }
  }

  return FALSE;
}

static LRESULT
TREEVIEW_Size(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  if (wParam == SIZE_RESTORED)
  {
    infoPtr->clientWidth = LOWORD(lParam);
    infoPtr->clientHeight = HIWORD(lParam);
    if (TREEVIEW_CalcItems(infoPtr))
      TREEVIEW_Refresh(infoPtr);
  }

  return 0;
}

static LRESULT
TREEVIEW_StyleChanged(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  if (wParam == GWL_STYLE)
  {
    DWORD dwNewStyle = ((LPSTYLESTRUCT)lParam)->styleNew;

    /* we have to take special care about tooltips */
    if ((infoPtr->dwStyle ^ dwNewStyle) & TVS_NOTOOLTIPS)
    {
      if (infoPtr->dwStyle & TVS_NOTOOLTIPS)
      {
        infoPtr->hwndToolTip = createToolTip(infoPtr->hwnd,TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT,TRUE);
        SendMessageA(infoPtr->hwndToolTip,WM_SETFONT,infoPtr->hFont,0);
      } else
      {
        DestroyWindow(infoPtr->hwndToolTip);
        infoPtr->hwndToolTip = 0;
      }
    }

    infoPtr->dwStyle = dwNewStyle;
  }

  if (infoPtr->hwndEdit) SetFocus(infoPtr->hwnd);

  infoPtr->uInternalStatus |= TV_CALCALL;
  TREEVIEW_QueueRefresh(infoPtr);

  return 0;
}

static LRESULT
TREEVIEW_SetFocus(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  sendNotify(infoPtr->hwnd,NM_SETFOCUS);

  if (!(infoPtr->dwStyle & TVS_SHOWSELALWAYS))
  {
    if (infoPtr->selectedItem)
      TREEVIEW_RefreshItem(infoPtr,infoPtr->selectedItem,TVIF_IMAGE | TVIF_TEXT);
    else if (infoPtr->firstVisible)
      TREEVIEW_DoSelectItem(infoPtr,TVGN_CARET,infoPtr->firstVisible,TVC_UNKNOWN);
  }

  return 0;
}

static LRESULT
TREEVIEW_KillFocus(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
  sendNotify(infoPtr->hwnd,NM_KILLFOCUS);

  if (!(infoPtr->dwStyle & TVS_SHOWSELALWAYS) && infoPtr->selectedItem)
    TREEVIEW_RefreshItem(infoPtr,infoPtr->selectedItem,TVIF_IMAGE | TVIF_TEXT);

  return 0;
}

static LRESULT TREEVIEW_RButtonDoubleClick(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  sendNotify(infoPtr->hwnd,NM_RDBLCLK);

  return DefWindowProcA(infoPtr->hwnd,WM_RBUTTONDBLCLK,wParam,lParam);
}


HTREEITEM TREEVIEW_GetInfoTipItem(TREEVIEW_INFO *infoPtr,POINT pt)
{
  TVHITTESTINFO ht;

  ht.pt = pt;
  TREEVIEW_HitTest(infoPtr,&ht,FALSE);

  if (ht.hItem && (ht.flags & TVHT_ONITEM))
  {
    TREEVIEW_ITEM *item = ht.hItem;

    if (item->inclient && ((item->textOffset < 0) || (item->textOffset+item->textWidth > infoPtr->clientWidth)))
      return ht.hItem;
  }

  //check tool rect -> no flickering on tip frame
  if (infoPtr->tipItem)
  {
    POINT pt2 = pt;
    RECT rect;

    GetWindowRect(infoPtr->hwndToolTip,&rect);
    ClientToScreen(infoPtr->hwnd,&pt2);
    return PtInRect(&rect,pt2) ? infoPtr->tipItem:0;
  }

  return 0;
}

VOID TREEVIEW_ShowInfoTip(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item)
{
  LPWSTR text;
  BOOL mustFree = FALSE;
  TTTOOLINFOW ti;
  POINT pt;

  if (infoPtr->dwStyle & TVS_INFOTIP)
  {
    NMTVGETINFOTIPW tvgit;
    WCHAR* buf = (WCHAR*)COMCTL32_Alloc(isUnicodeNotify(&infoPtr->header) ? INFOTIPSIZE*sizeof(WCHAR):INFOTIPSIZE*sizeof(CHAR));

    tvgit.pszText    = buf;
    tvgit.cchTextMax = INFOTIPSIZE;
    tvgit.hItem      = item;
    tvgit.lParam     = item->lParam;

    sendNotify(infoPtr->hwnd,isUnicodeNotify(&infoPtr->header) ? TVN_GETINFOTIPW:TVN_GETINFOTIPA,&tvgit.hdr);
    if (isUnicodeNotify(&infoPtr->header))
    {
      if (buf != tvgit.pszText) COMCTL32_Free(buf); else mustFree = TRUE;
      text = tvgit.pszText;
    } else
    {
      text = (WCHAR*)COMCTL32_Alloc(tvgit.cchTextMax*sizeof(WCHAR));
      mustFree = TRUE;
      lstrcpyAtoW(text,(LPSTR)tvgit.pszText);
      COMCTL32_Free(buf);
    }
  } else
  {
    TREEVIEW_UpdateDispInfo(infoPtr,item,TVIF_TEXT);
    text = item->pszText;
  }

  infoPtr->tipItem = item;
  SetTimer(infoPtr->hwnd,TV_INFOTIP_TIMER,TV_INFOTIP_DELAY,0);
  infoPtr->Timer |= TV_INFOTIP_TIMER_SET;

  ti.cbSize   = sizeof(ti);
  ti.uId      = 0;
  ti.hwnd     = infoPtr->hwnd;
  ti.hinst    = 0;
  ti.lpszText = text;
  SendMessageA(infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&ti);
  pt.x = item->textOffset;
  pt.y = item->rect.top;
  ClientToScreen(infoPtr->hwnd,&pt);
  SendMessageA(infoPtr->hwndToolTip,TTM_TRACKPOSITION,0,(LPARAM)MAKELPARAM(pt.x,pt.y));
  SendMessageA(infoPtr->hwndToolTip,TTM_UPDATETIPTEXTW,0,(LPARAM)&ti);
  SendMessageA(infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&ti);

  if (mustFree) COMCTL32_Free(text);
}

VOID TREEVIEW_HideInfoTip(TREEVIEW_INFO *infoPtr)
{
  if (infoPtr->tipItem)
  {
    TTTOOLINFOA ti;

    infoPtr->tipItem = 0;
    KillTimer(infoPtr->hwnd,TV_INFOTIP_TIMER);
    infoPtr->Timer &= ~TV_INFOTIP_TIMER_SET;

    ti.cbSize   = sizeof(TTTOOLINFOA);
    ti.uId      = 0;
    ti.hwnd     = (UINT)infoPtr->hwnd;

    SendMessageA(infoPtr->hwndToolTip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&ti);
  }
}

static VOID TREEVIEW_CheckInfoTip(TREEVIEW_INFO *infoPtr)
{
  HTREEITEM hItem;
  POINT pt;

  GetCursorPos(&pt);
  ScreenToClient(infoPtr->hwnd,&pt);
  hItem = TREEVIEW_GetInfoTipItem(infoPtr,pt);

  if (hItem != infoPtr->tipItem)
    TREEVIEW_HideInfoTip(infoPtr);
}

HTREEITEM TREEVIEW_GetHottrackItem(TREEVIEW_INFO *infoPtr,POINT pt)
{
  TVHITTESTINFO ht;

  ht.pt = pt;
  TREEVIEW_HitTest(infoPtr,&ht,FALSE);

  return (ht.hItem && (ht.flags & TVHT_ONITEM)) ? ht.hItem:0;
}

static LRESULT TREEVIEW_MouseMove(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  POINT pt;

  pt.x = (INT)LOWORD(lParam);
  pt.y = (INT)HIWORD(lParam);

  if (infoPtr->hwndToolTip)
  {
    HTREEITEM hItem = TREEVIEW_GetInfoTipItem(infoPtr,pt);

    if (infoPtr->tipItem != hItem)
    {
      if (hItem)
      {
        TREEVIEW_ITEM *item = hItem;

        TREEVIEW_ShowInfoTip(infoPtr,item);
      } else TREEVIEW_HideInfoTip(infoPtr);
    }
  }

  if (infoPtr->dwStyle & TVS_TRACKSELECT)
  {
    HTREEITEM hItem = TREEVIEW_GetHottrackItem(infoPtr,pt);

    if (infoPtr->hotItem != hItem)
    {
      TREEVIEW_ITEM *item;
      HDC hdc = 0;

      item = infoPtr->hotItem;
      if (TREEVIEW_ValidItem(infoPtr,infoPtr->hotItem))
      {
        if (!hdc) hdc = GetDC(infoPtr->hwnd);
        TREEVIEW_DrawHottrackLine(hdc,item);
      }
      if (hItem)
      {
        item = hItem;
        if (item)
        {
          if (!hdc) hdc = GetDC(infoPtr->hwnd);
          TREEVIEW_DrawHottrackLine(hdc,item);
        }
      }
      if (hdc) ReleaseDC(infoPtr->hwnd,hdc);
    }
  }

  return 0;
}

static LRESULT TREEVIEW_SetRedraw(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  if (wParam)
  {
    if (!(infoPtr->uInternalStatus & TV_NOREDRAW)) return 0;
    infoPtr->uInternalStatus &= ~TV_NOREDRAW;
    TREEVIEW_CalcItems(infoPtr);
    TREEVIEW_Refresh(infoPtr);
  } else
  {
    infoPtr->uInternalStatus |= TV_NOREDRAW;
  }

  return 0;
}

static LRESULT
TREEVIEW_SysColorChange(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  TREEVIEW_Refresh(infoPtr);

  return DefWindowProcA(infoPtr->hwnd,WM_SYSCOLORCHANGE,wParam,lParam);
}

static LRESULT TREEVIEW_Enable(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  TREEVIEW_Refresh(infoPtr);

  return DefWindowProcA(infoPtr->hwnd,WM_ENABLE,wParam,lParam);
}

static LRESULT
TREEVIEW_EraseBackground(TREEVIEW_INFO *infoPtr, WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBrush = CreateSolidBrush (infoPtr->clrBk);
    RECT rect;

    GetClientRect (infoPtr->hwnd, &rect);
    FillRect ((HDC)wParam, &rect, hBrush);
    DeleteObject (hBrush);

    return TRUE;
}

static LRESULT TREEVIEW_GetDlgCode(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  return DLGC_WANTARROWS | DLGC_WANTCHARS;
}


static LRESULT TREEVIEW_Char(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  CHAR ch = (CHAR)wParam;

  TREEVIEW_ISearch(infoPtr,ch);

  return 0;
}

static BOOL TREEVIEW_Compare(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item,LPWSTR text,INT textlen,BOOL *matchLast)
{
  BOOL res;
  INT itemlen;

  TREEVIEW_UpdateDispInfo(infoPtr,item,TVIF_TEXT);

  itemlen = lstrlenW(item->pszText);
  if (itemlen < textlen)
  {
    res = FALSE;
  } else
  {
    res = (lstrcmpniW(item->pszText,text,textlen) == 0);
  }
  if (!res && matchLast)
  {
    textlen--;
    if ((textlen > 0) && (itemlen >= textlen))
      *matchLast = (lstrcmpniW(item->pszText,text,textlen) == 0);
    else
      *matchLast = FALSE;
  }

  return res;
}

static TREEVIEW_ITEM* TREEVIEW_Search(TREEVIEW_INFO *infoPtr,TREEVIEW_ITEM *item,LPWSTR text,INT textlen,TREEVIEW_ITEM **nearest)
{
  TREEVIEW_ITEM *start;
  BOOL bMatchLast;

  start = item;
  if (nearest) *nearest = NULL;

  //search start to end
  while (item)
  {
    if (nearest)
    {
      if (TREEVIEW_Compare(infoPtr,item,text,textlen,(!*nearest) ? &bMatchLast:NULL))
        return item;
      else if (!*nearest && bMatchLast)
        *nearest = item;
    } else
    {
      if (TREEVIEW_Compare(infoPtr,item,text,textlen,NULL))
        return item;
    }
    item = TREEVIEW_GetNextListItem(infoPtr,item);
  }

  item = infoPtr->root;

  //search first to start
  while (item != start)
  {
    if (nearest)
    {
      if (TREEVIEW_Compare(infoPtr,item,text,textlen,(!*nearest) ? &bMatchLast:NULL))
        return item;
      else if (!*nearest && bMatchLast)
        *nearest = item;
    } else
    {
      if (TREEVIEW_Compare(infoPtr,item,text,textlen,NULL))
        return item;
    }
    item = TREEVIEW_GetNextListItem(infoPtr,item);
  }

  return NULL;
}

//NOTE: sister function in listview control -> sync changes

static VOID TREEVIEW_ISearch(TREEVIEW_INFO *infoPtr,CHAR ch)
{
  LPWSTR newString;
  INT len;
  CHAR ch2[2];
  TREEVIEW_ITEM *item,*nearest = NULL;
  DWORD dwISearchTime;
  BOOL checkNearest = TRUE;

  //check timer
  dwISearchTime = GetTickCount();
  if ((infoPtr->uISearchLen > 0) && (TICKDIFF(infoPtr->dwISearchTime,dwISearchTime) > TV_ISEARCH_DELAY))
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
  if (infoPtr->selectedItem)
  {
    item = infoPtr->selectedItem;

    //check if new string is valid for old selection
    if (TREEVIEW_Compare(infoPtr,item,newString,len,NULL))
      goto ISearchDone;

    //no match, continue with next item
    item = TREEVIEW_GetNextListItem(infoPtr,item);
    if (!item) item = infoPtr->root;
  } else item = infoPtr->root;

  //scan
  item = TREEVIEW_Search(infoPtr,item,newString,len,checkNearest ? &nearest:NULL);

  if (!item && nearest)
  {
    TREEVIEW_SelectItem(infoPtr,(WPARAM)TVGN_CARET,(LPARAM)nearest);
    TREEVIEW_EnsureVisible(infoPtr,nearest,FALSE);
    infoPtr->dwISearchTime = GetTickCount();

    COMCTL32_Free(newString);
    return;
  }

ISearchDone:
  //done
  if (item)
  {
    COMCTL32_Free(infoPtr->pszISearch);
    infoPtr->pszISearch = newString;
    infoPtr->uISearchLen = len;
    TREEVIEW_SelectItem(infoPtr,(WPARAM)TVGN_CARET,(LPARAM)item);
    TREEVIEW_EnsureVisible(infoPtr,item,FALSE);
    infoPtr->dwISearchTime = GetTickCount();
  } else
  {
    COMCTL32_Free(newString);
    MessageBeep(0xFFFFFFFF);
  }
}

static LRESULT TREEVIEW_GetISearchString(TREEVIEW_INFO *infoPtr,LPWSTR lpsz,BOOL unicode)
{
  if (infoPtr->uISearchLen == 0) return 0;

  if (unicode)
    lstrcpyW(lpsz,infoPtr->pszISearch);
  else
    lstrcpyWtoA((LPSTR)lpsz,infoPtr->pszISearch);

  return infoPtr->uISearchLen;
}

static LRESULT TREEVIEW_SetCursor(TREEVIEW_INFO *infoPtr,WPARAM wParam,LPARAM lParam)
{
  sendNotify(infoPtr->hwnd,NM_SETCURSOR); //CB: todo: result

  return DefWindowProcA(infoPtr->hwnd,WM_SETCURSOR,wParam,lParam);
}

static LRESULT TREEVIEW_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE),oldStyle = dwStyle;

  dwStyle &= ~(WS_HSCROLL | WS_VSCROLL);
  if (dwStyle != oldStyle) SetWindowLongA(hwnd,GWL_STYLE,dwStyle);

  return DefWindowProcA(hwnd,WM_NCCREATE,wParam,lParam);
}

static LRESULT WINAPI
TREEVIEW_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TREEVIEW_INFO *infoPtr;

    if (uMsg == WM_NCCREATE)
    {
      return TREEVIEW_NCCreate(hwnd,wParam,lParam);
    } else if (uMsg == WM_CREATE)
    {
      return TREEVIEW_Create(hwnd, wParam, lParam);
    }

    infoPtr = TREEVIEW_GetInfoPtr(hwnd);
    if (infoPtr) TREEVIEW_VerifyTree(infoPtr); else goto def;

    switch (uMsg)
    {
        case TVM_INSERTITEMA:
          return TREEVIEW_InsertItem(infoPtr,wParam,lParam,FALSE);

        case TVM_INSERTITEMW:
          return TREEVIEW_InsertItem(infoPtr,wParam,lParam,TRUE);

        case TVM_DELETEITEM:
          return TREEVIEW_DeleteItem(infoPtr,wParam,lParam);

        case TVM_EXPAND:
          return TREEVIEW_ExpandMsg(infoPtr,wParam,lParam);

        case TVM_GETITEMRECT:
          return TREEVIEW_GetItemRect(infoPtr,wParam,lParam);

        case TVM_GETCOUNT:
          return TREEVIEW_GetCount(infoPtr,wParam,lParam);

        case TVM_GETINDENT:
          return TREEVIEW_GetIndent(infoPtr);

        case TVM_SETINDENT:
          return TREEVIEW_SetIndent(infoPtr,wParam);

        case TVM_GETIMAGELIST:
          return TREEVIEW_GetImageList(infoPtr,wParam,lParam);

        case TVM_SETIMAGELIST:
          return TREEVIEW_SetImageList (infoPtr, wParam, lParam);

        case TVM_GETNEXTITEM:
          return TREEVIEW_GetNextItem (infoPtr, wParam, lParam);

        case TVM_SELECTITEM:
          return TREEVIEW_SelectItem (infoPtr, wParam, lParam);

        case TVM_GETITEMA:
          return TREEVIEW_GetItem(infoPtr,wParam,lParam,FALSE);

        case TVM_GETITEMW:
          return TREEVIEW_GetItem(infoPtr,wParam,lParam,TRUE);

        case TVM_SETITEMA:
          return TREEVIEW_SetItem(infoPtr,wParam,lParam,FALSE);

        case TVM_SETITEMW:
          return TREEVIEW_SetItem(infoPtr,wParam,lParam,TRUE);

        case TVM_EDITLABELA:
          return TREEVIEW_EditLabel(infoPtr,(HTREEITEM)lParam,FALSE);

        case TVM_EDITLABELW:
          return TREEVIEW_EditLabel(infoPtr,(HTREEITEM)lParam,TRUE);

        case TVM_GETEDITCONTROL:
          return TREEVIEW_GetEditControl (infoPtr);

        case TVM_GETVISIBLECOUNT:
          return TREEVIEW_GetVisibleCount (infoPtr);

        case TVM_HITTEST:
          return TREEVIEW_HitTest(infoPtr,(LPTVHITTESTINFO)lParam,FALSE);

        case TVM_CREATEDRAGIMAGE:
          return TREEVIEW_CreateDragImage (infoPtr, wParam, lParam);

        case TVM_SORTCHILDREN:
          return TREEVIEW_SortChildren(infoPtr, wParam, lParam);

        case TVM_ENSUREVISIBLE:
          return TREEVIEW_EnsureVisible(infoPtr,(HTREEITEM)lParam,TRUE);

        case TVM_SORTCHILDRENCB:
          return TREEVIEW_SortChildrenCB(infoPtr, wParam, lParam);

        case TVM_ENDEDITLABELNOW:
          return TREEVIEW_EndEditLabelNow (infoPtr,(BOOL)wParam);

        case TVM_GETISEARCHSTRINGA:
          return TREEVIEW_GetISearchString(infoPtr,(LPWSTR)lParam,FALSE);

        case TVM_GETISEARCHSTRINGW:
          return TREEVIEW_GetISearchString(infoPtr,(LPWSTR)lParam,TRUE);

        case TVM_GETTOOLTIPS:
          return TREEVIEW_GetToolTips (infoPtr);

        case TVM_SETTOOLTIPS:
          return TREEVIEW_SetToolTips (infoPtr, wParam);

        case TVM_SETINSERTMARK:
          return TREEVIEW_SetInsertMark (infoPtr,wParam, lParam);

        case TVM_SETITEMHEIGHT:
          return TREEVIEW_SetItemHeight (infoPtr, wParam);

        case TVM_GETITEMHEIGHT:
          return TREEVIEW_GetItemHeight (infoPtr);

        case TVM_SETBKCOLOR:
          return TREEVIEW_SetBkColor (infoPtr, wParam, lParam);

        case TVM_SETTEXTCOLOR:
          return TREEVIEW_SetTextColor (infoPtr, wParam, lParam);

        case TVM_GETBKCOLOR:
          return TREEVIEW_GetBkColor (infoPtr);

        case TVM_GETTEXTCOLOR:
          return TREEVIEW_GetTextColor (infoPtr);

        case TVM_SETSCROLLTIME:
          return TREEVIEW_SetScrollTime (infoPtr, (UINT)wParam);

        case TVM_GETSCROLLTIME:
          return TREEVIEW_GetScrollTime (infoPtr);

        case TVM_GETITEMSTATE:
          return TREEVIEW_GetItemState (infoPtr,wParam, lParam);

        case TVM_GETLINECOLOR:
          return TREEVIEW_GetLineColor (infoPtr,wParam, lParam);

        case TVM_SETLINECOLOR:
          return TREEVIEW_SetLineColor (infoPtr,wParam, lParam);

        case TVM_SETINSERTMARKCOLOR:
          return TREEVIEW_SetInsertMarkColor (infoPtr,wParam, lParam);

        case TVM_GETINSERTMARKCOLOR:
          return TREEVIEW_GetInsertMarkColor (infoPtr,wParam, lParam);

        case WM_COMMAND:
          return TREEVIEW_Command (infoPtr, wParam, lParam);

        case WM_DESTROY:
          return TREEVIEW_Destroy (infoPtr);

        case WM_ENABLE:
          return TREEVIEW_Enable(infoPtr,wParam,lParam);

        case WM_ERASEBKGND:
          return TREEVIEW_EraseBackground (infoPtr, wParam, lParam);

        case WM_GETDLGCODE:
          return TREEVIEW_GetDlgCode(infoPtr,wParam,lParam);

        case WM_PAINT:
          return TREEVIEW_Paint (infoPtr, wParam, lParam);

        case WM_GETFONT:
          return TREEVIEW_GetFont (infoPtr, wParam, lParam);

        case WM_SETFONT:
          return TREEVIEW_SetFont (infoPtr, wParam, lParam);

        case WM_KEYDOWN:
          return TREEVIEW_KeyDown (infoPtr, wParam, lParam);

        case WM_CHAR:
          return TREEVIEW_Char(infoPtr,wParam,lParam);

        case WM_SETFOCUS:
          return TREEVIEW_SetFocus (infoPtr, wParam, lParam);

        case WM_KILLFOCUS:
          return TREEVIEW_KillFocus (infoPtr, wParam, lParam);

        case WM_MOUSEMOVE:
          return TREEVIEW_MouseMove(infoPtr,wParam,lParam);

        case WM_LBUTTONDOWN:
          return TREEVIEW_LButtonDown (infoPtr, wParam, lParam);

        case WM_LBUTTONDBLCLK:
          return TREEVIEW_LButtonDoubleClick (infoPtr, wParam, lParam);

        case WM_RBUTTONDOWN:
          return TREEVIEW_RButtonDown (infoPtr, wParam, lParam);

        case WM_RBUTTONDBLCLK:
          return TREEVIEW_RButtonDoubleClick(infoPtr,wParam,lParam);

        case WM_STYLECHANGED:
          return TREEVIEW_StyleChanged (infoPtr, wParam, lParam);

        case WM_SYSCOLORCHANGE:
          return TREEVIEW_SysColorChange(infoPtr,wParam,lParam);

        case WM_SETCURSOR:
          return TREEVIEW_SetCursor(infoPtr,wParam,lParam);

        case WM_SETREDRAW:
          return TREEVIEW_SetRedraw(infoPtr,wParam,lParam);

        case WM_TIMER:
          return TREEVIEW_HandleTimer (infoPtr, wParam, lParam);

        case WM_SIZE:
          return TREEVIEW_Size (infoPtr, wParam,lParam);

        case WM_HSCROLL:
          return TREEVIEW_HScroll (infoPtr, wParam, lParam);

        case WM_VSCROLL:
          return TREEVIEW_VScroll (infoPtr, wParam, lParam);

        case WM_MOUSEWHEEL:
          return TREEVIEW_MouseWheel (infoPtr, wParam, lParam);

        case WM_DRAWITEM:
          //printf ("drawItem\n");
           return DefWindowProcA (hwnd, uMsg, wParam, lParam);

        default:
def:
          return defComCtl32ProcA(hwnd,uMsg,wParam,lParam);
      }
    return 0;
}

/* Tree Verification ****************************************************/

#ifndef NDEBUG
static inline void
TREEVIEW_VerifyChildren(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item);

static inline void TREEVIEW_VerifyItemCommon(TREEVIEW_INFO *infoPtr,
                                             TREEVIEW_ITEM *item)
{
    TREEVIEW_WriteVerify("TV: checking infoPtr");
    assert(infoPtr != NULL);
    TREEVIEW_WriteVerify("TV: checking item");
    assert(item != NULL);

    /* both NULL, or both non-null */
    TREEVIEW_WriteVerify("TV: checking firstChild,lastChild");
    assert((item->firstChild == NULL) == (item->lastChild == NULL));

    TREEVIEW_WriteVerify("TV: checking firstChild,lastChild != item");
    assert(item->firstChild != item);
    assert(item->lastChild != item);

    TREEVIEW_WriteVerify("TV: checking firstChild");
    if (item->firstChild)
    {
        assert(item->firstChild->parent == item);
        assert(item->firstChild->prevSibling == NULL);
    }

    TREEVIEW_WriteVerify("TV: checking lastChild");
    if (item->lastChild)
    {
        TREEVIEW_WriteVerify("TV: checking lastChild->parent");
        assert(item->lastChild->parent == item);
        TREEVIEW_WriteVerify("TV: checking lastChild->nextSibling");
        assert(item->lastChild->nextSibling == NULL);
    }

    TREEVIEW_WriteVerify("TV: checking nextSibling");
    assert(item->nextSibling != item);
    if (item->nextSibling)
    {
        assert(item->nextSibling->parent == item->parent);
        assert(item->nextSibling->prevSibling == item);
    }

    TREEVIEW_WriteVerify("TV: checking prevSibling");
    assert(item->prevSibling != item);
    if (item->prevSibling)
    {
        assert(item->prevSibling->parent == item->parent);
        assert(item->prevSibling->nextSibling == item);
    }
}

static inline void
TREEVIEW_VerifyItem(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    TREEVIEW_WriteVerify("TV: checking item");
    assert(item != NULL);

    TREEVIEW_WriteVerify("TV: checking parent, iLevel");
    assert(item->parent != NULL);
    assert(item->parent != item);
    assert(item->iLevel == item->parent->iLevel + 1);

    TREEVIEW_WriteVerify("TV: checking PtrIndex");
    assert(DPA_GetPtrIndex(infoPtr->items, item) != -1);

    TREEVIEW_VerifyItemCommon(infoPtr, item);

    TREEVIEW_VerifyChildren(infoPtr, item);
}

static inline void
TREEVIEW_VerifyChildren(TREEVIEW_INFO *infoPtr, TREEVIEW_ITEM *item)
{
    TREEVIEW_ITEM *child;
    assert(item != NULL);

    for (child = item->firstChild; child != NULL; child = child->nextSibling)
        TREEVIEW_VerifyItem(infoPtr, child);
}

static inline void
TREEVIEW_VerifyRoot(TREEVIEW_INFO *infoPtr)
{
    TREEVIEW_ITEM *root = infoPtr->root;

    TREEVIEW_WriteVerify("TV: checking root");
    assert(root != NULL);
    TREEVIEW_WriteVerify("TV: checking iLevel");
    assert(root->iLevel == -1);
    TREEVIEW_WriteVerify("TV: checking parent");
    assert(root->parent == NULL);
    TREEVIEW_WriteVerify("TV: checking prevSibling");
    assert(root->prevSibling == NULL);

    TREEVIEW_VerifyItemCommon(infoPtr, root);

    TREEVIEW_VerifyChildren(infoPtr, root);
}

static void
TREEVIEW_VerifyTree(TREEVIEW_INFO *infoPtr)
{
  TREEVIEW_WriteVerify("TV: checking infoPtr");
  assert(infoPtr != NULL);

  TREEVIEW_VerifyRoot(infoPtr);
}
#endif

/* Class Registration ***************************************************/

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
