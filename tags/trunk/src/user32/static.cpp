/* $Id: static.cpp,v 1.24 2001-05-11 08:39:44 sandervl Exp $ */
/*
 * Static control
 *
 * Copyright 1999 Christoph Bratschi
 *
 * Copyright  David W. Metcalfe, 1993
 *
 * Corel version: 20000317
 * (WINE version: 990923)
 *
 * SvL:
 * SS_REALSIZEIMAGE is supposed to prevent control resizing, but that doesn't
 * appear to happen in NT (although the SDK docs are very clear about this)
 * Need to figure out why. (not resizing messes up the bitmap control in
 * the Microsoft Visual C++ 4.2 install program)
 * Same thing happens with a very simply test app, so the installer
 * does nothing special.
 *
 *
 * Status:  complete
 * Version: 5.00
 */

#include <stdlib.h>
#include <string.h>
#include <os2win.h>
#include "controls.h"
#include "static.h"

#define DBG_LOCALLOG    DBG_static
#include "dbglocal.h"

//Prototypes

static void STATIC_PaintTextfn( HWND hwnd, HDC hdc );
static void STATIC_PaintRectfn( HWND hwnd, HDC hdc );
static void STATIC_PaintIconfn( HWND hwnd, HDC hdc );
static void STATIC_PaintBitmapfn( HWND hwnd, HDC hdc );
static void STATIC_PaintMetafilefn(HWND hwnd,HDC hdc);
static void STATIC_PaintOwnerDrawfn(HWND hwnd,HDC hdc);
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
    NULL,                    /* SS_USERITEM */
    STATIC_PaintTextfn,      /* SS_SIMPLE */
    STATIC_PaintTextfn,      /* SS_LEFTNOWORDWRAP */
    STATIC_PaintOwnerDrawfn, /* SS_OWNERDRAW */
    STATIC_PaintBitmapfn,    /* SS_BITMAP */
    STATIC_PaintMetafilefn,  /* SS_ENHMETAFILE */
    STATIC_PaintEtchedfn,    /* SS_ETCHEDHORIZ */
    STATIC_PaintEtchedfn,    /* SS_ETCHEDVERT */
    STATIC_PaintEtchedfn,    /* SS_ETCHEDFRAME */
};

static void STATIC_ResizeWindow(HWND hwnd,DWORD dwStyle,INT w,INT h)
{
  if (dwStyle & SS_RIGHTJUST)
  {
    RECT rect;

    GetWindowRect(hwnd,&rect);
    SetWindowPos(hwnd,0,rect.right-w,rect.bottom-h,w,h,SWP_NOACTIVATE | SWP_NOZORDER);
  } else SetWindowPos(hwnd,0,0,0,w,h,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}

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

    if (infoPtr == NULL)
      return 0;

    if ((dwStyle & SS_TYPEMASK) != SS_ICON) return 0;

    if (infoPtr->hIcon) DestroyIcon(infoPtr->hIcon);
    prevIcon = infoPtr->hIcon;
    infoPtr->hIcon = hicon;

    if (!GetIconInfo(hicon,&ii)) return prevIcon;
    if (ii.hbmColor)
      GetObjectA(ii.hbmColor,sizeof(BITMAP),(LPVOID)&bmp);
    else
    {
      GetObjectA(ii.hbmMask,sizeof(BITMAP),(LPVOID)&bmp);
      bmp.bmHeight /= 2;
    }

#if 1
    //SvL: Breaks MS Visual C++ install dialogs with static controls (no resize)
    if (!(dwStyle & (SS_CENTERIMAGE))) STATIC_ResizeWindow(hwnd,dwStyle,bmp.bmWidth,bmp.bmHeight);
#else
    if (!(dwStyle & (SS_CENTERIMAGE | SS_REALSIZEIMAGE))) STATIC_ResizeWindow(hwnd,dwStyle,bmp.bmWidth,bmp.bmHeight);
#endif

    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);

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

    if (infoPtr == NULL)
      return 0;

    if ((dwStyle & SS_TYPEMASK) != SS_BITMAP) return 0;

    if (hBitmap && (GetObjectType(hBitmap) != OBJ_BITMAP)) {
        //ERR("huh? hBitmap!=0, but not bitmap\n");
        return 0;
    }
    hOldBitmap = infoPtr->hIcon;
    infoPtr->hIcon = hBitmap;
#if 1
    //SvL: Breaks MS Visual C++ install dialogs with static controls (no resize)
    if (hBitmap && !(dwStyle & (SS_CENTERIMAGE)))
#else
    if (hBitmap && !(dwStyle & (SS_CENTERIMAGE | SS_REALSIZEIMAGE)))
#endif
    {
      BITMAP bm;

      GetObjectA(hBitmap,sizeof(bm),&bm);
      STATIC_ResizeWindow(hwnd,dwStyle,bm.bmWidth,bm.bmHeight);
    }

    return hOldBitmap;
}

static HENHMETAFILE STATIC_SetMetafile(HWND hwnd,HENHMETAFILE hMetafile)
{
  HENHMETAFILE hOldMetafile;

  STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if ((dwStyle & SS_TYPEMASK) != SS_ENHMETAFILE) return 0;

  hOldMetafile = infoPtr->hIcon;
  infoPtr->hIcon = hMetafile;

  return hOldMetafile;
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

static HBITMAP STATIC_LoadMetafile(HWND hwnd,LPCSTR name)
{
  HENHMETAFILE hMetafile;

  hMetafile = GetEnhMetaFileA(name); //CB: right?

  return hMetafile;
}

/* message handler */

LRESULT STATIC_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  CREATESTRUCTA *cs = (CREATESTRUCTA*)lParam;
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
    if (cs->lpszName) //CB: is 0 a valid icon id? winhlp32: lpszName = NULL
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
  if (style == SS_ENHMETAFILE)
  {
    if (cs->lpszName) STATIC_SetMetafile(hwnd,STATIC_LoadMetafile(hwnd,cs->lpszName));
    return TRUE;
  }
  if (!HIWORD(cs->lpszName) && (cs->lpszName)) return TRUE;

  return DefWindowProcA(hwnd,WM_NCCREATE,wParam,lParam);
}

LRESULT STATIC_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if ((style < 0L) || (style > SS_TYPEMASK))
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

  if ((style == SS_ICON) && infoPtr->hIcon)
  {
    DestroyIcon(infoPtr->hIcon);
  } else if ((style == SS_BITMAP) && infoPtr->hIcon)
  {
    DeleteObject(infoPtr->hIcon);
  } else if ((style == SS_ENHMETAFILE) && infoPtr->hIcon)
  {
    DeleteEnhMetaFile((HENHMETAFILE)infoPtr->hIcon);
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
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if (dwStyle & SS_NOTIFY) SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),wParam ? STN_ENABLE:STN_DISABLE),hwnd);

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
  else if (style == SS_ENHMETAFILE)
    STATIC_SetMetafile(hwnd,STATIC_LoadMetafile(hwnd,(LPCSTR)lParam));
  else
    DefWindowProcA(hwnd,WM_SETTEXT,wParam,lParam);

  InvalidateRect(hwnd,NULL,FALSE);

  return TRUE;
}

LRESULT STATIC_GetText(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if (style == SS_ICON)
  {
    STATICINFO* infoPtr = (STATICINFO*)GetInfoPtr(hwnd);

    if ((wParam < 4) || !lParam) return 0;
    memcpy((VOID*)lParam,&infoPtr->hIcon,4);

    return 4;
  } else return DefWindowProcA(hwnd,WM_GETTEXT,wParam,lParam);
}

LRESULT STATIC_GetTextLength(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if (style == SS_ICON) return 4;
  else return DefWindowProcA(hwnd,WM_GETTEXTLENGTH,wParam,lParam);
}

LRESULT STATIC_SetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  STATICINFO* infoPtr = (STATICINFO*)GetInfoPtr(hwnd);
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  if (style == SS_ICON) return 0;
  if (style == SS_BITMAP) return 0;
  if (style == SS_ENHMETAFILE) return 0;

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
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  LRESULT lResult;

  if (dwStyle & SS_NOTIFY)
       lResult = HTCLIENT;
  else lResult = HTTRANSPARENT;

  return lResult;
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

LRESULT STATIC_GetImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  STATICINFO* infoPtr = (STATICINFO*)GetInfoPtr(hwnd);
  DWORD style = GetWindowLongA(hwnd,GWL_STYLE) & SS_TYPEMASK;

  switch (wParam)
  {
    case IMAGE_BITMAP:
      if (style & SS_BITMAP) return infoPtr->hIcon;
      break;

    case IMAGE_CURSOR:
    case IMAGE_ICON:
      if (style & SS_ICON) return infoPtr->hIcon;
      break;

    case IMAGE_ENHMETAFILE:
      if (style & SS_ENHMETAFILE) return infoPtr->hIcon;
      break;

    default:
      break;
  }

  return 0;
}

LRESULT STATIC_SetImage(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LRESULT lResult = 0;

  switch (wParam)
  {
    case IMAGE_CURSOR:
    case IMAGE_ICON:
      lResult = STATIC_SetIcon(hwnd,(HICON)lParam);
      break;

    case IMAGE_BITMAP:
      lResult = STATIC_SetBitmap(hwnd,(HBITMAP)lParam);
      break;

    case IMAGE_ENHMETAFILE:
      lResult = STATIC_SetMetafile(hwnd,(HENHMETAFILE)lParam);
      break;

    default:
      return 0;
  }

  if (lResult) InvalidateRect(hwnd,NULL,FALSE);

  return lResult;
}

LRESULT STATIC_SetIconMsg(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  LRESULT lResult;

  lResult = STATIC_SetIcon(hwnd,(HICON)wParam);

  InvalidateRect(hwnd,NULL,FALSE);

  return lResult;
}

LRESULT STATIC_Click(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if (dwStyle & SS_NOTIFY) SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),STN_CLICKED),hwnd);

  return DefWindowProcA(hwnd,uMsg,wParam,lParam);
}

LRESULT STATIC_DoubleClick(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if (dwStyle & SS_NOTIFY) SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),STN_DBLCLK),hwnd);

  return DefWindowProcA(hwnd,uMsg,wParam,lParam);
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

    case WM_GETTEXT:
      return STATIC_GetText(hwnd,wParam,lParam);

    case WM_GETTEXTLENGTH:
      return STATIC_GetTextLength(hwnd,wParam,lParam);

    case WM_SETFONT:
      return STATIC_SetFont(hwnd,wParam,lParam);

    case WM_GETFONT:
      return STATIC_GetFont(hwnd,wParam,lParam);

    case WM_NCHITTEST:
      return STATIC_NCHitTest(hwnd,wParam,lParam);

    case WM_GETDLGCODE:
      return STATIC_GetDlgCode(hwnd,wParam,lParam);

    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
      return STATIC_Click(hwnd,uMsg,wParam,lParam);

    case WM_LBUTTONDBLCLK:
    case WM_NCLBUTTONDBLCLK:
      return STATIC_DoubleClick(hwnd,uMsg,wParam,lParam);

    case STM_GETIMAGE:
      return STATIC_GetImage(hwnd,wParam,lParam);

    case STM_GETICON:
      return STATIC_GetIcon(hwnd,wParam,lParam);

    case STM_SETIMAGE:
      return STATIC_SetImage(hwnd,wParam,lParam);

    case STM_SETICON:
      return STATIC_SetIconMsg(hwnd,wParam,lParam);

    case STM_MSGMAX:
      return 0; //CB: undocumented!

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
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);
    INT textLen;

    GetClientRect(hwnd,&rc);

    switch (dwStyle & SS_TYPEMASK)
    {
      case SS_LEFT:
        wFormat = DT_LEFT | DT_EXPANDTABS | DT_WORDBREAK | DT_NOCLIP;
        break;

      case SS_CENTER:
      case SS_CENTERIMAGE:
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

    if (dwStyle & SS_NOPREFIX) wFormat |= DT_NOPREFIX;
    if (dwStyle & SS_ENDELLIPSIS) wFormat |= DT_END_ELLIPSIS;
    if (dwStyle & SS_PATHELLIPSIS) wFormat |= DT_PATH_ELLIPSIS;
    if (dwStyle & SS_WORDELLIPSIS) wFormat |= DT_WORD_ELLIPSIS;

    if (infoPtr->hFont) SelectObject( hdc, infoPtr->hFont );
//SvL: This is what the latest Wine code does. Fixes the common color dialog
#if 1
    if ((dwStyle & SS_NOPREFIX) || ((dwStyle & SS_TYPEMASK) != SS_SIMPLE))
    {
        hBrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
        if (!hBrush) hBrush = GetStockObject(WHITE_BRUSH);
        FillRect( hdc, &rc, hBrush );
    }
#else
    hBrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
    if (!hBrush) hBrush = GetStockObject(WHITE_BRUSH);
    FillRect( hdc, &rc, hBrush );
#endif

    if (!IsWindowEnabled(hwnd)) SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT));

    textLen = GetWindowTextLengthA(hwnd);
    if (textLen > 0)
    {
      char* text;

      textLen++;
      text = (char*)malloc(textLen);
      GetWindowTextA(hwnd,text,textLen);
      DrawTextExA(hdc,text,-1,&rc,wFormat,NULL);
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
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    GetClientRect( hwnd, &rc );
    hbrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
    FillRect( hdc, &rc, hbrush );
    if (dwStyle & SS_CENTERIMAGE)
    {
      ICONINFO ii;
      BITMAP bmp;

      if (!GetIconInfo(infoPtr->hIcon,&ii)) return;
      if (ii.hbmColor)
        GetObjectA(ii.hbmColor,sizeof(BITMAP),(LPVOID)&bmp);
      else
      {
        GetObjectA(ii.hbmMask,sizeof(BITMAP),(LPVOID)&bmp);
        bmp.bmHeight /= 2;
      }
      DrawIcon(hdc,(rc.right-bmp.bmWidth)/2,(rc.bottom-bmp.bmHeight)/2,infoPtr->hIcon);
      if (ii.hbmColor) DeleteObject(ii.hbmColor);
      if (ii.hbmMask) DeleteObject(ii.hbmMask);
    } else if (infoPtr->hIcon) DrawIcon(hdc,rc.left,rc.top,infoPtr->hIcon);
}

static void STATIC_PaintBitmapfn(HWND hwnd, HDC hdc )
{
    RECT rc;
    HBRUSH hbrush;
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    HDC hMemDC;
    HBITMAP oldbitmap;

    GetClientRect( hwnd, &rc );
    hbrush = SendMessageA( GetParent(hwnd), WM_CTLCOLORSTATIC,
                             hdc, hwnd );
    FillRect( hdc, &rc, hbrush );

    if (infoPtr->hIcon)
    {
        BITMAP bm;

        if(GetObjectType(infoPtr->hIcon) != OBJ_BITMAP) return;
        if (!(hMemDC = CreateCompatibleDC( hdc ))) return;

        GetObjectA(infoPtr->hIcon, sizeof(bm), &bm);
        oldbitmap = SelectObject(hMemDC, infoPtr->hIcon);

        // Paint the image in center area
        if(dwStyle & SS_CENTERIMAGE)
        {
            SIZE szbm;
            SIZE szdc;

            if(bm.bmWidth > rc.right - rc.left)
            {
                szdc.cx = 0;
                szbm.cx = (bm.bmWidth - (rc.right - rc.left)) / 2;
                bm.bmWidth = rc.right - rc.left;
            }
            else
            {
                szbm.cx = 0;
                szdc.cx = ((rc.right - rc.left) - bm.bmWidth) / 2;
            }
            if(bm.bmHeight > rc.bottom - rc.top)
            {
                szdc.cy = 0;
                szbm.cy = (bm.bmHeight - (rc.bottom - rc.top)) / 2;
                bm.bmWidth = rc.bottom - rc.top;
            }
            else
            {
                szbm.cy = 0;
                szdc.cy = ((rc.bottom - rc.top) - bm.bmHeight) / 2;
            }
            BitBlt(hdc, szdc.cx, szdc.cy, bm.bmWidth, bm.bmHeight, hMemDC,
                szbm.cx, szbm.cy, SRCCOPY);
        }
        else
        {
            BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
        }

        SelectObject(hMemDC, oldbitmap);
        DeleteDC(hMemDC);
    }
}

static void STATIC_PaintMetafilefn(HWND hwnd,HDC hdc)
{
    RECT rect;
    HBRUSH hbrush;
    STATICINFO *infoPtr = (STATICINFO *)GetInfoPtr(hwnd);

    GetClientRect(hwnd,&rect);
    hbrush = SendMessageA(GetParent(hwnd),WM_CTLCOLORSTATIC,hdc,hwnd);
    FillRect(hdc,&rect,hbrush);

    if (infoPtr->hIcon) PlayEnhMetaFile(hdc,(HENHMETAFILE)infoPtr->hIcon,&rect);
}

static void STATIC_PaintOwnerDrawfn(HWND hwnd,HDC hdc)
{
  DRAWITEMSTRUCT di;

  di.CtlType    = ODT_STATIC;
  di.CtlID      = GetWindowLongA(hwnd,GWL_ID);
  di.itemID     = 0;
  di.itemAction = ODA_DRAWENTIRE;
  di.itemState  = ODS_DEFAULT;
  di.hwndItem   = hwnd;
  di.hDC        = hdc;
  GetClientRect(hwnd,&di.rcItem);
  di.itemData   = 0;

  SendMessageA(GetParent(hwnd),WM_CTLCOLORSTATIC,hdc,hwnd);
  SendMessageA(GetParent(hwnd),WM_DRAWITEM,di.CtlID,(LPARAM)&di);
}

static void STATIC_PaintEtchedfn( HWND hwnd, HDC hdc )
{
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    RECT rc;

    GetClientRect( hwnd, &rc );
    switch (dwStyle & SS_TYPEMASK)
    {
        case SS_ETCHEDHORZ:
            DrawEdge(hdc,&rc,EDGE_ETCHED,BF_TOP|BF_BOTTOM);
            break;
        case SS_ETCHEDVERT:
            DrawEdge(hdc,&rc,EDGE_ETCHED,BF_LEFT|BF_RIGHT);
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
