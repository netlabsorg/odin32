/* $Id: waveoutdaud.cpp,v 1.5 2001-04-30 21:06:56 sandervl Exp $ */

/*
 * Wave playback class (DirectAudio)
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/



#define  INCL_BASE
#define  INCL_OS2MM
#include <os2wrap.h>   //Odin32 OS/2 api wrappers
#include <os2mewrap.h> //Odin32 OS/2 MMPM/2 api wrappers
#include <stdlib.h>
#include <string.h>
#define  OS2_ONLY
#include <win32api.h>
#include <wprocess.h>
#include <audio.h>
#include <daudio.h>
#include <options.h>

#include "misc.h"
#include "waveoutdaud.h"

#define DBG_LOCALLOG    DBG_waveoutdaud
#include "dbglocal.h"

DWORD WIN32API DAudioThreadHandler(LPVOID pUserData);

//TODO: mulaw, alaw & adpcm
/******************************************************************************/
/******************************************************************************/
DAudioWaveOut::DAudioWaveOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
                  : WaveOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    APIRET          rc;
    ULONG           action;
    HFILE           hDriver;
    MCI_AUDIO_INIT  init = {0};
    DAUDIO_CMD      cmd;
    ULONG           ParmLength = 0, DataLength;

    fUnderrun = FALSE;
    hSem      = 0;

    rc = DosOpen("DAUDIO1$", &hDAudioDrv, &action, 0,
                 FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE |
                 OPEN_SHARE_DENYNONE | OPEN_FLAGS_WRITE_THROUGH,
                 NULL );
    if(rc) {
        dprintf(("DosOpen failed with error %d\n", rc));
        ulError = MMSYSERR_NODRIVER;
        goto fail;
    }

    DataLength = sizeof(init);

    init.lSRate         = pwfx->nSamplesPerSec;
    init.lBitsPerSRate  = pwfx->wBitsPerSample;
    init.sChannels      = pwfx->nChannels;
    init.sMode          = PCM;  //todo!!

    rc = DosDevIOCtl(hDAudioDrv, DAUDIO_IOCTL_CAT, DAUDIO_OPEN, NULL, 0,
                     &ParmLength, &init, DataLength, &DataLength);
    if(rc) {
        dprintf(("DosDevIOCtl failed with error %d\n", rc));
        ulError = MMSYSERR_NODRIVER;
        goto fail;
    }
    if(init.sReturnCode != 0) {
        dprintf(("init.sReturnCode = %d\n", init.sReturnCode));
        ulError = MMSYSERR_NODRIVER;
        goto fail;
    }

    rc = DosCreateEventSem(NULL, &hSem, DC_SEM_SHARED, 0);
    if(rc) {
        dprintf(("DosCreateEventSem failed with error %d\n", rc));
        ulError = MMSYSERR_NODRIVER;
        goto fail;
    }
    cmd.Thread.hSemaphore = hSem;
    rc = DosDevIOCtl(hDAudioDrv, DAUDIO_IOCTL_CAT, DAUDIO_REGISTER_THREAD, NULL, 0,
                     &ParmLength, &cmd, DataLength, &DataLength);
    if(rc) {
        dprintf(("DosDevIOCtl failed with error %d\n", rc));
        ulError = MMSYSERR_NODRIVER;
        goto fail;
    }

    hThread = CreateThread(NULL, 0x4000, (LPTHREAD_START_ROUTINE)DAudioThreadHandler,
                           (LPVOID)this, 0, &dwThreadID);

    setVolume(volume);

    if(!ulError)
        callback(WOM_OPEN, 0, 0);

fail:
    return;
}
/******************************************************************************/
/******************************************************************************/
DAudioWaveOut::~DAudioWaveOut()
{
    DAUDIO_CMD cmd;

    if(!ulError)
        callback(WOM_CLOSE, 0, 0);

    if(hDAudioDrv) {
        cmd.Thread.hSemaphore = hSem;
        sendIOCTL(DAUDIO_DEREGISTER_THREAD, &cmd);
        sendIOCTL(DAUDIO_CLOSE, &cmd);
        DosClose(hDAudioDrv);
        hDAudioDrv = 0;
    }
    if(hSem) {
        DosPostEventSem(hSem);
        DosCloseEventSem(hSem);
    }
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::write(LPWAVEHDR pwh, UINT cbwh)
{
    DAUDIO_CMD cmd;

    queuedbuffers++;
    pwh->lpNext   = NULL;
    pwh->reserved = 0;
    wmutex.enter(VMUTEX_WAIT_FOREVER);
    if(wavehdr) {
        WAVEHDR *chdr = wavehdr;
        while(chdr->lpNext) {
            chdr = chdr->lpNext;
        }
        chdr->lpNext = pwh;
    }
    else wavehdr = pwh;

    wmutex.leave();

    cmd.Buffer.lpBuffer       = (ULONG)pwh->lpData;
    cmd.Buffer.ulBufferLength = pwh->dwBufferLength;
    if(sendIOCTL(DAUDIO_ADDBUFFER, &cmd)) {
        dprintf(("Unable to add buffer!!!!!"));
        return MMSYSERR_ERROR;
    }

    if(State == STATE_STOPPED) {//continue playback
        restart();
    }
    else
    if(fUnderrun) {
        dprintf(("Resume playback after underrun"));
        fUnderrun = FALSE;
        State = STATE_PLAYING;

        // Resume the playback.
        resume();
    }
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::pause()
{
    DAUDIO_CMD cmd;

    dprintf(("WINMM: DAudioWaveOut::pause"));

    // Pause the playback.
    sendIOCTL(DAUDIO_PAUSE, &cmd);

    wmutex.enter(VMUTEX_WAIT_FOREVER);
    if(State != STATE_PLAYING) {
        State = STATE_PAUSED;
        wmutex.leave();
        return(MMSYSERR_NOERROR);
    }

    State = STATE_PAUSED;
    wmutex.leave();

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::resume()
{
    DAUDIO_CMD cmd;

    return sendIOCTL(DAUDIO_RESUME, &cmd);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::stop()
{
    DAUDIO_CMD cmd;
    MMRESULT   rc;

    dprintf(("DAudioWaveOut::stop %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
    if(State != STATE_PLAYING)
        return(MMSYSERR_HANDLEBUSY);

    // Stop the playback.
    rc = sendIOCTL(DAUDIO_STOP, &cmd);

    State     = STATE_STOPPED;
    fUnderrun = FALSE;

    bytesPlayed = bytesCopied = bytesReturned = 0;

    return rc;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::reset()
{
    DAUDIO_CMD cmd;
    LPWAVEHDR tmpwavehdr;

    dprintf(("DAudioWaveOut::reset %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
    if(State != STATE_PLAYING)
        return(MMSYSERR_HANDLEBUSY);

    // Stop the playback.
    sendIOCTL(DAUDIO_STOP, &cmd);

    wmutex.enter(VMUTEX_WAIT_FOREVER);
    while(wavehdr)
    {
        wavehdr->dwFlags  |= WHDR_DONE;
        wavehdr->dwFlags  &= ~WHDR_INQUEUE;
        wavehdr->reserved  = 0;
        tmpwavehdr         = wavehdr;
        wavehdr            = wavehdr->lpNext;
        tmpwavehdr->lpNext = NULL;
        wmutex.leave();

        callback(WOM_DONE, (ULONG)tmpwavehdr, 0);
        wmutex.enter(VMUTEX_WAIT_FOREVER);
    }
    wavehdr   = NULL;
    State     = STATE_STOPPED;
    fUnderrun = FALSE;

    bytesPlayed = bytesCopied = bytesReturned = 0;
    queuedbuffers = 0;

    wmutex.leave();
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::restart()
{
    DAUDIO_CMD cmd;

    dprintf(("DAudioWaveOut::restart"));
    if(State == STATE_PLAYING)
        return(MMSYSERR_NOERROR);

    wmutex.enter(VMUTEX_WAIT_FOREVER);
    State     = STATE_PLAYING;
    fUnderrun = FALSE;
    wmutex.leave();

    return sendIOCTL(DAUDIO_START, &cmd);
}
/******************************************************************************/
/******************************************************************************/
ULONG DAudioWaveOut::getPosition()
{
    DAUDIO_CMD cmd;
    MMRESULT   rc;

    rc = sendIOCTL(DAUDIO_GETPOS, &cmd);
    if(rc) {
        return 0xFFFFFFFF;
    }
    return cmd.Pos.ulCurrentPos;
}
/******************************************************************************/
/******************************************************************************/
BOOL DAudioWaveOut::queryFormat(ULONG formatTag, ULONG nChannels, ULONG nSamplesPerSec, ULONG sampleSize)
{
    ULONG           ParmLength = 0, DataLength;
    MCI_AUDIO_INIT  init = {0};
    APIRET          rc;
    ULONG           action;
    HFILE           hDriver;

    rc = DosOpen("DAUDIO1$", &hDriver, &action, 0,
                 FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE |
                 OPEN_SHARE_DENYNONE | OPEN_FLAGS_WRITE_THROUGH,
                 NULL );
    if(rc) {
        dprintf(("DosOpen failed with error %d\n", rc));
        return FALSE;
    }

    DataLength = sizeof(init);

    init.lSRate         = nSamplesPerSec;
    init.lBitsPerSRate  = sampleSize;
    init.sChannels      = nChannels;
    init.sMode          = PCM;      //TODO: check formattag for ulaw/alaw/adpcm
    rc = DosDevIOCtl(hDriver, DAUDIO_IOCTL_CAT, DAUDIO_QUERYFORMAT, NULL, 0,
                     &ParmLength, &init, DataLength, &DataLength);

    if(rc) {
        dprintf(("DosDevIOCtl failed with error %d\n", rc));
        goto fail;
    }
    if(init.sReturnCode != 0) {
        dprintf(("init.sReturnCode = %d\n", init.sReturnCode));
        goto fail;
    }

    DosClose(hDriver);
    return TRUE;

fail:
    DosClose(hDriver);
    return FALSE;
}
/******************************************************************************/
/******************************************************************************/
BOOL DAudioWaveOut::isDirectAudioAvailable()
{
    static BOOL fAvailable = FALSE;
    static BOOL fTested    = FALSE;

    APIRET          rc;
    ULONG           action;
    HFILE           hDriver;

    if(!fTested) {
        if(PROFILE_GetOdinIniInt(SECTION_WINMM, KEY_DIRECTAUDIO, 1) == 0) {
            fTested = TRUE;
            return FALSE;
        }
        rc = DosOpen("DAUDIO1$", &hDriver, &action, 0,
                     FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE |
                     OPEN_SHARE_DENYNONE | OPEN_FLAGS_WRITE_THROUGH,
                     NULL );
        fTested = TRUE;
        if(rc) {
            return FALSE;
        }
        DosClose(hDriver);
        fAvailable = TRUE;
    }
    return fAvailable;

}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::setVolume(ULONG ulVol)
{
    DAUDIO_CMD cmd;

    //Scale down from 0-64k-1 -> 0-100
    cmd.Vol.VolumeR = (((ulVol & 0xFFFF0000) >> 16)*100)/0xFFFF;
    cmd.Vol.VolumeL =  ((ulVol & 0x0000FFFF)       *100)/0xFFFF;
    return sendIOCTL(DAUDIO_SETVOLUME, &cmd);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::sendIOCTL(ULONG cmd, DAUDIO_CMD *pDataPacket)
{
    ULONG DataLength, ParmLength = 0;
    APIRET rc;

    DataLength = sizeof(DAUDIO_CMD);

    rc = DosDevIOCtl(hDAudioDrv, DAUDIO_IOCTL_CAT, cmd, NULL, 0,
                     &ParmLength, pDataPacket, DataLength, &DataLength);
    if(rc) {
        dprintf(("DosDevIOCtl failed with error %d (command %d)", rc, cmd));
        return MMSYSERR_ERROR;
    }
    return(MMSYSERR_NOERROR);

}
/******************************************************************************/
//TODO: Not entirely safe. (assumption that we get called for each buffer so we
//      always notify the win32 app for each buffer that was processed)
/******************************************************************************/
BOOL DAudioWaveOut::handler()
{
 LPWAVEHDR whdr = wavehdr;

    dprintf2(("WINMM: handler buf %X done (play %d/%d, cop %d, ret %d)", whdr, bytesPlayed, getPosition(), bytesCopied, bytesReturned));

    if(State != STATE_PLAYING || whdr == NULL)
        return FALSE;

    wmutex.enter();
    queuedbuffers--;
    whdr->dwFlags  &= ~WHDR_INQUEUE;
    whdr->dwFlags  |= WHDR_DONE;
    whdr->reserved  = 0;
    wavehdr         = whdr->lpNext;
    whdr->lpNext    = NULL;

    bytesReturned  += whdr->dwBufferLength;
    wmutex.leave();

    callback(WOM_DONE, (ULONG)whdr, 0);
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
DWORD WIN32API DAudioThreadHandler(LPVOID pUserData)
{
 APIRET rc;
 ULONG  postcnt;
 HEV    hSem;
 BOOL   fResult = TRUE;

    DAudioWaveOut *dwave = (DAudioWaveOut *)pUserData;

    if(WaveOut::find(dwave) == FALSE) {
        dprintf(("DAudioThreadHandler: can't find waveout stream %x!!!", pUserData));
        return 0;
    }
    hSem = dwave->hSem;

    DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, 0);

    while(TRUE)
    {
        dprintf2(("Waiting for buffer notification\n"));
        rc = DosWaitEventSem(hSem, SEM_INDEFINITE_WAIT);
        if(rc) {
            dprintf(("DosWaitEventSem failed with error %d\n", rc));
            return 0;
        }

        rc = DosResetEventSem(hSem, &postcnt);
        if(rc) {
            dprintf(("DosWaitEventSem failed with error %d\n", rc));
            return 0;
        }
        if(WaveOut::find(dwave) == FALSE) {
            dprintf(("DAudioThreadHandler: can't find waveout stream %x", pUserData));
            break;
        }
        for(int i=0;i<postcnt;i++) {
            fResult = dwave->handler();
            if(fResult == FALSE) {
                break;
            }
        }
        if(fResult == FALSE) {
            dprintf(("DAudioThreadHandler: finished waveout stream %x", pUserData));
            break;
        }
    }
    return 0;
}
/******************************************************************************/
/******************************************************************************/

