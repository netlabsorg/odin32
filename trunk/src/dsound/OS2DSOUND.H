/* $Id: OS2DSOUND.H,v 1.6 2000-05-18 20:37:08 mike Exp $ */

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

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

// A forward declaration is need here!
class OS2PrimBuff;

class OS2IDirectSound
{
 public:
    //this one has to go first!
    IDirectSoundVtbl *lpVtbl;
    IDirectSoundVtbl  Vtbl;

    OS2IDirectSound(const GUID *lpGUID);
    ~OS2IDirectSound();

    int           Referenced;

    static  BOOL          fDSExists;
    inline  HRESULT       GetLastError()    { return lastError;    };
    inline  DWORD         GetCoopLevel()    { return CoopLevel;    };

    OS2PrimBuff*     primary;                // Primary Buffer..Created on construct of DSound (KL)
                                             // (Public so that the Secondary buffer instances can access this.. Maybe a GetPrimary function would eb better?)

 private:

 protected:
          HRESULT lastError;

          DWORD   speakerConfig;
          DWORD   CoopLevel;
          HWND    hwndClient;


              // Linked list management
              OS2IDirectSound* next;                   // Next OS2IDirectSound
    static    OS2IDirectSound* dsound;                 // List of OS2IDirectSound

    friend    HRESULT __stdcall SoundQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
    friend    ULONG   __stdcall SoundAddRef(THIS);
    friend    ULONG   __stdcall SoundRelease(THIS);
    friend    HRESULT __stdcall SoundCreateSoundBuffer(THIS_ LPDSBUFFERDESC, LPDIRECTSOUNDBUFFER*,LPUNKNOWN);
    friend    HRESULT __stdcall SoundGetCaps(THIS_ LPDSCAPS );
    friend    HRESULT __stdcall SoundDuplicateSoundBuffer(THIS_ LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER );
    friend    HRESULT __stdcall SoundSetCooperativeLevel(THIS_ HWND, DWORD );
    friend    HRESULT __stdcall SoundCompact(THIS );
    friend    HRESULT __stdcall SoundGetSpeakerConfig(THIS_ LPDWORD );
    friend    HRESULT __stdcall SoundSetSpeakerConfig(THIS_ DWORD );
    friend    HRESULT __stdcall SoundInitialize(THIS_ LPGUID);
};
//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
ULONG   __stdcall SoundAddRef(THIS);
ULONG   __stdcall SoundRelease(THIS);
HRESULT __stdcall SoundCreateSoundBuffer(THIS_ LPDSBUFFERDESC, LPDIRECTSOUNDBUFFER*,LPUNKNOWN);
HRESULT __stdcall SoundGetCaps(THIS_ LPDSCAPS );
HRESULT __stdcall SoundDuplicateSoundBuffer(THIS_ LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER );
HRESULT __stdcall SoundSetCooperativeLevel(THIS_ HWND, DWORD );
HRESULT __stdcall SoundCompact(THIS );
HRESULT __stdcall SoundGetSpeakerConfig(THIS_ LPDWORD );
HRESULT __stdcall SoundSetSpeakerConfig(THIS_ DWORD );
HRESULT __stdcall SoundInitialize(THIS_ LPGUID);

#endif
