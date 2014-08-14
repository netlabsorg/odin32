/* $Id: waveindart.cpp,v 1.7 2003-07-28 11:30:44 sandervl Exp $ */

/*
 * Wave record class
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: mulaw, alaw & adpcm
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/******************************************************************************/
// Includes
/******************************************************************************/

#define  INCL_BASE
#define  INCL_OS2MM
#include <os2wrap.h>   //Odin32 OS/2 api wrappers
#include <os2mewrap.h> //Odin32 OS/2 MMPM/2 api wrappers
#include <stdlib.h>
#include <string.h>
#define  OS2_ONLY
#include <win32api.h>
#include <wprocess.h>

#include "misc.h"
#include "waveindart.h"
#include "initterm.h"

#define DBG_LOCALLOG    DBG_waveindart
#include "dbglocal.h"

#ifndef min
#define min(a, b) ((a > b) ? b : a)
#endif

LONG APIENTRY WaveInHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

/******************************************************************************/
/******************************************************************************/

static BOOL fwaveInFixedBuffers = FALSE;

/******************************************************************************/
/******************************************************************************/
// ODIN_waveInSetFixedBuffers
//
// Tell WINMM to use DART buffers of the same size as the first buffer delivered
// by waveInAddBuffer

void WIN32API ODIN_waveInSetFixedBuffers()
{
    fwaveInFixedBuffers = TRUE;
}
/******************************************************************************/
/******************************************************************************/
DartWaveIn::DartWaveIn(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
              : WaveInOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    DeviceId      = 0;
    ulBufSize     = DART_BUFSIZE_REC;
    fMixerSetup   = FALSE;
    fOverrun      = FALSE;
    mixHandle     = 0;
    pmixReadProc  = NULL;
    MixBuffer     = NULL;
    BufferParms   = NULL;

    dprintf(("DartWaveIn: samplerate %d, numChan %d bps %d (%d), format %x",
            SampleRate, nChannels, BitsPerSample, pwfx->nBlockAlign, pwfx->wFormatTag));
}

/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::open()
{
    MCI_AMP_OPEN_PARMS AmpOpenParms;
    MCI_GENERIC_PARMS  GenericParms = {0};
    MCI_MIXSETUP_PARMS MixSetupParms;
    APIRET rc;

    MixBuffer     = (MCI_MIX_BUFFER *)malloc(PREFILLBUF_DART_REC*sizeof(MCI_MIX_BUFFER));
    BufferParms   = (MCI_BUFFER_PARMS *)malloc(sizeof(MCI_BUFFER_PARMS));
    if(!MixBuffer || !BufferParms) {
        dprintf(("ERROR: malloc failed!!"));
        return MMSYSERR_NOMEM;
    }

    // Setup the open structure, pass the playlist and tell MCI_OPEN to use it
    memset(&AmpOpenParms, 0, sizeof(AmpOpenParms));
    AmpOpenParms.usDeviceID = 0;
    AmpOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_AUDIO_AMPMIX;

    rc = mymciSendCommand(0, MCI_OPEN,
                          MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                          (PVOID) &AmpOpenParms, 0);
    if(rc) {
        dprintf(("MCI_OPEN failed\n"));
        mciError(rc);
        return MMSYSERR_NODRIVER;
    }

    DeviceId = AmpOpenParms.usDeviceID;

    //Grab exclusive rights to device instance (NOT entire device)
    rc = mymciSendCommand(DeviceId, MCI_ACQUIREDEVICE,
                          MCI_EXCLUSIVE_INSTANCE,
                          (PVOID)&GenericParms, 0);
    if(rc) {
        dprintf(("MCI_ACQUIREDEVICE failed\n"));
        mciError(rc);
        return MMSYSERR_NOTENABLED;
    }

    dprintf(("device acquired\n"));
    dprintf(("bps %d, sps %d chan %d\n", BitsPerSample, SampleRate, nChannels));

    // Set the MixSetupParms data structure
    memset(&MixSetupParms, 0, sizeof(MCI_MIXSETUP_PARMS));
    MixSetupParms.ulBitsPerSample = BitsPerSample;
    MixSetupParms.ulSamplesPerSec = SampleRate;
    MixSetupParms.ulFormatTag     = MCI_WAVE_FORMAT_PCM;
    MixSetupParms.ulChannels      = nChannels;
    MixSetupParms.ulFormatMode    = MCI_RECORD;
    MixSetupParms.ulDeviceType    = MCI_DEVTYPE_WAVEFORM_AUDIO;
    MixSetupParms.pmixEvent       = WaveInHandler;

    rc = mymciSendCommand(DeviceId, MCI_MIXSETUP,
                          MCI_WAIT | MCI_MIXSETUP_INIT,
                          (PVOID)&MixSetupParms, 0);
    if ( rc != MCIERR_SUCCESS ) {
        mymciSendCommand(DeviceId, MCI_RELEASEDEVICE,
                         MCI_WAIT,
                         (PVOID)&GenericParms, 0);
        mciError(rc);
        return MMSYSERR_NOTSUPPORTED;
    }

    // Save the mixer handle & the ptr to the read proc.
    mixHandle    = MixSetupParms.ulMixHandle;
    pmixReadProc = MixSetupParms.pmixRead;


    callback(WIM_OPEN, 0, 0);

    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
DartWaveIn::~DartWaveIn()
{
   MCI_GENERIC_PARMS    GenericParms = {0};

   State = STATE_STOPPED;

    if (DeviceId)
    {
        // Stop recording.
        mymciSendCommand(DeviceId, MCI_STOP,
                         MCI_WAIT, (PVOID)&GenericParms, 0);

        if (fMixerSetup)
            mymciSendCommand(DeviceId, MCI_BUFFER,
                             MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                             (PVOID)&BufferParms, 0);

        // Close the device
        mymciSendCommand(DeviceId, MCI_CLOSE,
                         MCI_WAIT,
                         (PVOID)&GenericParms, 0);

        callback(WIM_CLOSE, 0, 0);
    }

    if(MixBuffer)
        free(MixBuffer);
    if(BufferParms)
        free(BufferParms);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::start()
{
    MCI_GENERIC_PARMS    GenericParms = {0};
    MCI_AMP_OPEN_PARMS   AmpOpenParms;
    MCI_CONNECTOR_PARMS  ConnectorParms;
    MCI_AMP_SET_PARMS    AmpSetParms ;
    APIRET rc;
    int i, curbuf, buflength;

    dprintf(("DartWaveIn::start"));
    if(State == STATE_RECORDING)
        return(MMSYSERR_NOERROR);

    //if the app hasn't yet given us any buffers, then we can't possibly determine the
    //right buffer size for DART recording, so postpone recording until the
    //first buffer has been added
    if(wavehdr == NULL) {
        wmutex.enter();
        State = STATE_POSTPONE_RECORDING;
        wmutex.leave();
        return MMSYSERR_NOERROR;
    }

    if(fMixerSetup == FALSE)
    {
        /*
         * Set up the BufferParms data structure and allocate
         * device buffers from the Amp-Mixer
         */

        int consumerate = getAvgBytesPerSecond();
        //SvL: Recording usually isn't as sensitive to timing as playback
        //     Dividing by 8 instead of 32 here. Change if necessary.
        int minbufsize = consumerate/8;

        LPWAVEHDR pwh = wavehdr;
        if(pwh) {
            if(fwaveInFixedBuffers) {
                ulBufSize = pwh->dwBufferLength;
            }
            else 
            {
                dprintf(("mix setup %d, %d\n", pwh->dwBufferLength, pwh->dwBufferLength));
                ulBufSize = pwh->dwBufferLength/2;
            }
            if(ulBufSize > minbufsize) {
                dprintf(("set buffer size to %d bytes (org size = %d)", minbufsize, pwh->dwBufferLength));
                ulBufSize = minbufsize;
            }
        }
        else ulBufSize = minbufsize;

        BufferParms->ulNumBuffers = PREFILLBUF_DART_REC;
        BufferParms->ulBufferSize = ulBufSize;
        BufferParms->pBufList     = MixBuffer;

        for(i=0;i<PREFILLBUF_DART_REC;i++) {
            MixBuffer[i].ulUserParm = (ULONG)this;
        }

        rc = mymciSendCommand(DeviceId, MCI_BUFFER,
                              MCI_WAIT | MCI_ALLOCATE_MEMORY,
                              (PVOID)BufferParms, 0);

        if(ULONG_LOWD(rc) != MCIERR_SUCCESS) {
            mciError(rc);
            mymciSendCommand(DeviceId, MCI_RELEASEDEVICE,
                             MCI_WAIT,
                             (PVOID)&GenericParms, 0);
            return MMSYSERR_NOTSUPPORTED;
        }

        //TODO: don't do this here. Select line or mic depending on aux settings
        //(until we really support the mixer extensions anyway)
        /* Set the connector to 'microphone' */
        memset(&ConnectorParms, 0, sizeof(MCI_CONNECTOR_PARMS));
        ConnectorParms.ulConnectorType = MCI_MICROPHONE_CONNECTOR;
        rc = mymciSendCommand(DeviceId, MCI_CONNECTOR,
                              MCI_WAIT | MCI_ENABLE_CONNECTOR | MCI_CONNECTOR_TYPE,
                              (PVOID)&ConnectorParms, 0 );

        /* Allow the user to hear what is being recorded
         * by turning the monitor on
         */
        memset(&AmpSetParms, 0, sizeof( MCI_AMP_SET_PARMS ) );
        AmpSetParms.ulItem = MCI_AMP_SET_MONITOR;
        rc = mymciSendCommand(DeviceId, MCI_SET,
                              MCI_WAIT | MCI_SET_ON | MCI_SET_ITEM,
                              (PVOID)&AmpSetParms, 0);

        wmutex.enter();
        fMixerSetup = TRUE;
    }

    for(i=0; i< PREFILLBUF_DART_REC; i++) {
        memset(MixBuffer[i].pBuffer, 0, MixBuffer[i].ulBufferLength);
    }
    dprintf(("Dart opened, bufsize = %d\n", MixBuffer[0].ulBufferLength));

    State    = STATE_RECORDING;
    fOverrun = FALSE;
    wmutex.leave();

    dprintf(("Dart recording"));

    // Start recording.
    USHORT selTIB = RestoreOS2FS(); // save current FS selector
    pmixReadProc(mixHandle, &MixBuffer[0], PREFILLBUF_DART_REC);
    SetFS(selTIB);           // switch back to the saved FS selector

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::stop()
{
    MCI_GENERIC_PARMS Params;

    wmutex.enter();
    if(State != STATE_RECORDING) {
        dprintf(("Not recording!!"));
        State = STATE_STOPPED;
        wmutex.leave();
        return(MMSYSERR_NOERROR);
    }

    State = STATE_STOPPED;
    wmutex.leave();

    memset(&Params, 0, sizeof(Params));

    // Stop recording.
    mymciSendCommand(DeviceId, MCI_STOP, MCI_WAIT, (PVOID)&Params, 0);

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::reset()
{
    MCI_GENERIC_PARMS Params;
    LPWAVEHDR tmpwavehdr;

    dprintf(("DartWaveIn::reset %s", (State == STATE_RECORDING) ? "recording" : "stopped"));
    
    wmutex.enter();
    if(State == STATE_POSTPONE_RECORDING) {
        dprintf(("reset postponed recording"));
        State = STATE_STOPPED;
        wmutex.leave();
        return MMSYSERR_NOERROR;
    }
    wmutex.leave();

    memset(&Params, 0, sizeof(Params));

    // Stop recording
    mymciSendCommand(DeviceId, MCI_STOP, MCI_WAIT, (PVOID)&Params, 0);

    wmutex.enter();
    while(wavehdr)
    {
        wavehdr->dwFlags |= WHDR_DONE;
        wavehdr->dwFlags &= ~WHDR_INQUEUE;
        tmpwavehdr         = wavehdr;
        wavehdr            = wavehdr->lpNext;
        tmpwavehdr->lpNext = NULL;

        wmutex.leave();

        callback(WIM_DATA, (ULONG)tmpwavehdr, 0);

        wmutex.enter();
    }
    wavehdr   = NULL;
    State     = STATE_STOPPED;
    fOverrun = FALSE;

    wmutex.leave();
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::addBuffer(LPWAVEHDR pwh, UINT cbwh)
{
    int i;

    wmutex.enter();
    pwh->lpNext          = NULL;
    pwh->dwBytesRecorded = 0;
    if(wavehdr) {
        WAVEHDR *chdr = wavehdr;
        while(chdr->lpNext) {
            chdr = chdr->lpNext;
        }
        chdr->lpNext = pwh;
    }
    else    wavehdr = pwh;
    wmutex.leave();

    if(State == STATE_POSTPONE_RECORDING) {
        //if recording was postponed due to lack of buffers, then start now
        start();
    }
    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
ULONG DartWaveIn::getPosition()
{
    MCI_STATUS_PARMS mciStatus = {0};
    ULONG rc, nrbytes;

    if(State == STATE_STOPPED) {
        dprintf(("Not recording; return 0 position"));
        return 0;
    }

    mciStatus.ulItem = MCI_STATUS_POSITION;
    rc = mymciSendCommand(DeviceId, MCI_STATUS, MCI_STATUS_ITEM|MCI_WAIT, (PVOID)&mciStatus, 0);
    if((rc & 0xFFFF) == MCIERR_SUCCESS) {
        nrbytes = (((double)mciStatus.ulReturn * (double)getAvgBytesPerSecond())/1000.0);
        return nrbytes;;
    }
    mciError(rc);
    return 0xFFFFFFFF;
}
/******************************************************************************/
/******************************************************************************/
int DartWaveIn::getNumDevices()
{
    MCI_GENERIC_PARMS  GenericParms;
    MCI_AMP_OPEN_PARMS AmpOpenParms;
    APIRET rc;

   // Setup the open structure, pass the playlist and tell MCI_OPEN to use it
   memset(&AmpOpenParms,0,sizeof(AmpOpenParms));

   AmpOpenParms.usDeviceID = ( USHORT ) 0;
   AmpOpenParms.pszDeviceType = ( PSZ ) MCI_DEVTYPE_AUDIO_AMPMIX;

   rc = mymciSendCommand(0, MCI_OPEN,
                       MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                       (PVOID) &AmpOpenParms,
                       0);

   if(rc) {
        return 0; //no devices present
   }

   // Generic parameters
   GenericParms.hwndCallback = 0;   //hwndFrame

   // Close the device
   mymciSendCommand(AmpOpenParms.usDeviceID, MCI_CLOSE, MCI_WAIT, (PVOID)&GenericParms, 0);

   return 1;
}
/******************************************************************************/
/******************************************************************************/
BOOL DartWaveIn::queryFormat(ULONG formatTag, ULONG nChannels,
                             ULONG nSamplesPerSec, ULONG wBitsPerSample)
{
    MCI_WAVE_GETDEVCAPS_PARMS mciAudioCaps;
    MCI_GENERIC_PARMS    GenericParms;
    MCI_OPEN_PARMS       mciOpenParms;         /* open parms for MCI_OPEN             */
    int i, freqbits = 0;
    ULONG rc, DeviceId;
    BOOL winrc;

    dprintf(("DartWaveIn::queryFormat %x srate=%d, nchan=%d, bps=%d", formatTag, nSamplesPerSec, nChannels, wBitsPerSample));

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
    mciAudioCaps.ulFormatMode    = MCI_RECORD;
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
void DartWaveIn::mciError(ULONG rc)
{
#ifdef DEBUG
    char szError[256] = "";

    mymciGetErrorString(rc, szError, sizeof(szError));
    dprintf(("WINMM: DartWaveIn: %s\n", szError));
#endif
}
/******************************************************************************/
//Simple implementation of recording. We fill up buffers queued by the application
//until we run out of data or buffers. If we run out of buffers, the recorded data
//is thrown away. We will continue when when the applications adds more buffers.
/******************************************************************************/
void DartWaveIn::handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
    ULONG    buflength, bufpos, bytestocopy;

    dprintf2(("WINMM: DartWaveIn handler %x\n", pBuffer));
    if(ulFlags == MIX_STREAM_ERROR) {
        if(ulStatus == ERROR_DEVICE_OVERRUN) {
            dprintf(("WINMM: ERROR: WaveIn handler ERROR_DEVICE_OVERRUN!\n"));
            if(State == STATE_RECORDING) {
                fOverrun = TRUE;
                stop();    //out of buffers, so stop playback
            }
            return;
        }
        dprintf(("WINMM: WaveIn handler, Unknown error %X\n", ulStatus));
        return;
    }
    wmutex.enter();

    if(wavehdr == NULL) {
        wmutex.leave();
        //last buffer recorded -> no new ones -> overrun
        //Windows doesn't care -> just continue
        dprintf(("WINMM: WARNING: WaveIn handler OVERRUN!\n"));
        return;
    }

    buflength = pBuffer->ulBufferLength;
    bufpos    = 0;
    while(buflength) {
        if(wavehdr) {
            dprintf2(("WINMM: DartWaveIn handler: bytes recorded %d, buffer length %d, room %d", buflength, wavehdr->dwBufferLength, wavehdr->dwBytesRecorded));
            bytestocopy = min(buflength, wavehdr->dwBufferLength - wavehdr->dwBytesRecorded);
            if(bytestocopy) {
                 memcpy(wavehdr->lpData + wavehdr->dwBytesRecorded, (char *)pBuffer->pBuffer + bufpos, bytestocopy);
            }
            else DebugInt3();   //should never happen

            bufpos                   += bytestocopy;
            wavehdr->dwBytesRecorded += bytestocopy;
            buflength                -= bytestocopy;

            if(wavehdr->dwBytesRecorded == wavehdr->dwBufferLength)
            {
                WAVEHDR *whdr = wavehdr;

                dprintf2(("WINMM: DartWaveIn handler buf %X done\n", whdr));
                whdr->dwFlags |= WHDR_DONE;
                whdr->dwFlags &= ~WHDR_INQUEUE;

                wavehdr = whdr->lpNext;
                whdr->lpNext = NULL;
                wmutex.leave();

                callback(WIM_DATA, (ULONG)whdr, whdr->dwBytesRecorded);

                wmutex.enter();
            }
        }
        else break;
    }
    wmutex.leave();

    //Transfer buffer to DART
    // MCI_MIXSETUP_PARMS->pMixWrite does alter FS: selector!
    USHORT selTIB = RestoreOS2FS(); // save current FS selector
    pmixReadProc(mixHandle, pBuffer, 1);
    SetFS(selTIB);           // switch back to the saved FS selector
}
/******************************************************************************/
/******************************************************************************/
LONG APIENTRY WaveInHandler(ULONG ulStatus,
                            PMCI_MIX_BUFFER pBuffer,
                            ULONG ulFlags)
{
    DartWaveIn *dwave;

    if(pBuffer && pBuffer->ulUserParm)
    {
        dwave = (DartWaveIn *)pBuffer->ulUserParm;
        dwave->handler(ulStatus, pBuffer, ulFlags);
    }
    return(TRUE);
}
/******************************************************************************/
/******************************************************************************/

