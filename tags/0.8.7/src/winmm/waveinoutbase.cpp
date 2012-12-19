/* $Id: waveinoutbase.cpp,v 1.7 2003-10-13 09:18:38 sandervl Exp $ */

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

/******************************************************************************/
// Includes
/******************************************************************************/

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

VMutex      wavemutex;
WaveInOut * WaveInOut::head = NULL;

/******************************************************************************/
/******************************************************************************/
WaveInOut::WaveInOut(LPWAVEFORMATEX pwfx, ULONG fdwOpen, ULONG nCallback, ULONG dwInstance)
{
    SampleRate    = pwfx->nSamplesPerSec;
    BitsPerSample = pwfx->wBitsPerSample;
    nChannels     = pwfx->nChannels;
    OpenFlags     = fdwOpen;
    Callback      = nCallback;
    Instance      = dwInstance;
    State         = STATE_STOPPED;
    queuedbuffers = 0;
    wavehdr       = NULL;
    next          = NULL;

    dprintf(("WaveInOutOpen: samplerate %d, numChan %d bps %d (%d), format %x", SampleRate, nChannels, BitsPerSample, pwfx->nBlockAlign, pwfx->wFormatTag));

    wavemutex.enter();

    if(head == NULL) {
        head = this;
    }
    else {
        WaveInOut *dwave = head;

        while(dwave->next) {
            dwave = dwave->next;
        }
        dwave->next = this;
    }
    wavemutex.leave();
}
/******************************************************************************/
/******************************************************************************/
WaveInOut::~WaveInOut()
{
    wavemutex.enter();

    State = STATE_STOPPED;

    if(head == this) {
        head = this->next;
    }
    else {
        WaveInOut *dwave = head;

        while(dwave->next != this) {
            dwave = dwave->next;
        }
        dwave->next = this->next;
    }
    wavemutex.leave();
}
/******************************************************************************/
/******************************************************************************/
void WaveInOut::callback(UINT uMessage, DWORD dw1, DWORD dw2)
{
    dprintf(("WINMM:WaveInOut::callback type %x, callback 0x%x (HDRVR h=%08xh, UINT uMessage=%08xh, DWORD dwUser=%08xh, DWORD dw1=%08xh, DWORD dw2=%08xh)",
             OpenFlags, Callback, this, uMessage, Instance, dw1, dw2));

    switch(OpenFlags & CALLBACK_TYPEMASK) {
    case CALLBACK_WINDOW:
        PostMessageA((HWND)Callback, uMessage, (WPARAM)this, dw1);
        break;

    case CALLBACK_TASK: // == CALLBACK_THREAD
        PostThreadMessageA(Callback, uMessage, (WPARAM)this, dw1);
        break;

    case CALLBACK_FUNCTION:
    {
        USHORT selTIB = GetFS(); // save current FS selector
        USHORT selCallback;
        LPDRVCALLBACK mthdCallback = (LPDRVCALLBACK)Callback;

        if(selTIB == SELECTOR_OS2_FS) {
             selCallback = GetProcessTIBSel();
        }
        else selCallback = selTIB;

        //TODO: may not be very safe. perhaps we should allocate a new TIB for the DART thread or let another thread do the actual callback
        if(selCallback)
        {
            SetFS(selCallback);      // switch to callback win32 tib selector (stored in WaveInOutOpen)

            //@@@PH 1999/12/28 Shockwave Flash seem to make assumptions on a
            // specific stack layout. Do we have the correct calling convention here?
            mthdCallback((HDRVR)this, uMessage, Instance, dw1, dw2);
            SetFS(selTIB);           // switch back to the saved FS selector
        }
        else {
            dprintf(("WARNING: no valid selector of main thread available (process exiting); skipping WaveInOut notify"));
        }
        break;
    }

    case CALLBACK_EVENT:
        SetEvent((HANDLE)Callback);
        break;

    case CALLBACK_NULL:
        break; //no callback

    default:
        dprintf(("WARNING: Unknown callback type %x %x", OpenFlags, Callback));
        break;
    } //switch
    dprintf2(("WINMM:WaveInOut::callback returned"));
}
/******************************************************************************/
//Delete all active wave objects
/******************************************************************************/
void WaveInOut::shutdown()
{
    dprintf(("WaveInOut::shutdown"));
    wavemutex.enter();
    while(head) {
        delete head;
    }
    wavemutex.leave();
    dprintf(("WaveInOut::shutdown end"));
}
/******************************************************************************/
/******************************************************************************/
BOOL WaveInOut::find(WaveInOut *dwave)
{
    wavemutex.enter();

    WaveInOut *curwave = head;

    while(curwave) {
        if(dwave == curwave) {
            wavemutex.leave();
            return(TRUE);
        }
        curwave = curwave->next;
    }
    wavemutex.leave();

    dprintf2(("WINMM:WaveInOut not found!\n"));
    return(FALSE);
}
/******************************************************************************/
/******************************************************************************/

