/* $Id: cursor.cpp,v 1.3 1999-06-10 17:08:52 phaller Exp $ */

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
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "pefile.h"
#include "lx.h"
#include "icon.h"
#include "cursor.h"
#include "misc.h"


//******************************************************************************
//******************************************************************************
OS2Cursor::OS2Cursor(int             id,
                     CursorComponent *curHdr,
                     int             size)
          : id(0),
            next(NULL),
            cursorhdr(NULL),
            cursorsize(0),
            prevoffset(0)
{
 OS2Cursor *cursor = OS2Cursor::cursors;
 RGBQUAD   *rgb;
 RGB       *os2rgb;
 WINBITMAPINFOHEADER *bhdr = (WINBITMAPINFOHEADER *)(curHdr+1);
 int        i, bwsize, bmpsize;

  if(cursor != NULL)
  {
    while(cursor->next != NULL)
    {
      cursor = cursor->next;
    }
    cursor->next = this;
  }
  else
    cursors = this;

  this->id = id;

  cout << "Cursor with id " << id << endl;
  cout << "Cursor size    " << size << endl;
  bmpsize = size - sizeof(CursorComponent) - (1<<bhdr->biBitCount)*sizeof(RGBQUAD);
  cursorsize = sizeof(BITMAPFILEHEADER) + bmpsize + (1<<bhdr->biBitCount)*sizeof(RGB);

  cursorhdr  = (BITMAPFILEHEADER *)malloc(cursorsize);
  cursorhdr->usType        = BFT_POINTER;
  cursorhdr->cbSize        = sizeof(BITMAPFILEHEADER);
  cursorhdr->xHotspot      = curHdr->xHotspot;

  /* @@@PH y-hotspot is upside down ! */
  cursorhdr->yHotspot      = (bhdr->biHeight >> 1) - 1       /* height div 2 */
                               - curHdr->yHotspot;         /* subtract hot.y */

  cout << "Cursor Hot.x   : " << curHdr->xHotspot << endl;
  cout << "Cursor Hot.y   : " << curHdr->yHotspot << endl;

  cursorhdr->offBits       = sizeof(BITMAPFILEHEADER) + 2*sizeof(RGB);
  cursorhdr->bmp.cbFix     = sizeof(BITMAPINFOHEADER);
  cursorhdr->bmp.cx        = (USHORT)bhdr->biWidth;
  cursorhdr->bmp.cy        = (USHORT)(bhdr->biHeight);
  cursorhdr->bmp.cPlanes   = bhdr->biPlanes;
  cursorhdr->bmp.cBitCount = bhdr->biBitCount;
  cout << "Cursor size    : " << bhdr->biSizeImage << endl;
  cout << "Cursor Width   : " << bhdr->biWidth << endl;
//height for both the XOR and AND bitmap (color & BW)
  cout << "Height         : " << bhdr->biHeight << endl;
  cout << "Cursor Bitcount: " << bhdr->biBitCount << endl;
  cout << "Cursor Compress: " << bhdr->biCompression << endl;

  os2rgb                   = (RGB *)(cursorhdr+1);
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
}
//******************************************************************************
//******************************************************************************
void OS2Cursor::SetCursorHdrOffset(int offset)
{
  cursorhdr->offBits  += offset - prevoffset;
  //remember in case cursors are used in multiple groups
  //(can't imagine this ever happening, but you never know)
  prevoffset         = offset;
}
//******************************************************************************
//******************************************************************************
OS2Cursor::~OS2Cursor()
{
  if(cursorhdr) free(cursorhdr);
}
//******************************************************************************
//******************************************************************************
int OS2Cursor::QueryCursorSize()
{
  return(cursorsize);
}
//******************************************************************************
//******************************************************************************
#ifdef __WATCOMC__
#pragma off (unreferenced)
#endif
BITMAPFILEHEADER *OS2Cursor::GetCursorHeader(int wWidth, int wHeight, int wPlanes, int wBitCount)
#ifdef __WATCOMC__
#pragma on (unreferenced)
#endif
{
//SvL: 20-11-'97: already filled in
#if 0
  cursorhdr->bmp.cx        = wWidth;
  cursorhdr->bmp.cy        = wHeight;
  cursorhdr->bmp.cPlanes   = wPlanes;
  cursorhdr->bmp.cBitCount = wBitCount;
#endif
  return(cursorhdr);
}
//******************************************************************************
//******************************************************************************
OS2Cursor *OS2Cursor::GetCursor(int id)
{
 OS2Cursor *cursor = OS2Cursor::cursors;

   while(cursor != NULL) {
        if(cursor->id == id) {
                return(cursor);
        }
        cursor = cursor->next;
   }
   return(NULL);
}
//******************************************************************************
//******************************************************************************
void OS2Cursor::DestroyAll()
{
 OS2Cursor *cursor = OS2Cursor::cursors, *next;

   while(cursor != NULL) {
        next = cursor->next;
        delete(cursor);
        cursor = next;
   }
}
//******************************************************************************
//******************************************************************************
OS2Cursor *OS2Cursor::cursors = NULL;
