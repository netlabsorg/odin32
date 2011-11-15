/* $Id: icon.cpp,v 1.15 2003-02-24 17:02:43 sandervl Exp $ */

/*
 * Win32 icon conversion functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_GPIBITMAPS
#define INCL_BITMAPFILEFORMAT
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#ifndef __EMX__
#include <iostream.h>
#endif

#include <win32api.h>
#include <win32type.h>
#include "dib.h"
#include <winicon.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_icon
#include "dbglocal.h"

#define DIB_RGB_COLORS_W   0
#define DIB_PAL_COLORS_W   1
#define CBM_INIT_W         4


//******************************************************************************
//******************************************************************************
ULONG QueryConvertedIconSize(WINBITMAPINFOHEADER *bmpHdr, int size, BOOL fResizeTo40x40 = FALSE)
{
 int bwsize, colorsize, rgbsize, iconsize;

  if(fResizeTo40x40) {
      bwsize    = DIB_GetDIBImageBytes(40, 40, 1);
      colorsize = DIB_GetDIBImageBytes(40, 40, bmpHdr->biBitCount);
  }
  else {
      bwsize    = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), 1);
      colorsize = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), bmpHdr->biBitCount);
  }

  if(bmpHdr->biBitCount <= 8)
        rgbsize = (1<<bmpHdr->biBitCount)*sizeof(RGB2);
  else  rgbsize = 0;

  if(bmpHdr->biSizeImage == 0 && bmpHdr->biCompression == 0) {
        bmpHdr->biSizeImage = bwsize + colorsize;
  }

  //SvL: 28-09-'98: cllngenu.dll has an incorrect size in the header
  if(bmpHdr->biSizeImage < colorsize) {
        bmpHdr->biSizeImage = colorsize;
  }
  //bitmapfileheader for AndXor mask + 2 RGB structs + bitmapfileheader
  //for color bitmap + RGB structs for all the colors
  //SvL, 3-3-98: 2*bwsize
  iconsize = 2*sizeof(BITMAPFILEHEADER2) + 2*sizeof(RGB2) +
             rgbsize + 2*bwsize + colorsize;

  return iconsize;
}
//******************************************************************************
//NOTE: offsetBits is the value added to the offBits bitmap structure members
//      (handy for converting icon groups)
//******************************************************************************
void *ConvertIcon(WINBITMAPINFOHEADER *bmpHdr, int size, int *os2size, int offsetBits,
                  BOOL fResizeTo40x40 = FALSE)
{
 RGBQUAD *rgb;
 RGB2    *os2rgb;
 int bwsize, i, colorsize, rgbsize, iconsize, orgbwsize, orgcolorsize;
 BITMAPFILEHEADER2 *iconhdr;
 BITMAPFILEHEADER2 *iconhdr2;
 char *pAnd, *pXor;

  if(fResizeTo40x40) {
      orgbwsize    = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), 1);
      orgcolorsize = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), bmpHdr->biBitCount);
      bwsize       = DIB_GetDIBImageBytes(40, 40, 1);
      colorsize    = DIB_GetDIBImageBytes(40, 40, bmpHdr->biBitCount);
  }
  else {
      bwsize    = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), 1);
      colorsize = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), bmpHdr->biBitCount);
  }
  //SvL: 28-09-'98: only for <= 8
  if(bmpHdr->biBitCount <= 8)
        rgbsize = (1<<bmpHdr->biBitCount)*sizeof(RGB2);
  else  rgbsize = 0;

  if(bmpHdr->biSizeImage == 0 && bmpHdr->biCompression == 0) {
        bmpHdr->biSizeImage = bwsize + colorsize;
  }
  dprintf(("Icon size    : %d", bmpHdr->biSizeImage));
  dprintf(("Icon Width   : %d", bmpHdr->biWidth));
  //height for both the XOR and AND bitmap (color & BW)
  dprintf(("Height       : %d", bmpHdr->biHeight));
  dprintf(("Icon Bitcount: %d", bmpHdr->biBitCount));
  dprintf(("Icon Compress: %d", bmpHdr->biCompression));

  //SvL: 28-09-'98: cllngenu.dll has an incorrect size in the header
  if(bmpHdr->biSizeImage < colorsize) {
        bmpHdr->biSizeImage = colorsize;
  }
  //bitmapfileheader for AndXor mask + 2 RGB structs + bitmapfileheader
  //for color bitmap + RGB structs for all the colors
  //SvL, 3-3-98: 2*bwsize
  iconsize = 2*sizeof(BITMAPFILEHEADER2) + 2*sizeof(RGB2) +
             rgbsize + 2*bwsize + colorsize;

  iconhdr  = (BITMAPFILEHEADER2 *)malloc(iconsize);
  memset(iconhdr, 0, iconsize);
  iconhdr->usType        = BFT_COLORICON;
  iconhdr->cbSize        = sizeof(BITMAPFILEHEADER2);
  iconhdr->xHotspot      = 0;
  iconhdr->yHotspot      = 0;
  iconhdr->offBits       = 2*sizeof(BITMAPFILEHEADER2) +
                           2*sizeof(RGB2) + rgbsize + offsetBits;
  iconhdr->bmp2.cbFix    = sizeof(BITMAPINFOHEADER2);
  if(fResizeTo40x40) {
      iconhdr->bmp2.cx   = (USHORT)40;
      iconhdr->bmp2.cy   = (USHORT)80;
  }
  else {
      iconhdr->bmp2.cx   = (USHORT)bmpHdr->biWidth;
      iconhdr->bmp2.cy   = (USHORT)bmpHdr->biHeight;
  }
  iconhdr->bmp2.cPlanes  = 1;
  iconhdr->bmp2.cBitCount= 1;
  iconhdr->bmp2.cbImage  = 2*bwsize;
  iconhdr->bmp2.cclrUsed = 2;
  iconhdr->bmp2.cclrImportant = 2;
  iconhdr->bmp2.ulCompression   = BCA_UNCOMP;
  iconhdr->bmp2.ulColorEncoding = BCE_RGB;
  os2rgb                 = (RGB2 *)(iconhdr+1);
  memset(os2rgb, 0, sizeof(RGB2));
  memset(os2rgb+1, 0xff, sizeof(RGB)); //not reserved byte!
  iconhdr2               = (BITMAPFILEHEADER2 *)(os2rgb+2);
  iconhdr2->usType       = BFT_COLORICON;
  iconhdr2->cbSize       = sizeof(BITMAPFILEHEADER2);
  iconhdr2->xHotspot     = 0;
  iconhdr2->yHotspot     = 0;
  iconhdr2->offBits      = 2*sizeof(BITMAPFILEHEADER2) +
                           2*sizeof(RGB2) + rgbsize + 2*bwsize + offsetBits;
  iconhdr2->bmp2.cbFix   = sizeof(BITMAPINFOHEADER2);
  if(fResizeTo40x40) {
      iconhdr2->bmp2.cx  = (USHORT)40;
      iconhdr2->bmp2.cy  = (USHORT)40;
  }
  else {
      iconhdr2->bmp2.cx  = (USHORT)bmpHdr->biWidth;
      iconhdr2->bmp2.cy  = (USHORT)(bmpHdr->biHeight/2);
  }
  iconhdr2->bmp2.cPlanes = bmpHdr->biPlanes;
  iconhdr2->bmp2.cBitCount= bmpHdr->biBitCount;
  iconhdr2->bmp2.cbImage  = colorsize;
  iconhdr2->bmp2.cclrUsed = bmpHdr->biClrUsed;
  iconhdr2->bmp2.cclrImportant = bmpHdr->biClrImportant;
  iconhdr2->bmp2.ulCompression   = BCA_UNCOMP;
  iconhdr2->bmp2.ulColorEncoding = BCE_RGB;
  os2rgb                 = (RGB2 *)(iconhdr2+1);
  rgb                    = (RGBQUAD *)(bmpHdr+1);
  if(bmpHdr->biBitCount <= 8) {
        for(i=0;i<(1<<bmpHdr->biBitCount);i++) {
                os2rgb->bRed   = rgb->red;
                os2rgb->bBlue  = rgb->blue;
                os2rgb->bGreen = rgb->green;
                os2rgb++;
                rgb++;
        }
  }

  if(fResizeTo40x40) 
  {
      BYTE *src, *dest;
      int linesizesrc, linesizedest;

      pXor = (char *)os2rgb;
      pAnd = (char *)os2rgb + bwsize;

      if ((size - (bmpHdr->biSize + rgbsize + orgcolorsize + orgbwsize)) == orgbwsize) 
      {//this means an AND and XOR mask is present (interleaved; and/xor)
        char *q;
        int i, linesize;

        //TODO:
        dprintf(("TODO: icon conversion not correct"));
        linesize = DIB_GetDIBWidthBytes(bmpHdr->biWidth, 1);
        q = (char *)rgb + orgcolorsize;
        for (i = 0; i < bmpHdr->biHeight/2; i++) {
          memcpy (pAnd, q, linesize);
          pAnd += linesize;
          q += linesize;

          memcpy (pXor, q, linesize);
          pXor += linesize;
          q += linesize;
        }
      } 
      else {
     	linesizesrc  = DIB_GetDIBWidthBytes(bmpHdr->biWidth, 1);
     	linesizedest = DIB_GetDIBWidthBytes(40, 1);

        src  = (BYTE *)rgb + orgcolorsize;
        dest = (BYTE *)pAnd + 4*linesizedest; //skip 4 lines
        memset((char *)pAnd, 0xFF, bwsize);
        for (i = 0; i < bmpHdr->biHeight/2; i++) {
            for(int j=0;j<linesizesrc;j++) {
                //must skip 4 pixels (4 bits)
                dest[j]   = (dest[j] & 0xF0) | ((src[j] >> 4));
                dest[j+1] = (dest[j+1] & 0x0F) | ((src[j] & 0xF) << 4);
            }
            dest += linesizedest;
            src  += linesizesrc;
        }
        memset (pXor, 0, bwsize);
      }
      linesizesrc  = DIB_GetDIBWidthBytes(32, bmpHdr->biBitCount);
      linesizedest = DIB_GetDIBWidthBytes(40, bmpHdr->biBitCount);
      int skipsize = (4*bmpHdr->biBitCount)/8; //must skip 4 pixels
      src  = (BYTE *)rgb;
      dest = (BYTE *)os2rgb+2*bwsize + 4*linesizedest; //skip 4 rows

      for (i = 0; i < 32; i++) {
          memcpy(dest+skipsize, src, linesizesrc);
          dest += linesizedest;
          src  += linesizesrc;
      }
  }
  else {
      pXor = (char *)os2rgb;
      pAnd = (char *)os2rgb + bwsize;

      if ((size - (bmpHdr->biSize + rgbsize + colorsize + bwsize)) == bwsize) 
      {//this means an AND and XOR mask is present (interleaved; and/xor)
        char *q;
        int i, linesize;

        linesize = DIB_GetDIBWidthBytes(bmpHdr->biWidth, 1);
        q = (char *)rgb + orgcolorsize;
        for (i = 0; i < bmpHdr->biHeight/2; i++) {
          memcpy (pAnd, q, linesize);
          pAnd += linesize;
          q += linesize;

          memcpy (pXor, q, linesize);
          pXor += linesize;
          q += linesize;
        }
      } 
      else {
        memcpy (pAnd, (char *)rgb + colorsize, bwsize);
        memset (pXor, 0, bwsize);
      }
      memcpy((char *)os2rgb+2*bwsize, (char *)rgb, colorsize);
  }
  *os2size = iconsize;
  return (void *)iconhdr;
}

extern "C" {

//******************************************************************************
//******************************************************************************
void * WIN32API ConvertIconGroup(void *hdr, HINSTANCE hInstance, DWORD *ressize)
{
 IconHeader *ihdr = (IconHeader *)hdr;
 ResourceDirectory *rdir = (ResourceDirectory *)(ihdr + 1);
 int i, groupsize = 0, os2iconsize;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 WINBITMAPINFOHEADER    *iconhdr;
 void                   *os2icon;
 HRSRC                   hRes;
 int                     nricons = 0;

  dprintf(("Icon Group type :%d", ihdr->wType));
  dprintf(("Icon Group count:%d", ihdr->wCount));
  for(i=0;i<ihdr->wCount;i++) {
        dprintf2(("Icon    : %d", rdir->wNameOrdinal));
        dprintf2(("Width   : %d", (int)rdir->bWidth));
        dprintf2(("Height  : %d", (int)rdir->bHeight));
        dprintf2(("Colors  : %d", (int)rdir->bColorCount));
        dprintf2(("Bits    : %d", rdir->wBitCount));
        dprintf2(("ResBytes: %d", rdir->lBytesInRes));
	hRes = FindResourceA(hInstance, 
                             (LPCSTR)rdir->wNameOrdinal, (LPSTR)NTRT_ICON);

        groupsize += QueryConvertedIconSize((WINBITMAPINFOHEADER *)LockResource(LoadResource(hInstance, hRes)),
                                            SizeofResource(hInstance, hRes));
        //add centered icon if size is 32x32
        if(rdir->bWidth == 32 && rdir->bHeight == 32 && rdir->wBitCount >= 4) 
        {
            groupsize += QueryConvertedIconSize((WINBITMAPINFOHEADER *)LockResource(LoadResource(hInstance, hRes)),
                                                SizeofResource(hInstance, hRes), TRUE);
            //extra pixels
            groupsize += (40*8 + 8*32)*rdir->wBitCount/8;
            nricons++;
        }
        nricons++;
        rdir++;
  }
  groupsize = groupsize+nricons*(sizeof(BITMAPARRAYFILEHEADER2) - sizeof(BITMAPFILEHEADER2));
  bafh    = (BITMAPARRAYFILEHEADER2 *)malloc(groupsize);
  memset(bafh, 0, groupsize);
  orgbafh = bafh;

  rdir = (ResourceDirectory *)(ihdr + 1);
  for(i=0;i<ihdr->wCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER2);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
	hRes = FindResourceA(hInstance, 
                             (LPCSTR)rdir->wNameOrdinal, (LPSTR)NTRT_ICON);
        if(hRes == NULL) {
                dprintf(("Can't find icon!"));
                rdir++;
                continue;
        }
	iconhdr = (WINBITMAPINFOHEADER *)LockResource(LoadResource(hInstance, hRes));
	os2icon = ConvertIcon(iconhdr, SizeofResource(hInstance, hRes), &os2iconsize, (ULONG)bafh - (ULONG)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));
        if(os2icon == NULL) {
                dprintf(("Can't convert icon!"));
                rdir++;
                continue;
        }

        if(rdir->bWidth == 32 && rdir->bHeight == 32 && rdir->wBitCount >= 4) 
        {
                //add 40x40 icon by centering 32x32 icon in 40x40 grid
                //(resize is really ugly)
               	bafh->offNext = (ULONG)&bafh->bfh2 - (ULONG)orgbafh + os2iconsize;
	        memcpy((char *)&bafh->bfh2, os2icon, os2iconsize);
		free(os2icon);

	        bafh = (BITMAPARRAYFILEHEADER2 *)((ULONG)&bafh->bfh2 + os2iconsize);

                os2icon = ConvertIcon(iconhdr, SizeofResource(hInstance, hRes), &os2iconsize, (ULONG)bafh - (ULONG)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2), TRUE);
        	if(os2icon == NULL) {
                	dprintf(("Can't convert icon!"));
	                rdir++;
	                continue;
	        }
        }

        if(i != ihdr->wCount -1) {
                bafh->offNext = (ULONG)&bafh->bfh2 - (ULONG)orgbafh + os2iconsize;
        }
        else    bafh->offNext = 0;

        memcpy((char *)&bafh->bfh2, os2icon, os2iconsize);
	free(os2icon);

        bafh = (BITMAPARRAYFILEHEADER2 *)((ULONG)&bafh->bfh2 + os2iconsize);

        rdir++;
  }
  *ressize = groupsize;
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************
void *WIN32API ConvertIconGroupIndirect(void *lpIconData, DWORD iconsize,
                                        DWORD *ressize)
{
 ICONDIR *ihdr = (ICONDIR *)lpIconData;
 ICONDIRENTRY *rdir = (ICONDIRENTRY *)(ihdr + 1);
 int i, groupsize = 0, os2iconsize;
 BITMAPARRAYFILEHEADER2 *bafh, *orgbafh;
 WINBITMAPINFOHEADER    *iconhdr;
 void                   *os2icon;
 int                     nricons = 0;
 void                   *winicon;

  dprintf(("Icon Group type :%d", ihdr->idType));
  dprintf(("Icon Group count:%d", ihdr->idCount));
  for(i=0;i<ihdr->idCount;i++) {
        dprintf2(("Icon    : %x", rdir->dwImageOffset));
        dprintf2(("Width   : %d", (int)rdir->bWidth));
        dprintf2(("Height  : %d", (int)rdir->bHeight));
        dprintf2(("Colors  : %d", (int)rdir->bColorCount));
        dprintf2(("Bits    : %d", rdir->wBitCount));
        dprintf2(("ResBytes: %d", rdir->dwBytesInRes));

        winicon = (char *)lpIconData + rdir->dwImageOffset;
        groupsize += QueryConvertedIconSize((WINBITMAPINFOHEADER *)winicon,
                                            rdir->dwBytesInRes);
        //add centered icon if size is 32x32
        if(rdir->bWidth == 32 && rdir->bHeight == 32 && rdir->wBitCount >= 4) 
        {
            groupsize += QueryConvertedIconSize((WINBITMAPINFOHEADER *)winicon,
                                                rdir->dwBytesInRes, TRUE);
            //extra pixels
            groupsize += (40*8 + 8*32)*rdir->wBitCount/8;
            nricons++;
        }
        nricons++;
        rdir++;
  }
  groupsize = groupsize+nricons*(sizeof(BITMAPARRAYFILEHEADER2) - sizeof(BITMAPFILEHEADER2));
  bafh    = (BITMAPARRAYFILEHEADER2 *)malloc(groupsize);
  memset(bafh, 0, groupsize);
  orgbafh = bafh;

  rdir = (ICONDIRENTRY *)(ihdr + 1);
  for(i=0;i<ihdr->idCount;i++) {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER2);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;

        winicon = (char *)lpIconData + rdir->dwImageOffset;
	iconhdr = (WINBITMAPINFOHEADER *)winicon;

	os2icon = ConvertIcon(iconhdr, rdir->dwBytesInRes, &os2iconsize, (ULONG)bafh - (ULONG)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2));
        if(os2icon == NULL) {
                dprintf(("Can't convert icon!"));
                rdir++;
                continue;
        }

        if(rdir->bWidth == 32 && rdir->bHeight == 32 && rdir->wBitCount >= 4) 
        {
                //add 40x40 icon by centering 32x32 icon in 40x40 grid
                //(resize is really ugly)
               	bafh->offNext = (ULONG)&bafh->bfh2 - (ULONG)orgbafh + os2iconsize;
	        memcpy((char *)&bafh->bfh2, os2icon, os2iconsize);
		free(os2icon);

	        bafh = (BITMAPARRAYFILEHEADER2 *)((ULONG)&bafh->bfh2 + os2iconsize);

                os2icon = ConvertIcon(iconhdr, rdir->dwBytesInRes, &os2iconsize, (ULONG)bafh - (ULONG)orgbafh + sizeof(BITMAPARRAYFILEHEADER2)-sizeof(BITMAPFILEHEADER2), TRUE);
        	if(os2icon == NULL) {
                	dprintf(("Can't convert icon!"));
	                rdir++;
	                continue;
	        }
        }

        if(i != ihdr->idCount -1) {
                bafh->offNext = (ULONG)&bafh->bfh2 - (ULONG)orgbafh + os2iconsize;
        }
        else    bafh->offNext = 0;

        memcpy((char *)&bafh->bfh2, os2icon, os2iconsize);
	free(os2icon);

        bafh = (BITMAPARRAYFILEHEADER2 *)((ULONG)&bafh->bfh2 + os2iconsize);

        rdir++;
  }
  *ressize = groupsize;
  return (void *)orgbafh;
}
//******************************************************************************
//******************************************************************************

} // extern "C"
