/* $Id: OS2CLIPPER.CPP,v 1.1 2000-04-07 18:21:03 mike Exp $ */

/*
 * DX clipper class routines
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <memory.h>

#define INITGUID
#include "os2ddraw.h"
#include "os2clipper.h"
#define _OS2WIN_H
#define FAR
#include <misc.h>
#include <winerror.h>

//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::OS2IDirectDrawClipper(OS2IDirectDraw *lpDirectDraw) :
                 Referenced(0), lastError(DD_OK),
                 clipWindow(0)
{
  lpVtbl                  = &Vtbl;
  Vtbl.AddRef             = ClipAddRef;
  Vtbl.Release            = ClipRelease;
  Vtbl.QueryInterface     = ClipQueryInterface;
  Vtbl.GetClipList        = ClipGetClipList;
  Vtbl.GetHWnd            = ClipGetHWnd;
  Vtbl.Initialize         = ClipInitialize;
  Vtbl.IsClipListChanged  = ClipIsClipListChanged;
  Vtbl.SetClipList        = ClipSetClipList;
  Vtbl.SetHWnd            = ClipSetHWnd;

  lpDraw                  = lpDirectDraw;
  lpDraw->Vtbl.AddRef(lpDraw);
  hDive                   = lpDirectDraw->GetDiveInstance();
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::~OS2IDirectDrawClipper()
{
  lpDraw->Vtbl.Release(lpDraw);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
  #ifdef DEBUG
    dprintf(("DDRAW: ClipQueryInterface\n"));
  #endif

  *ppvObj = NULL;

  if(!IsEqualGUID(riid, IID_IDirectDrawClipper) &&
     !IsEqualGUID(riid, CLSID_DirectDrawClipper))
//&& !IsEqualGUID(riid, IID_IUnknown))
  return E_NOINTERFACE;

  *ppvObj = This;

  ClipAddRef(This);
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
ULONG   __stdcall ClipAddRef(THIS This)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  #ifdef DEBUG
    dprintf(("DDRAW: OS2IDirectDrawClipper::AddRef %d\n", me->Referenced+1));
  #endif

  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG   __stdcall ClipRelease(THIS This)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  #ifdef DEBUG
    dprintf(("DDRAW: OS2IDirectDrawClipper::Release %d\n", me->Referenced-1));
  #endif

  if(me->Referenced)
  {
    me->Referenced--;
    if(me->Referenced == 0)
    {
      delete( me);
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
HRESULT __stdcall ClipGetClipList(THIS, LPRECT, LPRGNDATA, LPDWORD)
{
  #ifdef DEBUG
    dprintf(("DDRAW: ClipGetClipList\n"));
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipGetHWnd(THIS This, HWND FAR *pHwnd)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  #ifdef DEBUG
    dprintf(("DDRAW: ClipGetHWnd\n"));
  #endif

  *pHwnd = me->clipWindow;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipInitialize(THIS, LPDIRECTDRAW, DWORD)
{
  #ifdef DEBUG
    dprintf(("DDRAW: ClipInitialize\n"));
  #endif

  return(DDERR_ALREADYINITIALIZED); // Init is done during creation see M$ Doc
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipIsClipListChanged(THIS, BOOL FAR *)
{
  #ifdef DEBUG
    dprintf(("DDRAW: ClipIsClipListChanged\n"));
  #endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipSetClipList(THIS, LPRGNDATA,DWORD)
{
  #ifdef DEBUG
    dprintf(("DDRAW: ClipSetClipList\n"));
  #endif

  return(DD_OK);
}
//******************************************************************************
//DWORD param not used in DirectX 3
//******************************************************************************
HRESULT __stdcall ClipSetHWnd(THIS This, DWORD reserved, HWND hwnd)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  #ifdef DEBUG
    dprintf(("DDRAW: ClipSetHWnd\n"));
  #endif

  me->clipWindow = hwnd;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
