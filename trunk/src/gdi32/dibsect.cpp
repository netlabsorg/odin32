/* $Id: dibsect.cpp,v 1.49 2001-03-29 18:52:53 sandervl Exp $ */

/*
 * GDI32 DIB sections
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <winuser32.h>
#include <cpuhlp.h>
#include <dcdata.h>
#include "oslibgpi.h"
#include "rgbcvt.h"

#define DBG_LOCALLOG  DBG_dibsect
#include "dbglocal.h"

static VMutex dibMutex;

//******************************************************************************
//******************************************************************************
DIBSection::DIBSection(BITMAPINFOHEADER_W *pbmi, char *pColors, DWORD iUsage, DWORD hSection, DWORD dwOffset, DWORD handle, int fFlip)
                : bmpBits(NULL), pOS2bmp(NULL), next(NULL), bmpBitsDblBuffer(NULL),
                  hdc(0), hwndParent(0)
{
 int  palsize=0;

  bmpsize = pbmi->biWidth;
  /* @@@PH 98/06/07 -- high-color bitmaps don't have palette */

  this->fFlip = fFlip;
  os2bmphdrsize = sizeof(BITMAPINFO2);

  switch(pbmi->biBitCount)
  {
        case 1:
            bmpsize = ((bmpsize + 31) & ~31) / 8;
            palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
            os2bmphdrsize += palsize;
            break;
        case 4:
            bmpsize = ((bmpsize + 7) & ~7) / 2;
            palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
            os2bmphdrsize += palsize;
            break;
        case 8:
            palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
            os2bmphdrsize += palsize;
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
   this->dwOffset = dwOffset;
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

   pOS2bmp = (BITMAPINFO2 *)malloc(os2bmphdrsize);

   memset(pOS2bmp, /* set header + palette entries to zero */
          0,
          os2bmphdrsize);

   pOS2bmp->cbFix         = sizeof(BITMAPINFO2) - sizeof(RGB2);
   pOS2bmp->cx            = pbmi->biWidth;
   pOS2bmp->cy            = pbmi->biHeight;
   pOS2bmp->cPlanes       = pbmi->biPlanes;
   pOS2bmp->cBitCount     = pbmi->biBitCount;
   pOS2bmp->ulCompression = pbmi->biCompression; //same as OS/2 (uncompressed, rle8, rle4)
   //SvL: Ignore BI_BITFIELDS_W type (GpiDrawBits fails otherwise)
   if(pOS2bmp->ulCompression == BI_BITFIELDS_W) {
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

   dibinfo.dshSection       = hSection;
   dibinfo.dsOffset         = dwOffset;

   if(iUsage == DIB_PAL_COLORS || pbmi->biBitCount <= 8)
   {
        dibinfo.dsBitfields[0] = dibinfo.dsBitfields[1] = dibinfo.dsBitfields[2] = 0;
        if(palsize) {
            SetDIBColorTable(0, (1 << pbmi->biBitCount), (RGBQUAD *)(pbmi+1));
        }
   }
   else {
        switch(pbmi->biBitCount)
        {
           case 16:
                dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS_W) ? *(DWORD *)pColors : 0x7c00;
                dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 1) : 0x03e0;
                dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 2) : 0x001f;
                break;

           case 24:
                dibinfo.dsBitfields[0] = 0xff;
                dibinfo.dsBitfields[1] = 0xff00;
                dibinfo.dsBitfields[2] = 0xff0000;
                break;

           case 32:
                dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS_W) ? *(DWORD *)pColors : 0xff;
                dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 1) : 0xff00;
                dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 2) : 0xff0000;
                if(dibinfo.dsBitfields[0] != 0xff && dibinfo.dsBitfields[1] != 0xff00 && dibinfo.dsBitfields[2] != 0xff0000) {
                    dprintf(("DIBSection: unsupported bitfields for 32 bits bitmap!!"));
                }
                break;
        }
        dprintf(("BI_BITFIELDS_W %x %x %x", dibinfo.dsBitfields[0], dibinfo.dsBitfields[1], dibinfo.dsBitfields[2]));
   }
   //double buffer for rgb 555 dib sections (for conversion) or flipped sections
   if(dibinfo.dsBitfields[1] == 0x03e0 || (fFlip & FLIP_VERT)) {
        DosAllocMem((PPVOID)&bmpBitsDblBuffer, bmpsize*pbmi->biHeight, PAG_READ|PAG_WRITE|PAG_COMMIT);
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
////       dprintf2(("Increment section to %08X\n",dsect->next));
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

   if(bmpBitsDblBuffer)
        DosFreeMem(bmpBitsDblBuffer);

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
  int  palsize=0;

  bmpsize = pbmi->biWidth;
  os2bmphdrsize = sizeof(BITMAPINFO2);

  switch(pbmi->biBitCount)
  {
    case 1:
      bmpsize = ((bmpsize + 31) & ~31) / 8;
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmphdrsize += palsize;
      break;
    case 4:
      bmpsize = ((bmpsize + 7) & ~7) / 2;
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmphdrsize += palsize;
      break;
    case 8:
      palsize = ((1 << pbmi->biBitCount))*sizeof(RGB2);
      os2bmphdrsize += palsize;
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
   pOS2bmp    = (BITMAPINFO2 *)realloc(pOS2bmp, os2bmphdrsize);
   pOS2bmp->cbFix         = sizeof(BITMAPINFO2) - sizeof(RGB2);
   pOS2bmp->cx            = pbmi->biWidth;
   pOS2bmp->cy            = pbmi->biHeight;
   pOS2bmp->cPlanes       = pbmi->biPlanes;
   pOS2bmp->cBitCount     = pbmi->biBitCount;
   pOS2bmp->ulCompression = pbmi->biCompression; //same as OS/2 (uncompressed, rle8, rle4)
   //SvL: Ignore BI_BITFIELDS_W type (GpiDrawBits fails otherwise)
   if(pOS2bmp->ulCompression == BI_BITFIELDS_W) {
        pOS2bmp->ulCompression = 0;
   }
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
   dibinfo.dsOffset         = dwOffset;

   if(coloruse == DIB_PAL_COLORS || pbmi->biBitCount <= 8)
   {
        dibinfo.dsBitfields[0] = dibinfo.dsBitfields[1] = dibinfo.dsBitfields[2] = 0;
   }
   else {
        char *pColors = (char *)pbmi + 1;

        switch(pbmi->biBitCount)
        {
           case 16:
                dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS_W) ? *(DWORD *)pColors : 0x7c00;
                dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 1) : 0x03e0;
                dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 2) : 0x001f;
                break;

           case 24:
                dibinfo.dsBitfields[0] = 0xff;
                dibinfo.dsBitfields[1] = 0xff00;
                dibinfo.dsBitfields[2] = 0xff0000;
                break;

           case 32:
                dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS_W) ? *(DWORD *)pColors : 0xff;
                dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 1) : 0xff00;
                dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 2) : 0xff0000;
                if(dibinfo.dsBitfields[0] != 0xff && dibinfo.dsBitfields[1] != 0xff00 && dibinfo.dsBitfields[2] != 0xff0000) {
                    dprintf(("DIBSection: unsupported bitfields for 32 bits bitmap!!"));
                }
                break;
        }
        dprintf(("BI_BITFIELDS_W %x %x %x", dibinfo.dsBitfields[0], dibinfo.dsBitfields[1], dibinfo.dsBitfields[2]));
   }

   //double buffer for rgb 555 dib sections (for conversion) or flipped sections
   if(dibinfo.dsBitfields[1] == 0x03e0 || (fFlip & FLIP_VERT)) {
        if(bmpBitsDblBuffer) {
            DosFreeMem(bmpBitsDblBuffer);
        }
        DosAllocMem((PPVOID)&bmpBitsDblBuffer, dibinfo.dsBm.bmWidthBytes*pbmi->biHeight, PAG_READ|PAG_WRITE|PAG_COMMIT);
   }

   dprintf(("DIBSection::SetDIBits (%d,%d), %d %d", pbmi->biWidth, pbmi->biHeight, pbmi->biBitCount, pbmi->biCompression));
   if(palsize) {
        SetDIBColorTable(0, 1 << pbmi->biBitCount, (RGBQUAD *)(pbmi+1));
   }

   if(bits)
   {
      if(pOS2bmp->ulCompression == BCA_UNCOMP) {
          int size = bmpsize*lines;
          memcpy(bmpBits+bmpsize*startscan, bits, size);
      }
      else {
          dprintf(("Compressed image!!"));
          if(startscan != 0) {
               dprintf(("WARNING: Compressed image & startscan != 0!!!!"));
          }
          memcpy(bmpBits, bits, pbmi->biSizeImage);
      }
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
#ifdef DEBUG_PALETTE
    dprintf2(("Index %d : 0x%08X\n",i, *((ULONG*)(&pOS2bmp->argbColor[i])) ));
#endif
  }

  return(cEntries);
}
//******************************************************************************
//******************************************************************************
int DIBSection::SetDIBColorTable(int startIdx, int cEntries, PALETTEENTRY *palentry)
{
 int i;

  if(startIdx + cEntries > (1 << pOS2bmp->cBitCount))
  {
    dprintf(("DIBSection::SetDIBColorTable, invalid nr of entries %d %d\n", startIdx, cEntries));
    return(0);
  }

  for(i=startIdx;i<cEntries;i++)
  {
    pOS2bmp->argbColor[i].fcOptions = 0;
    pOS2bmp->argbColor[i].bBlue  = palentry[i].peBlue;
    pOS2bmp->argbColor[i].bGreen = palentry[i].peGreen;
    pOS2bmp->argbColor[i].bRed   = palentry[i].peRed;
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
 LONG   rc, hdcHeight, hdcWidth;
 PVOID  bitmapBits = NULL;
 int    oldyinversion = 0;
 BOOL   fRestoryYInversion = FALSE, fFrameWindowDC = FALSE;
 HWND   hwndDest;
 pDCData pHps;

  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdcDest);
  if(!pHps)
  {
        SetLastError(ERROR_INVALID_HANDLE_W);
        return FALSE;
  }

  hwndDest = WindowFromDC(hdcDest); //could return desktop window, so check that
  if(hwndDest && pHps->hwnd && !pHps->isClient) {
        fFrameWindowDC = TRUE;
  }

  dprintf(("DIBSection::BitBlt %x %X (hps %x) %x to(%d,%d)(%d,%d) from (%d,%d)(%d,%d) rop %x flip %x",
          handle, hdcDest, hps, hwndDest, nXdest, nYdest, nDestWidth, nDestHeight,
          nXsrc, nYsrc, nSrcWidth, nSrcHeight, Rop, fFlip));

  if(hwndDest) {
        RECT rect;

        if(fFrameWindowDC) {
              GetWindowRect(hwndDest, &rect);
        }
        else  GetClientRect(hwndDest, &rect);
        hdcHeight = rect.bottom - rect.top;
        hdcWidth  = rect.right - rect.left;
  }
  else {
        hdcHeight = pHps->bitmapHeight;
        hdcWidth  = pHps->bitmapWidth;
  }

  //win32 coordinates are relative to left top, OS/2 expects left bottom
  //source rectangle is non-inclusive (top, right not included)
  //destination rectangle is incl.-inclusive (everything included)

  if(nXdest + nDestWidth > hdcWidth) {
        nDestWidth  = hdcWidth - nXdest;
  }

  if(nYdest + nDestHeight > hdcHeight) {
        nDestHeight = hdcHeight - nYdest;
  }

  point[0].x = nXdest;
  point[0].y = hdcHeight - nYdest - nDestHeight;
  point[1].x = nXdest + nDestWidth - 1;
  point[1].y = hdcHeight - nYdest - 1;

  //target rectangle is inclusive-inclusive
  if(nXsrc + nSrcWidth > pOS2bmp->cx) {
        nSrcWidth  = pOS2bmp->cx - nXsrc;
  }
  if(nYsrc + nSrcHeight > pOS2bmp->cy) {
        nSrcHeight = pOS2bmp->cy - nYsrc;
  }
  point[2].x = nXsrc;
  point[2].y = pOS2bmp->cy - nYsrc - nSrcHeight;
  point[3].x = nXsrc + nSrcWidth;
  point[3].y = pOS2bmp->cy - nYsrc;

  dprintf(("DIBSection::BitBlt (%d,%d)(%d,%d) from (%d,%d)(%d,%d) dim (%d,%d)(%d,%d)", point[0].x, point[0].y,
           point[1].x, point[1].y, point[2].x, point[2].y, point[3].x, point[3].y,
           nDestWidth, nDestHeight, nSrcWidth, nSrcHeight));

#ifdef INVERT
  oldyinversion = GpiQueryYInversion(hps);
  if(oldyinversion != 0) {
        GpiEnableYInversion(hps, 0);
        fRestoryYInversion = TRUE;
  }
#endif

#ifdef DEBUG
  RECTL rcltemp;
  GreGetDCOrigin(hps, (PPOINTL)&rcltemp);
  dprintf(("origin (%d,%d) yinv %d", rcltemp.xLeft, rcltemp.yBottom, oldyinversion));
#endif

  if(fFlip & FLIP_HOR)
  {
      ULONG x;
      x = point[0].x;
      point[0].x = point[1].x;
      point[1].x = x;
  }

  ULONG os2mode, winmode;

  os2mode = BBO_OR;
  winmode = GetStretchBltMode(hdcDest);
  switch(winmode) {
  case BLACKONWHITE_W:
        os2mode = BBO_AND;
        break;
  case WHITEONBLACK_W:
  case HALFTONE_W: //TODO:
        os2mode = BBO_OR;
        break;
  case COLORONCOLOR_W:
        os2mode = BBO_IGNORE;
        break;
  }
  if(fFlip & FLIP_VERT) {
        //manually reverse bitmap data
        char *src = bmpBits + (pOS2bmp->cy-1)*dibinfo.dsBm.bmWidthBytes;
        char *dst = bmpBitsDblBuffer;
        for(int i=0;i<pOS2bmp->cy;i++) {
            memcpy(dst, src, dibinfo.dsBm.bmWidthBytes);
            dst += dibinfo.dsBm.bmWidthBytes;
            src -= dibinfo.dsBm.bmWidthBytes;
        }
        bitmapBits = bmpBitsDblBuffer;
  }
  else  bitmapBits = bmpBits;

  //SvL: Optimize this.. (don't convert entire bitmap if only a part will be blitted to the dc)
  if(dibinfo.dsBitfields[1] == 0x3E0) {//RGB 555?
        dprintf(("DIBSection::BitBlt; convert rgb 555 to 565 (old y inv. = %d)", oldyinversion));

        if(bmpBitsDblBuffer == NULL)
            DebugInt3();

        // PH 2000/10/01 - Fix for Beyond Compare 1.9d
        // Note: according to documentation, cmImage can be zero for
        // RGB- / non-compressed bitmaps.
        int iLength = pOS2bmp->cbImage;
        if (iLength == 0)
            iLength = pOS2bmp->cx * pOS2bmp->cy * (pOS2bmp->cBitCount >> 3);

        if (iLength > 0)
        {
            if(CPUFeatures & CPUID_MMX)
                    RGB555to565MMX((WORD *)bmpBitsDblBuffer, (WORD *)bitmapBits, iLength/sizeof(WORD));
            else    RGB555to565((WORD *)bmpBitsDblBuffer, (WORD *)bitmapBits, iLength/sizeof(WORD));
        }
        else
        {
            dprintf(("GDI32: DIBSect::BitBlt: WARNING! zero-length bitmap! %08xh", pOS2bmp));
        }


        rc = GpiDrawBits(hps, bmpBitsDblBuffer, pOS2bmp, 4, &point[0], ROP_SRCCOPY, os2mode);
  }
  else {
	rc = GpiDrawBits(hps, bitmapBits, pOS2bmp, 4, &point[0], ROP_SRCCOPY, os2mode);
  }
  if(rc == GPI_OK) {
        DIBSection *destdib = DIBSection::findHDC(hdcDest);
        if(destdib) {
            destdib->sync(hps, nYdest, nDestHeight, FALSE);
        }
#ifdef INVERT
        //restore old y inversion height
        if(fRestoryYInversion) GpiEnableYInversion(hps, oldyinversion);
#endif
            SetLastError(ERROR_SUCCESS_W);

        return(TRUE);
  }
#ifdef INVERT
  if(fRestoryYInversion) GpiEnableYInversion(hps, oldyinversion);
#endif

  dprintf(("DIBSection::BitBlt %X (%d,%d) (%d,%d) to (%d,%d) (%d,%d) returned %d\n", hps, point[0].x, point[0].y, point[1].x, point[1].y, point[2].x, point[2].y, point[3].x, point[3].y, rc));
  dprintf(("WinGetLastError returned %X\n", WinGetLastError(WinQueryAnchorBlock(hwndDest)) & 0xFFFF));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
void DIBSection::sync(HDC hdc, DWORD nYdest, DWORD nDestHeight, BOOL orgYInversion)
{
 APIRET rc;
 char  *destBuf;

  dprintf(("Sync destination dibsection %x (%x) (%d,%d) flip %d", handle, hdc, nYdest, nDestHeight, fFlip));

  BITMAPINFO2 *tmphdr = (BITMAPINFO2 *)malloc(os2bmphdrsize);
  memcpy(tmphdr, pOS2bmp, os2bmphdrsize);

#ifdef INVERT
  int oldyinversion = 0;
  if(orgYInversion == TRUE) {
      oldyinversion = GpiQueryYInversion(hdc);
      dprintf(("Sync destination dibsection: hdc y inversion = %d", oldyinversion));
      if(oldyinversion != 0) {
          GpiEnableYInversion(hdc, 0);
      }
  }
#endif

  if(fFlip & FLIP_VERT) {
        destBuf = bmpBitsDblBuffer + nYdest*dibinfo.dsBm.bmWidthBytes;

        rc = GpiQueryBitmapBits(hdc, nYdest, nDestHeight, destBuf,
                                tmphdr);
        //manually reverse bitmap data
        char *src = destBuf;
        char *dst = GetDIBObject() + (nYdest+nDestHeight-1)*dibinfo.dsBm.bmWidthBytes;
        for(int i=0;i<nDestHeight;i++) {
            memcpy(dst, src, dibinfo.dsBm.bmWidthBytes);
            dst -= dibinfo.dsBm.bmWidthBytes;
            src += dibinfo.dsBm.bmWidthBytes;
        }
  }
  else {
        destBuf = GetDIBObject() + nYdest*dibinfo.dsBm.bmWidthBytes;
        rc = GpiQueryBitmapBits(hdc, nYdest, nDestHeight, destBuf,
                                tmphdr);

#ifdef DEBUG_PALETTE
        if(rc != GPI_ALTERROR && tmphdr->cBitCount <= 8) {
            for(int i=0;i<(1<<tmphdr->cBitCount);i++)
            {
                dprintf2(("Index %d : 0x%08X\n",i, *((ULONG*)(&tmphdr->argbColor[i])) ));
            }
        }
#endif
  }

#if 0
  if(dibinfo.dsBitfields[1] == 0x3E0) {//RGB 555?
    dprintf(("DIBSection::sync: convert RGB 565 to RGB 555"));

    destBuf = GetDIBObject() + nYdest*dibinfo.dsBm.bmWidthBytes;

    if(CPUFeatures & CPUID_MMX) {
            RGB565to555MMX((WORD *)destBuf, (WORD *)destBuf, (nDestHeight*dibinfo.dsBm.bmWidthBytes)/sizeof(WORD));
    }
    else    RGB565to555((WORD *)destBuf, (WORD *)destBuf, (nDestHeight*dibinfo.dsBm.bmWidthBytes)/sizeof(WORD));
  }
#endif
  free(tmphdr);
  if(rc != nDestHeight) {
    DebugInt3();
  }

#ifdef INVERT
  if(oldyinversion) GpiEnableYInversion(hdc, oldyinversion);
#endif

}
//******************************************************************************
//manual sync if no stretching and bpp is the same
//WARNING: this also assumes the colortables are the same
//******************************************************************************
void DIBSection::sync(DWORD xDst, DWORD yDst, DWORD widthDst, DWORD heightDst, PVOID bits)
{
 char *srcbuf, *destbuf;
 int  linesize;

   srcbuf  = (char *)bits + dibinfo.dsBm.bmWidthBytes*yDst +
                    (xDst*dibinfo.dsBm.bmWidthBytes)/pOS2bmp->cx;
   destbuf = (char *)GetDIBObject() + dibinfo.dsBm.bmWidthBytes*yDst +
                    (xDst*dibinfo.dsBm.bmWidthBytes)/pOS2bmp->cx;
   linesize = (widthDst*dibinfo.dsBm.bmWidthBytes)/pOS2bmp->cx;
   for(int i=0;i<heightDst;i++) {
        memcpy(destbuf, srcbuf, linesize);
        destbuf += dibinfo.dsBm.bmWidthBytes;
        srcbuf  += linesize;
   }
}
//******************************************************************************
//******************************************************************************
void DIBSection::SelectDIBObject(HDC hdc)
{
  this->hdc  = hdc;
  hwndParent = WindowFromDC(hdc);
  dprintf(("SelectDIBObject %x into %x hwndParent = %x", handle, hdc, hwndParent));
}
//******************************************************************************
//******************************************************************************
DIBSection *DIBSection::findObj(HANDLE handle)
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
void DIBSection::deleteSection(HANDLE handle)
{
 DIBSection *dsect = findObj(handle);

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
int  DIBSection::GetBitCount()
{
   if(pOS2bmp == NULL)
     return 0;
   else
     return pOS2bmp->cBitCount;
}
//******************************************************************************
//******************************************************************************
int DIBSection::GetHeight()
{
   if(pOS2bmp == NULL)
     return 0;
   else
     return pOS2bmp->cy;
}
//******************************************************************************
//******************************************************************************
int DIBSection::GetWidth()
{
   if(pOS2bmp == NULL)
     return 0;
   else
     return pOS2bmp->cx;
}
//******************************************************************************
//******************************************************************************
DIBSection *DIBSection::section = NULL;
