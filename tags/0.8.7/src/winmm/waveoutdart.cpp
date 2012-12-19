/* $Id: waveoutdart.cpp,v 1.24 2004-03-16 11:20:40 sandervl Exp $ */

/*
 * Wave playback class (DART)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Note:
 * 2000/11/24 PH MCI_MIXSETUP_PARMS->pMixWrite does alter FS: selector!
 * TODO: mulaw, alaw & adpcm
 *
 */

/******************************************************************************/
// Includes
/******************************************************************************/

#define  INCL_BASE
#define  INCL_OS2MM
#define  INCL_DOSPROCESS
#include <os2wrap.h>   //Odin32 OS/2 api wrappers
#include <os2mewrap.h> //Odin32 OS/2 MMPM/2 api wrappers
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#define  OS2_ONLY
#include <win32api.h>
#include <wprocess.h>

#include "misc.h"
#include "waveoutdart.h"
#include "initterm.h"

#define DBG_LOCALLOG    DBG_waveoutdart
#include "dbglocal.h"

/******************************************************************************/

#ifndef min
#define min(a, b) ((a > b) ? b : a)
#endif

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

LONG APIENTRY WaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

#define DART_BUFCNT     64
#define DART_BUFSIZE    4096

/******************************************************************************/

//#define DEBUG_DUMP_PCM
#ifdef DEBUG_DUMP_PCM
#include <stdio.h>

typedef struct {
	int bits;
	int rate;
	int format;
	int numchan;
} REC_STRUCT;

FILE *pcmfile = NULL;
#endif

/******************************************************************************/

static BOOL fFixedWaveBufferSize = FALSE;

/******************************************************************************/
// ODIN_waveOutSetFixedBuffers
//
// Tell WINMM to use DART buffers of the same size as the first buffer delivered
// by waveOutWrite
//
// NOTE: This will only work in very specific cases; it is not a good general
//       purpose solution.
// 
/******************************************************************************/

void WIN32API ODIN_waveOutSetFixedBuffers()
{
    fFixedWaveBufferSize = TRUE;
}

/******************************************************************************/
/******************************************************************************/

DartWaveOut::DartWaveOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
                  : WaveOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    DeviceId        = 0;
    fMixerSetup     = FALSE;
    fUnderrun       = FALSE;
    curFillBuf      = 0;
    curPlayBuf      = 0;
    curFillPos      = 0;
    curPlayPos      = 0;
    ulBufSize       = DART_BUFSIZE;
    ulBufCount      = DART_BUFCNT;
    bytesPlayed     = 0;
    bytesCopied     = 0;
    bytesReturned   = 0;
    ulUnderrunBase  = 0;
    mixHandle       = 0;
    curhdr          = NULL;
    pmixWriteProc   = 0;
    MixBuffer       = 0;
    BufferParms     = 0;
}

/******************************************************************************/
/******************************************************************************/

DartWaveOut::~DartWaveOut()
{
    MCI_GENERIC_PARMS GenericParms = {0};

    State = STATE_STOPPED;

#ifdef DEBUG_DUMP_PCM
    if (pcmfile) fclose(pcmfile);
#endif

    if (DeviceId) {
        // Stop the playback.
        mymciSendCommand(DeviceId, MCI_STOP,
                         MCI_WAIT,
                         (PVOID)&GenericParms,0);

        if (fMixerSetup)
            mymciSendCommand(DeviceId, MCI_BUFFER,
                             MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                             (PVOID)&BufferParms, 0);

        // Close the device
        mymciSendCommand(DeviceId, MCI_CLOSE,
                         MCI_WAIT,
                         (PVOID)&GenericParms, 0);

        callback(WOM_CLOSE, 0, 0);
    }

    if (MixBuffer)
        free(MixBuffer);
    if (BufferParms)
        free(BufferParms);
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::open()
{
    MCI_AMP_OPEN_PARMS AmpOpenParms;
    MCI_GENERIC_PARMS  GenericParms = {0};
    MCI_MIXSETUP_PARMS MixSetupParms;
    APIRET rc;

    MixBuffer     = (MCI_MIX_BUFFER *)malloc(ulBufCount*sizeof(MCI_MIX_BUFFER));
    BufferParms   = (MCI_BUFFER_PARMS *)malloc(sizeof(MCI_BUFFER_PARMS));
    if (!MixBuffer || !BufferParms) {
        dprintf(("ERROR: malloc failed!!"));
        return MMSYSERR_NOMEM;
    }

    // Setup the open structure, then open the device
    memset(&AmpOpenParms, 0, sizeof(AmpOpenParms));
    AmpOpenParms.usDeviceID = 0;
    AmpOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_AUDIO_AMPMIX;

    rc = mymciSendCommand(0, MCI_OPEN,
                          MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                          (PVOID)&AmpOpenParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        dprintf(("MCI_OPEN failed\n"));
        mciError(rc);
        return MMSYSERR_NODRIVER;
    }
    DeviceId = AmpOpenParms.usDeviceID;

    // Grab exclusive rights to device instance (NOT entire device)
    rc = mymciSendCommand(DeviceId, MCI_ACQUIREDEVICE,
                          MCI_EXCLUSIVE_INSTANCE,
                          (PVOID)&GenericParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        dprintf(("MCI_ACQUIREDEVICE failed\n"));
        mciError(rc);
        return MMSYSERR_NOTENABLED;
    }
    dprintf(("device acquired\n"));
    dprintf(("bps %d, sps %d chan %d\n", BitsPerSample, SampleRate, nChannels));

    // Setup the mixer for playback of wave data
    memset(&MixSetupParms, 0, sizeof(MixSetupParms));
    MixSetupParms.ulBitsPerSample = BitsPerSample;
    MixSetupParms.ulSamplesPerSec = SampleRate;
    MixSetupParms.ulFormatTag     = MCI_WAVE_FORMAT_PCM;
    MixSetupParms.ulChannels      = nChannels;
    MixSetupParms.ulFormatMode    = MCI_PLAY;
    MixSetupParms.ulDeviceType    = MCI_DEVTYPE_WAVEFORM_AUDIO;
    MixSetupParms.pmixEvent       = WaveOutHandler;

    rc = mymciSendCommand(DeviceId, MCI_MIXSETUP,
                          MCI_WAIT | MCI_MIXSETUP_INIT,
                          (PVOID)&MixSetupParms, 0);

    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        mciError(rc);
        mymciSendCommand(DeviceId, MCI_RELEASEDEVICE,
                         MCI_WAIT,
                         (PVOID)&GenericParms, 0);
        return MMSYSERR_NOTSUPPORTED;
    }

    // Save the mixer handle & the ptr to the write proc.
    mixHandle     = MixSetupParms.ulMixHandle;
    pmixWriteProc = MixSetupParms.pmixWrite;

#ifdef DEBUG_DUMP_PCM
    REC_STRUCT recinfo;

    pcmfile = fopen("dartpcm.dat", "wb");
    recinfo.bits = BitsPerSample;
    recinfo.rate = SampleRate;
    recinfo.format = MCI_WAVE_FORMAT_PCM;
    recinfo.numchan = nChannels;
    fwrite(&recinfo, sizeof(recinfo), 1, pcmfile);
#endif

    setVolume(volume);
    callback(WOM_OPEN, 0, 0);

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::write(LPWAVEHDR pwh, UINT cbwh)
{
    APIRET rc;

    queuedbuffers++;
    pwh->lpNext = NULL;
    pwh->reserved = 0;

    // Set up the BufferParms data structure and allocate
    // device buffers from the Amp-Mixer
    if (fMixerSetup == FALSE)
    {
        ulBufSize = pwh->dwBufferLength;
        if (!ulBufSize || ulBufSize > 0x10000)
            return MMSYSERR_INVALPARAM;

        rc = initBuffers();
        if (rc != MMSYSERR_NOERROR)
            return rc;

        curhdr = pwh;
        fMixerSetup = TRUE;
    }

    wmutex.enter();

    if (wavehdr) {
        WAVEHDR *chdr = wavehdr;
        while (chdr->lpNext) {
#ifdef DEBUG
            if (chdr == pwh) dprintf(("adding already present buffer!!!!!"));
#endif
            chdr = chdr->lpNext;
        }
        chdr->lpNext = pwh;
    }
    else wavehdr = pwh;

    //don't start playback if paused
    if (!fUnderrun && State != STATE_STOPPED) {
       	//write new data to the DART buffers (if there's any room left)
        if (State == STATE_PLAYING)
            writeBuffer();  //must be called before (re)starting playback
        wmutex.leave();
        return MMSYSERR_NOERROR;
    }

    writeBuffer();  //must be called before (re)starting playback
    State     = STATE_PLAYING;
    fUnderrun = FALSE;
    wmutex.leave();

    //write buffers to DART; starts playback
    dprintf(("WINMM: transfer all buffers to DART"));

    USHORT selTIB = RestoreOS2FS(); // save current FS selector
    pmixWriteProc(mixHandle, MixBuffer, ulBufCount);
    SetFS(selTIB);           // switch back to the saved FS selector

    dprintf(("Dart playing\n"));

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::initBuffers()
{
    APIRET rc;
    MCI_GENERIC_PARMS  GenericParms = {0};
    int orgbufsize = ulBufSize;  // on entry, ulBufSize == pwh->dwBufferLength

#if 1
    //by default we need to select a small enough buffer
    //to be able to signal buffer completion in time 
    if (fFixedWaveBufferSize == FALSE) 
    {
        int consumerate = getAvgBytesPerSecond();
        int minbufsize = consumerate/32;

        ulBufSize /= 2;
        if (ulBufSize > minbufsize) {
            dprintf(("set buffer size to %d bytes (org size = %d)", minbufsize, orgbufsize));
            ulBufSize = minbufsize;
        }
    }
#else
    if (ulBufSize < 512 || ulBufSize > 1024)
        ulBufSize = 1024;
#endif

    dprintf(("buffer setup - WAVE size = %d, DART size = %d\n",
             orgbufsize, ulBufSize));

    BufferParms->ulNumBuffers = ulBufCount;
    BufferParms->ulBufferSize = ulBufSize;
    BufferParms->pBufList     = MixBuffer;

    rc = mymciSendCommand(DeviceId, MCI_BUFFER,
                        MCI_WAIT | MCI_ALLOCATE_MEMORY,
                        (PVOID)BufferParms, 0);

    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        mciError(rc);
        mymciSendCommand(DeviceId, MCI_RELEASEDEVICE,
                         MCI_WAIT,
                         (PVOID)&GenericParms, 0);
        return MMSYSERR_NOTSUPPORTED;
    }

    // DART may not have allocated all the buffers requested.
    ulBufCount = BufferParms->ulNumBuffers;

    for (int i = 0; i < ulBufCount; i++) {
        MixBuffer[i].ulUserParm = (ULONG)this;
        memset(MixBuffer[i].pBuffer, 0, MixBuffer[i].ulBufferLength);
    }
    dprintf(("Dart opened, bufsize = %d\n", MixBuffer[0].ulBufferLength));

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::pause()
{
    MCI_GENERIC_PARMS  GenericParms = {0};

    dprintf(("WINMM: DartWaveOut::pause"));

    wmutex.enter();
    if (State != STATE_PLAYING) {
        State = STATE_PAUSED;
        wmutex.leave();
        return MMSYSERR_NOERROR;
    }

    State = STATE_PAUSED;
    wmutex.leave();

    // Pause playback.
    mymciSendCommand(DeviceId, MCI_PAUSE,
                     MCI_WAIT,
                     (PVOID)&GenericParms, 0);

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::resume()
{
    int i, curbuf;

    dprintf(("DartWaveOut::resume"));
    
    wmutex.enter();
    if (State != STATE_PAUSED) {
        wmutex.leave();
        return MMSYSERR_NOERROR;
    }
    State = STATE_PLAYING;
    wmutex.leave();

    //Only write buffers to dart if mixer has been initialized; if not, then
    //the first buffer write will do this for us.
    if (fMixerSetup == TRUE)
    {
        wmutex.enter();
        State     = STATE_PLAYING;
        fUnderrun = FALSE;
        curbuf    = curPlayBuf;
        writeBuffer();  //must be called before (re)starting playback
        wmutex.leave();

        USHORT selTIB = GetFS(); // save current FS selector
        for (i = 0; i < ulBufCount; i++)
        {
            dprintf(("restart: write buffer at %x size %d", MixBuffer[curbuf].pBuffer, MixBuffer[curbuf].ulBufferLength));
            pmixWriteProc(mixHandle, &MixBuffer[curbuf], 1);
            if (++curbuf == ulBufCount)
                curbuf = 0;
        }
        SetFS(selTIB);           // switch back to the saved FS selector
    }
    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::stop()
{
    MCI_GENERIC_PARMS  GenericParms = {0};

    dprintf(("DartWaveOut::stop %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
    if (State != STATE_PLAYING)
        return MMSYSERR_HANDLEBUSY;

    // Stop the playback.
    mymciSendCommand(DeviceId, MCI_STOP,
                     MCI_WAIT,
                     (PVOID)&GenericParms, 0);

    State     = STATE_STOPPED;
    fUnderrun = FALSE;

    curPlayBuf = curFillBuf = curFillPos = curPlayPos = 0;
    bytesPlayed = bytesCopied = bytesReturned = 0;

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::reset()
{
    MCI_GENERIC_PARMS  GenericParms = {0};
    LPWAVEHDR tmpwavehdr;

    dprintf(("DartWaveOut::reset %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
    if (State != STATE_PLAYING)
        return MMSYSERR_HANDLEBUSY;

    wmutex.enter();
    State = STATE_STOPPED;
    wmutex.leave();

    // Stop the playback.
    mymciSendCommand(DeviceId, MCI_STOP,
                     MCI_WAIT,
                     (PVOID)&GenericParms, 0);

    wmutex.enter();
    while (wavehdr)
    {
        wavehdr->dwFlags  |= WHDR_DONE;
        wavehdr->dwFlags  &= ~WHDR_INQUEUE;
        wavehdr->reserved  = 0;
        tmpwavehdr         = wavehdr;
        wavehdr            = wavehdr->lpNext;
        tmpwavehdr->lpNext = NULL;
        wmutex.leave();

        callback(WOM_DONE, (ULONG)tmpwavehdr, 0);
        wmutex.enter();
    }
    wavehdr        = NULL;
    fUnderrun      = FALSE;
    ulUnderrunBase = 0;

    curPlayBuf     = curFillBuf = curFillPos = curPlayPos = 0;
    bytesPlayed    = bytesCopied = bytesReturned = 0;
    queuedbuffers  = 0;

    wmutex.leave();
    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

ULONG DartWaveOut::getPosition()
{
    MCI_STATUS_PARMS StatusParms = {0};
    ULONG rc, nrbytes;

    if (State == STATE_STOPPED) {
        dprintf(("Not playing; return 0 position"));
        return ulUnderrunBase;
    }

    StatusParms.ulItem = MCI_STATUS_POSITION;
    rc = mymciSendCommand(DeviceId, MCI_STATUS,
                          MCI_STATUS_ITEM|MCI_WAIT,
                          (PVOID)&StatusParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        mciError(rc);
        return 0xFFFFFFFF;
    }

    nrbytes = (ULONG)(((double)StatusParms.ulReturn * (double)getAvgBytesPerSecond())/1000.0);
    return (ulUnderrunBase + nrbytes);
}

/******************************************************************************/
/******************************************************************************/

MMRESULT DartWaveOut::setVolume(ULONG ulVol)
{
    APIRET  rc;
    ULONG   ulVolR   = ((ulVol >> 16)    * 100) / 0xFFFF;
    ULONG   ulVolL   = ((ulVol & 0xffff) * 100) / 0xFFFF;
    MCI_SET_PARMS SetParms = {0};

    dprintf(("DartWaveOut::setVolume %d %d", ulVolL, ulVolR));
    volume = ulVol;

// Some drivers can't set left & right volumes independently
#ifdef GOOD_AUDIO_CARD_DRIVER
    SetParms.ulAudio = MCI_SET_AUDIO_LEFT;
    SetParms.ulLevel = ulVolL;

    rc = mymciSendCommand(DeviceId, MCI_SET,
                          MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                          &SetParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS)
        mciError(rc);

    SetParms.ulAudio = MCI_SET_AUDIO_RIGHT;
    SetParms.ulLevel = ulVolR;
#else
    SetParms.ulAudio = MCI_SET_AUDIO_ALL;
    SetParms.ulLevel = (ulVolR + ulVolL) / 2;
#endif

    rc = mymciSendCommand(DeviceId, MCI_SET,
                          MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                          &SetParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS)
        mciError(rc);

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

void DartWaveOut::mciError(ULONG rc)
{
#ifdef DEBUG
    char szError[256] = "";

    mymciGetErrorString(rc, szError, sizeof(szError));
    dprintf(("WINMM: DartWaveOut: %s\n", szError));
#endif
}

//******************************************************************************
//******************************************************************************

void DartWaveOut::handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
    ULONG    buflength;
    WAVEHDR *whdr, *prevhdr = NULL;

    dprintf2(("WINMM: handler %d; buffers left %d", curPlayBuf, queuedbuffers));
    if (ulFlags == MIX_STREAM_ERROR) {
        if (ulStatus == ERROR_DEVICE_UNDERRUN) {
            dprintf(("WINMM: WaveOut handler UNDERRUN! state %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
            if (State == STATE_PLAYING) {
                fUnderrun = TRUE;
                //save current position for when we continue later
                ulUnderrunBase = bytesPlayed;
                stop();    //out of buffers, so stop playback
            }
            return;
        }
        dprintf(("WINMM: WaveOut handler, Unknown error %X\n", ulStatus));
        return;
    }
    if (State != STATE_PLAYING)
        return;

    wmutex.enter();

    bytesPlayed += MixBuffer[curPlayBuf].ulBufferLength;

    // update our buffer index
    if (++curPlayBuf >= ulBufCount)
        curPlayBuf = 0;

    fUnderrun = FALSE;

    whdr = wavehdr;
    while (whdr) {
        if (whdr->reserved != WHDR_DONE)
            break;

        if (bytesPlayed < bytesReturned + whdr->dwBufferLength) {
            dprintf2(("Buffer marked done, but not yet played completely (play %d/%d, cop %d, ret %d)", bytesPlayed, getPosition(), bytesCopied, bytesReturned));
            break;  //not yet done
        }

        dprintf2(("WINMM: handler buf %X done (play %d/%d, cop %d, ret %d)", whdr, bytesPlayed, getPosition(), bytesCopied, bytesReturned));
        queuedbuffers--;

        whdr->dwFlags  &= ~WHDR_INQUEUE;
        whdr->dwFlags  |= WHDR_DONE;
        whdr->reserved  = 0;

        if (prevhdr == NULL)
             wavehdr = whdr->lpNext;
        else prevhdr->lpNext = whdr->lpNext;

        whdr->lpNext = NULL;

        bytesReturned += whdr->dwBufferLength;

        wmutex.leave();
        callback(WOM_DONE, (ULONG)whdr, 0);
        wmutex.enter();

        prevhdr = whdr;
        whdr    = whdr->lpNext;
    }

    if (wavehdr == NULL) {
        // last buffer played -> no new ones -> return now
        dprintf(("WINMM: WaveOut handler LAST BUFFER PLAYED! state %s (play %d (%d), cop %d, ret %d)", (State == STATE_PLAYING) ? "playing" : "stopped", bytesPlayed, getPosition(), bytesCopied, bytesReturned));
        if (getPosition() > bytesPlayed) {
            dprintf(("WINMM: WaveOut handler UNDERRUN! state %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
            //save current position for when we continue later
            ulUnderrunBase = bytesPlayed;
            fUnderrun = TRUE;
            stop();    //out of buffers, so stop playback
        }
        wmutex.leave();
        return;
    }

    writeBuffer();

    wmutex.leave();

    //Transfer the buffer we just finished playing to DART
    dprintf2(("WINMM: handler transfer buffer %d", pBuffer - MixBuffer));
    USHORT selTIB = RestoreOS2FS(); // save current FS selector
    pmixWriteProc(mixHandle, pBuffer, 1);
    SetFS(selTIB);           // switch back to the saved FS selector

    dprintf2(("WINMM: handler DONE"));
}

/******************************************************************************/
/******************************************************************************/

void DartWaveOut::writeBuffer()
{
    ULONG buflength;

    if (!fUnderrun && State == STATE_PLAYING && wavehdr == NULL && curFillBuf == curPlayBuf) {
        dprintf2(("writeBuffer: no more room for more audio data"));
        return; //no room left
    }

    if (curhdr == NULL)
        curhdr = wavehdr;

    while (curhdr && (curhdr->reserved == WHDR_DONE))  {
        curhdr = curhdr->lpNext;
    }

    if (curhdr == NULL)
        return;     //no unprocessed buffers left

    if (State == STATE_PLAYING && curFillBuf == curPlayBuf)
    {
        dprintf(("curFillBuf == curPlayBuf; no more room (%d,%d)", curFillBuf, curPlayBuf));
        return;  //no more room left
    }

    dprintf2(("WINMM: handler cur (%d,%d), fill (%d,%d)\n", curPlayBuf, curPlayPos, curFillBuf, curFillPos));

    while (curhdr) {
        buflength = min((ULONG)MixBuffer[curFillBuf].ulBufferLength - curPlayPos,
                        (ULONG)curhdr->dwBufferLength - curFillPos);
        dprintf2(("WINMM: copied %d bytes, cufFillPos = %d, curPlayPos = %d, dwBufferLength = %d\n", buflength, curFillPos, curPlayPos, curhdr->dwBufferLength));

#ifdef DEBUG_DUMP_PCM
        fwrite(curhdr->lpData + curFillPos, buflength, 1, pcmfile);
#endif

        memcpy((char *)MixBuffer[curFillBuf].pBuffer + curPlayPos,
               curhdr->lpData + curFillPos,  buflength);
        curPlayPos  += buflength;
        curFillPos  += buflength;
        bytesCopied += buflength;

        if (curFillPos == curhdr->dwBufferLength) {
            dprintf2(("Buffer %d done ptr %x size %d %x %x %x %x %x %x", curFillBuf, curhdr->lpData, curhdr->dwBufferLength, curhdr->dwBytesRecorded, curhdr->dwUser, curhdr->dwFlags, curhdr->dwLoops, curhdr->lpNext, curhdr->reserved));

            curFillPos = 0;
            curhdr->reserved = WHDR_DONE;
            //search for next unprocessed buffer
            while (curhdr && (curhdr->reserved == WHDR_DONE))
                curhdr = curhdr->lpNext;
        }
        if (curPlayPos == MixBuffer[curFillBuf].ulBufferLength) {
            curPlayPos = 0;

            if (++curFillBuf >= ulBufCount)
                curFillBuf = 0;

            if (curFillBuf == curPlayBuf)
                break;  //no more room left
        }
    }
}

/******************************************************************************/
/******************************************************************************/

LONG APIENTRY WaveOutHandler(ULONG ulStatus,
                             PMCI_MIX_BUFFER pBuffer,
                             ULONG ulFlags)
{
    PTIB2 ptib2;
    DartWaveOut *dwave;

    dprintf2(("WaveOutHandler %x %x %x", ulStatus, pBuffer, ulFlags));

#ifdef __IBMC__
    ptib2 = (PTIB2)_getTIBvalue(offsetof(TIB, tib_ptib2));
#else
    PTIB ptib;
    DosGetInfoBlocks(&ptib, NULL);
    ptib2 = ptib->tib_ptib2;
#endif

    if (ptib2 && HIBYTE(ptib2->tib2_ulpri) != PRTYC_TIMECRITICAL && 
       LOBYTE(ptib2->tib2_ulpri) != PRTYD_MAXIMUM) 
    {
        dprintf(("Setting priority of DART thread to PRTYC_TIMECRITICAL/PRTYD_MAXIMUM"));
        DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, 0);
    }
    if (pBuffer && pBuffer->ulUserParm)
    {
        dwave = (DartWaveOut *)pBuffer->ulUserParm;
        dwave->handler(ulStatus, pBuffer, ulFlags);
    }
    return TRUE;
}

/******************************************************************************/
/******************************************************************************/

