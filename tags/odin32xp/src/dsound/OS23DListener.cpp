/* $Id: OS23DListener.cpp,v 1.2 2000-08-02 15:48:26 bird Exp $ */
/*
 * DirectSound OS2IDirectSound3DListener class
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

#define INITGUID
#include <dsound.h>

#include "OS2DSound.h"
#include "OS2SndBuffer.h"
#include "OS2PrimBuff.h"
#include "OS23DListener.h"
#include <misc.h>

// Note: the DSound headers are inconsistent; some require THIS to be VOID*, some
//       require it to be a pointer to the appropriate interface.

#undef THIS
#define THIS IDirectSound3DListener*This

#undef THIS_
#define THIS_ IDirectSound3DListener*This,

//******************************************************************************
//******************************************************************************
OS2IDirectSound3DListener::OS2IDirectSound3DListener(OS2PrimBuff *parentBuffer)
{
   lpVtbl = &Vtbl;
   Vtbl.AddRef                   = Sound3DListenerAddRef;
   Vtbl.Release                  = Sound3DListenerRelease;
   Vtbl.QueryInterface           = Sound3DListenerQueryInterface;
   Vtbl.GetAllParameters         = Sound3DListenerGetAllParameters;
   Vtbl.SetAllParameters         = Sound3DListenerSetAllParameters;
   Vtbl.CommitDeferredSettings   = Sound3DListenerCommitDeferredSettings;
   Vtbl.GetDistanceFactor        = Sound3DListenerGetDistanceFactor;
   Vtbl.SetDistanceFactor        = Sound3DListenerSetDistanceFactor;
   Vtbl.GetDopplerFactor         = Sound3DListenerGetDopplerFactor;
   Vtbl.SetDopplerFactor         = Sound3DListenerSetDopplerFactor;
   Vtbl.GetOrientation           = Sound3DListenerGetOrientation;
   Vtbl.SetOrientation           = Sound3DListenerSetOrientation;
   Vtbl.GetPosition              = Sound3DListenerGetPosition;
   Vtbl.SetPosition              = Sound3DListenerSetPosition;
   Vtbl.GetRolloffFactor         = Sound3DListenerGetRolloffFactor;
   Vtbl.SetRolloffFactor         = Sound3DListenerSetRolloffFactor;
   Vtbl.GetVelocity              = Sound3DListenerGetVelocity;
   Vtbl.SetVelocity              = Sound3DListenerSetVelocity;


   dprintf(("DSOUND-OS2IDirectSound3DListener::OS2IDirectSound3DListener (this=%X)", this));

   lpSoundBuffer = parentBuffer;
   Referenced    = 0;

   memset(&data3D, 0, sizeof(DS3DLISTENER));
   data3D.dwSize = sizeof(DS3DLISTENER);

   // add a reference to the parent primary SoundBuffer to make sure it won't suddenly disappear
   lpSoundBuffer->Vtbl.AddRef(lpSoundBuffer);
   // set pointer to ourselves in parent SoundBuffer
   lpSoundBuffer->Set3DListener(this);
}

//******************************************************************************
//******************************************************************************
OS2IDirectSound3DListener::~OS2IDirectSound3DListener()
{
   dprintf(("DSOUND-OS2IDirectSound3DListener::~OS2IDirectSound3DListener (this=%X)", this));
   lpSoundBuffer->Set3DListener(NULL);
   lpSoundBuffer->Vtbl.Release(lpSoundBuffer);
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-OS2IDirectSound3DListener::QueryInterface"));
   if (This == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *ppvObj = NULL;

   if (!IsEqualGUID(riid, IID_IDirectSound3DListener))
      return E_NOINTERFACE;

   *ppvObj = This;

   Sound3DListenerAddRef(This);
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall Sound3DListenerAddRef(THIS)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::AddRef (this=%X) %d", me, me->Referenced+1));
   if (me == NULL) {
      return 0;
   }
   return ++me->Referenced;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall Sound3DListenerRelease(THIS)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Release (this=%X) %d", me, me->Referenced-1));
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
void RecalcNow(OS2IDirectSound3DListener *me)
{
   dprintf(("DSOUND-3DListener: RecalcNow (this=%X)", me));

   // helper function - recalculate all 3D parameters
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerGetAllParameters(THIS, LPDS3DLISTENER lpListener)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerGetAllParameters (this=%X)", me));
   if (me == NULL || lpListener == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpListener = me->data3D;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerSetAllParameters(THIS, LPCDS3DLISTENER lpcListener, DWORD dwApply)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerSetAllParameters (this=%X)", me));
   if (me == NULL || lpcListener == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D = *lpcListener;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerCommitDeferredSettings(THIS)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerCommitDeferredSettings (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   RecalcNow(me);

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerGetDistanceFactor(THIS, LPD3DVALUE lpflDistanceFactor)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerGetDistanceFactor (this=%X)", me));
   if (me == NULL || lpflDistanceFactor == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpflDistanceFactor = me->data3D.flDistanceFactor;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerSetDistanceFactor(THIS, D3DVALUE flDistanceFactor, DWORD dwApply)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerSetDistanceFactor (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.flDistanceFactor = flDistanceFactor;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerGetDopplerFactor(THIS, LPD3DVALUE lpflDopplerFactor)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerGetDopplerFactor (this=%X)", me));
   if (me == NULL || lpflDopplerFactor == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpflDopplerFactor = me->data3D.flDopplerFactor;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerSetDopplerFactor(THIS, D3DVALUE flDopplerFactor, DWORD dwApply)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerSetDopplerFactor (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.flDopplerFactor = flDopplerFactor;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerGetRolloffFactor(THIS, LPD3DVALUE lpflRolloffFactor)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerGetRolloffFactor (this=%X)", me));
   if (me == NULL || lpflRolloffFactor == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpflRolloffFactor = me->data3D.flRolloffFactor;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerSetRolloffFactor(THIS, D3DVALUE flRolloffFactor, DWORD dwApply)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerSetRolloffFactor (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.flRolloffFactor = flRolloffFactor;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerGetPosition(THIS, LPD3DVECTOR lpvPosition)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerGetPosition (this=%X)", me));
   if (me == NULL || lpvPosition == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpvPosition = me->data3D.vPosition;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerSetPosition(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerSetPosition (this=%X)", me));
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
HRESULT __stdcall Sound3DListenerGetVelocity(THIS, LPD3DVECTOR lpvVelocity)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerGetVelocity (this=%X)", me));
   if (me == NULL || lpvVelocity == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpvVelocity = me->data3D.vVelocity;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerSetVelocity(THIS, D3DVALUE x, D3DVALUE y, D3DVALUE z, DWORD dwApply)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerSetVelocity (this=%X)", me));
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
HRESULT __stdcall Sound3DListenerGetOrientation(THIS, LPD3DVECTOR lpvOrientFront, LPD3DVECTOR lpvOrientTop)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerGetOrientation (this=%X)", me));
   if (me == NULL || lpvOrientFront == NULL || lpvOrientTop == NULL) {
      return DSERR_INVALIDPARAM;
   }

   *lpvOrientFront = me->data3D.vOrientFront;
   *lpvOrientTop   = me->data3D.vOrientTop;

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall Sound3DListenerSetOrientation(THIS, D3DVALUE xFront, D3DVALUE yFront, D3DVALUE zFront, D3DVALUE xTop, D3DVALUE yTop, D3DVALUE zTop, DWORD dwApply)
{
   OS2IDirectSound3DListener *me = (OS2IDirectSound3DListener *)This;

   dprintf(("DSOUND-OS2IDirectSound3DListener::Sound3DListenerSetOrientation (this=%X)", me));
   if (me == NULL) {
      return DSERR_INVALIDPARAM;
   }

   me->data3D.vOrientFront.x.x = xFront;
   me->data3D.vOrientFront.y.y = yFront;
   me->data3D.vOrientFront.z.z = zFront;
   me->data3D.vOrientTop.x.x   = xTop;
   me->data3D.vOrientTop.y.y   = yTop;
   me->data3D.vOrientTop.z.z   = zTop;

   if (dwApply == DS3D_IMMEDIATE) {
      RecalcNow(me);
   }

   return DS_OK;
}

