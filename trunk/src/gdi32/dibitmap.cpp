/* $Id: dibitmap.cpp,v 1.21 2001-06-08 11:03:32 sandervl Exp $ */

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
    ((BITMAPINFOHEADER *)lpbmih)->biHeight = -lpbmih->biHeight;
  }

  // 2000/09/01 PH Netscape 4.7
  // If color depth of lpbhmi is 16 bit and lpbmi is 8 bit,
  // Open32 will crash since it won't allocate any palette color memory,
  // however wants to copy it later on ...
  int biBitCount = lpbmih->biBitCount;

  if (lpbmih->biBitCount != lpbmi->bmiHeader.biBitCount)
  {
    dprintf(("GDI32: CreateDIBitmap: color depths of bitmaps differ! (%d,%d\n",
             lpbmih->biBitCount,
             lpbmi->bmiHeader.biBitCount));

    ((BITMAPINFOHEADER *)lpbmih)->biBitCount = lpbmi->bmiHeader.biBitCount;
  }

  rc = O32_CreateDIBitmap(hdc, lpbmih, fdwInit, lpbInit, lpbmi, fuUsage);

  dprintf(("GDI32: CreateDIBitmap %x %x %x %x %x returned %x (%d,%d, bps %d)", hdc, lpbmih, fdwInit, lpbInit, fuUsage, rc, lpbmih->biWidth, lpbmih->biHeight, lpbmih->biBitCount));

  ((BITMAPINFOHEADER *)lpbmih)->biHeight   = iHeight;
  ((BITMAPINFOHEADER *)lpbmih)->biBitCount = biBitCount;

  return rc;
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateCompatibleBitmap( HDC hdc, int nWidth, int nHeight)
{
 HBITMAP hBitmap;

    hBitmap = O32_CreateCompatibleBitmap(hdc, nWidth, nHeight);
    dprintf(("GDI32: CreateCompatibleBitmap %x (%d,%d) returned %x", hdc, nWidth, nHeight, hBitmap));
    return hBitmap;
}
//******************************************************************************
//CreateDisardableBitmap is obsolete and can be replaced by CreateCompatibleBitmap
//******************************************************************************
HBITMAP WIN32API CreateDiscardableBitmap(HDC hDC, int nWidth, int nHeight)
{
    dprintf(("GDI32: CreateDisardableBitmap\n"));
    return CreateCompatibleBitmap(hDC, nWidth, nHeight);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmap(int nWidth, int nHeight, UINT cPlanes,
                                 UINT cBitsPerPel, const void *lpvBits)
{
 HBITMAP hBitmap;

    hBitmap = O32_CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits);
    dprintf(("GDI32: CreateBitmap (%d,%d) bps %d returned %x", nWidth, nHeight, cBitsPerPel, hBitmap));
    return(hBitmap);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmapIndirect( const BITMAP * arg1)
{
    dprintf(("GDI32: CreateBitmapIndirect"));
    return O32_CreateBitmapIndirect(arg1);
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
    hdc = GetWindowDC(GetDesktopWindow());
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
//SvL: Shouldn't an app explicitely select the dib section into the hdc?
//     (RealPlayer does this)
#if 0
      // Set the hdc in the DIBSection so we can update the palete if a new
      // Palette etc. gets selected into the DC.

      dsect->SelectDIBObject(hdc);
#endif

      if(ppvBits!=NULL)
        *ppvBits = dsect->GetDIBObject();

      pbmi->bmiHeader.biWidth = iWidth;
      pbmi->bmiHeader.biHeight = iHeight;

      if(fCreateDC) ReleaseDC(GetDesktopWindow(), hdc);
      return(res);
    }
  }
  if(fCreateDC) ReleaseDC(GetDesktopWindow(), hdc);

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
  rc = O32_GetPaletteEntries(hpal, uStartIndex,
                             cEntries, (PALETTEENTRY *)pColors);
  for(i=0;
      i<cEntries;
      i++)
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
  else
    return(0);
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

    dprintf(("GDI32: GetDIBits %x %x %d %d %x %x (biBitCount %d) %d", hdc, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, lpbi->bmiHeader.biBitCount, uUsage));

    //SvL: WGSS screws up the DC if it's a memory DC
    //     TODO: Fix in WGSS
    HDC hdcMem = CreateCompatibleDC(0);

    nrlines = O32_GetDIBits(hdcMem, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, uUsage);

    DeleteDC(hdcMem);

    // set proper color masks
    switch(lpbi->bmiHeader.biBitCount) {
    case 16: //RGB 565
       ((DWORD*)(lpbi->bmiColors))[0] = 0xF800;
       ((DWORD*)(lpbi->bmiColors))[1] = 0x03E0;
       ((DWORD*)(lpbi->bmiColors))[2] = 0x001F;
       break;
    case 24:
    case 32:
       ((DWORD*)(lpbi->bmiColors))[0] = 0x0000FF;
       ((DWORD*)(lpbi->bmiColors))[1] = 0x00FF00;
       ((DWORD*)(lpbi->bmiColors))[2] = 0xFF0000;
       break;
    }
    if(nrlines && lpvBits && lpbi->bmiHeader.biBitCount == 16 && ((DWORD*)(lpbi->bmiColors))[1] == 0x3E0) 
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
    dprintf(("GDI32: GetDIBits returned %d", nrlines));
    return nrlines;
}
//******************************************************************************
//******************************************************************************
int WIN32API SetDIBits(HDC hdc, HBITMAP hBitmap, UINT startscan, UINT numlines, const VOID *pBits,
                       const BITMAPINFO *pBitmapInfo, UINT usage)
{
    int ret;

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
    ret = O32_SetDIBits(hdc, hBitmap, startscan, numlines, pBits, pBitmapInfo, usage);
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
