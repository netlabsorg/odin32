/* $Id: OS2SNDBUFFER.H,v 1.8 2001-03-10 06:21:06 mike Exp $ */

/*
 * DirectSound SoundBuffer class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2SNDBUFFER_H__
#define __OS2SNDBUFFER_H__

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

// simple structure to handle duplicated buffers - the first DWORD of
// a sound buffer contains the buffer's usage count (to prevent freeing
// of buffers still in use)
typedef struct _SOUNDBUF {
    DWORD   dwReferences;
    char    pData[1];
} SOUNDBUF;

// a forward declaration of some classes is needed here!
class OS2IDirectSoundNotify;
class OS2IDirectSound3DBuffer;
class OS2IDirectSound;

class OS2IDirectSoundBuffer
{
 public:
    //this one has to go first!
    IDirectSoundBufferVtbl *lpVtbl;
    IDirectSoundBufferVtbl  Vtbl;

    OS2IDirectSoundBuffer(OS2IDirectSound *DSound, const DSBUFFERDESC *lpDSBufferDesc);
    OS2IDirectSoundBuffer(OS2IDirectSound *DSound, OS2IDirectSoundBuffer *srcBuf);
    OS2IDirectSoundBuffer() {};
    ~OS2IDirectSoundBuffer();

    static void OS2IDirectSoundBuffer::DestroyAllBuffers(); // called when DSound is closed

    int     Referenced;
    inline  HRESULT       GetLastError()    { return lastError;    };
    inline  void          SetNotify(OS2IDirectSoundNotify *_notify) {
                          notify = _notify; };
    inline  void          Set3DBuffer(OS2IDirectSound3DBuffer *_buffer3D) {
                          buffer3D = _buffer3D; };
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

    OS2IDirectSoundNotify*   notify;    // pointer to the associated Notify object (if exists)
    OS2IDirectSound3DBuffer* buffer3D;  // pointer to the associated 3DBuffer object (if exists)
    OS2IDirectSound*         parentDS;

              // Linked list management
              OS2IDirectSoundBuffer* next;                   // Next OS2IDirectSoundBuffer
    static    OS2IDirectSoundBuffer* dsbroot;                // List of OS2IDirectSoundBuffers
    static    OS2IDirectSoundBuffer* primary;                // The primary SoundBuffer

    friend    HRESULT __stdcall SoundBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
    friend    ULONG   __stdcall SoundBufAddRef(THIS);
    friend    ULONG   __stdcall SoundBufRelease(THIS);
    friend    HRESULT __stdcall SoundBufGetCaps(THIS_ LPDSBCAPS );
    friend    HRESULT __stdcall SoundBufGetCurrentPosition(THIS_ LPDWORD,LPDWORD ) ;
    friend    HRESULT __stdcall SoundBufGetFormat(THIS_ LPWAVEFORMATEX, DWORD, LPDWORD ) ;
    friend    HRESULT __stdcall SoundBufGetVolume(THIS_ LPLONG ) ;
    friend    HRESULT __stdcall SoundBufGetPan(THIS_ LPLONG ) ;
    friend    HRESULT __stdcall SoundBufGetFrequency(THIS_ LPDWORD ) ;
    friend    HRESULT __stdcall SoundBufGetStatus(THIS_ LPDWORD ) ;
    friend    HRESULT __stdcall SoundBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC ) ;
    friend    HRESULT __stdcall SoundBufLock(THIS_ DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,DWORD ) ;
    friend    HRESULT __stdcall SoundBufPlay(THIS_ DWORD,DWORD,DWORD ) ;
    friend    HRESULT __stdcall SoundBufSetCurrentPosition(THIS_ DWORD ) ;
    friend    HRESULT __stdcall SoundBufSetFormat(THIS_ LPWAVEFORMATEX ) ;
    friend    HRESULT __stdcall SoundBufSetVolume(THIS_ LONG ) ;
    friend    HRESULT __stdcall SoundBufSetPan(THIS_ LONG ) ;
    friend    HRESULT __stdcall SoundBufSetFrequency(THIS_ DWORD ) ;
    friend    HRESULT __stdcall SoundBufStop(THIS  ) ;
    friend    HRESULT __stdcall SoundBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD ) ;
    friend    HRESULT __stdcall SoundBufRestore(THIS  ) ;

    friend    void MixCallback(ULONG cbMix);
    friend    void MixFunc (OS2IDirectSoundBuffer *firstBuf, OS2IDirectSoundBuffer *outBuf, ULONG cbMix);
    friend    void MixOneBuffer(OS2IDirectSoundBuffer *inBuf, int tomix, int outrate);
};
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
ULONG   __stdcall SoundBufAddRef(THIS);
ULONG   __stdcall SoundBufRelease(THIS);
HRESULT __stdcall SoundBufGetCaps(THIS_ LPDSBCAPS );
HRESULT __stdcall SoundBufGetCurrentPosition(THIS_ LPDWORD,LPDWORD ) ;
HRESULT __stdcall SoundBufGetFormat(THIS_ LPWAVEFORMATEX, DWORD, LPDWORD ) ;
HRESULT __stdcall SoundBufGetVolume(THIS_ LPLONG ) ;
HRESULT __stdcall SoundBufGetPan(THIS_ LPLONG ) ;
HRESULT __stdcall SoundBufGetFrequency(THIS_ LPDWORD ) ;
HRESULT __stdcall SoundBufGetStatus(THIS_ LPDWORD ) ;
HRESULT __stdcall SoundBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC ) ;
HRESULT __stdcall SoundBufLock(THIS_ DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,DWORD ) ;
HRESULT __stdcall SoundBufPlay(THIS_ DWORD,DWORD,DWORD ) ;
HRESULT __stdcall SoundBufSetCurrentPosition(THIS_ DWORD ) ;
HRESULT __stdcall SoundBufSetFormat(THIS_ LPWAVEFORMATEX ) ;
HRESULT __stdcall SoundBufSetVolume(THIS_ LONG ) ;
HRESULT __stdcall SoundBufSetPan(THIS_ LONG ) ;
HRESULT __stdcall SoundBufSetFrequency(THIS_ DWORD ) ;
HRESULT __stdcall SoundBufStop(THIS  ) ;
HRESULT __stdcall SoundBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD ) ;
HRESULT __stdcall SoundBufRestore(THIS  ) ;

#endif
