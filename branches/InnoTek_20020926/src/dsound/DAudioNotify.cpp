/* $Id: DAudioNotify.cpp,v 1.1 2001-04-27 17:39:48 sandervl Exp $ */
/*
 * DirectSound DirectSoundNotify class (DirectAudio)
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

#include "os2dsound.h"
#include "daudiobuffer.h"
#include "daudionotify.h"
#include <misc.h>

//******************************************************************************
//******************************************************************************
IDirectAudioNotify::IDirectAudioNotify(IDirectAudioBuffer *parentBuffer)
{
   lpVtbl = &Vtbl;
   Vtbl.AddRef                   = DAudioNotifyAddRef;
   Vtbl.Release                  = DAudioNotifyRelease;
   Vtbl.QueryInterface           = DAudioNotifyQueryInterface;
   Vtbl.SetNotificationPositions = DAudioNotifySetNotificationPositions;

   dprintf(("DSOUND-IDirectAudioNotify::IDirectAudioNotify (this=%X)", this));

   lpSoundBuffer = parentBuffer;
   lpPositions   = NULL;
   cPositions    = 0;
   Referenced    = 0;

   // add a reference to the parent SoundBuffer to make sure it won't suddenly disappear
   lpSoundBuffer->Vtbl.AddRef(lpSoundBuffer);
   // set pointer to ourselves in parent SoundBuffer
   lpSoundBuffer->SetNotify(this);
}

//******************************************************************************
//******************************************************************************
IDirectAudioNotify::~IDirectAudioNotify()
{
   dprintf(("DSOUND-IDirectAudioNotify::~IDirectAudioNotify (this=%X)", this));
   lpSoundBuffer->SetNotify(NULL);
   lpSoundBuffer->Vtbl.Release(lpSoundBuffer);
   if (lpPositions != NULL)
      free(lpPositions);
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall DAudioNotifyQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-IDirectAudioNotify::QueryInterface"));
   if (This == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *ppvObj = NULL;

   if (!IsEqualGUID(riid, IID_IDirectSoundNotify))
      return E_NOINTERFACE;

   *ppvObj = This;

   DAudioNotifyAddRef(This);
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall DAudioNotifyAddRef(THIS)
{
   IDirectAudioNotify *me = (IDirectAudioNotify *)This;

   dprintf(("DSOUND-IDirectAudioNotify::AddRef (this=%X) %d", me, me->Referenced+1));
   if (me == NULL) {
      return 0;
   }
   return ++me->Referenced;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall DAudioNotifyRelease(THIS)
{
   IDirectAudioNotify *me = (IDirectAudioNotify *)This;

   dprintf(("DSOUND-IDirectAudioNotify::Release (this=%X) %d", me, me->Referenced-1));
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
HRESULT __stdcall DAudioNotifySetNotificationPositions(THIS, DWORD cPositionNotifies,
                                            LPCDSBPOSITIONNOTIFY lpcPositionNotifies)
{
   IDirectAudioNotify *me = (IDirectAudioNotify *)This;

   dprintf(("DSOUND-IDirectAudioNotify::DAudioNotifySetPositions (%d, this=%X)", cPositionNotifies, me));
   if (me == NULL || lpcPositionNotifies == NULL) {
      return DSERR_INVALIDPARAM;
   }

   // we have to copy the notifications array to our own memory here; if any notifications
   // were registered previously, discard them
   me->cPositions  = cPositionNotifies;
   if (me->lpPositions != NULL)
      free(me->lpPositions);
   me->lpPositions = (LPDSBPOSITIONNOTIFY)malloc(cPositionNotifies * sizeof(DSBPOSITIONNOTIFY));
   for (int i = 0; i < cPositionNotifies; i++) {
      me->lpPositions[i].dwOffset     = lpcPositionNotifies[i].dwOffset;
      me->lpPositions[i].hEventNotify = lpcPositionNotifies[i].hEventNotify;
   }

   return DS_OK;
}

//******************************************************************************
//******************************************************************************
void  IDirectAudioNotify::CheckPos(DWORD dwOldpos, DWORD dwNewpos)
{
   //dprintf(("DSOUND-IDirectAudioNotify::CheckPos (%d, %d) (this=%X)", dwOldpos, dwNewpos, this));
   // loop thru all registered notifications and check if any offset was reached
   DWORD   i;

   for (i = 0; i < cPositions; i++) {
      // we do NOT handle the stop notification here
      if (lpPositions[i].dwOffset == DSBPN_OFFSETSTOP)
         continue;

      if (dwOldpos < dwNewpos) { // did we just wrap?
         if ((lpPositions[i].dwOffset >= dwOldpos) && (lpPositions[i].dwOffset < dwNewpos))
            SetEvent(lpPositions[i].hEventNotify);
      } else {                   // yes we wrapped
         if ((lpPositions[i].dwOffset >= dwOldpos) || (lpPositions[i].dwOffset < dwNewpos))
            SetEvent(lpPositions[i].hEventNotify);
      }
   }
}

//******************************************************************************
//******************************************************************************
void  IDirectAudioNotify::CheckStop()
{
   //dprintf(("DSOUND-IDirectAudioNotify::CheckStop (this=%X)", this));
   // just loop thru all notifications and check if DSBPN_OFFSETSTOP is there
   DWORD   i;

   for (i = 0; i < cPositions; i++) {
      if (lpPositions[i].dwOffset == DSBPN_OFFSETSTOP)
         SetEvent(lpPositions[i].hEventNotify);
   }
}
