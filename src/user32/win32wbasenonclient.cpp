/* $Id: win32wbasenonclient.cpp,v 1.5 2000-01-26 18:02:37 cbratschi Exp $ */
/*
 * Win32 Window Base Class for OS/2 (non-client methods)
 *
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
#include "pmframe.h"
#include "oslibwin.h"
#include "oslibmsg.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "dc.h"
#include "win32wdesktop.h"
#include "controls.h"
#include <menu.h>

#define SC_ABOUTODIN            (SC_SCREENSAVE+1)
#define SC_PUTMARK              (SC_SCREENSAVE+2)

/* bits in the dwKeyData */
#define KEYDATA_ALT         0x2000
#define KEYDATA_PREVSTATE   0x4000

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

static INT (* WINAPI ShellAboutA)(HWND,LPCSTR,LPCSTR,HICON) = 0;

//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCActivate(WPARAM wParam)
{
  WORD wStateChange;

  //CB: only caption is changed on activation
  if (!(dwStyle & WS_CAPTION)) return TRUE;

  if( wParam ) wStateChange = !(flags & WIN_NCACTIVATED);
  else wStateChange = flags & WIN_NCACTIVATED;

  if( wStateChange )
  {
    if (wParam) flags |= WIN_NCACTIVATED;
    else flags &= ~WIN_NCACTIVATED;

    if(!(dwStyle & WS_MINIMIZE))
      DoNCPaint((HRGN)1,FALSE);
  }

  return TRUE;
}
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
    state = GetMenuState(hSysMenu,SC_MINIMIZE,MF_BYCOMMAND);
  } else
  {
    /* If the style is not present, do nothing */
    if (!(dwStyle & WS_MAXIMIZEBOX))
      return;
    /* Check if the sysmenu item for maximize is there  */
    state = GetMenuState(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
  }
  SetCapture(Win32Hwnd);
  hdc = GetWindowDC(Win32Hwnd);
  if (wParam == HTMINBUTTON)
    DrawMinButton(hdc,NULL,TRUE,FALSE);
  else
    DrawMaxButton(hdc,NULL,TRUE,FALSE);
  do
  {
    BOOL oldstate = pressed;

    GetMessageA(&msg,Win32Hwnd,0,0);
    pressed = (HandleNCHitTest(msg.pt) == wParam);
    if (pressed != oldstate)
    {
      if (wParam == HTMINBUTTON)
        DrawMinButton(hdc,NULL,pressed,FALSE);
      else
        DrawMaxButton(hdc,NULL,pressed,FALSE);
    }
  } while (msg.message != WM_LBUTTONUP);
  if (wParam == HTMINBUTTON)
    DrawMinButton(hdc,NULL,FALSE,FALSE);
  else
    DrawMaxButton(hdc,NULL,FALSE,FALSE);
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

  if (hSysMenu == 0)
    return;
  state = GetMenuState(hSysMenu, SC_CLOSE, MF_BYCOMMAND);
  /* If the item close of the sysmenu is disabled or not there do nothing */
  if((state & MF_DISABLED) || (state & MF_GRAYED) || (state == 0xFFFFFFFF))
    return;
  hdc = GetWindowDC(Win32Hwnd);
  SetCapture(Win32Hwnd);
  DrawCloseButton(hdc,NULL,TRUE,FALSE);
  do
  {
    BOOL oldstate = pressed;

    GetMessageA(&msg,Win32Hwnd,0,0);
    pressed = (HandleNCHitTest(msg.pt) == wParam);
    if (pressed != oldstate)
      DrawCloseButton(hdc,NULL,pressed,FALSE);
  } while (msg.message != WM_LBUTTONUP);
  DrawCloseButton(hdc,NULL,FALSE,FALSE);
  ReleaseCapture();
  ReleaseDC(Win32Hwnd,hdc);
  if (!pressed) return;
  SendInternalMessageA(WM_SYSCOMMAND,SC_CLOSE,*(LPARAM*)&msg.pt);
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::TrackScrollBar(WPARAM wParam,POINT pt)
{
  INT scrollbar;
  MSG msg;

  if ((wParam & 0xfff0) == SC_HSCROLL)
  {
    if ((wParam & 0x0f) != HTHSCROLL) return;
    scrollbar = SB_HORZ;
  } else  /* SC_VSCROLL */
  {
    if ((wParam & 0x0f) != HTVSCROLL) return;
    scrollbar = SB_VERT;
  }

  pt.x -= rectWindow.left;
  pt.y -= rectWindow.top;
  SCROLL_HandleScrollEvent(Win32Hwnd,0,MAKELONG(pt.x,pt.y),scrollbar,WM_LBUTTONDOWN);
  if (GetCapture() != Win32Hwnd) return;
  do
  {
    GetMessageA(&msg, 0, 0, 0);
    if(msg.hwnd == getWindowHandle())
    {
        switch(msg.message)
        {
        case WM_LBUTTONUP:
        case WM_MOUSEMOVE:
            pt.x = msg.pt.x-rectWindow.left;
            pt.y = msg.pt.y-rectWindow.top;
            msg.lParam = MAKELONG(pt.x,pt.y);

        case WM_SYSTIMER:
            SCROLL_HandleScrollEvent(Win32Hwnd,msg.wParam,msg.lParam,scrollbar,msg.message);
            break;

        default:
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
            break;
        }
    }
    else {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    if (!IsWindow())
    {
      ReleaseCapture();
      break;
    }
  } while (msg.message != WM_LBUTTONUP);
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCLButtonDown(WPARAM wParam,LPARAM lParam)
{
  switch(wParam)  /* Hit test */
  {
    case HTCAPTION:
    {
      Win32BaseWindow *topparent = GetTopParent();

      if (GetActiveWindow() != topparent->getWindowHandle())
        topparent->SetActiveWindow();

      if (GetActiveWindow() == topparent->getWindowHandle())
        SendInternalMessageA(WM_SYSCOMMAND,SC_MOVE+HTCAPTION,lParam);
      break;
    }

    case HTSYSMENU:
      if(dwStyle & WS_SYSMENU )
      {
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
BOOL Win32BaseWindow::WindowNeedsWMBorder()
{
  if (!(dwStyle & WS_CHILD) &&
      //Options.managed  && //CB: to check
      !(dwExStyle & WS_EX_TOOLWINDOW) &&
      ( ((dwStyle & WS_CAPTION) == WS_CAPTION) ||
        (dwStyle & WS_THICKFRAME)))
      return TRUE;
  if (dwExStyle & WS_EX_TRAYWINDOW)
    return TRUE;
  return FALSE;
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::AdjustMaximizedRect(LPRECT rect)
{
  /* Decide if the window will be managed (see CreateWindowEx) */
  //if (!WindowNeedsWMBorder()) //CB: check Options.managed
  {
    if (HAS_THICKFRAME(dwStyle,dwExStyle ))
      InflateRect( rect, GetSystemMetrics(SM_CXFRAME), GetSystemMetrics(SM_CYFRAME) );
    else
      if (HAS_DLGFRAME( dwStyle, dwExStyle ))
        InflateRect(rect, GetSystemMetrics(SM_CXDLGFRAME), GetSystemMetrics(SM_CYDLGFRAME) );
      else
        if (HAS_THINFRAME( dwStyle ))
          InflateRect( rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::AdjustTrackInfo(PPOINT minTrackSize,PPOINT maxTrackSize)
{
  if ((dwStyle & WS_THICKFRAME) || !(dwStyle & (WS_POPUP | WS_CHILD)))
    GetMinMaxInfo(NULL,NULL,minTrackSize,maxTrackSize);
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::AdjustRectOuter(LPRECT rect,BOOL menu)
{
  if(dwStyle & WS_ICONIC) return;

  /* Decide if the window will be managed (see CreateWindowEx) */
  //if (!WindowNeedsWMBorder()) //CB: check Options.managed
  {
    if (HAS_THICKFRAME(dwStyle,dwExStyle ))
      InflateRect( rect, GetSystemMetrics(SM_CXFRAME), GetSystemMetrics(SM_CYFRAME) );
    else
      if (HAS_DLGFRAME( dwStyle, dwExStyle ))
        InflateRect(rect, GetSystemMetrics(SM_CXDLGFRAME), GetSystemMetrics(SM_CYDLGFRAME) );
      else
        if (HAS_THINFRAME( dwStyle ))
          InflateRect( rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

    if ((dwStyle & WS_CAPTION) == WS_CAPTION)
    {
      if (dwExStyle & WS_EX_TOOLWINDOW)
        rect->top -= GetSystemMetrics(SM_CYSMCAPTION);
      else
        rect->top -= GetSystemMetrics(SM_CYCAPTION);
    }
  }

  if (menu)
    rect->top -= GetSystemMetrics(SM_CYMENU);
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::AdjustRectInner(LPRECT rect)
{
  if(dwStyle & WS_ICONIC) return;

  if (dwExStyle & WS_EX_CLIENTEDGE)
    InflateRect (rect, GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));

  if (dwExStyle & WS_EX_STATICEDGE)
    InflateRect (rect, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

  if (dwStyle & WS_VSCROLL) rect->right  += GetSystemMetrics(SM_CXVSCROLL);
  if (dwStyle & WS_HSCROLL) rect->bottom += GetSystemMetrics(SM_CYHSCROLL);
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCCalcSize(BOOL calcValidRects,RECT *winRect)
{
  RECT tmpRect = { 0, 0, 0, 0 };
  LONG result = 0;
  UINT style;

    dprintf(("Default WM_NCCALCSIZE handler"));

    if (!calcValidRects) return 0;

    style = (UINT) GetClassLongA(Win32Hwnd,GCL_STYLE);

    if (style & CS_VREDRAW) result |= WVR_VREDRAW;
    if (style & CS_HREDRAW) result |= WVR_HREDRAW;

    if(!(dwStyle & WS_MINIMIZE))
    {
        AdjustRectOuter(&tmpRect,FALSE);

        winRect->left   -= tmpRect.left;
        winRect->top    -= tmpRect.top;
        winRect->right  -= tmpRect.right;
        winRect->bottom -= tmpRect.bottom;

        if (HAS_MENU())
        {
            winRect->top +=
                MENU_GetMenuBarHeight(Win32Hwnd,
                                      winRect->right - winRect->left,
                                      -tmpRect.left, -tmpRect.top ) + 1;
        }

        SetRect (&tmpRect, 0, 0, 0, 0);
        AdjustRectInner(&tmpRect);
        winRect->left   -= tmpRect.left;
        winRect->top    -= tmpRect.top;
        winRect->right  -= tmpRect.right;
        winRect->bottom -= tmpRect.bottom;
    }

    return result;
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCHitTest(POINT pt)
{
  RECT rect = rectWindow;

  if (dwStyle & WS_MINIMIZE) return HTCAPTION;

  if (!PtInRect(&rect,pt)) return HTNOWHERE;

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
  getClientRect(&rect);
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

  if (HAS_MENU())
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
  rect->right  = rectWindow.right - rectWindow.left;
  rect->bottom = rectWindow.bottom - rectWindow.top;

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
  PatBlt(hdc,rect->left,   rect->top,      rect->right-rect->left, height,PATCOPY);
  PatBlt(hdc,rect->left,   rect->top,      width,                  rect->bottom-rect->top,PATCOPY);
  PatBlt(hdc,rect->left,   rect->bottom-1, rect->right-rect->left,-height,PATCOPY);
  PatBlt(hdc,rect->right-1,rect->top,     -width,                  rect->bottom-rect->top,PATCOPY);
  SelectObject(hdc,oldBrush);

  InflateRect(rect,-width,-height);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::DrawSysButton(HDC hdc,RECT *rect)
{
  if(!(flags & WIN_MANAGED))
  {
    HICON  hIcon;
    RECT r;

    if (!rect) GetInsideRect(&r);
    else r = *rect;

    hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICONSM);
    if(!hIcon) hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICON);

    /* If there is no hIcon specified or this is not a modal dialog, */
    /* get the default one.                                          */
    if(hIcon == 0)
      if (!(dwStyle & DS_MODALFRAME))
        hIcon = LoadImageA(0, MAKEINTRESOURCEA(OIC_ODINICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

    if (hIcon)
      DrawIconEx(hdc,r.left+2,r.top+2,hIcon,
                 GetSystemMetrics(SM_CXSMICON),
                 GetSystemMetrics(SM_CYSMICON),
                 0, 0, DI_NORMAL);

    return (hIcon != 0);
  }
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::GetSysPopupPos(RECT* rect)
{
  if(hSysMenu)
  {
      if(dwStyle & WS_MINIMIZE )
        *rect = rectWindow;
      else
      {
          GetInsideRect(rect );
          OffsetRect( rect, rectWindow.left, rectWindow.top);
          rect->right = rect->left + GetSystemMetrics(SM_CYCAPTION) - 1;
          rect->bottom = rect->top + GetSystemMetrics(SM_CYCAPTION) - 1;
      }
      return TRUE;
  }
  return FALSE;
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
VOID Win32BaseWindow::DrawCloseButton(HDC hdc,RECT *rect,BOOL down,BOOL bGrayed)
{
  RECT r;
  HDC hdcMem;

  if( !(flags & WIN_MANAGED) )
  {
    BITMAP bmp;
    HBITMAP hBmp, hOldBmp;

    if (!rect) GetInsideRect(&r);
    else r = *rect;

    hdcMem = CreateCompatibleDC( hdc );
    hBmp = down ? hbitmapCloseD : hbitmapClose;
    hOldBmp = SelectObject (hdcMem, hBmp);
    GetObjectA (hBmp, sizeof(BITMAP), &bmp);

    BitBlt (hdc, r.right - (GetSystemMetrics(SM_CYCAPTION) + 1 + bmp.bmWidth) / 2,
            r.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2,
            bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

    if(bGrayed)
      DrawGrayButton(hdc,r.right - (GetSystemMetrics(SM_CYCAPTION) + 1 + bmp.bmWidth) / 2 + 2,
                     r.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2 + 2);

    SelectObject (hdcMem, hOldBmp);
    DeleteDC (hdcMem);
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DrawMaxButton(HDC hdc,RECT *rect,BOOL down,BOOL bGrayed)
{
  RECT r;
  HDC hdcMem;

  if( !(flags & WIN_MANAGED))
  {
    BITMAP  bmp;
    HBITMAP  hBmp,hOldBmp;

    if (!rect) GetInsideRect(&r);
    else r = *rect;
    hdcMem = CreateCompatibleDC( hdc );
    hBmp = IsZoomed(Win32Hwnd) ?
            (down ? hbitmapRestoreD : hbitmapRestore ) :
            (down ? hbitmapMaximizeD: hbitmapMaximize);
    hOldBmp=SelectObject( hdcMem, hBmp );
    GetObjectA (hBmp, sizeof(BITMAP), &bmp);

    if (dwStyle & WS_SYSMENU)
      r.right -= GetSystemMetrics(SM_CYCAPTION) + 1;

    BitBlt( hdc, r.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2,
          r.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2,
          bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY );

    if(bGrayed)
      DrawGrayButton(hdc, r.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2 + 2,
                      r.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2 + 2);


    SelectObject (hdcMem, hOldBmp);
    DeleteDC( hdcMem );
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DrawMinButton(HDC hdc,RECT *rect,BOOL down,BOOL bGrayed)
{
  RECT r;
  HDC hdcMem;

  if( !(flags & WIN_MANAGED))

  {
    BITMAP  bmp;
    HBITMAP  hBmp,hOldBmp;

    if (!rect) GetInsideRect(&r);
    else r = *rect;

    hdcMem = CreateCompatibleDC( hdc );
    hBmp = down ? hbitmapMinimizeD : hbitmapMinimize;
    hOldBmp= SelectObject( hdcMem, hBmp );
    GetObjectA (hBmp, sizeof(BITMAP), &bmp);

    if (dwStyle & WS_SYSMENU)
      r.right -= GetSystemMetrics(SM_CYCAPTION) + 1;

    /* In win 95 there is always a Maximize box when there is a Minimize one */
    if ((dwStyle & WS_MAXIMIZEBOX) || (dwStyle & WS_MINIMIZEBOX))
      r.right -= bmp.bmWidth;

    BitBlt( hdc, r.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2,
          r.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2,
          bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY );

    if(bGrayed)
      DrawGrayButton(hdc, r.right - (GetSystemMetrics(SM_CXSIZE) + bmp.bmWidth) / 2 + 2,
                      r.top + (GetSystemMetrics(SM_CYCAPTION) - 1 - bmp.bmHeight) / 2 + 2);


    SelectObject (hdcMem, hOldBmp);
    DeleteDC( hdcMem );
  }
}
//******************************************************************************
// redrawText: only redraws text
//******************************************************************************
VOID Win32BaseWindow::DrawCaption(HDC hdc,RECT *rect,BOOL active,BOOL redrawText)
{
  RECT  r = *rect,r2;
  char  buffer[256];
  HPEN  hPrevPen;
  HDC memDC;
  HBITMAP memBmp,oldBmp;

  if (flags & WIN_MANAGED) return;

  memDC = CreateCompatibleDC(hdc);
  r.right -= r.left;
  r.bottom -= r.top;
  r.left = r.top = 0;
  r2 = r;
  memBmp = CreateCompatibleBitmap(hdc,r.right,r.bottom);
  oldBmp = SelectObject(memDC,memBmp);

  hPrevPen = SelectObject(memDC,GetSysColorPen(COLOR_3DFACE));
  MoveToEx(memDC,r.left,r.bottom-1,NULL);
  LineTo(memDC,r.right,r.bottom-1);
  SelectObject(memDC,hPrevPen);
  r.bottom--;

  if (SYSCOLOR_GetUseWinColors())
  {
    COLORREF startColor = GetSysColor(active ? COLOR_ACTIVECAPTION:COLOR_INACTIVECAPTION),endColor = GetSysColor(active ? COLOR_GRADIENTACTIVECAPTION:COLOR_GRADIENTINACTIVECAPTION);

    if (startColor == endColor)
      FillRect(memDC,&r,GetSysColorBrush(startColor));
    else
    {
      INT rDiff = GetRValue(endColor)-GetRValue(startColor);
      INT gDiff = GetGValue(endColor)-GetGValue(startColor);
      INT bDiff = GetBValue(endColor)-GetBValue(startColor);
      INT steps = MAX(MAX(abs(rDiff),abs(gDiff)),abs(bDiff));
      INT w = r.right-r.left;
      RECT r2;

      if (w < steps) steps = w;
      r2.left = r2.right = r.left;
      r2.top = r.top;
      r2.bottom = r.bottom;
      for (INT x = 0;x <= steps;x++)
      {
        COLORREF color = RGB(GetRValue(startColor)+rDiff*x/steps,GetGValue(startColor)+gDiff*x/steps,GetBValue(startColor)+bDiff*x/steps);
        HBRUSH brush = CreateSolidBrush(color);

        r2.left = r2.right;
        r2.right = r.left+w*x/steps;
        FillRect(memDC,&r2,brush);
        DeleteObject(brush);
      }
    }
  } else FillRect(memDC,&r,GetSysColorBrush(active ? COLOR_ACTIVECAPTION:COLOR_INACTIVECAPTION));

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
    if (redrawText || DrawSysButton(memDC,&r))
      r.left += GetSystemMetrics(SM_CYCAPTION) - 1;
  }

  if (dwStyle & WS_SYSMENU)
  {
    UINT state;

    if (!redrawText)
    {
      /* Go get the sysmenu */
      state = GetMenuState(hSysMenu, SC_CLOSE, MF_BYCOMMAND);

      /* Draw a grayed close button if disabled and a normal one if SC_CLOSE is not there */
      DrawCloseButton(memDC,&r2,FALSE,
                      ((((state & MF_DISABLED) || (state & MF_GRAYED))) && (state != 0xFFFFFFFF)));
    }
    r.right -= GetSystemMetrics(SM_CYCAPTION) - 1;

    if ((dwStyle & WS_MAXIMIZEBOX) || (dwStyle & WS_MINIMIZEBOX))
    {
      /* In win95 the two buttons are always there */
      /* But if the menu item is not in the menu they're disabled*/

      if (!redrawText)
        DrawMaxButton(memDC,&r2,FALSE,(!(dwStyle & WS_MAXIMIZEBOX)));
      r.right -= GetSystemMetrics(SM_CXSIZE) + 1;

      if (!redrawText)
        DrawMinButton(memDC,&r2,FALSE,  (!(dwStyle & WS_MINIMIZEBOX)));
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
    hOldFont = SelectObject (memDC, hFont);
    SetTextColor(memDC,GetSysColor(active ? COLOR_CAPTIONTEXT:COLOR_INACTIVECAPTIONTEXT));
    SetBkMode(memDC, TRANSPARENT );
    r.left += 2;
    DrawTextA(memDC, buffer, -1, &r,
             DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT );
    DeleteObject (SelectObject (memDC, hOldFont));
  }

  BitBlt(hdc,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,memDC,0,0,SRCCOPY);
  SelectObject(memDC,oldBmp);
  DeleteObject(memBmp);
  DeleteDC(memDC);
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::UpdateCaptionText()
{
  BOOL active = flags & WIN_NCACTIVATED;
  HDC hdc;
  RECT rect,r;
  HRGN hrgn;

  if (!((dwStyle & WS_CAPTION) == WS_CAPTION)) return;

  rect.top = rect.left = 0;
  rect.right  = rectWindow.right - rectWindow.left;
  rect.bottom = rectWindow.bottom - rectWindow.top;
  if(!(flags & WIN_MANAGED))
  {
    if (HAS_BIGFRAME( dwStyle, dwExStyle))
    {
      InflateRect(&rect,-2,-2);
    }

    if (HAS_THICKFRAME(dwStyle,dwExStyle))
    {
      INT width = GetSystemMetrics(SM_CXFRAME) - GetSystemMetrics(SM_CXEDGE);
      INT height = GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYEDGE);

      InflateRect(&rect,-width,-height);
    }
    else if (HAS_DLGFRAME(dwStyle,dwExStyle))
    {
      INT width = GetSystemMetrics(SM_CXDLGFRAME) - GetSystemMetrics(SM_CXEDGE);
      INT height = GetSystemMetrics(SM_CYDLGFRAME) - GetSystemMetrics(SM_CYEDGE);

      InflateRect(&rect,-width,-height);
    }
    else if (HAS_THINFRAME(dwStyle))
    {
    }

    r = rect;
    if (dwExStyle & WS_EX_TOOLWINDOW)
    {
      r.bottom = rect.top + GetSystemMetrics(SM_CYSMCAPTION);
    }
    else
    {
      r.bottom = rect.top + GetSystemMetrics(SM_CYCAPTION);
    }

    //clip the buttons
    if ((dwStyle & WS_SYSMENU) && !(dwExStyle & WS_EX_TOOLWINDOW))
    {
      HICON hIcon;

      hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICONSM);
      if(!hIcon) hIcon = (HICON) GetClassLongA(Win32Hwnd, GCL_HICON);
      if (hIcon)
        rect.left += GetSystemMetrics(SM_CYCAPTION) - 1;
    }
    if (dwStyle & WS_SYSMENU)
    {
      rect.right -= GetSystemMetrics(SM_CYCAPTION) - 1;

      if ((dwStyle & WS_MAXIMIZEBOX) || (dwStyle & WS_MINIMIZEBOX))
      {
        rect.right -= GetSystemMetrics(SM_CXSIZE) + 1;
        rect.right -= GetSystemMetrics(SM_CXSIZE) + 1;
      }
    }

    hrgn = CreateRectRgnIndirect(&rect);
    hdc = GetDCEx(Win32Hwnd,hrgn,DCX_USESTYLE | DCX_WINDOW | DCX_INTERSECTRGN);
    SelectObject(hdc,GetSysColorPen(COLOR_WINDOWFRAME));
    DrawCaption(hdc,&r,active,TRUE);
    DeleteObject(hrgn);
    ReleaseDC(Win32Hwnd,hdc);
  }
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::DoNCPaint(HRGN clip,BOOL suppress_menupaint)
{
  BOOL active  = flags & WIN_NCACTIVATED;
  HDC hdc;
  RECT rect,rectClip,rfuzz;

  /* MSDN docs are pretty idiotic here, they say app CAN use clipRgn in
     the call to GetDCEx implying that it is allowed not to use it either.
     However, the suggested GetDCEx(    , DCX_WINDOW | DCX_INTERSECTRGN)
     will cause clipRgn to be deleted after ReleaseDC().
     Now, how is the "system" supposed to tell what happened?
   */

  if (!(hdc = GetDCEx( Win32Hwnd, (clip > 1) ? clip : 0, DCX_USESTYLE | DCX_WINDOW |
                      ((clip > 1) ?(DCX_INTERSECTRGN /*| DCX_KEEPCLIPRGN*/) : 0) ))) return;

  rect.top = rect.left = 0;
  rect.right  = rectWindow.right - rectWindow.left;
  rect.bottom = rectWindow.bottom - rectWindow.top;

  if( clip > 1 )
  {
    //CB: unknown WINE handling (clip == 1), clip client?
    GetRgnBox( clip, &rectClip );
  } else
  {
    clip = 0;
    rectClip = rect;
  }

  SelectObject( hdc, GetSysColorPen(COLOR_WINDOWFRAME) );

  if(!(flags & WIN_MANAGED))
  {
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
        DrawCaption(hdc,&r,active,FALSE);
    }
  }
  if (HAS_MENU())
  {
    RECT r = rect;
    r.bottom = rect.top + GetSystemMetrics(SM_CYMENU);

    rect.top += MENU_DrawMenuBar(hdc,&r,Win32Hwnd,suppress_menupaint)+1;
  }

  if (dwExStyle & WS_EX_CLIENTEDGE)
    DrawEdge (hdc, &rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

  if (dwExStyle & WS_EX_STATICEDGE)
    DrawEdge (hdc, &rect, BDR_SUNKENOUTER, BF_RECT | BF_ADJUST);

  /* Draw the scroll-bars */
  if (dwStyle & WS_VSCROLL)
    SCROLL_DrawScrollBar(Win32Hwnd,hdc,SB_VERT,TRUE,TRUE);
  if (dwStyle & WS_HSCROLL)
    SCROLL_DrawScrollBar(Win32Hwnd,hdc,SB_HORZ,TRUE,TRUE);

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

  ReleaseDC(Win32Hwnd,hdc);
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::HandleNCPaint(HRGN clip)
{
//CB: ignore it for now (SetWindowPos in WM_CREATE)
//  if (!(dwStyle & WS_VISIBLE)) return 0;

  if (dwStyle & WS_MINIMIZE) return 0;

  DoNCPaint(clip,FALSE);

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
LONG Win32BaseWindow::HandleNCRButtonUp(WPARAM wParam,LPARAM lParam)
{
  switch(wParam)
  {
    case HTCAPTION:
      if (GetActiveWindow() != Win32Hwnd)
        SetActiveWindow();

      if (((GetActiveWindow() == Win32Hwnd) || isMDIChild()) && (dwStyle & WS_SYSMENU))
      {
        SendInternalMessageA(WM_SYSCOMMAND,SC_MOUSEMENU+HTCAPTION,lParam);
      }
      break;

    default:
      break;
  }

  return 0;
}
/***********************************************************************
 *           NC_HandleSysCommand
 *
 * Handle a WM_SYSCOMMAND message. Called from DefWindowProc().
 *
 */
LONG Win32BaseWindow::HandleSysCommand(WPARAM wParam,POINT *pt32)
{
    UINT uCommand = wParam & 0xFFF0;

    switch (uCommand)
    {

    case SC_SIZE:
    {
      DWORD flags;

      if (dwStyle & WS_MAXIMIZE) break;

      switch ((wParam & 0xF)+2)
      {
        case HTLEFT:
          flags = TFOS_LEFT;
          break;

        case HTRIGHT:
          flags = TFOS_RIGHT;
          break;

        case HTTOP:
          flags = TFOS_TOP;
          break;

        case HTTOPLEFT:
          flags = TFOS_TOP | TFOS_LEFT;
          break;

        case HTTOPRIGHT:
          flags = TFOS_TOP | TFOS_RIGHT;
          break;

        case HTBOTTOM:
          flags = TFOS_BOTTOM;
          break;

        case HTBOTTOMLEFT:
          flags = TFOS_BOTTOM | TFOS_LEFT;
          break;

        case HTBOTTOMRIGHT:
          flags = TFOS_BOTTOM | TFOS_RIGHT;
          break;

        default:
          flags = TFOS_BOTTOM | TFOS_RIGHT;
          break;
      }
      if (flags) FrameTrackFrame(this,flags);
      break;
    }

    case SC_MOVE:
        if (dwStyle & WS_MAXIMIZE) break;
        FrameTrackFrame(this,TFOS_MOVE);
        break;

    case SC_MINIMIZE:
        ShowWindow(SW_MINIMIZE);
        break;

    case SC_MAXIMIZE:
        ShowWindow(SW_MAXIMIZE);
        break;

    case SC_RESTORE:
        ShowWindow(SW_RESTORE);
        break;

    case SC_CLOSE:
        return SendInternalMessageA(WM_CLOSE,0,0);

    case SC_VSCROLL:
    case SC_HSCROLL:
        TrackScrollBar(wParam,*pt32);
        break;

    case SC_MOUSEMENU:
        MENU_TrackMouseMenuBar(Win32Hwnd,wParam & 0x000F,*pt32);
        break;

    case SC_KEYMENU:
        MENU_TrackKbdMenuBar(Win32Hwnd,wParam,pt32->x);
        break;

    case SC_TASKLIST:
        OSLibWinShowTaskList(getOS2FrameWindowHandle());
        break;

    case SC_SCREENSAVE:
        if (wParam == SC_ABOUTODIN) {
            if(ShellAboutA == 0) {
                HINSTANCE hShell32 = LoadLibraryA("SHELL32");
                if(hShell32 == 0)
                    break;
                *(VOID **)&ShellAboutA = (VOID *)GetProcAddress(hShell32, "ShellAboutA");
            }
            ShellAboutA(Win32Hwnd,"Odin","Odin alpha release compiled with IBM VAC++",0);
        }
        else
        if (wParam == SC_PUTMARK)
            dprintf(("Mark requested by user\n"));
        break;

    case SC_HOTKEY:
    case SC_ARRANGE:
    case SC_NEXTWINDOW:
    case SC_PREVWINDOW:
        break;
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
