/* $Id: dialog.cpp,v 1.8 1999-09-04 19:42:27 sandervl Exp $ */

/*
 * Win32 dialog API functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (Wine port & OS/2 adaption)
 *
 *
 * Based on Wine code (990815; windows\dialog.c)
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include "win32wbase.h"
#include "win32dlg.h"

//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogParamA(HINSTANCE hInst, LPCSTR lpszTemplate,
                                 HWND hwndOwner, DLGPROC dlgproc,
                                 LPARAM lParamInit)
{
  HANDLE hrsrc = FindResourceA( hInst, lpszTemplate, RT_DIALOGA );

    if (!hrsrc)
        return 0;

    return CreateDialogIndirectParamA(hInst, (DLGTEMPLATE*)LoadResource(hInst, hrsrc),
                                      hwndOwner, dlgproc, lParamInit);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogParamW(HINSTANCE hInst, LPCWSTR lpszTemplate,
                 HWND hwndOwner, DLGPROC dlgproc,
                 LPARAM lParamInit)
{
  HANDLE hrsrc = FindResourceW( hInst, lpszTemplate, RT_DIALOGW );

    if (!hrsrc)
        return 0;

    return CreateDialogIndirectParamW(hInst, (DLGTEMPLATE*)LoadResource(hInst, hrsrc),
                                      hwndOwner, dlgproc, lParamInit);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamA(HINSTANCE hInst,
                         DLGTEMPLATE *dlgtemplate,
                         HWND hwndParent, DLGPROC dlgproc,
                         LPARAM lParamInit)
{
 Win32Dialog *dialog;

    dprintf(("CreateDialogIndirectParamA: %x %x %x %x %x", hInst, dlgtemplate, hwndParent, dlgproc, lParamInit));

    if (!dlgtemplate) return 0;

  	dialog = new Win32Dialog(hInst, (LPCSTR)dlgtemplate, hwndParent, dlgproc, lParamInit, FALSE);

    if(dialog == NULL)
    {
        dprintf(("Win32Dialog creation failed!!"));
        return 0;
    }
    if(GetLastError() != 0)
    {
        dprintf(("Win32Dialog error found!!"));
        delete dialog;
        return 0;
    }
    return dialog->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamW(HINSTANCE hInst,
                         DLGTEMPLATE *dlgtemplate,
                         HWND hwndParent, DLGPROC dlgproc,
                         LPARAM lParamInit)
{
 Win32Dialog *dialog;

    dprintf(("CreateDialogIndirectParamW: %x %x %x %x %x", hInst, dlgtemplate, hwndParent, dlgproc, lParamInit));

    if (!dlgtemplate) return 0;

  	dialog = new Win32Dialog(hInst, (LPCSTR)dlgtemplate, hwndParent, dlgproc, lParamInit, TRUE);

    if(dialog == NULL)
    {
        dprintf(("Win32Dialog creation failed!!"));
        return 0;
    }
    if(GetLastError() != 0)
    {
        dprintf(("Win32Dialog error found!!"));
        delete dialog;
        return 0;
    }
    return dialog->getWindowHandle();
}
//******************************************************************************
//******************************************************************************
INT  WIN32API DialogBoxIndirectParamA(HINSTANCE hInst,
                      DLGTEMPLATE *dlgtemplate,
                      HWND hwndParent, DLGPROC dlgproc,
                      LPARAM lParamInit)
{
    HWND hwnd = CreateDialogIndirectParamA(hInst, dlgtemplate, hwndParent, dlgproc,
                                           lParamInit);
    //TODO:
    if (hwnd) return 1; //return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}
//******************************************************************************
//******************************************************************************
INT  WIN32API DialogBoxIndirectParamW(HINSTANCE hInst, DLGTEMPLATE *dlgtemplate,
                                      HWND hwndParent, DLGPROC dlgproc,
                                      LPARAM lParamInit)
{
    HWND hwnd = CreateDialogIndirectParamW(hInst, dlgtemplate, hwndParent, dlgproc,
                                           lParamInit);
    //TODO:
    if (hwnd) return 1; //return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamA(HINSTANCE hInst, LPCSTR lpszTemplate, HWND hwndOwner,
                     DLGPROC dlgproc, LPARAM  lParamInit)
{
    HWND hwnd = CreateDialogParamA( hInst, lpszTemplate, hwndOwner, dlgproc, lParamInit);
    //TODO:
    if (hwnd) return 1; //return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamW(HINSTANCE hInst, LPCWSTR lpszTemplate, HWND hwndOwner,
                             DLGPROC dlgproc, LPARAM lParamInit)
{
    HWND hwnd = CreateDialogParamW( hInst, lpszTemplate, hwndOwner, dlgproc, lParamInit);
    //TODO:
    if (hwnd) return 1; //return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}
//******************************************************************************
//******************************************************************************

