/* $Id: defwndproc.cpp,v 1.1 1999-05-24 20:20:01 ktk Exp $ */

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

//******************************************************************************
//******************************************************************************
LRESULT WIN32API DefWindowProcA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
////    WriteLog("DEFWNDPROC ");
////    WriteLog("*DWP*");
#endif
    switch(Msg) {
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
	
	default:
	    	return O32_DefMDIChildProc(hwnd, Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
