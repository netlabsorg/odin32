/* $Id: winmm.cpp,v 1.1 1999-05-24 20:20:07 ktk Exp $ */

/*
 * Timer MM apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include "misc.h"

#include "os2timer.h"

/******************************************************************************/
UINT WIN32API mmsystemGetVersion(void)
{
  dprintf(("WINMM:mmsystemGetVersion\n"));
  return 4;
}

/******************************************************************************/
MMRESULT WIN32API timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc)
{
    dprintf(("WINMM:timeGetDevCaps Not really Implemented\n"));
    ptc->wPeriodMin = 1;
    ptc->wPeriodMax = 20;
    return TIMERR_NOERROR;
}

/******************************************************************************/
MMRESULT WIN32API timeBeginPeriod(UINT cMilliseconds)
{
    dprintf(("WINMM:timeBeginPeriod %d ms\n", cMilliseconds));
    return TIMERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API timeEndPeriod(UINT cMilliseconds)
{
    dprintf(("WINMM:timeEndPeriod %d ms\n", cMilliseconds));
    return TIMERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API timeKillEvent(UINT IDEvent)
{
    dprintf(("WINMM:timeKillEvent Not Implemented\n"));
    delete((OS2Timer *)IDEvent);
    return TIMERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API timeSetEvent(UINT wDelay, UINT wResolution, LPTIMECALLBACK lptc,
                                  DWORD dwUser, UINT fuEvent)
{
 OS2Timer *timer;

    dprintf(("WINMM:timeSetEvent %d\n", wDelay));

    timer = new OS2Timer();
    if(timer == NULL) {
        return(0);
    }
    if(timer->StartTimer(wDelay, wResolution, lptc, dwUser, fuEvent) == FALSE) {
        dprintf(("WINMM:timeSetEvent: couldn't start timer!\n"));
        delete(timer);
        return(0);
    }
    return(MMRESULT)timer;
}

/******************************************************************************/
MMRESULT WIN32API timeGetSystemTime( LPMMTIME arg1, UINT  arg2)
{
    dprintf(("WINMM:timeGetSystemTime\n"));
    return O32_timeGetSystemTime(arg1, arg2);
}
