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

// this should be defined in some #included .h file, but it isn't
typedef LONG (APIENTRY MIXERPROC)(ULONG ulHandle, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

class DartWaveIn : public WaveInOut
{
public:
                     DartWaveIn(LPWAVEFORMATEX pwfx, DWORD fdwOpen,
                                ULONG nCallback, ULONG dwInstance);
  virtual            ~DartWaveIn();

          MMRESULT   open();
          MMRESULT   addBuffer(LPWAVEHDR pwh, UINT cbwh);
          MMRESULT   start();
          MMRESULT   stop();
          MMRESULT   reset();
          ULONG      getPosition();

   static int        getNumDevices();
   static BOOL       queryFormat(ULONG formatTag, ULONG nChannels,
                                 ULONG nSamplesPerSec, ULONG sampleSize);

protected:
   static void       mciError(ULONG rc);
          void       handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer,
                             ULONG ulFlags);

private:
          USHORT     DeviceId;
          ULONG      ulBufSize;
          BOOL       fMixerSetup;
          BOOL       fOverrun;
          ULONG      mixHandle;
         MIXERPROC * pmixReadProc;
    MCI_MIX_BUFFER * MixBuffer;
  MCI_BUFFER_PARMS * BufferParms;

#ifndef _OS2WIN_H
   friend LONG APIENTRY WaveInHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer,
                                      ULONG ulFlags);
#endif
};

#endif
