/* $Id: time.h,v 1.1 1999-08-31 15:04:11 phaller Exp $ */

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

#if 0
typedef struct
{
    UINT    wPeriodMin;
    UINT    wPeriodMax;
} TIMECAPS, *LPTIMECAPS;


typedef struct mmtime_tag
{
  UINT wType;

  union
  {
    DWORD ms;
    DWORD sample;
    DWORD cb;
    DWORD ticks;

    struct
    {
      BYTE hour;
      BYTE min;
      BYTE sec;
      BYTE frame;
      BYTE fps;
      BYTE dummy;
      BYTE pad[2];
    } smpte;

    struct
    {
      DWORD songptrpos;
    } midi;
  } u;
} MMTIME, *PMMTIME, *LPMMTIME;
#endif


#endif
