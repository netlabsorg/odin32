/* $Id: user32.cpp,v 1.23 1999-09-01 19:12:18 phaller Exp $ */

/*
 * Win32 misc user32 API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1999 Christoph Bratschi
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*****************************************************************************
 * Name      : USER32.CPP
 * Purpose   : This module maps all Win32 functions contained in USER32.DLL
 *             to their OS/2-specific counterparts as far as possible.
 *****************************************************************************/

#include <os2win.h>
#include "misc.h"

#include "user32.h"
#include "wndproc.h"
#include "wndclass.h"
#include "usrcall.h"
#include "syscolor.h"

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <winicon.h>

//undocumented stuff
// WIN32API CalcChildScroll
// WIN32API CascadeChildWindows
// WIN32API ClientThreadConnect
// WIN32API DragObject
// WIN32API DrawFrame
// WIN32API EditWndProc
// WIN32API EndTask
// WIN32API GetInputDesktop
// WIN32API GetNextQueueWindow
// WIN32API GetShellWindow
// WIN32API InitSharedTable
// WIN32API InitTask
// WIN32API IsHungThread
// WIN32API LockWindowStation
// WIN32API ModifyAccess
// WIN32API PlaySoundEvent
// WIN32API RegisterLogonProcess
// WIN32API RegisterNetworkCapabilities
// WIN32API RegisterSystemThread
// WIN32API SetDeskWallpaper
// WIN32API SetDesktopBitmap
// WIN32API SetInternalWindowPos
// WIN32API SetLogonNotifyWindow
// WIN32API SetShellWindow
// WIN32API SetSysColorsTemp
// WIN32API SetWindowFullScreenState
// WIN32API SwitchToThisWindow
// WIN32API SysErrorBox
// WIN32API TileChildWindows
// WIN32API UnlockWindowStation
// WIN32API UserClientDllInitialize
// WIN32API UserSignalProc
// WIN32API WinOldAppHackoMatic
// WIN32API WNDPROC_CALLBACK
// WIN32API YieldTask




//******************************************************************************
//******************************************************************************
HWND WIN32API GetActiveWindow()
{
  return(O32_GetActiveWindow());
}
//******************************************************************************
//******************************************************************************
int __cdecl wsprintfA(char *lpOut, LPCSTR lpFmt, ...)
{
 int     rc;
 va_list argptr;

#ifdef DEBUG
  WriteLog("USER32:  wsprintfA\n");
  WriteLog("USER32:  %s\n", lpFmt);
#endif
  va_start(argptr, lpFmt);
  rc = O32_wvsprintf(lpOut, (char *)lpFmt, argptr);
  va_end(argptr);
#ifdef DEBUG
  WriteLog("USER32:  %s\n", lpOut);
#endif
  return(rc);
}
//******************************************************************************
//******************************************************************************
int __cdecl wsprintfW(LPWSTR lpOut, LPCWSTR lpFmt, ...)
{
  int     rc;
  char   *lpFmtA;
  char    szOut[512];
  va_list argptr;

  dprintf(("USER32: wsprintfW(%08xh,%08xh).\n",
           lpOut,
           lpFmt));

  lpFmtA  = UnicodeToAsciiString((LPWSTR)lpFmt);

  /* @@@PH 98/07/13 transform "%s" to "%ls" does the unicode magic */
  {
    PSZ     pszTemp;
    PSZ     pszTemp1;
    ULONG   ulStrings;
    ULONG   ulIndex;                             /* temporary string counter */

    for (ulStrings = 0,                  /* determine number of placeholders */
         pszTemp   = lpFmtA;

         (pszTemp != NULL) &&
         (*pszTemp != 0);

         ulStrings++)
    {
      pszTemp = strstr(pszTemp,
                       "%s");
      if (pszTemp != NULL)                              /* skip 2 characters */
      {
        pszTemp++;
        pszTemp++;
      }
      else
        break;                                     /* leave loop immediately */
    }

    if (ulStrings != 0)                         /* transformation required ? */
    {
                                                     /* now reallocate lpFmt */
      ulStrings += strlen(lpFmtA);           /* calculate total string length */
      pszTemp   = lpFmtA;                              /* save string pointer */
      pszTemp1  = lpFmtA;                              /* save string pointer */

             /* @@@PH allocation has to be compatible to FreeAsciiString !!! */
      lpFmtA     = (char *)malloc(ulStrings + 1);
      if (lpFmtA == NULL)                          /* check proper allocation */
        return (0);                                 /* raise error condition */

      for (ulIndex = 0;
           ulIndex <= ulStrings;
           ulIndex++,
           pszTemp++)
      {
        if ((pszTemp[0] == '%') &&
            (pszTemp[1] == 's') )
        {
          /* replace %s by %ls */
          lpFmtA[ulIndex++] = '%';
          lpFmtA[ulIndex  ] = 'l';
          lpFmtA[ulIndex+1] = 's';
        }
        else
          lpFmtA[ulIndex] = *pszTemp;         /* just copy over the character */
      }

      lpFmtA[ulStrings] = 0;                            /* string termination */

      FreeAsciiString(pszTemp1);          /* the original string is obsolete */
    }
  }

  dprintf(("USER32:  wsprintfW (%s).\n",
           lpFmt));

  va_start(argptr,
           lpFmt);

  rc = O32_wvsprintf(szOut,
                 lpFmtA,
                 argptr);

  AsciiToUnicode(szOut,
                 lpOut);

  FreeAsciiString(lpFmtA);
  return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API MessageBoxA(HWND hwndOwner, LPCTSTR lpszText, LPCTSTR lpszTitle, UINT fuStyle)
{
  dprintf(("USER32:  MessageBoxA %s %s\n", lpszText, lpszTitle));
  return(O32_MessageBox(hwndOwner, lpszText, lpszTitle, fuStyle));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MessageBeep( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  MessageBeep\n");
#endif
    return O32_MessageBeep(arg1);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageA( HWND arg1, int arg2, UINT arg3, WPARAM arg4, LPARAM  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SendDlgItemMessageA\n");
#endif
    return O32_SendDlgItemMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API PostQuitMessage( int arg1)
{
    dprintf(("USER32:  PostQuitMessage\n"));
    O32_PostQuitMessage(arg1);
}
//******************************************************************************
// Not implemented by Open32 (31-5-99 Christoph Bratschi)
//******************************************************************************
BOOL WIN32API IsDlgButtonChecked( HWND arg1, UINT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  IsDlgButtonChecked\n");
#endif
//    return O32_IsDlgButtonChecked(arg1, arg2);
    return (BOOL)SendDlgItemMessageA(arg1,arg2,BM_GETCHECK,0,0);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextLengthA( HWND arg1)
{
    dprintf(("USER32:  GetWindowTextLength\n"));
    return O32_GetWindowTextLength(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextA( HWND arg1, LPSTR arg2, int  arg3)
{
    dprintf(("USER32:  GetWindowTextA\n"));
    return O32_GetWindowText(arg1, arg2, arg3);
}
//******************************************************************************

/*******************************************************************
 *      InternalGetWindowText    (USER32.326)
 */
int WIN32API InternalGetWindowText(HWND   hwnd,
                                   LPWSTR lpString,
                                   INT    nMaxCount )
{
    dprintf(("USER32: InternalGetWindowText(%08xh,%08xh,%08xh) not properly implemented.\n",
             hwnd,
             lpString,
             nMaxCount));

    return GetWindowTextW(hwnd,lpString,nMaxCount);
}


//******************************************************************************
BOOL WIN32API GetWindowRect( HWND arg1, PRECT  arg2)
{
 BOOL rc;

    rc = O32_GetWindowRect(arg1, arg2);
    dprintf(("USER32:  GetWindowRect %X returned %d\n", arg1, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetNextDlgTabItem( HWND arg1, HWND arg2, BOOL  arg3)
{
    dprintf(("USER32:  GetNextDlgTabItem\n"));
    return O32_GetNextDlgTabItem(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMessageA( LPMSG arg1, HWND  arg2, UINT arg3, UINT  arg4)
{
////    dprintf(("USER32:  GetMessage\n"));
    return O32_GetMessage(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetFocus(void)
{
//    dprintf(("USER32:  GetFocus\n"));
    return O32_GetFocus();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetDlgItem(HWND arg1, int  arg2)
{
 HWND rc;

    rc = O32_GetDlgItem(arg1, arg2);
    dprintf(("USER32:  GetDlgItem %d returned %d\n", arg2, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDlgCtrlID( HWND arg1)
{
    dprintf(("USER32:  GetDlgCtrlID\n"));
    return O32_GetDlgCtrlID(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetDesktopWindow(void)
{
    dprintf(("USER32:  GetDesktopWindow\n"));
    return O32_GetDesktopWindow();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumThreadWindows(DWORD dwThreadId, WNDENUMPROC lpfn, LPARAM lParam)
{
 BOOL                rc;
 EnumWindowCallback *callback = new EnumWindowCallback(lpfn, lParam);

  dprintf(("USER32:  EnumThreadWindows\n"));
  rc = O32_EnumThreadWindows(dwThreadId, callback->GetOS2Callback(), (LPARAM)callback);
  if(callback)
    delete callback;
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndDialog( HWND arg1, int  arg2)
{
 BOOL rc;

    dprintf(("USER32:  EndDialog\n"));
    rc = O32_EndDialog(arg1, arg2);
    return(rc);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageA( const MSG * arg1)
{
////    dprintf(("USER32:  DispatchMessage\n"));
    return O32_DispatchMessage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetRect( PRECT arg1, int arg2, int  arg3)
{
#ifdef DEBUG
////    WriteLog("USER32:  OffsetRect\n");
#endif
    return O32_OffsetRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CopyRect( PRECT arg1, const RECT * arg2)
{
//    ddprintf(("USER32:  CopyRect\n"));
    return O32_CopyRect(arg1, arg2);
}
//******************************************************************************
// Not implemented by Open32 (5-31-99 Christoph Bratschi)
//******************************************************************************
BOOL WIN32API CheckDlgButton( HWND arg1, int arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  CheckDlgButton\n");
#endif
//    return O32_CheckDlgButton(arg1, arg2, arg3);
    return (BOOL)SendDlgItemMessageA(arg1,arg2,BM_SETCHECK,arg3,0);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetFocus( HWND arg1)
{
    dprintf(("USER32:  SetFocus\n"));
    return O32_SetFocus(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMessage( const MSG * arg1)
{
#ifdef DEBUG
////    WriteLog("USER32:  TranslateMessage\n");
#endif
    return O32_TranslateMessage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowPos( HWND arg1, HWND arg2, int arg3, int arg4, int arg5, int arg6, UINT  arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  SetWindowPos\n");
#endif
    return O32_SetWindowPos(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowWindow(HWND arg1, int arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ShowWindow %X %d\n", arg1, arg2);
#endif
    return O32_ShowWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowTextA(HWND arg1, LPCSTR  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  SetWindowText %s\n", arg2);
#endif
    return O32_SetWindowText(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetForegroundWindow(HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetForegroundWindow\n");
#endif
    return O32_SetForegroundWindow(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API ReleaseDC( HWND arg1, HDC  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ReleaseDC\n");
#endif
    return O32_ReleaseDC(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvalidateRect(HWND arg1, const RECT *arg2, BOOL arg3)
{
#ifdef DEBUG
    if(arg2)
        WriteLog("USER32:  InvalidateRect for window %X (%d,%d)(%d,%d) %d\n", arg1, arg2->left, arg2->top, arg2->right, arg2->bottom, arg3);
    else    WriteLog("USER32:  InvalidateRect for window %X NULL, %d\n", arg1, arg3);
#endif
    return O32_InvalidateRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetUpdateRect( HWND arg1, PRECT arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetUpdateRect\n");
#endif
    return O32_GetUpdateRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API GetDC( HWND arg1)
{
 HDC hdc;

   hdc = O32_GetDC(arg1);
#ifdef DEBUG
   WriteLog("USER32:  GetDC of %X returns %X\n", arg1, hdc);
#endif
   return(hdc);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API GetDCEx(HWND arg1, HRGN arg2, DWORD arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetDCEx\n");
#endif
    return O32_GetDCEx(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClientRect( HWND hwnd, PRECT pRect)
{
 BOOL rc;

    rc = O32_GetClientRect(hwnd, pRect);
    dprintf(("USER32:  GetClientRect of %X returned (%d,%d) (%d,%d)\n", hwnd, pRect->left, pRect->top, pRect->right, pRect->bottom));
    return rc;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowA(LPCSTR arg1, LPCSTR arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  FindWindow\n");
#endif
    return O32_FindWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowExA(HWND arg1, HWND arg2, LPCSTR arg3, LPCSTR arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  FindWindowExA, not completely implemented\n");
#endif
    return O32_FindWindow(arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlashWindow( HWND arg1, BOOL  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  FlashWindow\n");
#endif
    return O32_FlashWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndPaint( HWND arg1, const PAINTSTRUCT * arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EndPaint\n");
#endif
    return O32_EndPaint(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveWindow(HWND arg1, int arg2, int arg3, int arg4, int arg5, BOOL arg6)
{
 BOOL rc;

    rc = O32_MoveWindow(arg1, arg2, arg3, arg4, arg5, arg6);
    dprintf(("USER32:  MoveWindow %X to (%d,%d) size (%d,%d), repaint = %d returned %d\n", arg1, arg2, arg3, arg4, arg5, arg6, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateWindowExA(DWORD     dwExStyle,
                                 LPCSTR    arg2,
                                 LPCSTR    arg3,
                                 DWORD     dwStyle,
                                 int       x,
                                 int       y,
                                 int       nWidth,
                                 int       nHeight,
                                 HWND      parent,
                                 HMENU     arg10,
                                 HINSTANCE arg11,
                                 PVOID     arg12)
{
 HWND hwnd;
 Win32WindowProc *window = NULL;

  /* @@@PH 98/06/12 CreateWindow crashes somewhere in Open32 */
  if(arg3 == NULL)
        arg3 = (LPCSTR)"CRASH, CRASH";

  // 6-12-99 CB: WS_CLIPCHILDREN not set -> controls not redrawn
  // Problems with group boxes
  //SvL: Not necessary anymore (EB's fixes)
//  dwStyle |= WS_CLIPCHILDREN;

  //SvL: Correct window style (like Wine does)
  if(dwStyle & WS_CHILD) {
        dwStyle |= WS_CLIPSIBLINGS;
        if(!(dwStyle & WS_POPUP)) {
                dwStyle |= WS_CAPTION;
        }
  }
  if(dwExStyle & WS_EX_DLGMODALFRAME)
  {
        dwStyle &= ~WS_THICKFRAME;
  }

#ifdef DEBUG
    WriteLog("USER32:  CreateWindow: dwExStyle = %X\n", dwExStyle);
    if((int)arg2 >> 16 != 0)
         WriteLog("USER32:  CreateWindow: classname = %s\n", arg2);
    else WriteLog("USER32:  CreateWindow: classname = %X\n", arg2);
    WriteLog("USER32:  CreateWindow: windowname= %s\n", arg3);
    WriteLog("USER32:  CreateWindow: dwStyle   = %X\n", dwStyle);
    WriteLog("USER32:  CreateWindow: x         = %d\n", x);
    WriteLog("USER32:  CreateWindow: y         = %d\n", y);
    WriteLog("USER32:  CreateWindow: nWidth    = %d\n", nWidth);
    WriteLog("USER32:  CreateWindow: nHeight   = %d\n", nHeight);
    WriteLog("USER32:  CreateWindow: parent    = %X\n", parent);
    WriteLog("USER32:  CreateWindow: hwmenu    = %X\n", arg10);
    WriteLog("USER32:  CreateWindow: hinstance = %X\n", arg11);
    WriteLog("USER32:  CreateWindow: param     = %X\n", arg12);
 #endif

  if((int) arg2 >> 16 != 0 && strcmp(arg2, "COMBOBOX") == 0)
  {
      dprintf(("COMBOBOX creation"));
    //TODO: #%@#%$ Open32 doesn't support this
      dwStyle &= ~(CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE);

      /* @@@PH 98/06/12 drop down combos are problematic too */
      /* so we translate the styles to OS/2 style */
      dwStyle |= CBS_DROPDOWN | CBS_DROPDOWNLIST;
  }

    //Classname might be name of system class, in which case we don't
    //need to use our own callback
//    if(Win32WindowClass::FindClass((LPSTR)arg2) != NULL) {
        window = new Win32WindowProc(arg11, arg2);
//    }

    hwnd = O32_CreateWindowEx(dwExStyle,
                          arg2,
                          arg3,
                          dwStyle,
                          x,
                          y,
                          nWidth,
                          nHeight,
                          parent,
                          arg10,
                          arg11,
                          arg12);

    //SvL: 16-11-'97: window can be already destroyed if hwnd == 0
    if(hwnd == 0 && window != 0 && Win32WindowProc::FindWindowProc(window)) {
        delete(window);
        window = 0;
    }
    if(window) {
        window->SetWindowHandle(hwnd);
    }

    dprintf(("USER32:  ************CreateWindowExA %s (%d,%d,%d,%d), hwnd = %X\n", arg2, x, y, nWidth, nHeight, hwnd));
    return(hwnd);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageA(HWND arg1, UINT arg2, WPARAM  arg3, LPARAM arg4)
{
 LRESULT rc;

#ifdef DEBUG1
    WriteLog("USER32:  SendMessage....\n");
#endif
    rc = O32_SendMessage(arg1, arg2, arg3, arg4);
#ifdef DEBUG1
    WriteLog("USER32:  *****SendMessage %X %X %X %X returned %d\n", arg1, arg2, arg3, arg4, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetActiveWindow( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetActiveWindow\n");
#endif
    return O32_SetActiveWindow(arg1);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API BeginPaint(HWND arg1, PPAINTSTRUCT  arg2)
{
    dprintf(("USER32: BeginPaint %X\n", arg2));
    return O32_BeginPaint(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDialogMessageA( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  IsDialogMessage\n");
#endif
    return O32_IsDialogMessage(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API DrawTextA(HDC arg1, LPCSTR arg2, int arg3, PRECT arg4, UINT arg5)
{
#ifdef DEBUG
    WriteLog("USER32: DrawTextA %s", arg2);
#endif
    return O32_DrawText(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetSystemMetrics(int arg1)
{
 int rc;

   switch(arg1) {
    case SM_CXICONSPACING: //TODO: size of grid cell for large icons
        rc = O32_GetSystemMetrics(SM_CXICON);
        break;
    case SM_CYICONSPACING:
        rc = O32_GetSystemMetrics(SM_CYICON);
        break;
    case SM_PENWINDOWS:
        rc = FALSE;
        break;
    case SM_DBCSENABLED:
        rc = FALSE;
        break;
    case SM_CXEDGE: //size of 3D window edge (not supported)
        rc = 1;
        break;
    case SM_CYEDGE:
        rc = 1;
        break;
    case SM_CXMINSPACING: //can be SM_CXMINIMIZED or larger
        rc = O32_GetSystemMetrics(SM_CXMINIMIZED);
        break;
    case SM_CYMINSPACING:
        rc = GetSystemMetrics(SM_CYMINIMIZED);
        break;
    case SM_CXSMICON: //recommended size of small icons (TODO: adjust to screen res.)
        rc = 16;
        break;
    case SM_CYSMICON:
        rc = 16;
        break;
    case SM_CYSMCAPTION:    //size in pixels of a small caption (TODO: ????)
        rc = 8;
        break;
    case SM_CXSMSIZE:   //size of small caption buttons (pixels) (TODO: implement properly)
        rc = 16;
        break;
    case SM_CYSMSIZE:
        rc = 16;
        break;
    case SM_CXMENUSIZE: //TODO: size of menu bar buttons (such as MDI window close)
        rc = 16;
        break;
    case SM_CYMENUSIZE:
        rc = 16;
        break;
    case SM_ARRANGE:
        rc = ARW_BOTTOMLEFT | ARW_LEFT;
        break;
    case SM_CXMINIMIZED:
        break;
    case SM_CYMINIMIZED:
        break;
    case SM_CXMAXTRACK: //max window size
    case SM_CXMAXIMIZED:    //max toplevel window size
        rc = O32_GetSystemMetrics(SM_CXSCREEN);
        break;
    case SM_CYMAXTRACK:
    case SM_CYMAXIMIZED:
        rc = O32_GetSystemMetrics(SM_CYSCREEN);
        break;
    case SM_NETWORK:
        rc = 0x01;  //TODO: default = yes
        break;
    case SM_CLEANBOOT:
        rc = 0;     //normal boot
        break;
    case SM_CXDRAG:     //nr of pixels before drag becomes a real one
        rc = 2;
        break;
    case SM_CYDRAG:
        rc = 2;
        break;
    case SM_SHOWSOUNDS: //show instead of play sound
        rc = FALSE;
        break;
    case SM_CXMENUCHECK:
        rc = 4;     //TODO
        break;
    case SM_CYMENUCHECK:
        rc = O32_GetSystemMetrics(SM_CYMENU);
        break;
    case SM_SLOWMACHINE:
        rc = FALSE; //even a slow machine is fast with OS/2 :)
        break;
    case SM_MIDEASTENABLED:
        rc = FALSE;
        break;
    case SM_CMETRICS:
        rc = O32_GetSystemMetrics(44);  //Open32 changed this one
        break;
    default:
        rc = O32_GetSystemMetrics(arg1);
        break;
    }
    dprintf(("USER32:  GetSystemMetrics %d returned %d\n", arg1, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetTimer( HWND arg1, UINT arg2, UINT arg3, TIMERPROC  arg4)
{
#ifdef DEBUG
    WriteLog("USER32: SetTimer INCORRECT CALLING CONVENTION FOR HANDLER!!!!!\n");
#endif
    //SvL: Write callback handler class for this one
    return O32_SetTimer(arg1, arg2, arg3, (TIMERPROC_O32)arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API KillTimer(HWND arg1, UINT arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  KillTimer\n");
#endif
    return O32_KillTimer(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyWindow(HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyWindow\n");
#endif
    return O32_DestroyWindow(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostMessageA( HWND arg1, UINT arg2, WPARAM  arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostMessageA %X %X %X %X\n", arg1, arg2, arg3, arg4);
#endif
    return O32_PostMessage(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InflateRect( PRECT arg1, int arg2, int  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  InflateRect\n");
#endif
    return O32_InflateRect(arg1, arg2, arg3);
}
//******************************************************************************
//TODO:How can we emulate this one in OS/2???
//******************************************************************************
DWORD WIN32API WaitForInputIdle(HANDLE hProcess, DWORD dwTimeOut)
{
#ifdef DEBUG
  WriteLog("USER32:  WaitForInputIdle (Not Implemented) %d\n", dwTimeOut);
#endif

  if(dwTimeOut == INFINITE) return(0);

//  DosSleep(dwTimeOut/16);
  return(0);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDlgItemTextA(HWND arg1, int arg2, LPSTR arg3, UINT arg4)
{
 UINT rc;

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
BOOL WIN32API PeekMessageA(LPMSG arg1, HWND arg2, UINT arg3, UINT arg4, UINT arg5)
{
#ifdef DEBUG
//    WriteLog("USER32:  PeekMessage\n");
#endif
    return O32_PeekMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API ShowCursor( BOOL arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  ShowCursor\n");
#endif
    return O32_ShowCursor(arg1);
}
//******************************************************************************
//BUGBUG: UpdateWindow sends a WM_ERASEBKGRND when it shouldn't!
//        So we just do it manually
//******************************************************************************
BOOL WIN32API UpdateWindow(HWND hwnd)
{
 RECT rect;

#ifdef DEBUG
  WriteLog("USER32:  UpdateWindow\n");
#endif

#if 0 // EB: ->>> doesn't work. No correct update of Winhlp32 scrolling area.
  if(O32_GetUpdateRect(hwnd, &rect, FALSE) != FALSE) {//update region empty?
    WndCallback(hwnd, WM_PAINT, 0, 0);
//  O32_PostMessage(hwnd, WM_PAINT, 0, 0);
  }
#ifdef DEBUG
  else WriteLog("USER32:  Update region empty!\n");
#endif
  return(TRUE);
#endif

  return O32_UpdateWindow(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AdjustWindowRect( PRECT arg1, DWORD arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  AdjustWindowRect\n");
#endif
    return O32_AdjustWindowRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AdjustWindowRectEx( PRECT arg1, DWORD arg2, BOOL arg3, DWORD  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  AdjustWindowRectEx\n");
#endif
    return O32_AdjustWindowRectEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ClientToScreen( HWND arg1, PPOINT  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  ClientToScreen\n");
#endif
    return O32_ClientToScreen(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRect( PRECT arg1, int arg2, int arg3, int arg4, int  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SetRect\n");
#endif
    return O32_SetRect(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemInt( HWND arg1, int arg2, UINT arg3, BOOL  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  SetDlgItemInt\n");
#endif
    return O32_SetDlgItemInt(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetDlgItemTextA( HWND arg1, int arg2, LPCSTR  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  SetDlgItemText to %s\n", arg3);
#endif
    return O32_SetDlgItemText(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WinHelpA( HWND arg1, LPCSTR arg2, UINT arg3, DWORD  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  WinHelp not implemented %s\n", arg2);
#endif
//    return O32_WinHelp(arg1, arg2, arg3, arg4);
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsIconic( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsIconic\n");
#endif
    return O32_IsIconic(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API TranslateAcceleratorA(HWND arg1, HACCEL arg2, LPMSG  arg3)
{
#ifdef DEBUG
////    WriteLog("USER32:  TranslateAccelerator\n");
#endif
    return O32_TranslateAccelerator(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetWindow(HWND arg1, UINT arg2)
{
 HWND rc;

    rc = O32_GetWindow(arg1, arg2);
#ifdef DEBUG
    WriteLog("USER32:  GetWindow %X %d returned %d\n", arg1, arg2, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API GetWindowDC(HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowDC\n");
#endif
    return O32_GetWindowDC(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SubtractRect( PRECT arg1, const RECT * arg2, const RECT * arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  SubtractRect");
#endif
    return O32_SubtractRect(arg1, arg2, arg3);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
BOOL WIN32API ClipCursor(const RECT * arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  ClipCursor\n");
#endif
    return O32_ClipCursor(arg1);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//TODO: Not implemented
//******************************************************************************
WORD WIN32API GetAsyncKeyState(INT nVirtKey)
{
#ifdef DEBUG
////    WriteLog("USER32:  GetAsyncKeyState Not implemented\n");
#endif
    return 0;
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
HCURSOR WIN32API GetCursor(void)
{
#ifdef DEBUG
////    WriteLog("USER32:  GetCursor\n");
#endif
    return O32_GetCursor();
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
BOOL WIN32API GetCursorPos( PPOINT arg1)
{
#ifdef DEBUG
////    WriteLog("USER32:  GetCursorPos\n");
#endif
    return O32_GetCursorPos(arg1);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
UINT WIN32API RegisterWindowMessageA(LPCSTR arg1)
{
 UINT rc;

    rc = O32_RegisterWindowMessage(arg1);
#ifdef DEBUG
    WriteLog("USER32:  RegisterWindowMessageA %s returned %X\n", arg1, rc);
#endif
    return(rc);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
WORD WIN32API VkKeyScanA( char arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  VkKeyScanA\n");
#endif
    return O32_VkKeyScan(arg1);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
SHORT WIN32API GetKeyState( int arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetKeyState %d\n", arg1);
#endif
    return O32_GetKeyState(arg1);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API SetCursor( HCURSOR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetCursor\n");
#endif
    return O32_SetCursor(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCursorPos( int arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  SetCursorPos\n");
#endif
    return O32_SetCursorPos(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableScrollBar( HWND arg1, INT arg2, UINT  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnableScrollBar\n");
#endif
    return O32_EnableScrollBar(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableWindow( HWND arg1, BOOL  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnableWindow\n");
#endif
    return O32_EnableWindow(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetCapture( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetCapture\n");
#endif
    return O32_SetCapture(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReleaseCapture(void)
{
#ifdef DEBUG
    WriteLog("USER32:  ReleaseCapture\n");
#endif
    return O32_ReleaseCapture();
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API MsgWaitForMultipleObjects( DWORD arg1, LPHANDLE arg2, BOOL arg3, DWORD arg4, DWORD  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  MsgWaitForMultipleObjects\n");
#endif
    return O32_MsgWaitForMultipleObjects(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HDWP WIN32API BeginDeferWindowPos( int arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  BeginDeferWindowPos\n");
#endif
    return O32_BeginDeferWindowPos(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BringWindowToTop( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  BringWindowToTop\n");
#endif
    return O32_BringWindowToTop(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CallMsgFilterA( LPMSG arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CallMsgFilterA\n");
#endif
    return O32_CallMsgFilter(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CallMsgFilterW( LPMSG arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CallMsgFilterW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_CallMsgFilter(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API CallWindowProcA(WNDPROC wndprcPrev,
                                 HWND       arg2,
                                 UINT       arg3,
                                 WPARAM     arg4,
                                 LPARAM     arg5)
{
#ifdef DEBUG
////    WriteLog("USER32:  CallWindowProcA %X hwnd=%X, msg = %X\n", wndprcPrev, arg2, arg3);
#endif

    return wndprcPrev(arg2, arg3, arg4, arg5);   //win32 callback (__stdcall)
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API CallWindowProcW(WNDPROC arg1,
                                 HWND       arg2,
                                 UINT       arg3,
                                 WPARAM     arg4,
                                 LPARAM     arg5)
{
  dprintf(("USER32:  CallWindowProcW(%08xh,%08xh,%08xh,%08xh,%08xh) not properly implemented.\n",
           arg1,
           arg2,
           arg3,
           arg4,
           arg5));

  return CallWindowProcA(arg1,
                            arg2,
                            arg3,
                            arg4,
                            arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ChangeClipboardChain( HWND arg1, HWND  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ChangeClipboardChain\n");
#endif
    return O32_ChangeClipboardChain(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API ArrangeIconicWindows( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  ArrangeIconicWindows\n");
#endif
    return O32_ArrangeIconicWindows(arg1);
}
//******************************************************************************
// Not implemented by Open32 (5-31-99 Christoph Bratschi)
//******************************************************************************
BOOL WIN32API CheckRadioButton( HWND arg1, UINT arg2, UINT arg3, UINT  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  CheckRadioButton\n");
#endif
//    return O32_CheckRadioButton(arg1, arg2, arg3, arg4);
    if (arg2 > arg3) return (FALSE);
    for (UINT x=arg2;x <= arg3;x++)
    {
     SendDlgItemMessageA(arg1,x,BM_SETCHECK,(x == arg4) ? BST_CHECKED : BST_UNCHECKED,0);
    }
    return (TRUE);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API ChildWindowFromPoint( HWND arg1, POINT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ChildWindowFromPoint\n");
#endif
    return O32_ChildWindowFromPoint(arg1, arg2);
}
/*****************************************************************************
 * Name      : HWND WIN32API ChildWindowFromPointEx
 * Purpose   : The GetWindowRect function retrieves the dimensions of the
 *             bounding rectangle of the specified window. The dimensions are
 *             given in screen coordinates that are relative to the upper-left
 *             corner of the screen.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the window handle.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : FULLY IMPLEMENTED AND TESTED
 *
 * Author    : Rene Pronk [Sun, 1999/08/08 23:30]
 *****************************************************************************/


HWND WIN32API ChildWindowFromPointEx (HWND hwndParent, POINT pt, UINT uFlags)
{
        RECT rect;
        HWND hWnd;
        POINT absolutePt;

        dprintf(("USER32: ChildWindowFromPointEx(%08xh,%08xh,%08xh).\n",
                 hwndParent, pt, uFlags));

        if (GetWindowRect (hwndParent, &rect) == 0) {
                // oops, invalid handle
                return NULL;
        }

        // absolutePt has its top in the upper-left corner of the screen
        absolutePt = pt;
        ClientToScreen (hwndParent, &absolutePt);

        // make rect the size of the parent window
        GetWindowRect (hwndParent, &rect);
        rect.right = rect.right - rect.left;
        rect.bottom = rect.bottom - rect.top;
        rect.left = 0;
        rect.top = 0;

        if (PtInRect (&rect, pt) == 0) {
                // point is outside window
                return NULL;
        }

        // get first child
        hWnd = GetWindow (hwndParent, GW_CHILD);

        while (hWnd != NULL) {

                // do I need to skip this window?
                if (((uFlags & CWP_SKIPINVISIBLE) &&
                     (IsWindowVisible (hWnd) == FALSE)) ||
                    ((uFlags & CWP_SKIPDISABLED) &&
                     (IsWindowEnabled (hWnd) == FALSE)) ||
                    ((uFlags & CWP_SKIPTRANSPARENT) &&
                     (GetWindowLongA (hWnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT)))

                {
                        hWnd = GetWindow (hWnd, GW_HWNDNEXT);
                        continue;
                }

                // is the point in this window's rect?
                GetWindowRect (hWnd, &rect);
                if (PtInRect (&rect, absolutePt) == FALSE) {
                        hWnd = GetWindow (hWnd, GW_HWNDNEXT);
                        continue;
                }

                // found it!
                return hWnd;
        }

        // the point is in the parentwindow but the parentwindow has no child
        // at this coordinate
        return hwndParent;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseClipboard(void)
{
#ifdef DEBUG
    WriteLog("USER32:  CloseClipboard\n");
#endif
    return O32_CloseClipboard();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseWindow( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  CloseWindow\n");
#endif
    return O32_CloseWindow(arg1);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CopyIcon( HICON arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  CopyIcon\n");
#endif
    return O32_CopyIcon(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API CountClipboardFormats(void)
{
#ifdef DEBUG
    WriteLog("USER32:  CountClipboardFormats\n");
#endif
    return O32_CountClipboardFormats();
}
//******************************************************************************
//******************************************************************************
HACCEL WIN32API CreateAcceleratorTableA( LPACCEL arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateAcceleratorTableA\n");
#endif
    return O32_CreateAcceleratorTable(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HACCEL WIN32API CreateAcceleratorTableW( LPACCEL arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateAcceleratorTableW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_CreateAcceleratorTable(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CreateCaret( HWND arg1, HBITMAP arg2, int arg3, int  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateCaret\n");
#endif
    return O32_CreateCaret(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API CreateCursor( HINSTANCE arg1, int arg2, int arg3, int arg4, int arg5, const VOID * arg6, const VOID * arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateCursor\n");
#endif
    return O32_CreateCursor(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIcon( HINSTANCE arg1, INT arg2, INT arg3, BYTE arg4, BYTE arg5, LPCVOID arg6, LPCVOID arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIcon\n");
#endif
    return O32_CreateIcon(arg1, arg2, arg3, arg4, arg5, (const BYTE *)arg6, (const BYTE *)arg7);
}
//******************************************************************************
//ASSERT dwVer == win31 (ok according to SDK docs)
//******************************************************************************
HICON WIN32API CreateIconFromResource(PBYTE presbits,  UINT dwResSize,
                                      BOOL  fIcon,     DWORD dwVer)
{
 HICON hicon;
 DWORD OS2ResSize = 0;
 PBYTE OS2Icon    = ConvertWin32Icon(presbits, dwResSize, &OS2ResSize);

    hicon = O32_CreateIconFromResource(OS2Icon, OS2ResSize, fIcon, dwVer);
#ifdef DEBUG
    WriteLog("USER32:  CreateIconFromResource returned %X (%X)\n", hicon, GetLastError());
#endif
    if(OS2Icon)
        FreeIcon(OS2Icon);

    return(hicon);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIconFromResourceEx(PBYTE presbits,  UINT dwResSize,
                                        BOOL  fIcon,     DWORD dwVer,
                                        int   cxDesired, int cyDesired,
                                        UINT  Flags)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIconFromResourceEx %X %d %d %X %d %d %X, not completely supported!\n", presbits, dwResSize, fIcon, dwVer, cxDesired, cyDesired, Flags);
#endif
    return CreateIconFromResource(presbits, dwResSize, fIcon, dwVer);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIconIndirect(LPICONINFO arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIconIndirect\n");
#endif
    return O32_CreateIconIndirect(arg1);
}
//******************************************************************************
HWND WIN32API CreateMDIWindowA(LPCSTR arg1, LPCSTR arg2, DWORD arg3,
                               int arg4, int arg5, int arg6, int arg7,
                               HWND arg8, HINSTANCE arg9, LPARAM  arg10)
{
 HWND hwnd;

#ifdef DEBUG
    WriteLog("USER32:  CreateMDIWindowA\n");
#endif
    Win32WindowProc *window = new Win32WindowProc(arg9, arg1);
    hwnd = O32_CreateMDIWindow((LPSTR)arg1, (LPSTR)arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    //SvL: 16-11-'97: window can be already destroyed if hwnd == 0
    if(hwnd == 0 && window != 0 && Win32WindowProc::FindWindowProc(window)) {
        delete(window);
        window = 0;
    }

#ifdef DEBUG
    WriteLog("USER32:  CreateMDIWindowA returned %X\n", hwnd);
#endif
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateMDIWindowW(LPCWSTR arg1, LPCWSTR arg2, DWORD arg3, int arg4,
                               int arg5, int arg6, int arg7, HWND arg8, HINSTANCE arg9,
                               LPARAM arg10)
{
 HWND hwnd;
 char *astring1 = NULL, *astring2 = NULL;
 Win32WindowProc *window = NULL;

    if((int)arg1 >> 16 != 0) {
        astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    }
    else    astring1 = (char *)arg2;

    astring2 = UnicodeToAsciiString((LPWSTR)arg2);

    //Classname might be name of system class, in which case we don't
    //need to use our own callback
//    if(Win32WindowClass::FindClass((LPSTR)astring1) != NULL) {
     window = new Win32WindowProc(arg9, astring1);
//    }
    hwnd = O32_CreateMDIWindow(astring1, astring2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    //SvL: 16-11-'97: window can be already destroyed if hwnd == 0
    if(hwnd == 0 && window != 0 && Win32WindowProc::FindWindowProc(window)) {
        delete(window);
        window = 0;
    }
    if(window) {
        window->SetWindowHandle(hwnd);
    }

    if(astring1)    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
#ifdef DEBUG
    WriteLog("USER32:  CreateMDIWindowW hwnd = %X\n", hwnd);
#endif
    return(hwnd);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API CreateWindowExW(DWORD     arg1,
                              LPCWSTR   arg2,
                              LPCWSTR   arg3,
                              DWORD     dwStyle,
                              int       arg5,
                              int       arg6,
                              int       arg7,
                              int       arg8,
                              HWND      arg9,
                              HMENU     arg10,
                              HINSTANCE arg11,
                              PVOID     arg12)
{
  HWND hwnd;
  char *astring1 = NULL,
       *astring2 = NULL;
  Win32WindowProc *window = NULL;

  /* @@@PH 98/06/21 changed to call OS2CreateWindowExA */
  if(HIWORD(arg2) != 0)
    astring1 = UnicodeToAsciiString((LPWSTR)arg2);
  else
    astring1 = (char *)arg2;

  astring2 = UnicodeToAsciiString((LPWSTR)arg3);

#ifdef DEBUG
    WriteLog("USER32:  CreateWindowExW: dwExStyle = %X\n", arg1);
    if((int)arg2 >> 16 != 0)
         WriteLog("USER32:  CreateWindow: classname = %s\n", astring1);
    else WriteLog("USER32:  CreateWindow: classname = %X\n", arg2);
    WriteLog("USER32:  CreateWindow: windowname= %s\n", astring2);
    WriteLog("USER32:  CreateWindow: dwStyle   = %X\n", dwStyle);
    WriteLog("USER32:  CreateWindow: x         = %d\n", arg5);
    WriteLog("USER32:  CreateWindow: y         = %d\n", arg6);
    WriteLog("USER32:  CreateWindow: nWidth    = %d\n", arg7);
    WriteLog("USER32:  CreateWindow: nHeight   = %d\n", arg8);
    WriteLog("USER32:  CreateWindow: parent    = %X\n", arg9);
    WriteLog("USER32:  CreateWindow: hwmenu    = %X\n", arg10);
    WriteLog("USER32:  CreateWindow: hinstance = %X\n", arg11);
    WriteLog("USER32:  CreateWindow: param     = %X\n", arg12);
 #endif

  hwnd = CreateWindowExA(arg1,
                            astring1,
                            astring2,
                            dwStyle,
                            arg5,
                            arg6,
                            arg7,
                            arg8,
                            arg9,
                            arg10,
                            arg11,
                            arg12);

    if(HIWORD(arg1) != 0)
        FreeAsciiString(astring1);

    FreeAsciiString(astring2);

#ifdef DEBUG
    WriteLog("USER32:  ************CreateWindowExW hwnd = %X (%X)\n", hwnd, GetLastError());
#endif
    return(hwnd);
}
//******************************************************************************
//******************************************************************************
HDWP WIN32API DeferWindowPos( HDWP arg1, HWND arg2, HWND  arg3, int arg4, int arg5, int arg6, int arg7, UINT  arg8)
{
#ifdef DEBUG
    WriteLog("USER32:  DeferWindowPos\n");
#endif
    return O32_DeferWindowPos(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyAcceleratorTable( HACCEL arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyAcceleratorTable\n");
#endif
    return O32_DestroyAcceleratorTable(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyCaret(void)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyCaret\n");
#endif
    return O32_DestroyCaret();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyCursor( HCURSOR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyCursor\n");
#endif
    return O32_DestroyCursor(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyIcon( HICON arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyIcon\n");
#endif
    return O32_DestroyIcon(arg1);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageW( const MSG * arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DispatchMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_DispatchMessage(arg1);
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
BOOL WIN32API DrawFocusRect( HDC arg1, const RECT *  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  DrawFocusRect\n");
#endif
    return O32_DrawFocusRect(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DrawIcon( HDC arg1, int arg2, int arg3, HICON  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  DrawIcon\n");
#endif
    return O32_DrawIcon(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DrawIconEx(HDC hdc, int xLeft, int xRight, HICON hIcon,
                         int cxWidth, int cyWidth, UINT istepIfAniCur,
                         HBRUSH hbrFlickerFreeDraw, UINT diFlags)
{
#ifdef DEBUG
    WriteLog("USER32:  DrawIcon, partially implemented\n");
#endif
    return O32_DrawIcon(hdc, xLeft, xRight, hIcon);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DrawMenuBar( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DrawMenuBar\n");
#endif
    return O32_DrawMenuBar(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API DrawTextW( HDC arg1, LPCWSTR arg2, int arg3, PRECT arg4, UINT  arg5)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 int   rc;

#ifdef DEBUG
    WriteLog("USER32:  DrawTextW %s\n", astring);
#endif
    rc = O32_DrawText(arg1, astring, arg3, arg4, arg5);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EmptyClipboard(void)
{
#ifdef DEBUG
    WriteLog("USER32:  EmptyClipboard\n");
#endif
    return O32_EmptyClipboard();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndDeferWindowPos( HDWP arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  EndDeferWindowPos\n");
#endif
    return O32_EndDeferWindowPos(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumChildWindows(HWND hwnd, WNDENUMPROC lpfn, LPARAM lParam)
{
 BOOL                rc;
 EnumWindowCallback *callback = new EnumWindowCallback(lpfn, lParam);

#ifdef DEBUG
  WriteLog("USER32:  EnumChildWindows\n");
#endif
  rc = O32_EnumChildWindows(hwnd, callback->GetOS2Callback(), (LPARAM)callback);
  if(callback)
    delete callback;
  return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API EnumClipboardFormats(UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumClipboardFormats\n");
#endif
    return O32_EnumClipboardFormats(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsA(HWND arg1, PROPENUMPROCA arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsA DOES NOT WORK\n");
#endif
    //calling convention problems
    return 0;
//    return O32_EnumProps(arg1, (PROPENUMPROC_O32)arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsExA( HWND arg1, PROPENUMPROCEXA arg2, LPARAM  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsExA DOES NOT WORK\n");
#endif
    //calling convention problems
    return 0;
//    return O32_EnumPropsEx(arg1, arg2, (PROPENUMPROCEX_O32)arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsExW( HWND arg1, PROPENUMPROCEXW arg2, LPARAM  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsExW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    //calling convention problems
    return 0;
//    return O32_EnumPropsEx(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsW( HWND arg1, PROPENUMPROCW  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    //calling convention problems
    return 0;
//    return O32_EnumProps(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumWindows(WNDENUMPROC lpfn, LPARAM lParam)
{
 BOOL                rc;
 EnumWindowCallback *callback = new EnumWindowCallback(lpfn, lParam);

#ifdef DEBUG
  WriteLog("USER32:  EnumWindows\n");
#endif
  rc = O32_EnumWindows(callback->GetOS2Callback(), (LPARAM)callback);
  if(callback)
        delete callback;
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EqualRect( const RECT * arg1, const RECT *  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EqualRect\n");
#endif
    return O32_EqualRect(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
INT WIN32API ExcludeUpdateRgn( HDC arg1, HWND  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ExcludeUpdateRgn\n");
#endif
    return O32_ExcludeUpdateRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExitWindowsEx( UINT arg1, DWORD  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ExitWindowsEx\n");
#endif
    return O32_ExitWindowsEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API FillRect(HDC arg1, const RECT * arg2, HBRUSH arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  FillRect (%d,%d)(%d,%d) brush %X\n", arg2->left, arg2->top, arg2->right, arg2->bottom, arg3);
#endif
    return O32_FillRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API FindWindowW( LPCWSTR arg1, LPCWSTR arg2)
{
 char *astring1 = UnicodeToAsciiString((LPWSTR)arg1);
 char *astring2 = UnicodeToAsciiString((LPWSTR)arg2);
 HWND rc;

#ifdef DEBUG
    WriteLog("USER32:  FindWindowW\n");
#endif
    rc = O32_FindWindow(astring1, astring2);
    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API FrameRect( HDC arg1, const RECT * arg2, HBRUSH  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  FrameRect\n");
#endif
    return O32_FrameRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetCapture(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetCapture\n");
#endif
    return O32_GetCapture();
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetCaretBlinkTime(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetCaretBlinkTime\n");
#endif
    return O32_GetCaretBlinkTime();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCaretPos( PPOINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetCaretPos\n");
#endif
    return O32_GetCaretPos(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetClipCursor( PRECT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipCursor\n");
#endif
    return O32_GetClipCursor(arg1);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetClipboardData( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardData\n");
#endif
    return O32_GetClipboardData(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameA( UINT arg1, LPSTR arg2, int  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardFormatNameA %s\n", arg2);
#endif
    return O32_GetClipboardFormatName(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameW(UINT arg1, LPWSTR arg2, int arg3)
{
 int   rc;
 char *astring = UnicodeToAsciiString(arg2);

#ifdef DEBUG
    WriteLog("USER32:  GetClipboardFormatNameW %s\n", astring);
#endif
    rc = O32_GetClipboardFormatName(arg1, astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardOwner(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardOwner\n");
#endif
    return O32_GetClipboardOwner();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardViewer(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardViewer\n");
#endif
    return O32_GetClipboardViewer();
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
UINT WIN32API GetDoubleClickTime(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetDoubleClickTime\n");
#endif
    return O32_GetDoubleClickTime();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetForegroundWindow(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetForegroundWindow\n");
#endif
    return O32_GetForegroundWindow();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetIconInfo( HICON arg1, LPICONINFO  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetIconInfo\n");
#endif
    return O32_GetIconInfo(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetKeyNameTextA( LPARAM arg1, LPSTR arg2, int  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetKeyNameTextA\n");
#endif
    return O32_GetKeyNameText(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetKeyNameTextW( LPARAM arg1, LPWSTR arg2, int  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetKeyNameTextW DOES NOT WORK\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_GetKeyNameText(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetKeyboardType( int arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetKeyboardType\n");
#endif
    return O32_GetKeyboardType(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetLastActivePopup( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetLastActivePopup\n");
#endif
    return O32_GetLastActivePopup(arg1);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetMessageExtraInfo(void)
{
    dprintf(("USER32:  GetMessageExtraInfo\n"));
    return O32_GetMessageExtraInfo();
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMessagePos(void)
{
    dprintf(("USER32:  GetMessagePos\n"));
    return O32_GetMessagePos();
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetMessageTime(void)
{
    dprintf(("USER32:  GetMessageTime\n"));
    return O32_GetMessageTime();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMessageW(LPMSG arg1, HWND arg2, UINT arg3, UINT arg4)
{
 BOOL rc;

    // NOTE: This will not work as is (needs UNICODE support)
    rc = O32_GetMessage(arg1, arg2, arg3, arg4);
    dprintf(("USER32:  GetMessageW %X returned %d\n", arg2, rc));
    return(rc);
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
HWND WIN32API GetOpenClipboardWindow(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetOpenClipboardWindow\n");
#endif
    return O32_GetOpenClipboardWindow();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetParent( HWND arg1)
{
#ifdef DEBUG
////    WriteLog("USER32:  GetParent\n");
#endif
    return O32_GetParent(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetPriorityClipboardFormat( PUINT arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetPriorityClipboardFormat\n");
#endif
    return O32_GetPriorityClipboardFormat(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetPropA( HWND arg1, LPCSTR  arg2)
{
#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
     WriteLog("USER32:  GetPropA %s\n", arg2);
    else WriteLog("USER32:  GetPropA %X\n", arg2);
#endif
    return O32_GetProp(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetPropW(HWND arg1, LPCWSTR arg2)
{
 BOOL  handle;
 char *astring;

    if((int)arg2 >> 16 != 0)
         astring = UnicodeToAsciiString((LPWSTR)arg2);
    else astring = (char *)arg2;
#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
         WriteLog("USER32:  GetPropW %s\n", astring);
    else WriteLog("USER32:  GetPropW %X\n", astring);
#endif
    handle = GetPropA(arg1, (LPCSTR)astring);
    if((int)arg2 >> 16 != 0)
        FreeAsciiString(astring);

    return(handle);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetQueueStatus( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetQueueStatus\n");
#endif
    return O32_GetQueueStatus(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetScrollPos(HWND hwnd, int fnBar)
{
 int pos;

    pos = O32_GetScrollPos(hwnd, fnBar);
#ifdef DEBUG
    WriteLog("USER32:  GetScrollPos of %X type %d returned %d\n", hwnd, fnBar, pos);
#endif
    return(pos);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetScrollRange( HWND arg1, int arg2, int * arg3, int *  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  GetScrollRange\n");
#endif
    return O32_GetScrollRange(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTabbedTextExtentA( HDC arg1, LPCSTR arg2, int arg3, int arg4, int * arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  GetTabbedTextExtentA\n");
#endif
    return O32_GetTabbedTextExtent(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTabbedTextExtentW( HDC arg1, LPCWSTR arg2, int arg3, int arg4, int * arg5)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 DWORD rc;

#ifdef DEBUG
    WriteLog("USER32:  GetTabbedTextExtentW\n");
#endif
    rc = O32_GetTabbedTextExtent(arg1, astring, arg3, arg4, arg5);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetTopWindow( HWND arg1)
{
#ifdef DEBUG
////    WriteLog("USER32:  GetTopWindow\n");
#endif
    return O32_GetTopWindow(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetUpdateRgn( HWND arg1, HRGN arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetUpdateRgn\n");
#endif
    return O32_GetUpdateRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowPlacement( HWND arg1, LPWINDOWPLACEMENT arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowPlacement\n");
#endif
    return O32_GetWindowPlacement(arg1, arg2);
}
//******************************************************************************

/***********************************************************************
 *           GetInternalWindowPos   (USER32.245)
 */
UINT WIN32API GetInternalWindowPos(HWND    hwnd,
                                   LPRECT  rectWnd,
                                   LPPOINT ptIcon )
{
    WINDOWPLACEMENT wndpl;

    dprintf(("USER32: GetInternalWindowPos(%08xh,%08xh,%08xh)\n",
             hwnd,
             rectWnd,
             ptIcon));

    if (O32_GetWindowPlacement( hwnd, &wndpl ))
    {
   if (rectWnd) *rectWnd = wndpl.rcNormalPosition;
   if (ptIcon)  *ptIcon = wndpl.ptMinPosition;
   return wndpl.showCmd;
    }
    return 0;
}


//******************************************************************************
int WIN32API GetWindowTextLengthW( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowTextLengthW\n");
#endif
    return O32_GetWindowTextLength(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetWindowTextW(HWND hwnd, LPWSTR lpsz, int cch)
{
 char title[128];
 int  rc;

   rc = O32_GetWindowText(hwnd, title, sizeof(title));
#ifdef DEBUG
   WriteLog("USER32:  GetWindowTextW returned %s\n", title);
#endif
   if(rc > cch) {
    title[cch-1] = 0;
    rc = cch;
   }
   AsciiToUnicode(title, lpsz);
   return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetWindowThreadProcessId(HWND arg1, PDWORD  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetWindowThreadProcessId\n");
#endif
    return O32_GetWindowThreadProcessId(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API HideCaret( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  HideCaret\n");
#endif
    return O32_HideCaret(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InSendMessage(void)
{
#ifdef DEBUG
    WriteLog("USER32:  InSendMessage\n");
#endif
    return O32_InSendMessage();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IntersectRect( PRECT arg1, const RECT *  arg2, const RECT *  arg3)
{
#ifdef DEBUG
////    WriteLog("USER32:  IntersectRect\n");
#endif
    return O32_IntersectRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvalidateRgn( HWND arg1, HRGN arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  InvalidateRgn\n");
#endif
    return O32_InvalidateRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvertRect( HDC arg1, const RECT * arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  InvertRect\n");
#endif
    return O32_InvertRect(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsChild( HWND arg1, HWND  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  IsChild\n");
#endif
    return O32_IsChild(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsClipboardFormatAvailable( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsClipboardFormatAvailable\n");
#endif
    return O32_IsClipboardFormatAvailable(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDialogMessageW( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  IsDialogMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_IsDialogMessage(arg1, arg2);
}

//******************************************************************************
//******************************************************************************
BOOL WIN32API IsRectEmpty( const RECT * arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsRectEmpty\n");
#endif
    return O32_IsRectEmpty(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindow( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsWindow\n");
#endif
    return O32_IsWindow(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowEnabled( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsWindowEnabled\n");
#endif
    return O32_IsWindowEnabled(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowVisible( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsWindowVisible\n");
#endif
    return O32_IsWindowVisible(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsZoomed( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsZoomed\n");
#endif
    return O32_IsZoomed(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LockWindowUpdate( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  LockWindowUpdate\n");
#endif
    return O32_LockWindowUpdate(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MapDialogRect( HWND arg1, PRECT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  MapDialogRect\n");
#endif
    return O32_MapDialogRect(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API MapVirtualKeyA( UINT arg1, UINT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  MapVirtualKeyA\n");
#endif
    return O32_MapVirtualKey(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API MapVirtualKeyW( UINT arg1, UINT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  MapVirtualKeyW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_MapVirtualKey(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API MapWindowPoints( HWND arg1, HWND arg2, LPPOINT arg3, UINT arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  MapWindowPoints\n");
#endif
    return O32_MapWindowPoints(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API MessageBoxW(HWND arg1, LPCWSTR arg2, LPCWSTR arg3, UINT arg4)
{
 char *astring1, *astring2;
 int   rc;

    astring1 = UnicodeToAsciiString((LPWSTR)arg2);
    astring2 = UnicodeToAsciiString((LPWSTR)arg3);
#ifdef DEBUG
    WriteLog("USER32:  MessageBoxW %s %s\n", astring1, astring2);
#endif
    rc = O32_MessageBox(arg1, astring1, astring2, arg4);
    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OpenClipboard( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OpenClipboard\n");
#endif
    return O32_OpenClipboard(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekMessageW( LPMSG arg1, HWND arg2, UINT arg3, UINT arg4, UINT  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  PeekMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_PeekMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
// NOTE: Open32 function doesn't have the 'W'.
BOOL WIN32API PostMessageW( HWND arg1, UINT arg2, WPARAM  arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_PostMessage(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostThreadMessageA( DWORD arg1, UINT arg2, WPARAM arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostThreadMessageA\n");
#endif
    return O32_PostThreadMessage(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostThreadMessageW( DWORD arg1, UINT arg2, WPARAM arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostThreadMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_PostThreadMessage(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtInRect( const RECT * arg1, POINT  arg2)
{
#ifdef DEBUG1
    WriteLog("USER32:  PtInRect\n");
#endif
    return O32_PtInRect(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RedrawWindow( HWND arg1, const RECT * arg2, HRGN arg3, UINT arg4)
{
 BOOL rc;

  rc = O32_RedrawWindow(arg1, arg2, arg3, arg4);
#ifdef DEBUG
  WriteLog("USER32:  RedrawWindow %X , %X, %X, %X returned %d\n", arg1, arg2, arg3, arg4, rc);
#endif
  InvalidateRect(arg1, arg2, TRUE);
  UpdateWindow(arg1);
  SendMessageA(arg1, WM_PAINT, 0, 0);
  return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterClipboardFormatA( LPCSTR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  RegisterClipboardFormatA\n");
#endif
    return O32_RegisterClipboardFormat(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterClipboardFormatW(LPCWSTR arg1)
{
 UINT  rc;
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);

#ifdef DEBUG
    WriteLog("USER32:  RegisterClipboardFormatW %s\n", astring);
#endif
    rc = O32_RegisterClipboardFormat(astring);
    FreeAsciiString(astring);
#ifdef DEBUG
    WriteLog("USER32:  RegisterClipboardFormatW returned %d\n", rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterWindowMessageW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 UINT  rc;

#ifdef DEBUG
    WriteLog("USER32:  RegisterWindowMessageW\n");
#endif
    rc = O32_RegisterWindowMessage(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API RemovePropA( HWND arg1, LPCSTR  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  RemovePropA\n");
#endif
    return O32_RemoveProp(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API RemovePropW( HWND arg1, LPCWSTR  arg2)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HANDLE rc;

#ifdef DEBUG
    WriteLog("USER32:  RemovePropW\n");
#endif
    rc = O32_RemoveProp(arg1, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReplyMessage( LRESULT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  ReplyMessage\n");
#endif
    return O32_ReplyMessage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScreenToClient( HWND arg1, LPPOINT  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ScreenToClient\n");
#endif
    return O32_ScreenToClient(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScrollDC( HDC arg1, int arg2, int arg3, const RECT * arg4, const RECT * arg5, HRGN arg6, PRECT  arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  ScrollDC\n");
#endif
    return O32_ScrollDC(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScrollWindow( HWND arg1, int arg2, int arg3, const RECT * arg4, const RECT *  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  ScrollWindow\n");
#endif
    return O32_ScrollWindow(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
INT WIN32API ScrollWindowEx( HWND arg1, int arg2, int arg3, const RECT * arg4, const RECT * arg5, HRGN arg6, PRECT arg7, UINT  arg8)
{
#ifdef DEBUG
    WriteLog("USER32:  ScrollWindowEx\n");
#endif
    return O32_ScrollWindowEx(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageW( HWND arg1, int arg2, UINT arg3, WPARAM arg4, LPARAM  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SendDlgItemMessageW\n");
#endif
    return O32_SendDlgItemMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageW( HWND arg1, UINT arg2, WPARAM  arg3, LPARAM  arg4)
{
LRESULT rc;

#ifdef DEBUG
    WriteLog("USER32:  SendMessageW....\n");
#endif
    rc = O32_SendMessage(arg1, arg2, arg3, arg4);
#ifdef DEBUG
    WriteLog("USER32:  SendMessageW %X %X %X %X returned %d\n", arg1, arg2, arg3, arg4, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCaretBlinkTime( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetCaretBlinkTime\n");
#endif
    return O32_SetCaretBlinkTime(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCaretPos( int arg1, int  arg2)
{
    dprintf(("USER32:  SetCaretPos\n"));
    return O32_SetCaretPos(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API SetClipboardData( UINT arg1, HANDLE  arg2)
{
    dprintf(("USER32:  SetClipboardData\n"));
    return O32_SetClipboardData(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetClipboardViewer( HWND arg1)
{
    dprintf(("USER32:  SetClipboardViewer\n"));
    return O32_SetClipboardViewer(arg1);
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
BOOL WIN32API SetDoubleClickTime( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetDoubleClickTime\n");
#endif
    return O32_SetDoubleClickTime(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetParent( HWND arg1, HWND  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  SetParent\n");
#endif
    return O32_SetParent(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPropA( HWND arg1, LPCSTR arg2, HANDLE  arg3)
{
#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
     WriteLog("USER32:  SetPropA %S\n", arg2);
    else WriteLog("USER32:  SetPropA %X\n", arg2);
#endif
    return O32_SetProp(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPropW(HWND arg1, LPCWSTR arg2, HANDLE arg3)
{
 BOOL  rc;
 char *astring;

    if((int)arg2 >> 16 != 0)
         astring = UnicodeToAsciiString((LPWSTR)arg2);
    else astring = (char *)arg2;

#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
         WriteLog("USER32:  SetPropW %S\n", astring);
    else WriteLog("USER32:  SetPropW %X\n", astring);
#endif
    rc = O32_SetProp(arg1, astring, arg3);
    if((int)astring >> 16 != 0)
        FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRectEmpty( PRECT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetRectEmpty\n");
#endif
    return O32_SetRectEmpty(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetScrollPos( HWND arg1, int arg2, int arg3, BOOL  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  SetScrollPos\n");
#endif
    return O32_SetScrollPos(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetScrollRange( HWND arg1, int arg2, int arg3, int arg4, BOOL  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SetScrollRange\n");
#endif
    return O32_SetScrollRange(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowPlacement( HWND arg1, const WINDOWPLACEMENT *  arg2)
{
    dprintf(("USER32:  SetWindowPlacement\n"));
    return O32_SetWindowPlacement(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowTextW( HWND arg1, LPCWSTR arg2)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 BOOL  rc;

   rc = SetWindowTextA(arg1, (LPCSTR)astring);
   dprintf(("USER32:  SetWindowTextW %X %s returned %d\n", arg1, astring, rc));
   FreeAsciiString(astring);
   return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowCaret( HWND arg1)
{
    dprintf(("USER32:  ShowCaret\n"));
    return O32_ShowCaret(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowOwnedPopups( HWND arg1, BOOL  arg2)
{
    dprintf(("USER32:  ShowOwnedPopups\n"));
    return O32_ShowOwnedPopups(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowScrollBar( HWND arg1, int arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  ShowScrollBar\n");
#endif
    return O32_ShowScrollBar(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SwapMouseButton( BOOL arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SwapMouseButton\n");
#endif
    return O32_SwapMouseButton(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SystemParametersInfoA(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
 BOOL rc;
 NONCLIENTMETRICSA *cmetric = (NONCLIENTMETRICSA *)pvParam;

  switch(uiAction) {
    case SPI_SCREENSAVERRUNNING:
        *(BOOL *)pvParam = FALSE;
        rc = TRUE;
        break;
    case SPI_GETDRAGFULLWINDOWS:
        *(BOOL *)pvParam = FALSE;
        rc = TRUE;
        break;
    case SPI_GETNONCLIENTMETRICS:
        memset(cmetric, 0, sizeof(NONCLIENTMETRICSA));
        cmetric->cbSize = sizeof(NONCLIENTMETRICSA);
        //CB: font info not valid, needs improvements
        O32_SystemParametersInfo(SPI_GETICONTITLELOGFONT, 0, (LPVOID)&(cmetric->lfCaptionFont),0);
        O32_SystemParametersInfo(SPI_GETICONTITLELOGFONT, 0, (LPVOID)&(cmetric->lfMenuFont),0);
        //CB: experimental change for statusbar (and tooltips)

        //O32_SystemParametersInfo(SPI_GETICONTITLELOGFONT, 0, (LPVOID)&(cmetric->lfStatusFont),0);
        lstrcpyA(cmetric->lfStatusFont.lfFaceName,"WarpSans");
        cmetric->lfStatusFont.lfHeight = 9;

        O32_SystemParametersInfo(SPI_GETICONTITLELOGFONT, 0, (LPVOID)&(cmetric->lfMessageFont),0);
        cmetric->iBorderWidth     = GetSystemMetrics(SM_CXBORDER);
        cmetric->iScrollWidth     = GetSystemMetrics(SM_CXHSCROLL);
        cmetric->iScrollHeight    = GetSystemMetrics(SM_CYHSCROLL);
        cmetric->iCaptionWidth    = 32; //TODO
        cmetric->iCaptionHeight   = 16; //TODO
        cmetric->iSmCaptionWidth  = GetSystemMetrics(SM_CXSMSIZE);
        cmetric->iSmCaptionHeight = GetSystemMetrics(SM_CYSMSIZE);
        cmetric->iMenuWidth       = 32; //TODO
        cmetric->iMenuHeight      = GetSystemMetrics(SM_CYMENU);
        rc = TRUE;
        break;
    case 104: //TODO: Undocumented
        rc = 16;
        break;
    default:
        rc = O32_SystemParametersInfo(uiAction, uiParam, pvParam, fWinIni);
        break;
  }
#ifdef DEBUG
  WriteLog("USER32:  SystemParametersInfoA %d, returned %d\n", uiAction, rc);
#endif
  return(rc);
}
//******************************************************************************
//TODO: Check for more options that have different structs for Unicode!!!!
//******************************************************************************
BOOL WIN32API SystemParametersInfoW(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
 BOOL rc;
 NONCLIENTMETRICSW *clientMetricsW = (NONCLIENTMETRICSW *)pvParam;
 NONCLIENTMETRICSA  clientMetricsA = {0};
 PVOID  pvParamA;
 UINT   uiParamA;

    switch(uiAction) {
    case SPI_SETNONCLIENTMETRICS:
        clientMetricsA.cbSize = sizeof(NONCLIENTMETRICSA);
        clientMetricsA.iBorderWidth = clientMetricsW->iBorderWidth;
        clientMetricsA.iScrollWidth = clientMetricsW->iScrollWidth;
        clientMetricsA.iScrollHeight = clientMetricsW->iScrollHeight;
        clientMetricsA.iCaptionWidth = clientMetricsW->iCaptionWidth;
        clientMetricsA.iCaptionHeight = clientMetricsW->iCaptionHeight;
        ConvertFontWA(&clientMetricsW->lfCaptionFont, &clientMetricsA.lfCaptionFont);
        clientMetricsA.iSmCaptionWidth = clientMetricsW->iSmCaptionWidth;
        clientMetricsA.iSmCaptionHeight = clientMetricsW->iSmCaptionHeight;
        ConvertFontWA(&clientMetricsW->lfSmCaptionFont, &clientMetricsA.lfSmCaptionFont);
        clientMetricsA.iMenuWidth = clientMetricsW->iMenuWidth;
        clientMetricsA.iMenuHeight = clientMetricsW->iMenuHeight;
        ConvertFontWA(&clientMetricsW->lfMenuFont, &clientMetricsA.lfMenuFont);
        ConvertFontWA(&clientMetricsW->lfStatusFont, &clientMetricsA.lfStatusFont);
        ConvertFontWA(&clientMetricsW->lfMessageFont, &clientMetricsA.lfMessageFont);
        //no break
    case SPI_GETNONCLIENTMETRICS:
        uiParamA = sizeof(NONCLIENTMETRICSA);
        pvParamA = &clientMetricsA;
        break;
    default:
        pvParamA = pvParam;
        uiParamA = uiParam;
        break;
    }
    rc = SystemParametersInfoA(uiAction, uiParamA, pvParamA, fWinIni);

    switch(uiAction) {
    case SPI_GETNONCLIENTMETRICS:
        clientMetricsW->cbSize = sizeof(*clientMetricsW);
        clientMetricsW->iBorderWidth = clientMetricsA.iBorderWidth;
        clientMetricsW->iScrollWidth = clientMetricsA.iScrollWidth;
        clientMetricsW->iScrollHeight = clientMetricsA.iScrollHeight;
        clientMetricsW->iCaptionWidth = clientMetricsA.iCaptionWidth;
        clientMetricsW->iCaptionHeight = clientMetricsA.iCaptionHeight;
        ConvertFontAW(&clientMetricsA.lfCaptionFont, &clientMetricsW->lfCaptionFont);

        clientMetricsW->iSmCaptionWidth = clientMetricsA.iSmCaptionWidth;
        clientMetricsW->iSmCaptionHeight = clientMetricsA.iSmCaptionHeight;
        ConvertFontAW(&clientMetricsA.lfSmCaptionFont, &clientMetricsW->lfSmCaptionFont);

        clientMetricsW->iMenuWidth = clientMetricsA.iMenuWidth;
        clientMetricsW->iMenuHeight = clientMetricsA.iMenuHeight;
        ConvertFontAW(&clientMetricsA.lfMenuFont, &clientMetricsW->lfMenuFont);
        ConvertFontAW(&clientMetricsA.lfStatusFont, &clientMetricsW->lfStatusFont);
        ConvertFontAW(&clientMetricsA.lfMessageFont, &clientMetricsW->lfMessageFont);
        break;
    }
#ifdef DEBUG
    WriteLog("USER32:  SystemParametersInfoW %d, returned %d\n", uiAction, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API TabbedTextOutA( HDC arg1, int arg2, int arg3, LPCSTR arg4, int arg5, int arg6, int * arg7, int  arg8)
{
#ifdef DEBUG
    WriteLog("USER32:  TabbedTextOutA\n");
#endif
    return O32_TabbedTextOut(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API TabbedTextOutW( HDC arg1, int arg2, int arg3, LPCWSTR arg4, int arg5, int arg6, int * arg7, int  arg8)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg4);
 LONG rc;

#ifdef DEBUG
    WriteLog("USER32:  TabbedTextOutW\n");
#endif
    rc = O32_TabbedTextOut(arg1, arg2, arg3, astring, arg5, arg6, arg7, arg8);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API TranslateAccelerator( HWND arg1, HACCEL arg2, LPMSG  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  TranslateAccelerator\n");
#endif
    return O32_TranslateAccelerator(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API TranslateAcceleratorW( HWND arg1, HACCEL arg2, LPMSG  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  TranslateAcceleratorW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_TranslateAccelerator(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMDISysAccel( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  TranslateMDISysAccel\n");
#endif
    return O32_TranslateMDISysAccel(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnionRect( PRECT arg1, const RECT * arg2, const RECT *  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  UnionRect\n");
#endif
    return O32_UnionRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ValidateRect( HWND arg1, const RECT * arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ValidateRect\n");
#endif
    return O32_ValidateRect(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ValidateRgn( HWND arg1, HRGN  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ValidateRgn\n");
#endif
    return O32_ValidateRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API VkKeyScanW( WCHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  VkKeyScanW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_VkKeyScan((char)arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WaitMessage(void)
{
#ifdef DEBUG
    WriteLog("USER32:  WaitMessage\n");
#endif
    return O32_WaitMessage();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WinHelpW( HWND arg1, LPCWSTR arg2, UINT arg3, DWORD  arg4)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 BOOL  rc;

#ifdef DEBUG
    WriteLog("USER32:  WinHelpW\n");
#endif
    rc = WinHelpA(arg1, astring, arg3, arg4);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API WindowFromDC( HDC arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  WindowFromDC\n");
#endif
    return O32_WindowFromDC(arg1);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API WindowFromPoint( POINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  WindowFromPoint\n");
#endif
    return O32_WindowFromPoint(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API wvsprintfA( LPSTR arg1, LPCSTR arg2, va_list arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  wvsprintfA\n");
#endif
    return O32_wvsprintf(arg1, arg2, (LPCVOID *)arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API wvsprintfW(LPWSTR lpOut, LPCWSTR lpFmt, va_list argptr)
{
 int     rc;
 char    szOut[256];
 char   *lpFmtA;

  lpFmtA  = UnicodeToAsciiString((LPWSTR)lpFmt);
#ifdef DEBUG
  WriteLog("USER32:  wvsprintfW, DOES NOT HANDLE UNICODE STRINGS!\n");
  WriteLog("USER32:  %s\n", lpFmt);
#endif
  rc = O32_wvsprintf(szOut, lpFmtA, (LPCVOID)argptr);

  AsciiToUnicode(szOut, lpOut);
#ifdef DEBUG
  WriteLog("USER32:  %s\n", lpOut);
#endif
  FreeAsciiString(lpFmtA);
  return(rc);
}
//******************************************************************************
//No need to support this
//******************************************************************************
BOOL WIN32API SetMessageQueue(int cMessagesMax)
{
#ifdef DEBUG
  WriteLog("USER32:  SetMessageQueue\n");
#endif
  return(TRUE);
}
//******************************************************************************
//TODO: Not complete
//******************************************************************************
BOOL WIN32API GetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi)
{
#ifdef DEBUG
  WriteLog("USER32:  GetScrollInfo\n");
#endif
  if(lpsi == NULL)
    return(FALSE);

  if(lpsi->fMask & SIF_POS)
    lpsi->nPos = GetScrollPos(hwnd, fnBar);
  if(lpsi->fMask & SIF_RANGE)
    GetScrollRange(hwnd, fnBar, &lpsi->nMin, &lpsi->nMax);
  if(lpsi->fMask & SIF_PAGE) {
#ifdef DEBUG
    WriteLog("USER32:  GetScrollInfo, page info not implemented\n");
#endif
    lpsi->nPage     = 25;
  }
  return(TRUE);
}
//******************************************************************************
//TODO: Not complete
//******************************************************************************
INT WIN32API SetScrollInfo(HWND hwnd, INT fnBar, const SCROLLINFO *lpsi, BOOL fRedraw)
{
 int smin, smax;

#ifdef DEBUG
  WriteLog("USER32:  SetScrollInfo\n");
#endif
  if(lpsi == NULL)
    return(FALSE);

  if(lpsi->fMask & SIF_POS)
        SetScrollPos(hwnd, fnBar, lpsi->nPos, fRedraw);
  if(lpsi->fMask & SIF_RANGE)
        SetScrollRange(hwnd, fnBar, lpsi->nMin, lpsi->nMax, fRedraw);
  if(lpsi->fMask & SIF_PAGE) {
#ifdef DEBUG
        WriteLog("USER32:  GetScrollInfo, page info not implemented\n");
#endif
  }
  if(lpsi->fMask & SIF_DISABLENOSCROLL) {
#ifdef DEBUG
        WriteLog("USER32:  GetScrollInfo, disable scrollbar not yet implemented\n");
#endif
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GrayStringA(HDC hdc, HBRUSH hBrush, GRAYSTRINGPROC lpOutputFunc,
                          LPARAM lpData, int nCount, int X, int Y, int nWidth,
                          int nHeight)
{
 BOOL     rc;
 COLORREF curclr;

#ifdef DEBUG
  WriteLog("USER32:  GrayStringA, not completely implemented\n");
#endif
  if(lpOutputFunc == NULL && lpData == NULL) {
#ifdef DEBUG
    WriteLog("USER32:  lpOutputFunc == NULL && lpData == NULL\n");
#endif
    return(FALSE);
  }
  if(lpOutputFunc) {
        return(lpOutputFunc(hdc, lpData, nCount));
  }
  curclr = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
  rc = TextOutA(hdc, X, Y, (char *)lpData, nCount);
  SetTextColor(hdc, curclr);

  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GrayStringW(HDC hdc, HBRUSH hBrush, GRAYSTRINGPROC lpOutputFunc,
                          LPARAM lpData, int nCount, int X, int Y, int nWidth,
                          int nHeight)
{
 BOOL     rc;
 char    *astring;
 COLORREF curclr;

#ifdef DEBUG
  WriteLog("USER32:  GrayStringW, not completely implemented\n");
#endif

  if(lpOutputFunc == NULL && lpData == NULL) {
#ifdef DEBUG
    WriteLog("USER32:  lpOutputFunc == NULL && lpData == NULL\n");
#endif
    return(FALSE);
  }
  if(nCount == 0)
    nCount = UniStrlen((UniChar*)lpData);

  if(lpOutputFunc) {
    return(lpOutputFunc(hdc, lpData, nCount));
  }
  astring = UnicodeToAsciiString((LPWSTR)lpData);

  curclr = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
  rc = TextOutA(hdc, X, Y, astring, nCount);
  SetTextColor(hdc, curclr);

  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//TODO:
//******************************************************************************
int WIN32API CopyAcceleratorTableA(HACCEL hAccelSrc, LPACCEL lpAccelDest,
                      int cAccelEntries)
{
#ifdef DEBUG
  WriteLog("USER32:  CopyAcceleratorTableA, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//TODO:
//******************************************************************************
int WIN32API CopyAcceleratorTableW(HACCEL hAccelSrc, LPACCEL lpAccelDest,
                      int cAccelEntries)
{
#ifdef DEBUG
  WriteLog("USER32:  CopyAcceleratorTableW, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageTimeoutA(HWND hwnd, UINT Msg, WPARAM wParam,
                    LPARAM lParam, UINT fuFlags, UINT uTimeOut,
                    LPDWORD lpdwResult)
{
#ifdef DEBUG
  WriteLog("USER32:  SendMessageTimeoutA, partially implemented\n");
#endif
  //ignore fuFlags & wTimeOut
  *lpdwResult = SendMessageA(hwnd, Msg, wParam, lParam);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageTimeoutW(HWND hwnd, UINT Msg, WPARAM wParam,
                    LPARAM lParam, UINT fuFlags, UINT uTimeOut,
                    LPDWORD lpdwResult)
{
#ifdef DEBUG
  WriteLog("USER32:  SendMessageTimeoutW, partially implemented\n");
#endif
  return(SendMessageTimeoutA(hwnd, Msg, wParam, lParam, fuFlags, uTimeOut, lpdwResult));
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API CopyImage(HANDLE hImage, UINT uType, int cxDesired, int cyDesired, UINT fuFlags)
{
#ifdef DEBUG
  WriteLog("USER32:  CopyImage, not implemented\n");
#endif
  switch(uType) {
    case IMAGE_BITMAP:
    case IMAGE_CURSOR:
    case IMAGE_ICON:
    default:
#ifdef DEBUG
        WriteLog("USER32:  CopyImage, unknown type\n");
#endif
        return(NULL);
  }
  return(NULL);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetKeyboardState(PBYTE lpKeyState)
{
#ifdef DEBUG
  WriteLog("USER32:  GetKeyboardState, not properly implemented\n");
#endif
  memset(lpKeyState, 0, 256);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetKeyboardState(PBYTE lpKeyState)
{
#ifdef DEBUG
  WriteLog("USER32:  SetKeyboardState, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
  WriteLog("USER32:  SendNotifyMessageA, not completely implemented\n");
#endif
  return(SendMessageA(hwnd, Msg, wParam, lParam));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
  WriteLog("USER32:  SendNotifyMessageW, not completely implemented\n");
#endif
  return(SendMessageA(hwnd, Msg, wParam, lParam));
}
//******************************************************************************
//2nd parameter not used according to SDK (yet?)
//******************************************************************************
VOID WIN32API SetLastErrorEx(DWORD dwErrCode, DWORD dwType)
{
#ifdef DEBUG
  WriteLog("USER32:  SetLastErrorEx\n");
#endif
  SetLastError(dwErrCode);
}
//******************************************************************************
//******************************************************************************
LPARAM WIN32API SetMessageExtraInfo(LPARAM lParam)
{
#ifdef DEBUG
  WriteLog("USER32:  SetMessageExtraInfo, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ActivateKeyboardLayout(HKL hkl, UINT fuFlags)
{
#ifdef DEBUG
  WriteLog("USER32:  ActivateKeyboardLayout, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetKeyboardLayoutList(int nBuff, HKL *lpList)
{
#ifdef DEBUG
  WriteLog("USER32:  GetKeyboardLayoutList, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
HKL WIN32API GetKeyboardLayout(DWORD dwLayout)
{
#ifdef DEBUG
  WriteLog("USER32:  GetKeyboardLayout, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
int WIN32API LookupIconIdFromDirectory(PBYTE presbits, BOOL fIcon)
{
#ifdef DEBUG
  WriteLog("USER32:  LookupIconIdFromDirectory, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
int WIN32API LookupIconIdFromDirectoryEx(PBYTE presbits, BOOL  fIcon,
                        int cxDesired, int cyDesired,
                        UINT Flags)
{
#ifdef DEBUG
  WriteLog("USER32:  LookupIconIdFromDirectoryEx, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//DWORD idAttach;   /* thread to attach */
//DWORD idAttachTo; /* thread to attach to  */
//BOOL fAttach; /* attach or detach */
//******************************************************************************
BOOL WIN32API AttachThreadInput(DWORD idAttach, DWORD idAttachTo, BOOL fAttach)
{
#ifdef DEBUG
  WriteLog("USER32:  AttachThreadInput, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RegisterHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT uVirtKey)
{
#ifdef DEBUG
  WriteLog("USER32:  RegisterHotKey, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterHotKey(HWND hwnd, int idHotKey)
{
#ifdef DEBUG
  WriteLog("USER32:  UnregisterHotKey, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowContextHelpId(HWND hwnd, DWORD dwContextHelpId)
{
#ifdef DEBUG
  WriteLog("USER32:  SetWindowContextHelpId, not implemented\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetWindowContextHelpId(HWND hwnd)
{
#ifdef DEBUG
  WriteLog("USER32:  GetWindowContextHelpId, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//restores iconized window to previous size/position
//******************************************************************************
BOOL WIN32API OpenIcon(HWND hwnd)
{
#ifdef DEBUG
  WriteLog("USER32:  OpenIcon\n");
#endif
  if(!IsIconic(hwnd))
        return FALSE;
  ShowWindow(hwnd, SW_SHOWNORMAL);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsWindowUnicode(HWND hwnd)
{
#ifdef DEBUG
  WriteLog("USER32:  IsWindowUnicode, not implemented\n");
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMonitorInfoA(HMONITOR,LPMONITORINFO)
{
#ifdef DEBUG
  WriteLog("USER32:  GetMonitorInfoA not supported!!\n");
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMonitorInfoW(HMONITOR,LPMONITORINFO)
{
#ifdef DEBUG
  WriteLog("USER32:  GetMonitorInfoW not supported!!\n");
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromWindow(HWND hwnd, DWORD dwFlags)
{
#ifdef DEBUG
  WriteLog("USER32:  MonitorFromWindow not correctly supported??\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromRect(LPRECT rect, DWORD dwFlags)
{
#ifdef DEBUG
  WriteLog("USER32:  MonitorFromRect not correctly supported??\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
HMONITOR WIN32API MonitorFromPoint(POINT point, DWORD dwflags)
{
#ifdef DEBUG
  WriteLog("USER32:  MonitorFromPoint not correctly supported??\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumDisplayMonitors(HDC,LPRECT,MONITORENUMPROC,LPARAM)
{
#ifdef DEBUG
  WriteLog("USER32:  EnumDisplayMonitors not supported??\n");
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumDisplaySettingsA(LPCSTR lpszDeviceName, DWORD iModeNum,
                      LPDEVMODEA lpDevMode)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumDisplaySettingsA FAKED\n");
#endif
    switch(iModeNum) {
    case 0:
        lpDevMode->dmBitsPerPel       = 16;
        lpDevMode->dmPelsWidth        = 768;
        lpDevMode->dmPelsHeight       = 1024;
        lpDevMode->dmDisplayFlags     = 0;
        lpDevMode->dmDisplayFrequency = 70;
        break;
    case 1:
        lpDevMode->dmBitsPerPel       = 16;
        lpDevMode->dmPelsWidth        = 640;
        lpDevMode->dmPelsHeight       = 480;
        lpDevMode->dmDisplayFlags     = 0;
        lpDevMode->dmDisplayFrequency = 70;
        break;
    default:
        return(FALSE);
    }
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API ChangeDisplaySettingsA(LPDEVMODEA  lpDevMode, DWORD dwFlags)
{
#ifdef DEBUG
    if(lpDevMode) {
        WriteLog("USER32:  ChangeDisplaySettingsA FAKED %X\n", dwFlags);
        WriteLog("USER32:  ChangeDisplaySettingsA lpDevMode->dmBitsPerPel %d\n", lpDevMode->dmBitsPerPel);
        WriteLog("USER32:  ChangeDisplaySettingsA lpDevMode->dmPelsWidth  %d\n", lpDevMode->dmPelsWidth);
        WriteLog("USER32:  ChangeDisplaySettingsA lpDevMode->dmPelsHeight %d\n", lpDevMode->dmPelsHeight);
    }
#endif
    return(DISP_CHANGE_SUCCESSFUL);
}
//******************************************************************************
//******************************************************************************


/*****************************************************************************
 * Name      : BOOL WIN32API AnyPopup
 * Purpose   : The AnyPopup function indicates whether an owned, visible,
 *             top-level pop-up, or overlapped window exists on the screen. The
 *             function searches the entire Windows screen, not just the calling
 *             application's client area.
 * Parameters: VOID
 * Variables :
 * Result    : If a pop-up window exists, the return value is TRUE even if the
 *             pop-up window is completely covered by other windows. Otherwise,
 *             it is FALSE.
 * Remark    : AnyPopup is a Windows version 1.x function and is retained for
 *             compatibility purposes. It is generally not useful.
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API AnyPopup(VOID)
{
  dprintf(("USER32:AnyPopup() not implemented.\n"));

  return (FALSE);
}


/*****************************************************************************
 * Name      : long WIN32API BroadcastSystemMessage
 * Purpose   : The BroadcastSystemMessage function sends a message to the given
 *             recipients. The recipients can be applications, installable
 *             drivers, Windows-based network drivers, system-level device
 *             drivers, or any combination of these system components.
 * Parameters: DWORD   dwFlags,
               LPDWORD lpdwRecipients,
               UINT    uiMessage,
               WPARAM  wParam,
               LPARAM  lParam
 * Variables :
 * Result    : If the function succeeds, the return value is a positive value.
 *             If the function is unable to broadcast the message, the return value is -1.
 *             If the dwFlags parameter is BSF_QUERY and at least one recipient returned FALSE to the corresponding message, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

long WIN32API BroadcastSystemMessage(DWORD   dwFlags,
                                        LPDWORD lpdwRecipients,
                                        UINT    uiMessage,
                                        WPARAM  wParam,
                                        LPARAM  lParam)
{
  dprintf(("USER32:BroadcastSystemMessage(%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
        dwFlags,
        lpdwRecipients,
        uiMessage,
        wParam,
        lParam));

  return (-1);
}


/*****************************************************************************
 * Name      : WORD WIN32API CascadeWindows
 * Purpose   : The CascadeWindows function cascades the specified windows or
 *             the child windows of the specified parent window.
 * Parameters: HWND hwndParent         handle of parent window
 *             UINT wHow               types of windows not to arrange
 *             CONST RECT * lpRect     rectangle to arrange windows in
 *             UINT cKids              number of windows to arrange
 *             const HWND FAR * lpKids array of window handles
 * Variables :
 * Result    : If the function succeeds, the return value is the number of windows arranged.
 *             If the function fails, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

WORD WIN32API CascadeWindows(HWND       hwndParent,
                             UINT       wHow,
                             CONST LPRECT lpRect,
                             UINT       cKids,
                             const HWND *lpKids)
{
  dprintf(("USER32:CascadeWindows(%08xh,%u,%08xh,%u,%08x) not implemented.\n",
         hwndParent,
         wHow,
         lpRect,
         cKids,
         lpKids));

  return (0);
}


/*****************************************************************************
 * Name      : LONG WIN32API ChangeDisplaySettingsW
 * Purpose   : The ChangeDisplaySettings function changes the display settings
 *             to the specified graphics mode.
 * Parameters: LPDEVMODEW lpDevModeW
 *             DWORD      dwFlags
 * Variables :
 * Result    : DISP_CHANGE_SUCCESSFUL The settings change was successful.
 *             DISP_CHANGE_RESTART    The computer must be restarted in order for the graphics mode to work.
 *             DISP_CHANGE_BADFLAGS   An invalid set of flags was passed in.
 *             DISP_CHANGE_FAILED     The display driver failed the specified graphics mode.
 *             DISP_CHANGE_BADMODE    The graphics mode is not supported.
 *             DISP_CHANGE_NOTUPDATED Unable to write settings to the registry.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

LONG WIN32API ChangeDisplaySettingsW(LPDEVMODEW lpDevMode,
                                        DWORD      dwFlags)
{
  dprintf(("USER32:ChangeDisplaySettingsW(%08xh,%08x) not implemented.\n",
         lpDevMode,
         dwFlags));

  return (ChangeDisplaySettingsA((LPDEVMODEA)lpDevMode,
                                  dwFlags));
}

/*****************************************************************************
 * Name      : BOOL WIN32API CloseDesktop
 * Purpose   : The CloseDesktop function closes an open handle of a desktop
 *             object. A desktop is a secure object contained within a window
 *             station object. A desktop has a logical display surface and
 *             contains windows, menus and hooks.
 * Parameters: HDESK hDesktop
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the functions fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API CloseDesktop(HDESK hDesktop)
{
  dprintf(("USER32:CloseDesktop(%08x) not implemented.\n",
         hDesktop));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API CloseWindowStation
 * Purpose   : The CloseWindowStation function closes an open window station handle.
 * Parameters: HWINSTA hWinSta
 * Variables :
 * Result    :
 * Remark    : If the function succeeds, the return value is TRUE.
 *             If the functions fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API CloseWindowStation(HWINSTA hWinSta)
{
  dprintf(("USER32:CloseWindowStation(%08x) not implemented.\n",
         hWinSta));

  return (FALSE);
}


/*****************************************************************************
 * Name      : HDESK WIN32API CreateDesktopA
 * Purpose   : The CreateDesktop function creates a new desktop on the window
 *             station associated with the calling process.
 * Parameters: LPCTSTR   lpszDesktop      name of the new desktop
 *             LPCTSTR   lpszDevice       name of display device to assign to the desktop
 *             LPDEVMODE pDevMode         reserved; must be NULL
 *             DWORD     dwFlags          flags to control interaction with other applications
 *             DWORD     dwDesiredAccess  specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the desktop
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               newly created desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HDESK WIN32API CreateDesktopA(LPCTSTR               lpszDesktop,
                              LPCTSTR               lpszDevice,
                              LPDEVMODEA            pDevMode,
                              DWORD                 dwFlags,
                              DWORD                 dwDesiredAccess,
                              LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateDesktopA(%s,%s,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktop,
         lpszDevice,
         pDevMode,
         dwFlags,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}


/*****************************************************************************
 * Name      : HDESK WIN32API CreateDesktopW
 * Purpose   : The CreateDesktop function creates a new desktop on the window
 *             station associated with the calling process.
 * Parameters: LPCTSTR   lpszDesktop      name of the new desktop
 *             LPCTSTR   lpszDevice       name of display device to assign to the desktop
 *             LPDEVMODE pDevMode         reserved; must be NULL
 *             DWORD     dwFlags          flags to control interaction with other applications
 *             DWORD     dwDesiredAccess  specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the desktop
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               newly created desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HDESK WIN32API CreateDesktopW(LPCTSTR               lpszDesktop,
                              LPCTSTR               lpszDevice,
                              LPDEVMODEW            pDevMode,
                              DWORD                 dwFlags,
                              DWORD                 dwDesiredAccess,
                              LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateDesktopW(%s,%s,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktop,
         lpszDevice,
         pDevMode,
         dwFlags,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}


/*****************************************************************************
 * Name      : HWINSTA WIN32API CreateWindowStationA
 * Purpose   : The CreateWindowStation function creates a window station object.
 *             It returns a handle that can be used to access the window station.
 *             A window station is a secure object that contains a set of global
 *             atoms, a clipboard, and a set of desktop objects.
 * Parameters: LPTSTR lpwinsta            name of the new window station
 *             DWORD dwReserved           reserved; must be NULL
 *             DWORD dwDesiredAccess      specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the window station
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               newly created window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HWINSTA WIN32API CreateWindowStationA(LPTSTR lpWinSta,
                                         DWORD  dwReserved,
                                         DWORD  dwDesiredAccess,
                                         LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateWindowStationA(%s,%08xh,%08xh,%08x) not implemented.\n",
         lpWinSta,
         dwReserved,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}


/*****************************************************************************
 * Name      : HWINSTA WIN32API CreateWindowStationW
 * Purpose   : The CreateWindowStation function creates a window station object.
 *             It returns a handle that can be used to access the window station.
 *             A window station is a secure object that contains a set of global
 *             atoms, a clipboard, and a set of desktop objects.
 * Parameters: LPTSTR lpwinsta            name of the new window station
 *             DWORD dwReserved           reserved; must be NULL
 *             DWORD dwDesiredAccess      specifies access of returned handle
 *             LPSECURITY_ATTRIBUTES lpsa specifies security attributes of the window station
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               newly created window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HWINSTA WIN32API CreateWindowStationW(LPWSTR lpWinSta,
                                         DWORD  dwReserved,
                                         DWORD  dwDesiredAccess,
                                         LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("USER32:CreateWindowStationW(%s,%08xh,%08xh,%08x) not implemented.\n",
         lpWinSta,
         dwReserved,
         dwDesiredAccess,
         lpsa));

  return (NULL);
}

/*****************************************************************************
 * Name      : BOOL WIN32API DragDetect
 * Purpose   : The DragDetect function captures the mouse and tracks its movement
 * Parameters: HWND  hwnd
 *             POINT pt
 * Variables :
 * Result    : If the user moved the mouse outside of the drag rectangle while
 *               holding the left button down, the return value is TRUE.
 *             If the user did not move the mouse outside of the drag rectangle
 *               while holding the left button down, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API DragDetect(HWND  hwnd,
                            POINT pt)
{
  dprintf(("USER32:DragDetect(%08xh,...) not implemented.\n",
         hwnd));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API DrawAnimatedRects
 * Purpose   : The DrawAnimatedRects function draws a wire-frame rectangle
 *             and animates it to indicate the opening of an icon or the
 *             minimizing or maximizing of a window.
 * Parameters: HWND hwnd             handle of clipping window
 *             int idAni             type of animation
 *             CONST RECT * lprcFrom address of rectangle coordinates (minimized)
 *             CONST RECT * lprcTo   address of rectangle coordinates (restored)
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API DrawAnimatedRects(HWND hwnd,
                                   int  idAni,
                                   CONST RECT *lprcFrom,
                                   CONST RECT *lprcTo)
{
  dprintf(("USER32:DrawAnimatedRects (%08xh,%u,%08xh,%08x) not implemented.\n",
         hwnd,
         idAni,
         lprcFrom,
         lprcTo));

  return (TRUE);
}


/*****************************************************************************
 * Name      : VOID WIN32API DrawCaption
 * Purpose   : The DrawCaption function draws a window caption.
 * Parameters: HDC hdc        handle of device context
 *             LPRECT lprc    address of bounding rectangle coordinates
 *             HFONT hfont    handle of font for caption
 *             HICON hicon    handle of icon in caption
 *             LPSTR lpszText address of caption string
 *             WORD wFlags    drawing options
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API DrawCaption (HWND hwnd,
                           HDC  hdc,
                           const RECT *lprc,
                           UINT wFlags)
{
  dprintf(("USER32:DrawCaption (%08xh,%08xh,%08xh,%08xh) not implemented.\n",
         hwnd,
         hdc,
         lprc,
         wFlags));

  return FALSE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

/*****************************************************************************
 * Name      : BOOL WIN32API EnumDesktopWindows
 * Purpose   : The EnumDesktopWindows function enumerates all windows in a
 *             desktop by passing the handle of each window, in turn, to an
 *             application-defined callback function.
 * Parameters: HDESK       hDesktop handle of desktop to enumerate
 *             WNDENUMPROC lpfn     points to application's callback function
 *             LPARAM      lParam   32-bit value to pass to the callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API EnumDesktopWindows(HDESK       hDesktop,
                                    WNDENUMPROC lpfn,
                                    LPARAM      lParam)
{
  dprintf(("USER32:EnumDesktopWindows (%08xh,%08xh,%08x) not implemented.\n",
         hDesktop,
         lpfn,
         lParam));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API EnumDesktopsA
 * Purpose   : The EnumDesktops function enumerates all desktops in the window
 *             station assigned to the calling process. The function does so by
 *             passing the name of each desktop, in turn, to an application-
 *             defined callback function.
 * Parameters: HWINSTA         hwinsta    handle of window station to enumerate
 *             DESKTOPENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM          lParam     32-bit value to pass to the callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API EnumDesktopsA(HWINSTA          hWinSta,
                            DESKTOPENUMPROCA lpEnumFunc,
                            LPARAM           lParam)
{
  dprintf(("USER32:EnumDesktopsA (%08xh,%08xh,%08x) not implemented.\n",
         hWinSta,
         lpEnumFunc,
         lParam));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API EnumDesktopsW
 * Purpose   : The EnumDesktops function enumerates all desktops in the window
 *             station assigned to the calling process. The function does so by
 *             passing the name of each desktop, in turn, to an application-
 *             defined callback function.
 * Parameters: HWINSTA         hwinsta    handle of window station to enumerate
 *             DESKTOPENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM          lParam     32-bit value to pass to the callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API EnumDesktopsW(HWINSTA          hWinSta,
                            DESKTOPENUMPROCW lpEnumFunc,
                            LPARAM           lParam)
{
  dprintf(("USER32:EnumDesktopsW (%08xh,%08xh,%08x) not implemented.\n",
         hWinSta,
         lpEnumFunc,
         lParam));

  return (FALSE);
}



/*****************************************************************************
 * Name      : BOOL WIN32API EnumDisplaySettingsW
 * Purpose   : The EnumDisplaySettings function obtains information about one
 *             of a display device's graphics modes. You can obtain information
 *             for all of a display device's graphics modes by making a series
 *             of calls to this function.
 * Parameters: LPCTSTR   lpszDeviceName specifies the display device
 *             DWORD     iModeNum       specifies the graphics mode
 *             LPDEVMODE lpDevMode      points to structure to receive settings
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API EnumDisplaySettingsW(LPCSTR     lpszDeviceName,
                                      DWORD      iModeNum,
                                      LPDEVMODEW lpDevMode)
{
  dprintf(("USER32:EnumDisplaySettingsW (%s,%08xh,%08x) not implemented.\n",
         lpszDeviceName,
         iModeNum,
         lpDevMode));

  return (EnumDisplaySettingsA(lpszDeviceName,
                               iModeNum,
                               (LPDEVMODEA)lpDevMode));
}


/*****************************************************************************
 * Name      : BOOL WIN32API EnumWindowStationsA
 * Purpose   : The EnumWindowStations function enumerates all windowstations
 *             in the system by passing the name of each window station, in
 *             turn, to an application-defined callback function.
 * Parameters:
 * Variables : WINSTAENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM         lParam     32-bit value to pass to the callback function
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API EnumWindowStationsA(WINSTAENUMPROCA lpEnumFunc,
                                  LPARAM          lParam)
{
  dprintf(("USER32:EnumWindowStationsA (%08xh,%08x) not implemented.\n",
         lpEnumFunc,
         lParam));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API EnumWindowStationsW
 * Purpose   : The EnumWindowStations function enumerates all windowstations
 *             in the system by passing the name of each window station, in
 *             turn, to an application-defined callback function.
 * Parameters:
 * Variables : WINSTAENUMPROC lpEnumFunc points to application's callback function
 *             LPARAM         lParam     32-bit value to pass to the callback function
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API EnumWindowStationsW(WINSTAENUMPROCW lpEnumFunc,
                                  LPARAM          lParam)
{
  dprintf(("USER32:EnumWindowStationsW (%08xh,%08x) not implemented.\n",
         lpEnumFunc,
         lParam));

  return (FALSE);
}


/*****************************************************************************
 * Name      : HWND WIN32API FindWindowExW
 * Purpose   : The FindWindowEx function retrieves the handle of a window whose
 *             class name and window name match the specified strings. The
 *             function searches child windows, beginning with the one following
 *             the given child window.
 * Parameters: HWND    hwndParent     handle of parent window
 *             HWND    hwndChildAfter handle of a child window
 *             LPCTSTR lpszClass      address of class name
 *             LPCTSTR lpszWindow     address of window name
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               window that has the specified class and window names.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HWND WIN32API FindWindowExW(HWND    hwndParent,
                               HWND    hwndChildAfter,
                               LPCWSTR lpszClass,
                               LPCWSTR lpszWindow)
{
  dprintf(("USER32:FindWindowExW (%08xh,%08xh,%s,%s) not implemented.\n",
         hwndParent,
         hwndChildAfter,
         lpszClass,
         lpszWindow));

  return (NULL);
}

/*****************************************************************************
 * Name      : BOOL WIN32API GetInputState
 * Purpose   : The GetInputState function determines whether there are
 *             mouse-button or keyboard messages in the calling thread's message queue.
 * Parameters:
 * Variables :
 * Result    : If the queue contains one or more new mouse-button or keyboard
 *               messages, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API GetInputState(VOID)
{
  dprintf(("USER32:GetInputState () not implemented.\n"));

  return (FALSE);
}


/*****************************************************************************
 * Name      : UINT WIN32API GetKBCodePage
 * Purpose   : The GetKBCodePage function is provided for compatibility with
 *             earlier versions of Windows. In the Win32 application programming
 *             interface (API) it just calls the GetOEMCP function.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is an OEM code-page
 *             identifier, or it is the default identifier if the registry
 *             value is not readable. For a list of OEM code-page identifiers,
 *             see GetOEMCP.
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

UINT WIN32API GetKBCodePage(VOID)
{
  return (GetOEMCP());
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetKeyboardLayoutNameA
 * Purpose   : The GetKeyboardLayoutName function retrieves the name of the
 *             active keyboard layout.
 * Parameters: LPTSTR pwszKLID address of buffer for layout name
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

//@@@PH Win32 BOOLs are casted to INTs.
INT WIN32API GetKeyboardLayoutNameA(LPTSTR pwszKLID)
{
  dprintf(("USER32:GetKeyboardLayoutNameA (%08x) not implemented.",
         pwszKLID));

  return(FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetKeyboardLayoutNameW
 * Purpose   : The GetKeyboardLayoutName function retrieves the name of the
 *             active keyboard layout.
 * Parameters: LPTSTR pwszKLID address of buffer for layout name
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

//@@@PH Win32 BOOLs are casted to INTs.
INT  WIN32API GetKeyboardLayoutNameW(LPWSTR pwszKLID)
{
  dprintf(("USER32:GetKeyboardLayoutNameW (%08x) not implemented.",
         pwszKLID));

  return(FALSE);
}




/*****************************************************************************
 * Name      : HWINSTA WIN32API GetProcessWindowStation
 * Purpose   : The GetProcessWindowStation function returns a handle of the
 *             window station associated with the calling process.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               window station associated with the calling process.
 *             If the function fails, the return value is NULL. This can occur
 *               if the calling process is not an application written for Windows
 *               NT. To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HWINSTA WIN32API GetProcessWindowStation(VOID)
{
  dprintf(("USER32:GetProcessWindowStation () not implemented.\n"));

  return (NULL);
}



/*****************************************************************************
 * Name      : HDESK WIN32API GetThreadDesktop
 * Purpose   : The GetThreadDesktop function returns a handle to the desktop
 *             associated with a specified thread.
 * Parameters: DWORD dwThreadId thread identifier
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               desktop associated with the specified thread.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HDESK WIN32API GetThreadDesktop(DWORD dwThreadId)
{
  dprintf(("USER32:GetThreadDesktop (%u) not implemented.\n",
         dwThreadId));

  return (NULL);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetUserObjectInformationA
 * Purpose   : The GetUserObjectInformation function returns information about
 *               a window station or desktop object.
 * Parameters: HANDLE  hObj            handle of object to get information for
 *             int     nIndex          type of information to get
 *             PVOID   pvInfo          points to buffer that receives the information
 *             DWORD   nLength         size, in bytes, of pvInfo buffer
 *             LPDWORD lpnLengthNeeded receives required size, in bytes, of pvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API GetUserObjectInformationA(HANDLE  hObj,
                                           int     nIndex,
                                           PVOID   pvInfo,
                                           DWORD   nLength,
                                           LPDWORD lpnLengthNeeded)
{
  dprintf(("USER32:GetUserObjectInformationA (%08xh,%08xh,%08xh,%u,%08x) not implemented.\n",
         hObj,
         nIndex,
         pvInfo,
         nLength,
         lpnLengthNeeded));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetUserObjectInformationW
 * Purpose   : The GetUserObjectInformation function returns information about
 *               a window station or desktop object.
 * Parameters: HANDLE  hObj            handle of object to get information for
 *             int     nIndex          type of information to get
 *             PVOID   pvInfo          points to buffer that receives the information
 *             DWORD   nLength         size, in bytes, of pvInfo buffer
 *             LPDWORD lpnLengthNeeded receives required size, in bytes, of pvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API GetUserObjectInformationW(HANDLE  hObj,
                                           int     nIndex,
                                           PVOID   pvInfo,
                                           DWORD   nLength,
                                           LPDWORD lpnLengthNeeded)
{
  dprintf(("USER32:GetUserObjectInformationW (%08xh,%08xh,%08xh,%u,%08x) not implemented.\n",
         hObj,
         nIndex,
         pvInfo,
         nLength,
         lpnLengthNeeded));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetUserObjectSecurity
 * Purpose   : The GetUserObjectSecurity function retrieves security information
 *             for the specified user object.
 * Parameters: HANDLE                hObj            handle of user object
 *             SECURITY_INFORMATION * pSIRequested    address of requested security information
 *             LPSECURITY_DESCRIPTOR  pSID            address of security descriptor
 *             DWORD                 nLength         size of buffer for security descriptor
 *             LPDWORD               lpnLengthNeeded address of required size of buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API GetUserObjectSecurity(HANDLE                hObj,
                                       SECURITY_INFORMATION * pSIRequested,
                                       LPSECURITY_DESCRIPTOR  pSID,
                                       DWORD                 nLength,
                                       LPDWORD               lpnLengthNeeded)
{
  dprintf(("USER32:GetUserObjectSecurity (%08xh,%08xh,%08xh,%u,%08x) not implemented.\n",
         hObj,
         pSIRequested,
         pSID,
         nLength,
         lpnLengthNeeded));

  return (FALSE);
}



/*****************************************************************************
 * Name      : int WIN32API GetWindowRgn
 * Purpose   : The GetWindowRgn function obtains a copy of the window region of a window.
 * Parameters: HWND hWnd handle to window whose window region is to be obtained
 *             HRGN hRgn handle to region that receives a copy of the window region
 * Variables :
 * Result    : NULLREGION, SIMPLEREGION, COMPLEXREGION, ERROR
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API GetWindowRgn (HWND hWnd,
                              HRGN hRgn)
{
  dprintf(("USER32:GetWindowRgn (%08xh,%08x) not implemented.\n",
         hWnd,
         hRgn));

  return (NULLREGION);
}



/*****************************************************************************
 * Name      : HCURSOR WIN32API LoadCursorFromFileA
 * Purpose   : The LoadCursorFromFile function creates a cursor based on data
 *             contained in a file. The file is specified by its name or by a
 *             system cursor identifier. The function returns a handle to the
 *             newly created cursor. Files containing cursor data may be in
 *             either cursor (.CUR) or animated cursor (.ANI) format.
 * Parameters: LPCTSTR  lpFileName pointer to cursor file, or system cursor id
 * Variables :
 * Result    : If the function is successful, the return value is a handle to
 *               the new cursor.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError. GetLastError may return
 *               the following
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HCURSOR WIN32API LoadCursorFromFileA(LPCTSTR lpFileName)
{
  dprintf(("USER32:LoadCursorFromFileA (%s) not implemented.\n",
         lpFileName));

  return (NULL);
}


/*****************************************************************************
 * Name      : HCURSOR WIN32API LoadCursorFromFileW
 * Purpose   : The LoadCursorFromFile function creates a cursor based on data
 *             contained in a file. The file is specified by its name or by a
 *             system cursor identifier. The function returns a handle to the
 *             newly created cursor. Files containing cursor data may be in
 *             either cursor (.CUR) or animated cursor (.ANI) format.
 * Parameters: LPCTSTR  lpFileName pointer to cursor file, or system cursor id
 * Variables :
 * Result    : If the function is successful, the return value is a handle to
 *               the new cursor.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError. GetLastError may return
 *               the following
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HCURSOR WIN32API LoadCursorFromFileW(LPCWSTR lpFileName)
{
  dprintf(("USER32:LoadCursorFromFileW (%s) not implemented.\n",
         lpFileName));

  return (NULL);
}


/*****************************************************************************
 * Name      : HLK WIN32API LoadKeyboardLayoutA
 * Purpose   : The LoadKeyboardLayout function loads a new keyboard layout into
 *             the system. Several keyboard layouts can be loaded at a time, but
 *             only one per process is active at a time. Loading multiple keyboard
 *             layouts makes it possible to rapidly switch between layouts.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               keyboard layout.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HKL WIN32API LoadKeyboardLayoutA(LPCTSTR pwszKLID,
                                 UINT    Flags)
{
  dprintf(("USER32:LeadKeyboardLayoutA (%s,%u) not implemented.\n",
         pwszKLID,
         Flags));

  return (NULL);
}


/*****************************************************************************
 * Name      : HLK WIN32API LoadKeyboardLayoutW
 * Purpose   : The LoadKeyboardLayout function loads a new keyboard layout into
 *             the system. Several keyboard layouts can be loaded at a time, but
 *             only one per process is active at a time. Loading multiple keyboard
 *             layouts makes it possible to rapidly switch between layouts.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               keyboard layout.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HKL WIN32API LoadKeyboardLayoutW(LPCWSTR pwszKLID,
                                 UINT    Flags)
{
  dprintf(("USER32:LeadKeyboardLayoutW (%s,%u) not implemented.\n",
         pwszKLID,
         Flags));

  return (NULL);
}


/*****************************************************************************
 * Name      : UINT WIN32API MapVirtualKeyExA
 * Purpose   : The MapVirtualKeyEx function translates (maps) a virtual-key
 *             code into a scan code or character value, or translates a scan
 *             code into a virtual-key code. The function translates the codes
 *             using the input language and physical keyboard layout identified
 *             by the given keyboard layout handle.
 * Parameters:
 * Variables :
 * Result    : The return value is either a scan code, a virtual-key code, or
 *             a character value, depending on the value of uCode and uMapType.
 *             If there is no translation, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

UINT WIN32API MapVirtualKeyExA(UINT uCode,
                                  UINT uMapType,
                                  HKL  dwhkl)
{
  dprintf(("USER32:MapVirtualKeyExA (%u,%u,%08x) not implemented.\n",
         uCode,
         uMapType,
         dwhkl));

  return (0);
}


/*****************************************************************************
 * Name      : UINT WIN32API MapVirtualKeyExW
 * Purpose   : The MapVirtualKeyEx function translates (maps) a virtual-key
 *             code into a scan code or character value, or translates a scan
 *             code into a virtual-key code. The function translates the codes
 *             using the input language and physical keyboard layout identified
 *             by the given keyboard layout handle.
 * Parameters:
 * Variables :
 * Result    : The return value is either a scan code, a virtual-key code, or
 *             a character value, depending on the value of uCode and uMapType.
 *             If there is no translation, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB

 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

UINT WIN32API MapVirtualKeyExW(UINT uCode,
                                  UINT uMapType,
                                  HKL  dwhkl)
{
  dprintf(("USER32:MapVirtualKeyExW (%u,%u,%08x) not implemented.\n",
         uCode,
         uMapType,
         dwhkl));

  return (0);
}


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

//@@@PH Win32 BOOLs are casted to INTs.
INT  WIN32API MessageBoxIndirectW(LPMSGBOXPARAMSW lpMsgBoxParams)
{
  dprintf(("USER32:MessageBoxIndirectW (%08x) not implemented.\n",
         lpMsgBoxParams));

  return (FALSE);
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

//@@@PH Win32 BOOLs are casted to INTs.
INT  WIN32API MessageBoxIndirectA(LPMSGBOXPARAMSA lpMsgBoxParams)
{
  dprintf(("USER32:MessageBoxIndirectA (%08x) not implemented.\n",
         lpMsgBoxParams));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD WIN32API OemKeyScan
 * Purpose   : The OemKeyScan function maps OEM ASCII codes 0 through 0x0FF
 *             into the OEM scan codes and shift states. The function provides
 *             information that allows a program to send OEM text to another
 *             program by simulating keyboard input.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

DWORD WIN32API OemKeyScan(WORD wOemChar)
{
  dprintf(("USER32:OemKeyScan (%u) not implemented.\n",
         wOemChar));

  return (wOemChar);
}


/*****************************************************************************
 * Name      : HDESK WIN32API OpenDesktopA
 * Purpose   : The OpenDesktop function returns a handle to an existing desktop.
 *             A desktop is a secure object contained within a window station
 *             object. A desktop has a logical display surface and contains
 *             windows, menus and hooks.
 * Parameters: LPCTSTR lpszDesktopName name of the desktop to open
 *             DWORD dwFlags           flags to control interaction with other applications
 *             BOOL fInherit           specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess   specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               opened desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HDESK WIN32API OpenDesktopA(LPCTSTR lpszDesktopName,
                               DWORD   dwFlags,
                               BOOL    fInherit,
                               DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenDesktopA (%s,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktopName,
         dwFlags,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}


/*****************************************************************************
 * Name      : HDESK WIN32API OpenDesktopW
 * Purpose   : The OpenDesktop function returns a handle to an existing desktop.
 *             A desktop is a secure object contained within a window station
 *             object. A desktop has a logical display surface and contains
 *             windows, menus and hooks.
 * Parameters: LPCTSTR lpszDesktopName name of the desktop to open
 *             DWORD dwFlags           flags to control interaction with other applications
 *             BOOL fInherit           specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess   specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               opened desktop.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HDESK WIN32API OpenDesktopW(LPCTSTR lpszDesktopName,
                               DWORD   dwFlags,
                               BOOL    fInherit,
                               DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenDesktopW (%s,%08xh,%08xh,%08x) not implemented.\n",
         lpszDesktopName,
         dwFlags,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}


/*****************************************************************************
 * Name      : HDESK WIN32API OpenInputDesktop
 * Purpose   : The OpenInputDesktop function returns a handle to the desktop
 *             that receives user input. The input desktop is a desktop on the
 *             window station associated with the logged-on user.
 * Parameters: DWORD dwFlags         flags to control interaction with other applications
 *             BOOL  fInherit        specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is a handle of the
 *               desktop that receives user input.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HDESK WIN32API OpenInputDesktop(DWORD dwFlags,
                                   BOOL  fInherit,
                                   DWORD dwDesiredAccess)
{
  dprintf(("USER32:OpenInputDesktop (%08xh,%08xh,%08x) not implemented.\n",
         dwFlags,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}


/*****************************************************************************
 * Name      : HWINSTA WIN32API OpenWindowStationA
 * Purpose   : The OpenWindowStation function returns a handle to an existing
 *               window station.
 * Parameters: LPCTSTR lpszWinStaName name of the window station to open
 *             BOOL fInherit          specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess  specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               specified window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HWINSTA WIN32API OpenWindowStationA(LPCTSTR lpszWinStaName,
                                       BOOL    fInherit,
                                       DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenWindowStatieonA (%s,%08xh,%08x) not implemented.\n",
         lpszWinStaName,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}


/*****************************************************************************
 * Name      : HWINSTA WIN32API OpenWindowStationW
 * Purpose   : The OpenWindowStation function returns a handle to an existing
 *               window station.
 * Parameters: LPCTSTR lpszWinStaName name of the window station to open
 *             BOOL fInherit          specifies whether returned handle is inheritable
 *             DWORD dwDesiredAccess  specifies access of returned handle
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *               specified window station.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.


 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HWINSTA WIN32API OpenWindowStationW(LPCTSTR lpszWinStaName,
                                       BOOL    fInherit,
                                       DWORD   dwDesiredAccess)
{
  dprintf(("USER32:OpenWindowStatieonW (%s,%08xh,%08x) not implemented.\n",
         lpszWinStaName,
         fInherit,
         dwDesiredAccess));

  return (NULL);
}


/*****************************************************************************
 * Name      : BOOL WIN32API PaintDesktop
 * Purpose   : The PaintDesktop function fills the clipping region in the
 *             specified device context with the desktop pattern or wallpaper.
 *             The function is provided primarily for shell desktops.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API PaintDesktop(HDC hdc)
{
  dprintf(("USER32:PaintDesktop (%08x) not implemented.\n",
         hdc));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SendMessageCallbackA
 * Purpose   : The SendMessageCallback function sends the specified message to
 *             a window or windows. The function calls the window procedure for
 *             the specified window and returns immediately. After the window
 *             procedure processes the message, the system calls the specified
 *             callback function, passing the result of the message processing
 *             and an application-defined value to the callback function.
 * Parameters: HWND  hwnd                      handle of destination window
 *             UINT  uMsg                      message to send
 *             WPARAM  wParam                  first message parameter
 *             LPARAM  lParam                  second message parameter
 *             SENDASYNCPROC  lpResultCallBack function to receive message value
 *             DWORD  dwData                   value to pass to callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SendMessageCallbackA(HWND          hWnd,
                                   UINT          uMsg,
                                   WPARAM        wParam,
                                   LPARAM        lParam,
                                   SENDASYNCPROC lpResultCallBack,
                                   DWORD         dwData)
{
  dprintf(("USER32:SendMessageCallBackA (%08xh,%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hWnd,
         uMsg,
         wParam,
         lParam,
         lpResultCallBack,
         dwData));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SendMessageCallbackW
 * Purpose   : The SendMessageCallback function sends the specified message to
 *             a window or windows. The function calls the window procedure for
 *             the specified window and returns immediately. After the window
 *             procedure processes the message, the system calls the specified
 *             callback function, passing the result of the message processing
 *             and an application-defined value to the callback function.
 * Parameters: HWND  hwnd                      handle of destination window
 *             UINT  uMsg                      message to send
 *             WPARAM  wParam                  first message parameter
 *             LPARAM  lParam                  second message parameter
 *             SENDASYNCPROC  lpResultCallBack function to receive message value
 *             DWORD  dwData                   value to pass to callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SendMessageCallbackW(HWND          hWnd,
                                      UINT          uMsg,
                                      WPARAM        wParam,
                                      LPARAM        lParam,
                                      SENDASYNCPROC lpResultCallBack,
                                      DWORD         dwData)
{
  dprintf(("USER32:SendMessageCallBackW (%08xh,%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hWnd,
         uMsg,
         wParam,
         lParam,
         lpResultCallBack,
         dwData));

  return (FALSE);
}


/*****************************************************************************
 * Name      : VOID WIN32API SetDebugErrorLevel
 * Purpose   : The SetDebugErrorLevel function sets the minimum error level at
 *             which Windows will generate debugging events and pass them to a debugger.
 * Parameters: DWORD dwLevel debugging error level
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

VOID WIN32API SetDebugErrorLevel(DWORD dwLevel)
{
  dprintf(("USER32:SetDebugErrorLevel (%08x) not implemented.\n",
         dwLevel));
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetProcessWindowStation
 * Purpose   : The SetProcessWindowStation function assigns a window station
 *             to the calling process. This enables the process to access
 *             objects in the window station such as desktops, the clipboard,
 *             and global atoms. All subsequent operations on the window station
 *             use the access rights granted to hWinSta.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SetProcessWindowStation(HWINSTA hWinSta)
{
  dprintf(("USER32:SetProcessWindowStation (%08x) not implemented.\n",
         hWinSta));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetSystemCursor
 * Purpose   : The SetSystemCursor function replaces the contents of the system
 *             cursor specified by dwCursorId with the contents of the cursor
 *             specified by hCursor, and then destroys hCursor. This function
 *             lets an application customize the system cursors.
 * Parameters: HCURSOR  hCursor    set specified system cursor to this cursor's
 *                                 contents, then destroy this
 *             DWORD    dwCursorID system cursor specified by its identifier
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SetSystemCursor(HCURSOR hCursor,
                                 DWORD   dwCursorId)
{
  dprintf(("USER32:SetSystemCursor (%08xh,%08x) not implemented.\n",
         hCursor,
         dwCursorId));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetThreadDesktop
 * Purpose   : The SetThreadDesktop function assigns a desktop to the calling
 *             thread. All subsequent operations on the desktop use the access
 *             rights granted to hDesk.
 * Parameters: HDESK hDesk handle of the desktop to assign to this thread
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SetThreadDesktop(HDESK hDesktop)
{
  dprintf(("USER32:SetThreadDesktop (%08x) not implemented.\n",
         hDesktop));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetUserObjectInformationA
 * Purpose   : The SetUserObjectInformation function sets information about a
 *             window station or desktop object.
 * Parameters: HANDLE hObject handle of the object for which to set information
 *             int    nIndex  type of information to set
 *             PVOID  lpvInfo points to a buffer that contains the information
 *             DWORD  cbInfo  size, in bytes, of lpvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SetUserObjectInformationA(HANDLE hObject,
                                           int    nIndex,
                                           PVOID  lpvInfo,
                                           DWORD  cbInfo)
{
  dprintf(("USER32:SetUserObjectInformationA (%08xh,%u,%08xh,%08x) not implemented.\n",
           hObject,
           nIndex,
           lpvInfo,
           cbInfo));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetUserObjectInformationW
 * Purpose   : The SetUserObjectInformation function sets information about a
 *             window station or desktop object.
 * Parameters: HANDLE hObject handle of the object for which to set information
 *             int    nIndex  type of information to set
 *             PVOID  lpvInfo points to a buffer that contains the information
 *             DWORD  cbInfo  size, in bytes, of lpvInfo buffer
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SetUserObjectInformationW(HANDLE hObject,
                                           int    nIndex,
                                           PVOID  lpvInfo,
                                           DWORD  cbInfo)
{
  dprintf(("USER32:SetUserObjectInformationW (%08xh,%u,%08xh,%08x) not implemented.\n",
           hObject,
           nIndex,
           lpvInfo,
           cbInfo));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetUserObjectSecurity
 * Purpose   : The SetUserObjectSecurity function sets the security of a user
 *             object. This can be, for example, a window or a DDE conversation
 * Parameters: HANDLE  hObject           handle of user object
 *             SECURITY_INFORMATION * psi address of security information
 *             LPSECURITY_DESCRIPTOR  psd address of security descriptor
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SetUserObjectSecurity(HANDLE hObject,
                                       SECURITY_INFORMATION * psi,
                                       LPSECURITY_DESCRIPTOR  psd)
{
  dprintf(("USER32:SetUserObjectSecuroty (%08xh,%08xh,%08x) not implemented.\n",
           hObject,
           psi,
           psd));

  return (FALSE);
}


/*****************************************************************************
 * Name      : int WIN32API SetWindowRgn
 * Purpose   : The SetWindowRgn function sets the window region of a window. The
 *             window region determines the area within the window where the
 *             operating system permits drawing. The operating system does not
 *             display any portion of a window that lies outside of the window region
 * Parameters: HWND  hWnd    handle to window whose window region is to be set
 *             HRGN  hRgn    handle to region
 *             BOOL  bRedraw window redraw flag
 * Variables :
 * Result    : If the function succeeds, the return value is non-zero.
 *             If the function fails, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API SetWindowRgn(HWND hWnd,
                             HRGN hRgn,
                             BOOL bRedraw)
{
  dprintf(("USER32:SetWindowRgn (%08xh,%08xh,%u) not implemented.\n",
         hWnd,
         hRgn,
         bRedraw));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetWindowsHookW
 * Purpose   : The SetWindowsHook function is not implemented in the Win32 API.
 *             Win32-based applications should use the SetWindowsHookEx function.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : ARGH ! MICROSOFT !
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

HHOOK WIN32API SetWindowsHookW(int nFilterType, HOOKPROC pfnFilterProc)

{
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API ShowWindowAsync
 * Purpose   : The ShowWindowAsync function sets the show state of a window
 *             created by a different thread.
 * Parameters: HWND hwnd     handle of window
 *             int  nCmdShow show state of window
 * Variables :
 * Result    : If the window was previously visible, the return value is TRUE.
 *             If the window was previously hidden, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API ShowWindowAsync (HWND hWnd,
                               int  nCmdShow)
{
  dprintf(("USER32:ShowWindowAsync (%08xh,%08x) not implemented.\n",
         hWnd,
         nCmdShow));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API SwitchDesktop
 * Purpose   : The SwitchDesktop function makes a desktop visible and activates
 *             it. This enables the desktop to receive input from the user. The
 *             calling process must have DESKTOP_SWITCHDESKTOP access to the
 *             desktop for the SwitchDesktop function to succeed.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SwitchDesktop(HDESK hDesktop)
{
  dprintf(("USER32:SwitchDesktop (%08x) not implemented.\n",
         hDesktop));

  return (FALSE);
}


/*****************************************************************************
 * Name      : WORD WIN32API TileWindows
 * Purpose   : The TileWindows function tiles the specified windows, or the child
 *             windows of the specified parent window.
 * Parameters: HWND       hwndParent     handle of parent window
 *             WORD       wFlags         types of windows not to arrange
 *             LPCRECT    lpRect         rectangle to arrange windows in
 *             WORD       cChildrenb     number of windows to arrange
 *             const HWND *ahwndChildren array of window handles
 * Variables :
 * Result    : If the function succeeds, the return value is the number of
 *               windows arranged.
 *             If the function fails, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

WORD WIN32API TileWindows(HWND       hwndParent,
                          UINT       wFlags,
                          const LPRECT lpRect,
                          UINT       cChildrenb,
                          const HWND *ahwndChildren)
{
  dprintf(("USER32:TileWindows (%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
           hwndParent,
           wFlags,
           lpRect,
           cChildrenb,
           ahwndChildren));

   return (0);
}


/*****************************************************************************
 * Name      : int WIN32API ToAscii
 * Purpose   : The ToAscii function translates the specified virtual-key code
 *             and keyboard state to the corresponding Windows character or characters.
 * Parameters: UINT   uVirtKey    virtual-key code
 *             UINT   uScanCode   scan code
 *             PBYTE  lpbKeyState address of key-state array
 *             LPWORD lpwTransKey buffer for translated key
 *             UINT   fuState     active-menu flag
 * Variables :
 * Result    : 0 The specified virtual key has no translation for the current
 *               state of the keyboard.
 *             1 One Windows character was copied to the buffer.
 *             2 Two characters were copied to the buffer. This usually happens
 *               when a dead-key character (accent or diacritic) stored in the
 *               keyboard layout cannot be composed with the specified virtual
 *               key to form a single character.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API ToAscii(UINT   uVirtKey,
                        UINT   uScanCode,
                        PBYTE  lpbKeyState,
                        LPWORD lpwTransKey,
                        UINT   fuState)
{
  dprintf(("USER32:ToAscii (%u,%u,%08xh,%08xh,%u) not implemented.\n",
         uVirtKey,
         uScanCode,
         lpbKeyState,
         lpwTransKey,
         fuState));

  return (0);
}


/*****************************************************************************
 * Name      : int WIN32API ToAsciiEx
 * Purpose   : The ToAscii function translates the specified virtual-key code
 *             and keyboard state to the corresponding Windows character or characters.
 * Parameters: UINT   uVirtKey    virtual-key code
 *             UINT   uScanCode   scan code
 *             PBYTE  lpbKeyState address of key-state array
 *             LPWORD lpwTransKey buffer for translated key
 *             UINT   fuState     active-menu flag
 *             HLK    hlk         keyboard layout handle
 * Variables :
 * Result    : 0 The specified virtual key has no translation for the current
 *               state of the keyboard.
 *             1 One Windows character was copied to the buffer.
 *             2 Two characters were copied to the buffer. This usually happens
 *               when a dead-key character (accent or diacritic) stored in the
 *               keyboard layout cannot be composed with the specified virtual
 *               key to form a single character.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API ToAsciiEx(UINT   uVirtKey,
                          UINT   uScanCode,
                          PBYTE  lpbKeyState,
                          LPWORD lpwTransKey,
                          UINT   fuState,
                          HKL    hkl)
{
  dprintf(("USER32:ToAsciiEx (%u,%u,%08xh,%08xh,%u,%08x) not implemented.\n",
         uVirtKey,
         uScanCode,
         lpbKeyState,
         lpwTransKey,
         fuState,
         hkl));

  return (0);
}


/*****************************************************************************
 * Name      : int WIN32API ToUnicode
 * Purpose   : The ToUnicode function translates the specified virtual-key code
 *             and keyboard state to the corresponding Unicode character or characters.
 * Parameters: UINT   wVirtKey   virtual-key code
 *             UINT   wScanCode  scan code
 *             PBYTE  lpKeyState address of key-state array
 *             LPWSTR pwszBuff   buffer for translated key
 *             int    cchBuff    size of translated key buffer
 *             UINT   wFlags     set of function-conditioning flags
 * Variables :
 * Result    : - 1 The specified virtual key is a dead-key character (accent or
 *                 diacritic). This value is returned regardless of the keyboard
 *                 layout, even if several characters have been typed and are
 *                 stored in the keyboard state. If possible, even with Unicode
 *                 keyboard layouts, the function has written a spacing version of
 *                 the dead-key character to the buffer specified by pwszBuffer.
 *                 For example, the function writes the character SPACING ACUTE
 *                 (0x00B4), rather than the character NON_SPACING ACUTE (0x0301).
 *               0 The specified virtual key has no translation for the current
 *                 state of the keyboard. Nothing was written to the buffer
 *                 specified by pwszBuffer.
 *               1 One character was written to the buffer specified by pwszBuffer.
 *       2 or more Two or more characters were written to the buffer specified by
 *                 pwszBuff. The most common cause for this is that a dead-key
 *                 character (accent or diacritic) stored in the keyboard layout
 *                 could not be combined with the specified virtual key to form a
 *                 single character.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

int WIN32API ToUnicode(UINT   uVirtKey,
                          UINT   uScanCode,
                          PBYTE  lpKeyState,
                          LPWSTR pwszBuff,
                          int    cchBuff,
                          UINT   wFlags)
{
  dprintf(("USER32:ToUnicode (%u,%u,%08xh,%08xh,%u,%08x) not implemented.\n",
         uVirtKey,
         uScanCode,
         lpKeyState,
         pwszBuff,
         cchBuff,
         wFlags));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL WIN32API UnloadKeyboardLayout
 * Purpose   : The UnloadKeyboardLayout function removes a keyboard layout.
 * Parameters: HKL hkl handle of keyboard layout
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *             keyboard layout; otherwise, it is NULL. To get extended error
 *             information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API UnloadKeyboardLayout (HKL hkl)
{
  dprintf(("USER32:UnloadKeyboardLayout (%08x) not implemented.\n",
         hkl));

  return (0);
}


/*****************************************************************************
 * Name      : SHORT WIN32API VkKeyScanExW
 * Purpose   : The VkKeyScanEx function translates a character to the
 *             corresponding virtual-key code and shift state. The function
 *             translates the character using the input language and physical
 *             keyboard layout identified by the given keyboard layout handle.
 * Parameters: UINT uChar character to translate
 *             HKL  hkl   keyboard layout handle
 * Variables :
 * Result    : see docs
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

WORD WIN32API VkKeyScanExW(WCHAR uChar,
                           HKL   hkl)
{
  dprintf(("USER32:VkKeyScanExW (%u,%08x) not implemented.\n",
         uChar,
         hkl));

  return (uChar);
}


/*****************************************************************************
 * Name      : SHORT WIN32API VkKeyScanExA
 * Purpose   : The VkKeyScanEx function translates a character to the
 *             corresponding virtual-key code and shift state. The function
 *             translates the character using the input language and physical
 *             keyboard layout identified by the given keyboard layout handle.
 * Parameters: UINT uChar character to translate
 *             HKL  hkl   keyboard layout handle
 * Variables :
 * Result    : see docs
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

WORD WIN32API VkKeyScanExA(CHAR uChar,
                           HKL  hkl)
{
  dprintf(("USER32:VkKeyScanExA (%u,%08x) not implemented.\n",
         uChar,
         hkl));

  return (uChar);
}


/*****************************************************************************
 * Name      : VOID WIN32API keybd_event
 * Purpose   : The keybd_event function synthesizes a keystroke. The system
 *             can use such a synthesized keystroke to generate a WM_KEYUP or
 *             WM_KEYDOWN message. The keyboard driver's interrupt handler calls
 *             the keybd_event function.
 * Parameters: BYTE  bVk         virtual-key code

 *             BYTE  bScan       hardware scan code
 *             DWORD dwFlags     flags specifying various function options
 *             DWORD dwExtraInfo additional data associated with keystroke
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

VOID WIN32API keybd_event (BYTE bVk,
                           BYTE bScan,
                           DWORD dwFlags,
                           DWORD dwExtraInfo)
{
  dprintf(("USER32:keybd_event (%u,%u,%08xh,%08x) not implemented.\n",
         bVk,
         bScan,
         dwFlags,
         dwExtraInfo));
}


/*****************************************************************************
 * Name      : VOID WIN32API mouse_event
 * Purpose   : The mouse_event function synthesizes mouse motion and button clicks.
 * Parameters: DWORD dwFlags     flags specifying various motion/click variants
 *             DWORD dx          horizontal mouse position or position change
 *             DWORD dy          vertical mouse position or position change
 *             DWORD cButtons    unused, reserved for future use, set to zero
 *             DWORD dwExtraInfo 32 bits of application-defined information
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

VOID WIN32API mouse_event(DWORD dwFlags,
                             DWORD dx,
                             DWORD dy,
                             DWORD cButtons,
                             DWORD dwExtraInfo)
{
  dprintf(("USER32:mouse_event (%08xh,%u,%u,%u,%08x) not implemented.\n",
          dwFlags,
          dx,
          dy,
          cButtons,
          dwExtraInfo));
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetShellWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API SetShellWindow(DWORD x1)
{
  dprintf(("USER32: SetShellWindow(%08x) not implemented.\n",
           x1));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API PlaySoundEvent
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API PlaySoundEvent(DWORD x1)
{
  dprintf(("USER32: PlaySoundEvent(%08x) not implemented.\n",
           x1));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API TileChildWindows
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API TileChildWindows(DWORD x1,
                                  DWORD x2)
{
  dprintf(("USER32: TileChildWindows(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetSysColorsTemp
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API SetSysColorsTemp(void)
{
  dprintf(("USER32: SetSysColorsTemp() not implemented.\n"));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API RegisterNetworkCapabilities
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API RegisterNetworkCapabilities(DWORD x1,
                                             DWORD x2)
{
  dprintf(("USER32: RegisterNetworkCapabilities(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API EndTask
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API EndTask(DWORD x1,
                         DWORD x2,
                         DWORD x3)
{
  dprintf(("USER32: EndTask(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API SwitchToThisWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API SwitchToThisWindow(HWND hwnd,
                                 BOOL x2)
{
  dprintf(("USER32: SwitchToThisWindow(%08xh,%08xh) not implemented.\n",
           hwnd,
           x2));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetNextQueueWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API GetNextQueueWindow(DWORD x1,
                                    DWORD x2)
{
  dprintf(("USER32: GetNextQueueWindow(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API YieldTask
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API YieldTask(void)
{
  dprintf(("USER32: YieldTask() not implemented.\n"));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API WinOldAppHackoMatic
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API WinOldAppHackoMatic(DWORD x1)
{
  dprintf(("USER32: WinOldAppHackoMatic(%08x) not implemented.\n",
           x1));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API DragObject
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

DWORD WIN32API DragObject(HWND x1,HWND x2,UINT x3,DWORD x4,HCURSOR x5)
{
  dprintf(("USER32: DragObject(%08x,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API CascadeChildWindows
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API CascadeChildWindows(DWORD x1,
                                     DWORD x2)
{
  dprintf(("USER32: CascadeChildWindows(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API RegisterSystemThread
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API RegisterSystemThread(DWORD x1,
                                      DWORD x2)
{
  dprintf(("USER32: RegisterSystemThread(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API IsHungThread
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API IsHungThread(DWORD x1)
{
  dprintf(("USER32: IsHungThread(%08xh) not implemented.\n",
           x1));

  return (FALSE); /* default */
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


/*****************************************************************************
 * Name      : BOOL WIN32API UserSignalProc
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

BOOL WIN32API UserSignalProc(DWORD x1,
                                DWORD x2,
                                DWORD x3,
                                DWORD x4)
{
  dprintf(("USER32: SysErrorBox(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return (FALSE); /* default */
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetShellWindow
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/

HWND WIN32API GetShellWindow(void)
{
  dprintf(("USER32: GetShellWindow() not implemented.\n"));

  return (0); /* default */
}



/***********************************************************************
 *           RegisterTasklist32                [USER32.436]
 */
DWORD WIN32API RegisterTasklist (DWORD x)
{
    dprintf(("USER32: RegisterTasklist(%08xh) not implemented.\n",
             x));

    return TRUE;
}


/***********************************************************************
 * DrawCaptionTemp32A [USER32.599]
 *
 * PARAMS
 *
 * RETURNS
 *     Success:
 *     Failure:
 */

BOOL WIN32API DrawCaptionTempA(HWND       hwnd,
                               HDC        hdc,
                               const RECT *rect,
                               HFONT      hFont,
                               HICON      hIcon,
                               LPCSTR     str,
                               UINT       uFlags)
{
  RECT   rc = *rect;

  dprintf(("USER32: DrawCaptionTempA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           hwnd,
           hdc,
           rect,
           hFont,
           hIcon,
           str,
           uFlags));

  /* drawing background */
  if (uFlags & DC_INBUTTON)
  {
    O32_FillRect (hdc,
                  &rc,
                  GetSysColorBrush (COLOR_3DFACE));

    if (uFlags & DC_ACTIVE)
    {
      HBRUSH hbr = O32_SelectObject (hdc,
                                     GetSysColorBrush (COLOR_ACTIVECAPTION));
      O32_PatBlt (hdc,
                  rc.left,
                  rc.top,
                  rc.right - rc.left,
                  rc.bottom - rc.top,
                  0xFA0089);

      O32_SelectObject (hdc,
                        hbr);
    }
  }
  else
  {
    O32_FillRect (hdc,
                  &rc,
                  GetSysColorBrush ((uFlags & DC_ACTIVE) ?
                    COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
  }


  /* drawing icon */
  if ((uFlags & DC_ICON) && !(uFlags & DC_SMALLCAP))
  {
    POINT pt;

    pt.x = rc.left + 2;
    pt.y = (rc.bottom + rc.top - O32_GetSystemMetrics(SM_CYSMICON)) / 2;

    if (hIcon)
    {
      DrawIconEx (hdc,
                  pt.x,
                  pt.y,
                  hIcon,
                  O32_GetSystemMetrics(SM_CXSMICON),
                  O32_GetSystemMetrics(SM_CYSMICON),
                  0,
                  0,
                  DI_NORMAL);
    }
    else
    {
    /* @@@PH 1999/06/08 not ported yet, just don't draw any icon
      WND *wndPtr = WIN_FindWndPtr(hwnd);
      HICON hAppIcon = 0;

      if (wndPtr->class->hIconSm)
        hAppIcon = wndPtr->class->hIconSm;
      else
        if (wndPtr->class->hIcon)
          hAppIcon = wndPtr->class->hIcon;

      DrawIconEx (hdc,
                  pt.x,
                  pt.y,
                  hAppIcon,
                  GetSystemMetrics(SM_CXSMICON),
                  GetSystemMetrics(SM_CYSMICON),
                  0,
                  0,
                  DI_NORMAL);

      WIN_ReleaseWndPtr(wndPtr);
      */
    }

    rc.left += (rc.bottom - rc.top);
  }

  /* drawing text */
  if (uFlags & DC_TEXT)
  {
    HFONT hOldFont;

    if (uFlags & DC_INBUTTON)
      O32_SetTextColor (hdc,
                        O32_GetSysColor (COLOR_BTNTEXT));
    else
      if (uFlags & DC_ACTIVE)
        O32_SetTextColor (hdc,
                          O32_GetSysColor (COLOR_CAPTIONTEXT));
      else
        O32_SetTextColor (hdc,
                          O32_GetSysColor (COLOR_INACTIVECAPTIONTEXT));

    O32_SetBkMode (hdc,
                   TRANSPARENT);

    if (hFont)
      hOldFont = O32_SelectObject (hdc,
                                   hFont);
    else
    {
      NONCLIENTMETRICSA nclm;
      HFONT             hNewFont;

      nclm.cbSize = sizeof(NONCLIENTMETRICSA);
      O32_SystemParametersInfo (SPI_GETNONCLIENTMETRICS,
                                0,
                                &nclm,
                                0);
      hNewFont = O32_CreateFontIndirect ((uFlags & DC_SMALLCAP) ?
                                 &nclm.lfSmCaptionFont : &nclm.lfCaptionFont);
      hOldFont = O32_SelectObject (hdc,
                                   hNewFont);
    }

    if (str)
      O32_DrawText (hdc,
                    str,
                    -1,
                    &rc,
                    DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT);
    else
    {
      CHAR szText[128];
      INT  nLen;

      nLen = O32_GetWindowText (hwnd,
                                szText,
                                128);

      O32_DrawText (hdc,
                    szText,
                    nLen,
                    &rc,
                    DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT);
    }

    if (hFont)
      O32_SelectObject (hdc,
                        hOldFont);
    else
      O32_DeleteObject (O32_SelectObject (hdc,
                                          hOldFont));
  }

  /* drawing focus ??? */
  if (uFlags & 0x2000)
  {
    dprintf(("USER32: DrawCaptionTempA undocumented flag (0x2000)!\n"));
  }

  return 0;
}


/***********************************************************************
 * DrawCaptionTemp32W [USER32.602]
 *
 * PARAMS
 *
 * RETURNS
 *     Success:
 *     Failure:
 */

BOOL WIN32API DrawCaptionTempW (HWND       hwnd,
                                HDC        hdc,
                                const RECT *rect,
                                HFONT      hFont,
                                HICON      hIcon,
                                LPCWSTR    str,
                                UINT       uFlags)
{
  LPSTR strAscii = UnicodeToAsciiString((LPWSTR)str);

  BOOL res = DrawCaptionTempA (hwnd,
                               hdc,
                               rect,
                               hFont,
                               hIcon,
                               strAscii,
                               uFlags);

  FreeAsciiString(strAscii);

  return res;
}

