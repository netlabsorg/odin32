/*
 * RTMIDI code
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>
#include "midi.hpp"
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <unicode.h>

/******************************************************************************/
MMRESULT WIN32API midiStreamClose(HMIDISTRM hms)
{
  dprintf(("WINMM:midiStreamClose - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
MMRESULT WIN32API midiStreamOpen(LPHMIDISTRM phms,
                                 LPUINT puDeviceID,
                                 DWORD cMidi,
                                 DWORD dwCallback,
                                 DWORD dwInstance,
                                 DWORD fdwOpen)
{
  dprintf(("WINMM:midiStreamOpen - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_BADDEVICEID;
  return rc;
}

/******************************************************************************/
MMRESULT WIN32API midiStreamProperty(HMIDISTRM hms, LPBYTE lppropdata, DWORD dwProperty)
{
  dprintf(("WINMM:midiStreamProperty - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
MMRESULT WIN32API midiStreamPosition(HMIDISTRM hms, LPMMTIME lpmmt, UINT cbmmt)
{
  dprintf(("WINMM:midiStreamPosition - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
MMRESULT WIN32API midiStreamOut(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh)
{
  dprintf(("WINMM:midiStreamOut - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
MMRESULT WIN32API midiStreamPause(HMIDISTRM hms)
{
  dprintf(("WINMM:midiStreamPause - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
MMRESULT WIN32API midiStreamRestart(HMIDISTRM hms)
{
  dprintf(("WINMM:midiStreamRestart - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
MMRESULT WIN32API midiStreamStop(HMIDISTRM hms)
{
  dprintf(("WINMM:midiStreamStop - stub\n" ));
  MMRESULT rc;
  rc = MMSYSERR_INVALHANDLE;
  return rc;
}
