/* $Id: waveout.cpp,v 1.27 2003-07-16 15:47:24 sandervl Exp $ */

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

/******************************************************************************/
// Includes
/******************************************************************************/

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>

#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unicode.h>

#include "waveoutdart.h"
#include "waveoutdaud.h"
#include "waveoutflash.h"
#include "misc.h"
#include "winmm.h"
#include "initterm.h"

#define DBG_LOCALLOG    DBG_waveout
#include "dbglocal.h"

/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID, const LPWAVEFORMATEX pwfx,
                            DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
    MMRESULT rc;

    if (fMMPMAvailable == FALSE)
        return MMSYSERR_NODRIVER;

    if (pwfx == NULL)
        return(WAVERR_BADFORMAT);

    if (fdwOpen & WAVE_FORMAT_QUERY) {
        if (WaveOut::queryFormat(pwfx->wFormatTag, pwfx->nChannels,
                                 pwfx->nSamplesPerSec,
                                 pwfx->wBitsPerSample) == TRUE) {
            return (MMSYSERR_NOERROR);
        }
        return (WAVERR_BADFORMAT);
    }

    if (phwo == NULL)
        return(MMSYSERR_INVALPARAM);

    if (ODIN_IsFlashAudioEnabled())
        *phwo = (HWAVEOUT)new FlashWaveOut(pwfx, fdwOpen, dwCallback, dwInstance);
    else
    if (DAudioWaveOut::isDirectAudioAvailable())
        *phwo = (HWAVEOUT)new DAudioWaveOut(pwfx, fdwOpen, dwCallback, dwInstance);
    else
        *phwo = (HWAVEOUT)new DartWaveOut(pwfx, fdwOpen, dwCallback, dwInstance);

    if(*phwo == NULL)
        return(MMSYSERR_NODRIVER);

    rc = ((WaveOut *)*phwo)->open();
    if (rc != MMSYSERR_NOERROR)
        delete (WaveOut *)*phwo;

    return rc;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    WaveOut *dwave = (WaveOut *)hwo;

    if(WaveOut::find(dwave) == TRUE)
    {
        if(!(pwh->dwFlags & WHDR_PREPARED) || pwh->lpData == NULL) {
            dprintf(("waveOutWrite: WAVERR_UNPREPARED!!"));
            return WAVERR_UNPREPARED;
        }
        if(pwh->dwFlags & WHDR_INQUEUE) {
            dprintf(("waveOutWrite: WAVERR_STILLPLAYING!!"));
            return WAVERR_STILLPLAYING;
        }
        pwh->dwFlags |= WHDR_INQUEUE;
        pwh->dwFlags &= ~WHDR_DONE;

        dprintf(("waveOutWrite ptr %x size %d %x %x %x %x %x %x", pwh->lpData, pwh->dwBufferLength, pwh->dwBytesRecorded, pwh->dwUser, pwh->dwFlags, pwh->dwLoops, pwh->lpNext, pwh->reserved));
        return(dwave->write(pwh, cbwh));
    }
    else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutReset(HWAVEOUT hwaveout)
{
  WaveOut *dwave = (WaveOut *)hwaveout;

  if(WaveOut::find(dwave) == TRUE)
        return(dwave->reset());
  else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutBreakLoop(HWAVEOUT hwaveout)
{
    WaveOut *dwave = (WaveOut *)hwaveout;

    dprintf(("WINMM:waveOutBreakLoop (implemented as reset) %X\n", hwaveout));
    if(WaveOut::find(dwave) == TRUE)
         return(dwave->reset());
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutClose(HWAVEOUT hwaveout)
{
    WaveOut *dwave = (WaveOut *)hwaveout;

    if(WaveOut::find(dwave) == TRUE)
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
MMRESULT WINAPI waveOutPause(HWAVEOUT hwaveout)
{
    WaveOut *dwave = (WaveOut *)hwaveout;

    if(WaveOut::find(dwave) == TRUE)
        return(dwave->pause());
    else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutRestart(HWAVEOUT hwaveout)
{
    WaveOut *dwave = (WaveOut *)hwaveout;

    if(WaveOut::find(dwave) == TRUE)
         return(dwave->resume());
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    WaveOut *dwave = (WaveOut *)hwo;

    if(WaveOut::find(dwave) == TRUE)
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
MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    WaveOut *dwave = (WaveOut *)hwo;

    if(WaveOut::find(dwave) == TRUE)
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
MMRESULT WINAPI waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
  WaveOut *dwave = (WaveOut *)hwo;

    if(pmmt == NULL)
        return MMSYSERR_INVALPARAM;

    if(WaveOut::find(dwave) == TRUE)
    {
        ULONG position;

        position = dwave->getPosition();
        if(position == -1) {
            return MMSYSERR_HANDLEBUSY; //todo correct error value
        }
        switch (pmmt->wType) {
        case TIME_BYTES:
            pmmt->u.cb = position;
            dprintf2(("WINMM:waveOutGetPosition: TIME_BYTES %d (%x)", position, GetCurrentTime()));
            break;
        case TIME_SAMPLES:
            pmmt->u.sample = position * 8 / dwave->getBitsPerSample();
            dprintf2(("WINMM:waveOutGetPosition: TIME_SAMPLES %d", pmmt->u.sample));
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
            dprintf2(("WINMM:waveOutGetPosition: TIME_SAMPLES %d", position));
            break;
        }
        default:
            dprintf(("waveOutGetPosition: Format %d not supported ! use TIME_MS !\n", pmmt->wType));
            pmmt->wType = TIME_MS;
        case TIME_MS:
            pmmt->u.ms = position / (dwave->getAvgBytesPerSecond() / 1000);
            dprintf2(("WINMM:waveOutGetPosition: TIME_MS pos=%d ms=%d time=%d", position, pmmt->u.ms, GetCurrentTime()));
            break;
        }
        return MMSYSERR_NOERROR;
    }
    else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutGetDevCapsA(UINT uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc)
{
    if(fMMPMAvailable == FALSE) return MMSYSERR_NODRIVER;

    if(WaveOut::getNumDevices() == 0) {
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
MMRESULT WINAPI waveOutGetDevCapsW(UINT uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc)
{
    if(fMMPMAvailable == FALSE) return MMSYSERR_NODRIVER;

    if(WaveOut::getNumDevices() == 0) {
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
UINT WINAPI waveOutGetNumDevs()
{
  if(fMMPMAvailable == FALSE) return 0;

  return WaveOut::getNumDevices();
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutGetErrorTextA(MMRESULT wError, LPSTR lpText, UINT cchText)
{
    const char * theMsg = getWinmmMsg( wError );
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
MMRESULT WINAPI waveOutGetErrorTextW(MMRESULT wError, LPWSTR lpText, UINT cchText)
{
    const char * theMsg = getWinmmMsg( wError );
    if(theMsg) {
        AsciiToUnicodeN( theMsg, lpText, cchText );
    }
    else
    {
        char errMsg[100];
        sprintf( errMsg, "Unknown error number %d", wError );
        AsciiToUnicodeN( errMsg, lpText, cchText );
    }
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID)
{
    WaveOut *dwave = (WaveOut *)hwo;
    if(WaveOut::find(dwave) == TRUE)
    {
        *puDeviceID = 1;
        return MMSYSERR_NOERROR;
    }
    else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
    WaveOut *dwave = (WaveOut *)hwo;
    if(WaveOut::find(dwave) == TRUE)
        return MMSYSERR_NOTSUPPORTED;
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
    WaveOut *dwave = (WaveOut *)hwo;
    if(WaveOut::find(dwave) == TRUE)
         return MMSYSERR_NOTSUPPORTED;
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
    WaveOut *dwave = (WaveOut *)hwo;
    if(WaveOut::find(dwave) == TRUE)
    {
        if(pdwVolume!=NULL)
            *pdwVolume=dwave->getVolume();
        return MMSYSERR_NOERROR;
    }
    else return MMSYSERR_NOERROR;
//    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
    WaveOut *dwave = (WaveOut *)hwo;
    if(WaveOut::find(dwave) == TRUE)
    {
        return(dwave->setVolume(dwVolume));
    }
    if(hwo == NULL) {
        WaveOut::setDefaultVolume(dwVolume);
    }
    return MMSYSERR_NOERROR;
//    return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
    WaveOut *dwave = (WaveOut *)hwo;

    dprintf(("waveOutGetPlaybackRate: NOT IMPLEMENTED!!"));
    if(WaveOut::find(dwave) == TRUE)
    {
        return MMSYSERR_NOTSUPPORTED;
    }
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
    WaveOut *dwave = (WaveOut *)hwo;

    dprintf(("waveOutSetPlaybackRate: NOT IMPLEMENTED!!"));
    if(WaveOut::find(dwave) == TRUE)
    {
        return MMSYSERR_NOTSUPPORTED;
    }
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
{
    WaveOut *dwave = (WaveOut *)hwo;

    dprintf(("waveOutMessage: NOT IMPLEMENTED!!"));
    if(WaveOut::find(dwave) == TRUE)
    {
        return MMSYSERR_NOTSUPPORTED;
    }
    else
        return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/


