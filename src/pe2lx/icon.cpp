/* $Id: icon.cpp,v 1.1 1999-05-24 20:19:54 ktk Exp $ */

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
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "pefile.h"
#include "lx.h"
#include "icon.h"
#include "misc.h"


//******************************************************************************
//******************************************************************************
OS2Icon::OS2Icon(int id, WINBITMAPINFOHEADER *bmpHdr, int size) :
                id(0), next(NULL), iconhdr(NULL), iconsize(0), prevoffset(0)
{
 RGBQUAD *rgb;
 RGB     *os2rgb;
 int bwsize, i, colorsize, rgbsize;
 OS2Icon *icon = OS2Icon::icons;

  if(icon != NULL) {
        while(icon->next != NULL) {
                icon = icon->next;
        }
        icon->next = this;
  }
  else  icons      = this;

  this->id = id;
  bwsize   = (bmpHdr->biWidth*(bmpHdr->biHeight/2))/8;
  colorsize = bmpHdr->biWidth*(bmpHdr->biHeight/2);
  //SvL: 28-09-'98: only for <= 8
  if(bmpHdr->biBitCount <= 8)
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
  cout << "Icon size    : " << bmpHdr->biSizeImage << endl;
  cout << "Icon Width   : " << bmpHdr->biWidth << endl;
//height for both the XOR and AND bitmap (color & BW)
  cout << "Height       : " << bmpHdr->biHeight << endl;
  cout << "Icon Bitcount: " << bmpHdr->biBitCount << endl;
  cout << "Icon Compress: " << bmpHdr->biCompression << endl;

  //SvL: 28-09-'98: cllngenu.dll has an incorrect size in the header
  if(bmpHdr->biSizeImage < colorsize) {
        bmpHdr->biSizeImage = colorsize;
  }
  //bitmapfileheader for AndXor mask + 2 RGB structs + bitmapfileheader
  //for color bitmap + RGB structs for all the colors
  //SvL, 3-3-98: 2*bwsize
  iconsize = 2*sizeof(BITMAPFILEHEADER) + 2*sizeof(RGB) +
             rgbsize + 2*bwsize + bmpHdr->biSizeImage;
  //There are icons without an XOR mask, so check for it
  if(bmpHdr->biSizeImage == colorsize) {
        iconsize += bwsize;
  }
  iconhdr  = (BITMAPFILEHEADER *)malloc(iconsize);
  iconhdr->usType        = BFT_COLORICON;
  iconhdr->cbSize        = sizeof(BITMAPFILEHEADER);
  iconhdr->xHotspot      = 0;
  iconhdr->yHotspot      = 0;
  iconhdr->offBits       = 2*sizeof(BITMAPFILEHEADER) +
                           2*sizeof(RGB) + rgbsize;
  iconhdr->bmp.cbFix     = sizeof(BITMAPINFOHEADER);
  iconhdr->bmp.cx        = (USHORT)bmpHdr->biWidth;
  iconhdr->bmp.cy        = (USHORT)bmpHdr->biHeight;
  iconhdr->bmp.cPlanes   = 1;
  iconhdr->bmp.cBitCount = 1;
  os2rgb                 = (RGB *)(iconhdr+1);
  memset(os2rgb, 0, sizeof(RGB));
  memset(os2rgb+1, 0xff, sizeof(RGB));
  iconhdr2               = (BITMAPFILEHEADER *)(os2rgb+2);
  iconhdr2->usType       = BFT_COLORICON;
  iconhdr2->cbSize       = sizeof(BITMAPFILEHEADER);
  iconhdr2->xHotspot     = 0;
  iconhdr2->yHotspot     = 0;
  iconhdr2->offBits      = 2*sizeof(BITMAPFILEHEADER) +
                           2*sizeof(RGB) + rgbsize + 2*bwsize;
  iconhdr2->bmp.cbFix    = sizeof(BITMAPINFOHEADER);
  iconhdr2->bmp.cx       = (USHORT)bmpHdr->biWidth;
  iconhdr2->bmp.cy       = (USHORT)(bmpHdr->biHeight/2);
  iconhdr2->bmp.cPlanes  = bmpHdr->biPlanes;
  iconhdr2->bmp.cBitCount= bmpHdr->biBitCount;
  os2rgb                 = (RGB *)(iconhdr2+1);
  rgb                    = (RGBQUAD *)(bmpHdr+1);
  if(bmpHdr->biBitCount < 24) {
        for(i=0;i<(1<<bmpHdr->biBitCount);i++) {
////            cout << "(R,G,B) = (" << (int)rgb->red << ", " << (int)rgb->green << ", " << (int)rgb->blue << ")" << endl;
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
}
//******************************************************************************
//******************************************************************************
void OS2Icon::SetIconHdrOffset(int offset)
{
  iconhdr->offBits  += offset - prevoffset;
  iconhdr2->offBits += offset - prevoffset;
  //remember in case icons are used in multiple groups
  //(can't imagine this ever happening, but you never know)
  prevoffset         = offset;
}
//******************************************************************************
//******************************************************************************
OS2Icon::~OS2Icon()
{
  if(iconhdr)   free(iconhdr);
}
//******************************************************************************
//******************************************************************************
int OS2Icon::QueryIconSize()
{
  return(iconsize);
}
//******************************************************************************
//******************************************************************************
BITMAPFILEHEADER *OS2Icon::GetIconHeader()
{
  return(iconhdr);
}
//******************************************************************************
//******************************************************************************
/*static*/ OS2Icon *OS2Icon::GetIcon(int id)
{
 OS2Icon *icon = OS2Icon::icons;

   while(icon != NULL) {
        if(icon->id == id)      return(icon);
        icon = icon->next;
   }
   return(NULL);
}
//******************************************************************************
//******************************************************************************
/*static*/ void OS2Icon::DestroyAll()
{
 OS2Icon *icon = OS2Icon::icons, *next;

   while(icon != NULL) {
        next = icon->next;
        delete(icon);
        icon = next;
   }
}
//******************************************************************************
//******************************************************************************
OS2Icon *OS2Icon::icons = NULL;
