/* $Id: caret.cpp,v 1.2 1999-09-28 08:00:56 dengert Exp $ */

/*
 * Caret functions for USER32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <os2sel.h>
#include <stdlib.h>
#include "win32type.h"
#include <winconst.h>
#include <wprocess.h>
#include <misc.h>
#include <win32wbase.h>
#include "oslibwin.h"
#include "dcdata.h"
#include "dc.h"
#include "caret.h"

#undef SEVERITY_ERROR
#include <winerror.h>

#ifndef OPEN32API
#define OPEN32API _System
#endif

#pragma data_seg(_GLOBALDATA)

static HWND hwndCaret;
static HBITMAP hbmCaret;
static int CaretWidth, CaretHeight;
static int CaretPosX, CaretPosY;
static BOOL CaretIsVisible;

#pragma data_seg()

PVOID   APIENTRY  GpiQueryDCData (HPS hps);
VOID    OPEN32API _O32_SetLastError( DWORD );
BOOL    OPEN32API _O32_CreateCaret (HWND hwnd, HBITMAP hbm, int width, int height);
BOOL    APIENTRY  _DestroyCaret (void);
UINT    APIENTRY  _GetCaretBlinkTime (void);
BOOL    APIENTRY  _HideCaret (HWND hwnd);
BOOL    APIENTRY  _SetCaretBlinkTime (UINT mSecs);
BOOL    APIENTRY  _ShowCaret (HWND hwnd);

BOOL WIN32API CreateCaret (HWND hwnd, HBITMAP hBmp, int width, int height)
{
   dprintf(("USER32:  CreateCaret"));

   if (hwnd == NULLHANDLE)
   {
      return FALSE;
   }
   else
   {
       BOOL rc;
       Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

       if (!wnd) return (FALSE);

       USHORT sel = RestoreOS2FS();
       wnd->SetFakeOpen32();

       rc = _O32_CreateCaret (hwnd, hBmp, width, height);
       if (rc)
       {
           hwndCaret   = hwnd;
           hbmCaret    = hBmp;
           CaretWidth  = width;
           CaretHeight = height;
       }

       wnd->RemoveFakeOpen32();
       SetFS(sel);
       return (rc);
   }
}

BOOL WIN32API DestroyCaret()
{
   BOOL rc;
   USHORT sel = RestoreOS2FS();

   dprintf(("USER32:  DestroyCaret"));

   hwndCaret      = 0;
   hbmCaret       = 0;
   CaretWidth     = 0;
   CaretHeight    = 0;
   CaretIsVisible = FALSE;

   rc = _DestroyCaret();

   SetFS(sel);
   return (rc);
}

BOOL WIN32API SetCaretBlinkTime (UINT mSecs)
{
   USHORT sel = RestoreOS2FS();
   BOOL rc;

   dprintf(("USER32:  SetCaretBlinkTime"));

   rc = _SetCaretBlinkTime (mSecs);

   SetFS(sel);
   return (rc);
}

UINT WIN32API GetCaretBlinkTime()
{
   UINT rc;
   USHORT sel = RestoreOS2FS();

   dprintf(("USER32:  GetCaretBlinkTime"));

   rc = _GetCaretBlinkTime();

   SetFS(sel);
   return (rc);
}

BOOL WIN32API SetCaretPos (int x, int y)
{
   USHORT     sel = RestoreOS2FS();
   LONG       xNew = 0, yNew = 0;
   BOOL       result = TRUE;
   BOOL       rc;
   CURSORINFO cursorInfo;
   POINTL     caretPos = { x, y };

   dprintf(("USER32:  SetCaretPos"));

   rc = WinQueryCursorInfo (HWND_DESKTOP, &cursorInfo);
   if (rc == TRUE)
   {
      HWND hwnd = cursorInfo.hwnd;
      Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromOS2Handle (hwnd);
      if (wnd)
      {
        if (wnd->isOwnDC())
        {
           HPS     hps  = wnd->getOwnDC();
           pDCData pHps = (pDCData)GpiQueryDCData(hps);
           if (!pHps)
           {
              _O32_SetLastError (ERROR_INTERNAL_ERROR);
              SetFS(sel);
              return FALSE;
           }
           GpiConvert (pHps->hps, CVTC_WORLD, CVTC_DEVICE, 1, &caretPos);
           xNew = caretPos.x;

           if (isYup (pHps))
              yNew = caretPos.y;
           else
              yNew = caretPos.y - cursorInfo.cy;
        }
        else
        {
           long height = wnd->getWindowHeight();
           caretPos.y = height - caretPos.y - 1;
           xNew = caretPos.x;
           yNew = caretPos.y - cursorInfo.cy;
        }

        hwndCaret = wnd->getWindowHandle();
        CaretPosX = x;
        CaretPosY = y;

        rc = WinCreateCursor (cursorInfo.hwnd, xNew, yNew, 0, 0, CURSOR_SETPOS, NULL);
      }
   }
   if (rc == FALSE)
   {
      _O32_SetLastError (ERROR_INVALID_PARAMETER);
      result = FALSE;
   }

   SetFS(sel);
   return (result);
}

BOOL WIN32API GetCaretPos (PPOINT pPoint)
{
   USHORT sel = RestoreOS2FS();
   CURSORINFO cursorInfo;

   dprintf(("USER32:  GetCaretPos"));

   if (WinQueryCursorInfo (HWND_DESKTOP, &cursorInfo))
   {
      if (cursorInfo.hwnd != HWND_DESKTOP)
      {
         HPS hps   = NULLHANDLE;
         HWND hwnd = cursorInfo.hwnd;
         Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromOS2Handle (hwnd);

         if (wnd && wnd->isOwnDC())
            hps = wnd->getOwnDC();

         POINTL caretPos = {cursorInfo.x,cursorInfo.y} ;
         if (hps) {
            GpiConvert (hps, CVTC_DEVICE, CVTC_WORLD, 1, &caretPos);
            cursorInfo.x = caretPos.x;
            cursorInfo.y = caretPos.y;
         } else {
            long height  = wnd->getWindowHeight();
            caretPos.y  += cursorInfo.cy;
            cursorInfo.y = height - caretPos.y - 1;
         }
      }
      pPoint->x = cursorInfo.x;
      pPoint->y = cursorInfo.y;

      SetFS(sel);
      return TRUE;
   }
   else
   {
      SetFS(sel);
      return FALSE;
   }
}

BOOL WIN32API ShowCaret (HWND hwnd)
{
   USHORT sel = RestoreOS2FS();
   BOOL   rc = FALSE;

   dprintf(("USER32:  ShowCaret"));

   CaretIsVisible = TRUE;
   rc = _ShowCaret (Win32BaseWindow::Win32ToOS2Handle (hwnd));

   SetFS(sel);
   return (rc);
}

BOOL WIN32API HideCaret (HWND hwnd)
{
   USHORT sel = RestoreOS2FS();
   BOOL rc = FALSE;

   dprintf(("USER32:  HideCaret"));

   CaretIsVisible = FALSE;
   rc = _HideCaret (Win32BaseWindow::Win32ToOS2Handle (hwnd));

   SetFS(sel);
   return (rc);
}

void recreateCaret (HWND hwndFocus)
{
   CURSORINFO cursorInfo;

   if ((hwndCaret == hwndFocus) &&
       !WinQueryCursorInfo (HWND_DESKTOP, &cursorInfo))
   {
      CreateCaret (hwndCaret, hbmCaret, CaretWidth, CaretHeight);
      SetCaretPos (CaretPosX, CaretPosY);
      if (CaretIsVisible)
         ShowCaret (hwndCaret);
   }
}
