/* $Id: OS2SNDBUFFER.H,v 1.4 1999-10-23 23:00:50 sandervl Exp $ */

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

#include <mmsystem.h>

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

class OS2IDirectSoundBuffer
{
 public:
    //this one has to go first!
    IDirectSoundBufferVtbl *lpVtbl;
    IDirectSoundBufferVtbl  Vtbl;

    OS2IDirectSoundBuffer(const DSBUFFERDESC *lpDSBufferDesc);//KSO Apr 13 1999: const, SDK changes
    ~OS2IDirectSoundBuffer();

    int     Referenced;
    inline  HRESULT       GetLastError()    { return lastError;    };

 private:

 protected:
    HRESULT lastError;

    LONG  pan;
    ULONG writepos, playpos;
    ULONG frequency;
    WAVEFORMATEX *lpfxFormat;
    ULONG status;
    ULONG fLocked;
    LONG  volume;
    DSBUFFERDESC bufferdesc;
    char *lpBuffer;

              // Linked list management
              OS2IDirectSoundBuffer* next;                   // Next OS2IDirectSound
    static    OS2IDirectSoundBuffer* dsound;                 // List of OS2IDirectSound

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
