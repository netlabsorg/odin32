/*
 * DirectSound main class
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
#define INCL_DIRECTSOUND_GUID
#define WIN32SDK_NOPOSTWRAPPER


/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <stdlib.h>
#include <string.h>

#include "no.h" /* NO* defines for windows.h */
#include <w_windows.h>
#include <dsound.h>
#include <Win32SDKPostWrapper.h>

#include "os2dsound.h"
#include "os2sndbuffer.h"
#include "misc.h"


/* KSO Apr 19 1999: INTERFACE set again, since os2sndbuffer changed it.  *
 * (INTERFACE is used in the THIS and THIS_ macros)                      */
#undef  INTERFACE
#define INTERFACE   IDirectSound


//******************************************************************************
//******************************************************************************
OS2IDirectSound::OS2IDirectSound(const GUID *lpGUID) : Referenced(0),
        lastError(DS_OK), hwndClient(0)
{
  lpVtbl = &Vtbl;
  Vtbl.AddRef               = SoundAddRef;
  Vtbl.Release              = SoundRelease;
  Vtbl.QueryInterface       = SoundQueryInterface;
  Vtbl.Compact              = SoundCompact;
  Vtbl.CreateSoundBuffer    = SoundCreateSoundBuffer;
  Vtbl.GetCaps              = SoundGetCaps;
  Vtbl.DuplicateSoundBuffer = SoundDuplicateSoundBuffer;
  Vtbl.SetCooperativeLevel  = SoundSetCooperativeLevel;
  Vtbl.Compact              = SoundCompact;
  Vtbl.GetSpeakerConfig     = SoundGetSpeakerConfig;
  Vtbl.SetSpeakerConfig     = SoundSetSpeakerConfig;
  Vtbl.Initialize           = SoundInitialize;

  speakerConfig = DSSPEAKER_STEREO;
  CoopLevel     = DSSCL_EXCLUSIVE;  //default
}
//******************************************************************************
//******************************************************************************
OS2IDirectSound::~OS2IDirectSound()
{

}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("OS2IDirectSound::QueryInterface\n"));
  if(This == NULL) {
    return DSERR_INVALIDPARAM;
  }
  *ppvObj = NULL;

  if(!IsEqualGUID(riid, CLSID_DirectSound) &&
     !IsEqualGUID(riid, IID_IDirectSound))
    return E_NOINTERFACE;

  *ppvObj = This;

  SoundAddRef(This);
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundAddRef(THIS)
{
 OS2IDirectSound *me = (OS2IDirectSound *)This;

  dprintf(("OS2IDirectSound::AddRef %d\n", me->Referenced+1));
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundRelease(THIS)
{
 OS2IDirectSound *me = (OS2IDirectSound *)This;

  dprintf(("OS2IDirectSound::Release %d\n", me->Referenced-1));
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
HRESULT WIN32API SoundCompact(THIS)
{
  dprintf(("OS2IDirectSound::Compact\n"));
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
#if 0 //KSO Apr 13 1999: some parameter incompabilities between newer and older SDKs.
HRESULT WIN32API SoundCreateSoundBuffer(THIS_
                    LPDSBUFFERDESC lpDSBufferDesc, //old, non const
                    LPLPDIRECTSOUNDBUFFER lplpDirectSoundBuffer,
                    IUnknown FAR *pUnkOuter)
#else
HRESULT WIN32API SoundCreateSoundBuffer(THIS_
                    LPCDSBUFFERDESC lpDSBufferDesc, //new, const
                    LPDIRECTSOUNDBUFFER *lplpDirectSoundBuffer,
                    LPUNKNOWN pUnkOuter)
#endif
{
 OS2IDirectSound *me = (OS2IDirectSound *)This;
 OS2IDirectSoundBuffer *sndbuf;

  dprintf(("OS2IDirectSound::CreateSoundBuffer\n"));
  if(me == NULL || lpDSBufferDesc == NULL || lplpDirectSoundBuffer == NULL) {
    return(DSERR_INVALIDPARAM);
  }
  sndbuf = new OS2IDirectSoundBuffer(lpDSBufferDesc);
  if(sndbuf == NULL) {
    return(DSERR_OUTOFMEMORY);
  }
  if(sndbuf->GetLastError() != DS_OK) {
    ULONG lastErr = sndbuf->GetLastError();
    delete sndbuf;
    return lastErr;
  }
  *lplpDirectSoundBuffer = (LPDIRECTSOUNDBUFFER)sndbuf;
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundGetCaps(THIS_ LPDSCAPS lpCaps)
{
  dprintf(("OS2IDirectSound::GetCaps\n"));
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundDuplicateSoundBuffer(THIS_ LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER )
{
  dprintf(("OS2IDirectSound::DuplicateSoundBuffer\n"));
  return(DSERR_OUTOFMEMORY);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundSetCooperativeLevel(THIS_ W32_HWND hwndClient, DWORD level)
{
 OS2IDirectSound *me = (OS2IDirectSound *)This;

  dprintf(("OS2IDirectSound::SetCooperativeLevel\n"));
  if(me == NULL) {
    return(DSERR_INVALIDPARAM);
  }
  me->hwndClient = (HWND)hwndClient;
  me->CoopLevel  = level;
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundGetSpeakerConfig(THIS_ LPDWORD lpSpeakerCfg)
{
 OS2IDirectSound *me = (OS2IDirectSound *)This;

  dprintf(("OS2IDirectSound::GetSpeakerConfig\n"));
  if(me == NULL) {
    return(DSERR_INVALIDPARAM);
  }
  *lpSpeakerCfg = me->speakerConfig;

  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundSetSpeakerConfig(THIS_ DWORD speakerCfg)
{
 OS2IDirectSound *me = (OS2IDirectSound *)This;

  dprintf(("OS2IDirectSound::SetSpeakerConfig %X\n", speakerCfg));
  if(me == NULL) {
    return(DSERR_INVALIDPARAM);
  }
  me->speakerConfig = speakerCfg;
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
#if 0 //KSO Apr 13 1999: parameter declaration has changed in later SDKs
    HRESULT WIN32API SoundInitialize(THIS_ const GUID * )
#else
    HRESULT WIN32API SoundInitialize(THIS_ LPGUID)
#endif
{
  dprintf(("OS2IDirectSound::Initialize UNSUPPORTED\n"));
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
