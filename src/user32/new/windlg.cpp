/* $Id: windlg.cpp,v 1.2 1999-07-18 13:57:48 cbratschi Exp $ */
/*
 * Win32 dialog apis for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <wchar.h>
#include <os2win.h>
#include <misc.h>
#include <string.h>
#include <stdlib.h>

//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDlgButtonChecked( HWND arg1, UINT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  IsDlgButtonChecked\n");
#endif
    //CB: get button state
    return (BOOL)SendDlgItemMessageA(arg1,arg2,BM_GETCHECK,0,0);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetNextDlgTabItem( HWND arg1, HWND arg2, BOOL  arg3)
{
    dprintf(("USER32:  GetNextDlgTabItem\n"));
    //get next WS_TABSTOP
    return O32_GetNextDlgTabItem(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetDlgItem(HWND arg1, int  arg2)
{
 HWND rc;
    //return OSLibWinWindowFromID(arg1,arg2);
    rc = O32_GetDlgItem(arg1, arg2);
    dprintf(("USER32:  GetDlgItem %d returned %d\n", arg2, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDlgCtrlID( HWND arg1)
{
    dprintf(("USER32:  GetDlgCtrlID\n"));
    //return OSLibWinQueryWindowUShort(arg1,QWS_ID);
    //use internal ID -> DWORD
    return O32_GetDlgCtrlID(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndDialog( HWND arg1, int  arg2)
{
 BOOL rc;

    dprintf(("USER32:  EndDialog\n"));
    //return OSLibDismissDialog(arg1,arg2);
    rc = O32_EndDialog(arg1, arg2);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CheckDlgButton( HWND arg1, int arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  CheckDlgButton\n");
#endif
    //CB: set button state
    return (BOOL)SendDlgItemMessageA(arg1,arg2,BM_SETCHECK,arg3,0);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDlgItemTextA(HWND arg1, int arg2, LPSTR arg3, UINT arg4)
{
 UINT rc;

    //WinQueryDlgItemText(arg1,arg2,arg4,arg3);
    rc = O32_GetDlgItemText(arg1, arg2, arg3, arg4);
#ifdef DEBUG
    if(rc)
        WriteLog("USER32:  GetDlgItemTextA returned %s\n", arg3);
    else    WriteLog("USER32:  GetDlgItemTextA returned 0 (%d)\n", GetLastError());
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemInt( HWND hwndDlg, int idControl, UINT uValue, BOOL  fSigned)
{
    char buf[30];

#ifdef DEBUG
    WriteLog("USER32:  SetDlgItemInt\n");
#endif
    if (fSigned) sprintf(buf,"%u",uValue);
    else sprintf(buf,"%d",uValue);

    return SetDlgItemTextA(hwndDlg,idControl,buf);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemTextA( HWND arg1, int arg2, LPCSTR  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  SetDlgItemText to %s\n", arg3);
#endif
    //WinSetDlgItemText(arg1,arg2,arg3);
    return O32_SetDlgItemText(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API DlgDirListA( HWND arg1, LPSTR arg2, int arg3, int arg4, UINT  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirListA\n");
#endif
    return O32_DlgDirList(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API DlgDirListComboBoxA( HWND arg1, LPSTR arg2, int arg3, int arg4, UINT  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirListComboBoxA\n");
#endif
    return O32_DlgDirListComboBox(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API DlgDirListComboBoxW( HWND arg1, LPWSTR arg2, int arg3, int arg4, UINT  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirListComboBoxW NOT WORKING\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_DlgDirListComboBox(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API DlgDirListW( HWND arg1, LPWSTR arg2, int arg3, int arg4, UINT  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirListW NOT WORKING\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_DlgDirList(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectComboBoxExA( HWND arg1, LPSTR arg2, int arg3, int  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirSelectComboBoxExA\n");
#endif
    return O32_DlgDirSelectComboBoxEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectComboBoxExW( HWND arg1, LPWSTR arg2, int arg3, int  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirSelectComboBoxExW NOT WORKING\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_DlgDirSelectComboBoxEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectExA( HWND arg1, LPSTR arg2, int arg3, int  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirSelectExA\n");
#endif
    return O32_DlgDirSelectEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DlgDirSelectExW( HWND arg1, LPWSTR arg2, int arg3, int  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  DlgDirSelectExW NOT WORKING\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_DlgDirSelectEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetDialogBaseUnits(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetDialogBaseUnits\n");
#endif
    return O32_GetDialogBaseUnits();
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


/*****************************************************************************
 * Name      : UINT WIN32API GetDlgItemTextW
 * Purpose   : Determine the text of a window control
 * Parameters: HWND   arg1
 *             int    arg2
 *             LPWSTR arg3
 *             UINT   arg4
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

UINT WIN32API GetDlgItemTextW(HWND   arg1,
                              int    arg2,
                              LPWSTR arg3,
                              UINT   arg4)
{
  LPSTR lpBuffer;                   /* temporary buffer for the ascii result */
  UINT  uiResult;                   /* return value of the ascii variant     */

  dprintf(("USER32: GetDlgItemTextW(%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           arg2,
           arg3,
           arg4));


  lpBuffer = (LPSTR)malloc(arg4);              /* allocate temporary buffer */
  uiResult = GetDlgItemTextA(arg1,             /* call ascii variant        */
                             arg2,
                             lpBuffer,
                             arg4);

  AsciiToUnicodeN(lpBuffer,                /* now convert result to unicode */
                  arg3,
                  arg4);

  free(lpBuffer);                              /* free the temporary buffer */

  return (uiResult);                                       /* OK, that's it */
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
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemTextW( HWND arg1, int arg2, LPCWSTR  arg3)
{
char *astring = UnicodeToAsciiString((LPWSTR)arg3);
BOOL  rc;

#ifdef DEBUG
    WriteLog("USER32:  SetDlgItemTextW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    rc = O32_SetDlgItemText(arg1, arg2, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
