/* $Id: wintimer.h,v 1.3 1999-06-10 16:24:35 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINTIMER_H__
#define __WINTIMER_H__

#define TIMERR_BASE            96

#define TIMERR_NOERROR        (0)
#define TIMERR_NOCANDO        (TIMERR_BASE+1)
#define TIMERR_STRUCT         (TIMERR_BASE+33)

typedef void (CALLBACK TIMECALLBACK)(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
typedef TIMECALLBACK *LPTIMECALLBACK;

#define TIME_ONESHOT                0x0000
#define TIME_PERIODIC               0x0001
#define TIME_CALLBACK_FUNCTION      0x0000
#define TIME_CALLBACK_EVENT_SET     0x0010
#define TIME_CALLBACK_EVENT_PULSE   0x0020

typedef struct {
    UINT    wPeriodMin;
    UINT    wPeriodMax;
} TIMECAPS, *LPTIMECAPS;

#endif
