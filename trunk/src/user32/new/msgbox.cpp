/* $Id: msgbox.cpp,v 1.4 2000-01-01 14:57:17 cbratschi Exp $ */
/*
 * Win32 message box function for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include "win32wnd.h"

/*****************************************************************************
 * Name      : int WIN32API MessageBoxExA
 * Purpose   : The MessageBoxEx function creates, displays, and operates a message box.
 * Parameters: HWND    hWnd        handle of owner window
 *             LPCTSTR lpText      address of text in message box
 *             LPCTSTR lpCaption   address of title of message box
 *             UINT    uType       style of message box
 *             WORD    wLanguageId language identifier
 * Variables :
 * Result    : If the function succeeds, the return value is a nonzero menu-item
 *             value returned by the dialog box.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API MessageBoxExA(HWND    hWnd,
                           LPCTSTR lpText,
                           LPCTSTR lpCaption,
                           UINT    uType,
                           WORD    wLanguageId)
{
  dprintf(("USER32:MessageBoxExA (%08xh,%s,%s,%u,%08w) not implemented.\n",
         hWnd,
         lpText,
         lpCaption,
         uType,
         wLanguageId));

  return (MessageBoxA(hWnd,
                      lpText,
                      lpCaption,
                      uType));
}


/*****************************************************************************
 * Name      : int WIN32API MessageBoxExW
 * Purpose   : The MessageBoxEx function creates, displays, and operates a message box.
 * Parameters: HWND    hWnd        handle of owner window
 *             LPCTSTR lpText      address of text in message box
 *             LPCTSTR lpCaption   address of title of message box
 *             UINT    uType       style of message box
 *             WORD    wLanguageId language identifier
 * Variables :
 * Result    : If the function succeeds, the return value is a nonzero menu-item
 *             value returned by the dialog box.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API MessageBoxExW(HWND    hWnd,
                           LPCWSTR lpText,
                           LPCWSTR lpCaption,
                           UINT    uType,
                           WORD    wLanguageId)
{

  dprintf(("USER32:MessageBoxExW (%08xh,%x,%x,%u,%08w) not implemented.\n",
         hWnd,
         lpText,
         lpCaption,
         uType,
         wLanguageId));

  return MessageBoxW(hWnd, lpText, lpCaption, uType);
}


/*****************************************************************************
 * Name      : BOOL WIN32API MessageBoxIndirectW
 * Purpose   : The MessageBoxIndirect function creates, displays, and operates
 *             a message box. The message box contains application-defined
 *             message text and title, any icon, and any combination of
 *             predefined push buttons.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

// @@@PH Win32 BOOL's are casted to INTs
INT WIN32API MessageBoxIndirectW(LPMSGBOXPARAMSW lpMsgBoxParams)
{
  dprintf(("USER32:MessageBoxIndirectW (%08x) partially implemented.\n",
         lpMsgBoxParams));

  return (MessageBoxW(lpMsgBoxParams->hwndOwner,
                      lpMsgBoxParams->lpszText,
                      lpMsgBoxParams->lpszCaption,
                      lpMsgBoxParams->dwStyle));
}


/*****************************************************************************
 * Name      : BOOL WIN32API MessageBoxIndirectA
 * Purpose   : The MessageBoxIndirect function creates, displays, and operates
 *             a message box. The message box contains application-defined
 *             message text and title, any icon, and any combination of
 *             predefined push buttons.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

// @@@PH Win32 BOOL's are casted to INTs
INT  WIN32API MessageBoxIndirectA(LPMSGBOXPARAMSA lpMsgBoxParams)
{
  dprintf(("USER32:MessageBoxIndirectA (%08x) partially implemented.\n",
         lpMsgBoxParams));

  return (MessageBoxA(lpMsgBoxParams->hwndOwner,
                      lpMsgBoxParams->lpszText,
                      lpMsgBoxParams->lpszCaption,
                      lpMsgBoxParams->dwStyle));
}
//******************************************************************************
//******************************************************************************
int WIN32API MessageBoxA(HWND hwndOwner, LPCTSTR lpszText, LPCTSTR lpszTitle, UINT fuStyle)
{
  dprintf(("USER32:  MessageBoxA %s %s\n", lpszText, lpszTitle));
  hwndOwner = Win32Window::Win32ToOS2Handle(hwndOwner);
  return(O32_MessageBox(hwndOwner, lpszText, lpszTitle, fuStyle));
}
//******************************************************************************
//******************************************************************************
int WIN32API MessageBoxW(HWND hwndOwner, LPCWSTR arg2, LPCWSTR arg3, UINT arg4)
{
 char *astring1, *astring2;
 int   rc;

    astring1 = UnicodeToAsciiString((LPWSTR)arg2);
    astring2 = UnicodeToAsciiString((LPWSTR)arg3);
#ifdef DEBUG
    WriteLog("USER32:  MessageBoxW %s %s\n", astring1, astring2);
#endif
    hwndOwner = Win32Window::Win32ToOS2Handle(hwndOwner);
    rc = O32_MessageBox(hwndOwner, astring1, astring2, arg4);
    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    return(rc);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SysErrorBox
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    : HARDERR like ?
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API SysErrorBox(DWORD x1,
                             DWORD x2,
                             DWORD x3)
{
  dprintf(("USER32: SysErrorBox(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return (FALSE); /* default */
}
