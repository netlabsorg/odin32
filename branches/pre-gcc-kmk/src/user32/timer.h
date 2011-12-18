/* $Id: timer.h,v 1.6 2003-05-16 10:59:28 sandervl Exp $ */
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
extern BOOL TIMER_GetTimerInfo(HWND PMhwnd,ULONG PMid,PBOOL sys,PULONG id, PULONG proc);
extern BOOL TIMER_HandleTimer (PQMSG pMsg);
#endif
extern BOOL TIMER_IsTimerValid( HWND hwnd, UINT id, ULONG proc );
extern VOID TIMER_KillTimerFromWindow(HWND hwnd);

#endif //__TIMER_H__

