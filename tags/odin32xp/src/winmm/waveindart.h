/* $Id: waveindart.h,v 1.1 2001-03-23 16:23:44 sandervl Exp $ */

/*
 * Wave playback class
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DWAVEIN_H__
#define __DWAVEIN_H__

#include "waveinoutbase.h"

#define PREFILLBUF_DART_REC     8
#define DART_BUFSIZE_REC        16384

#ifdef OS2_ONLY
#include "winmmtype.h"
#endif
#include "vmutex.h"

#ifdef _OS2WIN_H
#define MCI_MIX_BUFFER      DWORD
#define PMCI_MIX_BUFFER     MCI_MIX_BUFFER *
#define MCI_MIXSETUP_PARMS  DWORD
#define MCI_BUFFER_PARMS    DWORD
#define MCI_PLAY_PARMS      DWORD
#endif

class DartWaveIn : public WaveInOut
{
public:
              DartWaveIn(LPWAVEFORMATEX pwfx, DWORD fdwOpen, ULONG nCallback, ULONG dwInstance);
     virtual ~DartWaveIn();

     MMRESULT addBuffer(LPWAVEHDR pwh, UINT cbwh);
     MMRESULT start();
     MMRESULT stop();
     int      getState()        { return State; };
     MMRESULT reset();
     ULONG    getPosition();


  static int  getNumDevices();

  static BOOL queryFormat(ULONG formatTag, ULONG nChannels,
                          ULONG nSamplesPerSec, ULONG sampleSize);

protected:
  static void mciError(ULONG ulError);
         void handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

private:
        USHORT DeviceId;
        ULONG  ulBufferCount;             /* Current file buffer     */
        ULONG  ulBufSize;

    MCI_MIX_BUFFER     *MixBuffer;          /* Device buffers          */
    MCI_MIXSETUP_PARMS *MixSetupParms;          /* Mixer parameters        */
    MCI_BUFFER_PARMS   *BufferParms;                /* Device buffer parms     */


    BOOL      fMixerSetup;
    BOOL      fOverrun;

#ifndef _OS2WIN_H
    friend    LONG APIENTRY WaveInHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);
#endif
};

#endif
