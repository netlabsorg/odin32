/*
 * Playsound stubs
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2.h>
#include "win32type.h"
#include "misc.h"

/*****************************************************************************
 * Name      : BOOL WIN32API PlaySound
 * Purpose   : Plays a sound specified by the given filename, resource, or
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
BOOL WIN32API PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound )
{
  dprintf(("WINMM:PlaySoundA not implemented\n"));
  return(FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API PlaySoundW
 * Purpose   : Plays a sound specified by the given filename, resource, or
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
BOOL WIN32API PlaySoundW(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound )
{
  dprintf(("WINMM:PlaySoundW not implemented\n"));
  return(FALSE);
}
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API sndPlaySoundA(LPCSTR pszSound, UINT fuSound)
{
  dprintf(("WINMM:sndPlaySoundA not implemented\n"));
  return(FALSE);
}
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API sndPlaySoundW(LPCWSTR pszSound, UINT fuSound)
{
  dprintf(("WINMM:sndPlaySoundW not implemented\n"));
  return(FALSE);
}
/******************************************************************************/
/******************************************************************************/
