/* $Id: waveoutdaud.cpp,v 1.2 2001-04-30 21:06:37 sandervl Exp $ */

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

#include "misc.h"
#include "waveoutdaud.h"
#include <options.h>

DWORD WIN32API DAudioThreadHandler(LPVOID pUserData);

//TODO: mulaw, alaw & adpcm
/******************************************************************************/
/******************************************************************************/
DAudioWaveOut::DAudioWaveOut(LPWAVEFORMATEX pwfx) : 
        ulError(0), hDAudioDrv(0), fUnderrun(FALSE), State(STATE_STOPPED)
{
    APIRET          rc;
    ULONG           action;
    HFILE           hDriver;
    MCI_AUDIO_INIT  init = {0};
    DAUDIO_CMD      cmd;
    ULONG           ParmLength = 0, DataLength;

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

    setVolume(volume);

fail:
    return;
}
/******************************************************************************/
/******************************************************************************/
DAudioWaveOut::~DAudioWaveOut()
{
    DAUDIO_CMD cmd;

    if(State != STATE_STOPPED) {
        stop();
    }
    if(hDAudioDrv) {
        sendIOCTL(DAUDIO_CLOSE, &cmd);
        DosClose(hDAudioDrv);
        hDAudioDrv = 0;
    }
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::write(LPVOID lpBuffer, UINT ulSize)
{
    DAUDIO_CMD cmd;

    cmd.Buffer.lpBuffer       = (ULONG)lpBuffer;
    cmd.Buffer.ulBufferLength = ulSize;
    if(sendIOCTL(DAUDIO_ADDBUFFER, &cmd)) {
        dprintf(("Unable to add buffer!!!!!"));
        return MMSYSERR_ERROR;
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

    if(State != STATE_PLAYING) {
        State = STATE_PAUSED;
        return(MMSYSERR_NOERROR);
    }
    State = STATE_PAUSED;

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

    return rc;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::restart()
{
    DAUDIO_CMD cmd;

    dprintf(("DAudioWaveOut::restart"));
    if(State == STATE_PLAYING)
        return(MMSYSERR_NOERROR);

    State     = STATE_PLAYING;
    fUnderrun = FALSE;

    return sendIOCTL(DAUDIO_START, &cmd);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DAudioWaveOut::setProperty(int type, ULONG value)
{
    DAUDIO_CMD cmd;

    dprintf(("DAudioWaveOut::setProperty %d %x", type, value));

    cmd.SetProperty.type  = type;
    cmd.SetProperty.value = type;
    return sendIOCTL(DAUDIO_SETPROPERTY, &cmd);
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
ULONG DAudioWaveOut::getPosition(PULONG pulWritePos)
{
    DAUDIO_CMD cmd;
    MMRESULT   rc;

    rc = sendIOCTL(DAUDIO_GETPOS, &cmd);
    if(rc) {
        return 0xFFFFFFFF;
    }
    *pulWritePos = cmd.Pos.ulWritePos;
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
/******************************************************************************/

