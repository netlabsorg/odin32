/* $Id: OS2D3D.CPP,v 1.3 1999-06-10 17:10:56 phaller Exp $ */

//******************************************************************************
//******************************************************************************
// Direct3D Main Class
//
// Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
//******************************************************************************
//******************************************************************************
// *
// * Project Odin Software License can be found in LICENSE.TXT
// *

/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define WIN32SDK_NOPOSTWRAPPER

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <dive.h>

#include <builtin.h>
#include <memory.h>

#include "no.h"
#include <w_windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <Win32SDKPostWrapper.h>

#include "os2ddraw.h"
#include "os2clipper.h"
#include "os2palette.h"
#include "os2surface.h"
#include "misc.h"

/*** IDirect3D methods ***/
/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirect3D


//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("D3DQueryInterface\n"));
  *ppvObj = NULL;

  if(IID_IDirect3D != (GUID &)&riid)
    return E_NOINTERFACE;

  *ppvObj = This;

  D3DAddRef(This);
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall D3DAddRef(THIS)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

  dprintf(("OS2IDirectDraw::AddRef %d\n", me->Referenced+1));
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall D3DRelease(THIS)
{
 OS2IDirectDraw *me = (OS2IDirectDraw *)This;

  dprintf(("OS2IDirectDraw::Release %d\n", me->Referenced-1));
  dprintf(("OS2IDirectDraw::%X \n", me));
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
HRESULT __stdcall D3DInitialize(THIS_ REFIID)
{
  dprintf(("D3DInitialize\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DEnumDevices(THIS_ LPD3DENUMDEVICESCALLBACK, LPVOID)
{
  dprintf(("D3DEnumDevices\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DCreateLight(THIS_ LPDIRECT3DLIGHT*, IUnknown*)
{
  dprintf(("D3DCreateLight\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DCreateMaterial(THIS_ LPDIRECT3DMATERIAL*, IUnknown*)
{
  dprintf(("D3DCreateMaterial\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DCreateViewport(THIS_ LPDIRECT3DVIEWPORT*, IUnknown*)
{
  dprintf(("D3DCreateViewport\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall D3DFindDevice(THIS_ LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT)
{
  dprintf(("D3DCreateFindDevice\n"));
  return(D3D_OK);
}
//******************************************************************************
//******************************************************************************
