/*
 * DirectDraw exported APIs
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INITGUID
#define INCL_GUIDS
#define WIN32SDK_NOPOSTWRAPPER

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <memory.h>
#include <builtin.h>
#include <dive.h>

#include <w_windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <Win32SDKPostWrapper.h>

#include "os2ddraw.h"
#include "misc.h"

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectDrawCreate(GUID FAR *lpGUID,
                                 LPDIRECTDRAW FAR *lplpDD,
                             IUnknown FAR *pUnkOuter)
{
 OS2IDirectDraw *newdraw = new OS2IDirectDraw(lpGUID);;
 HRESULT         rc;

  dprintf(("DirectDrawCreate %X %X %X\n", lpGUID, lplpDD, pUnkOuter));

  if(newdraw == NULL)   return(DDERR_NODIRECTDRAWHW);

//  newdraw->Vtbl.AddRef((IDirectDraw *)newdraw);
  rc = newdraw->GetLastError();
  if(rc != DD_OK) {
    *lplpDD = NULL;
    delete newdraw;
  }
  else  *lplpDD = (LPDIRECTDRAW)newdraw;
  return(rc);
}
//******************************************************************************
#if 0  //KSO Apr 19 1999: not needed any longer!
typedef BOOL (FAR PASCAL * LPDDENUMCALLBACKA)(GUID FAR *, LPSTR, LPSTR, LPVOID);
#endif
//******************************************************************************
HRESULT WIN32API OS2DirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback,
                             LPVOID lpContext)
{
  dprintf(("DirectDrawEnumerateA\n"));
  //call it twice for the DirectDraw & Direct3D classes
  if(lpCallback(NULL, "DIVE DirectDraw for OS/2",
        "DirectDraw/2 v0.1", lpContext) == DDENUMRET_CANCEL) {
    return(DD_OK);
  }
  else //now for Direct3D
  if(lpCallback((GUID *)&IID_IDirect3D, "3Dfx Voodoo Direct3D/2",
        "Direct3D/2 v0.1", lpContext) == DDENUMRET_CANCEL) {
    return(DD_OK);
  }
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DDHAL32_VidMemFree(DWORD address)
{
  dprintf(("DDHAL32_VidMemFree, not supported\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DDHAL32_VidMemAlloc(DWORD size)
{
  dprintf(("DDHAL32_VidMemAlloc, not supported\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
