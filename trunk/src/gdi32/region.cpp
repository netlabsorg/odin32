/* $Id: region.cpp,v 1.33 2003-11-12 14:10:55 sandervl Exp $ */

/*
 * GDI32 region code
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 * Copyright 2002-2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * TODO: Metafile recording
 * TODO: Do we need to translate & set the last error for Gpi operations?
 *
 *
 * NOTE: In windows there is a clip and a visible region (just like in PM)
 *       We do not map the Win32 visible region to the PM visible region
 *       as that's rather annoying.
 *       Instead we use the intersection of the win32 visible and win32 clip 
 *       region for the PM clip region.
 *       The original Win32 clip region is saved in pHps->hrgnWin32Clip.
 *       The Win32 visible region is saved in pHps->hrgnWinVis.
 * 
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define  INCL_GPI
#define  INCL_WIN
#include <os2wrap.h> //need odin wrappers

#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <misc.h>
#include <objhandle.h>
#include <dcdata.h>
#include <winuser32.h>
#include "oslibgpi.h"
#include <stats.h>
#include "dibsect.h"
#include <wingdi32.h>

#define DBG_LOCALLOG    DBG_region
#include "dbglocal.h"

//GPI and Windows use the same values
#define GPI_TO_WIN_COMPLEXITY(a)	a

typedef enum
{
    AS_DEVICE,
    AS_WORLD
} InterpMode;

#define MEM_HPS_MAX 768

int WIN32API OffsetRgn( HRGN  hrgn, int  xOffset, int  yOffset);

static void convertDeviceRect(HWND hwnd, pDCData pHps, PRECTL pRectl, ULONG count);

#define convertWinDeviceRectToPMDeviceRect(arg1, arg2, arg3) convertDeviceRect(arg1, arg2, arg3)
#define convertPMDeviceRectToWinDeviceRect(arg1, arg2, arg3) convertDeviceRect(arg1, arg2, arg3)

inline void convertDeviceRect(pDCData pHps, PRECTL pRectl, ULONG count)
{
   convertDeviceRect(0, pHps, pRectl, count);
}
inline void convertDeviceRect(HWND hwnd, PRECTL pRectl, ULONG count)
{
   convertDeviceRect(hwnd, 0, pRectl, count);
}

HPS hpsRegion = 0;


#ifdef DEBUG
//#define dprintfRegion(a,b,c) if(DbgEnabledLvl2GDI32[DBG_LOCALLOG] == 1) dprintfRegion1(a,b,c)
//#define dprintfRegion(a,b,c) dprintfRegion1(a,b,c)

void dprintfRegion(HPS hps, HRGN hrgnClip)
{
 RGNRECT rgnRect = {0, 16, 0, RECTDIR_LFRT_TOPBOT};
 RECTL   rectRegion[16];
 APIRET  rc;

   dprintf(("dprintfRegion %x", hps));
   rc = GpiQueryRegionRects(hps, hrgnClip, NULL, &rgnRect, &rectRegion[0]);
   for(int i=0;i<rgnRect.crcReturned;i++) {
        dprintf(("(%d,%d)(%d,%d)", rectRegion[i].xLeft, rectRegion[i].yBottom, rectRegion[i].xRight, rectRegion[i].yTop));
   }
}
#else
#define dprintfRegion(a,c)
#endif

//******************************************************************************
//******************************************************************************
BOOL InitRegionSpace()
{
    hpsRegion = WinGetScreenPS(HWND_DESKTOP);
    return hpsRegion != 0;
}
//******************************************************************************
//******************************************************************************
void DestroyRegionSpace()
{
    if(hpsRegion) WinReleasePS(hpsRegion);
    hpsRegion = 0;
}
//******************************************************************************
//******************************************************************************
LONG hdcHeight(HWND hwnd, pDCData pHps)
{
    if(hwnd == 0 && pHps != 0)
        hwnd = pHps->hwnd;

    if(hwnd != 0 || pHps == 0)
    {
        RECT rect;
        RECTL rectl;
        LONG y = 0;

        if(pHps && pHps->isClient) //client area
        {
            if(GetClientRect(OS2ToWin32Handle(hwnd), &rect) == TRUE) {
                y = rect.bottom - rect.top;
            }
        }
        else
        if(WinQueryWindowRect(hwnd, &rectl))    //whole window
            y = rectl.yTop;

        return y;
   }
   else
   if(pHps->bitmapHandle)
   {
      return pHps->bitmapHeight;
   }
   else
   if(pHps->isMetaPS)
   {
      return 0;
   }
   else
   if(pHps->isPrinter)
   {
      return GetDeviceCaps(pHps->hps, VERTRES_W);
   }
   else
   {
      return MEM_HPS_MAX;
   }
}
//******************************************************************************
//******************************************************************************
LONG hdcWidth(HWND hwnd, pDCData pHps)
{
    if(hwnd == 0 && pHps != 0)
        hwnd = pHps->hwnd;

    if(hwnd != 0 || pHps == 0)
    {
        RECT rect;
        RECTL rectl;
        LONG x = 0;

        if(pHps && pHps->isClient) //client area
        {
            if(GetClientRect(OS2ToWin32Handle(hwnd), &rect) == TRUE) {
                x = rect.right - rect.left;
            }
        }
        else
        if(WinQueryWindowRect(hwnd, &rectl))    //whole window
            x = rectl.xRight;

        return x;
   }
   else
   if(pHps->bitmapHandle)
   {
      return pHps->bitmapWidth;
   }
   else
   if(pHps->isMetaPS)
   {
      return 0;
   }
   else
   if(pHps->isPrinter)
   {
      return GetDeviceCaps(pHps->hps, HORZRES_W);
   }
//   else
//     DebugInt3();
   return 0;
}
//******************************************************************************
//******************************************************************************
static void convertWinWorldRectToPMDeviceRect(pDCData pHps_Hdc, PRECTL pRects, UINT count = 1)
{
   PRECTL pRectl;
   for (int x=0; x < count; x++)
   {
        pRectl = &pRects[x];

        if(pRectl->yTop < pRectl->yBottom) {
            ULONG temp      = pRectl->yBottom;
            pRectl->yBottom = pRectl->yTop;
            pRectl->yTop    = temp;
        }
        BOOL success = GpiConvert( pHps_Hdc->hps, CVTC_WORLD, CVTC_DEVICE, 2, (PPOINTL)pRectl);
        if(!success)
        {
            break;
        }
        if(pRectl->yTop < pRectl->yBottom) {
            ULONG temp      = pRectl->yBottom;
            pRectl->yBottom = pRectl->yTop;
            pRectl->yTop    = temp;
        }
   }
}
//******************************************************************************
//******************************************************************************
static void convertDeviceRect(HWND hwnd, pDCData pHps, PRECTL pRectl, ULONG count)
{
   long wHeight, wWidth;

    if(hwnd == HWND_DESKTOP) {
        wHeight = OSLibGetScreenHeight();
        wWidth  = OSLibGetScreenWidth();
    }
    else {
        wHeight = hdcHeight(hwnd, pHps);
        wWidth  = hdcWidth(hwnd, pHps);
    }

    if(pHps)
    {
        wHeight += pHps->HPStoHDCInversionHeight;
    }

    if(hwnd || (pHps && pHps->hwnd)) {
        for(int x=0; x < count; x++)
        {
            if(pRectl[x].xRight > wWidth) {
                 pRectl[x].xRight = wWidth;
            }
            if(pRectl[x].yTop > wHeight) {
                 pRectl[x].yTop = 0;
            }
            else pRectl[x].yTop    = wHeight - pRectl[x].yTop;

            pRectl[x].yBottom = wHeight - pRectl[x].yBottom;
        }
    }
    else {
        for(int x=0; x < count; x++)
        {
            pRectl[x].yTop    = wHeight - pRectl[x].yTop;
            pRectl[x].yBottom = wHeight - pRectl[x].yBottom;
        }
    }
}
//******************************************************************************
//******************************************************************************
static void convertPMDeviceRectToWinWorldRect(pDCData pHps_Hdc, PRECTL pRectl)
{
   GpiConvert(pHps_Hdc->hps, CVTC_DEVICE, CVTC_WORLD, 2, (PPOINTL)pRectl );
}
//******************************************************************************
//******************************************************************************
BOOL interpretRegionAs(pDCData pHps, HRGN hrgnDest, HRGN hrgnSrc, InterpMode mode)
{
 BOOL success = FALSE;
 int  x, i;
 HPS  hpsDest;

    if(pHps)
    {
        if(!hrgnDest) {
                hrgnDest = pHps->hrgnHDC;
#ifdef DEBUG
                if(hrgnDest == 0) {
                    DebugInt3();
                }
#endif
        }
        hpsDest  = pHps->hps;
    }
    else {
        DebugInt3();
        return FALSE;
    }

    RGNRECT  rgnRect;
    rgnRect.ircStart    = 1;
    rgnRect.crc         = 0;
    rgnRect.ulDirection = RECTDIR_LFRT_TOPBOT;
    success = GpiQueryRegionRects(hpsRegion, hrgnSrc, NULL, &rgnRect, NULL);
    if(success)
    {
        if(rgnRect.crcReturned > 0)
        {
            rgnRect.crc = rgnRect.crcReturned;
            PRECTL pRectl = new RECTL[rgnRect.crcReturned];
            success = GpiQueryRegionRects(hpsRegion, hrgnSrc, NULL, &rgnRect, pRectl);
            if(success)
            {
                if(mode == AS_DEVICE)
                {
                    ULONG temp;
                    for(x=0; x < rgnRect.crcReturned; x++)
                    {
                        temp = pRectl[x].yBottom;
                        pRectl[x].yBottom = pRectl[x].yTop;
                        pRectl[x].yTop    = temp;
                    }
                    convertWinDeviceRectToPMDeviceRect(pHps, pRectl, rgnRect.crcReturned);
                }
                else
                {
                    GpiConvert(pHps->hps, CVTC_WORLD, CVTC_DEVICE, rgnRect.crcReturned<<1, (PPOINTL)pRectl);

                    ULONG temp;
                    for(i=0; i < rgnRect.crcReturned; i++)
                    {
                        if(pRectl[i].yTop < pRectl[i].yBottom)
                        {
                            temp              = pRectl[i].yBottom + 1;
                            pRectl[i].yBottom = pRectl[i].yTop + 1;
                            pRectl[i].yTop    = temp;
                        }
                        else
                        {
                            pRectl[i].yTop++;
                            pRectl[i].yBottom++;
                        }
                    }
                }
                success = GpiSetRegion(hpsDest, hrgnDest, rgnRect.crcReturned, pRectl);
            }
            delete[] pRectl;
        }
        else
        {
            success = GpiSetRegion(hpsDest, hrgnDest, 0, NULL);
        }
    }
    return success;
}
//******************************************************************************
//exported (used by user32)
//******************************************************************************
BOOL WIN32API setWinDeviceRegionFromPMDeviceRegion(HRGN winHrgn, HRGN pmHrgn, pDCData pHpsPmHrgn, HWND hwndPmHrgn)
{
    BOOL    success;
    int     x;
    BOOL    mustReleaseScreenPS = FALSE;
    HPS     hps;

    if(pHpsPmHrgn || hwndPmHrgn)
    {
        if(pHpsPmHrgn)
            hps = pHpsPmHrgn->hps;
        else
        {
            hps = WinGetScreenPS(HWND_DESKTOP);
            mustReleaseScreenPS = TRUE;
        }

        RGNRECT  rgnRect;
        rgnRect.ircStart    = 1;
        rgnRect.crc         = 0;
        rgnRect.ulDirection = RECTDIR_LFRT_TOPBOT;
        success = GpiQueryRegionRects(hps, pmHrgn, NULL, &rgnRect, NULL);
        if(success )
        {
            if(rgnRect.crcReturned > 0)
            {
                rgnRect.crc = rgnRect.crcReturned;
                PRECTL pRectl = new RECTL[rgnRect.crcReturned];
                success = GpiQueryRegionRects(hps, pmHrgn, NULL, &rgnRect, pRectl);
                if(success)
                {
                    if(pHpsPmHrgn)
                        convertPMDeviceRectToWinDeviceRect(pHpsPmHrgn, pRectl, rgnRect.crcReturned);
                    else
                        convertPMDeviceRectToWinDeviceRect(hwndPmHrgn, pRectl, rgnRect.crcReturned);

                    for(x=0; x < rgnRect.crcReturned; x++)
                    {
                        ULONG temp         = pRectl[x].yBottom;
                        pRectl[x].yBottom  = pRectl[x].yTop;
                        pRectl[x].yTop     = temp;
                    }

                    success = GpiSetRegion(hpsRegion, winHrgn, rgnRect.crcReturned, pRectl);
                }
                delete[] pRectl;
            }
            else
            {
                success = GpiSetRegion(hpsRegion, winHrgn, 0, NULL);
            }
        }
    }
    else
        success = FALSE;

    if(mustReleaseScreenPS)
        WinReleasePS(hps);

   return success;
}
//******************************************************************************
// GdiSetVisRgn
//
// Change the Win32 visible region to the specified region
//
// Parameters:
// 
// pDCData pHps		- presentation space structure
// HRGN hrgn		- region handle (GPI)
//
//
// Returns:  	- ERROR_W         -> failure
//      	- NULLREGION_W    -> empty region
//              - SIMPLEREGION_W  -> rectangular region
//              - COMPLEXREGION_W -> complex region
//
// NOTE: In windows there is a clip and a visible region (just like in PM)
//       We do not map the Win32 visible region to the PM visible region
//       as that's rather annoying.
//       Instead we use the intersection of the win32 visible and win32 clip 
//       region for the PM clip region.
//       The Win32 clip region is saved in pHps->hrgnWin32Clip.
//       The Win32 visible region is saved in pHps->hrgnWinVis.
//
//******************************************************************************
INT WIN32API GdiSetVisRgn(pDCData pHps, HRGN hrgn)
{
    HRGN hrgnNewClip = NULLHANDLE, hrgnOldClip = NULLHANDLE;
    LONG lComplexity = RGN_NULL;
    RECTL rectl = {0, 0, 1, 1};

    if(pHps == NULL) {
        DebugInt3();
        return ERROR_W;
    }

    if(hrgn != NULLHANDLE) 
    {
        hrgnNewClip = GpiCreateRegion(pHps->hps, 1, &rectl);
        if(hrgnNewClip == NULLHANDLE) {
            dprintf(("ERROR: GdiCombineVisRgn: GpiCreateRegion failed!!"));
            DebugInt3();
            goto failure;
        }
        //make a copy of the new visible region
        lComplexity = GpiCombineRegion(pHps->hps, hrgnNewClip, hrgn, NULLHANDLE, CRGN_COPY);
    }
    else {
        hrgnNewClip = NULLHANDLE;
    }

    //and set that as the GPI clip region
    lComplexity = GpiSetClipRegion(pHps->hps, hrgnNewClip, &hrgnOldClip);
    if (lComplexity != RGN_ERROR)
    {
        //SvL: Must check if origin changed here. Sometimes happens when
        //     window looses focus. (don't know why....)
        checkOrigin(pHps);

        if(hrgnOldClip) GpiDestroyRegion(pHps->hps, hrgnOldClip);

        dprintf(("New visible region %x", hrgn));
        pHps->hrgnWinVis = hrgn;
        return GPI_TO_WIN_COMPLEXITY(lComplexity);
    }
failure:
    if(hrgnNewClip) GpiDestroyRegion(pHps->hps, hrgnNewClip);

    return ERROR_W;
}
//******************************************************************************
// GdiCombineVisRgn
//
// Combine the specified region with the visible region according to the operation
//
// Parameters:
// 
// pDCData pHps		- presentation space structure
// HRGN hrgn		- region handle (Win32)
// INT operation	- combine operation (RGN_*)
//
//
// Returns:  	- ERROR_W         -> failure
//      	- NULLREGION_W    -> empty region
//              - SIMPLEREGION_W  -> rectangular region
//              - COMPLEXREGION_W -> complex region
//
// NOTE: In windows there is a clip and a visible region (just like in PM)
//       We do not map the Win32 visible region to the PM visible region
//       as that's rather annoying.
//       Instead we use the intersection of the win32 visible and win32 clip 
//       region for the PM clip region.
//       The Win32 clip region is saved in pHps->hrgnWin32Clip.
//       The Win32 visible region is saved in pHps->hrgnWinVis.
//
//******************************************************************************
INT WIN32API GdiCombineVisRgn(pDCData pHps, HRGN hrgn, INT operation)
{
#ifdef DEBUG
    HRGN hrgn1 = hrgn;
#endif
    LONG lComplexity = RGN_NULL;

    if(pHps == NULL) {
        DebugInt3();
        return ERROR_W;
    }

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR || !pHps) {
        dprintf(("ERROR: GdiCombineVisRgn %x invalid handle", hrgn1));
        return ERROR_W;
    }
    //interpretRegionAs converts the region and saves it in pHps->hrgnHDC
    if(!interpretRegionAs(pHps, 0, hrgn, AS_DEVICE) )
    {
        dprintf(("ERROR: interpretRegionAs failed!!"));
        return ERROR_W;
    }

    //If there is no visible region, then create one that covers the whole DC
    if(pHps->hrgnWinVis == NULLHANDLE) 
    {
         RECTL rectl = {0, 0, hdcWidth(0, pHps), hdcHeight(0, pHps)};
         dprintf(("No visible region: create one (0,0)(%d,%d)", rectl.xRight, rectl.yTop));
         pHps->hrgnWinVis = GpiCreateRegion(pHps->hps, 1, &rectl);
         if(pHps->hrgnWinVis == NULLHANDLE) {
             dprintf(("ERROR: GdiCombineVisRgn: GpiCreateRegion failed!!"));
             DebugInt3();
             goto failure;
         }
    }

    LONG lMode;
    switch (operation)
    {
    case  RGN_AND_W  : lMode = CRGN_AND ; break;
    case  RGN_DIFF_W : lMode = CRGN_DIFF; break;
    default:
        dprintf(("ERROR: GdiCombineVisRgn %d invalid parameter", operation));
        DebugInt3();
        goto failure;
    }
    lComplexity = GpiCombineRegion(pHps->hps, pHps->hrgnWinVis, pHps->hrgnWinVis, pHps->hrgnHDC, lMode);
    if (lComplexity != RGN_ERROR)
    {
        return GdiSetVisRgn(pHps, pHps->hrgnWinVis);
    }
    dprintf(("ERROR: GdiCombineVisRgn: GpiCombineRegion failed"));
    DebugInt3();

failure:
    return ERROR_W;
}
//******************************************************************************
// GdiCombineVisRgnClipRgn
//
// Combine the specified clip region with the visible region according to the operation
//
// Parameters:
// 
// pDCData pHps		- presentation space structure
// HRGN hrgn		- region handle (GPI)
// INT operation	- combine operation (RGN_*)
//
//
// Returns:  	- ERROR_W         -> failure
//      	- NULLREGION_W    -> empty region
//              - SIMPLEREGION_W  -> rectangular region
//              - COMPLEXREGION_W -> complex region
//
// NOTE: In windows there is a clip and a visible region (just like in PM)
//       We do not map the Win32 visible region to the PM visible region
//       as that's rather annoying.
//       Instead we use the intersection of the win32 visible and win32 clip 
//       region for the PM clip region.
//       The Win32 clip region is saved in pHps->hrgnWin32Clip.
//       The Win32 visible region is saved in pHps->hrgnWinVis.
//
//******************************************************************************
INT WIN32API GdiCombineVisRgnClipRgn(pDCData pHps, HRGN hrgn, INT operation)
{
    HRGN hrgnClip = NULL, hrgnOldClip, hrgnNewClip;
    LONG lComplexity = RGN_NULL;

    //Create a region that will be used for the new GPI clip region
    RECTL rectl = {0, 0, 1, 1};
    hrgnNewClip = GpiCreateRegion(pHps->hps, 1, &rectl);
    if(hrgnNewClip == NULLHANDLE) {
        dprintf(("ERROR: GdiCombineVisRgnClipRgn: GpiCreateRegion failed!!"));
        DebugInt3();
        return ERROR_W;
    }

    if(hrgn == NULLHANDLE) 
    {
        if(pHps->hrgnWin32Clip != NULL) 
        {
            //Only reset the path if both the visible and clip regions are zero
            if(!pHps->hrgnWinVis) {
                GpiSetClipPath(pHps->hps, 0, SCP_RESET);

                GpiDestroyRegion(pHps->hps, hrgnNewClip);
                hrgnNewClip = NULLHANDLE;
            }
            else 
            {//set PM clip region to Win32 visible region
                lComplexity = GpiCombineRegion(pHps->hps, hrgnNewClip, pHps->hrgnWinVis, NULLHANDLE, CRGN_COPY);
                if(lComplexity == RGN_ERROR) {
                    dprintf(("ERROR: GdiCombineVisRgnClipRgn: GpiSetClipRegion failed!!"));
                    DebugInt3();
                    goto failure;
                }
            }
            dprintfRegion(pHps->hps, hrgnNewClip);
            lComplexity = GpiSetClipRegion(pHps->hps, hrgnNewClip, &hrgnOldClip);
            if(lComplexity == RGN_ERROR) {
                dprintf(("ERROR: GdiCombineVisRgnClipRgn: GpiSetClipRegion failed!!"));
                DebugInt3();
            }

            //SvL: Must check if origin changed here. Sometimes happens when
            //     window looses focus. (don't know why....)
            checkOrigin(pHps);
            if(hrgnOldClip) GpiDestroyRegion(pHps->hps, hrgnOldClip);
        }
        //else already NULL, so nothing to do.

        pHps->hrgnWin32Clip = hrgn;
        return NULLREGION_W;
    }

    LONG lMode;
    switch (operation)
    {
    case  RGN_AND_W  : lMode = CRGN_AND ; break;  //intersect clip & visible region
    default:
        dprintf(("ERROR: GdiCombineVisRgnClipRgn %d invalid parameter", operation));
        DebugInt3();
        goto failure;
    }
    // If there's no visible region (meaning entire DC is visible), then just set 
    // the GPI clip region to the win32 clip region
    if (pHps->hrgnWinVis == NULLHANDLE) 
    {
         lComplexity = GpiCombineRegion(pHps->hps, hrgnNewClip, hrgn, NULLHANDLE, CRGN_COPY);
    }
    else lComplexity = GpiCombineRegion(pHps->hps, hrgnNewClip, pHps->hrgnWinVis, hrgn, lMode);

    if (lComplexity != RGN_ERROR)
    {
        dprintfRegion(pHps->hps, hrgnNewClip);
        //And activate the new clip region
        lComplexity = GpiSetClipRegion(pHps->hps, hrgnNewClip, &hrgnOldClip);
        if (lComplexity == RGN_ERROR )
        {
            dprintf(("ERROR: GdiCombineVisRgnClipRgn: GpiSetClipRegion failed"));
            DebugInt3();
            goto failure;
        }
        //SvL: Must check if origin changed here. Sometimes happens when
        //     window looses focus. (don't know why....)
        checkOrigin(pHps);

        if(hrgnOldClip) GpiDestroyRegion(pHps->hps, hrgnOldClip);

        pHps->hrgnWin32Clip = hrgn;

        return GPI_TO_WIN_COMPLEXITY(lComplexity);
    }
    dprintf(("ERROR: GdiCombineVisRgnClipRgn: GpiCombineRegion failed"));
    DebugInt3();

failure:
    if(hrgnNewClip) GpiDestroyRegion(pHps->hps, hrgnNewClip);

    return ERROR_W;
}
//******************************************************************************
//******************************************************************************
int WIN32API ExtSelectClipRgn(HDC hdc, HRGN hrgn, int mode)
{
#ifdef DEBUG
   HRGN hrgn1 = hrgn;
#endif

   pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
   if (!pHps)
   {
        dprintf(("WARNING: ExtSelectRgn %x %x %d invalid hdc", hdc, hrgn, mode));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
   }

   LONG lComplexity;
   HRGN hrgnCurrent = NULLHANDLE, hrgnOld = NULLHANDLE;

   if(!hrgn && mode != RGN_COPY_W)
   {
        SetLastError(ERROR_INVALID_PARAMETER_W);
        dprintf(("WARNING: ExtSelectRgn %x %x %d invalid parameter", hdc, hrgn1, mode));
        return ERROR_W;
   }

   LONG lMode;
   switch (mode)
   {
        case  RGN_AND_W  : lMode = CRGN_AND ; break;
        case  RGN_COPY_W : lMode = CRGN_COPY; break;
        case  RGN_DIFF_W : lMode = CRGN_DIFF; break;
        case  RGN_OR_W   : lMode = CRGN_OR  ; break;
        case  RGN_XOR_W  : lMode = CRGN_XOR ; break;
        default:
        {
            SetLastError(ERROR_INVALID_PARAMETER_W);
            dprintf(("WARNING: ExtSelectRgn %x %x %d invalid parameter", hdc, hrgn1, mode));
            return ERROR_W;
        }
   }

   if(hrgn)
   {
        hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
        if(hrgn == HANDLE_OBJ_ERROR) {
            dprintf(("WARNING: ExtSelectRgn %x %x %d invalid region handle", hdc, hrgn1, mode));
            SetLastError(ERROR_INVALID_HANDLE_W);
            return 0;
        }
        dprintfRegion(hdc, hrgn);
   }

   //TODO: metafile recording
   if(hrgn)
   {
        //interpretRegionAs converts the region and saves it in pHps->hrgnHDC
        if(!interpretRegionAs(pHps, 0, hrgn, AS_DEVICE) )
        {
            return ERROR_W;
        }
   }
   else
   {
        //Intersect the Windows clip region with the current visible region
        lComplexity = GdiCombineVisRgnClipRgn(pHps, NULLHANDLE, RGN_AND_W);

        return NULLREGION_W;
   }

   // get current clipping region
   hrgnOld     = pHps->hrgnWin32Clip;
   hrgnCurrent = hrgnOld;

   if(hrgnCurrent == NULLHANDLE)
   {//if none, then create one
        lMode = CRGN_COPY;
        RECTL rectl = {0, 0, 1, 1};
        hrgnCurrent = GpiCreateRegion(pHps->hps, 1, &rectl);
   }

   HRGN hrgnSrc1;
   HRGN hrgnSrc2;
   if(lMode != CRGN_COPY)
   {
        hrgnSrc1 = hrgnCurrent;
        hrgnSrc2 = pHps->hrgnHDC;
   }
   else
   {
        hrgnSrc1 = pHps->hrgnHDC;
        hrgnSrc2 = NULLHANDLE;
   }

   lComplexity = GpiCombineRegion(pHps->hps, hrgnCurrent, hrgnSrc1, hrgnSrc2, lMode);
   if (lComplexity != RGN_ERROR)
   {
        HRGN hrgnOld;

        //Intersect the Windows clip region with the current visible region
        lComplexity = GdiCombineVisRgnClipRgn(pHps, hrgnCurrent, RGN_AND_W);

        SetLastError(ERROR_SUCCESS_W);

        if (lComplexity != RGN_ERROR) {
            return lComplexity;
        }
   }
   GpiDestroyRegion(pHps->hps, hrgnCurrent); //delete newly created region

   SetLastError(ERROR_INVALID_PARAMETER_W); //TODO
   return ERROR_W;
}
//******************************************************************************
//******************************************************************************
int WIN32API SelectClipRgn(HDC hdc, HRGN hrgn)
{
    return ExtSelectClipRgn(hdc, hrgn, RGN_COPY_W);
}
//******************************************************************************
// The GetClipBox function retrieves the dimensions of the tightest bounding 
// rectangle that can be drawn around the current visible area on the device. 
// The visible area is defined by the current clipping region or clip path, as well 
// as any overlapping windows.
//
// NOTE: We simply call GpiQueryClipBox as it behaves just like GetClipBox
//
//******************************************************************************
int WIN32API GetClipBox(HDC hdc, PRECT lpRect)
{
 pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
 RECTL    rectl;
 LONG     lComplexity;
 int      rc;

    if(!hdc || !lpRect || !pHps) {
        dprintf(("GDI32: GetClipBox %x %x ERROR_INVALID_PARAMETER", hdc, lpRect));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return ERROR_W;
    }
    if(pHps->isPrinter)
    {
        //Some printers return the wrong clip box. Return the full page instead.
        //(TODO: which is incorrect!)
        lpRect->left   = 0;
        lpRect->top    = 0;
        lpRect->right  = GetDeviceCaps( hdc, HORZRES_W);
        lpRect->bottom = GetDeviceCaps( hdc, VERTRES_W);
        includeBottomRightPoint(pHps, (PPOINTLOS2)&rectl);
        convertPMDeviceRectToWinWorldRect(pHps, &rectl);

        rc = SIMPLEREGION_W;
    }
    else {
        lComplexity = GpiQueryClipBox(pHps->hps, &rectl);
        if(lComplexity == RGN_ERROR)
        {
            rc = ERROR_W;
        }
        else
        if(lComplexity == RGN_NULL)
        {
            memset(lpRect, 0, sizeof(*lpRect));
            rc = NULLREGION_W;
        }
        else {
#ifndef INVERT
            //Convert coordinates from PM to win32
            if (pHps->yInvert > 0) {
                LONG temp     = pHps->yInvert - rectl.yBottom;
                rectl.yBottom = pHps->yInvert - rectl.yTop;
                rectl.yTop    = temp;
            }
#endif
            //Convert including/including to including/excluding
            includeBottomRightPoint(pHps, (PPOINTLOS2)&rectl);

            lpRect->left   = rectl.xLeft;
            lpRect->right  = rectl.xRight;

            if(rectl.yBottom > rectl.yTop) {
                lpRect->top    = rectl.yTop;
                lpRect->bottom = rectl.yBottom;
            }
            else {
                lpRect->top    = rectl.yBottom;
                lpRect->bottom = rectl.yTop;
            }

            rc = GPI_TO_WIN_COMPLEXITY(lComplexity);
        }
    }
//    if(lpRect->left == 0 && lpRect->top == 0 && lpRect->right == 0 && lpRect->bottom == 0)
//  DebugInt3();
    dprintf(("GDI32: GetClipBox of %X returned %d; (%d,%d)(%d,%d)", hdc, rc, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom));
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipRgn(HDC hdc, HRGN hrgn)
{
   pDCData    pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
   BOOL       success;
   LONG       lComplexity = RGN_RECT;
   HRGN       hrgnClip = NULL, hrgnTemp;
#ifdef DEBUG
   HRGN hrgn1 = hrgn;
#endif

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR || !pHps) {
        dprintf(("WARNING: GetClipRgn %x %x invalid handle", hdc, hrgn1));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }

    if(pHps->hrgnWin32Clip) {
        dprintfRegion(hdc, pHps->hrgnWin32Clip);
        if(!setWinDeviceRegionFromPMDeviceRegion(hrgn, pHps->hrgnWin32Clip, pHps, NULL)) {
            dprintf(("WARNING: GetClipRgn setWinDeviceRegionFromPMDeviceRegion failed!"));
            GpiSetClipRegion(pHps->hps, hrgnClip, &hrgnTemp);
            SetLastError(ERROR_INVALID_PARAMETER_W); //todo right errror
            return 0;
        }
    }
    else {
       //no clip region; return NULL
       lComplexity = RGN_NULL;
    }

    SetLastError(ERROR_SUCCESS_W);
    if(lComplexity == RGN_NULL)
         return 0;
    else return 1;
}
//******************************************************************************
// The GetRandomRgn function copies the system clipping region of a specified 
// device context to a specific region.
// It returns the visible region of the specified device context in screen
// coordinates (if it belongs to a window).
//
// VALUE = 1: This undocumented value will return the current Clip Region contained in the DC.
// VALUE = 2: This undocumented value will return the current Meta Region contained in the DC.
// VALUE = 3: This undocumented value will return the intersection of the Clip Region and Meta Region. 
// VALUE = 4, SYSRGN: The only value that is documented and defined for this function. 
//                    This value returns the System Region
//******************************************************************************
INT WIN32API GetRandomRgn(HDC hdc, HRGN hrgn, INT iNum)
{
    HWND hwnd;
    INT  ret;

    if(iNum != SYSRGN_W) {
        dprintf(("WARNING: GetRandomRgn: invalid parameter %x", iNum));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return ERROR_W;
    }
    ret = GetClipRgn(hdc, hrgn);

    hwnd = WindowFromDC(hdc);
    if(hwnd) {
        POINT pt = {0,0};
        //map from client to desktop coordinates
        MapWindowPoints(hwnd, 0, &pt, 1);

        OffsetRgn(hrgn, pt.x, pt.y);
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
int WIN32API ExcludeClipRect(HDC  hdc, int  left, int  top, int  right, int bottom)
{
    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("ERROR: ExcludeClipRgn %x (%d,%d)(%d,%d) invalid hdc", hdc, left, top, right, bottom));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
    }

    LONG   lComplexity;
    RECTL  rectl = { left, bottom, right, top };

    excludeBottomRightPoint(pHps, (PPOINTLOS2)&rectl);
    if(rectl.yTop < rectl.yBottom) {
        ULONG temp    = rectl.yBottom;
        rectl.yBottom = rectl.yTop;
        rectl.yTop    = temp;
    }

#ifndef INVERT
    if (pHps->yInvert > 0) {
        LONG temp     = pHps->yInvert - rectl.yBottom;
        rectl.yBottom = pHps->yInvert - rectl.yTop;
        rectl.yTop    = temp;
    }
#endif

    dprintf(("ExcludeClipRgn %x (%d,%d)(%d,%d)", hdc, left, top, right, bottom));

    lComplexity = GpiExcludeClipRectangle(pHps->hps, &rectl);
    if (lComplexity == RGN_ERROR) {
        dprintf(("ERROR: ExcludeClipRgn: GpiExcludeClipRectangle failed!!"));
        SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: wrong error
        return ERROR_W;
    }

    //Ok, success. Now update the cached win32 clip region
    if(pHps->hrgnWin32Clip != NULLHANDLE) 
    {
        HRGN hrgnClipRect = GpiCreateRegion(pHps->hps, 1, &rectl);
        if(hrgnClipRect == NULLHANDLE) {
            dprintf(("ERROR: ExcludeClipRgn GpiCreateRegion failed!!"));
            DebugInt3();
            SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: wrong error
            return ERROR_W;
        }
        //subtract rect region from clip region
        lComplexity = GpiCombineRegion(pHps->hps, pHps->hrgnWin32Clip, pHps->hrgnWin32Clip, hrgnClipRect, CRGN_DIFF);
        GpiDestroyRegion(pHps->hps, hrgnClipRect);

        if (lComplexity == RGN_ERROR) {
            dprintf(("ERROR: ExcludeClipRgn GpiCombineRegion failed!!"));
            DebugInt3();
            SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: wrong error
            return ERROR_W;
        }
    }

    //todo metafile recording

    SetLastError(ERROR_SUCCESS_W);
    return lComplexity;
}
//******************************************************************************
//******************************************************************************
int WIN32API IntersectClipRect(HDC hdc, int left, int top, int right, int bottom)
{
    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    LONG      lComplexity;
    BOOL      success;

    if(!pHps) {
        dprintf(("WARNING: IntersectClipRect %x (%d,%d)(%d,%d) invalid hdc", hdc, left, top, right, bottom));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
    }
    RECTL rectl = { left, bottom, right, top };

    excludeBottomRightPoint(pHps, (PPOINTLOS2)&rectl);
    if(rectl.yTop < rectl.yBottom) {
        ULONG temp    = rectl.yBottom;
        rectl.yBottom = rectl.yTop;
        rectl.yTop    = temp;
    }

#ifndef INVERT
    //Convert coordinates from PM to win32
    if (pHps->yInvert > 0) {
         LONG temp     = pHps->yInvert - rectl.yBottom;
         rectl.yBottom = pHps->yInvert - rectl.yTop;
         rectl.yTop    = temp;
    }
#endif

    dprintf(("IntersectClipRect %x (%d,%d)(%d,%d)", hdc, left, top, right, bottom));
    lComplexity = GpiIntersectClipRectangle(pHps->hps, &rectl);
    if (lComplexity == RGN_ERROR) {
        dprintf(("ERROR: IntersectClipRect: GpiIntersectClipRectangle failed!!"));
        SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: wrong error
        return ERROR_W;
    }

    //Ok, success. Now update the cached win32 clip region
    if(pHps->hrgnWin32Clip != NULLHANDLE) 
    {
        HRGN hrgnClipRect = GpiCreateRegion(pHps->hps, 1, &rectl);
        if(hrgnClipRect == NULLHANDLE) {
            dprintf(("ERROR: IntersectClipRect GpiCreateRegion failed!!"));
            DebugInt3();
            SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: wrong error
            return ERROR_W;
        }
        //intersect rect region with clip region
        lComplexity = GpiCombineRegion(pHps->hps, pHps->hrgnWin32Clip, pHps->hrgnWin32Clip, hrgnClipRect, CRGN_AND);
        GpiDestroyRegion(pHps->hps, hrgnClipRect);

        if (lComplexity == RGN_ERROR) {
            dprintf(("ERROR: IntersectClipRect GpiCombineRegion failed!!"));
            DebugInt3();
            SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: wrong error
            return ERROR_W;
        }
    }

    //todo metafile recording

    SetLastError(ERROR_SUCCESS_W);
    return lComplexity;
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetClipRgn(HDC  hdc, int  nXOffset, int  nYOffset )
{
   BOOL    success;
   pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
   LONG    lComplexity;

   if(!pHps) {
        dprintf(("OffsetClipRgn %x (%d,%d) invalid hdc", hdc, nXOffset, nYOffset));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
   }

   dprintf(("OffsetClipRgn %x (%d,%d)", hdc, nXOffset, nYOffset));
   POINTL  pointl = { nXOffset, nYOffset };
#ifndef INVERT
   if (pHps->yInvert > 0) {
        pointl.y = pHps->yInvert - pointl.y;
   }
#endif

   if(pHps->hrgnWin32Clip == NULLHANDLE) {
       lComplexity = NULLREGION_W;
   }
   else {
       lComplexity = GpiOffsetRegion(pHps->hps, pHps->hrgnWin32Clip, &pointl);
       if (lComplexity == RGN_ERROR) {
           dprintf(("ERROR: OffsetClipRgn: GpiOffsetRegion failed!!"));
           SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: wrong error
           return ERROR_W;
       }

       //Intersect the Windows clip region with the current visible region
       lComplexity = GdiCombineVisRgnClipRgn(pHps, pHps->hrgnWin32Clip, RGN_AND_W);

       lComplexity = GPI_TO_WIN_COMPLEXITY(lComplexity);
   }

   //todo metafile recording

   SetLastError(ERROR_SUCCESS_W);
   return lComplexity;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreatePolyPolygonRgn(const POINT *lppt, const int *pPolyCount, 
                                   int  nCount, int fnPolyFillMode)
{
 LONG  flMode;

   switch(fnPolyFillMode)
   {
   case ALTERNATE_W:
        flMode = POLYGON_ALTERNATE;
        break;
   case WINDING_W:
        flMode = POLYGON_WINDING;
        break;
   default:
        dprintf(("WARNING: CreatePolyPolygonRgn %x %x %d %d invalid parameter", lppt, pPolyCount, nCount, fnPolyFillMode));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return 0;
   }

   if(nCount < 1)
   {
        dprintf(("WARNING: CreatePolyPolygonRgn %x %x %d %d invalid parameter", lppt, pPolyCount, nCount, fnPolyFillMode));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return 0;
   }

   PPOLYGON pPolygon = new POLYGON[nCount];
   if(!pPolygon) {
        dprintf(("ERROR: CreatePolyPolygonRgn: out of memory!!"));
        SetLastError(ERROR_OUTOFMEMORY_W);
        return 0;
   }
   dprintf(("CreatePolyPolygonRgn %x %x %d %d", lppt, pPolyCount, nCount, fnPolyFillMode));

   PPOINTL pPointl = (PPOINTL)lppt+1;     // skip first point
   for (int x=0; x < nCount; x++) {
        pPolygon[x].ulPoints = (x == 0) ? pPolyCount[x] - 1 : pPolyCount[x];
        pPolygon[x].aPointl  = pPointl;
        pPointl += pPolygon[x].ulPoints;
   }
   GpiMove(hpsRegion, (PPOINTL)lppt);
   HRGN hrgn = GpiCreatePolygonRegion(hpsRegion, nCount, pPolygon, POLYGON_BOUNDARY | flMode);

   delete[] pPolygon;

   if(!hrgn) {
        SetLastError(ERROR_INVALID_PARAMETER_W); //todo: not right
        return 0;
   }

   if(ObjAllocateHandle(&hrgn, hrgn, HNDL_REGION) == FALSE) {
        GpiDestroyRegion(hpsRegion, hrgn);
        SetLastError(ERROR_OUTOFMEMORY_W);
        return 0;
   }
   STATS_CreatePolyPolygonRgn(hrgn, lppt, pPolyCount, nCount, fnPolyFillMode);

   SetLastError(ERROR_SUCCESS_W);
   return hrgn;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgn(int  left, int  top, int  right, int  bottom)
{
 HRGN   hrgn;
 RECTL  rectl = { left, top < bottom ? top : bottom, right, top < bottom ? bottom : top };

   hrgn = GpiCreateRegion(hpsRegion, 1, &rectl);
   if(!hrgn) {
        dprintf(("WARNING: CreateRectRgn: GpiCreateRectRegion failed! %x", WinGetLastError(0)));
        SetLastError(ERROR_INVALID_PARAMETER_W); //todo: not right
        return 0;
   }

   if(ObjAllocateHandle(&hrgn, hrgn, HNDL_REGION) == FALSE) {
        GpiDestroyRegion(hpsRegion, hrgn);
        SetLastError(ERROR_OUTOFMEMORY_W);
        return 0;
   }
   STATS_CreateRectRgn(hrgn, left, top, right, bottom);

   dprintf(("CreateRectRegion (%d,%d)(%d,%d) returned %x", left, top, right, bottom, hrgn));
   SetLastError(ERROR_SUCCESS_W);
   return hrgn;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgnIndirect( const RECT * lprc)
{
   return CreateRectRgn(lprc->left, lprc->top, lprc->right, lprc->bottom);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRoundRectRgn(int  left, int  top, int  right, int  bottom, int  nWidthEllipse, int  nHeightEllipse)
{
 HRGN   hrgn;
 RECTL  rectl = { left, top < bottom ? top : bottom, right, top < bottom ? bottom : top };

   PPOINTL pPointl = (PPOINTL)&rectl;

   GpiMove(hpsRegion, &pPointl[0]);
   hrgn = GpiCreateRoundRectRegion(hpsRegion, &pPointl[1], labs(nWidthEllipse), labs(nHeightEllipse));

   if(!hrgn) {
        SetLastError(ERROR_INVALID_PARAMETER_W); //todo: not right
        dprintf(("WARNING: CreateRoundRectRgn: GpiCreateRoundRectRegion failed! %x", WinGetLastError(0)));
        return 0;
   }

   if(ObjAllocateHandle(&hrgn, hrgn, HNDL_REGION) == FALSE) {
        GpiDestroyRegion(hpsRegion, hrgn);
        SetLastError(ERROR_OUTOFMEMORY_W);
        return 0;
   }
   STATS_CreateRoundRectRgn(hrgn, left, top, right, bottom, nWidthEllipse, nHeightEllipse);

   dprintf(("CreateRoundRectRegion (%d,%d)(%d,%d) (%d,%d) returned %x", left, top, right, bottom, nWidthEllipse, nHeightEllipse, hrgn));
   SetLastError(ERROR_SUCCESS_W);
   return hrgn;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API ExtCreateRegion(const XFORM_W * pXform, DWORD count, 
                              const RGNDATA * pData)
{
   HRGN hrgn;

    if(!pData || count < (sizeof(RGNDATAHEADER) +  pData->rdh.nCount * sizeof(RECT))) {
        dprintf(("WARNING: ExtCreateRegion %x %d %x; invalid parameter", pXform, count, pData));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return 0;
    }

    PRECTL  pRectl     = new RECTL[pData->rdh.nCount];
    PRECT pDataRects = (PRECT)pData->Buffer;
    for(int i=0; i < pData->rdh.nCount; i++) {
        MapWin32ToOS2Rect(pDataRects[i], pRectl[i]);
    }

    BOOL doShear = pXform && (pXform->eM12 || pXform->eM21);
    HPS  hpsTemp = NULLHANDLE;

    dprintf(("ExtCreateRegion %x %d %x", pXform, count, pData));
    if(doShear) {
         hpsTemp = WinGetPS(HWND_DESKTOP);
         GpiBeginPath(hpsTemp, 1);

         MATRIXLF  matrixlf;
         matrixlf.fxM11  = pXform->eM11 * (float)0x10000;
         matrixlf.fxM12  = pXform->eM12 * (float)0x10000;
         matrixlf.lM13   = 0;
         matrixlf.fxM21  = pXform->eM21 * (float)0x10000;
         matrixlf.fxM22  = pXform->eM22 * (float)0x10000;
         matrixlf.lM23   = 0;
         matrixlf.lM31   = pXform->eDx;
         matrixlf.lM32   = pXform->eDy;
         matrixlf.lM33   = 1;

         GpiSetModelTransformMatrix(hpsTemp, 9, &matrixlf, TRANSFORM_REPLACE);
    }

    for(i=0; i < pData->rdh.nCount; i++)
    {
         LONG temp         = pRectl[i].yTop;
         pRectl[i].yTop    = pRectl[i].yBottom;
         pRectl[i].yBottom = temp;

         if(pXform)
         {
            PPOINTL pPointl = ((PPOINTL)&pRectl[i]);

            if (doShear) {
                GpiMove(hpsTemp, pPointl);
                GpiBox(hpsTemp, DRO_OUTLINE, pPointl+1, 0, 0);
            }
            else
            {
                for(int j=0; j < 2; j++) {
                    pPointl[j].x = (pXform->eM11 * (float)pPointl[j].x) + (pXform->eM12 * (float)pPointl[j].y) + pXform->eDx;
                    pPointl[j].y = (pXform->eM21 * (float)pPointl[j].x) + (pXform->eM22 * (float)pPointl[j].y) + pXform->eDy;
                }

                PRECTL pRectlT = (PRECTL)pPointl;
                if (pRectlT->xLeft > pRectlT->xRight) {
                    LONG temp       = pRectlT->xLeft;
                    pRectlT->xLeft  = pRectlT->xRight;
                    pRectlT->xRight = temp;
                }
                if (pRectlT->yBottom > pRectlT->yTop) {
                    LONG temp         = pRectlT->yBottom;
                    pRectlT->yBottom  = pRectlT->yTop;
                    pRectlT->yTop     = temp;
                }
            }
         }
    }

    if(doShear) {
            GpiEndPath(hpsTemp);
            hrgn = GpiPathToRegion(hpsTemp, 1, FPATH_WINDING);

            if(pRectl)
                delete[] pRectl;

            if(hrgn == 0) {
                dprintf(("WARNING: ExtCreateRegion GpiCreateRegion failed! (%x)", WinGetLastError(0)));
                SetLastError(ERROR_INVALID_PARAMETER_W);
                return 0;
            }
            WinReleasePS(hpsTemp);
    }
    else {
            hrgn = GpiCreateRegion(hpsRegion, pData->rdh.nCount, pRectl);
            if(pRectl)
                delete[] pRectl;

            if(hrgn == 0) {
                dprintf(("WARNING: ExtCreateRegion GpiCreateRegion failed! (%x)", WinGetLastError(0)));
                SetLastError(ERROR_INVALID_PARAMETER_W);
                return 0;
            }
    }

    if(ObjAllocateHandle(&hrgn, hrgn, HNDL_REGION) == FALSE) {
        GpiDestroyRegion(hpsRegion, hrgn);
        SetLastError(ERROR_OUTOFMEMORY_W);
        return 0;
    }
    STATS_ExtCreateRegion(hrgn, (PVOID)pXform, count, pData);
    SetLastError(ERROR_SUCCESS_W);
    return hrgn;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgn(int  left, int  top, int  right, int  bottom)
{
 HRGN hrgn;

    RECTL  rectl = { left,
                     top < bottom ? top : bottom,
                     right,
                     top < bottom ? bottom : top };

    dprintf(("CreateEllipticRgn (%d,%d)(%d,%d)", left, top, right, bottom));
    hrgn = GpiCreateEllipticRegion(hpsRegion, &rectl);
    if(hrgn == RGN_ERROR) {
        SetLastError(ERROR_INVALID_PARAMETER_W); //todo: not right
        dprintf(("WARNING: CreateEllipticRgn: GpiCreateEllipticRegion failed! %x", WinGetLastError(0)));
        return 0;
    }

    if(ObjAllocateHandle(&hrgn, hrgn, HNDL_REGION) == FALSE) {
        GpiDestroyRegion(hpsRegion, hrgn);
        SetLastError(ERROR_OUTOFMEMORY_W);
        return 0;
    }
    STATS_CreateEllipticRgn(hrgn, left, top, right, bottom);
    SetLastError(ERROR_SUCCESS_W);
    return hrgn;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgnIndirect(const RECT *pRect)
{
   return CreateEllipticRgn(pRect->left, pRect->top, pRect->right, pRect->bottom);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreatePolygonRgn(const POINT * lppt, int  cPoints, int  fnPolyFillMode)
{
    HRGN hrgn;
    LONG flMode;

    if(!lppt || cPoints < 2) {
        dprintf(("WARNING: CreatePolygonRgn %x %d %d invalid parameter", lppt, cPoints, fnPolyFillMode));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return 0;
    }

    switch (fnPolyFillMode)
    {
    case ALTERNATE_W :
        flMode = POLYGON_ALTERNATE;
        break;
    case WINDING_W   :
        flMode = POLYGON_WINDING;
        break;
    default:
        dprintf(("WARNING: CreatePolygonRgn %x %d %d invalid parameter", lppt, cPoints, fnPolyFillMode));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return 0;
    }
    dprintf(("CreatePolygonRgn %x %d %d", lppt, cPoints, fnPolyFillMode));

    POLYGON  polygon;
    polygon.ulPoints = cPoints - 1;
    polygon.aPointl  = (PPOINTL)(lppt + 1);

#ifdef DEBUG
    for(int i=0;i<cPoints;i++) {
        dprintf(("Point %d: (%d,%d)", i, lppt[i].x, lppt[i].y));
    }
#endif
    GpiMove(hpsRegion, (PPOINTL)lppt);
    hrgn = GpiCreatePolygonRegion(hpsRegion, 1, &polygon, POLYGON_BOUNDARY | flMode);
    if(hrgn == RGN_ERROR) {
        SetLastError(ERROR_INVALID_PARAMETER_W); //todo: not right
        dprintf(("WARNING: CreatePolygonRgn: GpiCreatePolygonRegion failed! %x", WinGetLastError(0)));
        return 0;
    }

    if(ObjAllocateHandle(&hrgn, hrgn, HNDL_REGION) == FALSE) {
        GpiDestroyRegion(hpsRegion, hrgn);
        SetLastError(ERROR_OUTOFMEMORY_W);
        return 0;
    }
    STATS_CreatePolygonRgn(hrgn, lppt, cPoints, fnPolyFillMode);
    SetLastError(ERROR_SUCCESS_W);
    return hrgn;
}
//******************************************************************************
//******************************************************************************
int WIN32API CombineRgn(HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2, int combineMode)
{
    ULONG lComplexity;
    LONG  mode;
#ifdef DEBUG
   HRGN hrgn1 = hrgnDest;
   HRGN hrgn2 = hrgnSrc1;
   HRGN hrgn3 = hrgnSrc2;
#endif

    switch(combineMode) {
    case RGN_AND_W:
        mode = CRGN_AND;
        break;
    case RGN_COPY_W:
        mode = CRGN_COPY;
        break;
    case RGN_DIFF_W:
        mode = CRGN_DIFF;
        break;
    case RGN_OR_W:
        mode = CRGN_OR;
        break;
    case RGN_XOR_W:
        mode = CRGN_XOR;
        break;
    default:
        dprintf(("WARNING: CombineRgn %x %x %x %d; invalid mode!", hrgnDest, hrgnSrc1, hrgnSrc2, combineMode));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return ERROR_W;
    }

    hrgnDest = ObjQueryHandleData(hrgnDest, HNDL_REGION);
    hrgnSrc1 = ObjQueryHandleData(hrgnSrc1, HNDL_REGION);
    hrgnSrc2 = ObjQueryHandleData(hrgnSrc2, HNDL_REGION);
    if(hrgnDest == HANDLE_OBJ_ERROR || hrgnSrc1 == HANDLE_OBJ_ERROR  || (hrgnSrc2 == HANDLE_OBJ_ERROR && combineMode != RGN_COPY_W)) {
        dprintf(("WARNING: CombineRgn %x %x %x %d invalid region", hrgn1, hrgn2, hrgn3, combineMode));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
    }

    lComplexity = GpiCombineRegion(hpsRegion, hrgnDest, hrgnSrc1, hrgnSrc2, mode);
    if(lComplexity == RGN_ERROR) {
        dprintf(("WARNING: CombineRgn %x %x %x %d GpiCombineRegion failed (%x)", hrgn1, hrgn2, hrgn3, mode, WinGetLastError(0)));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
    }
    SetLastError(ERROR_SUCCESS_W);
    return lComplexity;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EqualRgn(HRGN  hrgn1, HRGN  hrgn2)
{
   LONG lEquality;
#ifdef DEBUG
   HRGN hrgnt1 = hrgn1;
   HRGN hrgnt2 = hrgn2;
#endif

   hrgn1 = ObjQueryHandleData(hrgn1, HNDL_REGION);
   hrgn2 = ObjQueryHandleData(hrgn2, HNDL_REGION);
   if(hrgn1 == HANDLE_OBJ_ERROR || hrgn2 == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: EqualRgn %x %x invalid region", hrgnt1, hrgnt2));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
   }
   lEquality = GpiEqualRegion(hpsRegion, hrgn1, hrgn2);

   dprintf(("EqualRgn %x %x = %d", hrgnt1, hrgnt2, lEquality));
   SetLastError(ERROR_SUCCESS_W);

   if(lEquality == EQRGN_EQUAL)
        return TRUE;
   else
   if(lEquality == EQRGN_NOTEQUAL)
        return FALSE;
   else {
        return FALSE;
   }
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRectRgn(HRGN  hrgn, int  left, int  top, int  right, int  bottom)
{
   BOOL    result = FALSE;
#ifdef DEBUG
   HRGN      hrgn1 = hrgn;
#endif

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: SetRectRgn %x (%d,%d)(%d,%d) invalid region handle", hrgn1, left, top, right, bottom));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }
    RECTL rectl = { left, top, right, bottom }; //reversed y coordinates
    if(GpiSetRegion(hpsRegion, hrgn, 1, &rectl)) {
        dprintf(("SetRectRgn %x (%d,%d)(%d,%d)", hrgn1, left, top, right, bottom));
        return TRUE;
    }
    dprintf(("WARNING: SetRectRgn %x (%d,%d)(%d,%d) GpiSetRegion failed %x", hrgn1, left, top, right, bottom, WinGetLastError(0)));
    return FALSE;
}
//******************************************************************************
//NOTE: depends on hps inversion
//******************************************************************************
ULONG WIN32API GetRegionData( HRGN  hrgn, ULONG count, PRGNDATA pData)
{
#ifdef DEBUG
   HRGN      hrgn1 = hrgn;
#endif

    if(!count && pData)
    {
        dprintf(("WARNING: GetRegionData %x %d %x; invalid parameter", hrgn1, count, pData));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return 0;
    }
    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: GetRegionData %x %d %x; invalid region handle", hrgn1, count, pData));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }
    RGNRECT rgnRect;
    rgnRect.ircStart    = 1;
    rgnRect.crc         = 0;
    rgnRect.ulDirection = RECTDIR_LFRT_TOPBOT;
    if(!GpiQueryRegionRects(hpsRegion, hrgn, NULL, &rgnRect, NULL))
    {
        dprintf(("WARNING: GetRegionData %x %d %x: GpiQueryRegionRects failed! (%x)", hrgn1, count, pData, WinGetLastError(0)));
        return 0;
    }

    dprintf(("GetRegionData %x %d %x", hrgn1, count, pData));

    ULONG bufSizeNeeded = rgnRect.crcReturned * sizeof(RECT) + sizeof (RGNDATAHEADER);

    if(pData && (count >= (sizeof(RGNDATAHEADER) + rgnRect.crcReturned * sizeof(RECT))))
    {
        //we actually need to flip the top & bottom values, but as the layout of the PM RECTL and
        //Win32 RECT have exactly those two members reversed, we don't do this for the returned
        //rectangles (more efficient)
        PRECTL pRectl = (PRECTL)pData->Buffer;
        rgnRect.crc   = rgnRect.crcReturned;
        if(!GpiQueryRegionRects(hpsRegion, hrgn, NULL, &rgnRect, pRectl))
        {
            dprintf(("WARNING: GetRegionData: GpiQueryRegionRects failed! (%x)", WinGetLastError(0)));
            return 0;
        }

        RECTL boundRect;
        GpiQueryRegionBox(hpsRegion, hrgn, &boundRect);  // no need to check rc

        pData->rdh.dwSize   = sizeof(pData->rdh);
        pData->rdh.iType    = RDH_RECTANGLES_W;    // one and only possible value
        pData->rdh.nCount   = rgnRect.crcReturned;
        pData->rdh.nRgnSize = rgnRect.crcReturned * sizeof(RECT);

        //flip top & bottom for bounding rectangle (not really necessary; but cleaner coding)
        LONG temp          = boundRect.yTop;
        boundRect.yTop     = boundRect.yBottom;
        boundRect.yBottom  = temp;
        MapOS2ToWin32Rect(boundRect, pData->rdh.rcBound);
    }

    //return size needed
    return bufSizeNeeded;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetRgnBox( HRGN  hrgn, PRECT pRect)
{
   BOOL      success;
   LONG      lComplexity;
#ifdef DEBUG
   HRGN      hrgn1 = hrgn;
#endif

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: GetRgnBox %x %x invalid region!", hrgn1, pRect));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
    }

    RECTL  rectl;
    lComplexity = GpiQueryRegionBox(hpsRegion, hrgn, &rectl);
    if(lComplexity != RGN_ERROR)
    {
            //no conversion required, just flip top & bottom
            ULONG temp    = rectl.yBottom;
            rectl.yBottom = rectl.yTop;
            rectl.yTop    = temp;

            MapOS2ToWin32Rect(rectl, *pRect);
    }
    else {
            lComplexity = ERROR_W;
            dprintf(("WARNING: GetRgnBox error in region!"));
    }
    dprintf(("GetRgnBox %x (%d,%d)(%d,%d)", hrgn1, pRect->left, pRect->top, pRect->right, pRect->bottom));

    SetLastError(ERROR_SUCCESS_W);
    return lComplexity;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvertRgn( HDC  hdc, HRGN  hrgn)
{
#ifdef DEBUG
    HRGN      hrgn1 = hrgn;
#endif
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(!pHps || hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: InvertRgn %x %x invalid handle!", hdc, hrgn));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }
    //todo metafile recording

    dprintf(("InvertRgn %x %x", hdc, hrgn1));

    interpretRegionAs(pHps, NULL, hrgn, AS_WORLD);

    LONG  lHits = GPI_ERROR;
    HRGN  hrgnOld;
    LONG  lComplexity ;
    RECTL boundingRect;      // this gets a rectangle in world cordinates!

    lComplexity = GpiQueryRegionBox(pHps->hps, pHps->hrgnHDC,  &boundingRect);
    if(lComplexity != RGN_ERROR)
    {
        lComplexity = GpiSetClipRegion(pHps->hps, pHps->hrgnHDC,  &hrgnOld);
        if(lComplexity != RGN_ERROR)
        {
            RECTL rectls[2];
            rectls[0] = boundingRect;
            rectls[1] = boundingRect;
            lHits = GpiBitBlt(pHps->hps, NULL, 3, (PPOINTL)rectls,
                              ROP_DSTINVERT, 0);

            /* Restore the old region */
            GpiSetClipRegion(pHps->hps, hrgnOld, &hrgnOld);
        }
    }

    if(lHits == GPI_ERROR || lComplexity == RGN_ERROR)
    {
        dprintf(("WARNING: InvertRgn error during Gpi operation (%x) (%d,%d)", WinGetLastError(0), lHits, lComplexity));
        return FALSE;
    }

    DIBSECTION_MARK_INVALID(hdc);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetRgn( HRGN  hrgn, int  xOffset, int  yOffset)
{
   LONG   lComplexity;
#ifdef DEBUG
   HRGN      hrgn1 = hrgn;
#endif

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: OffsetRgn %x %d %d invalid handle!", hrgn, xOffset, yOffset));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return ERROR_W;
    }
    dprintf(("OffsetRgn %x %d %d", hrgn1, xOffset, yOffset));

    POINTL   ptlOffset = {xOffset, yOffset};
    GpiOffsetRegion(hpsRegion, hrgn, &ptlOffset);

    RECTL    rectl[8];
    RGNRECT  rgnRect;
    rgnRect.ircStart    = 1;
    rgnRect.crc         = 8;
    rgnRect.ulDirection = RECTDIR_LFRT_TOPBOT;     // doesn't make a difference
    if(GpiQueryRegionRects(hpsRegion, hrgn, NULL, &rgnRect, &rectl[0]))
    {
        switch (rgnRect.crcReturned) {
        case 0:
                lComplexity = NULLREGION_W;
                break;
        case 1:
                lComplexity = SIMPLEREGION_W;
                break;
        default:
                lComplexity = COMPLEXREGION_W;
                break;
        }
   }
   else {
        lComplexity = ERROR_W;
        dprintf(("WARNING: OffsetRgn error in region! (%x)", WinGetLastError(0)));
   }

   SetLastError(ERROR_SUCCESS_W);

   return lComplexity;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FrameRgn(HDC hdc, HRGN hrgn, HBRUSH hBrush, int width, int height)
{
    HBRUSH hbrushRestore = 0;
#ifdef DEBUG
    HRGN   hrgn1 = hrgn;
#endif
    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(!pHps || hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: FrameRgn %x %x %x (%d,%d) invalid handle!", hdc, hrgn, hBrush, width, height));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    width  = abs(width);
    height = abs(height);

    if(pHps->lastBrushKey != (UINT)hBrush)
    {
        hbrushRestore = SelectObject(hdc, hBrush);
        if(!hbrushRestore)
        {
            dprintf(("WARNING: FrameRgn %x %x %x (%d,%d) invalid brush!", hdc, hrgn, hBrush, width, height));
            SetLastError(ERROR_INVALID_HANDLE_W);
            return FALSE;
        }
    }
    dprintf(("FrameRgn %x %x %x (%d,%d)", hdc, hrgn1, hBrush, width, height));
    interpretRegionAs(pHps, NULL, hrgn, AS_WORLD);

    SIZEL  thickness = { width, height };
    LONG   lHits = GpiFrameRegion(pHps->hps, pHps->hrgnHDC,  &thickness);

    SetLastError(ERROR_SUCCESS_W);

    // Restore the brush if necessary
    if(hbrushRestore)
        SelectObject(hdc, hbrushRestore);

    if(lHits != GPI_ERROR) 
        DIBSECTION_MARK_INVALID(hdc);

    //todo metafile recording
    return (lHits != GPI_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FillRgn(HDC hdc, HRGN hrgn, HBRUSH hBrush)
{
 BOOL   success;
 HBRUSH hbrushRestore = 0;
#ifdef DEBUG
    HRGN   hrgn1 = hrgn;
#endif

    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(!pHps || hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: FillRgn %x %x %x invalid handle!", hdc, hrgn1, hBrush));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(pHps->lastBrushKey != (UINT)hBrush)
    {
        hbrushRestore = SelectObject(hdc, hBrush);
        if (!hbrushRestore)
        {
            dprintf(("WARNING: FillRgn %x %x %x invalid brush!", hdc, hrgn1, hBrush));
            SetLastError(ERROR_INVALID_HANDLE_W);
            return FALSE;
        }
    }
    dprintf(("FillRgn %x %x %x", hdc, hrgn1, hBrush));

    DIBSECTION_CHECK_IF_DIRTY(hdc);
    interpretRegionAs(pHps, NULL, hrgn, AS_WORLD);

    dprintfRegion(pHps->hps, pHps->hrgnHDC);

    success = GpiPaintRegion(pHps->hps, pHps->hrgnHDC);

    //todo metafile recording

    /* Restore the brush if necessary */
    if(hbrushRestore)
        SelectObject(hdc, hbrushRestore);

    if(success)
        DIBSECTION_MARK_INVALID(hdc);

    return(success);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PaintRgn( HDC  hdc, HRGN  hrgn)
{
   return FillRgn(hdc, hrgn, (HBRUSH) GetCurrentObject(hdc, OBJ_BRUSH_W));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtInRegion( HRGN  hrgn, int  x, int  y)
{
   BOOL      success;
   LONG      lInside;
#ifdef DEBUG
   HRGN      hrgn1 = hrgn;
#endif

    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: PtInRgn %x (%d,%d) invalid region!", hrgn1, x, y));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    POINTL pointl = {x,y};
    lInside = GpiPtInRegion(hpsRegion, hrgn, &pointl);
    if(lInside == PRGN_ERROR) {
         success = FALSE;
    }
    else success = TRUE;

    SetLastError(ERROR_SUCCESS_W);

    dprintf(("PtInRgn %x (%d,%d) returned %d", hrgn1, x, y, (success && lInside == PRGN_INSIDE) ? 1 : 0));

    if(success && lInside == PRGN_INSIDE)
        return TRUE;
    else
        return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RectInRegion( HRGN  hrgn, const RECT * pRect)
{
   BOOL      success;
   LONG      lInside;
#ifdef DEBUG
   HRGN      hrgn1 = hrgn;
#endif

    if(!pRect) {
        dprintf(("WARNING: RectInRgn %x %x invalid parameter!", hrgn1, pRect));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }
    hrgn = ObjQueryHandleData(hrgn, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: RectInRgn %x %x invalid region", hrgn1, pRect));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    RECTL rectl;
    MapWin32ToOS2Rect(*pRect, rectl);
    //switch bottom & top
    UINT temp     = rectl.yBottom;
    rectl.yBottom = rectl.yTop;
    rectl.yTop    = temp;

    lInside = GpiRectInRegion(hpsRegion, hrgn, &rectl);
    if(lInside == RRGN_ERROR) {
            success = FALSE;
    }
    else    success = TRUE;

    SetLastError(ERROR_SUCCESS_W);

    dprintf(("RectInRgn %x %x returned %d", hrgn1, pRect, (success && (lInside == RRGN_INSIDE || lInside == RRGN_PARTIAL)) ? 1 : 0));

    if(success && (lInside == RRGN_INSIDE || lInside == RRGN_PARTIAL))
        return TRUE;
    else
        return FALSE;
}
//******************************************************************************
//Returned region in device coordinates (undocumented behaviour)
//******************************************************************************
HRGN WIN32API PathToRegion( HDC  hdc)
{
    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: PathToRegion %x; invalid hdc!", hdc));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return NULLHANDLE;
    }

    HRGN hrgn, hrgnTemp, hrgnwin;

    dprintf(("GDI32: PathToRegion %x", hdc));

    hrgnTemp = GpiPathToRegion(pHps->hps, 1, (pHps->polyFillMode == ALTERNATE_W) ? FPATH_ALTERNATE : FPATH_WINDING);
    if(hrgnTemp == NULLHANDLE)
        goto error;

    hrgnwin = CreateRectRgn(1, 1, 2, 2);
    hrgn = ObjQueryHandleData(hrgnwin, HNDL_REGION);
    if(hrgn == HANDLE_OBJ_ERROR) {
        dprintf(("WARNING: PathToRegion invalid region", hrgnwin));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return NULLHANDLE;
    }

    if(!setWinDeviceRegionFromPMDeviceRegion(hrgn, hrgnTemp, pHps, NULL))
        goto error;

    GpiDestroyRegion(pHps->hps, hrgnTemp);

    return hrgnwin;

error:
    if(hrgnwin)
        DeleteObject(hrgnwin);

    SetLastError(ERROR_INVALID_HANDLE_W);       //todo right error
    return NULLHANDLE;
}
//******************************************************************************
//Needs wrapper as this file includes os2.h!!
//******************************************************************************
BOOL WIN32API OSLibDeleteRegion(HANDLE hRegion)
{
    if(GpiDestroyRegion(hpsRegion, hRegion) == FALSE) {
        dprintf(("WARNING: OSLibDeleteRegion %x; GpiDestroyRegion failed (%x)", hRegion, WinGetLastError(0)));
    }
    return (0);
}
/*****************************************************************************
 * Name      : int GetMetaRgn
 * Purpose   : The GetMetaRgn function retrieves the current metaregion for
 *             the specified device context.
 * Parameters: HDC  hdc   handle of device context
 *             HRGN hrgn  handle of region
 * Variables :
 * Result    : 0 / 1
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API GetMetaRgn( HDC   hdc, HRGN  hrgn)
{
  dprintf(("GDI32: GetMetaRgn(%08xh, %08xh) not implemented.\n",
           hdc,
           hrgn));

  SetLastError(ERROR_SUCCESS_W);
  return (0);
}
/*****************************************************************************
 * Name      : int SetMetaRgn
 * Purpose   : The SetMetaRgn function intersects the current clipping region
 *             for the specified device context with the current metaregion
 *             and saves the combined region as the new metaregion for the
 *             specified device context. The clipping region is reset to a null region.
 * Parameters: HDC    hdc   handle of device context
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetMetaRgn( HDC  hdc)
{
  dprintf(("GDI32: SetMetaRgn(%08xh) not implemented.\n",
           hdc));

  SetLastError(ERROR_SUCCESS_W);
  return (NULLREGION_W);
}
//******************************************************************************
//******************************************************************************
