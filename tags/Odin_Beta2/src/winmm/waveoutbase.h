/* $Id: waveoutbase.h,v 1.3 2002-06-04 17:36:55 sandervl Exp $ */

/*
 * Wave playback class (base)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DWAVEOUTBASE_H__
#define __DWAVEOUTBASE_H__

#include "waveinoutbase.h"

#ifdef OS2_ONLY
#include "winmmtype.h"
#endif
#include <vmutex.h>

class WaveOut : public WaveInOut
{
public:
              WaveOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen, ULONG nCallback, ULONG dwInstance);
     virtual ~WaveOut();

     virtual  MMRESULT write(LPWAVEHDR pwh, UINT cbwh) = 0;
     virtual  MMRESULT pause() = 0;
     virtual  MMRESULT stop() = 0;
     virtual  MMRESULT resume() = 0;
     virtual  MMRESULT reset() = 0;
     virtual  ULONG    getPosition() = 0;
     virtual  MMRESULT setVolume(ULONG ulVol) = 0;
              ULONG    getVolume()              { return volume; };

  static int  getNumDevices();

  static void setDefaultVolume(ULONG volume);
 static DWORD getDefaultVolume();

protected:

    ULONG       bytesPlayed, bytesCopied, bytesReturned;
    ULONG       volume;                        // Volume state

    static      ULONG    defvolume;  //default volume for streams (if waveOutSetVolume called with NULL stream)

private:

};

#endif
