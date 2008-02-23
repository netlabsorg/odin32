/* $Id: spy.h,v 1.5 1999-06-27 16:23:36 sandervl Exp $ */

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
#define Q_SPYMSG_KILLSERVER     3

typedef struct _Q_SPYMSG {
	ULONG hwnd;
	ULONG Msg;
	ULONG wParam;
	ULONG lParam;
} Q_SPYMSG;

BOOL InitSpyQueue();
void CloseSpyQueue();

#ifdef DEBUG
BOOL PostSpyMessage(HWND hwnd, ULONG Msg, ULONG wParam, ULONG lParam);
#else
#define PostSpyMessage(a,b,c,d)	FALSE
#endif

#endif //__SPY_H__
