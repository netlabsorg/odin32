/* $Id: SYSCLR.H,v 1.3 1999-09-15 23:20:37 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 system color API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __SYSCLR_H__
#define __SYSCLR_H__

#define COLOR_SCROLLBAR              0
#define COLOR_BACKGROUND             1
#define COLOR_ACTIVECAPTION          2
#define COLOR_INACTIVECAPTION        3
#define COLOR_MENU                   4
#define COLOR_WINDOW                 5
#define COLOR_WINDOWFRAME            6
#define COLOR_MENUTEXT               7
#define COLOR_WINDOWTEXT             8
#define COLOR_CAPTIONTEXT            9
#define COLOR_ACTIVEBORDER           10
#define COLOR_INACTIVEBORDER         11
#define COLOR_APPWORKSPACE           12
#define COLOR_HIGHLIGHT              13
#define COLOR_HIGHLIGHTTEXT          14
#define COLOR_BTNFACE                15
#define COLOR_BTNSHADOW              16
#define COLOR_GRAYTEXT               17
#define COLOR_BTNTEXT                18
#define COLOR_INACTIVECAPTIONTEXT    19
#define COLOR_BTNHIGHLIGHT           20
//GetSysColor win95/NT4 extra options
#define COLOR_3DDKSHADOW        21
#define COLOR_3DLIGHT           22
#define COLOR_INFOTEXT          23
#define COLOR_INFOBK            24

#define COLOR_DESKTOP           COLOR_BACKGROUND
#define COLOR_3DFACE            COLOR_BTNFACE
#define COLOR_3DSHADOW          COLOR_BTNSHADOW
#define COLOR_3DHIGHLIGHT       COLOR_BTNHIGHLIGHT
#define COLOR_3DHILIGHT         COLOR_BTNHIGHLIGHT
#define COLOR_BTNHILIGHT        COLOR_BTNHIGHLIGHT

#endif
