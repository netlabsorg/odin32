/* $Id: dibitmap.cpp,v 1.6 2000-03-24 19:24:47 sandervl Exp $ */

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
#include "misc.h"
#include "dibsect.h"

#define DBG_LOCALLOG	DBG_dibitmap
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateDIBitmap(HDC hdc, const BITMAPINFOHEADER *lpbmih, 
                                DWORD fdwInit, const void *lpbInit, 
                                const BITMAPINFO *lpbmi, UINT fuUsage)
{
  int iHeight;
  HBITMAP rc;

    //TEMPORARY HACK TO PREVENT CRASH IN OPEN32 (WSeB GA)

    iHeight = lpbmih->biHeight;
    if(lpbmih->biHeight < 0)
    {
	dprintf(("GDI32: CreateDIBitmap negative height! (%d,%d)", lpbmih->biWidth, lpbmih->biHeight));
      	((BITMAPINFOHEADER *)lpbmih)->biHeight = -lpbmih->biHeight;
    }

    rc = O32_CreateDIBitmap(hdc, lpbmih, fdwInit, lpbInit, lpbmi, fuUsage);

    dprintf(("GDI32: CreateDIBitmap %x %x %x %x returned %x", hdc, fdwInit, lpbInit, fuUsage, rc));

    ((BITMAPINFOHEADER *)lpbmih)->biHeight = iHeight;

    return rc;
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateCompatibleBitmap( HDC hdc, int nWidth, int nHeight)
{
    dprintf(("GDI32: CreateCompatibleBitmap %x (%d,%d)", hdc, nWidth, nHeight));
    return O32_CreateCompatibleBitmap(hdc, nWidth, nHeight);
}
//******************************************************************************
//CreateDisardableBitmap is obsolete and can be replaced by CreateCompatibleBitmap
//******************************************************************************
HBITMAP WIN32API CreateDiscardableBitmap(HDC hDC, int nWidth, int nHeight)
{
    dprintf(("GDI32: CreateDisardableBitmap\n"));
    return O32_CreateCompatibleBitmap(hDC, nWidth, nHeight);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmap(int nWidth, int nHeight, UINT cPlanes,
                                 UINT cBitsPerPel, const void *lpvBits)
{
 HBITMAP rc;

    rc = O32_CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits);
    dprintf(("GDI32: CreateBitmap (%d,%d) bps %d returned %x", nWidth, nHeight, cBitsPerPel, rc));
    return(rc);
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

  dprintf(("GDI32: CreateDIBSection %x %x %x %x %d", hdc, iUsage, ppvBits, hSection, dwOffset));

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
       ULONG Pal[256];
       LOGPALETTE tmpPal = { 0x300,1,{0,0,0,0}};
       HPALETTE hpalCur, hpalTmp;

        // Now get the current Palette from the DC
        hpalTmp = CreatePalette(&tmpPal);
        hpalCur = SelectPalette(hdc, hpalTmp, FALSE);

        // and use it to set the DIBColorTable
        GetPaletteEntries( hpalCur, 0, 1<<PalSize, (LPPALETTEENTRY)&Pal);
        dsect->SetDIBColorTable(0, 1<< PalSize, (RGBQUAD*)&Pal);

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

      if(fCreateDC)	ReleaseDC(GetDesktopWindow(), hdc);
      return(res);
    }
  }
  if(fCreateDC)	ReleaseDC(GetDesktopWindow(), hdc);

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
UINT WIN32API GetDIBColorTable( HDC hdc, UINT uStartIndex, UINT cEntries,
                                  RGBQUAD *pColors)
{
 HPALETTE hpal = GetCurrentObject(hdc, OBJ_PAL);
 UINT rc;
 int i;

  rc = O32_GetPaletteEntries(hpal,
                         uStartIndex,
                         cEntries,
                         (PALETTEENTRY *)pColors);
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

  dprintf(("GDI32: SetDIBColorTable\n"));
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
 int rc;

    rc =  O32_GetDIBits(hdc, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, uUsage);
    dprintf(("GDI32: GetDIBits %x %x %d %d %x %x %d returned %d", hdc, hBitmap, uStartScan, cScanLines, lpvBits, lpbi, uUsage, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API SetDIBits( HDC arg1, HBITMAP arg2, UINT arg3, UINT arg4, const VOID * arg5, const BITMAPINFO * arg6, UINT  arg7)
{
    dprintf(("GDI32: SetDIBits %x %x %x %x %x %x %x\n", arg1, arg2, arg3, arg4, arg5, arg6, arg7));

    if(DIBSection::getSection() != NULL) 
    {
        DIBSection *dsect;

        dsect = DIBSection::find((DWORD)arg2);
        if(dsect) {
           return dsect->SetDIBits(arg1, arg2, arg3, arg4, arg5, (BITMAPINFOHEADER_W *)&arg6->bmiHeader, arg7);
        }
    }
    return O32_SetDIBits(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
