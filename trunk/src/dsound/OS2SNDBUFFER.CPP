/* $Id: OS2SNDBUFFER.CPP,v 1.11 2001-03-19 18:56:57 mike Exp $ */

/*
 * DirectSound SoundBuffer class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2001 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSMISC
#include <os2win.h>

#include <stdlib.h>
#include <string.h>

#define INITGUID
#include <dsound.h>

#include "OS2DSound.h"
#include "OS2SndBuffer.h"
#include "OS2PrimBuff.h"
#include "OS23DListener.h"
#include "OS23DBuffer.h"
#include "OS2Notify.h"
#include <misc.h>

#include "DSVolume.h"

WAVEFORMATEX wfxDefaultPrimaryBuf = { WAVE_FORMAT_PCM,
                                      2,
                                      22050,
                                      44100,
                                      2,
                                      8,
                                      0};

OS2IDirectSoundBuffer *OS2IDirectSoundBuffer::dsbroot = NULL;
OS2IDirectSoundBuffer *OS2IDirectSoundBuffer::primary = NULL;

//******************************************************************************
//******************************************************************************
OS2IDirectSoundBuffer::OS2IDirectSoundBuffer(OS2IDirectSound *DSound, const DSBUFFERDESC *lpDSBufferDesc)
        : Referenced(0), lastError(DS_OK)
{
   //This is the "normal" constructor
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
   Vtbl.GetFrequency         = SoundBufGetFrequency;
   Vtbl.Initialize           = SoundBufInitialize;
   Vtbl.Restore              = SoundBufRestore;
   Vtbl.SetFormat            = SoundBufSetFormat;
   Vtbl.SetVolume            = SoundBufSetVolume;
   Vtbl.SetCurrentPosition   = SoundBufSetCurrentPosition;
   Vtbl.SetPan               = SoundBufSetPan;
   Vtbl.SetFrequency         = SoundBufSetFrequency;
   Vtbl.Lock                 = SoundBufLock;
   Vtbl.Unlock               = SoundBufUnlock;
   Vtbl.Stop                 = SoundBufStop;
   Vtbl.Play                 = SoundBufPlay;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::OS2IDirectSoundBuffer (buf=%X)", this));

   // get the primary buffer from the DSound instance.
   primary = DSound->primary;

   // We can still to the below logic just in case!!  Should not be required thow!
   // If the primary SoundBuffer doesn't exist by now, we have to create it!
   if (primary == NULL) {
      OS2PrimBuff *primbuff;
      primbuff = new OS2PrimBuff(DSound, lpDSBufferDesc);

      if (primary == NULL) {
         lastError = DSERR_OUTOFMEMORY;
         return;
      }
   }
   // Add a reference to the primary buffer for _every_ secondary buffer; this makes
   // sure that as long as a secondary buffer exists the primary buffer won't get
   // destroyed; moreover if the user didn't create  a primary buffer himself/herself,
   // it will automatically get created and destroyed with the last secondary buffer
   primary->Vtbl.AddRef(primary);

   // Start playing the primary buffer
   // TODO: only start playing the primary buffer when necessary!
   OS2IDirectSoundBuffer::primary->Vtbl.Play(OS2IDirectSoundBuffer::primary, 0, 0, DSBPLAY_LOOPING);

   parentDS  = DSound;
   writepos  = 0;
   playpos   = 0;
   status    = 0;
   fLocked   = FALSE;
   fPrimary  = FALSE;
   volume    = 255;
   DSvolume  = 0;
   pan       = 0;
   DSpan     = 0;
   lpfxFormat= NULL;
   fPlaying  = FALSE;
   fLoop     = FALSE;
   notify    = NULL;
   memcpy(&bufferdesc, lpDSBufferDesc, sizeof(DSBUFFERDESC));

   // Note: this cannot be a primary buffer - those take a different route

   // frequency has to be set according to the bufer format
   frequency = bufferdesc.lpwfxFormat->nSamplesPerSec;

   if (bufferdesc.lpwfxFormat == NULL || bufferdesc.dwBufferBytes == 0) {
      dprintf(("bufferdesc not valid!"));
      lastError = DSERR_INVALIDPARAM;
      return;
   }

   // Some apps pass trash in cbSize, can't rely on that!
   lpfxFormat = (WAVEFORMATEX *)malloc(/*bufferdesc.lpwfxFormat->cbSize +*/ sizeof(WAVEFORMATEX));
   memcpy(lpfxFormat,
          bufferdesc.lpwfxFormat,
          /*bufferdesc.lpwfxFormat->cbSize + */sizeof(WAVEFORMATEX));

   dprintf((" Buffer format: %dHz, %dbit, %d channels", lpfxFormat->nSamplesPerSec, lpfxFormat->wBitsPerSample, lpfxFormat->nChannels));

   lpSndBuffer = (SOUNDBUF*)VirtualAlloc(0, bufferdesc.dwBufferBytes + sizeof(DWORD), MEM_COMMIT, PAGE_READWRITE);
   if (lpSndBuffer == NULL) {
      dprintf(("VirtualAlloc failed"));
      lpSndBuffer  = NULL;
      lastError = DSERR_OUTOFMEMORY;
      return;
   }
   lpBuffer = &(lpSndBuffer->pData[0]);
   lpSndBuffer->dwReferences = 1;

   next = dsbroot;
   dsbroot = this;
}

//******************************************************************************
//******************************************************************************
OS2IDirectSoundBuffer::OS2IDirectSoundBuffer(OS2IDirectSound *DSound, OS2IDirectSoundBuffer *srcBuf)
        : Referenced(0), lastError(DS_OK)
{
   //This is the constructor used for duplicating sound buffers
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
   Vtbl.GetFrequency         = SoundBufGetFrequency;
   Vtbl.Initialize           = SoundBufInitialize;
   Vtbl.Restore              = SoundBufRestore;
   Vtbl.SetFormat            = SoundBufSetFormat;
   Vtbl.SetVolume            = SoundBufSetVolume;
   Vtbl.SetCurrentPosition   = SoundBufSetCurrentPosition;
   Vtbl.SetPan               = SoundBufSetPan;
   Vtbl.SetFrequency         = SoundBufSetFrequency;
   Vtbl.Lock                 = SoundBufLock;
   Vtbl.Unlock               = SoundBufUnlock;
   Vtbl.Stop                 = SoundBufStop;
   Vtbl.Play                 = SoundBufPlay;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::OS2IDirectSoundBuffer/Duplicate (buf=%X)", this));

   if (srcBuf->fPrimary) {
      dprintf(("Error: Cannot duplicate primary sound buffer!"));
      lpBuffer  = NULL;
      lastError = DSERR_INVALIDPARAM;
      return;
   }

   // get the primary buffer from the DSound instance.
   primary = DSound->primary;

   // No need to check if primary exists - it has to (this can't be the first
   // secondary buffer being created)

   // Add a reference to the primary buffer for _every_ secondary buffer; this makes
   // sure that as long as a secondary buffer exists the primary buffer won't get
   // destroyed; moreover if the user didn't create  a primary buffer himself/herself,
   // it will automatically get created and destroyed with the last secondary buffer
   primary->Vtbl.AddRef(primary);

   parentDS  = DSound;
   writepos  = 0;
   playpos   = 0;
   status    = 0;
   fLocked   = FALSE;
   fPrimary  = FALSE;
   volume    = srcBuf->volume;
   DSvolume  = srcBuf->DSvolume;
   pan       = srcBuf->pan;
   DSpan     = srcBuf->DSpan;
   lpBuffer  = srcBuf->lpBuffer;
   fPlaying  = FALSE;
   fLoop     = srcBuf->fLoop;
   notify    = NULL;
   memcpy(&bufferdesc, &(srcBuf->bufferdesc), sizeof(DSBUFFERDESC));

   // Note: this cannot be a primary buffer - those take a different route

   // frequency has to be set according to the bufer format
   frequency = srcBuf->frequency;

   lpfxFormat = (WAVEFORMATEX *)malloc(sizeof(WAVEFORMATEX));
   memcpy(lpfxFormat,srcBuf->lpfxFormat,
          sizeof(WAVEFORMATEX));

   dprintf((" Buffer format: %dHz, %dbit, %d channels", lpfxFormat->nSamplesPerSec, lpfxFormat->wBitsPerSample, lpfxFormat->nChannels));

   // Increment reference count for the buffer memory
   lpSndBuffer  = srcBuf->lpSndBuffer;
   lpSndBuffer->dwReferences++;

   next = dsbroot;
   dsbroot = this;
}

//******************************************************************************
//******************************************************************************
OS2IDirectSoundBuffer::~OS2IDirectSoundBuffer()
{
   /* We must not run this destructor on the primary buffer! */
   if (fPrimary)
      return;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::~OS2IDirectSoundBuffer (buf=%X)", this));

   // free allocted memory - unless it's shared by a duplicated buffer
   if (lpSndBuffer) {
      lpSndBuffer->dwReferences--;
      if (lpSndBuffer->dwReferences == 0)
         VirtualFree(lpSndBuffer, 0, MEM_RELEASE);
   }

   if (lpfxFormat)
      free(lpfxFormat);

   // remove ourselves from the linked list
   OS2IDirectSoundBuffer *cur  = dsbroot;
   OS2IDirectSoundBuffer *prev = NULL;

   // release the primary buffer too
   primary->Vtbl.Release(primary);

   if (this == dsbroot)  // is this the first SoundBuffer?
      dsbroot = next;
   else {                // walk the chain
      while (cur->next != this)
         cur  = cur->next;

      cur->next = next;
   }

}

//******************************************************************************
//******************************************************************************
void OS2IDirectSoundBuffer::DestroyAllBuffers()
{
   dprintf(("DSOUND-OS2IDirectSoundBuffer::DestroyAllBuffers"));

   // if any SoundBuffers still exist when the OS2IDirectSound object is
   // being closed, just kill them all
   OS2IDirectSoundBuffer *cur  = dsbroot;
   OS2IDirectSoundBuffer *tmp;

   if (primary != NULL)
      primary->Vtbl.Stop(primary);

   while (cur != NULL) {
      tmp = cur->next;
      delete cur;
      cur = tmp;
   }
   dprintf(("DSOUND-OS2IDirectSoundBuffer::DestroyAllBuffers - %X", primary));
   if (primary != NULL)
      primary->Vtbl.Release(primary);
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-OS2IDirectSoundBuffer::QueryInterface"));
   if (This == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *ppvObj = NULL;

   if (IsEqualGUID(riid, IID_IDirectSoundBuffer)) {
      *ppvObj = This;

      SoundBufAddRef(This);
      return DS_OK;
   }

   if (IsEqualGUID(riid, IID_IDirectSoundNotify)) {
      OS2IDirectSoundBuffer  *me     = (OS2IDirectSoundBuffer *)This;
      OS2IDirectSoundNotify  *notify;

      notify     = new OS2IDirectSoundNotify(me);
      *ppvObj    = notify;
      notify->Vtbl.AddRef(notify);
      return DS_OK;
   }

   if (IsEqualGUID(riid, IID_IDirectSound3DBuffer)) {
      OS2IDirectSoundBuffer   *me     = (OS2IDirectSoundBuffer *)This;
      OS2IDirectSound3DBuffer *buffer3D;

      buffer3D   = new OS2IDirectSound3DBuffer(me);
      *ppvObj    = buffer3D;
      buffer3D->Vtbl.AddRef((IDirectSound3DBuffer *)buffer3D);
      return DS_OK;
   }

   return E_NOINTERFACE;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundBufAddRef(THIS)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::AddRef (buf=%X) %d", me, me->Referenced+1));
   if (me == NULL) {
      return 0;
   }
   return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundBufRelease(THIS)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::Release (buf=%X) %d", me, me->Referenced-1));
   if (me == NULL) {
      return 0;
   }
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
HRESULT __stdcall SoundBufGetCaps(THIS_ LPDSBCAPS lpDSCaps)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufGetCaps (buf=%X)", me));
   if (me == NULL || lpDSCaps == NULL) {
      return DSERR_INVALIDPARAM;
   }
   /* FIXME: fill with more realistic values */
   lpDSCaps->dwSize               = sizeof(DSBCAPS);
   lpDSCaps->dwFlags              = me->bufferdesc.dwFlags | DSBCAPS_LOCSOFTWARE;
   lpDSCaps->dwBufferBytes        = me->bufferdesc.dwBufferBytes;
   lpDSCaps->dwUnlockTransferRate = 0;     /* in KB/sec - 0 == no transfer needed */
   lpDSCaps->dwPlayCpuOverhead    = 0;     /* % CPU time - let's lie */

   return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetCurrentPosition(THIS_ LPDWORD lpdwCurrentPlayCursor,
                         LPDWORD lpdwCurrentWriteCursor)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufGetCurrentPosition (buf=%X)", me));
   if ( me == NULL || lpdwCurrentPlayCursor == NULL || lpdwCurrentWriteCursor == NULL) {
     dprintf(("  Invalid parameters %d %d %d",me,lpdwCurrentPlayCursor,lpdwCurrentWriteCursor));
     return DSERR_INVALIDPARAM;
   }

   dprintf(("  PlayPos %d, WritePos %d", me->playpos, me->writepos));
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

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufGetFormat (buf=%X)", me));
   if (me == NULL || lpwfxFormat == NULL || ddwSizeAllocated == 0) {
      return DSERR_INVALIDPARAM;
   }
   copysize = min(ddwSizeAllocated, (me->lpfxFormat->cbSize + sizeof(WAVEFORMATEX)));
   memcpy(lpwfxFormat, me->lpfxFormat, copysize);

   if (lpdwSizeWritten) {
      *lpdwSizeWritten = copysize;
   }
   return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetVolume(THIS_ LPLONG lplVolume)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufGetVolume (buf=%X)", me));
   if (me == NULL || lplVolume == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *lplVolume = me->DSvolume;
   return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetPan(THIS_ LPLONG lplPan)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufGetPan (buf=%X)", me));
   if (me == NULL || lplPan == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *lplPan = me->DSpan;
   return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetFrequency(THIS_ LPDWORD lpdwFrequency)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufGetFrequency (buf=%X)", me));
   if (me == NULL || lpdwFrequency == NULL) {
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

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufGetStatus (buf=%X)", me));
   if (me == NULL || lpdwStatus == NULL) {
      return DSERR_INVALIDPARAM;
   }

   if (me->fPlaying)
      if (me->fLoop)
         me->status |= DSBSTATUS_PLAYING | DSBSTATUS_LOOPING;
      else
         me->status |= DSBSTATUS_PLAYING;
   else
      me->status  &= ~(DSBSTATUS_PLAYING | DSBSTATUS_LOOPING);

   *lpdwStatus = me->status;
   return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC )
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-SoundBufInitialize (buf=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }
   return DSERR_ALREADYINITIALIZED;  //todo: for future extensions (dx5/6 ??)
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufLock(THIS_ DWORD dwWriteCursor, DWORD dwWriteBytes,
                   LPVOID lplpvAudioPtr1, LPDWORD lpdwAudioBytes1,
                   LPVOID lplpvAudioPtr2, LPDWORD lpdwAudioBytes2,
                   DWORD dwFlags)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufLock (buf=%X, %d bytes of %d)", me, dwWriteBytes, me->bufferdesc.dwBufferBytes));
   if (me == NULL || !lplpvAudioPtr1 || !lpdwAudioBytes1)
      return DSERR_INVALIDPARAM;

   //not sure if this is an error, but it's certainly a smart thing to do (cond. == true)
   if(dwWriteBytes > me->bufferdesc.dwBufferBytes) {
      dprintf(("SoundBufLock: dwWriteBytes > me->bufferdesc.dwBufferBytes"));
      return DSERR_INVALIDPARAM;
   }
   if (dwFlags & DSBLOCK_FROMWRITECURSOR) {
      dwWriteCursor = me->writepos;
   }
   if (dwWriteCursor + dwWriteBytes > me->bufferdesc.dwBufferBytes) {
      *(DWORD *)lplpvAudioPtr1  = (DWORD)(me->lpBuffer + dwWriteCursor);
      *lpdwAudioBytes1          = me->bufferdesc.dwBufferBytes - dwWriteCursor;
      if (lplpvAudioPtr2 && lpdwAudioBytes2) {
         *(DWORD *)lplpvAudioPtr2   = (DWORD)me->lpBuffer;
         *lpdwAudioBytes2           = dwWriteBytes - *lpdwAudioBytes1;
      }
   }
   else {
      *(DWORD *)lplpvAudioPtr1  = (DWORD)(me->lpBuffer + dwWriteCursor);
      *lpdwAudioBytes1          = dwWriteBytes;
      if (lplpvAudioPtr2 && lpdwAudioBytes2) {
         *(DWORD *)lplpvAudioPtr2   = 0;
         *lpdwAudioBytes2           = 0;
      }
   }

   me->fLocked = TRUE;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufPlay(THIS_ DWORD dwRes1, DWORD dwRes2, DWORD dwFlags)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufPlay (buf=%X)", me));
   if (me == NULL) {
      dprintf(("  Invalid parms - me is NULL"));
      return DSERR_INVALIDPARAM;
   }

   if (!me->fPlaying) {
      me->frac     = 0;
      me->fPlaying = TRUE;
      me->status   = DSBSTATUS_PLAYING;
      me->fLoop = dwFlags == DSBPLAY_LOOPING;
      if (me->fLoop)
         me->status |= DSBSTATUS_LOOPING;

      dprintf(("  Buffer %X: start at pos %d, loop %s",me, me->playpos, me->fLoop?"YES":"NO"));
   }
   else {
      me->fLoop = dwFlags == DSBPLAY_LOOPING;
      if (me->fLoop)
         me->status |= DSBSTATUS_LOOPING;

      dprintf(("  Buffer %X: already playing, loop %s",me, me->fLoop?"YES":"NO"));
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetCurrentPosition(THIS_ DWORD dwNewPosition)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufSetCurrentPosition (buf=%X) to %d", me, dwNewPosition));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }
   me->playpos   = dwNewPosition;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetFormat(THIS_ LPWAVEFORMATEX lpWaveFormatEx)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   // Note: this sets the format of the _primary_ buffer;
   // since this class only handles secondary buffers, we just return error
   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufSetFormat (buf=%X)", me));

   return DSERR_UNSUPPORTED;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetVolume(THIS_ LONG lVolume)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufSetVolume (%d, buf=%X)", lVolume, me));
   if (me == NULL || lVolume < -10000) {
      return DSERR_INVALIDPARAM;
   }
   // some apps pass positive values in lVolume, that's wrong right?!?
   me->DSvolume = (lVolume > 0) ? 0 : lVolume;

   /* = (10 ^ (1/10)) = 1dB - but the formula below gives results _very_ similar */
   /* to 'real' DirectSound, indistinguishable for all practical purposes        */
   //me->volume = 255.0 * pow(4, me->DSvolume / 1000.0);

   /* Note: for some strange reason the above code sometimes gives erroneous     */
   /* results, hence we now use a simple conversion table (in steps of 4/100 dB) */

   if (me->DSvolume < -4000)
      me->volume = 0;
   else
      me->volume = VolTable[-me->DSvolume / 4];

   dprintf(("  New volume: %d", me->volume));

   return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetPan(THIS_ LONG lPan)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufSetPan (%d, buf=%X)", lPan, me));
   if (me == NULL || lPan < -10000 || lPan > 10000) {
      return DSERR_INVALIDPARAM;
   }
   me->DSpan = lPan;
   if (lPan == 0) {
      me->pan = 0;
      return DS_OK;
   }

   /* Note: we use very similar code as for volume above   */
   if (lPan < 0)
      if (lPan < -4000)
         me->pan = -255;
      else
         me->pan = VolTable[-lPan / 4] - 255;
   else
      if (lPan > 4000)
         me->pan = 255;
      else
         me->pan = 255 - VolTable[lPan / 4];

   dprintf(("  New pan: %d", me->pan));

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetFrequency(THIS_ DWORD dwFrequency)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufSetFrequency (buf=%X)", me));
   if (me == NULL) {
     return DSERR_INVALIDPARAM;
   }

   // zero means default (buffer format) frequency
   if (dwFrequency)
      me->frequency = dwFrequency;
   else
     me->frequency = me->lpfxFormat->nSamplesPerSec;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufStop(THIS  )
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufStop (buf=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->fPlaying = FALSE;
   me->status  &= ~(DSBSTATUS_PLAYING | DSBSTATUS_LOOPING);

   if (me->notify != NULL)
      me->notify->CheckStop();

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufUnlock(THIS_
                                 LPVOID lpvAudioPtr1, DWORD dwAudioBytes1,
                                 LPVOID lpvAudioPtr2, DWORD dwAudioBytes2)
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufUnlock (buf=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->fLocked = TRUE;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufRestore(THIS  )
{
   OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSoundBuffer::SoundBufRestore (buf=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }
   return DS_OK;
}
//******************************************************************************
//******************************************************************************
