/* $Id: win32wbasepaint.cpp,v 1.1 2000-01-09 14:38:30 sandervl Exp $ */
/*
 * Win32 Window Base Class for OS/2
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Based on Wine code (windows\nonclient.c)
 *
 * Copyright 1994 Alexandre Julliard
 *
 * TODO: Not thread/process safe
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <win.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <heapstring.h>
#include <win32wbase.h>
#include <winres.h>
#include "wndmsg.h"
#include "oslibwin.h"
#include "oslibmsg.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibmenu.h"
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "dc.h"
#include "pmframe.h"
#include "pmtitlebar.h"
#include "win32wdesktop.h"
#include "pmwindow.h"
#include "controls.h"

#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_THICKFRAME)))

#define HAS_THICKFRAME(style,exStyle) \
    (((style) & WS_THICKFRAME) && \
     !((exStyle) & WS_EX_DLGMODALFRAME))

#define HAS_THINFRAME(style) \
    (((style) & WS_BORDER) || !((style) & (WS_CHILD | WS_POPUP)))

#define HAS_BIGFRAME(style,exStyle) \
    (((style) & (WS_THICKFRAME | WS_DLGFRAME)) || \
     ((exStyle) & WS_EX_DLGMODALFRAME))

#define HAS_ANYFRAME(style,exStyle) \
    (((style) & (WS_THICKFRAME | WS_DLGFRAME | WS_BORDER)) || \
     ((exStyle) & WS_EX_DLGMODALFRAME) || \
     !((style) & (WS_CHILD | WS_POPUP)))

#define HAS_3DFRAME(exStyle) \
    ((exStyle & WS_EX_CLIENTEDGE) || (exStyle & WS_EX_STATICEDGE) || (exStyle & WS_EX_WINDOWEDGE))

#define HAS_BORDER(style, exStyle) \
    ((style & WS_BORDER) || HAS_THICKFRAME(style) || HAS_DLGFRAME(style,exStyle))

#define IS_OVERLAPPED(style) \
    !(style & (WS_CHILD | WS_POPUP))

#define HAS_MENU(w) (!((w)->getStyle() & WS_CHILD) && ((w)->getWindowId() != 0))

static HBITMAP hbitmapClose = 0;
static HBITMAP hbitmapCloseD = 0;
static HBITMAP hbitmapMinimize = 0;
static HBITMAP hbitmapMinimizeD = 0;
static HBITMAP hbitmapMaximize = 0;
static HBITMAP hbitmapMaximizeD = 0;
static HBITMAP hbitmapRestore = 0;
static HBITMAP hbitmapRestoreD = 0;

BYTE lpGrayMask[] = { 0xAA, 0xA0,
                      0x55, 0x50,
                      0xAA, 0xA0,
                      0x55, 0x50,
                      0xAA, 0xA0,
                      0x55, 0x50,
                      0xAA, 0xA0,
                      0x55, 0x50,
                      0xAA, 0xA0,
                      0x55, 0x50};

//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::TrackMinMaxBox(WORD wParam)
{
  MSG msg;
  HDC hdc;
  BOOL pressed = TRUE;
  UINT state;

  if (wParam == HTMINBUTTON)
  {
    /* If the style is not present, do nothing */
    if (!(dwStyle & WS_MINIMIZEBOX))
      return;
    /* Check if the sysmenu item for minimize is there  */
#if 0 //CB: todo
    state = GetMenuState(hSysMenu,SC_MINIMIZE,MF_BYCOMMAND);
#endif
  } else
  {
    /* If the style is not present, do nothing */
    if (!(dwStyle & WS_MAXIMIZEBOX))
      return;
    /* Check if the sysmenu item for maximize is there  */
#if 0 //CB: todo
    state = GetMenuState(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
#endif
  }
  SetCapture(Win32Hwnd);
  hdc = GetWindowDC(Win32Hwnd);
  if (wParam == HTMINBUTTON)
    DrawMinButton(hdc,TRUE,FALSE);
  else
    DrawMaxButton(hdc,TRUE,FALSE);

  do
  {
    BOOL oldstate = pressed;

    GetMessageA(&msg,Win32Hwnd,0,0);
    pressed = (HandleNCHitTest(msg.pt) == wParam);
    if (pressed != oldstate)
    {
      if (wParam == HTMINBUTTON)
        DrawMinButton(hdc,pressed,FALSE);
      else
        DrawMaxButton(hdc,pressed,FALSE);
    }
  }

  while (msg.message != WM_LBUTTONUP);

  if (wParam == HTMINBUTTON)
    DrawMinButton(hdc,FALSE,FALSE);
  else
    DrawMaxButton(hdc,FALSE,FALSE);

  ReleaseCapture();
  ReleaseDC(Win32Hwnd,hdc);
  /* If the item minimize or maximize of the sysmenu are not there */
  /* or if the style is not present, do nothing */
  if ((!pressed) || (state == 0xFFFFFFFF))
    return;

  if (wParam == HTMINBUTTON)
    SendInternalMessageA(WM_SYSCOMMAND,SC_MINIMIZE,*(LPARAM*)&msg.pt);
  else
    SendInternalMessageA(WM_SYSCOMMAND,IsZoomed(Win32Hwnd) ? SC_RESTORE:SC_MAXIMIZE,*(LPARAM*)&msg.pt);
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::TrackCloseButton(WORD wParam)
{
  MSG msg;
  HDC hdc;
  BOOL pressed = TRUE;
  UINT state;

#if 0 //CB: todo
  if (hSysMenu == 0)
    return;
  state = GetMenuState(hSysMenu, SC_CLOSE, MF_BYCOMMAND);
#else
state = 0;
#endif
  /* If the item close of the sysmenu is disabled or not there do nothing */
  if((state & MF_DISABLED) || (state & MF_GRAYED) || (state == 0xFFFFFFFF))
    return;
  hdc = GetWindowDC(Win32Hwnd);
  SetCapture(Win32Hwnd);
  DrawCloseButton(hdc,TRUE,FALSE);
  do
  {
    BOOL oldstate = pressed;

    GetMessageA(&msg,Win32Hwnd,0,0);
    pressed = (HandleNCHitTest(msg.pt) == wParam);
    if (pressed != oldstate)
      DrawCloseButton(hdc, pressed, FALSE);
  } while (msg.message != WM_LBUTTONUP);
  DrawCloseButton(hdc,FALSE,FALSE);
  ReleaseCapture();
  ReleaseDC(Win32Hwnd,hdc);
  if (!pressed) return;
  SendInternalMessageA(WM_SYSCOMMAND,SC_CLOSE,*(LPARAM*)&msg.pt);
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCLButtonUp(WPARAM wParam,LPARAM lParam)
{
  switch(wParam)  /* Hit test */
  {
    case HTMINBUTTON:
      SendInternalMessageA(WM_SYSCOMMAND,SC_MINIMIZE,lParam);
      break;

    case HTMAXBUTTON:
      SendInternalMessageA(WM_SYSCOMMAND,SC_MAXIMIZE,lParam);
      break;

    case HTCLOSE:
      SendInternalMessageA(WM_SYSCOMMAND,SC_CLOSE,lParam);
      break;
  }

  return 0;
}
/***********************************************************************
 *           NC_HandleNCLButtonDblClk
 *
 * Handle a WM_NCLBUTTONDBLCLK message. Called from DefWindowProc().
 */
LONG Win32BaseWindow::HandleNCLButtonDblClk(WPARAM wParam,LPARAM lParam)
{
  /*
   * if this is an icon, send a restore since we are handling
   * a double click
   */
  if (dwStyle & WS_MINIMIZE)
  {
    SendInternalMessageA(WM_SYSCOMMAND,SC_RESTORE,lParam);
    return 0;
  }

  switch(wParam)  /* Hit test */
  {
    case HTCAPTION:
      /* stop processing if WS_MAXIMIZEBOX is missing */
      if (dwStyle & WS_MAXIMIZEBOX)
        SendInternalMessageA(WM_SYSCOMMAND,
                      (dwStyle & WS_MAXIMIZE) ? SC_RESTORE : SC_MAXIMIZE,
                      lParam);
      break;

    case HTSYSMENU:
      if (!(GetClassWord(Win32Hwnd,GCW_STYLE) & CS_NOCLOSE))
        SendInternalMessageA(WM_SYSCOMMAND,SC_CLOSE,lParam);
      break;

    case HTHSCROLL:
      SendInternalMessageA(WM_SYSCOMMAND,SC_HSCROLL+HTHSCROLL,lParam);
      break;

    case HTVSCROLL:
      SendInternalMessageA(WM_SYSCOMMAND,SC_VSCROLL+HTVSCROLL,lParam);
      break;
  }

  return 0;
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCLButtonDown(WPARAM wParam,LPARAM lParam)
{
  switch(wParam)  /* Hit test */
  {
    case HTCAPTION:
      SetActiveWindow();
      if (GetActiveWindow() == Win32Hwnd)
        SendInternalMessageA(WM_SYSCOMMAND,SC_MOVE+HTCAPTION,lParam);
      break;

    case HTSYSMENU:
      if(dwStyle & WS_SYSMENU )
      {
        if( !(dwStyle & WS_MINIMIZE) )
        {
          HDC hDC = GetWindowDC(Win32Hwnd);
          DrawSysButton(hDC,TRUE);
          ReleaseDC(Win32Hwnd,hDC);
        }
        SendInternalMessageA(WM_SYSCOMMAND,SC_MOUSEMENU+HTSYSMENU,lParam);
      }
      break;

    case HTMENU:
      SendInternalMessageA(WM_SYSCOMMAND,SC_MOUSEMENU,lParam);
      break;

    case HTHSCROLL:
      SendInternalMessageA(WM_SYSCOMMAND,SC_HSCROLL+HTHSCROLL,lParam);
      break;

    case HTVSCROLL:
      SendInternalMessageA(WM_SYSCOMMAND,SC_VSCROLL+HTVSCROLL,lParam);
      break;

    case HTMINBUTTON:
    case HTMAXBUTTON:
      TrackMinMaxBox(wParam);
      break;

    case HTCLOSE:
      TrackCloseButton(wParam);
      break;

    case HTLEFT:
    case HTRIGHT:
    case HTTOP:
    case HTTOPLEFT:
    case HTTOPRIGHT:
    case HTBOTTOM:
    case HTBOTTOMLEFT:
    case HTBOTTOMRIGHT:
        /* make sure hittest fits into 0xf and doesn't overlap with HTSYSMENU */
        SendInternalMessageA(WM_SYSCOMMAND,SC_SIZE+wParam-2,lParam);
        break;
    case HTBORDER:
        break;
  }

  return 0;
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCHitTest(POINT pt)
{
  RECT rect;

  rect = *getWindowRect();
  if (!PtInRect(&rect,pt)) return HTNOWHERE;

  if (dwStyle & WS_MINIMIZE) return HTCAPTION;

  if (!(flags & WIN_MANAGED))
  {
    /* Check borders */
    if (HAS_THICKFRAME(dwStyle,dwExStyle))
    {
      InflateRect(&rect,-GetSystemMetrics(SM_CXFRAME),-GetSystemMetrics(SM_CYFRAME));
      if (!PtInRect(&rect,pt))
      {
        /* Check top sizing border */
        if (pt.y < rect.top)
        {
          if (pt.x < rect.left+GetSystemMetrics(SM_CXSIZE)) return HTTOPLEFT;
          if (pt.x >= rect.right-GetSystemMetrics(SM_CXSIZE)) return HTTOPRIGHT;
          return HTTOP;
        }
        /* Check bottom sizing border */
        if (pt.y >= rect.bottom)
        {
          if (pt.x < rect.left+GetSystemMetrics(SM_CXSIZE)) return HTBOTTOMLEFT;
          if (pt.x >= rect.right-GetSystemMetrics(SM_CXSIZE)) return HTBOTTOMRIGHT;
          return HTBOTTOM;
        }
        /* Check left sizing border */
        if (pt.x < rect.left)
        {
          if (pt.y < rect.top+GetSystemMetrics(SM_CYSIZE)) return HTTOPLEFT;
          if (pt.y >= rect.bottom-GetSystemMetrics(SM_CYSIZE)) return HTBOTTOMLEFT;
          return HTLEFT;
        }
        /* Check right sizing border */
        if (pt.x >= rect.right)
        {
          if (pt.y < rect.top+GetSystemMetrics(SM_CYSIZE)) return HTTOPRIGHT;
          if (pt.y >= rect.bottom-GetSystemMetrics(SM_CYSIZE)) return HTBOTTOMRIGHT;
          return HTRIGHT;
        }
      }
    } else  /* No thick frame */
    {
      if (HAS_DLGFRAME(dwStyle,dwExStyle))
        InflateRect(&rect, -GetSystemMetrics(SM_CXDLGFRAME), -GetSystemMetrics(SM_CYDLGFRAME));
      else if (HAS_THINFRAME(dwStyle ))
        InflateRect(&rect, -GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER));
      if (!PtInRect( &rect, pt )) return HTBORDER;
    }

    /* Check caption */

    if ((dwStyle & WS_CAPTION) == WS_CAPTION)
    {
      if (dwExStyle & WS_EX_TOOLWINDOW)
        rect.top += GetSystemMetrics(SM_CYSMCAPTION)-1;
      else
        rect.top += GetSystemMetrics(SM_CYCAPTION)-1;
      if (!PtInRect(&rect,pt))
      {
        /* Check system menu */
        if(dwStyle & WS_SYSMENU)
        {
          /* Check if there is an user icon */
          HICON hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICONSM);
          if(!hIcon) hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICON);

          /* If there is an icon associated with the window OR              */
          /* If there is no hIcon specified and this is not a modal dialog, */
          /* there is a system menu icon.                                   */
          if((hIcon != 0) || (!(dwStyle & DS_MODALFRAME)))
          rect.left += GetSystemMetrics(SM_CYCAPTION) - 1;
        }
        if (pt.x < rect.left) return HTSYSMENU;

        /* Check close button */
        if (dwStyle & WS_SYSMENU)
        rect.right -= GetSystemMetrics(SM_CYCAPTION) - 1;
        if (pt.x > rect.right) return HTCLOSE;

        /* Check maximize box */
        /* In win95 there is automatically a Maximize button when there is a minimize one*/
        if ((dwStyle & WS_MAXIMIZEBOX)|| (dwStyle & WS_MINIMIZEBOX))
        rect.right -= GetSystemMetrics(SM_CXSIZE) + 1;
        if (pt.x > rect.right) return HTMAXBUTTON;

        /* Check minimize box */
        /* In win95 there is automatically a Maximize button when there is a Maximize one*/
        if ((dwStyle & WS_MINIMIZEBOX)||(dwStyle & WS_MAXIMIZEBOX))
        rect.right -= GetSystemMetrics(SM_CXSIZE) + 1;

        if (pt.x > rect.right) return HTMINBUTTON;
        return HTCAPTION;
      }
    }
  }

  /* Check client area */

  ScreenToClient(Win32Hwnd,&pt);
  rect = *getClientRect();
  if (PtInRect(&rect,pt)) return HTCLIENT;

  /* Check vertical scroll bar */

  if (dwStyle & WS_VSCROLL)
  {
    rect.right += GetSystemMetrics(SM_CXVSCROLL);
    if (PtInRect( &rect, pt )) return HTVSCROLL;
  }

  /* Check horizontal scroll bar */

  if (dwStyle & WS_HSCROLL)
  {
    rect.bottom += GetSystemMetrics(SM_CYHSCROLL);
    if (PtInRect( &rect, pt ))
    {
      /* Check size box */
      if ((dwStyle & WS_VSCROLL) &&
          (pt.x >= rect.right - GetSystemMetrics(SM_CXVSCROLL)))
        return (dwStyle & WS_CHILD) ? HTSIZE:HTBOTTOMRIGHT;
      return HTHSCROLL;
    }
  }

  /* Check menu bar */

  if (HAS_MENU(this))
  {
    if ((pt.y < 0) && (pt.x >= 0) && (pt.x < rect.right))
      return HTMENU;
  }

  /* Should never get here */
  return HTERROR;
}

//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::GetInsideRect(RECT *rect)
{
  rect->top    = rect->left = 0;
  rect->right  = getWindowRect()->right - getWindowRect()->left;
  rect->bottom = getWindowRect()->bottom - getWindowRect()->top;

  if ((dwStyle & WS_ICONIC) || (flags & WIN_MANAGED)) return;

  /* Remove frame from rectangle */
  if (HAS_THICKFRAME(dwStyle,dwExStyle))
  {
    InflateRect( rect, -GetSystemMetrics(SM_CXSIZEFRAME), -GetSystemMetrics(SM_CYSIZEFRAME) );
  }
  else if (HAS_DLGFRAME(dwStyle,dwExStyle ))
  {
    InflateRect( rect, -GetSystemMetrics(SM_CXFIXEDFRAME), -GetSystemMetrics(SM_CYFIXEDFRAME));
  }
  else if (HAS_THINFRAME(dwStyle))
  {
    InflateRect( rect, -GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER) );
  }

  /* We have additional border information if the window
   * is a child (but not an MDI child) */
  if ( (dwStyle & WS_CHILD)  &&
       ( (dwExStyle & WS_EX_MDICHILD) == 0 ) )
  {
    if (dwExStyle & WS_EX_CLIENTEDGE)
      InflateRect (rect, -GetSystemMetrics(SM_CXEDGE), -GetSystemMetrics(SM_CYEDGE));

    if (dwExStyle & WS_EX_STATICEDGE)
      InflateRect (rect, -GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER));
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DrawFrame(HDC hdc,RECT *rect,BOOL dlgFrame,BOOL active)
{
  INT width, height;
  HBRUSH oldBrush;

  if (dlgFrame)
  {
    width = GetSystemMetrics(SM_CXDLGFRAME) - GetSystemMetrics(SM_CXEDGE);
    height = GetSystemMetrics(SM_CYDLGFRAME) - GetSystemMetrics(SM_CYEDGE);
  }
  else
  {
    width = GetSystemMetrics(SM_CXFRAME) - GetSystemMetrics(SM_CXEDGE);
    height = GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYEDGE);
  }

  oldBrush = SelectObject(hdc,GetSysColorBrush(active ? COLOR_ACTIVEBORDER:COLOR_INACTIVEBORDER));

  /* Draw frame */

  PatBlt(hdc,rect->left,rect->top,rect->right-rect->left,height,PATCOPY);
  PatBlt(hdc,rect->left,rect->top,width,rect->bottom-rect->top,PATCOPY);
  PatBlt(hdc,rect->left,rect->bottom-1,rect->right-rect->left,-height,PATCOPY);
  PatBlt(hdc,rect->right-1,rect->top,-width,rect->bottom-rect->top,PATCOPY);
  SelectObject(hdc,oldBrush);

  InflateRect(rect,-width,-height);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::DrawSysButton(HDC hdc,BOOL down)
{
    HICON  hIcon;
    RECT rect;

    GetInsideRect(&rect);

    hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICONSM);
    if(!hIcon) hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICON);

    /* If there is no hIcon specified or this is not a modal dialog, */
    /* get the default one.                                          */
    if(hIcon == 0)
      if (!(dwStyle & DS_MODALFRAME))
        hIcon = LoadImageA(0, MAKEINTRESOURCEA(OIC_WINEICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

    if (hIcon)
      DrawIconEx (hdc, rect.left + 2, rect.top + 2, hIcon,
                  GetSystemMetrics(SM_CXSMICON),
                  GetSystemMetrics(SM_CYSMICON),
                  0, 0, DI_NORMAL);

    return (hIcon != 0);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::DrawGrayButton(HDC hdc,int x,int y)
{
  HBITMAP hMaskBmp;
  HDC hdcMask = CreateCompatibleDC (0);
  HBRUSH hOldBrush;
  hMaskBmp = CreateBitmap (12, 10, 1, 1, lpGrayMask);

  if(hMaskBmp == 0)
    return FALSE;

  SelectObject (hdcMask, hMaskBmp);

  /* Draw the grayed bitmap using the mask */
  hOldBrush = SelectObject (hdc, RGB(128, 128, 128));
  BitBlt (hdc, x, y, 12, 10,
          hdcMask, 0, 0, 0xB8074A);

  /* Clean up */
  SelectObject (hdc, hOldBrush);
  DeleteObject(hMaskBmp);
  DeleteDC (hdcMask);

  return TRUE;
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DrawCloseButton(HDC hdc,BOOL down,BOOL bGrayed)
{
  RECT rect;
  HDC hdcMem;

  if( !(flags & WIN_MANAGED) )
  {
    BITMAP bmp;
    HBITMAP hBmp, hOldBmp;

    GetInsideRect(&rect);

    hdcMem = CreateCompatibleDC( hdc );
    hBmp = down ? hbitmapCloseD : hbitmapClose;
    hOldBmp = SelectObject (hdcMem, hBmp);
    GetObjectA (hBmp, sizeof(BITMAP), &bmp);

    BitBlt (hdc, rect.right - (GetSystemMetrics(SM_CYCAPTION) + 1 + bmp.bmWidth) / 2,
            rect.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2,
            bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

    if(bGrayed)
      DrawGrayButton(hdc,rect.right - (GetSystemMetrics(SM_CYCAPTION) + 1 + bmp.bmWidth) / 2 + 2,
                     rect.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2 + 2);

    SelectObject (hdcMem, hOldBmp);
    DeleteDC (hdcMem);
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DrawMaxButton(HDC hdc,BOOL down,BOOL bGrayed)
{
  RECT rect;
  HDC hdcMem;

  if( !(flags & WIN_MANAGED))
  {
    BITMAP  bmp;
    HBITMAP  hBmp,hOldBmp;

    GetInsideRect(&rect);
    hdcMem = CreateCompatibleDC( hdc );
    hBmp = IsZoomed(Win32Hwnd) ?
            (down ? hbitmapRestoreD : hbitmapRestore ) :
            (down ? hbitmapMaximizeD: hbitmapMaximize);
    hOldBmp=SelectObject( hdcMem, hBmp );
    GetObjectA (hBmp, sizeof(BITMAP), &bmp);

    if (dwStyle & WS_SYSMENU)
      rect.right -= GetSystemMetrics(SM_CYCAPTION) + 1;

    BitBlt( hdc, rect.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2,
          rect.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2,
          bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY );

    if(bGrayed)
      DrawGrayButton(hdc, rect.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2 + 2,
                      rect.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2 + 2);


    SelectObject (hdcMem, hOldBmp);
    DeleteDC( hdcMem );
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DrawMinButton(HDC hdc,BOOL down,BOOL bGrayed)
{
  RECT rect;
  HDC hdcMem;

  if( !(flags & WIN_MANAGED))

  {
    BITMAP  bmp;
    HBITMAP  hBmp,hOldBmp;

    GetInsideRect(&rect);

    hdcMem = CreateCompatibleDC( hdc );
    hBmp = down ? hbitmapMinimizeD : hbitmapMinimize;
    hOldBmp= SelectObject( hdcMem, hBmp );
    GetObjectA (hBmp, sizeof(BITMAP), &bmp);

    if (dwStyle & WS_SYSMENU)
      rect.right -= GetSystemMetrics(SM_CYCAPTION) + 1;

    /* In win 95 there is always a Maximize box when there is a Minimize one */
    if ((dwStyle & WS_MAXIMIZEBOX) || (dwStyle & WS_MINIMIZEBOX))
      rect.right += -1 - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2;

    BitBlt( hdc, rect.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2,
          rect.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2,
          bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY );

    if(bGrayed)
      DrawGrayButton(hdc, rect.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2 + 2,
                      rect.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2 + 2);


    SelectObject (hdcMem, hOldBmp);
    DeleteDC( hdcMem );
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DrawCaption(HDC hdc,RECT *rect,BOOL active)
{
  RECT  r = *rect;
  char  buffer[256];
  HPEN  hPrevPen;
  HMENU hSysMenu;

  hPrevPen = SelectObject( hdc, GetSysColorPen(COLOR_3DFACE) );
  MoveToEx( hdc, r.left, r.bottom - 1, NULL );
  LineTo( hdc, r.right, r.bottom - 1 );
  SelectObject( hdc, hPrevPen );
  r.bottom--;

  FillRect( hdc, &r, GetSysColorBrush(active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION) );

  if (!hbitmapClose)
  {
    if (!(hbitmapClose = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_CLOSE)))) return;
    hbitmapCloseD    = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_CLOSED));
    hbitmapMinimize  = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_REDUCE));
    hbitmapMinimizeD = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_REDUCED));
    hbitmapMaximize  = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_ZOOM));
    hbitmapMaximizeD = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_ZOOMD));
    hbitmapRestore   = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_RESTORE));
    hbitmapRestoreD  = LoadBitmapA(0,MAKEINTRESOURCEA(OBM_RESTORED));
  }

  if ((dwStyle & WS_SYSMENU) && !(dwExStyle & WS_EX_TOOLWINDOW))
  {
    if (DrawSysButton(hdc,FALSE))
      r.left += GetSystemMetrics(SM_CYCAPTION) - 1;
  }

  if (dwStyle & WS_SYSMENU)
  {
    UINT state;
#if 0 //CB: todo
    /* Go get the sysmenu */
    state = GetMenuState(hSysMenu, SC_CLOSE, MF_BYCOMMAND);
#else
    state = 0;
#endif
    /* Draw a grayed close button if disabled and a normal one if SC_CLOSE is not there */
    DrawCloseButton(hdc, FALSE,
                    ((((state & MF_DISABLED) || (state & MF_GRAYED))) && (state != 0xFFFFFFFF)));
    r.right -= GetSystemMetrics(SM_CYCAPTION) - 1;

    if ((dwStyle & WS_MAXIMIZEBOX) || (dwStyle & WS_MINIMIZEBOX))
    {
      /* In win95 the two buttons are always there */
      /* But if the menu item is not in the menu they're disabled*/

      DrawMaxButton(hdc, FALSE, (!(dwStyle & WS_MAXIMIZEBOX)));
      r.right -= GetSystemMetrics(SM_CXSIZE) + 1;

      DrawMinButton(hdc, FALSE,  (!(dwStyle & WS_MINIMIZEBOX)));
            r.right -= GetSystemMetrics(SM_CXSIZE) + 1;
    }
  }

  if (GetWindowTextA(buffer, sizeof(buffer) ))
  {
    NONCLIENTMETRICSA nclm;
    HFONT hFont, hOldFont;
    nclm.cbSize = sizeof(NONCLIENTMETRICSA);
    SystemParametersInfoA (SPI_GETNONCLIENTMETRICS, 0, &nclm, 0);
    if (dwExStyle & WS_EX_TOOLWINDOW)
      hFont = CreateFontIndirectA (&nclm.lfSmCaptionFont);
    else
      hFont = CreateFontIndirectA (&nclm.lfCaptionFont);
    hOldFont = SelectObject (hdc, hFont);
    if (active) SetTextColor( hdc, GetSysColor( COLOR_CAPTIONTEXT ) );
    else SetTextColor( hdc, GetSysColor( COLOR_INACTIVECAPTIONTEXT ) );
    SetBkMode( hdc, TRANSPARENT );
    r.left += 2;
    DrawTextA( hdc, buffer, -1, &r,
             DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT );
    DeleteObject (SelectObject (hdc, hOldFont));
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::HandleNCPaint(HRGN clip, BOOL suppress_menupaint)
{
  BOOL active  = flags & WIN_NCACTIVATED;
  HDC hdc;
  RECT rect,rectClip,rfuzz;

  if (dwStyle & WS_MINIMIZE) return;

  /* MSDN docs are pretty idiotic here, they say app CAN use clipRgn in
     the call to GetDCEx implying that it is allowed not to use it either.
     However, the suggested GetDCEx(    , DCX_WINDOW | DCX_INTERSECTRGN)
     will cause clipRgn to be deleted after ReleaseDC().
     Now, how is the "system" supposed to tell what happened?
   */

  if (!(hdc = GetDCEx( Win32Hwnd, (clip > 1) ? clip : 0, DCX_USESTYLE | DCX_WINDOW |
                      ((clip > 1) ?(DCX_INTERSECTRGN /*| DCX_KEEPCLIPRGN*/) : 0) ))) return;

  rect.top = rect.left = 0;
  rect.right  = getWindowRect()->right - getWindowRect()->left;
  rect.bottom = getWindowRect()->bottom - getWindowRect()->top;

  if( clip > 1 )
    GetRgnBox( clip, &rectClip );
  else
  {
    clip = 0;
    rectClip = rect;
  }

  SelectObject( hdc, GetSysColorPen(COLOR_WINDOWFRAME) );

  if (HAS_BIGFRAME( dwStyle, dwExStyle))
  {
      DrawEdge (hdc, &rect, EDGE_RAISED, BF_RECT | BF_ADJUST);
  }
  if (HAS_THICKFRAME( dwStyle, dwExStyle ))
      DrawFrame(hdc, &rect, FALSE, active );
  else if (HAS_DLGFRAME( dwStyle, dwExStyle ))
      DrawFrame( hdc, &rect, TRUE, active );
  else if (HAS_THINFRAME( dwStyle ))
  {
      SelectObject( hdc, GetStockObject(NULL_BRUSH) );
      Rectangle( hdc, 0, 0, rect.right, rect.bottom );
  }

  if ((dwStyle & WS_CAPTION) == WS_CAPTION)
  {
      RECT  r = rect;
      if (dwExStyle & WS_EX_TOOLWINDOW)
      {
        r.bottom = rect.top + GetSystemMetrics(SM_CYSMCAPTION);
        rect.top += GetSystemMetrics(SM_CYSMCAPTION);
      }
      else
      {
        r.bottom = rect.top + GetSystemMetrics(SM_CYCAPTION);
        rect.top += GetSystemMetrics(SM_CYCAPTION);
      }
      if( !clip || IntersectRect( &rfuzz, &r, &rectClip ) )
        DrawCaption(hdc, &r, active);
  }

#if 0 //CB: todo
  if (HAS_MENU(wndPtr))
  {
    RECT r = rect;
    r.bottom = rect.top + GetSystemMetrics(SM_CYMENU);

    rect.top += MENU_DrawMenuBar( hdc, &r, hwnd, suppress_menupaint ) + 1;
  }

  if (dwExStyle & WS_EX_CLIENTEDGE)
    DrawEdge (hdc, &rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  if (dwExStyle & WS_EX_STATICEDGE)
    DrawEdge (hdc, &rect, BDR_SUNKENOUTER, BF_RECT | BF_ADJUST);

  /* Draw the scroll-bars */
#if 0 //CB: todo
  if (dwStyle & WS_VSCROLL)
    SCROLL_DrawScrollBar(hwnd,hdc,SB_VERT,TRUE,TRUE);
  if (wndPtr->dwStyle & WS_HSCROLL)
    SCROLL_DrawScrollBar(hwnd,hdc,SB_HORZ,TRUE,TRUE);
#endif
  /* Draw the "size-box" */
  if ((dwStyle & WS_VSCROLL) && (dwStyle & WS_HSCROLL))
  {
    RECT r = rect;
    r.left = r.right - GetSystemMetrics(SM_CXVSCROLL) + 1;
    r.top  = r.bottom - GetSystemMetrics(SM_CYHSCROLL) + 1;
    FillRect( hdc, &r,  GetSysColorBrush(COLOR_SCROLLBAR) );
    if (!(dwStyle & WS_CHILD))
    {
      POINT p1,p2;
      HPEN penDark = GetSysColorPen(COLOR_3DSHADOW);
      HPEN penWhite = GetSysColorPen(COLOR_3DHILIGHT);
      HPEN oldPen = SelectObject(hdc,penDark);
      INT x;

      p1.x = r.right-1;
      p1.y = r.bottom;
      p2.x = r.right;
      p2.y = r.bottom-1;
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
  }
#endif

  ReleaseDC(Win32Hwnd,hdc);
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCActivate(WPARAM wParam)
{
  WORD wStateChange;

  if( wParam ) {
        wStateChange = !(flags & WIN_NCACTIVATED);
  }
  else  wStateChange = flags & WIN_NCACTIVATED;

  if( wStateChange )
  {
    if (wParam)
         flags |= WIN_NCACTIVATED;
    else flags &= ~WIN_NCACTIVATED;

    if(!(dwStyle & WS_MINIMIZE))
      HandleNCPaint((HRGN)1,FALSE);
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************

