/* $Id: relaywin.cpp,v 1.7 1999-12-02 16:39:46 achimha Exp $ */

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


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

/* object.c: the object window procedure on thread 2  */
// os2 includes
#define INCL_DOSPROCESS
#define INCL_WIN
#include <os2.h>
// crt includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <odin.h>
#include <odinwrap.h>
#include <misc.h>

#include "relaywin.h"

#include <pmwsock.h>
#include <os2sel.h>
#include <wprocess.h>
#include <heapstring.h>
#include <win32wnd.h>
#include "wsock32.h"

ODINDEBUGCHANNEL(WSOCK32-RELAYWIN)


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

#define MAX_ASYNC_SOCKETS 64

// static table for id / hwnd-msg translation
static HWNDMSGPAIR arrHwndMsgPair[MAX_ASYNC_SOCKETS];
static char*       ODIN_WSOCK_RELAY_CLASS = "ODIN_WSOCK_RELAY";


// prototype for PostMessageA
BOOL __stdcall PostMessageA(HWND,UINT,ULONG,ULONG);


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/11/30 23:00]
 *****************************************************************************/

ULONG RelayAlloc(HWND hwnd, ULONG ulMsg, ULONG ulRequestType,
                 PVOID pvUserData1, PVOID pvUserData2)
{
  ULONG ulCounter;

  for (ulCounter = 0;
       ulCounter < MAX_ASYNC_SOCKETS;
       ulCounter++)
    if ( (arrHwndMsgPair[ulCounter].hwnd == 0) ||    // slot free?
         (arrHwndMsgPair[ulCounter].hwnd == hwnd) )  // same window?
    {
      // occupy slot
      arrHwndMsgPair[ulCounter].hwnd  = hwnd;
      arrHwndMsgPair[ulCounter].ulMsg = ulMsg;
      arrHwndMsgPair[ulCounter].ulRequestType = ulRequestType;
      arrHwndMsgPair[ulCounter].pvUserData1 = pvUserData1;
      arrHwndMsgPair[ulCounter].pvUserData2 = pvUserData2;
      return ulCounter; // return "id"
    }

  return -1; // not found
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/11/30 23:00]
 *****************************************************************************/

ULONG RelayFree(ULONG ulID)
{
  if ( (ulID < 0) ||  // check range
       (ulID >= MAX_ASYNC_SOCKETS) )
    return -1; // error

  arrHwndMsgPair[ulID].hwnd = 0; // mark free

  return 0; // OK
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/11/30 23:00]
 *****************************************************************************/

ULONG RelayFreeByHwnd(HWND hwnd)
{
  ULONG ulCounter;

  for (ulCounter = 0;
       ulCounter < MAX_ASYNC_SOCKETS;
       ulCounter++)
    if ( arrHwndMsgPair[ulCounter].hwnd == hwnd )  // same window?
    {
      arrHwndMsgPair[ulCounter].hwnd  = 0; // free slot
      return 0; // OK
    }

  return -1; // not found
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/11/30 23:00]
 *****************************************************************************/

PHWNDMSGPAIR RelayQuery(ULONG ulID)
{
  if ( (ulID < 0) ||  // check range
       (ulID >= MAX_ASYNC_SOCKETS) )
    return NULL; // error
  
  if (arrHwndMsgPair[ulID].hwnd == 0)
    return NULL; // error, free entry
  else
    return (&arrHwndMsgPair[ulID]);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/11/30 23:00]
 *****************************************************************************/

MRESULT EXPENTRY RelayWindowProc(HWND   hwnd,
                                 ULONG  ulMsg,
                                 MPARAM mp1,
                                 MPARAM mp2)
{
  PHWNDMSGPAIR pHM;

  // termination flag handling?
  // if (fTerminate)
  //   WinDefWindowProc()
  
  pHM = RelayQuery(ulMsg);                          // find registered message
  if (pHM != NULL)                                  // message pair found
  {
    /* check request type for special handling */
    switch (pHM->ulRequestType)
    {
      case ASYNCREQUEST_SELECT:
      {
        dprintf(("WSOCK32:RelayWindowProc, AsyncSelect notification\n"));
        break;
      }
      case ASYNCREQUEST_GETHOSTBYNAME:
      {
        dprintf(("WSOCK32:RelayWindowProc, Converting hostent for "
                 "WSAAyncGetHostByName\n"));
        /* we need to convert the hostent structure here */
        Whostent *WinHostent = (Whostent*)pHM->pvUserData1;
        hostent *OS2Hostent = (hostent*)pHM->pvUserData1;
        short h_addrtype = (short)OS2Hostent->h_addrtype;
        WinHostent->h_addrtype = h_addrtype;
        short h_length = (short)OS2Hostent->h_length;
        WinHostent->h_length = h_length;
        char **h_addr_list = OS2Hostent->h_addr_list;
        WinHostent->h_addr_list = h_addr_list;
  //TODO: the size of OS/2 hostent is 4 bytes bigger so the original buffer *might* be too small

        break;
      }
    }

    dprintf(("WSOCK32:RelayWinProc, Posting %d to %d\n", pHM->ulMsg, pHM->hwnd));
    PostMessageA(pHM->hwnd,
                 pHM->ulMsg,
                 (ULONG)mp1,
                 (ULONG)mp2);

    // if socket close, free entry
    //@@@PH

    return FALSE;                                   // OK, message sent
  }

  // default message processing
  return WinDefWindowProc( hwnd, ulMsg, mp1, mp2 );
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/11/30 23:00]
 *****************************************************************************/

HWND RelayInitialize(HWND hwndPost)
{
  BOOL       fSuccess;
  HAB        hab;
  HWND       hwnd;


  // thread initialization
  hab = WinQueryAnchorBlock(hwndPost);
  if (hab == NULLHANDLE)
    return NULLHANDLE;

  // register relay window class
  fSuccess = WinRegisterClass(hab,
                              ODIN_WSOCK_RELAY_CLASS,
                              (PFNWP)RelayWindowProc,
                              0,
                              0);
  if (fSuccess == FALSE)
    return NULLHANDLE;

  hwnd = WinCreateWindow(HWND_OBJECT,
                         ODIN_WSOCK_RELAY_CLASS,
                         "ODIN WSock Relay",
                         0, 0, 0, 0, 0,
                         HWND_OBJECT,
                         HWND_BOTTOM,
                         0,
                         NULL,
                         NULL );

  //WinDestroyWindow( pg->hwndObject );
  return hwnd;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/11/30 23:00]
 *****************************************************************************/

BOOL RelayTerminate(HWND hwndRelay)
{
  return WinDestroyWindow(hwndRelay);
}

