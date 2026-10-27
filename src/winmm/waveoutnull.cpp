/*
 * Wave playback class (null device) - see waveoutnull.h
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <string.h>
#include <process.h>

#include "misc.h"
#include "waveoutnull.h"

// OS/2 services used directly (the Win32 headers do not declare them)
extern "C" unsigned long _System DosSleep(unsigned long msec);
extern "C" unsigned long _System DosSetPriority(unsigned long scope, unsigned long cls, long delta, unsigned long id);
#define NW_PRTYS_THREAD   2
#define NW_PRTYC_REGULAR  2
#define NW_PRTYD_MAXIMUM  31


#define DBG_LOCALLOG    DBG_waveout
#include "dbglocal.h"

// Runs on a plain OS/2 thread, like the MMPM/2 (DART) buffer handler: the
// WaveInOut::callback() helper then switches to the process' main Win32 TIB, so
// the application's wave callback sees its own thread-local state.
static void _Optlink NullWaveThread(void *arg)
{
    ((NullWaveOut *)arg)->run();
}

NullWaveOut::NullWaveOut(LPWAVEFORMATEX pwfx, DWORD fdwOpen,
                         ULONG nCallback, ULONG dwInstance)
           : WaveOut(pwfx, fdwOpen, nCallback, dwInstance)
{
    hThread  = NULL;
    fQuit    = FALSE;
    fRunning = FALSE;
    lastTick = 0;
    played   = 0;
}

NullWaveOut::~NullWaveOut()
{
    fQuit = TRUE;
    if (hThread) {
        int i;
        for (i = 0; i < 200 && fRunning; i++)     // let the worker finish (max 2 s)
            DosSleep(10);
        hThread = NULL;
    }
    callback(WOM_CLOSE, 0, 0);
}

BOOL NullWaveOut::queryFormat(ULONG formatTag, ULONG nChannels,
                              ULONG nSamplesPerSec, ULONG sampleSize)
{
    return formatTag == WAVE_FORMAT_PCM && nChannels >= 1 && nChannels <= 2 &&
           (sampleSize == 8 || sampleSize == 16) &&
           nSamplesPerSec >= 1000 && nSamplesPerSec <= 96000;
}

MMRESULT NullWaveOut::open()
{
    if (!queryFormat(WAVE_FORMAT_PCM, nChannels, SampleRate, BitsPerSample))
        return WAVERR_BADFORMAT;

    fRunning = TRUE;
    hThread = (HANDLE)_beginthread(NullWaveThread, NULL, 0x8000, (void *)this);
    if ((int)hThread == -1) {
        hThread  = NULL;
        fRunning = FALSE;
        return MMSYSERR_NOMEM;
    }

    lastTick = GetTickCount();
    callback(WOM_OPEN, 0, 0);
    return MMSYSERR_NOERROR;
}

MMRESULT NullWaveOut::write(LPWAVEHDR pwh, UINT cbwh)
{
    wmutex.enter();
    pwh->lpNext   = NULL;
    pwh->reserved = pwh->dwBufferLength;     // bytes still to "play"
    if (wavehdr == NULL) {
        wavehdr  = pwh;
        lastTick = GetTickCount();
    }
    else {
        WAVEHDR *p = wavehdr;
        while (p->lpNext) p = p->lpNext;
        p->lpNext = pwh;
    }
    if (State == STATE_STOPPED)
        State = STATE_PLAYING;
    wmutex.leave();
    return MMSYSERR_NOERROR;
}

MMRESULT NullWaveOut::pause()
{
    wmutex.enter();
    if (State == STATE_PLAYING)
        State = STATE_PAUSED;
    wmutex.leave();
    return MMSYSERR_NOERROR;
}

MMRESULT NullWaveOut::resume()
{
    wmutex.enter();
    if (State == STATE_PAUSED) {
        State    = STATE_PLAYING;
        lastTick = GetTickCount();
    }
    wmutex.leave();
    return MMSYSERR_NOERROR;
}

MMRESULT NullWaveOut::stop()
{
    wmutex.enter();
    State = STATE_STOPPED;
    wmutex.leave();
    completeAll();
    return MMSYSERR_NOERROR;
}

MMRESULT NullWaveOut::reset()
{
    wmutex.enter();
    State  = STATE_STOPPED;
    played = 0;
    wmutex.leave();
    completeAll();
    return MMSYSERR_NOERROR;
}

MMRESULT NullWaveOut::setVolume(ULONG ulVol)
{
    volume = ulVol;
    return MMSYSERR_NOERROR;
}

ULONG NullWaveOut::getPosition()
{
    return played;
}

void NullWaveOut::completeAll()
{
    WAVEHDR *hdr;

    wmutex.enter();
    while (wavehdr) {
        hdr            = wavehdr;
        wavehdr        = hdr->lpNext;
        hdr->lpNext    = NULL;
        hdr->dwFlags  |= WHDR_DONE;
        hdr->dwFlags  &= ~WHDR_INQUEUE;
        hdr->reserved  = 0;
        wmutex.leave();
        callback(WOM_DONE, (ULONG)hdr, 0);
        wmutex.enter();
    }
    wmutex.leave();
}

// Consume queued data at the stream's real-time rate and hand back each
// buffer once its duration has elapsed.
void NullWaveOut::run()
{
    // above ordinary application threads (so buffer completions are not starved
    // by an application spinning on a flag), below time-critical
    DosSetPriority(NW_PRTYS_THREAD, NW_PRTYC_REGULAR, NW_PRTYD_MAXIMUM, 0);

    while (!fQuit) {
        DosSleep(10);

        wmutex.enter();
        ULONG now = GetTickCount();
        if (State == STATE_PLAYING && wavehdr) {
            ULONG elapsed = now - lastTick;
            ULONG bps     = getAvgBytesPerSecond();
            ULONG budget  = (ULONG)(((double)elapsed * bps) / 1000.0);

            if (budget) {
                lastTick = now;
                while (budget && wavehdr) {
                    WAVEHDR *hdr = wavehdr;
                    ULONG use = hdr->reserved < budget ? hdr->reserved : budget;

                    hdr->reserved -= use;
                    budget        -= use;
                    played        += use;
                    if (hdr->reserved == 0) {
                        wavehdr        = hdr->lpNext;
                        hdr->lpNext    = NULL;
                        hdr->dwFlags  |= WHDR_DONE;
                        hdr->dwFlags  &= ~WHDR_INQUEUE;
                        wmutex.leave();
                        callback(WOM_DONE, (ULONG)hdr, 0);
                        wmutex.enter();
                    }
                }
            }
        }
        else
            lastTick = now;
        if (State == STATE_PLAYING && wavehdr == NULL)
            State = STATE_STOPPED;          // queue drained: nothing playing
        wmutex.leave();
    }
    fRunning = FALSE;
}
