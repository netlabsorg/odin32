/* $Id: cvtcursor.cpp,v 1.11 2001-03-27 16:18:26 sandervl Exp $ */

/*
 * PE2LX cursor conversion code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
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
#include <os2wrap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <winicon.h>
#include <wincursor.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_cvtcursor
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
ULONG QueryConvertedCursorSize(CursorComponent *curHdr, int size)
{
 WINBITMAPINFOHEADER *bmpHdr = (WINBITMAPINFOHEADER *)(curHdr+1);
 int bwsize, colorsize, rgbsize, cursorsize;
 ULONG biSizeImage;

  bwsize   = (bmpHdr->biWidth*(bmpHdr->biHeight/2))/8;
  colorsize = bmpHdr->biWidth*(bmpHdr->biHeight/2);

  if(bmpHdr->biBitCount <= 8)
        rgbsize = (1<<bmpHdr->biBitCount)*sizeof(RGB2);
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
        biSizeImage = bwsize + colorsize;
  }
  else  biSizeImage = bmpHdr->biSizeImage;

  //SvL: 28-09-'98: cllngenu.dll has an incorrect size in the header
  if(biSizeImage < colorsize) {
        biSizeImage = colorsize;
  }

  if(bmpHdr->biBitCount > 1) {
	//And mask, xor mask (0) + color image
  	cursorsize = 2*sizeof(BITMAPFILEHEADER2) + 2*sizeof(RGB2) +
             	     rgbsize + 2*bwsize + biSizeImage;
  }
  else {
	//And + xor mask
	cursorsize = sizeof(BITMAPFILEHEADER2) + 2*sizeof(RGB2) + 2*bwsize;
  }

  return cursorsize;
}
//******************************************************************************
//NOTE: offsetBits is the value added to the offBits bitmap structure members
//      (handy for converting cursor groups)
//******************************************************************************
void *ConvertCursor(CursorComponent *curHdr, int size, int *os2size, int offsetBits)
{
 RGBQUAD   *rgb;
 RGB2      *os2rgb;
 WINBITMAPINFOHEADER *bmpHdr = (WINBITMAPINFOHEADER *)(curHdr+1);
 BITMAPFILEHEADER2   *cursorhdr, *cursorhdr2;
 int        i, bwsize, bmpsize, cursorsize, rgbsize, colorsize;
 ULONG      biSizeImage;

  dprintf(("ConvertCursor: Cursor size %d", size));
  bwsize   = (bmpHdr->biWidth*(bmpHdr->biHeight/2))/8;
  colorsize = bmpHdr->biWidth*(bmpHdr->biHeight/2);

  if(bmpHdr->biBitCount <= 8)
        rgbsize = (1<<bmpHdr->biBitCount)*sizeof(RGB2);
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
        biSizeImage = bwsize + colorsize;
  }
  else  biSizeImage = bmpHdr->biSizeImage;

  //SvL: 28-09-'98: cllngenu.dll has an incorrect size in the header
  if(biSizeImage < colorsize) {
        biSizeImage = colorsize;
  }

  if(bmpHdr->biBitCount == 1) {
	//And + xor mask 
	cursorsize = sizeof(BITMAPFILEHEADER2) + 2*sizeof(RGB2) + 2*bwsize;
  }
  else {
	//And mask, xor mask (0) + color image
  	cursorsize = 2*sizeof(BITMAPFILEHEADER2) + 2*sizeof(RGB2) +
             	     rgbsize + 2*bwsize + biSizeImage;
  }

  cursorhdr  = (BITMAPFILEHEADER2 *)malloc(cursorsize);
  memset(cursorhdr, 0, cursorsize);
  cursorhdr->usType        = BFT_POINTER;
  cursorhdr->cbSize        = sizeof(BITMAPFILEHEADER2);
  cursorhdr->xHotspot      = curHdr->xHotspot;

  /* @@@PH y-hotspot is upside down ! */
  cursorhdr->yHotspot      = (bmpHdr->biHeight >> 1)       /* height div 2 */
                             - curHdr->yHotspot;         /* subtract hot.y */

  dprintf2(("Cursor Hot.x   : %d", curHdr->xHotspot));
  dprintf2(("Cursor Hot.y   : %d", curHdr->yHotspot));

  if(bmpHdr->biBitCount == 1) {
  	cursorhdr->offBits = sizeof(BITMAPFILEHEADER2) +
        	             2*sizeof(RGB2) + offsetBits;
  }
  else {
  	cursorhdr->offBits = 2*sizeof(BITMAPFILEHEADER2) +
        	             2*sizeof(RGB2) + rgbsize + offsetBits;
  }

  cursorhdr->bmp2.cbFix     = sizeof(BITMAPINFOHEADER2);
  cursorhdr->bmp2.cx        = (USHORT)bmpHdr->biWidth;
  cursorhdr->bmp2.cy        = (USHORT)(bmpHdr->biHeight);
  cursorhdr->bmp2.cPlanes   = bmpHdr->biPlanes;
  cursorhdr->bmp2.cBitCount = 1;
  cursorhdr->bmp2.ulCompression   = BCA_UNCOMP;
  cursorhdr->bmp2.ulColorEncoding = BCE_RGB;
  dprintf2(("Cursor size    : %d", biSizeImage));
  dprintf2(("Cursor Width   : %d", bmpHdr->biWidth));
  //height for both the XOR and AND bitmap (color & BW)
  dprintf2(("Height         : %d", bmpHdr->biHeight));
  dprintf2(("Cursor Bitcount: %d", bmpHdr->biBitCount));
  dprintf2(("Cursor Compress: %d", bmpHdr->biCompression));

  os2rgb                   = (RGB2 *)(cursorhdr+1);
  rgb                      = (RGBQUAD *)(bmpHdr+1);
  if(bmpHdr->biBitCount == 1) {
  	for(i=0;i<2;i++) {
        	os2rgb->bRed   = rgb->red;
	        os2rgb->bBlue  = rgb->blue;
	        os2rgb->bGreen = rgb->green;
	        os2rgb++;
	        rgb++;
	}
  	//write XOR and AND mask
  	memcpy((char *)os2rgb, (char *)rgb, bwsize*2);
  }
  else {
  	memset(os2rgb, 0, sizeof(RGB2));
	memset(os2rgb+1, 0xff, sizeof(RGB)); //not reserved byte!
  	cursorhdr2               = (BITMAPFILEHEADER2 *)(os2rgb+2);
  	cursorhdr2->usType       = BFT_COLORICON;
  	cursorhdr2->cbSize       = sizeof(BITMAPFILEHEADER2);
  	cursorhdr2->xHotspot     = curHdr->xHotspot;
  	cursorhdr2->yHotspot     = (bmpHdr->biHeight >> 1)       /* height div 2 */
                                   - curHdr->yHotspot;         /* subtract hot.y */
  	cursorhdr2->offBits      = 2*sizeof(BITMAPFILEHEADER2) +
                           	   2*sizeof(RGB2) + rgbsize + 2*bwsize + offsetBits;
  	cursorhdr2->bmp2.cbFix   = sizeof(BITMAPINFOHEADER2);
  	cursorhdr2->bmp2.cx      = (USHORT)bmpHdr->biWidth;
  	cursorhdr2->bmp2.cy      = (USHORT)(bmpHdr->biHeight/2);
  	cursorhdr2->bmp2.cPlanes = bmpHdr->biPlanes;
  	cursorhdr2->bmp2.cBitCount= bmpHdr->biBitCount;
  	cursorhdr2->bmp2.ulCompression   = BCA_UNCOMP;
  	cursorhdr2->bmp2.ulColorEncoding = BCE_RGB;
  	os2rgb                 = (RGB2 *)(cursorhdr2+1);
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
  	//write XOR and AND mask
  	char *pXor = (char *)os2rgb;
  	char *pAnd = (char *)os2rgb + bwsize;

  	memcpy (pAnd, (char *)rgb + colorsize, bwsize);
  	memset (pXor, 0, bwsize);
  	memcpy((char *)os2rgb+2*bwsize, (char *)rgb, colorsize);
  }

  *os2size = cursorsize;
  return cursorhdr;
}
//******************************************************************************
//******************************************************************************
