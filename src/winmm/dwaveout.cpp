/* $Id: dwaveout.cpp,v 1.14 2000-02-17 14:09:30 sandervl Exp $ */

/*
 * Wave playback class
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



#define  INCL_BASE
#define  INCL_OS2MM
#include <os2wrap.h>   //Odin32 OS/2 api wrappers
#include <os2mewrap.h> //Odin32 OS/2 MMPM/2 api wrappers
#include <stdlib.h>
#include <string.h>
#define  OS2_ONLY
#include "win32type.h"
#include <wprocess.h>

#include "misc.h"
#include "dwaveout.h"

#define DBG_LOCALLOG	DBG_dwaveout
#include "dbglocal.h"

#ifndef min
#define min(a, b) ((a > b) ? b : a)
#endif

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

//SvL: 23/09/99: WinPostMsg no longer works, as win32 window handles are no longer PM handles
BOOL WIN32API PostMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LONG APIENTRY WaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

//TODO: mulaw, alaw & adpcm
/******************************************************************************/
/******************************************************************************/
DartWaveOut::DartWaveOut(LPWAVEFORMATEX pwfx)
{
   Init(pwfx);
}
/******************************************************************************/
/******************************************************************************/
DartWaveOut::DartWaveOut(LPWAVEFORMATEX pwfx, ULONG nCallback, ULONG dwInstance, USHORT usSel)
{
   Init(pwfx);

   mthdCallback         = (LPDRVCALLBACK)nCallback; // callback function
   selCallback          = usSel;                    // callback win32 tib selector
   this->dwInstance = dwInstance;

   if(!ulError)
   	callback((ULONG)this, WOM_OPEN, dwInstance, 0, 0);
}
/******************************************************************************/
/******************************************************************************/
DartWaveOut::DartWaveOut(LPWAVEFORMATEX pwfx, HWND hwndCallback)
{
   Init(pwfx);

   this->hwndCallback = hwndCallback;

   if(!ulError)
   	PostMessageA(hwndCallback, WOM_OPEN, 0, 0);
}
/******************************************************************************/
/******************************************************************************/
void DartWaveOut::callback(HDRVR h, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  USHORT selTIB = GetFS(); // save current FS selector

  dprintf(("WINMM:DartWaveOut::callback(HDRVR h=%08xh, UINT uMessage=%08xh, DWORD dwUser=%08xh, DWORD dw1=%08xh, DWORD dw2=%08xh)\n",
           h,
           uMessage,
           dwUser,
           dw1,
           dw2));

  if (selCallback != 0)
    SetFS(selCallback);      // switch to callback win32 tib selector (stored in waveOutOpen)
  else
    dprintf(("WINMM:DartWaveOut::callback - selCallback is invalid"));

  //@@@PH 1999/12/28 Shockwave Flashes seem to make assumptions on a
  // specific stack layout. Do we have the correct calling convention here?
  mthdCallback(h,uMessage,dwUser,dw1,dw2);
  SetFS(selTIB);           // switch back to the saved FS selector
}
/******************************************************************************/
/******************************************************************************/
void DartWaveOut::Init(LPWAVEFORMATEX pwfx)
{
 MCI_GENERIC_PARMS  GenericParms;
 MCI_AMP_OPEN_PARMS AmpOpenParms;
 APIRET rc;

   curPlayBuf = curFillBuf = curFillPos = curPlayPos = 0;

   fMixerSetup   = FALSE;
   next          = NULL;
   wavehdr       = NULL;
   curhdr        = NULL;
   mthdCallback  = NULL;
   hwndCallback  = 0;
   selCallback   = 0;
   dwInstance    = 0;
   ulError       = 0;
   selCallback   = 0;
   volume        = 0xFFFFFFFF;
   State         = STATE_STOPPED;

   MixBuffer     = (MCI_MIX_BUFFER *)malloc(PREFILLBUF_DART*sizeof(MCI_MIX_BUFFER));
   MixSetupParms = (MCI_MIXSETUP_PARMS *)malloc(sizeof(MCI_MIXSETUP_PARMS));
   BufferParms   = (MCI_BUFFER_PARMS *)malloc(sizeof(MCI_BUFFER_PARMS));

   switch(pwfx->nBlockAlign) {
    case 1://8 bits mono
        BitsPerSample = 8;
        break;
    case 2://16 bits mono or 8 bits stereo
        if(nChannels == 1)
            BitsPerSample = 16;
        else    BitsPerSample = 8;
        break;
    case 4://16 bits stereo
        BitsPerSample = 16;
        break;
   }
   SampleRate      = pwfx->nSamplesPerSec;
   this->nChannels = pwfx->nChannels;
   ulBufSize       = DART_BUFSIZE;

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
#ifdef DEBUG
    WriteLog("MCI_OPEN failed\n");
#endif
    mciError(rc);
    ulError = MMSYSERR_NODRIVER;
   }
   if(rc == 0) {
    //Grab exclusive rights to device instance (NOT entire device)
    GenericParms.hwndCallback = 0;  //Not needed, so set to 0
    rc = mciSendCommand(DeviceId, MCI_ACQUIREDEVICE, MCI_EXCLUSIVE_INSTANCE,
                        (PVOID)&GenericParms, 0);
    if(rc) {
#ifdef DEBUG
        WriteLog("MCI_ACQUIREDEVICE failed\n");
#endif
        mciError(rc);
        ulError = MMSYSERR_NOTENABLED;
    }
   }
   State    = STATE_STOPPED;

   setVolume(volume);

   wmutex   = new VMutex();
   if(wmutex == NULL) {
    ulError = MMSYSERR_NOTSUPPORTED;
   }
   if(wmutex)
    wmutex->enter(VMUTEX_WAIT_FOREVER);

   if(waveout == NULL) {
    waveout = this;
   }
   else {
    DartWaveOut *dwave = waveout;

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
DartWaveOut::~DartWaveOut()
{
   MCI_GENERIC_PARMS    GenericParms;

   if(!ulError) {
	// Generic parameters
   	GenericParms.hwndCallback = 0;   //hwndFrame

   	// Stop the playback.
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

   if(waveout == this) {
    waveout = this->next;
   }
   else {
    DartWaveOut *dwave = waveout;

    while(dwave->next != this) {
        dwave = dwave->next;
    }
    dwave->next = this->next;
   }
   if(wmutex)
    wmutex->leave();

   if(!ulError) {
   	if(mthdCallback) {
    		callback((ULONG)this, WOM_CLOSE, dwInstance, 0, 0);
   	}
   	else
   	if(hwndCallback)
    		PostMessageA(hwndCallback, WOM_CLOSE, 0, 0);
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
MMRESULT DartWaveOut::getError()
{
  return(ulError);
}
/******************************************************************************/
/******************************************************************************/
int DartWaveOut::getNumDevices()
{
 MCI_GENERIC_PARMS  GenericParms;
 MCI_AMP_OPEN_PARMS AmpOpenParms;
 APIRET rc;

   // Setup the open structure, pass the playlist and tell MCI_OPEN to use it
   memset(&AmpOpenParms,0,sizeof(AmpOpenParms));

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
MMRESULT DartWaveOut::write(LPWAVEHDR pwh, UINT cbwh)
{
 MCI_GENERIC_PARMS GenericParms = {0};
 APIRET rc;
 int i, buflength;

  if(fMixerSetup == FALSE) {
#ifdef DEBUG
    WriteLog("device acquired\n");
#endif
    /* Set the MixSetupParms data structure to match the loaded file.
     * This is a global that is used to setup the mixer.
     */
    memset(MixSetupParms, 0, sizeof( MCI_MIXSETUP_PARMS ) );

    MixSetupParms->ulBitsPerSample = BitsPerSample;
    MixSetupParms->ulSamplesPerSec = SampleRate;
    MixSetupParms->ulFormatTag     = MCI_WAVE_FORMAT_PCM;
    MixSetupParms->ulChannels      = nChannels;

#ifdef DEBUG
    WriteLog("bps %d, sps %d chan %d\n", BitsPerSample, SampleRate, nChannels);
#endif

        /* Setup the mixer for playback of wave data
         */
    MixSetupParms->ulFormatMode = MCI_PLAY;
        MixSetupParms->ulDeviceType = MCI_DEVTYPE_WAVEFORM_AUDIO;
        MixSetupParms->pmixEvent    = WaveOutHandler;

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
#ifdef DEBUG
    WriteLog("mix setup %d, %d\n", pwh->dwBufferLength, pwh->dwBufferLength);
#endif
#if 1
    ulBufSize = pwh->dwBufferLength/2;
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

    wmutex->enter(VMUTEX_WAIT_FOREVER);
    fMixerSetup = TRUE;

    curPlayBuf = curFillBuf = curFillPos = curPlayPos = 0;

        for(i=0;i<PREFILLBUF_DART;i++) {
        memset(MixBuffer[i].pBuffer, 0, MixBuffer[i].ulBufferLength);
    }
#ifdef DEBUG
    WriteLog("Dart opened, bufsize = %d\n", MixBuffer[i].ulBufferLength);
#endif

    wavehdr     = pwh;
    curhdr      = pwh;
    pwh->lpNext = NULL;

    while(TRUE) {
        buflength = min((ULONG)MixBuffer[curFillBuf].ulBufferLength - curPlayPos,
                (ULONG)wavehdr->dwBufferLength - curFillPos);
#ifdef DEBUG
        WriteLog("Copying %d data; curPlayPos = %d curFillPos = %d\n", buflength, curPlayPos, curFillPos);
#endif
        memcpy((char *)MixBuffer[curFillBuf].pBuffer + curPlayPos,
               wavehdr->lpData + curFillPos,
               buflength);

        curPlayPos  += buflength;
        curFillPos += buflength;
        if(curFillPos == wavehdr->dwBufferLength) {
#ifdef DEBUG
            WriteLog("Processed first win32 buffer\n");
#endif
            curFillPos        = 0;
            wavehdr->dwFlags |= WHDR_DONE;
            curhdr            = NULL;
        }
        if(curPlayPos == MixBuffer[curPlayBuf].ulBufferLength) {
            if(++curPlayBuf == PREFILLBUF_DART) {
                curPlayBuf = 0;
                break;
            }
            curPlayPos = 0;
        }
        if(curFillPos == 0)
            break;
    }
#ifdef DEBUG
    WriteLog("MixSetupParms = %X\n", MixSetupParms);
#endif
    State = STATE_PLAYING;
    wmutex->leave();

    MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle,
                             MixBuffer,
                             PREFILLBUF_DART);
#ifdef DEBUG
    WriteLog("Dart playing\n");
#endif
  }
  else {
    wmutex->enter(VMUTEX_WAIT_FOREVER);
    pwh->lpNext   = NULL;
    if(wavehdr) {
        WAVEHDR *chdr = wavehdr;
        while(chdr->lpNext) {
            chdr = chdr->lpNext;
        }
        chdr->lpNext = pwh;
    }
    else    wavehdr = pwh;
    wmutex->leave();
    if(State != STATE_PLAYING) {//continue playback
        restart();
    }
  }

  return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::pause()
{
 MCI_GENERIC_PARMS Params;

  if(State != STATE_PLAYING)
    return(MMSYSERR_HANDLEBUSY);

  wmutex->enter(VMUTEX_WAIT_FOREVER);
  State = STATE_PAUSED;
  wmutex->leave();

  memset(&Params, 0, sizeof(Params));

  // Stop the playback.
  mciSendCommand(DeviceId, MCI_PAUSE, MCI_WAIT, (PVOID)&Params, 0);

  return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::reset()
{
 MCI_GENERIC_PARMS Params;

  dprintf(("DartWaveOut::reset %s", (State == STATE_PLAYING) ? "playing" : "stopped"));
  if(State != STATE_PLAYING)
    return(MMSYSERR_HANDLEBUSY);

  memset(&Params, 0, sizeof(Params));

  // Stop the playback.
  mciSendCommand(DeviceId, MCI_STOP, MCI_WAIT, (PVOID)&Params, 0);

#ifdef DEBUG
  WriteLog("Nr of threads blocked on mutex = %d\n", wmutex->getNrBlocked());
#endif

  wmutex->enter(VMUTEX_WAIT_FOREVER);
  while(wavehdr) {
    wavehdr->dwFlags |= WHDR_DONE;
    wmutex->leave();
    if(mthdCallback) {
        callback((ULONG)this, WOM_DONE, dwInstance, wavehdr->dwUser, (ULONG)wavehdr);
    }
    else
        if(hwndCallback)
        PostMessageA(hwndCallback, WOM_DONE, wavehdr->dwUser, (ULONG)wavehdr);

    wmutex->enter(VMUTEX_WAIT_FOREVER);
    wavehdr = wavehdr->lpNext;
  }
  wavehdr = NULL;
  State = STATE_STOPPED;

  wmutex->leave();
  return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT DartWaveOut::restart()
{
  dprintf(("DartWaveOut::restart"));
  wmutex->enter(VMUTEX_WAIT_FOREVER);
  State = STATE_PLAYING;
  wmutex->leave();
  MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle,
                          &MixBuffer[curPlayBuf],
                          PREFILLBUF_DART);
  return(MMSYSERR_NOERROR);
}
/******************************************************************************/
/******************************************************************************/
BOOL DartWaveOut::queryFormat(ULONG formatTag, ULONG nChannels,
                  ULONG nSamplesPerSec, ULONG sampleSize)
{
 MCI_WAVE_GETDEVCAPS_PARMS mciAudioCaps;
 MCI_GENERIC_PARMS    GenericParms;
 MCI_OPEN_PARMS       mciOpenParms;         /* open parms for MCI_OPEN             */
 int i, freqbits = 0;
 ULONG rc, DeviceId;
 BOOL winrc;

  memset(&mciOpenParms,            /* Object to fill with zeros.       */
         0,                        /* Value to place into the object.  */
         sizeof( mciOpenParms ) ); /* How many zero's to use.          */

  mciOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_WAVEFORM_AUDIO;

  rc = mciSendCommand( (USHORT) 0,
                       MCI_OPEN,
                       MCI_WAIT | MCI_OPEN_TYPE_ID,
                       (PVOID) &mciOpenParms,
                       0);
  if (rc != 0) {
    return(FALSE);
  }
  DeviceId = mciOpenParms.usDeviceID;

  memset( &mciAudioCaps , 0, sizeof(MCI_WAVE_GETDEVCAPS_PARMS));

  switch(sampleSize) {
    case 1:
        mciAudioCaps.ulBitsPerSample = 8;
        break;
    case 2:
        if(nChannels == 1)
            mciAudioCaps.ulBitsPerSample = 16;
        else    mciAudioCaps.ulBitsPerSample = 8;
        break;
    case 4:
        mciAudioCaps.ulBitsPerSample = 16;
        break;
  }
  mciAudioCaps.ulFormatTag     = DATATYPE_WAVEFORM;
  mciAudioCaps.ulSamplesPerSec = nSamplesPerSec;
  mciAudioCaps.ulChannels      = nChannels;
  mciAudioCaps.ulFormatMode    = MCI_PLAY;
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
  else 	winrc = TRUE;

  // Close the device
  mciSendCommand(DeviceId,MCI_CLOSE,MCI_WAIT,(PVOID)&GenericParms,0);
  return(winrc);
}
/******************************************************************************/
/******************************************************************************/
void DartWaveOut::mciError(ULONG ulError)
{
#ifdef DEBUG
 char szError[256] = "";

  mciGetErrorString(ulError, szError, sizeof(szError));
  WriteLog("WINMM: DartWaveOut: %s\n", szError);
#endif
}
//******************************************************************************
//******************************************************************************
BOOL DartWaveOut::find(DartWaveOut *dwave)
{
 DartWaveOut *curwave = waveout;

  while(curwave) {
    if(dwave == curwave) {
        return(TRUE);
    }
    curwave = curwave->next;
  }

#ifdef DEBUG
//  WriteLog("WINMM:DartWaveOut not found!\n");
#endif
  return(FALSE);
}
/******************************************************************************/
/******************************************************************************/
void DartWaveOut::handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
 ULONG    buflength;
 WAVEHDR *whdr = wavehdr, *prevhdr = NULL;

#ifdef DEBUG1
  WriteLog("WINMM: handler %d\n", curPlayBuf);
#endif
  if(ulFlags == MIX_STREAM_ERROR) {
    if(ulStatus == ERROR_DEVICE_UNDERRUN) {
        dprintf(("WINMM: WaveOut handler UNDERRUN!\n"));
        pause();    //out of buffers, so pause playback
        return;
    }
    dprintf(("WINMM: WaveOut handler, Unknown error %X\n", ulStatus));
    return;
  }
  wmutex->enter(VMUTEX_WAIT_FOREVER);

  while(whdr) {
    if(whdr->dwFlags & WHDR_DONE) {
#ifdef DEBUG1
        WriteLog("WINMM: handler buf %X done\n", whdr);
#endif
        whdr->dwFlags &= ~WHDR_INQUEUE;

        if(prevhdr == NULL)
            wavehdr = whdr->lpNext;
        else    prevhdr->lpNext = whdr->lpNext;

        whdr->lpNext = NULL;
        wmutex->leave();

        if(mthdCallback) {
            callback((ULONG)this, WOM_DONE, dwInstance, whdr->dwUser, (ULONG)whdr);
        }
        else
            if(hwndCallback)
            PostMessageA(hwndCallback, WOM_DONE, whdr->dwUser, (ULONG)whdr);

        wmutex->enter(VMUTEX_WAIT_FOREVER);
    }
    prevhdr = whdr;
    whdr    = whdr->lpNext;
  }

  if(curhdr == NULL)
    curhdr = wavehdr;

#ifdef DEBUG1
  WriteLog("WINMM: handler cur (%d,%d), fill (%d,%d)\n", curPlayBuf, curPlayPos, curFillBuf, curFillPos);
#endif

  while(curhdr) {
    buflength = min((ULONG)MixBuffer[curFillBuf].ulBufferLength - curPlayPos,
            (ULONG)curhdr->dwBufferLength - curFillPos);
    memcpy((char *)MixBuffer[curFillBuf].pBuffer + curPlayPos,
           curhdr->lpData + curFillPos,
           buflength);
    curPlayPos += buflength;
    curFillPos += buflength;
#ifdef DEBUG1
    WriteLog("WINMM: copied %d bytes, cufFillPos = %d, dwBufferLength = %d\n", buflength, curFillPos, curhdr->dwBufferLength);
#endif
    if(curFillPos == curhdr->dwBufferLength) {
#ifdef DEBUG1
        WriteLog("Buffer %d done\n", curFillBuf);
#endif
        curFillPos = 0;
        curhdr->dwFlags |= WHDR_DONE;
        //search for next unprocessed buffer
        while(curhdr && curhdr->dwFlags & WHDR_DONE)
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

  if(curPlayBuf == PREFILLBUF_DART-1)
        curPlayBuf = 0;
  else  curPlayBuf++;

  wmutex->leave();
  //Transfer buffer to DART
  MixSetupParms->pmixWrite(MixSetupParms->ulMixHandle, &MixBuffer[curPlayBuf], 1);
}
/******************************************************************************/
/******************************************************************************/
LONG APIENTRY WaveOutHandler(ULONG ulStatus,
                             PMCI_MIX_BUFFER pBuffer,
                             ULONG ulFlags)
{
  // PTIB ptib;
  // PPIB ppib;
  // DosGetInfoBlocks(&ptib, &ppib);
  // dprintf(("WaveOutHandler: thread %d prio %X", ptib->tib_ptib2->tib2_ultid, ptib->tib_ptib2->tib2_ulpri));

  DartWaveOut *dwave;

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

  volume = ulVol;

// PD: My card (ESS 1868 PnP) driver can't change only
//     one channel Left or Right :-(
//
#ifdef GOOD_AUDIO_CARD_DRIVER  

  msp.ulAudio = MCI_SET_AUDIO_LEFT;
  msp.ulLevel = ulVolL;

  mciSendCommand(DeviceId, MCI_SET,
                 MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                 &msp, 0);

  msp.ulAudio = MCI_SET_AUDIO_RIGHT;
  msp.ulLevel = ulVolR;

#else
  msp.ulAudio = MCI_SET_AUDIO_ALL;
  msp.ulLevel = max(ulVolR,ulVolL);
#endif

  mciSendCommand(DeviceId, MCI_SET,
                 MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                 &msp, 0);
  return 0;
}
/******************************************************************************/
/******************************************************************************/
DartWaveOut *DartWaveOut::waveout = NULL;

