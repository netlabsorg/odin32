/* $Id: time.cpp,v 1.10 2001-03-21 12:33:22 sandervl Exp $ */

/*
 * Timer MM apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Patrick Haller     (phaller@gmx.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/
#include <os2win.h>
#include <odinwrap.h>
#include <misc.h>

#include "os2timer.h"
#include "time.h"

#define DBG_LOCALLOG	DBG_time
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-TIME)



/*****************************************************************************
 * Name      : mmsystemGetVersion
 * Purpose   : determine version of MM system
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/


ODINFUNCTION0(UINT, mmsystemGetVersion)
{
  return 4;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(MMRESULT,   timeGetDevCaps,
              LPTIMECAPS, ptc,
              UINT,       cbtc)
{
  dprintf(("WINMM:timeGetDevCaps Not really Implemented\n"));

  ptc->wPeriodMin = OS2TIMER_RESOLUTION_MINIMUM;
  ptc->wPeriodMax = OS2TIMER_RESOLUTION_MAXIMUM;

  return TIMERR_NOERROR;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION1(MMRESULT, timeBeginPeriod,
              UINT,     cMilliseconds)
{
  if (TRUE == OS2TimerResolution::enterResolutionScope(cMilliseconds))
    return TIMERR_NOERROR;
  else
    return TIMERR_NOCANDO;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION1(MMRESULT, timeEndPeriod,
              UINT,     cMilliseconds)
{
  if (TRUE == OS2TimerResolution::leaveResolutionScope(cMilliseconds))
    return TIMERR_NOERROR;
  else
  {
    dprintf(("WINMM: timeEndPeriod didn't match timeBeginPeriod.\n"));
    return TIMERR_NOCANDO;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION1(MMRESULT, timeKillEvent,
              UINT,     IDEvent)
{
  dprintf(("WINMM:timeKillEvent\n"));

  // return OS2Timer::killEvent(UINT IDEvent)

  delete((OS2Timer *)IDEvent);
  return TIMERR_NOERROR;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(MMRESULT,       timeSetEvent,
              UINT,           wDelay,
              UINT,           wResolution,
              LPTIMECALLBACK, lptc,
              DWORD,          dwUser,
              UINT,           fuEvent)
{
  OS2Timer *timer;

// @@@PH 1999/10/26 hack for RA95
  if (wDelay      < OS2TIMER_RESOLUTION_MINIMUM)
  {
    dprintf(("WINMM:Time:timeSetEvent - Warning: requested delay too low (%08xh)\n",
             wDelay));
    wDelay = OS2TIMER_RESOLUTION_MINIMUM;
  }

  if (wResolution < OS2TIMER_RESOLUTION_MINIMUM)
  {
    dprintf(("WINMM:Time:timeSetEvent - Warning: requested resolution too low (%08xh)\n",
             wResolution));
    wResolution = OS2TIMER_RESOLUTION_MINIMUM;
  }


  // check parameters
  if ((wDelay < OS2TIMER_RESOLUTION_MINIMUM) ||
      (wDelay > OS2TIMER_RESOLUTION_MAXIMUM))
    return NULL;

   if (wResolution == 0)
     wResolution = OS2TIMER_RESOLUTION_MINIMUM;
   else
     if ((wResolution < OS2TIMER_RESOLUTION_MINIMUM) ||
         (wResolution > OS2TIMER_RESOLUTION_MAXIMUM))
      return NULL;

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


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(MMRESULT, timeGetSystemTime,
              LPMMTIME, arg1,
              UINT,     arg2)
{
  dprintf2(("timeGetSystemTime %x %d", arg1, arg2));
  return O32_timeGetSystemTime(arg1, arg2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

DWORD WIN32API timeGetTime()
{
 DWORD time;
#if 0
 LARGE_INTEGER lint;
 static LARGE_INTEGER freq;
 static BOOL fInit = FALSE;

  if(fInit == FALSE) {
     QueryPerformanceFrequency(&freq);
     freq.LowPart /= 1000;
     fInit = TRUE;
  }
  QueryPerformanceCounter(&lint);
  time = lint.LowPart/freq.LowPart;
  dprintf2(("timeGetTime %x (%x:%x)", time, lint.LowPart, lint.HighPart));
#else
  //SvL: TODO: Inaccurate
  time = O32_timeGetTime();
  dprintf2(("timeGetTime %x", time));
#endif
  return time;
}

