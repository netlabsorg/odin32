/* $Id: cvtcursor.cpp,v 1.1 1999-09-15 23:32:52 sandervl Exp $ */

/*
 * PE2LX cursor conversion code
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
#include <wincursor.h>
#include <misc.h>

//******************************************************************************
//******************************************************************************
ULONG QueryConvertedCursorSize(CursorComponent *curHdr, int size)
{
 WINBITMAPINFOHEADER *bhdr = (WINBITMAPINFOHEADER *)(curHdr+1);
 int                  bmpsize, cursorsize;

  bmpsize = size - sizeof(CursorComponent) - (1<<bhdr->biBitCount)*sizeof(RGBQUAD);
  cursorsize = sizeof(BITMAPFILEHEADER2) + bmpsize + (1<<bhdr->biBitCount)*sizeof(RGB2);

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
 WINBITMAPINFOHEADER *bhdr = (WINBITMAPINFOHEADER *)(curHdr+1);
 BITMAPFILEHEADER2   *cursorhdr;
 int        i, bwsize, bmpsize, cursorsize;

  dprintf(("ConvertCursor: Cursor size %d", size));
  bmpsize = size - sizeof(CursorComponent) - (1<<bhdr->biBitCount)*sizeof(RGBQUAD);
  cursorsize = sizeof(BITMAPFILEHEADER2) + bmpsize + (1<<bhdr->biBitCount)*sizeof(RGB2);

  cursorhdr  = (BITMAPFILEHEADER2 *)malloc(cursorsize);
  cursorhdr->usType        = BFT_POINTER;
  cursorhdr->cbSize        = sizeof(BITMAPFILEHEADER2);
  cursorhdr->xHotspot      = curHdr->xHotspot;

  /* @@@PH y-hotspot is upside down ! */
  cursorhdr->yHotspot      = (bhdr->biHeight >> 1) - 1       /* height div 2 */
                             - curHdr->yHotspot;         /* subtract hot.y */

  dprintf(("Cursor Hot.x   : %d", curHdr->xHotspot));
  dprintf(("Cursor Hot.y   : %d", curHdr->yHotspot));

  cursorhdr->offBits       = sizeof(BITMAPFILEHEADER2) + 2*sizeof(RGB2) + offsetBits;
  cursorhdr->bmp2.cbFix     = sizeof(BITMAPINFOHEADER);
  cursorhdr->bmp2.cx        = (USHORT)bhdr->biWidth;
  cursorhdr->bmp2.cy        = (USHORT)(bhdr->biHeight);
  cursorhdr->bmp2.cPlanes   = bhdr->biPlanes;
  cursorhdr->bmp2.cBitCount = bhdr->biBitCount;
  dprintf(("Cursor size    : %d", bhdr->biSizeImage));
  dprintf(("Cursor Width   : %d", bhdr->biWidth));
  //height for both the XOR and AND bitmap (color & BW)
  dprintf(("Height         : %d", bhdr->biHeight));
  dprintf(("Cursor Bitcount: %d", bhdr->biBitCount));
  dprintf(("Cursor Compress: %d", bhdr->biCompression));

  os2rgb                   = (RGB2 *)(cursorhdr+1);
  rgb                      = (RGBQUAD *)(bhdr+1);
  for(i=0;i<(1<<bhdr->biBitCount);i++) {
        os2rgb->bRed   = rgb->red;
        os2rgb->bBlue  = rgb->blue;
        os2rgb->bGreen = rgb->green;
        os2rgb++;
        rgb++;
  }

  if(bhdr->biSizeImage > bmpsize || bhdr->biSizeImage == 0) {
        bwsize = bhdr->biWidth*(bhdr->biHeight);

        switch(bhdr->biBitCount) {
                case 1:
                        bwsize /= 8;
                        break;
                case 4:
                        bwsize /= 2;
                        break;
                case 8:
                        break;
                case 16:
                        bwsize *= 2;
                        break;
                case 24:
                        bwsize *= 3;
                        break;
                case 32:
                        bwsize *= 4;
                        break;
          }
  }
  else    bwsize = bhdr->biSizeImage;

  //write XOR and AND mask
  memcpy((char *)os2rgb, (char *)rgb, bwsize);

  *os2size = cursorsize;
  return cursorhdr;
}
//******************************************************************************
//******************************************************************************
