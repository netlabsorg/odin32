/* $Id: ddraw.CPP,v 1.2 2001-01-19 02:06:55 mike Exp $ */

/*
 * DXDraw DLL implementaion
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1999 Markus Montkowski
 *
 * WARNING: DirectDrawCreate defaults to ddraw v4 if lpGUID == NULL!!
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <memory.h>
#include <stdio.h>
#include <builtin.h>
#define INITGUID
#define ICOM_CINTERFACE 1
#define CINTERFACE

#include "os2ddraw.h"
#include "winerror.h"
// define the following as we include winnt.h
#define _OS2WIN_H
#define FAR

#include <misc.h>

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectDrawCreate( GUID FAR *lpGUID,
                                      LPDIRECTDRAW FAR *lplpDD,
                                      IUnknown FAR *pUnkOuter)
{
  OS2IDirectDraw *newdraw;
  HRESULT         rc;

  dprintf(("DDRAW: DirectDrawCreate %X %X %X\n", lpGUID, lplpDD, pUnkOuter));

  newdraw = new OS2IDirectDraw(lpGUID);

  if(newdraw == NULL)
  {
    rc = DDERR_NODIRECTDRAWHW;
  }
  else
  {
    newdraw->Vtbl.AddRef((IDirectDraw2 *)newdraw);

    rc = newdraw->GetLastError();
    if(rc != DD_OK)
    {
      *lplpDD = NULL;

      delete newdraw;
    }
    else
      *lplpDD = (LPDIRECTDRAW)newdraw;
  }
  return(rc);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API DirectDrawCreateEx(LPGUID lpGUID, LPVOID* lplpDD, REFIID iid, LPUNKNOWN pUnkOuter)
{
  dprintf(("DirectDrawCreateEx: %x %x %x %x", lpGUID, lplpDD, iid, pUnkOuter));
  /* I don't know about what functionality is unique to Ex */
  return OS2DirectDrawCreate(lpGUID,(LPDIRECTDRAW*)lplpDD,pUnkOuter);
}

//******************************************************************************
typedef BOOL (FAR PASCAL * LPDDENUMCALLBACKA)(GUID FAR *, LPSTR, LPSTR, LPVOID);
//******************************************************************************
HRESULT WIN32API OS2DirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback,
               LPVOID lpContext)
{
  dprintf(("DDRAW: DirectDrawEnumerateA\n Callback for DIVE\n"));
  //call it twice for the DirectDraw & Direct3D classes
 if(lpCallback(NULL, "DIVE DirectDraw for OS/2",
                "DirectDraw/2 v0.2", lpContext) == DDENUMRET_CANCEL)
  {
    dprintf(("DDRAW: Cancel Callback\n"));
    return(DD_OK);
  }
  else //now for Direct3D
  {
    dprintf(("DDRAW: Callback for 3Dfx Voodoo"));
    if(lpCallback((GUID *)&IID_IDirect3D, "3Dfx Voodoo Direct3D/2",
      "Direct3D/2 v0.2", lpContext) == DDENUMRET_CANCEL)
    {
      dprintf(("DDRAW: Cancel Callback\n"));
      return(DD_OK);
    }
  }
  dprintf(("DDRAW: Done Enumeration\n\n"));

  return(DD_OK);
}

//******************************************************************************
typedef struct
{
  LPDDENUMCALLBACKEXA lpCallbackEx;
  LPVOID lpContext;
} ENUMDATA, *PENUMDATA;

BOOL FAR PASCAL SimpleEnum ( GUID FAR *lpGUID,
                             LPSTR  lpDriverDescription,
                             LPSTR  lpDriverName,
                             LPVOID lpContext)
{
  BOOL rc;
  PENUMDATA pData;

  dprintf(("DDRAW: SimpleEnum\n"));

  pData = (PENUMDATA)lpContext;
  rc = pData->lpCallbackEx( lpGUID,
                            lpDriverDescription,
                            lpDriverName,
                            pData->lpContext,
                            NULL);

  dprintf(("DDRAW: Callback returned\n"));
  return rc;
}

//******************************************************************************
HRESULT WIN32API OS2DirectDrawEnumerateExA( LPDDENUMCALLBACKEXA lpCallbackEx,
                                            LPVOID lpContext,
                                            DWORD dwFlags)
{
  ENUMDATA data;

  dprintf(("DDRAW: DirectDrawEnumerateExA\n"));

  data.lpCallbackEx = lpCallbackEx;
  data.lpContext    = lpContext;

   OS2DirectDrawEnumerateA( SimpleEnum,
                            &data);
  return (DD_OK);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DDHAL32_VidMemFree(DWORD address)
{
  dprintf(("DDRAW: DDHAL32_VidMemFree, not supported\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DDHAL32_VidMemAlloc(DWORD size)
{
  dprintf(("DDRAW: DDHAL32_VidMemAlloc, not supported\n"));
  return(0);
}
//******************************************************************************

/*******************************************************************************
 * DirectDraw ClassFactory
 *
 */

typedef struct
{
    /* IUnknown fields */
    ICOM_VTABLE(IClassFactory) *lpvtbl;
    DWORD                       ref;
} IClassFactoryImpl;

static HRESULT WINAPI
DDCF_QueryInterface(LPCLASSFACTORY iface,REFIID riid,LPVOID *ppobj)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  char buf[80];

  if (HIWORD(riid))
    WINE_StringFromCLSID(riid,buf);
  else
    sprintf(buf,"<guid-0x%04x>",LOWORD(riid));
  dprintf(("DDRAW:(%p)->(%s,%p),stub!\n",This,buf,ppobj));
  return E_NOINTERFACE;
}

static ULONG WINAPI
DDCF_AddRef(LPCLASSFACTORY iface)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  return ++(This->ref);
}

static ULONG WINAPI DDCF_Release(LPCLASSFACTORY iface)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  /* static class, won't be  freed */
  return --(This->ref);
}

static HRESULT WINAPI DDCF_CreateInstance( LPCLASSFACTORY iface,
                                           LPUNKNOWN pOuter,
                                           REFIID riid,
                                           LPVOID *ppobj)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  LPGUID lpGUID;
  lpGUID = (LPGUID) riid;

  dprintf(("DDRAW:DDCF_CreateInstance\n"));
  if( lpGUID &&
      ( (*lpGUID == IID_IDirectDraw ) ||
        (*lpGUID == IID_IDirectDraw2) ||
        (*lpGUID == IID_IDirectDraw4))
    )
  {
    /* FIXME: reuse already created DirectDraw if present? */
    return OS2DirectDrawCreate(lpGUID,(LPDIRECTDRAW*)ppobj,pOuter);
  }
  return E_NOINTERFACE;
}

static HRESULT WINAPI DDCF_LockServer(LPCLASSFACTORY iface,BOOL dolock)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  dprintf(("DDRAW:(%p)->(%d),stub!\n",This,dolock));
  return S_OK;
}

static ICOM_VTABLE(IClassFactory) DDCF_Vtbl =
{
  DDCF_QueryInterface,
  DDCF_AddRef,
  DDCF_Release,
  DDCF_CreateInstance,
  DDCF_LockServer
};

static IClassFactoryImpl DDRAW_CF = {&DDCF_Vtbl, 1 };


HRESULT WINAPI DllGetClassObject( REFCLSID rclsid,
                                  REFIID riid,
                                  LPVOID *ppv)
{
  char buf[80],xbuf[80];

  if (HIWORD(rclsid))
    WINE_StringFromCLSID(rclsid,xbuf);
  else
    sprintf(xbuf,"<guid-0x%04x>",LOWORD(rclsid));
  if (HIWORD(riid))
    WINE_StringFromCLSID(riid,buf);
  else
    sprintf(buf,"<guid-0x%04x>",LOWORD(riid));
  WINE_StringFromCLSID(riid,xbuf);

  dprintf(("DDRAW:(%p,%p,%p)\n", xbuf, buf, ppv));
  if (!memcmp(riid,&IID_IClassFactory,sizeof(IID_IClassFactory)))
  {
    *ppv = (LPVOID)&DDRAW_CF;
    DDRAW_CF.lpvtbl->AddRef((IClassFactory*)&DDRAW_CF);
    return S_OK;
  }
  dprintf(("DDRAW: (%p,%p,%p): no interface found.\n", xbuf, buf, ppv));
  return E_NOINTERFACE;
}


/*******************************************************************************
 * DllCanUnloadNow [DDRAW.12]  Determines whether the DLL is in use.
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: S_FALSE
 */
HRESULT WINAPI DllCanUnloadNow(void)
{
  dprintf(("DllCanUnloadNow(void) stub\n"));
  return S_FALSE;
}//******************************************************************************

