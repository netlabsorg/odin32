/* $Id: relaywin.h,v 1.4 2000-03-22 20:07:24 sandervl Exp $ */

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

/* these are the request types so we can interpret the messages and convert the results */

#define  ASYNCREQUEST_GETHOSTBYNAME     100
#define  ASYNCREQUEST_GETHOSTBYADDR     101
#define  ASYNCREQUEST_GETSERVBYNAME     102
#define  ASYNCREQUEST_GETSERVBYPORT     103
#define  ASYNCREQUEST_GETPROTOBYNAME    104
#define  ASYNCREQUEST_GETPROTOBYNUMBER  105
#define  ASYNCREQUEST_SELECT            106


typedef struct tagHwndMsgPair
{
  HWND  hwnd;  /* target window            */
  ULONG ulMsg; /* the message code to send */
  ULONG ulRequestType; /* the type of request that this belongs to */
  PVOID pvUserData1; /* request specific data field */
  PVOID pvUserData2; /* request specific data field */
  PVOID pvUserData3; /* request specific data field */
} HWNDMSGPAIR, *PHWNDMSGPAIR;


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

ULONG            RelayAlloc     (HWND  hwnd,
                                 ULONG ulMsg, 
                                 ULONG ulRequestType,
                                 BOOL  fSingleRequestPerWindow,
                                 PVOID pvUserData1 = 0, 
                                 PVOID pvUserData2 = 0,
                                 PVOID pvUserData3 = 0);

ULONG            RelayFree      (ULONG ulID);

ULONG            RelayFreeByHwnd(HWND  hwnd);

PHWNDMSGPAIR     RelayQuery     (ULONG ulID);

MRESULT EXPENTRY RelayWindowProc(HWND   hwnd,
                                 ULONG  ulMsg, 
                                 MPARAM mp1, 
                                 MPARAM mp2);

HWND             RelayInitialize(HWND  hwndPost);

BOOL             RelayTerminate (HWND  hwndRelay);


#endif /* _RELAYWIN_H_ */

