/* $Id: winmm.cpp,v 1.4 1999-08-19 18:46:05 phaller Exp $ */

/*
 * Timer MM apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <odinwrap.h>
#include "misc.h"

#include "os2timer.h"

ODINDEBUGCHANNEL(WINMM-WINMM)


/******************************************************************************/
ODINFUNCTION0(UINT, mmsystemGetVersion)
{
  return 4;
}

/******************************************************************************/
ODINFUNCTION2(MMRESULT, timeGetDevCaps,
              LPTIMECAPS, ptc,
              UINT, cbtc)
{
  dprintf(("WINMM:timeGetDevCaps Not really Implemented\n"));
  ptc->wPeriodMin = 1;
  ptc->wPeriodMax = 20;
  return TIMERR_NOERROR;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, timeBeginPeriod,
              UINT, cMilliseconds)
{
  dprintf(("WINMM: timeBeginPeriod not implemented.\n"));
  return TIMERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, timeEndPeriod,
              UINT, cMilliseconds)
{
  dprintf(("WINMM: timeEndPeriod not implemented.\n"));
  return TIMERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION1(MMRESULT, timeKillEvent,
              UINT, IDEvent)
{
  dprintf(("WINMM:timeKillEvent Not Implemented\n"));
  delete((OS2Timer *)IDEvent);
  return TIMERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION5(MMRESULT, timeSetEvent,
              UINT, wDelay,
              UINT, wResolution,
              LPTIMECALLBACK, lptc,
              DWORD, dwUser,
              UINT, fuEvent)
{
  OS2Timer *timer;

  timer = new OS2Timer();
  if(timer == NULL)
      return(0);

  if(timer->StartTimer(wDelay, wResolution, lptc, dwUser, fuEvent) == FALSE)
  {
    dprintf(("WINMM:timeSetEvent: couldn't start timer!\n"));
    delete(timer);
    return(0);
  }
  return(MMRESULT)timer;
}

/******************************************************************************/
ODINFUNCTION2(MMRESULT, timeGetSystemTime,
              LPMMTIME, arg1,
              UINT, arg2)
{
  return O32_timeGetSystemTime(arg1, arg2);
}

