/* $Id: waveoutflash.cpp,v 1.00 2010-02-20 12:00:00 rlwalsh Exp $ */

/*
 * Wave playback class (DART for Flash10)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2010 Richard L Walsh (rich@e-vertise.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
#include <stdio.h>
#include <stdarg.h>
#define  OS2_ONLY
#include <win32api.h>
#include <wprocess.h>
#include <options.h>
#include "misc.h"
#include "waveoutflash.h"
#include "initterm.h"

#define DBG_LOCALLOG    DBG_waveoutflash
#include "dbglocal.h"

/******************************************************************************/

// number of DART buffers to allocate
#define FLASH_BUFCNT    8

// number of buffers to fill before writing when state == STOPPED or PAUSED
#define FLASH_PREFILL   3

// used to attach additional info to the MCI_MIX_BUFFERs
typedef struct {
    FlashWaveOut *      pThis;
    volatile LPWAVEHDR  pwh;
} MIX_EXTRA;

// a deprecated but useful function implemented in kernel32\thread.cpp
USHORT WIN32API ODIN_ThreadEnterOdinContext(void *pExceptionRegRec, BOOL fForceFSSwitch);

// debug info
void    showWriteMsg(LPWAVEHDR pwh, int free, int queued, BOOL fInit);

/******************************************************************************/
// Query/Enable/Disable use of FlashWaveOut
/******************************************************************************/

static BOOL useFlashAudio = FALSE;

/******************************************************************************/
/******************************************************************************/

BOOL WIN32API ODIN_IsFlashAudioEnabled()
{
    // Except for the first call, this is the same as 'return useFlashAudio'.
    return ODIN_EnableFlashAudio(useFlashAudio);
}

/******************************************************************************/
/******************************************************************************/

BOOL WIN32API ODIN_EnableFlashAudio(BOOL enable)
{
    static int forceFlashAudio = -1;

    // If the ini hasn't been checked yet, do so now.
    // If there's an entry, it will override the default & app setting.
    if (forceFlashAudio == -1) {
        int i = PROFILE_GetOdinIniInt(WINMM_SECTION, KEY_FLASHAUDIO, -1);
        if (i == -1)
            forceFlashAudio = 0;
        else {
            forceFlashAudio = 1;
            useFlashAudio = i ? TRUE : FALSE;
        }
    }

    // If FlashAudio hasn't been forced on or off, update the setting.
    if (!forceFlashAudio)
        useFlashAudio = enable ? TRUE : FALSE;

    return useFlashAudio;
}

/******************************************************************************/
// FlashWaveOut implementation
/******************************************************************************/

// event sem used to serialize ownership of the audio device
HEV FlashWaveOut::deviceSem = 0;

/******************************************************************************/
/******************************************************************************/

FlashWaveOut::FlashWaveOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen,
                           ULONG nCallback, ULONG dwInstance)
                    : WaveOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    deviceId      = 0;
    waveOffs      = 0;
    dartBufInit   = FALSE;
    dartBufSize   = 0;
    dartBufCnt    = FLASH_BUFCNT;
    dartFreeCnt   = FLASH_BUFCNT;
    dartFreeNdx   = 0;
    dartPlayed    = 0;
    readyCnt      = 0;
    readyNdx      = 0;
    mixHandle     = 0;
    pmixWriteProc = 0;
    pmixBuffers   = 0;

    dprintf(("FlashWaveOut - sps= %d  bps= %d  chan= %d",
             SampleRate, BitsPerSample, nChannels));

    return;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::open()
{
    APIRET      rc;
    MIX_EXTRA * pmixExtra;
    MCI_AMP_OPEN_PARMS AmpOpenParms;
    MCI_GENERIC_PARMS  GenericParms = {0};
    MCI_MIXSETUP_PARMS MixSetupParms;

    // Wait for the device to become available.
    if (!getDeviceSem(TRUE))
        return MMSYSERR_NOTENABLED;

    // Open the ampmix device in shared mode.
    memset(&AmpOpenParms, 0, sizeof(AmpOpenParms));
    AmpOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_AUDIO_AMPMIX;

    rc = mymciSendCommand(0, MCI_OPEN,
                          MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                          (PVOID)&AmpOpenParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        mciError("FlashWaveOut::open - MCI_OPEN", rc);
        return MMSYSERR_NODRIVER;
    }

    // Save the device ID
    deviceId = AmpOpenParms.usDeviceID;

    // Get exclusive rights to this device instance (not entire device).
    rc = mymciSendCommand(deviceId, MCI_ACQUIREDEVICE,
                          MCI_WAIT | MCI_EXCLUSIVE_INSTANCE,
                          (PVOID)&GenericParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        mciError("FlashWaveOut::open - MCI_ACQUIREDEVICE", rc);
        return MMSYSERR_NOTENABLED;
    }

    // Setup the Amp-Mixer to play wave data.
    memset(&MixSetupParms, 0, sizeof(MixSetupParms));
    MixSetupParms.ulBitsPerSample  = BitsPerSample;
    MixSetupParms.ulFormatTag      = MCI_WAVE_FORMAT_PCM;
    MixSetupParms.ulSamplesPerSec  = SampleRate;
    MixSetupParms.ulChannels       = nChannels;
    MixSetupParms.ulFormatMode     = MCI_PLAY;
    MixSetupParms.ulDeviceType     = MCI_DEVTYPE_WAVEFORM_AUDIO;
    MixSetupParms.pmixEvent        = FlashWaveOutHandler;

    rc = mymciSendCommand(deviceId, MCI_MIXSETUP,
                          MCI_WAIT | MCI_MIXSETUP_INIT,
                          (PVOID)&MixSetupParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        mciError("FlashWaveOut::open - MCI_MIXSETUP", rc);
        mymciSendCommand(deviceId, MCI_RELEASEDEVICE,
                         MCI_WAIT, (PVOID)&GenericParms, 0);
        return MMSYSERR_NOTSUPPORTED;
    }

    // Save the mixer handle & the ptr to the write proc.
    mixHandle     = MixSetupParms.ulMixHandle;
    pmixWriteProc = MixSetupParms.pmixWrite;

    // Allocate mixbuffer structures - the actual buffers
    // will be allocated the first time ::write() is called.
    int    cnt = dartBufCnt * (sizeof(MCI_MIX_BUFFER) + sizeof(MIX_EXTRA));
    void * ptr = malloc(cnt);
    if (!ptr) {
        dprintf(("FlashWaveOut::open - malloc failed"));
        mymciSendCommand(deviceId, MCI_RELEASEDEVICE,
                         MCI_WAIT, (PVOID)&GenericParms, 0);
        return MMSYSERR_NOMEM;
    }
    memset(ptr, 0, cnt);
    pmixBuffers = (MCI_MIX_BUFFER*)ptr;
    pmixExtra   = (MIX_EXTRA*)&pmixBuffers[dartBufCnt];

    // Initialize the members that don't change.
    for (int i = 0; i < dartBufCnt; i++) {
        pmixBuffers[i].ulStructLength = sizeof(MCI_MIX_BUFFER);
        pmixBuffers[i].ulUserParm = (ULONG)&pmixExtra[i];
        pmixExtra[i].pThis = this;
    }

    setVolume(volume);
    callback(WOM_OPEN, 0, 0);

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

FlashWaveOut::~FlashWaveOut()
{
    APIRET  rc;
    MCI_GENERIC_PARMS GenericParms = {0};

    dprintf((__FUNCTION__));

    State = STATE_STOPPED;

    // If the device was successfully opened...
    if (deviceId) {

        // Stop playback.
        rc = mymciSendCommand(deviceId, MCI_STOP,
                              MCI_WAIT, (PVOID)&GenericParms, 0);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("~FlashWaveOut - MCI_STOP", rc);

        // Free the DART buffers
        if (dartBufInit) {
            MCI_BUFFER_PARMS BufferParms;
            memset(&BufferParms, 0, sizeof(BufferParms));
            BufferParms.ulStructLength = sizeof(BufferParms);
            BufferParms.ulNumBuffers = dartBufCnt;
            BufferParms.ulBufferSize = dartBufSize;
            BufferParms.pBufList     = pmixBuffers;
            rc = mymciSendCommand(deviceId, MCI_BUFFER,
                                  MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                                  (PVOID)&BufferParms, 0);
            if (LOUSHORT(rc) != MCIERR_SUCCESS)
                mciError("~FlashWaveOut - MCI_BUFFER", rc);
        }

        // Close the device.
        rc = mymciSendCommand(deviceId, MCI_CLOSE,
                              MCI_WAIT, (PVOID)&GenericParms, 0);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("~FlashWaveOut - MCI_CLOSE", rc);

        // Advise the Win client that the device has been closed.
        callback(WOM_CLOSE, 0, 0);
    }

    // Free the MCI_MIX_BUFFER & MIX_EXTRA structures
    if (pmixBuffers)
        free(pmixBuffers);

    // Signal that the device is now available for use by another instance.
    getDeviceSem(FALSE);

    dprintf(("exit\n"));

    return;
}

/******************************************************************************/
/******************************************************************************/

// Flash may try to open a new instance of the audio device before it has
// finished closing the previous instance, causing a failure.  Because it
// is opened on the primary thread but usually closed by a secondary thread,
// a mutex sem can't be used to control access to the device.  Instead, an
// event sem is used, where:  reset == "in use" & posted == "available".
// Note:  DCE_POSTONE was added in Warp 4.0 FP4 & 3.0 FP29 - the sem is
//        reset automatically & only one waiting thread is unblocked

BOOL FlashWaveOut::getDeviceSem(BOOL get)
{
    APIRET  rc;

    // Create the semaphore.  Unless there's been some error,
    // this should only be called when 'get' is TRUE.
    if (!deviceSem) {
        dprintf(("FlashWaveOut::getDeviceSem - creating event sem in %s state",
                 (get ? "RESET" : "POSTED")));
        rc = DosCreateEventSem(0, &deviceSem, DCE_POSTONE, !get);
        if (rc) {
            dprintf(("FlashWaveOut::getDeviceSem - DosCreateEventSem failed - rc= %d", rc));
            return MMSYSERR_NOTENABLED;
        }
        return TRUE;
    }

    // Post the semaphore so other instances can claim the device.
    if (!get) {
        rc = DosPostEventSem(deviceSem);
        if (rc) {
            dprintf(("FlashWaveOut::getDeviceSem - DosPostEventSem failed - rc= %d", rc));
            return FALSE;
        }
        return TRUE;
    }

    // Wait for the semaphore (it will reset itself).  Limit the wait
    // to 5 seconds to avoid causing more problems than we're solving.
    dprintf(("FlashWaveOut::getDeviceSem - about to wait for event sem"));
    rc = DosWaitEventSem(deviceSem, 5000);
    if (rc) {
        dprintf(("FlashWaveOut::getDeviceSem - DosWaitEventSem failed - rc= %d", rc));
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::initBuffers()
{
    APIRET  rc;
    MCI_GENERIC_PARMS GenericParms = {0};
    MCI_BUFFER_PARMS  BufferParms;

    dprintf(("$ - bufsize= %d", __FUNCTION__, dartBufSize));

    // Set up the BufferParms data structure and
    // allocate device buffers from the Amp-Mixer.
    memset(&BufferParms, 0, sizeof(BufferParms));
    BufferParms.ulStructLength = sizeof(BufferParms);
    BufferParms.ulNumBuffers   = dartBufCnt;
    BufferParms.ulBufferSize   = dartBufSize;
    BufferParms.pBufList       = pmixBuffers;

    rc = mymciSendCommand(deviceId, MCI_BUFFER,
                          MCI_WAIT | MCI_ALLOCATE_MEMORY,
                          (PVOID)&BufferParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        mciError("FlashWaveOut::initBuffers - MCI_BUFFER", rc);
        mymciSendCommand(deviceId, MCI_RELEASEDEVICE,
                         MCI_WAIT, (PVOID)&GenericParms, 0);
        return MMSYSERR_NOTSUPPORTED;
    }

    // DART may not have allocated all the buffers requested.
    dartBufCnt = BufferParms.ulNumBuffers;
    dartFreeCnt = dartBufCnt;

    // Signal that the DART buffers have been created & initialized.
    dartBufInit = TRUE;

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::write(LPWAVEHDR pwh, UINT cbwh)
{
    APIRET  rc;

    #ifdef DEBUG
        showWriteMsg(pwh, dartFreeCnt, queuedbuffers, dartBufInit);
    #endif

    // Allocation of DART buffers is deferred until the first write
    // so their size can be based on the size of the Win WAVE buffers.
    if (!dartBufInit) {
        dartBufSize = pwh->dwBufferLength;
        if (!dartBufSize || dartBufSize > 0x10000)
            return MMSYSERR_INVALPARAM;

        rc = initBuffers();
        if (rc != MMSYSERR_NOERROR)
            return rc;
    }

    // Init members used by the Win MM subsystem.
    pwh->lpNext   = 0;
    pwh->reserved = 0;

    // Add this header to the tail of the chain of unplayed buffers
    if (!wavehdr)
        wavehdr = pwh;
    else {
        LPWAVEHDR pTail = wavehdr;
        while (pTail->lpNext) {
            pTail = pTail->lpNext;
        }
        pTail->lpNext = pwh;
    }
    queuedbuffers++;

    // Copy as many WAVE buffers to DART buffers as possible.  Exit
    // if none were copied, or we don't have enough to start playback, 
    writeBuffer();
    if (!readyCnt || (State != STATE_PLAYING && readyCnt < FLASH_PREFILL))
        return MMSYSERR_NOERROR;

    // Change the state flag to playing.
    LONG oldState = DosInterlockedExchange((PLONG)&State, STATE_PLAYING);
    if (oldState != STATE_PLAYING)
        dprintf(("state change to PLAYING from %s",
                (oldState == STATE_STOPPED ? "STOPPED" :
                 (oldState == STATE_PAUSED ? "PAUSED " : "UNKNOWN"))));

    // Write all of the DART buffers that are ready to the device.
    USHORT selTIB = RestoreOS2FS();
    if (readyNdx + readyCnt >= dartBufCnt) {
        ULONG cnt = dartBufCnt - readyNdx;
        rc = pmixWriteProc(mixHandle, &pmixBuffers[readyNdx], cnt);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("FlashWaveOut::write - pmixWriteProc", rc);
        readyNdx  = 0;
        readyCnt -= cnt;
    }
    if (readyCnt) {
        rc = pmixWriteProc(mixHandle, &pmixBuffers[readyNdx], readyCnt);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("FlashWaveOut::write - pmixWriteProc", rc);
        readyNdx += readyCnt;
        readyCnt  = 0;
    }
    SetFS(selTIB);

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

void FlashWaveOut::writeBuffer()
{
    // Loop while there are both WAVE buffers & DART buffers available.
    while (wavehdr && dartFreeCnt) {
        MIX_EXTRA * pMX;

        // Write one WAVE buffer to as many DART buffers as needed
        // (both buffers should be the same size, so this shouldn't loop).
        while (dartFreeCnt && waveOffs < wavehdr->dwBufferLength) {
            int cnt = wavehdr->dwBufferLength - waveOffs;
            if (cnt > dartBufSize)
                cnt = dartBufSize;
            memcpy(pmixBuffers[dartFreeNdx].pBuffer, &wavehdr->lpData[waveOffs], cnt);
            pmixBuffers[dartFreeNdx].ulBufferLength = cnt;
            pMX = (MIX_EXTRA*)pmixBuffers[dartFreeNdx].ulUserParm;
            pMX->pwh = 0;
            waveOffs += cnt;
            readyCnt++;
            DosInterlockedDecrement((PLONG)&dartFreeCnt);
            if (++dartFreeNdx >= dartBufCnt)
                dartFreeNdx = 0;
        }

        // Exit if the WAVE buffer still has data
        // but there are no more DART buffers.
        if (waveOffs < wavehdr->dwBufferLength)
            return;

        // Remove this wavehdr from the head of the chain and
        // store it in the MIX_EXTRA struct associated with this
        // DART buffer for use by the DART event callback proc.
        waveOffs = 0;
        pMX->pwh = wavehdr;
        wavehdr = wavehdr->lpNext;
        pMX->pwh->lpNext = 0;
        queuedbuffers--;
    }

    return;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::pause()
{
    APIRET  rc;
    MCI_GENERIC_PARMS GenericParms = {0};

    // Pause playback if current state == playing
    if (DosInterlockedCompareExchange((PLONG)&State, STATE_PAUSED, STATE_PLAYING)
        == STATE_PLAYING) {
        rc = mymciSendCommand(deviceId, MCI_PAUSE,
                         MCI_WAIT, (PVOID)&GenericParms, 0);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("FlashWaveOut::pause - MCI_PAUSE", rc);
    }

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::resume()
{
    APIRET  rc;
    MCI_GENERIC_PARMS GenericParms = {0};

    dprintf2((__FUNCTION__));

    // Resume playback if current state == paused
    if (DosInterlockedCompareExchange((PLONG)&State, STATE_PLAYING, STATE_PAUSED)
        == STATE_PAUSED) {
        rc = mymciSendCommand(deviceId, MCI_RESUME,
                              MCI_WAIT, (PVOID)&GenericParms, 0);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("FlashWaveOut::resume - MCI_RESUME", rc);
    }

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::stop()
{
    APIRET  rc;
    MCI_GENERIC_PARMS GenericParms = {0};

    dprintf2((__FUNCTION__));

    // Stop playback if current state == playing or paused
    if (DosInterlockedExchange((PLONG)&State, STATE_STOPPED)
        != STATE_STOPPED) {
        rc = mymciSendCommand(deviceId, MCI_STOP,
                              MCI_WAIT, (PVOID)&GenericParms, 0);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("FlashWaveOut::stop - MCI_STOP", rc);
    }

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::reset()
{
    APIRET  rc;
    MCI_GENERIC_PARMS GenericParms = {0};

    dprintf((__FUNCTION__));

    // Stop playback if current state == playing or paused
    if (DosInterlockedExchange((PLONG)&State, STATE_STOPPED)
        != STATE_STOPPED) {
        rc = mymciSendCommand(deviceId, MCI_STOP,
                              MCI_WAIT, (PVOID)&GenericParms, 0);
        if (LOUSHORT(rc) != MCIERR_SUCCESS)
            mciError("FlashWaveOut::reset - MCI_STOP", rc);
    }

    // Handle the wavehdrs that have already been processed.
    // Cycle thru every DART buffer, including those that should be
    // free to ensure that every wavehdr we know about is marked as done.
    int ndx = dartFreeNdx;
    for (int ctr = 0; ctr < dartBufCnt; ctr++) {
        LPWAVEHDR pwh = ((MIX_EXTRA*)pmixBuffers[ndx].ulUserParm)->pwh;
        if (pwh) {
            ((MIX_EXTRA*)pmixBuffers[ndx].ulUserParm)->pwh = 0;
            pwh->lpNext    = 0;
            pwh->dwFlags  |= WHDR_DONE;
            pwh->dwFlags  &= ~WHDR_INQUEUE;
            pwh->reserved  = 0;
            callback(WOM_DONE, (ULONG)pwh, 0);
        }
        if (++ndx >= dartBufCnt)
            ndx = 0;
    }

    // Handle the wavehdrs that have not been processed yet.
    while (wavehdr) {
        LPWAVEHDR pwh = wavehdr;
        wavehdr = pwh->lpNext;
        pwh->lpNext    = 0;
        pwh->dwFlags  |= WHDR_DONE;
        pwh->dwFlags  &= ~WHDR_INQUEUE;
        pwh->reserved  = 0;
        callback(WOM_DONE, (ULONG)pwh, 0);
    }

    // Reset variables to their initial state.
    waveOffs    = 0;
    dartFreeCnt = dartBufCnt;
    dartFreeNdx = 0;
    dartPlayed  = 0;
    readyCnt    = 0;
    readyNdx    = 0;

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

ULONG FlashWaveOut::getPosition()
{
    // This is the total of all bytes that have already been played.
    // It increases whenever the DART event callback is notified of
    // a free buffer (i.e. it is not a realtime, running byte counter).

    return (ULONG)dartPlayed;
}

/******************************************************************************/
/******************************************************************************/

MMRESULT FlashWaveOut::setVolume(ULONG ulVol)
{
    APIRET  rc;
    ULONG   ulVolR   = ((ulVol >> 16)    * 100) / 0xFFFF;
    ULONG   ulVolL   = ((ulVol & 0xffff) * 100) / 0xFFFF;
    MCI_SET_PARMS SetParms = {0};

    dprintf2((__FUNCTION__));

    volume = ulVol;

// Some drivers can't set left & right volumes independently
#ifdef GOOD_AUDIO_CARD_DRIVER
    SetParms.ulAudio = MCI_SET_AUDIO_LEFT;
    SetParms.ulLevel = ulVolL;

    rc = mymciSendCommand(deviceId, MCI_SET,
                          MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                          &SetParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS)
        mciError("FlashWaveOut::setVolume - MCI_SET (volume)", rc);

    SetParms.ulAudio = MCI_SET_AUDIO_RIGHT;
    SetParms.ulLevel = ulVolR;
#else
    SetParms.ulAudio = MCI_SET_AUDIO_ALL;
    SetParms.ulLevel = (ulVolR + ulVolL) / 2;
#endif

    rc = mymciSendCommand(deviceId, MCI_SET,
                          MCI_WAIT | MCI_SET_AUDIO | MCI_SET_VOLUME,
                          &SetParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS)
        mciError("FlashWaveOut::setVolume - MCI_SET (volume)", rc);

    return MMSYSERR_NOERROR;
}

/******************************************************************************/
/******************************************************************************/

LONG APIENTRY FlashWaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer,
                                  ULONG ulFlags)
{
    if (!pBuffer ||
        !pBuffer->ulUserParm ||
        !((MIX_EXTRA*)pBuffer->ulUserParm)->pThis) {
        fprintf(stderr, "FlashWaveOutHandler - no object ptr - "
                        "pBuffer= %x  ulStatus= %x  ulFlags= %x\n",
                pBuffer, ulStatus, ulFlags);
    }
    else {
        ((MIX_EXTRA*)pBuffer->ulUserParm)->pThis->handler(ulStatus, pBuffer, ulFlags);
    }

    return TRUE;
}

/******************************************************************************/
/******************************************************************************/

void FlashWaveOut::handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
    if (ulFlags == MIX_STREAM_ERROR) {

        // out of buffers, so pause playback
        if (ulStatus == ERROR_DEVICE_UNDERRUN) {
            pause();
            dprintf(("FlashWaveOut::handler - Hardware Underrun, state= %s",
                     (State == STATE_PLAYING) ? "playing" : "stopped"));
        }
        else
            dprintf(("FlashWaveOut::handler - Unknown Error= %x", ulStatus));
        return;
    }

    // If there are fewer than FLASH_PREFILL-1 buffers in use,
    // pause the device or else we're likely to have an underrun.
    // Do NOT inc dartFreeCnt until we're done with this buffer.
    // If this is the only one available, the other thread might
    // change something we need below.
    if (State == STATE_PLAYING && dartFreeCnt > dartBufCnt - FLASH_PREFILL)
        pause();

    // Get the wavehdr associated with this DART buffer (if any),
    // then clear the buffer's reference to it.
    LPWAVEHDR pwh = ((MIX_EXTRA*)pBuffer->ulUserParm)->pwh;
    ((MIX_EXTRA*)pBuffer->ulUserParm)->pwh = 0;

    // If the device is supposed to be stopped, don't update variables that
    // are used by the other thread because ::reset() may have cleared them.
    if (State != STATE_STOPPED) {
        // Add the size of this buffer to the total played.  A locked
        // add operation may not be necessary but is used "just in case".
        DosInterlockedExchangeAdd((PLONG)&dartPlayed, pBuffer->ulBufferLength);

        // Now that we're done with the buffer, we can inc the free count.
        // As above, the locked increament may be unnecessary.
        DosInterlockedIncrement((PLONG)&dartFreeCnt);
    }

    // On the first call to this function, create a Win32 TEB (equivalent
    // to an OS/2 TIB) for this thread.  Without it, the Win callback will
    // be called on this thread with the TEB for the primary thread (which
    // is probably a bad idea).  Note: the DART thread persists for the life
    // of the app, so all instances of FlashWaveOut will use the same TEB.

    static USHORT   odinFS = 0;
    if (!odinFS) {
        if (ODIN_ThreadEnterOdinContext(0, TRUE))
            odinFS = RestoreOS2FS();
        else {
            odinFS = (USHORT)-1;
            dprintf(("FlashWaveOut::handler - ODIN_ThreadEnterOdinContext failed"));
        }
    }

    // Switch to this thread's Win FS selector, then notify
    // the Win client that it's buffer has been played.
    if (pwh) {
        pwh->lpNext    = 0;
        pwh->reserved  = 0;
        pwh->dwFlags  &= ~WHDR_INQUEUE;
        pwh->dwFlags  |= WHDR_DONE;
        if (odinFS != (USHORT)-1)
            SetFS(odinFS);
        callback(WOM_DONE, (ULONG)pwh, 0);
        RestoreOS2FS();
    }

    return;
}

/******************************************************************************/
//  Debug messages
/******************************************************************************/

// Displays the text associated with an MCI error code.

void FlashWaveOut::mciError(const char * msg, ULONG rc)
{
#ifdef DEBUG
    char szError[64] = "";

    mymciGetErrorString(rc, szError, sizeof(szError));
    dprintf(("%s failed - '%s'", msg, szError));
#endif

    return;
}

/******************************************************************************/
/******************************************************************************/
#ifdef DEBUG

// This offers a potentially incorrect count of the number of WAVE
// buffers in use.  It saves the address of the very first buffer
// written, then counts how many writes occur before it's reused.
// It only displays a message if the count changes.

void showWriteMsg(LPWAVEHDR pwh, int free, int queued, BOOL fInit)
{
    static int          bufctr = 0;
    static int          buflast = 0;
    static LPWAVEHDR    pwhFirst = 0;

    bufctr++;
    if (!fInit) {
        pwhFirst = pwh;
    }
    else
    if (pwhFirst == pwh) {
        if (bufctr != buflast) {
            dprintf(("FlashWaveOut::write - bufctr= %d  free= %d  queued= %d",
                     bufctr, free, queued));
            buflast = bufctr;
        }
        bufctr = 0;
    }

    return;
}

#endif
/******************************************************************************/
/******************************************************************************/

