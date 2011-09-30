/* $Id: surfoverlay.cpp,v 1.1 2002-12-29 14:11:03 sandervl Exp $ */

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



//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUpdateOverlay(THIS This, LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX)
{
  dprintf(("DDRAW: SurfUpdateOverlay NOT IMPLEMENTED"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUpdateOverlay3(THIS This, LPRECT, LPDIRECTDRAWSURFACE3,LPRECT,DWORD, LPDDOVERLAYFX)
{
  dprintf(("DDRAW: SurfUpdateOverlay  NOT IMPLEMENTED"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUpdateOverlay4(THIS, LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX)
{
  dprintf(("DDRAW: SurfUpdateOverlay NOT IMPLEMENTED"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUpdateOverlayDisplay(THIS, DWORD)
{
  dprintf(("DDRAW: SurfUpdateOverlayDisplay  NOT IMPLEMENTED"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUpdateOverlayZOrder(THIS, DWORD, LPDIRECTDRAWSURFACE2)
{
  dprintf(("DDRAW: SurfUpdateOverlayZOrder NOT IMPLEMENTED"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUpdateOverlayZOrder3(THIS, DWORD, LPDIRECTDRAWSURFACE3)
{
  dprintf(("DDRAW: SurfUpdateOverlayZOrder NOT IMPLEMENTED"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfUpdateOverlayZOrder4(THIS, DWORD, LPDIRECTDRAWSURFACE4)
{
  dprintf(("DDRAW: SurfUpdateOverlayZOrder4 NOT IMPLEMENTED"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfGetOverlayPosition(THIS This, LPLONG lplX, LPLONG lplY)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;
  dprintf(("DDRAW: SurfGetOverlayPosition\n"));

  // Maybe simply return dderr_notsupported as we retun a max overlay value of 0 in the caps ?

  if( (NULL==lplX) || (NULL==lplY))
    return DDERR_INVALIDPARAMS;

  if(!(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_OVERLAY))
    return DDERR_NOTAOVERLAYSURFACE;

  if(!(me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_VISIBLE))
    return DDERR_OVERLAYNOTVISIBLE;

  if(!me->fOverlayValid)
    return DDERR_NOOVERLAYDEST;

  *lplX = me->lOverlayX;
  *lplY = me->lOverlayY;

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfSetOverlayPosition(THIS This, LONG lX, LONG lY)
{
  OS2IDirectDrawSurface *me = (OS2IDirectDrawSurface *)This;

  dprintf(("DDRAW: SurfSetOverlayPosition\n"));

  if( (me->DDSurfaceDesc.dwFlags & DDSD_CAPS) &&
      (me->DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_OVERLAY) )
  {
    if(me->fOverlayValid)
      return(DDERR_NOOVERLAYDEST);

    if(!(me->DDSurfaceDesc.dwFlags & DDSCAPS_VISIBLE))
      return(DDERR_OVERLAYNOTVISIBLE);

    // ToDo: If we implement alignment restricions to the Overlay position
    // check if the new values are OK otherwiese return DDERR_INVALIDPOSITION

    me->lOverlayX = lX;
    me->lOverlayY = lY;
    return(DD_OK);
  }

  return(DDERR_NOTAOVERLAYSURFACE);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfAddOverlayDirtyRect(THIS, LPRECT)
{
  dprintf(("DDRAW: SurfAddOverlayDirtyRect Not implemented by M$ in DX 6.0!\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfEnumOverlayZOrders(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK)
{
  dprintf(("DDRAW: SurfEnumOverlayZOrders\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SurfEnumOverlayZOrders4(THIS, DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2)
{
  dprintf(("DDRAW: SurfEnumOverlayZOrders\n"));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
