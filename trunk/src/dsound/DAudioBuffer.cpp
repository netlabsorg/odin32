/* $Id: DAudioBuffer.cpp,v 1.2 2001-04-30 21:06:37 sandervl Exp $ */

/*
 * DirectSound SoundBuffer (secondary) class (DirectAudio)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2001 Michal Necasek
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
#include "DAudioBuffer.h"
#include "OS2PrimBuff.h"
#include "OS23DListener.h"
#include "OS23DBuffer.h"
#include "DAudioNotify.h"
#include <misc.h>

extern BYTE VolTable[1000];

IDirectAudioBuffer *IDirectAudioBuffer::dsbroot = NULL;

//******************************************************************************
//******************************************************************************
IDirectAudioBuffer::IDirectAudioBuffer(OS2IDirectSound *DSound, const DSBUFFERDESC *lpDSBufferDesc)
        : Referenced(0), lastError(DS_OK), daudio(NULL), lpSndBuffer(NULL)
{
    initVtbl();

    dprintf(("DSOUND-IDirectAudioBuffer::IDirectAudioBuffer (buf=%X)", this));

    parentDS  = DSound;
    writepos  = 0;
    playpos   = 0;
    status    = 0;
    fLocked   = FALSE;
    volume    = 255;
    DSvolume  = 0;
    pan       = 0;
    DSpan     = 0;
    lpfxFormat= NULL;
    fPlaying  = FALSE;
    fLoop     = FALSE;
    notify    = NULL;
    memcpy(&bufferdesc, lpDSBufferDesc, sizeof(DSBUFFERDESC));

    // frequency has to be set according to the bufer format
    frequency = bufferdesc.lpwfxFormat->nSamplesPerSec;

    if (bufferdesc.lpwfxFormat == NULL || bufferdesc.dwBufferBytes == 0) {
        dprintf(("bufferdesc not valid!"));
        lastError = DSERR_INVALIDPARAM;
        return;
    }

    // Some apps pass trash in cbSize, can't rely on that!
    lpfxFormat = (WAVEFORMATEX *)malloc(/*bufferdesc.lpwfxFormat->cbSize +*/ sizeof(WAVEFORMATEX));
    memcpy(lpfxFormat, bufferdesc.lpwfxFormat,  /*bufferdesc.lpwfxFormat->cbSize + */sizeof(WAVEFORMATEX));

    dprintf((" Buffer format: %dHz, %dbit, %d channels", lpfxFormat->nSamplesPerSec, lpfxFormat->wBitsPerSample, lpfxFormat->nChannels));

    lpSndBuffer = (SOUNDBUF *)VirtualAlloc(0, bufferdesc.dwBufferBytes + sizeof(DWORD), MEM_COMMIT, PAGE_READWRITE);
    if (lpSndBuffer == NULL) {
        dprintf(("VirtualAlloc failed"));
        lpSndBuffer  = NULL;
        lastError = DSERR_OUTOFMEMORY;
        return;
    }
    lpBuffer = &(lpSndBuffer->pData[0]);
    lpSndBuffer->dwReferences = 1;

    daudio = new DAudioWaveOut(lpfxFormat);
    if(daudio == NULL || daudio->getError()) {
        dprintf(("DAudioWaveOut ctor failed"));
        lastError = DSERR_OUTOFMEMORY;
        return;
    }

    wmutex.enter(VMUTEX_WAIT_FOREVER);
    next = dsbroot;
    dsbroot = this;
    wmutex.leave();
}

//******************************************************************************
//******************************************************************************
IDirectAudioBuffer::IDirectAudioBuffer(OS2IDirectSound *DSound, IDirectAudioBuffer *srcBuf)
        : Referenced(0), lastError(DS_OK), daudio(NULL), lpSndBuffer(NULL)
{
    initVtbl();

    dprintf(("DSOUND-IDirectAudioBuffer::OS2IDirectAudioBuffer/Duplicate (buf=%X)", this));

    parentDS  = DSound;
    writepos  = 0;
    playpos   = 0;
    status    = 0;
    fLocked   = FALSE;
    volume    = srcBuf->volume;
    DSvolume  = srcBuf->DSvolume;
    pan       = srcBuf->pan;
    DSpan     = srcBuf->DSpan;
    lpBuffer  = srcBuf->lpBuffer;
    fPlaying  = FALSE;
    fLoop     = srcBuf->fLoop;
    notify    = NULL;
    memcpy(&bufferdesc, &(srcBuf->bufferdesc), sizeof(DSBUFFERDESC));

    // frequency has to be set according to the bufer format
    frequency = srcBuf->frequency;

    lpfxFormat = (WAVEFORMATEX *)malloc(sizeof(WAVEFORMATEX));
    memcpy(lpfxFormat,srcBuf->lpfxFormat, sizeof(WAVEFORMATEX));

    dprintf((" Buffer format: %dHz, %dbit, %d channels", lpfxFormat->nSamplesPerSec, lpfxFormat->wBitsPerSample, lpfxFormat->nChannels));

    // Increment reference count for the buffer memory
    lpSndBuffer  = srcBuf->lpSndBuffer;
    lpSndBuffer->dwReferences++;

    daudio = new DAudioWaveOut(lpfxFormat);
    if(daudio == NULL || daudio->getError()) {
        dprintf(("DAudioWaveOut ctor failed"));
        lastError = DSERR_OUTOFMEMORY;
        return;
    }

    wmutex.enter(VMUTEX_WAIT_FOREVER);
    next = dsbroot;
    dsbroot = this;
    wmutex.leave();
}
//******************************************************************************
//******************************************************************************
IDirectAudioBuffer::~IDirectAudioBuffer()
{
    dprintf(("DSOUND-IDirectAudioBuffer::~OS2IDirectAudioBuffer (buf=%X)", this));

    // free allocted memory - unless it's shared by a duplicated buffer
    if (lpSndBuffer) {
        lpSndBuffer->dwReferences--;
        if (lpSndBuffer->dwReferences == 0)
            VirtualFree(lpSndBuffer, 0, MEM_RELEASE);
    }

    if (lpfxFormat)
        free(lpfxFormat);

    if(daudio) {
        delete daudio;
        daudio = 0;
    }

    // remove ourselves from the linked list
    IDirectAudioBuffer *cur  = dsbroot;
    IDirectAudioBuffer *prev = NULL;

    wmutex.enter(VMUTEX_WAIT_FOREVER);
    if (this == dsbroot)  // is this the first DAudioBuffer?
        dsbroot = next;
    else {                // walk the chain
        while (cur->next != this)
            cur  = cur->next;

        cur->next = next;
    }
    wmutex.leave();
}
//******************************************************************************
//******************************************************************************
void IDirectAudioBuffer::initVtbl()
{
    lpVtbl = &Vtbl;
    Vtbl.AddRef               = DAudioBufAddRef;
    Vtbl.Release              = DAudioBufRelease;
    Vtbl.QueryInterface       = DAudioBufQueryInterface;
    Vtbl.GetCaps              = DAudioBufGetCaps;
    Vtbl.GetFormat            = DAudioBufGetFormat;
    Vtbl.GetVolume            = DAudioBufGetVolume;
    Vtbl.GetStatus            = DAudioBufGetStatus;
    Vtbl.GetCurrentPosition   = DAudioBufGetCurrentPosition;
    Vtbl.GetPan               = DAudioBufGetPan;
    Vtbl.GetFrequency         = DAudioBufGetFrequency;
    Vtbl.Initialize           = DAudioBufInitialize;
    Vtbl.Restore              = DAudioBufRestore;
    Vtbl.SetFormat            = DAudioBufSetFormat;
    Vtbl.SetVolume            = DAudioBufSetVolume;
    Vtbl.SetCurrentPosition   = DAudioBufSetCurrentPosition;
    Vtbl.SetPan               = DAudioBufSetPan;
    Vtbl.SetFrequency         = DAudioBufSetFrequency;
    Vtbl.Lock                 = DAudioBufLock;
    Vtbl.Unlock               = DAudioBufUnlock;
    Vtbl.Stop                 = DAudioBufStop;
    Vtbl.Play                 = DAudioBufPlay;
}
//******************************************************************************
//******************************************************************************
void IDirectAudioBuffer::DestroyAllBuffers()
{
   dprintf(("DSOUND-IDirectAudioBuffer::DestroyAllBuffers"));

   // if any DAudioBuffers still exist when the OS2IDirectSound object is
   // being closed, just kill them all
   IDirectAudioBuffer *cur  = dsbroot;
   IDirectAudioBuffer *tmp;

   while (cur != NULL) {
      tmp = cur->next;
      delete cur;
      cur = tmp;
   }
}

//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-IDirectAudioBuffer::QueryInterface %x %x", this, riid));
   *ppvObj = NULL;

   if (IsEqualGUID(riid, IID_IDirectSoundBuffer)) {
      *ppvObj = this;

      AddRef();
      return DS_OK;
   }

   if (IsEqualGUID(riid, IID_IDirectSoundNotify))
   {
      IDirectAudioNotify  *notify;

      notify     = new IDirectAudioNotify(this);
      *ppvObj    = notify;
      notify->Vtbl.AddRef(notify);
      return DS_OK;
   }
#if 0
   if (IsEqualGUID(riid, IID_IDirectSound3DBuffer)) {
      OS2IDirectSound3DBuffer *buffer3D;

      buffer3D   = new OS2IDirectSound3DBuffer(this);
      *ppvObj    = buffer3D;
      buffer3D->Vtbl.AddRef((IDirectSound3DBuffer *)buffer3D);
      return DS_OK;
   }
#endif
   return E_NOINTERFACE;
}
//******************************************************************************
//******************************************************************************
ULONG IDirectAudioBuffer::AddRef()
{
   dprintf(("DSOUND-IDirectAudioBuffer::AddRef (buf=%X) %d", this, Referenced+1));
   return ++Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG IDirectAudioBuffer::Release()
{
    dprintf(("DSOUND-IDirectAudioBuffer::Release (buf=%X) %d", this, Referenced-1));

    if (Referenced) {
        Referenced--;
        if (Referenced == 0) {
            delete this;
            return 0;
        }
        else
            return Referenced;
    }
    else
        return 0;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::GetCaps(LPDSBCAPS lpDSCaps)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufGetCaps (buf=%X)", this));
    if ( lpDSCaps == NULL) {
        return DSERR_INVALIDPARAM;
    }
    /* FIXME: fill with more realistic values */
    lpDSCaps->dwSize               = sizeof(DSBCAPS);
    lpDSCaps->dwFlags              = bufferdesc.dwFlags | DSBCAPS_LOCHARDWARE;
    lpDSCaps->dwBufferBytes        = bufferdesc.dwBufferBytes;
    lpDSCaps->dwUnlockTransferRate = 0;     /* in KB/sec - 0 == no transfer needed */
    lpDSCaps->dwPlayCpuOverhead    = 0;     /* % CPU time - let's lie */

    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::GetCurrentPosition(LPDWORD lpdwCurrentPlayCursor,
                                               LPDWORD lpdwCurrentWriteCursor)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufGetCurrentPosition (buf=%X)", this));
    if ( lpdwCurrentPlayCursor == NULL || lpdwCurrentWriteCursor == NULL) {
        dprintf(("  Invalid parameters %d %d %d",this,lpdwCurrentPlayCursor,lpdwCurrentWriteCursor));
        return DSERR_INVALIDPARAM;
    }

    *lpdwCurrentPlayCursor  = daudio->getPosition(lpdwCurrentWriteCursor);
    dprintf(("  PlayPos %d, WritePos %d", *lpdwCurrentPlayCursor, *lpdwCurrentWriteCursor));
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::SetCurrentPosition(DWORD dwNewPosition)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufSetCurrentPosition %x to %d", this, dwNewPosition));
    playpos   = dwNewPosition;

    //TODO:
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::GetFormat(LPWAVEFORMATEX lpwfxFormat,
                                      DWORD ddwSizeAllocated, LPDWORD lpdwSizeWritten)
{
    int copysize;

    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufGetFormat (buf=%X)", this));
    if (lpwfxFormat == NULL || ddwSizeAllocated == 0) {
        return DSERR_INVALIDPARAM;
    }
    copysize = min(ddwSizeAllocated, (lpfxFormat->cbSize + sizeof(WAVEFORMATEX)));
    memcpy(lpwfxFormat, lpfxFormat, copysize);

    if (lpdwSizeWritten) {
        *lpdwSizeWritten = copysize;
    }
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::SetFormat(LPWAVEFORMATEX lpWaveFormatEx)
{
    // Note: this sets the format of the _primary_ buffer;
    // since this class only handles secondary buffers, we just return error
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufSetFormat %x", this));

    return DSERR_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::GetVolume(LPLONG lplVolume)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufGetVolume (buf=%X)", this));
    if (lplVolume == NULL) {
        return DSERR_INVALIDPARAM;
    }
    *lplVolume = DSvolume;
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::SetVolume(LONG lVolume)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufSetVolume %x %d", this, lVolume));
    if (lVolume < -10000) {
        return DSERR_INVALIDPARAM;
    }
    // some apps pass positive values in lVolume, that's wrong right?!?
    DSvolume = (lVolume > 0) ? 0 : lVolume;

    /* = (10 ^ (1/10)) = 1dB - but the formula below gives results _very_ similar */
    /* to 'real' DirectSound, indistinguishable for all practical purposes        */
    //volume = 255.0 * pow(4, DSvolume / 1000.0);

    /* Note: for some strange reason the above code sometimes gives erroneous     */
    /* results, hence we now use a simple conversion table (in steps of 4/100 dB) */

    if (DSvolume < -4000)
        volume = 0;
    else
        volume = VolTable[-DSvolume / 4];

    dprintf(("  New volume: %d", volume));

    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::GetPan(LPLONG lplPan)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufGetPan (buf=%X)", this));
    if (lplPan == NULL) {
        return DSERR_INVALIDPARAM;
    }
    *lplPan = DSpan;
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::SetPan(LONG lPan)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufSetPan %x %d", this, lPan));
    if (lPan < -10000 || lPan > 10000) {
        return DSERR_INVALIDPARAM;
    }
    DSpan = lPan;
    if (lPan == 0) {
        pan = 0;
        return DS_OK;
    }

    /* Note: we use very similar code as for volume above   */
    if (lPan < 0)
        if (lPan < -4000)
            pan = -255;
        else
            pan = VolTable[-lPan / 4] - 255;
    else
        if (lPan > 4000)
            pan = 255;
        else
            pan = 255 - VolTable[lPan / 4];

    dprintf(("  New pan: %d", pan));

    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::GetFrequency(LPDWORD lpdwFrequency)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufGetFrequency (buf=%X)", this));
    if (lpdwFrequency == NULL) {
        return DSERR_INVALIDPARAM;
    }
    *lpdwFrequency = frequency;
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::SetFrequency(DWORD dwFrequency)
{
 DWORD oldfrequency = frequency;

    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufSetFrequency %x %d (old %d)", this, dwFrequency, oldfrequency));

    // zero means default (buffer format) frequency
    if (dwFrequency)
        frequency = dwFrequency;
    else
        frequency = lpfxFormat->nSamplesPerSec;

    if(frequency != oldfrequency) {
        daudio->setProperty(PROPERTY_FREQUENCY, frequency);
    }
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::GetStatus(LPDWORD lpdwStatus)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufGetStatus (buf=%X)", this));
    if (lpdwStatus == NULL) {
        return DSERR_INVALIDPARAM;
    }

    if (fPlaying)
        if (fLoop)
            status |= DSBSTATUS_PLAYING | DSBSTATUS_LOOPING;
        else
            status |= DSBSTATUS_PLAYING;
    else
        status  &= ~(DSBSTATUS_PLAYING | DSBSTATUS_LOOPING);

    *lpdwStatus = status;
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::Initialize(LPDIRECTSOUND, LPDSBUFFERDESC )
{
    dprintf(("DSOUND-DAudioBufInitialize (buf=%X)", this));

    return DSERR_ALREADYINITIALIZED;  //todo: for future extensions (dx5/6 ??)
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::Lock(DWORD dwWriteCursor, DWORD dwWriteBytes,
                                 LPVOID lplpvAudioPtr1, LPDWORD lpdwAudioBytes1,
                                 LPVOID lplpvAudioPtr2, LPDWORD lpdwAudioBytes2,
                                 DWORD dwFlags)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufLock (buf=%X, %d bytes of %d)", this, dwWriteBytes, bufferdesc.dwBufferBytes));
    if (!lplpvAudioPtr1 || !lpdwAudioBytes1)
        return DSERR_INVALIDPARAM;

    //not sure if this is an error, but it's certainly a smart thing to do (cond. == true)
    if(dwWriteBytes > bufferdesc.dwBufferBytes) {
        dprintf(("DAudioBufLock: dwWriteBytes > bufferdesc.dwBufferBytes"));
        return DSERR_INVALIDPARAM;
    }
    if (dwFlags & DSBLOCK_FROMWRITECURSOR) {
        dwWriteCursor = writepos;
    }
    if (dwWriteCursor + dwWriteBytes > bufferdesc.dwBufferBytes) {
        *(DWORD *)lplpvAudioPtr1  = (DWORD)(lpBuffer + dwWriteCursor);
        *lpdwAudioBytes1          = bufferdesc.dwBufferBytes - dwWriteCursor;
        if (lplpvAudioPtr2 && lpdwAudioBytes2) {
            *(DWORD *)lplpvAudioPtr2   = (DWORD)lpBuffer;
            *lpdwAudioBytes2           = dwWriteBytes - *lpdwAudioBytes1;
        }
    }
    else {
        *(DWORD *)lplpvAudioPtr1  = (DWORD)(lpBuffer + dwWriteCursor);
        *lpdwAudioBytes1          = dwWriteBytes;
        if (lplpvAudioPtr2 && lpdwAudioBytes2) {
            *(DWORD *)lplpvAudioPtr2   = 0;
            *lpdwAudioBytes2           = 0;
        }
    }
    fLocked = TRUE;
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::Unlock(LPVOID lpvAudioPtr1, DWORD dwAudioBytes1,
                                   LPVOID lpvAudioPtr2, DWORD dwAudioBytes2)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufUnlock %x", this));

    fLocked = TRUE;
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::Play(DWORD dwRes1, DWORD dwRes2, DWORD dwFlags)
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufPlay (buf=%X)", this));

    if (!fPlaying) {
        frac     = 0;
        fPlaying = TRUE;
        status   = DSBSTATUS_PLAYING;
        fLoop    = dwFlags == DSBPLAY_LOOPING;
        if (fLoop) {
            status |= DSBSTATUS_LOOPING;
            daudio->setProperty(PROPERTY_LOOPING, TRUE);
        }
        dprintf(("Buffer %X: start at pos %d, loop %s",this, playpos, fLoop?"YES":"NO"));
    }
    else {
        fLoop = dwFlags == DSBPLAY_LOOPING;
        if (fLoop)
            status |= DSBSTATUS_LOOPING;

        daudio->setProperty(PROPERTY_LOOPING, fLoop);
        dprintf(("  Buffer %X: already playing, loop %s",this, fLoop?"YES":"NO"));
    }

    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::Stop(  )
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufStop %x", this));
    fPlaying = FALSE;
    status  &= ~(DSBSTATUS_PLAYING | DSBSTATUS_LOOPING);

    daudio->stop();

    if (notify != NULL)
        notify->CheckStop();

    return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT IDirectAudioBuffer::Restore(  )
{
    dprintf(("DSOUND-IDirectAudioBuffer::DAudioBufRestore %x", this));
    return DS_OK;
}
//******************************************************************************
//******************************************************************************
//Wrapper functions. Can be removed when switching to a compiler that supports
//stdcall calling convention for class methods.
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->QueryInterface(riid, ppvObj);
}
//******************************************************************************
//******************************************************************************
ULONG   WINAPI DAudioBufAddRef(THIS)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->AddRef();
}
//******************************************************************************
//******************************************************************************
ULONG   WINAPI DAudioBufRelease(THIS)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->Release();
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufGetCaps(THIS_ LPDSBCAPS lpCaps)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->GetCaps(lpCaps);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufGetCurrentPosition(THIS_ LPDWORD a,LPDWORD b)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->GetCurrentPosition(a, b);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufGetFormat(THIS_ LPWAVEFORMATEX a, DWORD b, LPDWORD c)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->GetFormat(a,b,c);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufGetVolume(THIS_ LPLONG a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->GetVolume(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufGetPan(THIS_ LPLONG a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->GetPan(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufGetFrequency(THIS_ LPDWORD a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->GetFrequency(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufGetStatus(THIS_ LPDWORD a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->GetStatus(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufInitialize(THIS_ LPDIRECTSOUND a, LPDSBUFFERDESC b)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->Initialize(a,b);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufLock(THIS_ DWORD a,DWORD b,LPVOID c,LPDWORD d,LPVOID e,LPDWORD f,DWORD g)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->Lock(a,b,c,d,e,f,g);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufPlay(THIS_ DWORD a, DWORD b,DWORD c)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->Play(a,b,c);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufSetCurrentPosition(THIS_ DWORD a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->SetCurrentPosition(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufSetFormat(THIS_ LPWAVEFORMATEX a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->SetFormat(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufSetVolume(THIS_ LONG a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->SetVolume(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufSetPan(THIS_ LONG a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->SetPan(a);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufSetFrequency(THIS_ DWORD a)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->SetFrequency(a);

}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufStop(THIS  )
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->Stop();
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufUnlock(THIS_ LPVOID a,DWORD b,LPVOID c,DWORD d)
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->Unlock(a,b,c,d);
}
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufRestore(THIS  )
{
    IDirectAudioBuffer *me = (IDirectAudioBuffer *)This;
    if(This == NULL) {
        DebugInt3();
        return DSERR_INVALIDPARAM;
    }
    return me->Restore();
}
//******************************************************************************
//******************************************************************************

