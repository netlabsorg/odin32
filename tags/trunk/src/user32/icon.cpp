/* $Id: icon.cpp,v 1.11 2001-08-08 10:07:18 sandervl Exp $ */

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
#include <iostream.h>
#include <string.h>

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

#if 0
//******************************************************************************
//******************************************************************************
PBYTE ConvertWin32Icon(PBYTE presbits, DWORD dwResSize, DWORD *OS2ResSize)
{
 WINBITMAPINFOHEADER *bmpHdr = (WINBITMAPINFOHEADER *)presbits;
 BITMAPFILEHEADER    *iconhdr;  
 BITMAPFILEHEADER    *iconhdr2;
 RGBQUAD *rgb;
 RGB     *os2rgb;
 int bwsize, i, colorsize, rgbsize, iconsize;

  bwsize   = (bmpHdr->biWidth*(bmpHdr->biHeight/2))/8;
  colorsize = bmpHdr->biWidth*(bmpHdr->biHeight/2);
  if(bmpHdr->biBitCount < 24)	
	rgbsize = (1<<bmpHdr->biBitCount)*sizeof(RGB);
  else  rgbsize = 0;

  switch(bmpHdr->biBitCount) {
	case 1:
		colorsize /= 8;
		break;
	case 4:
		colorsize /= 2;
		break;
	case 8:
		break;
	case 16:
		colorsize *= 2;
		break;
	case 24:
		colorsize *= 3;
		break;
	case 32:
		colorsize *= 4;
		break;
  }
  if(bmpHdr->biSizeImage == 0 && bmpHdr->biCompression == 0) {
	bmpHdr->biSizeImage = bwsize + colorsize;
  }
#if 0
  cout << "Icon size    : " << bmpHdr->biSizeImage << endl;
  cout << "Icon Width   : " << bmpHdr->biWidth << endl;
//height for both the XOR and AND bitmap (color & BW)
  cout << "Height       : " << bmpHdr->biHeight << endl;
  cout << "Icon Bitcount: " << bmpHdr->biBitCount << endl;
  cout << "Icon Compress: " << bmpHdr->biCompression << endl;
#endif

  //bitmapfileheader for AndXor mask + 2 RGB structs + bitmapfileheader
  //for color bitmap + RGB structs for all the colors
  //SvL, 15-3-98: 2*bwsize 
  iconsize = 2*sizeof(BITMAPFILEHEADER) + 2*sizeof(RGB) + 
             rgbsize + 2*bwsize + bmpHdr->biSizeImage;
  //There are icons without an XOR mask, so check for it
  if(bmpHdr->biSizeImage == colorsize) {
	iconsize += bwsize;
  }
  iconhdr  = (BITMAPFILEHEADER *)malloc(iconsize);
  iconhdr->usType    	 = BFT_COLORICON;
  iconhdr->cbSize	 = sizeof(BITMAPFILEHEADER);
  iconhdr->xHotspot	 = 0;
  iconhdr->yHotspot	 = 0;
  iconhdr->offBits 	 = 2*sizeof(BITMAPFILEHEADER) + 
		           2*sizeof(RGB) + rgbsize;
  iconhdr->bmp.cbFix	 = sizeof(BITMAPINFOHEADER);
  iconhdr->bmp.cx	 = (USHORT)bmpHdr->biWidth;
  iconhdr->bmp.cy	 = (USHORT)bmpHdr->biHeight;
  iconhdr->bmp.cPlanes   = 1;
  iconhdr->bmp.cBitCount = 1;
  os2rgb                 = (RGB *)(iconhdr+1);
  memset(os2rgb, 0, sizeof(RGB));
  memset(os2rgb+1, 0xff, sizeof(RGB));
  iconhdr2               = (BITMAPFILEHEADER *)(os2rgb+2);
  iconhdr2->usType    	 = BFT_COLORICON;
  iconhdr2->cbSize	 = sizeof(BITMAPFILEHEADER);
  iconhdr2->xHotspot	 = 0;
  iconhdr2->yHotspot	 = 0;
  iconhdr2->offBits 	 = 2*sizeof(BITMAPFILEHEADER) + 
		           2*sizeof(RGB) + rgbsize + 2*bwsize;
  iconhdr2->bmp.cbFix	 = sizeof(BITMAPINFOHEADER);
  iconhdr2->bmp.cx	 = (USHORT)bmpHdr->biWidth;
  iconhdr2->bmp.cy	 = (USHORT)(bmpHdr->biHeight/2);
  iconhdr2->bmp.cPlanes  = bmpHdr->biPlanes;
  iconhdr2->bmp.cBitCount= bmpHdr->biBitCount; 
  os2rgb                 = (RGB *)(iconhdr2+1);
  rgb 			 = (RGBQUAD *)(bmpHdr+1);
  if(bmpHdr->biBitCount < 24) {
	for(i=0;i<(1<<bmpHdr->biBitCount);i++) {
		os2rgb->bRed   = rgb->red;
		os2rgb->bBlue  = rgb->blue;
	 	os2rgb->bGreen = rgb->green;
		os2rgb++;
		rgb++;
	}
  }
  //write 2*mono pixels + color pixels
  //There are icons without an XOR mask, so check for it
  if(bmpHdr->biSizeImage == colorsize) {
	memset((char *)os2rgb, 0, bwsize);
	memset((char *)os2rgb+bwsize, 0, bwsize);
	memcpy((char *)os2rgb+2*bwsize, (char *)rgb, colorsize);
  }
  else {
	memcpy((char *)os2rgb, (char *)rgb+colorsize, bwsize);
	memcpy((char *)os2rgb+bwsize, (char *)rgb+colorsize, bwsize);
	memcpy((char *)os2rgb+2*bwsize, (char *)rgb, colorsize);
  }
  *OS2ResSize = iconsize;
  return((PBYTE)iconhdr);
}
//******************************************************************************
//******************************************************************************
void FreeIcon(void *os2icon)
{
  free(os2icon);
}
//******************************************************************************
//******************************************************************************
#endif

//******************************************************************************
//******************************************************************************
ULONG QueryConvertedIconSize(WINBITMAPINFOHEADER *bmpHdr, int size)
{
 int bwsize, colorsize, rgbsize, iconsize;

  bwsize    = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), 1);
  colorsize = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), bmpHdr->biBitCount);

  //SvL: 28-09-'98: only for <= 8
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
             rgbsize + 2*bwsize + bmpHdr->biSizeImage;

  return iconsize;
}
//******************************************************************************
//NOTE: offsetBits is the value added to the offBits bitmap structure members
//      (handy for converting icon groups)
//******************************************************************************
void *ConvertIcon(WINBITMAPINFOHEADER *bmpHdr, int size, int *os2size, int offsetBits)
{
 RGBQUAD *rgb;
 RGB2    *os2rgb;
 int bwsize, i, colorsize, rgbsize, iconsize;
 BITMAPFILEHEADER2 *iconhdr;
 BITMAPFILEHEADER2 *iconhdr2;
 char *pAnd, *pXor;

  bwsize    = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), 1);
  colorsize = DIB_GetDIBImageBytes(bmpHdr->biWidth, (bmpHdr->biHeight/2), bmpHdr->biBitCount);
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
             rgbsize + 2*bwsize + bmpHdr->biSizeImage;

  iconhdr  = (BITMAPFILEHEADER2 *)malloc(iconsize);
  memset(iconhdr, 0, iconsize);
  iconhdr->usType        = BFT_COLORICON;
  iconhdr->cbSize        = sizeof(BITMAPFILEHEADER2);
  iconhdr->xHotspot      = 0;
  iconhdr->yHotspot      = 0;
  iconhdr->offBits       = 2*sizeof(BITMAPFILEHEADER2) +
                           2*sizeof(RGB2) + rgbsize + offsetBits;
  iconhdr->bmp2.cbFix    = sizeof(BITMAPINFOHEADER2);
  iconhdr->bmp2.cx       = (USHORT)bmpHdr->biWidth;
  iconhdr->bmp2.cy       = (USHORT)bmpHdr->biHeight;
  iconhdr->bmp2.cPlanes  = 1;
  iconhdr->bmp2.cBitCount= 1;
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
  iconhdr2->bmp2.cx      = (USHORT)bmpHdr->biWidth;
  iconhdr2->bmp2.cy      = (USHORT)(bmpHdr->biHeight/2);
  iconhdr2->bmp2.cPlanes = bmpHdr->biPlanes;
  iconhdr2->bmp2.cBitCount= bmpHdr->biBitCount;
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

  pXor = (char *)os2rgb;
  pAnd = (char *)os2rgb + bwsize;

  if ((size - (bmpHdr->biSize + rgbsize + colorsize + bwsize)) == bwsize) 
  {//this means an AND and XOR mask is present (interleaved; and/xor)
    char *q;
    int i, linesize;

    linesize = bmpHdr->biWidth / 8;
    q = (char *)rgb + colorsize;
    for (i = 0; i < (bmpHdr->biHeight/2); i++) {
      memcpy (pAnd, q, linesize);
      pAnd += linesize;
      q += linesize;

      memcpy (pXor, q, linesize);
      pXor += linesize;
      q += linesize;
    }
  } else {
    memcpy (pAnd, (char *)rgb + colorsize, bwsize);
    memset (pXor, 0, bwsize);
  }
  memcpy((char *)os2rgb+2*bwsize, (char *)rgb, colorsize);

  *os2size = iconsize;
  return (void *)iconhdr;
}
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
        rdir++;
  }
  groupsize = groupsize+ihdr->wCount*sizeof(BITMAPARRAYFILEHEADER2);
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
