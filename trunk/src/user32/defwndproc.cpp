/* $Id: defwndproc.cpp,v 1.6 1999-06-27 16:23:23 sandervl Exp $ */

/*
 * Win32 default window API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include "user32.h"
#include "syscolor.h"
#include <wndproc.h>

#ifdef DEBUG
char *GetMsgText(int Msg);
#endif

//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefWindowProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("DEFWNDPROC ");
////    WriteLog("*DWP*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefWindowProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAFMO (yet another feature missing in Open32)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
		return 0;

	case WM_MOUSEACTIVATE:
	{
		DWORD dwStyle = GetWindowLongA(hwnd, GWL_STYLE);
		DWORD dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
		dprintf(("DefWndProc: WM_MOUSEACTIVATE for %x Msg %s", hwnd, GetMsgText(HIWORD(lParam))));
		if(dwStyle & WS_CHILD && !(dwExStyle & WS_EX_NOPARENTNOTIFY) )
		{
			HWND hwndParent = GetParent(hwnd);
			Win32WindowProc *parentwnd = Win32WindowProc::FindProc(hwndParent);
			if(parentwnd) {
				LRESULT rc = parentwnd->SendMessageA(hwndParent, WM_MOUSEACTIVATE, wParam, lParam );
				if(rc)	return rc;
			}
		}
		return (LOWORD(lParam) == HTCAPTION) ? MA_NOACTIVATE : MA_ACTIVATE;
	}
	case WM_SETCURSOR:
	{
		DWORD dwStyle = GetWindowLongA(hwnd, GWL_STYLE);
		DWORD dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
		dprintf(("DefWndProc: WM_SETCURSOR for %x Msg %s", hwnd, GetMsgText(HIWORD(lParam))));
		if(dwStyle & WS_CHILD && !(dwExStyle & WS_EX_NOPARENTNOTIFY) )
		{
			HWND hwndParent = GetParent(hwnd);

			Win32WindowProc *parentwnd = Win32WindowProc::FindProc(hwndParent);
			if(parentwnd) {
				LRESULT rc = parentwnd->SendMessageA(hwndParent, WM_SETCURSOR, wParam, lParam);
				if(rc)	return rc;
			}
		}
		return O32_DefWindowProc(hwnd, Msg, wParam, lParam);
	}
        default:
                return O32_DefWindowProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//NOTE: Unicode msg translation!
//******************************************************************************
LRESULT WIN32API DefWindowProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DWPW*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefWindowProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
		return 0;

        default:
                return O32_DefWindowProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefDlgProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DDP*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
		return 0;

        default:
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//NOTE: Unicode msg translation!
//******************************************************************************
LRESULT WIN32API DefDlgProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DDPW*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
		return 0;

        default:
                return O32_DefDlgProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefFrameProcA(HWND hwndFrame, HWND hwndClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DFP*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwndClient, GWL_STYLE, GetWindowLongA (hwndClient, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwndClient, GWL_STYLE, GetWindowLongA (hwndClient, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefFrameProc(hwndFrame, hwndClient, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwndFrame));
		return 0;

        default:
                return O32_DefFrameProc(hwndFrame, hwndClient, Msg, wParam, lParam);
    }
}
//******************************************************************************
//NOTE: Unicode msg translation!
//******************************************************************************
LRESULT WIN32API DefFrameProcW(HWND hwndFrame, HWND hwndClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DFPW*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwndClient, GWL_STYLE, GetWindowLongA (hwndClient, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwndClient, GWL_STYLE, GetWindowLongA (hwndClient, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefFrameProc(hwndFrame, hwndClient, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwndFrame));
		return 0;

        default:
                return O32_DefFrameProc(hwndFrame, hwndClient, Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefMDIChildProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DMP*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefMDIChildProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
		return 0;

        default:
                return O32_DefMDIChildProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//NOTE: Unicode msg translation!
//******************************************************************************
LRESULT WIN32API DefMDIChildProcW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("*DMPW*");
#endif
    switch(Msg) {
        case WM_SETREDRAW: //Open32 does not set the visible flag
                if(wParam)
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) | WS_VISIBLE);
                else
                  SetWindowLongA (hwnd, GWL_STYLE, GetWindowLongA (hwnd, GWL_STYLE) & ~WS_VISIBLE);
                return O32_DefMDIChildProc(hwnd, Msg, wParam, lParam);
        case WM_NCCREATE://SvL: YAOFM (yet another open32 feature missing)
                return(TRUE);
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORSCROLLBAR:
                SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
                return GetSysColorBrush(COLOR_BTNFACE);

	case WM_PARENTNOTIFY: //Open32 doesn't like receiving those!!
		dprintf(("DefWndProc: WM_PARENTNOTIFY for %x", hwnd));
		return 0;

        default:
                return O32_DefMDIChildProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
