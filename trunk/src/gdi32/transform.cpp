/* $Id: transform.cpp,v 1.4 2001-05-19 19:43:54 sandervl Exp $ */

/*
 * GDI32 coordinate & transformation code
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: Metafile recording
 * TODO: Translate & set the last error for Gpi operations
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
#include <odinwrap.h>
#include <objhandle.h>
#include <dcdata.h>
#include <winuser32.h>
#include "oslibgpi.h"

#define DBG_LOCALLOG    DBG_transform
#include "dbglocal.h"

static const XFORM_W  XFORMIdentity    = { 1.0, 0.0, 0.0, 1.0, 0, 0 };

//region.cpp (todo; move to header)
LONG hdcHeight(HWND hwnd, pDCData pHps);
LONG hdcWidth(HWND hwnd, pDCData pHps);

//******************************************************************************
//******************************************************************************
BOOL WIN32API LPtoDP(HDC hdc, PPOINT lpPoints, int nCount)
{
 BOOL ret;
 DWORD hdcwidth, hdcheight;
 pDCData pHps;

    pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: LPtoDP %x invalid handle!!", hdc));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    dprintf(("LPtoDP %x %x %d", hdc, lpPoints, nCount));

    //GpiConvert doesn't like illegal values; TODO: check what NT does
    if(nCount && lpPoints) {
        hdcwidth  = hdcWidth(0, pHps);
        hdcheight = hdcHeight(0, pHps);
        for(int i=0;i<nCount;i++) {
            dprintf(("LPtoDP in (%d,%d)", lpPoints[i].x, lpPoints[i].y));
            if(lpPoints[i].x > hdcwidth) {
                dprintf(("WARNING: LPtoDP correcting x value; hdcwidth = %d", hdcwidth));
                lpPoints[i].x = 0;
            }
            if(lpPoints[i].y > hdcwidth) {
                dprintf(("WARNING: LPtoDP correcting y value; hdcheight = %d", hdcheight));
                lpPoints[i].y = 0;
            }
        }
    }
    ret = O32_LPtoDP(hdc, lpPoints, nCount);
#ifdef DEBUG
    if(nCount && lpPoints) {
        for(int i=0;i<nCount;i++) {
            dprintf(("LPtoDP out (%d,%d)", lpPoints[i].x, lpPoints[i].y));
        }
    }
#endif
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DPtoLP(HDC hdc, PPOINT lpPoints, int nCount)
{
    BOOL ret;
    DWORD hdcwidth, hdcheight;
    pDCData pHps;

    pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: DPtoLP %x invalid handle!!", hdc));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    dprintf(("GDI32: DPtoLP %x %x %d", hdc, lpPoints, nCount));

    //GpiConvert doesn't like illegal values; TODO: check what NT does
    if(nCount && lpPoints) {
        hdcwidth  = hdcWidth(0, pHps);
        hdcheight = hdcHeight(0, pHps);
        for(int i=0;i<nCount;i++) {
            dprintf(("DPtoLP in (%d,%d)", lpPoints[i].x, lpPoints[i].y));
            if(lpPoints[i].x > hdcwidth) {
                dprintf(("WARNING: DPtoLP correcting x value; hdcwidth = %d", hdcwidth));
                lpPoints[i].x = 0;
            }
            if(lpPoints[i].y > hdcwidth) {
                dprintf(("WARNING: DPtoLP correcting y value; hdcheight = %d", hdcheight));
                lpPoints[i].y = 0;
            }
        }
    }

    ret = O32_DPtoLP(hdc, lpPoints, nCount);
#ifdef DEBUG
    if(nCount && lpPoints) {
        for(int i=0;i<nCount;i++) {
            dprintf(("DPtoLP out (%d,%d)", lpPoints[i].x, lpPoints[i].y));
        }
    }
#endif
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWorldTransform(HDC hdc, const XFORM_W * pXform)
{
 BOOL ret;

    //Only proceed if DC is in GM_ADVANCED mode, unless it's a metafile PS
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps || ((pHps->graphicsMode != GM_ADVANCED_W) && !pHps->isMetaPS))
    {
        dprintf(("WARNING: SetWorldTransform %x %x; not in GM_ADVANCED mode!!", hdc, pXform));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(!pXform)
    {
        dprintf(("WARNING: SetWorldTransform %x %x; invalid parameter!!", hdc, pXform));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }

    //todo: metafile recording!!!!

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

    ret = GpiSetModelTransformMatrix(pHps->hps, 9, &matrixlf, TRANSFORM_REPLACE);
    if(ret)
    {
        TestWideLine(pHps);
        Calculate1PixelDelta(pHps);
        pHps->xform = *pXform;          // save transform in DC struct
        dprintf(("SetWorldTransform %x %x", hdc, pXform));
        SetLastError(ERROR_SUCCESS_W);
    }
    else {
        dprintf(("WARNING: SetWorldTransform %x %x; GpiSetModelTransformMatrix failed!!", hdc, pXform));
        SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWorldTransform(HDC hdc, LPXFORM_W pXform)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: GetWorldTransform %x %x -> INVALID HDC", hdc, pXform));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if (!pXform)
    {
        dprintf(("WARNING: GetWorldTransform %x NULL -> INVALID parameter", hdc));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }

    *pXform = pHps->xform;

    dprintf(("WARNING: GetWorldTransform %x %x", hdc, pXform));
    SetLastError(ERROR_SUCCESS_W);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ModifyWorldTransform(HDC hdc, const XFORM_W *pXform, DWORD mode)
{
  MATRIXLF matrixlf;
  LONG     lOptions;
  BOOL     ret;

    //Only proceed if DC is in GM_ADVANCED mode, unless it's a metafile PS
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps || ((pHps->graphicsMode != GM_ADVANCED_W) && !pHps->isMetaPS))
    {
        dprintf(("WARNING: ModifyWorldTransform %x %x %x; not in GM_ADVANCED mode!!", hdc, pXform, mode));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(mode == MWT_IDENTITY_W)
    {
        matrixlf.fxM11  = MAKEFIXED(1,0);
        matrixlf.fxM12  = 0;
        matrixlf.lM13   = 0;
        matrixlf.fxM21  = 0;
        matrixlf.fxM22  = MAKEFIXED(1,0);
        matrixlf.lM23   = 0;
        matrixlf.lM31   = 0;
        matrixlf.lM32   = 0;
        matrixlf.lM33   = MAKEFIXED(1,0);

        lOptions = TRANSFORM_REPLACE;
    }
    else
    if(mode == MWT_LEFTMULTIPLY_W  || mode == MWT_RIGHTMULTIPLY_W)
    {
        matrixlf.fxM11  = pXform->eM11 * (float)0x10000;
        matrixlf.fxM12  = pXform->eM12 * (float)0x10000;
        matrixlf.lM13   = 0;
        matrixlf.fxM21  = pXform->eM21 * (float)0x10000;
        matrixlf.fxM22  = pXform->eM22 * (float)0x10000;
        matrixlf.lM23   = 0;
        matrixlf.lM31   = pXform->eDx;
        matrixlf.lM32   = pXform->eDy;
        matrixlf.lM33   = 1;

        if(mode == MWT_LEFTMULTIPLY_W) {
             lOptions = TRANSFORM_PREEMPT;
        }
        else lOptions = TRANSFORM_ADD;
    }
    else
    {
        dprintf(("WARNING: ModifyWorldTransform %x %x %x; invalid parameter!!", hdc, pXform, mode));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }
    //todo: metafile recording!!!

    ret = GpiSetModelTransformMatrix( pHps->hps, 9, &matrixlf, lOptions);
    if(ret)
    {
        if(mode == MWT_IDENTITY_W) {
             pHps->xform = XFORMIdentity;
        }
        else GetWorldTransform(hdc, &pHps->xform);

        dprintf(("ModifyWorldTransform %x %x %d", hdc, pXform, mode));
        SetLastError(ERROR_SUCCESS_W);
    }
    else {
        dprintf(("ModifyWorldTransform %x %x %d; GpiSetModelTransformMatrix failed!!", hdc, pXform, mode));
        SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
    }

    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetViewportOrgEx(HDC hdc, int xOffset, int yOffset, LPPOINT pPoint)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    if(pHps && changePageXForm(pHps, (PPOINTL) &pHps->viewportOrg,
                               xOffset + pHps->viewportOrg.x,
                               yOffset + pHps->viewportOrg.y,
                               (PPOINTL) pPoint))
    {
        //todo: metafile recording!!!
        dprintf(("OffsetViewportOrgEx %x (%d,%d) %x", hdc, xOffset, yOffset));
        SetLastError(ERROR_SUCCESS_W);
        return TRUE;
    }

    dprintf(("WARNING: OffsetViewportOrgEx %x (%d,%d) %x; HDC invalid or changePageXForm failed!!", hdc, xOffset, yOffset));
    SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetWindowOrgEx(HDC hdc, int xOffset, int yOffset, LPPOINT pPoint)
{
   pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    if(pHps && changePageXForm(pHps, (PPOINTL) &pHps->windowOrg,
                               xOffset + pHps->windowOrg.x,
                               yOffset + pHps->windowOrg.y,
                               (PPOINTL) pPoint))
    {
        //todo: metafile recording!!!
        dprintf(("OffsetWindowOrgEx %x (%d,%d) %x", hdc, xOffset, yOffset));
        SetLastError(ERROR_SUCCESS_W);
        return TRUE;
    }

    dprintf(("WARNING: OffsetWindowOrgEx %x (%d,%d) %x; HDC invalid or changePageXForm failed!!", hdc, xOffset, yOffset));
    SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScaleViewportExtEx(HDC hdc, int xNum, int xDenom, int yNum, int yDenom, LPSIZE pSize)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    if(!pHps)
    {
        dprintf(("WARNING: ScaleViewportExtEx %x (%d,%d) (%d,%d), %x; HDC INVALID!!1", hdc, xNum, xDenom, yNum, yDenom, pSize));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }

    if(xNum && yNum && xDenom && yDenom &&
       (pHps->MapMode == MM_ISOTROPIC_W || pHps->MapMode == MM_ANISOTROPIC_W))
    {
        if(changePageXForm(pHps, NULL, pHps->viewportXExt * xNum / xDenom,
                           pHps->viewportYExt * yNum / yDenom, (PPOINTL) pSize))
        {
            //todo: metafile recording!!!
            dprintf(("ScaleViewportExtEx %x (%d,%d) (%d,%d), %x", hdc, xNum, xDenom, yNum, yDenom, pSize));
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }

        dprintf(("WARNING: ScaleViewportExtEx %x (%d,%d) (%d,%d), %x; changePageXForm failed!!", hdc, xNum, xDenom, yNum, yDenom, pSize));
        SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
        return FALSE;
    }
    else
    {
        pHps->lWndXExtSave = pHps->viewportXExt * xNum / xDenom;
        pHps->lWndYExtSave = pHps->viewportYExt * yNum / yDenom;

        //function is a no-op if map mode is not (AN)ISOTROPIC; NT returns TRUE
        dprintf(("ScaleViewportExtEx %x (%d,%d) (%d,%d), %x", hdc, xNum, xDenom, yNum, yDenom, pSize));
        SetLastError(ERROR_SUCCESS_W);
        return TRUE;
    }
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScaleWindowExtEx(HDC hdc, int xNum, int xDenom, int yNum, int yDenom, LPSIZE pSize )
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    if(pHps && xDenom && yDenom)
    {
        if(changePageXForm(pHps, (PPOINTL) &pHps->windowExt,
                           pHps->windowExt.cx * xNum / xDenom,
                           pHps->windowExt.cy * yNum / yDenom,
                           (PPOINTL) pSize))
        {
            //todo: metafile recording!!!
            dprintf(("ScaleWindowExtEx %x (%d,%d) (%d,%d), %x", hdc, xNum, xDenom, yNum, yDenom, pSize));
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }

        dprintf(("WARNING: ScaleWindowExtEx %x (%d,%d) (%d,%d), %x; changePageXForm failed!!", hdc, xNum, xDenom, yNum, yDenom, pSize));
        SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
        return FALSE;
    }

    dprintf(("WARNING: ScaleWindowExtEx %x (%d,%d) (%d,%d), %x; invalid HDC!!", hdc, xNum, xDenom, yNum, yDenom, pSize));
    SetLastError(ERROR_INVALID_PARAMETER_W);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
int WIN32API SetMapMode(HDC hdc, int mode)
{
    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: SetMapMode %x %d; invalid HDC!!", hdc, mode));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }

    //todo: metafile recording!!!
    dprintf(("SetMapMode %x %x", hdc, mode));
    return setMapMode(pHps, mode);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMapMode(HDC hdc)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(pHps) {
        dprintf(("GDI32: GetMapMode %x -> %d", hdc, pHps->MapMode));
        SetLastError(ERROR_SUCCESS_W);
        return pHps->MapMode;
    }
    dprintf(("WARNING: GetMapMode %x invalid HDC!!", hdc));
    SetLastError(ERROR_INVALID_HANDLE_W);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetViewportExtEx( HDC hdc, int xExt, int yExt, LPSIZE pSize)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if (!pHps)
    {
        dprintf(("WARNING: SetViewPortExtEx: HDC %x not found!!", hdc));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(xExt && yExt)
    {
        //todo: metafile recording!!! (always)
        if (pHps->MapMode == MM_ISOTROPIC_W || pHps->MapMode == MM_ANISOTROPIC_W )
        {
            if(changePageXForm(pHps, NULL, xExt, yExt, (PPOINTL)pSize))
            {
                dprintf(("SetViewPortExtEx: %x (%d,%d), %x", hdc, xExt, yExt, pSize));
                SetLastError(ERROR_SUCCESS_W);
                return TRUE;
            }
        }
        else
        {
            pHps->lVwpXExtSave = xExt, pHps->lVwpYExtSave = yExt;

            //function is a no-op if map mode is not (AN)ISOTROPIC; NT returns TRUE
            dprintf(("SetViewPortExtEx: %x (%d,%d), %x", hdc, xExt, yExt, pSize));
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }

        dprintf(("WARNING: SetViewPortExtEx: %x (%d,%d) -> changePageXForm failed!!!, %x", hdc, xExt, yExt, pSize));
        SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
        return FALSE;
    }

    dprintf(("WARNING: SetViewPortExtEx: %x (%d,%d) -> invalid parameters!!!, %x", hdc, xExt, yExt, pSize));
    SetLastError(ERROR_INVALID_PARAMETER_W);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetViewportExtEx(HDC hdc, LPSIZE pSize)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: GetViewportExtEx %x %x -> INVALID HDC", hdc, pSize));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(!pSize)
    {
        dprintf(("WARNING: GetViewportExtEx %x NULL -> INVALID parameter", hdc));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }

    pSize->cx = (LONG)pHps->viewportXExt;
    pSize->cy = (LONG)pHps->viewportYExt;
    dprintf(("GDI32: GetViewportExtEx %x -> (%d,%d)", hdc, pSize->cx, pSize->cy));

    SetLastError(ERROR_SUCCESS_W);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetViewportOrgEx(HDC hdc, int xOrg, int yOrg, LPPOINT pPoint)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

    if(pHps)
    {
        if(changePageXForm(pHps, (PPOINTL) &pHps->viewportOrg, xOrg,
                           yOrg, (PPOINTL) pPoint))
        {
            //todo: metafile recording!!!
            dprintf(("SetViewPortOrgEx: %x (%d,%d), %x", hdc, xOrg, yOrg, pPoint));
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }
        dprintf(("WARNING: SetViewPortOrgEx: %x (%d,%d) %x-> changePageXForm failed!!!, %x", hdc, xOrg, yOrg, pPoint));
        SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
        return FALSE;
    }

    dprintf(("WARNING: SetViewPortOrgEx: HDC %x not found!!", hdc));
    SetLastError(ERROR_INVALID_PARAMETER_W);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetViewportOrgEx(HDC hdc, LPPOINT pPoint)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: GetViewportOrgEx %x %x -> INVALID HDC", hdc, pPoint));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(!pPoint)
    {
        dprintf(("WARNING: GetViewportOrgEx %x NULL -> INVALID parameter", hdc));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }

    pPoint->x = pHps->viewportOrg.x;
    pPoint->y = pHps->viewportOrg.y;

    dprintf(("GDI32: GetViewportOrgEx %x -> (%d,%d)", hdc, pPoint->x, pPoint->y));

    SetLastError(ERROR_SUCCESS_W);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowExtEx(HDC hdc, int xExt, int yExt, LPSIZE pSize)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: SetWindowExtEx: HDC %x not found!!", hdc));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(xExt && yExt)
    {
        //todo: metafile recording!!! (always)

        if(pHps->MapMode == MM_ISOTROPIC_W || pHps->MapMode == MM_ANISOTROPIC_W)
        {
            if(changePageXForm(pHps, (PPOINTL) &pHps->windowExt,
                               xExt, yExt, (PPOINTL) pSize) )
            {
                dprintf(("SetWindowExtEx: %x (%d,%d), %x", hdc, xExt, yExt, pSize));
                SetLastError(ERROR_SUCCESS_W);
                return TRUE;
            }
            dprintf(("WARNING: SetWindowExtEx: %x (%d,%d) -> changePageXForm failed!!!, %x", hdc, xExt, yExt, pSize));
            SetLastError(ERROR_INVALID_PARAMETER_W);    //TODO: translate PM error
            return FALSE;
        }
        else
        {
            pHps->lWndXExtSave = xExt, pHps->lWndYExtSave = yExt;

            //function is a no-op if map mode is not (AN)ISOTROPIC; NT returns TRUE
            dprintf(("SetWindowExtEx: %x (%d,%d), %x", hdc, xExt, yExt, pSize));
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }
    }
    dprintf(("WARNING: SetWindowExtEx %x (%d,%d) %x; invalid parameter", hdc, xExt, yExt, pSize));
    SetLastError(ERROR_INVALID_PARAMETER_W);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowExtEx(HDC hdc, LPSIZE pSize)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: GetWindowExtEx %x %x -> INVALID HDC", hdc, pSize));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(!pSize)
    {
        dprintf(("WARNING: GetWindowExtEx %x NULL -> INVALID parameter", hdc));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }

    pSize->cx = pHps->windowExt.cx;
    pSize->cy = pHps->windowExt.cy;

    dprintf(("GDI32: GetWindowExtEx %x -> (%d,%d)", hdc, pSize->cx, pSize->cy));

    SetLastError(ERROR_SUCCESS_W);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowOrgEx(HDC hdc, int xOrg, int yOrg, LPPOINT pPoint)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(pHps)
    {
        if(changePageXForm(pHps, (PPOINTL) &pHps->windowOrg, xOrg,
                           yOrg, (PPOINTL) pPoint))
        {
            //todo: metafile recording!!!

            dprintf(("SetWindowOrgEx: %x (%d,%d), %x", hdc, xOrg, yOrg, pPoint));
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }
        //todo: set correct error
        dprintf(("WARNING: SetWindowOrgEx: %x (%d,%d) %x -> changePageXForm failed!!!, %x", hdc, xOrg, yOrg, pPoint));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }
    dprintf(("WARNING: SetWindowOrgEx: HDC %x not found!!", hdc));
    SetLastError(ERROR_INVALID_PARAMETER_W);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowOrgEx(HDC hdc, LPPOINT pPoint)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: GetWindowOrgEx %x %x -> INVALID HDC", hdc, pPoint));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
    }

    if(!pPoint)
    {
        dprintf(("WARNING: GetWindowOrgEx %x NULL -> INVALID parameter", hdc));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }

    pPoint->x = pHps->windowOrg.x;
    pPoint->y = pHps->windowOrg.y;

    dprintf(("GDI32: GetWindowOrgEx %x -> (%d,%d)", hdc, pPoint->x, pPoint->y));

    SetLastError(ERROR_SUCCESS_W);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
int WIN32API SetGraphicsMode(HDC hdc, int iMode)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        dprintf(("WARNING: SetGraphicsMode HDC %x NOT FOUND!!", hdc));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }

    if(iMode != GM_COMPATIBLE_W  && iMode != GM_ADVANCED_W)
    {
        dprintf(("WARNING: SetGraphicsMode invalid mode %x!!", iMode));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return 0;
    }

    //Only switch from GM_ADVANCED_W to GM_COMPATIBLE_W if default matrix is active
    if (iMode == GM_COMPATIBLE_W  &&  pHps->graphicsMode == GM_ADVANCED_W)
    {
        MATRIXLF matrixlf;

        if(!GpiQueryModelTransformMatrix(pHps->hps, 9, &matrixlf))
        {
            dprintf(("WARNING: SetGraphicsMode %x %d; GpiQueryModelTransformMatrix failed!", hdc, iMode));
            SetLastError(ERROR_INVALID_PARAMETER_W);  //todo translate PM error!!
            return 0;

        }

        if (!(matrixlf.fxM11 == MAKEFIXED(1,0) && matrixlf.fxM12 == MAKEFIXED(0,0) &&
              matrixlf.fxM21 == MAKEFIXED(0,0) && matrixlf.fxM22 == MAKEFIXED(1,0) &&
              matrixlf.lM31  == 0 && matrixlf.lM32  == 0 ))
        {
            dprintf(("WARNING: SetGraphicsMode %x %d; can't complete!", hdc, iMode));
            SetLastError(ERROR_CAN_NOT_COMPLETE_W);  // done by NT
            return 0;
        }
    }
    int oldMode = pHps->graphicsMode;

    dprintf(("SetGraphicsMode %x %d (old mode %d)", hdc, iMode, oldMode));
    pHps->graphicsMode = iMode;

    SetLastError(ERROR_SUCCESS_W);
    return oldMode;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetGraphicsMode(HDC hdc)
{
    pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if (!pHps)
    {
        dprintf(("WARNING: GetGraphicsMode HDC %x NOT FOUND!!", hdc));
        SetLastError(ERROR_INVALID_HANDLE_W);
        return 0;
    }

    dprintf(("GetGraphicsMode %x = %x", hdc, pHps->graphicsMode));
    SetLastError(ERROR_SUCCESS_W);
    return pHps->graphicsMode;
}
//******************************************************************************
//******************************************************************************

