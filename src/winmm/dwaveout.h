/*
 * Wave playback class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DWAVEOUT_H__
#define __DWAVEOUT_H__

#define STATE_STOPPED   0
#define STATE_PLAYING   1
#define STATE_PAUSED    2

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

class DartWaveOut
{
public:
              DartWaveOut(LPWAVEFORMATEX pwfx, ULONG nCallback, ULONG dwInstance);
              DartWaveOut(LPWAVEFORMATEX pwfx, HWND hwndCallback);
              DartWaveOut(LPWAVEFORMATEX pwfx);
             ~DartWaveOut();

     MMRESULT getError();
     MMRESULT write(LPWAVEHDR pwh, UINT cbwh);
     MMRESULT pause();
     MMRESULT restart();
     int      getState() { return State; };
     MMRESULT reset();

  static BOOL queryFormat(ULONG formatTag, ULONG nChannels,
              ULONG nSamplesPerSec, ULONG sampleSize);

  static BOOL find(DartWaveOut *dwave);

protected:
  static void mciError(ULONG ulError);
     	 void Init(LPWAVEFORMATEX pwfx);
         void handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

private:
        USHORT DeviceId;
        ULONG  ulBufferCount;             /* Current file buffer     */
        ULONG  ulBufSize;

    ULONG  ulError, State;

    MCI_MIX_BUFFER     *MixBuffer;          /* Device buffers          */
    MCI_MIXSETUP_PARMS *MixSetupParms;          /* Mixer parameters        */
    MCI_BUFFER_PARMS   *BufferParms;                /* Device buffer parms     */

    int SampleRate;
    int BitsPerSample;
    int nChannels;
    int curPlayBuf, curFillBuf;
    int curFillPos, curPlayPos; //fillpos == pos in os2 mix buffer, bufpos == pos in win buffer

 LPDRVCALLBACK callback;
         HWND hwndCallback;
        DWORD dwInstance;
    WAVEHDR  *wavehdr, *curhdr;

     BOOL fMixerSetup;

      VMutex *wmutex;
                              // Linked list management
              DartWaveOut*    next;                   // Next Timer
    static    DartWaveOut*    waveout;                // List of Timer

#ifndef _OS2WIN_H
    friend    LONG APIENTRY WaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);
#endif
};

#endif
