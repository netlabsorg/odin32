/* $Id: dibsect.cpp,v 1.13 2000-01-31 22:30:07 sandervl Exp $ */

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
#include <os2wrap.h>  //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include "win32type.h"
#include "misc.h"
#define  OS2_ONLY
#include "dibsect.h"
#include <vmutex.h>

HWND WIN32API WindowFromDC(HDC hdc);
HWND Win32ToOS2Handle(HWND hwnd);

BOOL    APIENTRY _GpiEnableYInversion (HPS hps, LONG lHeight);

inline BOOL APIENTRY GpiEnableYInversion (HPS hps, LONG lHeight)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiEnableYInversion(hps, lHeight);
    SetFS(sel);

    return yyrc;
}

static VMutex dibMutex;

//NOTE:
//This is not a complete solution for CreateDIBSection, but enough for Quake 2!
//******************************************************************************
//******************************************************************************
DIBSection::DIBSection(WINBITMAPINFOHEADER *pbmi, DWORD handle, int fFlip)
                : bmpBits(NULL), pOS2bmp(NULL), next(NULL)
{
  int  os2bmpsize;

  bmpsize = pbmi->biWidth;
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
   memset(bmpBits, 0, bmpsize*pbmi->biHeight);

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
   dprintf(("handle                 %x", handle));
   dprintf(("pOS2bmp->cx            %d\n", pOS2bmp->cx));
   dprintf(("pOS2bmp->cy            %d\n", pOS2bmp->cy));
   dprintf(("pOS2bmp->cPlanes       %d\n", pOS2bmp->cPlanes));
   dprintf(("pOS2bmp->cBitCount     %d\n", pOS2bmp->cBitCount));
   dprintf(("pOS2bmp->ulCompression %d\n", pOS2bmp->ulCompression));
   dprintf(("pOS2bmp->cbImage       %d\n", pOS2bmp->cbImage));
   dprintf(("Bits at 0x%08X\n",bmpBits));

   this->handle = handle;

   dibMutex.enter();
   if(section == NULL)
   {
     dprintf(("section was NULL\n"));
     section = this;
   }
   else
   {
     DIBSection *dsect = section;
     dprintf2(("Increment section starting at %08X\n",dsect));

     /* @@@PH 98/07/11 fix for dsect->next == NULL */
     while ( (dsect->next != this) &&
             (dsect->next != NULL) )
     {
       dprintf2(("Increment section to %08X\n",dsect->next));
       dsect = dsect->next;
     }
     dsect->next = this;
   }
   dibMutex.leave();
}
//******************************************************************************
//******************************************************************************
DIBSection::~DIBSection()
{
   dprintf(("Delete DIBSection %x", handle));
   if(bmpBits)
        free(bmpBits);
   if(pOS2bmp)
        free(pOS2bmp);

   dibMutex.enter();
   if(section == this)
   {
     section = this->next;
   }
   else
   {
     DIBSection *dsect = section;

     while(dsect->next != this)
     {
       dsect = dsect->next;
     }
     dsect->next = this->next;
   }
   dibMutex.leave();
}
//******************************************************************************
//******************************************************************************
int DIBSection::SetDIBits(HDC hdc, HBITMAP hbitmap, UINT startscan, UINT
                          lines, const VOID *bits, WINBITMAPINFOHEADER *pbmi,
                          UINT coloruse)
{
  lines = (int)lines >= 0 ? (int)lines : (int)-lines;
  int  os2bmpsize;
  int  palsize=0;

  bmpsize = pbmi->biWidth;
  os2bmpsize = sizeof(BITMAPINFO2);

  switch(pbmi->biBitCount)
  {
    case 1:
      bmpsize /= 8;
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmpsize += palsize;
      break;
    case 4:
      bmpsize /= 2;
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmpsize += palsize;
      break;
    case 8:
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmpsize += palsize;
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

   if(bmpsize & 3)
   {
     bmpsize = (bmpsize + 3) & ~3;
   }

   bmpBits    = (char *)realloc(bmpBits, bmpsize*pbmi->biHeight);
   pOS2bmp    = (BITMAPINFO2 *)realloc(pOS2bmp, os2bmpsize);

   pOS2bmp->cbFix         = sizeof(BITMAPINFO2) - sizeof(RGB2);
   pOS2bmp->cx            = pbmi->biWidth;
   pOS2bmp->cy            = pbmi->biHeight;
   pOS2bmp->cPlanes       = pbmi->biPlanes;
   pOS2bmp->cBitCount     = pbmi->biBitCount;
   pOS2bmp->ulCompression = pbmi->biCompression;
   pOS2bmp->cbImage       = pbmi->biSizeImage;

   if(palsize)
     memcpy(pOS2bmp->argbColor, (char *)pbmi + 1 , palsize);

   if(bits)
   {
     int size = bmpsize*lines;
     memcpy(bmpBits+bmpsize*startscan, bits, size);
   }
   return(lines);
}
//******************************************************************************
//******************************************************************************
int DIBSection::SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb)
{
 int i;

  if(startIdx + cEntries > (1 << pOS2bmp->cBitCount))
  {
    dprintf(("DIBSection::SetDIBColorTable, invalid nr of entries %d %d\n", startIdx, cEntries));
    return(0);
  }

  memcpy(&pOS2bmp->argbColor[startIdx], rgb, cEntries*sizeof(RGB2));

  for(i=startIdx;i<cEntries;i++)
  {
    pOS2bmp->argbColor[i].fcOptions = 0;
    dprintf2(("Index %d : 0x%08X\n",i, *((ULONG*)(&pOS2bmp->argbColor[i])) ));
  }

  return(cEntries);
}
//******************************************************************************
//******************************************************************************
BOOL DIBSection::BitBlt(HDC hdcDest, int nXdest, int nYdest, int nDestWidth,
                        int nDestHeight, int nXsrc, int nYsrc,
                        int nSrcWidth, int nSrcHeight, DWORD Rop)
{
 HPS    hps = (HPS)hdcDest;
 POINTL point[4];
 LONG   rc;

  HWND hwndDest = WindowFromDC(hdcDest);
  hwndDest = Win32ToOS2Handle(hwndDest);
  if(hwndDest != 0)
  {
    	hps = WinGetPS(hwndDest);
  }
  if(hps == 0)
  {
    dprintf(("ERROR: DIBSection::BitBlt, hps == 0 hwndDest = %X", hwndDest));
    return(FALSE);
  }

  dprintf(("DIBSection::BitBlt %x %X (hps %x) %x to(%d,%d)(%d,%d) from (%d,%d)(%d,%d) rop %x flip %x",
          handle, hdcDest, hps, hwndDest, nXdest, nYdest, nDestWidth, nDestHeight,
          nXsrc, nYsrc, nSrcWidth, nSrcHeight, Rop, fFlip));

  point[0].x = nXdest;
  point[0].y = nYdest;
  point[1].x = nXdest + nDestWidth - 1;
  point[1].y = nYdest + nDestHeight - 1;
  point[2].x = nXsrc;
  point[2].y = nYsrc;
  if(nXsrc + nSrcWidth > pOS2bmp->cx)
  {
    point[3].x = pOS2bmp->cx;
  }
  else
    point[3].x = nXsrc + nSrcWidth;

  if(nYsrc + nSrcHeight > pOS2bmp->cy)
  {
    point[3].y = pOS2bmp->cy;
  }
  else
    point[3].y = nYsrc + nSrcHeight;

#if 1
  if(fFlip & FLIP_VERT)
  {
    GpiEnableYInversion(hps, nDestHeight);
  }

  if(fFlip & FLIP_HOR)
  {
    ULONG x;
    x = point[0].x;
    point[0].x = point[1].x;
    point[1].x = x;
  }
#endif

  rc = GpiDrawBits(hps, bmpBits, pOS2bmp, 4, &point[0], ROP_SRCCOPY, BBO_OR);

  if(hwndDest != 0)
  {
    WinReleasePS(hps);
  }
  if(rc == GPI_OK)
    return(TRUE);

  dprintf(("DIBSection::BitBlt %X (%d,%d) (%d,%d) to (%d,%d) (%d,%d) returned %d\n", hps, point[0].x, point[0].y, point[1].x, point[1].y, point[2].x, point[2].y, point[3].x, point[3].y, rc));
  dprintf(("WinGetLastError returned %X\n", WinGetLastError(WinQueryAnchorBlock(hwndDest)) & 0xFFFF));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
void DIBSection::SelectDIBObject(HDC hdc)
{
  this->hdc  = hdc;
  hwndParent = WinWindowFromDC(hdc);
  dprintf(("SelectDIBObject(0x%08X) hwndParent = 0x%08X\n",hdc, hwndParent));
}
//******************************************************************************
//******************************************************************************
DIBSection *DIBSection::find(DWORD handle)
{
 DIBSection *dsect = section;

  dibMutex.enter();
  while(dsect)
  {
    if(dsect->handle == handle)
    {
  	dibMutex.leave();
      	return(dsect);
    }
    dsect = dsect->next;
  }
  dibMutex.leave();
  return(NULL);
}
//******************************************************************************
//A bitmap can only be selected into one DC, so this works.
//******************************************************************************
DIBSection *DIBSection::findHDC(HDC hdc)
{
 DIBSection *dsect = section;

  while(dsect)
  {
    if(dsect->hdc == hdc)
    {
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
int DIBSection::GetDIBSection(int iSize, void *lpBuffer)
{
 DIBSECTION *pDIBSection = (DIBSECTION *)lpBuffer;
 LPWINBITMAP dsBm        = (LPWINBITMAP)lpBuffer;

  if(iSize == sizeof(DIBSECTION))
  {
    // BITMAP struct
    pDIBSection->dsBm.bmType       = 0;  // TODO: put the correct value here
    pDIBSection->dsBm.bmWidth      = pOS2bmp->cx;
    pDIBSection->dsBm.bmHeight     = pOS2bmp->cy;
    pDIBSection->dsBm.bmWidthBytes = bmpsize;
    pDIBSection->dsBm.bmPlanes     = pOS2bmp->cPlanes;
    pDIBSection->dsBm.bmBitsPixel  = pOS2bmp->cBitCount;
    pDIBSection->dsBm.bmBits       = bmpBits;
    // BITMAPINFOHEADER data
    pDIBSection->dsBmih.biSize = sizeof(BITMAPINFOHEADER);
    pDIBSection->dsBmih.biWidth       = pOS2bmp->cx;
    pDIBSection->dsBmih.biHeight      = pOS2bmp->cy;
    pDIBSection->dsBmih.biPlanes      = pOS2bmp->cPlanes;
    pDIBSection->dsBmih.biBitCount    = pOS2bmp->cBitCount;
    pDIBSection->dsBmih.biCompression = pOS2bmp->ulCompression;
    pDIBSection->dsBmih.biSizeImage   = pOS2bmp->cbImage;
    pDIBSection->dsBmih.biXPelsPerMeter = 0; // TODO: put the correct value here
    pDIBSection->dsBmih.biYPelsPerMeter = 0;
    pDIBSection->dsBmih.biClrUsed       = (1<< pOS2bmp->cBitCount);
    pDIBSection->dsBmih.biClrImportant  = 0;

    pDIBSection->dsBitfields[0] = 0; // TODO: put the correct value here
    pDIBSection->dsBitfields[1] = 0;
    pDIBSection->dsBitfields[2] = 0;

    pDIBSection->dshSection = this->handle;

    pDIBSection->dsOffset = 0; // TODO: put the correct value here

    return sizeof(DIBSECTION);
  }
  else
  if(iSize == sizeof(WINBITMAP))
  {
    	dsBm->bmType       = 0;  // TODO: put the correct value here
    	dsBm->bmWidth      = pOS2bmp->cx;
    	dsBm->bmHeight     = pOS2bmp->cy;
    	dsBm->bmWidthBytes = bmpsize;
    	dsBm->bmPlanes     = pOS2bmp->cPlanes;
    	dsBm->bmBitsPixel  = pOS2bmp->cBitCount;
    	dsBm->bmBits       = bmpBits;
	return sizeof(WINBITMAP);
  }
  return 0;

}
//******************************************************************************
//******************************************************************************
char  DIBSection::GetBitCount()
{
   if(pOS2bmp == NULL)
     return 0;
   else
     return pOS2bmp->cBitCount;
}
//******************************************************************************
//******************************************************************************
DIBSection *DIBSection::section = NULL;
