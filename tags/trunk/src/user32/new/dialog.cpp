/* $Id: dialog.cpp,v 1.1 1999-07-14 08:35:34 sandervl Exp $ */

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

//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogParamA(HINSTANCE hinst, LPCSTR lpszTemplate, 
				 HWND hwndOwner, DLGPROC dlgproc, 
				 LPARAM lParamInit)
{
 HWND rc;

    if((int)lpszTemplate >> 16 != 0) {//convert string name identifier to numeric id
#ifdef DEBUG
         WriteLog("OS2CreateDialogParamA %s\n", lpszTemplate);
#endif
	 lpszTemplate = (LPCSTR)ConvertNameId(hinst, (char *)lpszTemplate);
    }
#ifdef DEBUG
    else WriteLog("OS2CreateDialogParamA %d\n", (int)lpszTemplate);
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

    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamA(HINSTANCE hinst, 
				         DLGTEMPLATE *dlgtemplate, 
				         HWND hwndParent, DLGPROC dlgproc, 
				         LPARAM lParamInit)
{
 HWND hwnd;

    return(0);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamW(HINSTANCE hinst, 
				         DLGTEMPLATE *dlgtemplate, 
				         HWND hwndParent, DLGPROC dlgproc, 
				         LPARAM lParamInit)
{
 HWND hwnd;

    return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DialogBoxIndirectParamA(HINSTANCE hinst, 
				      DLGTEMPLATE *dlgtemplate, 
				      HWND hwndParent, DLGPROC dlgproc, 
 				      LPARAM lParamInit)
{
    return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DialogBoxIndirectParamW(HINSTANCE hinst, 
				      DLGTEMPLATE *dlgtemplate, 
				      HWND hwndParent, DLGPROC dlgproc, 
				      LPARAM lParamInit)
{
    return(0);
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

    return(0);
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

    if((int)astring >> 16 != 0)	FreeAsciiString(astring);

    dprintf(("OS2DialogBoxIndirectParamA returned %d\n", rc));

    return(0);
}
//******************************************************************************
//******************************************************************************

