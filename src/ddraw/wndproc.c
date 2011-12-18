/*	User-based primary surface driver
 *
 * Copyright 2000 TransGaming Technologies Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <windows.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "winerror.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ddraw);

static LRESULT WINAPI DirectDrawSurface_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DirectDrawSurface_RegisterClass(void)
{
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = DirectDrawSurface_WndProc;
    wc.cbWndExtra  = sizeof(DWORD);
    wc.hCursor     = (HCURSOR)IDC_ARROWA;
    wc.lpszClassName = "WINE_DDRAW";
    RegisterClassA(&wc);
}

void DirectDrawSurface_UnregisterClass(void)
{
    UnregisterClassA("WINE_DDRAW", 0);
}

static LRESULT WINAPI DirectDrawSurface_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret;

    HWND window = (HWND)GetWindowLongA(hwnd, 0);

    if(window) {
	switch (msg) {
	case WM_DESTROY:
	case WM_NCDESTROY:
	case WM_SHOWWINDOW:
	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
	case WM_SIZE:
	case WM_MOVE:
	case WM_ERASEBKGND:
	case WM_SYNCPAINT:
	    /* since we're pretending fullscreen,
	     * let's not pass these on to the app */
	    ret = DefWindowProcA(hwnd, msg, wParam, lParam);
	    break;
	case WM_NCHITTEST:
	    ret = HTCLIENT;
	    break;
	case WM_MOUSEACTIVATE:
	    ret = MA_NOACTIVATE;
	    break;
	case WM_PAINT:
	    {
		PAINTSTRUCT ps;
		HDC dc;
		dc = BeginPaint(hwnd, &ps);
		/* call User_copy_to_screen? */
		EndPaint(hwnd, &ps);
		ret = 0;
	    }
	    break;
	default:
	    ret = window ? SendMessageA(window, msg, wParam, lParam)
			 : DefWindowProcA(hwnd, msg, wParam, lParam);
	}
    } else {
	if (msg == WM_CREATE) {
	    CREATESTRUCTA *cs = (CREATESTRUCTA *)lParam;
	    HWND hwndOwner = (HWND)cs->lpCreateParams;
	    SetWindowLongA(hwnd, 0, (LONG)hwndOwner);
	}
	ret = DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return ret;
}

HWND DirectDrawSurface_CreateWindow(DWORD dwWidth, DWORD dwHeight, HWND hwndOwner)
{
    HWND hwnd;

    hwnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
                           "WINE_DDRAW", "DirectDraw",
                           WS_POPUP, 0, 0, dwWidth, dwHeight,
                           GetDesktopWindow(), 0, 0, (LPVOID)hwndOwner);

    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
 	         SWP_DEFERERASE|SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|
	         SWP_NOREDRAW|SWP_NOSENDCHANGING|SWP_NOSIZE|SWP_SHOWWINDOW);
    UpdateWindow(hwnd);

    return hwnd;
}
