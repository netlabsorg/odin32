/* $Id: timer.h,v 1.2 1999-10-23 16:45:22 cbratschi Exp $ */
/*
 * public timer functions
 *
 *
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __TIMER_H__
#define __TIMER_H__

extern BOOL TIMER_GetTimerInfo(HWND PMhwnd,ULONG PMid,PBOOL sys,PULONG id);
extern BOOL TIMER_HandleTimer (PQMSG pMsg);

#endif //__TIMER_H__

