/* $Id: dwavein.cpp,v 1.1 2001-02-27 21:13:59 sandervl Exp $ */

/*
 * Wave record class
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
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

#include "misc.h"
#include "dwavein.h"

#define DBG_LOCALLOG    DBG_dwavein
#include "dbglocal.h"

#ifndef min
#define min(a, b) ((a > b) ? b : a)
#endif

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

LONG APIENTRY WaveInHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

//TODO: mulaw, alaw & adpcm
/******************************************************************************/
/******************************************************************************/
DartWaveIn::DartWaveIn(LPWAVEFORMATEX pwfx)
{
   Init(pwfx);
}
/******************************************************************************/
/******************************************************************************/
DartWaveIn::DartWaveIn(LPWAVEFORMATEX pwfx, ULONG nCallback, ULONG dwInstance, USHORT usSel)
{
   Init(pwfx);

   mthdCallback     = (LPDRVCALLBACK)nCallback; // callback function
   selCallback      = usSel;                    // callback win32 tib selector
   this->dwInstance = dwInstance;
   fOverrun        = FALSE;

   if(!ulError)
        callback((ULONG)this, WIM_OPEN, dwInstance, 0, 0);
}
/******************************************************************************/
/******************************************************************************/
DartWaveIn::DartWaveIn(LPWAVEFORMATEX pwfx, HWND hwndCallback)
{
   Init(pwfx);

   this->hwndCallback = hwndCallback;

   if(!ulError)
        PostMessageA(hwndCallback, WIM_OPEN, 0, 0);
}
/******************************************************************************/
/******************************************************************************/
void DartWaveIn::callback(HDRVR h, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  USHORT selTIB = GetFS(); // save current FS selector

    dprintf(("WINMM:DartWaveIn::callback(HDRVR h=%08xh, UINT uMessage=%08xh, DWORD dwUser=%08xh, DWORD dw1=%08xh, DWORD dw2=%08xh)\n",
              h, uMessage, dwUser, dw1, dw2));

    if (selCallback != 0)
        SetFS(selCallback);      // switch to callback win32 tib selector (stored in waveInOpen)
    else
        dprintf(("WINMM:DartWaveIn::callback - selCallback is invalid"));

    //@@@PH 1999/12/28 Shockwave Flashes seem to make assumptions on a
    // specific stack layout. Do we have the correct calling convention here?
    mthdCallback(h,uMessage,dwUser,dw1,dw2);
    SetFS(selTIB);           // switch back to the saved FS selector
}
/******************************************************************************/
/******************************************************************************/
void DartWaveIn::Init(LPWAVEFORMATEX pwfx)
{
 MCI_GENERIC_PARMS  GenericParms;
 MCI_AMP_OPEN_PARMS AmpOpenParms;
 APIRET rc;

   fMixerSetup   = FALSE;
   next          = NULL;
   wavehdr       = NULL;
   mthdCallback  = NULL;
   hwndCallback  = 0;
   selCallback   = 0;
   dwInstance    = 0;
   ulError       = 0;
   selCallback   = 0;
   State         = STATE_STOPPED;

   MixBuffer     = (MCI_MIX_BUFFER *)malloc(PREFILLBUF_DART_REC*sizeof(MCI_MIX_BUFFER));
   MixSetupParms = (MCI_MIXSETUP_PARMS *)malloc(sizeof(MCI_MIXSETUP_PARMS));
   BufferParms   = (MCI_BUFFER_PARMS *)malloc(sizeof(MCI_BUFFER_PARMS));

   BitsPerSample   = pwfx->wBitsPerSample;
   SampleRate      = pwfx->nSamplesPerSec;
   this->nChannels = pwfx->nChannels;
   ulBufSize       = DART_BUFSIZE_REC;

   dprintf(("waveInOpen: samplerate %d, numChan %d bps %d (%d), format %x", SampleRate, nChannels, BitsPerSample, pwfx->nBlockAlign, pwfx->wFormatTag));
   // Setup the open structure, pass the playlist and tell MCI_OPEN to use it
   memset(&AmpOpenParms,0,sizeof(AmpOpenParms));

   AmpOpenParms.usDeviceID = ( USHORT ) 0;
   AmpOpenParms.pszDeviceType = ( PSZ ) MCI_DEVTYPE_AUDIO_AMPMIX;

   rc = mciSendCommand(0, MCI_OPEN,
                       MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                       (PVOID) &AmpOpenParms,
                       0);
   DeviceId = AmpOpenParms.usDeviceID;
   if(rc) {
        dprintf(("MCI_OPEN failed\n"));
        mciError(rc);
        ulError = MMSYSERR_NODRIVER;
   }
   if(rc == 0) {
        //Grab exclusive rights to device instance (NOT entire device)
        GenericParms.hwndCallback = 0;  //Not needed, so set to 0
        rc = mciSendCommand(DeviceId, MCI_ACQUIREDEVICE, MCI_EXCLUSIVE_INSTANCE,
                            (PVOID)&GenericParms, 0);
        if(rc) {
            dprintf(("MCI_ACQUIREDEVICE failed\n"));
            mciError(rc);
            ulError = MMSYSERR_NOTENABLED;
        }
   }
   State    = STATE_STOPPED;

   wmutex   = new VMutex();
   if(wmutex == NULL) {
        DebugInt3();
        ulError = MMSYSERR_NOTSUPPORTED;
   }
   if(wmutex)
        wmutex->enter(VMUTEX_WAIT_FOREVER);

   if(wavein == NULL) {
        wavein = this;
   }
   else {
        DartWaveIn *dwave = wavein;

        while(dwave->next) {
            dwave = dwave->next;
        }
        dwave->next = this;
   }

   if(wmutex)
        wmutex->leave();
}
/******************************************************************************/
/******************************************************************************/
DartWaveIn::~DartWaveIn()
{
   MCI_GENERIC_PARMS    GenericParms;

   if(!ulError)
   {
        // Generic parameters
        GenericParms.hwndCallback = 0;   //hwndFrame

        // Stop recording.
        mciSendCommand(DeviceId, MCI_STOP,MCI_WAIT, (PVOID)&GenericParms,0);

        mciSendCommand(DeviceId,
                      MCI_BUFFER,
                      MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                      (PVOID)&BufferParms,
                      0);

        // Generic parameters
        GenericParms.hwndCallback = 0;   //hwndFrame

        // Close the device
        mciSendCommand(DeviceId, MCI_CLOSE, MCI_WAIT, (PVOID)&GenericParms, 0);
   }

   if(wmutex)
        wmutex->enter(VMUTEX_WAIT_FOREVER);

   State = STATE_STOPPED;

   if(wavein == this) {
        wavein = this->next;
   }
   else {
        DartWaveIn *dwave = wavein;

        while(dwave->next != this) {
            dwave = dwave->next;
        }
        dwave->next = this->next;
   }
   if(wmutex)
        wmutex->leave();

   if(!ulError) {
        if(mthdCallback) {
            callback((ULONG)this, WIM_CLOSE, dwInstance, 0, 0);
        }
        else
        if(hwndCallback)
            PostMessageA(hwndCallback, WIM_CLOSE, 0, 0);
   }

   if(wmutex)
        delete wmutex;

   if(MixBuffer)
        free(MixBuffer);
   if(MixSetupParms)
        free(MixSetupParms);
   if(BufferParms)
        free(BufferParms);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::getError()
{
  return(ulError);
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

    //TODO: correct for recording??
    AmpOpenParms.usDeviceID = ( USHORT ) 0;
    AmpOpenParms.pszDeviceType = ( PSZ ) MCI_DEVTYPE_AUDIO_AMPMIX;

    rc = mciSendCommand(0, MCI_OPEN,
                        MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                        (PVOID) &AmpOpenParms,
                        0);

    if(rc) {
        return 0; //no devices present
    }

    // Generic parameters
    GenericParms.hwndCallback = 0;   //hwndFrame

    // Close the device
    mciSendCommand(AmpOpenParms.usDeviceID, MCI_CLOSE, MCI_WAIT, (PVOID)&GenericParms, 0);
    return 1;
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

        /* Setup the mixer for recording of wave data
         */
        MixSetupParms->ulFormatMode = MCI_RECORD;
        MixSetupParms->ulDeviceType = MCI_DEVTYPE_WAVEFORM_AUDIO;
        MixSetupParms->pmixEvent    = WaveInHandler;

        rc = mciSendCommand(DeviceId,
                            MCI_MIXSETUP,
                            MCI_WAIT | MCI_MIXSETUP_INIT,
                            (PVOID)MixSetupParms,
                            0);

        if ( rc != MCIERR_SUCCESS ) {
            mciError(rc);
            mciSendCommand(DeviceId, MCI_RELEASEDEVICE, MCI_WAIT,
                           (PVOID)&GenericParms, 0);
            return(MMSYSERR_NOTSUPPORTED);
        }

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
            dprintf(("mix setup %d, %d\n", pwh->dwBufferLength, pwh->dwBufferLength));

            ulBufSize = pwh->dwBufferLength/2;
            if(ulBufSize < minbufsize) {
                dprintf(("set buffer size to %d bytes (org size = %d)", minbufsize, pwh->dwBufferLength));
                ulBufSize = minbufsize;
            }
        }
        else ulBufSize = minbufsize;

        MixSetupParms->ulBufferSize = ulBufSize;

        BufferParms->ulNumBuffers = PREFILLBUF_DART_REC;
        BufferParms->ulBufferSize = MixSetupParms->ulBufferSize;
        BufferParms->pBufList     = MixBuffer;

        for(i=0;i<PREFILLBUF_DART_REC;i++) {
            MixBuffer[i].ulUserParm = (ULONG)this;
        }

        rc = mciSendCommand(DeviceId,
                            MCI_BUFFER,
                            MCI_WAIT | MCI_ALLOCATE_MEMORY,
                            (PVOID)BufferParms,
                            0);

        if(ULONG_LOWD(rc) != MCIERR_SUCCESS) {
            mciError(rc);
            mciSendCommand(DeviceId, MCI_RELEASEDEVICE, MCI_WAIT,
                          (PVOID)&GenericParms, 0);
            return(MMSYSERR_NOTSUPPORTED);
        }

        //TODO: don't do this here. Select line or mic depending on aux settings
        //(until we really support the mixer extensions anyway)
        /* Set the connector to 'line in' */
        memset( &ConnectorParms, '\0', sizeof( MCI_CONNECTOR_PARMS ) );
        ConnectorParms.ulConnectorType = MCI_LINE_IN_CONNECTOR;
        rc = mciSendCommand( DeviceId,
                             MCI_CONNECTOR,
                             MCI_WAIT |
                             MCI_ENABLE_CONNECTOR |
                             MCI_CONNECTOR_TYPE,
                             ( PVOID ) &ConnectorParms,
                             0 );

        /* Allow the user to hear what is being recorded
         * by turning the monitor on
         */
        memset( &AmpSetParms, '\0', sizeof( MCI_AMP_SET_PARMS ) );
        AmpSetParms.ulItem = MCI_AMP_SET_MONITOR;
        rc = mciSendCommand(DeviceId,
                            MCI_SET,
                            MCI_WAIT | MCI_SET_ON | MCI_SET_ITEM,
                            ( PVOID ) &AmpSetParms,
                            0 );

        wmutex->enter(VMUTEX_WAIT_FOREVER);
        fMixerSetup = TRUE;
    }

    for(i=0;i<PREFILLBUF_DART_REC;i++) {
        memset(MixBuffer[i].pBuffer, 0, MixBuffer[i].ulBufferLength);
    }
    dprintf(("Dart opened, bufsize = %d\n", MixBuffer[0].ulBufferLength));

    dprintf(("MixSetupParms = %X\n", MixSetupParms));
    State     = STATE_RECORDING;
    fOverrun = FALSE;
    wmutex->leave();

    dprintf(("Dart recording"));

    // Start recording.
    USHORT selTIB = GetFS(); // save current FS selector
    MixSetupParms->pmixRead( MixSetupParms->ulMixHandle,
                             &MixBuffer[0],
                             PREFILLBUF_DART_REC);
    SetFS(selTIB);           // switch back to the saved FS selector

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::stop()
{
 MCI_GENERIC_PARMS Params;

    wmutex->enter(VMUTEX_WAIT_FOREVER);
    if(State != STATE_RECORDING) {
        State = STATE_STOPPED;
        wmutex->leave();
        return(MMSYSERR_NOERROR);
    }

    State = STATE_STOPPED;
    wmutex->leave();

    memset(&Params, 0, sizeof(Params));

    // Stop recording.
    mciSendCommand(DeviceId, MCI_STOP, MCI_WAIT, (PVOID)&Params, 0);

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::reset()
{
 MCI_GENERIC_PARMS Params;

  dprintf(("DartWaveIn::reset %s", (State == STATE_RECORDING) ? "recording" : "stopped"));
  if(State != STATE_RECORDING)
        return(MMSYSERR_HANDLEBUSY);

  memset(&Params, 0, sizeof(Params));

  // Stop recording
  mciSendCommand(DeviceId, MCI_STOP, MCI_WAIT, (PVOID)&Params, 0);

  dprintf(("Nr of threads blocked on mutex = %d\n", wmutex->getNrBlocked()));

  wmutex->enter(VMUTEX_WAIT_FOREVER);
  while(wavehdr)
  {
        wavehdr->dwFlags |= WHDR_DONE;
        wavehdr->dwFlags &= ~WHDR_INQUEUE;
        wavehdr = wavehdr->lpNext;
        wavehdr->lpNext = NULL;

        wmutex->leave();
        if(mthdCallback) {
            callback((ULONG)this, WIM_DATA, dwInstance, (ULONG)wavehdr, 0);
        }
        else
        if(hwndCallback) {
            dprintf(("Callback (msg) for buffer %x", wavehdr));
            PostMessageA(hwndCallback, WIM_DATA, (WPARAM)this, (ULONG)wavehdr);
        }
        wmutex->enter(VMUTEX_WAIT_FOREVER);
  }
  wavehdr   = NULL;
  State     = STATE_STOPPED;
  fOverrun = FALSE;

  wmutex->leave();
  return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveIn::addBuffer(LPWAVEHDR pwh, UINT cbwh)
{
 int i;

    wmutex->enter(VMUTEX_WAIT_FOREVER);
    pwh->lpNext   = NULL;
    pwh->dwBytesRecorded = 0;
    if(wavehdr) {
        WAVEHDR *chdr = wavehdr;
        while(chdr->lpNext) {
            chdr = chdr->lpNext;
        }
        chdr->lpNext = pwh;
    }
    else    wavehdr = pwh;
    wmutex->leave();

    return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
ULONG DartWaveIn::getPosition()
{
 MCI_STATUS_PARMS mciStatus = {0};
 ULONG rc, nrbytes;

    mciStatus.ulItem = MCI_STATUS_POSITION;
    rc = mciSendCommand(DeviceId, MCI_STATUS, MCI_STATUS_ITEM|MCI_WAIT, (PVOID)&mciStatus, 0);
    if((rc & 0xFFFF) == MCIERR_SUCCESS) {
        nrbytes = (mciStatus.ulReturn * (getAvgBytesPerSecond()/1000));
        return nrbytes;;
    }
    mciError(rc);
    return 0xFFFFFFFF;
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

  rc = mciSendCommand( (USHORT) 0,
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

  rc = mciSendCommand(DeviceId,   /* Device ID    */
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
  mciSendCommand(DeviceId,MCI_CLOSE,MCI_WAIT,(PVOID)&GenericParms,0);
  return(winrc);
}
/******************************************************************************/
/******************************************************************************/
void DartWaveIn::mciError(ULONG ulError)
{
#ifdef DEBUG
 char szError[256] = "";

  mciGetErrorString(ulError, szError, sizeof(szError));
  dprintf(("WINMM: DartWaveIn: %s\n", szError));
#endif
}
//******************************************************************************
//******************************************************************************
BOOL DartWaveIn::find(DartWaveIn *dwave)
{
 DartWaveIn *curwave = wavein;

  while(curwave) {
    if(dwave == curwave) {
        return(TRUE);
    }
    curwave = curwave->next;
  }

#ifdef DEBUG
//  WriteLog("WINMM:DartWaveIn not found!\n");
#endif
  return(FALSE);
}
/******************************************************************************/
//Simple implementation of recording. We fill up buffers queued by the application
//until we run out of data or buffers. If we run out of buffers, the recorded data
//is thrown away. We will continue when when the applications adds more buffers.
/******************************************************************************/
void DartWaveIn::handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
 ULONG    buflength, bufpos, bytestocopy;
 WAVEHDR *whdr, *prevhdr = NULL;

#ifdef DEBUG1
    dprintf(("WINMM: DartWaveIn handler %x\n", pBuffer));
#endif
    if(ulFlags == MIX_STREAM_ERROR) {
        if(ulStatus == ERROR_DEVICE_OVERRUN) {
            dprintf(("WINMM: ERROR: WaveIn handler ERROR_DEVICE_OVERRUN!\n"));
            if(State == STATE_RECORDING) {
                fOverrun = TRUE;
                stop();    //out of buffers, so pause playback
            }
            return;
        }
        dprintf(("WINMM: WaveIn handler, Unknown error %X\n", ulStatus));
        return;
    }
    wmutex->enter(VMUTEX_WAIT_FOREVER);

    whdr = wavehdr;
    if(whdr == NULL) {
        wmutex->leave();
        //last buffer recorded -> no new ones -> overrun
        //Windows doesn't care -> just continue
        dprintf(("WINMM: WARNING: WaveIn handler OVERRUN!\n"));
        return;
    }

#ifdef DEBUG1
    dprintf(("WINMM: DartWaveIn handler cur (%d,%d), fill (%d,%d)\n", curPlayBuf, curPlayPos, curFillBuf, curFillPos));
#endif

    buflength = pBuffer->ulBufferLength;
    bufpos    = 0;
    while(buflength) {
        if(wavehdr) {
            bytestocopy = min(buflength, wavehdr->dwBufferLength - wavehdr->dwBytesRecorded);
            if(bytestocopy) {
                 memcpy(wavehdr->lpData + wavehdr->dwBytesRecorded, (char *)pBuffer->pBuffer + bufpos, bytestocopy);
            }
            else DebugInt3();   //should never happen

            bufpos                   += bytestocopy;
            wavehdr->dwBytesRecorded += bytestocopy;
            buflength                -= bytestocopy;

            if(wavehdr->dwBytesRecorded == wavehdr->dwBufferLength) {
#ifdef DEBUG1
                dprintf(("WINMM: DartWaveIn handler buf %X done\n", whdr));
#endif
                whdr->dwFlags |= WHDR_DONE;
                whdr->dwFlags &= ~WHDR_INQUEUE;

                wavehdr = whdr->lpNext;
                whdr->lpNext = NULL;
                wmutex->leave();

                if(mthdCallback) {
                    callback((ULONG)this, WIM_DATA, dwInstance, (ULONG)whdr, 0);
                }
                else
                if(hwndCallback) {
    		        dprintf(("Callback (msg) for buffer %x", whdr));
    	            PostMessageA(hwndCallback, WIM_DATA, (WPARAM)this, (ULONG)whdr);
    	        }
                wmutex->enter(VMUTEX_WAIT_FOREVER);
            }
        }
        else break;
    }
    wmutex->leave();

    //Transfer buffer to DART
    // MCI_MIXSETUP_PARMS->pMixWrite does alter FS: selector!
    USHORT selTIB = GetFS(); // save current FS selector
    MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle, pBuffer, 1);
    SetFS(selTIB);           // switch back to the saved FS selector
}
/******************************************************************************/
/******************************************************************************/
LONG APIENTRY WaveInHandler(ULONG ulStatus,
                            PMCI_MIX_BUFFER pBuffer,
                            ULONG ulFlags)
{
    // PTIB ptib;
    // PPIB ppib;
    // DosGetInfoBlocks(&ptib, &ppib);
    // dprintf(("WaveInHandler: thread %d prio %X", ptib->tib_ptib2->tib2_ultid, ptib->tib_ptib2->tib2_ulpri));

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
DartWaveIn *DartWaveIn::wavein = NULL;

