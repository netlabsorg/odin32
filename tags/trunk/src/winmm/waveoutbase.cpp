/* $Id: waveoutbase.cpp,v 1.1 2001-03-23 16:23:45 sandervl Exp $ */

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
#include "waveoutbase.h"

#define DBG_LOCALLOG    DBG_waveoutbase
#include "dbglocal.h"

#ifndef min
#define min(a, b) ((a > b) ? b : a)
#endif

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

//TODO: mulaw, alaw & adpcm
/******************************************************************************/
/******************************************************************************/
WaveOut::WaveOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
           : WaveInOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    bytesPlayed = bytesCopied = bytesReturned = 0;

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
//Called if waveOutSetVolume is called by the application with waveout handle NULL
//Sets the default volume of each waveout stream (until it's volume is changed
//with an appropriate waveOutSetVolume call)
/******************************************************************************/
void WaveOut::setDefaultVolume(ULONG volume)
{
  defvolume = volume;
}
/******************************************************************************/
/******************************************************************************/
ULONG WaveOut::defvolume = 0xFFFFFFFF;

