/* $Id: header.c,v 1.16 1999-10-07 15:46:30 cbratschi Exp $ */
/*
 *  Header control
 *
 *  Copyright 1998 Eric Kohl
 *  Copyright 1999 Achim Hasenmueller
 *  Copyright 1999 Christoph Bratschi
 *
 *  TODO:
 *   - Order list support.
 *   - Control specific cursors (over dividers).
 *   - Custom draw support (including Notifications).
 *   - HDS_FILTERBAR
 *
 *
 * Status: Development in progress, a lot to do :)
 * Version: 5.00 (target)
 */

#include <string.h>

#include "winbase.h"
#include "commctrl.h"
#include "header.h"
#include "comctl32.h"


#define VERT_BORDER     4
#define DIVIDER_WIDTH  10
#define MIN_ITEMWIDTH  0
#define ITEM_FRAMESIZE 2
#define TEXT_MARGIN    3
#define TIMER_ID       1
#define TIMER_MS       200

#define HEADER_GetInfoPtr(hwnd) ((HEADER_INFO *)GetWindowLongA(hwnd,0))

static VOID
HEADER_DrawItemImage(HWND hwnd,HDC hdc,HEADER_INFO* infoPtr,HEADER_ITEM* phdi,RECT* r,INT iItem)
{
  if (phdi->fmt & HDF_IMAGE)
  {
    INT iImage;
    IMAGEINFO info;
    INT x,y,cx,cy,w,h,rx,ry;

    if (phdi->iImage == I_IMAGECALLBACK)
    {
      if (IsWindowUnicode(GetParent(hwnd)))
      {
        NMHDDISPINFOW nmhdr;

        nmhdr.hdr.hwndFrom = hwnd;
        nmhdr.hdr.idFrom   = GetWindowLongW(hwnd,GWL_ID);
        nmhdr.hdr.code     = HDN_GETDISPINFOW;
        nmhdr.iItem      = iItem;
        nmhdr.mask       = HDI_IMAGE;
        nmhdr.iImage     = 0;
        nmhdr.lParam     = phdi->lParam;

        SendMessageW(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);

        iImage = nmhdr.iImage;
        if (nmhdr.mask & HDI_DI_SETITEM) phdi->iImage = iImage;
      } else
      {
        NMHDDISPINFOA nmhdr;

        nmhdr.hdr.hwndFrom = hwnd;
        nmhdr.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
        nmhdr.hdr.code     = HDN_GETDISPINFOA;
        nmhdr.iItem      = iItem;
        nmhdr.mask       = HDI_IMAGE;
        nmhdr.iImage     = 0;
        nmhdr.lParam     = phdi->lParam;

        SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);

        iImage = nmhdr.iImage;
        if (nmhdr.mask & HDI_DI_SETITEM) phdi->iImage = iImage;
      }
    } else iImage = phdi->iImage;

    if (!ImageList_GetImageInfo(infoPtr->himl,phdi->iImage,&info)) return;
    w = info.rcImage.right-info.rcImage.left;
    h = info.rcImage.bottom-info.rcImage.top;

    ry = r->bottom-r->top;
    rx = r->right-r->left;

    if (ry >= h)
    {
      cy = h;
      y = r->top+(ry-h)/2;
    } else
    {
      cy = ry;
      y = r->top;
    }

    if (rx >= w+infoPtr->xBmpMargin)
      cx = w;
    else
      cx = MAX(0,rx-w-infoPtr->xBmpMargin);

    if (ImageList_DrawEx(infoPtr->himl,phdi->iImage,hdc,x,y,cx,cy,CLR_NONE,CLR_NONE,ILD_TRANSPARENT))
    {
      r->left += w+2*infoPtr->xBmpMargin;
    }
  }
}

static VOID
HEADER_DrawItemBitmap(HDC hdc,HEADER_INFO* infoPtr,HEADER_ITEM* phdi,RECT* r,WCHAR* pszText,UINT uTextJustify)
{
  if ((phdi->fmt & HDF_BITMAP) && !(phdi->fmt & HDF_BITMAP_ON_RIGHT) && (phdi->hbm))
  {
    BITMAP bmp;
    HDC    hdcBitmap;
    INT    yD,yS,cx,cy,rx,ry;

    GetObjectA (phdi->hbm,sizeof(BITMAP),(LPVOID)&bmp);

    ry = r->bottom-r->top;
    rx = r->right-r->left;

    if (ry >= bmp.bmHeight)
    {
      cy = bmp.bmHeight;
      yD = r->top + (ry - bmp.bmHeight) / 2;
      yS = 0;
    } else
    {
      cy = ry;
      yD = r->top;
      yS = (bmp.bmHeight - ry) / 2;
    }

    if (rx >= bmp.bmWidth+infoPtr->xBmpMargin)
      cx = bmp.bmWidth;
    else
      cx = MAX(0,rx-bmp.bmWidth-infoPtr->xBmpMargin);

    hdcBitmap = CreateCompatibleDC (hdc);
    SelectObject(hdcBitmap,phdi->hbm);
    BitBlt (hdc,r->left+infoPtr->xBmpMargin,yD,cx,cy,hdcBitmap,0,yS,SRCCOPY);
    DeleteDC (hdcBitmap);

    r->left += bmp.bmWidth+2*infoPtr->xBmpMargin;
  } else if ((phdi->fmt & HDF_BITMAP_ON_RIGHT) && (phdi->hbm))
  {
    BITMAP bmp;
    HDC    hdcBitmap;
    INT    xD,yD,yS,cx,cy,rx,ry,tx;
    RECT   textRect;

    GetObjectA (phdi->hbm,sizeof(BITMAP),(LPVOID)&bmp);

    textRect = *r;
    DrawTextExW(hdc,pszText,lstrlenW(pszText),&textRect,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_CALCRECT|DT_END_ELLIPSIS,NULL);
    tx = textRect.right - textRect.left;
    ry = r->bottom-r->top;
    rx = r->right-r->left;

    if (ry >= bmp.bmHeight)
    {
      cy = bmp.bmHeight;
      yD = r->top + (ry - bmp.bmHeight) / 2;
      yS = 0;
    } else
    {
      cy = ry;
      yD = r->top;
      yS = (bmp.bmHeight - ry) / 2;
    }

    if (r->left+tx+bmp.bmWidth+2*infoPtr->xBmpMargin <= r->right)
    {
      cx = bmp.bmWidth;
      if (uTextJustify & DT_LEFT)
        xD = r->left+tx+infoPtr->xBmpMargin;
      else
      {
        xD = r->right-infoPtr->xBmpMargin-bmp.bmWidth;
        r->right = xD;
      }
    } else
    {
      if (rx >= bmp.bmWidth+2*infoPtr->xBmpMargin)
      {
        cx = bmp.bmWidth;
        xD = r->right-bmp.bmWidth-infoPtr->xBmpMargin;
        r->right = xD-infoPtr->xBmpMargin;
      } else
      {
        cx = MAX(0,rx-infoPtr->xBmpMargin);
        xD = r->left;
        r->right = r->left;
      }
    }

    hdcBitmap = CreateCompatibleDC(hdc);
    SelectObject(hdcBitmap,phdi->hbm);
    BitBlt(hdc,xD,yD,cx,cy,hdcBitmap,0,yS,SRCCOPY);
    DeleteDC (hdcBitmap);
  }
}

static VOID
HEADER_DrawItemText(HDC hdc,HEADER_INFO* infoPtr,HEADER_ITEM* phdi,RECT* r,WCHAR* pszText,UINT uTextJustify,BOOL bEraseTextBkgnd,BOOL bHotTrack)
{
  if ((phdi->fmt & HDF_STRING) && (phdi->pszText))
  {
    INT  oldBkMode = SetBkMode(hdc,TRANSPARENT);

    if ((phdi->fmt & HDF_BITMAP_ON_RIGHT) || !(phdi->fmt & HDF_IMAGE)) r->left += TEXT_MARGIN;
    if (!(phdi->fmt & HDF_BITMAP_ON_RIGHT)) r->right -= TEXT_MARGIN;
    if (r->left >= r->right) return;

    SetTextColor (hdc,GetSysColor(bHotTrack ? COLOR_HOTLIGHT:COLOR_BTNTEXT));
    if (bEraseTextBkgnd)
    {
      HBRUSH hbrBk = GetSysColorBrush(COLOR_3DFACE);

      FillRect(hdc,r,hbrBk);
    }

    DrawTextExW(hdc,pszText,lstrlenW(pszText),r,uTextJustify|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS,NULL);
    if (oldBkMode != TRANSPARENT) SetBkMode(hdc,oldBkMode);
  }
}

static INT
HEADER_DrawItem (HWND hwnd, HDC hdc, INT iItem, BOOL bHotTrack,BOOL bEraseTextBkgnd)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HEADER_ITEM *phdi = &infoPtr->items[iItem];
    RECT r;

    r = phdi->rect;
    if (r.right - r.left == 0) return phdi->rect.right;

    if (GetWindowLongA (hwnd, GWL_STYLE) & HDS_BUTTONS)
    {
      if (phdi->bDown)
      {
        DrawEdge (hdc,&r,BDR_RAISEDOUTER,BF_RECT | BF_FLAT | BF_MIDDLE | BF_ADJUST);
        r.left += 2;
        r.top  += 2;
      } else
          DrawEdge(hdc,&r,EDGE_RAISED,BF_RECT | BF_SOFT | BF_MIDDLE | BF_ADJUST);
    } else
        DrawEdge(hdc,&r,EDGE_ETCHED, BF_BOTTOM | BF_RIGHT | BF_ADJUST);

    if (r.left >= r.right) return phdi->rect.right;

    if (phdi->fmt & HDF_OWNERDRAW)
    {
      DRAWITEMSTRUCT dis;
      dis.CtlType    = ODT_HEADER;
      dis.CtlID      = GetWindowLongA (hwnd, GWL_ID);
      dis.itemID     = iItem;
      dis.itemAction = ODA_DRAWENTIRE;
      dis.itemState  = phdi->bDown ? ODS_SELECTED : 0;
      dis.hwndItem   = hwnd;
      dis.hDC        = hdc;
      dis.rcItem     = r;
      dis.itemData   = phdi->lParam;
      SendMessageA (GetParent(hwnd),WM_DRAWITEM,(WPARAM)dis.CtlID,(LPARAM)&dis);
    } else
    {
      UINT uTextJustify;
      WCHAR* pszText = phdi->pszText;

      if ((phdi->fmt & HDF_JUSTIFYMASK) == HDF_CENTER)
        uTextJustify = DT_CENTER;
      else if ((phdi->fmt & HDF_JUSTIFYMASK) == HDF_RIGHT)
        uTextJustify = DT_RIGHT;
      else uTextJustify = DT_LEFT;

      if (phdi->pszText == LPSTR_TEXTCALLBACKW)
      {
        if (IsWindowUnicode(GetParent(hwnd)))
        {
          NMHDDISPINFOW nmhdr;

          nmhdr.hdr.hwndFrom = hwnd;
          nmhdr.hdr.idFrom   = GetWindowLongW(hwnd,GWL_ID);
          nmhdr.hdr.code     = HDN_GETDISPINFOW;
          nmhdr.iItem      = iItem;
          nmhdr.mask       = HDI_TEXT;
          nmhdr.cchTextMax = phdi->cchTextMax;
          nmhdr.pszText    = COMCTL32_Alloc(phdi->cchTextMax*sizeof(WCHAR));
          if (nmhdr.pszText) nmhdr.pszText[0] = 0;
          nmhdr.lParam     = phdi->lParam;

          SendMessageW(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);

          pszText = nmhdr.pszText;
          if (nmhdr.mask & HDI_DI_SETITEM)
          {
            INT len = pszText ? lstrlenW(pszText):0;

            if (len)
            {
              phdi->pszText = COMCTL32_Alloc((len+1)*sizeof(WCHAR));
              lstrcpyW(phdi->pszText,pszText);
            } else phdi->pszText = NULL;
            COMCTL32_Free(pszText);
            pszText = phdi->pszText;
          }
        } else
        {
          NMHDDISPINFOA nmhdr;
          INT len;

          nmhdr.hdr.hwndFrom = hwnd;
          nmhdr.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
          nmhdr.hdr.code     = HDN_GETDISPINFOA;
          nmhdr.iItem      = iItem;
          nmhdr.mask       = HDI_TEXT;
          nmhdr.cchTextMax = phdi->cchTextMax;
          nmhdr.pszText    = COMCTL32_Alloc(phdi->cchTextMax);
          if (nmhdr.pszText) nmhdr.pszText[0] = 0;
          nmhdr.lParam     = phdi->lParam;

          SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);

          len = nmhdr.pszText ? lstrlenA(nmhdr.pszText):0;

          if (len)
          {
            pszText = COMCTL32_Alloc((len+1)*sizeof(WCHAR));
            lstrcpyAtoW(pszText,nmhdr.pszText);
          } else pszText = NULL;
          COMCTL32_Free(nmhdr.pszText);

          if (nmhdr.mask & HDI_DI_SETITEM)
            phdi->pszText = pszText;
        }
      }

      if (!(phdi->fmt & HDI_ORDER) || (phdi->iOrder & (HDF_LEFT | HDF_CENTER)))
      {
        HEADER_DrawItemImage(hwnd,hdc,infoPtr,phdi,&r,iItem);
        if (r.left < r.right)
        {
          HEADER_DrawItemBitmap(hdc,infoPtr,phdi,&r,pszText,uTextJustify);
          if (r.left < r.right)
            HEADER_DrawItemText(hdc,infoPtr,phdi,&r,pszText,uTextJustify,bEraseTextBkgnd,bHotTrack);
        }
      } else
      {
        HEADER_DrawItemBitmap(hdc,infoPtr,phdi,&r,pszText,uTextJustify);
        if (r.left < r.right)
        {
          HEADER_DrawItemImage(hwnd,hdc,infoPtr,phdi,&r,iItem);
          if (r.left < r.right)
            HEADER_DrawItemText(hdc,infoPtr,phdi,&r,pszText,uTextJustify,bEraseTextBkgnd,bHotTrack);
        }
      }
      if (phdi->pszText == LPSTR_TEXTCALLBACKW) COMCTL32_Free(pszText);
    }

    return phdi->rect.right;
}

static void
HEADER_DrawTrackLine (HWND hwnd, HDC hdc, INT x)
{
    RECT rect;
    HPEN hOldPen;
    INT  oldRop;

    GetClientRect (hwnd, &rect);

    rect.left = x;
    rect.right = x+1;
    InvertRect(hdc,&rect);
}

static void
HEADER_Draw(HWND hwnd,HDC hdc)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    HFONT hFont, hOldFont;
    RECT rect;
    HBRUSH hbrBk;
    INT i, x;

    /* get rect for the bar, adjusted for the border */
    GetClientRect (hwnd, &rect);

    hFont = infoPtr->hFont ? infoPtr->hFont : GetStockObject (SYSTEM_FONT);
    hOldFont = SelectObject (hdc, hFont);

    /* draw Background */
    hbrBk = GetSysColorBrush(COLOR_3DFACE);
    FillRect(hdc, &rect, hbrBk);

    x = rect.left;
    for (i = 0; i < infoPtr->uNumItem; i++)
    {
      x = HEADER_DrawItem(hwnd,hdc,i,infoPtr->iHotItem == i,FALSE);
      if (x > rect.right)
      {
        x = -1;
        break;
      }
    }

    if (x != -1 && (x <= rect.right) && (infoPtr->uNumItem > 0))
    {
        rect.left = x;
        if (dwStyle & HDS_BUTTONS)
            DrawEdge (hdc, &rect, EDGE_RAISED, BF_TOP|BF_LEFT|BF_BOTTOM|BF_SOFT);
        else
            DrawEdge (hdc, &rect, EDGE_ETCHED, BF_BOTTOM);
    }

    SelectObject (hdc, hOldFont);

    if (infoPtr->bCaptured && infoPtr->bTracking && !(dwStyle & HDS_FULLDRAG)) HEADER_DrawTrackLine(hwnd,hdc,infoPtr->xOldTrack);

    if (infoPtr->bDragDrop)
    {
      if (infoPtr->iDragItem != infoPtr->iMoveItem && infoPtr->iDragItem != infoPtr->iMoveItem+1 && infoPtr->iDragItem != -1)
      {
        INT x,y,width;
        HPEN hPen = CreatePen(PS_DOT,0,GetSysColor(COLOR_HIGHLIGHT)),hOldPen;
        INT oldBkMode = SetBkMode(hdc,TRANSPARENT);

        hOldPen = SelectObject(hdc,hPen);
        if (infoPtr->iDragItem == infoPtr->uNumItem)
        {
          x = infoPtr->items[infoPtr->uNumItem-1].rect.right-1;
          width = 2;
        } else if (infoPtr->iDragItem == 0)
        {
          x = infoPtr->items[0].rect.left;
          width = 1;
        } else
        {
          x = infoPtr->items[infoPtr->iDragItem].rect.left-2;
          width = 3;
        }

        for (y = 0;y < width;y++)
        {
          MoveToEx(hdc,x,0,NULL);
          LineTo(hdc,x,rect.bottom);
          x++;
        }
        SetBkMode(hdc,oldBkMode);
        SelectObject(hdc,hOldPen);
        DeleteObject(hPen);
      }
      ImageList_Draw(infoPtr->dragImage,0,hdc,infoPtr->items[infoPtr->iMoveItem].rect.left+infoPtr->dragPos.x-infoPtr->dragStart.x,0,ILD_NORMAL);
    }
}


static void
HEADER_RefreshItem (HWND hwnd, HDC hdc, INT iItem)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HFONT hFont, hOldFont;

    hFont = infoPtr->hFont ? infoPtr->hFont : GetStockObject (SYSTEM_FONT);
    hOldFont = SelectObject (hdc, hFont);
    HEADER_DrawItem(hwnd,hdc,iItem,infoPtr->iHotItem == iItem,TRUE);
    SelectObject (hdc, hOldFont);
}

static void
HEADER_Refresh(HWND hwnd)
{
  HDC hdc,hdcCompatible;
  HBITMAP bitmap,oldbmp;
  RECT rect;

  GetClientRect(hwnd,&rect);
  hdc = GetDC(hwnd);
  hdcCompatible = CreateCompatibleDC(hdc);
  bitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
  oldbmp = SelectObject(hdcCompatible,bitmap);
  HEADER_Draw(hwnd,hdcCompatible);
  BitBlt(hdc,0,0,rect.right,rect.bottom,hdcCompatible,0,0,SRCCOPY);
  SelectObject(hdcCompatible,oldbmp);
  DeleteObject(bitmap);
  DeleteDC(hdcCompatible);
  ReleaseDC(hwnd,hdc);
}

static void
HEADER_SetItemBounds(HWND hwnd,INT start)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
  HEADER_ITEM *phdi;
  RECT rect;
  int i, x;

  if (infoPtr->uNumItem == 0) return;

  GetClientRect(hwnd,&rect);

  x = (start > 0) ? infoPtr->items[start-1].rect.right:rect.left;
  for (i = start;i < infoPtr->uNumItem;i++)
  {
    phdi = &infoPtr->items[i];
    phdi->rect.top = rect.top;
    phdi->rect.bottom = rect.bottom;
    phdi->rect.left = x;
    phdi->rect.right = phdi->rect.left + phdi->cxy;
    x = phdi->rect.right;
  }
}

static void
HEADER_InternalHitTest (HWND hwnd, LPPOINT lpPt, UINT *pFlags, INT *pItem)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    RECT rect, rcTest;
    INT  iCount, width,widthCount = 0;
    BOOL bNoWidth;

    GetClientRect (hwnd, &rect);

    *pFlags = 0;
    bNoWidth = FALSE;
    if (PtInRect(&rect,*lpPt))
    {
      if (infoPtr->uNumItem == 0)
      {
        *pFlags |= HHT_NOWHERE;
        *pItem = 1;
//          TRACE (header, "NOWHERE\n");
        return;
      } else
      {
        /* somewhere inside */
        for (iCount = 0; iCount < infoPtr->uNumItem; iCount++)
        {
          rect = infoPtr->items[iCount].rect;
          width = rect.right - rect.left;
          if (width == 0)
          {
            bNoWidth = TRUE;
            widthCount = 0;
            continue;
          } else widthCount++;
          if (widthCount > 1) bNoWidth = FALSE;
          if (PtInRect(&rect,*lpPt))
          {
            if (width <= 2 * DIVIDER_WIDTH)
            {
              *pFlags |= HHT_ONHEADER;
              *pItem = iCount;
//                      TRACE (header, "ON HEADER %d\n", iCount);
              return;
            }
            if (iCount > 0)
            {
              rcTest = rect;
              rcTest.right = rcTest.left + DIVIDER_WIDTH;
              if (PtInRect (&rcTest, *lpPt))
              {
                if (bNoWidth)
                {
                  *pFlags |= HHT_ONDIVOPEN;
                  *pItem = iCount - 1;
//                              TRACE (header, "ON DIVOPEN %d\n", *pItem);
                  return;
                } else
                {
                  *pFlags |= HHT_ONDIVIDER;
                  *pItem = iCount - 1;
//                              TRACE (header, "ON DIVIDER %d\n", *pItem);
                  return;
                }
              }
            }
            rcTest = rect;
            rcTest.left = rcTest.right - DIVIDER_WIDTH;
            if (PtInRect (&rcTest, *lpPt))
            {
              *pFlags |= HHT_ONDIVIDER;
              *pItem = iCount;
//                      TRACE (header, "ON DIVIDER %d\n", *pItem);
              return;
            }

            *pFlags |= HHT_ONHEADER;
            *pItem = iCount;
//                  TRACE (header, "ON HEADER %d\n", iCount);
            return;
          }
        }

        /* check for last divider part (on nowhere) */
        rect = infoPtr->items[infoPtr->uNumItem-1].rect;
        rect.left = rect.right;
        rect.right += DIVIDER_WIDTH;
        if (widthCount > 0) bNoWidth = FALSE;
        if (PtInRect (&rect, *lpPt))
        {
          if (bNoWidth)
          {
            *pFlags |= HHT_ONDIVOPEN;
            *pItem = infoPtr->uNumItem - 1;
//                  TRACE (header, "ON DIVOPEN %d\n", *pItem);
            return;
          } else
          {
            *pFlags |= HHT_ONDIVIDER;
            *pItem = infoPtr->uNumItem-1;
//                  TRACE (header, "ON DIVIDER %d\n", *pItem);
            return;
          }
        }

        *pFlags |= HHT_NOWHERE;
        *pItem = 1;
//          TRACE (header, "NOWHERE\n");
        return;
      }
    } else
    {
      if (lpPt->x < rect.left)
      {
//         TRACE (header, "TO LEFT\n");
        *pFlags |= HHT_TOLEFT;
      } else if (lpPt->x > rect.right)
      {
//          TRACE (header, "TO LEFT\n");
        *pFlags |= HHT_TORIGHT;
      }

      if (lpPt->y < rect.top)
      {
//          TRACE (header, "ABOVE\n");
        *pFlags |= HHT_ABOVE;
      } else if (lpPt->y > rect.bottom)
      {
//          TRACE (header, "BELOW\n");
        *pFlags |= HHT_BELOW;
      }
    }

    *pItem = 1;
//    TRACE (header, "flags=0x%X\n", *pFlags);
    return;
}

static BOOL
HEADER_SendSimpleNotify (HWND hwnd, UINT code)
{
    NMHDR nmhdr;

    nmhdr.hwndFrom = hwnd;
    nmhdr.idFrom   = GetWindowLongA (hwnd, GWL_ID);
    nmhdr.code     = code;

    return (BOOL)SendMessageA (GetParent (hwnd), WM_NOTIFY,
                                   (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
}

static BOOL
HEADER_SendHeaderNotifyA(HWND hwnd,UINT code,INT iItem,INT iButton)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
    NMHEADERA nmhdr;
    HDITEMA nmitem;
    char* text;
    BOOL res;

    if (infoPtr->items[iItem].pszText)
    {
      INT len = lstrlenW(infoPtr->items[iItem].pszText);

      if (len > 0)
      {
        text = COMCTL32_Alloc(len+1);
        lstrcpyWtoA(text,infoPtr->items[iItem].pszText);
      } else text = NULL;
    } else text = NULL;

    nmhdr.hdr.hwndFrom = hwnd;
    nmhdr.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
    nmhdr.hdr.code = code;
    nmhdr.iItem   = iItem;
    nmhdr.iButton = 0;
    nmhdr.pitem   = &nmitem;

    nmitem.mask       = 0;
    nmitem.cxy        = infoPtr->items[iItem].cxy;
    nmitem.hbm        = infoPtr->items[iItem].hbm;
    nmitem.pszText    = text;
    nmitem.cchTextMax = infoPtr->items[iItem].cchTextMax;
    nmitem.fmt        = infoPtr->items[iItem].fmt;
    nmitem.lParam     = infoPtr->items[iItem].lParam;
    nmitem.iImage     = infoPtr->items[iItem].iImage;
    nmitem.iOrder     = infoPtr->items[iItem].iOrder;
    nmitem.type       = infoPtr->items[iItem].type;
    nmitem.pvFilter   = infoPtr->items[iItem].pvFilter;

    res = (BOOL)SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);

    if (text) COMCTL32_Free(text);

    return res;
}

static BOOL
HEADER_SendHeaderNotifyW(HWND hwnd,UINT code,INT iItem,INT iButton)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
    NMHEADERW nmhdr;
    HDITEMW nmitem;

    nmhdr.hdr.hwndFrom = hwnd;
    nmhdr.hdr.idFrom   = GetWindowLongW(hwnd,GWL_ID);
    nmhdr.hdr.code = code;
    nmhdr.iItem   = iItem;
    nmhdr.iButton = 0;
    nmhdr.pitem   = &nmitem;

    nmitem.mask       = 0;
    nmitem.cxy        = infoPtr->items[iItem].cxy;
    nmitem.hbm        = infoPtr->items[iItem].hbm;
    nmitem.pszText    = infoPtr->items[iItem].pszText;
    nmitem.cchTextMax = infoPtr->items[iItem].cchTextMax;
    nmitem.fmt        = infoPtr->items[iItem].fmt;
    nmitem.lParam     = infoPtr->items[iItem].lParam;
    nmitem.iImage     = infoPtr->items[iItem].iImage;
    nmitem.iOrder     = infoPtr->items[iItem].iOrder;
    nmitem.type       = infoPtr->items[iItem].type;
    nmitem.pvFilter   = infoPtr->items[iItem].pvFilter;

    return (BOOL)SendMessageW(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);
}

static BOOL
HEADER_SendItemChanging(HWND hwnd,INT iItem)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendHeaderNotifyA(hwnd,HDN_ITEMCHANGINGA,iItem,0);
  else
    return HEADER_SendHeaderNotifyW(hwnd,HDN_ITEMCHANGINGW,iItem,0);
}

static BOOL
HEADER_SendItemChanged(HWND hwnd,INT iItem)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendHeaderNotifyA(hwnd,HDN_ITEMCHANGEDA,iItem,0);
  else
    return HEADER_SendHeaderNotifyW(hwnd,HDN_ITEMCHANGEDW,iItem,0);
}

static BOOL
HEADER_SendItemDblClick(HWND hwnd,INT iItem,INT iButton)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendHeaderNotifyA(hwnd,HDN_ITEMDBLCLICKA,iItem,iButton);
  else
    return HEADER_SendHeaderNotifyW(hwnd,HDN_ITEMDBLCLICKW,iItem,iButton);
}

static BOOL
HEADER_SendDividerDblClick(HWND hwnd,INT iItem,INT iButton)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendHeaderNotifyA(hwnd,HDN_DIVIDERDBLCLICKA,iItem,iButton);
  else
    return HEADER_SendHeaderNotifyW(hwnd,HDN_DIVIDERDBLCLICKW,iItem,iButton);
}

static BOOL
HEADER_SendBeginTrack(HWND hwnd,INT iItem,INT iButton)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendHeaderNotifyA(hwnd,HDN_BEGINTRACKA,iItem,iButton);
  else
    return HEADER_SendHeaderNotifyW(hwnd,HDN_BEGINTRACKW,iItem,iButton);
}

static BOOL
HEADER_SendEndTrack(HWND hwnd,INT iItem,INT iButton)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendHeaderNotifyA(hwnd,HDN_ENDTRACKA,iItem,iButton);
  else
    return HEADER_SendHeaderNotifyW(hwnd,HDN_ENDTRACKW,iItem,iButton);
}

static BOOL
HEADER_SendTrack(HWND hwnd,INT iItem)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendHeaderNotifyA(hwnd,HDN_TRACKA,iItem,0);
  else
    return HEADER_SendHeaderNotifyW(hwnd,HDN_TRACKW,iItem,0);
}

static BOOL
HEADER_SendClickNotifyA(HWND hwnd,UINT code,INT iItem,INT iButton)
{
    NMHEADERA nmhdr;

    nmhdr.hdr.hwndFrom = hwnd;
    nmhdr.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
    nmhdr.hdr.code     = code;
    nmhdr.iItem        = iItem;
    nmhdr.iButton      = iButton;
    nmhdr.pitem        = NULL;

    return (BOOL)SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);
}

static BOOL
HEADER_SendClickNotifyW(HWND hwnd,UINT code,INT iItem,INT iButton)
{
    NMHEADERW nmhdr;

    nmhdr.hdr.hwndFrom = hwnd;
    nmhdr.hdr.idFrom   = GetWindowLongW(hwnd,GWL_ID);
    nmhdr.hdr.code     = code;
    nmhdr.iItem        = iItem;
    nmhdr.iButton      = iButton;
    nmhdr.pitem        = NULL;

    return (BOOL)SendMessageW(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);
}

static BOOL
HEADER_SendItemClick(HWND hwnd,INT iItem,INT iButton)
{
  if (IsWindowUnicode(GetParent(hwnd)))
    return HEADER_SendClickNotifyA(hwnd,HDN_ITEMCLICKA,iItem,iButton);
  else
    return HEADER_SendClickNotifyW(hwnd,HDN_ITEMCLICKW,iItem,iButton);
}

static BOOL
HEADER_SendDragNotify(HWND hwnd,UINT code,INT iItem,INT iButton)
{
    NMHEADERA nmhdr;

    nmhdr.hdr.hwndFrom = hwnd;
    nmhdr.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
    nmhdr.hdr.code     = code;
    nmhdr.iItem        = iItem;
    nmhdr.iButton      = iButton;
    nmhdr.pitem        = NULL;

    return (BOOL)SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);
}

static BOOL
HEADER_SendFilterBtnClick(HWND hwnd,INT iItem,RECT iRect)
{
    NMHDFILTERBTNCLICK nmhdr;

    nmhdr.hdr.hwndFrom = hwnd;
    nmhdr.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
    nmhdr.hdr.code = HDN_FILTERBTNCLICK;
    nmhdr.iItem    = iItem;
    nmhdr.rc       = iRect;

    return (BOOL)SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);
}

static BOOL
HEADER_SendFilterChange(HWND hwnd,INT iItem)
{
    NMHEADERA nmhdr;

    nmhdr.hdr.hwndFrom = hwnd;
    nmhdr.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
    nmhdr.hdr.code     = HDN_FILTERCHANGE;
    nmhdr.iItem        = iItem;
    nmhdr.iButton      = 0;
    nmhdr.pitem        = NULL;

    return (BOOL)SendMessageA(GetParent(hwnd),WM_NOTIFY,(WPARAM)nmhdr.hdr.idFrom,(LPARAM)&nmhdr);
}

static LRESULT
HEADER_ClearFilter(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  INT iItem = (INT)lParam;

  if (iItem == -1)
  {
    INT x;

    for (x = 0;x < infoPtr->uNumItem;x++)
    {
      //CB:todo
    }
  } else
  {
    if ((iItem < 0) || (iItem >= (INT)infoPtr->uNumItem)) return iItem;

    //CB:todo
  }

  HEADER_SendFilterChange(hwnd,-1);

  return iItem;
}

static LRESULT
HEADER_EditFilter(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  INT iItem = (INT)wParam;

  if ((iItem < 0) || (iItem >= (INT)infoPtr->uNumItem)) return 0;

  if (!lParam)
  {
    //CB: save changes

    HEADER_SendFilterChange(hwnd,iItem);
  }

  //CB: todo

  return iItem;
}

static LRESULT
HEADER_SetFilterChangeTimeout(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  //CB: todo

  return 0;
}

static LRESULT
HEADER_CreateDragImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  INT iItem = (INT)wParam;
  HEADER_ITEM *phdi;
  RECT oldRect;
  HDC hdc,hdcBmp;
  HBITMAP bmp,mask,oldbmp;
  HBRUSH hbr;
  HFONT hFont,hOldFont;
  HIMAGELIST himl;

  if (iItem < 0 || iItem >= (INT)infoPtr->uNumItem) return 0;

  phdi = &infoPtr->items[iItem];
  oldRect = phdi->rect;
  phdi->rect.right -= phdi->rect.left;
  phdi->rect.left = 0;

  hdc = GetDC(hwnd);
  hFont = infoPtr->hFont ? infoPtr->hFont:GetStockObject(SYSTEM_FONT);
  hOldFont = SelectObject(hdc,hFont);
  hdcBmp = CreateCompatibleDC(hdc);
  bmp = CreateCompatibleBitmap(hdc,phdi->rect.right,phdi->rect.bottom);
  mask = CreateCompatibleBitmap(hdc,phdi->rect.right,phdi->rect.bottom);
  oldbmp = SelectObject(hdcBmp,bmp);
  HEADER_DrawItem(hwnd,hdcBmp,iItem,FALSE,TRUE);
  SelectObject(hdcBmp,mask);
  hbr = CreateSolidBrush(RGB(255,255,255));
  FillRect(hdcBmp,&phdi->rect,hbr);
  DeleteObject(hbr);
  hbr = CreateHatchBrush(HS_DIAGCROSS,RGB(0,0,0));
  FillRect(hdcBmp,&phdi->rect,hbr);
  DeleteObject(hbr);
  SelectObject(hdcBmp,oldbmp);

  himl = ImageList_Create(phdi->rect.right,phdi->rect.bottom,ILC_COLOR | ILC_MASK,1,0);
  ImageList_Add(himl,bmp,mask);

  DeleteObject(bmp);
  DeleteObject(mask);
  SelectObject(hdc,hOldFont);
  DeleteDC(hdcBmp);
  ReleaseDC(hwnd,hdc);
  phdi->rect = oldRect;

  return (LRESULT)himl;
}

static LRESULT
HEADER_SetHotDivider(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if (wParam)
  {
    //CB:todo
  } else
  {
    //CB:todo
  }

  return 0;
}

static LRESULT
HEADER_DeleteItem (HWND hwnd, WPARAM wParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
    INT iItem = (INT)wParam;
    HDC hdc;

//    TRACE(header, "[iItem=%d]\n", iItem);

    if ((iItem < 0) || (iItem >= (INT)infoPtr->uNumItem)) return FALSE;

    if (infoPtr->uNumItem == 1)
    {
      //clear
      if (infoPtr->items[0].pszText) COMCTL32_Free(infoPtr->items[0].pszText);
      COMCTL32_Free (infoPtr->items);
      infoPtr->items = 0;
      infoPtr->uNumItem = 0;

      InvalidateRect(hwnd,NULL,FALSE);
    } else
    {
      HEADER_ITEM *oldItems = infoPtr->items;
//        TRACE(header, "Complex delete! [iItem=%d]\n", iItem);

      if (infoPtr->items[iItem].pszText) COMCTL32_Free(infoPtr->items[iItem].pszText);

      infoPtr->uNumItem--;
      infoPtr->items = COMCTL32_Alloc (sizeof (HEADER_ITEM) * infoPtr->uNumItem);
      /* pre delete copy */
      if (iItem > 0)
        memcpy (&infoPtr->items[0], &oldItems[0],iItem*sizeof(HEADER_ITEM));

      /* post delete copy */
      if (iItem < infoPtr->uNumItem)
        memcpy (&infoPtr->items[iItem],&oldItems[iItem+1],(infoPtr->uNumItem-iItem)*sizeof(HEADER_ITEM));

      COMCTL32_Free(oldItems);

      if (iItem < infoPtr->uNumItem) HEADER_SetItemBounds(hwnd,iItem);

      HEADER_Refresh(hwnd);

    }

    return TRUE;
}

static LRESULT
HEADER_GetBitmapMargin(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);

  return infoPtr->xBmpMargin;
}

static LRESULT
HEADER_SetBitmapMargin(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
  INT oldMargin = infoPtr->xBmpMargin;

  if (infoPtr->xBmpMargin != (INT)wParam)
  {
    infoPtr->xBmpMargin = (INT)wParam;
    HEADER_Refresh(hwnd);
  }

  return oldMargin;
}

static LRESULT
HEADER_GetImageList (HWND hwnd)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);

  return (LRESULT)infoPtr->himl;
}


static LRESULT
HEADER_GetItemA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
    HDITEMA   *phdi = (HDITEMA*)lParam;
    INT       nItem = (INT)wParam;
    HEADER_ITEM *lpItem;

    if (!phdi) return FALSE;
    if ((nItem < 0) || (nItem >= (INT)infoPtr->uNumItem)) return FALSE;

//    TRACE (header, "[nItem=%d]\n", nItem);

    if (phdi->mask == 0) return TRUE;

    lpItem = (HEADER_ITEM*)&infoPtr->items[nItem];

    if (phdi->mask & HDI_BITMAP) phdi->hbm = lpItem->hbm;

    if (phdi->mask & HDI_FORMAT) phdi->fmt = lpItem->fmt;

    if (phdi->mask & HDI_FILTER)
    {
      phdi->type = lpItem->type;
      phdi->pvFilter = lpItem->pvFilter;
    }

    if (phdi->mask & (HDI_WIDTH | HDI_HEIGHT)) phdi->cxy = lpItem->cxy;

    if (phdi->mask & HDI_IMAGE) phdi->iImage = lpItem->iImage;

    if (phdi->mask & HDI_LPARAM) phdi->lParam = lpItem->lParam;

    if (phdi->mask & HDI_ORDER) phdi->iOrder = lpItem->iOrder;

    if (phdi->mask & HDI_TEXT)
    {
      if (lpItem->pszText != LPSTR_TEXTCALLBACKW)
      {
        if (lpItem->pszText)
          lstrcpynWtoA (phdi->pszText,lpItem->pszText,phdi->cchTextMax);
        else
          phdi->pszText = NULL;
      } else phdi->pszText = LPSTR_TEXTCALLBACKA;
    }

    return TRUE;
}

static LRESULT
HEADER_GetItemW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
    HDITEMW   *phdi = (HDITEMW*)lParam;
    INT       nItem = (INT)wParam;
    HEADER_ITEM *lpItem;

    if (!phdi) return FALSE;
    if ((nItem < 0) || (nItem >= (INT)infoPtr->uNumItem)) return FALSE;

//    TRACE (header, "[nItem=%d]\n", nItem);

    if (phdi->mask == 0) return TRUE;

    lpItem = (HEADER_ITEM*)&infoPtr->items[nItem];

    if (phdi->mask & HDI_BITMAP) phdi->hbm = lpItem->hbm;

    if (phdi->mask & HDI_FORMAT) phdi->fmt = lpItem->fmt;

    if (phdi->mask & HDI_FILTER)
    {
      phdi->type = lpItem->type;
      phdi->pvFilter = lpItem->pvFilter;
    }

    if (phdi->mask & (HDI_WIDTH | HDI_HEIGHT)) phdi->cxy = lpItem->cxy;

    if (phdi->mask & HDI_IMAGE) phdi->iImage = lpItem->iImage;

    if (phdi->mask & HDI_LPARAM) phdi->lParam = lpItem->lParam;

    if (phdi->mask & HDI_ORDER) phdi->iOrder = lpItem->iOrder;

    if (phdi->mask & HDI_TEXT)
    {
      if (lpItem->pszText != LPSTR_TEXTCALLBACKW)
      {
        if (lpItem->pszText)
          lstrcpynW (phdi->pszText,lpItem->pszText,phdi->cchTextMax);
        else
          phdi->pszText = NULL;
      } else phdi->pszText = LPSTR_TEXTCALLBACKW;
    }

    return TRUE;
}


static LRESULT
HEADER_GetItemCount (HWND hwnd)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);

    return infoPtr->uNumItem;
}


static LRESULT
HEADER_GetItemRect (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    INT iItem = (INT)wParam;
    LPRECT lpRect = (LPRECT)lParam;

    if (!lpRect || (iItem < 0) || (iItem >= (INT)infoPtr->uNumItem)) return FALSE;

    lpRect->left   = infoPtr->items[iItem].rect.left;
    lpRect->right  = infoPtr->items[iItem].rect.right;
    lpRect->top    = infoPtr->items[iItem].rect.top;
    lpRect->bottom = infoPtr->items[iItem].rect.bottom;

    return TRUE;
}

static LRESULT
HEADER_GetOrderArray(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  LPINT lpiArray = (LPINT)lParam;

  if (wParam != infoPtr->uNumItem || !lpiArray) return FALSE;

  //CB: todo

  return TRUE;
}

static LRESULT
HEADER_SetOrderArray(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  LPINT lpiArray = (LPINT)lParam;

  if (wParam != infoPtr->uNumItem || !lpiArray) return FALSE;

  //CB: todo
  HEADER_SetItemBounds(hwnd,0);
  HEADER_Refresh(hwnd);

  return TRUE;
}

static LRESULT
HEADER_OrderToIndex(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  INT iOrder = (INT)wParam;
  INT x;

  for (x = 0;x < infoPtr->uNumItem;x++)
  {
//CB:todo
  }

  return iOrder;
}

static LRESULT
HEADER_GetUnicodeFormat (HWND hwnd)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);

    return infoPtr->bUnicode;
}


static LRESULT
HEADER_HitTest (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    LPHDHITTESTINFO phti = (LPHDHITTESTINFO)lParam;

    HEADER_InternalHitTest (hwnd, &phti->pt, &phti->flags, &phti->iItem);

    return phti->iItem;
}


static LRESULT
HEADER_InsertItemA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HDITEMA *phdi = (HDITEMA*)lParam;
    INT nItem = (INT)wParam;
    HEADER_ITEM *lpItem;

    if ((phdi == NULL) || (nItem < 0)) return -1;

    if (nItem > infoPtr->uNumItem) nItem = infoPtr->uNumItem;

    if (infoPtr->uNumItem == 0)
    {
      infoPtr->items = COMCTL32_Alloc(sizeof(HEADER_ITEM));
      infoPtr->uNumItem++;
    } else
    {
      HEADER_ITEM *oldItems = infoPtr->items;

      infoPtr->uNumItem++;
      infoPtr->items = COMCTL32_Alloc(sizeof(HEADER_ITEM)*infoPtr->uNumItem);
      if (nItem == 0)
        memcpy (&infoPtr->items[1],&oldItems[0],(infoPtr->uNumItem-1)*sizeof(HEADER_ITEM));
      else
      {
        /* pre insert copy */
        memcpy (&infoPtr->items[0],&oldItems[0],nItem*sizeof(HEADER_ITEM));

        /* post insert copy */
        if (nItem < infoPtr->uNumItem-1)
          memcpy (&infoPtr->items[nItem+1],&oldItems[nItem],(infoPtr->uNumItem - nItem)*sizeof(HEADER_ITEM));
      }

      COMCTL32_Free (oldItems);
    }

    lpItem = (HEADER_ITEM*)&infoPtr->items[nItem];
    lpItem->bDown = FALSE;

    lpItem->cxy = (phdi->mask & HDI_WIDTH || phdi->mask & HDI_HEIGHT) ? phdi->cxy:0;

    if (phdi->mask & HDI_TEXT)
    {
      if (!phdi->pszText) lpItem->pszText = NULL;
      else if (phdi->pszText != LPSTR_TEXTCALLBACKA)
      {
        INT len;

        len = lstrlenA(phdi->pszText);
        if (len == 0) lpItem->pszText = NULL; else
        {
          lpItem->pszText = COMCTL32_Alloc((len+1)*sizeof(WCHAR));
          lstrcpyAtoW(lpItem->pszText,phdi->pszText);
        }
      } else lpItem->pszText = LPSTR_TEXTCALLBACKW;
      lpItem->cchTextMax = phdi->cchTextMax;
    } else
    {
      lpItem->pszText = NULL;
      lpItem->cchTextMax = 0;
    }

    lpItem->fmt = (phdi->mask & HDI_FORMAT) ? phdi->fmt:HDF_LEFT;
    if (lpItem->fmt == 0) lpItem->fmt = HDF_LEFT;

    lpItem->lParam = (phdi->mask & HDI_LPARAM) ? phdi->lParam:0;

    lpItem->iImage = (phdi->mask & HDI_IMAGE) ? phdi->iImage:0;

    lpItem->iOrder = (phdi->mask & HDI_ORDER) ? phdi->iOrder:HDF_LEFT;

    lpItem->hbm = (phdi->mask & HDI_BITMAP) ? phdi->hbm:0;

    if (phdi->mask & HDI_FILTER)
    {
      lpItem->type = phdi->type;
      lpItem->pvFilter = phdi->pvFilter;
    }

    HEADER_SetItemBounds (hwnd,nItem);
    HEADER_Refresh(hwnd);

    return nItem;
}

static LRESULT
HEADER_InsertItemW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HDITEMW *phdi = (HDITEMW*)lParam;
    INT nItem = (INT)wParam;
    HEADER_ITEM *lpItem;

    if ((phdi == NULL) || (nItem < 0)) return -1;

    if (nItem > infoPtr->uNumItem) nItem = infoPtr->uNumItem;

    if (infoPtr->uNumItem == 0)
    {
      infoPtr->items = COMCTL32_Alloc(sizeof(HEADER_ITEM));
      infoPtr->uNumItem++;
    } else
    {
      HEADER_ITEM *oldItems = infoPtr->items;

      infoPtr->uNumItem++;
      infoPtr->items = COMCTL32_Alloc(sizeof(HEADER_ITEM)*infoPtr->uNumItem);
      if (nItem == 0)
        memcpy (&infoPtr->items[1],&oldItems[0],(infoPtr->uNumItem-1)*sizeof(HEADER_ITEM));
      else
      {
        /* pre insert copy */
        memcpy (&infoPtr->items[0],&oldItems[0],nItem*sizeof(HEADER_ITEM));

        /* post insert copy */
        if (nItem < infoPtr->uNumItem-1)
          memcpy (&infoPtr->items[nItem+1],&oldItems[nItem],(infoPtr->uNumItem - nItem)*sizeof(HEADER_ITEM));
      }

      COMCTL32_Free (oldItems);
    }

    lpItem = (HEADER_ITEM*)&infoPtr->items[nItem];
    lpItem->bDown = FALSE;

    lpItem->cxy = (phdi->mask & HDI_WIDTH || phdi->mask & HDI_HEIGHT) ? phdi->cxy:0;

    if (phdi->mask & HDI_TEXT)
    {
      if (!phdi->pszText) lpItem->pszText = NULL;
      else if (phdi->pszText != LPSTR_TEXTCALLBACKW)
      {
        INT len;

        len = lstrlenW(phdi->pszText);
        if (len == 0) lpItem->pszText = NULL; else
        {
          lpItem->pszText = COMCTL32_Alloc((len+1)*sizeof(WCHAR));
          lstrcpyW(lpItem->pszText,phdi->pszText);
        }
      } else lpItem->pszText = LPSTR_TEXTCALLBACKW;
      lpItem->cchTextMax = phdi->cchTextMax;
    } else
    {
      lpItem->pszText = NULL;
      lpItem->cchTextMax = 0;
    }

    lpItem->fmt = (phdi->mask & HDI_FORMAT) ? phdi->fmt:HDF_LEFT;
    if (lpItem->fmt == 0) lpItem->fmt = HDF_LEFT;

    lpItem->lParam = (phdi->mask & HDI_LPARAM) ? phdi->lParam:0;

    lpItem->iImage = (phdi->mask & HDI_IMAGE) ? phdi->iImage:0;

    lpItem->iOrder = (phdi->mask & HDI_ORDER) ? phdi->iOrder:HDF_LEFT;

    lpItem->hbm = (phdi->mask & HDI_BITMAP) ? phdi->hbm:0;

    if (phdi->mask & HDI_FILTER)
    {
      lpItem->type = phdi->type;
      lpItem->pvFilter = phdi->pvFilter;
    }

    HEADER_SetItemBounds (hwnd,nItem);
    HEADER_Refresh(hwnd);

    return nItem;
}


static LRESULT
HEADER_Layout (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    LPHDLAYOUT lpLayout = (LPHDLAYOUT)lParam;

    if (!lpLayout) return FALSE;

    lpLayout->pwpos->hwnd = hwnd;
    lpLayout->pwpos->hwndInsertAfter = 0;
    lpLayout->pwpos->x = lpLayout->prc->left;
    lpLayout->pwpos->y = lpLayout->prc->top;
    lpLayout->pwpos->cx = lpLayout->prc->right - lpLayout->prc->left;
    if (GetWindowLongA (hwnd, GWL_STYLE) & HDS_HIDDEN)
        lpLayout->pwpos->cy = 0;
    else
        lpLayout->pwpos->cy = infoPtr->nHeight;
    lpLayout->pwpos->flags = SWP_NOZORDER;

//    TRACE (header, "Layout x=%d y=%d cx=%d cy=%d\n",
//           lpLayout->pwpos->x, lpLayout->pwpos->y,
//           lpLayout->pwpos->cx, lpLayout->pwpos->cy);

    return TRUE;
}


static LRESULT
HEADER_SetImageList (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HIMAGELIST himlOld;

    himlOld = infoPtr->himl;
    infoPtr->himl = (HIMAGELIST)lParam;

    HEADER_Refresh(hwnd);

    return (LRESULT)himlOld;
}


static LRESULT
HEADER_SetItemA (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HDITEMA *phdi = (HDITEMA*)lParam;
    INT nItem = (INT)wParam;
    HEADER_ITEM *lpItem;
    HDC hdc;

    if (phdi == NULL) return FALSE;
    if ((nItem < 0) || (nItem >= (INT)infoPtr->uNumItem)) return FALSE;
    if (phdi->mask == 0) return TRUE;

//    TRACE (header, "[nItem=%d]\n", nItem);
    if (HEADER_SendItemChanging(hwnd,nItem)) return FALSE;

    lpItem = (HEADER_ITEM*)&infoPtr->items[nItem];

    if (phdi->mask & HDI_BITMAP) lpItem->hbm = phdi->hbm;

    if (phdi->mask & HDI_FORMAT) lpItem->fmt = phdi->fmt;

    if (phdi->mask & HDI_FILTER)
    {
      lpItem->type = lpItem->type;
      lpItem->pvFilter = lpItem->pvFilter;
    }

    if (phdi->mask & (HDI_WIDTH | HDI_WIDTH)) lpItem->cxy = phdi->cxy;

    if (phdi->mask & HDI_IMAGE) lpItem->iImage = phdi->iImage;

    if (phdi->mask & HDI_LPARAM) lpItem->lParam = phdi->lParam;

    if (phdi->mask & HDI_ORDER) lpItem->iOrder = phdi->iOrder;

    if (phdi->mask & HDI_TEXT)
    {
      if (phdi->pszText != LPSTR_TEXTCALLBACKA)
      {
        if (lpItem->pszText)
        {
          COMCTL32_Free (lpItem->pszText);
          lpItem->pszText = NULL;
        }
        if (phdi->pszText)
        {
          INT len = lstrlenA (phdi->pszText);

          if (len)
          {
            lpItem->pszText = COMCTL32_Alloc((len+1)*sizeof(WCHAR));
            lstrcpyAtoW(lpItem->pszText,phdi->pszText);
          }
        }
      } else
      {
        if (lpItem->pszText != LPSTR_TEXTCALLBACKW) COMCTL32_Free(lpItem->pszText);
        lpItem->pszText = LPSTR_TEXTCALLBACKW;
      }
    }

    HEADER_SendItemChanged(hwnd,nItem);

    HEADER_SetItemBounds(hwnd,0);
    HEADER_Refresh(hwnd);

    return TRUE;
}


static LRESULT
HEADER_SetItemW (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HDITEMW *phdi = (HDITEMW*)lParam;
    INT nItem = (INT)wParam;
    HEADER_ITEM *lpItem;
    HDC hdc;

    if (phdi == NULL) return FALSE;
    if ((nItem < 0) || (nItem >= (INT)infoPtr->uNumItem)) return FALSE;
    if (phdi->mask == 0) return TRUE;

//    TRACE (header, "[nItem=%d]\n", nItem);

    if (HEADER_SendItemChanging(hwnd,nItem)) return FALSE;

    lpItem = (HEADER_ITEM*)&infoPtr->items[nItem];

    if (phdi->mask & HDI_BITMAP) lpItem->hbm = phdi->hbm;

    if (phdi->mask & HDI_FORMAT) lpItem->fmt = phdi->fmt;

    if (phdi->mask & HDI_FILTER)
    {
      lpItem->type = lpItem->type;
      lpItem->pvFilter = lpItem->pvFilter;
    }

    if (phdi->mask & (HDI_WIDTH | HDI_WIDTH)) lpItem->cxy = phdi->cxy;

    if (phdi->mask & HDI_IMAGE) lpItem->iImage = phdi->iImage;

    if (phdi->mask & HDI_LPARAM) lpItem->lParam = phdi->lParam;

    if (phdi->mask & HDI_ORDER) lpItem->iOrder = phdi->iOrder;

    if (phdi->mask & HDI_TEXT)
    {
      if (phdi->pszText != LPSTR_TEXTCALLBACKW)
      {
        if (lpItem->pszText)
        {
          COMCTL32_Free(lpItem->pszText);
          lpItem->pszText = NULL;
        }
        if (phdi->pszText)
        {
          INT len = lstrlenW(phdi->pszText);

          if (len)
          {
            lpItem->pszText = COMCTL32_Alloc((len+1)*sizeof(WCHAR));
            lstrcpyW(lpItem->pszText,phdi->pszText);
          }
        }
      } else
      {
        if (lpItem->pszText != LPSTR_TEXTCALLBACKW) COMCTL32_Free(lpItem->pszText);
        lpItem->pszText = LPSTR_TEXTCALLBACKW;
      }
    }

    HEADER_SendItemChanged(hwnd,nItem);

    HEADER_SetItemBounds(hwnd,0);
    HEADER_Refresh(hwnd);

    return TRUE;
}

static LRESULT
HEADER_SetUnicodeFormat (HWND hwnd, WPARAM wParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
    BOOL bTemp = infoPtr->bUnicode;

    infoPtr->bUnicode = (BOOL)wParam;

    return bTemp;
}


static LRESULT
HEADER_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr;
    TEXTMETRICA tm;
    HFONT hOldFont;
    HDC   hdc;

    infoPtr = (HEADER_INFO *)COMCTL32_Alloc (sizeof(HEADER_INFO));
    SetWindowLongA (hwnd, 0, (DWORD)infoPtr);

    infoPtr->uNumItem = 0;
    infoPtr->hFont = 0;
    infoPtr->items = 0;
    infoPtr->hcurArrow = LoadCursorA (0, IDC_ARROWA);
    infoPtr->hcurDivider = LoadCursorA (0, IDC_SIZEWEA);
    infoPtr->hcurDivopen = LoadCursorA (0, IDC_SIZENSA);
    infoPtr->bCaptured = FALSE;
    infoPtr->bPressed  = FALSE;
    infoPtr->bTracking = FALSE;
    infoPtr->bDragDrop = FALSE;
    infoPtr->bTimer    = FALSE;
    infoPtr->iMoveItem = 0;
    infoPtr->himl      = 0;
    infoPtr->dragImage = 0;
    infoPtr->iHotItem  = -1;
    infoPtr->xBmpMargin = 3*GetSystemMetrics(SM_CXEDGE);
    infoPtr->bUnicode = IsWindowUnicode(hwnd);

    hdc = GetDC (0);
    hOldFont = SelectObject (hdc, GetStockObject (SYSTEM_FONT));
    GetTextMetricsA (hdc, &tm);
    infoPtr->nHeight = tm.tmHeight + VERT_BORDER;
    SelectObject (hdc, hOldFont);
    ReleaseDC (0, hdc);

    return 0;
}


static LRESULT
HEADER_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    HEADER_ITEM *lpItem;
    INT nItem;

    if (infoPtr->items) {
        lpItem = (HEADER_ITEM*)infoPtr->items;
        for (nItem = 0; nItem < infoPtr->uNumItem; nItem++, lpItem++) {
            if ((lpItem->pszText) && (lpItem->pszText != LPSTR_TEXTCALLBACKW))
                COMCTL32_Free (lpItem->pszText);
        }
        COMCTL32_Free (infoPtr->items);
    }

    if (infoPtr->himl)
        ImageList_Destroy (infoPtr->himl);

    COMCTL32_Free (infoPtr);

    return 0;
}

static LRESULT
HEADER_EraseBackground(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return TRUE;
}

static LRESULT
HEADER_GetDlgCode(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return DLGC_WANTTAB | DLGC_WANTARROWS;
}

static LRESULT
HEADER_GetFont (HWND hwnd)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);

    return (LRESULT)infoPtr->hFont;
}


static LRESULT
HEADER_LButtonDblClk (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    UINT  flags;
    INT   nItem;

    pt.x = (INT)LOWORD(lParam);
    pt.y = (INT)HIWORD(lParam);
    HEADER_InternalHitTest (hwnd, &pt, &flags, &nItem);

    if ((GetWindowLongA (hwnd, GWL_STYLE) & HDS_BUTTONS) && (flags == HHT_ONHEADER))
        HEADER_SendItemDblClick(hwnd,nItem,0);
    else if ((flags == HHT_ONDIVIDER) || (flags == HHT_ONDIVOPEN))
        HEADER_SendDividerDblClick(hwnd,nItem,0);

    return 0;
}


static LRESULT
HEADER_LButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    POINT pt;
    UINT  flags;
    INT   nItem;
    HDC   hdc;

    pt.x = (SHORT)LOWORD(lParam);
    pt.y = (SHORT)HIWORD(lParam);
    HEADER_InternalHitTest (hwnd, &pt, &flags, &nItem);

    if ((dwStyle & HDS_BUTTONS) && (flags == HHT_ONHEADER))
    {
        SetCapture (hwnd);
        infoPtr->bCaptured = TRUE;
        infoPtr->bPressed  = TRUE;
        infoPtr->iMoveItem = nItem;
        infoPtr->dragStart = pt;
        infoPtr->iDragItem = nItem;

        infoPtr->items[nItem].bDown = TRUE;

        /* Send WM_CUSTOMDRAW */
        hdc = GetDC (hwnd);
        HEADER_RefreshItem (hwnd, hdc, nItem);
        ReleaseDC (hwnd, hdc);

//      TRACE (header, "Pressed item %d!\n", nItem);
    } else if ((flags == HHT_ONDIVIDER) || (flags == HHT_ONDIVOPEN))
    {
        if (!(HEADER_SendBeginTrack(hwnd,nItem,0)))
        {
            SetCapture (hwnd);
            infoPtr->bCaptured  = TRUE;
            infoPtr->bTracking  = TRUE;
            infoPtr->bTrackOpen = flags == HHT_ONDIVOPEN;
            infoPtr->iMoveItem  = nItem;
            infoPtr->nOldWidth  = infoPtr->items[nItem].cxy;
            infoPtr->xTrackOffset = infoPtr->items[nItem].rect.right - pt.x;

            if (!(dwStyle & HDS_FULLDRAG))
            {
                infoPtr->xOldTrack = infoPtr->items[nItem].rect.right;
                hdc = GetDC (hwnd);
                HEADER_DrawTrackLine (hwnd, hdc, infoPtr->xOldTrack);
                ReleaseDC (hwnd, hdc);
            }

//          TRACE (header, "Begin tracking item %d!\n", nItem);
        }
    }

    return 0;
}


static LRESULT
HEADER_LButtonUp (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    POINT pt;
    UINT  flags;
    INT   nItem, nWidth;
    HDC   hdc;

    pt.x = (SHORT)LOWORD(lParam);
    pt.y = (SHORT)HIWORD(lParam);
    HEADER_InternalHitTest (hwnd, &pt, &flags, &nItem);

    if (infoPtr->bPressed)
    {
      if (infoPtr->bDragDrop)
      {
        infoPtr->items[infoPtr->iMoveItem].bDown = FALSE;
        infoPtr->bDragDrop = FALSE;
        ImageList_Destroy(infoPtr->dragImage);
        infoPtr->dragImage = NULL;

        if (!HEADER_SendDragNotify(hwnd,HDN_ENDDRAG,infoPtr->iMoveItem,0) && infoPtr->iDragItem != infoPtr->iMoveItem && infoPtr->iDragItem != infoPtr->iMoveItem+1 && infoPtr->iDragItem != -1)
        {
          HEADER_ITEM tmpItem = infoPtr->items[infoPtr->iMoveItem];;

          //CB: notificatons??

          if (infoPtr->iDragItem > infoPtr->iMoveItem)
          {
            INT x ;

            for (x = infoPtr->iMoveItem;x < infoPtr->iDragItem-1;x++)
              memcpy(&infoPtr->items[x],&infoPtr->items[x+1],sizeof(HEADER_ITEM));

            infoPtr->items[infoPtr->iDragItem-1] = tmpItem;
          } else
          {
            INT x;

            for (x = infoPtr->iMoveItem;x > infoPtr->iDragItem;x--)
              memcpy(&infoPtr->items[x],&infoPtr->items[x-1],sizeof(HEADER_ITEM));

            infoPtr->items[infoPtr->iDragItem] = tmpItem;
          }

          HEADER_SetItemBounds(hwnd,0);
        }
        HEADER_Refresh(hwnd);
      } else if (PtInRect(&infoPtr->items[infoPtr->iMoveItem].rect,pt))
      {
        infoPtr->items[infoPtr->iMoveItem].bDown = FALSE;
        hdc = GetDC(hwnd);
        HEADER_RefreshItem(hwnd,hdc,infoPtr->iMoveItem);
        ReleaseDC(hwnd,hdc);

        HEADER_SendItemClick(hwnd,infoPtr->iMoveItem,0);
      }
//      TRACE (header, "Released item %d!\n", infoPtr->iMoveItem);
      infoPtr->bPressed = FALSE;
    } else if (infoPtr->bTracking)
    {
//      TRACE (header, "End tracking item %d!\n", infoPtr->iMoveItem);
      infoPtr->bTracking = FALSE;

      HEADER_SendEndTrack(hwnd,infoPtr->iMoveItem,0);

      if (!(dwStyle & HDS_FULLDRAG))
      {
        hdc = GetDC (hwnd);
        HEADER_DrawTrackLine (hwnd, hdc, infoPtr->xOldTrack);
        ReleaseDC (hwnd, hdc);
        if (HEADER_SendItemChanging(hwnd,infoPtr->iMoveItem))
          infoPtr->items[infoPtr->iMoveItem].cxy = infoPtr->nOldWidth;
        else
        {
          nWidth = pt.x - infoPtr->items[infoPtr->iMoveItem].rect.left + infoPtr->xTrackOffset;
          if (nWidth < MIN_ITEMWIDTH) nWidth = MIN_ITEMWIDTH;

          if (infoPtr->nOldWidth != nWidth)
          {
            infoPtr->items[infoPtr->iMoveItem].cxy = nWidth;
            HEADER_SendItemChanged(hwnd,infoPtr->iMoveItem);

            HEADER_SetItemBounds (hwnd,infoPtr->iMoveItem);

            HEADER_Refresh(hwnd);
          }
        }
      }
    }

    if (infoPtr->bCaptured)
    {
      infoPtr->bCaptured = FALSE;
      ReleaseCapture();
      HEADER_SendSimpleNotify(hwnd,NM_RELEASEDCAPTURE);
    }

    return 0;
}

static INT
HEADER_DragHitTest(HWND hwnd,HEADER_INFO* infoPtr,POINT pt)
{
  RECT rect;
  INT x,xDrag;

  GetClientRect(hwnd,&rect);
  if (pt.y < rect.top || pt.y > rect.bottom) return -1;

  xDrag = infoPtr->items[infoPtr->iMoveItem].rect.left+pt.x-infoPtr->dragStart.x;

  if (xDrag < infoPtr->items[0].rect.left) return 0;

  for (x = 0;x < infoPtr->uNumItem;x++)
  {
    if (xDrag >= infoPtr->items[x].rect.left && xDrag <= infoPtr->items[x].rect.right) return x;
  }

  return infoPtr->uNumItem;
}

static LRESULT
HEADER_MouseMove (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
    POINT pt;
    UINT  flags;
    INT   nItem, nWidth;
    HDC   hdc;

    pt.x = (SHORT)LOWORD(lParam);
    pt.y = (SHORT)HIWORD(lParam);
    HEADER_InternalHitTest (hwnd, &pt, &flags, &nItem);

    if (dwStyle & HDS_BUTTONS && dwStyle & HDS_HOTTRACK)
    {
        INT newItem;

        if (infoPtr->bCaptured && infoPtr->bPressed && PtInRect(&infoPtr->items[infoPtr->iMoveItem].rect,pt))
          newItem = infoPtr->iMoveItem;
        else
        {
          if (flags & (HHT_ONHEADER | HHT_ONDIVIDER | HHT_ONDIVOPEN))
              newItem = nItem;
          else
              newItem = -1;
        }
        if (newItem != infoPtr->iHotItem)
        {
          infoPtr->iHotItem = newItem;
          HEADER_Refresh(hwnd);
        }
        if (!infoPtr->bTimer && infoPtr->iHotItem != -1)
        {
          infoPtr->bTimer = TRUE;
          SetTimer(hwnd,TIMER_ID,TIMER_MS,NULL);
        } else if (infoPtr->bTimer && infoPtr->iHotItem == -1)
        {
          KillTimer(hwnd,TIMER_ID);
          infoPtr->bTimer = FALSE;
        }
    }

    if (infoPtr->bCaptured)
    {
      if (infoPtr->bPressed)
      {
        if (dwStyle & HDS_DRAGDROP && infoPtr->uNumItem > 1 && (infoPtr->bDragDrop || !HEADER_SendDragNotify(hwnd,HDN_BEGINDRAG,infoPtr->iMoveItem,0)))
        {
          if (!infoPtr->bDragDrop)
          {
            infoPtr->bDragDrop = TRUE;
            infoPtr->dragImage = (HIMAGELIST)HEADER_CreateDragImage(hwnd,infoPtr->iMoveItem,0);
          }
          infoPtr->dragPos   = pt;
          infoPtr->iDragItem = HEADER_DragHitTest(hwnd,infoPtr,pt);
          HEADER_Refresh(hwnd);
        } else
        {
          BOOL newDown;

          if (PtInRect(&infoPtr->items[infoPtr->iMoveItem].rect,pt))
            newDown = TRUE;
          else
            newDown = FALSE;

          if (newDown != infoPtr->items[infoPtr->iMoveItem].bDown)
          {
            infoPtr->items[infoPtr->iMoveItem].bDown = newDown;
            hdc = GetDC (hwnd);
            HEADER_RefreshItem (hwnd, hdc, infoPtr->iMoveItem);
            ReleaseDC (hwnd, hdc);
          }
        }
//          TRACE (header, "Moving pressed item %d!\n", infoPtr->iMoveItem);
      } else if (infoPtr->bTracking)
      {
        if (dwStyle & HDS_FULLDRAG)
        {
          if (HEADER_SendItemChanging(hwnd,infoPtr->iMoveItem))
            infoPtr->items[infoPtr->iMoveItem].cxy = infoPtr->nOldWidth;
          else
          {
            nWidth = pt.x - infoPtr->items[infoPtr->iMoveItem].rect.left + infoPtr->xTrackOffset;
            if (nWidth < MIN_ITEMWIDTH) nWidth = MIN_ITEMWIDTH;
            infoPtr->items[infoPtr->iMoveItem].cxy = nWidth;
            HEADER_SendItemChanged(hwnd,infoPtr->iMoveItem);
          }
          HEADER_SetItemBounds (hwnd,infoPtr->iMoveItem);
          HEADER_Refresh(hwnd);
        } else
        {
          INT lastPos = infoPtr->xOldTrack;

          hdc = GetDC (hwnd);
          infoPtr->xOldTrack = pt.x + infoPtr->xTrackOffset;
          if (infoPtr->xOldTrack < infoPtr->items[infoPtr->iMoveItem].rect.left+MIN_ITEMWIDTH)
            infoPtr->xOldTrack = infoPtr->items[infoPtr->iMoveItem].rect.left+MIN_ITEMWIDTH;
          infoPtr->items[infoPtr->iMoveItem].cxy =
            infoPtr->xOldTrack-infoPtr->items[infoPtr->iMoveItem].rect.left;
          if (lastPos != infoPtr->xOldTrack)
          {
            HEADER_DrawTrackLine(hwnd,hdc,lastPos);
            HEADER_DrawTrackLine(hwnd,hdc,infoPtr->xOldTrack);
          }
          ReleaseDC (hwnd, hdc);
        }

        HEADER_SendTrack(hwnd,infoPtr->iMoveItem);
//          TRACE (header, "Tracking item %d!\n", infoPtr->iMoveItem);
      }
    }

    return 0;
}

static LRESULT
HEADER_Size(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_SetItemBounds(hwnd,0);

  return DefWindowProcA(hwnd,WM_SIZE,wParam,lParam);
}

static LRESULT
HEADER_Timer(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  POINT point;
  RECT rect;

  GetCursorPos(&point);
  ScreenToClient(hwnd,&point);
  GetClientRect(hwnd,&rect);
  if (!PtInRect(&rect,point))
  {
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);

    infoPtr->iHotItem = -1;
    infoPtr->bTimer = FALSE;
    HEADER_Refresh(hwnd);

    KillTimer(hwnd,TIMER_ID);
  }

  return 0;
}

static LRESULT
HEADER_KeyDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  HDC hdc;

  if (wParam == VK_ESCAPE && infoPtr->bCaptured)
  {
    if (infoPtr->bPressed && infoPtr->bDragDrop)
    {
      infoPtr->items[infoPtr->iMoveItem].bDown = FALSE;
      infoPtr->bPressed = FALSE;
      infoPtr->bDragDrop = FALSE;
      ImageList_Destroy(infoPtr->dragImage);
      infoPtr->dragImage = NULL;

      HEADER_SendDragNotify(hwnd,HDN_ENDDRAG,infoPtr->iMoveItem,0);
      HEADER_Refresh(hwnd);

    } else if (infoPtr->bTracking)
    {
      infoPtr->bTracking = FALSE;

      HEADER_SendEndTrack(hwnd,infoPtr->iMoveItem,0);

      if (dwStyle & HDS_FULLDRAG)
      {
        infoPtr->items[infoPtr->iMoveItem].cxy = infoPtr->nOldWidth;
        HEADER_Refresh(hwnd);
      } else
      {
        hdc = GetDC(hwnd);
        HEADER_DrawTrackLine(hwnd,hdc,infoPtr->xOldTrack);
        ReleaseDC(hwnd,hdc);
      }
    }

    infoPtr->bCaptured = FALSE;
    ReleaseCapture();
    HEADER_SendSimpleNotify(hwnd,NM_RELEASEDCAPTURE);

    return TRUE;
  }

  return DefWindowProcA(WM_KEYDOWN,hwnd,wParam,lParam);
}

static LRESULT
HEADER_CaptureChanged(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HEADER_INFO *infoPtr = HEADER_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  HDC hdc;

  if (infoPtr->bCaptured && infoPtr->bPressed)
  {
    infoPtr->items[infoPtr->iMoveItem].bDown = FALSE;
    infoPtr->bPressed  = FALSE;
    if (infoPtr->bDragDrop)
    {
      infoPtr->bDragDrop = FALSE;
      ImageList_Destroy(infoPtr->dragImage);
      infoPtr->dragImage = NULL;

      HEADER_SendDragNotify(hwnd,HDN_ENDDRAG,infoPtr->iMoveItem,0);
      HEADER_Refresh(hwnd);
    } else
    {
      hdc = GetDC(hwnd);
      HEADER_RefreshItem(hwnd,hdc,infoPtr->iMoveItem);
      ReleaseDC(hwnd,hdc);
    }
  } else if (infoPtr->bCaptured && infoPtr->bTracking)
  {
    infoPtr->bTracking = FALSE;

    HEADER_SendEndTrack(hwnd,infoPtr->iMoveItem,0);

    if (dwStyle & HDS_FULLDRAG)
    {
      infoPtr->items[infoPtr->iMoveItem].cxy = infoPtr->nOldWidth;
      HEADER_Refresh(hwnd);
    } else
    {
      hdc = GetDC(hwnd);
      HEADER_DrawTrackLine(hwnd,hdc,infoPtr->xOldTrack);
      ReleaseDC(hwnd,hdc);
    }
  }

  infoPtr->bCaptured = FALSE;

  return 0;
}

static LRESULT
HEADER_Paint (HWND hwnd, WPARAM wParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = wParam==0 ? BeginPaint (hwnd, &ps) : (HDC)wParam;
    HEADER_Draw (hwnd, hdc);
    if(!wParam)
        EndPaint (hwnd, &ps);

    return 0;
}


static LRESULT
HEADER_RButtonUp (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  HEADER_SendSimpleNotify(hwnd,NM_RCLICK);

  return DefWindowProcA(hwnd,WM_RBUTTONUP,wParam,lParam);
}


static LRESULT
HEADER_SetCursor (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    POINT pt;
    UINT  flags;
    INT   nItem;

//    TRACE (header, "code=0x%X  id=0x%X\n", LOWORD(lParam), HIWORD(lParam));

    GetCursorPos (&pt);
    ScreenToClient (hwnd, &pt);

    HEADER_InternalHitTest (hwnd, &pt, &flags, &nItem);

    if (infoPtr->bCaptured)
    {
      if (infoPtr->bTracking)
      {
        if (infoPtr->bTrackOpen) SetCursor(infoPtr->hcurDivopen);
        else SetCursor(infoPtr->hcurDivider);
      } else SetCursor(infoPtr->hcurArrow);

      return 0;
    }

    if (flags == HHT_ONDIVIDER)
        SetCursor (infoPtr->hcurDivider);
    else if (flags == HHT_ONDIVOPEN)
        SetCursor (infoPtr->hcurDivopen);
    else
        SetCursor (infoPtr->hcurArrow);

    return 0;
}


static LRESULT
HEADER_SetFont (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HEADER_INFO *infoPtr = HEADER_GetInfoPtr (hwnd);
    TEXTMETRICA tm;
    HFONT hFont, hOldFont;
    HDC hdc;

    infoPtr->hFont = (HFONT)wParam;

    hFont = infoPtr->hFont ? infoPtr->hFont : GetStockObject (SYSTEM_FONT);

    hdc = GetDC (0);
    hOldFont = SelectObject (hdc, hFont);
    GetTextMetricsA (hdc, &tm);
    infoPtr->nHeight = tm.tmHeight + VERT_BORDER;
    SelectObject (hdc, hOldFont);
    ReleaseDC (0, hdc);

    if (lParam) HEADER_Refresh(hwnd);

    return 0;
}


static LRESULT WINAPI
HEADER_WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case HDM_CLEARFILTER:
            return HEADER_ClearFilter(hwnd,wParam,lParam);

        case HDM_CREATEDRAGIMAGE:
            return HEADER_CreateDragImage (hwnd,wParam,lParam);

        case HDM_DELETEITEM:
            return HEADER_DeleteItem (hwnd, wParam);

        case HDM_EDITFILTER:
            return HEADER_EditFilter(hwnd,wParam,lParam);

        case HDM_GETBITMAPMARGIN:
            return HEADER_GetBitmapMargin(hwnd,wParam,lParam);

        case HDM_GETIMAGELIST:
            return HEADER_GetImageList (hwnd);

        case HDM_GETITEMA:
            return HEADER_GetItemA (hwnd, wParam, lParam);

        case HDM_GETITEMW:
            return HEADER_GetItemW (hwnd, wParam, lParam);

        case HDM_GETITEMCOUNT:
            return HEADER_GetItemCount (hwnd);

        case HDM_GETITEMRECT:
            return HEADER_GetItemRect (hwnd, wParam, lParam);

        case HDM_GETORDERARRAY:
            return HEADER_GetOrderArray(hwnd,wParam,lParam);

        case HDM_GETUNICODEFORMAT:
            return HEADER_GetUnicodeFormat (hwnd);

        case HDM_HITTEST:
            return HEADER_HitTest (hwnd, wParam, lParam);

        case HDM_INSERTITEMA:
            return HEADER_InsertItemA (hwnd, wParam, lParam);

        case HDM_INSERTITEMW:
            return HEADER_InsertItemW (hwnd, wParam, lParam);

        case HDM_LAYOUT:
            return HEADER_Layout (hwnd, wParam, lParam);

        case HDM_ORDERTOINDEX:
            return HEADER_OrderToIndex(hwnd,wParam,lParam);

        case HDM_SETBITMAPMARGIN:
            return HEADER_SetBitmapMargin(hwnd,wParam,lParam);

        case HDM_SETFILTERCHANGETIMEOUT:
            return HEADER_SetFilterChangeTimeout(hwnd,wParam,lParam);

        case HDM_SETHOTDIVIDER:
            return HEADER_SetHotDivider(hwnd,wParam,lParam);

        case HDM_SETIMAGELIST:
            return HEADER_SetImageList (hwnd, wParam, lParam);

        case HDM_SETITEMA:
            return HEADER_SetItemA (hwnd, wParam, lParam);

        case HDM_SETITEMW:
            return HEADER_SetItemW (hwnd, wParam, lParam);

        case HDM_SETORDERARRAY:
            return HEADER_SetOrderArray(hwnd,wParam,lParam);

        case HDM_SETUNICODEFORMAT:
            return HEADER_SetUnicodeFormat (hwnd, wParam);

        case WM_CAPTURECHANGED:
            return HEADER_CaptureChanged(hwnd,wParam,lParam);

        case WM_CREATE:
            return HEADER_Create (hwnd, wParam, lParam);

        case WM_DESTROY:
            return HEADER_Destroy (hwnd, wParam, lParam);

        case WM_ERASEBKGND:
            return HEADER_EraseBackground(hwnd,wParam,lParam);

        case WM_GETDLGCODE:
            return HEADER_GetDlgCode(hwnd,wParam,lParam);

        case WM_GETFONT:
            return HEADER_GetFont (hwnd);

        case WM_KEYDOWN:
            return HEADER_KeyDown(hwnd,wParam,lParam);

        case WM_LBUTTONDBLCLK:
            return HEADER_LButtonDblClk (hwnd, wParam, lParam);

        case WM_LBUTTONDOWN:
            return HEADER_LButtonDown (hwnd, wParam, lParam);

        case WM_LBUTTONUP:
            return HEADER_LButtonUp (hwnd, wParam, lParam);

        case WM_MOUSEMOVE:
            return HEADER_MouseMove (hwnd, wParam, lParam);

/*      case WM_NOTIFYFORMAT: */

        case WM_PAINT:
            return HEADER_Paint(hwnd,wParam);

        case WM_RBUTTONUP:
            return HEADER_RButtonUp (hwnd, wParam, lParam);

        case WM_SETCURSOR:
            return HEADER_SetCursor (hwnd, wParam, lParam);

        case WM_SETFONT:
            return HEADER_SetFont (hwnd, wParam, lParam);

        case WM_SIZE:
            return HEADER_Size(hwnd,wParam,lParam);

        case WM_TIMER:
            return HEADER_Timer(hwnd,wParam,lParam);

        default:
//            if (msg >= WM_USER)
//              ERR (header, "unknown msg %04x wp=%04x lp=%08lx\n",
//                   msg, wParam, lParam );
            return DefWindowProcA (hwnd, msg, wParam, lParam);
    }
    return 0;
}


VOID
HEADER_Register (VOID)
{
    WNDCLASSA wndClass;

    if (GlobalFindAtomA (WC_HEADERA)) return;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)HEADER_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(HEADER_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.lpszClassName = WC_HEADERA;

    RegisterClassA (&wndClass);
}


VOID
HEADER_Unregister (VOID)
{
    if (GlobalFindAtomA (WC_HEADERA))
        UnregisterClassA (WC_HEADERA, (HINSTANCE)NULL);
}

