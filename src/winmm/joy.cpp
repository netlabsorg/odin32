/* $Id: joy.cpp,v 1.3 1999-06-10 16:24:33 phaller Exp $ */

/*
 * joystick stubs
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>
#include <misc.h>

#include "winmm.h"
#include "os2timer.h"

MMRESULT WIN32API joyConfigChanged( DWORD dwFlags )
{
  dprintf(("WINMM:joyConfigChanged - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API joyGetDevCapsA(UINT IDDevice, LPJOYCAPSA lpjc, UINT cbjc)
{
  dprintf(("WINMM:joyGetDevCapsA - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API joyGetDevCapsW(UINT IDDevice, LPJOYCAPSW lpjc, UINT cbjc)
{
  dprintf(("WINMM:joyGetDevCapsW - stub\n"));
  return(MMSYSERR_NODRIVER);
}

UINT WIN32API joyGetNumDevs()
{
  dprintf(("WINMM:joyGetNumDevs - stub\n"));
  return(0);
}

MMRESULT WIN32API joyGetPos(UINT uJoyID, LPJOYINFO pji)
{
  dprintf(("WINMM:joyGetPos - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API joyGetPosEx(UINT uJoyID, LPJOYINFOEX pji)
{
  dprintf(("WINMM:joyGetPosEx - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API joyGetThreshold(UINT uJoyID, LPUINT puThreshold)
{
  dprintf(("WINMM:joyGetThreshold - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API joyReleaseCapture(UINT uJoyID)
{
  dprintf(("WINMM:joyReleaseCapture - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API joySetCapture(HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged)
{
  dprintf(("WINMM:joySetCapture - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MMRESULT WIN32API joySetThreshold(UINT uJoyID, UINT uThreshold)
{
  dprintf(("WINMM:joySetThreshold - stub\n"));
  return(MMSYSERR_NODRIVER);
}
