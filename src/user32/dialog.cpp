/* $Id: dialog.cpp,v 1.5 1999-06-26 13:46:21 sandervl Exp $ */

/*
 * Win32 dialog API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <nameid.h>
#include "user32.h"
#include "wndproc.h"
#include "wndclass.h"
#include "dlgconvert.h"

//TODO: dlgproc == NULL
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogParamA(HINSTANCE hinst, LPCSTR lpszTemplate, 
				 HWND hwndOwner, DLGPROC dlgproc, 
				 LPARAM lParamInit)
{
 HWND rc;
 Win32WindowProc *dialog = NULL;

    if((int)lpszTemplate >> 16 != 0) {//convert string name identifier to numeric id
#ifdef DEBUG
         WriteLog("OS2CreateDialogParamA %s\n", lpszTemplate);
#endif
	 lpszTemplate = (LPCSTR)ConvertNameId(hinst, (char *)lpszTemplate);
    }
#ifdef DEBUG
    else WriteLog("OS2CreateDialogParamA %d\n", (int)lpszTemplate);
#endif

    if(dlgproc) {
	 dialog = new Win32WindowProc((WNDPROC)dlgproc);
	 rc = O32_CreateDialogParam(hinst, lpszTemplate, hwndOwner, (DLGPROC_O32)dialog->GetOS2Callback(), lParamInit);
    }
    else rc = O32_CreateDialogParam(hinst, lpszTemplate, hwndOwner, (DLGPROC_O32)O32_DefDlgProc, lParamInit);
    
    if(rc == 0 && dialog) {
	 delete(dialog);
    }

#ifdef DEBUG
    WriteLog("CreateDialogParamA %X %X %d returned %X (%d)\n", hinst, hwndOwner, lParamInit, rc, GetLastError());
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogParamW(HINSTANCE hinst, LPCWSTR lpszTemplate, 
				 HWND hwndOwner, DLGPROC dlgproc, 
				 LPARAM lParamInit)
{
 HWND rc;

    if((int)lpszTemplate >> 16 != 0) {//convert string name identifier to numeric id
	 char *astring = UnicodeToAsciiString((LPWSTR)lpszTemplate);
#ifdef DEBUG
         WriteLog("OS2CreateDialogParamW %s\n", astring);
#endif
	 lpszTemplate = (LPWSTR)ConvertNameId(hinst, astring);
	 FreeAsciiString(astring);
    }
#ifdef DEBUG
    else WriteLog("OS2CreateDialogParamW %d\n", (int)lpszTemplate);
#endif

    Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgproc);
    
    rc = O32_CreateDialogParam(hinst, (LPCSTR)lpszTemplate, hwndOwner, (DLGPROC_O32)dialog->GetOS2Callback(), lParamInit);
    if(rc == 0) {
	 delete(dialog);
    }

#ifdef DEBUG
    WriteLog("CreateDialogParamW returned %X\n", rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamA(HINSTANCE hinst, 
				         DLGTEMPLATE *dlgtemplate, 
				         HWND hwndParent, DLGPROC dlgproc, 
				         LPARAM lParamInit)
{
 HWND	      hwnd;
 DLGTEMPLATE *os2dlg;

    os2dlg = ConvertWin32DlgTemplate(dlgtemplate);
    Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgproc, os2dlg);

    hwnd = O32_CreateDialogIndirectParam(hinst, os2dlg, hwndParent, (DLGPROC_O32)dialog->GetOS2Callback(), lParamInit);
    if(hwnd == 0) {
	 delete(dialog);
    }
#ifdef DEBUG
    WriteLog("CreateDialogIndirectParamA returned %X\n", hwnd);
#endif
    return(hwnd);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamW(HINSTANCE hinst, 
				         DLGTEMPLATE *dlgtemplate, 
				         HWND hwndParent, DLGPROC dlgproc, 
				         LPARAM lParamInit)
{
 HWND	      hwnd;
 DLGTEMPLATE *os2dlg;

    os2dlg = ConvertWin32DlgTemplate(dlgtemplate);
    Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgproc, os2dlg);

    hwnd = O32_CreateDialogIndirectParam(hinst, os2dlg, hwndParent, (DLGPROC_O32)dialog->GetOS2Callback(), lParamInit);
    if(hwnd == 0) {
	 delete(dialog);
    }
    dprintf(("CreateDialogIndirectParamW returned %X\n", hwnd));
    return(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DialogBoxIndirectParamA(HINSTANCE hinst, 
				      DLGTEMPLATE *dlgtemplate, 
				      HWND hwndParent, DLGPROC dlgproc, 
 				      LPARAM lParamInit)
{
 BOOL rc;
 DLGTEMPLATE *os2dlg;

    os2dlg = ConvertWin32DlgTemplate(dlgtemplate);
    Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgproc, os2dlg);

    rc = O32_DialogBoxIndirectParam(hinst, os2dlg, hwndParent, (DLGPROC_O32)dialog->GetOS2Callback(), lParamInit);
    //dialog already destroyed when this returns
    dprintf(("OS2DialogBoxIndirectParamA returned %X\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DialogBoxIndirectParamW(HINSTANCE hinst, 
				      DLGTEMPLATE *dlgtemplate, 
				      HWND hwndParent, DLGPROC dlgproc, 
				      LPARAM lParamInit)
{
 BOOL rc;
 DLGTEMPLATE *os2dlg;

    os2dlg = ConvertWin32DlgTemplate(dlgtemplate);
    Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgproc, os2dlg);

    rc = O32_DialogBoxIndirectParam(hinst, os2dlg, hwndParent, (DLGPROC_O32)dialog->GetOS2Callback(), lParamInit);
    //dialog already destroyed when this returns
    dprintf(("OS2DialogBoxIndirectParamW returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamA(HINSTANCE hinst, LPCSTR lpszTemplate, HWND hwndOwner,
 	  	    	     DLGPROC dlgprc, LPARAM  lParamInit)
{
 int rc;

    if((int)lpszTemplate >> 16 != 0) {//convert string name identifier to numeric id
	dprintf(("DialogBoxParam %s\n", lpszTemplate));
	lpszTemplate = (LPCSTR)ConvertNameId(hinst, (char *)lpszTemplate);
    }
    else {	
	dprintf(("DialogBoxParam %d\n", (int)lpszTemplate));
    }

    Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgprc);
    rc = O32_DialogBoxParam(hinst, lpszTemplate, hwndOwner, (DLGPROC_O32)dialog->GetOS2Callback(), lParamInit);

    dprintf(("DialogBoxParam returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamW(HINSTANCE arg1, LPCWSTR arg2, HWND arg3, 
		  	     DLGPROC arg4, LPARAM arg5)
{
 int   rc;
 char *astring = NULL;


    if((int)arg2 >> 16 != 0) {
	    astring = UnicodeToAsciiString((LPWSTR)arg2);
    }
    else    astring = (char *)arg2;
    dprintf(("OS2DialogBoxParamW\n"));

    Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)arg4);
    rc = DialogBoxParamA(arg1, astring, arg3, arg4, arg5);
 
    if((int)astring >> 16 != 0)	FreeAsciiString(astring);

    dprintf(("OS2DialogBoxIndirectParamA returned %d\n", rc));

    return(rc);
}
//******************************************************************************
//******************************************************************************

