/* $Id: waveoutflash.h,v 1.00 2010-02-20 12:00:00 rlwalsh Exp $ */

/*
 * Wave playback class (DART for Flash10)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2010 Richard L Walsh (rich@e-vertise.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

/******************************************************************************/

#ifndef __WAVEOUTFLASH_H__
#define __WAVEOUTFLASH_H__

#include "waveoutbase.h"
#include "flashaudio.h"

#ifdef OS2_ONLY
#include "winmmtype.h"
#endif

// this should be defined in some #included .h file, but it isn't
typedef LONG (APIENTRY MIXERPROC)(ULONG ulHandle, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

/******************************************************************************/

class FlashWaveOut : public WaveOut
{
public:
                    FlashWaveOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen,
                                 ULONG nCallback, ULONG dwInstance);
  virtual           ~FlashWaveOut();

  virtual MMRESULT  open();
  virtual MMRESULT  write(LPWAVEHDR pwh, UINT cbwh);
  virtual MMRESULT  pause();
  virtual MMRESULT  stop();
  virtual MMRESULT  resume();
  virtual MMRESULT  setVolume(ULONG ulVol);
  virtual MMRESULT  reset();
  virtual ULONG     getPosition();

protected:
          BOOL      getDeviceSem(BOOL get);
          MMRESULT  initBuffers();
          void      writeBuffer();
          void      handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);
          void      mciError(char * msg, ULONG rc);

private:
   static HEV       deviceSem;
          USHORT    deviceId;
          int       waveOffs;
          BOOL      dartBufInit;
          int       dartBufSize;
          int       dartBufCnt;
 volatile int       dartFreeCnt;
          int       dartFreeNdx;
 volatile int       dartPlayed;
          int       readyCnt;
          int       readyNdx;
          ULONG     mixHandle;
        MIXERPROC * pmixWriteProc;
   MCI_MIX_BUFFER * pmixBuffers;

  friend LONG APIENTRY FlashWaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer,
                                           ULONG ulFlags);
};

#endif

/******************************************************************************/

