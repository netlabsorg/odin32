/*
 * GDI32 DIB sections
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_GPI
#define  INCL_WIN
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "win32type.h"
#include "misc.h"
#define  OS2_ONLY
#include "dibsect.h"

//NOTE:
//This is not a complete solution for CreateDIBSection, but enough for Quake 2!
//******************************************************************************
//******************************************************************************
DIBSection::DIBSection(WINBITMAPINFOHEADER *pbmi, DWORD handle, int fFlip)
                : bmpBits(NULL), pOS2bmp(NULL)
{
 int bmpsize = pbmi->biWidth, os2bmpsize;

  /* @@@PH 98/06/07 -- high-color bitmaps don't have palette */


  this->fFlip = fFlip;
  os2bmpsize = sizeof(BITMAPINFO2);

  switch(pbmi->biBitCount)
  {
        case 1:
                bmpsize /= 8;
                os2bmpsize += ((1 << pbmi->biBitCount)-1)*sizeof(RGB2);
                break;
        case 4:
                bmpsize /= 2;
                os2bmpsize += ((1 << pbmi->biBitCount)-1)*sizeof(RGB2);
                break;
        case 8:
                os2bmpsize += ((1 << pbmi->biBitCount)-1)*sizeof(RGB2);
                break;
        case 16:
                bmpsize *= 2;
                break;
        case 24:
                bmpsize *= 3;
                break;
        case 32:
                bmpsize *= 4;
                break;
   }
   if(bmpsize & 3) {
        bmpsize = (bmpsize + 3) & ~3;
   }
   bmpBits    = (char *)malloc(bmpsize*pbmi->biHeight);

   pOS2bmp    = (BITMAPINFO2 *)malloc(os2bmpsize);

  memset(pOS2bmp, /* set header + palette entries to zero */
         0,
         os2bmpsize);

   pOS2bmp->cbFix         = sizeof(BITMAPINFO2) - sizeof(RGB2);
   pOS2bmp->cx            = pbmi->biWidth;
   pOS2bmp->cy            = pbmi->biHeight;
   pOS2bmp->cPlanes       = pbmi->biPlanes;
   pOS2bmp->cBitCount     = pbmi->biBitCount;
   pOS2bmp->ulCompression = pbmi->biCompression;
   pOS2bmp->cbImage       = pbmi->biSizeImage;
   dprintf(("pOS2bmp->cx            %d\n", pOS2bmp->cx));
   dprintf(("pOS2bmp->cy            %d\n", pOS2bmp->cy));
   dprintf(("pOS2bmp->cPlanes       %d\n", pOS2bmp->cPlanes));
   dprintf(("pOS2bmp->cBitCount     %d\n", pOS2bmp->cBitCount));
   dprintf(("pOS2bmp->ulCompression %d\n", pOS2bmp->ulCompression));
   dprintf(("pOS2bmp->cbImage       %d\n", pOS2bmp->cbImage));

   this->handle = handle;

   if(section == NULL) {
        section = this;
   }
   else
   {
     DIBSection *dsect = section;

     /* @@@PH 98/07/11 fix for dsect->next == NULL */
     while ( (dsect->next != this) &&
             (dsect->next != NULL) )
     {
                dsect = dsect->next;
     }
     dsect->next = this;
   }
}
//******************************************************************************
//******************************************************************************
DIBSection::~DIBSection()
{
   if(bmpBits)
        free(bmpBits);
   if(pOS2bmp)
        free(pOS2bmp);

   if(section == this) {
        section = this->next;
   }
   else {
        DIBSection *dsect = section;

        while(dsect->next != this) {
                dsect = dsect->next;
        }
        dsect->next = this->next;
   }
}
//******************************************************************************
//******************************************************************************
int DIBSection::SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb)
{
 int i;

  if(startIdx + cEntries > (1 << pOS2bmp->cBitCount)) {
        dprintf(("DIBSection::SetDIBColorTable, invalid nr of entries %d %d\n", startIdx, cEntries));
        return(0);
  }
  memcpy(&pOS2bmp->argbColor[startIdx], rgb, cEntries*sizeof(RGB2));
  for(i=startIdx;i<cEntries;i++) {
        pOS2bmp->argbColor[i].fcOptions = 0;
  }
  return(cEntries);
}
//******************************************************************************
//******************************************************************************
#if 1
BOOL DIBSection::BitBlt(HDC hdcDest, HWND hwndDest, int nXdest, int nYdest, int nWidth,
                        int nHeight, int nXsrc, int nYsrc, DWORD Rop)
{
 HPS    hps = (HPS)hdcDest;
 POINTL point[4];
 LONG   rc;

  if(hwndDest != 0) {
         hps = WinGetPS(hwndDest);
  }
  if(hps == 0) {
        eprintf(("DIBSection::BitBlt, hps == 0 hwndDest = %X", hwndDest));
        return(FALSE);
  }

  point[0].x = nXdest;
  point[0].y = nYdest;
  point[1].x = nXdest + nWidth - 1;
  point[1].y = nYdest + nHeight - 1;
  point[2].x = nXsrc;
  point[2].y = nYsrc;
  if(nXsrc + nWidth > pOS2bmp->cx) {
        point[3].x = pOS2bmp->cx;
  }
  else  point[3].x = nXsrc + nWidth;

  if(nYsrc + nHeight > pOS2bmp->cy) {
        point[3].y = pOS2bmp->cy;
  }
  else  point[3].y = nYsrc + nHeight;

  if(fFlip & FLIP_VERT) {
    ULONG y;
        y = point[0].y;
        point[0].y = point[1].y;
        point[1].y = y;
  }

  if(fFlip & FLIP_HOR) {
    ULONG x;
        x = point[0].x;
        point[0].x = point[1].x;
        point[1].x = x;
  }

  rc = GpiDrawBits(hps, bmpBits, pOS2bmp, 4, &point[0], ROP_SRCCOPY, BBO_OR);

  if(hwndDest != 0) {
          WinReleasePS(hps);
  }
  if(rc == GPI_OK)
        return(TRUE);
  dprintf(("DIBSection::BitBlt %X (%d,%d) (%d,%d) to (%d,%d) (%d,%d) returned %d\n", hps, point[0].x, point[0].y, point[1].x, point[1].y, point[2].x, point[2].y, point[3].x, point[3].y, rc));
  dprintf(("WinGetLastError returned %X\n", WinGetLastError(WinQueryAnchorBlock(hwndDest)) & 0xFFFF));
  return(FALSE);
}
#else
BOOL DIBSection::BitBlt(HDC hdcDest, int nXdest, int nYdest, int nWidth,
                        int nHeight, int nXsrc, int nYsrc, DWORD Rop)
{
 HPS    hps = (HPS)hdcDest;
 POINTL point[4];
 LONG   rc;

  if(hps == 0) {
        eprintf(("DIBSection::BitBlt, hps == 0"));
        return(FALSE);
  }

//  dprintf(("DIBSection::BitBlt (%d,%d) to (%d,%d) (%d,%d)\n", nXsrc, nYsrc, nXdest, nYdest, nXdest+ nWidth, nYdest + nHeight));
  point[0].x = nXdest;
  point[0].y = nYdest;
  point[1].x = nXdest + nWidth - 1;
  point[1].y = nYdest + nHeight - 1;
  point[2].x = nXsrc;
  point[2].y = nYsrc;
  if(nXsrc + nWidth > pOS2bmp->cx) {
        point[3].x = pOS2bmp->cx;
  }
  else  point[3].x = nXsrc + nWidth;

  if(nYsrc + nHeight > pOS2bmp->cy) {
        point[3].y = pOS2bmp->cy;
  }
  else  point[3].y = nYsrc + nHeight;

  rc = GpiDrawBits(hps, bmpBits, pOS2bmp, 4, &point[0], ROP_SRCCOPY, BBO_OR);
//  dprintf(("DIBSection::BitBlt %X (%d,%d) (%d,%d) to (%d,%d) (%d,%d) returned %d\n", hps, point[0].x, point[0].y, point[1].x, point[1].y, point[2].x, point[2].y, point[3].x, point[3].y, rc));

  if(rc == GPI_OK)
        return(TRUE);
  dprintf(("DIBSection::BitBlt %X (%d,%d) (%d,%d) to (%d,%d) (%d,%d) returned %d\n", hps, point[0].x, point[0].y, point[1].x, point[1].y, point[2].x, point[2].y, point[3].x, point[3].y, rc));
  dprintf(("WinGetLastError returned %X\n", WinGetLastError(WinQueryAnchorBlock(hwndParent)) & 0xFFFF));
  return(FALSE);
}
#endif
//******************************************************************************
//******************************************************************************
void DIBSection::SelectDIBObject(HDC hdc)
{
  this->hdc  = hdc;
  hwndParent = WinWindowFromDC(hdc);
}
//******************************************************************************
//******************************************************************************
DIBSection *DIBSection::find(DWORD handle)
{
 DIBSection *dsect = section;

  while(dsect) {
        if(dsect->handle == handle) {
                return(dsect);
        }
        dsect = dsect->next;
  }
  return(NULL);
}
//******************************************************************************
//A bitmap can only be selected into one DC, so this works.
//******************************************************************************
DIBSection *DIBSection::findHDC(HDC hdc)
{
 DIBSection *dsect = section;

  while(dsect) {
        if(dsect->hdc == hdc) {
                return(dsect);
        }
        dsect = dsect->next;
  }
  return(NULL);
}
//******************************************************************************
//******************************************************************************
void DIBSection::deleteSection(DWORD handle)
{
 DIBSection *dsect = find(handle);

  if(dsect)
        delete dsect;

}
//******************************************************************************
//******************************************************************************
DIBSection *DIBSection::section = NULL;