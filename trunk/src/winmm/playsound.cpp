/* $Id: playsound.cpp,v 1.4 1999-08-19 18:46:05 phaller Exp $ */

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

#include <os2wrap.h>       //Odin32 OS/2 api wrappers
#include <odinwrap.h>
#include "win32type.h"
#include "misc.h"

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

