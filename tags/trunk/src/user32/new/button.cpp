/* File: button.c -- Button type widgets
 *
 * Copyright (c) 1999 Christoph Bratschi (ported from WINE)
 *
 * Copyright (C) 1993 Johannes Ruscheinski
 * Copyright (C) 1993 David Metcalfe
 * Copyright (C) 1994 Alexandre Julliard
 */

/* CB: todo
   - window words not right implemented
   - update checkboxes.bmp to Win9x style
   - LoadBitmapA() doesn't work with native resources
*/

#include <string.h>
#include <stdlib.h>

#include "winuser.h"
#include "winbase.h"
#include "controls.h"
#include "button.h"

#define BUTTONCLASSNAME "Button"

//Prototypes

static void PaintGrayOnGray(HDC hDC,HFONT hFont,RECT *rc,char *text,UINT format);

static void PB_Paint(HWND hwnd,HDC hDC,WORD action);
static void CB_Paint(HWND hwnd,HDC hDC,WORD action);
static void GB_Paint(HWND hwnd,HDC hDC,WORD action);
static void UB_Paint(HWND hwnd,HDC hDC,WORD action);
static void OB_Paint(HWND hwnd,HDC hDC,WORD action);
static void BUTTON_CheckAutoRadioButton(HWND hwnd);

#define MAX_BTN_TYPE  12

static const WORD maxCheckState[MAX_BTN_TYPE] =
{
    BUTTON_UNCHECKED,   /* BS_PUSHBUTTON */
    BUTTON_UNCHECKED,   /* BS_DEFPUSHBUTTON */
    BUTTON_CHECKED,     /* BS_CHECKBOX */
    BUTTON_CHECKED,     /* BS_AUTOCHECKBOX */
    BUTTON_CHECKED,     /* BS_RADIOBUTTON */
    BUTTON_3STATE,      /* BS_3STATE */
    BUTTON_3STATE,      /* BS_AUTO3STATE */
    BUTTON_UNCHECKED,   /* BS_GROUPBOX */
    BUTTON_UNCHECKED,   /* BS_USERBUTTON */
    BUTTON_CHECKED,     /* BS_AUTORADIOBUTTON */
    BUTTON_UNCHECKED,   /* Not defined */
    BUTTON_UNCHECKED    /* BS_OWNERDRAW */
};

typedef void (*pfPaint)(HWND hwnd,HDC hdc,WORD action);

static const pfPaint btnPaintFunc[MAX_BTN_TYPE] =
{
    PB_Paint,    /* BS_PUSHBUTTON */
    PB_Paint,    /* BS_DEFPUSHBUTTON */
    CB_Paint,    /* BS_CHECKBOX */
    CB_Paint,    /* BS_AUTOCHECKBOX */
    CB_Paint,    /* BS_RADIOBUTTON */
    CB_Paint,    /* BS_3STATE */
    CB_Paint,    /* BS_AUTO3STATE */
    GB_Paint,    /* BS_GROUPBOX */
    UB_Paint,    /* BS_USERBUTTON */
    CB_Paint,    /* BS_AUTORADIOBUTTON */
    NULL,        /* Not defined */
    OB_Paint     /* BS_OWNERDRAW */
};

#define PAINT_BUTTON(hwnd,style,action) \
     if (btnPaintFunc[style]) { \
         HDC hdc = GetDC(hwnd); \
         (btnPaintFunc[style])(hwnd,hdc,action); \
         ReleaseDC(hwnd,hdc); }

#define BUTTON_SEND_CTLCOLOR(hwnd,hdc) \
    SendMessageA( GetParent(hwnd), WM_CTLCOLORBTN, \
                    (hdc),hwnd)

static HBITMAP hbitmapCheckBoxes = 0;
static WORD checkBoxWidth = 0, checkBoxHeight = 0;


static LRESULT BUTTON_GetDlgCode(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  switch (dwStyle & 0x0f)
  {
    case BS_PUSHBUTTON:
      return DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON;
    case BS_DEFPUSHBUTTON:
      return DLGC_BUTTON | DLGC_DEFPUSHBUTTON;
    case BS_RADIOBUTTON:
    case BS_AUTORADIOBUTTON:
      return DLGC_BUTTON | DLGC_RADIOBUTTON;
    default:
      return DLGC_BUTTON;
  }
}

static LRESULT BUTTON_Enable(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  PAINT_BUTTON(hwnd,dwStyle & 0x0f,ODA_DRAWENTIRE);

  return 0;
}

static LRESULT BUTTON_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr;
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;

  if (!hbitmapCheckBoxes)
  {
    BITMAP bmp;

    hbitmapCheckBoxes = LoadBitmapA(0, MAKEINTRESOURCEA(OBM_CHECKBOXES));
    GetObjectA( hbitmapCheckBoxes, sizeof(bmp), &bmp );
    checkBoxWidth  = bmp.bmWidth / 4;
    checkBoxHeight = bmp.bmHeight / 3;
  }
  if (style < 0L || style >= MAX_BTN_TYPE) return -1; /* abort */

  infoPtr = (BUTTONINFO*)malloc(sizeof(BUTTONINFO));
  infoPtr->state = BUTTON_UNCHECKED;
  infoPtr->hFont = 0;
  infoPtr->hImage = NULL;
  SetInfoPtr(hwnd,(DWORD)infoPtr);

  return 0;
}

static LRESULT BUTTON_Destroy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

  free(infoPtr);

  return 0;
}

static LRESULT BUTTON_EraseBkgnd(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return 1;
}

static LRESULT BUTTON_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;

  if (btnPaintFunc[style])
  {
    PAINTSTRUCT ps;

    HDC hdc = wParam ? (HDC)wParam : BeginPaint(hwnd,&ps);
    SetBkMode(hdc,OPAQUE);
    (btnPaintFunc[style])(hwnd,hdc,ODA_DRAWENTIRE);
    if(!wParam) EndPaint(hwnd,&ps);
  }

  return 0;
}

static LRESULT BUTTON_LButtonDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  SendMessageA(hwnd,BM_SETSTATE,TRUE,0);
  SetFocus(hwnd);
  SetCapture(hwnd);

  return 0;
}

static LRESULT BUTTON_LButtonUp(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  RECT rect;
  POINT pt;

  pt.x = LOWORD(lParam);
  pt.y = HIWORD(lParam);

  ReleaseCapture();
  if (!(infoPtr->state & BUTTON_HIGHLIGHTED)) return 0;
  SendMessageA(hwnd,BM_SETSTATE,FALSE,0);
  GetClientRect(hwnd,&rect);
  if (PtInRect(&rect,pt))
  {
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    DWORD id = GetWindowLongA(hwnd,GWL_ID);

    switch(dwStyle & 0x0f)
    {
      case BS_AUTOCHECKBOX:
        SendMessageA(hwnd,BM_SETCHECK,!(infoPtr->state & BUTTON_CHECKED),0);
        break;
      case BS_AUTORADIOBUTTON:
        SendMessageA(hwnd,BM_SETCHECK,TRUE,0);
        break;
      case BS_AUTO3STATE:
        SendMessageA(hwnd,BM_SETCHECK,
                     (infoPtr->state & BUTTON_3STATE) ? 0 :
                     ((infoPtr->state & 3)+1),0);
        break;
    }
    SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),hwnd);
  }

  return 0;
}

static LRESULT BUTTON_MouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if (GetCapture() == hwnd)
  {
    RECT rect;
    POINT pt;

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);

    GetClientRect(hwnd,&rect);
    SendMessageA(hwnd,BM_SETSTATE,PtInRect(&rect,pt),0);
  }

  return 0;
}

static LRESULT BUTTON_NCHitTest(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;

  if (style == BS_GROUPBOX) return HTTRANSPARENT;

  return DefWindowProcA(hwnd,WM_NCHITTEST,wParam,lParam);
}

static LRESULT BUTTON_SetText(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  DefWindowProcA(hwnd,WM_SETTEXT,wParam,lParam);
  if (dwStyle & WS_VISIBLE) PAINT_BUTTON(hwnd,dwStyle & 0x0f,ODA_DRAWENTIRE);

  return 0;
}

static LRESULT BUTTON_SetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  infoPtr->hFont = (HFONT)wParam;
  if (lParam && (dwStyle & WS_VISIBLE)) PAINT_BUTTON(hwnd,dwStyle & 0x0f,ODA_DRAWENTIRE);

  return 0;
}

static LRESULT BUTTON_GetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

  return infoPtr->hFont;
}

static LRESULT BUTTON_SetFocus(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;

  infoPtr->state |= BUTTON_HASFOCUS;
  if (style == BS_AUTORADIOBUTTON)
  {
    SendMessageA(hwnd,BM_SETCHECK,1,0);
  }
  PAINT_BUTTON(hwnd,style,ODA_FOCUS);

  return 0;
}

static LRESULT BUTTON_KillFocus(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;

  infoPtr->state &= ~BUTTON_HASFOCUS;
  PAINT_BUTTON(hwnd,style,ODA_FOCUS);
  InvalidateRect(hwnd,NULL,TRUE);

  return 0;
}

static LRESULT BUTTON_SysColorChange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  InvalidateRect(hwnd,NULL,FALSE);

  return 0;
}

static LRESULT BUTTON_SetStyle(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if ((wParam & 0x0f) >= MAX_BTN_TYPE) return 0;
  dwStyle = (dwStyle & 0xfffffff0) | (wParam & 0x0000000f);
  SetWindowLongA(hwnd,GWL_STYLE,dwStyle);
  PAINT_BUTTON(hwnd,dwStyle & 0x0f,ODA_DRAWENTIRE);

  return 0;
}

static LRESULT BUTTON_SetImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  HANDLE oldHbitmap = infoPtr->hImage;

  if (dwStyle & BS_BITMAP) infoPtr->hImage = (HANDLE)lParam;

  return oldHbitmap;
}

static LRESULT BUTTON_GetImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

  return infoPtr->hImage;
}

static LRESULT BUTTON_GetCheck(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

  return infoPtr->state & 3;
}

static LRESULT BUTTON_SetCheck(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & 0x0f;

  if (wParam > maxCheckState[style]) wParam = maxCheckState[style];
  if ((infoPtr->state & 3) != wParam)
  {
    if ((style == BS_RADIOBUTTON) || (style == BS_AUTORADIOBUTTON))
    {
      DWORD oldStyle = dwStyle;

      if (wParam)
        dwStyle |= WS_TABSTOP;
      else
        dwStyle &= ~WS_TABSTOP;

      if (oldStyle != dwStyle) SetWindowLongA(hwnd,GWL_STYLE,dwStyle);
    }
    infoPtr->state = (infoPtr->state & ~3) | wParam;
    PAINT_BUTTON(hwnd,style,ODA_SELECT);
  }
  if ((style == BS_AUTORADIOBUTTON) && (wParam == BUTTON_CHECKED))
    BUTTON_CheckAutoRadioButton(hwnd);

  return 0;
}

static LRESULT BUTTON_GetState(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

  return infoPtr->state;
}

static LRESULT BUTTON_SetState(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;

  if (wParam)
  {
    if (infoPtr->state & BUTTON_HIGHLIGHTED) return 0;
    infoPtr->state |= BUTTON_HIGHLIGHTED;
  } else
  {
    if (!(infoPtr->state & BUTTON_HIGHLIGHTED)) return 0;
    infoPtr->state &= ~BUTTON_HIGHLIGHTED;
  }
  PAINT_BUTTON(hwnd,style,ODA_SELECT);

  return 0;
}

/***********************************************************************
 *           ButtonWndProc
 */
LRESULT WINAPI ButtonWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_GETDLGCODE:
      return BUTTON_GetDlgCode(hwnd,wParam,lParam);

    case WM_ENABLE:
      return BUTTON_Enable(hwnd,wParam,lParam);

    case WM_CREATE:
      return BUTTON_Create(hwnd,wParam,lParam);

    case WM_DESTROY:
      return BUTTON_Destroy(hwnd,wParam,lParam);

    case WM_ERASEBKGND:
      return BUTTON_EraseBkgnd(hwnd,wParam,lParam);

    case WM_PAINT:
      return BUTTON_Paint(hwnd,wParam,lParam);

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
      return BUTTON_LButtonDown(hwnd,wParam,lParam);

    case WM_LBUTTONUP:
      return BUTTON_LButtonUp(hwnd,wParam,lParam);

    case WM_MOUSEMOVE:
      return BUTTON_MouseMove(hwnd,wParam,lParam);

    case WM_NCHITTEST:
      return BUTTON_NCHitTest(hwnd,wParam,lParam);

    case WM_SETTEXT:
      return BUTTON_SetText(hwnd,wParam,lParam);

    case WM_SETFONT:
      return BUTTON_SetFont(hwnd,wParam,lParam);

    case WM_GETFONT:
      return BUTTON_GetFont(hwnd,wParam,lParam);

    case WM_SETFOCUS:
      return BUTTON_SetFocus(hwnd,wParam,lParam);

    case WM_KILLFOCUS:
      return BUTTON_KillFocus(hwnd,wParam,lParam);

    case WM_SYSCOLORCHANGE:
      return BUTTON_SysColorChange(hwnd,wParam,lParam);

    case BM_SETSTYLE:
      return BUTTON_SetStyle(hwnd,wParam,lParam);

    case BM_SETIMAGE:
      return BUTTON_SetImage(hwnd,wParam,lParam);

    case BM_GETIMAGE:
      return BUTTON_GetImage(hwnd,wParam,lParam);

    case BM_GETCHECK:
      return BUTTON_GetCheck(hwnd,wParam,lParam);

    case BM_SETCHECK:
      return BUTTON_SetCheck(hwnd,wParam,lParam);

    case BM_GETSTATE:
      return BUTTON_GetState(hwnd,wParam,lParam);

    case BM_SETSTATE:
      return BUTTON_SetState(hwnd,wParam,lParam);

    default:
      return DefWindowProcA(hwnd,uMsg,wParam,lParam);
  }

  return 0;
}


/**********************************************************************
 *       Push Button Functions
 */

static void PB_Paint(HWND hwnd,HDC hDC,WORD action)
{
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rc;
    HPEN hOldPen;
    HBRUSH hOldBrush;
    int xBorderOffset, yBorderOffset;
    xBorderOffset = yBorderOffset = 0;
    INT textLen;
    char* text;

    GetClientRect(hwnd,&rc );

      /* Send WM_CTLCOLOR to allow changing the font (the colors are fixed) */
    if (infoPtr->hFont) SelectObject( hDC, infoPtr->hFont );
    SendMessageA(GetParent(hwnd),WM_CTLCOLORBTN,hDC,hwnd);

    hOldPen = (HPEN)SelectObject(hDC, GetSysColorPen(COLOR_WINDOWFRAME));
    hOldBrush =(HBRUSH)SelectObject(hDC,GetSysColorBrush(COLOR_BTNFACE));
    SetBkMode(hDC, TRANSPARENT);
    Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
/*    if (action == ODA_DRAWENTIRE)*/
    {
        SetPixel( hDC, rc.left, rc.top, GetSysColor(COLOR_WINDOW) );
        SetPixel( hDC, rc.left, rc.bottom-1, GetSysColor(COLOR_WINDOW) );
        SetPixel( hDC, rc.right-1, rc.top, GetSysColor(COLOR_WINDOW) );
        SetPixel( hDC, rc.right-1, rc.bottom-1, GetSysColor(COLOR_WINDOW));
    }
    InflateRect( &rc, -1, -1 );

    if ((dwStyle & 0x000f) == BS_DEFPUSHBUTTON)
    {
        Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
        InflateRect( &rc, -1, -1 );
    }

    if (infoPtr->state & BUTTON_HIGHLIGHTED)
    {
        /* draw button shadow: */
        SelectObject(hDC, GetSysColorBrush(COLOR_BTNSHADOW));
        PatBlt(hDC, rc.left, rc.top, 1, rc.bottom-rc.top, PATCOPY );
        PatBlt(hDC, rc.left, rc.top, rc.right-rc.left, 1, PATCOPY );
        rc.left += 2;  /* To position the text down and right */
        rc.top  += 2;
    } else {
        rc.right++, rc.bottom++;
        DrawEdge( hDC, &rc, EDGE_RAISED, BF_RECT );

        /* To place de bitmap correctly */
        xBorderOffset += GetSystemMetrics(SM_CXEDGE);
        yBorderOffset += GetSystemMetrics(SM_CYEDGE);

        rc.right--, rc.bottom--;
    }

    /* draw button label, if any: */
    textLen = GetWindowTextLengthA(hwnd);
    if (textLen > 0)
    {
        LOGBRUSH lb;

        textLen++;
        text = (char*)malloc(textLen);
        GetWindowTextA(hwnd,text,textLen);
        GetObjectA( GetSysColorBrush(COLOR_BTNFACE), sizeof(lb), &lb );
        if (dwStyle & WS_DISABLED &&
            GetSysColor(COLOR_GRAYTEXT) == lb.lbColor)
            /* don't write gray text on gray background */
            PaintGrayOnGray( hDC,infoPtr->hFont,&rc,text,
                               DT_CENTER | DT_VCENTER );
        else
        {
            SetTextColor( hDC, (dwStyle & WS_DISABLED) ?
                                 GetSysColor(COLOR_GRAYTEXT) :
                                 GetSysColor(COLOR_BTNTEXT) );
            DrawTextA( hDC, text, -1, &rc,
                         DT_SINGLELINE | DT_CENTER | DT_VCENTER );
            /* do we have the focus? */
            if (infoPtr->state & BUTTON_HASFOCUS)
            {
                RECT r = { 0, 0, 0, 0 };
                INT xdelta, ydelta;

                DrawTextA( hDC, text, -1, &r,
                             DT_SINGLELINE | DT_CALCRECT );
                xdelta = ((rc.right - rc.left) - (r.right - r.left) - 1) / 2;
                ydelta = ((rc.bottom - rc.top) - (r.bottom - r.top) - 1) / 2;
                if (xdelta < 0) xdelta = 0;
                if (ydelta < 0) ydelta = 0;
                InflateRect( &rc, -xdelta, -ydelta );
                DrawFocusRect( hDC, &rc );
            }
        }
        free(text);
    }

    if((dwStyle & BS_BITMAP) && (infoPtr->hImage != NULL))
    {
        BITMAP bm;
        HDC hdcMem;
        int yOffset, xOffset, imageWidth, imageHeight;

        GetObjectA (infoPtr->hImage, sizeof(BITMAP), &bm);

        /* Center the bitmap */
        xOffset = (((rc.right - rc.left) - 2*xBorderOffset) - bm.bmWidth ) / 2;
        yOffset = (((rc.bottom - rc.top) - 2*yBorderOffset) - bm.bmHeight ) / 2;

        imageWidth = bm.bmWidth;
        imageHeight = bm.bmHeight;

        /* If the image is to big for the button */
        if (xOffset < 0)
        {
            imageWidth = rc.right - rc.left - 2*xBorderOffset -1;
            xOffset = xBorderOffset;
        }

        if (yOffset < 0)
        {
            imageHeight = rc.bottom - rc.top - 2*yBorderOffset -1;
            yOffset = yBorderOffset;
        }

        /* Let minimum 1 space from border */
        xOffset++, yOffset++;

        hdcMem = CreateCompatibleDC (hDC);
        SelectObject (hdcMem, (HBITMAP)infoPtr->hImage);
        BitBlt(hDC, rc.left + xOffset,
               rc.top + yOffset,
               imageWidth, imageHeight,
               hdcMem, 0, 0, SRCCOPY);

        DeleteDC (hdcMem);
    }

    SelectObject( hDC, hOldPen );
    SelectObject( hDC, hOldBrush );
}


/**********************************************************************
 *   PB_Paint & CB_Paint sub function                        [internal]
 *   Paint text using a raster brush to avoid gray text on gray
 *   background. 'format' can be a combination of DT_CENTER and
 *   DT_VCENTER to use this function in both PB_PAINT and
 *   CB_PAINT.   - Dirk Thierbach
 *
 *   FIXME: This and TEXT_GrayString should be eventually combined,
 *   so calling one common function from PB_Paint, CB_Paint and
 *   TEXT_GrayString will be enough. Also note that this
 *   function ignores the CACHE_GetPattern funcs.
 */

static void PaintGrayOnGray(HDC hDC,HFONT hFont,RECT *rc,char *text,
                            UINT format)
{
/*  This is the standard gray on gray pattern:
    static const WORD Pattern[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55};
*/
/*  This pattern gives better readability with X Fonts.
    FIXME: Maybe the user should be allowed to decide which he wants. */
    static const WORD Pattern[] = {0x55,0xFF,0xAA,0xFF,0x55,0xFF,0xAA,0xFF};

    HBITMAP hbm  = CreateBitmap( 8, 8, 1, 1, Pattern );
    HDC hdcMem = CreateCompatibleDC(hDC);
    HBITMAP hbmMem;
    HBRUSH hBr;
    RECT rect,rc2;

    rect=*rc;
    DrawTextA( hDC, text, -1, &rect, DT_SINGLELINE | DT_CALCRECT);
    /* now text width and height are in rect.right and rect.bottom */
    rc2=rect;
    rect.left = rect.top = 0; /* drawing pos in hdcMem */
    if (format & DT_CENTER) rect.left=(rc->right-rect.right)/2;
    if (format & DT_VCENTER) rect.top=(rc->bottom-rect.bottom)/2;
    hbmMem = CreateCompatibleBitmap( hDC,rect.right,rect.bottom );
    SelectObject( hdcMem, hbmMem);
    PatBlt( hdcMem,0,0,rect.right,rect.bottom,WHITENESS);
      /* will be overwritten by DrawText, but just in case */
    if (hFont) SelectObject( hdcMem, hFont);
    DrawTextA( hdcMem, text, -1, &rc2, DT_SINGLELINE);
      /* After draw: foreground = 0 bits, background = 1 bits */
    hBr = SelectObject( hdcMem, CreatePatternBrush(hbm) );
    DeleteObject( hbm );
    PatBlt( hdcMem,0,0,rect.right,rect.bottom,0xAF0229);
      /* only keep the foreground bits where pattern is 1 */
    DeleteObject( SelectObject( hdcMem,hBr) );
    BitBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hdcMem,0,0,SRCAND);
      /* keep the background of the dest */
    DeleteDC( hdcMem);
    DeleteObject( hbmMem );
}


/**********************************************************************
 *       Check Box & Radio Button Functions
 */

static void CB_Paint(HWND hwnd,HDC hDC,WORD action)
{
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rbox, rtext, client;
    HBRUSH hBrush;
    int textLen, delta;
    char* text = NULL;

    textLen = 0;
    GetClientRect(hwnd, &client);
    rbox = rtext = client;

    if (infoPtr->hFont) SelectObject( hDC, infoPtr->hFont );

    /* Something is still not right, checkboxes (and edit controls)
     * in wsping32 have white backgrounds instead of dark grey.
     * BUTTON_SEND_CTLCOLOR() is even worse since it returns 0 in this
     * particular case and the background is not painted at all.
     */

    hBrush = GetSysColorBrush(COLOR_BTNFACE);

    if (dwStyle & BS_LEFTTEXT)
    {
        /* magic +4 is what CTL3D expects */

        rtext.right -= checkBoxWidth + 4;
        rbox.left = rbox.right - checkBoxWidth;
    }
    else
    {
        rtext.left += checkBoxWidth + 4;
        rbox.right = checkBoxWidth;
    }

      /* Draw the check-box bitmap */

    textLen = GetWindowTextLengthA(hwnd);
    if (textLen > 0)
    {
      textLen++;
      text = (char*)malloc(textLen);
      GetWindowTextA(hwnd,text,textLen);
    }
    if (action == ODA_DRAWENTIRE || action == ODA_SELECT)
    {
        HDC hMemDC = CreateCompatibleDC( hDC );
        int x = 0, y = 0;
        delta = (rbox.bottom - rbox.top - checkBoxHeight) >> 1;

        if (action == ODA_SELECT) FillRect( hDC, &rbox, hBrush );
        else FillRect( hDC, &client, hBrush );

        if (infoPtr->state & BUTTON_HIGHLIGHTED) x += 2 * checkBoxWidth;
        if (infoPtr->state & (BUTTON_CHECKED | BUTTON_3STATE)) x += checkBoxWidth;
        if (((dwStyle & 0x0f) == BS_RADIOBUTTON) ||
            ((dwStyle & 0x0f) == BS_AUTORADIOBUTTON)) y += checkBoxHeight;
        else if (infoPtr->state & BUTTON_3STATE) y += 2 * checkBoxHeight;

        SelectObject( hMemDC, hbitmapCheckBoxes );
        BitBlt( hDC, rbox.left, rbox.top + delta, checkBoxWidth,
                  checkBoxHeight, hMemDC, x, y, SRCCOPY );
        DeleteDC( hMemDC );

        if( text && action != ODA_SELECT )
        {
          if (dwStyle & WS_DISABLED &&
              GetSysColor(COLOR_GRAYTEXT) == GetBkColor(hDC)) {
            /* don't write gray text on gray background */
            PaintGrayOnGray( hDC, infoPtr->hFont, &rtext, text,
                             DT_VCENTER);
          } else {
            if (dwStyle & WS_DISABLED)
                SetTextColor( hDC, GetSysColor(COLOR_GRAYTEXT) );
            DrawTextA( hDC, text, -1, &rtext,
                         DT_SINGLELINE | DT_VCENTER );
            textLen = 0; /* skip DrawText() below */
          }
        }
    }

    if ((action == ODA_FOCUS) ||
        ((action == ODA_DRAWENTIRE) && (infoPtr->state & BUTTON_HASFOCUS)))
    {
        /* again, this is what CTL3D expects */

        SetRectEmpty(&rbox);
        if(textLen > 0)
          DrawTextA( hDC, text, -1, &rbox,
                     DT_SINGLELINE | DT_CALCRECT );
        textLen = rbox.bottom - rbox.top;
        delta = ((rtext.bottom - rtext.top) - textLen)/2;
        rbox.bottom = (rbox.top = rtext.top + delta - 1) + textLen + 2;
        textLen = rbox.right - rbox.left;
        rbox.right = (rbox.left += --rtext.left) + textLen + 2;
        IntersectRect(&rbox, &rbox, &rtext);
        DrawFocusRect( hDC, &rbox );
    }
    if (text) free(text);
}


/**********************************************************************
 *       BUTTON_CheckAutoRadioButton
 *
 * wndPtr is checked, uncheck every other auto radio button in group
 */
static void BUTTON_CheckAutoRadioButton(HWND hwnd)
{
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    HWND parent, sibling, start;

    if (!(dwStyle & WS_CHILD)) return;
    parent = GetParent(hwnd);
    /* assure that starting control is not disabled or invisible */
    start = sibling = GetNextDlgGroupItem( parent, hwnd, TRUE );
    do
    {
        if (!sibling) break;
        if ((hwnd != sibling) &&
            ((GetWindowLongA(sibling,GWL_STYLE) & 0x0f) == BS_AUTORADIOBUTTON))
            SendMessageA( sibling, BM_SETCHECK, BUTTON_UNCHECKED, 0 );
        sibling = GetNextDlgGroupItem( parent, sibling, FALSE );
    } while (sibling != start);
}


/**********************************************************************
 *       Group Box Functions
 */

static void GB_Paint(HWND hwnd,HDC hDC,WORD action)
{
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rc, rcFrame;
    TEXTMETRICA tm;
    INT textLen;
    char* text;

    if (action != ODA_DRAWENTIRE) return;

    SendMessageA(GetParent(hwnd),WM_CTLCOLORBTN,hDC,hwnd);

    GetClientRect(hwnd,&rc);

    rcFrame = rc;

    if (infoPtr->hFont)
       SelectObject (hDC, infoPtr->hFont);
    GetTextMetricsA (hDC, &tm);
    rcFrame.top += (tm.tmHeight / 2) - 1;
    DrawEdge (hDC, &rcFrame, EDGE_ETCHED, BF_RECT);

    textLen = GetWindowTextLengthA(hwnd);
    if (textLen > 0)
    {
        textLen++;
        text = (char*)malloc(textLen);
        GetWindowTextA(hwnd,text,textLen);
        if (infoPtr->hFont) SelectObject( hDC, infoPtr->hFont );
        if (dwStyle & WS_DISABLED)
            SetTextColor( hDC, GetSysColor(COLOR_GRAYTEXT) );
        rc.left += 10;
        DrawTextA( hDC, text, -1, &rc, DT_SINGLELINE | DT_NOCLIP );
        free(text);
    }
}


/**********************************************************************
 *       User Button Functions
 */

static void UB_Paint(HWND hwnd,HDC hDC,WORD action)
{
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rc;
    HBRUSH hBrush;
    if (action == ODA_SELECT) return;

    GetClientRect(hwnd,&rc);

    if (infoPtr->hFont) SelectObject( hDC, infoPtr->hFont );
    hBrush = GetSysColorBrush(COLOR_BTNFACE);

    FillRect( hDC, &rc, hBrush );
    if ((action == ODA_FOCUS) ||
        ((action == ODA_DRAWENTIRE) && (infoPtr->state & BUTTON_HASFOCUS)))
        DrawFocusRect( hDC, &rc );
}


/**********************************************************************
 *       Ownerdrawn Button Functions
 */

static void OB_Paint(HWND hwnd,HDC hDC,WORD action)
{
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    DRAWITEMSTRUCT dis;

    dis.CtlType    = ODT_BUTTON;
    dis.CtlID      = GetWindowLongA(hwnd,GWL_ID);
    dis.itemID     = 0;
    dis.itemAction = action;
    dis.itemState  = ((infoPtr->state & BUTTON_HASFOCUS) ? ODS_FOCUS : 0) |
                     ((infoPtr->state & BUTTON_HIGHLIGHTED) ? ODS_SELECTED : 0) |
                     ((dwStyle & WS_DISABLED) ? ODS_DISABLED : 0);
    dis.hwndItem   = hwnd;
    dis.hDC        = hDC;
    dis.itemData   = 0;
    GetClientRect( hwnd, &dis.rcItem );
    SendMessageA( GetParent(hwnd), WM_DRAWITEM,
                    GetWindowLongA(hwnd,GWL_ID), (LPARAM)&dis );
}

void BUTTON_Register()
{
    WNDCLASSA wndClass;

    if (GlobalFindAtomA (BUTTONCLASSNAME)) return;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)ButtonWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(BUTTONINFO*);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wndClass.lpszClassName = BUTTONCLASSNAME;

    RegisterClassA (&wndClass);
}


void BUTTON_Unregister()
{
    if (GlobalFindAtomA (BUTTONCLASSNAME))
        UnregisterClassA (BUTTONCLASSNAME, (HINSTANCE)NULL);
}
