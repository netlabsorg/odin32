/* $Id: user32.cpp,v 1.18 1999-09-04 19:42:28 sandervl Exp $ */

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

/*
  CB:
  - don't replace GDI functions (hdc), only convert window handle
  - always use OSRECTL for PM functions
  - POINT == POINTL
*/

#include <os2win.h>
#include "misc.h"

#include "user32.h"
#include <winicon.h>
#include "usrcall.h"
#include "syscolor.h"

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <oslibwin.h>
#include <win32wnd.h>

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

/* Coordinate Transformation */

inline void OS2ToWin32ScreenPos(POINT *dest,POINT *source)
{
  dest->x = source->x;
  dest->y = OSLibWinQuerySysValue(OSLIB_HWND_DESKTOP,SVOS_CYSCREEN)-1-source->y;
}

inline void Win32ToOS2ScreenPos(POINT *dest,POINT *source)
{
  OS2ToWin32ScreenPos(dest,source); //transform back
}

/* Rectangle Functions - parts from wine/windows/rect.c */

BOOL WIN32API CopyRect( PRECT lprcDst, const RECT * lprcSrc)
{
//    ddprintf(("USER32:  CopyRect\n"));
    if (!lprcDst || !lprcSrc) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    memcpy(lprcDst,lprcSrc,sizeof(RECT));

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EqualRect( const RECT *lprc1, const RECT *lprc2)
{
#ifdef DEBUG
    WriteLog("USER32:  EqualRect\n");
#endif
    if (!lprc1 || !lprc2)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    return (lprc1->left == lprc2->left &&
            lprc1->right == lprc2->right &&
            lprc1->top == lprc2->top &&
            lprc1->bottom == lprc2->bottom);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InflateRect( PRECT lprc, int dx, int  dy)
{
#ifdef DEBUG
    WriteLog("USER32:  InflateRect\n");
#endif
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lprc->left   -= dx;
    lprc->right  += dx;
    lprc->top    -= dy;
    lprc->bottom += dy;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IntersectRect( PRECT lprcDst, const RECT * lprcSrc1, const RECT * lprcSrc2)
{
#ifdef DEBUG
////    WriteLog("USER32:  IntersectRect\n");
#endif
    if (!lprcDst || !lprcSrc1 || !lprcSrc2)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if (IsRectEmpty(lprcSrc1) || IsRectEmpty(lprcSrc2) ||
       (lprcSrc1->left >= lprcSrc2->right) || (lprcSrc2->left >= lprcSrc1->right) ||
       (lprcSrc1->top >= lprcSrc2->bottom) || (lprcSrc2->top >= lprcSrc1->bottom))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      SetRectEmpty(lprcDst);
      return FALSE;
    }
    lprcDst->left   = MAX(lprcSrc1->left,lprcSrc2->left);
    lprcDst->right  = MIN(lprcSrc1->right,lprcSrc2->right);
    lprcDst->top    = MAX(lprcSrc1->top,lprcSrc2->top);
    lprcDst->bottom = MIN(lprcSrc1->bottom,lprcSrc2->bottom);

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsRectEmpty( const RECT * lprc)
{
#ifdef DEBUG
    WriteLog("USER32:  IsRectEmpty\n");
#endif
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    return (lprc->left == lprc->right || lprc->top == lprc->bottom);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetRect( PRECT lprc, int x, int  y)
{
#ifdef DEBUG
////    WriteLog("USER32:  OffsetRect\n");
#endif
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lprc->left   += x;
    lprc->right  += x;
    lprc->top    += y;
    lprc->bottom += y;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtInRect( const RECT *lprc, POINT pt)
{
#ifdef DEBUG1
    WriteLog("USER32:  PtInRect\n");
#endif
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    return (pt.x >= lprc->left &&
            pt.x < lprc->right &&
            pt.y >= lprc->top &&
            pt.y < lprc->bottom);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRect( PRECT lprc, int nLeft, int nTop, int nRight, int  nBottom)
{
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lprc->left   = nLeft;
    lprc->top    = nTop;
    lprc->right  = nRight;
    lprc->bottom = nBottom;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRectEmpty( PRECT lprc)
{
#ifdef DEBUG
    WriteLog("USER32:  SetRectEmpty\n");
#endif
    if (!lprc)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lprc->left = lprc->right = lprc->top = lprc->bottom = 0;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SubtractRect( PRECT lprcDest, const RECT * lprcSrc1, const RECT * lprcSrc2)
{
#ifdef DEBUG
    WriteLog("USER32:  SubtractRect");
#endif
    RECT tmp;

    if (!lprcDest || !lprcSrc1 || !lprcSrc2)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if (IsRectEmpty(lprcSrc1))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      SetRectEmpty(lprcDest);
      return FALSE;
    }
    *lprcDest = *lprcSrc1;
    if (IntersectRect(&tmp,lprcSrc1,lprcSrc2))
    {
      if (EqualRect(&tmp,lprcDest))
      {
        SetRectEmpty(lprcDest);
        return FALSE;
      }
      if ((tmp.top == lprcDest->top) && (tmp.bottom == lprcDest->bottom))
      {
        if (tmp.left == lprcDest->left) lprcDest->left = tmp.right;
        else if (tmp.right == lprcDest->right) lprcDest->right = tmp.left;
      }
      else if ((tmp.left == lprcDest->left) && (tmp.right == lprcDest->right))
      {
        if (tmp.top == lprcDest->top) lprcDest->top = tmp.bottom;
        else if (tmp.bottom == lprcDest->bottom) lprcDest->bottom = tmp.top;
      }
    }

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnionRect( PRECT lprcDst, const RECT *lprcSrc1, const RECT *lprcSrc2)
{
#ifdef DEBUG
    WriteLog("USER32:  UnionRect\n");
#endif
    if (!lprcDst || !lprcSrc1 || !lprcSrc2)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if (IsRectEmpty(lprcSrc1))
    {
      if (IsRectEmpty(lprcSrc2))
      {
        SetLastError(ERROR_INVALID_PARAMETER);
        SetRectEmpty(lprcDst);
        return FALSE;
      }
      else *lprcDst = *lprcSrc2;
    }
    else
    {
      if (IsRectEmpty(lprcSrc2)) *lprcDst = *lprcSrc1;
      else
      {
        lprcDst->left   = MIN(lprcSrc1->left,lprcSrc2->left);
        lprcDst->right  = MAX(lprcSrc1->right,lprcSrc2->right);
        lprcDst->top    = MIN(lprcSrc1->top,lprcSrc2->top);
        lprcDst->bottom = MAX(lprcSrc1->bottom,lprcSrc2->bottom);	
      }
    }

    return TRUE;
}

/* String Manipulating Functions */

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
    PCHAR   pszTemp;
    PCHAR   pszTemp1;
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
BOOL WIN32API MessageBeep( UINT uType)
{
    INT flStyle;

#ifdef DEBUG
    WriteLog("USER32:  MessageBeep\n");
#endif

    switch (uType)
    {
      case 0xFFFFFFFF:
        OSLibDosBeep(500,50);
        return TRUE;
      case MB_ICONASTERISK:
        flStyle = WAOS_NOTE;
        break;
      case MB_ICONEXCLAMATION:
        flStyle = WAOS_WARNING;
        break;
      case MB_ICONHAND:
      case MB_ICONQUESTION:
      case MB_OK:
        flStyle = WAOS_NOTE;
        break;
      default:
        flStyle = WAOS_ERROR; //CB: should be right
        break;
    }
    return OSLibWinAlarm(OSLIB_HWND_DESKTOP,flStyle);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetSystemMetrics(int nIndex)
{
   int rc = 0;

   switch(nIndex) {
    case SM_CXICONSPACING: //TODO: size of grid cell for large icons
        rc = OSLibWinQuerySysValue(OSLIB_HWND_DESKTOP,SVOS_CXICON);
        //CB: return standard windows icon size?
        //rc = 32;
        break;
    case SM_CYICONSPACING:
        rc = OSLibWinQuerySysValue(OSLIB_HWND_DESKTOP,SVOS_CYICON);
        //read SM_CXICONSPACING comment
        //rc = 32;
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
        //CB: replace with const
        rc = O32_GetSystemMetrics(SM_CXMINIMIZED);
        break;
    case SM_CYMINSPACING:
        //CB: replace with const
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
        rc = OSLibWinQuerySysValue(OSLIB_HWND_DESKTOP,SVOS_CXSCREEN);
        break;
    case SM_CYMAXTRACK:
    case SM_CYMAXIMIZED:
        rc = OSLibWinQuerySysValue(OSLIB_HWND_DESKTOP,SVOS_CYSCREEN);
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
        rc = OSLibWinQuerySysValue(OSLIB_HWND_DESKTOP,SVOS_CYMENU);
        break;
    case SM_SLOWMACHINE:
        rc = FALSE; //even a slow machine is fast with OS/2 :)
        break;
    case SM_MIDEASTENABLED:
        rc = FALSE;
        break;
    case SM_CMETRICS:
        //CB: replace with const
        rc = O32_GetSystemMetrics(44);  //Open32 changed this one
        break;
    default:
        //better than nothing
        rc = O32_GetSystemMetrics(nIndex);
        break;
    }
#ifdef DEBUG
    WriteLog("USER32:  GetSystemMetrics %d returned %d\n", nIndex, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetTimer( HWND hwnd, UINT idTimer, UINT uTimeout, TIMERPROC  tmprc)
{
#ifdef DEBUG
    WriteLog("USER32: SetTimer INCORRECT CALLING CONVENTION FOR HANDLER!!!!!\n");
#endif
    hwnd = Win32Window::Win32ToOS2Handle(hwnd);
    //SvL: Write callback handler class for this one
    //CB: replace
    return O32_SetTimer(hwnd,idTimer,uTimeout,(TIMERPROC_O32)tmprc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API KillTimer(HWND hWnd, UINT uIDEvent)
{
#ifdef DEBUG
    WriteLog("USER32:  KillTimer\n");
#endif
    hWnd = Win32Window::Win32ToOS2Handle(hWnd);
    //WinStopTimer
    //CB: replace
    return O32_KillTimer(hWnd,uIDEvent);
}
//******************************************************************************
//******************************************************************************
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
int WIN32API ShowCursor( BOOL arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  ShowCursor\n");
#endif
    //WinShowCursor(OSLIB_HWND_DESKTOP,arg1); //CB: not the same
    return O32_ShowCursor(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WinHelpA( HWND hwnd, LPCSTR lpszHelp, UINT uCommand, DWORD  dwData)
{
#ifdef DEBUG
    WriteLog("USER32:  WinHelp not implemented %s\n", lpszHelp);
#endif
//    hwnd = Win32Window::Win32ToOS2Handle(hwnd);
//    return O32_WinHelp(arg1, arg2, arg3, arg4);

    return(TRUE);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
BOOL WIN32API ClipCursor(const RECT * lpRect)
{
#ifdef DEBUG
    WriteLog("USER32:  ClipCursor\n");
#endif
    //CB: how to replace?
    return O32_ClipCursor(lpRect);
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
BOOL WIN32API GetCursorPos( PPOINT lpPoint)
{
    BOOL rc;
    POINT point;
#ifdef DEBUG
////    WriteLog("USER32:  GetCursorPos\n");
#endif
    if (!lpPoint) return FALSE;
    if (OSLibWinQueryPointerPos(OSLIB_HWND_DESKTOP,&point)) //POINT == POINTL
    {
      OS2ToWin32ScreenPos(lpPoint,&point);
      return TRUE;
    } else return FALSE;
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
WORD WIN32API VkKeyScanA( char ch)
{
#ifdef DEBUG
    WriteLog("USER32:  VkKeyScanA\n");
#endif
    return O32_VkKeyScan(ch);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
SHORT WIN32API GetKeyState( int nVirtKey)
{
#ifdef DEBUG
    WriteLog("USER32:  GetKeyState %d\n", nVirtKey);
#endif
    return O32_GetKeyState(nVirtKey);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API SetCursor( HCURSOR hcur)
{
#ifdef DEBUG
    WriteLog("USER32:  SetCursor\n");
#endif
    return O32_SetCursor(hcur);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCursorPos( int arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  SetCursorPos\n");
#endif
//CB:{a} stopped here
    return O32_SetCursorPos(arg1, arg2);
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
BOOL WIN32API CheckRadioButton( HWND arg1, UINT arg2, UINT arg3, UINT  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  CheckRadioButton\n");
#endif
    //CB: check radio buttons in interval
    if (arg2 > arg3) return (FALSE);
    for (UINT x=arg2;x <= arg3;x++)
    {
     SendDlgItemMessageA(arg1,x,BM_SETCHECK,(x == arg4) ? BST_CHECKED : BST_UNCHECKED,0);
    }
    return (TRUE);
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
int WIN32API GetUpdateRgn( HWND arg1, HRGN arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetUpdateRgn\n");
#endif
    return O32_GetUpdateRgn(arg1, arg2, arg3);
}
//******************************************************************************


//******************************************************************************
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
BOOL WIN32API SetDoubleClickTime( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SetDoubleClickTime\n");
#endif
    return O32_SetDoubleClickTime(arg1);
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
BOOL WIN32API SwapMouseButton( BOOL arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  SwapMouseButton\n");
#endif
    return O32_SwapMouseButton(arg1);
}
//******************************************************************************
/* Not support by Open32 (not included are the new win9x parameters):
      case SPI_GETFASTTASKSWITCH:
      case SPI_GETGRIDGRANULARITY:
      case SPI_GETICONTITLELOGFONT:
      case SPI_GETICONTITLEWRAP:
      case SPI_GETMENUDROPALIGNMENT:
      case SPI_ICONHORIZONTALSPACING:
      case SPI_ICONVERTICALSPACING:
      case SPI_LANGDRIVER:
      case SPI_SETFASTTASKSWITCH:
      case SPI_SETGRIDGRANULARITY:
      case SPI_SETICONTITLELOGFONT:
      case SPI_SETICONTITLEWRAP:
      case SPI_SETMENUDROPALIGNMENT:
      case SPI_GETSCREENSAVEACTIVE:
      case SPI_GETSCREENSAVETIMEOUT:
      case SPI_SETDESKPATTERN:
      case SPI_SETDESKWALLPAPER:
      case SPI_SETSCREENSAVEACTIVE:
      case SPI_SETSCREENSAVETIMEOUT:
*/
//******************************************************************************
BOOL WIN32API SystemParametersInfoA(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
 BOOL rc = TRUE;
 NONCLIENTMETRICSA *cmetric = (NONCLIENTMETRICSA *)pvParam;

  switch(uiAction) {
    case SPI_SCREENSAVERRUNNING:
        *(BOOL *)pvParam = FALSE;
        break;
    case SPI_GETDRAGFULLWINDOWS:
        *(BOOL *)pvParam = FALSE;
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
        break;
    case SPI_GETICONTITLELOGFONT:
    {
	LPLOGFONTA lpLogFont = (LPLOGFONTA)pvParam;

	/* from now on we always have an alias for MS Sans Serif */
	strcpy(lpLogFont->lfFaceName, "MS Sans Serif");
	lpLogFont->lfHeight = -GetProfileIntA("Desktop","IconTitleSize", 8);
	lpLogFont->lfWidth = 0;
	lpLogFont->lfEscapement = lpLogFont->lfOrientation = 0;
	lpLogFont->lfWeight = FW_NORMAL;
	lpLogFont->lfItalic = FALSE;
	lpLogFont->lfStrikeOut = FALSE;
	lpLogFont->lfUnderline = FALSE;
	lpLogFont->lfCharSet = ANSI_CHARSET;
	lpLogFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lpLogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lpLogFont->lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	break;
    }
    case SPI_GETBORDER:
	*(INT *)pvParam = GetSystemMetrics( SM_CXFRAME );
	break;

    case SPI_GETWORKAREA:
	SetRect( (RECT *)pvParam, 0, 0,
		GetSystemMetrics( SM_CXSCREEN ),
		GetSystemMetrics( SM_CYSCREEN )
	);
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
    case SPI_GETICONTITLELOGFONT:
    {
	LPLOGFONTW lpLogFont = (LPLOGFONTW)pvParam;

	/* from now on we always have an alias for MS Sans Serif */
	lstrcpyW(lpLogFont->lfFaceName, (LPCWSTR)L"MS Sans Serif");
	lpLogFont->lfHeight = -GetProfileIntA("Desktop","IconTitleSize", 8);
	lpLogFont->lfWidth = 0;
	lpLogFont->lfEscapement = lpLogFont->lfOrientation = 0;
	lpLogFont->lfWeight = FW_NORMAL;
	lpLogFont->lfItalic = FALSE;
	lpLogFont->lfStrikeOut = FALSE;
	lpLogFont->lfUnderline = FALSE;
	lpLogFont->lfCharSet = ANSI_CHARSET;
	lpLogFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lpLogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lpLogFont->lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	return TRUE;
    }
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
//TODO: Not complete
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

// @@@PH Win32 BOOL's are casted to INTs
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

// @@@PH Win32 BOOL's are casted to INTs
INT WIN32API GetKeyboardLayoutNameW(LPWSTR pwszKLID)
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


