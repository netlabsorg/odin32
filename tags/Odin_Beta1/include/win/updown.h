/* $Id: updown.h,v 1.4 2001-03-31 13:23:34 sandervl Exp $ */

/*
 * Up-down class extra info
 *
 * Copyright 1997 Dimitrie O. Paun
 */

#ifndef __WINE_UPDOWN_H
#define __WINE_UPDOWN_H

#include "windef.h"
#include "commctrl.h"

#define UPDOWN_BUDDYCLASSNAMELEN 40

typedef struct
{
  COMCTL32_HEADER header;

  UINT      AccelCount;   /* Number of elements in AccelVect */
  UDACCEL*  AccelVect;    /* Vector containing AccelCount elements */
  INT       Base;         /* Base to display nr in the buddy window */
  INT       CurVal;       /* Current up-down value */
  INT       MinVal;       /* Minimum up-down value */
  INT       MaxVal;       /* Maximum up-down value */
  HWND      Buddy;        /* Handle to the buddy window */
  CHAR      szBuddyClass[UPDOWN_BUDDYCLASSNAMELEN]; /* Buddy window class name */
  INT       Flags;        /* Internal Flags FLAG_* */
} UPDOWN_INFO;

typedef struct tagNM_UPDOWN
{
  NMHDR hdr;
  int iPos;
  int iDelta;
} NM_UPDOWN;

#endif  /* __WINE_UPDOWN_H */
