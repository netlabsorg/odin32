/* $Id: dc.cpp,v 1.105 2001-05-23 08:35:57 sandervl Exp $ */

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

#include <string.h>
#include <win32type.h>
#include <win32api.h>
#include <winuser32.h>
#include <winconst.h>
#include <misc.h>
#include <win32wbase.h>
#include <math.h>
#include <limits.h>
#include "oslibwin.h"
#include "oslibmsg.h"
#include <dcdata.h>
#include <codepage.h>
#include <wingdi32.h>

#define INCLUDED_BY_DC
#include "dc.h"

#define DBG_LOCALLOG    DBG_dc
#include "dbglocal.h"

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

#ifdef DEBUG
#define dprintfRegion(a,b,c) if(DbgEnabledLvl2[DBG_LOCALLOG] == 1) dprintfRegion1(a,b,c)

void dprintfRegion1(HPS hps, HWND hWnd, HRGN hrgnClip)
{
 RGNRECT rgnRect = {0, 16, 0, RECTDIR_LFRT_TOPBOT};
 RECTL   rectRegion[16];
 APIRET  rc;

   dprintf(("dprintfRegion %x %x", hWnd, hps));
   rc = GpiQueryRegionRects(hps, hrgnClip, NULL, &rgnRect, &rectRegion[0]);
   for(int i=0;i<rgnRect.crcReturned;i++) {
        dprintf(("(%d,%d)(%d,%d)", rectRegion[i].xLeft, rectRegion[i].yBottom, rectRegion[i].xRight, rectRegion[i].yTop));
   }
}
#else
#define dprintfRegion(a,b,c)
#endif

//******************************************************************************
//******************************************************************************
void WIN32API TestWideLine (pDCData pHps)
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
//******************************************************************************
//******************************************************************************
void WIN32API Calculate1PixelDelta(pDCData pHps)
{
   POINTL aptl[2] = {0, 0, 1, 1};

   GpiConvert(pHps->hps, CVTC_DEVICE, CVTC_WORLD, 2, aptl);
   pHps->worldYDeltaFor1Pixel = (int)(aptl[1].y - aptl[0].y);
   pHps->worldXDeltaFor1Pixel = (int)(aptl[1].x - aptl[0].x);                   // 171182
}
//******************************************************************************
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
         SetLastError(ERROR_INVALID_PARAMETER_W);
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
         SetLastError(ERROR_INVALID_PARAMETER_W);
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
//******************************************************************************
//******************************************************************************
BOOL setPageXForm(Win32BaseWindow *wnd, pDCData pHps)
{
   MATRIXLF mlf;
   BOOL rc = TRUE;

   pHps->height = clientHeight(wnd, 0, pHps) - 1;

   double xScale =  pHps->viewportXExt / (double)pHps->windowExt.cx;
   double yScale =  pHps->viewportYExt / (double)pHps->windowExt.cy;

#if 0
   mlf.fxM11 = FLOAT_TO_FIXED(1.0);
   mlf.fxM12 = 0;
   mlf.lM13  = 0;
   mlf.fxM21 = 0;
   mlf.fxM22 = FLOAT_TO_FIXED(1.0);
   mlf.lM23  = 0;
   mlf.lM31  = pHps->viewportOrg.x - (LONG)(pHps->windowOrg.x * xScale);
   mlf.lM32  = pHps->viewportOrg.y - (LONG)(pHps->windowOrg.y * yScale);
   mlf.lM33  = 1;

//testestest
   if(wnd && wnd->getWindowWidth() && wnd->getWindowHeight())
   {
        RECTL recttmp, rectviewold;

        rc = GpiQueryPageViewport(pHps->hps, &rectviewold);
        if(rc == 0) {
            dprintf(("WARNING: GpiQueryPageViewport returned FALSE!!"));
        }
        recttmp.xLeft   = 0;
        recttmp.xRight  = ((double)GetScreenWidth() / xScale);
        recttmp.yBottom = 0;
        recttmp.yTop    = ((double)GetScreenHeight() / yScale);

        if(recttmp.yTop != rectviewold.yTop ||
           recttmp.xRight != rectviewold.xRight)
        {
            if(pHps->viewportYExt > pHps->windowExt.cy) {//scaling up means we have to invert the y values
                recttmp.yBottom = GetScreenHeight() - recttmp.yTop;
                recttmp.yTop    = GetScreenHeight();
            }
            dprintf(("GpiSetPageViewport %x (%d,%d)(%d,%d) %f %f", pHps->hps, recttmp.xLeft, recttmp.yBottom, recttmp.xRight, recttmp.yTop, xScale, yScale));
            rc = GpiSetPageViewport(pHps->hps, &recttmp);
            if(rc == 0) {
                dprintf(("WARNING: GpiSetPageViewport returned FALSE!!"));
            }
        }
   }
//testestest

#else
   mlf.fxM11 = FLOAT_TO_FIXED(xScale);
   mlf.fxM12 = 0;
   mlf.lM13  = 0;
   mlf.fxM21 = 0;
   mlf.fxM22 = FLOAT_TO_FIXED(yScale);
   mlf.lM23  = 0;
   mlf.lM31  = pHps->viewportOrg.x - (LONG)(pHps->windowOrg.x * xScale);
   mlf.lM32  = pHps->viewportOrg.y - (LONG)(pHps->windowOrg.y * yScale);
   mlf.lM33  = 1;
#endif

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
   if(rc == 0) {
      dprintf(("WARNING: GpiSetDefaultViewMatrix returned FALSE!!"));
   }
#ifdef INVERT
   if (bEnableYInversion)
      GpiEnableYInversion(pHps->hps, pHps->height + pHps->HPStoHDCInversionHeight);
   else
      GpiEnableYInversion(pHps->hps, 0);
#else
   pHps->yInvert = 0;
   if(bEnableYInversion)
   {
      pHps->yInvert4Enable = pHps->height + pHps->HPStoHDCInversionHeight;
      if(pHps->isPrinter)
      {
         pHps->yInvert = pHps->yInvert4Enable
                       * (pHps->windowExt.cy / (double)pHps->viewportYExt);
      } else {
         pHps->yInvert = pHps->yInvert4Enable;
      }
   }
#endif

   TestWideLine(pHps);
   Calculate1PixelDelta(pHps);
   return rc;
}
//******************************************************************************
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
VOID removeClientArea(Win32BaseWindow *window, pDCData pHps)
{
   pHps->isClient = FALSE;
   pHps->isClientArea = FALSE;
}
//******************************************************************************
//******************************************************************************
void selectClientArea(Win32BaseWindow *window, pDCData pHps)
{
   pHps->isClient = TRUE;
   pHps->isClientArea = TRUE;
}
//******************************************************************************
//******************************************************************************
void selectClientArea(Win32BaseWindow *wnd, HDC hdc)
{
  pDCData pHps = (pDCData)GpiQueryDCData (wnd->getOwnDC());

  if (pHps != NULLHANDLE)
      selectClientArea(wnd, pHps);
}
//******************************************************************************
//******************************************************************************
LONG clientHeight(Win32BaseWindow *wnd, HWND hwnd, pDCData pHps)
{
    if ((hwnd == 0) && (pHps != 0))
        hwnd = pHps->hwnd;

    if ((hwnd != 0) || (pHps == 0))
    {
        if(wnd) {
            if(pHps && !pHps->isClientArea) {
                return (wnd->getWindowHeight());
            }
            else return (wnd->getClientHeight());
        }
        else return OSLibQueryScreenHeight();
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
//******************************************************************************
//******************************************************************************
BOOL WIN32API changePageXForm(pDCData pHps, PPOINTL pValue, int x, int y, PPOINTL pPrev)
{
 Win32BaseWindow *wnd;

   if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(pHps->hwnd);
   }
   else wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(pHps->hwnd);
   return changePageXForm(wnd, pHps, pValue, x, y, pPrev);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API setPageXForm(pDCData pHps)
{
 Win32BaseWindow *wnd;

   if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(pHps->hwnd);
   }
   else wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(pHps->hwnd);
   return setPageXForm(wnd, pHps);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API removeClientArea(pDCData pHps)
{
 Win32BaseWindow *wnd;

   if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(pHps->hwnd);
   }
   else wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(pHps->hwnd);
   if(wnd) {
      removeClientArea(wnd, pHps);
   }
}
//******************************************************************************
//******************************************************************************
VOID WIN32API selectClientArea(pDCData pHps)
{
 Win32BaseWindow *wnd;

   if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(pHps->hwnd);
   }
   else wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(pHps->hwnd);
   if(wnd) {
      selectClientArea(wnd, pHps);
   }
}
//******************************************************************************
//******************************************************************************
VOID WIN32API checkOrigin(pDCData pHps)
{
 Win32BaseWindow *wnd;

   if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(pHps->hwnd);
   }
   else wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(pHps->hwnd);
   if(wnd) {
      if(pHps->isClient)
          selectClientArea(wnd, pHps);
   }
}
//******************************************************************************
//******************************************************************************
LONG WIN32API clientHeight(HWND hwnd, pDCData pHps)
{
 Win32BaseWindow *wnd;

   if(!pHps) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
   }
   else 
   if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(pHps->hwnd);
   }
   else wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(pHps->hwnd);
   return clientHeight(wnd, hwnd, pHps);
}
//******************************************************************************
//******************************************************************************
int WIN32API setMapMode(pDCData pHps, int mode)
{
 Win32BaseWindow *wnd;

   if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle(pHps->hwnd);
   }
   else wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(pHps->hwnd);
   return setMapMode(wnd, pHps, mode);
}
//******************************************************************************
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
void releaseOwnDC (HDC hps)
{
   pDCData pHps = (pDCData)GpiQueryDCData ((HPS)hps);

   dprintf2(("releaseOwnDC %x", hps));

   if (pHps) {
      if (pHps->hrgnHDC)
         GpiDestroyRegion (pHps->hps, pHps->hrgnHDC);

      GpiSetBitmap (pHps->hps, NULL);
      O32_DeleteObject (pHps->nullBitmapHandle);
      GpiDestroyPS(pHps->hps);

      if (pHps->hdc)
         DevCloseDC(pHps->hdc);
   }
}
//******************************************************************************
//******************************************************************************
HDC WIN32API BeginPaint (HWND hWnd, PPAINTSTRUCT_W lpps)
{
 HWND     hwnd = hWnd ? hWnd : HWND_DESKTOP;
 pDCData  pHps = NULLHANDLE;
 HPS      hPS_ownDC = NULLHANDLE, hpsPaint = 0;
 RECTL    rectl = {0, 0, 1, 1};
 HRGN     hrgnOldClip;
 LONG     lComplexity;
 RECTL    rectlClient;
 RECTL    rectlClip;

    if(lpps == NULL) {
        //BeginPaint does NOT change last error in this case
        //(verified in NT4, SP6)
        dprintf (("USER32: BeginPaint %x NULL PAINTSTRUCT pointer", hWnd));
        return 0;
    }
    memset(lpps, 0, sizeof(*lpps));

    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!wnd) {
        dprintf (("USER32: BeginPaint %x %x: invalid window handle!!", hWnd, lpps));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W); //(verified in NT4, SP6)
        return (HDC)0;
    }
    HWND hwndClient = wnd->getOS2WindowHandle();

    if(hwnd != HWND_DESKTOP && wnd->isOwnDC())
    {
        hPS_ownDC = wnd->getOwnDC();
        //SvL: Hack for memory.exe (doesn't get repainted properly otherwise)
        if(hPS_ownDC) {
            pHps = (pDCData)GpiQueryDCData(hPS_ownDC);
            if (!pHps)
            {
                dprintf (("USER32: BeginPaint %x invalid parameter %x", hWnd, lpps));
                SetLastError(ERROR_INVALID_PARAMETER_W);
                return (HDC)NULLHANDLE;
            }
            hpsPaint = hPS_ownDC;
        }
    }
    if(!hpsPaint) {
        hpsPaint = GetDCEx(hwnd, 0, (DCX_CACHE_W|DCX_USESTYLE_W));
        pHps = (pDCData)GpiQueryDCData(hpsPaint);
        if (!pHps)
        {
            dprintf (("USER32: BeginPaint %x invalid parameter %x", hWnd, lpps));
            SetLastError(ERROR_INVALID_PARAMETER_W);
            return (HDC)NULLHANDLE;
        }
    }

    if(WinQueryUpdateRect(hwndClient, &rectl) == FALSE)
    {
        memset(&rectl, 0, sizeof(rectl));
        dprintf (("USER32: WARNING: WinQueryUpdateRect failed (error or no update rectangle)!!"));

        HRGN hrgnClip = GpiCreateRegion(pHps->hps, 1, &rectl);
        GpiSetClipRegion(pHps->hps, hrgnClip, &hrgnOldClip);

        selectClientArea(wnd, pHps);

        //save old clip region (restored for CS_OWNDC windows in EndPaint)
        wnd->SetClipRegion(hrgnOldClip);
        lComplexity = RGN_NULL;
    }
    else {
        rectlClip.yBottom = rectlClip.xLeft = 0;
        rectlClip.yTop = rectlClip.xRight = 1;

        //Query update region
        HRGN hrgnClip = GpiCreateRegion(pHps->hps, 1, &rectlClip);
        WinQueryUpdateRegion(hwndClient, hrgnClip);
        WinValidateRegion(hwndClient, hrgnClip, FALSE);

        dprintfRegion(pHps->hps, wnd->getWindowHandle(), hrgnClip);

#ifdef DEBUG
        //Note: GpiQueryClipBox returns logical points
        lComplexity = GpiQueryClipBox(pHps->hps, &rectlClip);
        dprintf(("ClipBox (%d): (%d,%d)(%d,%d)", lComplexity, rectlClip.xLeft, rectlClip.yBottom, rectlClip.xRight, rectlClip.yTop));
#endif

        //set clip region
        lComplexity = GpiSetClipRegion(pHps->hps, hrgnClip, &hrgnOldClip);

        if(lComplexity == RGN_NULL) {
            dprintf (("BeginPaint %x: EMPTY update rectangle (show=%d/%d vis=%d/%d", hWnd,  WinIsWindowVisible(wnd->getOS2FrameWindowHandle()), WinIsWindowVisible(wnd->getOS2WindowHandle()),  WinIsWindowShowing(wnd->getOS2FrameWindowHandle()), WinIsWindowShowing(wnd->getOS2WindowHandle())));
        }
        else dprintf (("BeginPaint %x: (show=%d/%d vis=%d/%d)", hWnd,  WinIsWindowVisible(wnd->getOS2FrameWindowHandle()), WinIsWindowVisible(wnd->getOS2WindowHandle()),  WinIsWindowShowing(wnd->getOS2FrameWindowHandle()), WinIsWindowShowing(wnd->getOS2WindowHandle())));

        selectClientArea(wnd, pHps);

#ifdef DEBUG
        //Note: GpiQueryClipBox returns logical points
        GpiQueryClipBox(pHps->hps, &rectlClip);
        dprintf(("ClipBox (%d): (%d,%d)(%d,%d)", lComplexity, rectlClip.xLeft, rectlClip.yBottom, rectlClip.xRight, rectlClip.yTop));
#endif
        //save old clip region (restored for CS_OWNDC windows in EndPaint)
        wnd->SetClipRegion(hrgnOldClip);
    }

    if(hPS_ownDC == 0)
        setMapMode (wnd, pHps, MM_TEXT_W);
    else
        setPageXForm(wnd, pHps);

    pHps->hdcType = TYPE_3;

    HideCaret(hwnd);
    WinShowTrackRect(wnd->getOS2WindowHandle(), FALSE);

    if((wnd->needsEraseBkgnd() || wnd->IsVisibleRegionChanged()) && lComplexity != RGN_NULL) {
        wnd->setEraseBkgnd(FALSE);
        wnd->SetVisibleRegionChanged(FALSE);
        lpps->fErase = (wnd->MsgEraseBackGround(pHps->hps) == 0);
    }
    else lpps->fErase = TRUE;

    lpps->hdc    = (HDC)pHps->hps;

    if(lComplexity != RGN_NULL) {
        long height  = wnd->getClientHeight();
        lpps->rcPaint.top    = height - rectl.yTop;
        lpps->rcPaint.left   = rectl.xLeft;
        lpps->rcPaint.bottom = height - rectl.yBottom;
        lpps->rcPaint.right  = rectl.xRight;
        //BeginPaint must return logical points instead of device points.
        //(not the same if e.g. app changes page viewport)
        DPtoLP(lpps->hdc, (LPPOINT)&lpps->rcPaint, 2);
    }
    else {
        lpps->rcPaint.bottom = lpps->rcPaint.top = 0;
        lpps->rcPaint.right = lpps->rcPaint.left = 0;
    }

    SetLastError(0);
    dprintf(("USER32: BeginPaint %x -> hdc %x (%d,%d)(%d,%d)", hWnd, pHps->hps, lpps->rcPaint.left, lpps->rcPaint.top, lpps->rcPaint.right, lpps->rcPaint.bottom));
    return (HDC)pHps->hps;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndPaint (HWND hWnd, const PAINTSTRUCT_W *pPaint)
{
  HWND    hwnd = hWnd ? hWnd : HWND_DESKTOP;
  HRGN    hrgnOld;
  pDCData pHps;

    dprintf (("USER32: EndPaint(%x)", hwnd));

    if (!pPaint || !pPaint->hdc )
        return TRUE;

    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

    if (!wnd) goto exit;

    pHps = (pDCData)GpiQueryDCData((HPS)pPaint->hdc);
    if (pHps && (pHps->hdcType == TYPE_3))
    {
        GpiSetClipRegion(pHps->hps, wnd->GetClipRegion(), &hrgnOld);
        wnd->SetClipRegion(0);
        if(hrgnOld) {
            GpiDestroyRegion(pHps->hps, hrgnOld);
        }
        pHps->hdcType = TYPE_1; //otherwise Open32's ReleaseDC fails
        ReleaseDC(hwnd, pPaint->hdc);
    }
    else {
        dprintf(("EndPaint: wrong hdc %x!!", pPaint->hdc));
    }
    wnd->setEraseBkgnd(TRUE);
    ShowCaret(hwnd);
    WinShowTrackRect(wnd->getOS2WindowHandle(), TRUE);

exit:
    SetLastError(0);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
int WIN32API ReleaseDC (HWND hwnd, HDC hdc)
{
   BOOL isOwnDC = FALSE;
   int rc;

    if (hwnd)
    {
        Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
        if(wnd == NULL) {
            dprintf(("ERROR: ReleaseDC %x %x failed", hwnd, hdc));
            return 0;
        }
        isOwnDC = wnd->isOwnDC() && (wnd->getOwnDC() == hdc);
        if(!isOwnDC)
        {
            pDCData  pHps = (pDCData)GpiQueryDCData((HPS)hdc);
            if(pHps && pHps->psType == MICRO_CACHED) {
                removeClientArea(wnd, pHps);
                if(pHps->hrgnVis) {
                    GreDestroyRegion(pHps->hps, pHps->hrgnVis);
                    pHps->hrgnVis = 0;
                }
            }
            else {
                dprintf(("ERROR: ReleaseDC: pHps == NULL!!"));
                DebugInt3();
            }
        }
        else {
            dprintf2(("ReleaseDC: CS_OWNDC, not released"));
        }
    }

    if(isOwnDC) {
        rc = TRUE;
    }
    else {
        UnselectGDIObjects(hdc);
        rc = O32_ReleaseDC (0, hdc);
    }

    dprintf(("ReleaseDC %x %x", hwnd, hdc));
    return (rc);
}
//******************************************************************************
// This implementation of GetDCEx supports
// DCX_WINDOW
// DCX_CACHE
// DCX_EXCLUDERGN (complex regions allowed)
// DCX_INTERSECTRGN (complex regions allowed)
// DCX_USESTYLE
// DCX_CLIPSIBLINGS
// DCX_CLIPCHILDREN
// DCX_PARENTCLIP
//
//TODO: WM_SETREDRAW affects drawingAllowed flag!!
//******************************************************************************
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

    if(hwnd == 0) {
        dprintf(("error: GetDCEx window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        return 0;
    }

    if(hwnd)
    {
        wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(wnd == NULL) {
                dprintf (("ERROR: User32: GetDCEx bad window handle %X!!!!!", hwnd));
                SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
                return 0;
        }
        if(flags & DCX_WINDOW_W) {
             hWindow = wnd->getOS2FrameWindowHandle();
        }
        else hWindow = wnd->getOS2WindowHandle();
    }

    isWindowOwnDC = (((hWindow == HWND_DESKTOP) ? FALSE : (wnd->isOwnDC()))
                    && !(flags & (DCX_CACHE_W|DCX_WINDOW_W)));

    if(isWindowOwnDC) //own dc always for client area
    {
        hps = wnd->getOwnDC();
        if (hps)
        {
            pDCData pHps = (pDCData)GpiQueryDCData (hps);
            if (!pHps)
                goto error;

            selectClientArea(wnd, pHps);

            //TODO: Is this always necessary??
            setPageXForm (wnd, pHps);
            pHps->hdcType = TYPE_1;

            //TODO: intersect/exclude clip region?
            dprintf (("User32: GetDCEx hwnd %x (%x %x) -> wnd %x hdc %x", hwnd, hrgn, flags, wnd, hps));
            return (HDC)hps;
        }
        else creatingOwnDC = TRUE;
    }

    if(isWindowOwnDC)
    {
        SIZEL sizel = {0,0};
        hps = GpiCreatePS (WinQueryAnchorBlock (hWindow),
                           WinOpenWindowDC (hWindow),
                           &sizel, PU_PELS | GPIT_MICRO | GPIA_ASSOC );
        psType = MICRO;
        // default cp is OS/2 one: set to windows default (ODIN.INI)
        GpiSetCp(hps, GetDisplayCodepage());
    }
    else
    {
        if (hWindow == HWND_DESKTOP) {
            hps = WinGetScreenPS (hWindow);
        }
        else {
            int clipstyle = 0;
            int clipwnd   = HWND_TOP;
            if(flags & (DCX_USESTYLE_W)) {
                int style = wnd->getStyle();
                if(style & WS_CLIPCHILDREN_W) {
                    clipstyle |= PSF_CLIPCHILDREN;
                }
                if(style & WS_CLIPSIBLINGS_W) {
                    clipstyle |= PSF_CLIPSIBLINGS;
                }
                if(wnd->fHasParentDC()) {
                    clipstyle |= PSF_PARENTCLIP;
                }
            }
            if(flags & DCX_CLIPSIBLINGS_W) {
                clipstyle |= PSF_CLIPSIBLINGS;
            }
            if(flags & DCX_CLIPCHILDREN_W) {
                clipstyle |= PSF_CLIPCHILDREN;
            }
            if(flags & DCX_PARENTCLIP_W) {
                clipstyle |= PSF_PARENTCLIP;
            }
            if(clipstyle) {
                 dprintf2(("WinGetClipPS style %x", clipstyle));
                 hps = WinGetClipPS(hWindow, clipwnd, clipstyle);
            }
            else hps = WinGetPS (hWindow);

            // default cp is OS/2 one: set to windows default (ODIN.INI)
            GpiSetCp(hps, GetDisplayCodepage());
        }
        psType = MICRO_CACHED;
    }

    if (!hps)
        goto error;

    HPSToHDC (hWindow, hps, NULL, NULL);
    pHps = (pDCData)GpiQueryDCData (hps);

    if(flags & DCX_WINDOW_W) {
         removeClientArea(wnd, pHps);
    }
    else selectClientArea(wnd, pHps);

    setMapMode(wnd, pHps, MM_TEXT_W);

    if ((flags & DCX_EXCLUDERGN_W) || (flags & DCX_INTERSECTRGN_W))
    {
        ULONG BytesNeeded;
        PRGNDATA RgnData;
        PRECT pr;
        int i;
        RECTL rectl;

        if (!hrgn)
            goto error;

        success = TRUE;
        if (flags & DCX_EXCLUDERGN_W)
        {
#if 0 //CB: todo
         long height;

         BytesNeeded = GetRegionData (hrgn, 0, NULL);
         RgnData = (PRGNDATA_W)_alloca (BytesNeeded);
         if (RgnData == NULL)
                goto error;
         GetRegionData (hrgn, BytesNeeded, RgnData);

         i = RgnData->rdh.nCount;
         pr = (PRECT)(RgnData->Buffer);

         if (flags & DCX_WINDOW_W)
                height = wnd->getWindowHeight();
         else   height = wnd->getClientHeight();

         for (; (i > 0) && success; i--, pr++) {
            LONG y = pr->yBottom;

            pr->yBottom = height - pr->yTop;
            pr->yTop    = height - y;
            success &= GpiExcludeClipRectangle (pHps->hps, pr);
         }
#endif
        }
        else //DCX_INTERSECTRGN_W
        {
            //SvL: I'm getting paint problems when clipping a dc created in GetDCEx
            //     with a region that covers the entire window (RealPlayer 7 Update 1)
            //     Using SelectClipRgn here doesn't make any difference.
            if(ExtSelectClipRgn(pHps->hps, hrgn, RGN_AND_W) == ERROR_W) {
                dprintf(("ExtSelectClipRgn failed!!"));
            }
        }
        if (!success)
            goto error;
    }

    if (creatingOwnDC)
        wnd->setOwnDC ((HDC)hps);

    pHps->psType  = psType;
    pHps->hdcType = TYPE_1;
    //TODO: WM_SETREDRAW affects drawingAllowed flag!!
    GpiSetDrawControl (hps, DCTL_DISPLAY, drawingAllowed ? DCTL_ON : DCTL_OFF);

    dprintf (("User32: GetDCEx hwnd %x (%x %x) -> hdc %x", hwnd, hrgn, flags, pHps->hps));
    return (HDC)pHps->hps;

error:
    /* Something went wrong; clean up
     */
    dprintf(("ERROR: GetDCEx hwnd %x (%x %x) FAILED!", hwnd, hrgn, flags));
    DebugInt3();
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
    SetLastError(ERROR_INVALID_PARAMETER_W);
    return NULL;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API GetDC (HWND hwnd)
{
    if(!hwnd)
        return GetDCEx( GetDesktopWindow(), 0, DCX_CACHE_W | DCX_WINDOW_W );

    return GetDCEx (hwnd, NULL, 0);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API GetWindowDC (HWND hwnd)
{
    if (!hwnd) hwnd = GetDesktopWindow();
    return GetDCEx (hwnd, NULL, DCX_WINDOW_W);
}
//******************************************************************************
//Helper for RedrawWindow (RDW_ALLCHILDREN_W)
//******************************************************************************
LRESULT WIN32API RedrawChildEnumProc(HWND hwnd, LPARAM lParam)
{
    RedrawWindow(hwnd, NULL, 0, lParam);
    return TRUE;
}
//******************************************************************************
// This implementation of RedrawWindow supports
// RDW_ERASE
// RDW_NOERASE
// RDW_INTERNALPAINT
// RDW_NOINTERNALPAINT
// RDW_INVALIDATE
// RDW_VALIDATE
// RDW_ERASENOW
// RDW_UPDATENOW
// RDW_FRAME
//
//TODO: Works ok for RDW_FRAME??
//      SDK docs say RDW_FRAME is only valid for RDW_INVALIDATE...
//******************************************************************************
BOOL WIN32API RedrawWindow(HWND hwnd, const RECT* pRect, HRGN hrgn, DWORD redraw)
{
    Win32BaseWindow *wnd;

    if(pRect) {
         dprintf(("RedrawWindow %x (%d,%d)(%d,%d) %x %x", hwnd, pRect->left, pRect->top, pRect->right, pRect->bottom, hrgn, redraw));
    }
    else dprintf(("RedrawWindow %x %x %x %x", hwnd, pRect, hrgn, redraw));

    if (hwnd == NULLHANDLE)
    {
        hwnd = HWND_DESKTOP;
        wnd  = Win32BaseWindow::GetWindowFromOS2Handle(OSLIB_HWND_DESKTOP);

        if (!wnd)
        {
            dprintf(("USER32:dc: RedrawWindow can't find desktop window %08xh\n",
                      hwnd));
            SetLastError(ERROR_INVALID_PARAMETER_W);
            return FALSE;
        }
    }
    else
    {
        wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);

        if (!wnd)
        {
            dprintf(("USER32:dc: RedrawWindow can't find window %08xh\n",
                  hwnd));
            SetLastError(ERROR_INVALID_PARAMETER_W);
            return FALSE;
        }
        if(redraw & RDW_FRAME_W) {
             hwnd = wnd->getOS2FrameWindowHandle();
        }
        else hwnd = wnd->getOS2WindowHandle();
    }

    BOOL  IncludeChildren = (redraw & RDW_ALLCHILDREN_W) ? TRUE : FALSE;
    BOOL  success = TRUE;
    HPS   hpsTemp = NULLHANDLE;
    HRGN  hrgnTemp = NULLHANDLE;
    RECTL rectl;

    if (hrgn)
    {
        ULONG BytesNeeded;
        PRGNDATA RgnData;
        PRECTL pr;
        int i;
        LONG height;

        if(wnd) {
             height = (redraw & RDW_FRAME_W) ? wnd->getWindowHeight() : wnd->getClientHeight();
        }
        else height = OSLibQueryScreenHeight();

        if (!hrgn)
            goto error;

        BytesNeeded = GetRegionData (hrgn, 0, NULL);
        RgnData = (PRGNDATA)_alloca (BytesNeeded);
        if (RgnData == NULL)
          goto error;
        GetRegionData (hrgn, BytesNeeded, RgnData);

        pr = (PRECTL)(RgnData->Buffer);
        for (i = RgnData->rdh.nCount; i > 0; i--, pr++) {
            LONG temp = pr->yTop;
            pr->yTop = height - pr->yBottom;
            pr->yBottom = height - temp;
            dprintf2(("RedrawWindow: region (%d,%d) (%d,%d)", pr->xLeft, pr->yBottom, pr->xRight, pr->yTop));
        }

        hpsTemp = WinGetScreenPS (HWND_DESKTOP);
        hrgnTemp = GpiCreateRegion (hpsTemp, RgnData->rdh.nCount, (PRECTL)(RgnData->Buffer));
        if (!hrgnTemp) goto error;
    }
    else if (pRect)
    {
        if(wnd) {
            if(redraw & RDW_FRAME_W) {
                 mapWin32ToOS2Rect(wnd->getWindowHeight(), (PRECT)pRect, (PRECTLOS2)&rectl);
            }
            else mapWin32ToOS2Rect(wnd->getClientHeight(), (PRECT)pRect, (PRECTLOS2)&rectl);
        }
        else mapWin32ToOS2Rect(OSLibQueryScreenHeight(), (PRECT)pRect, (PRECTLOS2)&rectl);
    }

    if (redraw & RDW_INVALIDATE_W)
    {
        //TODO: SvL: pingpong.exe doesn't have RDW_NOERASE, but doesn't want WM_ERASEBKGND msgs
        if (redraw & RDW_ERASE_W) {
             wnd->setEraseBkgnd(TRUE);
        }
        else wnd->setEraseBkgnd(FALSE);

        if (!pRect && !hrgn)
            success = WinInvalidateRect (hwnd, NULL, IncludeChildren);
        else
        if (hrgn) {
            success = WinInvalidateRegion (hwnd, hrgnTemp, IncludeChildren);
            if(IncludeChildren && WinQueryUpdateRect(hwnd, NULL) == FALSE) {
                dprintf(("WARNING: WinQueryUpdateRect %x region %x returned false even though we just invalidated part of a window!!!", hwnd, hrgnTemp));
                success = success = WinInvalidateRegion (hwnd, hrgnTemp, FALSE);
            }
        }
        else {
            dprintf2(("WinInvalidateRect (%d,%d)(%d,%d)", rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop));
            success = WinInvalidateRect (hwnd, &rectl, IncludeChildren);
//SvL: If all children are included, then WinInvalidateRect is called
//     with fIncludeChildren=1 -> rect of hwnd isn't invalidated if child(ren)
//     overlap(s) the specified rectangle completely (EVEN if window doesn't
//     have WS_CLIPCHILREN!)
//     -> example: XWing vs Tie Fighter install window
//     WinInvalidateRect with fIncludeChildren=0 invalidates both the parent
//     and child windows
//SvL: Can't always set fIncludeChildren to FALSE or else some controls in
//     the RealPlayer setup application aren't redrawn when invalidating
//     their parent
//     SO: Check if IncludeChildren is set and part of the window is invalid
//         If not -> call WinInvalidateRect with IncludeChildren=0 to force
//         the window rectangle to be invalidated
//
            if(IncludeChildren && WinQueryUpdateRect(hwnd, NULL) == FALSE) {
                dprintf(("WARNING: WinQueryUpdateRect %x (%d,%d)(%d,%d) returned false even though we just invalidated part of a window!!!", hwnd, rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop));
                success = WinInvalidateRect (hwnd, &rectl, FALSE);
            }
        }

        if (!success) goto error;
    }
    else if (redraw & RDW_VALIDATE_W)
    {
        if (redraw & RDW_NOERASE_W)
            wnd->setEraseBkgnd(FALSE);

        if (WinQueryUpdateRect (hwnd, NULL))
        {
            if(!pRect && !hrgn) {
                success = WinValidateRect (hwnd, NULL, IncludeChildren);
            }
            else
            if(hrgn) {
                success = WinValidateRegion (hwnd, hrgnTemp, IncludeChildren);
            }
            else {
                success = WinValidateRect (hwnd, &rectl, IncludeChildren);
            }
            if(!success) goto error;
        }
    }

    if(WinQueryUpdateRect(hwnd, &rectl))
    {
        //TODO: Does this work if RDW_ALLCHILDREN is set??
        if(redraw & RDW_UPDATENOW_W) {
            RECT rectUpdate;

            if(redraw & RDW_FRAME_W) {
                mapOS2ToWin32Rect(wnd->getWindowHeight(), (PRECTLOS2)&rectl, &rectUpdate);
                wnd->MsgNCPaint(&rectUpdate);
            }

            wnd->MsgPaint(0, FALSE);
        }
        else
//        if((redraw & RDW_ERASE_W) && (redraw & RDW_ERASENOW_W))
        if(redraw & RDW_ERASENOW_W && wnd->needsEraseBkgnd())
            wnd->setEraseBkgnd(sendEraseBkgnd(wnd) == 0);
//      if(redraw & RDW_ALLCHILDREN_W) {
//              EnumChildWindows(wnd->getWindowHandle(), RedrawChildEnumProc, redraw);
//      }
    }
    else if((redraw & RDW_INTERNALPAINT_W) && !(redraw & RDW_INVALIDATE_W))
    {
        if(redraw & RDW_UPDATENOW_W) {
            wnd->MsgPaint(0, FALSE);
        }
        else PostMessageA(hwnd, WINWM_PAINT, 0, 0);
    }

error:
    /* clean up */
    if (hrgnTemp)
        GpiDestroyRegion (hpsTemp, hrgnTemp);

    if (hpsTemp)
        WinReleasePS (hpsTemp);

    if (!success) {
        dprintf(("RedrawWindow failure!"));
        SetLastError(ERROR_INVALID_PARAMETER_W);
    }
    return (success);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UpdateWindow (HWND hwnd)
{
  Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
  RECTL rectl;
  BOOL  rc;

    if(!wnd) {
        dprintf (("ERROR: User32: UpdateWindow INVALID hwnd %x", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        return FALSE;
    }

#ifdef DEBUG
    if(WinQueryUpdateRect(wnd->getOS2WindowHandle(), &rectl))
    {
         RECT rectUpdate;
         mapOS2ToWin32Rect(wnd->getClientHeight(), (PRECTLOS2)&rectl, &rectUpdate);

         dprintf (("User32: UpdateWindow hwnd %x: update rectangle (%d,%d)(%d,%d)", hwnd, rectUpdate.left, rectUpdate.top, rectUpdate.right, rectUpdate.bottom));
    }
    else dprintf (("User32: UpdateWindow hwnd %x; EMPTY update rectangle (vis=%d/%d, show=%d/%d)", hwnd, WinIsWindowVisible(wnd->getOS2FrameWindowHandle()), WinIsWindowVisible(wnd->getOS2WindowHandle()), WinIsWindowShowing(wnd->getOS2FrameWindowHandle()), WinIsWindowShowing(wnd->getOS2WindowHandle())));
#endif
    //SvL: This doesn't work right (Wine uses RDW_NOCHILDREN_W -> doesn't work here)
    //     Breaks vpbuddy
    //rc = RedrawWindow(hwnd, NULL, 0, RDW_UPDATENOW_W | RDW_ALLCHILDREN_W);
    //   -> RDW_UPDATENOW causes WM_PAINT messages to be directy posted to window
    //   handler; possibly bypassing queued WM_PAINT messages for parent window(s)
    //   -> out of sync painting (i.e. parent paints over child)

//    if(!WinIsWindowShowing(wnd->getOS2FrameWindowHandle()) || !WinIsWindowShowing(wnd->getOS2WindowHandle())) {
//        dprintf(("UpdateWindow: window not showing %d/%d", WinIsWindowShowing(wnd->getOS2FrameWindowHandle()), WinIsWindowShowing(wnd->getOS2WindowHandle()) ));
//        return FALSE;
//    }
    //Must use frame window here. If the frame window has a valid update region and we call
    //WinUpdateWindow for the client window, then no WM_PAINT messages will be sent.
    rc = WinUpdateWindow(wnd->getOS2FrameWindowHandle());
#ifdef DEBUG
    if(WinQueryUpdateRect(wnd->getOS2WindowHandle(), NULL))
    {
        //if parent has valid update region then WinUpdateWindow will still fail..
        //might be harmless and happen even in windows
        dprintf (("ERROR: User32: UpdateWindow didn't send WM_PAINT messages!!"));
    }
#endif
    return rc;
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
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvalidateRect (HWND hwnd, const RECT *pRect, BOOL erase)
{
   BOOL result;

    if(pRect) {
        dprintf(("InvalidateRect %x (%d,%d)(%d,%d) erase=%d", hwnd, pRect->left, pRect->top, pRect->right, pRect->bottom, erase));
    }
    else dprintf(("InvalidateRect %x NULL erase=%d", hwnd, erase));
#if 1
    result = RedrawWindow (hwnd, pRect, NULLHANDLE,
                           RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                           (erase ? RDW_ERASE_W : 0) |
                           (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
#else
    result = RedrawWindow (hwnd, pRect, NULLHANDLE,
                           RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                           (erase ? RDW_ERASE_W : RDW_NOERASE_W) |
                           (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
#endif
    return (result);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvalidateRgn (HWND hwnd, HRGN hrgn, BOOL erase)
{
    BOOL result;

    dprintf(("InvalidateRgn %x %x erase=%d", hwnd, hrgn, erase));
#if 1
    result = RedrawWindow (hwnd, NULL, hrgn,
                          RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                          (erase ? RDW_ERASE_W : 0) |
                          (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
#else
    result = RedrawWindow (hwnd, NULL, hrgn,
                          RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                          (erase ? RDW_ERASE_W : RDW_NOERASE_W) |
                          (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
#endif
    return (result);
}
//******************************************************************************
//TODO: Change for client rectangle!!!!!
//******************************************************************************
BOOL setPMRgnIntoWinRgn (HPS hps, HRGN hrgnPM, HRGN hrgnWin, LONG height)
{
   BOOL    rc;
   RGNRECT rgnRect;
   rgnRect.ircStart    = 1;
   rgnRect.crc         = 0;
   rgnRect.ulDirection = RECTDIR_LFRT_TOPBOT;

   rc = GpiQueryRegionRects (hps, hrgnPM, NULL, &rgnRect, NULL);

   if (rc && (rgnRect.crcReturned > 0))
   {
      PRECTL Rcls = new RECTL[rgnRect.crcReturned];
      PRECTL pRcl = Rcls;

      if (Rcls != NULL)
      {
         rgnRect.crc = rgnRect.crcReturned;
         rc = GpiQueryRegionRects (hps, hrgnPM, NULL, &rgnRect, Rcls);

         rc = SetRectRgn(hrgnWin, pRcl->xLeft, height - pRcl->yTop,
                         pRcl->xRight, height - pRcl->yBottom);

         if (rgnRect.crcReturned > 1)
         {
            int i;
            HRGN temp;
            temp = CreateRectRgn (0, 0, 1, 1);

            for (i = 1, pRcl++; rc && (i < rgnRect.crcReturned); i++, pRcl++)
            {
              rc = SetRectRgn (temp, pRcl->xLeft, height - pRcl->yTop,
                               pRcl->xRight, height - pRcl->yBottom);
              rc &= CombineRgn (hrgnWin, hrgnWin, temp, RGN_OR_W);
            }
            DeleteObject (temp);
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
      rc = SetRectRgn (hrgnWin, 0, 0, 0, 0);
   }

   return (rc);
}
//******************************************************************************
//TODO: Check if this one works correctly...
//Have to check if dc is for frame or client!!
//******************************************************************************
BOOL WIN32API ScrollDC(HDC hDC, int dx, int dy, const RECT *pScroll,
                       const RECT *pClip, HRGN hrgnUpdate, LPRECT pRectUpdate)
{
    Win32BaseWindow *wnd;
    BOOL rc = TRUE;

    dprintf (("USER32: ScrollDC %x (%d,%d), %x %x %x %x", hDC, dx, dy, pScroll, pClip, hrgnUpdate, pRectUpdate));

    if (!hDC)
    {
        return (FALSE);
    }

    pDCData pHps         = (pDCData)GpiQueryDCData ((HPS)hDC);
    HWND    hwnd         = pHps->hwnd;
    if(pHps->isClient) {
        wnd = Win32BaseWindow::GetWindowFromOS2Handle (hwnd);
    }
    else {
        //assume (for now) that this is not allowed (TODO)
        dprintf(("ScrollDC used for frame HDC!!"));
        DebugInt3();
        return FALSE;
    }

    if((hwnd == NULLHANDLE) || !wnd)
    {
        return (FALSE);
    }

    POINTL ptl[2] = { 0, 0, dx, dy };

    GpiConvert (pHps->hps, CVTC_WORLD, CVTC_DEVICE, 2, ptl);
    dx = (int)(ptl[1].x - ptl[0].x);
    dy = (int)(ptl[1].y - ptl[0].y);

    RECT scrollRect;
    RECT clipRect;

    if (pClip)
    {
        memcpy(&clipRect, pClip, sizeof(clipRect));

        if ((pHps->graphicsMode == GM_COMPATIBLE_W) &&
            (clipRect.left   != clipRect.right)   &&
            (clipRect.bottom != clipRect.top))
        {
            if (abs((int)pHps->viewportXExt) <= abs((int)pHps->windowExt.cx))
                clipRect.right -= abs(pHps->worldXDeltaFor1Pixel);
            else
                clipRect.left  += abs(pHps->worldXDeltaFor1Pixel);

            if (abs((int)pHps->viewportYExt) <= abs((int)pHps->windowExt.cy))
                clipRect.bottom -= abs(pHps->worldYDeltaFor1Pixel);
            else
                clipRect.top    += abs(pHps->worldYDeltaFor1Pixel);
        }
        GpiConvert(pHps->hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&clipRect);
        if (clipRect.right < clipRect.left) {
            ULONG temp     = clipRect.left;
            clipRect.left  = clipRect.right;
            clipRect.right = temp;
        }
        if (clipRect.bottom < clipRect.top) {
            ULONG temp       = clipRect.top;
            clipRect.top     = clipRect.bottom;
            clipRect.bottom  = temp;
        }
    }

    if (pScroll)
    {
        memcpy(&scrollRect, pScroll, sizeof(scrollRect));

        if ((pHps->graphicsMode == GM_COMPATIBLE_W) &&
            (scrollRect.left != scrollRect.right)   &&
            (scrollRect.top  != scrollRect.bottom))
        {
            if (abs((int)pHps->viewportXExt) <= abs((int)pHps->windowExt.cx))
                scrollRect.right -= abs(pHps->worldXDeltaFor1Pixel);
            else
                scrollRect.left  += abs(pHps->worldXDeltaFor1Pixel);

            if (abs((int)pHps->viewportYExt) <= abs((int)pHps->windowExt.cy))
                scrollRect.bottom  -= abs(pHps->worldYDeltaFor1Pixel);
            else
                scrollRect.top     += abs(pHps->worldYDeltaFor1Pixel);
        }
        GpiConvert (pHps->hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)&scrollRect);
        if (scrollRect.right < scrollRect.left) {
            ULONG temp       = scrollRect.left;
            scrollRect.left  = scrollRect.right;
            scrollRect.right = temp;
        }
        if (scrollRect.bottom < scrollRect.top) {
            ULONG temp        = scrollRect.top;
            scrollRect.top    = scrollRect.bottom;
            scrollRect.bottom = temp;
        }
    }
    RECTL  rectlUpdate;
    HRGN   hrgn = NULLHANDLE;
    RECTL  clipOS2;
    RECTL  scrollOS2;
    PRECTL pScrollOS2 = NULL;
    PRECTL pClipOS2   = NULL;
    HPS    hpsScreen  = 0;

    if(pScroll) {
        mapWin32ToOS2Rect(wnd->getClientHeight(), &scrollRect, (PRECTLOS2)&scrollOS2);
        pScrollOS2 = &scrollOS2;
    }

    if(pClip) {
        mapWin32ToOS2Rect(wnd->getClientHeight(), &clipRect, (PRECTLOS2)&clipOS2);
        pClipOS2 = &clipOS2;
    }

    if(hrgnUpdate) {
        RECTL rectl = { 1, 1, 2, 2 };

        hpsScreen = WinGetScreenPS (HWND_DESKTOP);
        hrgn      = GpiCreateRegion(hpsScreen, 1, &rectl);
    }

    LONG lComplexity = WinScrollWindow(hwnd, dx, dy, pScrollOS2,
                                       pClipOS2, hrgn, &rectlUpdate, 0);
    if (lComplexity == RGN_ERROR)
    {
        return (FALSE);
    }

    RECT winRectUpdate;

    mapOS2ToWin32Rect(wnd->getClientHeight(), (PRECTLOS2)&rectlUpdate, &winRectUpdate);

    if (pRectUpdate)
        *pRectUpdate = winRectUpdate;

    if (hrgnUpdate) {
        rc = setPMRgnIntoWinRgn(hpsScreen, hrgn, hrgnUpdate, wnd->getClientHeight());
        GpiDestroyRegion(hpsScreen, hrgn);
        WinReleasePS(hpsScreen);
    }

    return (rc);
}
//******************************************************************************
//Using WinScrollWindow to scroll child windows is better (smoother).
//******************************************************************************
INT WIN32API ScrollWindowEx(HWND hwnd, int dx, int dy, const RECT *pScroll, const RECT *pClip,
                            HRGN hrgnUpdate, PRECT pRectUpdate, UINT scrollFlag)
{
 Win32BaseWindow *window;
 APIRET  rc;
 RECTL   scrollRect;
 RECTL   clipRect;
 PRECTL  pScrollOS2 = NULL;
 PRECTL  pClipOS2   = NULL;
 ULONG   scrollFlagsOS2 = 0;
 int     orgdy = dy;
 int     regionType = ERROR_W;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("ScrollWindowEx, window %x not found", hwnd));
        return 0;
    }

    dprintf(("ScrollWindowEx %x %d %d %x %x", hwnd, dx, dy, pScroll, pClip));

    dy = revertDy (window, dy);

    if (scrollFlag & SW_INVALIDATE_W)      scrollFlagsOS2 |= SW_INVALIDATERGN;
    if (scrollFlag & SW_SCROLLCHILDREN_W)  scrollFlagsOS2 |= SW_SCROLLCHILDREN;

    if(pScroll) {
        mapWin32ToOS2Rect(window->getClientHeight(), (RECT *)pScroll, (PRECTLOS2)&scrollRect);
        pScrollOS2 = &scrollRect;
    }

    if(pClip) {
        mapWin32ToOS2Rect(window->getClientHeight(), (RECT *)pClip, (PRECTLOS2)&clipRect);
        pClipOS2 = &clipRect;
    }

    RECTL rectlUpdate;
    HRGN  hrgn = NULLHANDLE;
    HPS   hpsScreen  = 0;

    if(hrgnUpdate) {
        RECTL rectl = { 1, 1, 2, 2 };

        hpsScreen = WinGetScreenPS (HWND_DESKTOP);
        hrgn      = GpiCreateRegion(hpsScreen, 1, &rectl);
    }

    if (scrollFlag & SW_SMOOTHSCROLL_W)
    {
        INT time = (scrollFlag >> 16) & 0xFFFF;

        //CB: todo, scroll in several steps
        //    is time in ms? time <-> iteration count?
    }

    LONG lComplexity = WinScrollWindow(window->getOS2WindowHandle(), dx, dy,
                                       pScrollOS2, pClipOS2,
                                       hrgn, &rectlUpdate, scrollFlagsOS2);
    if (lComplexity == RGN_ERROR)
    {
        return ERROR_W;
    }

    //Notify children that they have moved (according to the SDK docs,
    //they only receive a WM_MOVE message)
    //(PM only does this for windows with CS_MOVENOTIFY class)
    //TODO: Verify this (no WM_WINDOWPOSCHANGING/ED?)
    if (scrollFlag & SW_SCROLLCHILDREN_W)
    {
        HWND hwndChild;
        RECT rectChild, rc;

            dprintf(("ScrollWindowEx: Scroll child windows"));
            GetClientRect(hwnd, &rc);
            if (pScroll) IntersectRect(&rc, &rc, pScroll);

            hwndChild = GetWindow(hwnd, GW_CHILD_W);

            while(hwndChild)
            {
                Win32BaseWindow *child;

                child = Win32BaseWindow::GetWindowFromHandle(hwndChild);
                if(!child) {
                    dprintf(("ERROR: ScrollWindowEx, child %x not found", hwnd));
                    return 0;
                }
                rectChild = *child->getWindowRect();
                if(!pScroll || IntersectRect(&rectChild, &rectChild, &rc))
                {
                     dprintf(("ScrollWindowEx: Scroll child window %x", hwndChild));
                     child->ScrollWindow(dx, orgdy);
                }
                hwndChild = GetWindow(hwndChild, GW_HWNDNEXT_W);
            }
            dprintf(("***ScrollWindowEx: Scroll child windows DONE"));
    }

    RECT winRectUpdate;

    mapOS2ToWin32Rect(window->getClientHeight(), (PRECTLOS2)&rectlUpdate, &winRectUpdate);

    if (pRectUpdate)
       *pRectUpdate = winRectUpdate;

    if (hrgnUpdate) {
        rc = setPMRgnIntoWinRgn(hpsScreen, hrgn, hrgnUpdate, window->getClientHeight());
        GpiDestroyRegion(hpsScreen, hrgn);
        WinReleasePS(hpsScreen);
    }

#if 0
    //SvL: WinScrollWindow already invalidates the area; no need to do it again
    //(call to invalidateRect was wrong; has to include erase flag)
    if ((scrollFlag & SW_INVALIDATE_W) &&
        ((lComplexity == RGN_RECT) || (lComplexity == RGN_COMPLEX)))
    {
       rc = RedrawWindow (hwnd, &winRectUpdate, NULLHANDLE,
                          RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                          ((scrollFlag & SW_ERASE_W) ? RDW_ERASE_W : 0) |
                          RDW_UPDATENOW_W);

//       rc = InvalidateRect (hwnd, &winRectUpdate, (scrollFlag & SW_ERASE_W) ? 1 : 0);
       if (rc == FALSE)
       {
          return (0);
       }
    }
#endif

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
BOOL WIN32API ScrollWindow(HWND hwnd, int dx, int dy, const RECT *pScroll, const RECT *pClip)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("ScrollWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("ScrollWindow %x %d %d %x %x", hwnd, dx, dy, pScroll, pClip));
    return (ERROR_W != ScrollWindowEx(hwnd, dx, dy, pScroll, pClip, 0, NULL,
                                      (pScroll ? 0 : SW_SCROLLCHILDREN_W) |
                                      SW_INVALIDATE_W ));
}
//******************************************************************************
//******************************************************************************
HWND WIN32API WindowFromDC(HDC hdc)
{
   pDCData pHps = (pDCData)GpiQueryDCData( (HPS)hdc );

   dprintf2(("USER32:  WindowFromDC %x", hdc));
   if ( pHps )
      return OS2ToWin32Handle(pHps->hwnd);
   else
      return 0;
}
//******************************************************************************
//******************************************************************************

