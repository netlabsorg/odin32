/* $Id: waveout.cpp,v 1.17 2001-02-27 21:14:00 sandervl Exp $ */
//#undef DEBUG
/*
 * Wave out MM apis
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * waveOutGetPosition partly based on Wine code (dll\winmm\wineoss\audio.c)
 * Copyright 1994 Martin Ayotte
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

#define DBG_LOCALLOG    DBG_waveout
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-WAVEOUT)


/******************************************************************************/
/******************************************************************************/

//@@@PH 1999/12/28 this function needs the win32 tib FS selector for callback purposes
//                 therefore, one cannot use ODINFUNCTION macro wrappers! The xxxFS
//                 macro passes in USHORT selFS as first parameter.
ODINFUNCTION6(MMRESULT, waveOutOpen,
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

  if(fdwOpen == WAVE_FORMAT_QUERY) {
        if(DartWaveOut::queryFormat(pwfx->wFormatTag, pwfx->nChannels, pwfx->nSamplesPerSec,
                                    pwfx->wBitsPerSample) == TRUE) {
                return(MMSYSERR_NOERROR);
        }
        else    return(WAVERR_BADFORMAT);
  }

  if(phwo == NULL)
        return(MMSYSERR_INVALPARAM);

  if(fdwOpen == CALLBACK_WINDOW) {
        *phwo = (HWAVEOUT)new DartWaveOut(pwfx, (HWND)dwCallback);
  }
  else
  if(fdwOpen == CALLBACK_FUNCTION)
  {
        //@@@PH 1999/12/28 save valid FS: to win32 TIB
        *phwo = (HWAVEOUT)new DartWaveOut(pwfx, dwCallback, dwInstance, GetFS());
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

        dprintf(("waveOutWrite %x %d %x", pwh->lpData, pwh->dwBufferLength, pwh->dwFlags));
        return(dwave->write(pwh, cbwh));
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutReset,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  if(DartWaveOut::find(dwave) == TRUE)
        return(dwave->reset());
  else  return(MMSYSERR_INVALHANDLE);
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
  else  return(MMSYSERR_INVALHANDLE);
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
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutPause,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  if(DartWaveOut::find(dwave) == TRUE)
        return(dwave->pause());
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, waveOutRestart,
              HWAVEOUT, hwaveout)
{
  DartWaveOut *dwave = (DartWaveOut *)hwaveout;

  if(DartWaveOut::find(dwave) == TRUE)
        return(dwave->restart());
  else  return(MMSYSERR_INVALHANDLE);
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
  else  return(MMSYSERR_INVALHANDLE);
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
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutGetPosition,
              HWAVEOUT, hwo,
              LPMMTIME, pmmt,
              UINT, cbmmt)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;

  if(pmmt == NULL)
        return MMSYSERR_INVALPARAM;

  if(DartWaveOut::find(dwave) == TRUE)
  {
   ULONG position;

        position = dwave->getPosition();
        if(position == -1) {
            return MMSYSERR_HANDLEBUSY; //todo correct error value
        }
        switch (pmmt->wType) {
        case TIME_BYTES:
            pmmt->u.cb = position;
            break;
        case TIME_SAMPLES:
            pmmt->u.sample = position * 8 / dwave->getBitsPerSample();
            break;
        case TIME_SMPTE:
        {
            ULONG timeval = position / (dwave->getAvgBytesPerSecond() / 1000);
            pmmt->u.smpte.hour = timeval / 108000;
            timeval -= pmmt->u.smpte.hour * 108000;
            pmmt->u.smpte.min = timeval / 1800;
            timeval -= pmmt->u.smpte.min * 1800;
            pmmt->u.smpte.sec = timeval / 30;
            timeval -= pmmt->u.smpte.sec * 30;
            pmmt->u.smpte.frame = timeval;
            pmmt->u.smpte.fps = 30;
            break;
        }
        default:
            dprintf(("waveOutGetPosition: Format %d not supported ! use TIME_MS !\n", pmmt->wType));
            pmmt->wType = TIME_MS;
        case TIME_MS:
            pmmt->u.ms = position / (dwave->getAvgBytesPerSecond() / 1000);
            dprintf(("WINMM:waveOutGetPosition: TIME_MS pos=%d ms=%d time=%d", position, pmmt->u.ms, GetCurrentTime()));
            break;
        }
        return MMSYSERR_NOERROR;
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutGetDevCapsA,
              UINT, uDeviceID,
              LPWAVEOUTCAPSA, pwoc,
              UINT, cbwoc)
{
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
  if(DartWaveOut::getNumDevices() == 0) {
        memset(pwoc, 0, sizeof(*pwoc));
        return MMSYSERR_NODRIVER;
  }
  // we have to fill in this thing
  pwoc->wMid = 0;                  /* manufacturer ID */
  pwoc->wPid = 0;                  /* product ID */
  pwoc->vDriverVersion = 0x0001;        /* version of the driver */
  lstrcpyW(pwoc->szPname, (LPCWSTR)L"OS/2 DART Wave Out"); /* product name */
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
  if(theMsg) {
        strncpy( lpText, theMsg, cchText );
  }
  else
  {
        char errMsg[100];
        sprintf( errMsg, "Unknown error number %d", wError );
        strncpy( lpText, errMsg, cchText );
  }
  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, waveOutGetErrorTextW,
              MMRESULT, wError,
              LPWSTR, lpText,
              UINT, cchText)
{
  char * theMsg = getWinmmMsg( wError );
  if(theMsg) {
        AsciiToUnicode( theMsg, lpText );
  }
  else
  {
        char errMsg[100];
        sprintf( errMsg, "Unknown error number %d", wError );
        AsciiToUnicode( errMsg, lpText );
  }
  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
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
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION2(MMRESULT, waveOutGetPitch,
              HWAVEOUT, hwo,
              LPDWORD, pdwPitch)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
        return MMSYSERR_NOTSUPPORTED;
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION2(MMRESULT, waveOutSetPitch,
              HWAVEOUT, hwo,
              DWORD, dwPitch)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
        return MMSYSERR_NOTSUPPORTED;
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION2(MMRESULT, waveOutGetVolume,
              HWAVEOUT, hwo,
              LPDWORD, pdwVolume)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
        if(pdwVolume!=NULL)
            *pdwVolume=dwave->getVolume();
        return MMSYSERR_NOERROR;
  }
  else
      return MMSYSERR_NOERROR;
//    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION2(MMRESULT, waveOutSetVolume,
              HWAVEOUT, hwo,
              DWORD, dwVolume)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
    return(dwave->setVolume(dwVolume));
  }
  if(hwo == NULL) {
    DartWaveOut::setDefaultVolume(dwVolume);
  }
  return MMSYSERR_NOERROR;
//    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION2(MMRESULT, waveOutGetPlaybackRate,
              HWAVEOUT, hwo,
              LPDWORD, pdwRate)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
        return MMSYSERR_NOTSUPPORTED;
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION2(MMRESULT, waveOutSetPlaybackRate,
              HWAVEOUT, hwo,
              DWORD, dwRate)
{
  DartWaveOut *dwave = (DartWaveOut *)hwo;
  if(DartWaveOut::find(dwave) == TRUE)
  {
        return MMSYSERR_NOTSUPPORTED;
  }
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
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
/******************************************************************************/
/******************************************************************************/


