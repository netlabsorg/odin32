/* $Id: dibitmap.cpp,v 1.44 2004-04-13 14:17:17 sandervl Exp $ */

/*
 * GDI32 dib & bitmap code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 * Copyright 2002-2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <misc.h>
#include <cpuhlp.h>
#include <winuser32.h>
#include "dibsect.h"
#include "rgbcvt.h"
#include <stats.h>
#include "oslibgpi.h"
#include <objhandle.h>

#define DBG_LOCALLOG    DBG_dibitmap
#include "dbglocal.h"

ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy);

//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateDIBitmap(HDC hdc, const BITMAPINFOHEADER *lpbmih,
                                DWORD fdwInit, const void *lpbInit,
                                const BITMAPINFO *lpbmi, UINT fuUsage)
{
    int iHeight;
    HBITMAP rc;
    DWORD bitfields[3];
    WORD *newbits = NULL;

    //TEMPORARY HACK TO PREVENT CRASH IN OPEN32 (WSeB GA)
    iHeight = lpbmih->biHeight;
    if(lpbmih->biHeight < 0)
    {
        dprintf(("GDI32: CreateDIBitmap negative height! (%d,%d)", lpbmih->biWidth, lpbmih->biHeight));
        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFOHEADER *)lpbmih)->biHeight = -lpbmih->biHeight;

        if(lpbInit && fdwInit == CBM_INIT)
        {
            // upside down
            HBITMAP rc = 0;
            long lLineByte = DIB_GetDIBWidthBytes(lpbmih->biWidth, lpbmih->biBitCount);
            long lHeight   = lpbmih->biHeight;

            newbits = (WORD *)malloc( lLineByte * lHeight );
            if(newbits) {
                unsigned char *pbSrc = (unsigned char *)lpbInit + lLineByte * (lHeight - 1);
                unsigned char *pbDst = (unsigned char *)newbits;
                for(int y = 0; y < lHeight; y++) {
                    memcpy( pbDst, pbSrc, lLineByte );
                    pbDst += lLineByte;
                    pbSrc -= lLineByte;
                }
                rc = CreateDIBitmap(hdc, lpbmih, fdwInit, newbits, lpbmi, fuUsage);
                free( newbits );
            }

            ((BITMAPINFOHEADER *)lpbmih)->biHeight = iHeight;
            return rc;
	}
    }

    //  For some reason, GPI messes up the colors in DIB_PAL_COLORS mode.
    //  Work around it by converting palette indexes to RGB values
    //  (SetDIBitsToDevice works under the same circumstances though)
    if(fuUsage == DIB_PAL_COLORS && lpbmi->bmiHeader.biSize == sizeof(BITMAPINFOHEADER) &&
       lpbmi->bmiHeader.biBitCount <= 8)
    {
      	// workaround for open32 bug.
      	// If syscolors > 256 and wUsage == DIB_PAL_COLORS.

      	int i;
        UINT biClrUsed;
      	USHORT *pColorIndex = (USHORT *)lpbmi->bmiColors;
      	RGBQUAD *pColors;
      	BITMAPINFO *infoLoc;

        biClrUsed = (lpbmi->bmiHeader.biClrUsed) ? lpbmi->bmiHeader.biClrUsed : (1<<lpbmi->bmiHeader.biBitCount);

        pColors = (RGBQUAD *) alloca(biClrUsed * sizeof(RGBQUAD));
        infoLoc = (BITMAPINFO *) alloca(sizeof(BITMAPINFO) + biClrUsed * sizeof(RGBQUAD));

      	memcpy(infoLoc, lpbmi, sizeof(BITMAPINFO));

      	if(GetDIBColorTable(hdc, 0, biClrUsed, pColors) == 0)
        {
            dprintf(("ERROR: StretchDIBits: GetDIBColorTable failed!!"));
            return FALSE;
        }
      	for(i=0;i<biClrUsed;i++, pColorIndex++)
      	{
            infoLoc->bmiColors[i] = pColors[*pColorIndex];
      	}

      	rc = CreateDIBitmap(hdc, lpbmih, fdwInit, lpbInit, (PBITMAPINFO)infoLoc,
                            DIB_RGB_COLORS);

    	return rc;
    }

    // 2000/09/01 PH Netscape 4.7
    // If color depth of lpbhmi is 16 bit and lpbmi is 8 bit,
    // Open32 will crash since it won't allocate any palette color memory,
    // however wants to copy it later on ...
    int biBitCount = lpbmih->biBitCount;

    if (lpbmih->biBitCount != lpbmi->bmiHeader.biBitCount)
    {
        dprintf(("GDI32: CreateDIBitmap: color depths of bitmaps differ! (%d,%d\n", lpbmih->biBitCount,
                lpbmi->bmiHeader.biBitCount));

        ((BITMAPINFOHEADER *)lpbmih)->biBitCount = lpbmi->bmiHeader.biBitCount;
    }

    switch(lpbmih->biBitCount) {
    case 15:
    case 16: //Default if BI_BITFIELDS not set is RGB 555
        bitfields[0] = (lpbmih->biCompression == BI_BITFIELDS) ? *(DWORD *)lpbmi->bmiColors       : DEFAULT_16BPP_RED_MASK;
        bitfields[1] = (lpbmih->biCompression == BI_BITFIELDS) ? *((DWORD *)lpbmi->bmiColors + 1) : DEFAULT_16BPP_GREEN_MASK;
        bitfields[2] = (lpbmih->biCompression == BI_BITFIELDS) ? *((DWORD *)lpbmi->bmiColors + 2) : DEFAULT_16BPP_BLUE_MASK;
        break;
    case 24:
    case 32:
        bitfields[0] = (lpbmih->biCompression == BI_BITFIELDS) ? *(DWORD *)lpbmi->bmiColors       : DEFAULT_24BPP_RED_MASK;
        bitfields[1] = (lpbmih->biCompression == BI_BITFIELDS) ? *((DWORD *)lpbmi->bmiColors + 1) : DEFAULT_24BPP_GREEN_MASK;
        bitfields[2] = (lpbmih->biCompression == BI_BITFIELDS) ? *((DWORD *)lpbmi->bmiColors + 2) : DEFAULT_24BPP_BLUE_MASK;
        break;
    default:
        bitfields[0] = 0;
        bitfields[1] = 0;
        bitfields[2] = 0;
        break;
    }
    if(bitfields[1] == RGB555_GREEN_MASK && lpbInit && fdwInit == CBM_INIT)
    {//RGB 555?
        dprintf(("RGB 555->565 conversion required %x %x %x", bitfields[0], bitfields[1], bitfields[2]));

        int imgsize = CalcBitmapSize(lpbmih->biBitCount, lpbmih->biWidth, lpbmih->biHeight);

        newbits = (WORD *)malloc(imgsize);
        pRGB555to565(newbits, (WORD *)lpbInit, imgsize/sizeof(WORD));

        lpbInit = newbits;
    }

    rc = O32_CreateDIBitmap(hdc, lpbmih, fdwInit, lpbInit, lpbmi, fuUsage);

    dprintf(("GDI32: CreateDIBitmap %x %x %x %x %x returned %x (%d,%d, bps %d)", hdc, lpbmih, fdwInit, lpbInit, fuUsage, rc, lpbmih->biWidth, lpbmih->biHeight, lpbmih->biBitCount));

    if(newbits) free(newbits);

    ((BITMAPINFOHEADER *)lpbmih)->biHeight   = iHeight;
    ((BITMAPINFOHEADER *)lpbmih)->biBitCount = biBitCount;

    if(rc) {
        STATS_CreateDIBitmap(rc, hdc, lpbmih, fdwInit, lpbInit, lpbmi, fuUsage);
        if(bitfields[1] == RGB565_GREEN_MASK) {
            //mark bitmap as RGB565
            dprintf(("RGB565 bitmap"));
            ObjSetHandleFlag(rc, OBJHANDLE_FLAG_BMP_RGB565, TRUE);
        }
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateCompatibleBitmap( HDC hdc, int nWidth, int nHeight)
{
    HBITMAP hBitmap;
    pDCData pHps;

    pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }

    hBitmap = O32_CreateCompatibleBitmap(hdc, nWidth, nHeight);
    if(hBitmap) {
        STATS_CreateCompatibleBitmap(hBitmap,hdc, nWidth, nHeight);
        if(pHps->hwnd == 1) { //1 == HWND_DESKTOP
            dprintf(("Screen compatible bitmap"));
            ObjSetHandleFlag(hBitmap, OBJHANDLE_FLAG_BMP_SCREEN_COMPATIBLE, 1);
        }
    }

    return hBitmap;
}
//******************************************************************************
//CreateDisardableBitmap is obsolete and can be replaced by CreateCompatibleBitmap
//******************************************************************************
HBITMAP WIN32API CreateDiscardableBitmap(HDC hDC, int nWidth, int nHeight)
{
    dprintf(("GDI32: CreateDisardableBitmap"));
    return CreateCompatibleBitmap(hDC, nWidth, nHeight);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmap(int nWidth, int nHeight, UINT cPlanes,
                              UINT cBitsPerPel, const void *lpvBits)
{
    HBITMAP hBitmap;

    hBitmap = O32_CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits);
    if(hBitmap) STATS_CreateBitmap(hBitmap,nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits);
    return(hBitmap);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmapIndirect( const BITMAP *pBitmap)
{
    HBITMAP hBitmap;

    dprintf(("GDI32: CreateBitmapIndirect (%d,%d) bpp %d bits %x", pBitmap->bmWidth, pBitmap->bmHeight, pBitmap->bmBitsPixel, pBitmap->bmBits));
    hBitmap = O32_CreateBitmapIndirect(pBitmap);
    if(hBitmap) STATS_CreateBitmapIndirect(hBitmap, pBitmap);
    return hBitmap;
}
//******************************************************************************
//*********************************************************************************
HBITMAP WIN32API CreateDIBSection( HDC hdc, BITMAPINFO *pbmi, UINT iUsage,
                                   VOID **ppvBits, HANDLE hSection, DWORD dwOffset)
{
    HBITMAP res = 0;
    BOOL    fFlip = 0;
    int     iHeight, iWidth;
    BOOL    fCreateDC = FALSE;

    dprintf(("GDI32: CreateDIBSection %x %x %x %x %x %d", hdc, pbmi, iUsage, ppvBits, hSection, dwOffset));

    //SvL: 13-9-98: StarCraft uses bitmap with negative height
    iWidth = pbmi->bmiHeader.biWidth;
    if(pbmi->bmiHeader.biWidth < 0)
    {
        dprintf(("CreateDIBSection: width %d", pbmi->bmiHeader.biWidth));
        pbmi->bmiHeader.biWidth = -pbmi->bmiHeader.biWidth;
        fFlip = FLIP_HOR;
    }
    iHeight = pbmi->bmiHeader.biHeight;
    if(pbmi->bmiHeader.biHeight < 0)
    {
        dprintf(("CreateDIBSection: height %d", pbmi->bmiHeader.biHeight));
        pbmi->bmiHeader.biHeight = -pbmi->bmiHeader.biHeight;
        fFlip |= FLIP_VERT;
  }

    //SvL: RP7 (update) calls this api with hdc == 0
    if(hdc == 0) {
        hdc = CreateCompatibleDC(0);
        fCreateDC = TRUE;
    }
    res = O32_CreateDIBitmap(hdc, &pbmi->bmiHeader, 0, NULL, pbmi, iUsage);
    if (res)
    {
        char PalSize;
        DIBSection *dsect;

        dsect = new DIBSection((BITMAPINFOHEADER_W *)&pbmi->bmiHeader, (char *)&pbmi->bmiColors, iUsage, hSection, dwOffset, (DWORD)res, fFlip);

        if(dsect != NULL)
        {
            PalSize = dsect->GetBitCount();

            if(!ppvBits && PalSize <= 8)
            {
                ULONG Pal[256], nrcolors;
                LOGPALETTE tmpPal = { 0x300,1,{0,0,0,0}};
                HPALETTE hpalCur;

                // Now get the current Palette from the DC
                hpalCur = GetCurrentObject(hdc, OBJ_PAL);

                // and use it to set the DIBColorTable
                nrcolors = GetPaletteEntries( hpalCur, 0, 1<<PalSize, (LPPALETTEENTRY)&Pal);
                dsect->SetDIBColorTable(0, nrcolors, (LPPALETTEENTRY)&Pal);
            }

            if(ppvBits!=NULL)
                *ppvBits = dsect->GetDIBObject();

            pbmi->bmiHeader.biWidth = iWidth;
            pbmi->bmiHeader.biHeight = iHeight;

            if(fCreateDC) DeleteDC(hdc);

            STATS_CreateDIBSection(res, hdc, pbmi, iUsage, ppvBits, hSection, dwOffset);

            return(res);
        }
    }
    if(fCreateDC) DeleteDC(hdc);

    /* Error.  */
    if (res)
        DeleteObject(res);
    *ppvBits = NULL;

#ifdef DEBUG
    dprintf(("GDI32: CreateDIBSection, error!\n"));
    dprintf(("pbmi->biWidth    %d", pbmi->bmiHeader.biWidth));
    dprintf(("pbmi->biHeight   %d", pbmi->bmiHeader.biHeight));
    dprintf(("pbmi->biBitCount %d", pbmi->bmiHeader.biBitCount));
#endif

    return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDIBColorTable(HDC hdc, UINT uStartIndex, UINT cEntries,
                               RGBQUAD *pColors)
{
    DIBSection *dsect = DIBSection::findHDC(hdc);
    UINT rc;
    int i;

    dprintf(("GetDIBColorTable %x %d->%d %x", hdc, uStartIndex, cEntries, pColors));

    if(dsect)
    {
       return(dsect->GetDIBColorTable(uStartIndex, cEntries, pColors));
    }
    //TODO: Is this correct?????
    //      Wine returns 0 if bitmap selected into dc with bpp > 8
    HPALETTE hpal = GetCurrentObject(hdc, OBJ_PAL);
    rc = O32_GetPaletteEntries(hpal, uStartIndex, cEntries, (PALETTEENTRY *)pColors);
    for(i=0;i<cEntries;i++)
    {
        BYTE tmp;
        tmp = pColors[i].rgbBlue;
        pColors[i].rgbBlue = pColors[i].rgbRed;
        pColors[i].rgbRed = tmp;
        pColors[i].rgbReserved = 0;
    }
    dprintf(("GDI32: GetDIBColorTable returns %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetDIBColorTable(HDC hdc, UINT uStartIndex, UINT cEntries,
                               RGBQUAD *pColors)
{
    DIBSection *dsect = DIBSection::findHDC(hdc);

    dprintf(("GDI32: SetDIBColorTable %x %d,%d %x", hdc, uStartIndex, cEntries, pColors));
    if(dsect)
    {
         return(dsect->SetDIBColorTable(uStartIndex, cEntries, pColors));
    }
    else return(0);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetBitmapBits( HBITMAP hBitmap, LONG arg2, PVOID lpvBits)
{
    dprintf(("GDI32: GetBitmapBits %x", hBitmap));

    if(lpvBits)
    {
        DIBSECTION_CHECK_IF_DIRTY_BMP(hBitmap);
    }
    return O32_GetBitmapBits(hBitmap, arg2, lpvBits);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetBitmapBits( HBITMAP hBitmap, LONG arg2, const VOID *  arg3)
{
    LONG ret;

    dprintf(("GDI32: SetBitmapBits %x", hBitmap));
    ret = O32_SetBitmapBits(hBitmap, (DWORD)arg2, arg3);
    DIBSECTION_MARK_INVALID_BMP(hBitmap);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBitmapDimensionEx( HBITMAP hBitmap, PSIZE pSize)
{
    dprintf(("GDI32: GetBitmapDimensionEx %x (%d,%d)", hBitmap, pSize->cx, pSize->cy));
    return O32_GetBitmapDimensionEx(hBitmap, pSize);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetBitmapDimensionEx( HBITMAP arg1, int arg2, int arg3, PSIZE  arg4)
{
    dprintf(("GDI32: SetBitmapDimensionEx"));
    return O32_SetBitmapDimensionEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDIBits(HDC hdc, HBITMAP hBitmap, UINT uStartScan, UINT cScanLines,
                       void *lpvBits, PBITMAPINFO lpbi, UINT uUsage)
{
    int nrlines;
    pDCData pHps;
    HDC hdcMem;
    DWORD biCompression;

    dprintf(("GDI32: GetDIBits %x %x %d %d %x %x (biBitCount %d) %d", hdc, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, lpbi->bmiHeader.biBitCount, uUsage));

    if(lpvBits)
    {
        DIBSECTION_CHECK_IF_DIRTY_BMP(hBitmap);
    }

    //SvL: WGSS screws up the DC if it's a memory DC
    //     TODO: Fix in WGSS (tries to select another bitmap in the DC which fails)
    pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }
    if(pHps->isMemoryPS) {
         hdcMem = CreateCompatibleDC(0);
    }
    else hdcMem = hdc;

    if(lpvBits) {
        biCompression = lpbi->bmiHeader.biCompression;
    }

    //If the app wants bitmap data and upside down, then flip image
    if(lpvBits && lpbi->bmiHeader.biHeight < 0 && (lpbi->bmiHeader.biCompression == BI_RGB ||
       lpbi->bmiHeader.biCompression == BI_BITFIELDS))
    {
        INT rc = -1;
        long lLineByte;
        LONG height = lpbi->bmiHeader.biHeight;

        lpbi->bmiHeader.biHeight = -lpbi->bmiHeader.biHeight;

        lLineByte = DIB_GetDIBWidthBytes(lpbi->bmiHeader.biWidth, lpbi->bmiHeader.biBitCount);

        dprintf(("flip bitmap (negative height)"));
        char *pNewBits = (char *)malloc( lLineByte * cScanLines );
        if(pNewBits) {
            nrlines = O32_GetDIBits(hdcMem, hBitmap, uStartScan, cScanLines, pNewBits, lpbi, uUsage);

            unsigned char *pbSrc = (unsigned char *)pNewBits + lLineByte * (cScanLines - 1);
            unsigned char *pbDst = (unsigned char *)lpvBits;
            for(int y = 0; y < cScanLines; y++) {
                memcpy( pbDst, pbSrc, lLineByte );
                pbDst += lLineByte;
                pbSrc -= lLineByte;
            }
            free(pNewBits);
        }
        else DebugInt3();

        //restore height
        lpbi->bmiHeader.biHeight = height;
    }
    else if (lpbi->bmiHeader.biHeight < 0)
    {
        LONG cySaved = lpbi->bmiHeader.biHeight;
        lpbi->bmiHeader.biHeight = -cySaved;
        nrlines = O32_GetDIBits(hdcMem, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, uUsage);
        lpbi->bmiHeader.biHeight = cySaved;
    }
    else
        nrlines = O32_GetDIBits(hdcMem, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, uUsage);

    if(lpvBits) {
        //WGSS always sets it to BI_RGB
        lpbi->bmiHeader.biCompression = biCompression;
    }

    if(pHps->isMemoryPS)
        DeleteDC(hdcMem);

    //Only return bitfields info if the app wants it
    if(lpvBits && lpbi->bmiHeader.biCompression == BI_BITFIELDS) {
        // set proper color masks (only if lpvBits not NULL)
        switch(lpbi->bmiHeader.biBitCount) {
        case 15:
        case 16: //RGB 565
        {
            DWORD dwFlags;

            dwFlags = ObjQueryHandleFlags(hBitmap);
            if(dwFlags & (OBJHANDLE_FLAG_BMP_SCREEN_COMPATIBLE|OBJHANDLE_FLAG_BMP_RGB565)) {
                ((DWORD*)(lpbi->bmiColors))[0] = RGB565_RED_MASK;
                ((DWORD*)(lpbi->bmiColors))[1] = RGB565_GREEN_MASK;
                ((DWORD*)(lpbi->bmiColors))[2] = RGB565_BLUE_MASK;
            }
            else {
                ((DWORD*)(lpbi->bmiColors))[0] = RGB555_RED_MASK;
                ((DWORD*)(lpbi->bmiColors))[1] = RGB555_GREEN_MASK;
                ((DWORD*)(lpbi->bmiColors))[2] = RGB555_BLUE_MASK;
            }
            break;
        }
        case 24:
        case 32:
            ((DWORD*)(lpbi->bmiColors))[0] = DEFAULT_24BPP_RED_MASK;
            ((DWORD*)(lpbi->bmiColors))[1] = DEFAULT_24BPP_GREEN_MASK;
            ((DWORD*)(lpbi->bmiColors))[2] = DEFAULT_24BPP_BLUE_MASK;
            break;
        }
        dprintf(("BI_BITFIELDS: %x %x %x", ((DWORD*)(lpbi->bmiColors))[0], ((DWORD*)(lpbi->bmiColors))[1], ((DWORD*)(lpbi->bmiColors))[2]));
    }
    if(nrlines && lpvBits && lpbi->bmiHeader.biBitCount == 16 && ((DWORD*)(lpbi->bmiColors))[1] == RGB555_GREEN_MASK)
    {//RGB 555?
        dprintf(("RGB 565->555 conversion required"));

        int imgsize = CalcBitmapSize(lpbi->bmiHeader.biBitCount,
                                     lpbi->bmiHeader.biWidth, nrlines);

        pRGB565to555((WORD *)lpvBits, (WORD *)lpvBits, imgsize/sizeof(WORD));
    }

    //WGSS/Open32 returns 1 when querying the bitmap info; must return nr of scanlines
    //(0 signals failure)
    #if 0//XP returns 1 just like Open32 does for the query bitmap info header mode.
    if (lpvBits == NULL)
    {
       if (nrlines != 0)
           nrlines = cScanLines;
       else
           dprintf(("GetDIBits failed!!"));
    }
    #else
    if (!nrlines)
        dprintf(("GetDIBits failed!!"));
    #endif
    return nrlines;
}
//******************************************************************************
//******************************************************************************
void WIN32API ConvertRGB555to565(LPVOID dest, LPVOID src, UINT imgsize)
{
    pRGB555to565((WORD *)dest, (WORD *)src, imgsize/sizeof(WORD));
}
//******************************************************************************
//******************************************************************************
int WIN32API SetDIBits(HDC hdc, HBITMAP hBitmap, UINT startscan, UINT numlines, const VOID *pBits,
                       const BITMAPINFO *pBitmapInfo, UINT usage)
{
    int ret;
    DWORD bitfields[3];
    WORD *newbits = NULL;

    dprintf(("GDI32: SetDIBits %x %x %x %x %x %x %x", hdc, hBitmap, startscan, numlines, pBits, pBitmapInfo, usage));

    //SvL: Open32's SetDIBits really messes things up for 1 bpp bitmaps, must use SetBitmapBits
    if(pBitmapInfo->bmiHeader.biBitCount == 1 && startscan == 0 && numlines == pBitmapInfo->bmiHeader.biHeight)
    {//WARNING: hack alert!
        int   dibwidth  = DIB_GetDIBWidthBytes(pBitmapInfo->bmiHeader.biWidth, 1);
        int   bmpwidth  = BITMAP_GetWidthBytes(pBitmapInfo->bmiHeader.biWidth, 1);
        char *newpix    = (char *)malloc(dibwidth*pBitmapInfo->bmiHeader.biHeight);
        char *orgpix    = (char *)pBits;
        int   ret;

        dprintf(("Flipping 1bpp bitmap and calling SetBitmapBits (WORKAROUND) (%d -> %d)", dibwidth, bmpwidth));
        newpix += ((pBitmapInfo->bmiHeader.biHeight-1)*bmpwidth);

        //flip bitmap here; SetDIBits assumes origin is left bottom, SetBitmapBits left top
        //SetDIBits assumes DWORD aligned data
        //SetBitmapBits assumes WORD aligned data
        for(int i=0;i<pBitmapInfo->bmiHeader.biHeight;i++) {
            memcpy(newpix, orgpix, bmpwidth);

            newpix -= bmpwidth;
            orgpix += dibwidth;
        }
        newpix += bmpwidth;
        ret = O32_SetBitmapBits(hBitmap, pBitmapInfo->bmiHeader.biSizeImage, newpix);

        DIBSECTION_MARK_INVALID_BMP(hBitmap);

        free(newpix);
        return ret;
    }
#ifdef DEBUG
    if(pBitmapInfo->bmiHeader.biBitCount == 1) {
        dprintf(("ERROR: SetDIBits does NOT work well for 1 bpp bitmaps!!!!!"));
    }
#endif

    switch(pBitmapInfo->bmiHeader.biBitCount) {
    case 15:
    case 16: //Default if BI_BITFIELDS not set is RGB 555
        bitfields[0] = (pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS) ?  *(DWORD *)pBitmapInfo->bmiColors       : DEFAULT_16BPP_RED_MASK;
        bitfields[1] = (pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)pBitmapInfo->bmiColors + 1) : DEFAULT_16BPP_GREEN_MASK;
        bitfields[2] = (pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)pBitmapInfo->bmiColors + 2) : DEFAULT_16BPP_BLUE_MASK;
        break;

    case 24:
    case 32:
        bitfields[0] = (pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS) ?  *(DWORD *)pBitmapInfo->bmiColors       : DEFAULT_24BPP_RED_MASK;
        bitfields[1] = (pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)pBitmapInfo->bmiColors + 1) : DEFAULT_24BPP_GREEN_MASK;
        bitfields[2] = (pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)pBitmapInfo->bmiColors + 2) : DEFAULT_24BPP_BLUE_MASK;
        break;
    default:
        bitfields[0] = 0;
        bitfields[1] = 0;
        bitfields[2] = 0;
        break;
    }
    if(pBits && bitfields[1] == RGB555_GREEN_MASK)
    {//RGB 555?
        dprintf(("RGB 555->565 conversion required %x %x %x", bitfields[0], bitfields[1], bitfields[2]));

        int imgsize = CalcBitmapSize(pBitmapInfo->bmiHeader.biBitCount,
                                     pBitmapInfo->bmiHeader.biWidth, numlines);

        newbits = (WORD *)malloc(imgsize);
        pRGB555to565(newbits, (WORD *)pBits, imgsize/sizeof(WORD));
        pBits = newbits;
    }

    //If upside down, reverse scanlines and call SetDIBits again
    if(pBitmapInfo->bmiHeader.biHeight < 0 && (pBitmapInfo->bmiHeader.biCompression == BI_RGB ||
       pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS))
    {
        // upside down
        INT rc = -1;
        long lLineByte;

        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFO *)pBitmapInfo)->bmiHeader.biHeight = -pBitmapInfo->bmiHeader.biHeight;

        lLineByte = DIB_GetDIBWidthBytes(pBitmapInfo->bmiHeader.biWidth, pBitmapInfo->bmiHeader.biBitCount);

        dprintf(("Reverse bitmap (negative height)"));
        char *pNewBits = (char *)malloc( lLineByte * numlines );
        if(pNewBits) {
            unsigned char *pbSrc = (unsigned char *)pBits + lLineByte * (numlines - 1);
            unsigned char *pbDst = (unsigned char *)pNewBits;
            for(int y = 0; y < numlines; y++) {
                memcpy( pbDst, pbSrc, lLineByte );
                pbDst += lLineByte;
                pbSrc -= lLineByte;
            }
            ret = O32_SetDIBits(hdc, hBitmap, startscan, numlines, pNewBits, pBitmapInfo, usage);
            free(pNewBits);
        }
        else DebugInt3();

        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFO *)pBitmapInfo)->bmiHeader.biHeight = -pBitmapInfo->bmiHeader.biHeight;
    }
    else {
        ret = O32_SetDIBits(hdc, hBitmap, startscan, numlines, pBits, pBitmapInfo, usage);
    }
    if(newbits) free(newbits);

    DIBSECTION_MARK_INVALID_BMP(hBitmap);

    return ret;
}
//******************************************************************************
//******************************************************************************
