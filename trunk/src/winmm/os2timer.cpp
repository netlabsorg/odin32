/* $Id: os2timer.cpp,v 1.9 1999-08-31 15:39:20 phaller Exp $ */

/*
 * OS/2 Timer class
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

#define INCL_DOSPROCESS
#define INCL_DOSDATETIME
#define INCL_DOSSEMAPHORES
#include <os2wrap.h>      //Odin32 OS/2 api wrappers
#include <process.h>
#include <win32type.h>
#include <wprocess.h>
#include <misc.h>

#include "time.h"
#include "os2timer.h"



/****************************************************************************
 * Structures                                                               *
 ****************************************************************************/


/****************************************************************************
 * Local Prototypes                                                         *
 ****************************************************************************/

static void _Optlink TimerHlpHandler(void *);




/****************************************************************************
 * Class: OS2TimerResolution                                                *
 ****************************************************************************/


/*****************************************************************************
 * Name      : OS2TimerResolution::OS2TimerResolution
 * Purpose   : create a new entry in the resolution stack
 * Parameters: -
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

OS2TimerResolution::OS2TimerResolution(int dwPeriod)
{
  // add to linked list
  OS2TimerResolution *timeRes = OS2TimerResolution::sTimerResolutions;

  if(timeRes != NULL)
  {
    while(timeRes->next != NULL)
    {
      timeRes = timeRes->next;
    }
    timeRes->next = this;
  }
  else
    OS2TimerResolution::sTimerResolutions = this;

  this->dwPeriod = dwPeriod;
}


/*****************************************************************************
 * Name      : OS2TimerResolution::~OS2TimerResolution
 * Purpose   : remove entry from the linked list
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

OS2TimerResolution::~OS2TimerResolution()
{
  // remove from linked list
  OS2TimerResolution *timeRes = OS2TimerResolution::sTimerResolutions;

  // leaveResolutionScope() if still entered???

  if(timeRes != this)
  {
    while(timeRes->next != this)
    {
      timeRes = timeRes->next;
    }
    timeRes->next = this->next;
  }
  else
    OS2TimerResolution::sTimerResolutions = timeRes->next;
}


/*****************************************************************************
 * Name      : OS2TimerResolution::enterResolutionScope
 * Purpose   : set the currently requested timer resolution for this entry
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL OS2TimerResolution::enterResolutionScope(int dwPeriod)
{
  OS2TimerResolution* timeRes = new OS2TimerResolution(dwPeriod);
  if (timeRes != NULL)
    return TRUE;
  else
    return FALSE;
}


/*****************************************************************************
 * Name      : OS2TimerResolution::leaveResolutionScope
 * Purpose   : remove specified entry from the list if periods match
 * Parameters: int dwPeriod
 * Variables :
 * Result    : TRUE or FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL OS2TimerResolution::leaveResolutionScope(int dwPeriod)
{
  OS2TimerResolution* timeRes = OS2TimerResolution::sTimerResolutions;

  if (timeRes != NULL)
  {
    for(;                           // walk to the end of the list
        timeRes->next != NULL;
        timeRes = timeRes->next)
      ;

    if (timeRes->dwPeriod == dwPeriod) // do the requested period match?
    {
      delete timeRes;              // so delete that object
      return TRUE;                 // OK, can remove the entry
    }
 }
 return FALSE;                     // nope, mismatch !
}


/*****************************************************************************
 * Name      : OS2TimerResolution::queryCurrentResolution
 * Purpose   : determine the minimum resolution currently requested
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

int OS2TimerResolution::queryCurrentResolution()
{
  OS2TimerResolution *timeRes = OS2TimerResolution::sTimerResolutions;
  int                iMin = -1;

  if (timeRes != NULL)              // do we have an entry yet?
    for (;                          // walk the linked list
         timeRes->next != NULL;
         timeRes = timeRes->next)
    {
      if (timeRes->dwPeriod < iMin) // determine minimum time period
        iMin = timeRes->dwPeriod;
    }

  return iMin;
}





/******************************************************************************/
/******************************************************************************/
OS2Timer::OS2Timer() : TimerSem(0), TimerHandle(0), TimerThreadID(0),
                  clientCallback(NULL), TimerStatus(Stopped), fFatal(FALSE)
{
 OS2Timer *timer = OS2Timer::timers;

  if(timer != NULL)
  {
    while(timer->next != NULL)
    {
      timer = timer->next;
    }
    timer->next = this;
  }
  else
    timers = this;

  TimerThreadID = _beginthread(TimerHlpHandler, NULL, 0x4000, (void *)this);
  //@@@PH: why the wait? DosSleep(100);
}
/******************************************************************************/
/******************************************************************************/
OS2Timer::~OS2Timer()
{
  OS2Timer *timer = OS2Timer::timers;

  KillTimer();

  if(timer != this)
  {
    while(timer->next != this)
    {
      timer = timer->next;
    }
    timer->next = this->next;
  }
  else
    timers = timer->next;
}
/******************************************************************************/
/******************************************************************************/
BOOL OS2Timer::StartTimer(int period,
                          int resolution,
                          LPTIMECALLBACK lptc,
                          int dwUser,
                          int fuEvent)
{
  APIRET rc;

  if(TimerThreadID == -1)
  {
    return(FALSE);
  }

  if(TimerStatus == Stopped)
  {
    clientCallback = lptc;
    userData       = dwUser;

    if(fuEvent == TIME_PERIODIC)
      rc = DosStartTimer(period, (HSEM)TimerSem, &TimerHandle);
    else
      rc = DosAsyncTimer(period, (HSEM)TimerSem, &TimerHandle);

    if(rc)
    {

#ifdef DEBUG
      if(fuEvent == TIME_PERIODIC)
        WriteLog("DosStartTimer failed %d\n", rc);
      else
        WriteLog("DosAsyncTimer failed %d\n", rc);
#endif

      return(FALSE);
    }

    TimerStatus = Running;
  }
  else
    return(FALSE);  //already running (must use timeKillEvent first)

  return(TRUE);
}
/******************************************************************************/
/******************************************************************************/
void OS2Timer::StopTimer()
{
  if(TimerStatus == Running)
  {
    DosStopTimer(TimerHandle);
    TimerStatus = Stopped;
  }
}
/******************************************************************************/
/******************************************************************************/
void OS2Timer::KillTimer()
{
  fFatal = TRUE;
  DosStopTimer(TimerHandle);
  if(DosPostEventSem(TimerSem))
  {  //something went wrong
     DosKillThread(TimerThreadID);
     DosCloseEventSem(TimerSem);
  }
  TimerStatus = InActive;
}
/******************************************************************************/
//******************************************************************************
void OS2Timer::TimerHandler()
{
  ULONG   Count = 0;
  APIRET  rc = 0;       /* Return code  */
  USHORT  selTIB;

  dprintf(("WINMM: TimerHandler thread created\n"));

  rc = DosSetPriority (PRTYS_THREAD,        /* Change a single thread */
                       PRTYC_TIMECRITICAL,  /* Time critical class    */
                       0L,                  /* Increase by 15         */
                       0L);                 /* Assume current thread  */

  rc = DosCreateEventSem(NULL, &TimerSem, DC_SEM_SHARED, 0);

  if(rc != 0)
      _endthread();

  dprintf(("WINMM: OS2Timer:Semaphore created\n"));

  TimerStatus = Stopped;

  while(!fFatal)
  {
    DosWaitEventSem(TimerSem, SEM_INDEFINITE_WAIT);
    DosResetEventSem(TimerSem, &Count);
    if(!fFatal)
    {
        // @@@PH: we're calling the client with PRTYC_TIMECRITICAL !!!
        //        It'd be much nicer to call with original priority!

        selTIB = SetWin32TIB();
        clientCallback((UINT)this, 0, userData, 0, 0);
        SetFS(selTIB);
    }
  }
  DosCloseEventSem(TimerSem);
}
//******************************************************************************
//******************************************************************************
static void _Optlink TimerHlpHandler(void *timer)
{
  ((OS2Timer *)timer)->TimerHandler();

  _endthread();
}


//******************************************************************************
//******************************************************************************
OS2TimerResolution *OS2TimerResolution::sTimerResolutions = NULL;
OS2Timer           *OS2Timer::timers                      = NULL;
int                 OS2Timer::timerPeriod                 = 0;

