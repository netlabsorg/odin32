/* sys/times.h (emx+gcc) */

#ifndef _EMXSYS_TIMES_H
#define _EMXSYS_TIMES_H

#if defined (__cplusplus)
extern "C" {
#endif

#if !defined (_CLOCK_T)
#define _CLOCK_T
typedef long clock_t;
#endif

struct tms
{
  clock_t tms_utime;
  clock_t tms_stime;
  clock_t tms_cutime;
  clock_t tms_cstime;
};

long emx__times (struct tms *);

#if defined (__cplusplus)
}
#endif

#endif /* not _SYS_TIMES_H */
