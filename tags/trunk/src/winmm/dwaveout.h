/* $Id: dwaveout.h,v 1.11 2001-03-19 19:28:38 sandervl Exp $ */

/*
 * Wave playback class
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Project Odin Software License can be found in LICENSE.TXT
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
     MMRESULT stop();
     MMRESULT restart();
     MMRESULT setVolume(ULONG ulVol);
     ULONG    getVolume() 		{ return volume; };
     int      getState() 		{ return State; };
     MMRESULT reset();
     ULONG    getPosition();

     ULONG    getSampleRate() 		{ return SampleRate; };
     ULONG    getBitsPerSample()        { return BitsPerSample; };
     ULONG    getnumChannels()          { return nChannels; };
     ULONG    getAvgBytesPerSecond()    { return (BitsPerSample/8) * nChannels * SampleRate; };

  static BOOL queryFormat(ULONG formatTag, ULONG nChannels,
                          ULONG nSamplesPerSec, ULONG sampleSize);
 
  static BOOL find(DartWaveOut *dwave);
  static int  getNumDevices();

  static void setDefaultVolume(ULONG volume);

protected:
  static void mciError(ULONG ulError);
     	 void Init(LPWAVEFORMATEX pwfx);
         void handler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);

private:
         void writeBuffer();

        USHORT DeviceId;
        ULONG  ulBufferCount;             /* Current file buffer     */
        ULONG  ulBufSize;

    ULONG  ulError, State;
    ULONG  volume;                        // Volume state

    MCI_MIX_BUFFER     *MixBuffer;          /* Device buffers          */
    MCI_MIXSETUP_PARMS *MixSetupParms;          /* Mixer parameters        */
    MCI_BUFFER_PARMS   *BufferParms;                /* Device buffer parms     */

    int   SampleRate;
    int   BitsPerSample;
    int   nChannels;
    int   curPlayBuf, curFillBuf;
    ULONG curFillPos, curPlayPos; //fillpos == pos in os2 mix buffer, bufpos == pos in win buffer
    ULONG readPos, writePos;

    // callback interface
    LPDRVCALLBACK mthdCallback; // pointer to win32 routine for the callback
    void          callback(HDRVR h, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);
    HWND          hwndCallback;
    DWORD         dwInstance;

    WAVEHDR  *wavehdr,
             *curhdr;

    BOOL      fMixerSetup;
    BOOL      fUnderrun;

    VMutex *wmutex;
                              // Linked list management
              DartWaveOut*    next;                   // Next Timer
    static    DartWaveOut*    waveout;                // List of Timer

    static    ULONG           defvolume;      //default volume for streams (if waveOutSetVolume called with NULL stream)

#ifndef _OS2WIN_H
    friend    LONG APIENTRY WaveOutHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);
#endif
};

#endif
