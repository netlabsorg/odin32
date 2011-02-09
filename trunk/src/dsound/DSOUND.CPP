/* $Id: DSOUND.CPP,v 1.9 2001-11-22 16:08:12 sandervl Exp $ */

/*
 * DirectSound exported APIs
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 2000 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITGUID
#define CINTERFACE
#include <dsound.h>

#include "os2dsound.h"

#include <misc.h>

OS2IDirectSound *DS = NULL;

extern "C" {

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundCreate(const GUID *lpGUID,
                                      LPDIRECTSOUND * ppDS,
                                      IUnknown *pUnkOuter)
{
   OS2IDirectSound *newsound;
   HRESULT         rc;

   dprintf(("DSOUND:DirectSoundCreate %X %X %X\n", lpGUID, ppDS, pUnkOuter));

   if (OS2IDirectSound::fDSExists) {
      *ppDS = (LPDIRECTSOUND)DS;
      DS->Vtbl.fnAddRef(DS);
      return DS_OK;
   }

   newsound = new OS2IDirectSound(lpGUID);

   if (newsound == NULL)
      return DSERR_OUTOFMEMORY;

   rc = newsound->GetLastError();
   if(rc != DS_OK) {
     *ppDS = NULL;
     delete newsound;
   }
   else {
     *ppDS = (LPDIRECTSOUND)newsound;
     newsound->Vtbl.fnAddRef(newsound);
     DS = newsound;
   }
   return rc;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundCaptureCreate(const GUID *lpGUID,
                                             LPDIRECTSOUNDCAPTURE * ppDS,
                                             IUnknown *pUnkOuter)
{
   return DSERR_OUTOFMEMORY;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA lpCallback,
                                                 LPVOID lpContext)
{
   dprintf(("DSOUND:DirectSoundCaptureEnumerateA\n"));

   lpCallback(NULL, "DART DirectSoundCapture for OS/2",
              "DirectSoundCapture/2 v0.1", lpContext);
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW lpCallback,
                                                 LPVOID lpContext)
{
   dprintf(("DSOUND:DirectSoundCaptureEnumerateW\n"));

   lpCallback(NULL, (LPWSTR)L"DART DirectSoundCapture for OS/2",
              (LPWSTR)L"DirectSoundCapture/2 v0.1", lpContext);
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundEnumerateA(LPDSENUMCALLBACKA lpCallback,
                                          LPVOID lpContext)
{
   dprintf(("DSOUND:DirectSoundEnumerateA\n"));

   // According to MSDN, the default sound device comes twice: first with a
   // null GUID and second wth its real GUID

   lpCallback(NULL,
              "DART DirectSound for OS/2", "DirectSound/2 v0.5",
              lpContext);

   lpCallback((LPGUID)&GUID_DirectSound_DART,
              "DART DirectSound for OS/2", "DirectSound/2 v0.5",
              lpContext);

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundEnumerateW(LPDSENUMCALLBACKW lpCallback,
                                          LPVOID lpContext)
{
   dprintf(("DSOUND:DirectSoundEnumerateW\n"));

   // According to MSDN, the default sound device comes twice: first with a
   // null GUID and second wth its real GUID

   lpCallback(NULL,
              (LPWSTR)L"DART DirectSound for OS/2", (LPWSTR)L"DirectSound/2 v0.5",
              lpContext);

   lpCallback((LPGUID)&GUID_DirectSound_DART,
              (LPWSTR)L"DART DirectSound for OS/2", (LPWSTR)L"DirectSound/2 v0.5",
              lpContext);

   return DS_OK;
}

//******************************************************************************
//******************************************************************************


/*******************************************************************************
 * DirectSound ClassFactory
 *
 */

typedef struct
{
    /* IUnknown fields */
    ICOM_VTABLE(IClassFactory) *lpvtbl;
    DWORD                       ref;
} IClassFactoryImpl;

static HRESULT WINAPI
DSCF_QueryInterface(LPCLASSFACTORY iface,REFIID riid,LPVOID *ppobj)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  char buf[80];

  if (HIWORD(riid))
    WINE_StringFromCLSID(riid,buf);
  else
    sprintf(buf,"<guid-0x%04x>",LOWORD(riid));
  dprintf(("DSOUND:DSCF_QueryInterface(%p)->(%s,%p),stub!\n",This,buf,ppobj));
  return E_NOINTERFACE;
}

static ULONG WINAPI
DSCF_AddRef(LPCLASSFACTORY iface)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  return ++(This->ref);
}

static ULONG WINAPI DSCF_Release(LPCLASSFACTORY iface)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  /* static class, won't be  freed */
  return --(This->ref);
}

static HRESULT WINAPI DSCF_CreateInstance( LPCLASSFACTORY iface,
                                           LPUNKNOWN pOuter,
                                           REFIID riid,
                                           LPVOID *ppobj)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  LPGUID lpGUID;
  lpGUID = (LPGUID) riid;

  dprintf(("DSOUND:DSCF_CreateInstance\n"));
  if(lpGUID && IsEqualGUID(lpGUID, &IID_IDirectSound)) {
    return OS2DirectSoundCreate(lpGUID,(LPDIRECTSOUND*)ppobj,pOuter);
  }
  return E_NOINTERFACE;
}

static HRESULT WINAPI DSCF_LockServer(LPCLASSFACTORY iface,BOOL dolock)
{
  ICOM_THIS(IClassFactoryImpl,iface);
  dprintf(("DSOUND:DSCF_LockServer(%p)->(%d),stub!\n",This,dolock));
  return S_OK;
}

static ICOM_VTABLE(IClassFactory) DSCF_Vtbl =
{
  DSCF_QueryInterface,
  DSCF_AddRef,
  DSCF_Release,
  DSCF_CreateInstance,
  DSCF_LockServer
};

static IClassFactoryImpl DSOUND_CF = {&DSCF_Vtbl, 1 };


HRESULT WINAPI DSOUNDDllGetClassObject( REFCLSID rclsid,
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

  dprintf(("DSOUND:(%p,%p,%p)\n", xbuf, buf, ppv));
  if (!memcmp(riid,&IID_IClassFactory,sizeof(IID_IClassFactory)))
  {
    *ppv = (LPVOID)&DSOUND_CF;
    DSOUND_CF.lpvtbl->fnAddRef((IClassFactory*)&DSOUND_CF);
    return S_OK;
  }
  dprintf(("DSOUND: (%p,%p,%p): no interface found.\n", xbuf, buf, ppv));
  return E_NOINTERFACE;
}


/*******************************************************************************
 * DllCanUnloadNow   Determines whether the DLL is in use.
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: S_FALSE
 */
HRESULT WINAPI DSOUNDDllCanUnloadNow(void)
{
  dprintf(("DSOUND:DllCanUnloadNow(void) stub\n"));
  return S_FALSE;
}//******************************************************************************
}
