/* $Id: icontitle.cpp,v 1.1 2001-05-11 08:38:37 sandervl Exp $ */
/*
 * Icontitle window class.
 *
 * Copyright 1997 Alex Korobka
 *
 * Copyright 1999 Christoph Bratschi
 */

#include <stdio.h>
#include <string.h>
#include <os2win.h>
#include <heapstring.h>
#include "controls.h"
#include "icontitle.h"

#define DBG_LOCALLOG	DBG_icontitle
#include "dbglocal.h"

static  LPCSTR  emptyTitleText = "<...>";

        BOOL    bMultiLineTitle;
        HFONT   hIconTitleFont;

/***********************************************************************
 *           ICONTITLE_Init
 */
BOOL ICONTITLE_Init(void)
{
    LOGFONTA logFont;

    SystemParametersInfoA( SPI_GETICONTITLELOGFONT, 0, &logFont, 0 );
    SystemParametersInfoA( SPI_GETICONTITLEWRAP, 0, &bMultiLineTitle, 0 );
    hIconTitleFont = CreateFontIndirectA( &logFont );
    return (hIconTitleFont) ? TRUE : FALSE;
}

/***********************************************************************
 *           ICONTITLE_Create
 */
HWND ICONTITLE_Create(Win32BaseWindow *parent)
{
    HWND hWnd;
    Win32BaseWindow *win32wnd;

    if (parent->getStyle() & WS_CHILD)
        hWnd = CreateWindowExA( 0, ICONTITLECLASSNAME, NULL,
                                  WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1,
                                  parent->getWindowHandle(),0,parent->getInstance(), NULL );
    else
        hWnd = CreateWindowExA( 0, ICONTITLECLASSNAME, NULL,
                                  WS_CLIPSIBLINGS, 0, 0, 1, 1,
                                  parent->getWindowHandle(),0,parent->getInstance(), NULL );
    win32wnd = Win32BaseWindow::GetWindowFromHandle(hWnd);
    if (win32wnd)
    {
        win32wnd->setOwner(parent);     /* MDI depends on this */
        win32wnd->setStyle(win32wnd->getStyle() & ~(WS_CAPTION | WS_BORDER));
        if (parent->getStyle() & WS_DISABLED ) win32wnd->setStyle(win32wnd->getStyle() | WS_DISABLED);
        return hWnd;
    }
    return 0;
}

/***********************************************************************
 *           ICONTITLE_GetTitlePos
 */
static BOOL ICONTITLE_GetTitlePos(Win32BaseWindow *wnd,LPRECT lpRect)
{
    LPSTR str;
    int length = wnd->getOwner()->GetWindowTextLengthA();

    if( length )
    {
        str = (LPSTR)HeapAlloc( GetProcessHeap(), 0, length + 1 );
        wnd->getOwner()->GetWindowTextA(str,length);
        while( str[length - 1] == ' ' ) /* remove trailing spaces */
        {
            str[--length] = '\0';
            if( !length )
            {
                HeapFree( GetProcessHeap(), 0, str );
                break;
            }
        }
    }
    if( !length )
    {
        str = (LPSTR)emptyTitleText;
        length = lstrlenA( str );
    }

    if( str )
    {
        HDC hDC = GetDC(wnd->getWindowHandle());
        if( hDC )
        {
            HFONT hPrevFont = SelectObject( hDC, hIconTitleFont );

            SetRect( lpRect, 0, 0, GetSystemMetrics(SM_CXICONSPACING) -
                       GetSystemMetrics(SM_CXBORDER) * 2,
                       GetSystemMetrics(SM_CYBORDER) * 2 );

            DrawTextA( hDC, str, length, lpRect, DT_CALCRECT |
                         DT_CENTER | DT_NOPREFIX | DT_WORDBREAK |
                         (( bMultiLineTitle ) ? 0 : DT_SINGLELINE) );

            SelectObject( hDC, hPrevFont );
            ReleaseDC(wnd->getWindowHandle(),hDC);

            RECT rectWindow = *wnd->getOwner()->getWindowRect();
            lpRect->right += 4 * GetSystemMetrics(SM_CXBORDER) - lpRect->left;
            lpRect->left = rectWindow.left + GetSystemMetrics(SM_CXICON) / 2 -
                                      (lpRect->right - lpRect->left) / 2;
            lpRect->bottom -= lpRect->top;
            lpRect->top = rectWindow.top + GetSystemMetrics(SM_CYICON);
        }
        if( str != emptyTitleText ) HeapFree( GetProcessHeap(), 0, str );
        return ( hDC ) ? TRUE : FALSE;
    }
    return FALSE;
}

/***********************************************************************
 *           ICONTITLE_Paint
 */
static BOOL ICONTITLE_Paint(Win32BaseWindow *wnd, HDC hDC, BOOL bActive )
{
    HFONT hPrevFont;
    HBRUSH hBrush = 0;
    COLORREF textColor = 0;

    if( bActive )
    {
        hBrush = GetSysColorBrush(COLOR_ACTIVECAPTION);
        textColor = GetSysColor(COLOR_CAPTIONTEXT);
    }
    else
    {
        if( wnd->getStyle() & WS_CHILD )
        {
            hBrush = (HBRUSH)wnd->getClass()->getClassLongA(GCL_HBRBACKGROUND);
            if( hBrush )
            {
                INT level;
                LOGBRUSH logBrush;
                GetObjectA( hBrush, sizeof(logBrush), &logBrush );
                level = GetRValue(logBrush.lbColor) +
                           GetGValue(logBrush.lbColor) +
                              GetBValue(logBrush.lbColor);
                if( level < (0x7F * 3) )
                    textColor = RGB( 0xFF, 0xFF, 0xFF );
            }
            else
                hBrush = GetStockObject( WHITE_BRUSH );
        }
        else
        {
            hBrush = GetStockObject( BLACK_BRUSH );
            textColor = RGB( 0xFF, 0xFF, 0xFF );
        }
    }

    //FillWindow16(wnd->getParent()->getWindowHandle(),hwnd,hDC,hBrush); //CB: todo

    hPrevFont = SelectObject( hDC, hIconTitleFont );
    if( hPrevFont )
    {
        RECT  rect,rectWindow = *wnd->getWindowRect();
        INT     length;
        char    buffer[80];

        rect.left = rect.top = 0;
        rect.right = rectWindow.right - rectWindow.left;
        rect.bottom = rectWindow.bottom - rectWindow.top;

        length = wnd->getOwner()->GetWindowTextA(buffer, 80 );
        SetTextColor( hDC, textColor );
        SetBkMode( hDC, TRANSPARENT );

        DrawTextA( hDC, buffer, length, &rect, DT_CENTER | DT_NOPREFIX |
                     DT_WORDBREAK | ((bMultiLineTitle) ? 0 : DT_SINGLELINE) );

        SelectObject( hDC, hPrevFont );
    }
    return ( hPrevFont ) ? TRUE : FALSE;
}

/***********************************************************************
 *           IconTitleWndProc
 */
LRESULT WINAPI IconTitleWndProc( HWND hWnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam )
{
    LRESULT retvalue;
    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hWnd);

    switch( msg )
    {
        case WM_NCHITTEST:
             retvalue = HTCAPTION;
             goto END;
        case WM_NCMOUSEMOVE:
        case WM_NCLBUTTONDBLCLK:
             retvalue = SendMessageA( wnd->getOwner()->getWindowHandle(), msg, wParam, lParam );
             goto END;
        case WM_ACTIVATE:
             if( wParam ) SetActiveWindow( wnd->getOwner()->getWindowHandle() );
             /* fall through */

        case WM_CLOSE:
             retvalue = 0;
             goto END;
        case WM_SHOWWINDOW:
             if( wnd && wParam )
             {
                 RECT titleRect;

                 ICONTITLE_GetTitlePos( wnd, &titleRect );
                 if( wnd->getOwner()->getNextChild() != wnd )  /* keep icon title behind the owner */
                     SetWindowPos( hWnd, wnd->getOwner()->getWindowHandle(),
                                     titleRect.left, titleRect.top,
                                     titleRect.right, titleRect.bottom, SWP_NOACTIVATE );
                 else
                     SetWindowPos( hWnd, 0, titleRect.left, titleRect.top,
                                     titleRect.right, titleRect.bottom,
                                     SWP_NOACTIVATE | SWP_NOZORDER );
             }
             retvalue = 0;
             goto END;
        case WM_ERASEBKGND:
             if( wnd )
             {
                 Win32BaseWindow* iconWnd = wnd->getOwner();

                 if( iconWnd->getStyle() & WS_CHILD )
                     lParam = SendMessageA( iconWnd->getWindowHandle(), WM_ISACTIVEICON, 0, 0 );
                 else
                     lParam = (iconWnd->getWindowHandle() == GetActiveWindow());

                 if( ICONTITLE_Paint( wnd, (HDC)wParam, (BOOL)lParam ) )
                     ValidateRect( hWnd, NULL );
                 retvalue = 1;
                 goto END;
             }
    }

    retvalue = DefWindowProcA( hWnd, msg, wParam, lParam );
END:
    return retvalue;
}

BOOL ICONTITLE_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(ICONTITLECLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)IconTitleWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hCursor       = LoadCursorA(0,IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)0;
    wndClass.lpszClassName = ICONTITLECLASSNAME;

    return RegisterClassA(&wndClass);
}
//******************************************************************************
//******************************************************************************
BOOL ICONTITLE_Unregister()
{
    if (GlobalFindAtomA(ICONTITLECLASSNAME))
        return UnregisterClassA(ICONTITLECLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}
//******************************************************************************
//******************************************************************************
