/* $Id: timer.h,v 1.5 2000-01-18 20:08:14 sandervl Exp $ */
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

#ifndef INCL_TIMERWIN32
extern BOOL TIMER_GetTimerInfo(HWND PMhwnd,ULONG PMid,PBOOL sys,PULONG id);
extern BOOL TIMER_HandleTimer (PQMSG pMsg);
#endif
extern VOID TIMER_KillTimerFromWindow(HWND hwnd);

#endif //__TIMER_H__
