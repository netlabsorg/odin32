/* $Id: spy.h,v 1.1 1999-06-25 21:34:49 sandervl Exp $ */

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
VOID PostSpyMessage(HWND hwnd, ULONG Msg, ULONG wParam, ULONG lParam);

#endif //__SPY_H__
