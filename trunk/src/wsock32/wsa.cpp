/* $Id: wsa.cpp,v 1.7 2001-10-13 18:51:08 sandervl Exp $ */

/*
 *
 * Win32 SOCK32 for OS/2 (WSA apis)
 *
 * Copyright (C) 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Based on Wine code: (dlls\winsock\async.c,socket.c)
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 * (C) 1999 Marcus Meissner
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#include <os2wrap.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <misc.h>
#include <win32api.h>

#include "wsock32.h"
#include "asyncthread.h"

#define DBG_LOCALLOG	DBG_wsa
#include "dbglocal.h"

ODINDEBUGCHANNEL(WSOCK32-WSA)

//kso: dirty fix to make this compile! not permanent!
BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER *p);
#define LowPart u.LowPart


#ifndef winsockcleanupsockets
// Exported by SO32DLL.  Used to close all open sockets.
void _System winsockcleanupsockets(void);
#endif

static void WSASetBlocking(BOOL fBlock);

BOOL fWSAInitialized = FALSE;
static int num_startup = 0;

//******************************************************************************
WSADATA WINSOCK_data = { 0x0101, 0x0101,
                        "Odin Sockets 1.1", "OS/2",
		         WS_MAX_SOCKETS_PER_PROCESS,
		         WS_MAX_UDP_DATAGRAM, NULL };
//******************************************************************************
ODINFUNCTION2(int, WSAStartup,
              USHORT,wVersionRequested,
              LPWSADATA,lpWSAData)
{
    LPWSINFO            pwsi;

    if (LOBYTE(wVersionRequested) < 1 || (LOBYTE(wVersionRequested) == 1 &&
        HIBYTE(wVersionRequested) < 1)) return WSAVERNOTSUPPORTED;

    if (!lpWSAData) return WSAEINVAL;

    if(fWSAInitialized == FALSE)
    {
        WINSOCK_CreateIData();
        pwsi = WINSOCK_GetIData();

    	sock_init();
    }
    else pwsi = WINSOCK_GetIData();

    if (!pwsi) return WSASYSNOTREADY;

    num_startup++;
    fWSAInitialized = TRUE;

    /* return winsock information */
    memcpy(lpWSAData, &WINSOCK_data, sizeof(WINSOCK_data));

    return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(int,WSACleanup)
{
    LPWSINFO pwsi = WINSOCK_GetIData();
    if( pwsi ) {
	if( --num_startup > 0 ) return 0;

        WSACancelAllAsyncRequests();
        winsockcleanupsockets();

	fWSAInitialized = FALSE;
	WINSOCK_DeleteIData();
	return 0;
    }
    SetLastError(WSANOTINITIALISED);
    return SOCKET_ERROR;
}
//******************************************************************************
//TODO: Implement this
//******************************************************************************
ODINFUNCTION1(FARPROC,WSASetBlockingHook,
              FARPROC,lpBlockFunc)
{
  FARPROC             prev;
  LPWSINFO            pwsi = WINSOCK_GetIData();

  if( pwsi ) {
      prev = (FARPROC)pwsi->blocking_hook;
      pwsi->blocking_hook = (DWORD)lpBlockFunc;
      pwsi->flags |= WSI_BLOCKINGHOOK;
      return prev;
  }
  return NULL;
}
//******************************************************************************
//TODO: Implement this
//******************************************************************************
ODINFUNCTION0(int,WSAUnhookBlockingHook)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	pwsi->blocking_hook = 0;
	pwsi->flags &= ~WSI_BLOCKINGHOOK;
	return 0;
    }
    return SOCKET_ERROR;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WSAIsBlocking()
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
      	return (pwsi->flags & WSI_BLOCKINGCALL) == WSI_BLOCKINGCALL;
    }
    DebugInt3();
    return FALSE; //shouldn't happen
}
//******************************************************************************
//******************************************************************************
void WSASetBlocking(BOOL fBlock, HANDLE tid)
{
    LPWSINFO pwsi = WINSOCK_GetIData(tid);

    if( pwsi )
    {
	if(fBlock) {
      		pwsi->flags |= WSI_BLOCKINGCALL;
	}
	else	pwsi->flags &= ~WSI_BLOCKINGCALL;
	return;
    }
    DebugInt3();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(int,__WSAFDIsSet,SOCKET, s, ws_fd_set*,set)
{
  int i = set->fd_count;

  if(set) {
  	while (i--) {
      		if (set->fd_array[i] == s) return 1;
	}
  }

  return 0;
}
//******************************************************************************
//******************************************************************************
UINT wsaErrno()
{
 int loc_errno = sock_errno();

    //Warp 4 returns errors with base 0
    if(loc_errno && loc_errno < SOCBASEERR) {
	loc_errno += SOCBASEERR;
    }
    switch(loc_errno)
    {
	case SOCEINTR:		return WSAEINTR;
	case SOCEBADF:		return WSAEBADF;
	case SOCEPERM:
	case SOCEACCES:		return WSAEACCES;
	case SOCEFAULT:		return WSAEFAULT;
	case SOCEINVAL:		return WSAEINVAL;
	case SOCEMFILE:		return WSAEMFILE;
	case SOCEWOULDBLOCK:	return WSAEWOULDBLOCK;
	case SOCEINPROGRESS:	return WSAEINPROGRESS;
	case SOCEALREADY:	return WSAEALREADY;
	case SOCENOTSOCK:	return WSAENOTSOCK;
	case SOCEDESTADDRREQ:	return WSAEDESTADDRREQ;
	case SOCEMSGSIZE:	return WSAEMSGSIZE;
	case SOCEPROTOTYPE:	return WSAEPROTOTYPE;
	case SOCENOPROTOOPT:	return WSAENOPROTOOPT;
	case SOCEPROTONOSUPPORT:return WSAEPROTONOSUPPORT;
	case SOCESOCKTNOSUPPORT:return WSAESOCKTNOSUPPORT;
	case SOCEOPNOTSUPP:	return WSAEOPNOTSUPP;
	case SOCEPFNOSUPPORT:	return WSAEPFNOSUPPORT;
	case SOCEAFNOSUPPORT:	return WSAEAFNOSUPPORT;
	case SOCEADDRINUSE:	return WSAEADDRINUSE;
	case SOCEADDRNOTAVAIL:	return WSAEADDRNOTAVAIL;
	case SOCENETDOWN:	return WSAENETDOWN;
	case SOCENETUNREACH:	return WSAENETUNREACH;
	case SOCENETRESET:	return WSAENETRESET;
	case SOCECONNABORTED:	return WSAECONNABORTED;
	case SOCEPIPE:
	case SOCECONNRESET:	return WSAECONNRESET;
	case SOCENOBUFS:	return WSAENOBUFS;
	case SOCEISCONN:	return WSAEISCONN;
	case SOCENOTCONN:	return WSAENOTCONN;
	case SOCESHUTDOWN:	return WSAESHUTDOWN;
	case SOCETOOMANYREFS:	return WSAETOOMANYREFS;
	case SOCETIMEDOUT:	return WSAETIMEDOUT;
	case SOCECONNREFUSED:	return WSAECONNREFUSED;
	case SOCELOOP:		return WSAELOOP;
	case SOCENAMETOOLONG:	return WSAENAMETOOLONG;
	case SOCEHOSTDOWN:	return WSAEHOSTDOWN;
	case SOCEHOSTUNREACH:	return WSAEHOSTUNREACH;
	case SOCENOTEMPTY:	return WSAENOTEMPTY;
	case SOCENXIO:
	case SOCESRCH:
	case SOCEOS2ERR:        return WSAEFAULT;

        /* just in case we ever get here and there are no problems */
	case 0:			return 0;
        default:
		dprintf(("Unknown errno %d!\n", loc_errno));
		return WSAEOPNOTSUPP;
    }
}
//******************************************************************************
//******************************************************************************
UINT wsaHerrno()
{
    int loc_errno = h_errno;

    switch(loc_errno)
    {
	case HOST_NOT_FOUND:	return WSAHOST_NOT_FOUND;
	case TRY_AGAIN:		return WSATRY_AGAIN;
	case NO_RECOVERY:	return WSANO_RECOVERY;
	case NO_DATA:		return WSANO_DATA;

	case 0:			return 0;
        default:
		dprintf(("Unknown h_errno %d!\n", loc_errno));
		return WSAEOPNOTSUPP;
    }
}
//******************************************************************************
//******************************************************************************
