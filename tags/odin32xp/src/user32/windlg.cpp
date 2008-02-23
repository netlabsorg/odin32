/* $Id: windlg.cpp,v 1.38 2005-06-26 15:16:18 sao2l02 Exp $ */
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "win32wbase.h"
#include "win32dlg.h"
#include <heapstring.h>
//#include <win\drive.h>
#include <custombuild.h>

#define DBG_LOCALLOG	DBG_windlg
#include "dbglocal.h"

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

    if (!hrsrc) {
        dprintf(("WARNING: CreateDialogParamA: Dialog %x not found!!", lpszTemplate));
        return 0;
    }

    return CreateDialogIndirectParamA(hInst, (LPCDLGTEMPLATEA)LoadResource(hInst, hrsrc),
                                      hwndOwner, dlgproc, lParamInit);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogParamW(HINSTANCE hInst, LPCWSTR lpszTemplate,
                 HWND hwndOwner, DLGPROC dlgproc,
                 LPARAM lParamInit)
{
  HANDLE hrsrc = FindResourceW( hInst, lpszTemplate, RT_DIALOGW );

    if (!hrsrc) {
        dprintf(("WARNING: CreateDialogParamW: Dialog %x not found!!", lpszTemplate));
        return 0;
    }
    return CreateDialogIndirectParamW(hInst, (LPCDLGTEMPLATEW)LoadResource(hInst, hrsrc),
                                      hwndOwner, dlgproc, lParamInit);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamA(HINSTANCE hInst,
                         LPCVOID dlgtemplate,
                         HWND hwndOwner, DLGPROC dlgproc,
                         LPARAM lParamInit)
{
 Win32Dialog *dialog;

    dprintf(("CreateDialogIndirectParamA: %x %x %x %x %x", hInst, dlgtemplate, hwndOwner, dlgproc, lParamInit));

    if (!dlgtemplate) return 0;

    dialog = new Win32Dialog(hInst, (LPCSTR)dlgtemplate, hwndOwner, dlgproc, lParamInit, FALSE);

    if(dialog == NULL)
    {
        dprintf(("Win32Dialog creation failed!!"));
        return 0;
    }
    if(GetLastError() != 0)
    {
        dprintf(("Win32Dialog error found (%0x)!!", GetLastError()));
        RELEASE_WNDOBJ(dialog);
        delete dialog;
        return 0;
    }
    HWND hwnd = dialog->getWindowHandle();
    HOOK_CallOdinHookA(HODIN_POSTDIALOGCREATION, hwnd, 0);

    RELEASE_WNDOBJ(dialog);
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateDialogIndirectParamW(HINSTANCE hInst,
                         LPCVOID dlgtemplate,
                         HWND hwndOwner, DLGPROC dlgproc,
                         LPARAM lParamInit)
{
 Win32Dialog *dialog;

    dprintf(("CreateDialogIndirectParamW: %x %x %x %x %x", hInst, dlgtemplate, hwndOwner, dlgproc, lParamInit));

    if (!dlgtemplate) return 0;

    dialog = new Win32Dialog(hInst, (LPCSTR)dlgtemplate, hwndOwner, dlgproc, lParamInit, TRUE);

    if(dialog == NULL)
    {
        dprintf(("Win32Dialog creation failed!!"));
        return 0;
    }
    if(GetLastError() != 0)
    {
        dprintf(("Win32Dialog error found!!"));
        RELEASE_WNDOBJ(dialog);
        delete dialog;
        return 0;
    }
    HWND hwnd = dialog->getWindowHandle();

    HOOK_CallOdinHookA(HODIN_POSTDIALOGCREATION, hwnd, 0);

    RELEASE_WNDOBJ(dialog);
    return hwnd;
}
//******************************************************************************
//******************************************************************************
INT  WIN32API DialogBoxIndirectParamA(HINSTANCE hInst,
                      LPCVOID dlgtemplate,
                      HWND hwndOwner, DLGPROC dlgproc,
                      LPARAM lParamInit)
{
    INT result;
    HWND hwnd = CreateDialogIndirectParamA(hInst, dlgtemplate, hwndOwner, dlgproc,
                                           lParamInit);
    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxIndirectParamA, dialog %x not found", hwnd));
            if(dialog) RELEASE_WNDOBJ(dialog);
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
        result = dialog->doDialogBox();
        RELEASE_WNDOBJ(dialog);
        return result;
    }
    return -1;
}
//******************************************************************************
//******************************************************************************
INT  WIN32API DialogBoxIndirectParamW(HINSTANCE hInst, LPCVOID dlgtemplate,
                                      HWND hwndOwner, DLGPROC dlgproc,
                                      LPARAM lParamInit)
{
    INT result;
    HWND hwnd = CreateDialogIndirectParamW(hInst, dlgtemplate, hwndOwner, dlgproc,
                                           lParamInit);
    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxIndirectParamW, dialog %x not found", hwnd));
            if(dialog) RELEASE_WNDOBJ(dialog);
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
        result = dialog->doDialogBox();
        RELEASE_WNDOBJ(dialog);
        return result;
    }
    return -1;
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamA(HINSTANCE hInst, LPCSTR lpszTemplate, HWND hwndOwner,
                     DLGPROC dlgproc, LPARAM  lParamInit)
{
    INT result;
    HWND hwnd = CreateDialogParamA( hInst, lpszTemplate, hwndOwner, dlgproc, lParamInit);

    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxParamA, dialog %x not found", hwnd));
            if(dialog) RELEASE_WNDOBJ(dialog);
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
        result = dialog->doDialogBox();
        RELEASE_WNDOBJ(dialog);
        return result;
    }
    return -1;
}
//******************************************************************************
//******************************************************************************
int WIN32API DialogBoxParamW(HINSTANCE hInst, LPCWSTR lpszTemplate, HWND hwndOwner,
                             DLGPROC dlgproc, LPARAM lParamInit)
{
    INT result;
    HWND hwnd = CreateDialogParamW( hInst, lpszTemplate, hwndOwner, dlgproc, lParamInit);

    if (hwnd)
    {
        Win32Dialog *dialog;

        dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(!dialog || !dialog->IsDialog()) {
            dprintf(("DialogBoxParamW, dialog %x not found", hwnd));
            if(dialog) RELEASE_WNDOBJ(dialog);
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
        result = dialog->doDialogBox();
        RELEASE_WNDOBJ(dialog);
        return result;
    }
    return -1;
}
/***********************************************************************
 *           MapDialogRect32   (USER32.382)
 */
BOOL WIN32API MapDialogRect(HWND hwndDlg, LPRECT rect)
{
  Win32Dialog *dialog;
  BOOL rc;
#ifdef DEBUG_LOGGING
  RECT dlgRect = *rect;
#endif

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwndDlg);
    if(!dialog || !dialog->IsDialog()) {
        dprintf(("MapDialogRect, window %x not found", hwndDlg));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        if(dialog) RELEASE_WNDOBJ(dialog);
        return 0;
    }
    rc = dialog->MapDialogRect(rect);
    dprintf(("USER32: MapDialogRect %x (%d,%d)(%d,%d) -> (%d,%d)(%d,%d)", hwndDlg, dlgRect.left, dlgRect.top, dlgRect.right, dlgRect.bottom, rect->left, rect->top, rect->right, rect->bottom));
    RELEASE_WNDOBJ(dialog);
    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API IsDlgButtonChecked( HWND hwnd, UINT id)
{
    dprintf(("USER32:  IsDlgButtonChecked\n"));

    return (BOOL)SendDlgItemMessageA(hwnd, id,BM_GETCHECK,0,0);
}
/***********************************************************************
 *           DIALOG_GetNextTabItem
 *
 * Helper for GetNextDlgTabItem
 */
static HWND DIALOG_GetNextTabItem( HWND hwndMain, HWND hwndDlg, HWND hwndCtrl, BOOL fPrevious )
{
    LONG dsStyle;
    LONG exStyle;
    UINT wndSearch = fPrevious ? GW_HWNDPREV : GW_HWNDNEXT;
    HWND retWnd = 0;
    HWND hChildFirst = 0;

    if(!hwndCtrl)
    {
        hChildFirst = GetWindow(hwndDlg,GW_CHILD);
        if(fPrevious) hChildFirst = GetWindow(hChildFirst,GW_HWNDLAST);
    }
#if 1
    else if (IsChild( hwndMain, hwndCtrl ))
    {
        hChildFirst = GetWindow(hwndCtrl,wndSearch);
        if(!hChildFirst)
        {
            if(GetParent(hwndCtrl) != hwndMain)
                hChildFirst = GetWindow(GetParent(hwndCtrl),wndSearch);
            else
            {
                if(fPrevious)
                    hChildFirst = GetWindow(hwndCtrl,GW_HWNDLAST);
                else
                    hChildFirst = GetWindow(hwndCtrl,GW_HWNDFIRST);
            }
        }
    }
#else
    else
    {
        HWND hParent = GetParent(hwndCtrl);
        BOOL bValid = FALSE;
        while( hParent)
        {
            if(hParent == hwndMain)
            {
                bValid = TRUE;
                break;
            }
            hParent = GetParent(hParent);
        }
        if(bValid)
        {
            hChildFirst = GetWindow(hwndCtrl,wndSearch);
            if(!hChildFirst)
            {
                if(GetParent(hwndCtrl) != hwndMain)
                    hChildFirst = GetWindow(GetParent(hwndCtrl),wndSearch);
                else
                {
                    if(fPrevious)
                        hChildFirst = GetWindow(hwndCtrl,GW_HWNDLAST);
                    else
                        hChildFirst = GetWindow(hwndCtrl,GW_HWNDFIRST);
                }
            }
        }	
    }
#endif
    while(hChildFirst)
    {
        BOOL bCtrl = FALSE;
        while(hChildFirst)
        {
            dsStyle = GetWindowLongA(hChildFirst,GWL_STYLE);
            exStyle = GetWindowLongA(hChildFirst,GWL_EXSTYLE);
            if( (dsStyle & DS_CONTROL || exStyle & WS_EX_CONTROLPARENT) && (dsStyle & WS_VISIBLE) && !(dsStyle & WS_DISABLED))
            {
                bCtrl=TRUE;
                break;
            }
            else if( (dsStyle & WS_TABSTOP) && (dsStyle & WS_VISIBLE) && !(dsStyle & WS_DISABLED))
                break;
            hChildFirst = GetWindow(hChildFirst,wndSearch);
        }
        if(hChildFirst)
        {
            if(bCtrl)
                retWnd = DIALOG_GetNextTabItem(hwndMain,hChildFirst,(HWND)NULL,fPrevious );
            else
                retWnd = hChildFirst;
        }
        if(retWnd) break;
        hChildFirst = GetWindow(hChildFirst,wndSearch);
    }
    if(!retWnd && hwndCtrl)
    {
        HWND hParent = GetParent(hwndCtrl);
        while(hParent)
	{
            if(hParent == hwndMain) break;
            retWnd = DIALOG_GetNextTabItem(hwndMain,GetParent(hParent),hParent,fPrevious );
            if(retWnd) break;
            hParent = GetParent(hParent);
	}
        if(!retWnd)
            retWnd = DIALOG_GetNextTabItem(hwndMain,hwndMain,(HWND)NULL,fPrevious );
    }
    return retWnd;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetNextDlgTabItem(HWND hwndDlg, HWND hwndCtrl, BOOL fPrevious)
{
    if(!IsWindow(hwndDlg)) {
        dprintf(("GetNextDlgTabItem, window %x not found", hwndDlg));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("USER32: GetNextDlgTabItem %x %x %d", hwndDlg,hwndCtrl,fPrevious));
    return DIALOG_GetNextTabItem(hwndDlg,hwndDlg,hwndCtrl,fPrevious);
}
//******************************************************************************
//Can be used for any parent-child pair
//NOTE: Returns ERROR_CONTROL_ID_NOT_FOUND when child with id not found
//      Does not change last error if successful
//******************************************************************************
HWND WIN32API GetDlgItem(HWND hwnd, int id)
{
  Win32BaseWindow *window;
  HWND             hwndDlgItem;

    window = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetDlgItem, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    hwndDlgItem = window->FindWindowById(id);
    RELEASE_WNDOBJ(window);
    if(hwndDlgItem) {
        dprintf(("USER32: GetDlgItem %x %d returned %x\n", hwnd, id, hwndDlgItem));
        return hwndDlgItem;
    }
    dprintf(("USER32: GetDlgItem %x %d NOT FOUND!\n", hwnd, id));
    SetLastError(ERROR_CONTROL_ID_NOT_FOUND);  //verified in NT4, SP6
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDlgCtrlID(HWND hwnd)
{
  Win32BaseWindow *dlgcontrol;
  int              ret;

    dlgcontrol = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dlgcontrol) {
        dprintf(("GetDlgCtrlID, control %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    ret = dlgcontrol->getWindowId();
    RELEASE_WNDOBJ(dlgcontrol);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndDialog(HWND hwnd, int retval)
{
  Win32Dialog *dialog;
  BOOL         ret;

    dialog = (Win32Dialog *)Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!dialog || !dialog->IsDialog()) {
        dprintf(("GetDlgItem, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    ret = dialog->endDialog(retval);
    RELEASE_WNDOBJ(dialog);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CheckDlgButton( HWND hwnd, int id, UINT check)
{
    dprintf(("USER32: CheckDlgButton %x %d %d", hwnd, id, check));

    return (BOOL)SendDlgItemMessageA(hwnd, id, BM_SETCHECK, check,0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CheckRadioButton( HWND hDlg, UINT nIDFirstButton, UINT nIDLastButton, UINT  nIDCheckButton)
{
    dprintf(("USER32: CheckRadioButton %x %d %d %d", hDlg, nIDFirstButton, nIDLastButton, nIDCheckButton));

    //CB: check radio buttons in interval
    if (nIDFirstButton > nIDLastButton)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return (FALSE);
    }

    for (UINT x = nIDFirstButton;x <= nIDLastButton;x++)
    {
     SendDlgItemMessageA(hDlg,x,BM_SETCHECK,(x == nIDCheckButton) ? BST_CHECKED : BST_UNCHECKED,0);
    }

    return (TRUE);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetDlgItemTextA(HWND hwnd, int id, LPSTR lpszName, UINT cch)
{
    return SendDlgItemMessageA( hwnd, id, WM_GETTEXT, cch, (LPARAM)lpszName);
}
//*****************************************************************************
//*****************************************************************************
INT WIN32API GetDlgItemTextW(HWND   hwnd, int id, LPWSTR lpszName, UINT cch)
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
UINT WIN32API GetDlgItemInt(HWND hwnd, INT id, BOOL *translated, BOOL fSigned)
{
  char str[30];
  char * endptr;
  long result = 0;

    dprintf(("USER32: GetDlgItemInt %x %x %x %d", hwnd, id, translated, fSigned));
    if (translated) *translated = FALSE;

    if (!SendDlgItemMessageA(hwnd, id, WM_GETTEXT, sizeof(str), (LPARAM)str))
        return 0;

    if (fSigned)
    {
        result = strtol( str, &endptr, 10 );
        if (!endptr || (endptr == str))  /* Conversion was unsuccessful */
            return 0;
        if (((result == LONG_MIN) || (result == LONG_MAX)) && (errno==ERANGE))
            return 0;
    }
    else
    {
        result = strtoul( str, &endptr, 10 );
        if (!endptr || (endptr == str))  /* Conversion was unsuccessful */
            return 0;
        if ((result == ULONG_MAX) && (errno == ERANGE)) return 0;
    }
    if (translated) *translated = TRUE;
    return (UINT)result;
}
//******************************************************************************
//Note: This implementation is correct, but it can't cope with z-order changes
//      of dialog controls. That's ok, since Windows can't either.
//******************************************************************************
HWND WIN32API GetNextDlgGroupItem( HWND hwndDlg, HWND hwndCtrl, BOOL fPrevious)
{
    HWND hwnd, retvalue;

    if(!IsWindow(hwndDlg) || (hwndCtrl && !IsWindow(hwndCtrl))) {
        dprintf(("GetNextDlgGroupItem, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    dprintf(("USER32: GetNextDlgGroupItem %x %x %d", hwndDlg, hwndCtrl, fPrevious));

    #define WIN_GetFullHandle(a)	a

    hwndDlg = WIN_GetFullHandle( hwndDlg );
    hwndCtrl = WIN_GetFullHandle( hwndCtrl );

    if(hwndCtrl)
    {
        /* if the hwndCtrl is the child of the control in the hwndDlg,
	 * then the hwndDlg has to be the parent of the hwndCtrl */
        if(GetParent(hwndCtrl) != hwndDlg && GetParent(GetParent(hwndCtrl)) == hwndDlg)
            hwndDlg = GetParent(hwndCtrl);
    }

    if (hwndCtrl)
    {
        /* Make sure hwndCtrl is a top-level child */
        HWND parent = GetParent( hwndCtrl );
        while (parent && parent != hwndDlg) parent = GetParent(parent);
        if (parent != hwndDlg) return 0;
    }
    else
    {
        /* No ctrl specified -> start from the beginning */
        if (!(hwndCtrl = GetWindow( hwndDlg, GW_CHILD ))) return 0;
        if (fPrevious) hwndCtrl = GetWindow( hwndCtrl, GW_HWNDLAST );
    }

    retvalue = hwndCtrl;
    hwnd = GetWindow( hwndCtrl, GW_HWNDNEXT );
    while (1)
    {
        if (!hwnd || (GetWindowLongW( hwnd, GWL_STYLE ) & WS_GROUP))
        {
            /* Wrap-around to the beginning of the group */
            HWND tmp;

            hwnd = GetWindow( hwndDlg, GW_CHILD );
#ifdef __WIN32OS2__
            if(!hwnd) break;
#endif
            for (tmp = hwnd; tmp; tmp = GetWindow( tmp, GW_HWNDNEXT ) )
            {
                if (GetWindowLongW( tmp, GWL_STYLE ) & WS_GROUP) hwnd = tmp;
                if (tmp == hwndCtrl) break;
            }
        }
        if (hwnd == hwndCtrl) break;
        if ((GetWindowLongW( hwnd, GWL_STYLE ) & (WS_VISIBLE|WS_DISABLED)) == WS_VISIBLE)
        {
            retvalue = hwnd;
	    if (!fPrevious) break;
	}
        hwnd = GetWindow( hwnd, GW_HWNDNEXT );
    }
    return retvalue;
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
/**********************************************************************
 *           DIALOG_DlgDirSelect
 *
 * Helper function for DlgDirSelect*
 */
static BOOL DIALOG_DlgDirSelect( HWND hwnd, LPSTR str, INT len,
                                 INT id, BOOL unicode, BOOL combo )
{
    char *buffer, *ptr;
    INT item, size;
    BOOL ret;
    HWND listbox = GetDlgItem( hwnd, id );

    dprintf(("%04x '%s' %d\n", hwnd, str, id ));
    if (!listbox) return FALSE;

    item = SendMessageA(listbox, combo ? CB_GETCURSEL : LB_GETCURSEL, 0, 0 );
    if (item == LB_ERR) return FALSE;
    size = SendMessageA(listbox, combo ? CB_GETLBTEXTLEN : LB_GETTEXTLEN, 0, 0 );
    if (size == LB_ERR) return FALSE;

    if (!(buffer = (char *)HeapAlloc( GetProcessHeap(), 0, size+1 ))) return FALSE;

    SendMessageA( listbox, combo ? CB_GETLBTEXT : LB_GETTEXT, item, (LPARAM)buffer );

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

    if (unicode)
    {
        if (len > 0 && !MultiByteToWideChar( CP_ACP, 0, ptr, -1, (LPWSTR)str, len ))
            ((LPWSTR)str)[len-1] = 0;
    }
    else lstrcpynA( str, ptr, len );
    HeapFree( GetProcessHeap(), 0, buffer );
    dprintf(("Returning %d '%s'\n", ret, str ));
    return ret;
}


/**********************************************************************
 *	    DIALOG_DlgDirList
 *
 * Helper function for DlgDirList*
 */
static INT DIALOG_DlgDirList( HWND hDlg, LPSTR spec, INT idLBox,
                                INT idStatic, UINT attrib, BOOL combo )
{
    HWND hwnd;
    LPSTR orig_spec = spec;

#define SENDMSG(msg,wparam,lparam) \
    ((attrib & DDL_POSTMSGS) ? PostMessageA( hwnd, msg, wparam, lparam ) \
                             : SendMessageA( hwnd, msg, wparam, lparam ))

    dprintf(("%04x '%s' %d %d %04x\n",
                    hDlg, spec ? spec : "NULL", idLBox, idStatic, attrib ));

    /* If the path exists and is a directory, chdir to it */
    if (!spec || !spec[0] || SetCurrentDirectoryA( spec )) spec = "*.*";
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
            if (!SetCurrentDirectoryA( spec ))
            {
                *p = sep;  /* Restore the original spec */
                return FALSE;
            }
            spec = p + 1;
        }
    }

    dprintf(( "mask=%s\n", spec ));

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
        char temp[MAX_PATH];
        GetCurrentDirectoryA( sizeof(temp), temp );
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
 *	    DIALOG_DlgDirListW
 *
 * Helper function for DlgDirList*W
 */
static INT DIALOG_DlgDirListW( HWND hDlg, LPWSTR spec, INT idLBox,
                                 INT idStatic, UINT attrib, BOOL combo )
{
    if (spec)
    {
        LPSTR specA = HEAP_strdupWtoA( GetProcessHeap(), 0, spec );
        INT ret = DIALOG_DlgDirList( hDlg, specA, idLBox, idStatic,
                                       attrib, combo );
        MultiByteToWideChar( CP_ACP, 0, specA, -1, spec, 0x7fffffff );
        HeapFree( GetProcessHeap(), 0, specA );
        return ret;
    }
    return DIALOG_DlgDirList( hDlg, NULL, idLBox, idStatic, attrib, combo );
}


/**********************************************************************
 *		DlgDirSelectExA (USER32.@)
 */
BOOL WINAPI DlgDirSelectExA( HWND hwnd, LPSTR str, INT len, INT id )
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id, FALSE, FALSE );
}


/**********************************************************************
 *		DlgDirSelectExW (USER32.@)
 */
BOOL WINAPI DlgDirSelectExW( HWND hwnd, LPWSTR str, INT len, INT id )
{
    return DIALOG_DlgDirSelect( hwnd, (LPSTR)str, len, id, TRUE, FALSE );
}


/**********************************************************************
 *		DlgDirSelectComboBoxExA (USER32.@)
 */
BOOL WINAPI DlgDirSelectComboBoxExA( HWND hwnd, LPSTR str, INT len,
                                         INT id )
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id, FALSE, TRUE );
}


/**********************************************************************
 *		DlgDirSelectComboBoxExW (USER32.@)
 */
BOOL WINAPI DlgDirSelectComboBoxExW( HWND hwnd, LPWSTR str, INT len,
                                         INT id)
{
    return DIALOG_DlgDirSelect( hwnd, (LPSTR)str, len, id, TRUE, TRUE );
}


/**********************************************************************
 *		DlgDirListA (USER32.@)
 */
INT WINAPI DlgDirListA( HWND hDlg, LPSTR spec, INT idLBox,
                            INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirList( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}


/**********************************************************************
 *		DlgDirListW (USER32.@)
 */
INT WINAPI DlgDirListW( HWND hDlg, LPWSTR spec, INT idLBox,
                            INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}


/**********************************************************************
 *		DlgDirListComboBoxA (USER32.@)
 */
INT WINAPI DlgDirListComboBoxA( HWND hDlg, LPSTR spec, INT idCBox,
                                    INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirList( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}


/**********************************************************************
 *		DlgDirListComboBoxW (USER32.@)
 */
INT WINAPI DlgDirListComboBoxW( HWND hDlg, LPWSTR spec, INT idCBox,
                                    INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}
