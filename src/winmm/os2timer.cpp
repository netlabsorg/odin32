/* $Id: os2timer.cpp,v 1.4 1999-06-19 10:54:48 sandervl Exp $ */

/*
 * OS/2 Timer class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSPROCESS
#define INCL_DOSDATETIME
#define INCL_DOSSEMAPHORES
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <process.h>
#include "win32type.h"
#include "wintimer.h"
#include "os2timer.h"
#include "misc.h"

static void _Optlink TimerHlpHandler(void *);

/******************************************************************************/
/******************************************************************************/
OS2Timer::OS2Timer() : TimerSem(0), TimerHandle(0), TimerThreadID(0),
	               clientCallback(NULL), TimerStatus(Stopped), fFatal(FALSE)
{
 OS2Timer *timer = OS2Timer::timers;

  if(timer != NULL) {
    while(timer->next != NULL) {
        timer = timer->next;
    }
    timer->next = this;
  }
  else  timers      = this;

  TimerThreadID = _beginthread(TimerHlpHandler, NULL, 0x4000, (void *)this);
  DosSleep(100);
}
/******************************************************************************/
/******************************************************************************/
OS2Timer::~OS2Timer()
{
 OS2Timer *timer = OS2Timer::timers;

  KillTimer();

  if(timer != this) {
    while(timer->next != this) {
        timer = timer->next;
    }
    timer->next = this->next;
  }
  else  timers = timer->next;
}
/******************************************************************************/
/******************************************************************************/
BOOL OS2Timer::StartTimer(int period, int resolution, LPTIMECALLBACK lptc,
        		  int dwUser, int fuEvent)
{
 APIRET rc;

  if(TimerThreadID == -1) {
    return(FALSE);
  }
  if(TimerStatus == Stopped) {
    clientCallback = lptc;
    userData       = dwUser;
        if(fuEvent == TIME_PERIODIC)
            rc = DosStartTimer(period, (HSEM)TimerSem, &TimerHandle);
    else    rc = DosAsyncTimer(period, (HSEM)TimerSem, &TimerHandle);
    if(rc) {
#ifdef DEBUG
        if(fuEvent == TIME_PERIODIC)
            WriteLog("DosStartTimer failed %d\n", rc);
        else    WriteLog("DosAsyncTimer failed %d\n", rc);
#endif
        return(FALSE);
    }
        TimerStatus = Running;
  }
  else  return(FALSE);  //already running (must use timeKillEvent first)
  return(TRUE);
}
/******************************************************************************/
/******************************************************************************/
void OS2Timer::StopTimer()
{
  if(TimerStatus == Running) {
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
  if(DosPostEventSem(TimerSem)) {//something went wrong
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

#ifdef DEBUG
    WriteLog("TimerHandler thread created\n");
#endif
    rc = DosSetPriority (PRTYS_THREAD,        /* Change a single thread */
                         PRTYC_TIMECRITICAL,  /* Time critical class    */
                         0L,                  /* Increase by 15         */
                         0L);                 /* Assume current thread  */

    rc = DosCreateEventSem(NULL, &TimerSem, DC_SEM_SHARED, 0);

    if(rc != 0)
        _endthread();

#ifdef DEBUG
    WriteLog("Semaphore created\n");
#endif
    TimerStatus = Stopped;

    while(!fFatal) {
        DosWaitEventSem(TimerSem, SEM_INDEFINITE_WAIT);
        DosResetEventSem(TimerSem, &Count);
        if(!fFatal) {
#ifdef DEBUG
////        WriteLog("T");
#endif
        clientCallback((UINT)this, 0, userData, 0, 0);
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
OS2Timer *OS2Timer::timers      = NULL;
int       OS2Timer::timerPeriod = 0;
