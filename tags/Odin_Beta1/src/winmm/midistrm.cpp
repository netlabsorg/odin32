/* $Id: midistrm.cpp,v 1.5 2000-02-17 14:09:31 sandervl Exp $ */

/*
 * RTMIDI code
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
#include "midi.hpp"
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <unicode.h>

#define DBG_LOCALLOG	DBG_midistrm
#include "dbglocal.h"


ODINDEBUGCHANNEL(WINMM-MIDISTRM)


/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiStreamClose,
              HMIDISTRM, hms)
{
  dprintf(("WINMM:midiStreamClose - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION6(MMRESULT, midiStreamOpen,
              LPHMIDISTRM, phms,
              LPUINT, puDeviceID,
              DWORD, cMidi,
              DWORD, dwCallback,
              DWORD, dwInstance,
              DWORD, fdwOpen)
{
  dprintf(("WINMM:midiStreamOpen - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_BADDEVICEID;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiStreamProperty,
              HMIDISTRM, hms,
              LPBYTE, lppropdata,
              DWORD, dwProperty)
{
  dprintf(("WINMM:midiStreamProperty - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiStreamPosition,
              HMIDISTRM, hms,
              LPMMTIME, lpmmt,
              UINT, cbmmt)
{
  dprintf(("WINMM:midiStreamPosition - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiStreamOut,
              HMIDISTRM, hms,
              LPMIDIHDR, pmh,
              UINT, cbmh)
{
  dprintf(("WINMM:midiStreamOut - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiStreamPause,
              HMIDISTRM, hms)
{
  dprintf(("WINMM:midiStreamPause - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiStreamRestart,
              HMIDISTRM, hms)
{
  dprintf(("WINMM:midiStreamRestart - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiStreamStop,
              HMIDISTRM, hms)
{
  dprintf(("WINMM:midiStreamStop - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

