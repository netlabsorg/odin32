/* $Id: playsound.cpp,v 1.6 2000-05-02 13:29:59 bird Exp $ */

/*
 * Playsound stubs
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#define  INCL_BASE
#define  INCL_OS2MM
#include <os2wrap.h>       //Odin32 OS/2 api wrappers
#include <odinwrap.h>
#include <win32type.h>
#include <winconst.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_playsound
#include "dbglocal.h"

//kso: dirty fix to make this compile! not permanent!
BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER *);
#define LowPart u.LowPart


ODINDEBUGCHANNEL(WINMM-PLAYSOUND)


/*****************************************************************************
 *             Plays a sound specified by the given filename, resource, or
 *             system event.
 * Parameters: LPCSTR  pszSound
 *             HMODULE hMod
 *             DWORD   fdwSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION3(BOOL, PlaySoundA,
              LPCSTR, pszSound,
              HMODULE, hmod,
              DWORD, fdwSound)
{
  dprintf(("WINMM:PlaySoundA not implemented\n"));
  if((fdwSound & (SND_SYNC_W|SND_ASYNC_W)) == (SND_SYNC_W|SND_ASYNC_W)) {
//	SetLastError(ERROR_INVALID_PARAMETER_W);
	return FALSE;
  }
//  if(fdwSound & (SND_MEMORY_W|SND_ALIAS_W|SND_FILENAME_W|SND_RESOURCE_W) == (SND_SYNC_W|SND_ASYNC_W)) {
//	SetLastError(ERROR_INVALID_PARAMETER_W);
//	return FALSE;
//  }
  return(FALSE);
}


/*****************************************************************************
 *             Plays a sound specified by the given filename, resource, or
 *             system event.
 * Parameters: LPCSTR  pszSound
 *             HMODULE hMod
 *             DWORD   fdwSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION3(BOOL, PlaySoundW,
              LPCWSTR, pszSound,
              HMODULE, hmod,
              DWORD, fdwSound)
{
  dprintf(("WINMM:PlaySoundW not implemented\n"));
  return(FALSE);
}


/*****************************************************************************
 *             Plays a sound specified by the given filename
 * Parameters: LPCSTR  pszSound
 *             UINT    fuSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION2(BOOL, sndPlaySoundA,
              LPCSTR, pszSound,
              UINT, fuSound)
{
  dprintf(("WINMM:sndPlaySoundA not implemented\n"));
  return(FALSE);
}


/*****************************************************************************
 *             Plays a sound specified by the given filename
 * Parameters: LPCWSTR  pszSound
 *             HMODULE  hMod
 *             DWORD    fdwSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION2(BOOL, sndPlaySoundW,
              LPCWSTR, pszSound,
              UINT, fuSound)
{
  dprintf(("WINMM:sndPlaySoundW not implemented\n"));
  return(FALSE);
}

