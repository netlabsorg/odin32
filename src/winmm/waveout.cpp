/* $Id: waveout.cpp,v 1.1 1999-05-24 20:20:07 ktk Exp $ */

/*
 * Wave out MM apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>

#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unicode.h>

#include "dwaveout.h"
#include "misc.h"
#include "winmm.h"

/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
                              const LPWAVEFORMATEX pwfx, DWORD dwCallback,
                              DWORD dwInstance, DWORD fdwOpen)
{
 MMRESULT rc;

  dprintf(("WINMM:waveOutOpen %d %X %X\n", uDeviceID, fdwOpen, dwCallback));
  dprintf(("WINMM:waveOutOpen = %X\n", waveOutOpen));

  if(pwfx == NULL)
        return(WAVERR_BADFORMAT);

  if(phwo == NULL)
        return(MMSYSERR_INVALPARAM);

  if(fdwOpen & WAVE_FORMAT_QUERY) {
        if(DartWaveOut::queryFormat(pwfx->wFormatTag, pwfx->nChannels,
                                    pwfx->nSamplesPerSec, pwfx->nBlockAlign) == TRUE) {
                return(MMSYSERR_NOERROR);
        }
        else    return(WAVERR_BADFORMAT);
  }

  if(fdwOpen & CALLBACK_WINDOW) {
        *phwo = (HWAVEOUT)new DartWaveOut(pwfx, (HWND)dwCallback);
  }
  else
  if(fdwOpen & CALLBACK_FUNCTION) {
        *phwo = (HWAVEOUT)new DartWaveOut(pwfx, dwCallback, dwInstance);
  }
  else  *phwo = (HWAVEOUT)new DartWaveOut(pwfx);

  if(*phwo == NULL) {
        return(MMSYSERR_NODRIVER);
  }

  rc = ((DartWaveOut *)*phwo)->getError();
  if(rc != MMSYSERR_NOERROR) {
        delete (DartWaveOut *)*phwo;
        return(rc);
  }
  dprintf(("WINMM:waveOutOpen success\n"));
  return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
 DartWaveOut *dwave = (DartWaveOut *)hwo;

#ifdef DEBUG1
  WriteLog("WINMM:waveOutWrite %X %d\n", pwh, cbwh);
#endif
  if(DartWaveOut::find(dwave) == TRUE) {
        if(!(pwh->dwFlags & WHDR_PREPARED) || pwh->lpData == NULL)
                return WAVERR_UNPREPARED;
        if(pwh->dwFlags & WHDR_INQUEUE)
                return WAVERR_STILLPLAYING;

        pwh->dwFlags |= WHDR_INQUEUE;
        pwh->dwFlags &= ~WHDR_DONE;

        return(dwave->write(pwh, cbwh));
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutReset(HWAVEOUT hwaveout)
{
 DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  dprintf(("WINMM:waveOutReset %X\n", hwaveout));
  if(DartWaveOut::find(dwave) == TRUE) {
        return(dwave->reset());
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutBreakLoop(HWAVEOUT hwaveout)
{
 DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  dprintf(("WINMM:waveOutBreakLoop (implemented as reset) %X\n", hwaveout));
  if(DartWaveOut::find(dwave) == TRUE) {
        return(dwave->reset());
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutClose(HWAVEOUT hwaveout)
{
 DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  dprintf(("WINMM:waveOutClose %X\n", hwaveout));
  if(DartWaveOut::find(dwave) == TRUE) {
        if(dwave->getState() == STATE_PLAYING)
                return(WAVERR_STILLPLAYING);
        delete dwave;
        return(MMSYSERR_NOERROR);
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutPause(HWAVEOUT hwaveout)
{
 DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  dprintf(("WINMM:waveOutPause %X\n", hwaveout));
  if(DartWaveOut::find(dwave) == TRUE) {
        return(dwave->pause());
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutRestart(HWAVEOUT hwaveout)
{
 DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  dprintf(("WINMM:waveOutRestart %X\n", hwaveout));
  if(DartWaveOut::find(dwave) == TRUE) {
        return(dwave->restart());
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
 DartWaveOut *dwave = (DartWaveOut *)hwo;

  dprintf(("WINMM:waveOutPrepareHeader %X %d\n", pwh, cbwh));

  if(DartWaveOut::find(dwave) == TRUE) {
        if(pwh->dwFlags & WHDR_INQUEUE)
                return WAVERR_STILLPLAYING;
        pwh->dwFlags |= WHDR_PREPARED;
        pwh->dwFlags &= ~WHDR_DONE;
        pwh->lpNext   = NULL;
        return(MMSYSERR_NOERROR);
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
 DartWaveOut *dwave = (DartWaveOut *)hwo;

  dprintf(("WINMM:waveOutUnprepareHeader %X %d\n", pwh, cbwh));
  if(DartWaveOut::find(dwave) == TRUE) {
        if(pwh->dwFlags & WHDR_INQUEUE)
                return WAVERR_STILLPLAYING;

        pwh->dwFlags &= ~WHDR_PREPARED;
        pwh->dwFlags |= WHDR_DONE;
        return(MMSYSERR_NOERROR);
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  dprintf(("WINMM:waveOutGetPosition - not implemented\n"));
  if(DartWaveOut::find(dwave) == TRUE) {
        return(MMSYSERR_NOERROR);
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutGetDevCapsA(UINT uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc)
{
  dprintf(("WINMM:waveOutGetDevCapsA - stub\n"));
  // we have to fill in this thing
  pwoc->wMid = 0;                  /* manufacturer ID */
  pwoc->wPid = 0;                  /* product ID */
  pwoc->vDriverVersion = 0x0001;        /* version of the driver */
  strcpy( pwoc->szPname, "OS/2 DART Wave Out" ); /* product name */
  pwoc->dwFormats = WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08 |
                    WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16 |
                    WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08 |
                    WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16 |
                    WAVE_FORMAT_4M08 | WAVE_FORMAT_4S08 |
                    WAVE_FORMAT_4M16 | WAVE_FORMAT_4S16;

  pwoc->wChannels  = 2;             /* number of sources supported */
  pwoc->wReserved1 = 0;             /* packing */
  pwoc->dwSupport  = WAVECAPS_LRVOLUME | WAVECAPS_VOLUME;

  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutGetDevCapsW(UINT uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc)
{
  dprintf(("WINMM:waveOutGetDevCapsW - stub\n"));
  // we have to fill in this thing
  pwoc->wMid = 0;                  /* manufacturer ID */
  pwoc->wPid = 0;                  /* product ID */
  pwoc->vDriverVersion = 0x0001;        /* version of the driver */
  AsciiToUnicode( "OS/2 DART Wave Out", pwoc->szPname ); /* product name */
  pwoc->dwFormats = WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08 |
                    WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16 |
                    WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08 |
                    WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16 |
                    WAVE_FORMAT_4M08 | WAVE_FORMAT_4S08 |
                    WAVE_FORMAT_4M16 | WAVE_FORMAT_4S16;

  pwoc->wChannels  = 2;             /* number of sources supported */
  pwoc->wReserved1 = 0;             /* packing */
  pwoc->dwSupport  = WAVECAPS_LRVOLUME | WAVECAPS_VOLUME;

  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
UINT WIN32API waveOutGetNumDevs(void)
{
  dprintf(("WINMM:waveOutGetNumDevs\n"));
  return 1;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API waveOutGetErrorTextA(MMRESULT wError, LPSTR lpText, UINT cchText)
{
  dprintf(("WINMM:waveOutGetErrorTextA(%d)\n", wError ));
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

MMRESULT WIN32API waveOutGetErrorTextW(MMRESULT wError, LPWSTR lpText, UINT cchText)
{
  dprintf(("WINMM:waveOutGetErrorTextW(%d) - stub\n", wError ));
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

MMRESULT WIN32API waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID)
{
  dprintf(("WINMM:waveOutGetID" ));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    *puDeviceID = 1;
    return MMSYSERR_NOERROR;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

MMRESULT WIN32API waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
  dprintf(("WINMM:waveOutGetPitch"));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

MMRESULT WIN32API waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
  dprintf(("WINMM:waveOutSetPitch"));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

MMRESULT WIN32API waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
  dprintf(("WINMM:waveOutGetVolume"));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

MMRESULT WIN32API waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
  dprintf(("WINMM:waveOutSetVolume"));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

MMRESULT WIN32API waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
  dprintf(("WINMM:waveOutGetPlaybackRate"));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

MMRESULT WIN32API waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
  dprintf(("WINMM:waveOutSetPlaybackRate"));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

MMRESULT WIN32API waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
{
  dprintf(("WINMM:waveOutMessage"));
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

