/* $Id: blit.cpp,v 1.44 2003-05-02 15:32:27 sandervl Exp $ */

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
#include <winuser32.h>
#include <dbglog.h>
#include "dibsect.h"
#include "rgbcvt.h"

#define DBG_LOCALLOG	DBG_blit
#include "dbglocal.h"

static ULONG QueryPaletteSize(BITMAPINFOHEADER *pBHdr);
ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy);

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
            rc  = dsect->BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                                nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);
            return rc;
        }
    }
    rc = O32_StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);
    if(DIBSection::getSection() != NULL) {
        DIBSection *destdib = DIBSection::findHDC(hdcDest);
        if(destdib) {
            destdib->sync(hdcDest, nYOriginDest, nHeightDest);
        }
    }
    if(rc == FALSE) {
        dprintf(("!WARNING!: GDI32: StretchBlt returned FALSE; last error %x", rc, GetLastError()));
    }
    return rc;
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

#ifdef DEBUG
      POINT point1, point2;
    GetViewportOrgEx(hdcDest, &point1);
    GetViewportOrgEx(hdcSrc, &point2);
    dprintf(("BitBlt: Viewport origin dest (%d,%d) src (%d,%d)", point1.x, point1.y, point2.x, point2.y));
#endif

    SetLastError(ERROR_SUCCESS);
    if(DIBSection::getSection() != NULL) 
    {
        DIBSection *dsect = DIBSection::findHDC(hdcSrc);
        if(dsect) 
        {
            return dsect->BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc, nWidth, nHeight, dwRop);
        }
    }
    dprintf(("GDI32: BitBlt to hdc %X from hdc %x (%d,%d) to (%d,%d), (%d,%d) rop %X\n", 
             hdcDest, hdcSrc, nXSrc, nYSrc, nXDest, nYDest, nWidth, nHeight, dwRop));

    rc = O32_BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);

    if(DIBSection::getSection() != NULL) {
        DIBSection *destdib = DIBSection::findHDC(hdcDest);
        if(destdib) {
            destdib->sync(hdcDest, nYDest, nHeight);
        }
    }
    return rc;
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
    case 16: //Default if BI_BITFIELDS not set is RGB 555
        bitfields[0] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *(DWORD *)info->bmiColors       : DEFAULT_16BPP_RED_MASK;
        bitfields[1] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 1) : DEFAULT_16BPP_GREEN_MASK;
        bitfields[2] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 2) : DEFAULT_16BPP_BLUE_MASK;
        break;

    case 24:
    case 32:
        bitfields[0] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *(DWORD *)info->bmiColors       : DEFAULT_24BPP_RED_MASK;
        bitfields[1] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 1) : DEFAULT_24BPP_GREEN_MASK;
        bitfields[2] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 2) : DEFAULT_24BPP_BLUE_MASK;
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
        else
          RGB555to565(newbits, (WORD *)bits, imgsize/sizeof(WORD));
        bits = newbits;
    }

    //SvL: Ignore BI_BITFIELDS type (SetDIBitsToDevice fails otherwise)
    if(info->bmiHeader.biCompression == BI_BITFIELDS) {
        ((BITMAPINFO *)info)->bmiHeader.biCompression = 0;
        compression = BI_BITFIELDS;
    }
    if(startscan != 0 || lines != info->bmiHeader.biHeight) {
	    dprintf(("WARNING: SetDIBitsToDevice startscan != 0 || lines != info->bmiHeader.biHeight"));
        dprintf(("info bmp (%d,%d)", info->bmiHeader.biWidth, info->bmiHeader.biHeight));
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
    static BOOL fMatrox32BppBug = FALSE;
    INT rc = 0;
    char *newBits = NULL;

    if(startscan != 0 || lines != abs(info->bmiHeader.biHeight)) {
        dprintf(("WARNING: SetDIBitsToDevice: startscan != 0 || lines != abs(info->bmiHeader.biHeight"));
    }

    //If upside down, reverse scanlines and call SetDIBitsToDevice again
//    if(info->bmiHeader.biHeight < 0 && info->bmiHeader.biBitCount != 8 && info->bmiHeader.biCompression == 0) {
    if(info->bmiHeader.biHeight < 0 && (info->bmiHeader.biCompression == BI_RGB ||
       info->bmiHeader.biCompression == BI_BITFIELDS)) 
    {
        // upside down
        INT rc = -1;
        long lLineByte = DIB_GetDIBWidthBytes(info->bmiHeader.biWidth, info->bmiHeader.biBitCount);
        long lHeight   = -info->bmiHeader.biHeight;

        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFO *)info)->bmiHeader.biHeight = -info->bmiHeader.biHeight;

        char *newBits = (char *)malloc( lLineByte * lHeight );
        if(newBits) {
            unsigned char *pbSrc = (unsigned char *)bits + xSrc + lLineByte * (ySrc + lHeight - 1);
            unsigned char *pbDst = (unsigned char *)newBits;
            for(int y = 0; y < lHeight; y++) {
                memcpy( pbDst, pbSrc, lLineByte );
                pbDst += lLineByte;
                pbSrc -= lLineByte;
            }
            //We only convert the necessary data so xSrc & ySrc are now 0
            rc = SetDIBitsToDevice( hdc, xDest, yDest, cx, cy, 0, 0, startscan, lines, (void *)newBits, info, coloruse );
            free( newBits );
        }
        else DebugInt3();

        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFO *)info)->bmiHeader.biHeight = -info->bmiHeader.biHeight;

        return rc;
    }

    //We must convert 32 bpp bitmap data to 24 bpp on systems with the Matrox 
    //display driver. (GpiDrawBits for 32 bpp fails with insufficient memory error)
    if(info->bmiHeader.biBitCount == 32 && fMatrox32BppBug) 
    {
        BITMAPINFO newInfo;
        newInfo.bmiHeader = info->bmiHeader;

        long lLineWidth;
        long lHeight    = (newInfo.bmiHeader.biHeight > 0) ? newInfo.bmiHeader.biHeight : -newInfo.bmiHeader.biHeight;
        long lWidth     = newInfo.bmiHeader.biWidth;

        newInfo.bmiHeader.biBitCount  = 24;
        newInfo.bmiHeader.biSizeImage = CalcBitmapSize(24, newInfo.bmiHeader.biWidth, 
                                                       newInfo.bmiHeader.biHeight);

        lLineWidth = newInfo.bmiHeader.biSizeImage / lHeight;

        //convert 32 bits bitmap data to 24 bits
        newBits = (char *)malloc(newInfo.bmiHeader.biSizeImage+16); //extra room needed for copying (too much)
        if(!newBits) {
            DebugInt3();
            return -1;
        }
        unsigned char *pbSrc = (unsigned char *)bits;
        unsigned char *pbDst = (unsigned char *)newBits;
        //not very efficient
        for(int i = 0; i < lHeight; i++) {
            for(int j=0;j<lWidth;j++) {
                *(DWORD *)pbDst = *(DWORD *)pbSrc;
                pbSrc += 4;
                pbDst += 3;
            }
            //24 bpp scanline must be aligned at 4 byte boundary
            pbDst += (lLineWidth - 3*lWidth);
        }
        rc = SetDIBitsToDevice_( hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, newBits, &newInfo, coloruse );
        free(newBits);
        return rc;
    }
    rc = SetDIBitsToDevice_( hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, bits, info, coloruse );

    if(rc == -1 && info->bmiHeader.biBitCount == 32 && !fMatrox32BppBug) 
    {
        //The Matrox driver seems to have some difficulty blitting 32bpp
        //data. (out of memory error) The same operation works fine with SDD.
        fMatrox32BppBug = TRUE;
        return SetDIBitsToDevice(hdc, xDest, yDest, cx,
                                 cy, xSrc, ySrc,
                                 startscan, lines, bits,
                                 info, coloruse);
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PatBlt(HDC hdc,int nXLeft,int nYLeft,int nWidth,int nHeight,DWORD dwRop)
{
    BOOL rc;

    dprintf(("PatBlt %x (%d,%d)(%d,%d) %x", hdc, nXLeft,nYLeft,nWidth,nHeight, dwRop));
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
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MaskBlt( HDC arg1, int arg2, int arg3, int arg4, int arg5, HDC   arg6, int arg7, int arg8, HBITMAP arg9, int arg10, int arg11, DWORD  arg12)
{
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
    DWORD bitfields[3], compression = 0;
    WORD *newbits = NULL;

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

    switch(info->bmiHeader.biBitCount) {
    case 15:
    case 16: //Default if BI_BITFIELDS not set is RGB 555
        bitfields[0] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *(DWORD *)info->bmiColors       : DEFAULT_16BPP_RED_MASK;
        bitfields[1] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 1) : DEFAULT_16BPP_GREEN_MASK;
        bitfields[2] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 2) : DEFAULT_16BPP_BLUE_MASK;
        break;
    case 24:
    case 32:
        bitfields[0] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *(DWORD *)info->bmiColors       : DEFAULT_24BPP_RED_MASK;
        bitfields[1] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 1) : DEFAULT_24BPP_GREEN_MASK;
        bitfields[2] = (info->bmiHeader.biCompression == BI_BITFIELDS) ? *((DWORD *)info->bmiColors + 2) : DEFAULT_24BPP_BLUE_MASK;
        break;
    default:
        bitfields[0] = 0;
        bitfields[1] = 0;
        bitfields[2] = 0;
        break;
    }
    if(bitfields[1] == RGB555_GREEN_MASK) 
    {//RGB 555?
        dprintf(("RGB 555->565 conversion required %x %x %x", bitfields[0], bitfields[1], bitfields[2]));

        ULONG imgsize = CalcBitmapSize(info->bmiHeader.biBitCount,
                                     widthSrc, heightSrc);
        ULONG offset = CalcBitmapSize(info->bmiHeader.biBitCount,
                                     xSrc, ySrc)/sizeof(WORD);
        newbits = (WORD *) HeapAlloc(GetProcessHeap(), 0, imgsize);
//bugbug (too much)
//bugbug
        if(CPUFeatures & CPUID_MMX) {
             RGB555to565MMX(newbits, (WORD *)bits+offset, imgsize/sizeof(WORD));
        }
        else 
          RGB555to565(newbits, (WORD *)bits+offset, imgsize/sizeof(WORD));
        bits = newbits;
    }
    //SvL: Ignore BI_BITFIELDS type (StretchDIBits fails otherwise)
    if(info->bmiHeader.biCompression == BI_BITFIELDS) {
        ((BITMAPINFO *)info)->bmiHeader.biCompression = 0;
        compression = BI_BITFIELDS;
    }

    rc = O32_StretchDIBits(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc,
                             widthSrc, heightSrc, (void *)bits,
                             (PBITMAPINFO)info, wUsage, dwRop);

    if(compression == BI_BITFIELDS) {
        ((BITMAPINFO *)info)->bmiHeader.biCompression = BI_BITFIELDS;
    }
    if(newbits) HeapFree(GetProcessHeap(), 0, newbits);

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
		    else destdib->sync(hdc, yDst, heightDst);
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

        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFO *)info)->bmiHeader.biHeight = -info->bmiHeader.biHeight;

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

        //TODO: doesn't work if memory is readonly!!
        ((BITMAPINFO *)info)->bmiHeader.biHeight = -info->bmiHeader.biHeight;
        return rc;
    } else {
        return StretchDIBits_(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc, widthSrc, heightSrc, bits, info, wUsage, dwRop);
    }
}
//******************************************************************************
//******************************************************************************
int WIN32API SetStretchBltMode( HDC arg1, int  arg2)
{
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
ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy)
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
                        return cx*cy*4;

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
