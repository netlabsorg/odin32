/* $Id: ddraw3d.cpp,v 1.1 2002-12-04 10:35:00 sandervl Exp $ */

/*
 * DX clipper class routines
 *
 * Copyright 1998 Sander van Leeuwen
 *
 * Note: MUST use DDraw3D_GetThisPtr macro to access DirectDraw object!
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <memory.h>

#define INITGUID
#define _OS2WIN_H
#define FAR

#include <odin.h>
#include <winbase.h>
#include "ddraw2d.h"
#include "clipper.h"
#include "palette.h"
#include "surface.h"
#include <misc.h>
#include <winerror.h>
#undef THIS
#define THIS VOID*

//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("DDRAW: D3DQueryInterface\n"));
  *ppvObj = NULL;

  if (!IsEqualGUID(riid, IID_IDirect3D))
    return E_NOINTERFACE;

  *ppvObj = This;

  D3DAddRef(This);
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall D3DAddRef(THIS This)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: OS2IDirectDraw::AddRef %d\n", me->Referenced+1));
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall D3DRelease(THIS This)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: OS2IDirectDraw::Release %d\n", me->Referenced-1));
  dprintf(("DDRAW: OS2IDirectDraw::%X \n", me));
  if(me->Referenced)
  {
    me->Referenced--;
    if(me->Referenced == 0)
    {
      delete(me);
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
HRESULT __stdcall D3DInitialize(THIS This, REFIID)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: D3DInitialize\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DEnumDevices(THIS This, LPD3DENUMDEVICESCALLBACK, LPVOID)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: D3DEnumDevices\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DCreateLight(THIS This, LPDIRECT3DLIGHT*, IUnknown*)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: D3DCreateLight\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DCreateMaterial(THIS This, LPDIRECT3DMATERIAL*, IUnknown*)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: D3DCreateMaterial\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DCreateViewport(THIS This, LPDIRECT3DVIEWPORT*, IUnknown*)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: D3DCreateViewport\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DFindDevice(THIS This, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT)
{
 OS2IDirectDraw *me = DDraw3D_GetThisPtr(This);

  dprintf(("DDRAW: D3DCreateFindDevice\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
