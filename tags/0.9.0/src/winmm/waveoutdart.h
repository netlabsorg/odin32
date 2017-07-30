/* $Id: waveoutdart.h,v 1.4 2003-04-03 14:04:50 sandervl Exp $ */

/*
 * Wave playback class (DART)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __DWAVEOUT_H__
#define __DWAVEOUT_H__

#include "waveoutbase.h"

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

class DartWaveOut : public WaveOut
{
public:
                    DartWaveOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen,
                                ULONG nCallback, ULONG dwInstance);
  virtual           ~DartWaveOut();

  virtual MMRESULT  open();
  virtual MMRESULT  write(LPWAVEHDR pwh, UINT cbwh);
  virtual MMRESULT  pause();
  virtual MMRESULT  stop();
  virtual MMRESULT  resume();
  virtual MMRESULT  setVolume(ULONG ulVol);
  virtual MMRESULT  reset();
  virtual ULONG     getPosition();

protected:
          MMRESULT  initBuffers();
          void      writeBuffer();
   static void      mciError(ULONG rc);
          void      handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

private:
          USHORT    DeviceId;
          BOOL      fMixerSetup;
          BOOL      fUnderrun;
          int       curFillBuf;
          int       curPlayBuf;
          ULONG     curFillPos;
          ULONG     curPlayPos;
          ULONG     ulBufSize;
          ULONG     ulBufCount;
          ULONG     bytesPlayed;
          ULONG     bytesCopied;
          ULONG     bytesReturned;
          ULONG     ulUnderrunBase;
          ULONG     mixHandle;
          WAVEHDR * curhdr;
        MIXERPROC * pmixWriteProc;
   MCI_MIX_BUFFER * MixBuffer;
 MCI_BUFFER_PARMS * BufferParms;

  friend LONG APIENTRY WaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer,
                                      ULONG ulFlags);
};

#endif
