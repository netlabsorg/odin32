/* $Id: mixer.cpp,v 1.3 1999-06-10 16:24:34 phaller Exp $ */

/*
 * Mixer stubs
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

//
// All Stubs
//

/******************************************************************************/
MMRESULT WIN32API mixerClose( HMIXER hmx )
{
  dprintf(("WINMM:mixerClose - stub\n" ));
  return MMSYSERR_INVALHANDLE;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
  dprintf(("WINMM:mixerGetControlDetailsA - stub\n" ));
  return MIXERR_INVALCONTROL;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetControlDetailsW(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
  dprintf(("WINMM:mixerGetControlDetailsW - stub\n" ));
  return MIXERR_INVALCONTROL;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetDevCapsA( UINT uMxId, LPMIXERCAPSA pmxcaps, UINT cbmxcaps)
{
  dprintf(("WINMM:mixerGetDevCapsA(%d) -stub\n", uMxId ));
  return MMSYSERR_BADDEVICEID;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls)
{
  dprintf(("WINMM:mixerGetGetLineControlsA - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetLineControlsW(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls)
{
  dprintf(("WINMM:mixerGetGetLineControlsW - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetDevCapsW( UINT uMxId, LPMIXERCAPSW pmxcaps, UINT cbmxcaps)
{
  dprintf(("WINMM:mixerGetDevCapsW(%u) - stub\n", uMxId ));
  return MMSYSERR_BADDEVICEID;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetID( HMIXEROBJ hmxobj, UINT *puMxId, DWORD fdwId)
{
  dprintf(("WINMM:mixerGetID - stub\n" ));
  return MMSYSERR_INVALHANDLE;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo)
{
  dprintf(("WINMM:mixerGetLineInfoA - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
MMRESULT WIN32API mixerGetLineInfoW(HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo)
{
  dprintf(("WINMM:mixerGetLineInfoW - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
MMRESULT WIN32API mixerMessage( HMIXER hmx, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
  dprintf(("WINMM:mixerMessage - stub\n" ));
  return 0;
}

/******************************************************************************/
UINT WIN32API mixerGetNumDevs( void )
{
  dprintf(("WINMM:mixerGetNumDevs - returning 0\n"));
  return 0;
}

/******************************************************************************/
MMRESULT WIN32API mixerOpen( LPHMIXER phmx, UINT uMxId, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
  dprintf(("WINMM:mixerOpen(%d) - stub\n", uMxId ));
  return MMSYSERR_BADDEVICEID;
}

MMRESULT WIN32API mixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
  dprintf(("WINMM:mixerSetControlDetails - stub\n" ));
  return MIXERR_INVALCONTROL;
}

