/* $Id: setwindow.cpp,v 1.6 1999-06-28 16:59:43 sandervl Exp $ */

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
    if(nIndex == GWL_WNDPROC || nIndex == DWL_DLGPROC) 
    {
        Win32WindowProc *wndproc = Win32WindowProc::FindProc(hwnd);
        if(wndproc == NULL) 
        {//dialog control that has was automatically created
                dprintf(("USER32:  SetWindowLong create new Win32 to OS/2 callback\n"));
		wndproc = new Win32WindowProc((WNDPROC)arg3, (WNDPROC_O32)O32_GetWindowLong(hwnd, nIndex));
		if(wndproc) {
			O32_SetWindowLong(hwnd, nIndex, (LONG)wndproc->GetOS2Callback());
			return (LONG)wndproc->GetWin32ToOS2Callback();
		}
                else	DebugInt3();
        }
        else {
                if(!(nIndex == DWL_DLGPROC && wndproc->IsWindow() == TRUE)) {
			if(arg3 == (LONG)wndproc->GetWin32ToOS2Callback()) {
				//restore original dialog control handler
				dprintf(("SetWindowLongA: restore original dialog control handler"));
				rc = (LONG)wndproc->GetWin32Callback();
				wndproc->SetWin32Callback(0);
#ifdef DEBUG
				if(wndproc->GetOrgOS2Callback() == NULL) {
					DebugInt3();
				}
#endif
				O32_SetWindowLong(hwnd, nIndex, (LONG)wndproc->GetOrgOS2Callback());
				return rc;
			}
	                rc = (LONG)wndproc->GetWin32Callback();
			if(rc == 0) {
				//window proc that was just created with os/2 callback only
				rc = (LONG) wndproc->GetWin32ToOS2Callback();
			}
	                dprintf(("USER32:  SetWindowLong change WindowProc %X to %X\n", rc, arg3));
	                wndproc->SetWin32Callback((WNDPROC)arg3);
			O32_SetWindowLong(hwnd, nIndex, (LONG)wndproc->GetOS2Callback());
        	        return(rc);
		}
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

    if(nIndex == GWL_WNDPROC || nIndex == DWL_DLGPROC) 
    {
        dprintf(("USER32:  GetWindowLong %X %d\n", hwnd, nIndex));

     	Win32WindowProc *wndproc = Win32WindowProc::FindProc(hwnd);
        if(wndproc) {
		if(!(nIndex == DWL_DLGPROC && wndproc->IsWindow() == TRUE))
		{
			rc = (LONG)wndproc->GetWin32Callback();
			if(rc == 0) {
				//window proc that was just created with os/2 callback only
				rc = (LONG) wndproc->GetWin32ToOS2Callback();
			}
        		return rc;
		}
     	}
	else {//probably a dialog box control
                dprintf(("USER32:  GetWindowLong create new Win32 to OS/2 callback\n"));
		wndproc = new Win32WindowProc(0, (WNDPROC_O32)O32_GetWindowLong(hwnd, nIndex));
		if(wndproc)
			return (LONG)wndproc->GetWin32ToOS2Callback();
	}
    }
    rc = O32_GetWindowLong(hwnd, nIndex);
    //SvL: We must return longs here, not shorts!
    //     (fixed Winhlp32 buttons)
    if(nIndex == GWL_ID && rc == 0xffff)
	rc = 0xffffffff;
    dprintf(("USER32:  GetWindowLong %X %d returned %x\n", hwnd, nIndex, rc));
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
