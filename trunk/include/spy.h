/* $Id: spy.h,v 1.3 1999-06-26 10:55:49 achimha Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __SPY_H__
#define __SPY_H__

#define TIMEOUT_LEN  (ULONG)1000
#define Q_NAME       "ODIN32SPY"
#define Q_SPYMSG_WNDMSG 	1
#define Q_SPYMSG_NEWWND 	2

typedef struct _Q_SPYMSG {
	ULONG hwnd;
	ULONG Msg;
	ULONG wParam;
	ULONG lParam;
} Q_SPYMSG;

BOOL InitSpyQueue();
void CloseSpyQueue();

#ifdef DEBUG
VOID PostSpyMessage(HWND hwnd, ULONG Msg, ULONG wParam, ULONG lParam);
#else
#define PostSpyMessage(a,b,c,d)	FALSE
#endif

#endif //__SPY_H__
