/* $Id: OS2PrimBuff.cpp,v 1.5 2001-03-06 20:11:16 mike Exp $ */
/*
 *  DirectSound Primary Buffer
 *
 *  Copyright 1999-2000  Kevin Langman
 *
 *  Project Odin Software License can be found in LICENSE.TXT
 *
 *
 */


/*  NOTES:

      Here is how the buffers, GetPosition and Locking works:
      Dart is allocated many little buffers. These buffers are filled
      when dart has finished playing one of the buffer, from a 32kb buffer
      that the Win32 program can write to. The GetPosition function will
      determine what buffer is being played, and return the offset to the
      end of this buffer.

      The Lock function will call the GetCurrentPosition function to determine
      where the end of the currently playing buffer is and allow the Win32
      program to lock between that location and 15ms from there
      (15ms should be past the next buffer).

*/

#define INCL_DOSMISC
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITGUID
#include <dsound.h>

//#include "DSound.h"
#include "OS2DSound.h"
#include "OS2SndBuffer.h"
#include "OS2PrimBuff.h"
#include "OS23DListener.h"

#include "dart.h"

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

//******************************************************************************
//******************************************************************************
OS2PrimBuff::OS2PrimBuff(OS2IDirectSound *DSound, const DSBUFFERDESC *lpDSBufferDesc )
{
   lpVtbl = &Vtbl;
   Vtbl.AddRef               = PrimBufAddRef;
   Vtbl.Release              = PrimBufRelease;
   Vtbl.QueryInterface       = PrimBufQueryInterface;
   Vtbl.GetCaps              = PrimBufGetCaps;
   Vtbl.GetFormat            = PrimBufGetFormat;
   Vtbl.GetVolume            = PrimBufGetVolume;
   Vtbl.GetStatus            = PrimBufGetStatus;
   Vtbl.GetCurrentPosition   = PrimBufGetCurrentPosition;
   Vtbl.GetPan               = PrimBufGetPan;
   Vtbl.GetFrequency         = PrimBufGetFrequency;
   Vtbl.Initialize           = PrimBufInitialize;
   Vtbl.Restore              = PrimBufRestore;
   Vtbl.SetFormat            = PrimBufSetFormat;
   Vtbl.SetVolume            = PrimBufSetVolume;
   Vtbl.SetCurrentPosition   = PrimBufSetCurrentPosition;
   Vtbl.SetPan               = PrimBufSetPan;
   Vtbl.SetFrequency         = PrimBufSetFrequency;
   Vtbl.Lock                 = PrimBufLock;
   Vtbl.Unlock               = PrimBufUnlock;
   Vtbl.Stop                 = PrimBufStop;
   Vtbl.Play                 = PrimBufPlay;

   dprintf(("DSOUND-PrimBuff: Constructor"));

   parentDS   = DSound;
   fPlaying   = FALSE;
   playpos    = 0;
   fPrimary   = TRUE;
   Referenced = 0;
   listener   = NULL;

   lpfxFormat = (WAVEFORMATEX *)malloc(/*bufferdesc.lpwfxFormat->cbSize +*/ sizeof(WAVEFORMATEX));
   lpfxFormat->wBitsPerSample = bps      = 16;
   lpfxFormat->nChannels      = channels = 2;
   lpfxFormat->nSamplesPerSec = rate     = 22500;

   bufferdesc.dwBufferBytes   = BUFFER_SIZE;

   lastError = Dart_Open_Device(&usDeviceID,  &vpMixBuffer, &vpMixSetup,
                                &vpBuffParms, (PVOID*)&lpBuffer );

   if (lastError == DS_OK)
      primary = this;
}

//******************************************************************************
//******************************************************************************
OS2PrimBuff::~OS2PrimBuff()
{
   dprintf(("DSOUND-PrimBuff: Destructor"));

   if (fPlaying)
      Dart_Stop(usDeviceID);

   lastError = Dart_Close_Device(usDeviceID, vpMixBuffer, vpMixSetup,
                                 vpBuffParms );

   dprintf(("DSOUND-PrimBuff: Freeing Memory"));
   free(lpfxFormat);
   //free( *vpMixBuffer );
   //free( *vpMixSetup  );
   //free( *vpBuffParms );
   //free( vpGenericParms );

   dprintf(("DSOUND-PrimBuff: Destructor Complete"));

   primary = NULL;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API PrimBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-PrimBuff: QueryInterface"));

   if (This == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *ppvObj = NULL;

   if (IsEqualGUID(riid, IID_IDirectSoundBuffer)) {
      *ppvObj = This;
      PrimBufAddRef(This);
      return DS_OK;
   }


   if (IsEqualGUID(riid, IID_IDirectSound3DListener)) {
      OS2PrimBuff                *me = (OS2PrimBuff *)This;
      OS2IDirectSound3DListener  *listener;

      listener     = new OS2IDirectSound3DListener(me);
      *ppvObj      = listener;
      listener->Vtbl.AddRef((IDirectSound3DListener *)listener);
      return DS_OK;
   }

   // I guess a Notify object should be creaed for the Primary Buffer also..
   // Does windows DSound have a notify object for Primary buffer???

   return E_NOINTERFACE;
}

//******************************************************************************
//******************************************************************************
ULONG WIN32API PrimBufAddRef(THIS)
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;

   dprintf(("DSOUND-PrimBuff: AddRef %d",  me->Referenced+1));

   if (me == NULL) {
      return DS_OK;
   }
   return ++me->Referenced;
}

//******************************************************************************
//******************************************************************************
ULONG WIN32API PrimBufRelease(THIS)
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;

   dprintf(("DSOUND-PrimBuff: Release %d", me->Referenced-1));

   if (me == NULL) {
      return DS_OK;
   }
   if (me->Referenced) {
      me->Referenced--;
      if (me->Referenced == 0) {
         delete me;
         return DS_OK;
      }
      else
         return me->Referenced;
   }
   else
      return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufGetCaps(THIS_ LPDSBCAPS lpDSCaps )
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;

   dprintf(("DSOUND-PrimBuff: GetCaps"));
   if(me == NULL || lpDSCaps == NULL)
   {
      return DSERR_INVALIDPARAM;
   }

   lpDSCaps->dwFlags              = DSBCAPS_PRIMARYBUFFER;
   lpDSCaps->dwBufferBytes        = BUFFER_SIZE;
   lpDSCaps->dwUnlockTransferRate = 0;
   lpDSCaps->dwPlayCpuOverhead    = 0;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufGetCurrentPosition(THIS_ LPDWORD lpdwCurrentPlayCursor,LPDWORD lpdwCurrentWriteCursor )
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;
   LONG lDartPos;

   dprintf(("DSOUND-PrimBuff: GetCurrentPosition"));

   if (me == NULL || lpdwCurrentPlayCursor == NULL || lpdwCurrentWriteCursor == NULL) {
      return DSERR_INVALIDPARAM;
   }

   if (me->fPlaying == FALSE) {
      *lpdwCurrentPlayCursor  = 0;
      *lpdwCurrentWriteCursor = 0;
      return DS_OK;
   }


   Dart_GetPosition( me->usDeviceID, &lDartPos ); // Returns the Playing Buffer * 'Bytes Per Dart Bufer'

   *lpdwCurrentPlayCursor  = lDartPos;

   // Calculate the Write Cursor as 15ms from the current play cursor.
   // NOTE: This may need to be changed to insure that the write cursor is always beyond the
   //       point where the next buffer will read from.
   *lpdwCurrentWriteCursor = (lDartPos + (me->rate * me->channels * (me->bps/8) * 15 / 1000) ) % BUFFER_SIZE;

   return DS_OK;
}


//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufGetFormat(THIS_ LPWAVEFORMATEX lpfxFormat, DWORD dwSizeAllocated , LPDWORD lpdwSizeWritten )
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;
   long copysize;

   dprintf(("DSOUND-PrimBuff: GetFormat"));

   if( me == NULL || lpfxFormat == NULL || dwSizeAllocated == 0)
   {
      return DSERR_INVALIDPARAM;
   }

   copysize = MIN(dwSizeAllocated, (me->lpfxFormat->cbSize + sizeof(WAVEFORMATEX)));
   memcpy(lpfxFormat, me->lpfxFormat, copysize);

   if(lpdwSizeWritten)
   {
      *lpdwSizeWritten = copysize;
   }
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufGetVolume(THIS_ LPLONG )
{
   dprintf(("DSOUND-PrimBuff: GetVolume"));

   return DSERR_INVALIDCALL;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufGetPan(THIS_ LPLONG )
{
   dprintf(("DSOUND-PrimBuff: GetPan"));

   return DSERR_INVALIDCALL;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufGetFrequency(THIS_ LPDWORD )
{
   dprintf(("DSOUND-PrimBuff: GetGrequency"));

   return DSERR_INVALIDCALL;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufGetStatus(THIS_ LPDWORD lpdwStatus )
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;
   dprintf(("DSOUND-PrimBuff: GetStatus"));

   if (me == NULL || lpdwStatus == NULL) {
      return(DSERR_INVALIDPARAM);
   }

   *lpdwStatus = DSBSTATUS_LOOPING /*| DSBSTATUS_LOCSOFTWARE*/;
   if (me->fPlaying == TRUE)
      *lpdwStatus |= DSBSTATUS_PLAYING;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC )
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;
   dprintf(("DSOUND-PrimBuff: Initialize"));

   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }
   return DSERR_ALREADYINITIALIZED;  //todo: for future extensions (dx5/6 ??) ??
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufLock(THIS_ DWORD dwWriteCursor, DWORD dwWriteBytes,
                              LPVOID lplpAudioPtr1, LPDWORD lpdwAudioBytes1,
                              LPVOID lplpAudioPtr2, LPDWORD lpdwAudioBytes2,
                              DWORD  dwFlags)
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;
   DWORD dwCurPlayPos, dwCurWritePos;
   LONG rc;

   dprintf(("DSOUND-PrimBuff: Lock"));

   if (me == NULL || !lplpAudioPtr1 || !lpdwAudioBytes1) {
      return DSERR_INVALIDPARAM;
   }
   //not sure if this is an error, but it's certainly a smart thing to do (cond. == true)
   if (dwWriteBytes > BUFFER_SIZE) {
      dprintf(("SoundBufLock: dwWriteBytes > me->bufferdesc.dwBufferBytes"));
      return DSERR_INVALIDPARAM;
   }

   if (me->fPlaying == FALSE) {
      *(DWORD*)lplpAudioPtr1 = (DWORD)(me->lpBuffer);
      *lpdwAudioBytes1       = dwWriteBytes;
      if (lplpAudioPtr2 != NULL) {
         *(DWORD*)lplpAudioPtr2 = NULL;
         *lpdwAudioBytes2       = 0;
      }
      return DS_OK;
   }

   rc = PrimBufGetCurrentPosition( me, &dwCurPlayPos, &dwCurWritePos );
   if (rc != DS_OK) {
      return DSERR_GENERIC;
   }

   /* Debugging */
   dprintf(("DSOUND-PrimBuff: CurPlay = %d, CurWrite = %d", dwCurPlayPos, dwCurWritePos));

   if (dwFlags & DSBLOCK_FROMWRITECURSOR) {
      dprintf(("DSOUND-PrimBuff: Locking FromCursor"));
      dwWriteCursor = dwCurWritePos;
   }

   *(DWORD*)lplpAudioPtr1 = (DWORD)(me->lpBuffer + dwWriteCursor);

   if (dwWriteCursor > dwCurPlayPos) {
      if (dwWriteCursor + dwWriteBytes > BUFFER_SIZE) {
         *lpdwAudioBytes1 = BUFFER_SIZE - dwWriteCursor;
         if (lplpAudioPtr2!=NULL) {
            *(DWORD*)lplpAudioPtr2 = (DWORD)me->lpBuffer;
            if (dwWriteBytes - *lpdwAudioBytes1 > dwCurPlayPos) {
               *lpdwAudioBytes2 = dwCurPlayPos;
            } else {
               *lpdwAudioBytes2 = dwWriteBytes - *lpdwAudioBytes1;
            }
         }
      } else {
         *lpdwAudioBytes1 = dwWriteBytes;
         if (lplpAudioPtr2!=NULL) {
            *(DWORD*)lplpAudioPtr2   = NULL;
            *lpdwAudioBytes2 = 0;
         }
      }
   } else { // The WriteCursor is behind of the PlayCursor
      if (lplpAudioPtr2!=NULL) {
         *(DWORD*)lplpAudioPtr2   = NULL;
         *lpdwAudioBytes2 = 0;
      }
      if (dwWriteCursor + dwWriteBytes > dwCurPlayPos) {
         *lpdwAudioBytes1 = dwWriteCursor - dwCurPlayPos;
      } else {
         *lpdwAudioBytes1 = dwWriteBytes;
      }
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufPlay(THIS_ DWORD dwRes1,DWORD dwRes2,DWORD dwFlags )
{
   long rc;
   OS2PrimBuff *me = (OS2PrimBuff *)This;

   if (me == NULL || !(dwFlags & DSBPLAY_LOOPING) ) {
     return DSERR_INVALIDPARAM;
   }

   if (me->fPlaying)
      return DS_OK;

   dprintf(("DSOUND-PrimBuff: Play"));

   rc = Dart_Play(me->usDeviceID, me->vpMixSetup, me->vpMixBuffer, me->fPlaying);
   me->fPlaying = TRUE;
   return rc;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufSetCurrentPosition(THIS_ DWORD )
{
   dprintf(("DSOUND-PrimBuff: SetCurrentPosition"));

   return DSERR_INVALIDCALL;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufSetFormat(THIS_ LPWAVEFORMATEX lpfxFormat )
{
   dprintf(("DSOUND-PrimBuff: SetFormat"));
   OS2PrimBuff *me = (OS2PrimBuff *)This;

   if (me == NULL || lpfxFormat == NULL) {
      return DSERR_INVALIDPARAM;
   }

   /* Note: the software mixer doesn't really support mono output but */
   /*       we can safely mix in stereo anyway                        */
   if (lpfxFormat->nChannels == 1) {
      if (me->parentDS->GetCoopLevel() != DSSCL_WRITEPRIMARY)
         lpfxFormat->nChannels = 2;
   }

   memcpy( me->lpfxFormat, lpfxFormat, sizeof(WAVEFORMATEX) );

   me->bps      = lpfxFormat->wBitsPerSample;
   me->rate     = lpfxFormat->nSamplesPerSec;
   me->channels = lpfxFormat->nChannels;

   dprintf(("DSOUND-PrimBuff: BPS = %d, Rate = %d, Ch = %d", me->bps, me->rate, me->channels));

   Dart_SetFormat(&me->usDeviceID, me->vpMixSetup, me->vpBuffParms, &me->vpMixBuffer, me->bps, me->rate, me->channels);

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufSetVolume(THIS_ LONG )
{
   dprintf(("DSOUND-PrimBuff: SetVolume"));

   return DSERR_CONTROLUNAVAIL;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufSetPan(THIS_ LONG )
{
   dprintf(("DSOUND-PrimBuff: SetPan"));

   return DSERR_CONTROLUNAVAIL;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufSetFrequency(THIS_ DWORD )
{
   dprintf(("DSOUND-PrimBuff: SetFrequency"));

   return DSERR_CONTROLUNAVAIL;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufStop(THIS  )
{
   OS2PrimBuff *me = (OS2PrimBuff *)This;
   dprintf(("DSOUND-PrimBuff: Stop"));

   if (me == NULL) {
     return DSERR_INVALIDPARAM;
   }

   if (!me->fPlaying)
      return DS_OK;

   me->fPlaying = FALSE;

   return Dart_Stop(me->usDeviceID);
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD )
{
   // I don't think we really need any code here..

   dprintf(("DSOUND-PrimBuff: Unlock"));

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PrimBufRestore(THIS )
{
   // This maybe a good place to re-aquire the device if some other process
   // has taken the audio focus, but before we do that we need to determine
   // if we have lost the device and set the falg in the GetStatus method!

   dprintf(("DSOUND-PrimBuff: Restore"));

   return DS_OK;
}


