/* $Id: OS2DDRAW.CPP,v 1.3 1999-06-10 17:10:56 phaller Exp $ */

/*
 * DirectDraw main class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define WIN32SDK_NOPOSTWRAPPER

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <dive.h>

#include <memory.h>
#include <builtin.h>

#include "no.h"
#include <w_windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <Win32SDKPostWrapper.h>

#include <fourcc.h>

#include "os2ddraw.h"
#include "os2clipper.h"
#include "os2palette.h"
#include "os2surface.h"
#include "misc.h"
#include "os2util.h"


/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDraw2



DIVE_CAPS dcaps           = {0};
FOURCC    fccFormats[100] = {0};        /* Color format code                 */


//******************************************************************************
//******************************************************************************
OS2IDirectDraw::OS2IDirectDraw(GUID *lpGUID) :
       Referenced(0), lastError(DD_OK),
       pFrameBuffer(NULL), hwndClient(0), screenwidth(640),
       screenheight(480), screenbpp(8), fFullScreen(TRUE)
{
  if(lpGUID && *lpGUID == IID_IDirect3D) {
      lpVtbl = (IDirectDraw2Vtbl *)&Vtbl3D;
      Vtbl3D.AddRef             = D3DAddRef;
      Vtbl3D.Release            = D3DRelease;
      Vtbl3D.QueryInterface     = D3DQueryInterface;
      Vtbl3D.Initialize         = D3DInitialize;
      Vtbl3D.EnumDevices        = D3DEnumDevices;
      Vtbl3D.CreateLight        = D3DCreateLight;
      Vtbl3D.CreateMaterial     = D3DCreateMaterial;
      Vtbl3D.CreateViewport     = D3DCreateViewport;
      Vtbl3D.FindDevice         = D3DFindDevice;
  }
  else {
      lpVtbl = &Vtbl;
      Vtbl.AddRef                   = DrawAddRef;
      Vtbl.Release                  = DrawRelease;
      Vtbl.QueryInterface           = DrawQueryInterface;
      Vtbl.Compact                  = DrawCompact;
      Vtbl.CreateClipper            = DrawCreateClipper;
      Vtbl.W32_CreatePalette        = DrawCreatePalette; //KSO Apr 19 1999: Sideeffect of the wrapping...
      Vtbl.CreateSurface            = DrawCreateSurface;
      Vtbl.DuplicateSurface         = DrawDuplicateSurface;
      Vtbl.EnumDisplayModes         = DrawEnumDisplayModes;
      Vtbl.EnumSurfaces             = DrawEnumSurfaces;
      Vtbl.FlipToGDISurface         = DrawFlipToGDISurface;
      Vtbl.GetCaps                  = DrawGetCaps;
      Vtbl.GetDisplayMode           = DrawGetDisplayMode;
      Vtbl.GetFourCCCodes           = DrawGetFourCCCodes;
      Vtbl.GetGDISurface            = DrawGetGDISurface;
      Vtbl.GetMonitorFrequency      = DrawGetMonitorFrequency;
      Vtbl.GetScanLine              = DrawGetScanLine;
      Vtbl.GetVerticalBlankStatus   = DrawGetVerticalBlankStatus;
      Vtbl.Initialize               = DrawInitialize;
      Vtbl.RestoreDisplayMode       = DrawRestoreDisplayMode;
      Vtbl.SetCooperativeLevel      = DrawSetCooperativeLevel;

      if(lpGUID && *lpGUID == IID_IDirectDraw2)
            *(ULONG *)&Vtbl.SetDisplayMode  = (ULONG)DrawSetDisplayMode2;
      else  *(ULONG *)&Vtbl.SetDisplayMode  = (ULONG)DrawSetDisplayMode;

      Vtbl.WaitForVerticalBlank = DrawWaitForVerticalBlank;
      Vtbl.GetAvailableVidMem   = DrawGetAvailableVidMem;
  }

  ULONG rc = DiveOpen(&hDive, FALSE, &pFrameBuffer);
  if(rc) {
    dprintf(("ERROR: DiveOpen returned %d\n", rc));
    lastError = DDERR_GENERIC;
    hDive     = NULL;
  }
  dcaps.pFormatData    = fccFormats;
  dcaps.ulFormatLength = sizeof(fccFormats);
  dcaps.ulStructLen    = sizeof(dcaps);
  DiveQueryCaps(&dcaps, DIVE_BUFFER_SCREEN);
}
//******************************************************************************
//******************************************************************************
OS2IDirectDraw::~OS2IDirectDraw()
{
  dprintf(("OS2IDirectDraw::~OS2IDirectDraw()\n"));
  if(hDive) DiveClose(hDive);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj)
{
#ifdef DEBUG
  WriteLog("OS2IDirectDraw::QueryInterface\n");
#endif
  *ppvObj = NULL;

  if(!IsEqualGUID(riid, CLSID_DirectDraw) &&
     !IsEqualGUID(riid, IID_IDirectDraw) &&
     !IsEqualGUID(riid, IID_IDirectDraw2))
//&& !IsEqualGUID(riid, IID_IUnknown))
    return E_NOINTERFACE;

  *ppvObj = This;

  DrawAddRef(This);
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall DrawAddRef(THIS)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

#ifdef DEBUG
  WriteLog("OS2IDirectDraw::AddRef %d\n", me->Referenced+1);
#endif
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall DrawRelease(THIS)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

#ifdef DEBUG
  WriteLog("OS2IDirectDraw::Release %d\n", me->Referenced-1);
  WriteLog("OS2IDirectDraw::%X \n", me);
#endif
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
HRESULT __stdcall DrawCompact(THIS)
{
#ifdef DEBUG
  WriteLog("Compact\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawCreateClipper(THIS, DWORD, LPDIRECTDRAWCLIPPER FAR *lplpDD, IUnknown FAR * )
{
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;
 OS2IDirectDrawClipper *newclip = new OS2IDirectDrawClipper(me);
 HRESULT                rc;

#ifdef DEBUG
  WriteLog("CreateClipper\n");
#endif
  if(newclip == NULL)   return(DDERR_OUTOFMEMORY);

  newclip->Vtbl.AddRef((IDirectDrawClipper *)newclip);
  rc = newclip->GetLastError();
  if(rc != DD_OK) {
        *lplpDD = NULL;
        delete newclip;
  }
  else  *lplpDD = (IDirectDrawClipper *)newclip;

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawCreatePalette(THIS, DWORD dwFlags,
                    W32_LPPALETTEENTRY lpColorTable,
                    LPDIRECTDRAWPALETTE FAR *lplpDD,
                    IUnknown FAR *pUnkOuter)
{
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;
 OS2IDirectDrawPalette *newpal;
 HRESULT                rc;
 int                    palsize = 0;

  if(dwFlags & (DDPCAPS_2BIT | DDPCAPS_4BIT) && !(dwFlags & DDPCAPS_8BITENTRIES)) {
    //only support 8 bits color palettes...
    return DDERR_INVALIDPARAMS;
  }
  if(dwFlags & DDPCAPS_2BIT)     palsize = 4;
  if(dwFlags & DDPCAPS_4BIT)     palsize = 16;
  if(dwFlags & DDPCAPS_8BIT)     palsize = 256;
  if(dwFlags & DDPCAPS_ALLOW256) palsize = 256;
  if(palsize == 0)  return DDERR_INVALIDPARAMS;

  dprintf(("CreatePalette with %d colors\n", palsize));

  newpal = new OS2IDirectDrawPalette(me, palsize, lpColorTable);
  if(newpal == NULL)    return(DDERR_OUTOFMEMORY);

  newpal->Vtbl.AddRef((IDirectDrawPalette *)newpal);
  rc = newpal->GetLastError();
  if(rc != DD_OK) {
        *lplpDD = NULL;
        delete newpal;
  }
  else  *lplpDD = (IDirectDrawPalette *)newpal;

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawCreateSurface(THIS, LPDDSURFACEDESC lpDDSurfaceDesc,
                    LPDIRECTDRAWSURFACE FAR *lplpDD,
                    IUnknown FAR *pUnkOuter)
{
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;
 OS2IDirectDrawSurface *newsurf;
 HRESULT                rc;

#ifdef DEBUG
  WriteLog("CreateSurface\n");
  WriteLog("  dwSize %d\n", lpDDSurfaceDesc->dwSize);
  WriteLog("  dwFlags %X\n", lpDDSurfaceDesc->dwFlags);
  WriteLog("  dwHeight %d\n", lpDDSurfaceDesc->dwHeight);
  WriteLog("  dwWidth %d\n", lpDDSurfaceDesc->dwWidth);
  WriteLog("  lPitch %d\n", lpDDSurfaceDesc->lPitch);
  WriteLog("  lpSurface %d\n", lpDDSurfaceDesc->lpSurface);
  WriteLog("  dwBackBufferCount %d\n", lpDDSurfaceDesc->dwBackBufferCount);
  WriteLog("  dwMipMapCount %d\n", lpDDSurfaceDesc->dwMipMapCount);
  WriteLog("  dwAlphaBitDepth %d\n", lpDDSurfaceDesc->dwAlphaBitDepth);
  WriteLog("  ddsCaps.dwCaps %X\n", lpDDSurfaceDesc->ddsCaps.dwCaps);
#endif

  newsurf = new OS2IDirectDrawSurface(me, lpDDSurfaceDesc);
  if(newsurf == NULL)   return(DDERR_OUTOFMEMORY);

  newsurf->Vtbl.AddRef((IDirectDrawSurface2 *)newsurf);
  rc = newsurf->GetLastError();
  if(rc != DD_OK) {
        *lplpDD = NULL;
        delete newsurf;
  }
  else  *lplpDD = (IDirectDrawSurface *)newsurf;

  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * )
{
#ifdef DEBUG
  WriteLog("DuplicateSurface\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawEnumDisplayModes(THIS, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK )
{
#ifdef DEBUG
  WriteLog("EnumDisplayModes\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK )
{
#ifdef DEBUG
  WriteLog("EnumSurfaces\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawFlipToGDISurface(THIS)
{
#ifdef DEBUG
  WriteLog("FlipToGDISurface\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawGetCaps(THIS, LPDDCAPS, LPDDCAPS)
{
#ifdef DEBUG
  WriteLog("GetCaps\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawGetDisplayMode(THIS, LPDDSURFACEDESC)
{
#ifdef DEBUG
  WriteLog("GetDisplayMode\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawGetFourCCCodes(THIS, LPDWORD, LPDWORD)
{
#ifdef DEBUG
  WriteLog("GetFourCCCodes\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE FAR *)
{
#ifdef DEBUG
  WriteLog("GetGDISurface\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawGetMonitorFrequency(THIS, LPDWORD)
{
#ifdef DEBUG
  WriteLog("GetMonitorFrequency\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawGetScanLine(THIS, LPDWORD)
{
#ifdef DEBUG
  WriteLog("GetScanLine\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawGetVerticalBlankStatus(THIS, LPBOOL)
{
#ifdef DEBUG
  WriteLog("GetVerticalBlankStatus\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawInitialize(THIS, GUID FAR *)
{
#ifdef DEBUG
  WriteLog("Initialize\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawRestoreDisplayMode(THIS)
{
#ifdef DEBUG
  WriteLog("RestoreDisplayMod\n");
#endif
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawSetCooperativeLevel(THIS, W32_HWND hwndClient, DWORD dwFlags)
{
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;

  dprintf(("SetCooperativeLevel: hwnd %X, Flags %X\n", hwndClient, dwFlags));
  if(dwFlags & DDSCL_FULLSCREEN) {
        me->fFullScreen = TRUE;
  }
  else  me->fFullScreen = FALSE;

  me->hwndClient = hwndClient;
  OS2MaximizeWindow((HWND)hwndClient);
  return(DD_OK);
}
//******************************************************************************
//Backwards compatibility, what's that??
//******************************************************************************
HRESULT __stdcall DrawSetDisplayMode2(THIS, DWORD dwWidth, DWORD dwHeight,
                      DWORD dwBPP, DWORD dwRefreshRate,
                      DWORD dwFlags)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

  dprintf(("SetDisplayMode2 to %dx%d with %d bits colors\n", dwWidth, dwHeight, dwBPP));
  me->screenwidth  = dwWidth;
  me->screenheight = dwHeight;
  me->screenbpp    = dwBPP;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawSetDisplayMode(THIS, DWORD dwWidth, DWORD dwHeight,
                     DWORD dwBPP)
{
 OS2IDirectDraw        *me = (OS2IDirectDraw *)This;

//  _interrupt(3);
  dprintf(("SetDisplayMode to %dx%d with %d bits colors\n", dwWidth, dwHeight, dwBPP));
  me->screenwidth  = dwWidth;
  me->screenheight = dwHeight;
  me->screenbpp    = dwBPP;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall DrawWaitForVerticalBlank(THIS, DWORD, W32_HANDLE)
{
  dprintf(("WaitForVerticalBlank\n"));
  return(DD_OK);
}
//******************************************************************************
/*** Added in the v2 interface ***/
//******************************************************************************
HRESULT __stdcall DrawGetAvailableVidMem(THIS, LPDDSCAPS, LPDWORD, LPDWORD)
{
  dprintf(("GetAvailableVidMem\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
