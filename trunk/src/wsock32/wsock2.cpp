/* $Id: wsock2.cpp,v 1.2 2001-07-07 14:29:40 achimha Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Winsock 2.0 functions for OS/2
 *
 * Copyright (C) 2001 Achim Hasenmueller <achimha@innotek.de>
 *
 * Some parts based on Wine code: (dlls\winsock\socket.c)
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 *
 */

#include <odin.h>
#include <debugtools.h>
#include "wsock32.h"


/***********************************************************************
 *		WSAEventSelect (WS2_32.39)
 */
int WINAPI WSAEventSelect(SOCKET s, WSAEVENT hEvent, LONG lEvent)
{
    TRACE("WS2_32: WSAEventSelect socket 0x%x, hEvent 0x%x, event 0x%x", s, hEvent, (unsigned)lEvent);

    // forward call to worker function in HEVENT notification mode
    return WSAAsyncSelectWorker(s, WSA_SELECT_HEVENT, (int)hEvent, 0, lEvent);
}

/***********************************************************************
 *		WSAEnumNetworkEvents
 */
int WINAPI WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEvent, LPWSANETWORKEVENTS lpEvent)
{
// called too often in some apps, makes log file grow too fast
//   dprintf(("WSAEnumNetworkEvents 0x%x 0x%x 0x%x NOT CORRECTLY IMPLEMENTED", s, hEvent, lpEvent));

   if(!fWSAInitialized)
   {
        dprintf(("WSA sockets not initialized"));
      	WSASetLastError(WSANOTINITIALISED);
	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking())
   {
        dprintf(("blocking call in progress"));
        WSASetLastError(WSAEINPROGRESS);      	// blocking call in progress
	return SOCKET_ERROR;
   }
   else
   if (!lpEvent)
   {
       dprintf(("network event buffer NULL"));
       WSASetLastError(WSAEINVAL);
       return SOCKET_ERROR;
   }
   // TODO: check if lpEvent is in user address space! (Win32 does)

   // forward call to worker routine
   return WSAEnumNetworkEventsWorker(s, hEvent, lpEvent);
}
