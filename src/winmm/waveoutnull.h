/*
 * Wave playback class (null device)
 *
 * Silent software device used when MMPM/2 is not usable (no sound hardware or
 * driver).  Buffers are "played" in real time and then reported done, so
 * applications that insist on a wave device keep running without sound.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __WAVEOUTNULL_H__
#define __WAVEOUTNULL_H__

#include "waveoutbase.h"

class NullWaveOut : public WaveOut
{
public:
                    NullWaveOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen,
                                ULONG nCallback, ULONG dwInstance);
  virtual           ~NullWaveOut();

  virtual MMRESULT  open();
  virtual MMRESULT  write(LPWAVEHDR pwh, UINT cbwh);
  virtual MMRESULT  pause();
  virtual MMRESULT  stop();
  virtual MMRESULT  resume();
  virtual MMRESULT  setVolume(ULONG ulVol);
  virtual MMRESULT  reset();
  virtual ULONG     getPosition();

  static  BOOL      queryFormat(ULONG formatTag, ULONG nChannels,
                                ULONG nSamplesPerSec, ULONG sampleSize);

          void      run();          // worker thread body

private:
          void      completeAll();  // return every queued buffer as done

          HANDLE    hThread;
          volatile BOOL fQuit;
          volatile BOOL fRunning;
          ULONG     lastTick;
          ULONG     played;         // bytes "played" so far
};

#endif
