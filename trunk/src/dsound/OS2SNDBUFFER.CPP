/* $Id: OS2SNDBUFFER.CPP,v 1.3 1999-06-10 17:09:30 phaller Exp $ */

/*
 * DirectSound SoundBuffer class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_DOSMEMMGR
#define INCL_DIRECTSOUNDBUFFER_GUID
#define WIN32SDK_NOPOSTWRAPPER

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <stdlib.h>
#include <string.h>

#include "no.h"     /* NO* defines for windows.h */
#include <w_windows.h>
#include <dsound.h>
#include <Win32SDKPostWrapper.h>

#include "os2dsound.h"
#include "os2sndbuffer.h"
#include "misc.h"


/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef INTERFACE
#define INTERFACE IDirectSoundBuffer



//******************************************************************************
//******************************************************************************
OS2IDirectSoundBuffer::OS2IDirectSoundBuffer(const DSBUFFERDESC *lpDSBufferDesc) //KSO Apr 13 1999: const, SDK changes
        : Referenced(0), lastError(DS_OK)
{
 APIRET rc;

  lpVtbl = &Vtbl;
  Vtbl.AddRef               = SoundBufAddRef;
  Vtbl.Release              = SoundBufRelease;
  Vtbl.QueryInterface       = SoundBufQueryInterface;
  Vtbl.GetCaps              = SoundBufGetCaps;
  Vtbl.GetFormat            = SoundBufGetFormat;
  Vtbl.GetVolume            = SoundBufGetVolume;
  Vtbl.GetStatus            = SoundBufGetStatus;
  Vtbl.GetCurrentPosition   = SoundBufGetCurrentPosition;
  Vtbl.GetPan               = SoundBufGetPan;
  Vtbl.Initialize           = SoundBufInitialize;
  Vtbl.Restore              = SoundBufRestore;
  Vtbl.SetFormat            = SoundBufSetFormat;
  Vtbl.SetVolume            = SoundBufSetVolume;
  Vtbl.SetCurrentPosition   = SoundBufSetCurrentPosition;
  Vtbl.SetPan               = SoundBufSetPan;
  Vtbl.Lock                 = SoundBufLock;
  Vtbl.Unlock               = SoundBufUnlock;
  Vtbl.Stop                 = SoundBufStop;
  Vtbl.Play                 = SoundBufPlay;

  pan       = 0;
  writepos  = 0;
  playpos   = 0;
  frequency = 22050;
  status    = 0;;
  fLocked   = FALSE;
  volume    = 0;
  lpfxFormat= NULL;
  lpBuffer  = NULL;

  memcpy(&bufferdesc, lpDSBufferDesc, sizeof(DSBUFFERDESC));
  if(!(bufferdesc.dwFlags & DSBCAPS_PRIMARYBUFFER)) {
    if(bufferdesc.lpwfxFormat == NULL || bufferdesc.dwBufferBytes == 0) {
        dprintf(("bufferdesc not valid!"));
        lastError = DSERR_INVALIDPARAM;
        return;
    }
    lpfxFormat = (WAVEFORMATEX *)malloc(bufferdesc.lpwfxFormat->cbSize + sizeof(WAVEFORMATEX));
    memcpy(lpfxFormat, bufferdesc.lpwfxFormat, bufferdesc.lpwfxFormat->cbSize + sizeof(WAVEFORMATEX));

    rc = DosAllocMem((PPVOID)&lpBuffer, bufferdesc.dwBufferBytes, PAG_READ|PAG_WRITE|PAG_COMMIT);
    if(rc) {
        dprintf(("DosAllocMem returned %d", rc));
        lpBuffer  = NULL;
        lastError = DSERR_OUTOFMEMORY;
        return;
    }
  }
  else {//todo: primary buffer
  }
}
//******************************************************************************
//******************************************************************************
OS2IDirectSoundBuffer::~OS2IDirectSoundBuffer()
{
  if(lpBuffer)
    DosFreeMem(lpBuffer);
  if(lpfxFormat)
    free(lpfxFormat);

}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundBufQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("OS2IDirectSoundBuffer::QueryInterface\n"));
  if(This == NULL) {
    return DSERR_INVALIDPARAM;
  }
  *ppvObj = NULL;

  if(!IsEqualGUID(riid, IID_IDirectSoundBuffer))
    return E_NOINTERFACE;

  *ppvObj = This;

  SoundBufAddRef(This);
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundBufAddRef(THIS)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::AddRef %d\n", me->Referenced+1));
  if(me == NULL) {
    return 0;
  }
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundBufRelease(THIS)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf((" OS2IDirectSoundBuffer::Release %d\n", me->Referenced-1));
  if(me == NULL) {
    return 0;
  }
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
HRESULT __stdcall SoundBufGetCaps(THIS_ LPDSBCAPS lpDSCaps)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufGetCaps"));
  if(me == NULL || lpDSCaps == NULL) {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetCurrentPosition(THIS_ LPDWORD lpdwCurrentPlayCursor,
                         LPDWORD lpdwCurrentWriteCursor)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufGetCurrentPosition"));
  if(me == NULL || lpdwCurrentPlayCursor == NULL || lpdwCurrentWriteCursor == NULL) {
    return DSERR_INVALIDPARAM;
  }
  *lpdwCurrentPlayCursor  = me->playpos;
  *lpdwCurrentWriteCursor = me->writepos;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetFormat(THIS_ LPWAVEFORMATEX lpwfxFormat,
                    DWORD ddwSizeAllocated, LPDWORD lpdwSizeWritten)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;
 int copysize;

  dprintf(("SoundBufGetFormat"));
  if(me == NULL || lpwfxFormat == NULL || ddwSizeAllocated == 0) {
    return DSERR_INVALIDPARAM;
  }
  copysize = min(ddwSizeAllocated, (me->lpfxFormat->cbSize + sizeof(WAVEFORMATEX)));
  memcpy(lpwfxFormat, me->lpfxFormat, copysize);

  if(lpdwSizeWritten) {
    *lpdwSizeWritten = copysize;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetVolume(THIS_ LPLONG lplVolume)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufGetVolume"));
  if(me == NULL || lplVolume == NULL) {
    return DSERR_INVALIDPARAM;
  }
  *lplVolume = me->volume;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetPan(THIS_ LPLONG lplPan)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufGetPan"));
  if(me == NULL || lplPan == NULL) {
    return DSERR_INVALIDPARAM;
  }
  *lplPan = me->pan;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetFrequency(THIS_ LPDWORD lpdwFrequency)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufGetFrequency"));
  if(me == NULL || lpdwFrequency == NULL) {
    return DSERR_INVALIDPARAM;
  }
  *lpdwFrequency = me->frequency;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetStatus(THIS_ LPDWORD lpdwStatus)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufGetStatus"));
  if(me == NULL || lpdwStatus == NULL) {
    return DSERR_INVALIDPARAM;
  }
  *lpdwStatus = me->status;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
#if 0 //KSO Apr 19 1999: some parameter incompabilities between newer and older SDKs.
HRESULT __stdcall SoundBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC )
#else
HRESULT __stdcall SoundBufInitialize(THIS_ LPDIRECTSOUND, LPCDSBUFFERDESC )
#endif
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufInitialize"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  return DSERR_ALREADYINITIALIZED;  //todo: for future extensions (dx5/6 ??)
}
//******************************************************************************
//******************************************************************************
#if 0 //KSO Apr 19 1999: some parameter incompabilities between newer and older SDKs.
HRESULT __stdcall SoundBufLock(THIS_ DWORD dwWriteCursor, DWORD dwWriteBytes,
                   LPVOID lplpvAudioPtr1, LPDWORD lpdwAudioBytes1,
                   LPVOID lplpvAudioPtr2, LPDWORD lpdwAudioBytes2,
                   DWORD dwFlags)
#else
HRESULT __stdcall SoundBufLock(THIS_ DWORD dwWriteCursor, DWORD dwWriteBytes,
                   LPVOID *lplpvAudioPtr1, LPDWORD lpdwAudioBytes1,
                   LPVOID *lplpvAudioPtr2, LPDWORD lpdwAudioBytes2,
                   DWORD dwFlags)
#endif
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufLock"));
  if(me == NULL || !lplpvAudioPtr1 || !lpdwAudioBytes1) {
    return(DSERR_INVALIDPARAM);
  }
  //not sure if this is an error, but it's certainly a smart thing to do (cond. == true)
  if(dwWriteBytes > me->bufferdesc.dwBufferBytes) {
    dprintf(("SoundBufLock: dwWriteBytes > me->bufferdesc.dwBufferBytes"));
    return(DSERR_INVALIDPARAM);
  }
  if(dwFlags & DSBLOCK_FROMWRITECURSOR) {
    dwWriteCursor = me->writepos;
  }
  if(dwWriteCursor + dwWriteBytes > me->bufferdesc.dwBufferBytes ) {
    *(DWORD *)lplpvAudioPtr1  = (DWORD)(me->lpBuffer + dwWriteCursor);
    *lpdwAudioBytes1          = me->bufferdesc.dwBufferBytes - dwWriteCursor;
    if(lplpvAudioPtr2 && lpdwAudioBytes2) {
        *(DWORD *)lplpvAudioPtr2   = (DWORD)me->lpBuffer;
        *lpdwAudioBytes2           = dwWriteBytes - *lpdwAudioBytes1;
    }
  }
  else {
    *(DWORD *)lplpvAudioPtr1  = (DWORD)(me->lpBuffer + dwWriteCursor);
    *lpdwAudioBytes1          = dwWriteBytes;
    if(lplpvAudioPtr2 && lpdwAudioBytes2) {
        *(DWORD *)lplpvAudioPtr2   = 0;
        *lpdwAudioBytes2           = 0;
    }
  }
  me->fLocked = TRUE;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufPlay(THIS_ DWORD,DWORD,DWORD )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufPlay"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetCurrentPosition(THIS_ DWORD dwNewPosition)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufSetCurrentPosition"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  //todo: check for primary buffer (can't do it with those)
  me->playpos   = dwNewPosition;
  me->writepos  = dwNewPosition;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
#if 0 //KSO Apr 19 1999: some parameter incompabilities between newer and older SDKs.
HRESULT __stdcall SoundBufSetFormat(THIS_ LPWAVEFORMATEX )
#else
HRESULT __stdcall SoundBufSetFormat(THIS_ LPCWAVEFORMATEX )
#endif
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufSetFormat"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetVolume(THIS_ LONG lVolume)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufSetVolume"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  me->volume = lVolume;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetPan(THIS_ LONG lPan)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufSetPan"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  me->pan = lPan;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetFrequency(THIS_ DWORD dwFrequency)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufSetFrequency"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  me->frequency = dwFrequency;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufStop(THIS  )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufStop"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufUnlock"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufRestore(THIS  )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufRestore"));
  if(me == NULL) {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
