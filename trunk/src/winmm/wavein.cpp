/* $Id: wavein.cpp,v 1.5 2000-02-17 14:09:32 sandervl Exp $ */

/*
 * Wave in stubs
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
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include <unicode.h>

#include "winmm.h"

#define DBG_LOCALLOG	DBG_wavein
#include "dbglocal.h"


ODINDEBUGCHANNEL(WINMM-WAVEIN)

// All stubs for now
ODINFUNCTION3(MMRESULT, waveInAddBuffer,
              HWAVEIN, hwi,
              LPWAVEHDR, pwh,
              UINT, cbwh)
{
  dprintf(("WINMM:waveInAddBuffer - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION1(MMRESULT, waveInClose,
              HWAVEIN, hwi)
{
  dprintf(("WINMM:waveInClose - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION3(MMRESULT, waveInGetDevCapsA,
              UINT, uDeviceID,
              LPWAVEINCAPSA, pwic,
              UINT, cbwic)
{
  dprintf(("WINMM:waveInGetDevCapsA(%d) - stub\n", uDeviceID ));
  return MMSYSERR_BADDEVICEID;
}

ODINFUNCTION3(MMRESULT, waveInGetDevCapsW,
              UINT, uDeviceID,
              LPWAVEINCAPSW, pwic,
              UINT, cbwic)
{
  dprintf(("WINMM:waveInGetDevCapsW(%d) - stub\n", uDeviceID ));
  return MMSYSERR_BADDEVICEID;
}

ODINFUNCTION3(MMRESULT, waveInGetErrorTextA,
              MMRESULT, wError,
              LPSTR, lpText,
              UINT, cchText)
{
  dprintf(("WINMM:waveInGetErrorTextA(%d)\n", wError ));
  char * theMsg = getWinmmMsg( wError );
  if ( theMsg )
    strncpy( lpText, theMsg, cchText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", wError );
    strncpy( lpText, errMsg, cchText );
  }
  return MMSYSERR_NOERROR;
}

ODINFUNCTION3(MMRESULT, waveInGetErrorTextW,
              MMRESULT, wError,
              LPWSTR, lpText,
              UINT, cchText)
{
  dprintf(("WINMM:waveInGetErrorTextW(%d) - stub\n", wError ));
  char * theMsg = getWinmmMsg( wError );
  if ( theMsg )
    AsciiToUnicode( theMsg, lpText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", wError );
    AsciiToUnicode( errMsg, lpText );
  }
  return MMSYSERR_NOERROR;
}

ODINFUNCTION2(MMRESULT, waveInGetID,
              HWAVEIN, hwi,
              LPUINT, puDeviceID)
{
  dprintf(("WINMM:waveInGetID - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION0(UINT, waveInGetNumDevs)
{
  dprintf(("WINMM:waveInGetNumDevs - stub\n"));
  return 0;
}

ODINFUNCTION3(MMRESULT, waveInGetPosition,
              HWAVEIN, hwi,
              LPMMTIME, pmmt,
              UINT, cbmmt)
{
  dprintf(("WINMM:waveInGetPosition - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION4(MMRESULT, waveInMessage,
              HWAVEIN, hwi,
              UINT, uMsg,
              DWORD, dw1,
              DWORD, dw2)
{
  dprintf(("WINMM:waveInMessage - stub\n"));
  return MMSYSERR_INVALHANDLE;
}


ODINFUNCTION6(MMRESULT, waveInOpen,
              LPHWAVEIN, phwi,
              UINT, uDeviceID,
              const LPWAVEFORMATEX, pwfx,
              DWORD, dwCallback,
              DWORD, dwInstance,
              DWORD, fdwOpen)
{
  dprintf(("WINMM:waveInOpen - stub\n"));
   return MMSYSERR_BADDEVICEID;
}

ODINFUNCTION3(MMRESULT, waveInPrepareHeader,
              HWAVEIN, hwi,
              LPWAVEHDR, pwh,
              UINT, cbwh)
{
  dprintf(("WINMM:waveInPrepareHeader - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION1(MMRESULT, waveInReset,
              HWAVEIN, hwi)
{
  dprintf(("WINMM:waveInReset - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION1(MMRESULT, waveInStart,
              HWAVEIN, hwi)
{
  dprintf(("WINMM:waveInStart - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION1(MMRESULT, waveInStop,
              HWAVEIN, hwi)
{
  dprintf(("WINMM:waveInStop - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

ODINFUNCTION3(MMRESULT, waveInUnprepareHeader,
              HWAVEIN, hwi,
              LPWAVEHDR, pwh,
              UINT, cbwh)
{
  dprintf(("WINMM:waveInUnprepareHeader - stub\n"));
  return MMSYSERR_INVALHANDLE;
}


