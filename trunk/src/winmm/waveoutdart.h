/* $Id: waveoutdart.h,v 1.1 2001-03-23 16:23:46 sandervl Exp $ */

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

#define PREFILLBUF_DART 4
#define DART_BUFSIZE    4096

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

class DartWaveOut : public WaveOut
{
public:
              DartWaveOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen, ULONG nCallback, ULONG dwInstance);
     virtual ~DartWaveOut();

     virtual  MMRESULT write(LPWAVEHDR pwh, UINT cbwh);
     virtual  MMRESULT pause();
     virtual  MMRESULT stop();
     virtual  MMRESULT restart();
     virtual  MMRESULT setVolume(ULONG ulVol);
     virtual  MMRESULT reset();
     virtual  ULONG    getPosition();

  static BOOL queryFormat(ULONG formatTag, ULONG nChannels,
                          ULONG nSamplesPerSec, ULONG sampleSize);

protected:
  static void mciError(ULONG ulError);
         void Init(LPWAVEFORMATEX pwfx);
         void handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

private:
         void writeBuffer();

        USHORT          DeviceId;
        ULONG           ulBufferCount;             /* Current file buffer     */
        ULONG           ulBufSize;

  MCI_MIX_BUFFER       *MixBuffer;          /* Device buffers          */
  MCI_MIXSETUP_PARMS   *MixSetupParms;          /* Mixer parameters        */
  MCI_BUFFER_PARMS     *BufferParms;                /* Device buffer parms     */

        int             curPlayBuf, curFillBuf;
        ULONG           curFillPos, curPlayPos; //fillpos == pos in os2 mix buffer, bufpos == pos in win buffer

        BOOL            fMixerSetup;
        BOOL            fUnderrun;

#ifndef _OS2WIN_H
        friend LONG APIENTRY WaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);
#endif
};

#endif
