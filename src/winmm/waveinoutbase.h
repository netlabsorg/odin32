/* $Id: waveinoutbase.h,v 1.1 2001-03-23 16:23:45 sandervl Exp $ */

/*
 * Wave playback & recording base class
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __WAVEINOUTBASE_H__
#define __WAVEINOUTBASE_H__

#define STATE_STOPPED   0
#define STATE_PLAYING   1
#define STATE_PAUSED    2
#define STATE_RECORDING 3

#ifdef OS2_ONLY
#include "winmmtype.h"
#endif
#include <vmutex.h>

class WaveInOut
{
public:
              WaveInOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen, ULONG nCallback, ULONG dwInstance);
     virtual ~WaveInOut();

              int      getState()               { return State; };
              MMRESULT getError()               { return ulError; };
              ULONG    getSampleRate()          { return SampleRate; };
              ULONG    getBitsPerSample()       { return BitsPerSample; };
              ULONG    getnumChannels()         { return nChannels; };
              ULONG    getAvgBytesPerSecond()   { return (BitsPerSample/8) * nChannels * SampleRate; };

  static BOOL find(WaveInOut *wave);

protected:

    ULONG       ulError, State;

    int         SampleRate;
    int         BitsPerSample;
    int         nChannels;

    int         queuedbuffers;

    // callback interface
    void        callback(UINT uMessage, DWORD dw1, DWORD dw2);

    DWORD       fdwOpen;
    DWORD       dwCallback;
    DWORD       dwInstance;

    WAVEHDR    *wavehdr,
               *curhdr;

    VMutex      wmutex;
                                          // Linked list management
                WaveInOut *next;          // Next wave class
    static      WaveInOut *wave;          // List of wave classes

private:

};

#endif
