/* $Id: winswitch.h,v 1.1 1999-12-30 17:35:38 cbratschi Exp $ */

/*
 * WinSwitch class extra info
 *
 * Copyright 1999 Christoph Bratschi
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __WIN_SWITCH_H
#define __WIN_SWITCH_H

#define WINSWITCHCLASSNAME "#32771"

typedef struct
{
} WINSWITCH_INFO;

BOOL WINSWITCH_Register();
BOOL WINSWITCH_Unregister();

#endif

