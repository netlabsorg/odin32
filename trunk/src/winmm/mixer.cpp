/* $Id: mixer.cpp,v 1.5 2000-02-17 14:09:31 sandervl Exp $ */

/*
 * Mixer stubs
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

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_mixer
#include "dbglocal.h"


ODINDEBUGCHANNEL(WINMM-MIXER)

//
// All Stubs
//

/******************************************************************************/
ODINFUNCTION1(MMRESULT, mixerClose,
              HMIXER, hmx)
{
  dprintf(("WINMM:mixerClose - stub\n" ));
  return MMSYSERR_INVALHANDLE;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetControlDetailsA,
              HMIXEROBJ, hmxobj,
              LPMIXERCONTROLDETAILS, pmxcd,
              DWORD, fdwDetails)
{
  dprintf(("WINMM:mixerGetControlDetailsA - stub\n" ));
  return MIXERR_INVALCONTROL;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetControlDetailsW,
              HMIXEROBJ, hmxobj,
              LPMIXERCONTROLDETAILS, pmxcd,
              DWORD, fdwDetails)
{
  dprintf(("WINMM:mixerGetControlDetailsW - stub\n" ));
  return MIXERR_INVALCONTROL;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetDevCapsA,
              UINT, uMxId,
              LPMIXERCAPSA, pmxcaps,
              UINT, cbmxcaps)
{
  dprintf(("WINMM:mixerGetDevCapsA(%d) -stub\n", uMxId ));
  return MMSYSERR_BADDEVICEID;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetLineControlsA,
              HMIXEROBJ, hmxobj,
              LPMIXERLINECONTROLSA, pmxlc,
              DWORD, fdwControls)
{
  dprintf(("WINMM:mixerGetGetLineControlsA - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetLineControlsW,
              HMIXEROBJ, hmxobj,
              LPMIXERLINECONTROLSW, pmxlc,
              DWORD, fdwControls)
{
  dprintf(("WINMM:mixerGetGetLineControlsW - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetDevCapsW,
              UINT, uMxId,
              LPMIXERCAPSW, pmxcaps,
              UINT, cbmxcaps)
{
  dprintf(("WINMM:mixerGetDevCapsW(%u) - stub\n", uMxId ));
  return MMSYSERR_BADDEVICEID;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetID,
              HMIXEROBJ, hmxobj,
              UINT *, puMxId,
              DWORD, fdwId)
{
  dprintf(("WINMM:mixerGetID - stub\n" ));
  return MMSYSERR_INVALHANDLE;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetLineInfoA,
              HMIXEROBJ, hmxobj,
              LPMIXERLINEA, pmxl,
              DWORD, fdwInfo)
{
  dprintf(("WINMM:mixerGetLineInfoA - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetLineInfoW,
              HMIXEROBJ, hmxobj,
              LPMIXERLINEW, pmxl,
              DWORD, fdwInfo)
{
  dprintf(("WINMM:mixerGetLineInfoW - stub\n" ));
  return MIXERR_INVALLINE;
}

/******************************************************************************/
ODINFUNCTION4(MMRESULT, mixerMessage,
              HMIXER, hmx,
              UINT, uMsg,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  dprintf(("WINMM:mixerMessage - stub\n" ));
  return 0;
}

/******************************************************************************/
ODINFUNCTION0(UINT, mixerGetNumDevs)
{
  dprintf(("WINMM:mixerGetNumDevs - returning 0\n"));
  return 0;
}

/******************************************************************************/
ODINFUNCTION5(MMRESULT, mixerOpen,
              LPHMIXER, phmx,
              UINT, uMxId,
              DWORD, dwCallback,
              DWORD, dwInstance,
              DWORD, fdwOpen)
{
  dprintf(("WINMM:mixerOpen(%d) - stub\n", uMxId ));
  return MMSYSERR_BADDEVICEID;
}

ODINFUNCTION3(MMRESULT, mixerSetControlDetails,
              HMIXEROBJ, hmxobj,
              LPMIXERCONTROLDETAILS, pmxcd,
              DWORD, fdwDetails)
{
  dprintf(("WINMM:mixerSetControlDetails - stub\n" ));
  return MIXERR_INVALCONTROL;
}


