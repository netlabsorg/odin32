/* $Id: time.cpp,v 1.3 1999-08-18 18:49:18 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1998       original WINE Author
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * Conversion between Time and TimeFields
 *
 * RtlTimeToTimeFields, RtlTimeFieldsToTime and defines are taken from ReactOS and
 * adapted to wine with special permissions of the author
 * Rex Jolliff (rex@lvcablemodem.com)
 *
 *
 */

#include "ntdll.h"
#include <string.h>


#define TICKSPERSEC        10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       0
#define DAYSPERWEEK        7
#define EPOCHYEAR          1601
#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12

static const int YearLengths[2] = {DAYSPERNORMALYEAR, DAYSPERLEAPYEAR};
static const int MonthLengths[2][MONSPERYEAR] =
{
   { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
   { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static int IsLeapYear(int Year)
{
   return Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0) ? 1 : 0;
}

static void NormalizeTimeFields(CSHORT *FieldToNormalize,
                                CSHORT *CarryField,
                                int    Modulus)
{
   *FieldToNormalize = (CSHORT) (*FieldToNormalize - Modulus);
   *CarryField = (CSHORT) (*CarryField + 1);
}

/******************************************************************************
 *  RtlTimeToTimeFields                     [NTDLL.265]
 *
 */

/* @@@PH we need 64-bit arithmetics here */
#if 0
VOID WINAPI RtlTimeToTimeFields(
   PLARGE_INTEGER liTime,
   PTIME_FIELDS TimeFields)
{
   const int *Months;
   int LeapSecondCorrections, SecondsInDay, CurYear;
   int LeapYear, CurMonth, GMTOffset;
   long int Days;
   long long int Time = *(long long int *)&liTime;

   /* Extract millisecond from time and convert time into seconds */
   TimeFields->Milliseconds = (CSHORT) ((Time % TICKSPERSEC) / TICKSPERMSEC);
   Time = Time / TICKSPERSEC;

   /* FIXME: Compute the number of leap second corrections here */
   LeapSecondCorrections = 0;

   /* FIXME: get the GMT offset here */
   GMTOffset = 0;

   /* Split the time into days and seconds within the day */
   Days = Time / SECSPERDAY;
   SecondsInDay = Time % SECSPERDAY;

   /* Adjust the values for GMT and leap seconds */
   SecondsInDay += (GMTOffset - LeapSecondCorrections);
   while (SecondsInDay < 0)
   { SecondsInDay += SECSPERDAY;
     Days--;
   }
   while (SecondsInDay >= SECSPERDAY)
   { SecondsInDay -= SECSPERDAY;
     Days++;
   }

   /* compute time of day */
   TimeFields->Hour = (CSHORT) (SecondsInDay / SECSPERHOUR);
   SecondsInDay = SecondsInDay % SECSPERHOUR;
   TimeFields->Minute = (CSHORT) (SecondsInDay / SECSPERMIN);
   TimeFields->Second = (CSHORT) (SecondsInDay % SECSPERMIN);

   /* FIXME: handle the possibility that we are on a leap second (i.e. Second = 60) */

   /* compute day of week */
   TimeFields->Weekday = (CSHORT) ((EPOCHWEEKDAY + Days) % DAYSPERWEEK);

   /* compute year */
   CurYear = EPOCHYEAR;
   /* FIXME: handle calendar modifications */
   while (1)
   { LeapYear = IsLeapYear(CurYear);
     if (Days < (long) YearLengths[LeapYear])
     { break;
     }
     CurYear++;
     Days = Days - (long) YearLengths[LeapYear];
   }
   TimeFields->Year = (CSHORT) CurYear;

   /* Compute month of year */
   Months = MonthLengths[LeapYear];
   for (CurMonth = 0; Days >= (long) Months[CurMonth]; CurMonth++)
     Days = Days - (long) Months[CurMonth];
   TimeFields->Month = (CSHORT) (CurMonth + 1);
   TimeFields->Day = (CSHORT) (Days + 1);
}


/******************************************************************************
 *  RtlTimeFieldsToTime                     [NTDLL.265]
 *
 */
BOOLEAN WINAPI RtlTimeFieldsToTime(
   PTIME_FIELDS tfTimeFields,
   PLARGE_INTEGER Time)
{
   int CurYear, CurMonth;
   long long int rcTime;
   TIME_FIELDS TimeFields = *tfTimeFields;

   rcTime = 0;

   /* FIXME: normalize the TIME_FIELDS structure here */
   while (TimeFields.Second >= SECSPERMIN)
   { NormalizeTimeFields(&TimeFields.Second, &TimeFields.Minute, SECSPERMIN);
   }
   while (TimeFields.Minute >= MINSPERHOUR)
   { NormalizeTimeFields(&TimeFields.Minute, &TimeFields.Hour, MINSPERHOUR);
   }
   while (TimeFields.Hour >= HOURSPERDAY)
   { NormalizeTimeFields(&TimeFields.Hour, &TimeFields.Day, HOURSPERDAY);
   }
   while (TimeFields.Day > MonthLengths[IsLeapYear(TimeFields.Year)][TimeFields.Month - 1])
   { NormalizeTimeFields(&TimeFields.Day, &TimeFields.Month, SECSPERMIN);
   }
   while (TimeFields.Month > MONSPERYEAR)
   { NormalizeTimeFields(&TimeFields.Month, &TimeFields.Year, MONSPERYEAR);
   }

   /* FIXME: handle calendar corrections here */
   for (CurYear = EPOCHYEAR; CurYear < TimeFields.Year; CurYear++)
   { rcTime += YearLengths[IsLeapYear(CurYear)];
   }
   for (CurMonth = 1; CurMonth < TimeFields.Month; CurMonth++)
   { rcTime += MonthLengths[IsLeapYear(CurYear)][CurMonth - 1];
   }
   rcTime += TimeFields.Day - 1;
   rcTime *= SECSPERDAY;
   rcTime += TimeFields.Hour * SECSPERHOUR + TimeFields.Minute * SECSPERMIN + TimeFields.Second;
   rcTime *= TICKSPERSEC;
   rcTime += TimeFields.Milliseconds * TICKSPERMSEC;
   *Time = *(LARGE_INTEGER *)&rcTime;

   return TRUE;
}
#endif

/************* end of code by Rex Jolliff (rex@lvcablemodem.com) *******************/

/******************************************************************************
 *  RtlSystemTimeToLocalTime             [NTDLL]
 */
VOID WINAPI RtlSystemTimeToLocalTime(PLARGE_INTEGER SystemTime,
                                     PLARGE_INTEGER LocalTime)
{
  dprintf(("NTDLL: RtlSystemTimeToLocalTime(%08xh,%08xh) not implemented.\n",
           SystemTime,
           LocalTime));

  memcpy (LocalTime,
          SystemTime,
          sizeof (PLARGE_INTEGER));
}


/******************************************************************************
 *  RtlToTimeInSecondsSince1980             [NTDLL]
 */
BOOLEAN WINAPI RtlTimeToSecondsSince1980(LPFILETIME ft,
                                         LPDWORD    timeret)
{
  dprintf(("NTDLL: RtlTimeToSecondsSince1980(%08xh,%08xh) not implemented.\n",
           ft,
           timeret));

  /* 1980 = 1970+10*365 days +  29. februar 1972 + 29.februar 1976 */
  //*timeret = DOSFS_FileTimeToUnixTime(ft,NULL) - (10*365+2)*24*3600;
  return 1;
}

/******************************************************************************
 *  RtlToTimeInSecondsSince1970             [NTDLL]
 */
BOOLEAN WINAPI RtlTimeToSecondsSince1970(LPFILETIME ft,
                                         LPDWORD timeret)
{
  dprintf(("NTDLL: RtlTimeToSecondsSince1970(%08xh,%08xh) not implemented.\n",
           ft,
           timeret));

  //*timeret = DOSFS_FileTimeToUnixTime(ft,NULL);
  return 1;
}


/******************************************************************************
 *  RtlSecondsSince1970ToTime             [NTDLL]
 */
BOOLEAN WINAPI RtlSecondsSince1970ToTime(LPSYSTEMTIME st,
                                         LPDWORD      timeret)
{
  dprintf(("NTDLL: RtlSecondsSince1970ToTime(%08xh,%08xh) not implemented.\n",
           st,
           timeret));

  //*timeret = DOSFS_UnixTimeToFileTime(st,NULL);
  return 1;
}


/******************************************************************************
 * RtlTimeToElapsedTimeFields [NTDLL.502]
 * FIXME: prototype guessed
 */
VOID WINAPI RtlTimeToElapsedTimeFields(PLARGE_INTEGER liTime,
                                       PTIME_FIELDS   TimeFields)
{
  dprintf(("NTDLL: RtlTimeToElapsedTimeFields(%08xh,%08xh) not implemented.\n",
           liTime,
           TimeFields));
}
