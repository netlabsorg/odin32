/* $Id: dc.cpp,v 1.6 1999-09-26 14:44:58 sandervl Exp $ */

/*
 * DC functions for USER32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_WIN
#define INCL_GPI
#define INCL_GREALL
#define INCL_DEV
#include <os2.h>
#include <pmddi.h>
#include <os2sel.h>
#include <stdlib.h>
#include "win32type.h"
#include <winconst.h>
#include <wprocess.h>
#include <misc.h>
#include <win32wbase.h>
#include <math.h>
#include <limits.h>
#include "oslibwin.h"

#ifndef OPEN32API
#define OPEN32API _System
#endif

/*********************/
typedef struct _tagXFORM
{
    FLOAT eM11;
    FLOAT eM12;
    FLOAT eM21;
    FLOAT eM22;
    FLOAT eDx;
    FLOAT eDy;
} XFORM_W, *PXFORM_W, *LPXFORM_W;

typedef struct
{
    HDC   hdc;
    BOOL  fErase;
    RECT  rcPaint;
    BOOL  fRestore;
    BOOL  IncUpdate;
    BYTE  rgbReserved[32];
} PAINTSTRUCT_W, *PPAINTSTRUCT_W, *LPPAINTSTRUCT_W;

#define PS_SOLID_W         0x00000000
#define PS_DASH_W          0x00000001
#define PS_DOT_W           0x00000002
#define PS_DASHDOT_W       0x00000003
#define PS_DASHDOTDOT_W    0x00000004
#define PS_NULL_W          0x00000005
#define PS_INSIDEFRAME_W   0x00000006
#define PS_USERSTYLE_W     0x00000007
#define PS_ALTERNATE_W     0x00000008
#define PS_STYLE_MASK_W    0x0000000f

typedef struct
{
    UINT   lopnStyle;
    POINT  lopnWidth;
    ULONG  lopnColor;
} LOGPEN_W, *LPLOGPEN_W;

typedef struct tagEXTLOGPEN
{
    DWORD elpPenStyle;
    DWORD elpWidth;
    DWORD elpBrushStyle;
    DWORD elpColor;
    DWORD elpNumEntries;
    DWORD elpStyleEntry[1];
} EXTLOGPEN_W, *PEXTLOGPEN_W, *NPEXTLOGPEN_W, *LPEXTLOGPEN_W;

typedef struct
{
    UINT   lbStyle;
    ULONG  lbColor;
    INT    lbHatch;
} LOGBRUSH_W, *LPLOGBRUSH_W;

typedef struct _penobject
{
   ULONG filler[9];
   union {
     struct {
       PEXTLOGPEN_W      pExtLogPen;
       LOGBRUSH_W        logbrush;
       LOGPEN_W          logpen;
     } ExtPen;
     struct {
       LOGPEN_W          logpen;
     } Pen;
   };
} tPenObject, *pPenObject;

/* DC Graphics Mode */
#define GM_COMPATIBLE_W     1
#define GM_ADVANCED_W       2

#define DCX_WINDOW_W                    0x00000001L
#define DCX_CACHE_W                     0x00000002L
#define DCX_NORESETATTRS_W              0x00000004L
#define DCX_CLIPCHILDREN_W              0x00000008L
#define DCX_CLIPSIBLINGS_W              0x00000010L
#define DCX_PARENTCLIP_W                0x00000020L
#define DCX_EXCLUDERGN_W                0x00000040L
#define DCX_INTERSECTRGN_W              0x00000080L
#define DCX_EXCLUDEUPDATE_W             0x00000100L
#define DCX_INTERSECTUPDATE_W           0x00000200L
#define DCX_LOCKWINDOWUPDATE_W          0x00000400L
#define DCX_VALIDATE_W                  0x00200000L

#define RDW_INVALIDATE_W       0x0001
#define RDW_INTERNALPAINT_W    0x0002
#define RDW_ERASE_W            0x0004
#define RDW_VALIDATE_W         0x0008
#define RDW_NOINTERNALPAINT_W  0x0010
#define RDW_NOERASE_W          0x0020
#define RDW_NOCHILDREN_W       0x0040
#define RDW_ALLCHILDREN_W      0x0080
#define RDW_UPDATENOW_W        0x0100
#define RDW_ERASENOW_W         0x0200
#define RDW_FRAME_W            0x0400
#define RDW_NOFRAME_W          0x0800

typedef struct _RGNDATAHEADER_W {
    DWORD       dwSize;
    DWORD       iType;
    DWORD       nCount;
    DWORD       nRgnSize;
    RECT        rcBound;
} RGNDATAHEADER_W, *LPRGNDATAHEADER_W;

typedef struct _RGNDATA_W {
    RGNDATAHEADER_W     rdh;
    char                Buffer[1];
} RGNDATA_W , *PRGNDATA_W , *LPRGNDATA_W ;


/* Xform FLAGS */
#define MWT_IDENTITY_W        1
#define MWT_LEFTMULTIPLY_W    2
#define MWT_RIGHTMULTIPLY_W   3

/* Mapping Modes */
#define MM_TEXT_W             1
#define MM_LOMETRIC_W         2
#define MM_HIMETRIC_W         3
#define MM_LOENGLISH_W        4
#define MM_HIENGLISH_W        5
#define MM_TWIPS_W            6
#define MM_ISOTROPIC_W        7
#define MM_ANISOTROPIC_W      8

enum PS_Type { MICRO_CACHED, MICRO, NORMAL };
enum HDC_Type{ TYPE_0, TYPE_1, TYPE_2, TYPE_3, TYPE_4 };

typedef struct _DCData
{
   HDC         hdc;
   HWND        hwnd;
   HPS         hps;

   UINT        lastBrushHandle;
   PVOID       lastBrushObject;
   UINT        lastPenHandle;
   pPenObject  lastPenObject;
   UINT        lastFontHandle;
   PVOID       lastFontObject;
   UINT        lastBitmapHandle;
   PVOID       lastBitmapObject;
   UINT        lastPaletteHandle;
   PVOID       lastPaletteObject;

   UINT        nullBitmapHandle;

   ULONG       BkColor;
   ULONG       TextColor;
   ULONG       BkColor_PM;
   ULONG       TextColor_PM;

   int         BkMode;
   ULONG       BkMode_PM;

   int         ROP2Mode;
   int         ROP2Mode_PM;

   unsigned    isMemoryPS:1;
   unsigned    isMetaPS:1;
   unsigned    isPrinter:1;
   unsigned    isFrameWindow:1;
   unsigned    isOD_QUEUED:1;
   unsigned    isOD_INFO:1;
   unsigned    isClient:1;
   unsigned    isClientArea:1;
   unsigned    isLeftLeft:1;
   unsigned    isTopTop:1;
   unsigned    isWideLine:1;
   unsigned    alignUpdateCP:1;
   unsigned    isCacheable:1;
   unsigned    penIsExtPen:1;
   unsigned    isValid:1;
   unsigned    inPath:1;
   unsigned    isStartDoc:1;
   unsigned    resetStockFonts:1;
   unsigned    unused:14;

   ULONG       MapMode;
   HBITMAP     bitmapHandle;
   ULONG       bitmapHeight;
   ULONG       bitmapWidth;
   ULONG       hMeta;
   PVOID       pMetaFileObject;
   int         polyFillMode;
   int         arcDirection;
   int         stretchBltMode;
   int         graphicsMode;
   HRGN        hrgnHDC;

   PS_Type     psType;

   HDC_Type    hdcType;
   USHORT      usFiller;
   POINTL      viewportOrg;
   double      viewportXExt;
   double      viewportYExt;
   POINTL      windowOrg;
   SIZEL       windowExt;
   HRGN        hrgnVis;
   POINTL      ptlOrigin;
   ULONG       printPageHeight;
   PVOID       printerObject;

   LONG        taMode;
   XFORM_W     xform;

   INT         worldYDeltaFor1Pixel;
   INT         worldXDeltaFor1Pixel;
   ULONG       colorMode;
   PULONG      pLogColorTable;

   ULONG       lcidBitfield;

   HWND        hwndRealize;
   ULONG       cpeMap;

   LONG        lTechnology;

   LONG        lWndXExtSave, lWndYExtSave,
               lVwpXExtSave, lVwpYExtSave;

   int         height;

   POINTL      brushOrgPoint;

   PVOID       pEnhMetaPalette;
   PVOID       lpAbortProc;
   ULONG       HPStoHDCInversionHeight;

   int         saveLevel;

   struct      _DCData *nextDCData;
} tDCData, *pDCData;

/*********************/

BOOL    APIENTRY GpiEnableYInversion (HPS hps, LONG lHeight);
LONG    APIENTRY GpiQueryYInversion (HPS hps);
PVOID   APIENTRY GpiAllocateDCData (HPS GpiH, ULONG size);
PVOID   APIENTRY GpiQueryDCData (HPS hps);
HDC     OPEN32API HPSToHDC (HWND hwnd, HPS hps, HDC hdc, PVOID);
void    OPEN32API DeleteHDC (HDC hdc);
BOOL    OPEN32API _O32_EndPaint (HWND hwnd, const PAINTSTRUCT_W *lpps);
int     OPEN32API _O32_GetUpdateRgn (HWND hwnd, HRGN hrgn, BOOL erase);
ULONG   OPEN32API _O32_GetRegionData (HRGN hrgn, ULONG count, PRGNDATA_W pData);
BOOL    OPEN32API _O32_DeleteObject (LHANDLE hgdiobj);
int     OPEN32API _O32_ReleaseDC (HWND hwnd, HDC hdc);
VOID    OPEN32API _O32_SetLastError( DWORD );

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
//         SET_ERROR_WIN(ERROR_INVALID_PARAMETER_W);
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
//         SET_ERROR_LAST();
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
         return (wnd->getWindowHeight());
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

VOID removeClientArea(pDCData pHps)
{
   pHps->isClient = FALSE;

   if (pHps->isClientArea)
   {
      pHps->isClientArea = FALSE;
      GreSetupDC(pHps->hps,
                 pHps->hrgnVis,
                 pHps->ptlOrigin.x,
                 pHps->ptlOrigin.y,
                 0,
                 SETUPDC_ORIGIN | SETUPDC_VISRGN | SETUPDC_RECALCCLIP);
   }
}

BOOL selectClientArea(Win32BaseWindow *wnd, pDCData pHps, PRECTL prclPaint)
{
   RECTL rcl;
   HRGN hrgnRect;
   HWND hwnd;

   if (!wnd) return (FALSE);

   pHps->isClient = TRUE;
   hwnd = pHps->hwnd;

   rcl.xLeft  = rcl.yBottom = 0;
   rcl.xRight = wnd->getWindowWidth();
   rcl.yTop   = wnd->getWindowHeight();

   WinMapWindowPoints(hwnd, HWND_DESKTOP, (PPOINTL) &rcl, 2);
   pHps->ptlOrigin = *((PPOINTL) &rcl);

   if (pHps->hrgnVis == 0)
      pHps->hrgnVis = GreCreateRectRegion(pHps->hps, &rcl, 1);

   hrgnRect = GreCreateRectRegion(pHps->hps, &rcl, 1);

   if (!pHps->isClientArea)
      GreCopyClipRegion(pHps->hps, pHps->hrgnVis, 0, COPYCRGN_VISRGN);

   GreCombineRegion(pHps->hps, hrgnRect, pHps->hrgnVis, hrgnRect, CRGN_AND);
   GreSetupDC(pHps->hps,
              hrgnRect,
              rcl.xLeft,
              rcl.yBottom,
              prclPaint,
              SETUPDC_ORIGIN | SETUPDC_VISRGN | SETUPDC_RECALCCLIP);

   pHps->isClientArea = TRUE;
   GreDestroyRegion(pHps->hps, hrgnRect);

   return (TRUE);
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
      _O32_DeleteObject (pHps->nullBitmapHandle);
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
//      SET_ERROR_WIN( ERROR_INVALID_PARAMETER_W );
      return (HDC)NULLHANDLE;
   }

   USHORT sel = RestoreOS2FS();
   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if ((hwnd != HWND_DESKTOP) && wnd->isOwnDC())
   {
      hPS_ownDC = wnd->getOwnDC();
      pHps = (pDCData)GpiQueryDCData(hPS_ownDC);
      if (!pHps)
      {
//         SET_ERROR_LAST();
         SetFS(sel);
         return (HDC)NULLHANDLE;
      }
   }

   HWND hwndClient = wnd->getOS2WindowHandle();
   HPS  hps = WinBeginPaint(hwndClient, hPS_ownDC, &rect);

   if (!pHps)
   {
      HDC hdc = HPSToHDC (hwndClient, hps, NULL, NULL);
      pHps = (pDCData)GpiQueryDCData(hps);
   }

   if (wnd->isFrameWindow())
   {
//      WinSendMsg( hwnd, /* WM_DRAW */ 0x20D, (MPARAM)hps, MPVOID );
      selectClientArea(wnd, pHps, &rect);
   }

   if (hPS_ownDC == 0)
      setMapMode (wnd, pHps, MM_TEXT_W);
   else
      setPageXForm (wnd, pHps);

   pHps->hdcType = TYPE_3;
   lpps->hdc = (HDC)hps;

   if (wnd->isEraseBkgnd())
       wnd->setEraseBkgnd (FALSE, !wnd->MsgEraseBackGround(lpps->hdc));
   wnd->setSupressErase (FALSE);
   lpps->fErase = wnd->isPSErase();

   if (!hPS_ownDC)
   {
      long height  = wnd->getWindowHeight();
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

   SetFS(sel);
   _O32_SetLastError(0);
   return (HDC)pHps->hps;
}

BOOL WIN32API EndPaint (HWND hwnd, const PAINTSTRUCT_W *pPaint)
{
dprintf (("USER32: EndPaint(%x)", hwnd));

   if (!pPaint || !pPaint->hdc )
      return TRUE;

   USHORT sel = RestoreOS2FS();
   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   if (!wnd) goto exit;

   if (wnd->isOwnDC())
   {
       pDCData pHps = (pDCData)GpiQueryDCData((HPS)pPaint->hdc);
       if (pHps && (pHps->hdcType == TYPE_3))
       {
           removeClientArea (pHps);
           WinEndPaint (pHps->hps);
       }
   }
   else
   {
       _O32_EndPaint (HWND_DESKTOP, pPaint);
   }

exit:
   SetFS(sel);
   _O32_SetLastError(0);
   return TRUE;
}

BOOL WIN32API GetUpdateRect (HWND hwnd, LPRECT pRect, BOOL erase)
{
   if (hwnd)
   {
//      SET_ERROR_WIN( ERROR_INVALID_HANDLE_W );
      return FALSE;
   }

   RECTL rectl;
   USHORT sel = RestoreOS2FS();
   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

   BOOL updateRegionExists = WinQueryUpdateRect (hwnd, pRect ? &rectl : NULL);
   if (!pRect) {
      SetFS(sel);
      return (updateRegionExists);
   }

   if (updateRegionExists)
   {
      if (wnd->isOwnDC())
      {
         pDCData pHps = NULL;
         pHps = (pDCData)GpiQueryDCData(wnd->getOwnDC());
         if (!pHps)
         {
//            SET_ERROR_WIN(ERROR_INVALID_HANDLE_W);
            SetFS(sel);
            return FALSE;
         }
         GpiConvert (pHps->hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)&rectl);
      }
      else
      {
         long height   = wnd->getWindowHeight();
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

   SetFS(sel);
   return updateRegionExists;
}

int WIN32API GetUpdateRgn (HWND hwnd, HRGN hrgn, BOOL erase)
{
   USHORT sel = RestoreOS2FS();
   LONG Complexity;

   Complexity = _O32_GetUpdateRgn (hwnd, hrgn, FALSE);
   if (erase && (Complexity > NULLREGION_W)) {
       Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
       sendEraseBkgnd (wnd);
   }

   SetFS(sel);
   return Complexity;
}

// This implementation of GetDCEx supports
// DCX_WINDOW
// DCX_CACHE
// DCX_EXCLUDERGN (complex regions allowed)
// DCX_INTERSECTRGN (complex regions allowed)

HDC WIN32API GetDCEx (HWND hwnd, HRGN hrgn, ULONG flags)
{
   USHORT sel = RestoreOS2FS();
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
      if (flags & DCX_WINDOW_W)
         hWindow = wnd->getOS2FrameWindowHandle();
      else
         hWindow = wnd->getOS2WindowHandle();
   }
   else
      hWindow = HWND_DESKTOP;

dprintf (("User32: GetDCEx hwnd %x (%x %x) -> wnd %x", hwnd, hrgn, flags, wnd));

   isWindowOwnDC = (((hWindow == HWND_DESKTOP) ? FALSE : wnd->isOwnDC())
                 && !(flags & DCX_CACHE_W));
   if (isWindowOwnDC)
   {
      hps = wnd->getOwnDC();
      if (hps)
      {
         pDCData pHps = (pDCData)GpiQueryDCData (hps);
         if (!pHps)
            goto error;

         if (flags & DCX_WINDOW_W)
            removeClientArea (pHps);
         else
            selectClientArea (wnd, pHps, 0);

         setPageXForm (wnd, pHps);

         pHps->hdcType = TYPE_1;
         SetFS(sel);
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

   if (!(flags & DCX_WINDOW_W)) {
      if (selectClientArea (wnd, pHps, 0))
         setMapMode (wnd, pHps, MM_TEXT_W);
   }

   if ((flags & DCX_EXCLUDERGN_W) || (flags & DCX_INTERSECTRGN_W))
   {
      ULONG BytesNeeded;
      PRGNDATA_W RgnData;
      PRECTL pr;
      int i;
      LONG height = OSLibQueryScreenHeight();

      if (!hrgn)
         goto error;

      BytesNeeded = _O32_GetRegionData (hrgn, 0, NULL);
      RgnData = (PRGNDATA_W)_alloca (BytesNeeded);
      if (RgnData == NULL)
          goto error;
      _O32_GetRegionData (hrgn, BytesNeeded, RgnData);

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

   SetFS(sel);
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

      _O32_DeleteObject (pHps->nullBitmapHandle);
   }
//   SET_ERROR_LAST();
   SetFS(sel);
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
   USHORT sel = RestoreOS2FS();
   BOOL isOwnDC = FALSE;
   int rc;

   if (hwnd)
   {
      Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
      isOwnDC = wnd->isOwnDC();
   }
   if (isOwnDC)
      rc = TRUE;
   else
      rc = _O32_ReleaseDC (0, hdc);

   SetFS(sel);
   dprintf(("ReleaseDC %x %x", hwnd, hdc));
   return (rc);
}

BOOL WIN32API UpdateWindow (HWND hwnd)
{
   if (!hwnd)
       return FALSE;

   USHORT sel = RestoreOS2FS();
   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);

dprintf (("User32: UpdateWindow hwnd %x -> wnd %x", hwnd, wnd));

   if (WinQueryUpdateRect (wnd->getOS2WindowHandle(), NULL))
       sendEraseBkgnd (wnd);

   wnd->MsgPaint(0);

   SetFS(sel);
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

BOOL WIN32API RedrawWindow (HWND hwnd, const RECT *pRect, HRGN hrgn, DWORD redraw)
{
   Win32BaseWindow *wnd;

   if (redraw & (RDW_FRAME_W | RDW_NOFRAME_W))
   {
//      SET_ERROR_WIN( ERROR_NOT_SUPPORTED_W );
      return FALSE;
   }

   USHORT sel = RestoreOS2FS();
 dprintf(("USER32: RedrawWindow %X, %X %X %X", hwnd, pRect, hrgn, redraw));

   if (hwnd == NULLHANDLE) {
      hwnd = HWND_DESKTOP;
      wnd  = Win32BaseWindow::GetWindowFromOS2Handle(OSLIB_HWND_DESKTOP);
   }
   else
   {
      wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);

      if (!wnd)
      {
//         SET_ERROR_LAST();
         SetFS(sel);
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

   if (redraw & RDW_UPDATENOW_W)
      wnd->setSupressErase (FALSE);
   else if (redraw & RDW_ERASENOW_W)
      wnd->setSupressErase (FALSE);
#if 0
   else
   {
      QMSG qmsg;
      BOOL erase;

      erase = (WinPeekMsg (HABX, &qmsg, hwnd, WM_PAINT, WM_PAINT, PM_REMOVE)
                && (redraw & RDW_NOERASE_W) == 0);

      wnd->setSupressErase (!erase);
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
      LONG height = wnd ? wnd->getWindowHeight() : OSLibQueryScreenHeight();

      if (!hrgn)
         goto error;

      BytesNeeded = _O32_GetRegionData (hrgn, 0, NULL);
      RgnData = (PRGNDATA_W)_alloca (BytesNeeded);
      if (RgnData == NULL)
          goto error;
      _O32_GetRegionData (hrgn, BytesNeeded, RgnData);

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
      LONG height = wnd ? wnd->getWindowHeight() : OSLibQueryScreenHeight();

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

   if ((redraw & RDW_INVALIDATE_W) == 0)
      wnd->setSupressErase (FALSE);
   else if ((redraw & RDW_ERASENOW_W) == RDW_ERASENOW_W)
      wnd->setSupressErase (TRUE);

//   if (!success)
//      SET_ERROR_LAST();

   SetFS(sel);
   return (success);
}

BOOL WIN32API InvalidateRect (HWND hwnd, const RECT *pRect, BOOL erase)
{
   USHORT sel = RestoreOS2FS();
//   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
   BOOL result;

// todo !!
//   if ( isFrame without client )
//      erase = TRUE;

   result = RedrawWindow (hwnd, pRect, NULLHANDLE,
                          RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                          (erase ? RDW_ERASE_W : 0) |
                          (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
   SetFS(sel);
   return (result);
}

BOOL WIN32API InvalidateRgn (HWND hwnd, HRGN hrgn, BOOL erase)
{
   USHORT sel = RestoreOS2FS();
//   Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle (hwnd);
   BOOL result;

// todo !!
//   if ( isFrame without client )
//      erase = TRUE;

   result = RedrawWindow (hwnd, NULL, hrgn,
                          RDW_ALLCHILDREN_W | RDW_INVALIDATE_W |
                          (erase ? RDW_ERASE_W : 0) |
                          (hwnd == NULLHANDLE ? RDW_UPDATENOW_W : 0));
   SetFS(sel);
   return (result);
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
 ULONG   scrollFlags = 0;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("ScrollWindow, window %x not found", hwnd));
        return 0;
    }
    dprintf(("ScrollWindow %x %d %d\n", hwnd, dx, dy));
    MapWin32ToOS2Rectl(window->getClientRect(), (PRECTLOS2)&clientRect);
    if(pScroll) {
    	 MapWin32ToOS2Rectl((RECT *)pScroll, (PRECTLOS2)&scrollRect);
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
    	 MapWin32ToOS2Rectl((RECT *)pClip, (PRECTLOS2)&clipRect);
	 pClipRect = &clipRect;

	 //Clip rectangle relative to client area
         pClipRect->xLeft     += clientRect.xLeft;
         pClipRect->xRight    += clientRect.xLeft;
         pClipRect->yTop      += clientRect.yBottom;
         pClipRect->yBottom   += clientRect.yBottom;
         WinIntersectRect ((HAB) 0, pClipRect, pClipRect, &clientRect);
    }

    dy = -dy; //always correct?

    rc = WinScrollWindow(window->getOS2WindowHandle(), dx, dy,
                         pScrollRect, pClipRect, NULLHANDLE,
                         NULL, scrollFlags);

    return (rc != RGN_ERROR);
}
//******************************************************************************
//TODO: Implement this one
//******************************************************************************
INT WIN32API ScrollWindowEx(HWND hwnd, int dx, int dy, const RECT *pScroll, const RECT *pClip, 
                            HRGN hrgnUpdate, PRECT pRectUpdate, UINT scrollFlag)
{
    dprintf(("USER32:  ScrollWindowEx NOT CORRECTLY IMPLEMENTED\n"));
    return ScrollWindow(hwnd, dx, dy, pScroll, pClip);
}
//******************************************************************************
//******************************************************************************
