/* $Id: DAudioBuffer.h,v 1.1 2001-04-27 17:39:48 sandervl Exp $ */

/*
 * DirectSound SoundBuffer class (DAudio)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DAUDIOBUFFER_H__
#define __DAUDIOBUFFER_H__

#include "os2sndbuffer.h"
#include <vmutex.h>
#include "waveoutdaud.h"

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

// a forward declaration of some classes is needed here!
class IDirectAudioNotify;
class OS2IDirectSound3DBuffer;
class OS2IDirectSound;

class IDirectAudioBuffer
{
 public:
    //this one has to go first!
    IDirectSoundBufferVtbl *lpVtbl;
    IDirectSoundBufferVtbl  Vtbl;

    IDirectAudioBuffer(OS2IDirectSound *DSound, const DSBUFFERDESC *lpDSBufferDesc);
    IDirectAudioBuffer(OS2IDirectSound *DSound, IDirectAudioBuffer *srcBuf);
    IDirectAudioBuffer() {};
    ~IDirectAudioBuffer();

    static void DestroyAllBuffers(); // called when DSound is closed

    int     Referenced;
    inline  HRESULT       GetLastError()    { return lastError;    };
    inline  void          SetNotify(IDirectAudioNotify *_notify) {
                          notify = _notify; };
    inline  void          Set3DBuffer(OS2IDirectSound3DBuffer *_buffer3D) {
                          buffer3D = _buffer3D; };

    HRESULT QueryInterface(REFIID riid, LPVOID * ppvObj);
    ULONG   AddRef();
    ULONG   Release();
    HRESULT GetCaps(LPDSBCAPS );
    HRESULT GetCurrentPosition(LPDWORD,LPDWORD ) ;
    HRESULT GetFormat(LPWAVEFORMATEX, DWORD, LPDWORD ) ;
    HRESULT GetVolume(LPLONG ) ;
    HRESULT GetPan(LPLONG ) ;
    HRESULT GetFrequency(LPDWORD ) ;
    HRESULT GetStatus(LPDWORD ) ;
    HRESULT Initialize(LPDIRECTSOUND, LPDSBUFFERDESC ) ;
    HRESULT Lock(DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,DWORD ) ;
    HRESULT Play(DWORD,DWORD,DWORD ) ;
    HRESULT SetCurrentPosition(DWORD ) ;
    HRESULT SetFormat(LPWAVEFORMATEX ) ;
    HRESULT SetVolume(LONG ) ;
    HRESULT SetPan(LONG ) ;
    HRESULT SetFrequency( DWORD ) ;
    HRESULT Stop() ;
    HRESULT Unlock( LPVOID,DWORD,LPVOID,DWORD ) ;
    HRESULT Restore() ;

 private:

 protected:
    HRESULT lastError;

    ULONG writepos, playpos;   // DSound write and play cursors
    ULONG frequency;           // DSB frequency - may differ from source sample
    WAVEFORMATEX *lpfxFormat;  // source sample format
    ULONG status;              // DSB status - playing, looping etc.
    BOOL  fLocked;             // is buffer currently locked?
    BOOL  fPrimary;            // is this a primary buffer?
    LONG  DSvolume;            // DSB volume (in dB)
    LONG  volume;              // volume (linear)
    LONG  DSpan;               // DSB pan (in dB)
    LONG  pan;                 // pan (linear)
    BOOL  fPlaying,fLoop;      // is buffer playing/looping?
    DSBUFFERDESC bufferdesc;   // DSB description
    LONG  frac;                // saved mixer position overflow

    SOUNDBUF *lpSndBuffer;     // sound data + reference count
    char     *lpBuffer;        // actual sound data

    IDirectAudioNotify*   notify;    // pointer to the associated Notify object (if exists)
    OS2IDirectSound3DBuffer* buffer3D;  // pointer to the associated 3DBuffer object (if exists)
    OS2IDirectSound*         parentDS;

    DAudioWaveOut           *daudio;

    VMutex                   wmutex;

              // Linked list management
              IDirectAudioBuffer* next;                   // Next OS2IDirectSoundBuffer
    static    IDirectAudioBuffer* dsbroot;                // List of OS2IDirectSoundBuffers
    static    IDirectAudioBuffer* primary;                // The primary SoundBuffer
};
//******************************************************************************
//******************************************************************************
HRESULT WINAPI DAudioBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
ULONG   WINAPI DAudioBufAddRef(THIS);
ULONG   WINAPI DAudioBufRelease(THIS);
HRESULT WINAPI DAudioBufGetCaps(THIS_ LPDSBCAPS );
HRESULT WINAPI DAudioBufGetCurrentPosition(THIS_ LPDWORD,LPDWORD ) ;
HRESULT WINAPI DAudioBufGetFormat(THIS_ LPWAVEFORMATEX, DWORD, LPDWORD ) ;
HRESULT WINAPI DAudioBufGetVolume(THIS_ LPLONG ) ;
HRESULT WINAPI DAudioBufGetPan(THIS_ LPLONG ) ;
HRESULT WINAPI DAudioBufGetFrequency(THIS_ LPDWORD ) ;
HRESULT WINAPI DAudioBufGetStatus(THIS_ LPDWORD ) ;
HRESULT WINAPI DAudioBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC ) ;
HRESULT WINAPI DAudioBufLock(THIS_ DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,DWORD ) ;
HRESULT WINAPI DAudioBufPlay(THIS_ DWORD,DWORD,DWORD ) ;
HRESULT WINAPI DAudioBufSetCurrentPosition(THIS_ DWORD ) ;
HRESULT WINAPI DAudioBufSetFormat(THIS_ LPWAVEFORMATEX ) ;
HRESULT WINAPI DAudioBufSetVolume(THIS_ LONG ) ;
HRESULT WINAPI DAudioBufSetPan(THIS_ LONG ) ;
HRESULT WINAPI DAudioBufSetFrequency(THIS_ DWORD ) ;
HRESULT WINAPI DAudioBufStop(THIS  ) ;
HRESULT WINAPI DAudioBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD ) ;
HRESULT WINAPI DAudioBufRestore(THIS  ) ;

#endif
