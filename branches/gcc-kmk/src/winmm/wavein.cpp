/* $Id: wavein.cpp,v 1.13 2003-07-16 15:47:24 sandervl Exp $ */

/*
 * Wave in MM apis
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * waveInGetPosition partly based on Wine code (dll\winmm\wineoss\audio.c)
 * Copyright 1994 Martin Ayotte
 *
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
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include <unicode.h>

#include "winmm.h"
#include "waveindart.h"
#include "initwinmm.h"

#define DBG_LOCALLOG    DBG_wavein
#include "dbglocal.h"

/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInOpen(LPHWAVEIN phwi, UINT uDeviceID, const LPWAVEFORMATEX pwfx,
                           DWORD  dwCallback, DWORD  dwInstance, DWORD  fdwOpen)
{
  MMRESULT rc;

    if(fMMPMAvailable == FALSE) return MMSYSERR_NODRIVER;

    if(pwfx == NULL)
        return(WAVERR_BADFORMAT);

    if(fdwOpen & WAVE_FORMAT_QUERY) {
        if(DartWaveIn::queryFormat(pwfx->wFormatTag, pwfx->nChannels, pwfx->nSamplesPerSec,
                                   pwfx->wBitsPerSample) == TRUE)
             return(MMSYSERR_NOERROR);

        return(WAVERR_BADFORMAT);
    }

    if(phwi == NULL)
        return(MMSYSERR_INVALPARAM);

    *phwi = (HWAVEOUT)new DartWaveIn(pwfx, fdwOpen, dwCallback, dwInstance);

    if(*phwi == NULL)
        return(MMSYSERR_NODRIVER);

    rc = ((DartWaveIn *)*phwi)->open();
    if(rc != MMSYSERR_NOERROR) {
        delete (DartWaveIn *)*phwi;
        return(rc);
    }
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInClose(HWAVEIN hwi)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(DartWaveIn::find(dwave) == TRUE)
    {
        if(dwave->getState() == STATE_RECORDING)
            return(WAVERR_STILLPLAYING);

        delete dwave;
        return(MMSYSERR_NOERROR);
    }
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInReset(HWAVEIN hwi)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(DartWaveIn::find(dwave) == TRUE)
         return(dwave->reset());
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInStart(HWAVEIN  hwi)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(DartWaveIn::find(dwave) == TRUE)
        return(dwave->start());
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInStop(
              HWAVEIN  hwi)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(DartWaveIn::find(dwave) == TRUE)
        return(dwave->stop());
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInGetPosition(HWAVEIN  hwi, LPMMTIME pmmt, UINT cbmmt)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(pmmt == NULL)
        return MMSYSERR_INVALPARAM;

    if(DartWaveIn::find(dwave) == TRUE)
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
            dprintf(("waveInGetPosition: Format %d not supported ! use TIME_MS !\n", pmmt->wType));
            pmmt->wType = TIME_MS;
        case TIME_MS:
            pmmt->u.ms = position / (dwave->getAvgBytesPerSecond() / 1000);
            dprintf(("WINMM:waveInGetPosition: TIME_MS pos=%d ms=%d time=%d", position, pmmt->u.ms, GetCurrentTime()));
            break;
        }
        return MMSYSERR_NOERROR;
    }
    else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInAddBuffer(HWAVEIN  hwi, LPWAVEHDR pwh, UINT cbwh)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(DartWaveIn::find(dwave) == TRUE)
    {
        if(!(pwh->dwFlags & WHDR_PREPARED) || pwh->lpData == NULL)
            return WAVERR_UNPREPARED;

        if(pwh->dwFlags & WHDR_INQUEUE)
            return WAVERR_STILLPLAYING;

        pwh->dwFlags |= WHDR_INQUEUE;
        pwh->dwFlags &= ~WHDR_DONE;

        dprintf(("waveInAddBuffer %x %d %x", pwh->lpData, pwh->dwBufferLength, pwh->dwFlags));
        return(dwave->addBuffer(pwh, cbwh));
    }
    else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInPrepareHeader(HWAVEIN  hwi, LPWAVEHDR pwh, UINT  cbwh)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(DartWaveIn::find(dwave) == TRUE)
    {
        if(pwh->dwFlags & WHDR_INQUEUE)
            return WAVERR_STILLPLAYING;

        pwh->dwFlags |= WHDR_PREPARED;
        pwh->dwFlags &= ~WHDR_DONE;
        pwh->lpNext   = NULL;
        return(MMSYSERR_NOERROR);
    }
    else return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInUnprepareHeader(HWAVEIN  hwi, LPWAVEHDR pwh, UINT  cbwh)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;

    if(DartWaveIn::find(dwave) == TRUE)
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
MMRESULT WINAPI waveInGetDevCapsA(UINT  uDeviceID, LPWAVEINCAPSA pwic, UINT  cbwic)
{
    if(fMMPMAvailable == FALSE) return MMSYSERR_NODRIVER;

    if(DartWaveIn::getNumDevices() == 0) {
        memset(pwic, 0, sizeof(*pwic));
        return MMSYSERR_NODRIVER;
    }

    // we have to fill in this thing
    pwic->wMid = 0;                  /* manufacturer ID */
    pwic->wPid = 0;                  /* product ID */
    pwic->vDriverVersion = 0x0001;        /* version of the driver */
    strcpy( pwic->szPname, "OS/2 DART Wave In" ); /* product name */
    pwic->dwFormats = WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08 |
                      WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16 |
                      WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08 |
                      WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16 |
                      WAVE_FORMAT_4M08 | WAVE_FORMAT_4S08 |
                      WAVE_FORMAT_4M16 | WAVE_FORMAT_4S16;

    pwic->wChannels  = 2;             /* number of sources supported */
    pwic->wReserved1 = 0;             /* packing */

    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInGetDevCapsW(UINT uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic)
{
    if(fMMPMAvailable == FALSE) return MMSYSERR_NODRIVER;

    if(DartWaveIn::getNumDevices() == 0) {
        memset(pwic, 0, sizeof(*pwic));
        return MMSYSERR_NODRIVER;
    }
    // we have to fill in this thing
    pwic->wMid = 0;                  /* manufacturer ID */
    pwic->wPid = 0;                  /* product ID */
    pwic->vDriverVersion = 0x0001;        /* version of the driver */
    lstrcpyW(pwic->szPname, (LPCWSTR)L"OS/2 DART Wave In"); /* product name */
    pwic->dwFormats = WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08 |
                      WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16 |
                      WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08 |
                      WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16 |
                      WAVE_FORMAT_4M08 | WAVE_FORMAT_4S08 |
                      WAVE_FORMAT_4M16 | WAVE_FORMAT_4S16;

    pwic->wChannels  = 2;             /* number of sources supported */
    pwic->wReserved1 = 0;             /* packing */

    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInGetErrorTextA(MMRESULT wError, LPSTR lpText, UINT cchText)
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
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInGetErrorTextW(MMRESULT wError, LPWSTR lpText, UINT cchText)
{
    dprintf(("WINMM:waveInGetErrorTextW(%d) - stub\n", wError ));
    char * theMsg = getWinmmMsg( wError );
    if ( theMsg )
        AsciiToUnicodeN( theMsg, lpText, cchText );
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
MMRESULT WINAPI waveInGetID(HWAVEIN hwi, LPUINT puDeviceID)
{
    DartWaveIn *dwave = (DartWaveIn *)hwi;
    if(DartWaveIn::find(dwave) == TRUE)
    {
        *puDeviceID = 1;
        return MMSYSERR_NOERROR;
    }
    else  return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/
UINT WINAPI waveInGetNumDevs()
{
    return DartWaveIn::getNumDevices();
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
    dprintf(("WINMM:waveInMessage - stub\n"));

    DartWaveIn *dwave = (DartWaveIn *)hwi;
    if(DartWaveIn::find(dwave) == TRUE)
    {
        return MMSYSERR_NOTSUPPORTED;
    }
    else
        return(MMSYSERR_INVALHANDLE);
}
/******************************************************************************/
/******************************************************************************/

