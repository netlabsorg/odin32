/* $Id: os2timer.h,v 1.13 2003-10-13 09:18:37 sandervl Exp $ */

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
 * Definitions                                                              *
 ****************************************************************************/

 /* 2001-09-16 PH
  with the new OS2KRNL's dating 2001-09-14 and later, there is
  a special CLOCKSCALE feature added which allows for much finer
  granularity of the OS/2 system timers up to a maximum resolution
  of 2ms. This is supposed to be enough for most timing issues here.
  
  Note:
  we need to add support for dynamic detection of this feature.
  */
#define OS2TIMER_RESOLUTION_MINIMUM 1
#define OS2TIMER_RESOLUTION_MAXIMUM 0x7ffffffe


/****************************************************************************
 * Structures                                                               *
 ****************************************************************************/

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

        DWORD         getTimerID()         { return timerID; };
        void          setTimerID(DWORD id) { timerID = id; };

#ifdef DEBUG
        LONG          addRef();
#else
        LONG          addRef()         { return InterlockedIncrement(&refCount); };
#endif
        LONG          getRefCount()    { return refCount; };
        LONG          release();

protected:

private:
        HEV            TimerSem;
        HTIMER         TimerHandle;
        HANDLE         hTimerThread;
        DWORD          TimerThreadID;
        LPTIMECALLBACK clientCallback;
        DWORD          userData;
        DWORD          dwFlags; // corresponds with fuEvent

        BOOL           fFatal;
        int            TimerStatus;
        DWORD          timerID;
        enum {
                InActive = 0,
                Running,
                Stopped
        };

        LONG           refCount;

        static  int    timerPeriod;

                              // Linked list management
              OS2Timer*        next;                   // Next Timer
    static    OS2Timer*        timers;                 // List of Timer

};

#endif
