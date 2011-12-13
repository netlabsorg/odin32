/*
 * DirectSound DirectSoundNotify class
 *
 * Copyright 2000 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2NOTIFY_H__
#define __OS2NOTIFY_H__

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

class OS2IDirectSoundNotify
{
 public:
    //this one has to go first!
    IDirectSoundNotifyVtbl *lpVtbl;
    IDirectSoundNotifyVtbl  Vtbl;

    OS2IDirectSoundNotify(OS2IDirectSoundBuffer *parentBuffer);
    ~OS2IDirectSoundNotify();

    int     Referenced;

    // helper functions to handle notify event signaling
    void    CheckPos(DWORD dwOldpos, DWORD dwNewpos);
    void    CheckStop();

 private:

 protected:
    OS2IDirectSoundBuffer      *lpSoundBuffer;   // parent sound buffer
    DWORD                      cPositions;       // # of notification positions
    LPDSBPOSITIONNOTIFY        lpPositions;      // notify positions and associated
                                                 // event object handles

    friend    HRESULT __stdcall SoundNotifyQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
    friend    ULONG   __stdcall SoundNotifyAddRef(THIS);
    friend    ULONG   __stdcall SoundNotifyRelease(THIS);
    friend    HRESULT __stdcall SoundNotifySetNotificationPositions(THIS, DWORD cPositionNotifies, LPCDSBPOSITIONNOTIFY lpcPositionNotifies);
};

HRESULT __stdcall SoundNotifyQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
ULONG   __stdcall SoundNotifyAddRef(THIS);
ULONG   __stdcall SoundNotifyRelease(THIS);
HRESULT __stdcall SoundNotifySetNotificationPositions(THIS, DWORD cPositionNotifies, LPCDSBPOSITIONNOTIFY lpcPositionNotifies);

#endif
