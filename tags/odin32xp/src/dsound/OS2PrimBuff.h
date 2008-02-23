/*
 * Direct Sound Primary Buffer Implemetation for ODIN
 *
 * Kevin Langman (langman@earthling.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __OS2PRIMBUFF_H__
#define __OS2PRIMBUFF_H__

#include <mmsystem.h>

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

// a forward declaration is needed here
class OS2IDirectSound3DListener;

class OS2PrimBuff: public OS2IDirectSoundBuffer
{
 public:

    OS2PrimBuff(OS2IDirectSound *DSound, const DSBUFFERDESC *lpDSBufferDesc);
    ~OS2PrimBuff();

    int     Referenced;
    inline  HRESULT       GetLastError()    { return lastError;    };
    inline  void          Set3DListener(OS2IDirectSound3DListener *_listener) {
                                            listener = _listener; };

 private:
 protected:

    void *vpMixBuffer;
    void *vpMixSetup;
    void *vpBuffParms;
    //void *vpGenericParms;

    USHORT usDeviceID;

    long  rate;    // Samples per second
    WORD  bps;     // Bits per sample 8 or 16 !!
    WORD  channels;// Number of Channels 1 or 2 MONO/Stereo

    OS2IDirectSound3DListener* listener;  // pointer to the associated 3DListener object

    HRESULT lastError;

    friend    HRESULT __stdcall PrimBufQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
    friend    ULONG   __stdcall PrimBufAddRef(THIS);
    friend    ULONG   __stdcall PrimBufRelease(THIS);
    friend    HRESULT __stdcall PrimBufGetCaps(THIS_ LPDSBCAPS );
    friend    HRESULT __stdcall PrimBufGetCurrentPosition(THIS_ LPDWORD,LPDWORD ) ;
    friend    HRESULT __stdcall PrimBufGetFormat(THIS_ LPWAVEFORMATEX, DWORD, LPDWORD ) ;
    friend    HRESULT __stdcall PrimBufGetVolume(THIS_ LPLONG ) ;
    friend    HRESULT __stdcall PrimBufGetPan(THIS_ LPLONG ) ;
    friend    HRESULT __stdcall PrimBufGetFrequency(THIS_ LPDWORD ) ;
    friend    HRESULT __stdcall PrimBufGetStatus(THIS_ LPDWORD ) ;
    friend    HRESULT __stdcall PrimBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC ) ;
    friend    HRESULT __stdcall PrimBufLock(THIS_ DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,DWORD ) ;
    friend    HRESULT __stdcall PrimBufPlay(THIS_ DWORD,DWORD,DWORD ) ;
    friend    HRESULT __stdcall PrimBufSetCurrentPosition(THIS_ DWORD ) ;
    friend    HRESULT __stdcall PrimBufSetFormat(THIS_ LPWAVEFORMATEX ) ;
    friend    HRESULT __stdcall PrimBufSetVolume(THIS_ LONG ) ;
    friend    HRESULT __stdcall PrimBufSetPan(THIS_ LONG ) ;
    friend    HRESULT __stdcall PrimBufSetFrequency(THIS_ DWORD ) ;
    friend    HRESULT __stdcall PrimBufStop(THIS  ) ;
    friend    HRESULT __stdcall PrimBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD ) ;
    friend    HRESULT __stdcall PrimBufRestore(THIS  ) ;
};

HRESULT __stdcall PrimBufGetCaps(THIS_ LPDSBCAPS );
HRESULT __stdcall PrimBufGetCurrentPosition(THIS_ LPDWORD,LPDWORD ) ;
HRESULT __stdcall PrimBufGetFormat(THIS_ LPWAVEFORMATEX, DWORD, LPDWORD ) ;
HRESULT __stdcall PrimBufGetVolume(THIS_ LPLONG ) ;
HRESULT __stdcall PrimBufGetPan(THIS_ LPLONG ) ;
HRESULT __stdcall PrimBufGetFrequency(THIS_ LPDWORD ) ;
HRESULT __stdcall PrimBufGetStatus(THIS_ LPDWORD ) ;
HRESULT __stdcall PrimBufInitialize(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC ) ;
HRESULT __stdcall PrimBufLock(THIS_ DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,DWORD ) ;
HRESULT __stdcall PrimBufPlay(THIS_ DWORD,DWORD,DWORD ) ;
HRESULT __stdcall PrimBufSetCurrentPosition(THIS_ DWORD ) ;
HRESULT __stdcall PrimBufSetFormat(THIS_ LPWAVEFORMATEX ) ;
HRESULT __stdcall PrimBufSetVolume(THIS_ LONG ) ;
HRESULT __stdcall PrimBufSetPan(THIS_ LONG ) ;
HRESULT __stdcall PrimBufSetFrequency(THIS_ DWORD ) ;
HRESULT __stdcall PrimBufStop(THIS  ) ;
HRESULT __stdcall PrimBufUnlock(THIS_ LPVOID,DWORD,LPVOID,DWORD ) ;
HRESULT __stdcall PrimBufRestore(THIS  ) ;

#endif
