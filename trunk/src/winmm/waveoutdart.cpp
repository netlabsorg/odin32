/* $Id: waveoutdart.cpp,v 1.22 2003-04-03 14:04:50 sandervl Exp $ */

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


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/



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
#include "initwinmm.h"

#define DBG_LOCALLOG    DBG_waveoutdart
#include "dbglocal.h"

#ifndef min
#define min(a, b) ((a > b) ? b : a)
#endif

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

LONG APIENTRY WaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

static BOOL fFixedWaveBufferSize = FALSE;

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

//******************************************************************************
// ODIN_waveOutSetFixedBuffers
//
// Tell WINMM to use DART buffers of the same size as the first buffer delivered
// by waveOutWrite
//
// NOTE: This will only work in very specific cases; it is not a good general
//       purpose solution.
// 
//******************************************************************************
void WIN32API ODIN_waveOutSetFixedBuffers()
{
    fFixedWaveBufferSize = TRUE;
}
/******************************************************************************/
/******************************************************************************/
DartWaveOut::DartWaveOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
                  : WaveOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    MCI_GENERIC_PARMS  GenericParms;
    MCI_AMP_OPEN_PARMS AmpOpenParms;
    APIRET rc;

    curPlayBuf = curFillBuf = curFillPos = curPlayPos = 0;
    fMixerSetup   = FALSE;
    fUnderrun     = FALSE;
    ulUnderrunBase= 0;

    ulBufSize     = DART_BUFSIZE;

    MixBuffer     = (MCI_MIX_BUFFER *)malloc(PREFILLBUF_DART*sizeof(MCI_MIX_BUFFER));
    MixSetupParms = (MCI_MIXSETUP_PARMS *)malloc(sizeof(MCI_MIXSETUP_PARMS));
    BufferParms   = (MCI_BUFFER_PARMS *)malloc(sizeof(MCI_BUFFER_PARMS));
    if(!MixBuffer || !MixSetupParms || !BufferParms) {
        dprintf(("ERROR: malloc failed!!"));
        ulError = MMSYSERR_NOMEM;
        return;
    }

    // Setup the open structure, pass the playlist and tell MCI_OPEN to use it
    memset(&AmpOpenParms,0,sizeof(AmpOpenParms));

    AmpOpenParms.usDeviceID = ( USHORT ) 0;
    AmpOpenParms.pszDeviceType = ( PSZ ) MCI_DEVTYPE_AUDIO_AMPMIX;

    rc = mymciSendCommand(0, MCI_OPEN, MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                       (PVOID) &AmpOpenParms, 0);

    DeviceId = AmpOpenParms.usDeviceID;
    if(rc) {
        dprintf(("MCI_OPEN failed\n"));
        mciError(rc);
        ulError = MMSYSERR_NODRIVER;
    }
    if(rc == 0) {
        //Grab exclusive rights to device instance (NOT entire device)
        GenericParms.hwndCallback = 0;  //Not needed, so set to 0
        rc = mymciSendCommand(DeviceId, MCI_ACQUIREDEVICE, MCI_EXCLUSIVE_INSTANCE,
                            (PVOID)&GenericParms, 0);
        if(rc) {
            dprintf(("MCI_ACQUIREDEVICE failed\n"));
            mciError(rc);
            ulError = MMSYSERR_NOTENABLED;
        }
    }
    setVolume(volume);

    if(!ulError)
        callback(WOM_OPEN, 0, 0);
}
/******************************************************************************/
/******************************************************************************/
DartWaveOut::~DartWaveOut()
{
    MCI_GENERIC_PARMS GenericParms;

    State = STATE_STOPPED;

#ifdef DEBUG_DUMP_PCM
    if(pcmfile) fclose(pcmfile);
#endif

    if(!ulError) {
        // Generic parameters
        GenericParms.hwndCallback = 0;   //hwndFrame

        // Stop the playback.
        mymciSendCommand(DeviceId, MCI_STOP,MCI_WAIT, (PVOID)&GenericParms,0);

        mymciSendCommand(DeviceId, MCI_BUFFER,
                       MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                       (PVOID)&BufferParms, 0);

        // Generic parameters
        GenericParms.hwndCallback = 0;   //hwndFrame

        // Close the device
        mymciSendCommand(DeviceId, MCI_CLOSE, MCI_WAIT, (PVOID)&GenericParms, 0);
    }

    if(!ulError)
    {
        callback(WOM_CLOSE, 0, 0);
    }

    if(MixBuffer)
        free(MixBuffer);
    if(MixSetupParms)
        free(MixSetupParms);
    if(BufferParms)
        free(BufferParms);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::write(LPWAVEHDR pwh, UINT cbwh)
{
    MCI_GENERIC_PARMS GenericParms = {0};
    APIRET rc;
    int i, buflength;

    queuedbuffers++;
    if(fMixerSetup == FALSE)
    {
        dprintf(("device acquired\n"));
        /* Set the MixSetupParms data structure to match the loaded file.
         * This is a global that is used to setup the mixer.
         */
        memset(MixSetupParms, 0, sizeof( MCI_MIXSETUP_PARMS ) );

        MixSetupParms->ulBitsPerSample = BitsPerSample;
        MixSetupParms->ulSamplesPerSec = SampleRate;
        MixSetupParms->ulFormatTag     = MCI_WAVE_FORMAT_PCM;
        MixSetupParms->ulChannels      = nChannels;

        dprintf(("bps %d, sps %d chan %d\n", BitsPerSample, SampleRate, nChannels));

#ifdef DEBUG_DUMP_PCM
        REC_STRUCT recinfo;

        pcmfile = fopen("dartpcm.dat", "wb");
        recinfo.bits = BitsPerSample;
        recinfo.rate = SampleRate;
        recinfo.format = MCI_WAVE_FORMAT_PCM;
        recinfo.numchan = nChannels;
        fwrite(&recinfo, sizeof(recinfo), 1, pcmfile);
#endif
        /* Setup the mixer for playback of wave data
         */
        MixSetupParms->ulFormatMode = MCI_PLAY;
        MixSetupParms->ulDeviceType = MCI_DEVTYPE_WAVEFORM_AUDIO;
        MixSetupParms->pmixEvent    = WaveOutHandler;

        rc = mymciSendCommand(DeviceId,
                            MCI_MIXSETUP,
                            MCI_WAIT | MCI_MIXSETUP_INIT,
                            (PVOID)MixSetupParms,
                            0);

        if ( rc != MCIERR_SUCCESS ) {
            mciError(rc);
            mymciSendCommand(DeviceId, MCI_RELEASEDEVICE, MCI_WAIT,
                           (PVOID)&GenericParms, 0);
            return(MMSYSERR_NOTSUPPORTED);
        }

        /*
         * Set up the BufferParms data structure and allocate
         * device buffers from the Amp-Mixer
         */
        dprintf(("mix setup %d, %d\n", pwh->dwBufferLength, pwh->dwBufferLength));

#if 1
        if(fFixedWaveBufferSize == FALSE) 
        {//by default we need to select a small enough buffer to be able to
         //signal buffer completion in time 
            int consumerate = getAvgBytesPerSecond();
            int minbufsize = consumerate/32;

            ulBufSize = pwh->dwBufferLength/2;
            if(ulBufSize > minbufsize) {
                dprintf(("set buffer size to %d bytes (org size = %d)", minbufsize, pwh->dwBufferLength));
                ulBufSize = minbufsize;
            }
        }
        else ulBufSize = pwh->dwBufferLength;
#else
        if(pwh->dwBufferLength >= 512 && pwh->dwBufferLength <= 1024)
                ulBufSize = pwh->dwBufferLength;
        else    ulBufSize = 1024;
#endif
        MixSetupParms->ulBufferSize = ulBufSize;

        BufferParms->ulNumBuffers = PREFILLBUF_DART;
        BufferParms->ulBufferSize = MixSetupParms->ulBufferSize;
        BufferParms->pBufList     = MixBuffer;

        for(i=0;i<PREFILLBUF_DART;i++) {
            MixBuffer[i].ulUserParm = (ULONG)this;
        }

        rc = mymciSendCommand(DeviceId,
                            MCI_BUFFER,
                            MCI_WAIT | MCI_ALLOCATE_MEMORY,
                            (PVOID)BufferParms,
                            0);

        if(ULONG_LOWD(rc) != MCIERR_SUCCESS) {
            mciError(rc);
            mymciSendCommand(DeviceId, MCI_RELEASEDEVICE, MCI_WAIT,
                          (PVOID)&GenericParms, 0);
            return(MMSYSERR_NOTSUPPORTED);
        }

        wmutex.enter();
        fMixerSetup = TRUE;

        curPlayBuf = curFillBuf = curFillPos = curPlayPos = 0;
        bytesPlayed = bytesCopied = bytesReturned = 0;

        for(i=0;i<PREFILLBUF_DART;i++) {
            memset(MixBuffer[i].pBuffer, 0, MixBuffer[i].ulBufferLength);
        }
        dprintf(("Dart opened, bufsize = %d\n", MixBuffer[0].ulBufferLength));

        wavehdr     = pwh;
        curhdr      = pwh;
        pwh->lpNext = NULL;
        pwh->reserved = 0;

        if(State != STATE_STOPPED) {//don't start playback if paused
            wmutex.leave();
            return(MMSYSERR_NOERROR);
        }

        writeBuffer();  //must be called before (re)starting playback

        dprintf(("MixSetupParms = %X\n", MixSetupParms));
        State     = STATE_PLAYING;
        fUnderrun = FALSE;
        wmutex.leave();

        //write buffers to DART; starts playback
        dprintf(("WINMM: transfer all buffers to DART"));
        USHORT selTIB = RestoreOS2FS(); // save current FS selector

        MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle,
                                 MixBuffer,
                                 PREFILLBUF_DART);
        SetFS(selTIB);           // switch back to the saved FS selector
        dprintf(("Dart playing\n"));
    }
    else
    {
        pwh->lpNext   = NULL;
        pwh->reserved = 0;
        wmutex.enter();
        if(wavehdr) {
            WAVEHDR *chdr = wavehdr;
            while(chdr->lpNext) {
#ifdef DEBUG
                if(chdr == pwh) dprintf(("adding already present buffer!!!!!"));
#endif
                chdr = chdr->lpNext;
            }
            chdr->lpNext = pwh;
        }
        else wavehdr = pwh;

        if(!fUnderrun && State != STATE_STOPPED) {//don't start playback if paused
   	    //write new data to the DART buffers (if there's any room left)
            if(State == STATE_PLAYING) {
                writeBuffer();  //must be called before (re)starting playback
            }
            wmutex.leave();
            return(MMSYSERR_NOERROR);
        }
        writeBuffer();  //must be called before (re)starting playback
        State     = STATE_PLAYING;
        fUnderrun = FALSE;
        wmutex.leave();

        //write buffers to DART; starts playback
        dprintf(("WINMM: transfer all buffers to DART"));
        USHORT selTIB = RestoreOS2FS(); // save current FS selector

        MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle,
                                 MixBuffer,
                                 PREFILLBUF_DART);
        SetFS(selTIB);           // switch back to the saved FS selector

        dprintf(("Dart playing\n"));
    }
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::pause()
{
    MCI_GENERIC_PARMS Params;

    dprintf(("WINMM: DartWaveOut::pause"));

    wmutex.enter();
    if(State != STATE_PLAYING) {
        State = STATE_PAUSED;
        wmutex.leave();
        return(MMSYSERR_NOERROR);
    }

    State = STATE_PAUSED;
    wmutex.leave();

    memset(&Params, 0, sizeof(Params));

    // Pause playback.
    mymciSendCommand(DeviceId, MCI_PAUSE, MCI_WAIT, (PVOID)&Params, 0);

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::resume()
{
    MCI_GENERIC_PARMS Params;
    int i, curbuf;

    dprintf(("DartWaveOut::resume"));
    
    wmutex.enter();
    if(State != STATE_PAUSED) {
        wmutex.leave();
        return(MMSYSERR_NOERROR);
    }
    State = STATE_PLAYING;
    wmutex.leave();

    //Only write buffers to dart if mixer has been initialized; if not, then
    //the first buffer write will do this for us.
    if(fMixerSetup == TRUE)
    {
        wmutex.enter();
        State     = STATE_PLAYING;
        fUnderrun = FALSE;
        curbuf = curPlayBuf;
        writeBuffer();  //must be called before (re)starting playback
        wmutex.leave();

        // MCI_MIXSETUP_PARMS->pMixWrite does alter FS: selector!
        USHORT selTIB = GetFS(); // save current FS selector

        for(i=0;i<PREFILLBUF_DART;i++)
        {
            dprintf(("restart: write buffer at %x size %d", MixBuffer[curbuf].pBuffer, MixBuffer[curbuf].ulBufferLength));
            MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle, &MixBuffer[curbuf], 1);
            if(++curbuf == PREFILLBUF_DART)
                curbuf = 0;
        }
        SetFS(selTIB);           // switch back to the saved FS selector
    }
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::stop()
{
    MCI_GENERIC_PARMS Params;

    dprintf(("DartWaveOut::stop %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
    if(State != STATE_PLAYING)
        return(MMSYSERR_HANDLEBUSY);

    memset(&Params, 0, sizeof(Params));

    // Stop the playback.
    mymciSendCommand(DeviceId, MCI_STOP, MCI_WAIT, (PVOID)&Params, 0);

    State     = STATE_STOPPED;
    fUnderrun = FALSE;

    curPlayBuf = curFillBuf = curFillPos = curPlayPos = 0;
    bytesPlayed = bytesCopied = bytesReturned = 0;

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::reset()
{
    MCI_GENERIC_PARMS Params;
    LPWAVEHDR tmpwavehdr;

    dprintf(("DartWaveOut::reset %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
    if(State != STATE_PLAYING)
        return(MMSYSERR_HANDLEBUSY);

    memset(&Params, 0, sizeof(Params));

    wmutex.enter();
    State = STATE_STOPPED;
    wmutex.leave();

    // Stop the playback.
    mymciSendCommand(DeviceId, MCI_STOP, MCI_WAIT, (PVOID)&Params, 0);

    wmutex.enter();
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
        wmutex.enter();
    }
    wavehdr        = NULL;
    fUnderrun      = FALSE;
    ulUnderrunBase = 0;

    curPlayBuf     = curFillBuf = curFillPos = curPlayPos = 0;
    bytesPlayed    = bytesCopied = bytesReturned = 0;
    queuedbuffers  = 0;

    wmutex.leave();
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
ULONG DartWaveOut::getPosition()
{
 MCI_STATUS_PARMS mciStatus = {0};
 ULONG rc, nrbytes;

    if(State == STATE_STOPPED) {
        dprintf(("Not playing; return 0 position"));
        return ulUnderrunBase;
    }

    mciStatus.ulItem = MCI_STATUS_POSITION;
    rc = mymciSendCommand(DeviceId, MCI_STATUS, MCI_STATUS_ITEM|MCI_WAIT, (PVOID)&mciStatus, 0);
    if((rc & 0xFFFF) == MCIERR_SUCCESS) {
        nrbytes = (ULONG)(((double)mciStatus.ulReturn * (double)getAvgBytesPerSecond())/1000.0);
        return ulUnderrunBase+nrbytes;
    }
    mciError(rc);
    return 0xFFFFFFFF;
}
/******************************************************************************/
/******************************************************************************/
BOOL DartWaveOut::queryFormat(ULONG formatTag, ULONG nChannels,
                              ULONG nSamplesPerSec, ULONG wBitsPerSample)
{
 MCI_WAVE_GETDEVCAPS_PARMS mciAudioCaps;
 MCI_GENERIC_PARMS    GenericParms;
 MCI_OPEN_PARMS       mciOpenParms;         /* open parms for MCI_OPEN             */
 int i, freqbits = 0;
 ULONG rc, DeviceId;
 BOOL winrc;

    dprintf(("DartWaveOut::queryFormat %x srate=%d, nchan=%d, bps=%d", formatTag, nSamplesPerSec, nChannels, wBitsPerSample));

    memset(&mciOpenParms,            /* Object to fill with zeros.       */
           0,                        /* Value to place into the object.  */
           sizeof( mciOpenParms ) ); /* How many zero's to use.          */

    mciOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_WAVEFORM_AUDIO;

    rc = mymciSendCommand( (USHORT) 0,
                        MCI_OPEN,
                        MCI_WAIT | MCI_OPEN_TYPE_ID,
                        (PVOID) &mciOpenParms,
                        0);
    if((rc & 0xFFFF) != MCIERR_SUCCESS) {
        mciError(rc);
        return(FALSE);
    }
    DeviceId = mciOpenParms.usDeviceID;

    memset( &mciAudioCaps , 0, sizeof(MCI_WAVE_GETDEVCAPS_PARMS));

    mciAudioCaps.ulBitsPerSample = wBitsPerSample;
    mciAudioCaps.ulFormatTag     = DATATYPE_WAVEFORM;
    mciAudioCaps.ulSamplesPerSec = nSamplesPerSec;
    mciAudioCaps.ulChannels      = nChannels;
    mciAudioCaps.ulFormatMode    = MCI_PLAY;
    mciAudioCaps.ulItem          = MCI_GETDEVCAPS_WAVE_FORMAT;

    rc = mymciSendCommand(DeviceId,   /* Device ID    */
                        MCI_GETDEVCAPS,
                        MCI_WAIT | MCI_GETDEVCAPS_EXTENDED | MCI_GETDEVCAPS_ITEM,
                        (PVOID) &mciAudioCaps,
                        0);
    if((rc & 0xFFFF) != MCIERR_SUCCESS) {
        mciError(rc);
        winrc = FALSE;
    }
    else  winrc = TRUE;

    // Close the device
    mymciSendCommand(DeviceId,MCI_CLOSE,MCI_WAIT,(PVOID)&GenericParms,0);
    return(winrc);
}
/******************************************************************************/
/******************************************************************************/
void DartWaveOut::mciError(ULONG ulError)
{
#ifdef DEBUG
    char szError[256] = "";

    mymciGetErrorString(ulError, szError, sizeof(szError));
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
    if(ulFlags == MIX_STREAM_ERROR) {
        if(ulStatus == ERROR_DEVICE_UNDERRUN) {
            dprintf(("WINMM: WaveOut handler UNDERRUN! state %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
            if(State == STATE_PLAYING) {
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
    if(State != STATE_PLAYING) {
        return;
    }

    wmutex.enter();

    bytesPlayed += MixBuffer[curPlayBuf].ulBufferLength;

    //update our buffer index
    if(curPlayBuf == PREFILLBUF_DART-1)
         curPlayBuf = 0;
    else curPlayBuf++;

    fUnderrun = FALSE;

    whdr = wavehdr;
    while(whdr) {
        if(whdr->reserved == WHDR_DONE)
        {
            if(bytesPlayed < bytesReturned + whdr->dwBufferLength) {
                dprintf2(("Buffer marked done, but not yet played completely (play %d/%d, cop %d, ret %d)", bytesPlayed, getPosition(), bytesCopied, bytesReturned));
                break;  //not yet done
            }

            dprintf2(("WINMM: handler buf %X done (play %d/%d, cop %d, ret %d)", whdr, bytesPlayed, getPosition(), bytesCopied, bytesReturned));
            queuedbuffers--;

            whdr->dwFlags  &= ~WHDR_INQUEUE;
            whdr->dwFlags  |= WHDR_DONE;
            whdr->reserved  = 0;

            if(prevhdr == NULL)
                 wavehdr = whdr->lpNext;
            else prevhdr->lpNext = whdr->lpNext;

            whdr->lpNext = NULL;

            bytesReturned += whdr->dwBufferLength;
            wmutex.leave();

            callback(WOM_DONE, (ULONG)whdr, 0);

            wmutex.enter();
        }
        else break;

        prevhdr = whdr;
        whdr    = whdr->lpNext;
    }

    if(wavehdr == NULL) {
        //last buffer played -> no new ones -> return now
        dprintf(("WINMM: WaveOut handler LAST BUFFER PLAYED! state %s (play %d (%d), cop %d, ret %d)", (State == STATE_PLAYING) ? "playing" : "stopped", bytesPlayed, getPosition(), bytesCopied, bytesReturned));
        if(getPosition() > bytesPlayed) {
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

sendbuffer:
    wmutex.leave();

    //Transfer the buffer we just finished playing to DART
    dprintf2(("WINMM: handler transfer buffer %d", pBuffer - MixBuffer));
    USHORT selTIB = RestoreOS2FS(); // save current FS selector
    MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle, pBuffer, 1);
    SetFS(selTIB);           // switch back to the saved FS selector

    dprintf2(("WINMM: handler DONE"));
}
/******************************************************************************/
/******************************************************************************/
void DartWaveOut::writeBuffer()
{
    ULONG buflength;

    if(!fUnderrun && State == STATE_PLAYING && wavehdr == NULL && curFillBuf == curPlayBuf) {
        dprintf2(("writeBuffer: no more room for more audio data"));
        return; //no room left
    }

    if(curhdr == NULL)
        curhdr = wavehdr;

    while(curhdr && (curhdr->reserved == WHDR_DONE))  {
        curhdr = curhdr->lpNext;
    }

    if(curhdr == NULL) {
        return;     //no unprocessed buffers left
    }

    dprintf2(("WINMM: handler cur (%d,%d), fill (%d,%d)\n", curPlayBuf, curPlayPos, curFillBuf, curFillPos));

    while(curhdr) {
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

        if(curFillPos == curhdr->dwBufferLength) {
            dprintf2(("Buffer %d done ptr %x size %d %x %x %x %x %x %x", curFillBuf, curhdr->lpData, curhdr->dwBufferLength, curhdr->dwBytesRecorded, curhdr->dwUser, curhdr->dwFlags, curhdr->dwLoops, curhdr->lpNext, curhdr->reserved));

            curFillPos = 0;
            curhdr->reserved = WHDR_DONE;
            //search for next unprocessed buffer
            while(curhdr && (curhdr->reserved == WHDR_DONE))
                curhdr = curhdr->lpNext;
        }
        if(curPlayPos == MixBuffer[curFillBuf].ulBufferLength) {
            curPlayPos = 0;

            if(++curFillBuf == PREFILLBUF_DART) {
                curFillBuf = 0;
            }
            if(curFillBuf == curPlayBuf)
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

    ptib2 = (PTIB2)_getTIBvalue(offsetof(TIB, tib_ptib2));
    if(ptib2 && HIBYTE(ptib2->tib2_ulpri) != PRTYC_TIMECRITICAL && 
       LOBYTE(ptib2->tib2_ulpri) != PRTYD_MAXIMUM) 
    {
        dprintf(("Setting priority of DART thread to PRTYC_TIMECRITICAL/PRTYD_MAXIMUM"));
        DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, 0);
    }
    if(pBuffer && pBuffer->ulUserParm)
    {
        dwave = (DartWaveOut *)pBuffer->ulUserParm;
        dwave->handler(ulStatus, pBuffer, ulFlags);
    }
    return(TRUE);
}

/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::setVolume(ULONG ulVol)
{
  ULONG ulVolR     = (((ulVol & 0xffff0000) >> 16 )*100)/0xFFFF; // Right Volume
  ULONG ulVolL      = ((ulVol& 0x0000ffff)*100)/0xFFFF;          // Left Volume
  MCI_SET_PARMS msp = {0};

  dprintf(("DartWaveOut::setVolume %d %d", ulVolL, ulVolR));
  volume = ulVol;

// PD: My card (ESS 1868 PnP) driver can't change only
//     one channel Left or Right :-(
//
#ifdef GOOD_AUDIO_CARD_DRIVER

  msp.ulAudio = MCI_SET_AUDIO_LEFT;
  msp.ulLevel = ulVolL;

  mymciSendCommand(DeviceId, MCI_SET,
                 MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                 &msp, 0);

  msp.ulAudio = MCI_SET_AUDIO_RIGHT;
  msp.ulLevel = ulVolR;

#else
  msp.ulAudio = MCI_SET_AUDIO_ALL;
  msp.ulLevel = max(ulVolR,ulVolL);
#endif

  mymciSendCommand(DeviceId, MCI_SET,
                 MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                 &msp, 0);
  return 0;
}
/******************************************************************************/
/******************************************************************************/

