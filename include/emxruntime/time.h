/* time.h (emx+gcc) */

#ifndef _EMXTIME_H
#define _EMXTIME_H

#ifndef CRTWRAP
#define CRTWRAP(a)   emx_##a
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#if !defined (_SIZE_T)
#define _SIZE_T
typedef unsigned long size_t;
#endif

#if !defined (NULL)
#if defined (__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#if !defined (_TIME_T)
#define _TIME_T
#ifdef _SQUID_TIME_
typedef signed long time_t;
#else
typedef unsigned long time_t;
#endif
#endif

#if !defined (_CLOCK_T)
#define _CLOCK_T
typedef long clock_t;
#endif

#if !defined (_TM)
#define _TM
struct tm         /* cf. <emx/thread.h> */
{
  int tm_sec;     /* 0..59 */
  int tm_min;     /* 0..59 */
  int tm_hour;    /* 0..23 */
  int tm_mday;    /* 1..31 */
  int tm_mon;     /* 0..11 */
  int tm_year;    /* 0(:=1900).. */
  int tm_wday;    /* 0..6 */
  int tm_yday;    /* 0..365 */
  int tm_isdst;   /* 0 */
};
#endif

#if !defined (CLOCKS_PER_SEC)
#define CLOCKS_PER_SEC 100
#endif

#define asctime CRTWRAP(asctime)
#define ctime CRTWRAP(ctime)
#define clock CRTWRAP(clock)
#define difftime CRTWRAP(difftime)
#define gmtime CRTWRAP(gmtime)
#define localtime CRTWRAP(localtime)
#define mktime CRTWRAP(mktime)
#define strftime CRTWRAP(strftime)
#define time CRTWRAP(time)

char *asctime (__const__ struct tm *);
char *ctime (__const__ time_t *);
clock_t clock (void);
double difftime (time_t, time_t);
struct tm *gmtime (__const__ time_t *);
struct tm *localtime (__const__ time_t *);
time_t mktime (struct tm *);
size_t strftime (char *, size_t, __const__ char *, __const__ struct tm *);
time_t time (time_t *);


#if !defined (__STRICT_ANSI__)

/* POSIX.1 */
#define tzset CRTWRAP(tzset)

void tzset (void);

#endif


#if !defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)

#if !defined (CLK_TCK)
#define CLK_TCK 100
#endif

#define daylight CRTWRAP(daylight)
#define timezone CRTWRAP(timezone)
#define tzname CRTWRAP(tzname)
#define strptime CRTWRAP(strptime)

extern int daylight;
extern long timezone;
extern char *tzname[2];

char *strptime (__const__ char *, __const__ char *, struct tm *);

#endif


#if (!defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)) \
    || defined (_WITH_UNDERSCORE)

#define _daylight CRTWRAP(_daylight)
#define _timezone CRTWRAP(_timezone)
#define _tzname CRTWRAP(_tzname)
#define _strptime CRTWRAP(_strptime)
#define _tzset CRTWRAP(_tzset)

extern int _daylight;
extern long _timezone;
extern char *_tzname[2];

char *_strptime (__const__ char *, __const__ char *, struct tm *);
void _tzset (void);

#endif


#if defined (__cplusplus)
}
#endif

#endif /* not _TIME_H */
