/* $Id: time.cpp,v 1.14 2001-10-03 13:47:59 sandervl Exp $ */

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
#include <handlemanager.h>

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
  //Returned by winmm.dll from NT4, SP6
  return 0x030A;
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
  
  /* 2001-09-16 PH
   add dynamic detection of OS/2's minimum timer resolution
   */
  
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
  OS2Timer *os2timer = NULL;

  if(HMHandleTranslateToOS2(IDEvent, (PULONG)&os2timer) != NO_ERROR) {
      dprintf(("invalid timer id"));
      return TIMERR_NOERROR; //TODO: should we return an error here??
  }
  HMHandleFree(IDEvent);
  // return OS2Timer::killEvent(UINT IDEvent)

  delete os2timer;
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
  ULONG     timerID = 0;

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

  if(HMHandleAllocate(&timerID, (ULONG)timer) != NO_ERROR) {
      dprintf(("HMHandleAllocate failed!!"));
      delete timer;
      return 0;
  }

  timer->setTimerID(timerID);
  if(timer->StartTimer(wDelay, wResolution, lptc, dwUser, fuEvent) == FALSE)
  {
    dprintf(("WINMM:timeSetEvent: couldn't start timer!\n"));
    delete(timer);
    return(0);
  }
  return(MMRESULT)timerID;
}

ULONG OPEN32API WinGetCurrentTime(ULONG hab);

inline ULONG _WinGetCurrentTime(ULONG a)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinGetCurrentTime(a);
    SetFS(sel);

    return yyrc;
} 

#undef  WinGetCurrentTime
#define WinGetCurrentTime _WinGetCurrentTime

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
              LPMMTIME, pTime,
              UINT,     cbTime)
{
  dprintf2(("timeGetSystemTime %x %d", pTime, cbTime));

  if(pTime == NULL || cbTime < sizeof(MMTIME)) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
  }
  pTime->wType = TIME_MS;
  pTime->u.ms = WinGetCurrentTime(0);

  SetLastError(ERROR_SUCCESS);
  return 0;
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
  time = WinGetCurrentTime(0);
  dprintf2(("timeGetTime %x", time));
#endif
  return time;
}

