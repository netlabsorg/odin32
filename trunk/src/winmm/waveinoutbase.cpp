/* $Id: waveinoutbase.cpp,v 1.2 2001-03-23 17:43:46 sandervl Exp $ */

/*
 * Wave playback class (DART)
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/



#define  INCL_BASE
#define  INCL_OS2MM
#include <os2wrap.h>   //Odin32 OS/2 api wrappers
#include <os2mewrap.h> //Odin32 OS/2 MMPM/2 api wrappers
#include <stdlib.h>
#include <string.h>
#define  OS2_ONLY
#include <win32api.h>
#include <wprocess.h>

#include "misc.h"
#include "waveinoutbase.h"

#define DBG_LOCALLOG    DBG_waveinoutbase
#include "dbglocal.h"


/******************************************************************************/
/******************************************************************************/
WaveInOut::WaveInOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
{
    next            = NULL;
    wavehdr         = NULL;
    curhdr          = NULL;
    ulError         = 0;
    State           = STATE_STOPPED;
    queuedbuffers   = 0;

    BitsPerSample   = pwfx->wBitsPerSample;
    SampleRate      = pwfx->nSamplesPerSec;
    this->nChannels = pwfx->nChannels;

    dprintf(("WaveInOutOpen: samplerate %d, numChan %d bps %d (%d), format %x", SampleRate, nChannels, BitsPerSample, pwfx->nBlockAlign, pwfx->wFormatTag));

    State    = STATE_STOPPED;

    wmutex.enter(VMUTEX_WAIT_FOREVER);

    if(wave == NULL) {
        wave = this;
    }
    else {
        WaveInOut *dwave = wave;

        while(dwave->next) {
            dwave = dwave->next;
        }
        dwave->next = this;
    }
    wmutex.leave();

    this->fdwOpen = fdwOpen;
    dwCallback    = nCallback;
    this->dwInstance = dwInstance;
}
/******************************************************************************/
/******************************************************************************/
WaveInOut::~WaveInOut()
{
    wmutex.enter(VMUTEX_WAIT_FOREVER);

    State = STATE_STOPPED;

    if(wave == this) {
        wave = this->next;
    }
    else {
        WaveInOut *dwave = wave;

        while(dwave->next != this) {
            dwave = dwave->next;
        }
        dwave->next = this->next;
    }
    wmutex.leave();
}
/******************************************************************************/
/******************************************************************************/
void WaveInOut::callback(UINT uMessage, DWORD dw1, DWORD dw2)
{
    dprintf(("WINMM:WaveInOut::callback type %x (HDRVR h=%08xh, UINT uMessage=%08xh, DWORD dwUser=%08xh, DWORD dw1=%08xh, DWORD dw2=%08xh)\n",
             fdwOpen, this, uMessage, dwInstance, dw1, dw2));

    switch(fdwOpen & CALLBACK_TYPEMASK) {
    case CALLBACK_WINDOW:
        PostMessageA((HWND)dwCallback, uMessage, (WPARAM)this, dw1);
        break;

    case CALLBACK_TASK: // == CALLBACK_THREAD
        PostThreadMessageA(dwCallback, uMessage, (WPARAM)this, dw1);
        break;

    case CALLBACK_FUNCTION:
    {
        USHORT selTIB = GetFS(); // save current FS selector
        USHORT selCallback;
        LPDRVCALLBACK mthdCallback = (LPDRVCALLBACK)dwCallback;

        selCallback = GetProcessTIBSel();

        //TODO: may not be very safe. perhaps we should allocate a new TIB for the DART thread or let another thread do the actual callback
        if(selCallback)
        {
            SetFS(selCallback);      // switch to callback win32 tib selector (stored in WaveInOutOpen)

            //@@@PH 1999/12/28 Shockwave Flashes seem to make assumptions on a
            // specific stack layout. Do we have the correct calling convention here?
            mthdCallback((HDRVR)this, uMessage, dwInstance, dw1, dw2);
            SetFS(selTIB);           // switch back to the saved FS selector
        }
        else {
            dprintf(("WARNING: no valid selector of main thread available (process exiting); skipping WaveInOut notify"));
        }
    }

    case CALLBACK_EVENT:
        SetEvent((HANDLE)dwCallback);
        break;

    default:
        dprintf(("WARNING: Unknown callback type %x %x", fdwOpen, dwCallback));
        break;
    } //switch
    dprintf2(("WINMM:WaveInOut::callback returned"));
}
/******************************************************************************/
/******************************************************************************/
BOOL WaveInOut::find(WaveInOut *dwave)
{
 WaveInOut *curwave = wave;

    while(curwave) {
        if(dwave == curwave) {
            return(TRUE);
        }
        curwave = curwave->next;
    }

    dprintf2(("WINMM:WaveInOut not found!\n"));
    return(FALSE);
}
/******************************************************************************/
/******************************************************************************/
WaveInOut *WaveInOut::wave = NULL;

