/* $Id: caret.cpp,v 1.6 1999-10-22 18:11:42 sandervl Exp $ */

/*
 * Caret functions for USER32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_WIN
#define INCL_GPI
#include <os2wrap.h>
#include <os2sel.h>
#include <stdlib.h>
#include "win32type.h"
#include <winconst.h>
#include <wprocess.h>
#include <misc.h>
#include <win32wbase.h>
#include "oslibwin.h"
#include "dcdata.h"
#define INCLUDED_BY_DC
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

       wnd->SetFakeOpen32();

       rc = _O32_CreateCaret (wnd->getOS2WindowHandle(), hBmp, width, height);
       if (rc)
       {
           hwndCaret   = hwnd;
           hbmCaret    = hBmp;
           CaretWidth  = width;
           CaretHeight = height;
       }

       wnd->RemoveFakeOpen32();
       return (rc);
   }
}

BOOL WIN32API DestroyCaret()
{
   BOOL rc;

   dprintf(("USER32:  DestroyCaret"));

   hwndCaret      = 0;
   hbmCaret       = 0;
   CaretWidth     = 0;
   CaretHeight    = 0;
   CaretIsVisible = FALSE;

   rc = _DestroyCaret();

   return (rc);
}

BOOL WIN32API SetCaretBlinkTime (UINT mSecs)
{
   BOOL rc;

   dprintf(("USER32:  SetCaretBlinkTime"));

   rc = _SetCaretBlinkTime (mSecs);

   return (rc);
}

UINT WIN32API GetCaretBlinkTime()
{
   UINT rc;

   dprintf(("USER32:  GetCaretBlinkTime"));

   rc = _GetCaretBlinkTime();
   return (rc);
}

BOOL WIN32API SetCaretPos (int x, int y)
{
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

   return (result);
}

BOOL WIN32API GetCaretPos (PPOINT pPoint)
{
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
         else
         {
           return FALSE;
         }

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

      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

BOOL WIN32API ShowCaret (HWND hwnd)
{
   BOOL   rc = FALSE;

   dprintf(("USER32:  ShowCaret"));

   CaretIsVisible = TRUE;
   rc = _ShowCaret (Win32BaseWindow::Win32ToOS2Handle (hwnd));

   return (rc);
}

BOOL WIN32API HideCaret (HWND hwnd)
{
   BOOL rc = FALSE;

   dprintf(("USER32:  HideCaret"));

   CaretIsVisible = FALSE;
   rc = _HideCaret (Win32BaseWindow::Win32ToOS2Handle (hwnd));

   return (rc);
}

void recreateCaret (HWND hwndFocus)
{
   CURSORINFO cursorInfo;

   if ((hwndFocus != 0) && (hwndCaret == hwndFocus) &&
       !WinQueryCursorInfo (HWND_DESKTOP, &cursorInfo))
   {
      dprintf(("recreateCaret for %x", hwndFocus));

      CreateCaret (hwndCaret, hbmCaret, CaretWidth, CaretHeight);
      SetCaretPos (CaretPosX, CaretPosY);
      if (CaretIsVisible)
         ShowCaret (hwndCaret);
   }
}
