/* $Id: waveoutdaud.h,v 1.1 2001-03-23 16:23:47 sandervl Exp $ */

/*
 * Wave playback class (DirectAudio)
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DWAVEOUTEX_H__
#define __DWAVEOUTEX_H__

#include "waveoutbase.h"

class DAudioWaveOut : public WaveOut
{
public:
               DAudioWaveOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance);
     virtual  ~DAudioWaveOut();

     virtual  MMRESULT write(LPWAVEHDR pwh, UINT cbwh);
     virtual  MMRESULT pause();
     virtual  MMRESULT stop();
     virtual  MMRESULT restart();
     virtual  MMRESULT setVolume(ULONG ulVol);
     virtual  MMRESULT reset();
     virtual  ULONG    getPosition();

     static   BOOL     queryFormat(ULONG formatTag, ULONG nChannels,
                                   ULONG nSamplesPerSec, ULONG sampleSize);


protected:

private:
       MMRESULT resume();
       void     handler();
       MMRESULT sendIOCTL(ULONG cmd, DAUDIO_CMD *pDataPacket);

        HEV     hSem;

        ULONG   bytesPlayed, bytesReturned;
        int     queuedbuffers;

        BOOL    fUnderrun;
        HANDLE  hThread;
        DWORD   dwThreadID;

        HFILE   hDAudioDrv;

        friend  DWORD WIN32API DAudioThreadHandler(LPVOID pUserData);
};

#endif
