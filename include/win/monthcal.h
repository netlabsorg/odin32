/* $Id: monthcal.h,v 1.2 1999-08-14 17:23:24 achimha Exp $ */

/*
 * Month calendar class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Alex Priem
 */

#ifndef __WINE_MONTHCAL_H
#define __WINE_MONTHCAL_H

#define MC_SEL_LBUTUP           0                       /* Left button released */
#define MC_SEL_LBUTDOWN         1                       /* Left button pressed in calendar */
#define MC_PREVPRESSED      1           /* Prev month button pressed */
#define MC_NEXTPRESSED      2           /* Next month button pressed */
#define MC_NEXTMONTHDELAY       500                     /* when continuously pressing `next */
                                                                                /* month', wait 500 ms before going */
                                                                                /* to the next month */
#define MC_NEXTMONTHTIMER   1                   /* Timer ID's */
#define MC_PREVMONTHTIMER   2

typedef struct tagMONTHCAL_INFO
{
    COLORREF bk;
    COLORREF txt;
    COLORREF titlebk;
    COLORREF titletxt;
    COLORREF monthbk;
    COLORREF trailingtxt;
        HFONT    hFont;
        HFONT    hBoldFont;
        int              textHeight;
        int              textWidth;
        int              firstDayplace; /* place of the first day of the current month */
        int              delta;                         /* scroll rate; # of months that the */
                          /* control moves when user clicks a scroll button */
        int      visible;               /* # of months visible */
        int      firstDay;              /* Start month calendar with firstDay's day */
        int              monthRange;
        MONTHDAYSTATE *monthdayState;
        SYSTEMTIME todaysDate;
        DWORD    currentMonth;
        DWORD    currentYear;
        int              status;                /* See MC_SEL flags */
        int              curSelDay;         /* current selected day */
        int              firstSelDay;   /* first selected day */
        int              maxSelCount;
        SYSTEMTIME minSel;
        SYSTEMTIME maxSel;
        DWORD   rangeValid;
        SYSTEMTIME minDate;
        SYSTEMTIME maxDate;

        RECT    rcClient;                       /* rect for whole client area */
        RECT    title;                          /* rect for the header above the calendar */
        RECT    titlebtnnext;           /* the `next month' button in the header */
        RECT    titlebtnprev;       /* the `prev month' button in the header */
        RECT    titlemonth;                     /* the `month name' txt in the header */
        RECT    titleyear;                      /* the `year number' txt in the header */
        RECT    prevmonth;                      /* day numbers of the previous month */
        RECT    nextmonth;                      /* day numbers of the next month */
        RECT    days;                   /* week numbers at left side */
        RECT    weeknums;                       /* week numbers at left side */
        RECT    today;                          /* `today: xx/xx/xx' text rect */
} MONTHCAL_INFO, *LPMONTHCAL_INFO;
/*
int mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};

char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};

char *days[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                "Thursday", "Friday", "Saturday", NULL};

char *monthtxt[] = {"January", "February", "March", "April", "May",
                      "June", "July", "August", "September", "October",
                      "November", "December"};

char *daytxt[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
int DayOfWeekTable[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
*/
extern void MONTHCAL_CopyTime (const SYSTEMTIME *from, SYSTEMTIME *to);
extern int MONTHCAL_CalculateDayOfWeek (DWORD day, DWORD month, DWORD year);

extern VOID MONTHCAL_Register (VOID);
extern VOID MONTHCAL_Unregister (VOID);

#endif  /* __WINE_MONTHCAL_H */
