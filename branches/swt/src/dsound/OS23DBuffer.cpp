/* $Id: OS23DBuffer.cpp,v 1.2 2000-08-02 15:48:26 bird Exp $ */
/*
 * DirectSound OS2IDirectSound3DBuffer class
 *
 * Copyright 2000 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSMISC
#include <os2win.h>

#include <stdlib.h>
#include <string.h>

#define CINTERFACE
#include <dsound.h>

#include "OS2DSound.h"
#include "OS2SndBuffer.h"
#include "OS2PrimBuff.h"
#include "OS23DBuffer.h"
#include <misc.h>

// Note: the DSound headers are inconsistent; some require THIS to be VOID*, some
//       require it to be a pointer to the appropriate interface.

#undef THIS
#define THIS IDirectSound3DBuffer*This

#undef THIS_
#define THIS_ IDirectSound3DBuffer*This,

//******************************************************************************
//******************************************************************************
OS2IDirectSound3DBuffer::OS2IDirectSound3DBuffer(OS2IDirectSoundBuffer *parentBuffer)
{
   lpVtbl = &Vtbl;
   Vtbl.fnAddRef                   = Sound3DBufferAddRef;
   Vtbl.fnRelease                  = Sound3DBufferRelease;
   Vtbl.fnQueryInterface           = Sound3DBufferQueryInterface;
   Vtbl.fnGetAllParameters         = Sound3DBufferGetAllParameters;
   Vtbl.fnSetAllParameters         = Sound3DBufferSetAllParameters;
   Vtbl.fnGetMaxDistance           = Sound3DBufferGetMaxDistance;
   Vtbl.fnSetMaxDistance           = Sound3DBufferSetMaxDistance;
   Vtbl.fnGetMinDistance           = Sound3DBufferGetMinDistance;
   Vtbl.fnSetMinDistance           = Sound3DBufferSetMinDistance;
   Vtbl.fnGetMode                  = Sound3DBufferGetMode;
   Vtbl.fnSetMode                  = Sound3DBufferSetMode;
   Vtbl.fnGetPosition              = Sound3DBufferGetPosition;
   Vtbl.fnSetPosition              = Sound3DBufferSetPosition;
   Vtbl.fnGetConeAngles            = Sound3DBufferGetConeAngles;
   Vtbl.fnSetConeAngles            = Sound3DBufferSetConeAngles;
   Vtbl.fnGetConeOrientation       = Sound3DBufferGetConeOrientation;
   Vtbl.fnSetConeOrientation       = Sound3DBufferSetConeOrientation;
   Vtbl.fnGetConeOutsideVolume     = Sound3DBufferGetConeOutsideVolume;
   Vtbl.fnSetConeOutsideVolume     = Sound3DBufferSetConeOutsideVolume;
   Vtbl.fnGetVelocity              = Sound3DBufferGetVelocity;
   Vtbl.fnSetVelocity              = Sound3DBufferSetVelocity;


   dprintf(("DSOUND-OS2IDirectSound3DBuffer::OS2IDirectSound3DBuffer (this=%X)", this));

   lpSoundBuffer = parentBuffer;
   Referenced    = 0;

   memset(&data3D, 0, sizeof(DS3DBUFFER));
   data3D.dwSize = sizeof(DS3DBUFFER);

   // add a reference to the parent primary SoundBuffer to make sure it won't suddenly disappear
   lpSoundBuffer->Vtbl.fnAddRef(lpSoundBuffer);
   // set pointer to ourselves in parent SoundBuffer
   lpSoundBuffer->Set3DBuffer(this);
}

//******************************************************************************
//******************************************************************************
OS2IDirectSound3DBuffer::~OS2IDirectSound3DBuffer()
{
   dprintf(("DSOUND-OS2IDirectSound3DBuffer::~OS2IDirectSound3DBuffer (this=%X)", this));
   lpSoundBuffer->Set3DBuffer(NULL);
   lpSoundBuffer->Vtbl.fnRelease(lpSoundBuffer);
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-OS2IDirectSound3DBuffer::QueryInterface"));
   if (This == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *ppvObj = NULL;

   if (!IsEqualGUID(riid, &IID_IDirectSound3DBuffer))
      return E_NOINTERFACE;

   *ppvObj = This;

   Sound3DBufferAddRef(This);
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall Sound3DBufferAddRef(THIS)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::AddRef (this=%X) %d", me, me->Referenced+1));
   if (me == NULL) {
      return 0;
   }
   return ++me->Referenced;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall Sound3DBufferRelease(THIS)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Release (this=%X) %d", me, me->Referenced-1));
   if (me == NULL) {
      return 0;
   }
   if (me->Referenced) {
      me->Referenced--;
      if (me->Referenced == 0) {
         delete me;
         return DS_OK;
      }
      else
         return me->Referenced;
   }
   else
      return DS_OK;
}

//******************************************************************************
//******************************************************************************
void RecalcNow(OS2IDirectSound3DBuffer *me)
{
   dprintf(("DSOUND-3DBuffer: RecalcNow (this=%X)", me));

   // helper function - recalculate all 3D parameters
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetAllParameters(THIS, LPDS3DBUFFER lpDs3dBuffer)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetAllParameters (this=%X)", me));
   if (me == NULL || lpDs3dBuffer == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpDs3dBuffer = me->data3D;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetAllParameters(THIS, LPCDS3DBUFFER lpcDs3dBuffer, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetAllParameters (this=%X)", me));
   if (me == NULL || lpcDs3dBuffer == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D = *lpcDs3dBuffer;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetMaxDistance(THIS, LPD3DVALUE lpflMaxDistance)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetMaxDistance (this=%X)", me));
   if (me == NULL || lpflMaxDistance == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpflMaxDistance = me->data3D.flMaxDistance;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetMaxDistance(THIS, D3DVALUE flMaxDistance, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetMaxDistance (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.flMaxDistance = flMaxDistance;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetMinDistance(THIS, LPD3DVALUE lpflMinDistance)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetMinDistance (this=%X)", me));
   if (me == NULL || lpflMinDistance == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpflMinDistance = me->data3D.flMinDistance;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetMinDistance(THIS, D3DVALUE flMinDistance, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetMinDistance (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.flMinDistance = flMinDistance;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetMode(THIS, LPDWORD lpdwMode)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetMode (this=%X)", me));
   if (me == NULL || lpdwMode == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpdwMode = me->data3D.dwMode;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetMode(THIS, DWORD dwMode, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetMode (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.dwMode = dwMode;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetPosition(THIS, LPD3DVECTOR lpvPosition)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetPosition (this=%X)", me));
   if (me == NULL || lpvPosition == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpvPosition = me->data3D.vPosition;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetPosition(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetPosition (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.vPosition.x.x = x;
   me->data3D.vPosition.y.y = y;
   me->data3D.vPosition.z.z = z;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetVelocity(THIS, LPD3DVECTOR lpvVelocity)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetVelocity (this=%X)", me));
   if (me == NULL || lpvVelocity == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpvVelocity = me->data3D.vVelocity;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetVelocity(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetVelocity (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.vVelocity.x.x = x;
   me->data3D.vVelocity.y.y = y;
   me->data3D.vVelocity.z.z = z;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetConeOrientation(THIS, LPD3DVECTOR lpvConeOrientation)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetConeOrientation (this=%X)", me));
   if (me == NULL || lpvConeOrientation == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpvConeOrientation = me->data3D.vConeOrientation;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetConeOrientation(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetConeOrientation (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.vConeOrientation.x.x = x;
   me->data3D.vConeOrientation.y.y = y;
   me->data3D.vConeOrientation.z.z = z;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetConeAngles(THIS, LPDWORD lpdwInsideConeAngle, LPDWORD lpdwOutsideConeAngle)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetConeOrientation (this=%X)", me));
   if (me == NULL || lpdwInsideConeAngle == NULL || lpdwOutsideConeAngle == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpdwInsideConeAngle  = me->data3D.dwInsideConeAngle;
   *lpdwOutsideConeAngle = me->data3D.dwOutsideConeAngle;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetConeAngles(THIS, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetConeOrientation (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.dwInsideConeAngle  = dwInsideConeAngle;
   me->data3D.dwOutsideConeAngle = dwOutsideConeAngle;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferGetConeOutsideVolume(THIS, LPLONG lplConeOutsideVolume)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferGetConeOrientation (this=%X)", me));
   if (me == NULL || lplConeOutsideVolume == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lplConeOutsideVolume  = me->data3D.lConeOutsideVolume;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DBufferSetConeOutsideVolume(THIS, LONG lConeOutsideVolume, DWORD dwApply)
{
   OS2IDirectSound3DBuffer *me = (OS2IDirectSound3DBuffer *)This;

   dprintf(("DSOUND-OS2IDirectSound3DBuffer::Sound3DBufferSetConeOrientation (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.lConeOutsideVolume  = lConeOutsideVolume;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

