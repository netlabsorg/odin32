/* $Id: OS2DSOUND.CPP,v 1.2 2000-03-08 18:26:47 mike Exp $ */

/*
 * DirectSound main class
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
#include <string.h>

#define INITGUID
#include <dsound.h>

#include "OS2DSound.h"
#include "OS2SndBuffer.h"
#include "OS2PrimBuff.h"
#include "OS23DListener.h"
#include "OS2Notify.h"
#include <misc.h>

// TODO: primary buffer should probably created right away when creating the DSound
// object
// TODO: handle cooperative levels properly!!

// this flag is set if the OS2IDirectObject exists
BOOL OS2IDirectSound::fDSExists = FALSE;

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

   dprintf(("DSOUND-OS2IDirectSound::OS2IDirectSound\n"));

   speakerConfig = DSSPEAKER_STEREO;
   CoopLevel     = DSSCL_EXCLUSIVE;  //default

   OS2IDirectSound::fDSExists = TRUE;

   dprintf(("Sound init OK\n"));
}
//******************************************************************************
//******************************************************************************
OS2IDirectSound::~OS2IDirectSound()
{
   // TODO: Close all SoundBuffers here
   OS2IDirectSoundBuffer::DestroyAllBuffers();
   OS2IDirectSound::fDSExists = FALSE;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-OS2IDirectSound::QueryInterface\n"));
   if (This == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *ppvObj = NULL;

   if (!IsEqualGUID(riid, CLSID_DirectSound) &&
       !IsEqualGUID(riid, IID_IDirectSound))
      return E_NOINTERFACE;

   *ppvObj = This;

   SoundAddRef(This);
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundAddRef(THIS)
{
   OS2IDirectSound *me = (OS2IDirectSound *)This;

   dprintf(("DSOUND-OS2IDirectSound::AddRef %d\n", me->Referenced+1));
   return ++me->Referenced;
}

//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundRelease(THIS)
{
   OS2IDirectSound *me = (OS2IDirectSound *)This;

   dprintf(("DSOUND-OS2IDirectSound::Release %d\n", me->Referenced-1));
   if (me->Referenced) {
      me->Referenced--;
      if (me->Referenced == 0) {
         delete me;
         return 0;
      }
      else
         return me->Referenced;
   }
   else
      return 0;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundCompact(THIS)
{
  dprintf(("DSOUND-OS2IDirectSound::Compact\n"));
  return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundCreateSoundBuffer(THIS_
                    LPDSBUFFERDESC lpDSBufferDesc, //new, const
                    LPDIRECTSOUNDBUFFER *lplpDirectSoundBuffer,
                    LPUNKNOWN pUnkOuter)
{
   OS2IDirectSound *me = (OS2IDirectSound *)This;
   OS2IDirectSoundBuffer *sndbuf;

   dprintf(("DSOUND-OS2IDirectSound::CreateSoundBuffer\n"));
   if (me == NULL || lpDSBufferDesc == NULL || lplpDirectSoundBuffer == NULL) {
      return DSERR_INVALIDPARAM;
   }

   if ((lpDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER) && (lpDSBufferDesc->lpwfxFormat != NULL)) {
      // Primary buffers must be created without format info!
      return DSERR_INVALIDPARAM;
   }

   if (lpDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER) {
      OS2PrimBuff *primbuff;
      primbuff = new OS2PrimBuff(me, lpDSBufferDesc);

      if (primbuff == NULL)
         return DSERR_OUTOFMEMORY;

      if ( primbuff->GetLastError() != DS_OK ){
         ULONG lastErr = primbuff->GetLastError();

         dprintf(("LastErr = %d\n", lastErr));
         delete primbuff;
         return lastErr;
      }
      *lplpDirectSoundBuffer = (LPDIRECTSOUNDBUFFER)primbuff;
      primbuff->Vtbl.AddRef(primbuff);
      dprintf(("Created PrimBuff... Exiting Create Buffer function\n"));

      return DS_OK;
   }

   sndbuf = new OS2IDirectSoundBuffer(me, lpDSBufferDesc);
   if (sndbuf == NULL) {
      return DSERR_OUTOFMEMORY;
   }
   if(sndbuf->GetLastError() != DS_OK) {
      ULONG lastErr = sndbuf->GetLastError();
      delete sndbuf;
      return lastErr;
   }
   sndbuf->Vtbl.AddRef(sndbuf);

   *lplpDirectSoundBuffer = (LPDIRECTSOUNDBUFFER)sndbuf;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundGetCaps(THIS_ LPDSCAPS lpCaps)
{
   dprintf(("DSOUND-OS2IDirectSound::GetCaps\n"));

   if (lpCaps == NULL)
      return DSERR_INVALIDPARAM;

   lpCaps->dwSize                           = sizeof(DSCAPS);
   lpCaps->dwFlags                          = DSCAPS_SECONDARY8BIT | DSCAPS_SECONDARY16BIT  |
                                              DSCAPS_SECONDARYMONO | DSCAPS_SECONDARYSTEREO |
                                              DSCAPS_PRIMARY8BIT   | DSCAPS_PRIMARY16BIT    |
                                             /* DSCAPS_PRIMARYMONO   |*/ DSCAPS_PRIMARYSTEREO;
   lpCaps->dwMinSecondarySampleRate         = 100;
   lpCaps->dwMaxSecondarySampleRate         = 100000;
   lpCaps->dwPrimaryBuffers                 = 1;
   lpCaps->dwMaxHwMixingAllBuffers          = 0;  // no HW support, no HW buffers...
   lpCaps->dwMaxHwMixingStaticBuffers       = 0;
   lpCaps->dwMaxHwMixingStreamingBuffers    = 0;
   lpCaps->dwFreeHwMixingAllBuffers         = 0;
   lpCaps->dwFreeHwMixingStaticBuffers      = 0;
   lpCaps->dwFreeHwMixingStreamingBuffers   = 0;
   lpCaps->dwMaxHw3DAllBuffers              = 0;
   lpCaps->dwMaxHw3DStaticBuffers           = 0;
   lpCaps->dwMaxHw3DStreamingBuffers        = 0;
   lpCaps->dwFreeHw3DAllBuffers             = 0;
   lpCaps->dwFreeHw3DStaticBuffers          = 0;
   lpCaps->dwFreeHw3DStreamingBuffers       = 0;
   lpCaps->dwTotalHwMemBytes                = 0;
   lpCaps->dwFreeHwMemBytes                 = 0;
   lpCaps->dwMaxContigFreeHwMemBytes        = 0;
   lpCaps->dwUnlockTransferRateHwBuffers    = 0;  // no transfer needed
   lpCaps->dwPlayCpuOverheadSwBuffers       = 0;  // we lie here...
   lpCaps->dwReserved1                      = 0;
   lpCaps->dwReserved2                      = 0;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundDuplicateSoundBuffer(THIS_ LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER )
{
   dprintf(("DSOUND-OS2IDirectSound::DuplicateSoundBuffer\n"));
   return DSERR_OUTOFMEMORY;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundSetCooperativeLevel(THIS_ HWND hwndClient, DWORD level)
{
   OS2IDirectSound *me = (OS2IDirectSound *)This;

   dprintf(("DSOUND-OS2IDirectSound::SetCooperativeLevel (to %d)\n", level));
   if (me == NULL) {
     return DSERR_INVALIDPARAM;
   }
   me->hwndClient = (HWND)hwndClient;
   me->CoopLevel  = level;
   return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundGetSpeakerConfig(THIS_ LPDWORD lpSpeakerCfg)
{
   OS2IDirectSound *me = (OS2IDirectSound *)This;

   dprintf(("DSOUND-OS2IDirectSound::GetSpeakerConfig\n"));
   if (me == NULL) {
      return(DSERR_INVALIDPARAM);
   }
   *lpSpeakerCfg = me->speakerConfig;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundSetSpeakerConfig(THIS_ DWORD speakerCfg)
{
   OS2IDirectSound *me = (OS2IDirectSound *)This;

   dprintf(("DSOUND-OS2IDirectSound::SetSpeakerConfig %X\n", speakerCfg));
   if (me == NULL) {
     return DSERR_INVALIDPARAM;
   }
   me->speakerConfig = speakerCfg;
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundInitialize(THIS_ LPGUID)
{
   dprintf(("DSOUND-OS2IDirectSound::Initialize NOP\n"));
   return DS_OK;
}
//******************************************************************************
//******************************************************************************
