/* $Id: static.cpp,v 1.7 1999-10-24 22:56:07 sandervl Exp $ */
/*
 * Static control
 *
 * Copyright 1999 Christoph Bratschi (ported from WINE)
 *
 * Copyright  David W. Metcalfe, 1993
 *
 * WINE version: 990923
 */

#include <stdlib.h>
#include <os2win.h>
#include "controls.h"
#include "static.h"

//Prototypes

static void STATIC_PaintTextfn( HWND hwnd, HDC hdc );
static void STATIC_PaintRectfn( HWND hwnd, HDC hdc );
static void STATIC_PaintIconfn( HWND hwnd, HDC hdc );
static void STATIC_PaintBitmapfn( HWND hwnd, HDC hdc );
static void STATIC_PaintEtchedfn( HWND hwnd, HDC hdc );

static COLORREF color_windowframe, color_background, color_window;


typedef void (*pfPaint)( HWND, HDC );

static pfPaint staticPaintFunc[SS_TYPEMASK+1] =
{
    STATIC_PaintTextfn,      /* SS_LEFT */
    STATIC_PaintTextfn,      /* SS_CENTER */
    STATIC_PaintTextfn,      /* SS_RIGHT */
    STATIC_PaintIconfn,      /* SS_ICON */
    STATIC_PaintRectfn,      /* SS_BLACKRECT */
    STATIC_PaintRectfn,      /* SS_GRAYRECT */
    STATIC_PaintRectfn,      /* SS_WHITERECT */
    STATIC_PaintRectfn,      /* SS_BLACKFRAME */
    STATIC_PaintRectfn,      /* SS_GRAYFRAME */
    STATIC_PaintRectfn,      /* SS_WHITEFRAME */
    NULL,                    /* Not defined */
    STATIC_PaintTextfn,      /* SS_SIMPLE */
    STATIC_PaintTextfn,      /* SS_LEFTNOWORDWRAP */
    NULL,                    /* SS_OWNERDRAW */
    STATIC_PaintBitmapfn,    /* SS_BITMAP */
    NULL,                    /* SS_ENHMETAFILE */
    STATIC_PaintEtchedfn,    /* SS_ETCHEDHORIZ */
    STATIC_PaintEtchedfn,    /* SS_ETCHEDVERT */
    STATIC_PaintEtchedfn,    /* SS_ETCHEDFRAME */
};


/***********************************************************************
 *           STATIC_SetIcon
 *
 * Set the icon for an SS_ICON control.
 */
static HICON STATIC_SetIcon( HWND hwnd, HICON hicon )
{
    HICON prevIcon;
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    ICONINFO ii;
    BITMAP bmp;

    if ((dwStyle & SS_TYPEMASK) != SS_ICON) return 0;

    if (infoPtr->hIcon) DestroyIcon(infoPtr->hIcon);
    prevIcon = infoPtr->hIcon;
    infoPtr->hIcon = hicon;

    GetIconInfo(hicon,&ii);
    GetObjectA(ii.hbmColor,sizeof(BITMAP),(LPVOID)&bmp);

    SetWindowPos(hwnd,0,0,0,bmp.bmWidth,bmp.bmHeight,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );

    return prevIcon;
}

/***********************************************************************
 *           STATIC_SetBitmap
 *
 * Set the bitmap for an SS_BITMAP control.
 */
static HBITMAP STATIC_SetBitmap( HWND hwnd, HBITMAP hBitmap )
{
    HBITMAP hOldBitmap;
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    if ((dwStyle & SS_TYPEMASK) != SS_BITMAP) return 0;

    if (hBitmap && GetObjectType(hBitmap) != OBJ_BITMAP) {
        //ERR("huh? hBitmap!=0, but not bitmap\n");
        return 0;
    }
    hOldBitmap = infoPtr->hIcon;
    infoPtr->hIcon = hBitmap;
    if (hBitmap)
    {
        BITMAP bm;
        GetObjectA(hBitmap, sizeof(bm), &bm);
        SetWindowPos( hwnd, 0, 0, 0, bm.bmWidth, bm.bmHeight,
                      SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
    }
    return hOldBitmap;
}

/***********************************************************************
 *           STATIC_LoadIcon
 *
 * Load the icon for an SS_ICON control.
 */
static HICON STATIC_LoadIcon( HWND hwnd, LPCSTR name )
{
    HICON hicon;

    hicon = LoadIconA(GetWindowLongA(hwnd,GWL_HINSTANCE),name);

    if (!hicon)
        hicon = LoadIconA(0, name);

    return hicon;
}

/***********************************************************************
 *           STATIC_LoadBitmap
 *
 * Load the bitmap for an SS_BITMAP control.
 */
static HBITMAP STATIC_LoadBitmap( HWND hwnd, LPCSTR name )
{
    HBITMAP hbitmap;

    hbitmap = LoadBitmapA(GetWindowLongA(hwnd,GWL_HINSTANCE),name);

    if (!hbitmap)  /* Try OEM icon (FIXME: is this right?) */
        hbitmap = LoadBitmapA(0,name);

    return hbitmap;
}

/* message handler */

LRESULT STATIC_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  CREATESTRUCTA *cs = (CREATESTRUCTA *)lParam;
  STATICINFO* infoPtr;
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  DWORD style = dwStyle & SS_TYPEMASK;
  DWORD dwExStyle = GetWindowLongA(hwnd,GWL_EXSTYLE);

  infoPtr = (STATICINFO*)malloc(sizeof(STATICINFO));
  infoPtr->hFont = 0;
  infoPtr->dummy = 0;
  infoPtr->hIcon = 0;
  SetInfoPtr(hwnd,(DWORD)infoPtr);

  if (dwStyle & SS_SUNKEN)
  {
    dwExStyle |= WS_EX_STATICEDGE;
    SetWindowLongA(hwnd,GWL_EXSTYLE,dwExStyle);
  }

  if (style == SS_ICON)
  {
    if (cs->lpszName)
    {
      if (!HIWORD(cs->lpszName) || cs->lpszName[0])
        STATIC_SetIcon(hwnd,STATIC_LoadIcon(hwnd,cs->lpszName));
    }
    return TRUE;
  }
  if (style == SS_BITMAP)
  {
    if (cs->lpszName)
      STATIC_SetBitmap(hwnd,STATIC_LoadBitmap(hwnd,cs->lpszName));
    return TRUE;
  }
  if (!HIWORD(cs->lpszName) && (cs->lpszName)) return TRUE;

  return DefWindowProcA(hwnd,WM_NCCREATE,wParam,lParam);
}

LRESULT STATIC_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if (style < 0L || style > SS_TYPEMASK)
  {
    //Unknown style
    return (LRESULT)-1;
  }

  /* initialise colours */
  color_windowframe  = GetSysColor(COLOR_WINDOWFRAME);
  color_background   = GetSysColor(COLOR_BACKGROUND);
  color_window       = GetSysColor(COLOR_WINDOW);

  return 0;
}

LRESULT STATIC_NCDestroy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  STATICINFO* infoPtr = (STATICINFO*)GetInfoPtr(hwnd);
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if (style == SS_ICON && infoPtr->hIcon)
  {
    DestroyIcon(infoPtr->hIcon);
  } else if (style == SS_BITMAP && infoPtr->hIcon)
  {
    DeleteObject(infoPtr->hIcon);
  }

  free(infoPtr);

  return DefWindowProcA(hwnd,WM_NCDESTROY,wParam,lParam);
}

LRESULT STATIC_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;
  PAINTSTRUCT ps;

  BeginPaint(hwnd,&ps);
  if (staticPaintFunc[style]) (staticPaintFunc[style])(hwnd,ps.hdc);
  EndPaint(hwnd,&ps);

  return 0;
}

LRESULT STATIC_Enable(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  InvalidateRect(hwnd,NULL,FALSE);

  return 0;
}

LRESULT STATIC_SysColorChange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  color_windowframe  = GetSysColor(COLOR_WINDOWFRAME);
  color_background   = GetSysColor(COLOR_BACKGROUND);
  color_window       = GetSysColor(COLOR_WINDOW);

  InvalidateRect(hwnd,NULL,TRUE);

  return 0;
}

LRESULT STATIC_SetText(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if (style == SS_ICON)
    STATIC_SetIcon(hwnd,STATIC_LoadIcon(hwnd,(LPCSTR)lParam));
  else if (style == SS_BITMAP)
    STATIC_SetBitmap(hwnd,STATIC_LoadBitmap(hwnd,(LPCSTR)lParam));
  else
    DefWindowProcA(hwnd,WM_SETTEXT,wParam,lParam);

  InvalidateRect(hwnd,NULL,FALSE);
  UpdateWindow(hwnd);

  return 0;
}

LRESULT STATIC_SetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  STATICINFO* infoPtr = (STATICINFO*)GetInfoPtr(hwnd);
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if (style == SS_ICON) return 0;
  if (style == SS_BITMAP) return 0;

  infoPtr->hFont = (HFONT)wParam;

  if (LOWORD(lParam))
  {
    InvalidateRect(hwnd,NULL,FALSE);
    UpdateWindow(hwnd);
  }

  return 0;
}

LRESULT STATIC_GetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  STATICINFO* infoPtr = (STATICINFO*)GetInfoPtr(hwnd);

  return infoPtr->hFont;
}

LRESULT STATIC_NCHitTest(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return HTTRANSPARENT;
}

LRESULT STATIC_GetDlgCode(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return DLGC_STATIC;
}

LRESULT STATIC_GetIcon(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  STATICINFO* infoPtr = (STATICINFO*)GetInfoPtr(hwnd);

  return infoPtr->hIcon;
}

LRESULT STATIC_SetImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LRESULT lResult;

  switch (wParam)
  {
    case IMAGE_CURSOR:
    case IMAGE_ICON:
      lResult = STATIC_SetIcon(hwnd,(HICON)lParam);
      break;
    case IMAGE_BITMAP:
      lResult = STATIC_SetBitmap(hwnd,(HBITMAP)lParam);
    case IMAGE_ENHMETAFILE:
      return 0; //CB: not supported!
    default:
      return 0;
  }

  InvalidateRect(hwnd,NULL,FALSE);
  UpdateWindow(hwnd);

  return lResult;
}

LRESULT STATIC_SetIcon(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LRESULT lResult;

  lResult = STATIC_SetIcon(hwnd,(HICON)wParam);

  InvalidateRect(hwnd,NULL,FALSE);
  UpdateWindow(hwnd);

  return lResult;
}

/***********************************************************************
 *           StaticWndProc
 */
LRESULT WINAPI StaticWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_NCCREATE:
      return STATIC_NCCreate(hwnd,wParam,lParam);

    case WM_CREATE:
      return STATIC_Create(hwnd,wParam,lParam);

    case WM_NCDESTROY:
      return STATIC_NCDestroy(hwnd,wParam,lParam);

    case WM_PAINT:
      return STATIC_Paint(hwnd,wParam,lParam);

    case WM_ENABLE:
      return STATIC_Enable(hwnd,wParam,lParam);

    case WM_SYSCOLORCHANGE:
      return STATIC_SysColorChange(hwnd,wParam,lParam);

    case WM_SETTEXT:
      return STATIC_SetText(hwnd,wParam,lParam);

    case WM_SETFONT:
      return STATIC_SetFont(hwnd,wParam,lParam);

    case WM_GETFONT:
      return STATIC_GetFont(hwnd,wParam,lParam);

    case WM_NCHITTEST:
      return STATIC_NCHitTest(hwnd,wParam,lParam);

    case WM_GETDLGCODE:
      return STATIC_GetDlgCode(hwnd,wParam,lParam);

    case STM_GETIMAGE:
    case STM_GETICON:
      return STATIC_GetIcon(hwnd,wParam,lParam);

    case STM_SETIMAGE:
      return STATIC_SetImage(hwnd,wParam,lParam);

    case STM_SETICON:
      return STATIC_SetIcon(hwnd,wParam,lParam);

    default:
      return DefWindowProcA(hwnd,uMsg,wParam,lParam);
      break;
  }

  return DefWindowProcA(hwnd,uMsg,wParam,lParam);
}


static void STATIC_PaintTextfn(HWND hwnd, HDC hdc )
{
    RECT rc;
    HBRUSH hBrush;
    WORD wFormat;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    LONG style = dwStyle;
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);
    INT textLen;

    GetClientRect(hwnd,&rc);

    switch (style & SS_TYPEMASK)
    {
    case SS_LEFT:
        wFormat = DT_LEFT | DT_EXPANDTABS | DT_WORDBREAK | DT_NOCLIP;
        break;

    case SS_CENTER:
        wFormat = DT_CENTER | DT_EXPANDTABS | DT_WORDBREAK | DT_NOCLIP;
        break;

    case SS_RIGHT:
        wFormat = DT_RIGHT | DT_EXPANDTABS | DT_WORDBREAK | DT_NOCLIP;
        break;

    case SS_SIMPLE:
        wFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP;
        break;

    case SS_LEFTNOWORDWRAP:
        wFormat = DT_LEFT | DT_EXPANDTABS | DT_VCENTER;
        break;

    default:
        return;
    }

    if (style & SS_NOPREFIX)
        wFormat |= DT_NOPREFIX;

    if (infoPtr->hFont) SelectObject( hdc, infoPtr->hFont );
    hBrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
    if (!hBrush) hBrush = GetStockObject(WHITE_BRUSH);
    FillRect( hdc, &rc, hBrush );

    if (!IsWindowEnabled(hwnd)) SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT));

    textLen = GetWindowTextLengthA(hwnd);
    if (textLen > 0)
    {
      char* text;

      textLen++;
      text = (char*)malloc(textLen);
      GetWindowTextA(hwnd,text,textLen);
      DrawTextA( hdc, text, -1, &rc, wFormat );
      free(text);
    }
}

static void STATIC_PaintRectfn( HWND hwnd, HDC hdc )
{
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rc;
    HBRUSH hBrush;

    GetClientRect( hwnd, &rc);

    switch (dwStyle & SS_TYPEMASK)
    {
    case SS_BLACKRECT:
        hBrush = CreateSolidBrush(color_windowframe);
        FillRect( hdc, &rc, hBrush );
        break;
    case SS_GRAYRECT:
        hBrush = CreateSolidBrush(color_background);
        FillRect( hdc, &rc, hBrush );
        break;
    case SS_WHITERECT:
        hBrush = CreateSolidBrush(color_window);
        FillRect( hdc, &rc, hBrush );
        break;
    case SS_BLACKFRAME:
        hBrush = CreateSolidBrush(color_windowframe);
        FrameRect( hdc, &rc, hBrush );
        break;
    case SS_GRAYFRAME:
        hBrush = CreateSolidBrush(color_background);
        FrameRect( hdc, &rc, hBrush );
        break;
    case SS_WHITEFRAME:
        hBrush = CreateSolidBrush(color_window);
        FrameRect( hdc, &rc, hBrush );
        break;
    default:
        return;
    }
    DeleteObject( hBrush );
}


static void STATIC_PaintIconfn( HWND hwnd, HDC hdc )
{
    RECT rc;
    HBRUSH hbrush;
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);

    GetClientRect( hwnd, &rc );
    hbrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
    FillRect( hdc, &rc, hbrush );
    if (infoPtr->hIcon) DrawIcon( hdc, rc.left, rc.top, infoPtr->hIcon );
}

static void STATIC_PaintBitmapfn(HWND hwnd, HDC hdc )
{
    RECT rc;
    HBRUSH hbrush;
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);
    HDC hMemDC;
    HBITMAP oldbitmap;

    GetClientRect( hwnd, &rc );
    hbrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
    FillRect( hdc, &rc, hbrush );

    if (infoPtr->hIcon) {
        BITMAP bm;
        SIZE sz;

        if(GetObjectType(infoPtr->hIcon) != OBJ_BITMAP)
            return;
        if (!(hMemDC = CreateCompatibleDC( hdc ))) return;
        GetObjectA(infoPtr->hIcon, sizeof(bm), &bm);
        GetBitmapDimensionEx(infoPtr->hIcon, &sz);
        oldbitmap = SelectObject(hMemDC, infoPtr->hIcon);
        BitBlt(hdc, sz.cx, sz.cy, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0,
               SRCCOPY);
        SelectObject(hMemDC, oldbitmap);
        DeleteDC(hMemDC);
    }
}


static void STATIC_PaintEtchedfn( HWND hwnd, HDC hdc )
{
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rc;
    HBRUSH hbrush;
    HPEN hpen;

    GetClientRect( hwnd, &rc );
    hbrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
    FillRect( hdc, &rc, hbrush );

    switch (dwStyle & SS_TYPEMASK)
    {
        case SS_ETCHEDHORZ:
            hpen = SelectObject (hdc, GetSysColorPen (COLOR_3DSHADOW));
            MoveToEx (hdc, rc.left, rc.bottom / 2 - 1, NULL);
            LineTo (hdc, rc.right - 1, rc.bottom / 2 - 1);
            SelectObject (hdc, GetSysColorPen (COLOR_3DHIGHLIGHT));
            MoveToEx (hdc, rc.left, rc.bottom / 2, NULL);
            LineTo (hdc, rc.right, rc.bottom / 2);
            LineTo (hdc, rc.right, rc.bottom / 2 - 1);
            SelectObject (hdc, hpen);
            break;

        case SS_ETCHEDVERT:
            hpen = SelectObject (hdc, GetSysColorPen (COLOR_3DSHADOW));
            MoveToEx (hdc, rc.right / 2 - 1, rc.top, NULL);
            LineTo (hdc, rc.right / 2 - 1, rc.bottom - 1);
            SelectObject (hdc, GetSysColorPen (COLOR_3DHIGHLIGHT));
            MoveToEx (hdc, rc.right / 2, rc.top, NULL);
            LineTo (hdc, rc.right / 2, rc.bottom);
            LineTo (hdc, rc.right / 2 -1 , rc.bottom);
            SelectObject (hdc, hpen);
            break;

        case SS_ETCHEDFRAME:
            DrawEdge (hdc, &rc, EDGE_ETCHED, BF_RECT);
            break;
    }
}

BOOL STATIC_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(STATICCLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_HREDRAW | CS_PARENTDC;
    wndClass.lpfnWndProc   = (WNDPROC)StaticWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(STATICINFO);
    wndClass.hCursor       = LoadCursorA (0,IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)0;
    wndClass.lpszClassName = STATICCLASSNAME;

    return RegisterClassA(&wndClass);
}


BOOL STATIC_Unregister()
{
    if (GlobalFindAtomA (STATICCLASSNAME))
        return UnregisterClassA(STATICCLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}
