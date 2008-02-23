/*
 * DirectSound OS2IDirectSound3DListener class
 *
 * Copyright 2000 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS23DLISTENER_H__
#define __OS23DLISTENER_H__

#undef THIS
#define THIS IDirectSound3DListener*This

#undef THIS_
#define THIS_ IDirectSound3DListener*This,

class OS2IDirectSound3DListener
{
 public:
    //this one has to go first!
    IDirectSound3DListenerVtbl *lpVtbl;
    IDirectSound3DListenerVtbl  Vtbl;

    OS2IDirectSound3DListener(OS2PrimBuff *parentBuffer);
    ~OS2IDirectSound3DListener();

    int     Referenced;

 private:

 protected:
    OS2PrimBuff                *lpSoundBuffer;   // parent sound buffer

    DS3DLISTENER               data3D;           // all the 3DListener data

    friend    HRESULT __stdcall Sound3DListenerQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
    friend    ULONG   __stdcall Sound3DListenerAddRef(THIS);
    friend    ULONG   __stdcall Sound3DListenerRelease(THIS);
    friend    HRESULT __stdcall Sound3DListenerGetAllParameters(THIS, LPDS3DLISTENER lpListener);
    friend    HRESULT __stdcall Sound3DListenerSetAllParameters(THIS, LPCDS3DLISTENER lpcListener, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DListenerCommitDeferredSettings(THIS);
    friend    HRESULT __stdcall Sound3DListenerGetDistanceFactor(THIS, LPD3DVALUE lpflDistanceFactor);
    friend    HRESULT __stdcall Sound3DListenerSetDistanceFactor(THIS, D3DVALUE flDistanceFactor, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DListenerGetDopplerFactor(THIS, LPD3DVALUE lpflDopplerFactor);
    friend    HRESULT __stdcall Sound3DListenerSetDopplerFactor(THIS, D3DVALUE flDopplerFactor, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DListenerGetOrientation(THIS, LPD3DVECTOR lpvOrientFront, LPD3DVECTOR lpvOrientTop);
    friend    HRESULT __stdcall Sound3DListenerSetOrientation(THIS, D3DVALUE xFront, D3DVALUE yFront, D3DVALUE zFront, D3DVALUE xTop, D3DVALUE yTop, D3DVALUE zTop, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DListenerGetPosition(THIS, LPD3DVECTOR lpvPosition);
    friend    HRESULT __stdcall Sound3DListenerSetPosition(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DListenerGetRolloffFactor(THIS, LPD3DVALUE lpflRolloffFactor);
    friend    HRESULT __stdcall Sound3DListenerSetRolloffFactor(THIS, D3DVALUE flRolloffFactor, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DListenerGetVelocity(THIS, LPD3DVECTOR lpvVelocity);
    friend    HRESULT __stdcall Sound3DListenerSetVelocity(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);
};

#endif
