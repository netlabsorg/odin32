/* $Id: cvticon.cpp,v 1.7 1999-12-29 12:39:09 sandervl Exp $ */

/*
 * PE2LX icons
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <misc.h>

//******************************************************************************
//******************************************************************************
ULONG QueryConvertedIconSize(WINBITMAPINFOHEADER *bmpHdr, int size)
{
 int bwsize, colorsize, rgbsize, iconsize;

  bwsize   = (bmpHdr->biWidth*(bmpHdr->biHeight/2))/8;
  colorsize = bmpHdr->biWidth*(bmpHdr->biHeight/2);
  //SvL: 28-09-'98: only for <= 8
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

  bwsize   = (bmpHdr->biWidth*(bmpHdr->biHeight/2))/8;
  colorsize = bmpHdr->biWidth*(bmpHdr->biHeight/2);
  //SvL: 28-09-'98: only for <= 8
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
        bmpHdr->biSizeImage = bwsize + colorsize;
  }
  dprintf2(("Icon size    : %d", bmpHdr->biSizeImage));
  dprintf2(("Icon Width   : %d", bmpHdr->biWidth));
  //height for both the XOR and AND bitmap (color & BW)
  dprintf2(("Height       : %d", bmpHdr->biHeight));
  dprintf2(("Icon Bitcount: %d", bmpHdr->biBitCount));
  dprintf2(("Icon Compress: %d", bmpHdr->biCompression));

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

  if ((size - (bmpHdr->biSize - rgbsize - colorsize)) == bwsize) {
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
