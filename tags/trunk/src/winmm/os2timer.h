/* $Id: os2timer.h,v 1.4 1999-08-31 15:04:11 phaller Exp $ */

#ifndef __OS2TIMER_H__
#define __OS2TIMER_H__
/*
 * OS/2 Timer class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Patrick Haller     (phaller@gmx.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifdef _OS2WIN_H
 #define HEV    int
 #define HTIMER int
#endif


/****************************************************************************
 * Structures                                                               *
 ****************************************************************************/

#if 0
typedef struct _MMTIMEREVENT
{
  struct _MMTIMEREVENT* prev;
  struct _MMTIMEREVENT* next;

  DWORD           id;                    // event id
  DWORD           timeScheduled;         // system time to fire event
  DWORD           timePeriod;            // period if periodic event
  TID             tidCaller;             // thread ID of caller thread
  DWORD           dwUser;                // user supplied value
  LPTIMERCALLBACK lpCallback;            // address to call
  DWORD           dwFlags;               // event flags
} MMTIMEREVENT, *PMMTIMEREVENT, *LPTIMEREVENT;
#endif

/*
  addEvent
  removeEvent
  rescheduleEvent
  callbackCaller
*/


/****************************************************************************
 * Class: OS2TimerResolution                                                *
 ****************************************************************************/

class OS2TimerResolution
{
  public:
    // public entries
    static BOOL enterResolutionScope(int dwPeriod); // request timer resolution
    static BOOL leaveResolutionScope(int dwPeriod); // release resolution request
    static int  queryCurrentResolution();           // query maximum resolution

    // public variables
    int dwPeriod;

  protected:
    // constructors and destructors
    OS2TimerResolution(void);
    OS2TimerResolution(int dwPeriod);
    ~OS2TimerResolution();

    // simple linked list
    static OS2TimerResolution* sTimerResolutions; // list of resolution scoped
           OS2TimerResolution* next;              // link to next entry
};


/****************************************************************************
 * Class: OS2Timer                                                          *
 ****************************************************************************/

class OS2Timer
{
public:
              OS2Timer();
             ~OS2Timer();

        void          TimerHandler();
        BOOL          StartTimer(int period, int resolution, LPTIMECALLBACK lptc,
                                 int dwUser, int fuEvent);
        void          StopTimer();
        void          KillTimer();

protected:

private:
        HEV            TimerSem;
        HTIMER         TimerHandle;
        int            TimerThreadID;
        LPTIMECALLBACK clientCallback;
        DWORD          userData;

        BOOL           fFatal;
        int            TimerStatus;
        enum {
                InActive = 0,
                Running,
                Stopped
        };
        static  int    timerPeriod;

                              // Linked list management
              OS2Timer*        next;                   // Next Timer
    static    OS2Timer*        timers;                 // List of Timer

};

#endif
