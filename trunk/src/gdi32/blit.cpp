/* $Id: blit.cpp,v 1.28 2001-05-24 08:18:45 sandervl Exp $ */

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

  dprintf(("GDI32: StretchBlt Dest: %x (%d, %d) size (%d, %d) ROP %x",
           hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, dwRop));
  dprintf(("GDI32: StretchBlt Src : %x (%d, %d) size (%d, %d)\n",
           hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc));
  SetLastError(ERROR_SUCCESS);
  if(DIBSection::getSection() != NULL)
  {
    DIBSection *dsect = DIBSection::findHDC(hdcSrc);
    if(dsect)
    {
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
BOOL WIN32API BitBlt(HDC hdcDest, 
                     int nXDest, 
                     int nYDest,
                     int nWidth,
                     int nHeight,
                     HDC hdcSrc, 
                     int nXSrc,
                     int nYSrc,
                     DWORD  dwRop)
{
  BOOL rc;

  SetLastError(ERROR_SUCCESS);
  if(DIBSection::getSection() != NULL) 
  {
    DIBSection *dsect = DIBSection::findHDC(hdcSrc);
    if(dsect) 
    {
      return dsect->BitBlt(hdcDest,
                           nXDest,
                           nYDest,
                           nWidth,
                           nHeight,
                           nXSrc,
                           nYSrc,
                           nWidth,
                           nHeight,
                           dwRop);
    }
  }
  dprintf(("GDI32: BitBlt to hdc %X from hdc %x (%d,%d) to (%d,%d), (%d,%d) rop %X\n", 
           hdcDest, hdcSrc, nXSrc, nYSrc, nXDest, nYDest, nWidth, nHeight, dwRop));
  return O32_BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}
//******************************************************************************
//******************************************************************************
static INT SetDIBitsToDevice_(HDC hdc, INT xDest, INT yDest, DWORD cx,
                               DWORD cy, INT xSrc, INT ySrc,
                               UINT startscan, UINT lines, LPCVOID bits,
                               const BITMAPINFO *info, UINT coloruse)
{
    INT result, imgsize, palsize, height, width;
    char *ptr;
    ULONG compression = 0, bmpsize;
    WORD *newbits = NULL;
    DWORD bitfields[3];

    dprintf(("GDI32: SetDIBitsToDevice hdc:%X xDest:%d yDest:%d, cx:%d, cy:%d, xSrc:%d, ySrc:%d, startscan:%d, lines:%d \nGDI32: bits 0x%X, info 0x%X, coloruse %d",
              hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (LPVOID) bits, (PBITMAPINFO)info, coloruse));

    SetLastError(ERROR_SUCCESS);
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

    //SvL: RP7's bitmap size is not correct; fix it here or else
    //     the blit is messed up in Open32
    imgsize = CalcBitmapSize(info->bmiHeader.biBitCount,
                             info->bmiHeader.biWidth, info->bmiHeader.biHeight);
    bmpsize = info->bmiHeader.biSizeImage;
    if(info->bmiHeader.biCompression == 0 && info->bmiHeader.biSizeImage && 
       info->bmiHeader.biSizeImage < imgsize)
    {
	((BITMAPINFO *)info)->bmiHeader.biSizeImage = imgsize;
    }

    switch(info->bmiHeader.biBitCount) {
    case 15:
    case 16:
        bitfields[0] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *(DWORD *)info->bmiColors : 0x7c00;
        bitfields[1] = (info->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)info->bmiColors + 1) : 0x03e0;
        bitfields[2] = (info->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)info->bmiColors + 2) : 0x001f;
        break;
    case 32:
        bitfields[0] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *(DWORD *)info->bmiColors : 0xff0000;
        bitfields[1] = (info->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)info->bmiColors + 1) : 0xff00;
        bitfields[2] = (info->bmiHeader.biCompression == BI_BITFIELDS) ?  *((DWORD *)info->bmiColors + 2) : 0xff;
        break;
    default:
        bitfields[0] = 0;
        bitfields[1] = 0;
        bitfields[2] = 0;
        break;
    }

    if(bitfields[1] == 0x3E0) 
    {//RGB 555?
        dprintf(("RGB 555->565 conversion required %x %x %x", bitfields[0], bitfields[1], bitfields[2]));

        newbits = (WORD *)malloc(imgsize);
        if(CPUFeatures & CPUID_MMX) {
             RGB555to565MMX(newbits, (WORD *)bits, imgsize/sizeof(WORD));
        }
        else RGB555to565(newbits, (WORD *)bits, imgsize/sizeof(WORD));
        bits = newbits;
    }

    //SvL: Ignore BI_BITFIELDS type (SetDIBitsToDevice fails otherwise)
    if(info->bmiHeader.biCompression == BI_BITFIELDS) {
        ((BITMAPINFO *)info)->bmiHeader.biCompression = 0;
        compression = BI_BITFIELDS;

    }
    if(startscan != 0 || lines != info->bmiHeader.biHeight) {
	dprintf(("WARNING: SetDIBitsToDevice startscan != 0 || lines != info->bmiHeader.biHeight"));
    }

    result = O32_StretchDIBits(hdc, xDest, yDest, cx, cy, xSrc, ySrc,
                             cx, cy, (void *)bits,
                             (PBITMAPINFO)info, coloruse, SRCCOPY);

    //Open32 always returns height of bitmap (regardless of how many scanlines were copied)
    if(result != info->bmiHeader.biHeight) {
	dprintf(("SetDIBitsToDevice failed with rc %x", result));
    }
    else 
    {
	result = info->bmiHeader.biHeight;

  	DIBSection *destdib = DIBSection::findHDC(hdc);
 	if(destdib) {
		if(cx == info->bmiHeader.biWidth && cy == info->bmiHeader.biHeight &&
                   destdib->GetBitCount() == info->bmiHeader.biBitCount &&
                   destdib->GetBitCount() == 8) 
                {
			destdib->sync(xDest, yDest, cx, cy, (PVOID)bits);
		}
		else	destdib->sync(hdc, yDest, cy);
	}
    }
    dprintf(("GDI32: SetDIBitsToDevice hdc:%X xDest:%d yDest:%d, cx:%d, cy:%d, xSrc:%d, ySrc:%d, startscan:%d, lines:%d \nGDI32: bits 0x%X, info 0x%X, coloruse %d returned %d",
                 hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (LPVOID) bits, (PBITMAPINFO)info, coloruse, result));
    dprintf(("GDI32: SetDIBitsToDevice %d %d %d %d %x %d", info->bmiHeader.biWidth, info->bmiHeader.biHeight, info->bmiHeader.biPlanes, info->bmiHeader.biBitCount, info->bmiHeader.biCompression, info->bmiHeader.biSizeImage));

    if(compression == BI_BITFIELDS) {
        ((BITMAPINFO *)info)->bmiHeader.biCompression = BI_BITFIELDS;
    }
    if(newbits) free(newbits);
    ((BITMAPINFO *)info)->bmiHeader.biSizeImage = bmpsize;
    return result;

invalid_parameter:
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}
//******************************************************************************
//******************************************************************************
INT WIN32API SetDIBitsToDevice(HDC hdc, INT xDest, INT yDest, DWORD cx,
                               DWORD cy, INT xSrc, INT ySrc,
                               UINT startscan, UINT lines, LPCVOID bits,
                               const BITMAPINFO *info, UINT coloruse)
{
    if(info->bmiHeader.biHeight < 0 && info->bmiHeader.biBitCount != 8 && info->bmiHeader.biCompression == 0) {
        // upside down
        INT rc = 0;
        BITMAPINFO newInfo;
        newInfo.bmiHeader = info->bmiHeader;
        long lLineByte = ((newInfo.bmiHeader.biWidth * (info->bmiHeader.biBitCount == 15 ? 16 : info->bmiHeader.biBitCount) + 31) / 32) * 4;
        long lHeight   = -newInfo.bmiHeader.biHeight;
        newInfo.bmiHeader.biHeight = -info->bmiHeader.biHeight;

        char *newBits = (char *)malloc( lLineByte * lHeight );
        if(newBits) {
            unsigned char *pbSrc = (unsigned char *)bits + lLineByte * (lHeight - 1);
            unsigned char *pbDst = (unsigned char *)newBits;
            for(int y = 0; y < lHeight; y++) {
                memcpy( pbDst, pbSrc, lLineByte );
                pbDst += lLineByte;
                pbSrc -= lLineByte;
            }
            rc = SetDIBitsToDevice_( hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (void *)newBits, &newInfo, DIB_RGB_COLORS );
            free( newBits );
        }
        return rc;
    }
//SvL: Breaks startup bitmap of Acrobat Reader 4.05
#if 0
    else
    if(info->bmiHeader.biBitCount == 8 && info->bmiHeader.biCompression == 0 && !(GetDeviceCaps( hdc, RASTERCAPS ) & RC_PALETTE)) {
        INT rc = 0;
        // convert 8bit to 24bit

        BITMAPINFO newInfo;
        newInfo.bmiHeader = info->bmiHeader;
        newInfo.bmiHeader.biBitCount = 24;
        long lLineByte24 = ((newInfo.bmiHeader.biWidth * 24 + 31) / 32) * 4;
        long lLineByte8  = ((newInfo.bmiHeader.biWidth *  8 + 31) / 32) * 4;
        long lHeight   = newInfo.bmiHeader.biHeight;
        if(lHeight < 0) lHeight = -lHeight;

        char *newBits = (char *)malloc( lLineByte24 * lHeight );
        if(newBits) {
            //
            // Get Palette Entries
            //
            PALETTEENTRY aEntries[256];
            LOGPALETTE *pLog = (LOGPALETTE *)malloc( sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 256 );
            pLog->palVersion = 0x300;
            pLog->palNumEntries = 256;

            HPALETTE hPaletteDummy = CreatePalette( pLog );
            free( pLog );
            HPALETTE hPalette = SelectPalette( hdc, hPaletteDummy, FALSE );
            GetPaletteEntries( hPalette, 0, 255, aEntries  );
            SelectPalette( hdc, hPalette, FALSE );
            DeleteObject( hPaletteDummy );

            //
            // convert 8bit to 24bit
            //
            if(newInfo.bmiHeader.biHeight > 0) {
                unsigned char *pbSrc = (unsigned char *)bits;
                unsigned char *pbDst = (unsigned char *)newBits;
                for(int y = 0; y < lHeight; y++) {
                    for(int x = 0; x < newInfo.bmiHeader.biWidth; x++) {
                        PALETTEENTRY src = aEntries[pbSrc[x]];
                        pbDst[x * 3 + 0] = src.peBlue;
                        pbDst[x * 3 + 1] = src.peGreen;
                        pbDst[x * 3 + 2] = src.peRed;
                    }
                    pbDst += lLineByte24;
                    pbSrc += lLineByte8;
                }
            } else {
                // upside down
                newInfo.bmiHeader.biHeight = -info->bmiHeader.biHeight;
                unsigned char *pbSrc = (unsigned char *)bits + lLineByte8 * (lHeight - 1);
                unsigned char *pbDst = (unsigned char *)newBits;
                for(int y = 0; y < lHeight; y++) {
                    for(int x = 0; x < newInfo.bmiHeader.biWidth; x++) {
                        PALETTEENTRY src = aEntries[pbSrc[x]];
                        pbDst[x * 3 + 0] = src.peBlue;
                        pbDst[x * 3 + 1] = src.peGreen;
                        pbDst[x * 3 + 2] = src.peRed;
                    }
                    pbDst += lLineByte24;
                    pbSrc -= lLineByte8;
                }
            }
            rc = SetDIBitsToDevice_( hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (void *)newBits, &newInfo, DIB_RGB_COLORS );
            free( newBits );
        }
        return rc;
    } 
#endif
    else {
        return SetDIBitsToDevice_( hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, bits, info, coloruse );
    }
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
  if(rc) {
  	DIBSection *destdib = DIBSection::findHDC(hdc);
  	if(destdib) {
		destdib->sync(hdc, nYLeft, nHeight);
  	}
  }

  dprintf(("GDI32: PatBlt hdc %x (%d,%d) (%d,%d) returned %d\n",hdc, nXLeft,nYLeft,nWidth,nHeight,rc));
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
static INT StretchDIBits_(HDC hdc, INT xDst, INT yDst, INT widthDst,
                           INT heightDst, INT xSrc, INT ySrc, INT widthSrc,
                           INT heightSrc, const void *bits,
                           const BITMAPINFO *info, UINT wUsage, DWORD dwRop )
{
 INT rc;

    dprintf(("GDI32: StretchDIBits %x to (%d,%d) (%d,%d) from (%d,%d) (%d,%d), %x %x %x %x", hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc, widthSrc, heightSrc, bits, info, wUsage, dwRop));

    if(info->bmiHeader.biBitCount == 1) {
        dprintf(("WARNING: StretchDIBits does NOT work correctly for 1 bpp bitmaps!!"));
    }

    if(wUsage == DIB_PAL_COLORS && info->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
    {
      	// workaround for open32 bug.
      	// If syscolors > 256 and wUsage == DIB_PAL_COLORS.

      	int i;
      	USHORT *pColorIndex = (USHORT *)info->bmiColors;
      	RGBQUAD *pColors = (RGBQUAD *) alloca(info->bmiHeader.biClrUsed *
                         sizeof(RGBQUAD));
      	BITMAPINFO *infoLoc = (BITMAPINFO *) alloca(sizeof(BITMAPINFO) +
                             info->bmiHeader.biClrUsed * sizeof(RGBQUAD));

      	memcpy(infoLoc, info, sizeof(BITMAPINFO));

      	if(GetDIBColorTable(hdc, 0, info->bmiHeader.biClrUsed, pColors) == 0) {
		dprintf(("ERROR: StretchDIBits: GetDIBColorTable failed!!"));
        	return FALSE;
        }
      	for(i=0;i<info->bmiHeader.biClrUsed;i++, pColorIndex++)
      	{
         	infoLoc->bmiColors[i] = pColors[*pColorIndex];
      	}

      	rc = O32_StretchDIBits(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc,
                               widthSrc, heightSrc, (void *)bits,
                               (PBITMAPINFO)infoLoc, DIB_RGB_COLORS, dwRop);

        //Open32 always returns height of bitmap (regardless of how many scanlines were copied)
      	if(rc != heightSrc && rc != infoLoc->bmiHeader.biHeight) {
		dprintf(("StretchDIBits failed with rc %x", rc));
      	}
	else {
		rc = heightSrc;

  		DIBSection *destdib = DIBSection::findHDC(hdc);
  		if(destdib) {
			if(widthDst == widthSrc && heightDst == heightSrc &&
                           destdib->GetBitCount() == infoLoc->bmiHeader.biBitCount &&
                           destdib->GetBitCount() == 8) 
                        {
				destdib->sync(xDst, yDst, widthDst, heightDst, (PVOID)bits);
 			}
			else	destdib->sync(hdc, yDst, heightDst);
  		}
	}

	return rc;
    }
    rc = O32_StretchDIBits(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc,
                             widthSrc, heightSrc, (void *)bits,
                             (PBITMAPINFO)info, wUsage, dwRop);
    //Open32 always returns height of bitmap (regardless of how many scanlines were copied)
    if(rc != heightSrc && rc != info->bmiHeader.biHeight) {
	dprintf(("StretchDIBits failed with rc %x", rc));
    }
    else 
    {
	rc = heightSrc;

  	DIBSection *destdib = DIBSection::findHDC(hdc);
 	if(destdib) {
		if(widthDst == widthSrc && heightDst == heightSrc &&
                   destdib->GetBitCount() == info->bmiHeader.biBitCount &&
                   destdib->GetBitCount() == 8) 
                {
			destdib->sync(xDst, yDst, widthDst, heightDst, (PVOID)bits);
		}
		else	destdib->sync(hdc, yDst, heightDst);
	}
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
INT WIN32API StretchDIBits(HDC hdc, INT xDst, INT yDst, INT widthDst,
                           INT heightDst, INT xSrc, INT ySrc, INT widthSrc,
                           INT heightSrc, const void *bits,
                           const BITMAPINFO *info, UINT wUsage, DWORD dwRop )
{

    if(info->bmiHeader.biHeight < 0) {
        // upside down
        INT rc = 0;
        BITMAPINFO newInfo;
        newInfo.bmiHeader = info->bmiHeader;
        long lLineByte = ((newInfo.bmiHeader.biWidth * (info->bmiHeader.biBitCount == 15 ? 16 : info->bmiHeader.biBitCount) + 31) / 32) * 4;
        long lHeight   = -newInfo.bmiHeader.biHeight;
        newInfo.bmiHeader.biHeight = -newInfo.bmiHeader.biHeight;

        char *newBits = (char *)malloc( lLineByte * lHeight );
        if(newBits) {
            unsigned char *pbSrc = (unsigned char *)bits + lLineByte * (lHeight - 1);
            unsigned char *pbDst = (unsigned char *)newBits;
            for(int y = 0; y < lHeight; y++) {
                memcpy( pbDst, pbSrc, lLineByte );
                pbDst += lLineByte;
                pbSrc -= lLineByte;
            }
            rc = StretchDIBits_(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc, widthSrc, heightSrc, newBits, info, wUsage, dwRop);
            free( newBits );
        }
        return rc;
    } else {
        return StretchDIBits_(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc, widthSrc, heightSrc, bits, info, wUsage, dwRop);
    }
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
