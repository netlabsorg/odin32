/* $Id: os2timer.h,v 1.1 1999-05-24 20:20:07 ktk Exp $ */

#ifndef __OS2TIMER_H__
#define __OS2TIMER_H__
/*
 * OS/2 Timer class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifdef _OS2WIN_H
 #define HEV    int
 #define HTIMER int
#endif

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
