/* $Id: OS2CLIPPER.CPP,v 1.12 2001-05-28 17:17:09 phaller Exp $ */

/*
 * DX clipper class routines
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <memory.h>

#define INITGUID
#include "os2ddraw.h"
#include "os2clipper.h"
#define _OS2WIN_H
#define FAR
#include <misc.h>
#include <winerror.h>
#include <winuser.h>

//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::OS2IDirectDrawClipper(void) :
                 Referenced(0), lastError(DD_OK),
                 clipWindow(0)
{
  // this constructor creates an unassociated instance of the ddraw clipper,
  // no ddraw object is associated

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

  lpDraw                  = NULL;
  // lpDraw->Vtbl.AddRef(lpDraw);
  // hDive                   = lpDirectDraw->GetDiveInstance();
  windowRect.left = windowRect.right = windowRect.top = windowRect.bottom = 0;
}

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
  windowRect.left = windowRect.right = windowRect.top = windowRect.bottom = 0;
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::~OS2IDirectDrawClipper()
{
  lpDraw->Vtbl.Release(lpDraw);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API ClipQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("DDRAW: ClipQueryInterface\n"));

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
ULONG   WIN32API ClipAddRef(THIS This)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: OS2IDirectDrawClipper::AddRef %d\n", me->Referenced+1));

  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG   WIN32API ClipRelease(THIS This)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: OS2IDirectDrawClipper::Release %d", me->Referenced-1));

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
HRESULT WIN32API ClipGetClipList(THIS This, LPRECT lpRect, LPRGNDATA lpClipList, 
                                 LPDWORD lpdwSize)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;
 HRESULT rc = DD_OK;

  dprintf(("DDRAW: ClipGetClipList %x %x %x %x", This, lpRect, lpClipList, lpdwSize));

  if(!lpdwSize) {
	return DDERR_INVALIDPARAMS;
  }
  if(me->clipWindow) 
  {
      HRGN hRgn, hRgn1;
      HDC  hdc;
      int  bufneeded;

	hRgn = CreateRectRgn(0, 0, 1, 1);
	hdc = GetDC(me->clipWindow);
	if(GetClipRgn(hdc, hRgn)) {
		if(lpRect) {
			hRgn1 = CreateRectRgnIndirect(lpRect);
			CombineRgn(hRgn, hRgn, hRgn1, RGN_AND);
			DeleteObject(hRgn1);
		}
		bufneeded = GetRegionData(hRgn, *lpdwSize, lpClipList);
		if(bufneeded > *lpdwSize) {
			rc = DDERR_REGIONTOOSMALL;
		}
	}
	else	rc = DDERR_NOCLIPLIST;
	DeleteObject(hRgn);
	ReleaseDC(me->clipWindow, hdc);	
  }
  else { //todo!!
	dprintf(("ClipGetClipList not complete if not associated with window!"));
	rc = DDERR_NOCLIPLIST;
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API ClipGetHWnd(THIS This, HWND FAR *pHwnd)
{
  OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: ClipGetHWnd %x %x", This, me->clipWindow));

  *pHwnd = me->clipWindow;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API ClipInitialize(THIS This, LPDIRECTDRAW lpDD, DWORD dwFlags)
{
  OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: ClipInitialize %x %x %x", This, lpDD, dwFlags));

  // check parameters
  if (dwFlags != 0)
      return DDERR_INVALIDPARAMS;

  // check if already initialized
  if (me->hDive != NULL)
    return(DDERR_ALREADYINITIALIZED); // Init is done during creation see M$ Doc

  // initialize this instance (created with DirectDrawCreateClipper)

  if (lpDD != NULL)
  {
      OS2IDirectDraw *os2DD = (OS2IDirectDraw *)lpDD;
      me->lpDraw = os2DD;
      me->lpDraw->Vtbl.AddRef(me->lpDraw);
      me->hDive = me->lpDraw->GetDiveInstance();
  }
  else
  {
      // we're supposed to create an independent DirectDrawClipper object
      // now.
      if (me->lpDraw != NULL)
      {
          // we're not handling this any other currently
          return(DDERR_ALREADYINITIALIZED);
      }
      // else
      //   we've been independent and remain so ...
  }

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API ClipIsClipListChanged(THIS This, BOOL *lpbChanged)
{
  dprintf(("DDRAW: ClipIsClipListChanged %x %x", This, lpbChanged));

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API ClipSetClipList(THIS This, LPRGNDATA lpClipList, DWORD lpdwSize)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: ClipSetClipList %x %x %x", This, lpClipList, lpdwSize));

  if(me->clipWindow) {
	return DDERR_CLIPPERISUSINGHWND;
  }
  return(DD_OK);
}
//******************************************************************************
//DWORD param not used in DirectX 3
//******************************************************************************
HRESULT WIN32API ClipSetHWnd(THIS This, DWORD reserved, HWND hwnd)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: ClipSetHWnd %x %x %x", This, reserved, hwnd));

  me->clipWindow = hwnd;
  GetClientRect(hwnd, &me->windowRect);

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
