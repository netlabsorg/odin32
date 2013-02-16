/* $Id: waveinoutbase.h,v 1.3 2003-01-14 19:38:38 sandervl Exp $ */

/*
 * Wave playback & recording base class
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __WAVEINOUTBASE_H__
#define __WAVEINOUTBASE_H__

#define STATE_STOPPED               0
#define STATE_PLAYING               1
#define STATE_PAUSED                2
#define STATE_RECORDING             3
#define STATE_POSTPONE_RECORDING    4

#ifdef OS2_ONLY
#include "winmmtype.h"
#endif
#include <vmutex.h>

class WaveInOut
{
public:
                    WaveInOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen,
                              ULONG nCallback, ULONG dwInstance);
  virtual           ~WaveInOut();

          int       getState()               { return State; };
          ULONG     getSampleRate()          { return SampleRate; };
          ULONG     getBitsPerSample()       { return BitsPerSample; };
          ULONG     getnumChannels()         { return nChannels; };
          ULONG     getAvgBytesPerSecond()   { return (BitsPerSample/8) * nChannels * SampleRate; };

   static BOOL      find(WaveInOut *wave);
   static void      shutdown();

protected:
          void      callback(UINT uMessage, DWORD dw1, DWORD dw2);

          int       SampleRate;
          int       BitsPerSample;
          int       nChannels;
          DWORD     OpenFlags;
          DWORD     Callback;
          DWORD     Instance;
          ULONG     State;
          int       queuedbuffers;
          WAVEHDR * wavehdr;
          VMutex    wmutex;

private:
 static WaveInOut * head;          // List of wave classes 
        WaveInOut * next;          // Next wave class
};

#endif
