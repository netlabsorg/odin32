/* $Id: OS2CLIPPER.CPP,v 1.14 2001-10-03 13:49:39 sandervl Exp $ */

/*
 * DX clipper class routines
 *
 * Copyright 1998-2001 Sander van Leeuwen
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: This is NOT THREAD SAFE!!!
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
#include <winuser32.h>
#include "oslibgpi.h"

//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::OS2IDirectDrawClipper(void) :
                 Referenced(0), lastError(DD_OK), fDrawingAllowed(TRUE),
                 clipWindow(0), lpRgnData(NULL), fClipListChanged(FALSE)
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

}

//******************************************************************************
//******************************************************************************
OS2IDirectDrawClipper::OS2IDirectDrawClipper(OS2IDirectDraw *lpDirectDraw) :
                 Referenced(0), lastError(DD_OK), fDrawingAllowed(TRUE),
                 clipWindow(0), lpRgnData(NULL), fClipListChanged(FALSE)
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
  if(lpRgnData) free(lpRgnData);
  if(clipWindow) {
      WinSetVisibleRgnNotifyProc(clipWindow, NULL, 0);
  }
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
  if(lpRect) {
      dprintf(("WARNING: clipping the cliplist is not yet implemented"));
  }
  if(me->clipWindow) 
  {
      RECT *rect = (RECT *)&lpClipList->Buffer;
      DWORD regiondatasize;

      if(me->lpRgnData == NULL) {
          DebugInt3();
          return DDERR_NOCLIPLIST;
      }
      //TODO: This is NOT THREAD SAFE!!!
      regiondatasize = me->lpRgnData->rdh.dwSize + me->lpRgnData->rdh.nCount*sizeof(RECT);
      if(lpClipList == NULL) {
          *lpdwSize = regiondatasize;
          return DD_OK;
      }

      if(*lpdwSize < regiondatasize) {
          *lpdwSize = regiondatasize;
          return DDERR_REGIONTOOSMALL;
      }
      memcpy(lpClipList, me->lpRgnData, regiondatasize);
      dprintf(("Boundary rectangle (%d,%d)(%d,%d)", lpClipList->rdh.rcBound.left, lpClipList->rdh.rcBound.top, lpClipList->rdh.rcBound.right, lpClipList->rdh.rcBound.bottom));
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
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: ClipIsClipListChanged %x %x", This, lpbChanged));

  if(lpbChanged == NULL) {
      return DDERR_INVALIDPARAMS;
  }
  *lpbChanged = me->fClipListChanged;
  me->fClipListChanged = FALSE;
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API ClipSetClipList(THIS This, LPRGNDATA lpClipList, DWORD dwFlags)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: ClipSetClipList %x %x %x", This, lpClipList, dwFlags));

  if(me->clipWindow) {
      return DDERR_CLIPPERISUSINGHWND;
  }
  if(lpClipList == NULL) {
      //TODO: should protect this with a critical section
      if(me->lpRgnData) {
          free(me->lpRgnData);
          me->lpRgnData = NULL;
      }
      return DD_OK;
  }
  //TODO: return error if a clip list is already active??
  if(me->lpRgnData) {
      free(me->lpRgnData);
      me->lpRgnData = NULL;
  }
  me->lpRgnData = (LPRGNDATA)malloc(lpClipList->rdh.dwSize + lpClipList->rdh.nCount*sizeof(RECT));
  memcpy(me->lpRgnData, lpClipList, lpClipList->rdh.dwSize + lpClipList->rdh.nCount*sizeof(RECT));
  return DD_OK;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ClipVisRgnCallback(HWND hwnd, BOOL fDrawingAllowed, DWORD dwUserData)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)dwUserData;
 LPRGNDATA lpClipList, lpRgnOld;

  dprintf(("DDRAW: ClipVisRgnCallback %x %x %x", me, hwnd, fDrawingAllowed));
  if(me && me->clipWindow == hwnd) {
      me->fClipListChanged = TRUE;
      lpRgnOld = me->lpRgnData;
      if(fDrawingAllowed == FALSE) {
          me->fDrawingAllowed = FALSE;
          lpClipList = (LPRGNDATA)malloc(sizeof(RGNDATA));
          lpClipList->rdh.dwSize   = sizeof(lpClipList->rdh);
          lpClipList->rdh.iType    = RDH_RECTANGLES;    // one and only possible value
          lpClipList->rdh.nCount   = 0;
          lpClipList->rdh.nRgnSize = 0;
          me->lpRgnData = lpClipList;
      }
      else {
          me->fDrawingAllowed = TRUE;
          me->lpRgnData = OSLibQueryVisibleRegion(hwnd, me->lpDraw->GetScreenHeight());
      }
      free(lpRgnOld);
  }
  return TRUE;
}
//******************************************************************************
//DWORD param not used in DirectX 3
//******************************************************************************
HRESULT WIN32API ClipSetHWnd(THIS This, DWORD reserved, HWND hwnd)
{
 OS2IDirectDrawClipper *me = (OS2IDirectDrawClipper *)This;

  dprintf(("DDRAW: ClipSetHWnd %x %x %x", This, reserved, hwnd));

  if(!IsWindow(hwnd)) {
      dprintf(("WARNING: ClipSetHWnd invalid window handle"));
      return DDERR_INVALIDPARAMS;
  }
  if(me->clipWindow) {
      //TODO: Is this really allowed??
      WinSetVisibleRgnNotifyProc(me->clipWindow, NULL, 0);
  }
  me->clipWindow = hwnd;
  WinSetVisibleRgnNotifyProc(hwnd, ClipVisRgnCallback, (DWORD)me);

#if 1
  me->lpRgnData = OSLibQueryVisibleRegion(hwnd, me->lpDraw->GetScreenHeight());
#else
  me->lpRgnData = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + sizeof(RECT));
  me->lpRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
  me->lpRgnData->rdh.iType  = RDH_RECTANGLES;
  me->lpRgnData->rdh.nCount = 1;
  me->lpRgnData->rdh.nRgnSize = sizeof(RECT);
  //TODO: Do we need to return the window rectangle or the client
  //      rectangle converted to screen coordinates??????
  GetWindowRect(me->clipWindow, &me->lpRgnData->rdh.rcBound);
  RECT *rect = (RECT *)&me->lpRgnData->Buffer;
  *rect = me->lpRgnData->rdh.rcBound;
#endif

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
