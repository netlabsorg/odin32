/* $Id: surfblit.cpp,v 1.3 2003-01-21 11:20:36 sandervl Exp $ */

/*
 * DirectDraw Surface class implementaion
 *
 * Copyright 1999 Markus Montkowski
 * Copyright 2000 Michal Necasek
 * Copyright 1998-2001 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define _OS2WIN_H
#define FAR

#include <odin.h>
#include <winbase.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                         \
                  ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                  ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#include <fourcc.h>

#define CINTERFACE
#include "ddraw2d.h"
#include "clipper.h"
#include "palette.h"
#include "surface.h"
#include "surfacehlp.h"
#include "os2util.h"
#include "rectangle.h"

#include <misc.h>
#include "asmutil.h"
#include "bltFunc.h"
#include "colorconv.h"
#include "fillfunc.h"
#include <winerror.h>
#include <os2win.h>
#include <cpuhlp.h>
#include "asmutil.h"
#include "wndproc.h"
#include "divewrap.h"

#ifndef __WATCOMC__
  #include <builtin.h>
#endif



//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfBlt(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE2 lpDDSrcSurface,
        LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
 return SurfBlt4( This,
                  lpDestRect,
                  (LPDIRECTDRAWSURFACE4)lpDDSrcSurface,
                  lpSrcRect,
                  dwFlags,
                  lpDDBltFx);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfBlt3(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE3 lpDDSrcSurface,
        LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
 return SurfBlt4( This,
                  lpDestRect,
                  (LPDIRECTDRAWSURFACE4)lpDDSrcSurface,
                  lpSrcRect,
                  dwFlags,
                  lpDDBltFx);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfBlt4(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
        LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
 OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
 OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)lpDDSrcSurface;

 DDSURFACEDESC2  DestSurfaceDesc, SrcSurfaceDesc;
 DDRectangle    *pIRectDest,*pIRectSrc,*pIRectTest;
 RECT            DestRect, SrcRect;
 BOOL            Found;
 int             i;
 HRESULT         ret = DD_OK;

    dprintf(("DDRAW: SurfBlt4 To Surf %08X, from Surf %08X\n",dest,src));
    if ( (NULL!=lpDestRect)&& (NULL!=lpSrcRect))
      dprintf(("DDRAW: SurfBlt4 to (%d,%d)(%d,%d) at %08X from (%d,%d)(%d,%d) at %08X\n", lpDestRect->left, lpDestRect->top,
               lpDestRect->right, lpDestRect->bottom, dest, lpSrcRect->left, lpSrcRect->top,
               lpSrcRect->right, lpSrcRect->bottom, src));

    _dump_DDBLT(dwFlags);

  if (NULL!=lpDestRect)
  {
    // HACK: RA does pass in negative values we might be better return an error,
    //for now we clip
#define RA_HACK 1

#ifdef RA_HACK
    int top,left,bottom,right;

    top    = lpDestRect->top;
    left   = lpDestRect->left;
    bottom = lpDestRect->bottom;
    right  = lpDestRect->right;

    if(top<0)
    {
      bottom += top;
      top = 0;
    }

    if(top > dest->height)
      return DDERR_INVALIDPARAMS;

    if(bottom<0)
      return DDERR_INVALIDPARAMS;

    if(bottom>dest->height)
      bottom=dest->height;

    if(left<0)
    {
      right += left;
      left = 0;
    }

    if(left>dest->width)
      return DDERR_INVALIDPARAMS;

    if(right<0)
      return DDERR_INVALIDPARAMS;

    if(right>dest->width)
       right = dest->width;
#endif    // RA_HACK

    pIRectDest = new DDRectangle( left, top, right, bottom);
#ifdef RA_HACK
    DestRect.top    = top;
    DestRect.left   = left;
    DestRect.bottom = bottom;
    DestRect.right  = right;
#else
    memcpy(&DestRect,lpDestRect,sizeof(RECT) );
#endif //RA_HACK
  }
  else
  {
    pIRectDest = new DDRectangle( 0, 0, dest->width, dest->height);
    DestRect.top    = 0;
    DestRect.left   = 0;
    DestRect.bottom = dest->height;
    DestRect.right  = dest->width;
  }

  if(dest->fLocked)
  {
    if (NULL==lpDestRect)
    {
      // If anything is locked we can't blit to the complete surface as
      // a part is locked
      Found = TRUE;
    }
    else
    {
      // If the dest Rectangle intersects with any of the locked rectangles
      // we can't blit to it

      Found = FALSE;
      i=0;
      while( (i<DPA_GetPtrCount(dest->DPA_LockedRects))  && !Found)
      {
        pIRectTest = (DDRectangle*) DPA_FastGetPtr(dest->DPA_LockedRects,i);
        Found = pIRectDest->intersects(*pIRectTest);
        i++;
      }

    }
    if (Found)
    {
      delete pIRectDest;
      dprintf(("DDRAW: Blt: Dest Surface partially locked\n"));
      return(DDERR_SURFACEBUSY);
    }
  }
  delete pIRectDest;

  //src == NULL for colorfill
  if(src) 
  {
   if (NULL!=lpSrcRect)
   {
#ifdef RA_HACK
    // Same as for dest rectangle now for src

    int top,left,bottom,right;

    top    = lpSrcRect->top;
    left   = lpSrcRect->left;
    bottom = lpSrcRect->bottom;
    right  = lpSrcRect->right;

    if(top<0)
    {
      bottom += top;
      top = 0;
    }

    if(top > src->height)
      return DDERR_INVALIDPARAMS;

    if(bottom<0)
      return DDERR_INVALIDPARAMS;

    if(bottom>src->height)
      bottom=src->height;

    if(left<0)
    {
      right += left;
      left = 0;
    }

    if(left>src->width)
      return DDERR_INVALIDPARAMS;

    if(right<0)
      return DDERR_INVALIDPARAMS;

    if(right>src->width)
       right = src->width;
#endif    // RA_HACK

    pIRectSrc = new DDRectangle( left, top, right, bottom );
#ifdef RA_HACK
    SrcRect.top    = top;
    SrcRect.left   = left;
    SrcRect.bottom = bottom;
    SrcRect.right  = right;
#else
    memcpy(&SrcRect,lpSrcRect,sizeof(RECT) );
#endif
   }
   else
   {
    pIRectSrc = new DDRectangle( 0, 0, src->width, src->height);
    SrcRect.top    = 0;
    SrcRect.left   = 0;
    SrcRect.bottom = src->height;
    SrcRect.right  = src->width;
   }
  
   if(src->fLocked)
   {
    if (NULL==lpSrcRect)
    {
      // If anything is locked we can't blit from the complete surface as
      // a part is locked
      Found = TRUE;
    }
    else
    {
      // If the src Rectangle intersects with any of the locked rectangles of the
      // source surface we can't blit from it

      Found = FALSE;
      i=0;

      while((i<DPA_GetPtrCount(src->DPA_LockedRects) ) && !Found)
      {
        pIRectTest = (DDRectangle*) DPA_FastGetPtr(src->DPA_LockedRects,i);
        Found = pIRectDest->intersects(*pIRectTest);
        i++;
      }

    }

    if (Found)
    {
      delete pIRectSrc;
      dprintf(("DDRAW: Blt: Src Surface partly locked\n"));

      return(DDERR_SURFACEBUSY);
    }
   }
   delete pIRectSrc;
  } //if(src)

#if 0
  if(dest->diveBufNr == DIVE_BUFFER_SCREEN &&
     !(dwFlags & (DDBLT_COLORFILL|DDBLT_DEPTHFILL|DDBLT_ROP) ) &&
     dest->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount > 8 && 
     src->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount > 8)
  {
      int rc, temp, fChanged = FALSE;
      int destheight = RECT_HEIGHT(&DestRect);
      int destwidth  = RECT_WIDTH(&DestRect);
      int srcheight  = RECT_HEIGHT(&SrcRect);
      int srcwidth   = RECT_WIDTH(&SrcRect);

      //if full surface blit or stretching blit, then use Dive
      if( (src->DDSurfaceDesc.dwHeight == srcheight &&
           src->DDSurfaceDesc.dwWidth  == srcwidth) ||
          (srcwidth != destwidth && srcheight != destheight) )  
      {
          SETUP_BLITTER          sBlt = {0};

          sBlt.ulStructLen       = sizeof(sBlt);
          sBlt.fInvert           = 0;
          sBlt.fccSrcColorFormat = src->DDSurfaceDesc.ddpfPixelFormat.dwFourCC;
          sBlt.ulSrcWidth        = srcwidth;
          sBlt.ulSrcHeight       = srcheight;
          sBlt.ulSrcPosX         = SrcRect.left;
          sBlt.ulSrcPosY         = src->DDSurfaceDesc.dwHeight-SrcRect.bottom;
          sBlt.ulDitherType      = 0;
          sBlt.fccDstColorFormat = dest->DDSurfaceDesc.ddpfPixelFormat.dwFourCC;
          sBlt.ulDstWidth        = destwidth;
          sBlt.ulDstHeight       = destheight;
          sBlt.lDstPosX          = 0;
          sBlt.lDstPosY          = 0;
          sBlt.lScreenPosX       = DestRect.left;
          sBlt.lScreenPosY       = dest->DDSurfaceDesc.dwHeight-DestRect.bottom;
          sBlt.ulNumDstRects     = DIVE_FULLY_VISIBLE;
          sBlt.pVisDstRects      = NULL;

          if(dest->lpClipper && dest->lpClipper->IsClipListChangedInt()) 
          {
              DWORD rgnsize;
              if(ClipGetClipList((IDirectDrawClipper*)dest->lpClipper, NULL, NULL, &rgnsize) == DD_OK) 
              {
                  LPRGNDATA lpRgnData = (LPRGNDATA)alloca(rgnsize);
                  if(lpRgnData == NULL) {
                      DebugInt3();
                      goto dodiveblit;
                  }
                  if(ClipGetClipList((IDirectDrawClipper*)dest->lpClipper, NULL, lpRgnData, &rgnsize) == DD_OK) 
                  {
                      OS2RECTL *pRectl = (OS2RECTL *)&lpRgnData->Buffer;

                      if(sBlt.ulNumDstRects == 0) {
                          dprintf(("empty cliplist, return"));
                          return DD_OK;
                      }
                      dprintf(("visible region"));
                      for(i=0;i<lpRgnData->rdh.nCount;i++) 
                      {
                          //win32 -> os2 coordinates (region data in screen coordinates)
                          temp               = pRectl[i].yTop;
                          pRectl[i].yTop     = src->lpDraw->GetScreenHeight() - pRectl[i].yBottom;
                          pRectl[i].yBottom  = src->lpDraw->GetScreenHeight() - temp;
                          dprintf(("(%d,%d)(%d,%d)", pRectl[i].xLeft, pRectl[i].yBottom, pRectl[i].xRight, pRectl[i].yTop));

                          //clip rectangle must be relative to lScreenPos
                          pRectl[i].xLeft   -= sBlt.lScreenPosX;
                          pRectl[i].xRight  -= sBlt.lScreenPosX;
                          pRectl[i].yTop    -= sBlt.lScreenPosY;
                          pRectl[i].yBottom -= sBlt.lScreenPosY;
                          dprintf(("(%d,%d)(%d,%d)", pRectl[i].xLeft, pRectl[i].yBottom, pRectl[i].xRight, pRectl[i].yTop));
                      }
                      fChanged = TRUE;
                      sBlt.ulNumDstRects = lpRgnData->rdh.nCount;
                      sBlt.pVisDstRects  = (PRECTL)&lpRgnData->Buffer;
                  }
              }
          }
dodiveblit:
#ifdef PERFTEST
          QueryPerformanceCounter(&liStart);
#endif
          if(fChanged || memcmp(&sBlt, &dest->sBlt, sizeof(sBlt)-sizeof(PRECTL)-sizeof(ULONG)))
          {
              dprintf(("Setting up blitter: src  (%d,%d)(%d,%d)", sBlt.ulSrcPosX, sBlt.ulSrcPosY, sBlt.ulSrcWidth, sBlt.ulSrcHeight));
              dprintf(("Setting up blitter: dest (%d,%d)(%d,%d)", sBlt.lScreenPosX, sBlt.lScreenPosY, sBlt.ulDstWidth, sBlt.ulDstHeight));
              rc = DiveSetupBlitter(dest->hDive, &sBlt);
              if(rc) {
                  dprintf(("DiveSetupBlitter returned %d", rc));
                  return(DD_OK);
              }
              sBlt.ulNumDstRects     = DIVE_FULLY_VISIBLE;
              sBlt.pVisDstRects      = NULL;
              memcpy(&dest->sBlt, &sBlt, sizeof(sBlt));
          }
          dprintf(("DiveBlitImage %x %d->%d", dest->hDive, src->diveBufNr, dest->diveBufNr));
          rc = DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);
          if(rc) {
              dprintf(("DiveBlitImage returned %d", rc));
              return(DD_OK);
          }
#ifdef PERFTEST
          QueryPerformanceCounter(&liEnd);
          if(liEnd.HighPart == liStart.HighPart) {
              if(average == 0) {
                    average = (liEnd.LowPart - liStart.LowPart);
              }
              else  average = (average + (liEnd.LowPart - liStart.LowPart))/2;
          }
#endif
          SurfChangeUniquenessValue(dest);
          return DD_OK;
      }
  }
#endif

  //TODO: do we need to check the source for clipping information in case
  //      the app wants to copy from the frame buffer?
  if(dest->lpClipper) 
  {
      DWORD rgnsize;
      if(ClipGetClipList((IDirectDrawClipper*)dest->lpClipper, NULL, NULL, &rgnsize) == DD_OK) 
      {
          LPRGNDATA lpRgnData = (LPRGNDATA)alloca(rgnsize);
          if(lpRgnData == NULL) {
              DebugInt3();
              goto doblit;
          }
          if(ClipGetClipList((IDirectDrawClipper*)dest->lpClipper, NULL, lpRgnData, &rgnsize) == DD_OK) 
          {
              RECT newdest, newsrc;
              LPRECT lpClipRect = (LPRECT)&lpRgnData->Buffer;

#ifdef PERFTEST
              QueryPerformanceCounter(&liStart);
#endif
              for(i=0;i<lpRgnData->rdh.nCount;i++) 
              {
                  dprintf(("Clip list %d (%d,%d)(%d,%d)", i, lpClipRect[i].left, lpClipRect[i].bottom, lpClipRect[i].right, lpClipRect[i].top));

                  if(IntersectRect(&newdest, &DestRect, &lpClipRect[i]) == TRUE) 
                  {
                      //TODO: This is not correct for stretching blits
                      if(lpSrcRect) {
                          newsrc.left   = SrcRect.left + (newdest.left - DestRect.left);
                          newsrc.top    = SrcRect.top + (newdest.top - DestRect.top);
                          newsrc.right  = newsrc.left + RECT_WIDTH(&newdest);
                          newsrc.bottom = newsrc.top + RECT_HEIGHT(&newdest);
                      }
//                      DDSURFACEDESC2 surfdesc = {0};
//                      SurfLock4(dest, &newdest, &surfdesc, 0, 0);
   
                      ret = SurfDoBlt(This, &newdest, lpDDSrcSurface, (lpSrcRect) ? &newsrc : NULL, dwFlags, lpDDBltFx);
                      if(ret != DD_OK) {
                          break;
                      }
//                      SurfUnlock(dest, surfdesc.lpSurface);
                  }
              }
#ifdef PERFTEST
              if(liEnd.HighPart == liStart.HighPart) {
                  QueryPerformanceCounter(&liEnd);
                  if(average == 0) {
                        average = (liEnd.LowPart - liStart.LowPart);
                  }
                  else  average = (average + (liEnd.LowPart - liStart.LowPart))/2;
              }
#endif

              return ret;
          }
      }
  }
doblit:
  return SurfDoBlt(This, &DestRect, lpDDSrcSurface, (lpSrcRect) ? &SrcRect : NULL, dwFlags, lpDDBltFx);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfDoBlt(THIS This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
                           LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
   // We have determine between 3 different blit senarios.
   // 1. Blitting between Divebuffers (Front/Backbuffer and primary surface)
   // 2. Blitting between memory and Divebuffers (Front/Backbuffer and primary surface).
   // 3. Blitting between memory buffers.
   // 1 and 3 are easy. DiveBlitImage or memcpy will do the job for non transparent blits
   // 2 is now also easy as we do colorconverion via Dive after each unlocking of a surface
   // The advantage is that we don't have to call DiveSetupBlitter each time. The Blitter will be
   // setup only when the screen resolution is changed by ddraw. I guess we should see a big performance
   // increase by doing it this way, unless the software blits directly from memory to the primary surface)
   // But even then this could be faster as the SetupBlitter call is timeconsumeing and DIVE does emulate
   // the blit in SW anyway as there is no interface in the driver to blit with HW support from the sysmem.

 OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
 OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)lpDDSrcSurface;

 int                    x, i, BlitWidth, BlitHeight;
 char                  *pBltPos, *pSrcPos;
 DDSURFACEDESC2         DestSurfaceDesc, SrcSurfaceDesc;
 BOOL Found;
 DWORD dwSrcColor, dwDestColor;

  dprintf(("DDRAW: SurfDoBlt To Surf %08X, from Surf %08X\n",dest,src));
  if ( (NULL!=lpDestRect)&& (NULL!=lpSrcRect))
      dprintf(("DDRAW: SurfDoBlt to (%d,%d)(%d,%d) at %08X from (%d,%d)(%d,%d) at %08X\n", lpDestRect->left, lpDestRect->top,
               lpDestRect->right, lpDestRect->bottom, dest, lpSrcRect->left, lpSrcRect->top,
               lpSrcRect->right, lpSrcRect->bottom, src));

  DestSurfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
  SrcSurfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

  // First check the simple first

  dwFlags &= ~(DDBLT_WAIT|DDBLT_ASYNC); // FIXME: can't handle right now

  if(dwFlags & DDBLT_COLORFILL)
  {
    dprintf(("DDRAW: ColorFill\n"));
    if((NULL==lpDDBltFx)||(lpDDBltFx->dwSize!=sizeof(DDBLTFX)) )
      return DDERR_INVALIDPARAMS;

    dest->DoColorFill(lpDestRect,lpDDBltFx->dwFillColor);

    return(DD_OK); // according to the M$ DDK only one flag shall/can be set.
  } // end of colorfill

  if (dwFlags & DDBLT_DEPTHFILL)
  {
    dprintf(("DDRAW: DepthFill\n"));
  #ifdef USE_OPENGL
    GLboolean ztest;
    // Todo support more than one Z-Buffer
    // Clears the screen
    dprintf(("DDRAW: Filling depth buffer with %ld\n", lpbltfx->b.dwFillDepth));
    glClearDepth(lpDDBltFx->b.dwFillDepth / 65535.0); // We suppose a 16 bit Z Buffer
    glGetBooleanv(GL_DEPTH_TEST, &ztest);
    glDepthMask(GL_TRUE); // Enables Z writing to be sure to delete also the Z buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(ztest);

    return (DD_OK);
  #endif // USE_OPENGL
  }

  if(dwFlags & DDBLT_ROP)
  {
    // HEL and we only support the following ROPS
    // SRC_COPY
    // BLACKNESS
    // WHITENESS
    //
    if(lpDDBltFx->dwROP & SRCCOPY)
      dwFlags = 0;  // srccopy is a normal fast blt
    else
    {
      if(lpDDBltFx->dwROP & BLACKNESS)
      {
        if(1==dest->dwBytesPPDive)
        {
          // ToDo: Realy implement code to get the correct index for black in 8 Bitmode
          dest->DoColorFill(lpDestRect, 0 );
        }
        else
          dest->DoColorFill(lpDestRect, 0);
        return DD_OK;
      }

      if(lpDDBltFx->dwROP & WHITENESS)
      {
        if(1==dest->dwBytesPPDive)
        {
          // ToDo: Realy implement code to get the correct index for white in 8 Bitmode
          dest->DoColorFill(lpDestRect, 0xFFFFFFFF );
        }
        else
          dest->DoColorFill(lpDestRect, 0xFFFFFFFF);
        return (DD_OK);
      }

      return DDERR_NORASTEROPHW;
    }
  }

  if(NULL==src)
  {
    dprintf(("DDRAW: Unsupported sourceless FX operation. Flags = 0x%04X\n",dwFlags));

    return DD_OK;
  }

  if( ( (NULL==lpDestRect) && (NULL!=lpSrcRect) ) ||
      ( (NULL==lpSrcRect) && (NULL!=lpDestRect) ) )
  {
    dprintf(("DDRAW: Blitting with scaling\n Not supported.\n"));

    return DDERR_NOSTRETCHHW;
  }

  if( ( RECT_WIDTH(lpDestRect)  != RECT_WIDTH(lpSrcRect) ) ||
      ( RECT_HEIGHT(lpDestRect) != RECT_HEIGHT(lpSrcRect) )
    )
  {
    // Stretching not supported
    dprintf(("DDRAW: No stretched blits\n"));

    return DDERR_NOSTRETCHHW;
  }

  if (dest->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
  {
    dprintf(("DDRAW: Dest is Primary Surface\n"));
    if(src->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
    {
      // special Type 1 : Bliting between parts of the screen

      dprintf(("DDRAW: Src is Primary Surface\n"));

      if( RECT_EQUAL(lpDestRect, lpSrcRect))
        return DD_OK; // rects are the same => no blit needed

      // Todo: might add transparent blits but I don't think they are used here, so later!

      MoveRects( dest->pDiveBuffer,
                 lpDestRect,
                 lpSrcRect,
                 dest->dwBytesPPDive,
                 dest->dwPitchDB);
      // MoveRects in framebuufer if we use colorconversion
      if(dest->pFrameBuffer != dest->pDiveBuffer)
      {
        MoveRects( dest->pFrameBuffer,
                   lpDestRect,
                   lpSrcRect,
                   dest->lpDraw->GetScreenBpp()>>3,
                   dest->dwPitchFB);
      }

      // End of Special Type 1 blitting on the screen
    }
    else
    {
      if( src->diveBufNr>0)
      {
        dprintf(("DDRAW: DIVE Blit of all"));
        if( (NULL==lpSrcRect)&&( NULL== lpDestRect))
        {
          // No Rectangles so use Dive to blit everything
          // ToDo : Implement transparent blitting but that seams more
          //        inportant for partial blits.
          //        If we do this later we could skip this check and don't
          //        use Dive .This keeps our simpler and smaler
          //
          DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);

//SvL: ???? shouldn't we return here?
          SurfChangeUniquenessValue(dest);
          return DD_OK;
        }
      }

      // Everything else we do yourselfs
      // Type 2 Sysmem to Primarysurface ca also be handled by this

      if(!dwFlags)
      {
        dprintf(("DDRAW: Solid Blit\n"));

        dest->BltSolid( dest->pDiveBuffer,
                        dest->pFrameBuffer,
                        lpDestRect->top,
                        lpDestRect->left,
                        dest->dwPitchDB,
                        dest->dwPitchFB,
                        src->pDiveBuffer,
                        src->pFrameBuffer,
                        lpSrcRect->top,
                        lpSrcRect->left,
                        RECT_WIDTH(lpDestRect),
                        RECT_HEIGHT(lpDestRect),
                        src->dwPitchDB,
                        src->dwPitchFB
                        );

      }
      else
      {
        pBltPos = (char*) dest->pDiveBuffer + (lpDestRect->top * dest->dwPitchDB) +
                  (lpDestRect->left * dest->dwBytesPPDive);

        pSrcPos = (char*) src->pDiveBuffer + (lpSrcRect->top * src->dwPitchDB) +
                  (lpSrcRect->left * src->dwBytesPPDive);

        BlitHeight = RECT_HEIGHT(lpDestRect);
        BlitWidth  = RECT_WIDTH(lpDestRect);
        // Transparent Blit
        if( (dwFlags &DDBLT_KEYSRC) || (dwFlags & DDBLT_KEYSRCOVERRIDE) )
        {
          dprintf(("DDRAW: Transparent src blit not done yet for primary!!"));
        }
        else
        {
          dprintf(("DDRAW: Unhandled Flags Destination colorkey ? 0x%04X",dwFlags));
        }
      }
    } // end of handling blitting to primary
  }  // end of target primary surface
  else
  {
    if(0==src->diveBufNr)
    {
      // copy from the screen to a buffer

      if( (NULL==lpDestRect) &&
          (NULL==lpSrcRect) &&
          (dest->diveBufNr>0) )
      {
        // Blitting everything from frontbuffer to a Divebuffer
        // ToDo: Might should add checking for flags here
        DiveBlitImage(dest->hDive, src->diveBufNr, dest->diveBufNr);
      }
      else
      {
        // DIVE => DIVE  or Mem => Dive
        // or a rectangle from streen to a buffer can be handelt in the same way
        pBltPos = (char*) dest->pDiveBuffer + (lpDestRect->top * dest->dwPitchDB) +
                  (lpDestRect->left * dest->dwBytesPPDive);

        pSrcPos = (char*) src->pDiveBuffer + (lpSrcRect->top * src->dwPitchDB) +
                  (lpSrcRect->left * src->dwBytesPPDive);

        BlitHeight = RECT_HEIGHT(lpDestRect);
        BlitWidth  = RECT_WIDTH(lpDestRect);

        // Check for transparent blit
        if(!dwFlags)
        {
          dest->BltSolid( dest->pDiveBuffer,
                          dest->pFrameBuffer,
                          lpDestRect->top,
                          lpDestRect->left,
                          dest->dwPitchDB,
                          dest->dwPitchFB,
                          src->pDiveBuffer,
                          src->pFrameBuffer,
                          lpSrcRect->top,
                          lpSrcRect->left,
                          RECT_WIDTH(lpDestRect),
                          RECT_HEIGHT(lpDestRect),
                          src->dwPitchDB,
                          src->dwPitchFB
                          );
        }
        else
        {
          dprintf(("DDRAW: Transblt not done yet"));
          if(dwFlags & DDBLT_KEYSRC)
          {
            if(!(src->DDSurfaceDesc.dwFlags & DDCKEY_SRCBLT))
            {
            }
          }
          else
          {
            if(dwFlags & DDBLT_KEYSRCOVERRIDE)
            {
            }
            else
            {
            }
          }
        }
      }
    } // end handling source screen
    else
    {
      // DIVE => DIVE  or Mem => Dive can be handelt in the same way

      if( (src->pDiveBuffer == dest->pDiveBuffer) &&
          (intersects(lpDestRect, lpSrcRect) ) )
      {
        // Overlapping rects  on the same surface ?

        // ToDo : Maybe implement all the fancy blit flags here too ? ;)

        MoveRects( dest->pDiveBuffer,
                   lpDestRect,
                   lpSrcRect,
                   dest->dwBytesPPDive,
                   dest->dwPitchDB);

        // MoveRects in framebuufer if we use colorconversion
        if(dest->pFrameBuffer != dest->pDiveBuffer)
        {
          MoveRects( dest->pFrameBuffer,
                     lpDestRect,
                     lpSrcRect,
                     dest->lpDraw->GetScreenBpp()>>3,
                     dest->dwPitchFB);
        }
        return DD_OK;
      }

      // Check for transparent blit
      if(!dwFlags)
      {
        dest->BltSolid( dest->pDiveBuffer,
                        dest->pFrameBuffer,
                        lpDestRect->top,
                        lpDestRect->left,
                        dest->dwPitchDB,
                        dest->dwPitchFB,
                        src->pDiveBuffer,
                        src->pFrameBuffer,
                        lpSrcRect->top,
                        lpSrcRect->left,
                        RECT_WIDTH(lpDestRect),
                        RECT_HEIGHT(lpDestRect),
                        src->dwPitchDB,
                        src->dwPitchFB
                        );
      }
      else
      {
        pBltPos = (char*) dest->pDiveBuffer + (lpDestRect->top * dest->dwPitchDB) +
                  (lpDestRect->left * dest->dwBytesPPDive);

        pSrcPos = (char*) src->pDiveBuffer + (lpSrcRect->top * src->dwPitchDB) +
                  (lpSrcRect->left * src->dwBytesPPDive);

        BlitHeight = RECT_HEIGHT(lpDestRect);
        BlitWidth  = RECT_WIDTH(lpDestRect);
        DWORD dwPitch = dest->dwPitchDB;

        if(dwFlags &DDBLT_ROTATIONANGLE)
        {
          return DDERR_NOROTATIONHW;
        }

        if(dwFlags & DDBLT_DDFX)
        {
          DWORD dwFx;

          dwFlags &= ~DDBLT_DDFX; // remove the handled flag

          if( NULL==lpDDBltFx)
            return DDERR_INVALIDPARAMS;

          dwFx = lpDDBltFx->dwDDFX;

          // Remove unsupported Flags
          dwFx &= ~(DDBLTFX_ARITHSTRETCHY |    // Not streach support
                    DDBLTFX_ZBUFFERBASEDEST |  // All ZBuffer flags are not
                    DDBLTFX_ZBUFFERRANGE |     // implementet in M$ Dx 6
                    DDBLTFX_NOTEARING );       // No sync with VRetrace yet

          if(dwFx & DDBLTFX_ROTATE180)
          {
            // 180 degree turn is a mix of a flip up/down and one left/right
            dwFx |= (DDBLTFX_MIRRORUPDOWN | DDBLTFX_MIRRORLEFTRIGHT);
            dwFx &= ~DDBLTFX_ROTATE180; // remove handled flag
          }
          if(dwFx & DDBLTFX_MIRRORUPDOWN)
          {
            // switching the the direction can be integrated with other flags
            dwPitch = -dwPitch;
            pBltPos = (char*) dest->pDiveBuffer +
                      ((lpDestRect->top +BlitHeight)* dest->dwPitchDB) +
                      (lpDestRect->left * dest->dwBytesPPDive);

            dwFx &= ~DDBLTFX_MIRRORUPDOWN;  // remove handled flag
          }

          if(dwFx & DDBLTFX_MIRRORLEFTRIGHT)
          {
            // 180 degree turn or a LR Mirroring
            // don't support any other dwFlags like transparent at the moment

            switch(dest->dwBytesPPDive)
            {
              case 1:
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    pBltPos[BlitWidth-x] = pSrcPos[x];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 2:
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    ((USHORT*)pBltPos)[BlitWidth-x] = ((USHORT*)pSrcPos)[x];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 3:
                BlitWidth *= 3;
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    pBltPos[BlitWidth-x] = pSrcPos[x-2];
                    x--;
                    pBltPos[BlitWidth-x] = pSrcPos[x];
                    x--;
                    pBltPos[BlitWidth-x] = pSrcPos[x+2];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 4:
                while(BlitHeight--)
                {
                  x = BlitWidth;
                  while(x)
                  {
                    ((DWORD*)pBltPos)[BlitWidth-x] = ((DWORD*)pSrcPos)[x];
                    x--;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
            }  // end switch
            SurfChangeUniquenessValue(dest);
            return DD_OK;
          }

            if(dwFx)
              _dump_DDBLTFX(dwFx);

          // We ignore unhandled flags at the moment
        }

        if( (dwFlags & DDBLT_KEYSRC) |
            (dwFlags & DDBLT_KEYSRCOVERRIDE) )
        {
          if(dwFlags & DDBLT_KEYSRCOVERRIDE)
          {

            if( NULL==lpDDBltFx)
              return DDERR_INVALIDPARAMS;

            dwFlags &= ~DDBLT_KEYSRCOVERRIDE;

            // We work like the HEL and test only the low value
            dwSrcColor = lpDDBltFx->ddckSrcColorkey.dwColorSpaceLowValue;

          }
          else
          {

            dwFlags &= ~DDBLT_KEYSRC;

            // Not sure if that is OK maybe check if one is set ?
            // if(!(src->DDSurfaceDesc.dwFlags & DDCKEY_SRCBLT)) return DDERR_WRONGPARAM;?

            dwSrcColor = src->DDSurfaceDesc.ddckCKSrcBlt.dwColorSpaceLowValue;
          }

          // ToDo : We currently indicate that we don't support
          // DDBLT_KEYDEST but HEL does change that!
          // also add this key in the get/setColorKey functions

          if( (dwFlags & DDBLT_KEYDEST) |
              (dwFlags & DDBLT_KEYDESTOVERRIDE) )
          {
            // Source and dest color keying SLOW!!!
            if(dwFlags & DDBLT_KEYDESTOVERRIDE)
            {
              if( NULL==lpDDBltFx)
                return DDERR_INVALIDPARAMS;

              dwFlags &= ~DDBLT_KEYDESTOVERRIDE;

              // We work like the HEL and test only the low value
              dwDestColor = lpDDBltFx->ddckDestColorkey.dwColorSpaceLowValue;

            }
            else
            {

              dwFlags &= ~DDBLT_KEYDEST;

              // Not sure if that is OK maybe check if one is set ?
              // if(!(Dest->DDSurfaceDesc.dwFlags & DDCKEY_DESTBLT)) return DDERR_WRONGPARAM;?

              dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
            }

            // This will be be slow maybe move to ASM ?
            // using MMX should be much faster
            switch(dest->dwBytesPPDive)
            {
              case 1:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(pSrcPos[x] != (char) dwSrcColor)
                    {
                      if(pBltPos[x] != (char) dwDestColor)
                        pBltPos[x] = pSrcPos[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 2:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(((USHORT*)pSrcPos)[x] != (USHORT) dwSrcColor)
                    {
                      if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                        ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 3:
              {
                char *pSC, *pDC;
                pSC = (char*)&dwSrcColor;
                pDC = (char*)&dwDestColor;
                BlitWidth *=3;

                while(BlitHeight--)
                {
                  x = 0;

                  while(x<BlitWidth)
                  {
                    if( (pSrcPos[x]   != pSC[1]) &&
                        (pSrcPos[x+1] != pSC[2]) &&
                        (pSrcPos[x+2] != pSC[3])
                      )
                    {
                      if( (pBltPos[x]   != pDC[1]) &&
                          (pBltPos[x+1] != pDC[2]) &&
                          (pBltPos[x+2] != pDC[3])
                        )
                        {
                          pBltPos[x] = pSrcPos[x];
                          pBltPos[x+1] = pSrcPos[x+2];
                          pBltPos[x+1] = pSrcPos[x+2];
                        }
                    }
                    x +=3;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              }
              case 4:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(((DWORD*)pSrcPos)[x] != dwSrcColor)
                    {
                      if(((DWORD*)pBltPos)[x] != dwDestColor)
                        ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
            }  // End switch
          }
          else
          {
            // This will be be slow maybe move to ASM ?
            // using MMX should be much faster
            switch(dest->dwBytesPPDive)
            {
              case 1:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(pSrcPos[x] != (char) dwSrcColor)
                    {
                      pBltPos[x] = pSrcPos[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 2:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(((USHORT*)pSrcPos)[x] != (USHORT) dwSrcColor)
                    {
                      ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              case 3:
              {
                char *pSC, *pDC;
                pSC = (char*)&dwSrcColor;
                pDC = (char*)&dwDestColor;
                BlitWidth *=3;

                while(BlitHeight--)
                {
                  x = 0;

                  while(x<BlitWidth)
                  {
                    if( (pSrcPos[x]   != pSC[1]) &&
                        (pSrcPos[x+1] != pSC[2]) &&
                        (pSrcPos[x+2] != pSC[3])
                      )
                    {
                       pBltPos[x] = pSrcPos[x];
                       pBltPos[x+1] = pSrcPos[x+2];
                       pBltPos[x+1] = pSrcPos[x+2];
                    }
                    x +=3;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
              }
              case 4:
                while(BlitHeight--)
                {
                  x = 0;
                  while(x<BlitWidth)
                  {
                    if(((DWORD*)pSrcPos)[x] != dwSrcColor)
                    {
                      ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                    }
                    x++;
                  }
                  pBltPos += dwPitch;
                  pSrcPos += src->dwPitchDB;
                }
                break;
            }  // End switch
            // Only Source colorkey
          }
          SurfChangeUniquenessValue(dest);
          return DD_OK;
        }

        if( (dwFlags & DDBLT_KEYDEST) |
            (dwFlags & DDBLT_KEYDESTOVERRIDE) )
        {
          // Dest color keying SLOW!!!
          if(dwFlags & DDBLT_KEYSRCOVERRIDE)
          {
            if( NULL==lpDDBltFx)
              return DDERR_INVALIDPARAMS;

            dwFlags &= ~DDBLT_KEYSRCOVERRIDE;

            // We work like the HEL and test only the low value
            dwDestColor = lpDDBltFx->ddckDestColorkey.dwColorSpaceLowValue;

          }
          else
          {

            dwFlags &= ~DDBLT_KEYDEST;

            // Not sure if that is OK maybe check if one is set ?
            // if(!(src->DDSurfaceDesc.dwFlags & DDCKEY_DESTBLT)) return DDERR_WRONGPARAM;?

            dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
          }

          // This will be be slow maybe move to ASM ?
          // using MMX should be much faster
          switch(dest->dwBytesPPDive)
          {
            case 1:
              while(BlitHeight--)
              {
                x = 0;
                while(x<BlitWidth)
                {
                  if(pBltPos[x] != (char) dwDestColor)
                    pBltPos[x] = pSrcPos[x];
                  x++;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
            case 2:
              while(BlitHeight--)
              {
                x = 0;
                while(x<BlitWidth)
                {
                  if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                    ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                  x++;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
            case 3:
            {
              char *pSC, *pDC;
              pSC = (char*)&dwSrcColor;
              pDC = (char*)&dwDestColor;
              BlitWidth *=3;

              while(BlitHeight--)
              {
                x = 0;

                while(x<BlitWidth)
                {
                  if( (pBltPos[x]   != pDC[1]) &&
                      (pBltPos[x+1] != pDC[2]) &&
                      (pBltPos[x+2] != pDC[3])
                    )
                  {
                    pBltPos[x] = pSrcPos[x];
                    pBltPos[x+1] = pSrcPos[x+2];
                    pBltPos[x+1] = pSrcPos[x+2];
                  }
                  x +=3;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
            }
            case 4:
              while(BlitHeight--)
              {
                x = 0;
                while(x<BlitWidth)
                {
                  if( ((DWORD*)pBltPos)[x] != dwDestColor)
                    ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                  x++;
                }
                pBltPos += dwPitch;
                pSrcPos += src->dwPitchDB;
              }
              break;
          }  // End switch
        } // End of Dest ColorKey


      }// end  handling dwFlags
    } // End handling source not Framebuffer

  }// end handling destination not framebuffer

  SurfChangeUniquenessValue(dest);
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfBltBatch(THIS, LPDDBLTBATCH, DWORD, DWORD )
{
  dprintf(("DDRAW: SurfBltBatch Not implemented by M$\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfBltFast( THIS This ,
                               DWORD dwX,
                               DWORD dwY,
                               LPDIRECTDRAWSURFACE2 lpDDSrcSurface,
                               LPRECT lpSrcRect,
                               DWORD dwTrans)
{
  dprintf(("DDRAW: SurfBltFast=>"));
  return SurfBltFast4( This,
                       dwX,
                       dwY,
                       (LPDIRECTDRAWSURFACE4) lpDDSrcSurface,
                       lpSrcRect,
                       dwTrans);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfBltFast3(THIS This ,
                               DWORD dwX,
                               DWORD dwY,
                               LPDIRECTDRAWSURFACE3 lpDDSrcSurface,
                               LPRECT lpSrcRect,
                               DWORD dwTrans)
{
  dprintf(("DDRAW: SurfBltFast3=>"));
  return SurfBltFast4( This,
                       dwX,
                       dwY,
                       (LPDIRECTDRAWSURFACE4) lpDDSrcSurface,
                       lpSrcRect,
                       dwTrans);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfBltFast4( THIS This,
                                DWORD dwX,
                                DWORD dwY,
                                LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
                                LPRECT lpSrcRect,
                                DWORD dwTrans)
{
  OS2IDirectDrawSurface *dest = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *src  = (OS2IDirectDrawSurface *)lpDDSrcSurface;
  RECTL SrcRect;
  char *pBltPos, *pSrcPos;
  DWORD dwDestColor, dwSrcColor, BlitWidth, BlitHeight, x;

  dprintf(("DDRAW: SurfBltFast4 %08X at(%d/%d) onto %08X with flags %08X\n",src, dwX,dwY, dest, dwTrans));

  if( (NULL == lpDDSrcSurface) ||
      ((LONG)dwX < 0) || ((LONG)dwY < 0) ||
      (dwX > dest->width) ||
      (dwY > dest->height))
  {
    dprintf(("DDRAW: Invalid Parameters %08X, %d %d", lpDDSrcSurface ,dest->width , dest->height));
    return DDERR_INVALIDPARAMS;
  }

  if (NULL != lpSrcRect)
  {
    memcpy(&SrcRect,lpSrcRect,sizeof(RECTL) );
  }
  else
  {
    SrcRect.top    = 0;
    SrcRect.left   = 0;
    SrcRect.bottom = src->height;
    SrcRect.right  = src->width;
  }

  // Todo: Test for locked src/dest

  pBltPos = (char*) dest->pDiveBuffer + (dwY * dest->dwPitchDB) +
            (dwX * dest->dwBytesPPDive);

  pSrcPos = (char*) src->pDiveBuffer + (SrcRect.top * src->dwPitchDB) +
            (SrcRect.left * src->dwBytesPPDive);

  BlitHeight = SrcRect.bottom - SrcRect.top;
  BlitWidth  = (SrcRect.right - SrcRect.left) * src->dwBytesPPDive;

  // Remove unsupported wait flag
  dwTrans &= ~DDBLTFAST_WAIT;

  if(DDBLTFAST_NOCOLORKEY == dwTrans )
  {
    dprintf(( "Solid Blit, %d bits => %d bytes per line\n",
              (SrcRect.right - SrcRect.left),
              BlitWidth) );
    #ifdef USE_ASM
      BltRec(pBltPos, pSrcPos, BlitWidth, BlitHeight,
             dest->dwPitchDB,
             src->dwPitchDB);
    #else
      // Solid Blit
      while(1)
      {
        memcpy(pBltPos,pSrcPos,BlitWidth);
        pBltPos += dest->dwPitchDB;
        pSrcPos += src->dwPitchDB;
        if(! (--BlitHeight))
          break;
      }
    #endif

  }
  else
  {
    dprintf(("DDRAW: TransBlit\n"));

    if(dwTrans & DDBLTFAST_SRCCOLORKEY)
    {
      dprintf(("DDRAW: Trans SRC\n"));
      // transparent source
      dwSrcColor = src->DDSurfaceDesc.ddckCKSrcBlt.dwColorSpaceLowValue;
      if(dwTrans & DDBLTFAST_DESTCOLORKEY)
      {
        dprintf(("DDRAW: And Dest Colorkey"));
        dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
        // Source and dest colorkeying
        switch(dest->dwBytesPPDive)
        {
          case 1:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(pSrcPos[x] != (char) dwSrcColor)
                {
                  if(pBltPos[x] != (char) dwDestColor)
                    pBltPos[x] = pSrcPos[x];
                }
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 2:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((USHORT*)pSrcPos)[x] != (USHORT) dwSrcColor)
                {
                  if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                    ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                }
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 3:
          {
            char *pSC, *pDC;
            pSC = (char*)&dwSrcColor;
            pDC = (char*)&dwDestColor;
            BlitWidth *=3;

            while(BlitHeight--)
            {
              x = 0;

              while(x<BlitWidth)
              {
                if( (pSrcPos[x]   != pSC[1]) &&
                    (pSrcPos[x+1] != pSC[2]) &&
                    (pSrcPos[x+2] != pSC[3])
                  )
                {
                  if( (pBltPos[x]   != pDC[1]) &&
                      (pBltPos[x+1] != pDC[2]) &&
                      (pBltPos[x+2] != pDC[3])
                    )
                    {
                      pBltPos[x] = pSrcPos[x];
                      pBltPos[x+1] = pSrcPos[x+2];
                      pBltPos[x+1] = pSrcPos[x+2];
                    }
                }
                x +=3;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          }
          case 4:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((DWORD*)pSrcPos)[x] != dwSrcColor)
                {
                  if(((DWORD*)pBltPos)[x] != dwDestColor)
                    ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                }
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
        }  // End switch
      }
      else
      {
        // This MMX detection should be moved into OS2Draw
        // and into the surface constructor a setup for blitting pointers
        dprintf(("DDRAW: Only Src ColorKey"));
        switch(dest->dwBytesPPDive)
        {
          case 1:
            if (CPUHasMMX())
              while (BlitHeight--)
              {
                BlitColorKey8MMX((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            else
              while (BlitHeight--)
              {
                BlitColorKey8((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            break;
          case 2:

            if (CPUHasMMX())
              while(BlitHeight--)
              {
                BlitColorKey16MMX((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            else
              while(BlitHeight--)
              {
                BlitColorKey16((PBYTE)pBltPos,(PBYTE)pSrcPos,dwSrcColor,BlitWidth);
                pBltPos += dest->dwPitchDB;
                pSrcPos += src->dwPitchDB;
              }
            break;
          case 3:
            char *pSC;
            pSC = (char*)&dwSrcColor;
            BlitWidth *=3;

            while(BlitHeight--)
            {
              x = 0;

              while(x<BlitWidth)
              {
                if( (pSrcPos[x]   != pSC[1]) &&
                    (pSrcPos[x+1] != pSC[2]) &&
                    (pSrcPos[x+2] != pSC[3])
                  )
                {
                  pBltPos[x] = pSrcPos[x];
                  pBltPos[x+1] = pSrcPos[x+1];
                  pBltPos[x+1] = pSrcPos[x+2];
                }
                x +=3;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 4:
            break;
        }
      }
    }
    else
    {
      if (dwTrans & DDBLTFAST_DESTCOLORKEY)
      {
        dprintf(("DDRAW: DestColorKey\n"));

        dwDestColor = dest->DDSurfaceDesc.ddckCKDestBlt.dwColorSpaceLowValue;
        switch(dest->dwBytesPPDive)
        {
          case 1:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(pBltPos[x] != (char) dwDestColor)
                  pBltPos[x] = pSrcPos[x];
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 2:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((USHORT*)pBltPos)[x] != (USHORT) dwDestColor)
                  ((USHORT*)pBltPos)[x] = ((USHORT*)pSrcPos)[x];
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          case 3:
          {
            char *pSC, *pDC;
            pSC = (char*)&dwSrcColor;
            pDC = (char*)&dwDestColor;
            BlitWidth *=3;

            while(BlitHeight--)
            {
              x = 0;

              while(x<BlitWidth)
              {
                if( (pBltPos[x]   != pDC[1]) &&
                    (pBltPos[x+1] != pDC[2]) &&
                    (pBltPos[x+2] != pDC[3])
                  )
                {
                  pBltPos[x] = pSrcPos[x];
                  pBltPos[x+1] = pSrcPos[x+2];
                  pBltPos[x+1] = pSrcPos[x+2];
                }
                x +=3;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
          }
          case 4:
            while(BlitHeight--)
            {
              x = 0;
              while(x<BlitWidth)
              {
                if(((DWORD*)pBltPos)[x] != dwDestColor)
                  ((DWORD*)pBltPos)[x] = ((DWORD*)pSrcPos)[x];
                x++;
              }
              pBltPos += dest->dwPitchDB;
              pSrcPos += src->dwPitchDB;
            }
            break;
        }  // End switch
      }
      else
      {
        dprintf(("DDRAW: Unexpected Flags"));
      }
    }
  }

  // if(dest->lpVtbl == dest->Vtbl4)
  //   dest->Vtbl4->ChangeUniquenessValue(dest);

  return DD_OK;
}
//******************************************************************************
//******************************************************************************
