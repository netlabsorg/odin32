/* $Id: dc.cpp,v 1.7 2000-01-02 19:30:42 cbratschi Exp $ */

/*
 * DC functions for USER32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>

#define INCL_WIN
#define INCL_GPI
#define INCL_GREALL
#define INCL_DEV
#include <os2wrap.h>
//#include <pmddi.h>
#include <stdlib.h>

#include "win32type.h"
#include <winconst.h>
#include <misc.h>
#include <win32wbase.h>
#include <math.h>
#include <limits.h>
#include "oslibwin.h"
#include "dcdata.h"

#define INCLUDED_BY_DC
#include "dc.h"

#undef SEVERITY_ERROR
#include <winerror.h>

#ifndef DEVESC_SETPS
  #define DEVESC_SETPS  49149L
#endif

#define FLOAT_TO_FIXED(x) ((FIXED) ((x) * 65536.0))
#define MICRO_HPS_TO_HDC(x) ((x) & 0xFFFFFFFE)

#define PMRECT_FROM_WINRECT( pmRect, winRect )  \
{                                               \
   (pmRect).xLeft   = (winRect).left;           \
   (pmRect).yBottom = (winRect).bottom;         \
   (pmRect).xRight  = (winRect).right;          \
   (pmRect).yTop    = (winRect).top;            \
}

#define WINRECT_FROM_PMRECT( winRect, pmRect )  \
{                                               \
   (winRect).left   = (pmRect).xLeft;           \
   (winRect).top    = (pmRect).yTop;            \
   (winRect).right  = (pmRect).xRight;          \
   (winRect).bottom = (pmRect).yBottom;         \
}

#define MEM_HPS_MAX 768

const XFORM_W XFORMIdentity = { 1.0, 0.0, 0.0, 1.0, 0, 0 };
const MATRIXLF matrixlfIdentity = { 0x10000, 0, 0, 0, 0x10000, 0, 0, 0, 0};

BOOL setPageXForm(Win32BaseWindow *wnd, pDCData pHps);
BOOL changePageXForm(Win32BaseWindow *wnd, pDCData pHps, PPOINTL pValue, int x, int y, PPOINTL pPrev);
LONG clientHeight(Win32BaseWindow *wnd, HWND hwnd, pDCData pHps);

void TestWideLine (pDCData pHps)
{
   const LOGPEN_W *pLogPen;

   pHps->isWideLine = FALSE;
   pLogPen = pHps->penIsExtPen ?
       &(pHps->lastPenObject->ExtPen.logpen) :
       &(pHps->lastPenObject->Pen.logpen);

   if (((pLogPen->lopnStyle & PS_STYLE_MASK_W) != PS_NULL_W) &&
        (pLogPen->lopnWidth.x > 0))
   {
      POINTL aptl[2] = { 0, 0, pLogPen->lopnWidth.x, pLogPen->lopnWidth.x };

      GpiConvert(pHps->hps, CVTC_WORLD, CVTC_DEVICE, 2, aptl);

      ULONG dx = abs(aptl[0].x - aptl[1].x);
      ULONG dy = abs(aptl[0].y - aptl[1].y);

      pHps->isWideLine = (dx > 1) || (dy > 1);
   }
}

void Calculate1PixelDelta(pDCData pHps)
{
   POINTL aptl[2] = {0, 0, 1, 1};

   GpiConvert(pHps->hps, CVTC_DEVICE, CVTC_WORLD, 2, aptl);
   pHps->worldYDeltaFor1Pixel = (int)(aptl[1].y - aptl[0].y);
   pHps->worldXDeltaFor1Pixel = (int)(aptl[1].x - aptl[0].x);                   // 171182
}

//******************************************************************************

int setMapMode(Win32BaseWindow *wnd, pDCData pHps, int mode)
{
   int    prevMode = 0;
   ULONG  flOptions;

   switch (mode)
   {
      case MM_HIENGLISH_W  : flOptions = PU_HIENGLISH; break;
      case MM_LOENGLISH_W  : flOptions = PU_LOENGLISH; break;
      case MM_HIMETRIC_W   : flOptions = PU_HIMETRIC ; break;
      case MM_LOMETRIC_W   : flOptions = PU_LOMETRIC ; break;
      case MM_TEXT_W       : flOptions = PU_PELS     ; break;
      case MM_TWIPS_W      : flOptions = PU_TWIPS    ; break;
      case MM_ANISOTROPIC_W: flOptions = PU_PELS     ; break;
      case MM_ISOTROPIC_W  : flOptions = PU_LOMETRIC ; break;
      default:
         O32_SetLastError (ERROR_INVALID_PARAMETER);
         return FALSE;
   }

   prevMode = pHps->MapMode;  /* store previous mode */
   pHps->MapMode = mode;

   if (mode == MM_TEXT_W)
   {
      pHps->viewportXExt =
      pHps->viewportYExt = 1.0;
      pHps->windowExt.cx =
      pHps->windowExt.cy = 1;
   }
   else if (mode != MM_ANISOTROPIC_W)
   {
      RECTL rectl;
      SIZEL sizel;
      ULONG data[3];

      data[0] = flOptions;
      data[1] = data[2] = 0;

      if (DevEscape(pHps->hdc ? pHps->hdc : pHps->hps, DEVESC_SETPS, 12, (PBYTE)data, 0, 0) == DEVESC_ERROR)
      {
         O32_SetLastError (ERROR_INVALID_PARAMETER);
         return 0;
      }

      GpiQueryPageViewport(pHps->hps, &rectl);
      pHps->viewportXExt = (double)rectl.xRight;
      pHps->viewportYExt = -(double)rectl.yTop;

      GreGetPageUnits(pHps->hdc? pHps->hdc : pHps->hps, &sizel);
      pHps->windowExt.cx = sizel.cx;
      pHps->windowExt.cy = sizel.cy;

      data[0] = PU_PELS;
      DevEscape(pHps->hdc ? pHps->hdc : pHps->hps, DEVESC_SETPS, 12, (PBYTE)data, 0, 0);
   }

   if (((prevMode != MM_ISOTROPIC_W) && (prevMode != MM_ANISOTROPIC_W)) &&
       ((mode == MM_ISOTROPIC_W) || (mode == MM_ANISOTROPIC_W)))
   {
      if (pHps->lWndXExtSave && pHps->lWndYExtSave)
      {
         changePageXForm (wnd, pHps, (PPOINTL)&pHps->windowExt,
                          pHps->lWndXExtSave, pHps->lWndYExtSave, NULL );
         pHps->lWndXExtSave = pHps->lWndYExtSave = 0;
      }
      if (pHps->lVwpXExtSave && pHps->lVwpYExtSave)
      {
         changePageXForm (wnd, pHps, NULL,
                          pHps->lVwpXExtSave, pHps->lVwpYExtSave, NULL );
         pHps->lVwpXExtSave = pHps->lVwpYExtSave = 0;
      }
   }

   setPageXForm(wnd, pHps);

   return prevMode;
}

BOOL setPageXForm(Win32BaseWindow *wnd, pDCData pHps)
{
   MATRIXLF mlf;
   BOOL rc = TRUE;

   pHps->height = clientHeight(wnd, 0, pHps) - 1;

   double xScale =  pHps->viewportXExt / (double)pHps->windowExt.cx;
   double yScale =  pHps->viewportYExt / (double)pHps->windowExt.cy;

   mlf.fxM11 = FLOAT_TO_FIXED(xScale);
   mlf.fxM12 = 0;
   mlf.lM13  = 0;
   mlf.fxM21 = 0;
   mlf.fxM22 = FLOAT_TO_FIXED(yScale);
   mlf.lM23  = 0;
   mlf.lM31  = pHps->viewportOrg.x - (LONG)(pHps->windowOrg.x * xScale);
   mlf.lM32  = pHps->viewportOrg.y - (LONG)(pHps->windowOrg.y * yScale);

   pHps->isLeftLeft = mlf.fxM11 >= 0;
   pHps->isTopTop = mlf.fxM22 >= 0;

   BOOL bEnableYInversion = FALSE;
   if ((mlf.fxM22 > 0) ||
        ((pHps->graphicsMode == GM_ADVANCED_W) &&
          ((pHps->MapMode == MM_ANISOTROPIC_W) ||
           (pHps->MapMode == MM_ISOTROPIC_W))))
   {
      bEnableYInversion = TRUE;
   }
   else
   {
      bEnableYInversion = FALSE;
      mlf.lM32 = pHps->HPStoHDCInversionHeight + pHps->height - mlf.lM32;
      mlf.fxM22 = -mlf.fxM22;
   }

   if (!pHps->isMetaPS)
//   if ((!pHps->isMetaPS) ||
//      (pHps->pMetaFileObject && pHps->pMetaFileObject->isEnhanced()))
      rc = GpiSetDefaultViewMatrix(pHps->hps, 8, &mlf, TRANSFORM_REPLACE);

   if (bEnableYInversion)
      GpiEnableYInversion(pHps->hps, pHps->height + pHps->HPStoHDCInversionHeight);
   else
      GpiEnableYInversion(pHps->hps, 0);

   TestWideLine(pHps);
   Calculate1PixelDelta(pHps);
   return rc;
}

BOOL changePageXForm(Win32BaseWindow *wnd, pDCData pHps, PPOINTL pValue, int x, int y, PPOINTL pPrev)
{
   BOOL result = FALSE;

   if (pValue)
   {
      if (pPrev)
         *pPrev = *pValue;

      if ((pValue->x == x) && (pValue->y == y)) {
         return TRUE;
      }
      pValue->x = x;
      pValue->y = y;
   }
   else
   {
      if (pPrev)
      {
         pPrev->x = (int)pHps->viewportXExt;
         pPrev->y = (int)pHps->viewportYExt;
      }
      pHps->viewportXExt = (double)x;
      pHps->viewportYExt = (double)y;
   }

   if (pHps->MapMode == MM_ISOTROPIC_W)
   {
      double xExt = fabs(pHps->viewportXExt);
      double yExt = fabs(pHps->viewportYExt);
      double sf = fabs((double)pHps->windowExt.cx / pHps->windowExt.cy);

      if (xExt > (yExt * sf))
      {
         xExt = yExt * sf;

         if ((double)LONG_MAX <= xExt) return (result);

         if (pHps->viewportXExt < 0.0)
            pHps->viewportXExt = -xExt;
         else
            pHps->viewportXExt = xExt;
      }
      else
      {
         yExt = xExt / sf;

         if ((double)LONG_MAX <= yExt) return (result);

         if (pHps->viewportYExt < 0.0)
            pHps->viewportYExt = -yExt;
         else
            pHps->viewportYExt = yExt;
      }
   }
   result = setPageXForm(wnd, pHps);

   return (result);
}

LONG clientHeight(Win32BaseWindow *wnd, HWND hwnd, pDCData pHps)
{
   if ((hwnd == 0) && (pHps != 0))
      hwnd = pHps->hwnd;

   if ((hwnd != 0) || (pHps == 0))
   {
      if (wnd)
         return (wnd->getClientHeight());
      else
         return OSLibQueryScreenHeight();
   }
   else if (pHps->bitmapHandle)
   {
      return pHps->bitmapHeight;
   }
   else if (pHps->isMetaPS)
   {
      return 0;
   }
   else if (pHps->isPrinter)
   {
      return pHps->printPageHeight;
   }
   else
   {
      return MEM_HPS_MAX;
   }
}

BOOL isYup (pDCData pHps)
{
   if (((pHps->windowExt.cy < 0) && (pHps->viewportYExt > 0.0)) ||
       ((pHps->windowExt.cy > 0) && (pHps->viewportYExt < 0.0)))
   {
      if ((pHps->graphicsMode == GM_COMPATIBLE_W) ||
          ((pHps->graphicsMode == GM_ADVANCED_W) && (pHps->xform.eM22 >= 0.0)))
         return TRUE;
   }
   else
   {
      if ((pHps->graphicsMode == GM_ADVANCED_W) && (pHps->xform.eM22 < 0.0))
         return TRUE;
   }
   return FALSE;
}

INT revertDy (Win32BaseWindow *wnd, INT dy)
{
   //SvL: Hack for memory.exe (doesn't get repainted properly otherwise)
//   if (wnd->isOwnDC() && wnd->getOwnDC())
   if (wnd->isOwnDC())
   {
      pDCData pHps = (pDCData)GpiQueryDCData (wnd->getOwnDC());

      if (pHps != NULLHANDLE)
         if (!isYup (pHps))
            dy = -dy;
   }
   else
   {
      dy = -dy;
   }
   return (dy);
}

HDC sendEraseBkgnd (Win32BaseWindow *wnd)
{
   BOOL  erased;
   HWND  hwnd;
   HDC   hdc;
   HPS   hps;
   HRGN  hrgnUpdate, hrgnOld, hrgnClip, hrgnCombined;
   RECTL rectl = { 1, 1, 2, 2 };

   hwnd = wnd->getOS2WindowHandle();
   hps = WinGetPS(hwnd);

   hrgnUpdate = GpiCreateRegion (hps, 1, &rectl);
   WinQueryUpdateRegion (hwnd, hrgnUpdate);
   hrgnClip = GpiQueryClipRegion (hps);

   if (hrgnClip == NULLHANDLE)
   {
      GpiSetClipRegion (hps, hrgnUpdate, &hrgnOld);
   }
   else
   {
      hrgnCombined = GpiCreateRegion (hps, 1, &rectl);
      GpiCombineRegion (hps, hrgnCombined, hrgnClip, hrgnUpdate, CRGN_AND);
      GpiSetClipRegion (hps, hrgnCombined, &hrgnOld);
      GpiDestroyRegion (hps, hrgnUpdate);
      GpiDestroyRegion (hps, hrgnClip);
   }
   if (hrgnOld != NULLHANDLE)
      GpiDestroyRegion (hps, hrgnOld);

   hdc = HPSToHDC (hwnd, hps, NULL, NULL);

   erased = wnd->MsgEraseBackGround (hdc);

   DeleteHDC (hdc);
   WinReleasePS (hps);

   return erased;
}

void releaseOwnDC (HDC hps)
{
   pDCData pHps = (pDCData)GpiQueryDCData ((HPS)hps);

   if (pHps) {
      if (pHps->hrgnHDC)
         GpiDestroyRegion (pHps->hps, pHps->hrgnHDC);

      GpiSetBitmap (pHps->hps, NULL);
      O32_DeleteObject (pHps->nullBitmapHandle);
      GpiDestroyPS(pHps->hps);

      if (pHps->hdc)
         DevCloseDC(pHps->hdc);

// how can a memory chunk allocated by GpiAllocateDCData freed by delete?
//      delete pHps;
   }
}

HDC WIN32API BeginPaint (HWND hWnd, PPAINTSTRUCT_W lpps)
{
   HWND     hwnd = hWnd ? hWnd : HWND_DESKTOP;
   pDCData  pHps = NULLHANDLE;
   RECTL    rect;
   HPS      hPS_ownDC = NULLHANDLE;

   dprintf (("USER32: BeginPaint(%x)", hWnd));

   if ( !lpps )
   {
      O32_SetLastError (ERROR_INVALID_PARAMETER);
      return (HDC)NULLHANDLE;
   }

   if(hWnd == 0x6800003a) {
        hwnd = 0x6800003a;
   }
   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if ((hwnd != HWND_DESKTOP) && wnd->isOwnDC())
   {
        hPS_ownDC = wnd->getOwnDC();
        //SvL: Hack for memory.exe (doesn't get repainted properly otherwise)
        if(hPS_ownDC) {
                pHps = (pDCData)GpiQueryDCData(hPS_ownDC);
                if (!pHps)
                {
                        O32_SetLastError (ERROR_INVALID_PARAMETER);
                        return (HDC)NULLHANDLE;
                }
        }
   }

   HWND hwndClient = wnd->getOS2WindowHandle();
   HPS  hps = WinBeginPaint(hwndClient, hPS_ownDC, &rect);

   if (!pHps)
   {
      HDC hdc = HPSToHDC (hwndClient, hps, NULL, NULL);
      pHps = (pDCData)GpiQueryDCData(hps);
   }

   if (hPS_ownDC == 0)
      setMapMode (wnd, pHps, MM_TEXT_W);
   else
      setPageXForm (wnd, pHps);

   pHps->hdcType = TYPE_3;
   lpps->hdc = (HDC)hps;

   if(!wnd->isSuppressErase()) {
        wnd->setSuppressErase(TRUE);
        wnd->setEraseBkgnd (FALSE, !wnd->MsgEraseBackGround(lpps->hdc));
   }
   lpps->fErase = wnd->isPSErase();

   if (!hPS_ownDC)
   {
      long height  = wnd->getClientHeight();
      rect.yTop    = height - rect.yTop;
      rect.yBottom = height - rect.yBottom;
   }
   else
   {
      rect.yTop--;
      rect.yBottom--;
      GpiConvert(pHps->hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rect);
   }

   WINRECT_FROM_PMRECT(lpps->rcPaint, rect);

   O32_SetLastError(0);
   return (HDC)pHps->hps;
}

BOOL WIN32API EndPaint (HWND hwnd, const PAINTSTRUCT_W *pPaint)
{
   dprintf (("USER32: EndPaint(%x)", hwnd));

   if (!pPaint || !pPaint->hdc )
      return TRUE;

   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if (!wnd) goto exit;

   //SvL: Hack for memory.exe (doesn't get repainted properly otherwise)
   if(wnd->isOwnDC() && wnd->getOwnDC())
//   if(wnd->isOwnDC())
   {
       pDCData pHps = (pDCData)GpiQueryDCData((HPS)pPaint->hdc);
       if (pHps && (pHps->hdcType == TYPE_3))
       {
          WinEndPaint (pHps->hps);
       }
   }
   else
   {
       O32_EndPaint (HWND_DESKTOP, pPaint);
   }
   wnd->setSuppressErase(FALSE);

exit:
   O32_SetLastError(0);
   return TRUE;
}

BOOL WIN32API GetUpdateRect (HWND hwnd, LPRECT pRect, BOOL erase)
{
   if (!hwnd)
   {
      O32_SetLastError (ERROR_INVALID_HANDLE);
      return FALSE;
   }

   RECTL rectl;
   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if (!wnd)
   {
      O32_SetLastError (ERROR_INVALID_HANDLE);
      return FALSE;
   }

   BOOL updateRegionExists = WinQueryUpdateRect (wnd->getOS2WindowHandle(), pRect ? &rectl : NULL);
   if (!pRect) {
      return (updateRegionExists);
   }

   if (updateRegionExists)
   {
      //SvL: Hack for memory.exe (doesn't get repainted properly otherwise)
//      if (wnd->isOwnDC() && wnd->getOwnDC())
      if (wnd->isOwnDC())
      {
         pDCData pHps = NULL;
         pHps = (pDCData)GpiQueryDCData(wnd->getOwnDC());
         if (!pHps)
         {
            O32_SetLastError (ERROR_INVALID_HANDLE);
            return FALSE;
         }
         GpiConvert (pHps->hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rectl);
      }
      else
      {
         long height   = wnd->getClientHeight();
         rectl.yTop    = height - rectl.yTop;
         rectl.yBottom = height - rectl.yBottom;
      }

      if (pRect)
         WINRECT_FROM_PMRECT (*pRect, rectl);

      if (erase)
         sendEraseBkgnd (wnd);
   }
   else
   {
      if (pRect)
         pRect->left = pRect->top = pRect->right = pRect->bottom = 0;
   }

   return updateRegionExists;
}

int WIN32API GetUpdateRgn (HWND hwnd, HRGN hrgn, BOOL erase)
{
   LONG Complexity;

   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if (!wnd)
   {
      O32_SetLastError (ERROR_INVALID_HANDLE);
      return ERROR_W;
   }

   Complexity = O32_GetUpdateRgn (wnd->getOS2WindowHandle(), hrgn, FALSE);
   if (erase && (Complexity > NULLREGION_W)) sendEraseBkgnd (wnd);

   return Complexity;
}

// This implementation of GetDCEx supports
// DCX_WINDOW
// DCX_CACHE
// DCX_EXCLUDERGN (complex regions allowed)
// DCX_INTERSECTRGN (complex regions allowed)

HDC WIN32API GetDCEx (HWND hwnd, HRGN hrgn, ULONG flags)
{
   Win32BaseWindow *wnd = NULL;
   HWND     hWindow;
   BOOL     success;
   pDCData  pHps = NULL;
   HPS      hps  = NULLHANDLE;
   BOOL     drawingAllowed = TRUE;
   BOOL     isWindowOwnDC;
   BOOL     creatingOwnDC = FALSE;
   PS_Type  psType;

   if (hwnd)
   {
        wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(wnd == NULL) {
                dprintf (("ERROR: User32: GetDCEx bad window handle %X!!!!!", hwnd));
                O32_SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
                return 0;
        }
//SvL: Experimental change (doesn't work right)
#if 0
        if(wnd->fHasParentDC() && wnd->getParent()) {
                wnd = wnd->getParent();
        }
#endif
        if (flags & DCX_WINDOW_W)
                hWindow = wnd->getOS2FrameWindowHandle();
        else
                hWindow = wnd->getOS2WindowHandle();
   }
   else
      hWindow = HWND_DESKTOP;

   dprintf (("User32: GetDCEx hwnd %x (%x %x) -> wnd %x", hwnd, hrgn, flags, wnd));

   //SvL: Hack for memory.exe (doesn't get repainted properly otherwise)
//   isWindowOwnDC = (((hWindow == HWND_DESKTOP) ? FALSE : (wnd->isOwnDC() && wnd->getOwnDC()))
   isWindowOwnDC = (((hWindow == HWND_DESKTOP) ? FALSE : (wnd->isOwnDC()))
                 && !(flags & DCX_CACHE_W));

   if (isWindowOwnDC)
   {
      hps = wnd->getOwnDC();
      if (hps)
      {
         pDCData pHps = (pDCData)GpiQueryDCData (hps);
         if (!pHps)
            goto error;

         setPageXForm (wnd, pHps);

         pHps->hdcType = TYPE_1;
         return (HDC)hps;
      }
      else
         creatingOwnDC = TRUE;
   }

   if (isWindowOwnDC)
   {
      SIZEL sizel = {0,0};
      hps = GpiCreatePS (WinQueryAnchorBlock (hWindow),
                         WinOpenWindowDC (hWindow),
                         &sizel, PU_PELS | GPIT_MICRO | GPIA_ASSOC );
      psType = MICRO;
   }
   else
   {
      if (hWindow == HWND_DESKTOP)
         hps = WinGetScreenPS (hWindow);
      else
         hps = WinGetPS (hWindow);

      psType = MICRO_CACHED;
   }

   if (!hps)
      goto error;

   HPSToHDC (hWindow, hps, NULL, NULL);
   pHps = (pDCData)GpiQueryDCData (hps);

   if ((flags & DCX_EXCLUDERGN_W) || (flags & DCX_INTERSECTRGN_W))
   {
      ULONG BytesNeeded;
      PRGNDATA_W RgnData;
      PRECTL pr;
      int i;
      LONG height = OSLibQueryScreenHeight();

      if (!hrgn)
         goto error;

      BytesNeeded = O32_GetRegionData (hrgn, 0, NULL);
      RgnData = (PRGNDATA_W)_alloca (BytesNeeded);
      if (RgnData == NULL)
          goto error;
      O32_GetRegionData (hrgn, BytesNeeded, RgnData);

      i = RgnData->rdh.nCount;
      pr = (PRECTL)(RgnData->Buffer);

      success = TRUE;
      if (flags & DCX_EXCLUDERGN_W)
         for (; (i > 0) && success; i--, pr++) {
            LONG y = pr->yBottom;

            pr->yBottom = height - pr->yTop;
            pr->yTop    = height - y;
            success &= GpiExcludeClipRectangle (pHps->hps, pr);
         }
      else
         for (; (i > 0) && success; i--, pr++) {
            LONG y = pr->yBottom;

            pr->yBottom = height - pr->yTop;
            pr->yTop    = height - y;
            success &= GpiIntersectClipRectangle (pHps->hps, pr);
         }
      if (!success)
         goto error;
   }

   if (creatingOwnDC)
      wnd->setOwnDC ((HDC)hps);

   pHps->psType  = psType;
   pHps->hdcType = TYPE_1;
   GpiSetDrawControl (hps, DCTL_DISPLAY, drawingAllowed ? DCTL_ON : DCTL_OFF);

   return (HDC)pHps->hps;

error:
   /* Something went wrong; clean up
    */
   if (pHps)
   {
      if (pHps->hps)
      {
         if(pHps->psType == MICRO_CACHED)
            WinReleasePS(pHps->hps);
         else
            GpiDestroyPS(pHps->hps);
      }

      if (pHps->hdc)     DevCloseDC(pHps->hdc);
      if (pHps->hrgnHDC) GpiDestroyRegion(pHps->hps, pHps->hrgnHDC);

      O32_DeleteObject (pHps->nullBitmapHandle);
   }
   O32_SetLastError (ERROR_INVALID_PARAMETER);
   return NULL;
}

HDC WIN32API GetDC (HWND hwnd)
{
  return GetDCEx (hwnd, NULL, 0);
}

HDC WIN32API GetWindowDC (HWND hwnd)
{
  return GetDCEx (hwnd, NULL, DCX_WINDOW_W);
}

int WIN32API ReleaseDC (HWND hwnd, HDC hdc)
{
   BOOL isOwnDC = FALSE;
   int rc;

   if (hwnd)
   {
      Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
      //SvL: Hack for memory.exe (doesn't get repainted properly otherwise)
//      isOwnDC = wnd->isOwnDC() && wnd->getOwnDC();
      isOwnDC = wnd->isOwnDC();
   }
   if (isOwnDC)
      rc = TRUE;
   else
      rc = O32_ReleaseDC (0, hdc);

   dprintf(("ReleaseDC %x %x", hwnd, hdc));
   return (rc);
}

BOOL WIN32API UpdateWindow (HWND hwnd)
{
   if (!hwnd)
       return FALSE;

   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);

   dprintf (("User32: UpdateWindow hwnd %x -> wnd %x", hwnd, wnd));

#if 0
   if (WinQueryUpdateRect (wnd->getOS2WindowHandle(), NULL))
       sendEraseBkgnd (wnd);
#endif

#if 1
   WinUpdateWindow(wnd->getOS2FrameWindowHandle());
#else
   wnd->MsgPaint(0);
#endif

   return (TRUE);
}

// This implementation of RedrawWindow supports
// RDW_ERASE
// RDW_NOERASE
// RDW_INTERNALPAINT
// RDW_NOINTERNALPAINT
// RDW_INVALIDATE
// RDW_VALIDATE
// RDW_ERASENOW
// RDW_UPDATENOW

BOOL WIN32API RedrawWindow(HWND hwnd, const RECT* pRect, HRGN hrgn, DWORD redraw)
{
   Win32BaseWindow *wnd;

   if (redraw & (RDW_FRAME_W | RDW_NOFRAME_W))
   {
      O32_SetLastError (ERROR_NOT_SUPPORTED);
      return FALSE;
   }

   if (hwnd == NULLHANDLE)
   {
#if 1
        // Don't do this for now (causes lots of desktop repaints in WordPad)
         O32_SetLastError (ERROR_INVALID_PARAMETER);
         return FALSE;
#else
      hwnd = HWND_DESKTOP;
      wnd  = Win32BaseWindow::GetWindowFromOS2Handle(OSLIB_HWND_DESKTOP);

      if (!wnd)
      {
         dprintf(("USER32:dc: RedrawWindow can't find desktop window %08xh\n",
                  hwnd));
         O32_SetLastError (ERROR_INVALID_PARAMETER);
         return FALSE;
      }
#endif
   }
   else
   {
      wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);

      if (!wnd)
      {
         dprintf(("USER32:dc: RedrawWindow can't find window %08xh\n",
                  hwnd));
         O32_SetLastError (ERROR_INVALID_PARAMETER);
         return FALSE;
      }
      hwnd = wnd->getOS2WindowHandle();
   }

   BOOL  IncludeChildren = redraw & RDW_ALLCHILDREN_W ? TRUE : FALSE;
   BOOL  success = TRUE;
   HPS   hpsTemp = NULLHANDLE;
   HRGN  hrgnTemp = NULLHANDLE;
   RECTL rectl;

   if (redraw & RDW_UPDATENOW_W) redraw &= ~RDW_ERASENOW_W;

   if (redraw & RDW_NOERASE_W)
      wnd->setEraseBkgnd (FALSE);

//SvL: Test
//   if (redraw & RDW_UPDATENOW_W)
//      wnd->setSuppressErase (FALSE);
//   else if (redraw & RDW_ERASENOW_W)
//      wnd->setSuppressErase (FALSE);
#if 0
   else
   {
      QMSG qmsg;
      BOOL erase;

      erase = (WinPeekMsg (HABX, &qmsg, hwnd, WM_PAINT, WM_PAINT, PM_REMOVE)
                && (redraw & RDW_NOERASE_W) == 0);

      wnd->setSuppressErase (!erase);
   }

   if (redraw & (RDW_NOINTERNALPAINT_W | RDW_INTERNALPAINT_W))
   {
      QMSG qmsg;

      WinPeekMsg( (HAB)0, &qmsg, hwnd, WM_VIRTUAL_INTERNALPAINT,
                  WM_VIRTUAL_INTERNALPAINT, PM_REMOVE );
   }
#endif

   if (hrgn)
   {
      ULONG BytesNeeded;
      PRGNDATA_W RgnData;
      PRECTL pr;
      int i;
      LONG height = wnd ? wnd->getClientHeight() : OSLibQueryScreenHeight();

      if (!hrgn)
         goto error;

      BytesNeeded = O32_GetRegionData (hrgn, 0, NULL);
      RgnData = (PRGNDATA_W)_alloca (BytesNeeded);
      if (RgnData == NULL)
          goto error;
      O32_GetRegionData (hrgn, BytesNeeded, RgnData);

      pr = (PRECTL)(RgnData->Buffer);
      for (i = RgnData->rdh.nCount; i > 0; i--, pr++) {
         LONG temp = pr->yTop;
         pr->yTop = height - pr->yBottom;
         pr->yBottom = height - temp;
      }

      hpsTemp = WinGetScreenPS (HWND_DESKTOP);
      hrgnTemp = GpiCreateRegion (hpsTemp, RgnData->rdh.nCount, (PRECTL)(RgnData->Buffer));
      if (!hrgnTemp) goto error;
   }
   else if (pRect)
   {
      LONG height = wnd ? wnd->getClientHeight() : OSLibQueryScreenHeight();

      PMRECT_FROM_WINRECT (rectl, *pRect);
      rectl.yTop    = height - rectl.yTop;
      rectl.yBottom = height - rectl.yBottom;
   }

   if (redraw & RDW_INVALIDATE_W)
   {
      if (redraw & RDW_ERASE_W)
         wnd->setEraseBkgnd (TRUE, TRUE);

      if (!pRect && !hrgn)
         success = WinInvalidateRect (hwnd, NULL, IncludeChildren);
      else if (hrgn)
         success = WinInvalidateRegion (hwnd, hrgnTemp, IncludeChildren);
      else
         success = WinInvalidateRect (hwnd, &rectl, IncludeChildren);
      if (!success) goto error;
   }
   else if (redraw & RDW_VALIDATE_W)
   {
      if (WinQueryUpdateRect (hwnd, NULL))
      {
         if (!pRect && !hrgn)
            success = WinValidateRect (hwnd, NULL, IncludeChildren);
         else if (hrgn)
            success = WinValidateRegion (hwnd, hrgnTemp, IncludeChildren);
         else
            success = WinValidateRect (hwnd, &rectl, IncludeChildren);
         if (!success) goto error;
      }
   }

   if (WinQueryUpdateRect (hwnd, NULL))
   {
      if (redraw & RDW_UPDATENOW_W)
         wnd->MsgPaint (0, FALSE);

      else if ((redraw & RDW_ERASE_W) && (redraw & RDW_ERASENOW_W))
         wnd->setEraseBkgnd (FALSE, !sendEraseBkgnd (wnd));
   }
   else if ((redraw & RDW_INTERNALPAINT_W) && !(redraw & RDW_INVALIDATE_W))
   {
      if (redraw & RDW_UPDATENOW_W)
         wnd->MsgPaint (0, FALSE);
//      else
//         WinPostMsg( hwnd, WM_VIRTUAL_INTERNALPAINT, MPVOID, MPVOID );
   }

error:
   /* clean up */
   if (hrgnTemp)
      GpiDestroyRegion (hpsTemp, hrgnTemp);

   if (hpsTemp)
      WinReleasePS (hpsTemp);

//SvL: Test
//   if ((redraw & RDW_INVALIDATE_W) == 0)
//      wnd->setSuppressErase (FALSE);
//   else if ((redraw & RDW_ERASENOW_W) == RDW_ERASENOW_W)
//      wnd->setSuppressErase (TRUE);

   if (!success)
      O32_SetLastError (ERROR_INVALID_PARAMETER);

   return (success);
}

BOOL WIN32API InvalidateRect (HWND hwnd, const RECT *pRect, BOOL erase)
{
//   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
   BOOL result;

// todo !!
//   if ( isFrame without client )
//      erase = TRUE;

   result = RedrawWindow (hwnd, pRect, NULLHANDLE,
                          RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                          (erase ? RDW_ERASE_W : 0) |
                          (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
   return (result);
}

BOOL WIN32API InvalidateRgn (HWND hwnd, HRGN hrgn, BOOL erase)
{
//   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
   BOOL result;

// todo !!
//   if ( isFrame without client )
//      erase = TRUE;

   result = RedrawWindow (hwnd, NULL, hrgn,
                          RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                          (erase ? RDW_ERASE_W : 0) |
                          (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
   return (result);
}

BOOL setPMRgnIntoWinRgn (HRGN hrgnPM, HRGN hrgnWin, LONG height)
{
   BOOL    rc;
   HPS     hps = WinGetScreenPS (HWND_DESKTOP);
   RGNRECT rgnRect;
   rgnRect.ircStart    = 1;
   rgnRect.crc         = 0;
   rgnRect.ulDirection = RECTDIR_LFRT_TOPBOT;     // doesn't make a difference because we're getting them all

   rc = GpiQueryRegionRects (hps, hrgnPM, NULL, &rgnRect, NULL);

   if (rc && (rgnRect.crcReturned > 0))
   {
      PRECTL Rcls = new RECTL[rgnRect.crcReturned];
      PRECTL pRcl = Rcls;

      if (Rcls != NULL)
      {
         rgnRect.crc = rgnRect.crcReturned;
         rc = GpiQueryRegionRects (hps, hrgnPM, NULL, &rgnRect, Rcls);

         rc = O32_SetRectRgn (hrgnWin, pRcl->xLeft,
                                        pRcl->xRight,
                                        height - pRcl->yTop,
                                        height - pRcl->yBottom);

         if (rgnRect.crcReturned > 1)
         {
            int i;
            HRGN temp;
            temp = O32_CreateRectRgn (0, 0, 1, 1);

            for (i = 1, pRcl++; rc && (i < rgnRect.crcReturned); i++, pRcl++)
            {
              rc = O32_SetRectRgn (temp, pRcl->xLeft,
                                          pRcl->xRight,
                                          height - pRcl->yTop,
                                          height - pRcl->yBottom);
              rc &= O32_CombineRgn (hrgnWin, hrgnWin, temp, RGN_OR_W);
            }
            O32_DeleteObject (temp);
         }
         delete[] Rcls;
      }
      else
      {
         rc = FALSE;
      }
   }
   else
   {
      rc = O32_SetRectRgn (hrgnWin, 0, 0, 0, 0);
   }

   WinReleasePS (hps);
   return (rc);
}

BOOL WIN32API ScrollDC (HDC hDC, int dx, int dy, const RECT *pScroll,
                        const RECT *pClip, HRGN hrgnUpdate, LPRECT pRectUpdate)
{
   BOOL rc = TRUE;

   dprintf (("USER32:  ScrollDC"));

   if (!hDC)
   {
      return (FALSE);
   }

   pDCData pHps         = (pDCData)GpiQueryDCData ((HPS)hDC);
   HWND    hwnd         = pHps->hwnd;
   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);

   if ((hwnd == NULLHANDLE) || !wnd)
   {
      return (FALSE);
   }

   POINTL ptl[2] = { 0, 0, dx, dy };

   GpiConvert (pHps->hps, CVTC_WORLD, CVTC_DEVICE, 2, ptl);
   dx = (int)(ptl[1].x - ptl[0].x);
   dy = (int)(ptl[1].y - ptl[0].y);

   RECTL scrollRect;
   RECTL clipRect;

   if (pClip)
   {
      clipRect.xLeft   = min (pClip->left, pClip->right);
      clipRect.xRight  = max (pClip->left, pClip->right);
      clipRect.yTop    = max (pClip->top,  pClip->bottom);
      clipRect.yBottom = min (pClip->top,  pClip->bottom);

      if ((pHps->graphicsMode == GM_COMPATIBLE_W) &&
          (clipRect.xLeft   != clipRect.xRight)   &&
          (clipRect.yBottom != clipRect.yTop))
      {
          if (abs((int)pHps->viewportXExt) <= abs((int)pHps->windowExt.cx))
              clipRect.xRight -= abs(pHps->worldXDeltaFor1Pixel);
          else
              clipRect.xLeft  += abs(pHps->worldXDeltaFor1Pixel);

          if (abs((int)pHps->viewportYExt) <= abs((int)pHps->windowExt.cy))
              clipRect.yTop    -= abs(pHps->worldYDeltaFor1Pixel);
          else
              clipRect.yBottom += abs(pHps->worldYDeltaFor1Pixel);
      }
      GpiConvert (pHps->hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&clipRect);
      if (clipRect.xRight < clipRect.xLeft) {
         ULONG temp      = clipRect.xLeft;
         clipRect.xLeft  = clipRect.xRight;
         clipRect.xRight = temp;
      }
      if (clipRect.yTop < clipRect.yBottom) {
         ULONG temp       = clipRect.yBottom;
         clipRect.yBottom = clipRect.yTop;
         clipRect.yTop    = temp;
      }
   }

   if (pScroll)
   {
      scrollRect.xLeft   = min (pScroll->left, pScroll->right);
      scrollRect.xRight  = max (pScroll->left, pScroll->right);
      scrollRect.yTop    = max (pScroll->top,  pScroll->bottom);
      scrollRect.yBottom = min (pScroll->top,  pScroll->bottom);

      if ((pHps->graphicsMode == GM_COMPATIBLE_W) &&
          (scrollRect.xLeft   != scrollRect.xRight)   &&
          (scrollRect.yBottom != scrollRect.yTop))
      {
          if (abs((int)pHps->viewportXExt) <= abs((int)pHps->windowExt.cx))
              scrollRect.xRight -= abs(pHps->worldXDeltaFor1Pixel);
          else
              scrollRect.xLeft  += abs(pHps->worldXDeltaFor1Pixel);

          if (abs((int)pHps->viewportYExt) <= abs((int)pHps->windowExt.cy))
              scrollRect.yTop    -= abs(pHps->worldYDeltaFor1Pixel);
          else
              scrollRect.yBottom += abs(pHps->worldYDeltaFor1Pixel);
      }
      GpiConvert (pHps->hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&scrollRect);
      if (scrollRect.xRight < scrollRect.xLeft) {
         ULONG temp        = scrollRect.xLeft;
         scrollRect.xLeft  = scrollRect.xRight;
         scrollRect.xRight = temp;
      }
      if (scrollRect.yTop < scrollRect.yBottom) {
         ULONG temp         = scrollRect.yBottom;
         scrollRect.yBottom = scrollRect.yTop;
         scrollRect.yTop    = temp;
      }
   }
   RECTL rectlUpdate;
   HRGN  hrgn;

   LONG lComplexity = WinScrollWindow (hwnd, dx, dy, (pScroll) ? &scrollRect : NULL, (pClip) ? &clipRect : NULL, hrgn, &rectlUpdate, 0);
   if (lComplexity == RGN_ERROR)
   {
      return (FALSE);
   }

   RECT winRectUpdate;
   LONG height = wnd->getClientHeight();

   winRectUpdate.left   = rectlUpdate.xLeft;
   winRectUpdate.right  = rectlUpdate.xRight;
   winRectUpdate.top    = height - rectlUpdate.yTop;
   winRectUpdate.bottom = height - rectlUpdate.yBottom;

   if (pRectUpdate)
      *pRectUpdate = winRectUpdate;

   if (hrgnUpdate)
      rc = setPMRgnIntoWinRgn (hrgn, hrgnUpdate, height);

   return (rc);
}

//******************************************************************************
//******************************************************************************
BOOL WIN32API ScrollWindow(HWND hwnd, int dx, int dy, const RECT *pScroll, const RECT *pClip)
{
 Win32BaseWindow *window;
 APIRET  rc;
 RECTL   clientRect;
 RECTL   scrollRect;
 RECTL   clipRect;
 PRECTL  pScrollRect = NULL;
 PRECTL  pClipRect   = NULL;
 ULONG   scrollFlags = SW_INVALIDATERGN;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("ScrollWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("ScrollWindow %x %d %d %x %x", hwnd, dx, dy, pScroll, pClip));
    mapWin32ToOS2Rect(window,window->getClientRectPtr(),(PRECTLOS2)&clientRect);
    //Rectangle could be relative to parent window, so fix this
    if(clientRect.yBottom != 0) {
        clientRect.yTop   -= clientRect.yBottom;
        clientRect.yBottom = 0;
    }
    if(clientRect.xLeft != 0) {
        clientRect.xRight -= clientRect.xLeft;
        clientRect.xLeft   = 0;
    }
    if(pScroll) {
         mapWin32ToOS2Rect(window,(RECT *)pScroll, (PRECTLOS2)&scrollRect);
         pScrollRect = &scrollRect;

         //Scroll rectangle relative to client area
         pScrollRect->xLeft   += clientRect.xLeft;
         pScrollRect->xRight  += clientRect.xLeft;
         pScrollRect->yTop    += clientRect.yBottom;
         pScrollRect->yBottom += clientRect.yBottom;
         WinIntersectRect ((HAB) 0, pScrollRect, pScrollRect, &clientRect);
    }
    else scrollFlags |= SW_SCROLLCHILDREN;

    if(pClip) {
         mapWin32ToOS2Rect(window,(RECT *)pClip, (PRECTLOS2)&clipRect);
         pClipRect = &clipRect;

         //Clip rectangle relative to client area
         pClipRect->xLeft     += clientRect.xLeft;
         pClipRect->xRight    += clientRect.xLeft;
         pClipRect->yTop      += clientRect.yBottom;
         pClipRect->yBottom   += clientRect.yBottom;
         WinIntersectRect ((HAB) 0, pClipRect, pClipRect, &clientRect);
    }

    dy = revertDy (window, dy);

    rc = WinScrollWindow(window->getOS2WindowHandle(), dx, dy,
                         pScrollRect, pClipRect, NULLHANDLE,
                         NULL, scrollFlags);

    return (rc != RGN_ERROR);
}
//******************************************************************************
//******************************************************************************
INT WIN32API ScrollWindowEx(HWND hwnd, int dx, int dy, const RECT *pScroll, const RECT *pClip,
                            HRGN hrgnUpdate, PRECT pRectUpdate, UINT scrollFlag)
{
    Win32BaseWindow *window;
    APIRET  rc;
    RECTL   scrollRect;
    RECTL   clipRect;
    ULONG   scrollFlags = 0;
    int     regionType = ERROR_W;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("ScrollWindowEx, window %x not found", hwnd));
        return 0;
    }

    dprintf(("ScrollWindowEx %x %d %d %x %x", hwnd, dx, dy, pScroll, pClip));

    dy = revertDy (window, dy);

    if (scrollFlag & SW_INVALIDATE_W)      scrollFlags |= SW_INVALIDATERGN;
    if (scrollFlag & SW_SCROLLCHILDREN_W)  scrollFlags |= SW_SCROLLCHILDREN;

    if(pScroll) mapWin32ToOS2Rect(window,(RECT *)pScroll, (PRECTLOS2)&scrollRect);
    if(pClip)   mapWin32ToOS2Rect(window,(RECT *)pClip, (PRECTLOS2)&clipRect);

    RECTL rectlUpdate;
    HRGN  hrgn;

    if (scrollFlag & SW_SMOOTHSCROLL_W)
    {
      INT time = (scrollFlag >> 16) & 0xFFFF;

      //CB: todo, scroll in several steps
      //    is time in ms? time <-> iteration count?
    }

    LONG lComplexity = WinScrollWindow (window->getOS2WindowHandle(), dx, dy,
                                        (pScroll) ? &scrollRect : NULL,
                                        (pClip) ? &clipRect : NULL,
                                        hrgn, &rectlUpdate, scrollFlags);
    if (lComplexity == RGN_ERROR)
    {
        return (0);
    }

    RECT winRectUpdate;
    LONG height = window->getClientHeight();

    winRectUpdate.left   = rectlUpdate.xLeft;
    winRectUpdate.right  = rectlUpdate.xRight;
    winRectUpdate.top    = height - rectlUpdate.yTop;
    winRectUpdate.bottom = height - rectlUpdate.yBottom;

    if (pRectUpdate)
       *pRectUpdate = winRectUpdate;

    if (hrgnUpdate)
       rc = setPMRgnIntoWinRgn (hrgn, hrgnUpdate, height);

    if ((scrollFlag & SW_INVALIDATE_W) &&
        ((lComplexity == RGN_RECT) || (lComplexity == RGN_COMPLEX)))
    {
       rc = InvalidateRect (hwnd, &winRectUpdate, scrollFlag & SW_ERASE_W);
       if (rc == FALSE)
       {
          return (0);
       }
    }

    switch (lComplexity)
    {
       case RGN_NULL:
         regionType = NULLREGION_W;
         break;
       case RGN_RECT:
         regionType = SIMPLEREGION_W;
         break;
       case RGN_COMPLEX:
         regionType = COMPLEXREGION_W;
         break;
       default:
         regionType = ERROR_W;
         break;
    }

    return (regionType);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API WindowFromDC(HDC hdc)
{
   pDCData pHps = (pDCData)GpiQueryDCData( (HPS)hdc );

   dprintf2(("USER32:  WindowFromDC %x", hdc));
   if ( pHps )
      return Win32BaseWindow::OS2ToWin32Handle(pHps->hwnd);
   else
      return 0;
}
//******************************************************************************
//******************************************************************************
INT WIN32API ExcludeUpdateRgn( HDC hDC, HWND  hWnd)
{
    dprintf(("USER32:  ExcludeUpdateRgn\n"));
    hWnd = Win32BaseWindow::Win32ToOS2Handle(hWnd);

    return O32_ExcludeUpdateRgn(hDC,hWnd);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ValidateRect( HWND hwnd, const RECT * lprc)
{
    if(lprc) {
         dprintf(("USER32: ValidateRect %x (%d,%d)(%d,%d)", hwnd, lprc->left, lprc->top, lprc->right, lprc->bottom));
    }
    else dprintf(("USER32: ValidateRect %x", hwnd));

    return RedrawWindow( hwnd, lprc, 0, RDW_VALIDATE_W | RDW_NOCHILDREN_W | (hwnd==0 ? RDW_UPDATENOW_W : 0));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ValidateRgn( HWND hwnd, HRGN  hrgn)
{
    dprintf(("USER32: ValidateRgn %x %x", hwnd, hrgn));
    return RedrawWindow( hwnd, NULL, hrgn, RDW_VALIDATE_W | RDW_NOCHILDREN_W | (hwnd==0 ? RDW_UPDATENOW_W : 0));
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
  //Attention: Win32 hwnd handle!

  return (NULLREGION_W);
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
  //Attention: Win32 hwnd handle!

  return (0);
}
//******************************************************************************
//******************************************************************************
