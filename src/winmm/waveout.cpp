/* $Id: waveout.cpp,v 1.9 1999-12-31 13:55:51 sandervl Exp $ */
//#undef DEBUG
/*
 * Wave out MM apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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

#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unicode.h>

#include "dwaveout.h"
#include "misc.h"
#include "winmm.h"

ODINDEBUGCHANNEL(WINMM-WAVEOUT)


/******************************************************************************/
/******************************************************************************/

//@@@PH 1999/12/28 this function needs the win32 tib FS selector for callback purposes
//                 therefore, one cannot use ODINFUNCTION macro wrappers! The xxxFS
//                 macro passes in USHORT selFS as first parameter.
ODINFUNCTION6FS(MMRESULT, waveOutOpen,
                LPHWAVEOUT, phwo,
                UINT, uDeviceID,
                const LPWAVEFORMATEX, pwfx,
                DWORD, dwCallback,
                DWORD, dwInstance,
                DWORD, fdwOpen)
{
  MMRESULT rc;

  if(pwfx == NULL)
        return(WAVERR_BADFORMAT);

  if(phwo == NULL)
        return(MMSYSERR_INVALPARAM);

  if(fdwOpen == WAVE_FORMAT_QUERY) {
        if(DartWaveOut::queryFormat(pwfx->wFormatTag, pwfx->nChannels,
                                    pwfx->nSamplesPerSec, pwfx->nBlockAlign) == TRUE) {
                return(MMSYSERR_NOERROR);
        }
        else    return(WAVERR_BADFORMAT);
  }

  if(fdwOpen == CALLBACK_WINDOW) {
        *phwo = (HWAVEOUT)new DartWaveOut(pwfx, (HWND)dwCallback);
  }
  else
  if(fdwOpen == CALLBACK_FUNCTION)
  {
    //@@@PH 1999/12/28 save valid FS: to win32 TIB
        *phwo = (HWAVEOUT)new DartWaveOut(pwfx, dwCallback, dwInstance, selFS);
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
  return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutWrite,
              HWAVEOUT, hwo,
              LPWAVEHDR, pwh,
              UINT, cbwh)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;

  if(DartWaveOut::find(dwave) == TRUE)
  {
    if(!(pwh->dwFlags & WHDR_PREPARED) || pwh->lpData == NULL)
      return WAVERR_UNPREPARED;

    if(pwh->dwFlags & WHDR_INQUEUE)
      return WAVERR_STILLPLAYING;

    pwh->dwFlags |= WHDR_INQUEUE;
    pwh->dwFlags &= ~WHDR_DONE;

    return(dwave->write(pwh, cbwh));
  }
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutReset,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  if(DartWaveOut::find(dwave) == TRUE)
    return(dwave->reset());
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutBreakLoop,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  dprintf(("WINMM:waveOutBreakLoop (implemented as reset) %X\n", hwaveout));
  if(DartWaveOut::find(dwave) == TRUE)
    return(dwave->reset());
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutClose,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  if(DartWaveOut::find(dwave) == TRUE)
  {
    if(dwave->getState() == STATE_PLAYING)
      return(WAVERR_STILLPLAYING);

    delete dwave;
    return(MMSYSERR_NOERROR);
  }
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutPause,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  if(DartWaveOut::find(dwave) == TRUE)
    return(dwave->pause());
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutRestart,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  if(DartWaveOut::find(dwave) == TRUE)
    return(dwave->restart());
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutPrepareHeader,
              HWAVEOUT, hwo,
              LPWAVEHDR, pwh,
              UINT, cbwh)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;

  if(DartWaveOut::find(dwave) == TRUE)
  {
    if(pwh->dwFlags & WHDR_INQUEUE)
      return WAVERR_STILLPLAYING;

    pwh->dwFlags |= WHDR_PREPARED;
    pwh->dwFlags &= ~WHDR_DONE;
    pwh->lpNext   = NULL;
    return(MMSYSERR_NOERROR);
  }
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutUnprepareHeader,
              HWAVEOUT, hwo,
              LPWAVEHDR, pwh,
              UINT, cbwh)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;

  if(DartWaveOut::find(dwave) == TRUE)
  {
    if(pwh->dwFlags & WHDR_INQUEUE)
      return WAVERR_STILLPLAYING;

    pwh->dwFlags &= ~WHDR_PREPARED;
    pwh->dwFlags |= WHDR_DONE;
   return(MMSYSERR_NOERROR);
  }
  else
   return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutGetPosition,
              HWAVEOUT, hwo,
              LPMMTIME, pmmt,
              UINT, cbmmt)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  dprintf(("WINMM:waveOutGetPosition - not implemented\n"));
  if(DartWaveOut::find(dwave) == TRUE)
    return(MMSYSERR_NOERROR);
  else
    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutGetDevCapsA,
              UINT, uDeviceID,
              LPWAVEOUTCAPSA, pwoc,
              UINT, cbwoc)
{
  dprintf(("WINMM:waveOutGetDevCapsA"));

  if(DartWaveOut::getNumDevices() == 0) {
	memset(pwoc, 0, sizeof(*pwoc));
	return MMSYSERR_NODRIVER;
  }

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
ODINFUNCTION3(MMRESULT, waveOutGetDevCapsW,
              UINT, uDeviceID,
              LPWAVEOUTCAPSW, pwoc,
              UINT, cbwoc)
{
  dprintf(("WINMM:waveOutGetDevCapsW"));

  if(DartWaveOut::getNumDevices() == 0) {
	memset(pwoc, 0, sizeof(*pwoc));
	return MMSYSERR_NODRIVER;
  }
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
ODINFUNCTION0(UINT, waveOutGetNumDevs)
{
  return DartWaveOut::getNumDevices();
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutGetErrorTextA,
              MMRESULT, wError,
              LPSTR, lpText,
              UINT, cchText)
{
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

ODINFUNCTION3(MMRESULT, waveOutGetErrorTextW,
              MMRESULT, wError,
              LPWSTR, lpText,
              UINT, cchText)
{
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

ODINFUNCTION2(MMRESULT, waveOutGetID,
              HWAVEOUT, hwo,
              LPUINT, puDeviceID)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    *puDeviceID = 1;
    return MMSYSERR_NOERROR;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

ODINFUNCTION2(MMRESULT, waveOutGetPitch,
              HWAVEOUT, hwo,
              LPDWORD, pdwPitch)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
    return MMSYSERR_NOTSUPPORTED;
  else
    return(MMSYSERR_INVALHANDLE);
}

ODINFUNCTION2(MMRESULT, waveOutSetPitch,
              HWAVEOUT, hwo,
              DWORD, dwPitch)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
    return MMSYSERR_NOTSUPPORTED;
  else
    return(MMSYSERR_INVALHANDLE);
}

ODINFUNCTION2(MMRESULT, waveOutGetVolume,
              HWAVEOUT, hwo,
              LPDWORD, pdwVolume)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    if (pdwVolume!=NULL)
      *pdwVolume=dwave->getVolume();
    return MMSYSERR_NOERROR;    
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

ODINFUNCTION2(MMRESULT, waveOutSetVolume,
              HWAVEOUT, hwo,
              DWORD, dwVolume)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return(dwave->setVolume(dwVolume));
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

ODINFUNCTION2(MMRESULT, waveOutGetPlaybackRate,
              HWAVEOUT, hwo,
              LPDWORD, pdwRate)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

ODINFUNCTION2(MMRESULT, waveOutSetPlaybackRate,
              HWAVEOUT, hwo,
              DWORD, dwRate)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}

ODINFUNCTION4(MMRESULT, waveOutMessage,
              HWAVEOUT, hwo,
              UINT, uMsg,
              DWORD, dw1,
              DWORD, dw2)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return MMSYSERR_NOTSUPPORTED;
  }
  else
    return(MMSYSERR_INVALHANDLE);
}


