/* $Id: wsock32.h,v 1.16 2001-02-28 20:28:09 sandervl Exp $ */

/* WSOCK32.H--definitions & conversions for Odin's wsock32.dll.
 * Unused/unneeded Microsoft declarations removed.
 *
 * This header file corresponds to version 1.1 of the Windows Sockets specification.
 *
 * This file includes parts which are Copyright (c) 1982-1986 Regents
 * of the University of California.  All rights reserved.  The
 * Berkeley Software License Agreement specifies the terms and
 * conditions for redistribution.
 *
 */

#ifndef _WINSOCK32CONST_
#define _WINSOCK32CONST_

#include <sys\socket.h>
#include <sys\ioctl.h>
#include <netdb.h>
#include <netinet\in.h>
#include <netinet\tcp.h>
#include <nerrno.h>
#include <sys\time.h>

#include <odinwrap.h>

/*
 * Socket state bits.
 */
#define SS_NOFDREF              0x0001  /* no file table ref any more */
#define SS_ISCONNECTED          0x0002  /* socket connected to a peer */
#define SS_ISCONNECTING         0x0004  /* in process of connecting to peer */
#define SS_ISDISCONNECTING      0x0008  /* in process of disconnecting */
#define SS_CANTSENDMORE         0x0010  /* can't send more data to peer */
#define SS_CANTRCVMORE          0x0020  /* can't receive more data from peer */
#define SS_RCVATMARK            0x0040  /* at mark on input */

#define SS_NBIO                 0x0100  /* non-blocking ops */
#define SS_ASYNC                0x0200  /* async i/o notify */
#define SS_ISCONFIRMING         0x0400  /* deciding to accept connection req */

#define SS_INCOMP               0x0800  /* unaccepted, incomplete connection */
#define SS_COMP                 0x1000  /* unaccepted, complete connection */
#define SS_ISDISCONNECTED       0x2000  /* socket disconnected from peer */

//socketopt options
#define SO_DONTLINGER   (u_int)(~SO_LINGER)

#define WSAMAKESELECTREPLY(event,error)     MAKELONG(event,error)

// WSAAsyncSelect flags
#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_OOB          0x04
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20

#define SOCKET_ERROR 		-1
#define NO_ERROR		0

#define WSABASEERR              10000

#define WSAEINTR                (WSABASEERR+4)
#define WSAEBADF                (WSABASEERR+9)
#define WSAEACCES               (WSABASEERR+13)
#define WSAEFAULT               (WSABASEERR+14)
#define WSAEINVAL               (WSABASEERR+22)
#define WSAEMFILE               (WSABASEERR+24)

#define WSAEWOULDBLOCK          (WSABASEERR+35)
#define WSAEINPROGRESS          (WSABASEERR+36)
#define WSAEALREADY             (WSABASEERR+37)
#define WSAENOTSOCK             (WSABASEERR+38)
#define WSAEDESTADDRREQ         (WSABASEERR+39)
#define WSAEMSGSIZE             (WSABASEERR+40)
#define WSAEPROTOTYPE           (WSABASEERR+41)
#define WSAENOPROTOOPT          (WSABASEERR+42)
#define WSAEPROTONOSUPPORT      (WSABASEERR+43)
#define WSAESOCKTNOSUPPORT      (WSABASEERR+44)
#define WSAEOPNOTSUPP           (WSABASEERR+45)
#define WSAEPFNOSUPPORT         (WSABASEERR+46)
#define WSAEAFNOSUPPORT         (WSABASEERR+47)
#define WSAEADDRINUSE           (WSABASEERR+48)
#define WSAEADDRNOTAVAIL        (WSABASEERR+49)
#define WSAENETDOWN             (WSABASEERR+50)
#define WSAENETUNREACH          (WSABASEERR+51)
#define WSAENETRESET            (WSABASEERR+52)
#define WSAECONNABORTED         (WSABASEERR+53)
#define WSAECONNRESET           (WSABASEERR+54)
#define WSAENOBUFS              (WSABASEERR+55)
#define WSAEISCONN              (WSABASEERR+56)
#define WSAENOTCONN             (WSABASEERR+57)
#define WSAESHUTDOWN            (WSABASEERR+58)
#define WSAETOOMANYREFS         (WSABASEERR+59)
#define WSAETIMEDOUT            (WSABASEERR+60)
#define WSAECONNREFUSED         (WSABASEERR+61)
#define WSAELOOP                (WSABASEERR+62)
#define WSAENAMETOOLONG         (WSABASEERR+63)
#define WSAEHOSTDOWN            (WSABASEERR+64)
#define WSAEHOSTUNREACH         (WSABASEERR+65)
#define WSAENOTEMPTY            (WSABASEERR+66)
#define WSAEPROCLIM             (WSABASEERR+67)
#define WSAEUSERS               (WSABASEERR+68)
#define WSAEDQUOT               (WSABASEERR+69)
#define WSAESTALE               (WSABASEERR+70)
#define WSAEREMOTE              (WSABASEERR+71)
#define WSASYSNOTREADY          (WSABASEERR+91)
#define WSAVERNOTSUPPORTED      (WSABASEERR+92)
#define WSANOTINITIALISED       (WSABASEERR+93)
#define WSAHOST_NOT_FOUND       (WSABASEERR+1001)
#define WSATRY_AGAIN            (WSABASEERR+1002)
#define WSANO_RECOVERY          (WSABASEERR+1003)
#define WSANO_DATA              (WSABASEERR+1004)
#define WSANO_ADDRESS           WSANO_DATA

typedef u_int           SOCKET;

#define FD_SETSIZE      64

#pragma pack(1)

#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128

typedef struct WSAData {
        USHORT              wVersion;
        USHORT              wHighVersion;
        char                szDescription[WSADESCRIPTION_LEN+1];
        char                szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short      iMaxSockets;
        unsigned short      iMaxUdpDg;
        char *              lpVendorInfo;
} WSADATA;
typedef WSADATA *PWSADATA;
typedef WSADATA *LPWSADATA;

typedef struct ws_fd_set {
        u_int   fd_count;
        SOCKET  fd_array[FD_SETSIZE];
} _ws_fd_set;

/*
 * Structure used for manipulating linger option.
 */
typedef struct  ws_linger {
        u_short l_onoff;                /* option on/off */
        u_short l_linger;               /* linger time */
} _ws_linger;

/*
 * Structures returned by network data base library, taken from the
 * BSD file netdb.h.  All addresses are supplied in host order, and
 * returned in network order (suitable for use in system calls).
 */

typedef struct ws_hostent
{
        char *  h_name;         /* official name of host */
        char ** h_aliases;      /* alias list */
        INT16   h_addrtype;     /* host address type */
        INT16   h_length;       /* length of address */
        char ** h_addr_list;    /* list of addresses from name server */
} _ws_hostent;

typedef struct ws_protoent
{
        char *  p_name;         /* official protocol name */
        char ** p_aliases;      /* alias list */
        INT16   p_proto;        /* protocol # */
} _ws_protoent;

typedef struct ws_servent 
{
        char *  s_name;         /* official service name */
        char ** s_aliases;      /* alias list */
        INT16   s_port;         /* port # */
        char *  s_proto;        /* protocol to use */
} _ws_servent;

typedef struct ws_netent
{
        char *  n_name;         /* official name of net */
        char ** n_aliases;      /* alias list */
        INT16   n_addrtype;     /* net address type */
        INT     n_net;          /* network # */
} _ws_netent;

#pragma pack()

#define WS_MAX_SOCKETS_PER_PROCESS      2048
#define WS_MAX_UDP_DATAGRAM             32767

#define WSI_BLOCKINGCALL        0x00000001      /* per-thread info flags */
#define WSI_BLOCKINGHOOK        0x00000002      /* 32-bit callback */

typedef struct _WSINFO
{
  DWORD			dwThisThread;
  struct _WSINFO       *lpNextIData;

  unsigned		flags;
  INT16			num_startup;		/* reference counter */
  INT16			num_async_rq;
  INT16			last_free;		/* entry in the socket table */
  USHORT		buflen;
  char*			buffer;			/* allocated from char * heap */
  struct ws_hostent	*he;
  int			helen;
  struct ws_servent	*se;
  int			selen;
  struct ws_protoent	*pe;
  int			pelen;
  char*			dbuffer;		/* buffer for dummies (32 bytes) */

  DWORD			blocking_hook;
} WSINFO, *LPWSINFO;

void WIN32API WSASetLastError(int iError);
int  WIN32API WSAGetLastError(void);
int  WIN32API WSAAsyncSelect(SOCKET s, HWND hWnd, u_int wMsg, long lEvent);
BOOL WIN32API WSAIsBlocking(void);

extern BOOL fWSAInitialized;

UINT wsaErrno();
UINT wsaHerrno();

#define CURRENT_THREAD -1
void WSASetBlocking(BOOL fBlock, HANDLE tid = CURRENT_THREAD);

void WINSOCK_DeleteIData(void);
BOOL WINSOCK_CreateIData(void);
LPWSINFO WINSOCK_GetIData(HANDLE tid = CURRENT_THREAD);

#endif  /* _WINSOCK32CONST_ */


