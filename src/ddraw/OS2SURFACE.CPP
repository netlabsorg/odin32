/* $Id: OS2SURFACE.CPP,v 1.3 1999-06-10 17:10:56 phaller Exp $ */

/*
 * DirectDraw Surface class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INIT_GUID
#define INCL_MM_OS2
#define WIN32SDK_NOPOSTWRAPPER

#define BYTE0(a)    (char)(a & 0xFF)
#define BYTE1(a)    (char)((a>>8) & 0xFF)
#define BYTE2(a)    (char)((a>>16) & 0xFF)
#define BYTE3(a)    (char)(a>>24)

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <dive.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "no.h"
#include <w_windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <Win32SDKPostWrapper.h>

#include <fourcc.h>

#include "os2ddraw.h"
#include "os2surface.h"
#include "misc.h"
#include "os2util.h"
#include "asmutil.h"

#ifndef __WATCOMC__
    #include <builtin.h>
#endif


/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDrawSurface2

extern DIVE_CAPS dcaps;
extern FOURCC    fccFormats[100];

//******************************************************************************
//Assumes 8 bits or more per pixel
//******************************************************************************
OS2IDirectDrawSurface::OS2IDirectDrawSurface(OS2IDirectDraw *lpDirectDraw,
                         LPDDSURFACEDESC lpDDSurfaceDesc) :
                           Referenced(0), lastError(DD_OK),
                           diveBufNr(-1), lpClipper(NULL),
                           lpPalette(NULL), lpDraw(NULL),
                           fLocked(FALSE), hdcImage(NULL),
                           hbmImage(NULL), bitmapData(NULL),
                           pFrameBuffer(NULL), attached(NULL),
                           flip(NULL),lpBuffer(NULL), ColorKeyFlags(0)
{
 ULONG rc;

  lpVtbl                        = &Vtbl;
  Vtbl.AddRef                   = SurfAddRef;
  Vtbl.Release                  = SurfRelease;
  Vtbl.QueryInterface           = SurfQueryInterface;
  Vtbl.AddAttachedSurface       = SurfAddAttachedSurface;
  Vtbl.AddOverlayDirtyRect      = SurfAddOverlayDirtyRect;
  Vtbl.Blt                      = SurfBlt;
  Vtbl.BltBatch                 = SurfBltBatch;
  Vtbl.BltFast                  = SurfBltFast;
  Vtbl.DeleteAttachedSurface    = SurfDeleteAttachedSurface;
  Vtbl.EnumAttachedSurfaces     = SurfEnumAttachedSurfaces;
  Vtbl.EnumOverlayZOrders       = SurfEnumOverlayZOrders;
  Vtbl.Flip                     = SurfFlip;
  Vtbl.GetAttachedSurface       = SurfGetAttachedSurface;
  Vtbl.GetBltStatus             = SurfGetBltStatus;
  Vtbl.GetCaps                  = SurfGetCaps;
  Vtbl.GetClipper               = SurfGetClipper;
  Vtbl.GetColorKey              = SurfGetColorKey;
  Vtbl.W32_GetDC                = SurfGetDC;    //KSO Apr 19 1999: side effect of the wrapper...
  Vtbl.GetFlipStatus            = SurfGetFlipStatus;
  Vtbl.GetOverlayPosition       = SurfGetOverlayPosition;
  Vtbl.GetPalette               = SurfGetPalette;
  Vtbl.GetPixelFormat           = SurfGetPixelFormat;
  Vtbl.GetSurfaceDesc           = SurfGetSurfaceDesc;
  Vtbl.Initialize               = SurfInitialize;
  Vtbl.IsLost                   = SurfIsLost;
  Vtbl.Lock                     = SurfLock;
  Vtbl.W32_ReleaseDC            = SurfReleaseDC;//KSO Apr 19 1999: side effect of the wrapper...
  Vtbl.Restore                  = SurfRestore;
  Vtbl.SetClipper               = SurfSetClipper;
  Vtbl.SetColorKey              = SurfSetColorKey;
  Vtbl.SetOverlayPosition       = SurfSetOverlayPosition;
  Vtbl.SetPalette               = SurfSetPalette;
  Vtbl.Unlock                   = SurfUnlock;
  Vtbl.UpdateOverlay            = SurfUpdateOverlay;
  Vtbl.UpdateOverlayDisplay     = SurfUpdateOverlayDisplay;
  Vtbl.UpdateOverlayZOrder      = SurfUpdateOverlayZOrder;
  Vtbl.GetDDInterface           = SurfGetDDInterface;
  Vtbl.PageLock                 = SurfPageLock;
  Vtbl.PageUnlock               = SurfPageUnlock;

  lpDraw                        = lpDirectDraw;
  lpDraw->Vtbl.AddRef((IDirectDraw2*)lpDraw);
  hDive                         = lpDirectDraw->GetDiveInstance();
  surfaceType                   = DDSCAPS_OFFSCREENPLAIN;

  memcpy((char *)&DDSurfaceDesc, (char *)lpDDSurfaceDesc, sizeof(DDSURFACEDESC));

  //TODO: finish up and do better error checking (can't create multiple
  //      primary surfaces etc etc
  if(lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)
        height = lpDDSurfaceDesc->dwHeight;
  if(lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)
        width = lpDDSurfaceDesc->dwWidth;

  if(height == 0)
        height = lpDraw->GetScreenHeight();
  if(width == 0)
        width = lpDraw->GetScreenWidth();
//test
  lpDDSurfaceDesc->dwHeight = height;
  lpDDSurfaceDesc->dwWidth  = width;

  //SvL: TODO: Check if DIVE supports all ddraw fourcc codes
  if(lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT) {
    if(lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
        fccColorFormat = lpDDSurfaceDesc->ddpfPixelFormat.dwFourCC;
    }
    else {
        dprintf(("   Unsupported pixel format, defaulting to LUT8"));
        fccColorFormat = FOURCC_LUT8;
    }
  }
  else  fccColorFormat = FOURCC_LUT8;
  screenXFact    = 1.0;
  screenYFact    = 1.0;
  //TODO: base this on the fourcc value!!
  bpp            = 8;

  if(lpDDSurfaceDesc->dwFlags & DDSD_CAPS) {
    surfaceType = lpDDSurfaceDesc->ddsCaps.dwCaps;
    if(surfaceType & DDSCAPS_PRIMARYSURFACE) {
        dprintf(("   Primary surface!\n"));
        diveBufNr    = DIVE_BUFFER_SCREEN;
        pFrameBuffer = lpDraw->GetFrameBuffer();
        //SvL: Needed for stretching if we're not running in 640x480
        if(lpDraw->IsFullScreen() == TRUE) {
            screenYFact  = (double)dcaps.ulVerticalResolution/(double)height;
            screenXFact  = (double)dcaps.ulHorizontalResolution/(double)width;
        }
        fccColorFormat = FOURCC_SCRN;
        bpp            = dcaps.ulDepth;
    }
    if(!(surfaceType & (DDSCAPS_PRIMARYSURFACE | DDSCAPS_OFFSCREENPLAIN))) {
        lastError = DDERR_INVALIDPARAMS;
    }
  }
  else  surfaceType = 0;

  dprintf(("   fccColorFormat %c%c%c%c", BYTE0(fccColorFormat), BYTE1(fccColorFormat), BYTE2(fccColorFormat), BYTE3(fccColorFormat)));

  if(height && width && diveBufNr == -1) {
    diveBufNr = 0;  //need to set to 0 for new buffer!
    lpBuffer = (PBYTE)OS2AllocMem(width*height*bpp/8);
    if(lpBuffer == NULL) {
        lastError = DDERR_INVALIDPARAMS;
        return;
    }
    dprintf(("   lpBuffer = %X", lpBuffer));
    rc = DiveAllocImageBuffer(hDive, &diveBufNr, fccColorFormat, width, height, width, lpBuffer);
    if(rc != DIVE_SUCCESS) {
        dprintf(("   DiveAllocImageBuffer failed with %d", rc));
        lastError = DDERR_INVALIDPARAMS;    //TODO: better errors
        return;
    }
    else {
        dprintf(("   DiveAllocImageBuffer returned %d", diveBufNr));
    }
  }

  dprintf(("   Buf %X Screen Caps (%d,%d), bitcount %d\n", this, dcaps.ulVerticalResolution, dcaps.ulHorizontalResolution,
       dcaps.ulDepth));

  //todo: checking for flip & complex bits if backbuffer bit present!
  if(lpDDSurfaceDesc->dwFlags & DDSD_BACKBUFFERCOUNT) {
    OS2IDirectDrawSurface *attachedSurface, *tmpSurface = NULL;
    DDSURFACEDESC          surfDescription;

    dprintf(("   Create %d background surface(s)", lpDDSurfaceDesc->dwBackBufferCount));
    memcpy((char *)&surfDescription, (char *)lpDDSurfaceDesc, sizeof(DDSURFACEDESC));
    surfDescription.ddsCaps.dwCaps   &= ~DDSCAPS_PRIMARYSURFACE;
    surfDescription.dwBackBufferCount = 0;
    surfDescription.ddsCaps.dwCaps    = DDSCAPS_BACKBUFFER; //only the first one!

    //SvL: Create background surfaces
    for(int i=0;i<lpDDSurfaceDesc->dwBackBufferCount;i++) {
        attachedSurface = new OS2IDirectDrawSurface(lpDirectDraw,
                                    &surfDescription);
        if(tmpSurface) {
            tmpSurface->attached = attachedSurface;
        }
        else {
            attached = attachedSurface;
            flip     = attached;
        }

        tmpSurface = attachedSurface;
        //SvL: All other back surfaces have the flip bit set
        surfDescription.ddsCaps.dwCaps = DDSCAPS_FLIP;
    }
  }
  memset((char *)&SurfaceCaps, 0, sizeof(DDCAPS));
  SurfaceCaps.ddsCaps.dwCaps = surfaceType;
  SurfaceCaps.dwSize         = sizeof(DDCAPS);
  //TODO: might need to change some flags
  SurfaceCaps.dwCaps         = DDCAPS_COLORKEY | DDCAPS_NOHARDWARE;
  SurfaceCaps.dwCaps2        = DDCAPS2_CERTIFIED; //of course we're certified
  //TODO: might need to change some flags as we might support more in the future
  SurfaceCaps.dwCKeyCaps     = DDCKEYCAPS_SRCBLT;
  SurfaceCaps.dwFXCaps       = 0;
  SurfaceCaps.dwFXAlphaCaps  = 0;
  //TODO
  if(bpp == 8) {
      SurfaceCaps.dwPalCaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
  }
  SurfaceCaps.dwVidMemTotal  = 1024*1024;   //todo
  SurfaceCaps.dwVidMemFree   = SurfaceCaps.dwVidMemTotal - dcaps.ulVerticalResolution*dcaps.ulVerticalResolution*dcaps.ulDepth/8;
  SurfaceCaps.dwNumFourCCCodes   = 1;
  SurfaceCaps.dwAlignBoundarySrc = 4;
  SurfaceCaps.dwAlignSizeDest    = 4;
  SurfaceCaps.dwAlignStrideAlign = 4;

  dprintf(("DirectDrawSurface ctor end!"));
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawSurface::~OS2IDirectDrawSurface()
{
  if(diveBufNr != -1) {
    if(fLocked)
        DiveEndImageBufferAccess(hDive, diveBufNr);
    fLocked = FALSE;
    DiveFreeImageBuffer(hDive, diveBufNr);
    diveBufNr = -1;
  }
  if(lpClipper) {
    lpClipper->Vtbl.Release((IDirectDrawClipper*)lpClipper);
    lpClipper = NULL;
  }
  if(lpPalette) {
    if(surfaceType & DDSCAPS_PRIMARYSURFACE)
        lpPalette->RestorePhysPalette();
    lpPalette->Vtbl.Release((IDirectDrawPalette*)lpPalette);
    lpPalette = NULL;
  }
  if(lpBuffer)
    OS2FreeMem((char *)lpBuffer);
  if(hbmImage)
    DeleteObject((HANDLE)hbmImage);
  if(hdcImage)
    DeleteDC(hdcImage);
  if(bitmapData)
    free(bitmapData);
  lpDraw->Vtbl.Release((IDirectDraw2*)lpDraw);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("OS2IDirectDrawSurface::SurfQueryInterface\n"));
  *ppvObj = NULL;

  if(!IsEqualGUID(riid, IID_IDirectDrawSurface) &&
     !IsEqualGUID(riid, IID_IDirectDrawSurface2))
//&& !IsEqualGUID(riid, IID_IUnknown))
    return E_NOINTERFACE;

  *ppvObj = This;

  SurfAddRef(This);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall SurfAddRef(THIS)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("OS2IDirectDrawSurface::SurfAddRef %d\n", me->Referenced+1));
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall SurfRelease(THIS)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("OS2IDirectDrawSurface::SurfRelease %d\n", me->Referenced-1));
  if(me->Referenced) {
    me->Referenced--;
    if(me->Referenced == 0) {
        delete me;
        return(0);
    }
    else    return me->Referenced;
  }
  else  return(0);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfAddAttachedSurface(THIS_ LPDIRECTDRAWSURFACE2)
{
  dprintf(("SurfAddAttachedSurfacer\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfAddOverlayDirtyRect(THIS_ W32_LPRECT)
{
  dprintf(("SurfAddOverlayDirtyRect\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBlt(THIS_ W32_LPRECT lpDestRect, LPDIRECTDRAWSURFACE2 lpDDSrcSurface,
              W32_LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
 OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
 OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)lpDDSrcSurface;
 SETUP_BLITTER          blit;
 ULONG                  rc;
 RECTL                  cliprect;

  dprintf(("SurfBlt to (%d,%d)(%d,%d) from (%d,%d)(%d,%d)\n", lpDestRect->left, lpDestRect->top,
           lpDestRect->right, lpDestRect->bottom, lpSrcRect->left, lpSrcRect->top,
       lpSrcRect->right, lpSrcRect->bottom));

  blit.ulStructLen = sizeof(blit);
  blit.fInvert     = FALSE;
  blit.fccSrcColorFormat = src->fccColorFormat;
  blit.ulSrcWidth  = lpSrcRect->right  - lpSrcRect->left;
  blit.ulSrcHeight = lpSrcRect->bottom - lpSrcRect->top;
  blit.ulSrcPosX   = lpSrcRect->left;
  blit.ulSrcPosY   = lpSrcRect->top;

  blit.fccDstColorFormat = dest->fccColorFormat;
  blit.ulDstWidth  = lpDestRect->right  - lpDestRect->left;
  blit.ulDstHeight = lpDestRect->bottom - lpDestRect->top;
  blit.lDstPosX    = lpDestRect->left;
  blit.lDstPosY    = dcaps.ulVerticalResolution - lpDestRect->bottom;
  blit.lScreenPosX = 0;
  blit.lScreenPosY = 0;
  blit.ulNumDstRects = 1;
  blit.pVisDstRects  = &cliprect;
  cliprect.top     = 0;
  cliprect.bottom  = dcaps.ulVerticalResolution;
  cliprect.left    = lpDestRect->left;
  cliprect.right   = lpDestRect->right;
#if 0
  rc = DiveSetupBlitter(dest->hDive, &blit);
  if(rc != DIVE_SUCCESS) {
    dprintf(("Error setting up blitter %d\n", rc));
    return(DDERR_GENERIC);
  }
  rc = DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);
  if(rc != DIVE_SUCCESS) {
    dprintf(("Error while blitting %d\n", rc));
    return(DDERR_GENERIC);
  }
#else
  char *srcbuf, *destbuf;
  ULONG srcscanbytes, destscanbytes, i, nrScanLines;
  OS2RECTL rectl;

  if(dest->diveBufNr == DIVE_BUFFER_SCREEN) {
        dprintf(("Dest == DIVE_BUFFER_SCREEN\n"));
        rectl.xLeft    = blit.lDstPosX;
        rectl.yBottom  = blit.lDstPosY;
        rectl.xRight   = blit.lDstPosX + blit.ulSrcWidth;
        rectl.yTop     = blit.lDstPosY + blit.ulSrcHeight;
        destscanbytes  = dcaps.ulHorizontalResolution;
        destbuf        = dest->pFrameBuffer;
        rc = DiveAcquireFrameBuffer(dest->hDive, (PRECTL)&rectl);
        if(rc != DIVE_SUCCESS) {
            dprintf(("frame buffer access error %d\n", rc));
            return(DDERR_INVALIDPARAMS);
        }
  }
  else {
    rc = DiveBeginImageBufferAccess(dest->hDive, dest->diveBufNr, (PBYTE *)&destbuf,
                            &destscanbytes, &nrScanLines);
    if(rc != DIVE_SUCCESS) {
        dprintf(("dest bufffer access error %d\n", rc));
        return(DDERR_INVALIDPARAMS);
    }
  }
  rc = DiveBeginImageBufferAccess(src->hDive, src->diveBufNr, (PBYTE *)&srcbuf,
                      &srcscanbytes, &nrScanLines);
  if(rc != DIVE_SUCCESS) {
    dprintf(("src bufffer access error %d\n", rc));
    return(DDERR_INVALIDPARAMS);
  }
  dprintf(("src %X, dest %X\n", srcbuf, destbuf));

  for(i=0;i<blit.ulSrcHeight;i++) {
 #ifdef DEBUG1
    dprintf(("%d blit (%d,%d) from (%d,%d) len %d\n", i, blit.lDstPosX, destscanbytes*lpDestRect->top + i*destscanbytes,
         blit.ulSrcPosX, srcscanbytes*blit.ulSrcPosY + i*srcscanbytes, blit.ulSrcWidth));
    dprintf(("dest address %X\n", destbuf + blit.lDstPosX + destscanbytes*lpDestRect->top + i*destscanbytes + blit.ulSrcWidth - 1));
 #endif
    memcpy(destbuf + blit.lDstPosX + destscanbytes*lpDestRect->top + i*destscanbytes,
           srcbuf  + blit.ulSrcPosX + srcscanbytes*blit.ulSrcPosY + i*srcscanbytes,
           blit.ulSrcWidth);
  }

  if(dest->diveBufNr == DIVE_BUFFER_SCREEN) {
       DiveDeacquireFrameBuffer(dest->hDive);
  }
  else DiveEndImageBufferAccess(dest->hDive, dest->diveBufNr);

  DiveEndImageBufferAccess(src->hDive, src->diveBufNr);

#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBltBatch(THIS_ LPDDBLTBATCH, DWORD, DWORD )
{
  dprintf(("SurfBltBatch\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfBltFast(THIS_ DWORD dwDestX, DWORD dwDestY,
                  LPDIRECTDRAWSURFACE2 srcSurface,
                  W32_LPRECT lpSrcRect, DWORD dwTrans)
{
 OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)srcSurface;
 OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
 SETUP_BLITTER          blit;
 ULONG                  rc;
 RECTL                  cliprect;

  dprintf(("SurfBltFast (%d,%d) (%d,%d) to (%d,%d) dest %X src %X\n", lpSrcRect->left, lpSrcRect->top, lpSrcRect->right, lpSrcRect->bottom, dwDestX, dwDestY, dest, src));

  blit.ulStructLen = sizeof(blit);
  blit.fInvert     = FALSE;
  blit.fccSrcColorFormat = src->fccColorFormat;
  blit.ulSrcWidth  = lpSrcRect->right  - lpSrcRect->left;
  blit.ulSrcHeight = lpSrcRect->bottom - lpSrcRect->top;
  blit.ulSrcPosX   = lpSrcRect->left;
  blit.ulSrcPosY   = lpSrcRect->top;

  //to prevent inaccuracies when stretching
  if(blit.ulSrcWidth == src->lpDraw->GetScreenWidth()) {
    blit.ulDstWidth  = dcaps.ulHorizontalResolution;
  }
  else  blit.ulDstWidth  = (int)((double)blit.ulSrcWidth*dest->screenXFact);

  //to prevent inaccuracies when stretching
  if(blit.ulSrcHeight == src->lpDraw->GetScreenHeight()) {
    blit.ulDstHeight = dcaps.ulVerticalResolution;
  }
  else  blit.ulDstHeight = (int)((double)blit.ulSrcHeight*dest->screenYFact);

  blit.fccDstColorFormat = dest->fccColorFormat;
  blit.lDstPosX    = (int)((double)dwDestX*dest->screenXFact);
  blit.lDstPosY    = (int)((double)dwDestY*dest->screenXFact);
  blit.lScreenPosX = 0;
  blit.lScreenPosY = 0;

  blit.ulNumDstRects = 1;
  blit.pVisDstRects  = &cliprect;
  cliprect.top       = 0;
  cliprect.bottom    = dcaps.ulVerticalResolution;
  cliprect.left      = 0;
  cliprect.right     = dcaps.ulHorizontalResolution;

  //We can't do anything but simple 1:1 blitting ourselves
  if(src->diveBufNr == DIVE_BUFFER_SCREEN || dest->diveBufNr == DIVE_BUFFER_SCREEN ||
     blit.fccSrcColorFormat != blit.fccDstColorFormat ||
     blit.ulDstWidth != blit.ulSrcWidth || blit.ulDstHeight != blit.ulSrcHeight)
  {
//SvL: Other color key types not supported in dive (dest)
    if(dwTrans & DDBLTFAST_SRCCOLORKEY && src->ColorKeyFlags & DDCKEY_SRCBLT) {
        dprintf(("Set colorkey for blitting"));
        rc = DiveSetTransparentBlitMode(dest->hDive, DIVE_TBM_EXCLUDE_SOURCE_VALUE,
                        src->ColorSpaceLowValue[COLORKEY_SRC],
                        src->ColorSpaceHighValue[COLORKEY_SRC]);
        if(rc != DIVE_SUCCESS) {
            dprintf(("Error setting up colorkey for blitter %d\n", rc));
            return(DDERR_GENERIC);
        }
    }
//  dprintf(("Blit: (%d,%d) (%d,%d) to (%d,%d) (%d,%d)", blit.ulSrcPosX, blit.ulSrcPosY, blit.ulSrcWidth, blit.ulSrcHeight, blit.lDstPosX, blit.lDstPosY, blit.ulDstWidth, blit.ulDstHeight));
    rc = DiveSetupBlitter(dest->hDive, &blit);
    if(rc != DIVE_SUCCESS) {
        dprintf(("Error setting up blitter %d\n", rc));
        return(DDERR_GENERIC);
    }
    rc = DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);
    if(rc != DIVE_SUCCESS) {
        dprintf(("Error while blitting %d\n", rc));
        return(DDERR_GENERIC);
    }
  }
  else { //manual blit
   PBYTE srcbuf, destbuf;
   ULONG i, j, destscanlinesize, srcscanlinesize, blitlinesize;

     srcbuf   = src->lpBuffer  + blit.ulSrcPosX + src->width*blit.ulSrcPosY;
     destbuf  = dest->lpBuffer + blit.lDstPosX  + dest->width*blit.lDstPosY;

     if(blit.ulSrcWidth == src->width && blit.ulSrcWidth == dest->width) {
    //TODO: other colorkey types, ranges and only 8 bits supported now!!
    if(dwTrans & DDBLTFAST_SRCCOLORKEY && src->ColorKeyFlags & DDCKEY_SRCBLT) {
        BlitColorKey8(destbuf, srcbuf, src->ColorSpaceLowValue[COLORKEY_SRC],
                  blit.ulSrcWidth*blit.ulSrcHeight*src->bpp/8);
    }
    else {
        memcpy(destbuf, srcbuf, blit.ulSrcWidth*blit.ulSrcHeight*src->bpp/8);
    }
     }
     else {
        destscanlinesize = dest->width*dest->bpp/8;
        srcscanlinesize  = src->width*src->bpp/8;
    blitlinesize     = blit.ulSrcWidth*src->bpp/8;

    //TODO: other colorkey types, ranges and only 8 bits supported now!!
    if(dwTrans & DDBLTFAST_SRCCOLORKEY && src->ColorKeyFlags & DDCKEY_SRCBLT) {
#if 0
         BYTE colorkey = (BYTE)src->ColorSpaceLowValue[COLORKEY_SRC];
     PBYTE endpos;
        for(i=0;i<blit.ulSrcHeight;i++) {
            endpos = destbuf + blitlinesize;
            while(destbuf < endpos) {
                if(*srcbuf == colorkey) {
                    destbuf++;
                }
                else    *destbuf++ = *srcbuf;
                srcbuf++;
            }
            destbuf += (destscanlinesize-blitlinesize);
            srcbuf  += (srcscanlinesize-blitlinesize);
        }
#else
        for(i=0;i<blit.ulSrcHeight;i++) {
            BlitColorKey8(destbuf+i*destscanlinesize, srcbuf+i*srcscanlinesize,
                      src->ColorSpaceLowValue[COLORKEY_SRC], blitlinesize);
        }
#endif
    }
    else {
        for(i=0;i<blit.ulSrcHeight;i++) {
         memcpy(destbuf + i*destscanlinesize, srcbuf + i*srcscanlinesize,
                blitlinesize);
        }
         }
    }
  }
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfDeleteAttachedSurface(THIS_ DWORD,LPDIRECTDRAWSURFACE2)
{
  dprintf(("SurfDeleteAttachedSurface\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfEnumAttachedSurfaces(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK)
{
  dprintf(("SurfEnumAttachedSurfaces\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfEnumOverlayZOrders(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK)
{
  dprintf(("SurfEnumOverlayZOrders\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfFlip(THIS_ LPDIRECTDRAWSURFACE2 lpDDSurfaceTargetOverride, DWORD dwFlags)
{
 OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
 OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)dest->flip;
 SETUP_BLITTER          blit;
 ULONG                  rc;
 RECTL                  cliprect;

  dprintf(("SurfFlip dest %X, src %X", dest, src));
  if(dest == NULL || src == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  if(lpDDSurfaceTargetOverride) {
    dprintf(("SurfFlip, lpDDSurfaceTargetOverride not supported yet\n"));
    return(DDERR_UNSUPPORTED);
  }
  if(!(dest->surfaceType & DDSCAPS_PRIMARYSURFACE) ||
     src == NULL) {
    return(DDERR_INVALIDOBJECT);
  }

  dprintf(("SurfFlip (%d,%d) (%d,%d) %d %d", src->width, src->height, dest->width, dest->height, src->diveBufNr, dest->diveBufNr));

  blit.ulStructLen = sizeof(blit);
  blit.fInvert     = FALSE;
  blit.fccSrcColorFormat = src->fccColorFormat;
  blit.ulSrcWidth  = src->width;
  blit.ulSrcHeight = src->height;
  blit.ulSrcPosX   = 0;
  blit.ulSrcPosY   = 0;

  //to prevent inaccuracies when stretching
  if(blit.ulSrcWidth == src->lpDraw->GetScreenWidth()) {
    blit.ulDstWidth  = dcaps.ulHorizontalResolution;
  }
  else  blit.ulDstWidth  = (int)((double)blit.ulSrcWidth*dest->screenXFact);

  //to prevent inaccuracies when stretching
  if(blit.ulSrcHeight == src->lpDraw->GetScreenHeight()) {
    blit.ulDstHeight = dcaps.ulVerticalResolution;
  }
  else  blit.ulDstHeight = (int)((double)blit.ulSrcHeight*dest->screenYFact);

  blit.fccDstColorFormat = dest->fccColorFormat;
  blit.lDstPosX    = 0;
  blit.lDstPosY    = 0;
  blit.lScreenPosX = 0;
  blit.lScreenPosY = 0;
  blit.ulNumDstRects = 1;
  blit.pVisDstRects  = &cliprect;
  cliprect.top     = 0;
  cliprect.bottom  = blit.ulDstHeight;
  cliprect.left    = 0;
  cliprect.right   = blit.ulDstWidth;

//  dprintf(("Flip: (%d,%d) (%d,%d) to (%d,%d) (%d,%d)", blit.ulSrcPosX, blit.ulSrcPosY, blit.ulSrcWidth, blit.ulSrcHeight, blit.lDstPosX, blit.lDstPosY, blit.ulDstWidth, blit.ulDstHeight));
  rc = DiveSetupBlitter(dest->hDive, &blit);
  if(rc != DIVE_SUCCESS) {
    dprintf(("Error setting up blitter %d\n", rc));
    return(DDERR_GENERIC);
  }
  rc = DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);
  if(rc != DIVE_SUCCESS) {
    dprintf(("Error while blitting %d\n", rc));
    return(DDERR_GENERIC);
  }

  //find next flip back surface if present
  if(src->attached) {
    dest->flip = src->attached;
  }
  else  dest->flip = dest->attached;    //start with first back buffer
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetAttachedSurface(THIS_ LPDDSCAPS lpddscaps, LPDIRECTDRAWSURFACE2 FAR *lpAttachedSurface)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 OS2IDirectDrawSurface *attached = me->attached;

  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  while(attached) {
    dprintf(("SurfGetAttachedSurface %x %x\n", attached->DDSurfaceDesc.ddsCaps.dwCaps, lpddscaps->dwCaps));
    if(attached->DDSurfaceDesc.ddsCaps.dwCaps == lpddscaps->dwCaps) {
        *lpAttachedSurface = (LPDIRECTDRAWSURFACE2)attached;
        return(DD_OK);
    }
    attached = attached->attached;
  }
  return(DDERR_NOTFOUND);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetBltStatus(THIS_ DWORD)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfGetBltStatus\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetCaps(THIS_ LPDDSCAPS lpCaps)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfGetCaps\n"));
  if(me == NULL || lpCaps == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  lpCaps->dwCaps = me->surfaceType;

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetClipper(THIS_ LPDIRECTDRAWCLIPPER FAR *lplpClipper)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfGetClipper\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  if(me->lpClipper) {
    *lplpClipper = (LPDIRECTDRAWCLIPPER) me->lpClipper;
    return(DD_OK);
  }
  else  return(DDERR_NOCLIPPERATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetColorKey(THIS_ DWORD dwFlags, LPDDCOLORKEY lpColorkey)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfGetColorKey\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }

  if(dwFlags & DDCKEY_DESTBLT) {
    lpColorkey->dwColorSpaceLowValue  = me->ColorSpaceLowValue[COLORKEY_DEST];
    lpColorkey->dwColorSpaceHighValue = me->ColorSpaceHighValue[COLORKEY_DEST];
    return(DD_OK);
  }
  if(dwFlags & DDCKEY_DESTOVERLAY) {
    lpColorkey->dwColorSpaceLowValue  = me->ColorSpaceLowValue[COLORKEY_DESTOVERLAY];
    lpColorkey->dwColorSpaceHighValue = me->ColorSpaceHighValue[COLORKEY_DESTOVERLAY];
    return(DD_OK);
  }
  if(dwFlags & DDCKEY_SRCBLT) {
    lpColorkey->dwColorSpaceLowValue  = me->ColorSpaceLowValue[COLORKEY_SRC];
    lpColorkey->dwColorSpaceHighValue = me->ColorSpaceHighValue[COLORKEY_SRC];
    return(DD_OK);
  }
  if(dwFlags & DDCKEY_SRCOVERLAY) {
    lpColorkey->dwColorSpaceLowValue  = me->ColorSpaceLowValue[COLORKEY_SRCOVERLAY];
    lpColorkey->dwColorSpaceHighValue = me->ColorSpaceHighValue[COLORKEY_SRCOVERLAY];
    return(DD_OK);
  }
  return(DDERR_INVALIDPARAMS);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetDC(THIS_ W32_HDC FAR *hdc)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfGetDC %X\n", me));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }

  if(me->surfaceType & DDSCAPS_PRIMARYSURFACE) {
    dprintf(("Primary surface!\n"));
  }

  if(me->fLocked) {
    return(DDERR_DCALREADYCREATED);
  }
  me->fLocked = TRUE;
  if(me->hdcImage == NULL && me->hbmImage == NULL) {
    me->hdcImage = CreateCompatibleDC(NULL);
    if(me->hdcImage == NULL) {
        dprintf(("Can't create compatible DC!\n"));
        me->fLocked = FALSE;
        return(DDERR_GENERIC);
    }
    dprintf(("CreateBitmap %d %d", me->width, me->height));
    me->hbmImage = (HBITMAP)CreateBitmap(me->width, me->height, 1, me->bpp, NULL); //KSO Apr 19 1999: TODO! Why do I have to case this?
    if(me->hbmImage == NULL) {
        dprintf(("Can't create compatible bitmap!\n"));
        DeleteDC(me->hdcImage);
        me->hdcImage = NULL;
        me->fLocked = FALSE;
        return(DDERR_GENERIC);
    }
    if((me->hgdiOld = SelectObject(me->hdcImage, (HGDIOBJ)me->hbmImage)) == NULL) {
        dprintf(("Can't select bitmap into dc!\n"));
        DeleteDC(me->hdcImage);
        me->hdcImage = NULL;
        DeleteObject((HANDLE)me->hbmImage);
        me->hbmImage = NULL;
        me->fLocked = FALSE;
        return(DDERR_GENERIC);
    }
    me->bitmapData = (char *)malloc(sizeof(BITMAPINFOHEADER) +
                        me->width*me->height*me->bpp/8);
  }
  *hdc = (W32_HDC)me->hdcImage;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetFlipStatus(THIS_ DWORD)
{
  dprintf(("SurfGetFlipStatus\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetOverlayPosition(THIS_ LPLONG, LPLONG )
{
  dprintf(("SurfGetOverlayPosition\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetPalette(THIS_ LPDIRECTDRAWPALETTE FAR *lplpPalette)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfGetPalette\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  if(me->lpPalette) {
    *lplpPalette = (LPDIRECTDRAWPALETTE)me->lpPalette;
    return(DD_OK);
  }
  else  return(DDERR_NOPALETTEATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetPixelFormat(THIS_ LPDDPIXELFORMAT)
{
  dprintf(("SurfGetPixelFormat\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetSurfaceDesc(THIS_ LPDDSURFACEDESC lpSurface)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 ULONG dwFlags;

  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  if(lpSurface == NULL)
    return(DDERR_INVALIDPARAMS);

  dwFlags = lpSurface->dwFlags;
  dprintf(("SurfGetSurfaceDesc %X\n", lpSurface->dwFlags));
  memcpy((char *)lpSurface, (char *)&me->DDSurfaceDesc, sizeof(DDSURFACEDESC));

  lpSurface->dwFlags = dwFlags;
  if(lpSurface->dwFlags & DDSD_HEIGHT) {
    lpSurface->dwHeight = me->height;
  }
  if(lpSurface->dwFlags & DDSD_WIDTH) {
    lpSurface->dwWidth = me->width;
  }
  //TODO: handle rest of flags
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfInitialize(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC)
{
  dprintf(("SurfInitialize\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfIsLost(THIS)
{
  dprintf(("SurfIsLost\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfLock(THIS_ W32_LPRECT lpRect, LPDDSURFACEDESC lpSurfaceDesc,
               DWORD dwFlags, W32_HANDLE hEvent)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 ULONG                  pImageBuffer, nrBytesPerScanLine, nrScanLines, rc;
 OS2RECTL   rectl;
 static int times = 0;

  dprintf(("SurfLock %X %X %d %d\n", me, (int)lpRect, dwFlags, hEvent));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }

  if(me->fLocked) {
    dprintf(("SurfLock: Surface already locked\n"));
    return(DDERR_SURFACEBUSY);
  }
  me->fLocked = TRUE;       //TODO: not really safe
  if(me->diveBufNr == DIVE_BUFFER_SCREEN) {
    if(lpRect) {
        rectl.xLeft    = lpRect->left;
            rectl.yBottom  = lpRect->bottom;
            rectl.xRight   = lpRect->right;
            rectl.yTop     = lpRect->top;
    }
    else {
        rectl.xLeft    = 0;
            rectl.yBottom  = me->lpDraw->GetScreenHeight();
            rectl.xRight   = me->lpDraw->GetScreenWidth();
            rectl.yTop     = 0;
    }
    dprintf(("SurfLock: Screen buffer!\n"));
    rc = DiveAcquireFrameBuffer(me->hDive, (PRECTL)&rectl);
    if(rc != DIVE_SUCCESS) {
        dprintf(("frame buffer access error %d\n", rc));
        me->fLocked = FALSE;
        return(DDERR_INVALIDPARAMS);
    }
    pImageBuffer = (ULONG)me->pFrameBuffer;
  }
  else {
    rc = DiveBeginImageBufferAccess(me->hDive, me->diveBufNr, (PBYTE *)&pImageBuffer,
                            &nrBytesPerScanLine, &nrScanLines);
    if(rc != DIVE_SUCCESS) {
        dprintf(("SurfLock: DiveBeginImageBufferAccess returned %d\n", rc));
        me->fLocked = FALSE;
        return(DDERR_INVALIDPARAMS);
    }
  }

  if(!(dwFlags & DDLOCK_SURFACEMEMORYPTR) && lpRect != NULL) {
    pImageBuffer += lpRect->top*nrBytesPerScanLine + lpRect->left;
    lpSurfaceDesc->lpSurface = (LPVOID)pImageBuffer;
        dprintf(("SurfLock %X (x,y) = (%d,%d)\n", pImageBuffer, lpRect->top, lpRect->left));
  }
  else {
    dprintf(("SurfLock %X \n", pImageBuffer));
  }
  //copy buffer address in surface structure
  me->DDSurfaceDesc.lpSurface = (LPVOID)pImageBuffer;
  memcpy((char *)lpSurfaceDesc, (char *)&me->DDSurfaceDesc, sizeof(DDSURFACEDESC));

  lpSurfaceDesc->dwFlags |= DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
  lpSurfaceDesc->dwHeight = me->height;
  lpSurfaceDesc->dwWidth  = me->width;
  lpSurfaceDesc->lPitch   = me->width*me->bpp/8;
  memset((char *)&lpSurfaceDesc->ddpfPixelFormat, 0, sizeof(DDPIXELFORMAT));
  lpSurfaceDesc->ddpfPixelFormat.dwFourCC = me->fccColorFormat;
  lpSurfaceDesc->ddpfPixelFormat.dwSize   = sizeof(DDPIXELFORMAT);
  lpSurfaceDesc->ddpfPixelFormat.dwFlags  = DDPF_FOURCC | DDPF_PALETTEINDEXED8 | DDPF_RGB;
  lpSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 8;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfReleaseDC(THIS_ W32_HDC hdc)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 ULONG                  pImageBuffer, nrBytesPerScanLine, nrScanLines, rc;
 int                    i;

  dprintf(("SurfReleaseDC %X\n", me));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }

  if((HDC)hdc != me->hdcImage) {
    dprintf(("hdc != me->hdcImage %d != %d", hdc, me->hdcImage));
    return(DDERR_INVALIDOBJECT);
  }
  rc = GetBitmapBits((HANDLE)me->hbmImage, me->width*me->height*me->bpp/8, me->bitmapData);
  if(rc == 0) {
    dprintf(("GetBitmapBits error"));
    return(DDERR_INVALIDOBJECT);
  }

  rc = DiveBeginImageBufferAccess(me->hDive, me->diveBufNr, (PBYTE *)&pImageBuffer,
                      &nrBytesPerScanLine, &nrScanLines);
  if(rc != DIVE_SUCCESS) {
    dprintf(("DiveBeginImageBufferAccess returned %d", rc));
    me->fLocked = FALSE;
    return(DDERR_GENERIC);
  }
  dprintf(("SurfReleaseDC; copy bitmap to image buffer %d %d", me->width, me->height));

  if(memcmp((char *)pImageBuffer, me->bitmapData, me->height*me->width*me->bpp/8) == 0) {
    dprintf(("identical!!"));
  }
  memcpy((char *)pImageBuffer, me->bitmapData, me->height*me->width*me->bpp/8);

  DiveEndImageBufferAccess(me->hDive, me->diveBufNr);
  me->fLocked = FALSE;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfRestore(THIS)
{
  dprintf(("SurfRestore\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetClipper(THIS_ LPDIRECTDRAWCLIPPER lpClipper)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfSetClipper\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  if(lpClipper == NULL) {//deattach surface
    if(me->lpClipper) {
        me->lpClipper->Vtbl.Release((IDirectDrawClipper*)me->lpClipper);
        me->lpClipper = NULL;
        return(DD_OK);
    }
    else    return(DDERR_NOCLIPPERATTACHED);
  }
  if(lpClipper == (LPDIRECTDRAWCLIPPER)me->lpClipper)
    return(DD_OK);  //already attached
  if(me->lpClipper != NULL) {
    me->lpClipper->Vtbl.Release((IDirectDrawClipper*)me->lpClipper); //attach other surface
    return(DD_OK);
  }
  me->lpClipper = (OS2IDirectDrawClipper *)lpClipper;
  me->lpClipper->Vtbl.AddRef((IDirectDrawClipper*)me->lpClipper);
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetColorKey(THIS_ DWORD dwFlags, LPDDCOLORKEY lpColorkey)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfSetColorKey\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  me->ColorKeyFlags |= dwFlags;
  if(dwFlags & DDCKEY_DESTBLT) {
    me->ColorSpaceLowValue[COLORKEY_DEST]  = lpColorkey->dwColorSpaceLowValue;
    me->ColorSpaceHighValue[COLORKEY_DEST] = lpColorkey->dwColorSpaceHighValue;
    return(DD_OK);
  }
  if(dwFlags & DDCKEY_DESTOVERLAY) {
    me->ColorSpaceLowValue[COLORKEY_DESTOVERLAY]  = lpColorkey->dwColorSpaceLowValue;
    me->ColorSpaceHighValue[COLORKEY_DESTOVERLAY] = lpColorkey->dwColorSpaceHighValue;
    return(DD_OK);
  }
  if(dwFlags & DDCKEY_SRCBLT) {
    me->ColorSpaceLowValue[COLORKEY_SRC]  = lpColorkey->dwColorSpaceLowValue;
    me->ColorSpaceHighValue[COLORKEY_SRC] = lpColorkey->dwColorSpaceHighValue;
    return(DD_OK);
  }
  if(dwFlags & DDCKEY_SRCOVERLAY) {
    me->ColorSpaceLowValue[COLORKEY_SRCOVERLAY]  = lpColorkey->dwColorSpaceLowValue;
    me->ColorSpaceHighValue[COLORKEY_SRCOVERLAY] = lpColorkey->dwColorSpaceHighValue;
    return(DD_OK);
  }
  return(DDERR_INVALIDPARAMS);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetOverlayPosition(THIS_ LONG, LONG )
{
  dprintf(("SurfSetOverlayPosition\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfSetPalette(THIS_ LPDIRECTDRAWPALETTE lpPalette)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfSetPalette %X\n", me));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  if(lpPalette == NULL) {//deattach palette
    if(me->lpPalette) {
        me->lpPalette->Vtbl.Release((IDirectDrawPalette*)me->lpPalette);
        me->lpPalette = NULL;
        return(DD_OK);
    }
    else    return(DDERR_NOCLIPPERATTACHED);
  }
  if(lpPalette == (LPDIRECTDRAWPALETTE)me->lpPalette)
    return(DD_OK);  //already attached
//?????
#if 0
  if(me->lpPalette != NULL) {
    me->lpClipper->Vtbl.Release(me->lpPalette); //attach other palette
    return(DD_OK);
  }
#endif

  me->lpPalette = (OS2IDirectDrawPalette *)lpPalette;
  me->lpPalette->Vtbl.AddRef((IDirectDrawPalette*)me->lpPalette);
  if(me->surfaceType & DDSCAPS_PRIMARYSURFACE)
    me->lpPalette->SetPhysPalette();

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUnlock(THIS_ LPVOID lpSurfaceData)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfUnlock\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  if(me->fLocked == FALSE)
    return(DDERR_NOTLOCKED);

  if(me->diveBufNr == DIVE_BUFFER_SCREEN) {
        DiveDeacquireFrameBuffer(me->hDive);
    dprintf(("SurfUnlock: Frame buffer unlocked\n"));
    me->fLocked = FALSE;
    return(DD_OK);
  }

  if(lpSurfaceData != NULL &&
     ((int)lpSurfaceData < (int)me->DDSurfaceDesc.lpSurface ||
      (int)lpSurfaceData > (int)me->DDSurfaceDesc.lpSurface + me->height*me->width)) {
    dprintf(("SurfUnlock: invalid params\n"));
    return(DDERR_INVALIDPARAMS);
  }
  me->fLocked = FALSE;
  DiveEndImageBufferAccess(me->hDive, me->diveBufNr);

  dprintf(("SurfUnlock ok\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlay(THIS_ W32_LPRECT, LPDIRECTDRAWSURFACE2, W32_LPRECT,DWORD, LPDDOVERLAYFX)
{
  dprintf(("SurfUpdateOverlay\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlayDisplay(THIS_ DWORD)
{
  dprintf(("SurfUpdateOverlayDisplay\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfUpdateOverlayZOrder(THIS_ DWORD, LPDIRECTDRAWSURFACE2)
{
  dprintf(("SurfUpdateOverlayZOrder\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfGetDDInterface(THIS_ LPVOID FAR *lplpDirectDraw)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("SurfGetDDInterface\n"));
  if(me == NULL) {
    return DDERR_INVALIDOBJECT;
  }
  *lplpDirectDraw = (LPVOID FAR *)me->lpDraw;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfPageLock(THIS_ DWORD)
{
  dprintf(("SurfPageLock\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SurfPageUnlock(THIS_ DWORD)
{
  dprintf(("SurfPageUnlock\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
