/* $Id: dwavein.h,v 1.2 2001-03-19 19:28:38 sandervl Exp $ */

/*
 * Wave playback class
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DWAVEIN_H__
#define __DWAVEIN_H__

#define STATE_STOPPED     0
#define STATE_RECORDING   1

#define PREFILLBUF_DART_REC 	8
#define DART_BUFSIZE_REC    	16384

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

class DartWaveIn
{
public:
              DartWaveIn(LPWAVEFORMATEX pwfx, ULONG nCallback, ULONG dwInstance);
              DartWaveIn(LPWAVEFORMATEX pwfx, HWND hwndCallback);
              DartWaveIn(LPWAVEFORMATEX pwfx);
             ~DartWaveIn();

     MMRESULT getError();
     MMRESULT addBuffer(LPWAVEHDR pwh, UINT cbwh);
     MMRESULT start();
     MMRESULT stop();
     int      getState() 		{ return State; };
     MMRESULT reset();
     ULONG    getPosition();

     ULONG    getSampleRate() 		{ return SampleRate; };
     ULONG    getBitsPerSample()        { return BitsPerSample; };
     ULONG    getnumChannels()          { return nChannels; };
     ULONG    getAvgBytesPerSecond()    { return (BitsPerSample/8) * nChannels * SampleRate; };

  static BOOL queryFormat(ULONG formatTag, ULONG nChannels,
                          ULONG nSamplesPerSec, ULONG sampleSize);
 
  static BOOL find(DartWaveIn *dwave);
  static int  getNumDevices();

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

    // callback interface
    LPDRVCALLBACK mthdCallback; // pointer to win32 routine for the callback
    void          callback(HDRVR h, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);
    HWND          hwndCallback;
    DWORD         dwInstance;

    WAVEHDR      *wavehdr;

    BOOL      fMixerSetup;
    BOOL      fOverrun;

    VMutex *wmutex;
                              // Linked list management
              DartWaveIn*    next;                   // Next Timer
    static    DartWaveIn*    wavein;                 // List of Timer

#ifndef _OS2WIN_H
    friend    LONG APIENTRY WaveInHandler(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags);
#endif
};

#endif
