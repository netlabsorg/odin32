/*
 * DirectSound OS2IDirectSound3DBuffer class
 *
 * Copyright 2000 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS23DBUFFER_H__
#define __OS23DBUFFER_H__

#undef THIS
#define THIS IDirectSound3DBuffer*This

#undef THIS_
#define THIS_ IDirectSound3DBuffer*This,

class OS2IDirectSound3DBuffer
{
 public:
    //this one has to go first!
    IDirectSound3DBufferVtbl *lpVtbl;
    IDirectSound3DBufferVtbl  Vtbl;

    OS2IDirectSound3DBuffer(OS2IDirectSoundBuffer *parentBuffer);
    ~OS2IDirectSound3DBuffer();

    int     Referenced;

 private:

 protected:
    OS2IDirectSoundBuffer     *lpSoundBuffer;   // parent sound buffer

    DS3DBUFFER                data3D;           // all the 3DBuffer data

    friend    HRESULT __stdcall Sound3DBufferQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
    friend    ULONG   __stdcall Sound3DBufferAddRef(THIS);
    friend    ULONG   __stdcall Sound3DBufferRelease(THIS);
    friend    HRESULT __stdcall Sound3DBufferGetAllParameters(THIS, LPDS3DBUFFER lpDs3dBuffer);
    friend    HRESULT __stdcall Sound3DBufferSetAllParameters(THIS, LPCDS3DBUFFER lpcDs3dBuffer, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferGetMaxDistance(THIS, LPD3DVALUE lpflMaxDistance);
    friend    HRESULT __stdcall Sound3DBufferGetMinDistance(THIS, LPD3DVALUE lpflMinDistance);
    friend    HRESULT __stdcall Sound3DBufferSetMaxDistance(THIS, D3DVALUE flMaxDistance, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferSetMinDistance(THIS, D3DVALUE flMinDistance, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferGetMode(THIS, LPDWORD lpdwMode);
    friend    HRESULT __stdcall Sound3DBufferSetMode(THIS, DWORD dwMode, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferGetPosition(THIS, LPD3DVECTOR lpvPosition);
    friend    HRESULT __stdcall Sound3DBufferSetPosition(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferGetConeAngles(THIS, LPDWORD lpdwInsideConeAngle, LPDWORD lpdwOutsideConeAngle);
    friend    HRESULT __stdcall Sound3DBufferSetConeAngles(THIS, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferGetConeOrientation(THIS, LPD3DVECTOR lpvOrientation);
    friend    HRESULT __stdcall Sound3DBufferSetConeOrientation(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferGetConeOutsideVolume(THIS, LPLONG lplConeOutsideVolume);
    friend    HRESULT __stdcall Sound3DBufferSetConeOutsideVolume(THIS, LONG lConeOutsideVolume, DWORD dwApply);
    friend    HRESULT __stdcall Sound3DBufferGetVelocity(THIS, LPD3DVECTOR lpvVelocity);
    friend    HRESULT __stdcall Sound3DBufferSetVelocity(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);
};

HRESULT __stdcall Sound3DBufferQueryInterface(THIS, REFIID riid, LPVOID * ppvObj);
ULONG   __stdcall Sound3DBufferAddRef(THIS);
ULONG   __stdcall Sound3DBufferRelease(THIS);
HRESULT __stdcall Sound3DBufferGetAllParameters(THIS, LPDS3DBUFFER lpDs3dBuffer);
HRESULT __stdcall Sound3DBufferSetAllParameters(THIS, LPCDS3DBUFFER lpcDs3dBuffer, DWORD dwApply);
HRESULT __stdcall Sound3DBufferGetMaxDistance(THIS, LPD3DVALUE lpflMaxDistance);
HRESULT __stdcall Sound3DBufferGetMinDistance(THIS, LPD3DVALUE lpflMinDistance);
HRESULT __stdcall Sound3DBufferSetMaxDistance(THIS, D3DVALUE flMaxDistance, DWORD dwApply);
HRESULT __stdcall Sound3DBufferSetMinDistance(THIS, D3DVALUE flMinDistance, DWORD dwApply);
HRESULT __stdcall Sound3DBufferGetMode(THIS, LPDWORD lpdwMode);
HRESULT __stdcall Sound3DBufferSetMode(THIS, DWORD dwMode, DWORD dwApply);
HRESULT __stdcall Sound3DBufferGetPosition(THIS, LPD3DVECTOR lpvPosition);
HRESULT __stdcall Sound3DBufferSetPosition(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);
HRESULT __stdcall Sound3DBufferGetConeAngles(THIS, LPDWORD lpdwInsideConeAngle, LPDWORD lpdwOutsideConeAngle);
HRESULT __stdcall Sound3DBufferSetConeAngles(THIS, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply);
HRESULT __stdcall Sound3DBufferGetConeOrientation(THIS, LPD3DVECTOR lpvOrientation);
HRESULT __stdcall Sound3DBufferSetConeOrientation(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);
HRESULT __stdcall Sound3DBufferGetConeOutsideVolume(THIS, LPLONG lplConeOutsideVolume);
HRESULT __stdcall Sound3DBufferSetConeOutsideVolume(THIS, LONG lConeOutsideVolume, DWORD dwApply);
HRESULT __stdcall Sound3DBufferGetVelocity(THIS, LPD3DVECTOR lpvVelocity);
HRESULT __stdcall Sound3DBufferSetVelocity(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply);

#endif
