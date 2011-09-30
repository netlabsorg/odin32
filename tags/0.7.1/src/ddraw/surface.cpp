/* $Id: surface.cpp,v 1.4 2002-12-30 14:05:43 sandervl Exp $ */

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

#ifndef __WATCOMC__
  #include <builtin.h>
#endif

extern IDirectDrawSurface4Vtbl DDrawSurfaceV4Table;
extern IDirectDrawSurface3Vtbl DDrawSurfaceV3Table;
extern IDirectDrawSurface2Vtbl DDrawSurfaceV2Table;

//******************************************************************************
//******************************************************************************
OS2IDirectDrawSurface::OS2IDirectDrawSurface(OS2IDirectDraw *lpDirectDraw,
               LPDDSURFACEDESC2 lpDDSurfaceDesc, BOOL Implicit, BOOL Mainchain) :
                  Referenced(0), lastError(DD_OK),
                 diveBufNr(-1), lpClipper(NULL),
                 lpPalette(NULL), lpDraw(NULL),
                 fLocked(FALSE), hdcImage(NULL),
                 hbmImage(NULL),
                 pFrameBuffer(NULL),Updated(FALSE),
                 fOverlayValid(FALSE),
                 BackBuffer(NULL),FrontBuffer(NULL),
                 pDBreal(NULL),pFBreal(NULL), hwndFullScreen(0)

{
  DWORD rc;
  DDSURFACEDESC2 ComplexSurfaceDesc;
  OS2IDirectDrawSurface *MipMapSurface;

  memset(&sBlt, 0, sizeof(sBlt));

  lpVtbl                     = &Vtbl2;
  lpVtbl2                    = &Vtbl2;
  dwUnknownData              = 0xDEADDEAD;

  //copy DirectDraw V2, V3 & V4 tables
  Vtbl  = DDrawSurfaceV4Table;
  Vtbl3 = DDrawSurfaceV3Table;
  Vtbl2 = DDrawSurfaceV2Table;

  lpDraw                     = lpDirectDraw;
  lpDraw->Vtbl.fnAddRef(lpDraw);

  ImplicitSurface = Implicit;

  hDive                      = lpDirectDraw->GetDiveInstance();
  hDiveCC                    = lpDirectDraw->GetCCDiveInstance();
  surfaceType                = DDSCAPS_OFFSCREENPLAIN;
  memcpy((char *)&DDSurfaceDesc, (char *)lpDDSurfaceDesc, sizeof(DDSURFACEDESC2));

  dprintf(("Pixel format:"));
  _dump_pixelformat(&lpDDSurfaceDesc->ddpfPixelFormat);
  dprintf(("Capabilities"));
  _dump_DDSCAPS(lpDDSurfaceDesc->ddsCaps.dwCaps);
  _dump_DDSCAPS2(lpDDSurfaceDesc->ddsCaps.dwCaps2);

  if(lpDraw->dCaps.ulDepth != 15)
  {
    if(lpDraw->dCaps.ulDepth >= 8)
      dwBytesPPDive = lpDraw->dCaps.ulDepth >> 3;
    else
      dwBytesPPDive = 1; // not sure if this is the case
  }
  else
    dwBytesPPDive = 2; // 2 bytes for 15Bit

  // Setting up Cursors for handling attached Surfaces

  DPA_SurfaceMipMaps = DPA_Create(8);
  if(NULL==DPA_SurfaceMipMaps)
  {
    dprintf(("DDRAW: Internal : Error creating DPA for MipMaps\n"));
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  DPA_SurfaceAttached = DPA_Create(8);
  if(NULL==DPA_SurfaceAttached)
  {
    dprintf(("DDRAW: Internal : Error creating DPA for attached surfaces\n"));
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  DPA_LockedRects = DPA_Create(8);
  if(NULL==DPA_LockedRects)
  {
    dprintf(("DDRAW: Internal : Error creating DPA for Locked Rectangles\n"));
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  DPA_SurfacePrivateData = DPA_Create(8);
  if(NULL==DPA_SurfacePrivateData)
  {
    dprintf(("DDRAW: Internal : Error creating DPA for private surface Data\n"));
    lastError = DDERR_OUTOFMEMORY ;
    return;
  }

  switch(lpDraw->GetScreenBpp())
  {
    case 8:
      switch(lpDraw->dCaps.ulDepth)
      {
        case 8:
          BltSolid  = &BltSolid8to8;
          ColorConv = NULL;
          ColorFill = &Fill8on8;
          break;
        case 16:
          BltSolid  = &BltSolid8to16;
          ColorConv = &Conv8to16;
          ColorFill = &Fill8on16;
          break;
        case 24:
          BltSolid  = &BltSolid8to24;
          ColorConv = &Conv8to24;
          ColorFill = &Fill8on24;
          break;
        case 32:
          BltSolid  = &BltSolid8to32;
          ColorConv = &Conv8to32;
          ColorFill = &Fill8on32;
          break;
        default:
          dprintf(("DDRAW: Unsupported System ColorDepth %d",lpDraw->dCaps.ulDepth));
          BltSolid  = NULL;
          ColorConv = NULL;
          ColorFill = NULL;
          break;
      }
      break;
    case 16:
      switch(lpDraw->dCaps.ulDepth)
      {
        case 8:
          BltSolid = &BltSolid16to8;
          ColorConv = &Conv16to8;
          ColorFill = &Fill16on8;
          break;
        case 16:
          BltSolid  = &BltSolid16to16;
          ColorConv = NULL;
          ColorFill = &Fill16on16;
          break;
        case 24:
          BltSolid  = &BltSolid16to24;
          ColorConv = &Conv16to24;
          ColorFill = &Fill16on24;
          break;
        case 32:
          BltSolid  = &BltSolid16to32;
          ColorConv = &Conv16to32;
          ColorFill = &Fill16on32;
          break;
        default:
          dprintf(("DDRAW: Unsupported System ColorDepth %d",lpDraw->dCaps.ulDepth));
          BltSolid  = NULL;
          ColorConv = NULL;
          ColorFill = NULL;
          break;
      }
      break;
    case 24:
      switch(lpDraw->dCaps.ulDepth)
      {
        case 8:
          BltSolid  = &BltSolid24to8;
          ColorConv = &Conv24to8;
          ColorFill = &Fill24on8;
          break;
        case 16:
          BltSolid  = &BltSolid24to16;
          ColorConv = &Conv24to16;
          ColorFill = &Fill24on16;
          break;
        case 24:
          BltSolid  = &BltSolid24to24;
          ColorConv = NULL;
          ColorFill = &Fill24on24;
          break;
        case 32:
          BltSolid  = &BltSolid24to32;
          ColorConv = &Conv24to32;
          ColorFill = &Fill24on32;
          break;
        default:
          dprintf(("DDRAW: Unsupported System ColorDepth %d",lpDraw->dCaps.ulDepth));
          BltSolid  = NULL;
          ColorConv = NULL;
          ColorFill = NULL;
          break;
      }
      break;
    case 32:
      switch(lpDraw->dCaps.ulDepth)
      {
        case 8:
          BltSolid  = &BltSolid32to8;
          ColorConv = &Conv32to8;
          ColorFill = &Fill32on8;
          break;
        case 16:
          BltSolid  = &BltSolid32to16;
          ColorConv = &Conv32to16;
          ColorFill = &Fill32on16;
          break;
        case 24:
          BltSolid  = &BltSolid32to24;
          ColorConv = &Conv32to24;
          ColorFill = &Fill32on24;
          break;
        case 32:
          BltSolid  = &BltSolid32to32;
          ColorConv = NULL;
          ColorFill = &Fill32on32;
          break;
        default:
          dprintf(("DDRAW: Unsupported System ColorDepth %d",lpDraw->dCaps.ulDepth));
          BltSolid  = NULL;
          ColorConv = NULL;
          ColorFill = NULL;
          break;
      }
      break;
    default:
      dprintf(("DDRAW: Unsupported DX ColorDepth %d",lpDraw->GetScreenBpp()));
      BltSolid  = NULL;
      ColorConv = NULL;
      ColorFill = NULL;
      break;
  }
  // hack set DDSD_CAPS if the value is other than 0
  if( DDSurfaceDesc.ddsCaps.dwCaps)
    DDSurfaceDesc.dwFlags |= DDSD_CAPS;

  if( DDSurfaceDesc.dwFlags & DDSD_CAPS )
  {
#if 0
//TODO: Docs say this is done, but does it really happen in windows?
    // check for invalid flag combinations
    switch(lpDDSurfaceDesc->ddsCaps.dwCaps2 & (DDSCAPS2_OPAQUE|DDSCAPS2_HINTDYNAMIC|DDSCAPS2_HINTSTATIC))
    {
    case 0:
    case DDSCAPS2_OPAQUE:
    case DDSCAPS2_HINTDYNAMIC:
    case DDSCAPS2_HINTSTATIC:
       break;

    default:
       dprintf(("DDRAW: Internal : illegal ddscaps2 flag combination (static/dynamic/opaque)"));
       lastError = DDERR_INVALIDCAPS ;
       return;
    }
#endif

    // First check if we want to create a primary surface while the ddraw object already has one
    surfaceType = DDSurfaceDesc.ddsCaps.dwCaps;

    if( surfaceType & DDSCAPS_PRIMARYSURFACE)
    {
      dprintf(("DDRAW: Primary surface!\n"));
      if( lpDraw->HasPrimarySurface())
      {
        dprintf(("DDRAW: Primary surface already exits!\n"));
        lastError = DDERR_PRIMARYSURFACEALREADYEXISTS;
        return;
      }

      if( (DDSurfaceDesc.dwFlags & DDSD_HEIGHT) ||
          (DDSurfaceDesc.dwFlags & DDSD_WIDTH)  ||
          (DDSurfaceDesc.dwFlags & DDSD_PIXELFORMAT)
        )
      {
        // Dx doc says passing width,height etc. for primary surface in not permitted!!
        dprintf(("DDRAW: Invalid parameters\n\n"));
        lastError = DDERR_INVALIDPARAMS;
        return;
      }

      // Check if OS/2 is running in the requested colormode

      diveBufNr    = DIVE_BUFFER_SCREEN;
      if( lpDraw->dCaps.ulDepth == lpDraw->GetScreenBpp() )
      {
        dprintf(("DDRAW: Direct Screen Access possible\n"));

        // Yes so direct access to framebuffer is possible

        pFrameBuffer = lpDraw->GetFrameBuffer();
        pDiveBuffer  = pFrameBuffer;
        dwPitchDB    = lpDraw->dCaps.ulScanLineBytes;
        dwPitchFB    = dwPitchDB;
        DDSurfaceDesc.lPitch = dwPitchDB;
      }
      else
      {
        // No so we create a virtual framebuffer which the program can access
        // and blit to the real framebuffer on Unlock to do color conversion

        dprintf( ("Need Color conversion %d => %d Bit\n",
                  lpDraw->GetScreenBpp(),
                  lpDraw->dCaps.ulDepth
                ));

        dwPitchFB = (lpDraw->GetScreenWidth() * lpDraw->GetScreenBpp() +7) & ~7;
        DDSurfaceDesc.lPitch = dwPitchFB;

        // 24 byte more to enable alignment and speed up blitting

        pFBreal = (char*)malloc( lpDraw->GetScreenHeight() * dwPitchFB + 24);
        pFrameBuffer = (char*)(((int)pFBreal + 7) & ~7); // align to QWORD

        // DiveBuffer points to real framebuffer
        dwPitchDB    = lpDraw->dCaps.ulScanLineBytes;
        pDiveBuffer = lpDraw->GetFrameBuffer();

      }

      // Update passed in and local Surface description

      dprintf(("DDRAW: Setting up Surface\n"));
      DDSurfaceDesc.dwFlags     |= DDSD_WIDTH | DDSD_HEIGHT |
                                   DDSD_PITCH | DDSD_LPSURFACE |
                                   DDSD_PIXELFORMAT;
      DDSurfaceDesc.dwHeight     = lpDraw->GetScreenHeight();
      DDSurfaceDesc.dwWidth      = lpDraw->GetScreenWidth();
      DDSurfaceDesc.lpSurface    = pFrameBuffer;
      DDSurfaceDesc.ddsCaps.dwCaps    |= DDSCAPS_VISIBLE;
      lpDraw->SetPrimarySurface(TRUE);
      DDSurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
      DDSurfaceDesc.ddpfPixelFormat.dwFourCC = (DWORD) lpDraw->GetScreenFourCC();
      DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = lpDraw->GetScreenBpp();

      switch(DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
      {
        case 4:
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED4 | DDPF_RGB;
          break;
        case 8:
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_FOURCC | DDPF_RGB;
          break;
        case 16:
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
          DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x0000F800;
          DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x000007E0;
          DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x0000001F;
          break;
        case 24:
        case 32:
          DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
          DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
          DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
          DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
          break;
        default:
          // Remove the Pixelformat flag
          DDSurfaceDesc.dwFlags    &= ~DDSD_PIXELFORMAT;
          dprintf(("DDRAW: Unexpected BitDepth : %d\n",lpDraw->GetScreenBpp()));
          break;
      } // end switch

      dprintf(("DDRAW: Surface set up, checking other Caps\n"));

      if( DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_COMPLEX)
      {
        dprintf(("DDRAW: Complex Surface\n"));

        if(DDSurfaceDesc.dwFlags & DDSD_BACKBUFFERCOUNT)
        {
          dprintf(("DDRAW: Backbuffer # = %d\n",DDSurfaceDesc.dwBackBufferCount));
          memset( &ComplexSurfaceDesc,
                  0,
                  sizeof(DDSURFACEDESC2));

          ComplexSurfaceDesc.dwSize  = sizeof(DDSURFACEDESC2);
          ComplexSurfaceDesc.dwFlags = DDSD_CAPS |
                                       DDSD_WIDTH |
                                       DDSD_HEIGHT |
                                       DDSD_PIXELFORMAT;
          ComplexSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;
          ComplexSurfaceDesc.dwHeight = DDSurfaceDesc.dwHeight;
          ComplexSurfaceDesc.dwWidth = DDSurfaceDesc.dwWidth;
          ComplexSurfaceDesc.ddpfPixelFormat.dwFlags = DDSurfaceDesc.ddpfPixelFormat.dwFlags;
          ComplexSurfaceDesc.ddpfPixelFormat.dwRBitMask = DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwGBitMask = DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwBBitMask = DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

          if(DDSurfaceDesc.dwBackBufferCount>1)
          {
            ComplexSurfaceDesc.dwFlags |=DDSD_BACKBUFFERCOUNT;
            ComplexSurfaceDesc.dwBackBufferCount = DDSurfaceDesc.dwBackBufferCount -1;
            ComplexSurfaceDesc.ddsCaps.dwCaps|= DDSCAPS_COMPLEX;
          }

          BackBuffer = new OS2IDirectDrawSurface(lpDraw, &ComplexSurfaceDesc, TRUE, TRUE);
          BackBuffer->Vtbl.fnAddRef((IDirectDrawSurface *)BackBuffer);

          if (BackBuffer->GetLastError()==DD_OK)
          {
            dprintf(("DDRAW: created backbuffer"));
            // Our Primary Buffer is also the frontbuffer of a flipchain
            DDSurfaceDesc.ddsCaps.dwCaps    |= DDSCAPS_FRONTBUFFER | DDSCAPS_FLIP;
            BackBuffer->SetFrontBuffer(this);
          }
          else
          {
            dprintf(("DDRAW: Error creating backbuffer"));
          }
        }
        else
        {
          dprintf(("DDRAW: Unsupported Complex Surface\n"));
          _dump_DDSCAPS(DDSurfaceDesc.dwFlags);

          lastError = DDERR_OUTOFMEMORY;
          return;
        } //endif Backbuffer
      } // endif DDSCAPS_COMPLEX

      width  = DDSurfaceDesc.dwWidth;
      height = DDSurfaceDesc.dwHeight;

      lpDraw->pPrimSurf = this;

      lastError = DD_OK;

#if 0
//NOTE: This isn't working at all (interferes with mouse messages for application
//      window)
      //If in fullscreen mode, create transparent window covering the entire desktop
      if(lpDraw->GetCooperativeLevel() & DDSCL_FULLSCREEN)
      {
           DirectDrawSurface_RegisterClass();
           hwndFullScreen = DirectDrawSurface_CreateWindow(DDSurfaceDesc.dwWidth,
                                                           DDSurfaceDesc.dwHeight,
                                                           lpDraw->GetClientWindow());

           if(hwndFullScreen == 0) {
               DebugInt3();
               lastError = DDERR_OUTOFMEMORY;
           }
           dprintf(("DDRAW: Created fullscreen transparent window %x", hwndFullScreen));
      }
#endif
      return;
    } // endif  DDSCAPS_PRIMARYSURFACE

    //
    // ToDo : Do better calulation of Bitmap Size to support the compressed Bitmaps in Dx6
    //

    if( (DDSurfaceDesc.dwFlags & DDSD_HEIGHT) &&
        (DDSurfaceDesc.dwFlags & DDSD_WIDTH)
      )

    {
      DWORD dwBpp;
      DWORD dwCaps;
      dprintf(( " Requested Size %dx%d\n",
                DDSurfaceDesc.dwWidth,
                DDSurfaceDesc.dwHeight));

      if(DDSurfaceDesc.dwFlags & DDSD_PIXELFORMAT)         // Pixelformat passed in ?
      {
        dprintf(("DDRAW:  Pixelformat requested :"));
        // YES use it
        if(DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_RGB)
        {
          dwBpp = DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
        }
        else
        {
          if(DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
            dwBpp = 8;
          else
          if(DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
            dwBpp = 4;
          else
          if(DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
               dprintf(("DDRAW:  Pixelformat FOURCC %4s", &DDSurfaceDesc.ddpfPixelFormat.dwFourCC));
               switch(DDSurfaceDesc.ddpfPixelFormat.dwFourCC) {
               case FOURCC_RGB3:
                   dwBpp = 24;
                   break;
               case FOURCC_RGB4:
                   dwBpp = 32;
                   break;
               default:
                   dprintf(("DDRAW: Unsupported FOURCC!"));
                   lastError = DDERR_UNSUPPORTEDFORMAT;
                   return;
               }
          }
        }
      }
      else
      {
        dprintf(("DDRAW:  Use Screen Format :"));
        dwBpp = lpDraw->GetScreenBpp(); // No use Screenformat
        DDSurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        DDSurfaceDesc.ddpfPixelFormat.dwFourCC = (DWORD) lpDraw->GetScreenFourCC();
        DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = dwBpp;
        switch(DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
        {
          case 4:
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED4 | DDPF_RGB;
            break;
          case 8:
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_FOURCC | DDPF_RGB;
            break;
          case 16:
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
            DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x0000F800;
            DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x000007E0;
            DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x0000001F;
            break;
          case 24:
          case 32:
            DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
            DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
            DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
            DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
            break;
          default:
            // Remove the Pixelformat flag
            DDSurfaceDesc.dwFlags    &= ~DDSD_PIXELFORMAT;
            dprintf(("DDRAW: Unexpected BitDepth : %d\n",lpDraw->GetScreenBpp()));
            break;
        } // end switch

      }

      dprintf(("DDRAW: %d Bits\n",dwBpp));

      // three possible situaltions
      // 1. User supplied pointer to surface -> use it
      // 2. Delayed allocation of a texture -> don't alloc
      // 3. Normal allocation
      // After this check for complex flag.

      dwCaps = DDSurfaceDesc.ddsCaps.dwCaps;

      if(DDSurfaceDesc.dwFlags & DDSD_LPSURFACE)
      {
        // 1.

        dprintf(("DDRAW:  Usersupplied Surface\n"));

        if(NULL==DDSurfaceDesc.lpSurface)
        {
          // pointer is NULL! Stupid user ;)
          lastError = DDERR_INVALIDPARAMS;
          return;
        }

        // User allocated the Buffer for the surface so we don't have to

        Updated = TRUE; // Set Flag to indicate User supplied buffer so we don't free it

        // As we allready copied the surface description we are done if the # of colors
        // of the surface is the same then the screen

        pFrameBuffer = (char*)DDSurfaceDesc.lpSurface;
        diveBufNr    = -1;
        dwPitchFB    = DDSurfaceDesc.lPitch;

        if( lpDraw->dCaps.ulDepth  == dwBpp )
        {
          // Yes No Colorconversion is needed so point to the same buffer
          dwPitchDB    = dwPitchFB;
          pDiveBuffer  = pFrameBuffer;
        }
        else
        {
          // No so we must create the Divebuffer to do the color conversion
          // and blit to the real framebuffer on Unlock to do color conversion

          dwPitchDB = (DDSurfaceDesc.dwWidth * dwBytesPPDive +7) & ~7;

          // 24 byte more to enable alignment and speed up blitting

          pDBreal = (char*)malloc( DDSurfaceDesc.dwHeight * dwPitchDB + 24);
          pDiveBuffer = (char*)(((int)pDBreal + 7) & ~7); // align to QWORD

          // Not sure if that is ok but we need this for translation
          // I hope no game uses YUV or such a crap

          DDSurfaceDesc.ddpfPixelFormat.dwFourCC = SupportedFourCCs[dwBpp>>3];
        }

      }
      else
      {

        if(dwCaps & DDSCAPS_ALLOCONLOAD)
        {
          // 2.

          dprintf(("DDRAW:  Alloc on Load Texture?!\n"));

          dwCaps &= ~DDSCAPS_ALLOCONLOAD; // remove flag

          // only allow this flag for textures
          if(!dwCaps & DDSCAPS_TEXTURE)
          {
            lastError = DDERR_INVALIDPARAMS;
            return;
          }

          dwCaps &= ~DDSCAPS_TEXTURE; // remove flag
          pFrameBuffer = NULL;
          pDiveBuffer  = NULL;

          // This surface isn't allocated yet, but when the texture is loaded
          dprintf(("DDRAW: Warning : Delayed memory allocation on request\n"));
          DDSurfaceDesc.lpSurface = NULL;
        }
        else
        {
          // 3.

          dprintf(("DDRAW:  Alloc now!\n"));

          DDSurfaceDesc.dwFlags      = DDSurfaceDesc.dwFlags;

          dwPitchFB = DDSurfaceDesc.dwWidth * (dwBpp<8?1:dwBpp/8);
//SvL:
//align on dword boundary; makes it easier to deal with GetDIBits/SetDIBits (they
//expect dword alignment)
          dwPitchFB = (dwPitchFB + 3) & ~3;  // Align on DWords
////          dwPitchFB = (dwPitchFB +7) & ~7;  // Align on QWords
          DDSurfaceDesc.lPitch = dwPitchFB;
            if(dwBpp<8)
            {
              dprintf(("DDRAW: 1 or 4 Bit Surface encountered may not work !"));
            }


          // 24 byte more to enable alignment and speed up blitting

          pFBreal = (char*)malloc( DDSurfaceDesc.dwHeight * dwPitchFB + 24);

          if(NULL==pFBreal)
          {
            lastError = DDERR_OUTOFMEMORY;
            return;
          }

          pFrameBuffer = (char*)(((int)pFBreal + 7) & ~7); // align to QWORD

          dprintf(("DDRAW:  Framebuffer @ %08X QWAligned @ %08X with a Pitch of %d\n",
                    pFBreal, pFrameBuffer, dwPitchFB));

          DDSurfaceDesc.lpSurface    = pFrameBuffer;

          if( (lpDraw->dCaps.ulDepth ) == dwBpp )
          {
            dprintf(("DDRAW:  No CC_Buffer needed\n"));
            // Yes => No Colorconversion is needed so point to the same buffer
            pDiveBuffer  = pFrameBuffer;
            dwPitchDB = dwPitchFB;
          }
          else
          {
            dwPitchDB = (DDSurfaceDesc.dwWidth * (lpDraw->dCaps.ulDepth/8) +7) & ~7;
            dprintf( ("DDRAW:  Alloc CCBuf with malloc (%dx%d) Pitch %d ",
                      DDSurfaceDesc.dwHeight,
                      DDSurfaceDesc.dwWidth,
                      dwPitchDB));
            // No so we must create the Divebuffer to do the colortranslation
            // and blit to the real framebuffer on Unlock to do color conversion
            pDBreal = (char*)malloc( DDSurfaceDesc.dwHeight *
                                     dwPitchDB + 24);
            pDiveBuffer = (char*)(((int)pDBreal + 7) & ~7); // align to QWORD
            dprintf(( " @ %08X\n", pDiveBuffer));

          }

//          if(Mainchain)
//          {
              diveBufNr = 0;
              rc = DiveAllocImageBuffer( hDive,
                                       &diveBufNr,
                                       lpDraw->dCaps.fccColorEncoding,
                                       DDSurfaceDesc.dwWidth,
                                       DDSurfaceDesc.dwHeight,
                                       dwPitchDB,
                                       (PBYTE)pDiveBuffer);
              dprintf(("DDRAW: DiveAllocImageBuffer %x -> %d (rc=%d)",pDiveBuffer, diveBufNr, rc));
//          }

        }  // end of 3rd case
      } // End of alloc surfaces

      width  = DDSurfaceDesc.dwWidth;
      height = DDSurfaceDesc.dwHeight;

      if( dwCaps & DDSCAPS_COMPLEX)
      {
        // remove the flag
        dwCaps &= ~DDSCAPS_COMPLEX;
        dprintf(("DDRAW: Complex Surface\n"));

        if(DDSurfaceDesc.dwFlags & DDSD_BACKBUFFERCOUNT)
        {
          dprintf(("DDRAW: Backbuffer # = %d\n",DDSurfaceDesc.dwBackBufferCount));

#if 1
          memset( &ComplexSurfaceDesc,
                  0,
                  sizeof(DDSURFACEDESC2));

          ComplexSurfaceDesc.dwSize  = sizeof(DDSURFACEDESC2);
          ComplexSurfaceDesc.dwFlags = DDSD_CAPS |
                                       DDSD_WIDTH |
                                       DDSD_HEIGHT |
                                       DDSD_PIXELFORMAT;
          ComplexSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;
          ComplexSurfaceDesc.dwHeight = DDSurfaceDesc.dwHeight;
          ComplexSurfaceDesc.dwWidth = DDSurfaceDesc.dwWidth;
          ComplexSurfaceDesc.ddpfPixelFormat.dwFlags = DDSurfaceDesc.ddpfPixelFormat.dwFlags;
          ComplexSurfaceDesc.ddpfPixelFormat.dwRBitMask = DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwGBitMask = DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwBBitMask = DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
          ComplexSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

          if(DDSurfaceDesc.dwBackBufferCount>1)
          {
            ComplexSurfaceDesc.dwFlags |=DDSD_BACKBUFFERCOUNT;
            ComplexSurfaceDesc.dwBackBufferCount = DDSurfaceDesc.dwBackBufferCount -1;
            ComplexSurfaceDesc.ddsCaps.dwCaps|= DDSCAPS_COMPLEX;
          }

          BackBuffer = new OS2IDirectDrawSurface(lpDraw, &ComplexSurfaceDesc, TRUE, TRUE);
          BackBuffer->Vtbl.fnAddRef((IDirectDrawSurface *)BackBuffer);

          if (BackBuffer->GetLastError()==DD_OK)
          {
            dprintf(("DDRAW: created backbuffer"));
            DDSurfaceDesc.dwFlags |= DDSCAPS_FLIP;
            BackBuffer->SetFrontBuffer(this);
          }
          else
          {
            dprintf(("DDRAW: Error creating backbuffer"));
          }
#else
          memcpy( &ComplexSurfaceDesc,
                  &DDSurfaceDesc,
                  sizeof(DDSURFACEDESC2));
          ComplexSurfaceDesc.dwFlags        &= ~DDSD_BACKBUFFERCOUNT;
          ComplexSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FLIP;        // set flip
          ComplexSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_BACKBUFFER; // remove backbuffer

          if(ComplexSurfaceDesc.dwBackBufferCount>1)
          {
            ComplexSurfaceDesc.dwBackBufferCount--;
          }
          else
          {
            ComplexSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
            ComplexSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_COMPLEX;
          }
          BackBuffer = new OS2IDirectDrawSurface(lpDraw, &ComplexSurfaceDesc, TRUE, Mainchain);
          BackBuffer->Vtbl.fnAddRef((IDirectDrawSurface *)BackBuffer);

          if (BackBuffer->GetLastError()==DD_OK)
          {
            DDSurfaceDesc.dwFlags |= DDSCAPS_FLIP;
            BackBuffer->SetFrontBuffer(this);
          }
#endif
        }

        // MipMap Surfaces are handled here
        if( (DDSurfaceDesc.dwFlags & DDSD_MIPMAPCOUNT) &&
            (dwCaps & DDSCAPS_TEXTURE) &&
            (dwCaps & DDSCAPS_MIPMAP) )
        {
          dwCaps &= ~ (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP);

          dprintf(("DDRAW: Mipmpa # = %d\n",DDSurfaceDesc.dwMipMapCount));
          memcpy( &ComplexSurfaceDesc,
                  &DDSurfaceDesc,
                  sizeof(DDSURFACEDESC2));
          ComplexSurfaceDesc.dwMipMapCount = 0;
          ComplexSurfaceDesc.dwFlags &= ~DDSD_MIPMAPCOUNT;

          for(int i =0; i < DDSurfaceDesc.dwMipMapCount; i++)
          {
            dprintf(("DDRAW: Creating MipMap %d\n",i));
            // Mpmaps shirnk by 2
            ComplexSurfaceDesc.dwWidth  /= 2;
            ComplexSurfaceDesc.dwHeight /= 2;

            MipMapSurface = new OS2IDirectDrawSurface(lpDraw, &ComplexSurfaceDesc, TRUE);
            MipMapSurface->Vtbl.fnAddRef((IDirectDrawSurface *)MipMapSurface);

            DPA_InsertPtr( DPA_SurfaceMipMaps,
                           DPA_GetPtrCount(DPA_SurfaceMipMaps),
                           MipMapSurface);

            if(MipMapSurface->GetLastError() != DD_OK)
            {
              lastError = MipMapSurface->GetLastError();
              dprintf(("DDRAW: Attached surface creation returned error %d\n",lastError));
              return;
            } // Endif Errorcheck
          } //End for(i =0; i < DDSurfaceDesc.dwMipMapCount; i++)
        } // End of MipMaps

        #if 0
          if(DDSurfaceDesc.dwFlags)
            dprintf(("DDRAW: Unsupported Complex Surface\n"));
        #endif
      } // Endif DDSCAPS_COMPLEX
    }
    else
    {
      lastError = DDERR_INVALIDPARAMS;
    }
  } // Endif DDCAPS is valid
  else
  {
    dprintf(("DDRAW: CAPS not valid\n"));
    lastError = DDERR_INVALIDPARAMS;
  }
    dprintf(("DDRAW: Buf %X Screen Caps (%d,%d), bitcount %d\n\n", this, lpDraw->GetScreenHeight(), lpDraw->GetScreenWidth(),
    lpDraw->dCaps.ulDepth));
    if(DD_OK!=lastError)
    {
      dprintf(("DDRAW: Some Error Check Flags\n"));
      _dump_DDSCAPS(DDSurfaceDesc.dwFlags);
    }
}
//******************************************************************************
//******************************************************************************
HRESULT OS2IDirectDrawSurface::DoColorFill(LPRECT lpDestRect,DWORD dwFillColor)
{

  int  FillWidth, FillHeight, Top, Left;
  DWORD *pPal24;
  WORD  *pPal16;
  dprintf(("DDRAW: ColorFill with %08X\n", dwFillColor));

  if(NULL!=lpDestRect)
  {
    dprintf(("DDRAW: Fill only Rect(%d,%d)(%d,%d) (%d,%d)", lpDestRect->left, lpDestRect->top,
             lpDestRect->right, lpDestRect->bottom, width, height));
    FillWidth  = RECT_WIDTH(lpDestRect);
    FillHeight = RECT_HEIGHT(lpDestRect);
    Top        = lpDestRect->top;
    Left       = lpDestRect->left;
    if(Top + FillHeight > height) {
        FillHeight = height - Top;
        dprintf(("correcting FillHeight to %d", FillHeight));
    }
    if(Left + FillWidth > width) {
        FillWidth = width - Left;
        dprintf(("correcting FillWidth to %d", FillWidth));
    }
  }
  else
  {
    dprintf(("DDRAW: Fill all at addr "));
    Top = 0;
    Left = 0;
    FillWidth  = width;
    FillHeight = height;
    //pLine = pDiveBuffer;
  }
  //dprintf(("DDRAW: 0x%08X (%d/%d) at\n", pLine,FillWidth,FillHeight));

  // Better safe than sorry
  if (FillHeight <= 0)
    return(DD_OK);

  if(pDiveBuffer!=pFrameBuffer)
  {
    if( (NULL== lpDraw->pPrimSurf) ||
        (NULL== ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette) )
    {
      if(!fPalInit)
      {
        for(DWORD i=0;i<256;i++)
        {
          wDefaultPalete16[i] = (DefaultPalette[i*3+2]>>3) +
                                ((DefaultPalette[i*3+1]>>2) <<5) +
                                ((DefaultPalette[i*3]>>3) << 11);
          dwDefaultPalete24[i]= ((DefaultPalette[i*3+2]>>3) <<8)+
                                ((DefaultPalette[i*3+1]>>2) <<16) +
                                ((DefaultPalette[i*3]>>3) << 24);
          dprintf(( " # %d : RGB=%02X/%02X/%02X => %04X\n",
                    i,
                    DefaultPalette[i*3],
                    DefaultPalette[i*3+1],
                    DefaultPalette[i*3+2],
                    wDefaultPalete16[i]));
         // aPal24[i] = (lpColorTable[i].peBlue <<8) +
         //             (lpColorTable[i].peGreen<<16) +
         //             (lpColorTable[i].peRed<<24);
        }
        fPalInit = TRUE;
      }
      pPal16 = &wDefaultPalete16[0];
      pPal24 = &dwDefaultPalete24[0];
    }
    else
    {
      pPal16 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal16;
      pPal24 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal24;
    }
  }

  if(NULL!=ColorFill) {
    ColorFill( pDiveBuffer, pFrameBuffer, Top, Left,
              FillWidth, FillHeight, dwPitchDB,dwPitchFB,
              dwFillColor,(16==lpDraw->dCaps.ulDepth)?(VOID*)pPal16:(VOID*)pPal24);
    SurfChangeUniquenessValue(this);
  }
  else
  {
    dprintf(("DDRAW: ColorFill function is NULL!!"));
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawSurface::ColorConversion(LPRECT lpRect)
{
  //char *pSrc,*pDst,*pSLine,*pDLine;
  DWORD CCwidth,CCheight,x,y;
  DWORD *pPal24;
  WORD  *pPal16;

  if( (NULL== lpDraw->pPrimSurf) ||
      (NULL== ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette) )
  {
    if(!fPalInit)
    {
      for(DWORD i=0;i<256;i++)
      {
          wDefaultPalete16[i] = (DefaultPalette[i*3+2]>>3) +
                                ((DefaultPalette[i*3+1]>>2) <<5) +
                                ((DefaultPalette[i*3]>>3) << 11);
          dwDefaultPalete24[i]= ((DefaultPalette[i*3+2]>>3) <<8)+
                                ((DefaultPalette[i*3+1]>>2) <<16) +
                                ((DefaultPalette[i*3]>>3) << 24);
        dprintf(( " # %d : RGB=%02X/%02X/%02X => %04X\n",
                  i,
                  DefaultPalette[i*3],
                  DefaultPalette[i*3+1],
                  DefaultPalette[i*3+2],
                  wDefaultPalete16[i]));
       // aPal24[i] = (lpColorTable[i].peBlue <<8) +
       //             (lpColorTable[i].peGreen<<16) +
       //             (lpColorTable[i].peRed<<24);
      }
      fPalInit = TRUE;
    }
    pPal16 = &wDefaultPalete16[0];
    pPal24 = &dwDefaultPalete24[0];
  }
  else
  {
    pPal16 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal16;
    pPal24 = ((OS2IDirectDrawSurface*)lpDraw->pPrimSurf)->lpPalette->aPal24;
  }

  if(NULL==lpRect)
  {
    //pSrc = pFrameBuffer;
    //pDst = pDiveBuffer;
    CCwidth  = width;
    CCheight = height;
    x = 0;
    y = 0;
  }
  else
  {
    // ToDo: Check why the top/bottom values come in swaped here
    // for now simply reverse them with the following 3 lines
    y = lpRect->top;
    lpRect->top = lpRect->bottom;
    lpRect->bottom = y;
    // end of hack
  #if 0
    pSrc = pFrameBuffer +
           (lpRect->top * dwPitchFB) +
           (lpRect->left* (lpDraw->dCaps.ulDepth/8));
    pDst = pDiveBuffer +
           (lpRect->top * dwPitchDB) +
           (lpRect->left* dwBytesPPDive);
  #endif
    y = lpRect->top;
    x = lpRect->left;
    CCwidth  = lpRect->right - lpRect->left;
    CCheight = lpRect->bottom - lpRect->top;

  }

//  dprintf( ("H: %d W: %d\n SRC @ %08X\n DST @ %08X\n",
//            CCheight, CCwidth, pSrc,pDst));
  if(NULL!=ColorConv)
    ColorConv( pDiveBuffer, pFrameBuffer, y, x,
               CCwidth, CCheight, dwPitchDB,dwPitchFB,
               16==lpDraw->dCaps.ulDepth?(VOID*)pPal16:(VOID*)pPal24);
  else
  {
    dprintf(("DDRAW: ColorConv function is NULL!!"));
  }

/*
  pSLine = pSrc;
  pDLine = pDst;

  switch(lpDraw->dCaps.ulDepth)
  {
    case 8:
      dprintf(("DDRAW:  8Bit target CC not implemented\n"));
      break;
    case 15:
      dprintf(("DDRAW:  15 Bit not implemented using 16bit might look ugly\n"));
    case 16:
      if(8==lpDraw->GetScreenBpp())
      {
        dprintf(("DDRAW: 8->16Bit CC\n"));
        for(y=0;CCheight;CCheight--,y++)
        {
          for(x=0;x<width;x++)
          {
            *(((WORD*)pDLine)+x) = pPal16[pSLine[x]];

          }
          pSLine += dwPitchFB;
          pDLine += dwPitchDB;
        }
      }
      else
      {
        dprintf(("DDRAW: %d ->16Bit Not implemented",lpDraw->GetScreenBpp()));
      }
      break;
    case 24:
      if(8==lpDraw->GetScreenBpp())
      {
        dprintf(("DDRAW: 8->24Bit CC"));
        for(y=0;CCheight;CCheight--,y++)
        {
          char *pIter;
          for(x=0,pIter=pDLine;x<width;x++,pIter=pDLine)
          {
            *((DWORD*)pIter) = pPal24[pSLine[x]];
            pIter+=3;
          }
          pSLine += dwPitchFB;
          pDLine += dwPitchDB;
        }
      }
      else
      {
        dprintf(("DDRAW: %d ->24Bit Not implemented",lpDraw->GetScreenBpp()));
      }
      break;
    case 32:
      if(8==lpDraw->GetScreenBpp())
      {
        dprintf(("DDRAW: 8->32Bit CC"));
        for(y=0;CCheight;CCheight--,y++)
        {
          for(x=0;x<width;x++)
          {
            *(((DWORD*)pDLine)+x) = pPal24[pSLine[x]];

          }
          pSLine += dwPitchFB;
          pDLine += dwPitchDB;
        }
      }
      else
      {
        dprintf(("DDRAW: %d ->32Bit Not implemented",lpDraw->GetScreenBpp()));
      }
      break;
    default:
        dprintf( ("Unexpected Screen Bitdepth %d\n",
                  lpDraw->dCaps.ulDepth));
      break;
  }
*/
}
//******************************************************************************
//******************************************************************************
// Internal callbacks uses by destructor
INT CALLBACK DestroyRects(LPVOID lpItem, DWORD dwRes)
{
  DDRectangle *pItem = (DDRectangle*) lpItem;
  delete(pItem);
  return 1;
}

INT CALLBACK ReleaseSurfaces(LPVOID lpItem, DWORD dwRes)
{
  OS2IDirectDrawSurface *pSurf;
  pSurf = (OS2IDirectDrawSurface *)lpItem;
  pSurf->Vtbl.fnRelease(pSurf);
  return 1;
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawSurface::~OS2IDirectDrawSurface()
{
  if(hwndFullScreen) {
      DestroyWindow(hwndFullScreen);
  }

  if(DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
  {
    lpDraw->SetPrimarySurface(FALSE);
    if(lpPalette)
      lpPalette->RestorePhysPalette();

  }
  else
  {
    if( diveBufNr != -1)
    {
      if(fLocked)
        DiveEndImageBufferAccess(hDive, diveBufNr);
      DiveFreeImageBuffer(hDive, diveBufNr);

    }

    fLocked = FALSE;
    diveBufNr = -1;
  }
  // Free Buffers if they have been allocated
  // DIVE Buffer

  if(NULL!=pDBreal)
    free(pDBreal);

  // FrameBuffer

  if(NULL!=pFBreal)
    free(pFBreal);

  // Clear the list of locked rectangles
  if (DPA_GetPtrCount(DPA_LockedRects)>0)
  {
    DPA_DestroyCallback( DPA_LockedRects,
                         DestroyRects,
                         0);
  }

  if(lpClipper)
  {
    lpClipper->Vtbl.fnRelease((IDirectDrawClipper*)lpClipper);
    lpClipper = NULL;
  }

  if(lpPalette)
  {
    lpPalette->Vtbl.fnRelease((IDirectDrawPalette*)lpPalette);
    lpPalette = NULL;
  }

  if(hbmImage)
    DeleteObject(hbmImage);

  if(hdcImage)
    DeleteDC(hdcImage);

  if (NULL!=BackBuffer)
    BackBuffer->Vtbl.fnRelease(BackBuffer);

  if (DPA_GetPtrCount(DPA_SurfaceMipMaps)>0)
  {
    DPA_DestroyCallback( DPA_SurfaceMipMaps,
                         ReleaseSurfaces,
                         0);
  }

  if (DPA_GetPtrCount(DPA_SurfaceAttached)>0)
  {
    DPA_DestroyCallback( DPA_SurfaceAttached,
                         ReleaseSurfaces,
                         0);
  }

  lpDraw->Vtbl.fnRelease(lpDraw);
}

//******************************************************************************
//******************************************************************************
inline  void OS2IDirectDrawSurface::SetFrontBuffer( OS2IDirectDrawSurface* NewFBuffer)
{
  FrontBuffer = NewFBuffer;
  if (NULL==NewFBuffer)
  {

    dprintf(("DDRAW: Remove Frontbuffer\n"));

    // The real Frontbuffer was removed check if I'm now the one
    if(NULL!=BackBuffer)
    {
      dprintf(("DDRAW: We have the a backbuffer so we're the Frontbuffer\n"));
      NextFlip = BackBuffer;
      DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_BACKBUFFER;
      DDSurfaceDesc.ddsCaps.dwCaps |=  DDSCAPS_FRONTBUFFER;
      BackBuffer->SetFrontBuffer(this);
    }
    else
    {
      // Flipchain is destroyed
      dprintf(("DDRAW: No longer part of a flipchain\n"));
      DDSurfaceDesc.ddsCaps.dwCaps &= ~(DDSCAPS_BACKBUFFER | DDSCAPS_FLIP);
    }
  }
  else
  {
    dprintf(("DDRAW: Set new Frontbuffer to 0x%08X\n",NewFBuffer));
    if(NULL==NewFBuffer->GetFrontBuffer())
    {
      dprintf(("DDRAW: We're the 1st backbuffer\n"));
      DDSurfaceDesc.ddsCaps.dwCaps |=  DDSCAPS_BACKBUFFER | DDSCAPS_FLIP;
      FrontBuffer->NextFlip = this;
    }
    else
    {
      dprintf(("DDRAW: We are one of many buffers\n"));
      DDSurfaceDesc.ddsCaps.dwCaps &=  ~DDSCAPS_BACKBUFFER ;
      DDSurfaceDesc.ddsCaps.dwCaps |=  DDSCAPS_FLIP;
    }

    if(NULL!=BackBuffer)
      BackBuffer->SetFrontBuffer(this);

    DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FRONTBUFFER;
  }
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
  // ToDo: Add Interface handling for D3D Textures
  HRESULT rc;
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: OS2IDirectDrawSurface::SurfQueryInterface\n"));

  if(NULL==ppvObj)
  {
    rc = DDERR_INVALIDPARAMS;
    goto RetFn;
  }

  rc = E_NOINTERFACE;
  *ppvObj = NULL;

  if(IsEqualGUID(riid, &IID_IDirectDrawSurface))
  {
    *ppvObj = &me->lpVtbl;  // ToDo DO a real V1 table
    rc = DD_OK;
    goto RetFn;
  }
  if(IsEqualGUID(riid, &IID_IDirectDrawSurface2))
  {
    *ppvObj = &me->lpVtbl2;
    rc = DD_OK;
    goto RetFn;
  }
  if(IsEqualGUID(riid, &IID_IDirectDrawSurface3))
  {
    *ppvObj = &me->Vtbl3;
    rc = DD_OK;
    goto RetFn;
  }
  if(IsEqualGUID(riid, &IID_IDirectDrawSurface4))
  {
    *ppvObj = This;
    rc =DD_OK;
  }

  //if(IsEqualGUID(riid, &IID_IUnknown)) ...

RetFn:

  if(DD_OK==rc)
    SurfAddRef(This);

  return(rc);
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SurfAddRef(THIS This)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: OS2IDirectDrawSurface::SurfAddRef %d\n", me->Referenced+1));

  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SurfRelease(THIS This)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: OS2IDirectDrawSurface::SurfRelease %d\n", me->Referenced-1));
  dprintf(("DDRAW: OS2IDirectDrawSurface::Surface %X\n", me));
  if(me->Referenced)
  {
    me->Referenced--;
    if(me->Referenced == 0)
    {
      delete( me);
      #ifndef __WATCOMC__
        //_interrupt(3);
      #endif
      return(0);
    }
    else
      return me->Referenced;
  }
  else
    return(0);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfAddAttachedSurface(THIS This, LPDIRECTDRAWSURFACE2 lpDDSurface)
{

  dprintf(("DDRAW: SurfAddAttachedSurface\n"));
  return SurfAddAttachedSurface4(This, (LPDIRECTDRAWSURFACE4)lpDDSurface);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfAddAttachedSurface3(THIS This, LPDIRECTDRAWSURFACE3 lpDDSurface)
{

  dprintf(("DDRAW: SurfAddAttachedSurface\n"));
  return SurfAddAttachedSurface4(This, (LPDIRECTDRAWSURFACE4)lpDDSurface);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfAddAttachedSurface4(THIS This, LPDIRECTDRAWSURFACE4 lpDDSurface)
{
  OS2IDirectDrawSurface *AttachedSurface;
  OS2IDirectDrawSurface *BBCursor;
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int rc;

  dprintf(("DDRAW: SurfAddAttachedSurface4\n"));

  if (NULL==lpDDSurface)
    return DDERR_INVALIDPARAMS;

  AttachedSurface = (OS2IDirectDrawSurface*) lpDDSurface;

  if(AttachedSurface->IsImplicitSurface())
  {
    dprintf(("DDRAW: Internal : Can't attach an implicitly created surface to another surface\n"));
    return(DDERR_CANNOTATTACHSURFACE);
  }

  if(This == AttachedSurface)
  {
    dprintf(("DDRAW: Can't attach a surface to itself\n"));
    return(DDERR_CANNOTATTACHSURFACE);
  }

  if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_TEXTURE)
  {
    if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
    {
      if(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
      {
          rc = DPA_InsertPtr( me->DPA_SurfaceMipMaps,
                              DPA_GetPtrCount(me->DPA_SurfaceMipMaps),
                              AttachedSurface);

        if( rc>=0)
        {
          me->DDSurfaceDesc.dwFlags |= DDSD_MIPMAPCOUNT;
          me->DDSurfaceDesc.dwMipMapCount++;

          AttachedSurface->Vtbl.fnAddRef(AttachedSurface);
        }
        else
        {
          dprintf(("DDRAW: Internal : Error attaching to MipMap\n"));
          return(DDERR_CANNOTATTACHSURFACE);
        }
      }
      else
      {
        dprintf(("DDRAW: Target Surface isn't a MipMap\n"));
        return(DDERR_CANNOTATTACHSURFACE);
      }
    }
    else
    {
        rc = DPA_InsertPtr( me->DPA_SurfaceAttached,
                            DPA_GetPtrCount(me->DPA_SurfaceAttached),
                            AttachedSurface);

      if(rc>=0)
      {
        AttachedSurface->Vtbl.fnAddRef(AttachedSurface);
      }
      else
      {
        dprintf(("DDRAW: Internal : Error attaching to general Set\n"));
        return(DDERR_CANNOTATTACHSURFACE);
      }
    }
  } // endif DDSCAPS_TEXTURE
  else
  {
    if( (AttachedSurface->DDSurfaceDesc.dwWidth != me->DDSurfaceDesc.dwWidth)
        || (AttachedSurface->DDSurfaceDesc.dwHeight != me->DDSurfaceDesc.dwHeight)
    //    || (AttachedSurface->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount !=
    //        me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
       )
    {
      dprintf(("DDRAW: Surfaces don't have equal dimensions\n"));
      return(DDERR_CANNOTATTACHSURFACE);
    }
    else
    {
      if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
      {
        if( (AttachedSurface->GetFrontBuffer()!=NULL) || (AttachedSurface->BackBuffer!= NULL))
        {
          dprintf(("DDRAW: Surface already has a front/backbuffer\n"));
          return(DDERR_SURFACEALREADYATTACHED);
        }

        if(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER)
        {
          if(NULL!=me->BackBuffer)
          {
            BBCursor = me->BackBuffer;
            while(NULL!=BBCursor)
            {
              BBCursor->DDSurfaceDesc.dwFlags |= DDSD_BACKBUFFERCOUNT;
              BBCursor->DDSurfaceDesc.dwBackBufferCount++;
              BBCursor->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FLIP;
              BBCursor = BBCursor->BackBuffer;
            }
            BBCursor->BackBuffer = AttachedSurface;
            AttachedSurface->SetFrontBuffer(BBCursor);
          }
          else
          {
            me->BackBuffer = AttachedSurface;
            AttachedSurface->SetFrontBuffer(me);
          }
          me->DDSurfaceDesc.dwFlags |= DDSD_BACKBUFFERCOUNT;
          me->DDSurfaceDesc.dwBackBufferCount++;
          me->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FLIP;

          AttachedSurface->Vtbl.fnAddRef(AttachedSurface);
          return (DD_OK);
        }
        else
        {
          dprintf(("DDRAW: Can't attach backbuffer to anything but a frontbuffer\n"));
          return(DDERR_CANNOTATTACHSURFACE);
        }
      }
      else
      {
        if(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
        {
          if(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)
          {
            if( DPA_InsertPtr( me->DPA_SurfaceMipMaps,
                               DPA_GetPtrCount(me->DPA_SurfaceMipMaps),
                               AttachedSurface) >=0)
            {
              me->DDSurfaceDesc.dwFlags |= DDSD_MIPMAPCOUNT;
              me->DDSurfaceDesc.dwMipMapCount++;
              me->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_FRONTBUFFER;

              AttachedSurface->Vtbl.fnAddRef(AttachedSurface);
            }
            else
            {
              dprintf(("DDRAW: Internal : Error attaching to MipMap\n"));
              return(DDERR_CANNOTATTACHSURFACE);
            }
          }
          else
          {
            dprintf(("DDRAW: Target Surface isn't a MipMap\n"));
           return(DDERR_CANNOTATTACHSURFACE);
          }
        }
        else
        {
          if( DPA_InsertPtr( me->DPA_SurfaceAttached,
                             DPA_GetPtrCount(me->DPA_SurfaceAttached),
                             AttachedSurface) >=0)
          {
            AttachedSurface->Vtbl.fnAddRef(AttachedSurface);
          }
          else
          {
            dprintf(("DDRAW: Internal : Error attaching to general Set\n"));
            return(DDERR_CANNOTATTACHSURFACE);
          }
        }
      }// End if not DDSCAPS_BACKBUFFER
    }
  }
  dprintf(("DDRAW: Surface attached\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfDeleteAttachedSurface(THIS This, DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSurface)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfDeleteAttachedSurface\n"));

  return(SurfDeleteAttachedSurface4(me, dwFlags, (LPDIRECTDRAWSURFACE4)lpDDSurface));
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfDeleteAttachedSurface3(THIS This, DWORD dwFlags, LPDIRECTDRAWSURFACE3 lpDDSurface)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfDeleteAttachedSurface\n"));

  return(SurfDeleteAttachedSurface4(me, dwFlags, (LPDIRECTDRAWSURFACE4)lpDDSurface));
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfDeleteAttachedSurface4(THIS This, DWORD dwFlags , LPDIRECTDRAWSURFACE4 lpDDSurface)
{

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *AttachedSurface;
  OS2IDirectDrawSurface *SurfaceCursor;
  int i;

  BOOL Found = FALSE;
  dprintf(("DDRAW: SurfDeleteAttachedSurface\n"));

  if((0!=dwFlags)||(NULL==lpDDSurface))
    return(DDERR_INVALIDPARAMS);

  AttachedSurface = (OS2IDirectDrawSurface*) lpDDSurface;
  if (AttachedSurface->IsImplicitSurface())
    return (DDERR_CANNOTDETACHSURFACE);

  if ( (AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & ( DDSCAPS_FLIP | DDSCAPS_BACKBUFFER)) &&
       !(AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER) )
  {
    // Surface seams to be a backbuffer in a flipchain search it

    // Goto top of list
    if(me->FrontBuffer!=NULL)
    {
      SurfaceCursor = me->FrontBuffer;
      while(SurfaceCursor->GetFrontBuffer()!=NULL)
        SurfaceCursor = SurfaceCursor->GetFrontBuffer();
    }
    else
      SurfaceCursor = me;

    // now iterrate through the list skip first in list as we don't remove the frontbuffer

    SurfaceCursor = SurfaceCursor->BackBuffer;
    while((SurfaceCursor!= AttachedSurface)&&(SurfaceCursor!=NULL))
      SurfaceCursor = SurfaceCursor->BackBuffer;

    if(SurfaceCursor!=NULL)
    {
      Found = TRUE;
      // remove the Surface from the list
      SurfaceCursor->FrontBuffer->BackBuffer = SurfaceCursor->BackBuffer;
      if(SurfaceCursor->BackBuffer!=NULL)
      {
        SurfaceCursor->BackBuffer->SetFrontBuffer(SurfaceCursor->FrontBuffer);

      }
      else
      {
        // we were the last buffer in the list have we been the only backbuffer?
        if(SurfaceCursor->FrontBuffer->FrontBuffer == NULL)
        {
          // Yepp so "destroy" the flipchain
          SurfaceCursor->FrontBuffer->DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FLIP;
          SurfaceCursor->FrontBuffer->DDSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
        }
      }
      // decrement the backbuffer count of all buffers in the chain in front of us
      while(SurfaceCursor->GetFrontBuffer()!=NULL)
      {
        SurfaceCursor = SurfaceCursor->GetFrontBuffer();
        SurfaceCursor->DDSurfaceDesc.dwBackBufferCount-- ;
      }

      AttachedSurface->DDSurfaceDesc.dwBackBufferCount = 0;
      AttachedSurface->DDSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
      AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FLIP;
      AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER; // Set this flag as adding to the chain removed it
      AttachedSurface->lpVtbl->fnRelease(AttachedSurface);

    }
  } //endif delete back/frontbuffers

  if ( (!Found) && (AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps & ( DDSCAPS_FLIP & DDSCAPS_FRONTBUFFER)) )
  {
    // seams like someone wants a new frontbuffer

    // Goto top of list

    if(me->FrontBuffer!=NULL)
    {
      SurfaceCursor = me->FrontBuffer;
      while(SurfaceCursor->GetFrontBuffer()!=NULL)
        SurfaceCursor = SurfaceCursor->GetFrontBuffer();
    }
    else
      SurfaceCursor = me;

    if(SurfaceCursor == AttachedSurface)
    {
      Found = TRUE;
      SurfaceCursor->BackBuffer->SetFrontBuffer(NULL);
      AttachedSurface->DDSurfaceDesc.dwBackBufferCount = 0;
      AttachedSurface->DDSurfaceDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
      AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps &= ~DDSCAPS_FLIP;
      AttachedSurface->lpVtbl->fnRelease(AttachedSurface);
    }

  }


  if ( (!Found) && (AttachedSurface->DDSurfaceDesc.ddsCaps.dwCaps &  DDSCAPS_MIPMAP ) )
  {
    // Surface seams to be a mipmap
    i = 0;
    while((DPA_GetPtrCount(me->DPA_SurfaceMipMaps)>i ) && !Found)
    {
      if (DPA_FastGetPtr(me->DPA_SurfaceMipMaps,i) == AttachedSurface)
      {
        Found = TRUE;
        DPA_DeletePtr(me->DPA_SurfaceMipMaps,i);
        AttachedSurface->lpVtbl->fnRelease(AttachedSurface);
        // adjust our info
        me->DDSurfaceDesc.dwMipMapCount-- ;
        if (!me->DDSurfaceDesc.dwMipMapCount)
        {
          me->DDSurfaceDesc.dwFlags &= ~DDSD_MIPMAPCOUNT;
        }
      }
      i++;
    }
  }

  if(!Found)
  {
    // Surface seams to be an attached one
    i = 0;
    while((DPA_GetPtrCount(me->DPA_SurfaceAttached)>i ) && !Found)
    {
      if (DPA_FastGetPtr(me->DPA_SurfaceAttached,i) == AttachedSurface)
      {
        Found = TRUE;
        DPA_DeletePtr(me->DPA_SurfaceAttached,i);
        AttachedSurface->lpVtbl->fnRelease(AttachedSurface);
      }
      i++;
    }
  }


  return(Found?DD_OK:DDERR_SURFACENOTATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfEnumAttachedSurfaces(THIS This, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpCallBack)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfEnumAttachedSurfaces\n"));

  return(SurfEnumAttachedSurfaces4(me,lpContext, (LPDDENUMSURFACESCALLBACK2) lpCallBack));
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfEnumAttachedSurfaces4(THIS This, LPVOID lpContext ,LPDDENUMSURFACESCALLBACK2 lpCallBack)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *EnumSurface;
  DDSURFACEDESC2         EnumDesc;
  int i,count;
  HRESULT rc;

  dprintf(("DDRAW: SurfEnumAttachedSurfaces\n"));
  if (NULL==lpCallBack)
    return (DDERR_INVALIDPARAMS);

  rc = DDENUMRET_OK;


  if(me->BackBuffer != NULL)
  {
    memcpy(&EnumDesc,&(me->DDSurfaceDesc),sizeof(DDSURFACEDESC2));
    rc = lpCallBack((LPDIRECTDRAWSURFACE4)me->BackBuffer,&EnumDesc,lpContext);
  }

  count = DPA_GetPtrCount(me->DPA_SurfaceMipMaps);

  if(count>0)
  {
    i=0;
    while( (DDENUMRET_OK == rc) && i<count )
    {
        EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceMipMaps,i);
        memcpy( &EnumDesc,
                &(EnumSurface->DDSurfaceDesc),
                sizeof(DDSURFACEDESC2));
      // Calling back into WIN32 app so we had to reset FS
      rc = lpCallBack((LPDIRECTDRAWSURFACE4)EnumSurface,&EnumDesc,lpContext);
      i++;
    }
  }

  count = DPA_GetPtrCount(me->DPA_SurfaceAttached);

  if(count>0)
  {
    i=0;
    while( (DDENUMRET_OK == rc) && i<count )
    {
      EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceAttached,i);
      memcpy( &EnumDesc,
              &(EnumSurface->DDSurfaceDesc),
              sizeof(DDSURFACEDESC2));
      rc = lpCallBack((LPDIRECTDRAWSURFACE4)EnumSurface,&EnumDesc,lpContext);
      i++;
    }
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfFlip(THIS This, LPDIRECTDRAWSURFACE2 lpDDSurf, DWORD dwFlags)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfFlip\n"));

  return(SurfFlip4(me, (LPDIRECTDRAWSURFACE4) lpDDSurf, dwFlags));
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfFlip3(THIS This, LPDIRECTDRAWSURFACE3 lpDDSurf, DWORD dwFlags)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfFlip\n"));

  return(SurfFlip4(me, (LPDIRECTDRAWSURFACE4) lpDDSurf, dwFlags));
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfFlip4(THIS This, LPDIRECTDRAWSURFACE4 lpDDSurf, DWORD dwFlags)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *FlipSurface;
  OS2IDirectDrawSurface *FlipCursor;
  LPVOID Data;
  char *pcrFB,*pcFB,*pcrDB,*pcDB;

  dprintf(("DDRAW: SurfFlip4\n"));

  if(!((me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER) &&
       (me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_FLIP))
    )
  {
      dprintf(("DDRAW: Flip called on none Frontbuffer/Flip surface\n Flags:\n"));
      _dump_DDSCAPS(me->DDSurfaceDesc.ddsCaps.dwCaps);
      dprintf(("DDRAW: \n"));
      return(DDERR_NOTFLIPPABLE);
  }

  if(NULL!=lpDDSurf)
  {
    dprintf(("DDRAW: Check if Surface is in Flipchain!\n"));

    // We got an override surface check if it is in the flipchain
    FlipSurface = (OS2IDirectDrawSurface*) lpDDSurf;
    FlipCursor = me->BackBuffer;
    while((NULL!=FlipCursor)&&(FlipCursor!=FlipSurface))
    {
      FlipCursor = FlipCursor->BackBuffer;
    }

    if(FlipCursor!=FlipSurface)
    {
      dprintf(("DDRAW: Surface not in Flipchain!\n"));

      return (DDERR_INVALIDPARAMS); // Not sure if the returnvalue is right
    }
  }
  else
  {
    FlipSurface = me->NextFlip; // Take the next Surface in the Flipchain
    dprintf(("DDRAW: Next Surface @ 0x%08X\n",FlipSurface));
  }

  if((me->fLocked)||(FlipSurface->fLocked))
  {
    dprintf(("DDRAW: Locked surface(s) Dest %d Src %d\n",me->fLocked,FlipSurface->fLocked));

    return(DDERR_SURFACEBUSY);
  }

  if (-1 != me->diveBufNr)
  {
    //dprintf(("DDRAW: DIVE Flipchain DiveBuffer #%d",FlipSurface->diveBufNr));

    // we got some DIVE surfaces
    // On Dive Buffers More then Double buffering won't get any perf. gain
    // as we have to move all the data to the Frontbuffer and can't simply exchange the pointers
    // Doulebuffering should work best.

    //rc = DiveBlitImage(me->hDive, FlipSurface->diveBufNr, me->diveBufNr);
    //dprintf(("DDRAW: DiveBlitImage rc = 0x%08X\n"));
    SurfBltFast4( me,
                  0,
                  0,
                  (LPDIRECTDRAWSURFACE4)FlipSurface,
                  NULL,
                  DDBLTFAST_NOCOLORKEY);

    if(NULL==lpDDSurf)
    {
      // advance in the flipchain if no valid override surface was passed in
      // if we reached the end of the flipchain The Frontbuffer is the next to flip to
      me->NextFlip = FlipSurface->BackBuffer!=NULL?FlipSurface->BackBuffer:me->BackBuffer;
    }
  }
  else
  {
    dprintf(("DDRAW: Memory Flipchain"));
    // Memory Flipchain
    //
    // ToDo : Check what happens to src/dest colorkeys etc do the move also ?
    //
    // We only change the memory pointer to the buffers no need to copy all the data
    // So the NextFlip is here allways the Backbuffer so we won't advance this
    //
    // Sample  (triple buffering) :               Before Flip    After Flip
    //                               Buffer:      FB  BB  TB     FB  BB  TB
    //                               Memory:      11  22  33     22  33  11
    //

    Data  = me->DDSurfaceDesc.lpSurface;
    pcrFB = me->pFBreal;
    pcFB  = me->pFrameBuffer;
    pcrDB = me->pDBreal;
    pcDB  = me->pDiveBuffer;
    me->DDSurfaceDesc.lpSurface = FlipSurface->DDSurfaceDesc.lpSurface;
    me->pFBreal                 = FlipSurface->pFBreal;
    me->pFrameBuffer            = FlipSurface->pFrameBuffer;
    me->pDBreal                 = FlipSurface->pDBreal;
    me->pDiveBuffer             = FlipSurface->pDiveBuffer;

    if (NULL==lpDDSurf)
    {
      while (NULL!=FlipSurface->BackBuffer)
      {
        FlipSurface->DDSurfaceDesc.lpSurface = FlipSurface->BackBuffer->DDSurfaceDesc.lpSurface;
        FlipSurface->pFBreal                 = FlipSurface->BackBuffer->pFBreal;
        FlipSurface->pFrameBuffer            = FlipSurface->BackBuffer->pFrameBuffer;
        FlipSurface->pDBreal                 = FlipSurface->BackBuffer->pDBreal;
        FlipSurface->pDiveBuffer             = FlipSurface->BackBuffer->pDiveBuffer;
        FlipSurface = FlipSurface->BackBuffer;
      }
    }
    FlipSurface->DDSurfaceDesc.lpSurface = Data;
    FlipSurface->pFBreal                 = pcrFB;
    FlipSurface->pFrameBuffer            = pcFB;
    FlipSurface->pDBreal                 = pcrDB;
    FlipSurface->pDiveBuffer             = pcDB;
  }

  return DD_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetAttachedSurface(THIS This, LPDDSCAPS lpDDCaps,
                                         LPDIRECTDRAWSURFACE2 FAR * lpDDSurf)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfGetAttachedSurface\n"));

  return(SurfGetAttachedSurface4(me, (LPDDSCAPS2)lpDDCaps , (LPDIRECTDRAWSURFACE4*)lpDDSurf));
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetAttachedSurface3(THIS This, LPDDSCAPS lpDDCaps,
                                         LPDIRECTDRAWSURFACE3 FAR * lpDDSurf)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfGetAttachedSurface3\n"));

  return(SurfGetAttachedSurface4(me, (LPDDSCAPS2)lpDDCaps , (LPDIRECTDRAWSURFACE4*)lpDDSurf));
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetAttachedSurface4(THIS This, LPDDSCAPS2 lpDDCaps,
                                          LPDIRECTDRAWSURFACE4 FAR * lpDDSurf)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  OS2IDirectDrawSurface *EnumSurface = NULL;
  OS2IDirectDrawSurface *AttachedSurface = NULL;
  HRESULT rc;
  int i;

    dprintf(("DDRAW: SurfGetAttachedSurface4\n>Requested Caps: "));
    _dump_DDSCAPS(lpDDCaps->dwCaps);
    dprintf(("DDRAW: \n"));

  if( (NULL==lpDDCaps)||(NULL==lpDDSurf))
  {
    dprintf(("DDRAW: Invalid params\n\n"));
    return (DDERR_INVALIDPARAMS);
  }


  rc = DD_OK;

  if( (me->BackBuffer!=NULL) &&
      (me->BackBuffer->DDSurfaceDesc.ddsCaps.dwCaps & lpDDCaps->dwCaps) )
  {
    dprintf(("DDRAW: Return Backbuffer\n"));
    AttachedSurface = me->BackBuffer;
  }

  if(DPA_GetPtrCount(me->DPA_SurfaceMipMaps)>0)
  {
    i=0;
    while( i<DPA_GetPtrCount(me->DPA_SurfaceMipMaps) )
    {
      EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceMipMaps,i);
      if(EnumSurface->DDSurfaceDesc.ddsCaps.dwCaps == lpDDCaps->dwCaps)
      {
        if(NULL==AttachedSurface)
          AttachedSurface = EnumSurface;
        else
          rc = DDERR_NOTFOUND; // Not sure if this is the right return value,
                                  // but function must fail if more then one surface fits

      }
      i++;
    }
  }

  if(DPA_GetPtrCount(me->DPA_SurfaceAttached)>0)
  {
    i=0;
    while( i<DPA_GetPtrCount(me->DPA_SurfaceAttached) )
    {
      EnumSurface = (OS2IDirectDrawSurface*) DPA_FastGetPtr(me->DPA_SurfaceAttached,i);
      if(EnumSurface->DDSurfaceDesc.ddsCaps.dwCaps == lpDDCaps->dwCaps)
      {
        if(NULL==AttachedSurface)
          AttachedSurface = EnumSurface;
        else
          rc = DDERR_NOTFOUND; // Not sure if this is the right return value,
                                  // but function must fail if more then one surface fits

      }
      i++;
    }
  }

  if( (DD_OK==rc) &&
      (NULL!=AttachedSurface) )
  {
    *lpDDSurf = (IDirectDrawSurface4*)AttachedSurface;
    // not sure but as we returned an reference rains usage count
    AttachedSurface->lpVtbl->fnAddRef(AttachedSurface);
  }
  else
  {
    *lpDDSurf = NULL;
    rc = DDERR_NOTFOUND;
  }


  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetBltStatus(THIS This, DWORD)
{
  dprintf(("DDRAW: SurfGetBltStatus\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetCaps(THIS This, LPDDSCAPS lpDDCaps)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfGetCaps\n"));

  if(NULL==lpDDCaps)
    return(DDERR_INVALIDPARAMS);

  lpDDCaps->dwCaps = me->DDSurfaceDesc.ddsCaps.dwCaps;
  _dump_DDSCAPS(lpDDCaps->dwCaps);
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetCaps4(THIS This, LPDDSCAPS2 lpDDCaps)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfGetCaps4\n"));

  if(NULL==lpDDCaps)
    return(DDERR_INVALIDPARAMS);

  memcpy(lpDDCaps, &(me->DDSurfaceDesc.ddsCaps), sizeof(DDSCAPS2) );
  _dump_DDSCAPS(lpDDCaps->dwCaps);
  _dump_DDSCAPS2(lpDDCaps->dwCaps2);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetClipper(THIS This, LPDIRECTDRAWCLIPPER FAR *lplpClipper)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfGetClipper\n"));

  if(me->lpClipper)
  {
    *lplpClipper = (LPDIRECTDRAWCLIPPER) me->lpClipper;
    return(DD_OK);
  }
  else
    return(DDERR_NOCLIPPERATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetColorKey(THIS This, DWORD dwFlags, LPDDCOLORKEY lpDDColKey)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfGetColorKey\n"));

  if ((0==dwFlags) || (NULL==lpDDColKey))
    return (DDERR_INVALIDPARAMS);

  // as we report only src colorkey in the caps return error on all others flags
  if( (DDCKEY_DESTBLT|DDCKEY_DESTOVERLAY|DDCKEY_SRCOVERLAY) & dwFlags)
    return(DDERR_UNSUPPORTED);

  if(me->DDSurfaceDesc.dwFlags & dwFlags)
  {
    if(DDCKEY_SRCBLT & dwFlags)
    {
      memcpy(lpDDColKey,&(me->DDSurfaceDesc.ddckCKSrcBlt),sizeof(DDCOLORKEY) );
    }
    else
      return (DDERR_INVALIDPARAMS); // some other flags where set => error
  }
  else
    return(DDERR_NOCOLORKEY); // surface doesn't have a color key set

  return (DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetFlipStatus(THIS This, DWORD dwFlags)
{
  dprintf(("DDRAW: SurfGetFlipStatus\n"));

  if( (DDGFS_CANFLIP!=dwFlags) && (DDGFS_ISFLIPDONE!=dwFlags) )
    return DDERR_INVALIDPARAMS;

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetPalette(THIS This, LPDIRECTDRAWPALETTE FAR *lplpPalette)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfGetPalette\n"));

  if(me->lpPalette)
  {
    *lplpPalette = (LPDIRECTDRAWPALETTE)me->lpPalette;
    return(DD_OK);
  }
  else
    return(DDERR_NOPALETTEATTACHED);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetPixelFormat(THIS This, LPDDPIXELFORMAT lpPixelFormat)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfGetPixelFormat %x %x", This, lpPixelFormat));

  if(NULL==lpPixelFormat)
    return DDERR_INVALIDPARAMS;

  _dump_pixelformat(&me->DDSurfaceDesc.ddpfPixelFormat);

  memcpy( (char*)lpPixelFormat,
          (char*)&(me->DDSurfaceDesc.ddpfPixelFormat),
          sizeof(DDPIXELFORMAT));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetSurfaceDesc(THIS This, LPDDSURFACEDESC lpSurface)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfGetSurfaceDesc\n"));

  if((lpSurface == NULL)||(lpSurface->dwSize != sizeof(DDSURFACEDESC)) )
    return(DDERR_INVALIDPARAMS);

  memcpy( (char *)lpSurface,
          (char *)&me->DDSurfaceDesc,
          sizeof(DDSURFACEDESC));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetSurfaceDesc4(THIS This, LPDDSURFACEDESC2 lpSurface)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfGetSurfaceDesc4\n"));

  if((lpSurface == NULL)||(lpSurface->dwSize != sizeof(DDSURFACEDESC2)) )
    return(DDERR_INVALIDPARAMS);

  memcpy( (char *)lpSurface,
          (char *)&me->DDSurfaceDesc,
          sizeof(DDSURFACEDESC2));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfInitialize(THIS, LPDIRECTDRAW, LPDDSURFACEDESC)
{
  dprintf(("DDRAW: SurfInitialize\n"));

  return(DDERR_ALREADYINITIALIZED); // Init is done during creation see M$ Doc
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfInitialize4(THIS, LPDIRECTDRAW, LPDDSURFACEDESC2)
{
  dprintf(("DDRAW: SurfInitialize\n"));

  return(DDERR_ALREADYINITIALIZED); // Init is done during creation see M$ Doc
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfIsLost(THIS)
{
  // We don't loose any surface ;)
  // But we might shoud check for primary and/or Dive Buffers as I don't know
  // if  they are preserved if switching to a FS DOS/OS2 session
  //
  dprintf(("DDRAW: SurfIsLost\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfLock( THIS This,
                            LPRECT lpRect,
                            LPDDSURFACEDESC lpSurfaceDesc,
                            DWORD dwFlags,
                            HANDLE hEvent)
{
  DDSURFACEDESC2 SurfaceDesc4;
  HRESULT rc;

  dprintf(("DDRAW: SurfLock %d %08X %d %d\n", (int)lpRect, (int)lpSurfaceDesc, dwFlags, hEvent));

  if((NULL==lpSurfaceDesc)|| ((dwFlags & DDLOCK_EVENT) && NULL != hEvent)) {
    dprintf(("Invalid parameters"));
    return DDERR_INVALIDPARAMS;
  }

  if(lpSurfaceDesc->dwSize != sizeof(DDSURFACEDESC) && lpSurfaceDesc->dwSize != sizeof(DDSURFACEDESC2)) {
    dprintf(("Invalid parameters"));
    return DDERR_INVALIDPARAMS;
  }

  SurfaceDesc4.dwSize = sizeof(DDSURFACEDESC2);

  rc = SurfLock4( This,
                  lpRect,
                  &SurfaceDesc4,
                  dwFlags,
                  hEvent);
  if (DD_OK==rc)
  {
    memcpy( (char*)lpSurfaceDesc,
            (char*)&SurfaceDesc4,
            lpSurfaceDesc->dwSize );
  }

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfLock4( THIS This,
                             LPRECT lpRect,
                             LPDDSURFACEDESC2 lpSurfaceDesc,
                             DWORD dwFlags,
                             HANDLE hEvent)
{

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;

  BOOL Found;
  DDRectangle *pIRectCurrent,*pIRectNew;
  HRESULT rc;

  dprintf( ("SurfLock4 %08X %08X %08X %d %d\n",
            me,
            (int)lpRect,
            (int)lpSurfaceDesc,
            dwFlags,
            hEvent) );

  if( (NULL==lpSurfaceDesc) ||
      (NULL!=hEvent)
    )
  {
    dprintf(("DDERR_INVALIDPARAMS"));
    return DDERR_INVALIDPARAMS;
  }

  if (NULL!=lpRect)
    pIRectNew = new DDRectangle( lpRect->left, lpRect->top, lpRect->right, lpRect->bottom );
  else
    pIRectNew = new DDRectangle( 0, 0, me->width, me->height);

  // ToDo : the lockchecking should be done in a critcal seq.
  dprintf( ("Lock Rectangle (%d/%d) x (%d/%d)\n",
            pIRectNew->Left(),
            pIRectNew->Top(),
            pIRectNew->Right(),
            pIRectNew->Bottom()));

  rc = DD_OK;

  if(me->fLocked)
  {
    if (NULL==lpRect)
    {
      // If anything is locked we can't lock the complete surface
      dprintf(("DDRAW: Surface has locked Rectangles and we want to completely lock it\n"));
      Found = TRUE;
    }
    else
    {
      // If the new Rectangle intersects with any of the already locked rectangles it can't
      // be locked so check for this

      dprintf(("DDRAW: Surface has locked Rectangles, check if they overlap\n"));

      i=0;
      Found = FALSE;

      while(i<DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
      {
        pIRectCurrent = (DDRectangle*) DPA_FastGetPtr(me->DPA_LockedRects,i);
        dprintf( ("Test with Rectangle (%d/%d) x (%d/%d)\n",
                  pIRectCurrent->Top(),
                  pIRectCurrent->Left(),
                  pIRectCurrent->Bottom(),
                  pIRectCurrent->Right() ));
        Found = pIRectCurrent->intersects(*pIRectNew);
        i++;
      }

    }

    if (Found)
    {
      delete pIRectNew;
      dprintf(("DDRAW: SurfLock4: Surface already locked\n\n"));
      rc = DDERR_SURFACEBUSY;
    }

  }

  if(DD_OK == rc)
  {
    memcpy((char *)lpSurfaceDesc, (char *)&me->DDSurfaceDesc, sizeof(DDSURFACEDESC2));

    if(lpRect != NULL)
    {
      lpSurfaceDesc->lpSurface =  (LPVOID)((char*)me->pFrameBuffer +
                                  (lpRect->top * me->dwPitchFB) +
                                  (lpRect->left * (lpSurfaceDesc->ddpfPixelFormat.dwRGBBitCount>>3)));
      dprintf(("DDRAW: SurfLock4 %08X (x,y) = (%d,%d)\n\n", lpSurfaceDesc->lpSurface, lpRect->top, lpRect->left));
    }
    else
    {
      dprintf(("DDRAW: SurfLock4 %08X \n\n", lpSurfaceDesc->lpSurface));
    }
    // Add the rectangle to the list of locked rectangles

    pIRectNew->SetMemPtr(lpSurfaceDesc->lpSurface);

    DPA_InsertPtr( me->DPA_LockedRects,
                   DPA_GetPtrCount(me->DPA_LockedRects),
                   pIRectNew);

#if 0
    if(me->diveBufNr == DIVE_BUFFER_SCREEN)
    {
        OS2RECTL rectOS2;

        rectOS2.xLeft   = pIRectNew->Left();
        rectOS2.yBottom = me->DDSurfaceDesc.dwHeight - pIRectNew->Bottom();
        rectOS2.xRight  = pIRectNew->Right();
        rectOS2.yTop    = me->DDSurfaceDesc.dwHeight - pIRectNew->Top();
        dprintf(("DiveAcquireFrameBuffer (%d,%d)(%d,%d)", rectOS2.xLeft, rectOS2.yBottom, rectOS2.xRight, rectOS2.yTop));
        int ret = DiveAcquireFrameBuffer(me->hDive, (PRECTL)&rectOS2);
        if(ret) {
            dprintf(("ERROR: DiveAcquireFrameBuffer failed with %d", ret));
        }
    }
#endif
    me->fLocked = TRUE;

    if(me->diveBufNr == DIVE_BUFFER_SCREEN)
    {
        //If fHideCursorOnLock is set, then we hide the cursor to prevent
        //the app from corruption the mouse cursor (color/animated pointers)
        if(fHideCursorOnLock) ShowCursor(FALSE);
    }
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetDC(THIS This, HDC FAR *hdc)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 DDSURFACEDESC2        LockedSurfaceDesc;
 HRESULT               rc;
 BITMAP bmpSurface;
 struct
 {
   BITMAPINFOHEADER  bmiHead;
   RGBQUAD           bmiCols[256];
 } BitmapInfo;

  dprintf(("DDRAW: SurfGetDC\n"));

  if (hdc == NULL)
    return(DDERR_INVALIDPARAMS);

  LockedSurfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

  if(DD_OK != me->Vtbl.fnLock(me,NULL,&LockedSurfaceDesc,0,0))
  {
    return(DDERR_DCALREADYCREATED);
  }

  rc = DD_OK;

  if(me->hdcImage == NULL)
  {
    // Create a Device context
    me->hdcImage = CreateCompatibleDC(NULL);
    if(me->hdcImage == NULL)
    {
      dprintf(("DDRAW: Can't create compatible DC!\n"));
      me->Vtbl.fnUnlock(me,NULL);
      rc = DDERR_GENERIC;
    }
  }

  if( (DD_OK==rc) && (me->hbmImage == NULL) )
  {
    OS2IDirectDrawPalette *ddpal = NULL;

    dprintf( ("Trying to create Bitmap (%d/%d) at %d Bit\n",
              LockedSurfaceDesc.dwWidth,
              LockedSurfaceDesc.dwHeight,
              LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount
            ));
    memset(&BitmapInfo, 0, sizeof(BitmapInfo));
    BitmapInfo.bmiHead.biSize     = sizeof(BITMAPINFOHEADER);
    BitmapInfo.bmiHead.biWidth    = LockedSurfaceDesc.dwWidth;
    BitmapInfo.bmiHead.biHeight   = -LockedSurfaceDesc.dwHeight;
    BitmapInfo.bmiHead.biPlanes   = 1;
    BitmapInfo.bmiHead.biBitCount = (WORD)LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

    switch(LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
    {
      case 1:
      case 4:
        dprintf(("DDRAW: 1/4 Bit Not yet supported\n"));
        break;
      case 8:
        BitmapInfo.bmiHead.biCompression = BI_RGB;
        if (me->lpPalette != NULL)
           ddpal = me->lpPalette;
        if ((me->FrontBuffer != NULL) && (me->FrontBuffer->lpPalette != NULL))
           ddpal = me->FrontBuffer->lpPalette;
        if (ddpal != NULL) {
           ddpal->Vtbl.fnGetEntries((IDirectDrawPalette*)ddpal,
              0, 0, 256, (PPALETTEENTRY)&BitmapInfo.bmiCols[0]);
        }
        else {
           dprintf(("DDRAW: Using default palette\n"));
           for (DWORD i = 0; i < 256; i++) {
              BitmapInfo.bmiCols[i].rgbBlue  = DefaultPalette[i*3+2];
              BitmapInfo.bmiCols[i].rgbGreen = DefaultPalette[i*3+1];
              BitmapInfo.bmiCols[i].rgbRed   = DefaultPalette[i*3];
              BitmapInfo.bmiCols[i].rgbReserved = 0;
           }
        }
        me->hbmImage = CreateDIBitmap( me->hdcImage,
                                       &BitmapInfo.bmiHead,
                                       CBM_INIT,
                                       LockedSurfaceDesc.lpSurface,
                                       (PBITMAPINFO)&BitmapInfo,
                                       DIB_RGB_COLORS);
        break;

      case 16:
      case 24:
      case 32:
        BitmapInfo.bmiHead.biCompression = BI_BITFIELDS;
        BitmapInfo.bmiHead.biClrUsed     = 3;
        *((DWORD *) &(BitmapInfo.bmiCols[0])) = me->DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
        *((DWORD *) &(BitmapInfo.bmiCols[1])) = me->DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
        *((DWORD *) &(BitmapInfo.bmiCols[2])) = me->DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
        me->hbmImage = CreateDIBitmap( me->hdcImage,
                                       &BitmapInfo.bmiHead,
                                       CBM_INIT,
                                       LockedSurfaceDesc.lpSurface,
                                       (PBITMAPINFO)&BitmapInfo,
                                       DIB_RGB_COLORS );
        break;

      default:
        dprintf( ("Unexpected BitCount %d \n",
                    LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount));
        me->hbmImage=NULL;
    } // end switch (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)

    if(me->hbmImage == NULL)
    {
      dprintf(("DDRAW: Can't create bitmap!\n"));
      DeleteDC(me->hdcImage);
      me->hdcImage = NULL;
      me->Vtbl.fnUnlock(me,NULL);
      rc = DDERR_GENERIC;
    }
  }
  else
  {
    if( (DD_OK==rc) && (me->dwLastDCUnique != me->dwUniqueValue) )
    {
      dprintf(("DDRAW: The Surface was locked/unlocked after the last DC was created =>Update Bitmap!\n"));

      memset(&BitmapInfo,0, sizeof(BitmapInfo));
      BitmapInfo.bmiHead.biSize     = sizeof(BITMAPINFOHEADER);
      BitmapInfo.bmiHead.biWidth    = LockedSurfaceDesc.dwWidth;
      BitmapInfo.bmiHead.biHeight   = -LockedSurfaceDesc.dwHeight;
      BitmapInfo.bmiHead.biPlanes   = 1;
      BitmapInfo.bmiHead.biBitCount = (WORD)LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

      switch(LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
      {
        case 1:
        case 4:
        case 8:
          BitmapInfo.bmiHead.biCompression = BI_RGB;
          GetSystemPaletteEntries(me->hdcImage,0,255,(PPALETTEENTRY)&BitmapInfo.bmiCols[0]);
          SetDIBits(me->hdcImage, me->hbmImage, 0, LockedSurfaceDesc.dwHeight,
                    me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
          break;
        case 16:
        case 32:
          BitmapInfo.bmiHead.biCompression = BI_BITFIELDS;
          BitmapInfo.bmiHead.biClrUsed     = 3;
          *((DWORD *) &(BitmapInfo.bmiCols[0])) = me->DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
          *((DWORD *) &(BitmapInfo.bmiCols[1])) = me->DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
          *((DWORD *) &(BitmapInfo.bmiCols[2])) = me->DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
          SetDIBits(me->hdcImage, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                    me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
          break;
        case 24:
          BitmapInfo.bmiHead.biCompression = BI_RGB;
          SetDIBits(me->hdcImage, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                    me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
          break;
        default:
          dprintf(("DDRAW: Unexpected BitCount %d => Bitmap not updated!\n",LockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount));
          break;
      } // end switch (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)

    }
  }

  // Allways select the bitmap into the DC! No matter if the old or a new one

  if (DD_OK==rc)
  {
    if ((me->hgdiOld = SelectObject(me->hdcImage, me->hbmImage)) == NULL)
    {
      dprintf(("DDRAW: Can't select bitmap into DC!\n"));
      DeleteDC(me->hdcImage);
      me->hdcImage = NULL;
      DeleteObject(me->hbmImage);
      me->hbmImage = NULL;
      me->Vtbl.fnUnlock(me,NULL);
      rc = DDERR_GENERIC;
    }
    else
    {
      *hdc = me->hdcImage;
    }
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfReleaseDC(THIS This, HDC hdc)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
 struct
 {
   BITMAPINFOHEADER  bmiHead;
   RGBQUAD           bmiCols[256];
 } BitmapInfo;
 int rc;

  dprintf(("DDRAW: SurfReleaseDC\n"));

  if(hdc != me->hdcImage)
    return(DDERR_INVALIDOBJECT);

  //unselect our bitmap
  SelectObject(me->hdcImage, me->hgdiOld);

  memset(&BitmapInfo,0, sizeof(BitmapInfo));
  BitmapInfo.bmiHead.biSize     = sizeof(BITMAPINFOHEADER);
  BitmapInfo.bmiHead.biBitCount = me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
  BitmapInfo.bmiHead.biPlanes   = 1;
  BitmapInfo.bmiHead.biWidth    = me->DDSurfaceDesc.dwWidth; /// (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount>>3);
  BitmapInfo.bmiHead.biHeight   = -me->DDSurfaceDesc.dwHeight;

  switch(me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)
  {
    case 1:
    case 4:
    case 8:
      BitmapInfo.bmiHead.biCompression = BI_RGB;
      //GetSystemPaletteEntries(me->hdcImage,0,255,(PPALETTEENTRY)&BitmapInfo.bmiCols[0]);
      rc = GetDIBits( hdc,
                      me->hbmImage,
                      0,
                      me->DDSurfaceDesc.dwHeight,
                      NULL,
                      (PBITMAPINFO)&BitmapInfo,
                      DIB_RGB_COLORS);
      BitmapInfo.bmiHead.biHeight = -BitmapInfo.bmiHead.biHeight;
      dprintf( ("GetDIBits rc=%d\n Size   :%d\n Width  :%d\n Height :%d\n"
                " Planes :%d\n BitCount :%d\nLastEror = %d\nPixel[0,0] = 0x%02X\n",
                rc,
                BitmapInfo.bmiHead.biSize,
                BitmapInfo.bmiHead.biWidth,
                BitmapInfo.bmiHead.biHeight,
                BitmapInfo.bmiHead.biPlanes,
                BitmapInfo.bmiHead.biBitCount,
                GetLastError(),
                ((char*)me->DDSurfaceDesc.lpSurface)[0]));

      rc = GetDIBits( hdc,
                      me->hbmImage,
                      0,
                      me->DDSurfaceDesc.dwHeight,
                      me->DDSurfaceDesc.lpSurface,
                      (PBITMAPINFO)&BitmapInfo,
                      DIB_RGB_COLORS);
      dprintf( ("GetDIBits rc=%d\n LastEror = %d\nPixel[0,0] = 0x%02X\n",
                rc,
                GetLastError(),
                ((char*)me->DDSurfaceDesc.lpSurface)[0]));
      break;
    case 16:
      BitmapInfo.bmiHead.biCompression = BI_BITFIELDS;
      BitmapInfo.bmiHead.biClrUsed     = 3;
      *((DWORD *) &(BitmapInfo.bmiCols[0])) = me->DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
      *((DWORD *) &(BitmapInfo.bmiCols[1])) = me->DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
      *((DWORD *) &(BitmapInfo.bmiCols[2])) = me->DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
      GetDIBits(hdc, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
      break;

    case 32:
      BitmapInfo.bmiHead.biCompression = BI_BITFIELDS;
      BitmapInfo.bmiHead.biClrUsed     = 3;
      *((DWORD *) &(BitmapInfo.bmiCols[0])) = me->DDSurfaceDesc.ddpfPixelFormat.dwRBitMask;
      *((DWORD *) &(BitmapInfo.bmiCols[1])) = me->DDSurfaceDesc.ddpfPixelFormat.dwGBitMask;
      *((DWORD *) &(BitmapInfo.bmiCols[2])) = me->DDSurfaceDesc.ddpfPixelFormat.dwBBitMask;
      GetDIBits(hdc, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
      break;

    case 24:
      BitmapInfo.bmiHead.biCompression = BI_RGB;
      GetDIBits(hdc, me->hbmImage, 0, me->DDSurfaceDesc.dwHeight,
                me->DDSurfaceDesc.lpSurface,(PBITMAPINFO)&BitmapInfo,DIB_RGB_COLORS);
      break;
    default:
      dprintf(("DDRAW: Unexpected BitCount %d => Surface unlocked but no data copied back\n",me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount));
      // we might could keep the surface locked and return an error but this is more "safe"
      break;
  } // end switch (me->DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount)

  me->Vtbl.fnUnlock(me,NULL);
  me->dwLastDCUnique = me->dwUniqueValue; // Store this to see if the surface was locked after we released the DC

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfRestore(THIS)
{
  dprintf(("DDRAW: SurfRestore\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfSetClipper(THIS This, LPDIRECTDRAWCLIPPER lpClipper)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfSetClipper %x %x", This, lpClipper));

  if(lpClipper == NULL)
  {
    //deattach surface
    if(me->lpClipper)
    {
      me->lpClipper->Vtbl.fnRelease((IDirectDrawClipper*)me->lpClipper);
      me->lpClipper = NULL;
      return(DD_OK);
    }
    else
      return(DDERR_NOCLIPPERATTACHED);
  }

  if(lpClipper == (LPDIRECTDRAWCLIPPER)me->lpClipper)
    return(DD_OK);  //already attached

  if(me->lpClipper != NULL)
  {
    me->lpClipper->Vtbl.fnRelease((IDirectDrawClipper*)me->lpClipper);  //attach other surface
    return(DD_OK);
  }

  me->lpClipper = (OS2IDirectDrawClipper *)lpClipper;
  me->lpClipper->Vtbl.fnAddRef((IDirectDrawClipper*)me->lpClipper);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfSetColorKey(THIS  This, DWORD dwFlags, LPDDCOLORKEY lpDDColKey)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  HRESULT rc;

  dprintf(("DDRAW: SurfSetColorKey %d %08X\n", dwFlags, lpDDColKey));

  if (0==dwFlags)
  {
    return (DDERR_INVALIDPARAMS);
  }

  // as we report only src colorkey in the caps return error on all others flags
  if( (DDCKEY_DESTBLT|DDCKEY_DESTOVERLAY|DDCKEY_SRCOVERLAY|DDCKEY_COLORSPACE) & dwFlags)
  {
    dprintf(("Unspported colorkey\n"));
    return(DDERR_UNSUPPORTED);
  }

  if(DDCKEY_SRCBLT & dwFlags)
  {

    //(me->lpVtbl == me->Vtbl4)
    //  me->Vtbl4->ChangeUniquenessValue(me); // we changed somethin so change this value
    if(NULL!=lpDDColKey)
    {
      dprintf(("copy colorkey"));
      memcpy(&(me->DDSurfaceDesc.ddckCKSrcBlt), lpDDColKey, sizeof(DDCOLORKEY) );
      me->DDSurfaceDesc.dwFlags |= DDCKEY_SRCBLT;

      // ToDo: Generate a maskbitmap for transparent blitting here
    }
    else
    {
      dprintf(("clear colorkey"));
      memset(&(me->DDSurfaceDesc.ddckCKSrcBlt), 0, sizeof(DDCOLORKEY) );
      me->DDSurfaceDesc.dwFlags &= ~DDCKEY_SRCBLT;
    }
    rc = DD_OK;
  }
  else
  {
    dprintf(("Unsupported flags"));
    _dump_DDCOLORKEY(dwFlags);
    rc = DDERR_INVALIDPARAMS; // some other flags where set => error
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfSetPalette(THIS This, LPDIRECTDRAWPALETTE lpPalette)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfSetPalette\n"));

  if(lpPalette == NULL)
  {
    //deattach palette
    if(me->lpPalette)
    {
      // If removed from a primary surface notify
      // palette that it is no longer attached to the
      // primary surface => doesn't modify physical palette
      if(me->surfaceType & DDSCAPS_PRIMARYSURFACE)
      {
        me->lpPalette->SetIsPrimary(FALSE);
      }
      me->lpPalette->Vtbl.fnRelease((IDirectDrawPalette*)me->lpPalette);
      me->lpPalette = NULL;
      return(DD_OK);
    }
    else
      return(DDERR_NOPALETTEATTACHED);
  }

  if(lpPalette == (LPDIRECTDRAWPALETTE)me->lpPalette)
    return(DD_OK);  //already attached

  if(me->lpPalette != NULL)
  {
    me->lpPalette->Vtbl.fnRelease((IDirectDrawPalette*)me->lpPalette);  //attach other palette
    //return(DD_OK);
  }
  me->lpPalette = (OS2IDirectDrawPalette *)lpPalette;
  me->lpPalette->Vtbl.fnAddRef((IDirectDrawPalette*)me->lpPalette);

  // If Attached to a primary surface notify
  // palette that it is attached to the primary surface
  //  => It does modify physical palette.
  // This is important as an palette can be attached to
  // multiple surfaces. If one is the primary surface
  // changes done to it via any surface must result in
  // changes in the phys pal.

  if(me->surfaceType & DDSCAPS_PRIMARYSURFACE)
  {
    me->lpPalette->SetIsPrimary(TRUE);
  }
  // me->lpVtbl->ChangeUniquenessValue(me);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUnlock(THIS This, LPVOID lpSurfaceData)
{

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  DDRectangle *pIRectUnlock, *pIRectEnum;
  BOOL Found = FALSE;
  OS2RECTL CCRect;
  HRESULT rc;

  dprintf(("DDRAW: SurfUnlock at %08X\n",lpSurfaceData));

  if(me->fLocked == FALSE)
  {
    dprintf(("DDRAW: Surface not locked!\n"));
    return(DDERR_NOTLOCKED);
  }

  i=0;
  if(NULL!=lpSurfaceData)
  {
    dprintf(("DDRAW: Buffer Pointer Compare"));

    // We got a pinter to the surface memory so we must search for
    // this pointer in the locked rects DPA to unlock the right rect.

    while(i < DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
    {
      pIRectEnum = (DDRectangle*)DPA_FastGetPtr(me->DPA_LockedRects,i);
      dprintf(( "Test with Rectangle %d (%d/%d) x (%d/%d) Mem at %08X\n",
                i,
                pIRectEnum->Top(),
                pIRectEnum->Left(),
                pIRectEnum->Bottom(),
                pIRectEnum->Right(),
                pIRectEnum->GetMemPtr() ));

      Found = ( pIRectEnum->GetMemPtr() == lpSurfaceData);
      if(!Found)
      {
        dprintf(("DDRAW: Not Found, try Next rect\n"));
        i++;
      }
      else
      {
        dprintf(("DDRAW: Found Rect\n"));
      }
    }
  }
  else
  {
    // If a NULL pointer was passed in the SW tries to unlock the
    // complete surface so we must compare the rects.
    dprintf(("DDRAW: Rectangle compare"));

#if 1
    i = 0;
    pIRectEnum = (DDRectangle*)DPA_FastGetPtr(me->DPA_LockedRects,0);
    Found = (pIRectEnum != NULL);
#else
    pIRectUnlock = new DDRectangle( 0, 0, me->width, me->height);

    while(i<DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
    {
      pIRectEnum = (DDRectangle*)DPA_FastGetPtr(me->DPA_LockedRects,i);
      dprintf(( "Test with Rectangle %d (%d/%d) x (%d/%d) Mem at %08X\n",
                i,
                pIRectEnum->Top(),
                pIRectEnum->Left(),
                pIRectEnum->Bottom(),
                pIRectEnum->Right(),
                pIRectEnum->GetMemPtr() ));

      Found = (*pIRectEnum == *pIRectUnlock);
      if(!Found)
      {
        dprintf(("DDRAW: Not Found, try Next rect\n"));
        i++;
      }
      else
      {
        dprintf(("DDRAW: Found Rect\n"));
      }
    }
    delete pIRectUnlock;
#endif
  }

  if(!Found)
  {
    dprintf(("DDRAW: Rectangle not locked, wrong Rect!\n\n"));
    rc = DDERR_INVALIDRECT;
  }
  else
  {
    dprintf(("DDRAW: Remove Rect %d from Seq.\n",i));

    DPA_DeletePtr(me->DPA_LockedRects,i);

    dprintf(("DDRAW: Test if locked Rects main\n"));

    if(0==DPA_GetPtrCount(me->DPA_LockedRects))  // Do we have unlocked last rectangle
    {
      dprintf(("DDRAW: No Locked Rects left for surface\n"));
      me->fLocked = FALSE;
    }
    else
      dprintf(( "%d Rects in Seq\n",
                DPA_GetPtrCount(me->DPA_LockedRects)));

    if(me->pFrameBuffer != me->pDiveBuffer)
    {
      dprintf(( "ColorConversion Needed %08X != %08X\n",
                me->pFrameBuffer,
                me->pDiveBuffer));
      if(NULL!=lpSurfaceData)
      {
        CCRect.yTop    = pIRectEnum->Top();
        CCRect.xLeft   = pIRectEnum->Left();
        CCRect.xRight  = pIRectEnum->Right();
        CCRect.yBottom = pIRectEnum->Bottom();

        me->ColorConversion( (LPRECT)&CCRect);
      }
      else
      {
        me->ColorConversion(NULL);
      }
    }
    else
      dprintf( ("No ColorConversion Needed"));


#if 0
    if(me->diveBufNr == DIVE_BUFFER_SCREEN)
    {
        OS2RECTL rectOS2;

        rectOS2.xLeft   = pIRectEnum->Left();
        rectOS2.yBottom = me->DDSurfaceDesc.dwHeight - pIRectEnum->Bottom();
        rectOS2.xRight  = pIRectEnum->Right();
        rectOS2.yTop    = me->DDSurfaceDesc.dwHeight - pIRectEnum->Top();
        dprintf(("DiveDeacquireFrameBuffer (%d,%d)(%d,%d)", rectOS2.xLeft, rectOS2.yBottom, rectOS2.xRight, rectOS2.yTop));
        int ret = DiveDeacquireFrameBuffer(me->hDive);
        if(ret) {
            dprintf(("ERROR: DiveDeacquireFrameBuffer failed with %d", ret));
        }
    }
#endif

    // delete tne DDRectobject of the found rectangle
    delete pIRectEnum;

    // me->lpVtbl->ChangeUniquenessValue(me);

    dprintf(("DDRAW: Unlock OK\n\n"));

    rc = DD_OK;

    if(me->diveBufNr == DIVE_BUFFER_SCREEN)
    {
        //If fHideCursorOnLock is set, then we hide the cursor to prevent in SurfLock4
        //the app from corruption the mouse cursor (color/animated pointers)
        //We need to show it again here
        if(fHideCursorOnLock) ShowCursor(TRUE);
    }
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUnlock4(THIS This, LPRECT lpSurfaceRect)
{
  // MS changed the parameter from LPVOID to LPRECT with DX6
  // as DX-6 allways returns a DDSurface4 on create surface this
  // is a problem i not NULL is passed in.
  // Solution We first test with the pointer ns assuming a rectangle
  // if we don't find a rect Which is very likely if we get a pointer
  // SurfaceMemory we call SurfUnlock and test for the pointer there.


  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  DDRectangle *pIRectUnlock, *pIRectEnum;
  BOOL Found = FALSE;
  HRESULT rc;

  dprintf(("DDRAW: SurfUnlock4\n"));

  if(me->fLocked == FALSE)
  {
    dprintf(("DDRAW: Surface not locked!\n"));
    return(DDERR_NOTLOCKED);
  }

  if(NULL!=lpSurfaceRect)
  {
    dprintf(("DDRAW: Unlock rectangle\n"));
    pIRectUnlock = new DDRectangle( lpSurfaceRect->left, lpSurfaceRect->top,
                                    lpSurfaceRect->right, lpSurfaceRect->bottom);
  }
  else
  {
    dprintf(("DDRAW: Unlock complete surface\n"));
    pIRectUnlock = new DDRectangle( 0, 0, me->width, me->height);
  }

  dprintf(( "Try to Unlock Rectangle (%d/%d) x (%d/%d)\n",
            pIRectUnlock->Top(),
            pIRectUnlock->Left(),
            pIRectUnlock->Bottom(),
            pIRectUnlock->Right() ));

  dprintf(("DDRAW: Start Enumeration of Locked Rects\n"));

  i=0;
  while(i<DPA_GetPtrCount(me->DPA_LockedRects) && !Found)
  {
    pIRectEnum = (DDRectangle*)DPA_FastGetPtr(me->DPA_LockedRects,i);
    dprintf(( "Test with Rectangle (%d/%d) x (%d/%d)\n",
              pIRectEnum->Top(),
              pIRectEnum->Left(),
              pIRectEnum->Bottom(),
              pIRectEnum->Right() ));

    Found = (*pIRectEnum == *pIRectUnlock);
    if(!Found)
    {
      dprintf(("DDRAW: Not Found, try Next rect\n"));
      i++;
    }
    else
    {
      dprintf(("DDRAW: Found Rect\n"));
    }
  }

  if(!Found)
  {

    if(NULL==lpSurfaceRect)
    {
      dprintf(("DDRAW: Rectangle not locked, wrong Rect!\n\n"));
      return(DDERR_INVALIDRECT);
    }
    else
      rc = SurfUnlock(This, (LPVOID)lpSurfaceRect);
  }
  else
  {
    dprintf(("DDRAW: Remove Rect from Seq.\n"));

    DPA_DeletePtr(me->DPA_LockedRects,i);

    if(0==DPA_GetPtrCount(me->DPA_LockedRects))  // Do we have unlocked last rectangle
    {
      dprintf(("DDRAW: No Locked Rects left for surface\n"));
      me->fLocked = FALSE;
    }

    if(me->pFrameBuffer != me->pDiveBuffer)
    {
      dprintf(( "ColorConversion Needed %08X != %08X\n",
                me->pFrameBuffer,
                me->pDiveBuffer));
      me->ColorConversion(lpSurfaceRect);
    }

    // me->lpVtbl->ChangeUniquenessValue(me);

    dprintf(("DDRAW: Unlock OK\n\n"));
    rc = DD_OK;
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetDDInterface(THIS This, LPVOID FAR *lplpDirectDraw)
{
 OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfGetDDInterface\n"));
  *lplpDirectDraw = (LPVOID FAR *)me->lpDraw;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfPageLock(THIS, DWORD)
{
  // Only used for DMA memory access
  // If we implement this for the None dive buffers with a pdd the we must change
  // from malloc to DosAllocMem and use OBJ_TILE flag
  dprintf(("DDRAW: SurfPageLock\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfPageUnlock(THIS, DWORD)
{
  dprintf(("DDRAW: SurfPageUnlock\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
// V3 Interface Functions

HRESULT WIN32API SurfSetSurfaceDesc(THIS This, LPDDSURFACEDESC lpSurfDesc, DWORD dwFlags)
{
  dprintf(("DDRAW: SurfSetSurfaceDesc\n"));

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  if ( (NULL==lpSurfDesc) || (dwFlags!=0) )
    return DDERR_INVALIDPARAMS;

  // Is this ok ? Not sure if Front/BackBuffer should be handled like the Primary one.
  //  if ( ( DDSCAPS_PRIMARYSURFACE == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_FRONTBUFFER == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_BACKBUFFER  == me->DDSurfaceDesc.ddsCaps.dwCaps) )
  if(-1==me->diveBufNr)
    return DDERR_INVALIDSURFACETYPE; // only work for system alloced surfaces

  if (!me->Updated)
  {
    me->Updated = TRUE;
    // free our allocated Memory
    if(me->DDSurfaceDesc.dwFlags & DDSD_LPSURFACE)
    {
      if(me->pFBreal)
        free(me->pFBreal);
      if(me->pDBreal)
        free(me->pFBreal);
    }
  }
  // me->lpVtbl->ChangeUniquenessValue(me);
  memcpy( (char*)&(me->DDSurfaceDesc),
          (char*)lpSurfDesc,
          sizeof(DDSURFACEDESC));

  me->dwPitchFB = me->DDSurfaceDesc.lPitch;

  if( me->lpDraw->dCaps.ulDepth != me->lpDraw->GetScreenBpp() )
  {
    // create CC buffer ....
    me->dwPitchDB = (me->DDSurfaceDesc.dwWidth * me->dwBytesPPDive +7) & ~7;
    me->pDBreal = (char*)malloc( me->DDSurfaceDesc.dwHeight * me->dwPitchDB + 24);
    me->pDiveBuffer = (char*)(((int)me->pDBreal + 7) & ~7); // align to QWORD
  }
  return DD_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfSetSurfaceDesc4(THIS This, LPDDSURFACEDESC2 lpSurfDesc, DWORD dwFlags)
{
  dprintf(("DDRAW: SurfSetSurfaceDesc4\n"));

  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  if ( (NULL==lpSurfDesc) || (dwFlags!=0) )
    return DDERR_INVALIDPARAMS;

  // Is this ok ? Not sure if Front/BackBuffer should be handled like the Primary one.
  //  if ( ( DDSCAPS_PRIMARYSURFACE == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_FRONTBUFFER == me->DDSurfaceDesc.ddsCaps.dwCaps) ||
  //       ( DDSCAPS_BACKBUFFER  == me->DDSurfaceDesc.ddsCaps.dwCaps) )
  if(-1==me->diveBufNr)
    return DDERR_INVALIDSURFACETYPE; // only work for system alloced surfaces

  if (!me->Updated)
  {
    me->Updated = TRUE;
    // free our allocated Memory
    // free our allocated Memory
    if(me->DDSurfaceDesc.dwFlags & DDSD_LPSURFACE)
    {
      if(me->pFBreal)
        free(me->pFBreal);
      if(me->pDBreal)
        free(me->pFBreal);
    }
  }
  // me->lpVtbl->ChangeUniquenessValue(me);
  memcpy( (char *)&(me->DDSurfaceDesc),
          (char*)lpSurfDesc,
          sizeof(DDSURFACEDESC2));
  me->dwPitchFB = me->DDSurfaceDesc.lPitch;

  if( me->lpDraw->dCaps.ulDepth != me->lpDraw->GetScreenBpp() )
  {
    // create CC buffer ....
    me->dwPitchDB = (me->DDSurfaceDesc.dwWidth * me->dwBytesPPDive +7) & ~7;
    me->pDBreal = (char*)malloc( me->DDSurfaceDesc.dwHeight * me->dwPitchDB + 24);
    me->pDiveBuffer = (char*)(((int)me->pDBreal + 7) & ~7); // align to QWORD
  }

  return DD_OK;
}
//******************************************************************************
//******************************************************************************
// V4 Interface Functions

HRESULT WIN32API SurfSetPrivateData(THIS This, REFGUID refGUID, LPVOID lpData,
                                     DWORD dwDataSize, DWORD dwFlags)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;
  void *pBuffer;
  BOOL bFound = FALSE;
  HRESULT rc;

  dprintf(("DDRAW: SurfSetPrivateData\n"));

  if(NULL==me)
    return(DDERR_INVALIDOBJECT);

  if((NULL==lpData)||(0==dwDataSize)||
     (dwFlags & ~(DDSPD_IUNKNOWNPOINTER|DDSPD_VOLATILE)))
    return(DDERR_INVALIDPARAMS);

  // first check if the refGUID is stored as then the content will be updated
  if( DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0 )
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData) && !bFound)
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);

      if (IsEqualGUID(&pSData->guidTag,refGUID))
        bFound = TRUE;

      i++;
    }
  }

  if(bFound)
  {
    // update Private Data

    if (!pSData->isValid)
    {
      // Current data is invalid we need to update/allocate

      if(dwFlags & DDSPD_IUNKNOWNPOINTER)
      {
        pSData->pData = lpData;
        pSData->dwSize = 4;
        pSData->dwFlags = dwFlags;
        pSData->isValid = TRUE;
        ((OS2IDirectDrawSurface *) lpData)->lpVtbl->fnAddRef(lpData);
      }
      else
      {
        pSData->pData = malloc(dwDataSize);
        if(NULL!=pSData->pData)
        {
          memcpy(pSData->pData,lpData,dwDataSize);
          pSData->dwSize  = dwDataSize;
          pSData->dwFlags = dwFlags;
          pSData->isValid = TRUE;
        }
        else
        {
          delete pSData;
          rc = DDERR_OUTOFMEMORY;
        }
      }
    }
    else
    {
      if(pSData->dwFlags & DDSPD_IUNKNOWNPOINTER)
      {
        if(dwFlags & DDSPD_IUNKNOWNPOINTER)
        {
          if(pSData->pData != lpData)
          {
            // Change of IUNKOWNPOINTER => release old and add ref to new one
            ((OS2IDirectDrawSurface *)pSData->pData)->lpVtbl->fnRelease(pSData->pData);
            ((OS2IDirectDrawSurface *)lpData)->lpVtbl->fnAddRef(lpData);
            pSData->pData = lpData;
          }
          pSData->dwFlags = dwFlags; // Update the flags, size is the same
        }
        else
        {
          // Replace IUNKOWN through data
          pBuffer = malloc(dwDataSize); // get new buffer first
          if(NULL!=pBuffer)
          {
            // release old ref and copy data
            ((OS2IDirectDrawSurface *)pSData->pData)->lpVtbl->fnRelease(pSData->pData);
            memcpy(pBuffer,lpData,dwDataSize);
            pSData->pData = pBuffer;
            pSData->dwSize = dwDataSize; // Update the size
            pSData->dwFlags = dwFlags;   // Update the flags
          }
          else
            rc = DDERR_OUTOFMEMORY;
        }
      }
      else
      {
        if(dwFlags & DDSPD_IUNKNOWNPOINTER)
        {
          // Change of data to IUNKOWNPOINTER => free old memory and add ref to new one
          free(pSData->pData);
          ((OS2IDirectDrawSurface *)lpData)->lpVtbl->fnAddRef(lpData);
          pSData->pData = lpData;
          pSData->dwSize = dwDataSize; // Update the size
          pSData->dwFlags = dwFlags;   // Update the flags
        }
        else
        {
          //  Update/Replace data
          if(pSData->dwSize!=dwDataSize)
            pBuffer = realloc(pSData->pData,dwDataSize); // update buffer to new size
          else
            pBuffer = pSData->pData;

          if(NULL!=pBuffer)
          {
            // release old ref and copy data
            memcpy(pBuffer,lpData,dwDataSize);
            pSData->pData = pBuffer;
            pSData->dwSize = dwDataSize; // Update the size
            pSData->dwFlags = dwFlags;   // Update the flags
          }
          else
            rc = DDERR_OUTOFMEMORY;
        }
      }
    }
  }
  else
  {
    // New data

    pSData = new(SURFPRIVATEDATA);
    if (NULL!=pSData)
    {
      if(dwFlags & DDSPD_IUNKNOWNPOINTER)
      {
        memcpy(&(pSData->guidTag),&refGUID,sizeof(GUID));
        pSData->pData = lpData;
        pSData->dwSize = 4;
        pSData->dwFlags = dwFlags;
        pSData->isValid = TRUE;
        ((OS2IDirectDrawSurface *)lpData)->lpVtbl->fnAddRef(lpData);
      }
      else
      {
        pSData->pData = malloc(dwDataSize);
        if(NULL!=pSData->pData)
        {
          memcpy(&(pSData->guidTag),&refGUID,sizeof(GUID));
          memcpy(pSData->pData,lpData,dwDataSize);
          pSData->dwSize  = dwDataSize;
          pSData->dwFlags = dwFlags;
          pSData->isValid = TRUE;

          if( DPA_InsertPtr( me->DPA_SurfacePrivateData,
                             DPA_GetPtrCount(me->DPA_SurfacePrivateData),
                             pSData) <0)
          {
            delete(pSData);
            rc = DDERR_OUTOFMEMORY;
          }
        }
        else
        {
          delete(pSData);
          rc = DDERR_OUTOFMEMORY;
        }
      }

    }
    else
      rc = DDERR_OUTOFMEMORY;
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetPrivateData(THIS This, REFGUID refGUID, LPVOID lpData, LPDWORD lpDataSize)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;
  HRESULT rc;
  BOOL bFound = FALSE;

  dprintf(("DDRAW: SurfGetPrivateData\n"));

  if(NULL==me)
    return(DDERR_INVALIDOBJECT);

  if((NULL==lpData)||(NULL==lpDataSize))
    return(DDERR_INVALIDPARAMS);

  if(DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0)
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData) && !bFound)
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);

      if (IsEqualGUID(&pSData->guidTag,refGUID))
        bFound = TRUE;

      i++;
    }
  }

  if(bFound)
  {
    if(!pSData->isValid)
    {
      rc =DDERR_EXPIRED;
    }
    else
    {
      if(pSData->dwSize > *lpDataSize)
      {
        // Buffer to small return needed Size
        *lpDataSize = pSData->dwSize;
        rc = DDERR_MOREDATA;
      }
      else
      {
        memcpy(lpData,pSData->pData,pSData->dwSize);
        rc = DD_OK;
      }
    }
  }
  else
   rc = DDERR_NOTFOUND;


  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfFreePrivateData(THIS This, REFGUID refGUID)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;
  BOOL bFound = FALSE;

  dprintf(("DDRAW: SurfFreePrivateData\n"));

  if(NULL==me)
    return(DDERR_INVALIDOBJECT);

  if(DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0)
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData) && !bFound)
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);

      if (IsEqualGUID(&pSData->guidTag,refGUID))
      {
        bFound = TRUE;

        if(pSData->isValid)
        {
          // delete the data if valid
          if (pSData->dwFlags & DDSPD_IUNKNOWNPOINTER)
          {
            // pointer to com stored so calll its release
            ((OS2IDirectDrawSurface *) pSData->pData)->lpVtbl->fnRelease(pSData->pData);
          }
          else
          {
            // Free allocated data
            free( pSData->pData);
          }
        }
        // Now remove the entry from the list
        DPA_DeletePtr(me->DPA_SurfacePrivateData,i);
      }
      i++;
    }
  }

  return (bFound?DD_OK:DDERR_NOTFOUND);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetUniquenessValue(THIS This, LPDWORD lpValue)
{
  dprintf(("DDRAW: SurfGetUniquenessValue\n"));
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  if (NULL==lpValue)
    return DDERR_INVALIDPARAMS;

  *lpValue = me->dwUniqueValue;
  return DD_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfChangeUniquenessValue(THIS This)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  int i;
  PSURFPRIVATEDATA pSData;


  dprintf(("DDRAW: SurfChangeUniquenessValue\n"));
  me->dwUniqueValue++;

  if(DPA_GetPtrCount(me->DPA_SurfacePrivateData)>0)
  {
    i=0;
    while(i<DPA_GetPtrCount(me->DPA_SurfacePrivateData))
    {
      pSData = (PSURFPRIVATEDATA) DPA_FastGetPtr(me->DPA_SurfacePrivateData,i);
      if (pSData->dwFlags & DDSPD_VOLATILE)
      {
        // Data becomes unvalid after a Surface change
        if (pSData->dwFlags & DDSPD_IUNKNOWNPOINTER)
        {
          // pointer to com stored so call its release
          ((OS2IDirectDrawSurface *) pSData->pData)->lpVtbl->fnRelease(pSData->pData);
        }
        else
        {
          // Free allocated data
          free( pSData->pData);
        }
        pSData->pData = NULL;
        pSData->isValid = FALSE; // set flag to invalid
      }
      i++;
    }
  }

  return (DD_OK);
}

//******************************************************************************
//******************************************************************************
IDirectDrawSurface4Vtbl DDrawSurfaceV4Table =
{
 SurfQueryInterface,
 SurfAddRef,
 SurfRelease,
 SurfAddAttachedSurface4,
 SurfAddOverlayDirtyRect,
 SurfBlt4,
 SurfBltBatch,
 SurfBltFast4,
 SurfDeleteAttachedSurface4,
 SurfEnumAttachedSurfaces4,
 SurfEnumOverlayZOrders4,
 SurfFlip4,
 SurfGetAttachedSurface4,
 SurfGetBltStatus,
 SurfGetCaps4,
 SurfGetClipper,
 SurfGetColorKey,
 SurfGetDC,
 SurfGetFlipStatus,
 SurfGetOverlayPosition,
 SurfGetPalette,
 SurfGetPixelFormat,
 SurfGetSurfaceDesc4,
 SurfInitialize4,
 SurfIsLost,
 SurfLock4,
 SurfReleaseDC,
 SurfRestore,
 SurfSetClipper,
 SurfSetColorKey,
 SurfSetOverlayPosition,
 SurfSetPalette,
 SurfUnlock4,
 SurfUpdateOverlay4,
 SurfUpdateOverlayDisplay,
 SurfUpdateOverlayZOrder4,
 SurfGetDDInterface,
 SurfPageLock,
 SurfPageUnlock,
 SurfSetSurfaceDesc4,
 SurfSetPrivateData,
 SurfGetPrivateData,
 SurfFreePrivateData,
 SurfGetUniquenessValue,
 SurfChangeUniquenessValue
};
//******************************************************************************
//******************************************************************************
IDirectDrawSurface3Vtbl DDrawSurfaceV3Table =
{
 SurfQueryInterface,
 SurfAddRef,
 SurfRelease,
 SurfAddAttachedSurface3,
 SurfAddOverlayDirtyRect,
 SurfBlt3,
 SurfBltBatch,
 SurfBltFast3,
 SurfDeleteAttachedSurface3,
 SurfEnumAttachedSurfaces,
 SurfEnumOverlayZOrders,
 SurfFlip3,
 SurfGetAttachedSurface3,
 SurfGetBltStatus,
 SurfGetCaps,
 SurfGetClipper,
 SurfGetColorKey,
 SurfGetDC,
 SurfGetFlipStatus,
 SurfGetOverlayPosition,
 SurfGetPalette,
 SurfGetPixelFormat,
 SurfGetSurfaceDesc,
 SurfInitialize,
 SurfIsLost,
 SurfLock,
 SurfReleaseDC,
 SurfRestore,
 SurfSetClipper,
 SurfSetColorKey,
 SurfSetOverlayPosition,
 SurfSetPalette,
 SurfUnlock,
 SurfUpdateOverlay3,
 SurfUpdateOverlayDisplay,
 SurfUpdateOverlayZOrder3,
 SurfGetDDInterface,
 SurfPageLock,
 SurfPageUnlock,
 SurfSetSurfaceDesc
};
//******************************************************************************
//******************************************************************************
IDirectDrawSurface2Vtbl DDrawSurfaceV2Table =
{
 SurfQueryInterface,
 SurfAddRef,
 SurfRelease,
 SurfAddAttachedSurface,
 SurfAddOverlayDirtyRect,
 SurfBlt,
 SurfBltBatch,
 SurfBltFast,
 SurfDeleteAttachedSurface,
 SurfEnumAttachedSurfaces,
 SurfEnumOverlayZOrders,
 SurfFlip,
 SurfGetAttachedSurface,
 SurfGetBltStatus,
 SurfGetCaps,
 SurfGetClipper,
 SurfGetColorKey,
 SurfGetDC,
 SurfGetFlipStatus,
 SurfGetOverlayPosition,
 SurfGetPalette,
 SurfGetPixelFormat,
 SurfGetSurfaceDesc,
 SurfInitialize,
 SurfIsLost,
 SurfLock,
 SurfReleaseDC,
 SurfRestore,
 SurfSetClipper,
 SurfSetColorKey,
 SurfSetOverlayPosition,
 SurfSetPalette,
 SurfUnlock,
 SurfUpdateOverlay,
 SurfUpdateOverlayDisplay,
 SurfUpdateOverlayZOrder,
 SurfGetDDInterface,
 SurfPageLock,
 SurfPageUnlock
};
//******************************************************************************
//******************************************************************************
