/* $Id: blit.cpp,v 1.7 2000-04-02 10:12:53 sandervl Exp $ */

/*
 * GDI32 blit code
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
#include <cpuhlp.h>
#include "misc.h"
#include "dibsect.h"
#include "rgbcvt.h"

#define DBG_LOCALLOG	DBG_blit
#include "dbglocal.h"

static ULONG QueryPaletteSize(BITMAPINFOHEADER *pBHdr);
static ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy);

//******************************************************************************
//******************************************************************************
BOOL WIN32API StretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
                            int nWidthDest, int nHeightDest,
                            HDC hdcSrc, int nXOriginSrc, int nYOriginSrc,
                            int nWidthSrc, int nHeightSrc, DWORD dwRop)
{
 BOOL rc;

  dprintf(("GDI32: StretchBlt Dest: %x (%d, %d) size (%d, %d)\n",
           hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest));
  dprintf(("GDI32: StretchBlt Src : %x (%d, %d) size (%d, %d)\n",
           hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc));
  if(DIBSection::getSection() != NULL)
  {
    DIBSection *dsect = DIBSection::findHDC(hdcSrc);
    if(dsect)
    {
        dprintf((" Do stretched DIB Blt\n"));
        rc  = dsect->BitBlt( hdcDest,
                             nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                             nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc,
                             dwRop);
        return rc;
    }
  }
  return O32_StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BitBlt(HDC hdcDest, int arg2, int arg3, int arg4, int arg5, HDC hdcSrc, int arg7, int arg8, DWORD  arg9)
{
 BOOL rc;

    if(DIBSection::getSection() != NULL) {
        DIBSection *dsect = DIBSection::findHDC(hdcSrc);
        if(dsect) {
                rc = dsect->BitBlt(hdcDest, arg2, arg3, arg4, arg5, arg7, arg8, arg4, arg5, arg9);
                if(rc) {
                        BITMAPINFO bmpinfo = {0};
                        DIBSection *dest = DIBSection::findHDC(hdcDest);
                        if(dest) {
                                dprintf(("Sync dest DIB section"));
                                bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                                GetDIBits(hdcDest, dest->GetBitmapHandle(), 0, 0, 0, &bmpinfo, dest->GetRGBUsage());
                                dprintf(("height        %d", bmpinfo.bmiHeader.biHeight));
                                dprintf(("width         %d", bmpinfo.bmiHeader.biWidth));
                                dprintf(("biBitCount    %d", bmpinfo.bmiHeader.biBitCount));
                                GetDIBits(hdcDest, dest->GetBitmapHandle(), 0, bmpinfo.bmiHeader.biHeight, dest->GetDIBObject(), &bmpinfo, dest->GetRGBUsage());
                        }
                }
                return rc;
        }
    }
    dprintf(("GDI32: BitBlt to hdc %X from (%d,%d) to (%d,%d), (%d,%d) rop %X\n", hdcDest, arg7, arg8, arg2, arg3, arg4, arg5, arg9));
    return O32_BitBlt(hdcDest, arg2, arg3, arg4, arg5, hdcSrc, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
INT WIN32API SetDIBitsToDevice(HDC hdc, INT xDest, INT yDest, DWORD cx,
                               DWORD cy, INT xSrc, INT ySrc,
                               UINT startscan, UINT lines, LPCVOID bits,
                               const BITMAPINFO *info, UINT coloruse)
{
    INT result, imgsize, palsize, height, width;
    char *ptr;
    ULONG compression = 0, iHeight, bmpsize;
    WORD *newbits = 0;

    SetLastError(0);
    if(info == NULL) {
        goto invalid_parameter;
    }
    height = info->bmiHeader.biHeight;
    width  = info->bmiHeader.biWidth;

    if (height < 0) height = -height;
    if (!lines || (startscan >= height)) {
        goto invalid_parameter;
    }
    if (startscan + lines > height) lines = height - startscan;

    if (ySrc < startscan) ySrc = startscan;
    else if (ySrc >= startscan + lines) goto invalid_parameter;

    if (xSrc >= width) goto invalid_parameter;

    if (ySrc + cy >= startscan + lines) cy = startscan + lines - ySrc;

    if (xSrc + cx >= width) cx = width - xSrc;

    if (!cx || !cy) goto invalid_parameter;

    // EB: ->>> Crazy. Nobody seen this Open32 bug ?
    // Dont't like dirty pointers, but Open32 needs a bit help.
    // Only tested with winmine.
    palsize = QueryPaletteSize((BITMAPINFOHEADER*)&info->bmiHeader);
    imgsize = CalcBitmapSize(info->bmiHeader.biBitCount,
                             info->bmiHeader.biWidth, info->bmiHeader.biHeight);
    ptr = ((char *)info) + palsize + sizeof(BITMAPINFOHEADER);
    if(bits >= ptr && bits < ptr + imgsize)
    {
        bits = (char *)bits - imgsize +
                   CalcBitmapSize(info->bmiHeader.biBitCount,
                                  info->bmiHeader.biWidth, lines);
    }
    // EB: <<<-

    //SvL: RP7's bitmap size is not correct; fix it here or else
    //     the blit is messed up in Open32
    bmpsize = info->bmiHeader.biSizeImage;
    if(info->bmiHeader.biSizeImage && info->bmiHeader.biSizeImage < imgsize)
    {
	((BITMAPINFO *)info)->bmiHeader.biSizeImage = imgsize;
    }

    //SvL: Ignore BI_BITFIELDS type (SetDIBitsToDevice fails otherwise)
    if(info->bmiHeader.biCompression == BI_BITFIELDS) {
        DWORD *bitfields = (DWORD *)info->bmiColors;

        ((BITMAPINFO *)info)->bmiHeader.biCompression = 0;
        compression = BI_BITFIELDS;

        if(*(bitfields+1) == 0x3E0) 
	{//RGB 555?
        	dprintf(("BI_BITFIELDS compression %x %x %x", *bitfields, *(bitfields+1), *(bitfields+2)));

                newbits = (WORD *)malloc(imgsize);
		if(CPUFeatures & CPUID_MMX) {
			RGB555to565MMX(newbits, (WORD *)bits, imgsize/sizeof(WORD));
		}
		else   	RGB555to565(newbits, (WORD *)bits, imgsize/sizeof(WORD));
                bits = newbits;
        }
    }

    iHeight = info->bmiHeader.biHeight;
    if(info->bmiHeader.biHeight < 0) {
        ((BITMAPINFO *)info)->bmiHeader.biHeight = -info->bmiHeader.biHeight;
    }
    result = O32_SetDIBitsToDevice(hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (PVOID) bits, (PBITMAPINFO)info, coloruse);
    //SvL: Wrong Open32 return value
    result = (result == TRUE) ? lines : 0;

    dprintf(("GDI32: SetDIBitsToDevice hdc:%X xDest:%d yDest:%d, cx:%d, cy:%d, xSrc:%d, ySrc:%d, startscan:%d, lines:%d \nGDI32: bits 0x%X, info 0x%X, coloruse %d returned %d",
                 hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (LPVOID) bits, (PBITMAPINFO)info, coloruse, result));
    dprintf(("GDI32: SetDIBitsToDevice %d %d %d %d %x %d", info->bmiHeader.biWidth, info->bmiHeader.biHeight, info->bmiHeader.biPlanes, info->bmiHeader.biBitCount, info->bmiHeader.biCompression, info->bmiHeader.biSizeImage));

    if(compression == BI_BITFIELDS) {
        ((BITMAPINFO *)info)->bmiHeader.biCompression = BI_BITFIELDS;
        if(newbits) free(newbits);
    }
    ((BITMAPINFO *)info)->bmiHeader.biHeight = iHeight;
    ((BITMAPINFO *)info)->bmiHeader.biSizeImage = bmpsize;
    return result;

invalid_parameter:
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PatBlt(HDC hdc,int nXLeft,int nYLeft,int nWidth,int nHeight,DWORD dwRop)
{
  BOOL rc;

  //CB: Open32 bug: negative width/height not supported!
  if (nWidth < 0)
  {
    nXLeft += nWidth+1;
    nWidth = -nWidth;
  }
  if (nHeight < 0)
  {
    nYLeft += nHeight+1;
    nHeight = -nHeight;
  }
  rc = O32_PatBlt(hdc,nXLeft,nYLeft,nWidth,nHeight,dwRop);
  dprintf(("GDI32: PatBlt (%d,%d) (%d,%d) returned %d\n",nXLeft,nYLeft,nWidth,nHeight,rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MaskBlt( HDC arg1, int arg2, int arg3, int arg4, int arg5, HDC   arg6, int arg7, int arg8, HBITMAP arg9, int arg10, int arg11, DWORD  arg12)
{
    dprintf(("GDI32: MaskBlt"));
    return O32_MaskBlt(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlgBlt(HDC hdcDest, CONST POINT *lpPoint, HDC hdcSrc, int nXSrc,
                        int nYSrc, int nWidth, int nHeight, HBITMAP hbmMask,
                        int xMast, int yMask)
{
  dprintf(("GDI32: PlgBlt, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetStretchBltMode( HDC arg1, int  arg2)
{
  dprintf(("GDI32: SetStretchBltMode 0x%08X, 0x%08X\n",arg1, arg2));

  if(DIBSection::getSection() != NULL)
  {
    DIBSection *dsect = DIBSection::findHDC(arg1);
    if(dsect)
    {
      dprintf(("       - DC is DIBSection\n"));
    }
  }
  return O32_SetStretchBltMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetStretchBltMode( HDC arg1)
{
    dprintf(("GDI32: GetStretchBltMode"));
    return O32_GetStretchBltMode(arg1);
}
//******************************************************************************
//******************************************************************************
static ULONG QueryPaletteSize(BITMAPINFOHEADER *pBHdr)
{
        ULONG cbPalette;

        switch (pBHdr->biBitCount)
        {
                case 1:
                case 4:
                case 8:
                        cbPalette = (1 << pBHdr->biBitCount) * sizeof(RGBQUAD);
                        break;

                case 16:
                case 24:
                case 32:
                        cbPalette = 0;
                        break;

                default:
                        dprintf(("QueryPaletteSize: error pBHdr->biBitCount = %d", pBHdr->biBitCount));
                        cbPalette = -1;
        }

   return cbPalette;
}
//******************************************************************************
//******************************************************************************
static ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy)
{
        ULONG alignment;
        ULONG factor;
        BOOL flag = TRUE;       //true: '*'     false: '/'

        cy = cy < 0 ? -cy : cy;

        switch(cBits)
        {
                case 1:
                        factor = 8;
                        flag = FALSE;
                        break;

                case 4:
                        factor = 2;
                        flag = FALSE;
                        break;

                case 8:
                        factor = 1;
                        break;

                case 16:
                        factor = 2;
                        break;

                case 24:
                        factor = 3;
                        break;

                case 32:
                        return cx*cy;

                default:
                        return 0;
        }

        if (flag)
                alignment = (cx = (cx*factor)) % 4;
        else
                alignment = (cx = ((cx+factor-1)/factor)) % 4;

        if (alignment != 0)
                cx += 4 - alignment;

        return cx*cy;
}
//******************************************************************************
//******************************************************************************
