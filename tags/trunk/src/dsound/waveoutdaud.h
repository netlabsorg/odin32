/* $Id: waveoutdaud.h,v 1.2 2001-04-30 21:06:38 sandervl Exp $ */

/*
 * Wave playback class (DirectAudio)
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DWAVEOUTEX_H__
#define __DWAVEOUTEX_H__

#ifdef OS2_ONLY
#include "winmmtype.h"
#endif
#include <daudio.h>

#define STATE_STOPPED   0
#define STATE_PLAYING   1
#define STATE_PAUSED    2
#define STATE_RECORDING 3

#define SECTION_WINMM      "WINMM"
#define KEY_DIRECTAUDIO    "DirectAudio"

class DAudioWaveOut
{
public:
               DAudioWaveOut(LPWAVEFORMATEX pwfx);
              ~DAudioWaveOut();

              MMRESULT write(LPVOID lpBuffer, UINT ulSize);
              MMRESULT pause();
              MMRESULT stop();
              MMRESULT restart();
              MMRESULT setVolume(ULONG ulVol);
              ULONG    getPosition(PULONG pulWritePos);

              MMRESULT setProperty(int type, ULONG value);

              int      getState()               { return State; };
              MMRESULT getError()               { return ulError; };

     static   BOOL     queryFormat(ULONG formatTag, ULONG nChannels,
                                   ULONG nSamplesPerSec, ULONG sampleSize);


     static   BOOL     isDirectAudioAvailable();

protected:

private:
       MMRESULT resume();
       BOOL     handler();
       MMRESULT sendIOCTL(ULONG cmd, DAUDIO_CMD *pDataPacket);

        ULONG   volume;                        // Volume state
        ULONG   ulError;
        ULONG   State;

        BOOL    fUnderrun;

        HFILE   hDAudioDrv;
};

#endif
