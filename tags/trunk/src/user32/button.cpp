/* $Id: button.cpp,v 1.40 2001-06-14 12:56:51 sandervl Exp $ */
/* File: button.cpp -- Button type widgets
 *
 * Copyright (C) 1993 Johannes Ruscheinski
 * Copyright (C) 1993 David Metcalfe
 * Copyright (C) 1994 Alexandre Julliard
 * Copyright (c) 1999 Christoph Bratschi
 *
 * Corel version: 20000513
 * (WINE version: 20000130)
 *
 * Status: complete
 * Version: 5.00
 */

#include <string.h>
#include <stdlib.h>
#include <os2win.h>
#include "controls.h"
#include "button.h"
#include <misc.h>
#include "initterm.h"
#include "syscolor.h"

#define DBG_LOCALLOG    DBG_button
#include "dbglocal.h"

#ifdef DEBUG
char *GetMsgText(int Msg);
#endif

//Prototypes

static void DrawDisabledText(HDC hdc,char* text,RECT* rtext,UINT format);

static void PB_Paint(HWND hwnd,HDC hDC,WORD action);
static void CB_Paint(HWND hwnd,HDC hDC,WORD action);
static void GB_Paint(HWND hwnd,HDC hDC,WORD action);
static void UB_Paint(HWND hwnd,HDC hDC,WORD action);
static void OB_Paint(HWND hwnd,HDC hDC,WORD action);
static void BUTTON_CheckAutoRadioButton(HWND hwnd);
static void BUTTON_DrawPushButton(HWND hwnd,HDC hDC,WORD action,BOOL pushedState);
static LRESULT BUTTON_LButtonDown(HWND hwnd,WPARAM wParam,LPARAM lParam);

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

static LRESULT BUTTON_SendNotify(HWND hwnd,DWORD code)
{
  return SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),code),hwnd);
}

static LRESULT BUTTON_GetDlgCode(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  switch (dwStyle & 0x0f)
  {
    case BS_AUTOCHECKBOX:
    case BS_CHECKBOX:
     return DLGC_WANTCHARS | DLGC_BUTTON;

    case BS_PUSHBUTTON:
      return DLGC_UNDEFPUSHBUTTON;

    case BS_DEFPUSHBUTTON:
      return DLGC_DEFPUSHBUTTON;

    case BS_AUTORADIOBUTTON:
    case BS_RADIOBUTTON:
      return DLGC_RADIOBUTTON;

    case BS_GROUPBOX:;
      return DLGC_STATIC;

    default:
      return DLGC_BUTTON;
  }
}

static LRESULT BUTTON_Enable(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if ((dwStyle & BS_NOTIFY) && !wParam) BUTTON_SendNotify(hwnd,BN_DISABLE);

  //PAINT_BUTTON(hwnd,dwStyle & 0x0f,ODA_DRAWENTIRE);
  //SvL: 09/10/99 Force it to redraw properly
  InvalidateRect( hwnd, NULL, FALSE );

  return 0;
}

static LRESULT BUTTON_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr;
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;

  if (!hbitmapCheckBoxes)
  {
    BITMAP bmp;

    hbitmapCheckBoxes = LoadBitmapA(hInstanceUser32, MAKEINTRESOURCEA(OBM_CHECKBOXES));
    GetObjectA( hbitmapCheckBoxes, sizeof(bmp), &bmp );
    if (GetObjectA(hbitmapCheckBoxes,sizeof(bmp),&bmp))
    {
      checkBoxWidth  = bmp.bmWidth / 4;
      checkBoxHeight = bmp.bmHeight / 3;
    } else checkBoxWidth = checkBoxHeight = 0;
  }
  if ((style < 0L) || (style >= MAX_BTN_TYPE)) return -1; /* abort */

  infoPtr = (BUTTONINFO*)malloc(sizeof(BUTTONINFO));
  infoPtr->state = BUTTON_UNCHECKED;
  infoPtr->hFont = 0;
  infoPtr->hImage = 0;
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
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0f;
  //SvL: TODO: NT does something extra for ownerdrawn buttons; check this 
  if(style == BS_OWNERDRAW) {
        return DefWindowProcA(hwnd, WM_ERASEBKGND, wParam, lParam);
  }
  return 1;
}

static LRESULT BUTTON_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & 0x0f;

  if (dwStyle & BS_NOTIFY) BUTTON_SendNotify(hwnd,BN_PAINT);

  if (btnPaintFunc[style])
  {
    PAINTSTRUCT ps;

    HDC hdc = wParam ? (HDC)wParam : BeginPaint(hwnd,&ps);
    SetBkMode(hdc,OPAQUE);
    (btnPaintFunc[style])(hwnd,hdc,ODA_DRAWENTIRE);
    if(!wParam) EndPaint(hwnd,&ps);
  } else return DefWindowProcA(hwnd,WM_PAINT,wParam,lParam);

  return 0;
}

static LRESULT BUTTON_LButtonDblClk(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & 0x0f;

  if(dwStyle & BS_NOTIFY || style == BS_RADIOBUTTON ||
     style == BS_USERBUTTON || style == BS_OWNERDRAW)
    BUTTON_SendNotify(hwnd,BN_DOUBLECLICKED);
  else BUTTON_LButtonDown(hwnd,wParam,lParam);

  return 0;
}

static LRESULT BUTTON_LButtonDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & 0x0F;

  if (style == BS_GROUPBOX) return 0;
  SetCapture(hwnd);
  SetFocus(hwnd);
  SendMessageA(hwnd,BM_SETSTATE,TRUE,0);
  infoPtr->state |= BUTTON_BTNPRESSED;

  if (dwStyle & BS_NOTIFY) BUTTON_SendNotify(hwnd,BN_HILITE);

  return 0;
}

static LRESULT BUTTON_LButtonUp(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & 0x0F;
  RECT rect;
  POINT pt;

  pt.x = LOWORD(lParam);
  pt.y = HIWORD(lParam);

  if (!(infoPtr->state & BUTTON_BTNPRESSED)) return 0;
  infoPtr->state &= BUTTON_NSTATES;
  if (!(infoPtr->state & BUTTON_HIGHLIGHTED))
  {
    ReleaseCapture();
    return 0;
  }
  SendMessageA(hwnd,BM_SETSTATE,FALSE,0);
  ReleaseCapture();
  GetClientRect(hwnd,&rect);
  if (PtInRect(&rect,pt))
  {
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
    BUTTON_SendNotify(hwnd,BN_CLICKED);
  }

  if (dwStyle & BS_NOTIFY) BUTTON_SendNotify(hwnd,BN_UNHILITE);

  return 0;
}

static LRESULT BUTTON_CaptureChanged(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if (infoPtr->state & BUTTON_BTNPRESSED)
  {
    infoPtr->state &= BUTTON_NSTATES;
    if (infoPtr->state & BUTTON_HIGHLIGHTED)
      SendMessageA( hwnd, BM_SETSTATE, FALSE, 0 );
  }

  if (dwStyle & BS_NOTIFY) BUTTON_SendNotify(hwnd,BN_UNHILITE);

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

#ifdef __WIN32OS2__
  if ((dwStyle & 0x0f) == BS_GROUPBOX) {
    RECT rc;
    TEXTMETRICA tm;
    HDC  hdc = GetDC(hwnd);
    int  fh1 = 0, fh2 = 0;

    // select old font (if exists)
    if (infoPtr->hFont) {
       SelectObject (hdc, infoPtr->hFont);
       GetTextMetricsA (hdc, &tm);
       fh1 = tm.tmHeight;
    }
    // select new font (if exists)
    if (wParam) {
       SelectObject (hdc, wParam);
       GetTextMetricsA (hdc, &tm);
       fh2 = tm.tmHeight;
    }
    // Erases top border line and (old) text background
    GetClientRect(hwnd, &rc);
    rc.bottom = rc.top + max( fh1, fh2) + 1;
    HBRUSH hbr = GetControlBrush( hwnd, hdc, CTLCOLOR_STATIC );
    FillRect(hdc, &rc, hbr);
    ReleaseDC(hwnd,hdc);
  }
#endif

  infoPtr->hFont = (HFONT)wParam;
  if (lParam && (dwStyle & WS_VISIBLE)) PAINT_BUTTON(hwnd,dwStyle & 0x0f,ODA_DRAWENTIRE);

  return 0;
}

static LRESULT BUTTON_GetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

  return infoPtr->hFont;
}

static LRESULT BUTTON_KeyDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if (wParam == VK_SPACE)
  {
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

    SendMessageA(hwnd,BM_SETSTATE,TRUE,0);
    infoPtr->state |= BUTTON_BTNPRESSED;
  }

  return 0;
}

static LRESULT BUTTON_KeyUp(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if (wParam == VK_SPACE)
  {
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    if (!(infoPtr->state & BUTTON_BTNPRESSED)) return 0;
    infoPtr->state &= BUTTON_NSTATES;
    if (!(infoPtr->state & BUTTON_HIGHLIGHTED)) return 0;
    SendMessageA(hwnd,BM_SETSTATE,FALSE,0);

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
    BUTTON_SendNotify(hwnd,BN_CLICKED);
  }

  return 0;
}

static LRESULT BUTTON_SysKeyUp(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if (wParam != VK_TAB) ReleaseCapture();

  return 0;
}

static LRESULT BUTTON_SetFocus(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & 0x0f;

  if (dwStyle & BS_NOTIFY) BUTTON_SendNotify(hwnd,BN_SETFOCUS);

  if (((style == BS_AUTORADIOBUTTON) || (style == BS_RADIOBUTTON)) &&
      (GetCapture() != hwnd) && !(SendMessageA(hwnd,BM_GETCHECK,0,0) & BST_CHECKED))
  {
    /* The notification is sent when the button (BS_AUTORADIOBUTTON)
       is unckecked and the focus was not given by a mouse click. */
    if (style == BS_AUTORADIOBUTTON) SendMessageA(hwnd,BM_SETCHECK,TRUE,0);
    BUTTON_SendNotify(hwnd,BN_CLICKED);
  }

  infoPtr->state |= BUTTON_HASFOCUS;
  PAINT_BUTTON(hwnd,style,ODA_FOCUS);

  return 0;
}

static LRESULT BUTTON_KillFocus(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & 0x0f;

  if (dwStyle & BS_NOTIFY) BUTTON_SendNotify(hwnd,BN_KILLFOCUS);

  if (infoPtr->state & BUTTON_HASFOCUS)
  {
    infoPtr->state &= ~BUTTON_HASFOCUS;
    PAINT_BUTTON(hwnd,style,ODA_FOCUS);
  }

  return 0;
}

static LRESULT BUTTON_SysColorChange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  InvalidateRect(hwnd,NULL,FALSE);

  return 0;
}

static LRESULT BUTTON_Click(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  RECT rect;
  LPARAM point;

  GetClientRect(hwnd,&rect);
  point = MAKELPARAM(rect.right/2,rect.bottom/2);
  SendMessageA(hwnd,WM_LBUTTONDOWN,MK_LBUTTON,point);
  Sleep(100);
  SendMessageA(hwnd,WM_LBUTTONUP,0,point);

  return 0;
}

static LRESULT BUTTON_SetStyle(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE),newStyle;

  if ((wParam & 0x0F) >= MAX_BTN_TYPE) return 0;
  newStyle = (dwStyle & 0xFFFFFFF0) | (wParam & 0x0000000F);

  if (newStyle != dwStyle)
  {
    SetWindowLongA(hwnd,GWL_STYLE,newStyle);
    PAINT_BUTTON(hwnd,newStyle & 0x0F,ODA_DRAWENTIRE);
  }

  return 0;
}

static LRESULT BUTTON_SetImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  HANDLE oldHbitmap = infoPtr->hImage;

  if ((dwStyle & BS_BITMAP) || (dwStyle & BS_ICON))
  {
    infoPtr->hImage = (HANDLE)lParam;
    InvalidateRect(hwnd,NULL,FALSE);
  }

  return oldHbitmap;
}

static LRESULT BUTTON_GetImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);

  switch(wParam)
  {
    case IMAGE_BITMAP:
      return (HBITMAP)infoPtr->hImage;
    case IMAGE_ICON:
      return (HICON)infoPtr->hImage;
    default:
      return (HICON)0;
  }
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
#ifndef __WIN32OS2__
  //Must clear WS_TABSTOP of control that is already disabled or else 
  //multiple control can have this style ((auto)radiobutton) and 
  //GetNextDlgTabItem will return the wrong one
  //Happens in Opera preferences dialog (multimedia) where all autoradio
  //buttons have the WS_TABSTOP style.
  if ((infoPtr->state & 3) != wParam)
  {
#endif
    if ((style == BS_RADIOBUTTON) || (style == BS_AUTORADIOBUTTON))
    {
      DWORD oldStyle = dwStyle;

      if (wParam)
        dwStyle |= WS_TABSTOP;
      else
        dwStyle &= ~WS_TABSTOP;

      if (oldStyle != dwStyle) SetWindowLongA(hwnd,GWL_STYLE,dwStyle);
    }
#ifdef __WIN32OS2__
    if ((infoPtr->state & 3) != wParam)
    {
#endif
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
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & 0x0F;

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
static
LRESULT WINAPI ButtonWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
//  dprintf(("ButtonWndProc hwnd: %04x, msg %s, wp %08x lp %08lx\n",
//           hwnd, GetMsgText(uMsg), wParam, lParam));

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

    case WM_LBUTTONDBLCLK:
      return BUTTON_LButtonDblClk(hwnd,wParam,lParam);

    case WM_LBUTTONDOWN:
      return BUTTON_LButtonDown(hwnd,wParam,lParam);

    case WM_LBUTTONUP:
      return BUTTON_LButtonUp(hwnd,wParam,lParam);

    case WM_CAPTURECHANGED:
      return BUTTON_CaptureChanged(hwnd,wParam,lParam);

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

    case WM_KEYDOWN:
      return BUTTON_KeyDown(hwnd,wParam,lParam);

    case WM_KEYUP:
      return BUTTON_KeyUp(hwnd,wParam,lParam);

    case WM_SYSKEYUP:
      return BUTTON_SysKeyUp(hwnd,wParam,lParam);

    case WM_SETFOCUS:
      return BUTTON_SetFocus(hwnd,wParam,lParam);

    case WM_KILLFOCUS:
      return BUTTON_KillFocus(hwnd,wParam,lParam);

    case WM_SYSCOLORCHANGE:
      return BUTTON_SysColorChange(hwnd,wParam,lParam);

    case BM_CLICK:
      return BUTTON_Click(hwnd,wParam,lParam);

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
static void PB_Paint( HWND hwnd, HDC hDC, WORD action )
{
    BUTTONINFO *infoPtr      = (BUTTONINFO *)GetInfoPtr(hwnd);
    BOOL        bHighLighted = (infoPtr->state & BUTTON_HIGHLIGHTED);

    /*
     * Delegate this to the more generic pushbutton painting
     * method.
     */
    BUTTON_DrawPushButton(hwnd,
                          hDC,
                          action,
                          bHighLighted);
}

static INT BUTTON_GetTextFormat(DWORD dwStyle,DWORD dwExStyle,INT defHorz,INT defVert)
{
  INT format = 0;

  if (dwStyle & BS_LEFT) format = DT_LEFT;
  else if (dwStyle & BS_CENTER) format = DT_CENTER;
  else if ((dwStyle & BS_RIGHT) || (dwExStyle & WS_EX_RIGHT)) format = DT_RIGHT;
  else format = defHorz;

  if (dwStyle & BS_TOP) format |= DT_TOP;
  else if (dwStyle & BS_VCENTER) format |= DT_VCENTER;
  else if (dwStyle & BS_BOTTOM) format |= DT_BOTTOM;
  else format |= defVert;

  if (!(dwStyle & BS_MULTILINE)) format |= DT_SINGLELINE;

  return format;
}

/**********************************************************************
 * This method will actually do the drawing of the pushbutton
 * depending on it's state and the pushedState parameter.
 */
static void BUTTON_DrawPushButton(
  HWND hwnd,
  HDC  hDC,
  WORD action,
  BOOL pushedState )
{
    RECT rc, focus_rect;
    HPEN hOldPen;
    HBRUSH hOldBrush;
    BUTTONINFO *infoPtr = (BUTTONINFO *)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    int xBorderOffset, yBorderOffset;
    xBorderOffset = yBorderOffset = 0;
    INT textLen;
    char* text;

    GetClientRect( hwnd, &rc );

      /* Send WM_CTLCOLOR to allow changing the font (the colors are fixed) */
    if (infoPtr->hFont) SelectObject( hDC, infoPtr->hFont );
    BUTTON_SEND_CTLCOLOR( hwnd, hDC );
    hOldPen = (HPEN)SelectObject(hDC, GetSysColorPen(COLOR_WINDOWFRAME));
    hOldBrush =(HBRUSH)SelectObject(hDC,GetSysColorBrush(COLOR_BTNFACE));
    SetBkMode(hDC, TRANSPARENT);

    if ((dwStyle & 0x000f) == BS_DEFPUSHBUTTON)
    {
        Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
        InflateRect( &rc, -1, -1 );
    }

    UINT uState = DFCS_BUTTONPUSH;

    if (pushedState)
    {
      if ( (dwStyle & 0x000f) == BS_DEFPUSHBUTTON )
        uState |= DFCS_FLAT;
      else
        uState |= DFCS_PUSHED;
    }

    if (dwStyle & BS_FLAT) uState |= DFCS_FLAT;

    DrawFrameControl( hDC, &rc, DFC_BUTTON, uState );
    InflateRect( &rc, -2, -2 );

    focus_rect = rc;

    if (pushedState)
    {
      rc.left += 2;  /* To position the text down and right */
      rc.top  += 2;
    }


    /* draw button label, if any:
     *
     * In win9x we don't show text if there is a bitmap or icon.
     * I don't know about win31 so I leave it as it was for win31.
     * Dennis Björklund 12 Jul, 99
     */
    textLen = GetWindowTextLengthA(hwnd);
    if ((textLen > 0) && (!(dwStyle & (BS_ICON|BS_BITMAP))))
    {
        INT format = BUTTON_GetTextFormat(dwStyle,GetWindowLongA(hwnd,GWL_EXSTYLE),DT_CENTER,DT_VCENTER);

        textLen++;
        text = (char*)malloc(textLen);
        GetWindowTextA(hwnd,text,textLen);

        if (dwStyle & WS_DISABLED) DrawDisabledText(hDC,text,&rc,format); else
        {
            SetTextColor(hDC,GetSysColor(COLOR_BTNTEXT));
            DrawTextA(hDC,text,-1,&rc,format);
            /* do we have the focus?
             * Win9x draws focus last with a size prop. to the button
             */
        }
        free(text);
    }
    if ( ((dwStyle & BS_ICON) || (dwStyle & BS_BITMAP) ) &&
         (infoPtr->hImage != 0) )
    {
        int yOffset, xOffset;
        int imageWidth, imageHeight;

        /*
         * We extract the size of the image from the handle.
         */
        if (dwStyle & BS_ICON)
        {
            ICONINFO iconInfo;
            BITMAP   bm;

            GetIconInfo((HICON)infoPtr->hImage,&iconInfo);
            if (iconInfo.hbmColor)
            {
              GetObjectA(iconInfo.hbmColor,sizeof(BITMAP),&bm);
              imageWidth  = bm.bmWidth;
              imageHeight = bm.bmHeight;
            } else
            {
              GetObjectA(iconInfo.hbmMask,sizeof(BITMAP),&bm);
              imageWidth  = bm.bmWidth;
              imageHeight = bm.bmHeight/2;
            }

            if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
            if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);

        }
        else
        {
            BITMAP   bm;

            GetObjectA (infoPtr->hImage, sizeof(BITMAP), &bm);

            imageWidth  = bm.bmWidth;
            imageHeight = bm.bmHeight;
        }

        /* Center the bitmap */
        xOffset = (((rc.right - rc.left) - 2*xBorderOffset) - imageWidth ) / 2;
        yOffset = (((rc.bottom - rc.top) - 2*yBorderOffset) - imageHeight) / 2;

        /* If the image is too big for the button then create a region*/
        if(xOffset < 0 || yOffset < 0)
        {
            HRGN hBitmapRgn = 0;
            hBitmapRgn = CreateRectRgn(
                rc.left + xBorderOffset, rc.top +yBorderOffset,
                rc.right - xBorderOffset, rc.bottom - yBorderOffset);
            SelectClipRgn(hDC, hBitmapRgn);
            DeleteObject(hBitmapRgn);
        }

        /* Let minimum 1 space from border */
        xOffset++, yOffset++;

        /*
         * Draw the image now.
         */
        if (dwStyle & BS_ICON)
        {
            DrawIcon(hDC,
                rc.left + xOffset, rc.top + yOffset,
                     (HICON)infoPtr->hImage);
        }
        else
        {
            HDC hdcMem;

            hdcMem = CreateCompatibleDC (hDC);
            SelectObject (hdcMem, (HBITMAP)infoPtr->hImage);
            BitBlt(hDC,
                   rc.left + xOffset,
                   rc.top + yOffset,
                   imageWidth, imageHeight,
                   hdcMem, 0, 0, SRCCOPY);

            DeleteDC (hdcMem);
        }

        if(xOffset < 0 || yOffset < 0)
        {
            SelectClipRgn(hDC, 0);
        }
    }

    SelectObject( hDC, hOldPen );
    SelectObject( hDC, hOldBrush );

    if ((infoPtr->state & BUTTON_HASFOCUS) && IsWindowEnabled(hwnd))
    {
        InflateRect( &focus_rect, -1, -1 );
        DrawFocusRect( hDC, &focus_rect );
    }
}


static void DrawDisabledText(HDC hdc,char* text,RECT* rtext,UINT format)
{
  COLORREF textColor = (GetSysColor(COLOR_GRAYTEXT) == GetBkColor(hdc)) ? COLOR_BTNTEXT:COLOR_GRAYTEXT;
  RECT rect = *rtext;
  COLORREF oldMode;

  //CB: bug in Open32 DrawText: underscore is always black! -> two black lines!
  SetTextColor(hdc,GetSysColor(COLOR_3DHILIGHT));
  DrawTextA(hdc,text,-1,&rect,format);
  SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT));
  oldMode = SetBkMode(hdc,TRANSPARENT);
  OffsetRect(&rect,-1,-1);
  DrawTextA(hdc,text,-1,&rect,format);
  SetBkMode(hdc,oldMode);
}

/**********************************************************************
 *       Check Box & Radio Button Functions
 */

static void CB_Paint(HWND hwnd,HDC hDC,WORD action)
{
    BUTTONINFO* infoPtr = (BUTTONINFO*)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rbox, rtext, client;
    HBRUSH hBrush, hOldBrush;
    HPEN hPen, hOldPen;
    COLORREF colour;
    int textLen, delta;
    char* text = NULL;

    /*
     * if the button has a bitmap/icon, draw a normal pushbutton
     * instead of a radion button.
     */
    if (infoPtr->hImage != 0)
    {
        BOOL bHighLighted = ((infoPtr->state & BUTTON_HIGHLIGHTED) ||
                             (infoPtr->state & BUTTON_CHECKED));

        BUTTON_DrawPushButton(hwnd,
                              hDC,
                              action,
                              bHighLighted);
        return;
    }

    textLen = 0;
    GetClientRect(hwnd, &client);
    rbox = rtext = client;

    if (infoPtr->hFont) SelectObject( hDC, infoPtr->hFont );

    /* GetControlBrush16 sends WM_CTLCOLORBTN, plus it returns default brush
     * if parent didn't return valid one. So we kill two hares at once
     */
    hBrush = GetControlBrush( hwnd, hDC, CTLCOLOR_BTN );

    /* In order to make things right, draw the rectangle background! */
    if ( !(infoPtr->state & BUTTON_HASFOCUS) && (action == ODA_DRAWENTIRE) )
    {
        colour = GetBkColor(hDC);
        hPen = CreatePen( PS_SOLID, 1, colour );
        if ( hPen )
        {
            hOldBrush = SelectObject( hDC, hBrush );
            hOldPen = SelectObject( hDC, hPen );
            Rectangle( hDC, client.left, client.top, client.right, client.bottom );
            SelectObject( hDC, hOldPen );
            SelectObject( hDC, hOldBrush );
            DeleteObject( hPen );
        }
    }

    if (dwStyle & BS_LEFTTEXT)
    {
        /* magic +4 is what CTL3D expects */

        rtext.right -= checkBoxWidth + 4;
        //CB: space for focus rect
        rtext.left++;
        rtext.right++;
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
    if ((action == ODA_DRAWENTIRE) || (action == ODA_SELECT))
    {
        UINT state;

        if (((dwStyle & 0x0f) == BS_RADIOBUTTON) ||
            ((dwStyle & 0x0f) == BS_AUTORADIOBUTTON)) state = DFCS_BUTTONRADIO;
        else if (infoPtr->state & BUTTON_3STATE) state = DFCS_BUTTON3STATE;
        else state = DFCS_BUTTONCHECK;

        if (infoPtr->state & (BUTTON_CHECKED | BUTTON_3STATE)) state |= DFCS_CHECKED;

        if (infoPtr->state & BUTTON_HIGHLIGHTED) state |= DFCS_PUSHED;

        if (dwStyle & WS_DISABLED) state |= DFCS_INACTIVE;

        if (dwStyle & BS_FLAT) state |= DFCS_FLAT;

        DrawFrameControl( hDC, &rbox, DFC_BUTTON, state );

        if( text && action != ODA_SELECT )
        {
          INT format = BUTTON_GetTextFormat(dwStyle,GetWindowLongA(hwnd,GWL_EXSTYLE),DT_TOP,DT_VCENTER);

          if (dwStyle & WS_DISABLED) DrawDisabledText(hDC,text,&rtext,format);
          else DrawTextA(hDC,text,-1,&rtext,format);
        }
    }

    if ((action == ODA_FOCUS) ||
        ((action == ODA_DRAWENTIRE) && (infoPtr->state & BUTTON_HASFOCUS) && IsWindowEnabled(hwnd)))
    {
        /* again, this is what CTL3D expects */

        SetRectEmpty(&rbox);
        if(textLen > 0)
          DrawTextA(hDC,text,-1,&rbox,DT_SINGLELINE | DT_CALCRECT);
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
    //start = sibling = GetNextDlgGroupItem( parent, hwnd, TRUE );
    //@YD: bugfix
    //CB: doesn't work!
    start = sibling = GetNextDlgGroupItem( parent, hwnd, FALSE );
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
    HBRUSH hbr;

    if (action != ODA_DRAWENTIRE) return;

    /* GroupBox acts like static control, so it sends CTLCOLORSTATIC */
    hbr = GetControlBrush( hwnd, hDC, CTLCOLOR_STATIC );

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
      INT format = BUTTON_GetTextFormat(dwStyle,GetWindowLongA(hwnd,GWL_EXSTYLE),DT_LEFT,DT_TOP) | DT_NOCLIP | DT_SINGLELINE;

      textLen++;
      text = (char*)malloc(textLen);
      GetWindowTextA(hwnd,text,textLen);
      if (infoPtr->hFont) SelectObject( hDC, infoPtr->hFont );
      rc.left += 10;

      if (dwStyle & WS_DISABLED) {
           DrawDisabledText(hDC,text,&rc,format); 
      }
      else
      {
           SetTextColor(hDC,GetSysColor(COLOR_BTNTEXT));
           DrawTextA(hDC,text,-1,&rc,format);
      }

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
    hBrush = GetControlBrush( hwnd, hDC, CTLCOLOR_BTN );

    FillRect( hDC, &rc, hBrush );
    if ((action == ODA_FOCUS) ||
        ((action == ODA_DRAWENTIRE) && (infoPtr->state & BUTTON_HASFOCUS) && IsWindowEnabled(hwnd)))
    {
      DrawFocusRect( hDC, &rc );
      InflateRect(&rc,-1,-1);
    }
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

    SetBkColor( hDC, GetSysColor( COLOR_BTNFACE ) );

    dprintf(("OWNERDRAW button %x, enabled %d", hwnd, !(dwStyle & WS_DISABLED)));
    SendMessageA( GetParent(hwnd), WM_DRAWITEM,
                    GetWindowLongA(hwnd,GWL_ID), (LPARAM)&dis );
}

BOOL BUTTON_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(BUTTONCLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW | CS_PARENTDC | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)ButtonWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(BUTTONINFO);
    wndClass.hCursor       = LoadCursorA(0,IDC_ARROWA);
    wndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    wndClass.lpszClassName = BUTTONCLASSNAME;

    return RegisterClassA(&wndClass);
}

BOOL BUTTON_Unregister()
{
    if (GlobalFindAtomA(BUTTONCLASSNAME))
        return UnregisterClassA(BUTTONCLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}
