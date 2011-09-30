/*
 * DirectSound DirectSoundNotify class
 *
 * Copyright 2000 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DAUDIONOTIFY_H__
#define __DAUDIONOTIFY_H__

#undef THIS
#define THIS VOID*This

#undef THIS_
#define THIS_ VOID*This,

class IDirectAudioNotify
{
 public:
    //this one has to go first!
    IDirectSoundNotifyVtbl *lpVtbl;
    IDirectSoundNotifyVtbl  Vtbl;

    IDirectAudioNotify(IDirectAudioBuffer *parentBuffer);
    ~IDirectAudioNotify();

    int     Referenced;

    // helper functions to handle notify event signaling
    void    CheckPos(DWORD dwOldpos, DWORD dwNewpos);
    void    CheckStop();

 private:

 protected:
    IDirectAudioBuffer        *lpSoundBuffer;   // parent sound buffer
    DWORD                      cPositions;       // # of notification positions
    LPDSBPOSITIONNOTIFY        lpPositions;      // notify positions and associated
                                                 // event object handles

    friend    HRESULT __stdcall DAudioNotifyQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
    friend    ULONG   __stdcall DAudioNotifyAddRef(THIS);
    friend    ULONG   __stdcall DAudioNotifyRelease(THIS);
    friend    HRESULT __stdcall DAudioNotifySetNotificationPositions(THIS, DWORD cPositionNotifies, LPCDSBPOSITIONNOTIFY lpcPositionNotifies);
};

#endif //__DAUDIONOTIFY_H__
