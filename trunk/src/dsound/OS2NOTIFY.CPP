/* $Id: OS2NOTIFY.CPP,v 1.2 2000-08-02 15:48:26 bird Exp $ */
/*
 * DirectSound DirectSoundNotify class
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
#include "os2sndbuffer.h"
#include "os2notify.h"
#include <misc.h>

//******************************************************************************
//******************************************************************************
OS2IDirectSoundNotify::OS2IDirectSoundNotify(OS2IDirectSoundBuffer *parentBuffer)
{
   lpVtbl = &Vtbl;
   Vtbl.AddRef                   = SoundNotifyAddRef;
   Vtbl.Release                  = SoundNotifyRelease;
   Vtbl.QueryInterface           = SoundNotifyQueryInterface;
   Vtbl.SetNotificationPositions = SoundNotifySetNotificationPositions;

   dprintf(("DSOUND-OS2IDirectSoundNotify::OS2IDirectSoundNotify (this=%X)", this));

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
OS2IDirectSoundNotify::~OS2IDirectSoundNotify()
{
   dprintf(("DSOUND-OS2IDirectSoundNotify::~OS2IDirectSoundNotify (this=%X)", this));
   lpSoundBuffer->SetNotify(NULL);
   lpSoundBuffer->Vtbl.Release(lpSoundBuffer);
   if (lpPositions != NULL)
      free(lpPositions);
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall SoundNotifyQueryInterface(THIS, REFIID riid, LPVOID * ppvObj)
{
   dprintf(("DSOUND-OS2IDirectSoundNotify::QueryInterface"));
   if (This == NULL) {
      return DSERR_INVALIDPARAM;
   }
   *ppvObj = NULL;

   if (!IsEqualGUID(riid, IID_IDirectSoundNotify))
      return E_NOINTERFACE;

   *ppvObj = This;

   SoundNotifyAddRef(This);
   return DS_OK;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall SoundNotifyAddRef(THIS)
{
   OS2IDirectSoundNotify *me = (OS2IDirectSoundNotify *)This;

   dprintf(("DSOUND-OS2IDirectSoundNotify::AddRef (this=%X) %d", me, me->Referenced+1));
   if (me == NULL) {
      return 0;
   }
   return ++me->Referenced;
}

//******************************************************************************
//******************************************************************************
ULONG __stdcall SoundNotifyRelease(THIS)
{
   OS2IDirectSoundNotify *me = (OS2IDirectSoundNotify *)This;

   dprintf(("DSOUND-OS2IDirectSoundNotify::Release (this=%X) %d", me, me->Referenced-1));
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
HRESULT __stdcall SoundNotifySetNotificationPositions(THIS, DWORD cPositionNotifies,
                                            LPCDSBPOSITIONNOTIFY lpcPositionNotifies)
{
   OS2IDirectSoundNotify *me = (OS2IDirectSoundNotify *)This;

   dprintf(("DSOUND-OS2IDirectSoundNotify::SoundNotifySetPositions (%d, this=%X)", cPositionNotifies, me));
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
void  OS2IDirectSoundNotify::CheckPos(DWORD dwOldpos, DWORD dwNewpos)
{
   //dprintf(("DSOUND-OS2IDirectSoundNotify::CheckPos (%d, %d) (this=%X)", dwOldpos, dwNewpos, this));
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
void  OS2IDirectSoundNotify::CheckStop()
{
   //dprintf(("DSOUND-OS2IDirectSoundNotify::CheckStop (this=%X)", this));
   // just loop thru all notifications and check if DSBPN_OFFSETSTOP is there
   DWORD   i;

   for (i = 0; i < cPositions; i++) {
      if (lpPositions[i].dwOffset == DSBPN_OFFSETSTOP)
         SetEvent(lpPositions[i].hEventNotify);
   }
}
