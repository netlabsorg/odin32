/* $Id: relaywin.h,v 1.2 1999-12-02 07:47:26 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 SOCK32 for OS/2
 *
 * Copyright (C) 1999 Patrick Haller <phaller@gmx.net>
 *
 */

/* Remark:
 * - this is an object window that acts as "relay", this is
 *   it receives WSAAsyncSelect()'s messages and redirects
 *   them to the appropriate PostMessageA function of USER32.
 */


#ifndef _RELAYWIN_H_
#define _RELAYWIN_H_


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

/* object.c: the object window procedure on thread 2  */
// os2 includes
#define INCL_DOSPROCESS
#define INCL_WIN
#include <os2.h>

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct tagHwndMsgPair
{
  HWND  hwnd;  /* target window            */
  ULONG ulMsg; /* the message code to send */
} HWNDMSGPAIR, *PHWNDMSGPAIR;

/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

ULONG        RelayAlloc     (HWND  hwnd, ULONG ulMsg);
ULONG        RelayFree      (ULONG ulID);
ULONG        RelayFreeByHwnd(HWND  hwnd);
PHWNDMSGPAIR RelayQuery     (ULONG ulID);
MRESULT EXPENTRY RelayWindowProc(HWND  hwnd, ULONG  ulMsg, MPARAM mp1, MPARAM mp2);
HWND         RelayInitialize(HWND  hwndPost);
BOOL         RelayTerminate (HWND  hwndRelay);


#endif /* _RELAYWIN_H_ */

