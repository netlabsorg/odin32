/*
 * Wave in stubs
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>

#include <stdio.h>
#include <string.h>

#include <misc.h>
#include <unicode.h>

#include "winmm.h"


// All stubs for now
MMRESULT WIN32API waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
  dprintf(("WINMM:waveInAddBuffer - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

MMRESULT WIN32API waveInClose(HWAVEIN hwi)
{
  dprintf(("WINMM:waveInClose - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

MMRESULT WIN32API waveInGetDevCapsA(UINT uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic)
{
  dprintf(("WINMM:waveInGetDevCapsA(%d) - stub\n", uDeviceID ));
  return MMSYSERR_BADDEVICEID;
}

MMRESULT WIN32API waveInGetDevCapsW(UINT uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic)
{
  dprintf(("WINMM:waveInGetDevCapsW(%d) - stub\n", uDeviceID ));
  return MMSYSERR_BADDEVICEID;
}

MMRESULT WIN32API waveInGetErrorTextA(MMRESULT wError, LPSTR lpText, UINT cchText)
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

MMRESULT WIN32API waveInGetErrorTextW(MMRESULT wError, LPWSTR lpText, UINT cchText)
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

MMRESULT WIN32API waveInGetID(HWAVEIN hwi, LPUINT puDeviceID)
{
  dprintf(("WINMM:waveInGetID - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

UINT WIN32API waveInGetNumDevs(void)
{
  dprintf(("WINMM:waveInGetNumDevs - stub\n"));
  return 0;
}

MMRESULT WIN32API waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
  dprintf(("WINMM:waveInGetPosition - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

MMRESULT WIN32API waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
  dprintf(("WINMM:waveInMessage - stub\n"));
  return MMSYSERR_INVALHANDLE;
}


MMRESULT WIN32API waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
                             const LPWAVEFORMATEX pwfx,
                             DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
  dprintf(("WINMM:waveInOpen - stub\n"));
   return MMSYSERR_BADDEVICEID;
}

MMRESULT WIN32API waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
  dprintf(("WINMM:waveInPrepareHeader - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

MMRESULT WIN32API waveInReset(HWAVEIN hwi)
{
  dprintf(("WINMM:waveInReset - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

MMRESULT WIN32API waveInStart(HWAVEIN hwi)
{
  dprintf(("WINMM:waveInStart - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

MMRESULT WIN32API waveInStop(HWAVEIN hwi)
{
  dprintf(("WINMM:waveInStop - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

MMRESULT WIN32API waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
  dprintf(("WINMM:waveInUnprepareHeader - stub\n"));
  return MMSYSERR_INVALHANDLE;
}

