/* $Id: waveoutbase.cpp,v 1.4 2001-10-24 22:47:42 sandervl Exp $ */

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
#include "waveoutbase.h"
#include "initterm.h"

#define DBG_LOCALLOG    DBG_waveoutbase
#include "dbglocal.h"

/******************************************************************************/
/******************************************************************************/

ULONG WaveOut::defvolume = 0xFFFFFFFF;

/******************************************************************************/
/******************************************************************************/
WaveOut::WaveOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
           : WaveInOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    volume = defvolume;

    dprintf(("waveOutOpen: samplerate %d, numChan %d bps %d (%d), format %x", SampleRate, nChannels, BitsPerSample, pwfx->nBlockAlign, pwfx->wFormatTag));
}
/******************************************************************************/
/******************************************************************************/
WaveOut::~WaveOut()
{
}
/******************************************************************************/
/******************************************************************************/
int WaveOut::getNumDevices()
{
    MCI_AMP_OPEN_PARMS AmpOpenParms;
    MCI_GENERIC_PARMS  GenericParms = {0};
    APIRET rc;

    // Try to open the device to see if it's there
    memset(&AmpOpenParms, 0, sizeof(AmpOpenParms));
    AmpOpenParms.usDeviceID = 0;
    AmpOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_AUDIO_AMPMIX;

    rc = mymciSendCommand(0, MCI_OPEN,
                          MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                          (PVOID) &AmpOpenParms, 0);

    if (LOUSHORT(rc) != MCIERR_SUCCESS)
        return 0;

    // Close the device
    mymciSendCommand(AmpOpenParms.usDeviceID, MCI_CLOSE,
                     MCI_WAIT,
                     (PVOID)&GenericParms, 0);

    return 1;
}
/******************************************************************************/
/******************************************************************************/
BOOL WaveOut::queryFormat(ULONG formatTag, ULONG nChannels,
                          ULONG nSamplesPerSec, ULONG wBitsPerSample)
{
    APIRET  rc;
    BOOL    winrc = TRUE;
    MCI_OPEN_PARMS            OpenParms;
    MCI_WAVE_GETDEVCAPS_PARMS DevCapsParms;
    MCI_GENERIC_PARMS         GenericParms = {0};
#ifdef DEBUG
    char szError[64] = "";
#endif

    dprintf(("WINMM: WaveOut::queryFormat %x srate=%d, nchan=%d, bps=%d", formatTag, nSamplesPerSec, nChannels, wBitsPerSample));

    // Open the device.
    memset(&OpenParms, 0, sizeof(OpenParms));
    OpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_WAVEFORM_AUDIO;

    rc = mymciSendCommand(0, MCI_OPEN,
                          MCI_WAIT | MCI_OPEN_TYPE_ID,
                          (PVOID)&OpenParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        #ifdef DEBUG
        mymciGetErrorString(rc, szError, sizeof(szError));
        dprintf(("WINMM: WaveOut::queryFormat: %s\n", szError));
        #endif
        return FALSE;
    }

    // See if the device can handle the specified format.
    memset(&DevCapsParms, 0, sizeof(MCI_WAVE_GETDEVCAPS_PARMS));
    DevCapsParms.ulBitsPerSample = wBitsPerSample;
    DevCapsParms.ulFormatTag     = DATATYPE_WAVEFORM;
    DevCapsParms.ulSamplesPerSec = nSamplesPerSec;
    DevCapsParms.ulChannels      = nChannels;
    DevCapsParms.ulFormatMode    = MCI_PLAY;
    DevCapsParms.ulItem          = MCI_GETDEVCAPS_WAVE_FORMAT;

    rc = mymciSendCommand(OpenParms.usDeviceID, MCI_GETDEVCAPS,
                          MCI_WAIT | MCI_GETDEVCAPS_EXTENDED | MCI_GETDEVCAPS_ITEM,
                          (PVOID)&DevCapsParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        #ifdef DEBUG
        mymciGetErrorString(rc, szError, sizeof(szError));
        dprintf(("WINMM: WaveOut::queryFormat: %s\n", szError));
        #endif
        winrc = FALSE;
    }

    // Close the device
    rc = mymciSendCommand(OpenParms.usDeviceID, MCI_CLOSE,
                          MCI_WAIT, (PVOID)&GenericParms, 0);
    if (LOUSHORT(rc) != MCIERR_SUCCESS) {
        #ifdef DEBUG
        mymciGetErrorString(rc, szError, sizeof(szError));
        dprintf(("WINMM: WaveOut::queryFormat: %s\n", szError));
        #endif
        winrc = FALSE;
    }

    return winrc;
}
/******************************************************************************/
// Called if waveOutSetVolume is called by the application with waveout handle NULL
// Sets the default volume of each waveout stream (until it's volume is changed
// with an appropriate waveOutSetVolume call)
/******************************************************************************/
void WaveOut::setDefaultVolume(ULONG volume)
{
    dprintf(("WaveOut::setDefaultVolume %x", volume));
    defvolume = volume;
}
/******************************************************************************/
/******************************************************************************/
DWORD WaveOut::getDefaultVolume()
{
    return defvolume;
}
/******************************************************************************/
/******************************************************************************/

