/* $Id: OS2SNDBUFFER.CPP,v 1.6 2000-02-04 19:29:16 hugh Exp $ */

/*
 * DirectSound SoundBuffer class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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

#include "os2dsound.h"
#include "os2sndbuffer.h"
#include <misc.h>


extern DWORD GetTickCountOS2();

WAVEFORMATEX wfxDefaultPrimaryBuf = { WAVE_FORMAT_PCM,
                                      2,
                                      22050,
                                      44100,
                                      2,
                                      8,
                                      0};

//******************************************************************************
//******************************************************************************
OS2IDirectSoundBuffer::OS2IDirectSoundBuffer(const DSBUFFERDESC *lpDSBufferDesc) //KSO Apr 13 1999: const, SDK changes
        : Referenced(0), lastError(DS_OK)
{
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
  fPlaying  = FALSE;
  fLoop     = FALSE;
  newWritePos = 0;
  LastTickCnt = 0;
  memcpy(&bufferdesc, lpDSBufferDesc, sizeof(DSBUFFERDESC));

  if(bufferdesc.dwFlags & DSBCAPS_PRIMARYBUFFER)
  {
    // Primary Buffer
    // Fake this for now and setup the default values for it
    //todo: Do the primary buffer right
    bufferdesc.dwBufferBytes = 4096;
    bufferdesc.lpwfxFormat = &wfxDefaultPrimaryBuf;

  }

  if(bufferdesc.lpwfxFormat == NULL || bufferdesc.dwBufferBytes == 0)
  {
      dprintf(("bufferdesc not valid!"));
      lastError = DSERR_INVALIDPARAM;
      return;
  }

  lpfxFormat = (WAVEFORMATEX *)malloc(bufferdesc.lpwfxFormat->cbSize + sizeof(WAVEFORMATEX));
  memcpy( lpfxFormat,
          bufferdesc.lpwfxFormat,
          bufferdesc.lpwfxFormat->cbSize + sizeof(WAVEFORMATEX));

  bufferdesc.lpwfxFormat = lpfxFormat;

  lpBuffer = (LPSTR)VirtualAlloc( 0, bufferdesc.dwBufferBytes, MEM_COMMIT, PAGE_READWRITE);
  if(lpBuffer == NULL)
  {
      dprintf(("VirtualAlloc failed"));
      lpBuffer  = NULL;
      lastError = DSERR_OUTOFMEMORY;
      return;
  }

}
//******************************************************************************
//******************************************************************************
OS2IDirectSoundBuffer::~OS2IDirectSoundBuffer()
{
  if(lpBuffer)
    VirtualFree(lpBuffer, 0, MEM_RELEASE);
  if(lpfxFormat)
    free(lpfxFormat);

}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SoundBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
  dprintf(("OS2IDirectSoundBuffer::QueryInterface\n"));
  if(This == NULL)
  {
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
  if(me == NULL)
  {
    return 0;
  }
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API SoundBufRelease(THIS)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::Release %d\n", me->Referenced-1));
  if(me == NULL)
  {
    return 0;
  }
  if(me->Referenced)
  {
    me->Referenced--;
    if(me->Referenced == 0)
    {
      delete me;
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
HRESULT __stdcall SoundBufGetCaps(THIS_ LPDSBCAPS lpDSCaps)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufGetCaps"));
  if(me == NULL || lpDSCaps == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufGetCurrentPosition"));
  if( me == NULL || lpdwCurrentPlayCursor == NULL || lpdwCurrentWriteCursor == NULL)
  {
    dprintf(("  Invalid parameters %d %d %d",me,lpdwCurrentPlayCursor,lpdwCurrentWriteCursor));
    return DSERR_INVALIDPARAM;
  }

  // Simply advance the bufferpointer according to time;
  if(me->fPlaying)
  {
    ULONG CurrentTicks, Ticks, NewPos;
    CurrentTicks = GetTickCountOS2();
    Ticks = CurrentTicks-me->LastTickCnt;
    dprintf(("  Is Playing, Ticks: Last %d Current %d Difference %d",me->LastTickCnt, CurrentTicks,Ticks));
    dprintf(("  Frequency: %d Bits per Sample %d",me->frequency,me->lpfxFormat->wBitsPerSample));

    NewPos = ((me->frequency*Ticks)/1000)*(me->lpfxFormat->wBitsPerSample/8);
    dprintf(("  advance pos by %d bytes",NewPos));

    if(NewPos)
      me->LastTickCnt = CurrentTicks;

    if(me->playpos+NewPos >= me->bufferdesc.dwBufferBytes)
    {
      if(me->fLoop)
      {
        me->playpos = (me->playpos+NewPos) % me->bufferdesc.dwBufferBytes;
      }
      else
      {
        me->playpos  = me->bufferdesc.dwBufferBytes;
        me->fPlaying = FALSE;
      }

    }
    else
    {
      me->playpos += NewPos;
    }
    me->writepos = me->playpos + 2048;
    if(me->writepos >= me->bufferdesc.dwBufferBytes)
      me->writepos = me->writepos % me->bufferdesc.dwBufferBytes;
    dprintf(("  new Pos Write: %d   Play: %d",me->writepos,me->playpos));
  }
  else
  {
    dprintf(("  Buffer Not playing"));
  }
  dprintf(("  Buffer %d: PlayPos %d, WritePos %d\n",me, me->playpos,me->writepos));
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufGetFormat"));
  if( me == NULL || lpwfxFormat == NULL || ddwSizeAllocated == 0)
  {
    return DSERR_INVALIDPARAM;
  }
  copysize = min(ddwSizeAllocated, (me->lpfxFormat->cbSize + sizeof(WAVEFORMATEX)));
  memcpy(lpwfxFormat, me->lpfxFormat, copysize);

  if(lpdwSizeWritten)
  {
    *lpdwSizeWritten = copysize;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufGetVolume(THIS_ LPLONG lplVolume)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufGetVolume"));
  if(me == NULL || lplVolume == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufGetPan"));
  if(me == NULL || lplPan == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufGetFrequency"));
  if(me == NULL || lpdwFrequency == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufGetStatus"));
  if(me == NULL || lpdwStatus == NULL)
  {
    return DSERR_INVALIDPARAM;
  }
  *lpdwStatus = me->status;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("SoundBufInitialize"));
  if(me == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufLock"));
  if(me == NULL || !lplpvAudioPtr1 || !lpdwAudioBytes1)
  {
    return(DSERR_INVALIDPARAM);
  }
  //not sure if this is an error, but it's certainly a smart thing to do (cond. == true)
  if(dwWriteBytes > me->bufferdesc.dwBufferBytes)
  {
    dprintf(("SoundBufLock: dwWriteBytes > me->bufferdesc.dwBufferBytes"));
    return(DSERR_INVALIDPARAM);
  }
  if(dwFlags & DSBLOCK_FROMWRITECURSOR)
  {
    dwWriteCursor = me->writepos;
  }
  dprintf(("  Buffer at 0x%08X Size %d",me->lpBuffer,me->bufferdesc.dwBufferBytes));
  if(dwWriteCursor + dwWriteBytes > me->bufferdesc.dwBufferBytes )
  {
    *(DWORD *)lplpvAudioPtr1  = (DWORD)(me->lpBuffer + dwWriteCursor);
    *lpdwAudioBytes1          = me->bufferdesc.dwBufferBytes - dwWriteCursor;
    dprintf(("  Lock1 at 0x%08X Size %d",*(DWORD *)lplpvAudioPtr1,*lpdwAudioBytes1));
    if(lplpvAudioPtr2 && lpdwAudioBytes2)
    {
      *(DWORD *)lplpvAudioPtr2   = (DWORD)me->lpBuffer;
      *lpdwAudioBytes2           = dwWriteBytes - *lpdwAudioBytes1;
      dprintf(("  Lock2 at 0x%08X Size %d",*(DWORD *)lplpvAudioPtr2,*lpdwAudioBytes2));
    }
  }
  else
  {
    *(DWORD *)lplpvAudioPtr1  = (DWORD)(me->lpBuffer + dwWriteCursor);
    *lpdwAudioBytes1          = dwWriteBytes;
    dprintf(("  Lock1 at 0x%08X Size %d",*(DWORD *)lplpvAudioPtr1,*lpdwAudioBytes1));
    if(lplpvAudioPtr2 && lpdwAudioBytes2)
    {
      *(DWORD *)lplpvAudioPtr2   = 0;
      *lpdwAudioBytes2           = 0;
      dprintf(("  Lock2 at 0x%08X Size %d",*(DWORD *)lplpvAudioPtr2,*lpdwAudioBytes2));
    }
  }
  me->fLocked = TRUE;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufPlay(THIS_ DWORD dwRes1,DWORD dwRes2,DWORD dwFlags)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufPlay"));
  if(me == NULL)
  {
    dprintf(("  Invalid parms me is NULL\n"));
    return DSERR_INVALIDPARAM;
  }
  me->fPlaying = TRUE;
  me->fLoop = dwFlags == DSBPLAY_LOOPING;
  me->LastTickCnt = GetTickCountOS2();
  dprintf(("  Buffer %d: Start at Tick %d Loop %s",me,me->LastTickCnt,me->fLoop?"YES":"NO"));
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetCurrentPosition(THIS_ DWORD dwNewPosition)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufSetCurrentPosition"));
  if(me == NULL)
  {
    return DSERR_INVALIDPARAM;
  }
  //todo: check for primary buffer (can't do it with those)
  me->playpos   = dwNewPosition;
  me->writepos  = dwNewPosition;
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetFormat(THIS_ LPWAVEFORMATEX )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufSetFormat"));
  if(me == NULL)
  {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufSetVolume(THIS_ LONG lVolume)
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufSetVolume"));
  if(me == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufSetPan"));
  if(me == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufSetFrequency"));
  if(me == NULL)
  {
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

  dprintf(("OS2IDirectSoundBuffer::SoundBufStop"));
  if(me == NULL)
  {
    return DSERR_INVALIDPARAM;
  }
  // Simply advance the bufferpointer according to time;
  if(me->fPlaying)
  {
    ULONG CurrentTicks, Ticks, NewPos;
    CurrentTicks = GetTickCountOS2();

    Ticks = CurrentTicks - me->LastTickCnt;
    me->LastTickCnt = CurrentTicks;

    NewPos = ((me->frequency*Ticks)/1000)*(me->lpfxFormat->wBitsPerSample>>8);
    if(me->playpos+NewPos >= me->bufferdesc.dwBufferBytes)
    {
      if(me->fLoop)
      {
        me->playpos = (me->playpos+NewPos % me->bufferdesc.dwBufferBytes);
      }
      else
      {
        me->playpos  = me->bufferdesc.dwBufferBytes;
        me->fPlaying = FALSE;
      }

    }
    else
    {
      me->playpos += NewPos;
    }
    me->writepos = me->playpos + 2048;
    if(me->writepos >= me->bufferdesc.dwBufferBytes)
      me->writepos = me->writepos % me->bufferdesc.dwBufferBytes;

    me->fPlaying = FALSE;

  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufUnlock"));
  if(me == NULL)
  {
    return DSERR_INVALIDPARAM;
  }

  return DS_OK;
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufRestore(THIS  )
{
 OS2IDirectSoundBuffer *me = (OS2IDirectSoundBuffer *)This;

  dprintf(("OS2IDirectSoundBuffer::SoundBufRestore"));
  if(me == NULL)
  {
    return DSERR_INVALIDPARAM;
  }
  return DS_OK;
}
//******************************************************************************
//******************************************************************************
