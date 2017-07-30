/* $Id: caret.cpp,v 1.20 2004-01-11 12:03:13 sandervl Exp $ */

/*
 * Caret functions for USER32
 *
 *
 * TODO: Getting height of window instead of checking whether it needs 
 *       to be window or client appears to be wrong....
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_WIN
#define INCL_GPI
#include <os2wrap.h>
#include <os2sel.h>
#include <stdlib.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <winuser32.h>
#include <wprocess.h>
#include <misc.h>
#include "win32wbase.h"
#include "oslibwin.h"
#include <dcdata.h>
#define INCLUDED_BY_DC
#include "dc.h"
#include "caret.h"

#define DBG_LOCALLOG	DBG_caret
#include "dbglocal.h"

#undef SEVERITY_ERROR
#include <winerror.h>

#ifndef OPEN32API
#define OPEN32API _System
#endif

//
// Global DLL Data (keep it in sync with globaldata.asm!)
//
extern HWND hwndCaret; // = 0
extern HBITMAP hbmCaret; // = 0
extern int CaretWidth, CaretHeight; // = 0
extern int CaretPosX, CaretPosY; // = 0
extern INT CaretIsVisible; // =0, visible if > 0

extern "C" {

BOOL WIN32API CreateCaret (HWND hwnd, HBITMAP hBmp, int width, int height)
{
   dprintf(("USER32:  CreateCaret %x", hwnd));

   if (hwnd == NULLHANDLE)
   {
      return FALSE;
   }
   else
   {
       BOOL rc;
       Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

       if (!wnd) return (FALSE);

       rc = O32_CreateCaret (wnd->getOS2WindowHandle(), hBmp, width, height);
       if (rc)
       {
           hwndCaret      = hwnd;
           hbmCaret       = hBmp;
           CaretWidth     = width;
           CaretHeight    = height;
           CaretIsVisible = 0;
       }

       RELEASE_WNDOBJ(wnd);
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
   CaretIsVisible = 0;

   rc = _DestroyCaret();

   return (rc);
}

BOOL WIN32API SetCaretBlinkTime (UINT mSecs)
{
   BOOL rc;

   dprintf(("USER32: SetCaretBlinkTime %d ms", mSecs));

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

/* 
 * The SetCaretPos function moves the caret to the specified coordinates. If 
 * the window that owns the caret was created with the CS_OWNDC class style, 
 * then the specified coordinates are subject to the mapping mode of the device
 * context associated with that window.
 *
 */
BOOL WIN32API SetCaretPos (int x, int y)
{
   LONG       xNew = 0, yNew = 0;
   BOOL       result = TRUE;
   BOOL       rc;
   CURSORINFO cursorInfo;
   POINTL     caretPos = { x, y };

   dprintf(("USER32: SetCaretPos (%d,%d)", x, y));

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
              RELEASE_WNDOBJ(wnd);
              SetLastError(ERROR_INTERNAL_ERROR);
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
           long height = wnd->getClientHeight();
           caretPos.y = height - caretPos.y;
           xNew = caretPos.x;
           yNew = caretPos.y - cursorInfo.cy;
        }

        hwndCaret = wnd->getWindowHandle();
        CaretPosX = x;
        CaretPosY = y;
        RELEASE_WNDOBJ(wnd);

        rc = WinCreateCursor (cursorInfo.hwnd, xNew, yNew, 0, 0, CURSOR_SETPOS, NULL);
      }
   }
   if (rc == FALSE)
   {
      SetLastError (ERROR_INVALID_PARAMETER);
      result = FALSE;
   }

   return (result);
}

BOOL WIN32API GetCaretPos (PPOINT pPoint)
{
   CURSORINFO cursorInfo;

   dprintf(("USER32: GetCaretPos"));

   if (WinQueryCursorInfo (HWND_DESKTOP, &cursorInfo))
   {
      if (cursorInfo.hwnd != HWND_DESKTOP)
      {
         HPS hps   = NULLHANDLE;
         HWND hwnd = cursorInfo.hwnd;
         Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromOS2Handle (hwnd);

         if (wnd && wnd->isOwnDC())
            hps = wnd->getOwnDC();

         if(wnd == NULL) {
            dprintf(("ERROR: GetCaretPos: wnd == NULL!"));
            return FALSE;
         }

         POINTL caretPos = {cursorInfo.x,cursorInfo.y} ;
         if (hps) {
            GpiConvert (hps, CVTC_DEVICE, CVTC_WORLD, 1, &caretPos);
            cursorInfo.x = caretPos.x;
            cursorInfo.y = caretPos.y;
         } 
         else {
            long height   = wnd->getClientHeight();
            caretPos.y   += cursorInfo.cy;
            cursorInfo.y  = height - caretPos.y;
         }
         RELEASE_WNDOBJ(wnd);
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

   dprintf(("USER32:  ShowCaret %x", hwnd));

   CaretIsVisible++;
   if (CaretIsVisible == 1)
     rc = _ShowCaret (Win32ToOS2Handle (hwnd));
   else
     rc = TRUE;

   return (rc);
}

BOOL WIN32API HideCaret (HWND hwnd)
{
   BOOL rc = FALSE;

   dprintf(("USER32:  HideCaret"));

   CaretIsVisible--;
   if (CaretIsVisible == 0)
     rc = _HideCaret (Win32ToOS2Handle (hwnd));
   else
     rc = TRUE;

   return (rc);
}

} // extern "C"

void recreateCaret (HWND hwndFocus)
{
   CURSORINFO cursorInfo;
   INT x;

   if ((hwndFocus != 0) && (hwndCaret == hwndFocus) &&
       !WinQueryCursorInfo (HWND_DESKTOP, &cursorInfo))
   {
      dprintf(("recreateCaret for %x", hwndFocus));

      CreateCaret (hwndCaret, hbmCaret, CaretWidth, CaretHeight);
      SetCaretPos (CaretPosX, CaretPosY);
      if (CaretIsVisible > 0)
        _ShowCaret(Win32ToOS2Handle(hwndCaret));
   }
}
