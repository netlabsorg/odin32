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
#include <win32api.h>
#include "wsock32.h"

ODINDEBUGCHANNEL(WSOCK32-RELAYWIN)


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

#define MAX_ASYNC_SOCKETS 64

// static table for id / hwnd-msg translation
static HWNDMSGPAIR arrHwndMsgPair[MAX_ASYNC_SOCKETS] = {0};
static char*       ODIN_WSOCK_RELAY_CLASS = "ODIN_WSOCK_RELAY";
static HWND        hwndRelay              = NULLHANDLE;


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

ULONG RelayAlloc(HWND  hwnd,
                 ULONG ulMsg,
                 ULONG ulRequestType,
                 BOOL  fSingleRequestPerWindow,
                 PVOID pvUserData1,
                 PVOID pvUserData2,
                 PVOID pvUserData3)
{
  ULONG ulCounter;

  for (ulCounter = 0;
       ulCounter < MAX_ASYNC_SOCKETS;
       ulCounter++)
    if ( (arrHwndMsgPair[ulCounter].hwnd == 0) ||    // slot free?
         ( (fSingleRequestPerWindow == TRUE) &&      // more than one request
                                                     // per window ?
           (arrHwndMsgPair[ulCounter].hwnd == hwnd) ) ) // same window?
    {
      // occupy slot
      arrHwndMsgPair[ulCounter].hwnd          = hwnd;
      arrHwndMsgPair[ulCounter].ulMsg         = ulMsg;
      arrHwndMsgPair[ulCounter].ulRequestType = ulRequestType;
      arrHwndMsgPair[ulCounter].pvUserData1   = pvUserData1;
      arrHwndMsgPair[ulCounter].pvUserData2   = pvUserData2;
      arrHwndMsgPair[ulCounter].pvUserData3   = pvUserData3;
      return ulCounter + 1; // return "id"
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
  if ( (ulID < 1) ||  // check range
       (ulID > MAX_ASYNC_SOCKETS) )
    return -1; // error

  arrHwndMsgPair[ulID-1].hwnd = 0; // mark free
  arrHwndMsgPair[ulID-1].ulMsg = 0;
  arrHwndMsgPair[ulID-1].ulRequestType = 0;
  arrHwndMsgPair[ulID-1].pvUserData1 = 0;
  arrHwndMsgPair[ulID-1].pvUserData2 = 0;
  arrHwndMsgPair[ulID-1].pvUserData3 = 0;

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
  if ( (ulID < 1) ||  // check range
       (ulID > MAX_ASYNC_SOCKETS) )
    return NULL; // error

  if (arrHwndMsgPair[ulID-1].hwnd == 0)
    return NULL; // error, free entry
  else
    return (&arrHwndMsgPair[ulID-1]);
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
  int          rc;

  // termination flag handling?
  // if (fTerminate)
  //   WinDefWindowProc()

  /* find registered message */
  pHM = RelayQuery(ulMsg);
  /* message pair found */
  if (pHM != NULL)
  {
    rc = SHORT1FROMMP(mp2);                /* asynchronous operation result */

    dprintf(("WSOCK32: RelayWindowProc, message %x for window %x with "
             "mp1 = %d and mp2 = %d (rc = %d) received\n",
             ulMsg, hwnd, mp1, mp2, rc));

    /* check request type for special handling */
    switch (pHM->ulRequestType)
    {
      /**********
       * SELECT *
       **********/
      case ASYNCREQUEST_SELECT:
      {
        dprintf(("WSOCK32:RelayWindowProc, AsyncSelect notification\n"));
        break;
      }


      /*****************
       * GETHOSTBYNAME *
       *****************/
      case ASYNCREQUEST_GETHOSTBYNAME:
      {
        dprintf(("WSOCK32:RelayWindowProc, Converting hostent for "
                 "WSAAyncGetHostByName\n"));

        /* is there a valid result ? */
        if (rc == 0)
        {
          /* we need to convert the hostent structure here */
          Whostent *WinHostent             = (Whostent*)pHM->pvUserData1;
          hostent  *OS2Hostent             = (hostent*)pHM->pvUserData1;

          short    h_addrtype              = (short)OS2Hostent->h_addrtype;
                   WinHostent->h_addrtype  = h_addrtype;
          short    h_length                = (short)OS2Hostent->h_length;
                   WinHostent->h_length    = h_length;
          char     **h_addr_list           = OS2Hostent->h_addr_list;
                   WinHostent->h_addr_list = h_addr_list;
          //TODO: the size of OS/2 hostent is 4 bytes bigger
          //      so the original buffer *might* be too small
        }
        break;
      }


      /*****************
       * GETHOSTBYADDR *
       *****************/
      case ASYNCREQUEST_GETHOSTBYADDR:
      {
        dprintf(("WSOCK32:RelayWindowProc, Converting hostent for "
                 "WSAAyncGetHostByAddr\n"));

        if (rc == 0)
        {
          dprintf(("WSOCK32:RelayWindowProc, hostent buffer: %d\n", pHM->pvUserData1));
          /* we need to convert the hostent structure here */
          Whostent *WinHostent             = (Whostent*)pHM->pvUserData1;
          hostent  *OS2Hostent             = (hostent*)pHM->pvUserData1;

          short    h_addrtype              = (short)OS2Hostent->h_addrtype;
                   WinHostent->h_addrtype  = h_addrtype;
          short    h_length                = (short)OS2Hostent->h_length;
                   WinHostent->h_length    = h_length;
          char     **h_addr_list           = OS2Hostent->h_addr_list;
                   WinHostent->h_addr_list = h_addr_list;
          //TODO: the size of OS/2 hostent is 4 bytes bigger
          //      so the original buffer *might* be too small
        }
        break;
      }


      /*****************
       * GETSERVBYNAME *
       *****************/
      case ASYNCREQUEST_GETSERVBYNAME:
      {
        dprintf(("WSOCK32:RelayWindowProc, Converting servent for "
                 "WSAAyncGetServByName\n"));

        if (rc == 0)
        {
          /* we need to convert the servent structure here */
          Wservent *WinServent             = (Wservent*)pHM->pvUserData1;
          servent  *OS2Servent             = (servent*)pHM->pvUserData1;

          WinServent->s_port  = OS2Servent->s_port;
          WinServent->s_proto = OS2Servent->s_proto;
          //TODO: the size of OS/2 servent is 2 bytes bigger
          //      so the original buffer *might* be too small
        }
        break;
      }


      /*****************
       * GETSERVBYPORT *
       *****************/
      case ASYNCREQUEST_GETSERVBYPORT:
      {
        dprintf(("WSOCK32:RelayWindowProc, Converting servent for "
                 "WSAAyncGetServByPort\n"));

        if (rc == 0)
        {
          /* we need to convert the servent structure here */
          Wservent *WinServent             = (Wservent*)pHM->pvUserData1;
          servent  *OS2Servent             = (servent*)pHM->pvUserData1;

          WinServent->s_port  = OS2Servent->s_port;
          WinServent->s_proto = OS2Servent->s_proto;
          //TODO: the size of OS/2 servent is 2 bytes bigger
          //      so the original buffer *might* be too small
        }
        break;
      }


      /******************
       * GETPROTOBYNAME *
       ******************/
      case ASYNCREQUEST_GETPROTOBYNAME:
      {
        dprintf(("WSOCK32:RelayWindowProc, Converting protoent for "
                 "WSAAyncGetProtoByName\n"));

        if (rc == 0)
        {
          /* we need to convert the protoent structure here */
          Wprotoent *WinProtoent             = (Wprotoent*)pHM->pvUserData1;
          protoent  *OS2Protoent             = (protoent*)pHM->pvUserData1;

          WinProtoent->p_proto = OS2Protoent->p_proto;

          //TODO: the size of OS/2 hostent is 2 bytes bigger
          //      so the original buffer *might* be too small
        }
        break;
      }


      /********************
       * GETPROTOBYNUMBER *
       ********************/
      case ASYNCREQUEST_GETPROTOBYNUMBER:
      {
        dprintf(("WSOCK32:RelayWindowProc, Converting protoent for "
                 "WSAAyncGetProtoByNumber\n"));

        if (rc == 0)
        {
          /* we need to convert the protoent structure here */
          Wprotoent *WinProtoent             = (Wprotoent*)pHM->pvUserData1;
          protoent  *OS2Protoent             = (protoent*)pHM->pvUserData1;

          WinProtoent->p_proto = OS2Protoent->p_proto;

          //TODO: the size of OS/2 hostent is 2 bytes bigger
          //      so the original buffer *might* be too small
        }
        break;
      }

      default:
      {
        dprintf(("WSOCK32:RelayWindowProc, Unknown request type!!!"
                 "window: %d, msg: %d, mp1 %d, mp2%d\n", pHM->hwnd,
                 pHM->ulMsg, mp1, mp2));
        break;
      }
    }


    dprintf(("WSOCK32:RelayWinProc, Posting hwnd=%08xh, msg=%08xh, w=%08xh, l=%08xh\n",
             pHM->hwnd,
             pHM->ulMsg,
             mp1,
             mp2));

    PostMessageA(pHM->hwnd,
                 pHM->ulMsg,
                 (ULONG)mp1,
                 (ULONG)mp2);

    // if socket close or non-select call, free entry
    // @@@PH
    if (pHM->ulRequestType != ASYNCREQUEST_SELECT)
    {
      dprintf(("WSOCK32:RelayWindowProc, Free handle %d\n", pHM->ulMsg));
      RelayFree(pHM->ulMsg);
    }

    return FALSE;                                   // OK, message sent
  } 
  else
  {
    dprintf(("WSOCK32:AsyncRelayWindowProc: Handle not found, message ignored %x %x %x %x", hwnd, ulMsg, mp1, mp2));
  }

  // default message processing
  return WinDefWindowProc( hwnd, ulMsg, mp1, mp2 );
}




#if 0
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

#else

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


//----------------------------------------------------------------------
// thread 2 entry point: gets and dispatches object window messages
// _Optlink is an IBM C Set/2 function modifier
void _Optlink RelayThreadMain(PVOID pParameter)
{
  BOOL       fSuccess;
  HAB        hab;
  HMQ        hmq;
  QMSG       qmsg;

  // thread initialization
  hab = WinInitialize( 0 );
  hmq = WinCreateMsgQueue( hab, 0 );

  // prevent system from posting object window a WM_QUIT
  // I'll post WM_QUIT when it's time.
  fSuccess = WinCancelShutdown( hmq, TRUE );
  if (fSuccess != TRUE)
  {
    dprintf(("WSOCK32:RelayWin: ERROR WinCancelShutdown failed\n"));
    return;
  }

  // register relay window class
  fSuccess = WinRegisterClass(hab,
                              ODIN_WSOCK_RELAY_CLASS,
                              (PFNWP)RelayWindowProc,
                              0,
                              0);
  if (fSuccess == FALSE)
  {
    dprintf(("WSOCK32:RelayWin: ERROR WinRegisterClass failed\n"));
    return;
  }

  hwndRelay = WinCreateWindow(HWND_OBJECT,
                         ODIN_WSOCK_RELAY_CLASS,
                         "ODIN WSock Relay",
                         0, 0, 0, 0, 0,
                         HWND_OBJECT,
                         HWND_BOTTOM,
                         0,
                         NULL,
                         NULL );
  if (hwndRelay == NULLHANDLE)
  {
    dprintf(("WSOCK32:RelayWin: ERROR WinCreateWindow failed\n"));
    return;
  }

  // get/dispatch messages; user messages, for the most part
  while( WinGetMsg ( hab, &qmsg, 0, 0, 0 ))
  {
    WinDispatchMsg ( hab, &qmsg );
  }

  // clean up
  WinDestroyWindow( hwndRelay );
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );

  // reset relay window handle
  hwndRelay = NULLHANDLE;
  return;
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
  int tidRelay;  // thread identifier

  if (hwndRelay != NULLHANDLE)
  {
    // relay thread has been initialized
    return hwndRelay;
  }

  // else create new subsystem
    // create thread
#if defined(__IBMCPP__)
    tidRelay  = _beginthread(RelayThreadMain,
                             NULL,
                             16384,
                             (PVOID)0);
#else
    tidRelay  = _beginthread(RelayThreadMain,
                             16384,
                             (PVOID)0);
#endif

  // wait for thread to come up and send valid HWND
  // @@@PH this is an ugly hack
  dprintf(("WSOCK32:RELAYWIN:RelayInitialize wait for window handle\n"));
  while (hwndRelay == NULL)
  {
    DosSleep(10);
  }
  dprintf(("WSOCK32:RELAYWIN:RelayInitialize window handle = %08xh",
            hwndRelay));

  return hwndRelay;
}

#endif


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

