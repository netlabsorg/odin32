/* $Id: DSOUND.CPP,v 1.4 1999-10-23 12:35:14 sandervl Exp $ */

/*
 * DirectSound exported APIs
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define WIN32SDK_NOPOSTWRAPPER

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <stdlib.h>
#include <string.h>

#include <dsound.h>
//#include "os2dsound.h"

#include <misc.h>

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundCreate(const GUID *lpGUID,
                                      LPDIRECTSOUND * ppDS,
                                      IUnknown *pUnkOuter)
{
  dprintf(("DirectSoundCreate %X %X %X\n", lpGUID, ppDS, pUnkOuter));
#if 0
 OS2IDirectSound *newsound = new OS2IDirectSound(lpGUID);
 HRESULT         rc;

  if(newsound == NULL)  return(DSERR_OUTOFMEMORY);

  rc = newsound->GetLastError();
  if(rc != DS_OK) {
    *ppDS = NULL;
    delete newsound;
  }
  else  *ppDS = (LPDIRECTSOUND)newsound;
  return(rc);
#else
  return(DSERR_NODRIVER);
#endif
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectSoundEnumerateA(LPDSENUMCALLBACKA lpCallback,
                                          LPVOID lpContext)
{
  dprintf(("DirectSoundEnumerateA\n"));

  lpCallback(NULL, "DART DirectSound for OS/2", "DirectSound/2 v0.1",
         lpContext);
  return(DS_OK);
}
//******************************************************************************
//******************************************************************************
