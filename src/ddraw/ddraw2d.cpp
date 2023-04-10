/* $Id: ddraw2d.cpp,v 1.5 2002-12-29 14:11:01 sandervl Exp $ */

/*
 * DX Draw base class implementation
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1999 Markus Montkowski
 * Copyright 2000 Przemyslaw Dobrowolski
 * Copyright 2001 Michal Necasek
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
#include <algorithm>
#include <winuser32.h>

#define CINTERFACE
#define INITGUID
#include "ddraw2d.h"
#include "clipper.h"
#include "palette.h"
#include "surface.h"

#include <misc.h>
#include <string.h>
#include <winreg.h>
#include <winuser.h>
#include <winerror.h>
#include <options.h>
#include "os2util.h"
#include "os2fsdd.h"
// include with the videomodes we support
// better would be to get these modes from the card
// But for now we use standard VESA 2.0 modes with 70Hz
#include "os2ddrawmodes.h"
#include "os2DDWindow.h"
#include "os2palset.h"

#define KEY_DIRECT2 "\\Software\\Win32OS2\\Direct2"
#define KEY_DIRECT2DRAW "\\Software\\Win32OS2\\Direct2\\Draw"


BOOL bUseFSDD          = FALSE;
BOOL fNoFSDD           = FALSE;
BOOL fHideCursorOnLock = FALSE;

FOURCC SupportedFourCCs[]   = {FOURCC_SCRN,FOURCC_LUT8,FOURCC_R565,FOURCC_RGB3,FOURCC_RGB4};

#define MAX_DIVE_FOURCC   32
int    nrOfDiveFourCCs              = 0;
FOURCC DiveFOURCCs[MAX_DIVE_FOURCC] = {0};

extern IDirectDrawVtbl  DDrawV1Table;
extern IDirectDraw2Vtbl DDrawV2Table;
extern IDirectDraw4Vtbl DDrawV4Table;
extern IDirect3DVtbl    DDraw3DTable;

//******************************************************************************
//Force DirectDraw to report only one fullscreen mode (useful for custom builds)
//******************************************************************************
void WIN32API SetCustomFullScreenMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
    NUM_MODES_DIVE = 1;

    dprintf(("SetCustomFullScreenMode (%d,%d) bpp %d", dwWidth, dwHeight, dwBPP));
    ModesDive[0].iXRes  = dwWidth;
    ModesDive[0].iYRes  = dwHeight;
    ModesDive[0].iBits  = dwBPP;
    ModesDive[0].iRatio = 0;
    fNoFSDD = TRUE;
}
//******************************************************************************
//Force DirectSurface Lock & Unlock methods to hide & show the mouse cursor
//******************************************************************************
BOOL WIN32API SetCustomHideCursorOnLock(BOOL state)
{
    BOOL preState = fHideCursorOnLock;
    fHideCursorOnLock = state;
    return preState;
}
//******************************************************************************
//******************************************************************************
OS2IDirectDraw::OS2IDirectDraw(GUID *lpGUID) :
     Referenced(0), lastError(DD_OK),
     pFrameBuffer(NULL), hwndClient(0), screenwidth(640),
     screenheight(480), screenbpp(8),PrimaryExists(FALSE),pPrimSurf(NULL)

{
  HKEY  hkDirectDraw2;
  DWORD dwVSize, dwVType;
  ULONG rc;

  // Setup table for 3d devices
  Vtbl3D = DDraw3DTable;

  // Copy Interface tables
  Vtbl  = DDrawV1Table;
  Vtbl2 = DDrawV2Table;
  Vtbl4 = DDrawV4Table;


  if(lpGUID && IsEqualGUID(lpGUID, &IID_IDirect3D))
  {
    dprintf(("DDRAW: D3D Interface -> using V4 Interface"));

    lpVtbl = &Vtbl4;
  }
  else
  {
    if(lpGUID && IsEqualGUID(lpGUID, &IID_IDirectDraw4))
    {
      dprintf(("DDRAW: V4 Interface\n"));
      lpVtbl = &Vtbl4;
    }
    else
    {
      dprintf(("DDRAW: <V4 Interface\n"));
      lpVtbl = (IDirectDraw4Vtbl *) &Vtbl;
    }
  }

  lpVtbl2 = lpVtbl;

  pdwUnknownData = (DWORD*) malloc (255*sizeof(DWORD));
  int i;
  for(i= 0 ;i<255;i++)
    pdwUnknownData[i] = 0;

  rc = DiveOpen( &hDive,
                 FALSE,
                 &pFrameBuffer);
  if(rc)
  {
    dprintf(("DDRAW: ERROR: DiveOpen returned %d\n", rc));
    lastError = DDERR_GENERIC;
    hDive     = NULL;
  }
  else
  {
    dprintf(("DDRAW: DiveOpen OK Framebuufer at 0x%08X\n",pFrameBuffer));
  }
  memset( &dCaps,
          0,
          sizeof(DIVE_CAPS) );
  dCaps.ulStructLen = sizeof(DIVE_CAPS);
  dCaps.ulFormatLength = sizeof(DiveFOURCCs);
  dCaps.pFormatData    = &DiveFOURCCs[0];

  rc = DiveQueryCaps( &dCaps, DIVE_BUFFER_SCREEN);

  dprintf(("DDRAW: DiveQueryCaps rc=0x%08X\n",rc));

  FOURCC *pFourCC = (FOURCC *)dCaps.pFormatData;
  nrOfDiveFourCCs = dCaps.ulInputFormats;
  for(i=0;i<dCaps.ulInputFormats+dCaps.ulOutputFormats;i++) {
      if(i < dCaps.ulInputFormats) {
           dprintf(("INPUT FOURCC %C%C%C%C", (char)(pFourCC[i]), (char)(pFourCC[i] >> 8), (char)(pFourCC[i] >> 16), (char)(pFourCC[i] >> 24)));
      }
      else dprintf(("OUTPUT FOURCC %C%C%C%C", (char)(pFourCC[i]), (char)(pFourCC[i] >> 8), (char)(pFourCC[i] >> 16), (char)(pFourCC[i] >> 24)));
  }

  pdwUnknownData[235] = dCaps.ulHorizontalResolution;
  pdwUnknownData[236] = dCaps.ulVerticalResolution;
  pdwUnknownData[241] = dCaps.ulDepth;

  screenwidth  = dCaps.ulHorizontalResolution;
  screenheight = dCaps.ulVerticalResolution;
  screenbpp    = dCaps.ulDepth;

  // Shall we run in FS mode ?
  if(ERROR_SUCCESS==RegOpenKeyA(HKEY_LOCAL_MACHINE,KEY_DIRECT2DRAW,&hkDirectDraw2))
  {
    dwVSize = 4;
    dwVType = REG_DWORD;
    if(ERROR_SUCCESS!=RegQueryValueExA(hkDirectDraw2,"Fullscreen",NULL,&dwVType,(LPBYTE)&bScale,&dwVSize))
      bScale = FALSE;
  }
  else
    bScale = FALSE;

  // Check to see if FS DDraw is enabled in ODIN.INI
  if (!fNoFSDD && PROFILE_GetOdinIniBool(ODINDDRAW_SECTION, "Fullscreen", FALSE))
  {
#if 0
      rc = InitIO();

      if(rc)  // try to get IOPL for the thread
      {
          dprintf(("DDRAW: No IOPL"));
      }
      else
      {
          dprintf(("DDRAW: IOPL 3!"));
      }
#endif

      rc = LoadPMIService();
      dprintf(("DDRAW: LoadPMIService() rc=0x%08X",rc));

      if (!rc) {
          SetUpModeTable();

   	  bUseFSDD = TRUE;
          dprintf(("DDRAW: Fullscreen enabled",rc));
      }
  }
}
//******************************************************************************
//******************************************************************************
OS2IDirectDraw::~OS2IDirectDraw()
{
  dprintf(("DDRAW: OS2IDirectDraw::~OS2IDirectDraw()"));

  if (hDive)
    DiveClose(hDive);

  // Safety call in case the program did set the pal to all black
  // so if the destructor gets called we might be able change this back
  OS2ResetPhysPalette();

  // FSDDRAW additions
  // we need this in Full Screen, but don't do anything in windowed
  // session.
  if (bUseFSDD) {
    RestorePM();
    FreeModeTable();
  }
}
//******************************************************************************
//******************************************************************************
FOURCC OS2IDirectDraw::GetScreenFourCC()
{
    return SupportedFourCCs[screenbpp>>3];
}
//******************************************************************************
//******************************************************************************
BOOL OS2IDirectDraw::IsFourCCSupported(FOURCC fourcc)
{
    for(int i=0;i<nrOfDiveFourCCs;i++) {
        if(DiveFOURCCs[i] == fourcc) {
            return TRUE;
        }
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: OS2IDirectDraw::QueryInterface %x %x %x", This, riid, ppvObj));

  *ppvObj = NULL;

  if(!IsEqualGUID(riid, &CLSID_DirectDraw) &&
     !IsEqualGUID(riid, &IID_IDirectDraw) &&
     !IsEqualGUID(riid, &IID_IDirectDraw2) &&
     !IsEqualGUID(riid, &IID_IDirectDraw4) &&
     !IsEqualGUID(riid, &IID_IDirect3D))
//     !IsEqualGUID(riid, IID_IUnknown))
  return E_NOINTERFACE;

  // ToDo Better way of returning differnent interfaces for same class
  if(IsEqualGUID(riid, &IID_IDirect3D))
  {
    me->lpVtbl3D         = &me->Vtbl3D;
    me->lpVtbl3D2        = me->lpVtbl3D;
    me->pdwUnknownData3D = me->pdwUnknownData;
    *ppvObj = (LPVOID)&me->lpVtbl3D;
    DrawAddRef(This);
    return DD_OK;
  }
  else
  {
    if(IsEqualGUID(riid, &IID_IDirectDraw4))
    {
      dprintf(("DDRAW: IID_IDirectDraw4 Interface"));
      me->lpVtbl = &me->Vtbl4;
    }
    else
    {
      if(IsEqualGUID(riid, &IID_IDirectDraw2))
      {
        dprintf(("DDRAW: IID_IDirectDraw2 Interface"));
        me->lpVtbl = (IDirectDraw4Vtbl *) &me->Vtbl2;
      }
      else
      {
        dprintf(("DDRAW: IID_IDirectDraw Interface"));
        me->lpVtbl = (IDirectDraw4Vtbl *) &me->Vtbl;
      }
    }
  }
  me->lpVtbl2 = me->lpVtbl;
  *ppvObj = This;
  DrawAddRef(This);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API DrawAddRef(THIS This)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: OS2IDirectDraw::AddRef %d", me->Referenced+1));

  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API DrawRelease(THIS This)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;
 ULONG rc;

  dprintf(("DDRAW: OS2IDirectDraw::Release %d", me->Referenced-1));
  dprintf(("DDRAW: OS2IDirectDraw::%X", me));

  if(me->Referenced)
  {

    me->Referenced--;
    if(me->Referenced == 0)
    {
      delete(me);
      rc = 0;
    }
    else
      rc = me->Referenced;
  }
  else
    rc = 0;

  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawCompact(THIS)
{
  dprintf(("DDRAW: Compact"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawCreateClipper(THIS This, DWORD, LPDIRECTDRAWCLIPPER FAR *lplpDD, IUnknown FAR * )
{
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;
 OS2IDirectDrawClipper *newclip;
 HRESULT                rc;

  newclip = new OS2IDirectDrawClipper(me);

  dprintf(("DDRAW: CreateClipper"));

  if(newclip == NULL)
  {
    rc = DDERR_OUTOFMEMORY;
  }
  else
  {
    newclip->Vtbl.fnAddRef((IDirectDrawClipper *)newclip);
    rc = newclip->GetLastError();
    if(rc != DD_OK)
    {
      *lplpDD = NULL;
      delete newclip;
    }
    else
      *lplpDD = (IDirectDrawClipper *)newclip;
  }

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawCreatePalette(THIS This, DWORD dwFlags,
            LPPALETTEENTRY lpColorTable,
            LPDIRECTDRAWPALETTE FAR *lplpDD,
            IUnknown FAR *pUnkOuter)
{
  OS2IDirectDraw        *me = (OS2IDirectDraw *)This;
  OS2IDirectDrawPalette *newpal;
  HRESULT                rc = DD_OK;
  int                    palsize = 0;

  if(dwFlags & DDPCAPS_8BITENTRIES)
  {
    // We Don't support Indexed palettes...

    rc = DDERR_INVALIDPARAMS;
  }

  if(dwFlags & DDPCAPS_2BIT)
    palsize = 4;
  if(dwFlags & DDPCAPS_4BIT)
    palsize = 16;
  if(dwFlags & DDPCAPS_8BIT)
    palsize = 256;
  if(dwFlags & DDPCAPS_ALLOW256)
    palsize = 256;

  if(palsize == 0)
    rc = DDERR_INVALIDPARAMS;

  if(DD_OK == rc)
  {
    dprintf(("DDRAW: CreatePalette with %d colors", palsize));

    newpal = new OS2IDirectDrawPalette((VOID*)me, palsize, lpColorTable, dwFlags);

    if(newpal == NULL)
    {
      rc = DDERR_OUTOFMEMORY;
    }
    else
    {
      newpal->Vtbl.fnAddRef((IDirectDrawPalette *)newpal);
      rc = newpal->GetLastError();

      if(DD_OK != rc)
      {
        *lplpDD = NULL;
        delete newpal;
      }
      else
        *lplpDD = (IDirectDrawPalette *)newpal;
    }
  }

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawCreateSurface(THIS This, LPDDSURFACEDESC lpDDSurfaceDesc,
            LPDIRECTDRAWSURFACE FAR *lplpDD,
            IUnknown FAR *pUnkOuter)
{
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;
 OS2IDirectDrawSurface *newsurf;
 HRESULT                rc;

  dprintf(("DDRAW: CreateSurface\n"));
  dprintf(("DDRAW: dwSize %d\n", lpDDSurfaceDesc->dwSize));
  dprintf(("DDRAW: dwFlags %X\n", lpDDSurfaceDesc->dwFlags));
  dprintf(("DDRAW: dwHeight %d\n", lpDDSurfaceDesc->dwHeight));
  dprintf(("DDRAW: dwWidth %d\n", lpDDSurfaceDesc->dwWidth));
  dprintf(("DDRAW: lPitch %d\n", lpDDSurfaceDesc->lPitch));
  dprintf(("DDRAW: dwBackBufferCount %d\n", lpDDSurfaceDesc->dwBackBufferCount));
  dprintf(("DDRAW: dwMipMapCount %d\n", lpDDSurfaceDesc->dwMipMapCount));
  dprintf(("DDRAW: dwAlphaBitDepth %d\n", lpDDSurfaceDesc->dwAlphaBitDepth));
  dprintf(("DDRAW: ddsCaps.dwCaps %X\n", lpDDSurfaceDesc->ddsCaps.dwCaps));

  newsurf = new OS2IDirectDrawSurface(me, (LPDDSURFACEDESC2)lpDDSurfaceDesc);
  if(newsurf == NULL)
  {
    rc = DDERR_OUTOFMEMORY;
  }
  else
  {
    newsurf->Vtbl.fnAddRef((IDirectDrawSurface *)newsurf);
    rc = newsurf->GetLastError();
    if(rc != DD_OK)
    {
      dprintf(("DDRAW: Error createing Surface rc = %d\n\n", rc));
      *lplpDD = NULL;
      delete newsurf;
    }
    else
      *lplpDD = (IDirectDrawSurface *)newsurf;

    dprintf(("DDRAW: New Surface created at %08X\n\n", newsurf));
  }

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawCreateSurface4(THIS This, LPDDSURFACEDESC2 lpDDSurfaceDesc2,
            LPDIRECTDRAWSURFACE4 FAR *lplpDD,
            IUnknown FAR *pUnkOuter)
{
 OS2IDirectDraw         *me = (OS2IDirectDraw *)This;
 OS2IDirectDrawSurface *newsurf;
 HRESULT                rc;

  dprintf(("DDRAW: CreateSurface4\n"));
  dprintf(("DDRAW: dwSize %d\n", lpDDSurfaceDesc2->dwSize));
  dprintf(("DDRAW: dwHeight %d\n", lpDDSurfaceDesc2->dwHeight));
  dprintf(("DDRAW: dwWidth %d\n", lpDDSurfaceDesc2->dwWidth));
  dprintf(("DDRAW: lPitch %d\n", lpDDSurfaceDesc2->lPitch));
  dprintf(("DDRAW: dwBackBufferCount %d\n", lpDDSurfaceDesc2->dwBackBufferCount));
  dprintf(("DDRAW: dwMipMapCount %d\n", lpDDSurfaceDesc2->dwMipMapCount));
  dprintf(("DDRAW: dwAlphaBitDepth %d\n", lpDDSurfaceDesc2->dwAlphaBitDepth));
  dprintf(("DDRAW: ddsCaps.dwCaps %X\n", lpDDSurfaceDesc2->ddsCaps.dwCaps));

  newsurf = new OS2IDirectDrawSurface(me, lpDDSurfaceDesc2);

  if(newsurf == NULL)
  {
    rc = DDERR_OUTOFMEMORY;
  }
  else
  {
    newsurf->Vtbl.fnAddRef((IDirectDrawSurface *)newsurf);
    rc = newsurf->GetLastError();
    if(rc != DD_OK)
    {
      dprintf(("DDRAW: Error createing Surface\n\n"));
      *lplpDD = NULL;

      delete newsurf;
    }
    else
      *lplpDD = (IDirectDrawSurface4 *)newsurf;

    dprintf(("DDRAW: New Surface created at %08X\n\n", newsurf));
  }

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * )
{
  dprintf(("DDRAW: DuplicateSurface NIY\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawDuplicateSurface4(THIS, LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4 FAR * )
{
  dprintf(("DDRAW: DuplicateSurface4 NIY\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawEnumDisplayModes( THIS This,
                                        DWORD dwFlags,
                                        LPDDSURFACEDESC lpDDSurfaceDesc,
                                        LPVOID lpContext,
                                        LPDDENUMMODESCALLBACK lpDDEnumModesCallback)
{
  int iMode = 0;
  DDSURFACEDESC DDSurfAct;
  BOOL fCallAgain;
  OS2IDirectDraw        *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: EnumDisplayModes\n"));

  // Check for Pointer to callback function
  if (NULL == lpDDEnumModesCallback)
  {
    dprintf(("DDRAW: EnumDisplayModes : Error NO EnumFunction passed in\n"));

    return(DDERR_GENERIC);
  }


  // Setting up the surface
  // During enum we report resolution and bitdepth, maybe we should
  // also report back : Caps and Pitch
  memset(&DDSurfAct,0,sizeof(DDSURFACEDESC));
  DDSurfAct.dwSize = sizeof(DDSURFACEDESC);
  DDSurfAct.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT ;
  // Only report the bitdepth hope this is ok this way, we must set the BitMask fields

  DDSurfAct.ddpfPixelFormat.dwSize  = sizeof (DDPIXELFORMAT);
  DDSurfAct.ddpfPixelFormat.dwFlags = DDPF_RGB;
  // Check if we use DIVE or Voodoo
  if(me->lpVtbl != (IDirectDraw4Vtbl *) &(me->Vtbl3D))
  {
    // DIVE modes

    dprintf(("DDRAW: EnumDisplayModes : DIVE modes\n"));

    // Enumerate all modes ?
    if (NULL==lpDDSurfaceDesc)
    {
      // Check if we shall report 320x200 mode

      dprintf(("DDRAW: EnumDisplayModes : ALL modes\n"));

      //SvL: Don't report 320x200 if we've been told to report only one
      //     display mode
      if(NUM_MODES_DIVE != 1 && (dwFlags & DDEDM_STANDARDVGAMODES))
      {
        dprintf(("DDRAW: EnumDisplayModes : STANDARDVGAMODES\n"));

        DDSurfAct.dwHeight = ModesDive[0].iYRes;
        DDSurfAct.dwWidth  = ModesDive[0].iXRes;
        DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[0].iBits;
        DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8 ;
        if(!lpDDEnumModesCallback(&DDSurfAct,lpContext))
        {
          dprintf(("DDRAW: EnumDisplayModes : Enum done\n"));
          return (DD_OK);
        }
      }
      // Don't know the flag for Mode X so we skip reporting it

      // Now report all our modes
      iMode = (std::min<unsigned int>)(2, NUM_MODES_DIVE-1);
      fCallAgain = TRUE;
      do
      {
        // if the mode fits in the current resolution report it
        // Change this if we support Fullscreen later !!!

        if(ModesDive[iMode].iXRes <= me->dCaps.ulHorizontalResolution)
        {
          DDSurfAct.dwHeight = ModesDive[iMode].iYRes;
          DDSurfAct.dwWidth  = ModesDive[iMode].iXRes;
          DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[iMode].iBits;
          switch(ModesDive[iMode].iBits)
          {
            case 8:
              DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
              break;
            case 16:
              // VESA uses 565 encoding in 16 bit modes
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x0000F800;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x000007E0;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x0000001F;
              break;
            case 24:
            case 32:
              // VESA uses per default RGB4
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x000000FF;
              break;
            default:
              break;
          }
          dprintf( ("EnumDisplayModes : Enum Mode %dx%d @ %d\n",
                    DDSurfAct.dwHeight,
                    DDSurfAct.dwWidth,
                    DDSurfAct.ddpfPixelFormat.dwRGBBitCount));
          fCallAgain = lpDDEnumModesCallback(&DDSurfAct,lpContext);
          dprintf( ("EnumDisplayModes : Callback returned with %d\n",
                    fCallAgain));
        }
        iMode++;
      }
      while( (iMode < NUM_MODES_DIVE) &&
             (ModesDive[iMode].iBits <= me->dCaps.ulDepth) &&
             (TRUE==fCallAgain) );
    }
    else
    {
      // No, so filter modes with lpDDSurfaceDesc

      // Return Error if the program want to use other than the 3 supported values
      // for filtering

      if (lpDDSurfaceDesc->dwFlags & !(DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT))
        return(DDERR_INVALIDPARAMS);

      iMode = 0;
      if( (dwFlags & DDEDM_STANDARDVGAMODES) &&
         (
          (((lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)&&
            (ModesDive[iMode].iXRes==lpDDSurfaceDesc->dwWidth)
           )||(!(lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)))&&
          (((lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)&&
            (ModesDive[iMode].iYRes==lpDDSurfaceDesc->dwHeight))||
           (!(lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)))&&
          (((lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT) &&
            (ModesDive[iMode].iBits==lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount))||
           (!(lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT)))
         )
        )
      {
        DDSurfAct.dwHeight = ModesDive[0].iYRes;
        DDSurfAct.dwWidth  = ModesDive[0].iXRes;
        DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[0].iBits;
        DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
        if(!lpDDEnumModesCallback(&DDSurfAct,lpContext))
        {
          return (DD_OK);
        }
      }
      // Don't know the flag for Mode X so we skip reporting it

      // Now report all our modes
      iMode = (std::min<unsigned int>)(2, NUM_MODES_DIVE-1);
      fCallAgain = TRUE;
      do
      {
        // if the mode fits in the current resolution and the filter applies report it
        // Change this if we support Fullscreen later !!!
        if( (ModesDive[iMode].iXRes <= me->dCaps.ulHorizontalResolution)&&
           (
            (((lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)&&
              (ModesDive[iMode].iXRes==lpDDSurfaceDesc->dwWidth))||
             (!(lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)))&&
            (((lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)&&
              (ModesDive[iMode].iYRes==lpDDSurfaceDesc->dwHeight))||
             (!(lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)))&&
            (((lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT) &&
              (ModesDive[iMode].iBits==lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount))||
             (!(lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT)))
           )
          )
        {
          DDSurfAct.dwHeight = ModesDive[iMode].iYRes;
          DDSurfAct.dwWidth  = ModesDive[iMode].iXRes;
          DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[iMode].iBits;
          switch(ModesDive[iMode].iBits)
          {
            case 8:
              DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
              break;
            case 16:
              // VESA uses 565 encoding in 16 bit modes
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x0000F800;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x000007E0;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x0000001F;
              break;
            case 24:
            case 32:
              // VESA uses per default RGB4
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x000000FF;
              break;
            default:
              break;
          }

          dprintf(("Call lpDDEnumModesCallback with mode (%d,%d) bps %d", DDSurfAct.dwWidth, DDSurfAct.dwHeight, ModesDive[iMode].iBits));
          fCallAgain = lpDDEnumModesCallback(&DDSurfAct,lpContext);
        }
        iMode++;
      }
      while((ModesDive[iMode].iBits <= me->dCaps.ulDepth) &&
            (iMode < NUM_MODES_DIVE) && (TRUE==fCallAgain));

    }
  }
  else
  {

    // VOODOO modes

    // Enumerate all modes ?
    if (NULL==lpDDSurfaceDesc)
    {

      // report all our modes
      iMode = 0;
      fCallAgain = TRUE;
      do
      {
        DDSurfAct.dwHeight = ModesVoodoo[iMode].iYRes;
        DDSurfAct.dwWidth  = ModesVoodoo[iMode].iXRes;
        DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesVoodoo[iMode].iBits;

        fCallAgain = lpDDEnumModesCallback(&DDSurfAct,lpContext);
        iMode++;
      }
      while((iMode < NUM_MODES_VOODOO) && (TRUE==fCallAgain));
    }
    else
    {
      // No, so filter modes with lpDDSurfaceDesc

      // Return Error if the program want to use other than the 3 supported values
      // for filtering

      if (lpDDSurfaceDesc->dwFlags & !(DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT))
      {
        return(DDERR_INVALIDPARAMS);
      }

      iMode = 2;
      fCallAgain = TRUE;
      // All reported mode are 16bit
      DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x0000F800;
      DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x000007E0;
      DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x0000001F;
      do
      {
        // if the mode fits the filter applies report it
        if(
            (((lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)&&
              (ModesVoodoo[iMode].iXRes==lpDDSurfaceDesc->dwWidth))||
             (!(lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)))&&
            (((lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)&&
              (ModesVoodoo[iMode].iYRes==lpDDSurfaceDesc->dwHeight))||
             (!(lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)))&&
            (((lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT) &&
              (ModesVoodoo[iMode].iBits==lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount))||
             (!(lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT)))
          )
        {
          DDSurfAct.dwHeight = ModesVoodoo[iMode].iYRes;
          DDSurfAct.dwWidth  = ModesVoodoo[iMode].iXRes;
          DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesVoodoo[iMode].iBits;

          fCallAgain = lpDDEnumModesCallback(&DDSurfAct,lpContext);
        }
        iMode++;
      }
      while((iMode < NUM_MODES_VOODOO) && (TRUE==fCallAgain));

    }

  }
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawEnumDisplayModes4(THIS This, DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc2,
                                       LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpDDEnumModesCallback2)
{
  int iMode = 0;
  DDSURFACEDESC2 DDSurfAct;
  BOOL fCallAgain;
  OS2IDirectDraw        *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: EnumDisplayModes4"));

  // Check for Pointer to callback function
  if (NULL == lpDDEnumModesCallback2)
  {
    dprintf(("DDRAW: EnumDisplayModes4 : Error NO EnumFunction passed in\n"));

    return(DDERR_GENERIC);
  }


  // Setting up the surface
  // During enum we report resolution and bitdepth, maybe we should
  // also report back : Caps and Pitch
  memset(&DDSurfAct,0,sizeof(DDSURFACEDESC2));
  DDSurfAct.dwSize = sizeof(DDSURFACEDESC2);
  DDSurfAct.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT ;
  // Only report the bitdepth hope this is ok this way, we must set the BitMask fields

  DDSurfAct.ddpfPixelFormat.dwSize  = sizeof (DDPIXELFORMAT);
  DDSurfAct.ddpfPixelFormat.dwFlags = DDPF_RGB;

  // Check if we use DIVE or Voodoo
  if(me->lpVtbl != (IDirectDraw4Vtbl *) &(me->Vtbl3D))
  {
    // DIVE modes

    dprintf(("DDRAW: EnumDisplayModes : DIVE modes\n"));

    // Enumerate all modes ?
    if (NULL==lpDDSurfaceDesc2)
    {
      // Check if we shall report 320x200 mode

      dprintf(("DDRAW: EnumDisplayModes : ALL modes\n"));

      //SvL: Don't report 320x200 if we've been told to report only one
      //     display mode
      if(NUM_MODES_DIVE != 1 && (dwFlags & DDEDM_STANDARDVGAMODES))
      {
        dprintf(("DDRAW: EnumDisplayModes : STANDARDVGAMODES\n"));

        DDSurfAct.dwHeight = ModesDive[0].iYRes;
        DDSurfAct.dwWidth  = ModesDive[0].iXRes;
        DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[0].iBits;
        DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8 ;
        if(!lpDDEnumModesCallback2(&DDSurfAct,lpContext))
        {
          dprintf(("DDRAW: EnumDisplayModes : Enum done\n"));
          return (DD_OK);
        }
      }
      // Don't know the flag for Mode X so we skip reporting it

      // Now report all our modes
      iMode = (std::min<unsigned int>)(2, NUM_MODES_DIVE-1);
      fCallAgain = TRUE;
      do
      {
        // if the mode fits in the current resolution report it
        // Change this if we support Fullscreen later !!!

        if(ModesDive[iMode].iXRes <= me->dCaps.ulHorizontalResolution)
        {
          DDSurfAct.dwHeight = ModesDive[iMode].iYRes;
          DDSurfAct.dwWidth  = ModesDive[iMode].iXRes;
          DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[iMode].iBits;
          switch(ModesDive[iMode].iBits)
          {
            case 8:
              DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
              break;
            case 16:
              // VESA uses 565 encoding in 16 bit modes
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x0000F800;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x000007E0;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x0000001F;
              break;
            case 24:
            case 32:
              // VESA uses per default RGB4
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x000000FF;
              break;
            default:
              break;
          }
          dprintf( ("EnumDisplayModes : Enum Mode %dx%d @ %d\n",
                    DDSurfAct.dwHeight,
                    DDSurfAct.dwWidth,
                    DDSurfAct.ddpfPixelFormat.dwRGBBitCount));
          fCallAgain = lpDDEnumModesCallback2(&DDSurfAct,lpContext);
          dprintf( ("EnumDisplayModes : Callback returned with %d\n",
                    fCallAgain));
        }
        iMode++;
      }
      while( (iMode < NUM_MODES_DIVE) &&
             (ModesDive[iMode].iBits <= me->dCaps.ulDepth) &&
             (TRUE==fCallAgain) );
    }
    else
    {
      // No, so filter modes with lpDDSurfaceDesc2

      // Return Error if the program want to use other than the 3 supported values
      // for filtering

      if (lpDDSurfaceDesc2->dwFlags & !(DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT))
        return(DDERR_INVALIDPARAMS);

      iMode = 0;
      if( (dwFlags & DDEDM_STANDARDVGAMODES) &&
         (
          (((lpDDSurfaceDesc2->dwFlags & DDSD_WIDTH)&&
            (ModesDive[iMode].iXRes==lpDDSurfaceDesc2->dwWidth)
           )||(!(lpDDSurfaceDesc2->dwFlags & DDSD_WIDTH)))&&
          (((lpDDSurfaceDesc2->dwFlags & DDSD_HEIGHT)&&
            (ModesDive[iMode].iYRes==lpDDSurfaceDesc2->dwHeight))||
           (!(lpDDSurfaceDesc2->dwFlags & DDSD_HEIGHT)))&&
          (((lpDDSurfaceDesc2->dwFlags & DDSD_PIXELFORMAT) &&
            (ModesDive[iMode].iBits==lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount))||
           (!(lpDDSurfaceDesc2->dwFlags & DDSD_PIXELFORMAT)))
         )
        )
      {
        DDSurfAct.dwHeight = ModesDive[0].iYRes;
        DDSurfAct.dwWidth  = ModesDive[0].iXRes;
        DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[0].iBits;
        DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
        if(!lpDDEnumModesCallback2(&DDSurfAct,lpContext))
        {
          return (DD_OK);
        }
      }
      // Don't know the flag for Mode X so we skip reporting it

      // Now report all our modes
      iMode = (std::min<unsigned int>)(2, NUM_MODES_DIVE-1);
      fCallAgain = TRUE;
      do
      {
        // if the mode fits in the current resolution and the filter applies report it
        // Change this if we support Fullscreen later !!!
        if( (ModesDive[iMode].iXRes <= me->dCaps.ulHorizontalResolution)&&
           (
            (((lpDDSurfaceDesc2->dwFlags & DDSD_WIDTH)&&
              (ModesDive[iMode].iXRes==lpDDSurfaceDesc2->dwWidth))||
             (!(lpDDSurfaceDesc2->dwFlags & DDSD_WIDTH)))&&
            (((lpDDSurfaceDesc2->dwFlags & DDSD_HEIGHT)&&
              (ModesDive[iMode].iYRes==lpDDSurfaceDesc2->dwHeight))||
             (!(lpDDSurfaceDesc2->dwFlags & DDSD_HEIGHT)))&&
            (((lpDDSurfaceDesc2->dwFlags & DDSD_PIXELFORMAT) &&
              (ModesDive[iMode].iBits==lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount))||
             (!(lpDDSurfaceDesc2->dwFlags & DDSD_PIXELFORMAT)))
           )
          )
        {
          DDSurfAct.dwHeight = ModesDive[iMode].iYRes;
          DDSurfAct.dwWidth  = ModesDive[iMode].iXRes;
          DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesDive[iMode].iBits;
          switch(ModesDive[iMode].iBits)
          {
            case 8:
              DDSurfAct.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
              break;
            case 16:
              // VESA uses 565 encoding in 16 bit modes
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x0000F800;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x000007E0;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x0000001F;
              break;
            case 24:
            case 32:
              // VESA uses per default RGB4
              DDSurfAct.ddpfPixelFormat.dwFlags &= ~DDPF_PALETTEINDEXED8;
              DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
              DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
              DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x000000FF;
              break;
            default:
              break;
          }

          dprintf(("Call lpDDEnumModesCallback with mode (%d,%d) bps %d", DDSurfAct.dwWidth, DDSurfAct.dwHeight, ModesDive[iMode].iBits));
          fCallAgain = lpDDEnumModesCallback2(&DDSurfAct,lpContext);
        }
        iMode++;
      }
      while((ModesDive[iMode].iBits <= me->dCaps.ulDepth) &&
            (iMode < NUM_MODES_DIVE) && (TRUE==fCallAgain));

    }
  }
  else
  {

    // VOODOO modes

    // Enumerate all modes ?
    if (NULL==lpDDSurfaceDesc2)
    {

      // report all our modes
      iMode = 0;
      fCallAgain = TRUE;
      do
      {
        DDSurfAct.dwHeight = ModesVoodoo[iMode].iYRes;
        DDSurfAct.dwWidth  = ModesVoodoo[iMode].iXRes;
        DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesVoodoo[iMode].iBits;

        fCallAgain = lpDDEnumModesCallback2(&DDSurfAct,lpContext);
        iMode++;
      }
      while((iMode < NUM_MODES_VOODOO) && (TRUE==fCallAgain));
    }
    else
    {
      // No, so filter modes with lpDDSurfaceDesc2

      // Return Error if the program want to use other than the 3 supported values
      // for filtering

      if (lpDDSurfaceDesc2->dwFlags & !(DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT))
      {
        return(DDERR_INVALIDPARAMS);
      }

      iMode = 2;
      fCallAgain = TRUE;
      // All reported mode are 16bit
      DDSurfAct.ddpfPixelFormat.dwRBitMask = 0x0000F800;
      DDSurfAct.ddpfPixelFormat.dwGBitMask = 0x000007E0;
      DDSurfAct.ddpfPixelFormat.dwBBitMask = 0x0000001F;
      do
      {
        // if the mode fits the filter applies report it
        if(
            (((lpDDSurfaceDesc2->dwFlags & DDSD_WIDTH)&&
              (ModesVoodoo[iMode].iXRes==lpDDSurfaceDesc2->dwWidth))||
             (!(lpDDSurfaceDesc2->dwFlags & DDSD_WIDTH)))&&
            (((lpDDSurfaceDesc2->dwFlags & DDSD_HEIGHT)&&
              (ModesVoodoo[iMode].iYRes==lpDDSurfaceDesc2->dwHeight))||
             (!(lpDDSurfaceDesc2->dwFlags & DDSD_HEIGHT)))&&
            (((lpDDSurfaceDesc2->dwFlags & DDSD_PIXELFORMAT) &&
              (ModesVoodoo[iMode].iBits==lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount))||
             (!(lpDDSurfaceDesc2->dwFlags & DDSD_PIXELFORMAT)))
          )
        {
          DDSurfAct.dwHeight = ModesVoodoo[iMode].iYRes;
          DDSurfAct.dwWidth  = ModesVoodoo[iMode].iXRes;
          DDSurfAct.ddpfPixelFormat.dwRGBBitCount = ModesVoodoo[iMode].iBits;

          fCallAgain = lpDDEnumModesCallback2(&DDSurfAct,lpContext);
        }
        iMode++;
      }
      while((iMode < NUM_MODES_VOODOO) && (TRUE==fCallAgain));

    }

  }
  return(DD_OK);
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK )
{
  dprintf(("DDRAW: EnumSurfaces NIY\n"));

  return(DD_OK);
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawEnumSurfaces4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 )
{
  dprintf(("DDRAW: EnumSurfaces4 NIY\n"));

  return(DD_OK);
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawFlipToGDISurface(THIS)
{
  dprintf(("DDRAW: FlipToGDISurface NIY\n"));

  return(DD_OK);
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetCaps(THIS This, LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps)
{
 DWORD dwSize;
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: DDGetCaps %x %x %x", This, lpDDDriverCaps, lpDDHELCaps));

  if( (NULL==lpDDDriverCaps) && (NULL==lpDDHELCaps) )
    return(DDERR_INVALIDPARAMS);

  if(NULL!=lpDDDriverCaps)
  {
    // Caller want Driver Caps

    dprintf(("DDRAW: Driver\n"));

    if( (sizeof(DDCAPS)!=lpDDDriverCaps->dwSize)     &&
        (sizeof(DDCAPS_DX5)!=lpDDDriverCaps->dwSize) &&
        (sizeof(DDCAPS_DX3)!=lpDDDriverCaps->dwSize) )
    {
      dprintf( ("Size %d Not supported ",
                lpDDDriverCaps->dwSize));
      return(DDERR_INVALIDPARAMS);
    }
    // Clear structure so we only have to set the supported flags

    dwSize = lpDDDriverCaps->dwSize;
    memset( lpDDDriverCaps,
            0,
            lpDDDriverCaps->dwSize);


    // Reset the size
    lpDDDriverCaps->dwSize = dwSize;

    // Now report the CAPs back which we support
    lpDDDriverCaps->dwCaps = DDCAPS_BLT |              // We do blitting
                             DDCAPS_BLTSTRETCH |
                             DDCAPS_BLTCOLORFILL |     // We do colorfills
                             DDCAPS_BLTDEPTHFILL |
                             DDCAPS_COLORKEY |         // We support Colorkeying
                             DDCAPS_COLORKEYHWASSIST | // But we (may) use the CPU
                             DDCAPS_GDI |              // Maybe check if we are on Voodoo ?
                             DDCAPS_PALETTEVSYNC |     // Got VSync
//                             DDCAPS_READSCANLINE |
                             DDCAPS_BLTFOURCC;
    lpDDDriverCaps->dwCaps2 = DDCAPS2_CERTIFIED |         // Who cares so say yes
                              DDCAPS2_CANRENDERWINDOWED | // Better check for Voodoo ?!
                              DDCAPS2_COPYFOURCC |        // yepp memcpy will do this
                              DDCAPS2_NONLOCALVIDMEM |    // All surfaces are in memory
                              DDCAPS2_WIDESURFACES;       // Any size you want!

    lpDDDriverCaps->dwCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting

    lpDDDriverCaps->dwFXCaps = DDFXCAPS_BLTMIRRORUPDOWN |
                               DDFXCAPS_BLTMIRRORLEFTRIGHT; // DIVE supports this, do we also ?
                                                         // Maybe later add stretching support?

    lpDDDriverCaps->dwPalCaps = DDPCAPS_8BIT |          // Only 8 Bits pals
                                DDPCAPS_ALLOW256 |      // But all 256 colors
                                DDPCAPS_VSYNC  |        // Vsync yet
                                DDPCAPS_PRIMARYSURFACE; //
    lpDDDriverCaps->dwVidMemTotal = me->dCaps.ulApertureSize;          // total video memory
    lpDDDriverCaps->dwVidMemFree  = me->dCaps.ulApertureSize;          // total free video memory

    lpDDDriverCaps->dwNumFourCCCodes = MAX_FOURCC_CODES;        // number of supported FOURCC codes
/*
    ToDo: must finde out ow the array is used for this
    lpDDDriverCaps->dwRops[DD_ROP_SPACE] = SRCCOPY |
                                           BLACKNESS |
                                           WHITENESS;  // Raster OPs implemented
*/
    lpDDDriverCaps->dwSVBCaps = DDCAPS_BLT |              // We do blitting
                                DDCAPS_BLTCOLORFILL |     // We do colorfills
                                DDCAPS_COLORKEY |         // We support Colorkeying
                                DDCAPS_COLORKEYHWASSIST;
    lpDDDriverCaps->dwSVBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
    lpDDDriverCaps->dwSVBFXCaps = DDCAPS_BLT |              // We do blitting
                                  DDCAPS_BLTCOLORFILL |     // We do colorfills
                                  DDCAPS_COLORKEY |         // We support Colorkeying
                                  DDCAPS_COLORKEYHWASSIST;
/*
    ToDo: must finde out ow the array is used for this
    lpDDDriverCaps->dwSVBRops[DD_ROP_SPACE] = SRCCOPY |
                                              BLACKNESS |
                                              WHITENESS;  // Raster OPs implemented
*/
    lpDDDriverCaps->dwVSBCaps = DDCAPS_BLT |              // We do blitting
                                DDCAPS_BLTCOLORFILL |     // We do colorfills
                                DDCAPS_COLORKEY |         // We support Colorkeying
                                DDCAPS_COLORKEYHWASSIST;
    lpDDDriverCaps->dwVSBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
    lpDDDriverCaps->dwVSBFXCaps = DDCAPS_BLT |              // We do blitting
                                  DDCAPS_BLTCOLORFILL |     // We do colorfills
                                  DDCAPS_COLORKEY |         // We support Colorkeying
                                  DDCAPS_COLORKEYHWASSIST;
/*
    ToDo: must finde out ow the array is used for this
    lpDDDriverCaps->dwVSBRops[DD_ROP_SPACE] = SRCCOPY |
                                              BLACKNESS |
                                              WHITENESS;  // Raster OPs implemented
*/
    lpDDDriverCaps->dwSSBCaps = DDCAPS_BLT |              // We do blitting
                                DDCAPS_BLTCOLORFILL |     // We do colorfills
                                DDCAPS_COLORKEY |         // We support Colorkeying
                                DDCAPS_COLORKEYHWASSIST;
    lpDDDriverCaps->dwSSBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
    lpDDDriverCaps->dwSSBFXCaps = DDCAPS_BLT |              // We do blitting
                                  DDCAPS_BLTCOLORFILL |     // We do colorfills
                                  DDCAPS_COLORKEY |         // We support Colorkeying
                                  DDCAPS_COLORKEYHWASSIST;
/*
    ToDo: must finde out ow the array is used for this
    lpDDDriverCaps->dwSSBRops[SRCCOPY]   = 1;
    lpDDDriverCaps->dwSSBRops[BLACKNESS] = 1;
    lpDDDriverCaps->dwSSBRops[WHITENESS] = 1;  // Raster OPs implemented
*/
    // These are ony in >DX5
    if(dwSize>sizeof(DDCAPS_DX3))
    {
      lpDDDriverCaps->dwSVBCaps2 = //DDCAPS2_CANRENDERWINDOWED | // Better check for Voodoo ?!
                                   DDCAPS2_COPYFOURCC |        // yepp memcpy will do this
                                   DDCAPS2_WIDESURFACES;       // Any size you want!
      lpDDDriverCaps->dwNLVBCaps = DDCAPS_BLT |              // We do blitting
                                   DDCAPS_BLTCOLORFILL |     // We do colorfills
                                   DDCAPS_COLORKEY |         // We support Colorkeying
                                   DDCAPS_COLORKEYHWASSIST;
      lpDDDriverCaps->dwNLVBCaps2 = //DDCAPS2_CANRENDERWINDOWED | // Better check for Voodoo ?!
                                    DDCAPS2_COPYFOURCC |        // yepp memcpy will do this
                                    DDCAPS2_WIDESURFACES;       // Any size you want!
      lpDDDriverCaps->dwNLVBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
      lpDDDriverCaps->dwNLVBFXCaps = DDCAPS_BLT |              // We do blitting
                                     DDCAPS_BLTCOLORFILL |     // We do colorfills
                                     DDCAPS_COLORKEY |         // We support Colorkeying
                                     DDCAPS_COLORKEYHWASSIST;
      lpDDDriverCaps->dwNLVBRops[DD_ROP_SPACE];//  ToDo: Again the array ...

      // General Surface caps only in DX6
      if(dwSize>sizeof(DDCAPS_DX5))
      {
        lpDDDriverCaps->ddsCaps.dwCaps = DDSCAPS_ALPHA | DDSCAPS_BACKBUFFER |
                                         DDSCAPS_COMPLEX | DDSCAPS_FLIP |
                                         DDSCAPS_FRONTBUFFER | DDSCAPS_LOCALVIDMEM |
                                         DDSCAPS_NONLOCALVIDMEM | DDSCAPS_OFFSCREENPLAIN |
                                         // DDSCAPS_OVERLAY |
                                         DDSCAPS_PALETTE | DDSCAPS_PRIMARYSURFACE |
                                         DDSCAPS_SYSTEMMEMORY |DDSCAPS_VIDEOMEMORY |
                                         DDSCAPS_VISIBLE;

      #ifdef USE_OPENGL
        lpDDDriverCaps->dwCaps |= DDCAPS_3D | DDCAPS_ZBLTS;
        // ToDO find and put the value for DDCAPS2_NO2DDURING3DSCENE in ddraw.h
        // lpDDDriverCaps->dwCaps2 |=  DDCAPS2_NO2DDURING3DSCENE;
        lpDDDriverCaps->ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | DDSCAPS_MIPMAP |
                                          DDSCAPS_TEXTURE | DDSCAPS_ZBUFFER;
      #endif
      }
    }


  }

  if(NULL!=lpDDHELCaps)
  {
    // Caller wants HEL Caps
    dprintf(("DDRAW: CHEL\n"));

    //SvL: PowerDVD uses size 316
    if( (sizeof(DDCAPS)     != lpDDHELCaps->dwSize) &&
        (sizeof(DDCAPS_DX5) != lpDDHELCaps->dwSize) &&
        (sizeof(DDCAPS_DX3) != lpDDHELCaps->dwSize) )
    {
      dprintf( ("Size %d Not supported ",
                lpDDHELCaps->dwSize));
      return(DDERR_INVALIDPARAMS);
    }
    // Clear structure so we only have to set the supported flags

    dwSize = lpDDHELCaps->dwSize;
    memset( lpDDHELCaps,
            0,
            lpDDHELCaps->dwSize);

    // Reset the size
    lpDDHELCaps->dwSize = dwSize;

    // Now report the CAPs back which we support
    lpDDHELCaps->dwCaps = DDCAPS_BLT |              // We do blitting
                          DDCAPS_BLTCOLORFILL |     // We do colorfills
                          DDCAPS_COLORKEY |         // We support Colorkeying
//SvL: This is the emulation layer; no hw support here
//                          DDCAPS_COLORKEYHWASSIST | // But we (may) use the CPU
                          DDCAPS_GDI |              // Maybe check if we are on Voodoo ?
                          DDCAPS_PALETTEVSYNC |     // Got VSync
                          DDCAPS_BLTFOURCC;

    lpDDHELCaps->dwCaps2 = DDCAPS2_CERTIFIED |         // Who cares so say yes
                           DDCAPS2_CANRENDERWINDOWED | // Better check for Voodoo ?!
                           DDCAPS2_COPYFOURCC |        // yepp memcpy will do this
                           DDCAPS2_NONLOCALVIDMEM |    // All surfaces are in memory
                           DDCAPS2_WIDESURFACES;       // Any size you want!

    lpDDHELCaps->dwCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting

    lpDDHELCaps->dwFXCaps = DDFXCAPS_BLTMIRRORUPDOWN |
                            DDFXCAPS_BLTMIRRORLEFTRIGHT; // DIVE supports this, do we also ?
                                                         // Maybe later add stretching support?

    lpDDHELCaps->dwPalCaps = DDPCAPS_8BIT |          // Only 8 Bits pals
                             DDPCAPS_ALLOW256 |      // But all 256 colors
                             DDPCAPS_VSYNC  |        // Vsync yet
                             DDPCAPS_PRIMARYSURFACE;

    //Video memory for emulation layer must be 0 (verified in NT)
    lpDDHELCaps->dwVidMemTotal = 0;           // total video memory
    lpDDHELCaps->dwVidMemFree  = 0;           // total free video memory

    lpDDHELCaps->dwNumFourCCCodes = MAX_FOURCC_CODES;        // number of supported FOURCC codes
    lpDDHELCaps->dwRops[DD_ROP_SPACE];    // supported raster ops
    lpDDHELCaps->dwSVBCaps = DDCAPS_BLT |              // We do blitting
                                DDCAPS_BLTCOLORFILL |     // We do colorfills
                                DDCAPS_COLORKEY |         // We support Colorkeying
                                DDCAPS_COLORKEYHWASSIST;
    lpDDHELCaps->dwSVBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
    lpDDHELCaps->dwSVBFXCaps;             //  .
    lpDDHELCaps->dwSVBRops[DD_ROP_SPACE]; //  .
    lpDDHELCaps->dwVSBCaps = DDCAPS_BLT |              // We do blitting
                                DDCAPS_BLTCOLORFILL |     // We do colorfills
                                DDCAPS_COLORKEY |         // We support Colorkeying
                                DDCAPS_COLORKEYHWASSIST;
    lpDDHELCaps->dwVSBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
    lpDDHELCaps->dwVSBFXCaps;             //  .
    lpDDHELCaps->dwVSBRops[DD_ROP_SPACE]; //  .
    lpDDHELCaps->dwSSBCaps = DDCAPS_BLT |              // We do blitting
                                DDCAPS_BLTCOLORFILL |     // We do colorfills
                                DDCAPS_COLORKEY |         // We support Colorkeying
                                DDCAPS_COLORKEYHWASSIST;
    lpDDHELCaps->dwSSBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
    lpDDHELCaps->dwSSBFXCaps;            //  .
    lpDDHELCaps->dwSSBRops[DD_ROP_SPACE]; //  .
    // These are ony in >DX5
    if(dwSize>sizeof(DDCAPS_DX3))
    {
      lpDDHELCaps->dwSVBCaps2 = DDCAPS2_CANRENDERWINDOWED | // Better check for Voodoo ?!
                            DDCAPS2_COPYFOURCC |        // yepp memcpy will do this
                            DDCAPS2_WIDESURFACES;       // Any size you want!
      lpDDHELCaps->dwNLVBCaps = DDCAPS_BLT |              // We do blitting
                                  DDCAPS_BLTCOLORFILL |     // We do colorfills
                                  DDCAPS_COLORKEY |         // We support Colorkeying
                                  DDCAPS_COLORKEYHWASSIST;
      lpDDHELCaps->dwNLVBCaps2 = DDCAPS2_CANRENDERWINDOWED | // Better check for Voodoo ?!
                                   DDCAPS2_COPYFOURCC |        // yepp memcpy will do this
                                   DDCAPS2_WIDESURFACES;       // Any size you want!
      lpDDHELCaps->dwNLVBCKeyCaps = DDCKEYCAPS_SRCBLT;      // Only source transparent blitting
      lpDDHELCaps->dwNLVBFXCaps;            //  .
      lpDDHELCaps->dwNLVBRops[DD_ROP_SPACE];//  .
    }

  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetDisplayMode(THIS This, LPDDSURFACEDESC lpDDSurfaceDesc)
{
  OS2IDirectDraw        *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: GetDisplayMode %x %x", This, lpDDSurfaceDesc));

  // Check Parameter
  if(NULL==lpDDSurfaceDesc)
    return(DDERR_INVALIDPARAMS);

  if(sizeof(DDSURFACEDESC)!=lpDDSurfaceDesc->dwSize)
    return(DDERR_INVALIDPARAMS);

  // We report back the DIVE caps. maybe we should set up a local DDSURFACEDESC
  // for the object so we can change the values when we switch modes (or say so)
  // as a program may use this function to check the values after a mode change
  // An other reason to to so is Voodoo supports maybe more functions

  // Tell what we report
  lpDDSurfaceDesc->dwFlags  = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  lpDDSurfaceDesc->dwHeight = me->dCaps.ulHorizontalResolution;
  lpDDSurfaceDesc->dwWidth  = me->dCaps.ulVerticalResolution;
  // Set the PixelFormat
  lpDDSurfaceDesc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

  lpDDSurfaceDesc->ddpfPixelFormat.dwFourCC = me->dCaps.fccColorEncoding;
  lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = me->dCaps.ulDepth;
  lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask = 0; // No Alpha support
  switch(me->dCaps.ulDepth)
  {
    case 4:
      // Assume that no one will run OS/2 PM with less then 16 colors and try
      // to start a DirectX program ;)
      lpDDSurfaceDesc->ddpfPixelFormat.dwFlags =  DDPF_RGB | DDPF_PALETTEINDEXED4;
      lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0;
      lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0;
      lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0;
      break;
    case 8:
      lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8 |
                                                 DDPF_FOURCC;
      lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0;
      lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0;
      lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0;
      break;
    case 15:
    case 16:
      lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 16; // No sure about 15Bit modes
      lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_FOURCC;
      if (FOURCC_R555 == me->dCaps.fccColorEncoding)
      {
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x00007C00;
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000003E0;
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000001F;
      }
      else
      {
        if(FOURCC_R565 == me->dCaps.fccColorEncoding)
        {
          lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x0000F800;
          lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000007E0;
          lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000001F;
        }
        else
        {
          // R664
          lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x0000F800;
          lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000003F0;
          lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000000F;
        }
      }
      break;
    case 24:
      lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_FOURCC;
      if(FOURCC_RGB3 == me->dCaps.fccColorEncoding)
      {
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x00FF0000;
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x000000FF;
      }
      else
      {
        // BGR3
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x000000FF;
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x00FF0000;
      }
      break;
    case 32:
      lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_FOURCC;
      if(FOURCC_RGB4 == me->dCaps.fccColorEncoding)
      {
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x00FF0000;
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x000000FF;
      }
      else
      {
        // BGR4
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x000000FF;
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x00FF0000;
      }
      break;
    default:
      dprintf(("DDRAW: Unsupported mode\n"));
      return(DDERR_UNSUPPORTEDMODE);
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetDisplayMode4(THIS This, LPDDSURFACEDESC2 lpDDSurfaceDesc2)
{
  OS2IDirectDraw        *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: GetDisplayMode4 %x %x", This, lpDDSurfaceDesc2));

  // Check Parameter
  if(NULL==lpDDSurfaceDesc2)
    return(DDERR_INVALIDPARAMS);

  if(sizeof(DDSURFACEDESC)!=lpDDSurfaceDesc2->dwSize)
    return(DDERR_INVALIDPARAMS);

  // We report back the DIVE caps. maybe we should set up a local DDSURFACEDESC
  // for the object so we can change the values when we switch modes (or say so)
  // as a program may use this function to check the values after a mode change
  // An other reason to to so is Voodoo supports maybe more functions

  // Tell what we report
  lpDDSurfaceDesc2->dwFlags  = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  lpDDSurfaceDesc2->dwHeight = me->dCaps.ulHorizontalResolution;
  lpDDSurfaceDesc2->dwWidth  = me->dCaps.ulVerticalResolution;
  // Set the PixelFormat
  lpDDSurfaceDesc2->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

  lpDDSurfaceDesc2->ddpfPixelFormat.dwFourCC = me->dCaps.fccColorEncoding;
  lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount = me->dCaps.ulDepth;
  lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBAlphaBitMask = 0; // No Alpha support
  switch(me->dCaps.ulDepth)
  {
    case 4:
      // Assume that no one will run OS/2 PM with less then 16 colors and try
      // to start a DirectX program ;)
      lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags =  DDPF_RGB | DDPF_PALETTEINDEXED4;
      lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0;
      lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0;
      lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0;
      break;
    case 8:
      lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8 |
                                                 DDPF_FOURCC;
      lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0;
      lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0;
      lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0;
      break;
    case 15:
    case 16:
      lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount = 16; // No sure about 15Bit modes
      lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_FOURCC;
      if (FOURCC_R555 == me->dCaps.ulDepth)
      {
        lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0x00007C00;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0x000003E0;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0x0000001F;
      }
      else
      {
        if(FOURCC_R565 == me->dCaps.fccColorEncoding)
        {
          lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0x0000F800;
          lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0x000007E0;
          lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0x0000001F;
        }
        else
        {
          // R664
          lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0x0000F800;
          lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0x000003F0;
          lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0x0000000F;
        }
      }
      break;
    case 24:
      lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_FOURCC;
      if(FOURCC_RGB3 == me->dCaps.fccColorEncoding)
      {
        lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0x00FF0000;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0x000000FF;
      }
      else
      {
        // BGR3
        lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0x000000FF;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0x00FF0000;
      }
      break;
    case 32:
      lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_FOURCC;
      if(FOURCC_RGB4 == me->dCaps.fccColorEncoding)
      {
        lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0x00FF0000;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0x000000FF;
      }
      else
      {
        // BGR4
        lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask = 0x000000FF;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask = 0x00FF0000;
      }
      break;
    default:
      dprintf(("DDRAW: Unsupported mode\n"));
      return(DDERR_UNSUPPORTEDMODE);
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetFourCCCodes(THIS This, LPDWORD lpNumCodes, LPDWORD lpCodes)
{
  DWORD dwFCC[MAX_FOURCC_CODES] = {FOURCC_LUT8,FOURCC_R565,FOURCC_R555,FOURCC_RGB3};

  dprintf(("DDRAW: GetFourCCCodes %x %x %x", This, lpNumCodes, lpCodes));

  if(NULL==lpNumCodes)
    return(DDERR_INVALIDPARAMS);

  if(NULL==lpCodes)
  {
#ifdef SUPPORT_ALL_DIVE_FOURCCS
    *lpNumCodes = nrOfDiveFourCCs;
#else
    *lpNumCodes = MAX_FOURCC_CODES; // LUT8, R565, RGB3 are the FourCC we support for now
#endif
  }
  else
  {
#ifdef SUPPORT_ALL_DIVE_FOURCCS
    for(int i=0;(i<nrOfDiveFourCCs)&&(i<*lpNumCodes);i++)
    {
      *lpCodes = DiveFOURCCs[i];
      lpCodes++;
    }
    *lpNumCodes = nrOfDiveFourCCs;
#else
    for(int i=0;(i<MAX_FOURCC_CODES)&&(i<*lpNumCodes);i++)
    {
      *lpCodes = dwFCC[i];
      lpCodes++;
    }
    *lpNumCodes = MAX_FOURCC_CODES;
#endif
  }
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE FAR *)
{
  dprintf(("DDRAW: GetGDISurface NYI\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetGDISurface4(THIS, LPDIRECTDRAWSURFACE4 FAR *)
{
  dprintf(("DDRAW: GetGDISurface NYI\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetMonitorFrequency(THIS This, LPDWORD lpdwFreq)
{
  ULONG ulTime1, ulTime2;
  DWORD dwFlags = DDWAITVB_BLOCKBEGIN;
  dprintf(("DDRAW: GetMonitorFrequency\n"));
  if(NULL==lpdwFreq)
    return(DDERR_INVALIDPARAMS);



  if(DD_OK==DrawWaitForVerticalBlank(This, dwFlags, 0))
  {
    ulTime1 = GetTickCount();
    // Timer has an accuracy of 4 ms so call it al least 4 times
    DrawWaitForVerticalBlank(This, dwFlags, 0);
    DrawWaitForVerticalBlank(This, dwFlags, 0);
    DrawWaitForVerticalBlank(This, dwFlags, 0);
    DrawWaitForVerticalBlank(This, dwFlags, 0);
    ulTime2 = GetTickCount();
    ulTime2 -= ulTime1;
    if(ulTime2) // paranoid check to avoid DIV0
      *lpdwFreq = 4000 / ulTime2;
    else
      *lpdwFreq = 70;
  }
  else
  {
    // Assume 70 Hz maybe better return DDERR_UNSUPPORTED if this function isn't mandatory
    *lpdwFreq = 70;
  }



  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetScanLine(THIS, LPDWORD lpdwLine)
{
//  BOOL bVertBlank;
  dprintf(("DDRAW: GetScanLine\n"));
  // ToDO find a way to get this position, so for now simply return DDERR_UNSUPPORTED
  // as we indicated in DDCAPS we don't support this.

  return(DDERR_UNSUPPORTED);

  //the following code could be used if we implement this
  /*
  if(NULL==lpdwLine)
    return(DDERR_INVALIDPARAMS);
  DrawGetVertcalBlackStatus(This,&bVertBlank);
  if(bVertBlank)
    return (DDERR_VERTICALBLANKINPROGRESS);
  *lpdwLine = GetLine(); // GetLine would be the function which gets us the line
  return(DD_OK);
  */
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetVerticalBlankStatus(THIS , LPBOOL lpbIsInVB)
{
//  int rc;
  dprintf(("DDRAW: GetVerticalBlankStatus\n"));
  if(NULL==lpbIsInVB)
    return(DDERR_INVALIDPARAMS);

  #if 0
    rc = io_init1();


  if(0==rc)  // try to get IOPL for the thread
  {
    *lpbIsInVB = (c_inb1(0x3da)&0x08)!=0;

      io_exit1(); // reset IOPL

    return(DD_OK);
  }
  #else
    *lpbIsInVB = ! (*lpbIsInVB);
  #endif
  return(DDERR_UNSUPPORTED);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawInitialize(THIS, GUID FAR *)
{
  dprintf(("DDRAW: Initialize\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawRestoreDisplayMode(THIS This)
{
  OS2IDirectDraw        *me = (OS2IDirectDraw *)This;

  dprintf(("DDRAW: RestoreDisplayMode"));

  // FSDDRAW additions
  ULONG  ulModeInd;
  /* restore previous screen mode */
  if ((bUseFSDD) && (me->bInFullScreen))
  {
    SetSVGAMode(me->oldwidth, me->oldheight, me->oldbpp, 0xFF, &ulModeInd, NULL);
    RestorePM();
    /* restore DIVE caps entries */
    me->dCaps.ulScanLineBytes        = me->oldscanlines;
    me->dCaps.ulHorizontalResolution = me->oldwidth;
    me->dCaps.ulVerticalResolution   = me->oldheight;
    me->dCaps.ulDepth                = me->oldbpp;
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawSetCooperativeLevel(THIS This, HWND hwndClient, DWORD dwFlags)
{
    OS2IDirectDraw *me = (OS2IDirectDraw *)This;

    dprintf(("DDRAW: SetCooperativeLevel: hwnd %X, Flags %X\n", hwndClient, dwFlags));

    if (!(dwFlags & (DDSCL_EXCLUSIVE|DDSCL_NORMAL)))
	return DDERR_INVALIDPARAMS;

    me->dwCoopMode = dwFlags;

    if(dwFlags & DDSCL_NORMAL) {
        //normal mode, no window required
	if(dwFlags & DDSCL_FULLSCREEN)    return DDERR_INVALIDPARAMS;
    }
    else {
        //fullscreen mode, window required
        if(!hwndClient)                   return DDERR_INVALIDPARAMS;
	if(!(dwFlags & DDSCL_FULLSCREEN)) return DDERR_INVALIDPARAMS;
    }
    me->hwndClient = hwndClient;
    if(hwndClient && (dwFlags & DDSCL_FULLSCREEN)) {
        //Changing the size of the win32 window in SetCooperativeLevel can
        //fail if this happens during WM_ADJUSTWINDOWPOS
        //NOTE: This is not a good solution, but a proper fix is more difficult
        //      with the current window mess
        OSLibPostMessage(Win32ToOS2Handle(me->hwndClient), WIN32APP_DDRAWFULLSCREEN, me->screenwidth, me->screenheight);
//        SetWindowPos(me->hwndClient, HWND_TOP, 0, 0, me->screenwidth, me->screenheight,0);
//        ShowWindow(hwndClient, SW_SHOW);
    }
    return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawSetDisplayMode2(THIS This, DWORD dwWidth, DWORD dwHeight,
                                     DWORD dwBPP, DWORD dwRefreshRate,
                                     DWORD dwFlags)
{
 ULONG rc;
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;
 SETUP_BLITTER sBlt;

  dprintf(("DDRAW: SetDisplayMode2 to %dx%d with %d bits colors\n", dwWidth, dwHeight, dwBPP));

  me->screenwidth  = dwWidth;
  me->screenheight = dwHeight;
  me->screenbpp    = dwBPP;

  if(me->dwCoopMode & DDSCL_FULLSCREEN)
  {
    // FSDDRAW additions
    ULONG  ulModeInd;

    SetWindowPos(me->hwndClient, HWND_TOP,0,0,dwWidth,dwHeight,0);
    SetCursorPos(dwWidth / 2, dwHeight / 2);

    if ((bUseFSDD) && (ModeInSVGAModeTable(dwWidth,dwHeight,dwBPP)))
    {
      KillPM();

      rc = SetSVGAMode(dwWidth, dwHeight, dwBPP, 0xFF, &ulModeInd, NULL);

      if (!rc) // When mode switched successfully
      {
        /* save old screen values */
        me->bInFullScreen=TRUE;
        me->oldwidth     = me->dCaps.ulHorizontalResolution;
        me->oldheight    = me->dCaps.ulVerticalResolution;
        me->oldscanlines = me->dCaps.ulScanLineBytes;
        me->oldbpp       = me->dCaps.ulDepth;

        /* now 'fix' some DIVE caps entries */
        me->dCaps.ulScanLineBytes        = dwWidth * (dwBPP >> 3); /* FIX: use real value */
        me->dCaps.ulHorizontalResolution = dwWidth;
        me->dCaps.ulVerticalResolution   = dwHeight;
        me->dCaps.ulDepth                = dwBPP;
      }
      else
      // Display must be return to PM Session
      RestorePM();
    }
  }

  memset(&sBlt,0,sizeof(sBlt));
  sBlt.ulStructLen = sizeof(sBlt);
  sBlt.fccSrcColorFormat = FOURCC_SCRN;
  sBlt.ulSrcWidth        = dwWidth;
  sBlt.ulSrcHeight       = dwHeight;
  sBlt.ulSrcPosX         = 0;
  sBlt.ulSrcPosY         = 0;
  sBlt.fccDstColorFormat = FOURCC_SCRN;
  sBlt.ulDstWidth        = dwWidth;
  sBlt.ulDstHeight       = dwHeight;
  sBlt.lDstPosX          = 0;
  sBlt.lDstPosY          = 0;
  sBlt.lScreenPosX       = 0;
  sBlt.lScreenPosY       = me->dCaps.ulVerticalResolution-dwHeight;
  sBlt.ulNumDstRects     = DIVE_FULLY_VISIBLE;

  rc = DiveSetupBlitter( me->hDive,
                         &sBlt);
  dprintf(("       DiveSetupBlitter rc=%d",rc));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawSetDisplayMode(THIS This, DWORD dwWidth, DWORD dwHeight,
                                    DWORD dwBPP)
{
    return DrawSetDisplayMode2(This, dwWidth, dwHeight, dwBPP, 70, 0);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawWaitForVerticalBlank(THIS, DWORD dwFlags, HANDLE hEvent)
{
//  HRESULT rc;
//  int rci;
//  USHORT sel;
  dprintf(("DDRAW: WaitForVerticalBlank\n"));

  if(DDWAITVB_BLOCKBEGINEVENT == dwFlags) // This parameter isn't support in DX
    return (DDERR_UNSUPPORTED);

  return DD_OK;

#if 0
  rci = InitIO();

  if(rci)  // try to get IOPL for the thread
  {
    dprintf(("DDRAW: No IOPL\n"));
    return (DDERR_UNSUPPORTED);  // we failed so return error that we don't support this
  }
  // AT this point we should have IOPL so lets use it!
  dprintf(("DDRAW: IOPL 3!\n"));

  rc = DDERR_INVALIDPARAMS; // set returnvalue to fail

  if(DDWAITVB_BLOCKBEGIN == dwFlags)
  {
    dprintf(("DDRAW: BLOCKBEGIN\n"));

    while ((c_inb1(0x3da)&0x08)!=0); // Wait for end of vert. retrace if one is running
    while ((c_inb1(0x3da)&0x08)==0); // Wait for new start of retrace
    rc = DD_OK;
  }

  if(DDWAITVB_BLOCKEND == dwFlags)
  {
    dprintf(("DDRAW: BLOCKEND\n"));
    rc = DD_OK;
    if((c_inb1(0x3da)&0x08)!=0)        // Are we in a vert. retrace
    {
      while ((c_inb1(0x3da)&0x08)!=0); // Wait for end of retrace
    }
    else
    {
      while ((c_inb1(0x3da)&0x08)==0); // Wait for new start of retrace
      while ((c_inb1(0x3da)&0x08)!=0); // Wait for end of vert. retrace
    }

  }


  //  io_exit1();


  return (rc);
#endif
}

//******************************************************************************
//*** Added in the v2 interface ***
//******************************************************************************
HRESULT WIN32API DrawGetAvailableVidMem(THIS, LPDDSCAPS lpDDSCaps,
                                         LPDWORD lpdwTotal, LPDWORD lpdwFree)
{
  dprintf(("DDRAW: GetAvailableVidMem\n"));

  // Check parameters
  if(NULL==lpDDSCaps)
    return(DDERR_INVALIDPARAMS);

  if((NULL==lpdwTotal)&&(NULL==lpdwFree))
    return(DDERR_INVALIDPARAMS);

  if(NULL!=lpdwTotal)
    *lpdwTotal = 2048 *1024;

  if(NULL!=lpdwFree)
    *lpdwFree = 2048 *1024;

  return(DD_OK);
}
//******************************************************************************
//
//******************************************************************************
HRESULT WIN32API DrawGetAvailableVidMem4(THIS, LPDDSCAPS2 lpDDSCaps2,
                                         LPDWORD lpdwTotal, LPDWORD lpdwFree)
{
  dprintf(("DDRAW: GetAvailableVidMem\n"));

  // Check parameters
  if(NULL==lpDDSCaps2)
    return(DDERR_INVALIDPARAMS);

  if((NULL==lpdwTotal)&&(NULL==lpdwFree))
    return(DDERR_INVALIDPARAMS);

  if(NULL!=lpdwTotal)
    *lpdwTotal = 2048 *1024;

  if(NULL!=lpdwFree)
    *lpdwFree = 2048 *1024;

  return(DD_OK);
}
//******************************************************************************
//*** Added in the v4 interface ***
//******************************************************************************
HRESULT WIN32API DrawGetSurfaceFromDC(THIS, HDC hdc, LPDIRECTDRAWSURFACE4 *)
{
  dprintf(("DDRAW: GetSurfaceFromDC NYI\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawRestoreAllSurfaces(THIS)
{
  dprintf(("DDRAW: RestoreAllSurfaces\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawTestCooperativeLevel(THIS)
{
  dprintf(("DDRAW: TestCooperativeLevel\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DrawGetDeviceIdentifier( THIS, LPDDDEVICEIDENTIFIER lpdddi,
                                           DWORD dwFlags)
{
  dprintf(("DDRAW: GetDeviceIdentifier Flags = %d\n",dwFlags));
  if(NULL==lpdddi)
    return DDERR_INVALIDPARAMS;



  memset( lpdddi,
          0,
          sizeof(DDDEVICEIDENTIFIER));
  // ToDo: Cretae a GUID and put some better data inside
  strcpy( lpdddi->szDriver,
          "OS/2 DIVE Driver");
  strcpy( lpdddi->szDescription,
          "ODIN DD Emulation Driver");



  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
VOID OS2IDirectDraw::SwitchDisplay(HWND hwnd)
{
  DWORD dwVType, dwVSize;
  HKEY hkDirectDraw2;



  if (bScale)
  {
    if (ERROR_SUCCESS==RegOpenKeyA(HKEY_LOCAL_MACHINE,KEY_DIRECT2DRAW,&hkDirectDraw2))
    {
      dwVSize = 4;
      dwVType = REG_DWORD;
      if (ERROR_SUCCESS!=RegQueryValueExA( hkDirectDraw2, "Fullscreen", NULL, &dwVType,
                                         (LPBYTE)&bScale, &dwVSize))
        bScale = FALSE;
    }
    else
      bScale = FALSE;
  }


}
//******************************************************************************
// Setup table for 3d devices
//******************************************************************************
IDirect3DVtbl DDraw3DTable =
{
 D3DQueryInterface,
 D3DAddRef,
 D3DRelease,
 D3DInitialize,
 D3DEnumDevices,
 D3DCreateLight,
 D3DCreateMaterial,
 D3DCreateViewport,
 D3DFindDevice
};
//******************************************************************************
// Org Interface
//******************************************************************************
IDirectDrawVtbl DDrawV1Table =
{
 DrawQueryInterface,
 DrawAddRef,
 DrawRelease,
 DrawCompact,
 DrawCreateClipper,
 DrawCreatePalette,
 DrawCreateSurface,
 DrawDuplicateSurface,
 DrawEnumDisplayModes,
 DrawEnumSurfaces,
 DrawFlipToGDISurface,
 DrawGetCaps,
 DrawGetDisplayMode,
 DrawGetFourCCCodes,
 DrawGetGDISurface,
 DrawGetMonitorFrequency,
 DrawGetScanLine,
 DrawGetVerticalBlankStatus,
 DrawInitialize,
 DrawRestoreDisplayMode,
 DrawSetCooperativeLevel,
 DrawSetDisplayMode,
 DrawWaitForVerticalBlank
};
//******************************************************************************
// V2 Interface
//******************************************************************************
IDirectDraw2Vtbl DDrawV2Table =
{
 DrawQueryInterface,
 DrawAddRef,
 DrawRelease,
 DrawCompact,
 DrawCreateClipper,
 DrawCreatePalette,
 DrawCreateSurface,
 DrawDuplicateSurface,
 DrawEnumDisplayModes,
 DrawEnumSurfaces,
 DrawFlipToGDISurface,
 DrawGetCaps,
 DrawGetDisplayMode,
 DrawGetFourCCCodes,
 DrawGetGDISurface,
 DrawGetMonitorFrequency,
 DrawGetScanLine,
 DrawGetVerticalBlankStatus,
 DrawInitialize,
 DrawRestoreDisplayMode,
 DrawSetCooperativeLevel,
 DrawSetDisplayMode2,
 DrawWaitForVerticalBlank,
 DrawGetAvailableVidMem
};
//******************************************************************************
// New V4 interface
//******************************************************************************
IDirectDraw4Vtbl DDrawV4Table =
{
 DrawQueryInterface,
 DrawAddRef,   // todo change to a DrawAddRef4 as handling this has change,
 DrawRelease,  // see abov,
 DrawCompact,
 DrawCreateClipper,
 DrawCreatePalette,
 DrawCreateSurface4,
 DrawDuplicateSurface4,
 DrawEnumDisplayModes4,
 DrawEnumSurfaces4,
 DrawFlipToGDISurface,
 DrawGetCaps,
 DrawGetDisplayMode4,
 DrawGetFourCCCodes,
 DrawGetGDISurface4,
 DrawGetMonitorFrequency,
 DrawGetScanLine,
 DrawGetVerticalBlankStatus,
 DrawInitialize,
 DrawRestoreDisplayMode,
 DrawSetCooperativeLevel,
 DrawSetDisplayMode2,
 DrawWaitForVerticalBlank,
 DrawGetAvailableVidMem4,
 DrawGetSurfaceFromDC,
 DrawRestoreAllSurfaces,
 DrawTestCooperativeLevel,
 DrawGetDeviceIdentifier,
};
//******************************************************************************
//******************************************************************************
