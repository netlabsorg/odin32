/* $Id: waveoutdaud.h,v 1.3 2001-04-06 14:36:43 sandervl Exp $ */

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
#include <daudio.h>

#ifdef _OS2WIN_H
typedef DWORD HEV;
#endif

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


     static   BOOL     isDirectAudioAvailable();

protected:

private:
       MMRESULT resume();
       BOOL     handler();
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
