/* $Id: icon.cpp,v 1.5 2001-05-11 08:38:36 sandervl Exp $ */

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

#include <win32type.h>

#include <winicon.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_icon
#include "dbglocal.h"

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
