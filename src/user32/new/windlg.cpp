/* $Id: windlg.cpp,v 1.4 1999-09-05 15:53:10 sandervl Exp $ */
/*
 * Win32 dialog apis for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 * Parts based on Wine code (990815; window\dialog.c)
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <ctype.h>
#include <wchar.h>
#include <os2win.h>
#include <misc.h>
#include <string.h>
#include <stdlib.h>
#include "win32wbase.h"
#include "win32dlg.h"
#include <heapstring.h>
#include <win\drive.h>

static INT  DIALOG_DlgDirListW( HWND hDlg, LPWSTR spec, INT idLBox, INT idStatic, UINT attrib, BOOL combo );
static INT  DIALOG_DlgDirList( HWND hDlg, LPSTR spec, INT idLBox, INT idStatic, UINT attrib, BOOL combo );
static BOOL DIALOG_DlgDirSelect( HWND hwnd, LPSTR str, INT len, INT id, BOOL win32, BOOL unicode, BOOL combo );

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
    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxIndirectParamA, dialog %x not found", hwnd));
            return 0;
        }
        return dialog->doDialogBox();
    }
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
    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxIndirectParamW, dialog %x not found", hwnd));
            return 0;
        }
        return dialog->doDialogBox();
    }
    return -1;
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamA(HINSTANCE hInst, LPCSTR lpszTemplate, HWND hwndOwner,
                     DLGPROC dlgproc, LPARAM  lParamInit)
{
    HWND hwnd = CreateDialogParamA( hInst, lpszTemplate, hwndOwner, dlgproc, lParamInit);

    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxParamA, dialog %x not found", hwnd));
            return 0;
        }
        return dialog->doDialogBox();
    }
    return -1;
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamW(HINSTANCE hInst, LPCWSTR lpszTemplate, HWND hwndOwner,
                             DLGPROC dlgproc, LPARAM lParamInit)
{
    HWND hwnd = CreateDialogParamW( hInst, lpszTemplate, hwndOwner, dlgproc, lParamInit);

    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxParamW, dialog %x not found", hwnd));
            return 0;
        }
        return dialog->doDialogBox();
    }
    return -1;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDlgButtonChecked( HWND hwnd, UINT id)
{
    dprintf(("USER32:  IsDlgButtonChecked\n"));
    //CB: get button state
    return (BOOL)SendDlgItemMessageA(hwnd, id,BM_GETCHECK,0,0);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetNextDlgTabItem(HWND hwndDlg, HWND hwndCtrl, BOOL fPrevious)
{
  Win32Dialog *dialog;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwndDlg);
    if(!dialog || !dialog->IsDialog()) {
        dprintf(("GetNextDlgTabItem, window %x not found", hwndDlg));
        return 0;
    }
    dprintf(("USER32:  GetNextDlgTabItem\n"));
    return dialog->getNextDlgTabItem(hwndCtrl, fPrevious);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetDlgItem(HWND hwnd, int id)
{
  Win32Dialog *dialog;
  Win32BaseWindow *dlgcontrol;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog || !dialog->IsDialog()) {
        dprintf(("GetDlgItem, window %x not found", hwnd));
        return 0;
    }
    dprintf(("USER32:  GetDlgItem\n"));
    dlgcontrol = dialog->getDlgItem(id);
    if(dlgcontrol) {
        return dlgcontrol->getWindowHandle();
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDlgCtrlID(HWND hwnd)
{
  Win32BaseWindow *dlgcontrol;

    dlgcontrol = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dlgcontrol) {
        dprintf(("GetDlgCtrlID, control %x not found", hwnd));
        return 0;
    }
    dprintf(("USER32:  GetDlgCtrlID\n"));
    return dlgcontrol->getWindowId();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndDialog(HWND hwnd, int retval)
{
  Win32Dialog *dialog;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog || !dialog->IsDialog()) {
        dprintf(("GetDlgItem, window %x not found", hwnd));
        return 0;
    }
    dprintf(("USER32: EndDialog\n"));
    return dialog->endDialog(retval);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CheckDlgButton( HWND hwnd, int id, UINT check)
{
    dprintf(("USER32:  CheckDlgButton\n"));
    //CB: set button state
    return (BOOL)SendDlgItemMessageA(hwnd, id, BM_SETCHECK, check,0);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDlgItemTextA(HWND hwnd, int id, LPSTR lpszName, UINT cch)
{
    return SendDlgItemMessageA( hwnd, id, WM_GETTEXT, cch, (LPARAM)lpszName);
}
//*****************************************************************************
//*****************************************************************************
UINT WIN32API GetDlgItemTextW(HWND   hwnd, int id, LPWSTR lpszName, UINT cch)
{
    return SendDlgItemMessageW( hwnd, id, WM_GETTEXT, cch, (LPARAM)lpszName);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemInt( HWND hwnd, int idControl, UINT uValue, BOOL  fSigned)
{
  char str[20];

    dprintf(("USER32:  SetDlgItemInt\n"));

    if (fSigned)
            sprintf( str, "%d", (INT)uValue );
    else    sprintf( str, "%u", uValue );

    return SendDlgItemMessageA( hwnd, idControl, WM_SETTEXT, 0, (LPARAM)str );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemTextA( HWND hwnd, int id, LPCSTR lpszName)
{
    return SendDlgItemMessageA( hwnd, id, WM_SETTEXT, 0, (LPARAM)lpszName );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemTextW( HWND hwnd, int id, LPCWSTR lpszName)
{
    return SendDlgItemMessageW( hwnd, id, WM_SETTEXT, 0, (LPARAM)lpszName );
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDlgItemInt( HWND arg1, int arg2, PBOOL arg3, BOOL  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  GetDlgItemInt\n");
#endif
    return O32_GetDlgItemInt(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetNextDlgGroupItem( HWND arg1, HWND arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetNextDlgGroupItem\n");
#endif
    return O32_GetNextDlgGroupItem(arg1, arg2, arg3);
}
/***********************************************************************
 *           GetDialogBaseUnits   (USER.243) (USER32.233)
 */
DWORD WIN32API GetDialogBaseUnits(void)
{
    return Win32Dialog::GetDialogBaseUnits();
}
//******************************************************************************
//******************************************************************************
INT WIN32API DlgDirListA(HWND hDlg, LPSTR spec, INT idLBox, INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirList( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}
//******************************************************************************
//******************************************************************************
int WIN32API DlgDirListW(HWND hDlg, LPWSTR spec, INT idLBox, INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}
//******************************************************************************
//******************************************************************************
INT WIN32API DlgDirListComboBoxA(HWND hDlg, LPSTR spec, INT idCBox, INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirList( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}
//******************************************************************************
//******************************************************************************
INT WIN32API DlgDirListComboBoxW( HWND hDlg, LPWSTR spec, INT idCBox, INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectComboBoxExA(HWND hwnd, LPSTR str, INT len, INT id )
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id, TRUE, FALSE, TRUE );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectComboBoxExW(HWND hwnd, LPWSTR str, INT len, INT id)
{
    return DIALOG_DlgDirSelect( hwnd, (LPSTR)str, len, id, TRUE, TRUE, TRUE );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectExA(HWND hwnd, LPSTR str, INT len, INT id)
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id, TRUE, FALSE, FALSE );
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectExW(HWND hwnd, LPWSTR str, INT len, INT id)
{
    return DIALOG_DlgDirSelect( hwnd, (LPSTR)str, len, id, TRUE, TRUE, FALSE );
}
/**********************************************************************
 *           DIALOG_DlgDirSelect
 *
 * Helper function for DlgDirSelect*
 */
static BOOL DIALOG_DlgDirSelect( HWND hwnd, LPSTR str, INT len,
                                 INT id, BOOL win32, BOOL unicode,
                                 BOOL combo )
{
    char *buffer, *ptr;
    INT item, size;
    BOOL ret;
    HWND listbox = GetDlgItem( hwnd, id );

    if (!listbox) return FALSE;

    item = SendMessageA(listbox, combo ? CB_GETCURSEL
                                             : LB_GETCURSEL, 0, 0 );
    if (item == LB_ERR) return FALSE;
    size = SendMessageA(listbox, combo ? CB_GETLBTEXTLEN
                                             : LB_GETTEXTLEN, 0, 0 );
    if (size == LB_ERR) return FALSE;

    if (!(buffer = (char *)malloc( size+1 ))) return FALSE;

    SendMessageA( listbox, combo ? CB_GETLBTEXT : LB_GETTEXT,
                  item, (LPARAM)buffer );

    if ((ret = (buffer[0] == '[')))  /* drive or directory */
    {
        if (buffer[1] == '-')  /* drive */
        {
            buffer[3] = ':';
            buffer[4] = 0;
            ptr = buffer + 2;
        }
        else
        {
            buffer[strlen(buffer)-1] = '\\';
            ptr = buffer + 1;
        }
    }
    else ptr = buffer;

    if (unicode) lstrcpynAtoW( (LPWSTR)str, ptr, len );
    else lstrcpynA( str, ptr, len );

    free( buffer );
    return ret;
}


/**********************************************************************
 *      DIALOG_DlgDirList
 *
 * Helper function for DlgDirList*
 */
static INT DIALOG_DlgDirList( HWND hDlg, LPSTR spec, INT idLBox,
                              INT idStatic, UINT attrib, BOOL combo )
{
    int drive;
    HWND hwnd;
    LPSTR orig_spec = spec;

#define SENDMSG(msg,wparam,lparam) \
    ((attrib & DDL_POSTMSGS) ? PostMessageA( hwnd, msg, wparam, lparam ) \
                             : SendMessageA( hwnd, msg, wparam, lparam ))

    if (spec && spec[0] && (spec[1] == ':'))
    {
        drive = _toupper( spec[0] ) - 'A';
        spec += 2;
        if (!DRIVE_SetCurrentDrive( drive )) return FALSE;
    }
    else drive = DRIVE_GetCurrentDrive();

    /* If the path exists and is a directory, chdir to it */
    if (!spec || !spec[0] || DRIVE_Chdir( drive, spec )) spec = "*.*";
    else
    {
        char *p, *p2;
        p = spec;
        if ((p2 = strrchr( p, '\\' ))) p = p2;
        if ((p2 = strrchr( p, '/' ))) p = p2;
        if (p != spec)
        {
            char sep = *p;
            *p = 0;
            if (!DRIVE_Chdir( drive, spec ))
            {
                *p = sep;  /* Restore the original spec */
                return FALSE;
            }
            spec = p + 1;
        }
    }

    if (idLBox && ((hwnd = GetDlgItem( hDlg, idLBox )) != 0))
    {
        SENDMSG( combo ? CB_RESETCONTENT : LB_RESETCONTENT, 0, 0 );
        if (attrib & DDL_DIRECTORY)
        {
            if (!(attrib & DDL_EXCLUSIVE))
            {
                if (SENDMSG( combo ? CB_DIR : LB_DIR,
                             attrib & ~(DDL_DIRECTORY | DDL_DRIVES),
                             (LPARAM)spec ) == LB_ERR)
                    return FALSE;
            }
            if (SENDMSG( combo ? CB_DIR : LB_DIR,
                       (attrib & (DDL_DIRECTORY | DDL_DRIVES)) | DDL_EXCLUSIVE,
                         (LPARAM)"*.*" ) == LB_ERR)
                return FALSE;
        }
        else
        {
            if (SENDMSG( combo ? CB_DIR : LB_DIR, attrib,
                         (LPARAM)spec ) == LB_ERR)
                return FALSE;
        }
    }

    if (idStatic && ((hwnd = GetDlgItem( hDlg, idStatic )) != 0))
    {
        char temp[512], curpath[512];
        int drive = DRIVE_GetCurrentDrive();
        strcpy( temp, "A:\\" );
        temp[0] += drive;
        lstrcpynA( temp + 3, DRIVE_GetDosCwd(curpath, drive), sizeof(temp)-3 );
        CharLowerA( temp );
        /* Can't use PostMessage() here, because the string is on the stack */
        SetDlgItemTextA( hDlg, idStatic, temp );
    }

    if (orig_spec && (spec != orig_spec))
    {
        /* Update the original file spec */
        char *p = spec;
        while ((*orig_spec++ = *p++));
    }

    return TRUE;
#undef SENDMSG
}


/**********************************************************************
 *      DIALOG_DlgDirListW
 *
 * Helper function for DlgDirList*32W
 */
static INT DIALOG_DlgDirListW( HWND hDlg, LPWSTR spec, INT idLBox,
                               INT idStatic, UINT attrib, BOOL combo )
{
    if (spec)
    {
        LPSTR specA = HEAP_strdupWtoA( GetProcessHeap(), 0, spec );
        INT ret = DIALOG_DlgDirList( hDlg, specA, idLBox, idStatic,
                                       attrib, combo );
        lstrcpyAtoW( spec, specA );
        HeapFree( GetProcessHeap(), 0, specA );
        return ret;
    }
    return DIALOG_DlgDirList( hDlg, NULL, idLBox, idStatic, attrib, combo );
}
//******************************************************************************
//******************************************************************************

