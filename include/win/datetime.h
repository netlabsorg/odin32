/* $Id: datetime.h,v 1.3 2000-02-25 16:58:33 cbratschi Exp $ */

/*
 * Date and time picker class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Alex Priem
 */

#ifndef __WINE_DATETIME_H
#define __WINE_DATETIME_H

typedef struct tagDATETIME_INFO
{
        COMCTL32_HEADER header;
        HWND hMonthCal;
        HWND hUpdown;
        SYSTEMTIME date;
        BOOL dateValid;
        HWND hwndCheckbut;
        RECT rect;
        RECT checkbox;
        RECT daytxt;
        RECT daynumtxt;
        RECT rmonthtxt;
        RECT yeartxt;
        RECT calbutton;
        int  select;
        HFONT hFont;
} DATETIME_INFO, *LPDATETIME_INFO;

extern VOID DATETIME_Register (VOID);
extern VOID DATETIME_Unregister (VOID);

#define DTHT_NONE     0
#define DTHT_MCPOPUP  1
#define DTHT_YEAR     2
#define DTHT_DAYNUM   3
#define DTHT_MONTH    4
#define DTHT_DAY      5
#define DTHT_CHECKBOX 6
#define DTHT_GOTFOCUS 128

#endif  /* __WINE_DATETIME_H */
