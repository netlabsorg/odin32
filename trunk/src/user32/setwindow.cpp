/* $Id: setwindow.cpp,v 1.1 1999-06-26 13:21:12 sandervl Exp $ */

/*
 * Win32 Get/SetWindowLong/Word user32 API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>

#include <wndproc.h>
#include <wndclass.h>

//******************************************************************************
//******************************************************************************
LONG WIN32API SetWindowLongA(HWND hwnd, int nIndex, LONG  arg3)
{
 LONG rc;

    dprintf(("USER32:  SetWindowLongA %X %d %X\n", hwnd, nIndex, arg3));
    if(nIndex == GWL_WNDPROC || nIndex == DWL_DLGPROC) {
        Win32WindowProc *wndproc = Win32WindowProc::FindProc(hwnd);
        if(wndproc == NULL) {//created with system class and app wants to change the handler
                dprintf(("USER32:  ERROR: SetWindowLong new WindowProc for system class\n"));
		//SvL: 26/06/99 Should no longer be possible
		DebugInt3();
                return(0);
        }
        else {
                if(!(nIndex == DWL_DLGPROC && wndproc->IsWindow() == TRUE)) {
                        rc = (LONG)wndproc->GetWin32Callback();
                        dprintf(("USER32:  SetWindowLong change WindowProc %X to %X\n", rc, arg3));
                        wndproc->SetWin32Callback((WNDPROC)arg3);
                        return(rc);
                }
                //else window that accesses it's normal window data
       }
    }
    return O32_SetWindowLong(hwnd, nIndex, arg3);
}
//******************************************************************************
//TODO: Is this always correct? (GWL_ID: window identifier??)
//******************************************************************************
LONG WIN32API SetWindowLongW(HWND arg1, int arg2, LONG  arg3)
{
    dprintf(("USER32:  SetWindowLongW %X %d %X\n", arg1, arg2, arg3));
    return SetWindowLongA(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API GetWindowWord( HWND arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowWord\n");
#endif
    return O32_GetWindowWord(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API SetWindowWord( HWND arg1, int arg2, WORD  arg3)
{
    dprintf(("USER32:  SetWindowWord\n"));
    return O32_SetWindowWord(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetWindowLongA(HWND hwnd, int nIndex)
{
 LONG rc;

    if(nIndex == GWL_WNDPROC || nIndex == DWL_DLGPROC) {
#ifdef DEBUG
	WriteLog("USER32:  GetWindowLong %X %d\n", hwnd, nIndex);
#endif
     	Win32WindowProc *window = Win32WindowProc::FindProc(hwnd);
     	if(window && !(nIndex == DWL_DLGPROC && window->IsWindow() == TRUE)) {
        	return (LONG)window->GetWin32Callback();
     	}
    }
    rc = O32_GetWindowLong(hwnd, nIndex);
    return(rc);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetWindowLongW( HWND arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowLongW\n");
#endif
    return GetWindowLongA(arg1, arg2);   //class procedures..
}
//******************************************************************************
//******************************************************************************
