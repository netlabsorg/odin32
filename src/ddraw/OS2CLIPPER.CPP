/* $Id: OS2CLIPPER.CPP,v 1.3 1999-06-10 17:10:56 phaller Exp $ */

/*
 * DirectDraw Clipper Class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_GUID
#define WIN32SDK_NOPOSTWRAPPER

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <dive.h>

#include <memory.h>

#include "no.h"
#include <w_windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <Win32SDKPostWrapper.h>

#include "os2ddraw.h"
#include "os2clipper.h"
#include "misc.h"

/*** IDirect3D methods ***/
/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDrawClipper


//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::OS2IDirectDrawClipper(OS2IDirectDraw *lpDirectDraw) :
                           Referenced(0), lastError(DD_OK),
                           clipWindow(0)
{
  lpVtbl                    = &Vtbl;
  Vtbl.AddRef               = ClipAddRef;
  Vtbl.Release              = ClipRelease;
  Vtbl.QueryInterface       = ClipQueryInterface;
  Vtbl.GetClipList          = ClipGetClipList;
  Vtbl.GetHWnd              = ClipGetHWnd;
  Vtbl.Initialize           = ClipInitialize;
  Vtbl.IsClipListChanged    = ClipIsClipListChanged;
  Vtbl.SetClipList          = ClipSetClipList;
  Vtbl.SetHWnd              = ClipSetHWnd;

  lpDraw                        = lpDirectDraw;
  lpDraw->Vtbl.AddRef((IDirectDraw2*)lpDraw);
  hDive                         = lpDirectDraw->GetDiveInstance();
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::~OS2IDirectDrawClipper()
{
  lpDraw->Vtbl.Release((IDirectDraw2*)lpDraw);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipQueryInterface(THIS_ REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("ClipQueryInterface\n"));
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
ULONG   __stdcall ClipAddRef(THIS)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("OS2IDirectDrawClipper::AddRef %d\n", me->Referenced+1));
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG   __stdcall ClipRelease(THIS)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("OS2IDirectDrawClipper::Release %d\n", me->Referenced-1));
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
HRESULT __stdcall ClipGetClipList(THIS_ W32_LPRECT, W32_LPRGNDATA, LPDWORD)
{
  dprintf(("ClipGetClipList\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipGetHWnd(THIS_ W32_HWND FAR *pHwnd)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("ClipGetHWnd\n"));
  *pHwnd = me->clipWindow;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipInitialize(THIS_ LPDIRECTDRAW, DWORD)
{
  dprintf(("ClipInitialize\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipIsClipListChanged(THIS_ BOOL FAR *)
{
  dprintf(("ClipIsClipListChanged\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall ClipSetClipList(THIS_ W32_LPRGNDATA,DWORD)
{
  dprintf(("ClipSetClipList\n"));
  return(DD_OK);
}
//******************************************************************************
//DWORD param not used in DirectX 3
//******************************************************************************
HRESULT __stdcall ClipSetHWnd(THIS_ DWORD reserved, W32_HWND hwnd)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("ClipSetHWnd\n"));
  me->clipWindow = hwnd;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
