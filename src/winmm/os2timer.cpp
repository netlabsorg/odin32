/* $Id: os2timer.cpp,v 1.22 2003-10-13 09:18:37 sandervl Exp $ */

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
#include <win32api.h>
#include <wprocess.h>
#include <dbglog.h>
#include <vmutex.h>

#include "time.h"
#include "os2timer.h"

#define DBG_LOCALLOG	DBG_os2timer
#include "dbglocal.h"


/****************************************************************************
 * Structures                                                               *
 ****************************************************************************/
static VMutex timeMutex;

/****************************************************************************
 * Local Prototypes                                                         *
 ****************************************************************************/

static DWORD WIN32API TimerHlpHandler(LPVOID timer);



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
  : next(NULL)
{
  dprintf2(("WINMM:OS2Timer: OS2TimerResolution::OS2TimerResolution(%08xh,%08xh)\n",
           this,
           dwPeriod));

  timeMutex.enter();
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

  timeMutex.leave();

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
  dprintf2(("WINMM:OS2Timer: OS2TimerResolution::~OS2TimerResolution(%08xh)\n",
           this));


  timeMutex.enter();

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

  timeMutex.leave();
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
  dprintf2(("WINMM:OS2Timer: OS2TimerResolution::enterResolutionScope(%08xh)\n",
           dwPeriod));

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
  dprintf2(("WINMM:OS2Timer: OS2TimerResolution::leaveResolutionScope(%08xh)\n",
           dwPeriod));

  timeMutex.enter();
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
      timeMutex.leave();
      return TRUE;                 // OK, can remove the entry
    }
 }
 timeMutex.leave();

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
  timeMutex.enter();
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
  timeMutex.leave();

  dprintf(("WINMM:OS2Timer: OS2TimerResolution::queryCurrentResolution == %08xh\n",
           iMin));

  return iMin;
}





/******************************************************************************/
/******************************************************************************/
OS2Timer::OS2Timer() : TimerSem(0), TimerHandle(0), hTimerThread(0),
                  clientCallback(NULL), TimerStatus(Stopped), fFatal(FALSE),
                  next(NULL), timerID(0), refCount(0)
{
  dprintf(("WINMM:OS2Timer: OS2Timer::OS2Timer(%08xh)\n",
           this));

  timeMutex.enter();
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
  timeMutex.leave();
  
  // create timer semaphore
  int rc = DosCreateEventSem(NULL, &TimerSem, DC_SEM_SHARED, 0);
  if(rc != 0)
  {
    dprintf(("WINMM: OS2Timer: DosCreateEventSem failed rc=#%08xh\n", rc));
    return; // terminate thread
  }
 
  //increase reference count for creation
  addRef();

  //increase reference count since the thread will access the object
  addRef();  
  hTimerThread = CreateThread(NULL,
                              0x4000,
                              TimerHlpHandler,
                              (LPVOID)this,
                              0, // thread creation flags
                              &TimerThreadID);
  
  if (hTimerThread == NULL)
  {
    dprintf(("WINMM: OS2Timer: CreateThread failed rc=#%08xh\n", 
             GetLastError()));
    DosCloseEventSem(TimerSem);
  }
}
/******************************************************************************/
/******************************************************************************/
OS2Timer::~OS2Timer()
{
  dprintf(("WINMM:OS2Timer: OS2Timer::~OS2Timer(%08xh)\n",
           this));

  KillTimer();

  timeMutex.enter();
  OS2Timer *timer = OS2Timer::timers;

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

  timeMutex.leave();
}
/******************************************************************************/
/******************************************************************************/
BOOL OS2Timer::StartTimer(int period,
                          int resolution,
                          LPTIMECALLBACK lptc,
                          int dwUser,
                          int fuEvent)
{
  dprintf(("WINMM:OS2Timer: OS2Timer::StartTimer(%08xh, %08xh, %08xh, %08xh, %08xh, %08xh)\n",
           this,
           period,
           resolution,
           lptc,
           dwUser,
           fuEvent));

  APIRET rc;
  
  // has the thread been created properly?
  if(hTimerThread == NULLHANDLE)
    return(FALSE);

  if(TimerStatus == Stopped)
  {
    clientCallback = lptc;    // callback data (id / addr)
    userData       = dwUser;  // user-supplied data
    dwFlags        = fuEvent; // type of timer / callback

    if(fuEvent & TIME_PERIODIC)
      rc = DosStartTimer(period, (HSEM)TimerSem, &TimerHandle);
    else
      rc = DosAsyncTimer(period, (HSEM)TimerSem, &TimerHandle);

    if(rc)
    {

#ifdef DEBUG
      if(fuEvent & TIME_PERIODIC)
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
  dprintf(("WINMM:OS2Timer: OS2Timer::StopTimer(%08xh)\n",
           this));

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
  dprintf(("WINMM:OS2Timer: OS2Timer::KillTimer(%08xh)\n",
           this));

  fFatal = TRUE;
  
  StopTimer();
  
  DosPostEventSem(TimerSem);

  TimerStatus = InActive;
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
LONG OS2Timer::addRef()
{ 
////  dprintf2(("addRef %x -> refcount %x", this, refCount+1));
    return InterlockedIncrement(&refCount); 
};
#endif
/******************************************************************************/
//******************************************************************************
LONG OS2Timer::release()
{
#ifdef DEBUG
  if(refCount-1 < 0) {
      DebugInt3();
  }
#endif
  if(InterlockedDecrement(&refCount) == 0) {
      dprintf2(("marked for deletion -> delete now"));
      delete this;
      return 0;
  }
  return refCount;
}
/******************************************************************************/
//******************************************************************************
void OS2Timer::TimerHandler()
{
  ULONG   Count = 0;
  APIRET  rc = 0;       /* Return code  */

  dprintf(("WINMM: TimerHandler thread created (%08xh)\n",
           this));

  // save the current thread priority
  PTIB ptib = NULL;
  DosGetInfoBlocks(&ptib, NULL);
  ULONG prio = ptib->tib_ptib2->tib2_ulpri;

  // increase the thread priority to improve timer resolution
  DosSetPriority (PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0);

  while(!fFatal)
  {
    dprintf(("WINMM: OS2Timer::TimerHandler waiting on timer (%04xh, %08xh\n",
             dwFlags,
            clientCallback));

    rc = DosWaitEventSem(TimerSem, SEM_INDEFINITE_WAIT);
    if(rc) {
        dprintf(("DosWaitEventSem failed with %d", rc));
        DebugInt3();
    }
    rc = DosResetEventSem(TimerSem, &Count);
    if(rc) {
        dprintf(("DosResetEventSem failed with %d", rc));
        DebugInt3();
    }
    if(!fFatal)
    {
        // check timer running condition
        if (TimerStatus == Running)
        {
          dprintf(("WINMM: OS2Timer::TimerHandler firing (%04xh, %08xh\n",
                   dwFlags,
                  clientCallback));
          
          // process the event
          switch (dwFlags & 0x0030)
          {
            case TIME_CALLBACK_FUNCTION:
              if (clientCallback != NULL)
              {
                  // restore the original priority (we never know what the callback
                  // code does)
                  DosSetPriority (PRTYS_THREAD, (prio >> 8) & 0xFF, 0, 0);

                  clientCallback((UINT)timerID, 0, userData, 0, 0);

                  DosSetPriority (PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0);
              }
              break;
            
            case TIME_CALLBACK_EVENT_SET:
              SetEvent( (HANDLE)clientCallback );
              break;
            
            case TIME_CALLBACK_EVENT_PULSE:
              PulseEvent( (HANDLE)clientCallback );
              break;
            
            default:
              dprintf(("WINMM: OS2Timer %08xh: unknown type for mmtime callback(%08xh)\n",
                       this,
                       dwFlags));
          }
        }
    }
  }
  
  // last message
  dprintf(("WINMM: OS2Timer: Timer thread terminating (%08xh)\n",
           this));
  
  DosCloseEventSem(TimerSem);

  //release object
  release();
}
//******************************************************************************
//******************************************************************************
static DWORD WIN32API TimerHlpHandler(LPVOID timer)
{
  ((OS2Timer *)timer)->TimerHandler();

  return 0;
}
//******************************************************************************
//******************************************************************************
OS2TimerResolution *OS2TimerResolution::sTimerResolutions = NULL;
OS2Timer           *OS2Timer::timers                      = NULL;
int                 OS2Timer::timerPeriod                 = 0;

