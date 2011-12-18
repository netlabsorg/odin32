/*
 * msvcrt.dll date/time functions
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997,2000 Uwe Bonnes
 * Copyright 2000 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifdef __WIN32OS2__
#include <emxheader.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#else
#include "config.h"
#endif

#include <time.h>
#ifdef HAVE_SYS_TIMES_H
# include <sys/times.h>
#endif

#include "msvcrt.h"
#include "msvcrt/sys/timeb.h"
#include "msvcrt/time.h"


#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);


/* INTERNAL: Return formatted current time/date */
char* msvcrt_get_current_time(char* out, const char* format)
{
  static const MSVCRT_time_t bad_time = (MSVCRT_time_t)-1;
  time_t t;
  struct tm *_tm = NULL;
  char *retval = NULL;

  if (time(&t) != bad_time && (_tm = localtime(&t)) &&
      strftime(out,9,format,_tm) == 8)
    retval = out;
  return retval;
}

/**********************************************************************
 *		mktime (MSVCRT.@)
 */
MSVCRT_time_t MSVCRT_mktime(struct MSVCRT_tm *t)
{
  MSVCRT_time_t res;
  dprintf(("MSVCRT: mktime"));
  struct tm tm;

  tm.tm_sec = t->tm_sec;
  tm.tm_min = t->tm_min;
  tm.tm_hour = t->tm_hour;
  tm.tm_mday = t->tm_mday;
  tm.tm_mon = t->tm_mon;
  tm.tm_year = t->tm_year;
  tm.tm_isdst = t->tm_isdst;
  res = mktime(&tm);
  if (res != -1)
  {
      t->tm_sec = tm.tm_sec;
      t->tm_min = tm.tm_min;
      t->tm_hour = tm.tm_hour;
      t->tm_mday = tm.tm_mday;
      t->tm_mon = tm.tm_mon;
      t->tm_year = tm.tm_year;
      t->tm_isdst = tm.tm_isdst;
  }
  return res;
}

/**********************************************************************
 *		_strdate (MSVCRT.@)
 */
char* MSVCRT__strdate(char* date)
{
  dprintf(("MSVCRT: _strdate"));
  return msvcrt_get_current_time(date,"%m/%d/%y");
}

/*********************************************************************
 *		_strtime (MSVCRT.@)
 */
char* MSVCRT__strtime(char* date)
{
  dprintf(("MSVCRT: _strtime"));
  return msvcrt_get_current_time(date,"%H:%M:%S");
}

clock_t emx__times(struct tms *) __asm__("emx__times");

/*********************************************************************
 *		clock (MSVCRT.@)
 */
MSVCRT_clock_t MSVCRT_clock(void)
{
  struct tms alltimes;
  clock_t res;

  dprintf(("MSVCRT: clock"));

  emx__times(&alltimes);
  res = alltimes.tms_utime + alltimes.tms_stime +
        alltimes.tms_cutime + alltimes.tms_cstime;
  /* FIXME: We need some symbolic representation for CLOCKS_PER_SEC,
   *  10 holds only for Windows/Linux_i86)
   */
  return 10*res;
}

/*********************************************************************
 *		difftime (MSVCRT.@)
 */
double MSVCRT_difftime(MSVCRT_time_t time1, MSVCRT_time_t time2)
{
    dprintf(("MSVCRT: difftime"));
    return (double)(time1 - time2);
}

/*********************************************************************
 *		time (MSVCRT.@)
 */
MSVCRT_time_t MSVCRT_time(MSVCRT_time_t* buf)
{
  MSVCRT_time_t curtime = time(NULL);
  dprintf(("MSVCRT: time"));
  return buf ? *buf = curtime : curtime;
}

/*********************************************************************
 *		_ftime (MSVCRT.@)
 */
#ifdef __WIN32OS2__
void MSVCRT__ftime(struct _timeb *buf)
#else
void _ftime(struct _timeb *buf)
#endif
{
  dprintf(("MSVCRT: _ftime %p",buf));
  buf->time = MSVCRT_time(NULL);
  buf->millitm = 0; /* FIXME */
  buf->timezone = 0;
  buf->dstflag = 0;
}

/*********************************************************************
 *		_daylight (MSVCRT.@)
 */
int MSVCRT___daylight = 1; /* FIXME: assume daylight */

/*********************************************************************
 *		__p_daylight (MSVCRT.@)
 */
void *MSVCRT___p__daylight(void)
{
	return &MSVCRT___daylight;
}
