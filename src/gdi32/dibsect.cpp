/* $Id: dibsect.cpp,v 1.23 2000-03-25 12:19:07 sandervl Exp $ */

/*
 * GDI32 DIB sections
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * NOTE:
 * This is not a complete solution for CreateDIBSection, but enough for Quake 2!
 *
 */
#define  INCL_GPI
#define  INCL_WIN
#include <os2wrap.h>  //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#define  OS2_ONLY
#include "dibsect.h"
#include <vmutex.h>
#include <win32api.h>
#include <winconst.h>
#include <win32wnd.h>
#include <cpuhlp.h>
#include "oslibgpi.h"
#include "rgbcvt.h"

#define DBG_LOCALLOG	DBG_dibsect
#include "dbglocal.h"

static VMutex dibMutex;

//******************************************************************************
//******************************************************************************
DIBSection::DIBSection(BITMAPINFOHEADER_W *pbmi, char *pColors, DWORD iUsage, DWORD hSection, DWORD dwOffset, DWORD handle, int fFlip)
                : bmpBits(NULL), pOS2bmp(NULL), next(NULL), bmpBitsRGB565(NULL)
{
  int  os2bmpsize;

  bmpsize = pbmi->biWidth;
  /* @@@PH 98/06/07 -- high-color bitmaps don't have palette */

  this->fFlip = fFlip;
  os2bmpsize = sizeof(BITMAPINFO2);

  switch(pbmi->biBitCount)
  {
        case 1:
                bmpsize = ((bmpsize + 31) & ~31) / 8;
                os2bmpsize += ((1 << pbmi->biBitCount)-1)*sizeof(RGB2);
                break;
        case 4:
                bmpsize = ((bmpsize + 7) & ~7) / 2;
                os2bmpsize += ((1 << pbmi->biBitCount)-1)*sizeof(RGB2);
                break;
        case 8:
                os2bmpsize += ((1 << pbmi->biBitCount)-1)*sizeof(RGB2);
        	bmpsize = (bmpsize + 3) & ~3;
                break;
        case 16:
                bmpsize *= 2;
        	bmpsize = (bmpsize + 3) & ~3;
                break;
        case 24:
                bmpsize *= 3;
        	bmpsize = (bmpsize + 3) & ~3;
                break;
        case 32:
                bmpsize *= 4;
                break;
	default:
		dprintf(("Unsupported nr of bits %d", pbmi->biBitCount));
		DebugInt3();
		break;
   }

   this->hSection = hSection;
   if(hSection) {
	bmpBits = (char *)MapViewOfFile(hSection, FILE_MAP_ALL_ACCESS_W, 0, dwOffset, bmpsize*pbmi->biHeight);
	if(!bmpBits) {
		dprintf(("Dibsection: mapViewOfFile %x failed!", hSection));
		DebugInt3();
	}
   }
   if(!bmpBits) {
	DosAllocMem((PPVOID)&bmpBits, bmpsize*pbmi->biHeight, PAG_READ|PAG_WRITE|PAG_COMMIT);
   }
   memset(bmpBits, 0, bmpsize*pbmi->biHeight);

   pOS2bmp = (BITMAPINFO2 *)malloc(os2bmpsize);

   memset(pOS2bmp, /* set header + palette entries to zero */
          0,
          os2bmpsize);

   pOS2bmp->cbFix         = sizeof(BITMAPINFO2) - sizeof(RGB2);
   pOS2bmp->cx            = pbmi->biWidth;
   pOS2bmp->cy            = pbmi->biHeight;
   pOS2bmp->cPlanes       = pbmi->biPlanes;
   pOS2bmp->cBitCount     = pbmi->biBitCount;
   pOS2bmp->ulCompression = pbmi->biCompression;
   //SvL: Ignore BI_BITFIELDS type (GpiDrawBits fails otherwise)
   if(pOS2bmp->ulCompression == BI_BITFIELDS) {
	pOS2bmp->ulCompression = 0;
   }
   pOS2bmp->cbImage       = pbmi->biSizeImage;
   dprintf(("handle                 %x", handle));
   dprintf(("pOS2bmp->cx            %d\n", pOS2bmp->cx));
   dprintf(("pOS2bmp->cy            %d\n", pOS2bmp->cy));
   dprintf(("pOS2bmp->cPlanes       %d\n", pOS2bmp->cPlanes));
   dprintf(("pOS2bmp->cBitCount     %d\n", pOS2bmp->cBitCount));
   dprintf(("pOS2bmp->ulCompression %d\n", pOS2bmp->ulCompression));
   dprintf(("pOS2bmp->cbImage       %d\n", pOS2bmp->cbImage));
   dprintf(("Bits at %x, size %d",bmpBits, bmpsize*pbmi->biHeight));

   // clear DIBSECTION structure
   memset(&dibinfo, 0, sizeof(dibinfo));

   // copy BITMAPINFOHEADER data into DIBSECTION structure
   memcpy(&dibinfo.dsBmih, pbmi, sizeof(*pbmi));
   dibinfo.dsBm.bmType      = 0;
   dibinfo.dsBm.bmWidth     = pbmi->biWidth;
   dibinfo.dsBm.bmHeight    = pbmi->biHeight;
   dibinfo.dsBm.bmWidthBytes= bmpsize;
   dibinfo.dsBm.bmPlanes    = pbmi->biPlanes;
   dibinfo.dsBm.bmBitsPixel = pbmi->biBitCount;
   dibinfo.dsBm.bmBits      = bmpBits;

   dibinfo.dshSection       = handle;
   dibinfo.dsOffset         = 0; // TODO: put the correct value here (if createdibsection with file handle)

   if(iUsage == DIB_PAL_COLORS || pbmi->biBitCount <= 8)
   {
        dibinfo.dsBitfields[0] = dibinfo.dsBitfields[1] = dibinfo.dsBitfields[2] = 0;
   }
   else {
	switch(pbmi->biBitCount)
   	{
           case 16:
               	dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS) ? *(DWORD *)pColors : 0x7c00;
               	dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 1) : 0x03e0;
               	dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 2) : 0x001f;

		//double buffer for rgb 555 dib sections (for conversion)
		if(dibinfo.dsBitfields[1] == 0x03e0) {
			DosAllocMem((PPVOID)&bmpBitsRGB565, bmpsize*pbmi->biHeight, PAG_READ|PAG_WRITE|PAG_COMMIT);
		}
               	break;

           case 24:
               	dibinfo.dsBitfields[0] = 0xff;
               	dibinfo.dsBitfields[1] = 0xff00;
               	dibinfo.dsBitfields[2] = 0xff0000;
               	break;

           case 32:
               	dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS) ? *(DWORD *)pColors : 0xff;
               	dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 1) : 0xff00;
               	dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 2) : 0xff0000;
		if(dibinfo.dsBitfields[0] != 0xff && dibinfo.dsBitfields[1] != 0xff00 && dibinfo.dsBitfields[2] != 0xff0000) {
			dprintf(("DIBSection: unsupported bitfields for 32 bits bitmap!!"));
		}
               	break;
       	}
	dprintf(("BI_BITFIELDS %x %x %x", dibinfo.dsBitfields[0], dibinfo.dsBitfields[1], dibinfo.dsBitfields[2]));
   }

   this->handle = handle;
   this->iUsage = iUsage;

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

   if(hSection) {
	UnmapViewOfFile(bmpBits);
   }
   else
   if(bmpBits)
        DosFreeMem(bmpBits);

   if(bmpBitsRGB565)
 	DosFreeMem(bmpBitsRGB565);

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
                          lines, const VOID *bits, BITMAPINFOHEADER_W *pbmi,
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
      bmpsize = ((bmpsize + 31) & ~31) / 8;
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmpsize += palsize;
      break;
    case 4:
      bmpsize = ((bmpsize + 7) & ~7) / 2;
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmpsize += palsize;
      break;
    case 8:
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmpsize += palsize;
      bmpsize = (bmpsize + 3) & ~3;
      break;
    case 16:
      bmpsize *= 2;
      bmpsize = (bmpsize + 3) & ~3;
      break;
    case 24:
      bmpsize *= 3;
      bmpsize = (bmpsize + 3) & ~3;
      break;
    case 32:
      bmpsize *= 4;
      break;
   }

   //SvL: TODO: Correct??
   if(!hSection && pOS2bmp->cx != pbmi->biWidth && pOS2bmp->cy != pbmi->biHeight &&
      pOS2bmp->cBitCount != pbmi->biBitCount) 
   {
	char *oldbits = bmpBits;
	int oldsize = dibinfo.dsBm.bmWidthBytes * dibinfo.dsBm.bmHeight;

	DosAllocMem((PPVOID)&bmpBits, bmpsize*pbmi->biHeight, PAG_READ|PAG_WRITE|PAG_COMMIT);
	memcpy(bmpBits, oldbits, min(oldsize, bmpsize*pbmi->biHeight));
	DosFreeMem(oldbits);
   }
   pOS2bmp    = (BITMAPINFO2 *)realloc(pOS2bmp, os2bmpsize);
   pOS2bmp->cbFix         = sizeof(BITMAPINFO2) - sizeof(RGB2);
   pOS2bmp->cx            = pbmi->biWidth;
   pOS2bmp->cy            = pbmi->biHeight;
   pOS2bmp->cPlanes       = pbmi->biPlanes;
   pOS2bmp->cBitCount     = pbmi->biBitCount;
   pOS2bmp->ulCompression = pbmi->biCompression;
   pOS2bmp->cbImage       = pbmi->biSizeImage;

   // clear DIBSECTION structure
   memset(&dibinfo, 0, sizeof(dibinfo));

   // copy BITMAPINFOHEADER data into DIBSECTION structure
   memcpy(&dibinfo.dsBmih, pbmi, sizeof(*pbmi));
   dibinfo.dsBm.bmType      = 0;
   dibinfo.dsBm.bmWidth     = pbmi->biWidth;
   dibinfo.dsBm.bmHeight    = pbmi->biHeight;
   dibinfo.dsBm.bmWidthBytes= bmpsize;
   dibinfo.dsBm.bmPlanes    = pbmi->biPlanes;
   dibinfo.dsBm.bmBitsPixel = pbmi->biBitCount;
   dibinfo.dsBm.bmBits      = bmpBits;

   dibinfo.dshSection       = hSection;
   dibinfo.dsOffset         = 0; // TODO: put the correct value here (if createdibsection with file handle)

   if(coloruse == DIB_PAL_COLORS || pbmi->biBitCount <= 8)
   {
        dibinfo.dsBitfields[0] = dibinfo.dsBitfields[1] = dibinfo.dsBitfields[2] = 0;
   }
   else {
	char *pColors = (char *)pbmi + 1;

	switch(pbmi->biBitCount)
   	{
           case 16:
               	dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS) ? *(DWORD *)pColors : 0x7c00;
               	dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 1) : 0x03e0;
               	dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 2) : 0x001f;
               	break;

           case 24:
               	dibinfo.dsBitfields[0] = 0xff;
               	dibinfo.dsBitfields[1] = 0xff00;
               	dibinfo.dsBitfields[2] = 0xff0000;
               	break;

           case 32:
               	dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS) ? *(DWORD *)pColors : 0xff;
               	dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 1) : 0xff00;
               	dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS) ? *((DWORD *)pColors + 2) : 0xff0000;
		if(dibinfo.dsBitfields[0] != 0xff && dibinfo.dsBitfields[1] != 0xff00 && dibinfo.dsBitfields[2] != 0xff0000) {
			dprintf(("DIBSection: unsupported bitfields for 32 bits bitmap!!"));
		}
               	break;
       	}
	dprintf(("BI_BITFIELDS %x %x %x", dibinfo.dsBitfields[0], dibinfo.dsBitfields[1], dibinfo.dsBitfields[2]));
   }

   dprintf(("DIBSection::SetDIBits (%d,%d), %d %d", pbmi->biWidth, pbmi->biHeight, pbmi->biBitCount, pbmi->biCompression));
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
 PVOID  bitmapBits = NULL;

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

  //SvL: Optimize this.. (don't convert entire bitmap if only a part will be blitted to the dc)
  if(dibinfo.dsBitfields[1] == 0x3E0) {//RGB 555?
       	dprintf(("DIBSection::BitBlt; convert rgb 555 to 565"));

	if(bmpBitsRGB565 == NULL)
		DebugInt3();

	if(CPUFeatures & CPUID_MMX) {
		RGB555to565MMX((WORD *)bmpBitsRGB565, (WORD *)bmpBits, pOS2bmp->cbImage/sizeof(WORD));
	}
	else   	RGB555to565((WORD *)bmpBitsRGB565, (WORD *)bmpBits, pOS2bmp->cbImage/sizeof(WORD));
	rc = GpiDrawBits(hps, bmpBitsRGB565, pOS2bmp, 4, &point[0], ROP_SRCCOPY, BBO_OR);
  }
  else	rc = GpiDrawBits(hps, bmpBits, pOS2bmp, 4, &point[0], ROP_SRCCOPY, BBO_OR);

  if(hwndDest != 0)
  {
    WinReleasePS(hps);
  }
  if(rc == GPI_OK) {
    	return(TRUE);
  }
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
  dprintf(("SelectDIBObject %x into %x hwndParent = %x", handle, hdc, hwndParent));
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
 LPBITMAP_W  dsBm        = (LPBITMAP_W)lpBuffer;

  dprintf2(("GetDIBSection %x %d %x", handle, iSize, lpBuffer));
  if(iSize == sizeof(DIBSECTION))
  {
	memcpy(pDIBSection, &dibinfo, sizeof(dibinfo));
        return sizeof(DIBSECTION);
  }
  else
  if(iSize == sizeof(BITMAP_W))
  {
	memcpy(dsBm, &dibinfo.dsBm, sizeof(dibinfo.dsBm));
	return sizeof(BITMAP_W);
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
