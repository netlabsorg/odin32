/*
 * DirectSound main class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2DSOUND_H__
#define __OS2DSOUND_H__

/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectSound

class OS2IDirectSound
{
 public:
    //this one has to go first!
    IDirectSoundVtbl *lpVtbl;
    IDirectSoundVtbl  Vtbl;

    OS2IDirectSound(const GUID *lpGUID);
       ~OS2IDirectSound();

            int           Referenced;
        inline  HRESULT       GetLastError()    { return lastError;    };

 private:

 protected:
              HRESULT lastError;

          DWORD   speakerConfig;
          DWORD   CoopLevel;
          HWND    hwndClient;

              // Linked list management
              OS2IDirectSound* next;                   // Next OS2IDirectSound
    static    OS2IDirectSound* dsound;                 // List of OS2IDirectSound

    friend    HRESULT __stdcall SoundQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall SoundAddRef(THIS);
    friend    ULONG   __stdcall SoundRelease(THIS);
    #if 0 //KSO Apr 13 1999: some parameter incompabilities between newer and older SDKs.
       friend HRESULT __stdcall SoundCreateSoundBuffer(THIS_ LPDSBUFFERDESC, LPLPDIRECTSOUNDBUFFER, IUnknown FAR *);
    #else
       friend HRESULT __stdcall SoundCreateSoundBuffer(THIS_ LPCDSBUFFERDESC, LPDIRECTSOUNDBUFFER*,LPUNKNOWN);
    #endif
    friend    HRESULT __stdcall SoundGetCaps(THIS_ LPDSCAPS );
    friend    HRESULT __stdcall SoundDuplicateSoundBuffer(THIS_ LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER );
    friend    HRESULT __stdcall SoundSetCooperativeLevel(THIS_ W32_HWND, DWORD );
    friend    HRESULT __stdcall SoundCompact(THIS );
    friend    HRESULT __stdcall SoundGetSpeakerConfig(THIS_ LPDWORD );
    friend    HRESULT __stdcall SoundSetSpeakerConfig(THIS_ DWORD );
    #if 0 //KSO Apr 13 1999: parameter declaration has changed in later SDKs
       friend HRESULT __stdcall SoundInitialize(THIS_ const GUID * );
    #else
       friend HRESULT __stdcall SoundInitialize(THIS_ LPGUID);
    #endif
};
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall SoundAddRef(THIS);
ULONG   __stdcall SoundRelease(THIS);
#if 0 //KSO Apr 13 1999: some parameter incompabilities between newer and older SDKs.
    HRESULT __stdcall SoundCreateSoundBuffer(THIS_ LPDSBUFFERDESC, LPLPDIRECTSOUNDBUFFER, IUnknown FAR *);
#else
    HRESULT __stdcall SoundCreateSoundBuffer(THIS_ LPCDSBUFFERDESC, LPDIRECTSOUNDBUFFER*,LPUNKNOWN);
#endif
HRESULT __stdcall SoundGetCaps(THIS_ LPDSCAPS );
HRESULT __stdcall SoundDuplicateSoundBuffer(THIS_ LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER );
HRESULT __stdcall SoundSetCooperativeLevel(THIS_ W32_HWND, DWORD );
HRESULT __stdcall SoundCompact(THIS );
HRESULT __stdcall SoundGetSpeakerConfig(THIS_ LPDWORD );
HRESULT __stdcall SoundSetSpeakerConfig(THIS_ DWORD );
#if 0 //KSO Apr 13 1999: parameter declaration has changed in later SDKs
   HRESULT __stdcall SoundInitialize(THIS_ const GUID * );
#else
   HRESULT __stdcall SoundInitialize(THIS_ LPGUID);
#endif

#endif