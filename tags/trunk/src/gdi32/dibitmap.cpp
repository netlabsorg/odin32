/* $Id: dibitmap.cpp,v 1.35 2002-11-26 10:53:07 sandervl Exp $ */

/*
 * GDI32 dib & bitmap code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
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

    //SvL: Completely wrong result when creating a 1bpp bitmap here (converted
    //     to 8bpp by Open32)
    if(lpbmih->biBitCount == 1) {
        dprintf(("WARNING: CreateDIBitmap doesn't handle 1bpp bitmaps very well!!!!!"));
    }

    //TEMPORARY HACK TO PREVENT CRASH IN OPEN32 (WSeB GA)
    iHeight = lpbmih->biHeight;
    if(lpbmih->biHeight < 0)
    {
        dprintf(("GDI32: CreateDIBitmap negative height! (%d,%d)", lpbmih->biWidth, lpbmih->biHeight));
        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFOHEADER *)lpbmih)->biHeight = -lpbmih->biHeight;

        if(lpbInit && fdwInit == CBM_INIT) {
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
        if(CPUFeatures & CPUID_MMX) {
             RGB555to565MMX(newbits, (WORD *)lpbInit, imgsize/sizeof(WORD));
        }
        else RGB555to565(newbits, (WORD *)lpbInit, imgsize/sizeof(WORD));
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
            if(PalSize <= 8)
            {
                ULONG Pal[256], nrcolors;
                LOGPALETTE tmpPal = { 0x300,1,{0,0,0,0}};
                HPALETTE hpalCur, hpalTmp;

                // Now get the current Palette from the DC
                hpalTmp = CreatePalette(&tmpPal);
                hpalCur = SelectPalette(hdc, hpalTmp, FALSE);

                // and use it to set the DIBColorTable
                nrcolors = GetPaletteEntries( hpalCur, 0, 1<<PalSize, (LPPALETTEENTRY)&Pal);
                dsect->SetDIBColorTable(0, nrcolors, (LPPALETTEENTRY)&Pal);

                // Restore the DC Palette
                SelectPalette(hdc,hpalCur,FALSE);
                DeleteObject(hpalTmp);
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
LONG WIN32API GetBitmapBits( HBITMAP hBitmap, LONG arg2, PVOID  arg3)
{
    dprintf(("GDI32: GetBitmapBits %x", hBitmap));
    return O32_GetBitmapBits(hBitmap, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetBitmapBits( HBITMAP hBitmap, LONG arg2, const VOID *  arg3)
{
    dprintf(("GDI32: SetBitmapBits %x", hBitmap));
    return O32_SetBitmapBits(hBitmap, (DWORD)arg2, arg3);
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

    dprintf(("GDI32: GetDIBits %x %x %d %d %x %x (biBitCount %d) %d", hdc, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, lpbi->bmiHeader.biBitCount, uUsage));

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
    else  hdcMem = hdc;

    if(lpbi->bmiHeader.biHeight < 0) {
        //NOTE: workaround for WGSS bug; remove when fixed
        LONG height = lpbi->bmiHeader.biHeight;

        nrlines = O32_GetDIBits(hdcMem, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, uUsage);

        //NOTE: workaround for WGSS bug; remove when fixed
        lpbi->bmiHeader.biHeight = height;
    }
    else {
        nrlines = O32_GetDIBits(hdcMem, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, uUsage);
    }

    if(pHps->isMemoryPS)
        DeleteDC(hdcMem);

    if(lpvBits) {
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
        if(lpbi->bmiHeader.biCompression == BI_RGB && lpbi->bmiHeader.biBitCount > 8) {
            lpbi->bmiHeader.biCompression = BI_BITFIELDS;
            dprintf(("BI_BITFIELDS: %x %x %x", ((DWORD*)(lpbi->bmiColors))[0], ((DWORD*)(lpbi->bmiColors))[1], ((DWORD*)(lpbi->bmiColors))[2]));
        }
    }
    if(nrlines && lpvBits && lpbi->bmiHeader.biBitCount == 16 && ((DWORD*)(lpbi->bmiColors))[1] == RGB555_GREEN_MASK)
    {//RGB 555?
        dprintf(("RGB 565->555 conversion required"));

        int imgsize = CalcBitmapSize(lpbi->bmiHeader.biBitCount,
                                     lpbi->bmiHeader.biWidth, nrlines);

        if(CPUFeatures & CPUID_MMX) {
             RGB565to555MMX((WORD *)lpvBits, (WORD *)lpvBits, imgsize/sizeof(WORD));
        }
        else RGB565to555((WORD *)lpvBits, (WORD *)lpvBits, imgsize/sizeof(WORD));
    }

    //WGSS/Open32 returns 0 when querying the bitmap info; must return nr of scanlines
    //as 0 signals failure
    if(lpvBits == NULL) {
       nrlines = cScanLines;
    }
    return nrlines;
}
//******************************************************************************
//******************************************************************************
void WIN32API ConvertRGB555to565(LPVOID dest, LPVOID src, UINT imgsize)
{
    if(CPUFeatures & CPUID_MMX) {
         RGB555to565MMX((WORD *)dest, (WORD *)src, imgsize/sizeof(WORD));
    }
    else RGB555to565((WORD *)dest, (WORD *)src, imgsize/sizeof(WORD));
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
        if(CPUFeatures & CPUID_MMX) {
             RGB555to565MMX(newbits, (WORD *)pBits, imgsize/sizeof(WORD));
        }
        else RGB555to565(newbits, (WORD *)pBits, imgsize/sizeof(WORD));
        pBits = newbits;
    }

    if(pBitmapInfo->bmiHeader.biHeight < 0) {
        dprintf(("!WARNING! Negative height!!"));
    }
    ret = O32_SetDIBits(hdc, hBitmap, startscan, numlines, pBits, pBitmapInfo, usage);
    if(newbits) free(newbits);

    if(DIBSection::getSection() != NULL)
    {
        DIBSection *dsect;

        dsect = DIBSection::findObj(hBitmap);
        if(dsect) {
             HBITMAP hBmpOld = SelectObject(hdc, hBitmap);
             dsect->sync(hdc, 0, dsect->GetHeight());
             SelectObject(hdc, hBmpOld);
        }
    }

    return ret;
}
//******************************************************************************
//******************************************************************************
