/* $Id: dibsect.cpp,v 1.69 2004-02-16 16:53:32 sandervl Exp $ */

/*
 * GDI32 DIB sections
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 * Copyright 2002-2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Basic GPI object and bitmap data synchronization:
 *
 * - CreateDIBSection:
 *      register a memory range for exception notification
 *      set to read/commit during creation
 *
 * - if(exception)
 *      if(invalid)
 *         sync and set to commit/read, clear invalid flag
 *      if(write violation) 
 *         make as dirty and set range to read/write
 *
 * - before GDI operation
 *       if(dirty) 
 *           flush (set bitmap bits), clear dirty flag, set range to readonly
 *
 * - after GDI operation
 *       set invalid flag, set range to invalid
 *
 * - GdiFlush: 
 *       NOP
 *
 * Should probably detect partial changes to avoid having to sync the entire
 * DIB section each time something minor changes.
 *
 * Might get a little bit more complicated when the application gives us a memory
 * mapped file handle. (although we can change the protection flags of aliased pages)
 *
 */
#define  INCL_GPI
#define  INCL_WIN
#include <os2wrap.h>  //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include <win32api.h>
#include <winconst.h>
#include <winuser32.h>
#include <dcdata.h>
#include "dibsect.h"
#include "oslibgpi.h"
#include "rgbcvt.h"
#include <memmap.h>

#define DBG_LOCALLOG  DBG_dibsect
#include "dbglocal.h"

static BOOL WIN32API DIBExceptionNotify(LPVOID lpBase, ULONG offset, BOOL fWriteAccess, 
                                        DWORD dwSize, DWORD dwUserData);


//******************************************************************************
//******************************************************************************
DIBSection::DIBSection(BITMAPINFOHEADER_W *pbmi, char *pColors, DWORD iUsage, DWORD hSection, DWORD dwOffset, HBITMAP hBitmap, int fFlip)
                : bmpBits(NULL), pOS2bmp(NULL), next(NULL), bmpBitsDblBuffer(NULL),
                  hdc(0), hwndParent(0), fDirty(FALSE), fInvalid(FALSE), dwSize(0)
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
        bmpBits = (char *)MapViewOfFile(hSection, FILE_MAP_ALL_ACCESS_W, 0, dwOffset, bmpsize*pbmi->biHeight - dwOffset);
        if(!bmpBits) {
            dprintf(("Dibsection: mapViewOfFile %x failed!", hSection));
            DebugInt3();
        }
   }
   if(!bmpBits) {
        APIRET rc = DosAllocMem((PPVOID)&bmpBits, bmpsize*pbmi->biHeight, PAG_READ|PAG_COMMIT);
        if(rc) {
            dprintf(("DosAllocMem failed with %d", rc));
            DebugInt3();
        }
        if(MMAP_RegisterMemoryRange(DIBExceptionNotify, bmpBits, bmpsize*pbmi->biHeight, hBitmap) == FALSE) 
        {
            dprintf(("MMAP_RegisterMemoryRange failed!!"));
            DebugInt3();
        }
        dwSize = bmpsize*pbmi->biHeight;
   }
   else memset(bmpBits, 0, bmpsize*pbmi->biHeight);

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
   dprintf(("hBitmap                %x", hBitmap));
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
            SetDIBColorTable(0, (pbmi->biClrUsed) ? pbmi->biClrUsed : (1 << pbmi->biBitCount), (RGBQUAD *)(pbmi+1));
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
           case 32:
                dibinfo.dsBitfields[0] = (pbmi->biCompression == BI_BITFIELDS_W) ? *(DWORD *)pColors       : 0xff0000;
                dibinfo.dsBitfields[1] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 1) : 0x00ff00;
                dibinfo.dsBitfields[2] = (pbmi->biCompression == BI_BITFIELDS_W) ? *((DWORD *)pColors + 2) : 0x0000ff;
                if(dibinfo.dsBitfields[0] != 0xff0000 && dibinfo.dsBitfields[1] != 0xff00 && dibinfo.dsBitfields[2] != 0xff) {
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

   this->hBitmap = hBitmap;
   this->iUsage = iUsage;

   lock();
   if(section == NULL)
   {
     dprintf(("section was NULL\n"));
     section = this;
   }
   else
   {
     DIBSection *dsect = section;
     dprintf2(("Increment section starting at %08X\n",dsect));

     while ( (dsect->next != this) &&
             (dsect->next != NULL) )
     {
////       dprintf2(("Increment section to %08X\n",dsect->next));
       dsect = dsect->next;
     }
     dsect->next = this;
   }
   unlock();
}
//******************************************************************************
//******************************************************************************
DIBSection::~DIBSection()
{
   dprintf(("Delete DIBSection %x", hBitmap));

   if(hSection) {
       UnmapViewOfFile(bmpBits);
   }
   else 
   {
       if(MMAP_UnregisterMemoryRange(bmpBits) == FALSE) 
       {
           dprintf(("MMAP_UnregisterMemoryRange failed!!"));
           DebugInt3();
       }
       if(bmpBits)
           DosFreeMem(bmpBits);
   }

   if(bmpBitsDblBuffer)
       DosFreeMem(bmpBitsDblBuffer);

   if(pOS2bmp)
       free(pOS2bmp);

   lock();
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
   unlock();
}
//******************************************************************************
//******************************************************************************
int DIBSection::SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb)
{
 int i, end;

  dprintf(("SetDIBColorTable %d %d %x", startIdx, cEntries, rgb));

  if(pOS2bmp->cBitCount > 8) {
      dprintf(("DIBSection::SetDIBColorTable: bpp > 8; ignore"));
      return 0;
  }

  end = startIdx + cEntries;
  if(end > (1 << pOS2bmp->cBitCount)) {
      end = (1 << pOS2bmp->cBitCount);
      cEntries = end - startIdx;
  }

  memcpy(&pOS2bmp->argbColor[startIdx], rgb, cEntries*sizeof(RGB2));

  for(i=startIdx;i<end;i++)
  {
    pOS2bmp->argbColor[i].fcOptions = 0;
    dprintf2(("Index %d : 0x%08X\n",i, *((ULONG*)(&pOS2bmp->argbColor[i])) ));
  }
  return(cEntries);
}
//******************************************************************************
//******************************************************************************
int DIBSection::SetDIBColorTable(int startIdx, int cEntries, PALETTEENTRY *palentry)
{
 int i, end;

  if(pOS2bmp->cBitCount > 8) {
      dprintf(("DIBSection::SetDIBColorTable: bpp > 8; ignore"));
      return 0;
  }

  end = startIdx + cEntries;
  if(end > (1 << pOS2bmp->cBitCount)) {
      end = (1 << pOS2bmp->cBitCount);
  }
  for(i=startIdx;i<end;i++)
  {
    pOS2bmp->argbColor[i].fcOptions = 0;
    pOS2bmp->argbColor[i].bBlue  = palentry[i].peBlue;
    pOS2bmp->argbColor[i].bGreen = palentry[i].peGreen;
    pOS2bmp->argbColor[i].bRed   = palentry[i].peRed;
    dprintf2(("Index %d : 0x%08X\n",i, *((ULONG*)(&pOS2bmp->argbColor[i])) ));
  }

  return end - startIdx;
}
//******************************************************************************
//******************************************************************************
int DIBSection::GetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb)
{
  int i, end = startIdx + cEntries;

  if(pOS2bmp->cBitCount > 8) {
      dprintf(("DIBSection::GetDIBColorTable: bpp > 8 -> return 0"));
      return 0;
  }
  if(end > (1 << pOS2bmp->cBitCount)) {
      end = (1 << pOS2bmp->cBitCount);
      dprintf(("DIBSection::GetDIBColorTable: %d->%d", startIdx, end));
  }
  memcpy(rgb, &pOS2bmp->argbColor[startIdx], cEntries*sizeof(RGBQUAD));

  for(i=0;i<cEntries;i++) {
    rgb[i].rgbReserved = 0;
  }

  return end - startIdx;
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
          hBitmap, hdcDest, hps, hwndDest, nXdest, nYdest, nDestWidth, nDestHeight,
          nXsrc, nYsrc, nSrcWidth, nSrcHeight, Rop, fFlip));

  //shortcut; better to sync it here instead of in the exception handler
  //(pages touched inside PMGPI)
  if(isInvalid()) {
      sync();
  }

  if(hwndDest) {
        RECT rect;

        if(fFrameWindowDC) {
              GetWindowRect(hwndDest, &rect);
        }
        else  GetClientRect(hwndDest, &rect);
        hdcHeight = rect.bottom - rect.top;
        hdcWidth  = rect.right - rect.left;
        dprintf(("DIBSection::BitBlt hdc size (%d,%d) (WINDOW)", hdcWidth, hdcHeight));
  }
  else {
        hdcHeight = pHps->bitmapHeight;
        hdcWidth  = pHps->bitmapWidth;
        dprintf(("DIBSection::BitBlt hdc size (%d,%d) (BMP)", hdcWidth, hdcHeight));
  }

  //Don't clip destination size to destination DC size
  //This messes up the two bitmaps in the opening window of Opera 6
  //(choice between MDI & SDI interface)

  //win32 coordinates are relative to left top, OS/2 expects left bottom
  //source rectangle is non-inclusive (top, right not included)
  //destination rectangle is incl.-inclusive (everything included)

  point[0].x = nXdest;
  point[1].x = nXdest + nDestWidth - 1;
#ifdef INVERT
  point[0].y = hdcHeight - nYdest - nDestHeight;
  point[1].y = hdcHeight - nYdest - 1;
#else
  point[0].y = nYdest;
  point[1].y = nYdest + nDestHeight - 1;
#endif

  //target rectangle is inclusive-inclusive
  point[2].x = nXsrc;
  point[3].x = nXsrc + nSrcWidth;
#ifdef INVERT
  point[2].y = pOS2bmp->cy - nYsrc - nSrcHeight;
  point[3].y = pOS2bmp->cy - nYsrc;
#else
  point[2].y = nYsrc;
  point[3].y = nYsrc + nSrcHeight;
#endif

#ifdef INVERT
  oldyinversion = GpiQueryYInversion(hps);
  if(oldyinversion != 0) {
        POINT viewpt, winpt;

        GetViewportOrgEx(hps, &viewpt);
        GetWindowOrgEx(hps, &winpt);

        dprintf(("Viewport origin (%d,%d)", viewpt.x, viewpt.y));
        dprintf(("Windows  origin (%d,%d)", winpt.x, winpt.y));
        
        /* By resetting y inversion to 0, we must take the new windows
         * origin into account. The default matrix set up for the origin
         * depends on y inversion. Therefor we must add the y origin value,
         * multiplied by two, to the top & bottom coordinates 
         */
        point[0].y -= winpt.y*2;
        point[1].y -= winpt.y*2;

        /* By resetting y inversion to 0, we must take the new viewport
         * origin into account. The default matrix set up for the origin
         * depends on y inversion. Therefor we must subtract the y origin value,
         * multiplied by two, from the top & bottom coordinates 
         */
        point[0].y -= viewpt.y*2;
        point[1].y -= viewpt.y*2;

        GpiEnableYInversion(hps, 0);
        fRestoryYInversion = TRUE;
  }
#else
  dprintf(("Sync destination dibsection: hdc y inversion = %d", GpiQueryYInversion(hdc)));
#endif

  dprintf(("DIBSection::BitBlt (%d,%d)(%d,%d) from (%d,%d)(%d,%d) dim (%d,%d)(%d,%d)", point[0].x, point[0].y,
           point[1].x, point[1].y, point[2].x, point[2].y, point[3].x, point[3].y,
           nDestWidth, nDestHeight, nSrcWidth, nSrcHeight));

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
#ifndef INVERT
  if(!(fFlip & FLIP_VERT)) {
#else
  if(fFlip & FLIP_VERT) {
#endif
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

  //Translate ROP
  Rop = Rop >> 16;

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
            pRGB555to565((WORD *)bmpBitsDblBuffer, (WORD *)bitmapBits, iLength/sizeof(WORD));
        }
        else
        {
            dprintf(("GDI32: DIBSect::BitBlt: WARNING! zero-length bitmap! %08xh", pOS2bmp));
        }
        rc = GpiDrawBits(hps, bmpBitsDblBuffer, pOS2bmp, 4, &point[0], Rop, os2mode);
  }
  else {
	rc = GpiDrawBits(hps, bitmapBits, pOS2bmp, 4, &point[0], Rop, os2mode);
  }
  if(rc == GPI_OK) {
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

  dprintf(("Sync destination dibsection %x (%x) (%d,%d) flip %d", hBitmap, hdc, nYdest, nDestHeight, fFlip));

  BITMAPINFO2 *tmphdr = (BITMAPINFO2 *)malloc(os2bmphdrsize);
  memcpy(tmphdr, pOS2bmp, os2bmphdrsize);

#ifdef INVERT
  int oldyinversion = 0;
  if(orgYInversion == TRUE) {
      oldyinversion = GpiQueryYInversion(hdc);
      dprintf(("Sync destination dibsection: hdc y inversion = %d", oldyinversion));
      if(oldyinversion != 0) {
#ifdef DEBUG
          POINT point;
          GetViewportOrgEx(hdc, &point);
          dprintf(("Viewport origin (%d,%d)", point.x, point.y));
#endif
          GpiEnableYInversion(hdc, 0);
      }
  }
#else
  dprintf(("Sync destination dibsection: hdc y inversion = %d", GpiQueryYInversion(hdc)));
#endif

#ifndef INVERT
  if(!(fFlip & FLIP_VERT)) {
#else
  if(fFlip & FLIP_VERT) {
#endif
        //origin is top left, so no y conversion is necessary
        destBuf = bmpBitsDblBuffer + nYdest*dibinfo.dsBm.bmWidthBytes;

        //SvL: cbImage can be too small for compressed images; GpiQueryBitmapBits
        //     will fail in that case (CoolEdit 2000). Perhaps because the returned
        //     compressed image is larger than the original.
        //     Use uncompressed size instead
        //     NOTE: The correct size will be returned by GpiQueryBitmapBits
        tmphdr->cbImage = dibinfo.dsBm.bmHeight*dibinfo.dsBm.bmWidthBytes;

#ifdef INVERT
        int dest = dibinfo.dsBm.bmHeight - nYdest - nDestHeight;
#else
        int dest = nYdest;
#endif
        rc = GpiQueryBitmapBits(hdc, dest, nDestHeight, destBuf,
                                tmphdr);
        if(rc == GPI_ALTERROR) {
            dprintf(("ERROR: GpiQueryBitmapBits failed with %x", WinGetLastError(0)));
        }

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
        //SvL: cbImage can be too small for compressed images; GpiQueryBitmapBits
        //     will fail in that case (CoolEdit 2000). Perhaps because the returned
        //     compressed image is larger than the original.
        //     Use uncompressed size instead
        //     NOTE: The correct size will be returned by GpiQueryBitmapBits
        tmphdr->cbImage = dibinfo.dsBm.bmHeight*dibinfo.dsBm.bmWidthBytes;

        //origin is bottom left, nYdest is based on top left coordinate system
#ifdef INVERT
        int dest = dibinfo.dsBm.bmHeight - nYdest - nDestHeight;
#else
        int dest = nYdest;
#endif

        destBuf = GetDIBObject() + dest*dibinfo.dsBm.bmWidthBytes;

        rc = GpiQueryBitmapBits(hdc, dest, nDestHeight, destBuf,
                                tmphdr);
        if(rc == GPI_ALTERROR) {
            dprintf(("ERROR: GpiQueryBitmapBits failed with %x", WinGetLastError(0)));
        }
#ifdef DEBUG_PALETTE
        if(rc != GPI_ALTERROR && tmphdr->cBitCount <= 8) {
            for(int i=0;i<(1<<tmphdr->cBitCount);i++)
            {
                dprintf2(("Index %d : 0x%08X\n",i, *((ULONG*)(&tmphdr->argbColor[i])) ));
            }
        }
#endif
  }
  memcpy(pOS2bmp, tmphdr, os2bmphdrsize);

  if(dibinfo.dsBitfields[1] == 0x3E0) {//RGB 555?
      dprintf(("DIBSection::sync: convert RGB 565 to RGB 555"));

      destBuf = GetDIBObject() + nYdest*dibinfo.dsBm.bmWidthBytes;

      pRGB565to555((WORD *)destBuf, (WORD *)destBuf, (nDestHeight*dibinfo.dsBm.bmWidthBytes)/sizeof(WORD));
  }

  free(tmphdr);
  if(rc != nDestHeight) {
      dprintf(("!WARNING!: GpiQueryBitmapBits returned %d instead of %d scanlines", rc, nDestHeight));
  }

#ifdef INVERT
  if(oldyinversion) GpiEnableYInversion(hdc, oldyinversion);
#endif

}
//******************************************************************************
//******************************************************************************
void DIBSection::flush(HDC hdc, DWORD nYdest, DWORD nDestHeight, BOOL orgYInversion)
{
 APIRET rc;
 char  *destBuf;

  dprintf(("flush destination dibsection %x (%x) (%d,%d) flip %d", hBitmap, hdc, nYdest, nDestHeight, fFlip));

#ifdef INVERT
  int oldyinversion = 0;
  if(orgYInversion == TRUE) {
      oldyinversion = GpiQueryYInversion(hdc);
      dprintf(("Flush destination dibsection: hdc y inversion = %d", oldyinversion));
      if(oldyinversion != 0) {
#ifdef DEBUG
          POINT point;
          GetViewportOrgEx(hdc, &point);
          dprintf(("Viewport origin (%d,%d)", point.x, point.y));
#endif
          GpiEnableYInversion(hdc, 0);
      }
  }
#else
  dprintf(("flush destination dibsection: hdc y inversion = %d", GpiQueryYInversion(hdc)));
#endif

#ifndef INVERT
  if(!(fFlip & FLIP_VERT)) {
#else
  if(fFlip & FLIP_VERT) {
#endif
        //origin is top left, so no y conversion is necessary
        destBuf = bmpBitsDblBuffer + nYdest*dibinfo.dsBm.bmWidthBytes;

        //SvL: cbImage can be too small for compressed images; GpiQueryBitmapBits
        //     will fail in that case (CoolEdit 2000). Perhaps because the returned
        //     compressed image is larger than the original.
        //     Use uncompressed size instead
        //     NOTE: The correct size will be returned by GpiQueryBitmapBits
        pOS2bmp->cbImage = dibinfo.dsBm.bmHeight*dibinfo.dsBm.bmWidthBytes;

        //manually reverse bitmap data
        char *src = GetDIBObject() + (nYdest+nDestHeight-1)*dibinfo.dsBm.bmWidthBytes;
        char *dst = destBuf;
        for(int i=0;i<nDestHeight;i++) {
            memcpy(dst, src, dibinfo.dsBm.bmWidthBytes);
            dst += dibinfo.dsBm.bmWidthBytes;
            src -= dibinfo.dsBm.bmWidthBytes;
        }

        if(dibinfo.dsBitfields[1] == 0x3E0) {//RGB 555?
            dprintf(("DIBSection::flush: convert RGB 555 to RGB 565"));

            pRGB555to565((WORD *)destBuf, (WORD *)destBuf, (nDestHeight*dibinfo.dsBm.bmWidthBytes)/sizeof(WORD));
        }

#ifdef INVERT
        int dest = dibinfo.dsBm.bmHeight - nYdest - nDestHeight;
#else
        int dest = nYdest;
#endif
        rc = GpiSetBitmapBits(hdc, dest, nDestHeight, destBuf,
                              pOS2bmp);
        if(rc == GPI_ALTERROR) {
            dprintf(("ERROR: GpiQueryBitmapBits failed with %x", WinGetLastError(0)));
        }

  }
  else {
        //SvL: cbImage can be too small for compressed images; GpiQueryBitmapBits
        //     will fail in that case (CoolEdit 2000). Perhaps because the returned
        //     compressed image is larger than the original.
        //     Use uncompressed size instead
        //     NOTE: The correct size will be returned by GpiQueryBitmapBits
        pOS2bmp->cbImage = dibinfo.dsBm.bmHeight*dibinfo.dsBm.bmWidthBytes;

        //origin is bottom left, nYdest is based on top left coordinate system
#ifdef INVERT
        int dest = dibinfo.dsBm.bmHeight - nYdest - nDestHeight;
#else
        int dest = nYdest;
#endif

        destBuf = GetDIBObject() + dest*dibinfo.dsBm.bmWidthBytes;

        if(dibinfo.dsBitfields[1] == 0x3E0) {//RGB 555?
            dprintf(("DIBSection::flush: convert RGB 555 to RGB 565"));

            pRGB555to565((WORD *)bmpBitsDblBuffer, (WORD *)destBuf, (nDestHeight*dibinfo.dsBm.bmWidthBytes)/sizeof(WORD));
            destBuf = bmpBitsDblBuffer;
        }

        rc = GpiSetBitmapBits(hdc, dest, nDestHeight, destBuf,
                              pOS2bmp);
        if(rc == GPI_ALTERROR) {
            dprintf(("ERROR: GpiQueryBitmapBits failed with %x", WinGetLastError(0)));
        }
#ifdef DEBUG_PALETTE
        if(rc != GPI_ALTERROR && pOS2bmp->cBitCount <= 8) {
            for(int i=0;i<(1<<pOS2bmp->cBitCount);i++)
            {
                dprintf2(("Index %d : 0x%08X\n",i, *((ULONG*)(&pOS2bmp->argbColor[i])) ));
            }
        }
#endif
  }
  if(rc != nDestHeight) {
      dprintf(("!WARNING!: GpiQueryBitmapBits returned %d instead of %d scanlines", rc, nDestHeight));
  }

#ifdef INVERT
  if(oldyinversion) GpiEnableYInversion(hdc, oldyinversion);
#endif

}
//******************************************************************************
//Mark the DIB section as invalid; a subsequent read or write access must
//cause a pagefault
//******************************************************************************
void DIBSection::setInvalid()
{
    if(!fInvalid) {
        dprintf(("DIBSection::setInvalid %x (%x)", GetBitmapHandle(), GetDIBObject()));
        APIRET rc = DosSetMem(bmpBits, dwSize, PAG_DECOMMIT);
        if(rc) {
            dprintf(("DIBSection::setInvalid: DosSetMem failed with %d!!", rc));
            DebugInt3();
        }
        fInvalid = TRUE;
    }
}
//******************************************************************************
//******************************************************************************
void DIBSection::flush()
{ 
    if(pOS2bmp == NULL) {
        DebugInt3();
        return;
    }
    if(hdc == 0) {
         HBITMAP hOldBmp;
         HDC hdcFlush = CreateCompatibleDC(0);

         hOldBmp = SelectObject(hdcFlush, hBitmap);
         flush(hdcFlush, 0, pOS2bmp->cy); 

         SelectObject(hdcFlush, hOldBmp);
         DeleteDC(hdcFlush);
    }
    else flush(hdc, 0, pOS2bmp->cy); 

    APIRET rc = DosSetMem(bmpBits, dwSize, PAG_READ);
    if(rc) {
        dprintf(("DIBSection::flush: DosSetMem failed with %d!!", rc));
        DebugInt3();
    }
    fDirty = FALSE;
}
//******************************************************************************
//******************************************************************************
void DIBSection::sync() 
{ 
    if(pOS2bmp == NULL) {
        DebugInt3();
        return;
    }
    APIRET rc = DosSetMem(bmpBits, dwSize, PAG_COMMIT|PAG_READ|PAG_WRITE);
    if(rc) {
        //might already be committed
        rc = DosSetMem(bmpBits, dwSize, PAG_READ|PAG_WRITE);
        if(rc) {
            dprintf(("DIBSection::sync: DosSetMem failed with %d!!", rc));
            DebugInt3();
        }
    }
    if(hdc == 0) {
         HBITMAP hOldBmp;
         HDC hdcFlush = CreateCompatibleDC(0);

         hOldBmp = SelectObject(hdcFlush, hBitmap);
         sync(hdcFlush, 0, pOS2bmp->cy); 

         SelectObject(hdcFlush, hOldBmp);
         DeleteDC(hdcFlush);
    }
    else sync(hdc, 0, pOS2bmp->cy); 
   
    fInvalid = FALSE;

    //Set bitmap memory to readonly again to detect updates
    rc = DosSetMem(bmpBits, dwSize, PAG_READ);
    if(rc) {
        dprintf(("DosSetMem failed with %d!!", rc));
        DebugInt3();
    }
}
//******************************************************************************
//******************************************************************************
void DIBSection::syncAll()
{
  if (!section)
      return;

  lock();
  DIBSection *dsect = section;

  do
  {
      if(dsect->isDirty() && dsect->isInvalid()) {
          DebugInt3();
      }
      if(dsect->isInvalid())
      {
          dsect->sync();
      }
      dsect = dsect->next;
  }
  while(dsect);

  unlock();

  return;
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
  dprintf(("SelectDIBObject %x into %x hwndParent = %x", hBitmap, hdc, hwndParent));
}
//******************************************************************************
//******************************************************************************
DIBSection *DIBSection::findObj(HBITMAP hBitmap)
{
  // PH 2001-08-18 shortcut for performance optimization
  if (!section)
      return NULL;

  lock();
  DIBSection *dsect = section;

  do
  {
    if(dsect->hBitmap == hBitmap)
    {
        unlock();
        return(dsect);
    }
    dsect = dsect->next;
  }
  while(dsect);

  unlock();
  return(NULL);
}
//******************************************************************************
//A bitmap can only be selected into one DC, so this works.
//******************************************************************************
DIBSection *DIBSection::findHDC(HDC hdc)
{
  // PH 2001-08-18 shortcut for performance optimization
  if (!section)
      return NULL;
  
  lock();
  DIBSection *dsect = section;
  
  do
  {
        if(dsect->hdc == hdc)
        {
            unlock();
            return(dsect);
        }
        dsect = dsect->next;
  }
  while(dsect);

  unlock();   
  return(NULL);
}
//******************************************************************************
//******************************************************************************
void DIBSection::deleteSection(HBITMAP hBitmap)
{
 DIBSection *dsect = findObj(hBitmap);

  if(dsect)
        delete dsect;
}
//******************************************************************************
//******************************************************************************
int DIBSection::GetDIBSection(int iSize, void *lpBuffer)
{
 DIBSECTION *pDIBSection = (DIBSECTION *)lpBuffer;
 LPBITMAP_W  dsBm        = (LPBITMAP_W)lpBuffer;

  dprintf2(("GetDIBSection %x %d %x", hBitmap, iSize, lpBuffer));
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
void DIBSection::initDIBSection() 
{
    InitializeCriticalSection(&dibcritsect);
}
//******************************************************************************
//******************************************************************************
DIBSection      *DIBSection::section = NULL;
CRITICAL_SECTION DIBSection::dibcritsect;


//******************************************************************************
//******************************************************************************
static BOOL WIN32API DIBExceptionNotify(LPVOID lpBase, ULONG offset, BOOL fWriteAccess, 
                                        DWORD dwSize, DWORD dwUserData)
{
    DIBSection *dsect;
    HBITMAP hBitmap = (HBITMAP)dwUserData;

    dprintf(("DIBExceptionNotify %x %x %d %d %x", lpBase, offset, fWriteAccess, dwSize, dwUserData));

    dsect = DIBSection::findObj(hBitmap);
    if(dsect == NULL) {
        dprintf(("dib section not found!!"));
        DebugInt3();
        return FALSE;
    }
    if(dsect->isInvalid()) 
    {//implies read or write to reserved pages
        //synchronize bitmap memory and bitmap object
        dsect->sync();
    }
    else
    if(!fWriteAccess) {
        ULONG  ulMemFlags = 0;
        ULONG  ulMemCommit = PAG_COMMIT;
        ULONG  ulMemSize  = 0;
        APIRET rc = DosQueryMem(lpBase, &ulMemSize, &ulMemFlags);
        if(rc) {
            dprintf(("DosQueryMem failed with %d!!", rc));
            DebugInt3();
            return FALSE;
        }
        dprintf(("DosQueryMem size %d, flags %x", ulMemSize, ulMemFlags));
        // YD cannot commit memory two times!
        if ((ulMemFlags & PAG_COMMIT) == PAG_COMMIT)
            ulMemCommit = 0;
        rc = DosSetMem(lpBase, dwSize, PAG_READ|ulMemCommit);
        if(rc) {
            dprintf(("DosSetMem failed with %d!!", rc));
            DebugInt3();
            return FALSE;
        }
    }

    if(fWriteAccess) {
        APIRET rc = DosSetMem(lpBase, dwSize, PAG_READ|PAG_WRITE);
        if(rc) {
            dprintf(("DosSetMem failed with %d!!", rc));
            DebugInt3();
            return FALSE;
        }
        dsect->setDirty();
    }


    return TRUE;
}
//******************************************************************************
//******************************************************************************
