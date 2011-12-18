/* $Id: wsock32.cpp,v 1.52 2003-03-03 16:34:39 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 SOCK32 for OS/2
 *
 * Copyright (C) 1999 Patrick Haller <phaller@gmx.net>
 * Copyright (C) 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Some parts based on Wine code: (dlls\winsock\socket.c)
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 *
 */

/* Remark:
 * 1999/11/21 experimental rewrite using IBM's PMWSock only
 *            -> some structural differences remain! (hostent)
 * 1999/12/01 experimental rewrite works (TELNET)
 *            -> open issue: WSASetLastError / WSAGetLastError
 *               call SetLastError / GetLastError according to docs
 *
 * 2000/22/03 Complete rewrite -> got rid of pmwsock
 *
 * identical structures:
 * - sockaddr_in
 * - WSADATA
 * - sockaddr
 * - fd_set
 * - timeval
 *
 * incompatible structures:
 * - hostent
 * - netent
 * - servent
 * - protent
 * - linger
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define INCL_BASE
#include <os2wrap.h>	//Odin32 OS/2 api wrappers

#include <string.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <stdlib.h>
#include <win32api.h>
#define NO_DCDATA
#include <winuser32.h>
#include <wprocess.h>
#include <dbglog.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

#include "wsock32.h"
#include "ws2defs.h"
#include "wsastruct.h"
#include "asyncthread.h"

#define DBG_LOCALLOG	DBG_wsock32
#include "dbglocal.h"

//
#define DUMP_PACKETS

ODINDEBUGCHANNEL(WSOCK32-WSOCK32)

/*****************************************************************************
 * Local variables                                                           *
 *****************************************************************************/

static LPWSINFO lpFirstIData = NULL;


//******************************************************************************
//******************************************************************************

// Note: for the TCP/IP 4.0 headers, SO_SNDTIMEO and SO_RCDTIMEO are
// unfortunately not defined, so we do this here.
#ifndef SO_SNDTIMEO
#define SO_SNDTIMEO 0x1005
#endif

#ifndef SO_RCVTIMEO
#define SO_RCVTIMEO 0x1006
#endif


//******************************************************************************
//******************************************************************************
LPWSINFO WINSOCK_GetIData(HANDLE tid)
{
    LPWSINFO iData;
    BOOL     fCurrentThread = FALSE;

    if(tid == CURRENT_THREAD) {
	tid = GetCurrentThread();
	fCurrentThread = TRUE;
    }
tryagain:
    for (iData = lpFirstIData; iData; iData = iData->lpNextIData) {
	if (iData->dwThisThread == tid)
	    break;
    }
    if(iData == NULL && fCurrentThread) {
	WINSOCK_CreateIData();
	fCurrentThread = FALSE; //just to prevent infinite loops
	goto tryagain;
    }
    if(iData == NULL) {
	dprintf(("WINSOCK_GetIData: couldn't find struct for thread %x", tid));
	DebugInt3();// should never happen!!!!!!!
    }
    return iData;
}
//******************************************************************************
//******************************************************************************
BOOL WINSOCK_CreateIData(void)
{
    LPWSINFO iData;

    iData = (LPWSINFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WSINFO));
    if (!iData)
	return FALSE;
    iData->dwThisThread = GetCurrentThread();
    iData->lpNextIData = lpFirstIData;
    lpFirstIData = iData;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
void WINSOCK_DeleteIData(void)
{
    LPWSINFO iData = WINSOCK_GetIData();
    LPWSINFO* ppid;
    if (iData) {
	for (ppid = &lpFirstIData; *ppid; ppid = &(*ppid)->lpNextIData) {
	    if (*ppid == iData) {
	        *ppid = iData->lpNextIData;
	        break;
	    }
	}

	if( iData->flags & WSI_BLOCKINGCALL )
		dprintf(("\tinside blocking call!\n"));

	/* delete scratch buffers */
	if(iData->he)	free(iData->he);
	if(iData->se)	free(iData->se);
	if(iData->pe)	free(iData->pe);

	////	if( iData->buffer ) SEGPTR_FREE(iData->buffer);
	////	if( iData->dbuffer ) SEGPTR_FREE(iData->dbuffer);

	HeapFree(GetProcessHeap(), 0, iData);
    }
}
//******************************************************************************
//******************************************************************************
void WIN32API WSASetLastError(int iError)
{
#ifdef DEBUG
    char msg[20];
#endif
    // according to the docs, WSASetLastError() is just a call-through
    // to SetLastError()
    SetLastError(iError);
#ifdef DEBUG
    switch (iError)
    {
        case NO_ERROR:
            strcpy(msg, "no error");
            break;
        case WSAEINTR:
            strcpy(msg, "WSAEINTR");
            break;
        case WSAEBADF:
            strcpy(msg, "WSAEBADF");
            break;
        case WSAEACCES:
            strcpy(msg, "WSAEACCES");
            break;
        case WSAEFAULT:
            strcpy(msg, "WSAEFAULT");
            break;
        case WSAEINVAL:
            strcpy(msg, "WSAEINVAL");
            break;
        case WSAEMFILE:
            strcpy(msg, "WSAEMFILE");
            break;
        case WSAEWOULDBLOCK:
            strcpy(msg, "WSAEWOULDBLOCK");
            break;
        case WSAEINPROGRESS:
            strcpy(msg, "WSAEINPROGRESS");
            break;
        case WSAEALREADY:
            strcpy(msg, "WSAEALREADY");
            break;
        case WSAHOST_NOT_FOUND:
            strcpy(msg, "WSAHOST_NOT_FOUND");
            break;
        case WSAENOPROTOOPT:
            strcpy(msg, "WSAENOPROTOOPT");
            break;
        case WSAEHOSTUNREACH:
            strcpy(msg, "WSAEHOSTUNREACH");
            break;
        case WSAETIMEDOUT:
            strcpy(msg, "WSAETIMEDOUT");
            break;
        default:
            strcpy(msg, "unknown");
    }
    if (iError != 0)
    {
        dprintf(("WSASetLastError 0x%x - %s", iError, msg));
    }
#endif
}
//******************************************************************************
//******************************************************************************
int WIN32API WSAGetLastError()
{
  return GetLastError();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(int,OS2shutdown,
              SOCKET,s,
              int,how)
{
 int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   ret = shutdown(s, how);

   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(SOCKET,OS2socket,
              int,af,
              int,type,
              int,protocol)
{
 SOCKET s;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   s = socket(af, type, protocol);

   if(s == SOCKET_ERROR && sock_errno() == SOCEPFNOSUPPORT) {
	//map SOCEPFNOSUPPORT to SOCEPFNOSUPPORT
       	WSASetLastError(SOCEPFNOSUPPORT);
   }
   else
   if(s == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return s;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(int,OS2closesocket,SOCKET, s)
{
 int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   //Close WSAAsyncSelect thread if one was created for this socket
   FindAndSetAsyncEvent(s, WSA_SELECT_HWND, 0, 0, 0);

   // wait thread termination
   DosSleep(10);
   ret = soclose(s);

   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,OS2connect,
              SOCKET, s,
              const struct sockaddr *,name,
              int, namelen)
{
 int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   dprintf(("connect to %s", inet_ntoa(((sockaddr_in*)name)->sin_addr)));

   ret = connect(s, (sockaddr *)name, namelen);
   // map BSD error codes
   if(ret == SOCKET_ERROR) {
	int sockerror = sock_errno();
   	if(sockerror && sockerror < SOCBASEERR) {
		sockerror += SOCBASEERR;
	}
      	if(sockerror == SOCEINPROGRESS) {
         	WSASetLastError(WSAEWOULDBLOCK);
      	}
	else
	if(sockerror == SOCEOPNOTSUPP) {
         	WSASetLastError(WSAEINVAL);
      	}
	else	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,OS2ioctlsocket,
              SOCKET,s,
              long, cmd,
              u_long *,argp)
{
 int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   // clear high word (not used in OS/2's tcpip stack)
   cmd = LOUSHORT(cmd);

   if(cmd != FIONBIO && cmd != FIONREAD && cmd != SIOCATMARK) {
        WSASetLastError(WSAEINVAL);
      	return SOCKET_ERROR;
   }

   WSASetLastError(NO_ERROR);

   //check if app want to set a socket, which has an outstanding async select,
   //to blocking mode
   if (cmd == FIONBIO) {
	ULONG ulNotifyHandle, ulNotifyData;
	int   mode;
 	ULONG lEvent;

	if(QueryAsyncEvent(s, &mode, &ulNotifyHandle, &ulNotifyData, &lEvent) == TRUE) 
        {
		if(*argp != 0) {
			//nothing to do; already non-blocking
			return NO_ERROR;
		}
		else 
		if(lEvent != 0) {
			dprintf(("Trying to set socket to blocking mode while async select active -> return error!"));
                  	WSASetLastError(WSAEINVAL);
                  	return SOCKET_ERROR;
		}
	}
   }
   ret = ioctl(s, cmd, (char *)argp, sizeof(int));

   // Map EOPNOTSUPP to EINVAL
   int sockerror = sock_errno();
   if(sockerror && sockerror < SOCBASEERR) {
	sockerror += SOCBASEERR;
   }

   if(ret == SOCKET_ERROR && sockerror == SOCEOPNOTSUPP)
     	WSASetLastError(WSAEINVAL);
   else
   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,OS2getpeername,
              SOCKET, s,
              struct sockaddr *,name,
              int *, namelen)
{
 int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   else
   if (namelen == NULL || *namelen < (int)sizeof(struct sockaddr_in)) {
      	WSASetLastError(WSAEFAULT);
      	return SOCKET_ERROR;
   }
   ret = getsockname(s, name, namelen);
   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,OS2getsockname,
              SOCKET,s,
              struct sockaddr *,name,
              int *, namelen)
{
 int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   else
   if (namelen == NULL || *namelen < (int)sizeof(struct sockaddr_in)) {
      	WSASetLastError(WSAEFAULT);
      	return SOCKET_ERROR;
   }
   ret = getsockname(s, name, namelen);
   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(u_long,OS2htonl,
              u_long,hostlong)
{
  return(htonl(hostlong));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(u_short,OS2htons,
              u_short,hostshort)
{
  return(htons(hostshort));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(u_long,OS2ntohl,
              u_long,netlong)
{
  return(ntohl(netlong));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(u_short,OS2ntohs,
              u_short,netshort)
{
  return(ntohs(netshort));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(unsigned long,OS2inet_addr,
              const char *, cp)
{
  dprintf(("WSOCK32: OS2inet_addr(%s)\n",
           cp));

  return (inet_addr((char *)cp));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(char *,OS2inet_ntoa,
              struct in_addr, in)
{
  return(inet_ntoa(in));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(SOCKET,OS2accept, SOCKET,           s,
                                struct sockaddr *,addr,
                                int *,            addrlen)
{
 int   ret, mode;
 ULONG lEvent, notifyData, notifyHandle;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   else
   if ((addr != NULL) && (addrlen != NULL)) {
        if (*addrlen < (int)sizeof(struct sockaddr_in)) {
      		WSASetLastError(WSAEFAULT);
      		return SOCKET_ERROR;
        }
   }
   ret = accept(s, addr, addrlen);

   if(ret != SOCKET_ERROR) {
	//Enable FD_ACCEPT event flag if WSAAsyncSelect was called for this socket
      	EnableAsyncEvent(s, FD_ACCEPT);

	//if this socket has an active async. select pending, then call WSAAsyncSelect
        //with the same parameters for the new socket (see docs)
	if(QueryAsyncEvent(s, &mode, &notifyHandle, &notifyData, &lEvent) == TRUE) {
                dprintf(("Setting async select for socket %x, mode %d, %x %x %x", ret, mode, notifyHandle, notifyData, lEvent));
        	if(WSAAsyncSelectWorker(ret, mode, notifyHandle, notifyData, lEvent) == SOCKET_ERROR) {
            		ret = SOCKET_ERROR;
		}
	}
   }
   if(ret == SOCKET_ERROR) {
   	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,OS2bind,
              SOCKET ,s,
              const struct sockaddr *,addr,
              int, namelen)
{
 int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   else
   if(namelen < (int)sizeof(struct sockaddr_in)) {
      	WSASetLastError(WSAEFAULT);
      	return SOCKET_ERROR;
   }
   dprintf(("bind to %s", inet_ntoa(((sockaddr_in*)addr)->sin_addr)));
   ret = bind(s, (struct sockaddr *)addr, namelen);

   if(ret == SOCKET_ERROR) {
   	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(int,OS2listen,
              SOCKET, s,
              int, backlog)
{
   int ret, tmp, namelen;
   struct sockaddr_in name;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   namelen = sizeof(name);
   ret = getsockname(s, (struct sockaddr *)&name, &namelen);
   if (ret == 0) {
        if (name.sin_port == 0 && name.sin_addr.s_addr == 0) {
            	// Socket is not bound
            	WSASetLastError(WSAEINVAL);
            	return SOCKET_ERROR;
        }
      	ret = ioctl(s, FIOBSTATUS, (char *)&tmp, sizeof(tmp)) &
                   (SS_ISCONNECTING | SS_ISCONNECTED | SS_ISDISCONNECTING);
        if(ret) {
        	// Socket is already connected
            	WSASetLastError(WSAEISCONN);
               	return SOCKET_ERROR;
	}
        ret = listen(s, backlog);
	//todo: reset FD_ACCEPT bit? (wine seems to do this, but it's not documented)
   }
   if(ret == SOCKET_ERROR) {
   	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(int,OS2recv,
              SOCKET,s,
              char *,buf,
              int,len,
              int,flags)
{
   int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   ret = recv(s, buf, len, flags);

   if(ret == SOCKET_ERROR) {
        if(wsaErrno() == WSAEWOULDBLOCK) {
            struct timeval tv;
            int optlen = sizeof(tv);
            ret = getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, &optlen);
            if(ret == 0 && (tv.tv_sec > 0 || tv.tv_usec > 0)) {
                  dprintf(("WSAEWOULDBLOCK: recv timeout set to %ds%dus -> return WSAETIMEDOUT", tv.tv_sec, tv.tv_usec));
                  WSASetLastError(WSAETIMEDOUT);
            }
            else  WSASetLastError(WSAEWOULDBLOCK);
            ret = SOCKET_ERROR;
        }
        else WSASetLastError(wsaErrno());
   }
   else 
   if(ret == 0) {
	int tmp, state;

       	state = ioctl(s, FIOBSTATUS, (char *)&tmp, sizeof(tmp));
        if(state & SS_CANTRCVMORE) {
		dprintf(("recv returned 0, socket is no longer connected -> return WSAENOTCONN"));
 		WSASetLastError(WSANO_DATA);
		return 0; //graceful close
        }
        else
        if(state & (SS_ISDISCONNECTING|SS_ISDISCONNECTED)) {
		dprintf(("recv returned 0, socket is no longer connected -> return WSAENOTCONN"));
 		WSASetLastError(WSAENOTCONN);
		return 0; //graceful close
        }
        else
	if(state & SS_ISCONNECTED && flags != MSG_PEEK) {
		dprintf(("recv returned 0, but socket is still connected -> return WSAEWOULDBLOCK"));
 		WSASetLastError(WSAEWOULDBLOCK);
		return SOCKET_ERROR;
	}
   }
   else {
#ifdef DUMP_PACKETS
       dprintf(("Packet length %d", ret));
       for(int i=0;i<(ret+7)/8;i++) {
           dprintf2(("%02x %02x %02x %02x %02x %02x %02x %02x %c %c %c %c %c %c %c %c", buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7], buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7]));
       }
#endif
       WSASetLastError(NO_ERROR);
   }

   //Reset FD_READ event flag for WSAAsyncSelect thread if one was created for this socket
   EnableAsyncEvent(s, FD_READ);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(int,OS2recvfrom,
              SOCKET,s,
              char *,buf,
              int,len,
              int,flags,
              struct sockaddr *,from,
              int *,fromlen)
{
   int ret;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   //NOTE: do not check from & fromlen; they are allowed to be NULL/0

   if(from) {
       dprintf(("recvfrom to %s", inet_ntoa(((sockaddr_in*)from)->sin_addr)));
   }
   ret = recvfrom(s, buf, len, flags, from, fromlen);

   if(ret == SOCKET_ERROR) {
        if(wsaErrno() == WSAEWOULDBLOCK) {
            struct timeval tv;
            int optlen = sizeof(tv);
            ret = getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, &optlen);
            if(ret == 0 && (tv.tv_sec > 0 || tv.tv_usec > 0)) {
                  dprintf(("WSAEWOULDBLOCK: recvfrom timeout set to %ds%dus -> return WSAETIMEDOUT", tv.tv_sec, tv.tv_usec));
                  WSASetLastError(WSAETIMEDOUT);
            }
            else  WSASetLastError(WSAEWOULDBLOCK);
            ret = SOCKET_ERROR;
        }
 	else WSASetLastError(wsaErrno());
   }
   else {
#ifdef DUMP_PACKETS
       dprintf2(("Packet length %d", ret));
       for(int i=0;i<(ret+7)/8;i++) {
           dprintf2(("%02x %02x %02x %02x %02x %02x %02x %02x %c %c %c %c %c %c %c %c", buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7], buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7]));
       }
#endif
       WSASetLastError(NO_ERROR);
   }

   //Reset FD_READ event flagfor  WSAAsyncSelect thread if one was created for this socket
   EnableAsyncEvent(s, FD_READ);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(int,OS2send,
              SOCKET,s,
              const char *,buf,
              int,len,
              int,flags)
{
   int ret;
   int optlen;
   int option;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   // check if the socket is a raw socket and has the IP_HDRINCL switch
   // if this is the case, we overwrite the IP header length field with
   // the actual length because some apps tend to put garbage in there
   // and rely on Windows to correct this
   optlen = sizeof(option);
   option = 0;
   ret = getsockopt(s, IPPROTO_IP, IP_HDRINCL_OS2, (char *)&option, &optlen);
   if(ret == 0 && option != FALSE) {
       *(u_short *)&buf[2] = len;
   }

#ifdef DUMP_PACKETS
   dprintf2(("Packet length %d", len));
   for(int i=0;i<(len+7)/8;i++) {
           dprintf2(("%02x %02x %02x %02x %02x %02x %02x %02x %c %c %c %c %c %c %c %c", buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7], buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7]));
   }
#endif
   ret = send(s, (char *)buf, len, flags);

   if(ret == SOCKET_ERROR) {
        if(wsaErrno() == WSAEWOULDBLOCK) {
            struct timeval tv;
            int optlen = sizeof(tv);
            ret = getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, &optlen);
            if(ret == 0 && (tv.tv_sec > 0 || tv.tv_usec > 0)) {
                  dprintf(("WSAEWOULDBLOCK: send timeout set to %ds%dus -> return WSAETIMEDOUT", tv.tv_sec, tv.tv_usec));
                  WSASetLastError(WSAETIMEDOUT);
            }
            else  WSASetLastError(WSAEWOULDBLOCK);
            ret = SOCKET_ERROR;
        }
 	else WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);

   //Reset FD_WRITE event flagfor  WSAAsyncSelect thread if one was created for this socket
   EnableAsyncEvent(s, FD_WRITE);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(int,OS2sendto,
              SOCKET,s,
              const char *,buf,
              int,len,
              int,flags,
              const struct sockaddr *,to,
              int,tolen)
{
   int ret;
   int optlen;
   int option;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   //NOTE: do not check to & tolen; they are allowed to be NULL/0

   // check if the socket is a raw socket and has the IP_HDRINCL switch
   // if this is the case, we overwrite the IP header length field with
   // the actual length because some apps tend to put garbage in there
   // and rely on Windows to correct this
   optlen = sizeof(option);
   option = 0;
   ret = getsockopt(s, IPPROTO_IP, IP_HDRINCL_OS2, (char *)&option, &optlen);
   if(ret == 0 && option != FALSE) {
       *(u_short *)&buf[2] = len;
   }
   if(to) {
       dprintf(("sending to %s", inet_ntoa(((sockaddr_in*)to)->sin_addr)));
   }
#ifdef DUMP_PACKETS
   dprintf2(("Packet length %d", len));
   for(int i=0;i<(len+7)/8;i++) {
           dprintf2(("%02x %02x %02x %02x %02x %02x %02x %02x %c %c %c %c %c %c %c %c", buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7], buf[i*8], buf[i*8+1], buf[i*8+2], buf[i*8+3], buf[i*8+4], buf[i*8+5], buf[i*8+6], buf[i*8+7]));
   }
#endif
   ret = sendto(s, (char *)buf, len, flags, (struct sockaddr *)to, tolen);

   if(ret == SOCKET_ERROR) {
        if(wsaErrno() == WSAEWOULDBLOCK) {
            struct timeval tv;
            int optlen = sizeof(tv);
            ret = getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, &optlen);
            if(ret == 0 && (tv.tv_sec > 0 || tv.tv_usec > 0)) {
                  dprintf(("WSAEWOULDBLOCK: sendto timeout set to %ds%dus -> return WSAETIMEDOUT", tv.tv_sec, tv.tv_usec));
                  WSASetLastError(WSAETIMEDOUT);
            }
            else  WSASetLastError(WSAEWOULDBLOCK);
            ret = SOCKET_ERROR;
        }
 	else WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);

   //Reset FD_WRITE event flag for WSAAsyncSelect thread if one was created for this socket
   EnableAsyncEvent(s, FD_WRITE);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,OS2select,
              int,nfds,
              ws_fd_set *,readfds,
              ws_fd_set *,writefds,
              ws_fd_set *,exceptfds,
              const struct timeval *,timeout)
{
 int ret, i, j;
 int *sockets, *socktmp;
 int nrread, nrwrite, nrexcept;
 ULONG ttimeout;

   WSASetLastError(NO_ERROR);

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   else {
	nrread = nrwrite = nrexcept = 0;
	if(readfds) {
		nrread += readfds->fd_count;
	}
	if(writefds) {
		nrwrite += writefds->fd_count;
	}
	if(exceptfds) {
		nrexcept += exceptfds->fd_count;
	}
#if 0
      	if(nrread + nrwrite + nrexcept  == 0) {
                dprintf(("ERROR: nrread + nrwrite + nrexcept  == 0"));
         	WSASetLastError(WSAEINVAL);
         	return SOCKET_ERROR;
	}
#endif
      	if(timeout != NULL && (timeout->tv_sec < 0 || timeout->tv_usec < 0)) {
                dprintf(("ERROR: timeout->tv_sec < 0 || timeout->tv_usec < 0"));
         	WSASetLastError(WSAEINVAL);
         	return SOCKET_ERROR;
      	}
        if(timeout == NULL) {
            	ttimeout = -1L; // no timeout
        }
	else    ttimeout = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;

	sockets = (int *)malloc(sizeof(int) * (nrread+nrwrite+nrexcept));
	if(readfds && nrread) {
		memcpy(&sockets[0], readfds->fd_array, nrread * sizeof(SOCKET));
	}
	if(writefds && nrwrite) {
		memcpy(&sockets[nrread], writefds->fd_array, nrwrite * sizeof(SOCKET));
	}
	if(exceptfds && nrexcept) {
		memcpy(&sockets[nrread+nrwrite], exceptfds->fd_array, nrexcept * sizeof(SOCKET));
	}

        ret = select(sockets, nrread, nrwrite, nrexcept, ttimeout);

      	if(ret == SOCKET_ERROR)
	{
            	if(readfds != NULL)
               		readfds->fd_count = 0;

            	if(writefds != NULL)
               		writefds->fd_count = 0;

            	if(exceptfds != NULL)
               		exceptfds->fd_count = 0;

 		WSASetLastError(wsaErrno());
		free(sockets);
		return SOCKET_ERROR;
      	}

        if(ret != 0) {
		socktmp = sockets;
            	if(readfds != NULL) {
			j = 0;
               		for(i=0;i<nrread;i++) {
                  		if(socktmp[i] != -1) {
	                     		readfds->fd_array[j] = socktmp[i];
	                     		dprintf(("Socket %d read pending", socktmp[i]));
					j++;
				}
	                }
	               	readfds->fd_count = j;
			socktmp += nrread;
	        }

            	if(writefds != NULL) {
			j = 0;
               		for(i=0;i<nrwrite;i++) {
                  		if(socktmp[i] != -1) {
	                     		writefds->fd_array[j] = socktmp[i];
	                     		dprintf(("Socket %d write pending", socktmp[i]));
					j++;
				}
	                }
	               	writefds->fd_count = j;
			socktmp += nrwrite;
            	}
            	if(exceptfds != NULL) {
			j = 0;
               		for(i=0;i<nrexcept;i++) {
                  		if(socktmp[i] != -1) {
	                     		exceptfds->fd_array[j] = socktmp[i];
					j++;
				}
	                }
	               	exceptfds->fd_count = j;
            	}
         }
	else {
            if(readfds != NULL)
               readfds->fd_count = 0;

            if(writefds != NULL)
               writefds->fd_count = 0;

            if(exceptfds != NULL)
               exceptfds->fd_count = 0;
        }
	free(sockets);
   }
   return ret;
}
#ifdef DEBUG_LOGGING
//******************************************************************************
//******************************************************************************
const char *debugsockopt(int optname)
{
    switch(optname) {
    case SO_DONTLINGER:
         return "SO_DONTLINGER";
    case SO_LINGER:
         return "SO_LINGER";
    case SO_REUSEADDR:
         return "SO_REUSEADDR";
    case SO_SNDTIMEO:
         return "SO_SNDTIMEO";
    case SO_RCVTIMEO:
         return "SO_RCVTIMEO";
    case SO_SNDBUF:
         return "SO_SNDBUF";
    case SO_RCVBUF:
         return "SO_RCVBUF";
    case SO_BROADCAST:
         return "SO_BROADCAST";
    case SO_DEBUG:
         return "SO_DEBUG";
    case SO_KEEPALIVE:
         return "SO_KEEPALIVE";
    case SO_DONTROUTE:
         return "SO_DONTROUTE";
    case SO_OOBINLINE:
         return "SO_OOBINLINE";
    case SO_TYPE:
         return "SO_TYPE";
    case SO_ERROR:
         return "SO_ERROR";
    case SO_SNDLOWAT:
         return "SO_SNDLOWAT";
    case SO_RCVLOWAT:
         return "SO_RCVLOWAT";
    case SO_USELOOPBACK:
         return "SO_USELOOPBACK";
    case SO_ACCEPTCONN:
         return "SO_ACCEPTCONN";
    default:
        return "unknown option";
    }
}
#endif
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,OS2setsockopt,
              SOCKET,s,
              int,level,
              int,optname,
              const char *,optval,
              int,optlen)
{
  struct ws_linger *yy;
  struct linger     xx;
  int               ret, val;
  ULONG             size;
  char             *safeoptval;

   if(!fWSAInitialized) {
        dprintf(("WSA not initialized"));
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
        dprintf(("WSA is blocking"));
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   //SvL: The 16 bits TCP/IP stack doesn't like high addresses, so copy
   //     the option value(s) on the stack.
   safeoptval = (char *)alloca(optlen);
   if(safeoptval == NULL) { 
	DebugInt3();
      	WSASetLastError(WSAEFAULT);
	return SOCKET_ERROR;
   }
   memcpy(safeoptval, optval, optlen);
   optval = safeoptval;

   if (level == SOL_SOCKET)
   {
	switch(optname)
        {
	case SO_DONTLINGER:
	case SO_LINGER:
            	if(optlen < (int)sizeof(ws_linger))
                {
                        dprintf(("SOL_SOCKET, SO_LINGER, optlen too small"));
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
    		yy = (struct ws_linger *)optval;
                dprintf(("%s: onoff %x linger %x", (optname == SO_DONTLINGER) ? "SO_DONTLINGER" : "SO_LINGER", (int)yy->l_onoff, (int)yy->l_linger));
		xx.l_onoff  = (optname == SO_DONTLINGER) ? !yy->l_onoff : yy->l_onoff;
		xx.l_linger = yy->l_linger;

		ret = setsockopt(s,level,optname,(char *)&xx, sizeof(xx));
		break;
	case SO_SNDBUF:
	case SO_RCVBUF:
            	if(optlen < (int)sizeof(int))
                {
                        dprintf(("SOL_SOCKET, SO_RCVBUF, optlen too small"));
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}

		size = *(ULONG *)optval;
tryagain:
		ret = setsockopt(s,level,optname, (char *)&size, sizeof(ULONG));
		if(ret == SOCKET_ERROR && wsaErrno() == WSAENOBUFS && size > 4096) {
			int newsize = (size > 65535) ? 63*1024 : (size-1024);
			dprintf(("setsockopt: change size from %d to %d", size, newsize));
			//SvL: Limit send & receive buffer length to 64k
	                //     (only happens with 16 bits tcpip stack?)
			size = newsize;
			goto tryagain;
		}
		break;
        
        case SO_SNDTIMEO:
        case SO_RCVTIMEO:
        {
            	if(optlen < (int)sizeof(int))
                {
                        dprintf(("SO_RCVTIMEO, SO_SNDTIMEO, optlen too small"));
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
          // convert "int" to "struct timeval"
          struct timeval tv;
                tv.tv_sec = (*(int *)optval) / 1000;
                tv.tv_usec = ((*(int *)optval) % 1000) * 1000;
                if(optname == SO_SNDTIMEO) {
                     dprintf(("SO_SNDTIMEO: int val %x sec %d, usec %d", *(int *)optval, tv.tv_sec, tv.tv_usec));
                }
                else dprintf(("SO_RCVTIMEO: int val %x sec %d, usec %d", *(int *)optval, tv.tv_sec, tv.tv_usec));

          ret = setsockopt(s, level, optname, (char *)&tv, sizeof(tv) );
          break;
        }

	case SO_REUSEADDR:
            	if(optlen < (int)sizeof(int)) {
                        dprintf(("SO_REUSEADDR, optlen too small"));
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                dprintf(("option SO_REUSEADDR value %d", *(int *)optval));
                ret = setsockopt(s, level, SO_REUSEADDR, (char *)optval, optlen);
                if(ret) {
                    dprintf(("setsockopt SO_REUSEADDR failed!!"));
                }
                ret = setsockopt(s, level, SO_REUSEPORT_OS2, (char *)optval, optlen);
                if(ret) {
                    dprintf(("setsockopt SO_REUSEPORT failed!!"));
                }
		break;

	case SO_BROADCAST:
	case SO_DEBUG:
	case SO_KEEPALIVE:
	case SO_DONTROUTE:
        case SO_OOBINLINE:
        case SO_ERROR:
        case SO_SNDLOWAT:
        case SO_RCVLOWAT:
        case SO_ACCEPTCONN:
        case SO_USELOOPBACK:
            	if(optlen < (int)sizeof(int)) {
                        dprintf(("%s optlen too small", debugsockopt(optname)));
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                dprintf(("option %s = %x", debugsockopt(optname), *(int *)optval));
                ret = setsockopt(s, level, optname, (char *)optval, optlen);
		break;
	default:
		dprintf(("setsockopt: SOL_SOCKET, unknown option %x", optname));
            	WSASetLastError(WSAENOPROTOOPT);
            	return SOCKET_ERROR;
        }
   }
   else
   if(level == IPPROTO_TCP)
   {
       	if(optname == TCP_NODELAY) {
            	if(optlen < (int)sizeof(int)) {
                        dprintf(("IPPROTO_TCP, TCP_NODELAY, optlen too small"));
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                dprintf(("IPPROTO_TCP, TCP_NODELAY 0x%x", *optval));
               	ret = setsockopt(s, level, optname, (char *)optval, optlen);
        }
	else {
		dprintf(("setsockopt: IPPROTO_TCP, unknown option %x", optname));
       		WSASetLastError(WSAENOPROTOOPT);
      		return SOCKET_ERROR;
        }
   }
   else
   if (level == IPPROTO_IP)
   {
       switch (optname)
       {
           case IP_MULTICAST_IF:
           case WS2_IPPROTO_OPT(IP_MULTICAST_IF_WS2):
           {
               if (optlen < sizeof(in_addr))
               {
                   dprintf(("IPPROTO_IP, IP_MULTICAST_IP, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               //TODO convert common interface names!
               ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF_OS2, (char *)optval, optlen);
               break;
           }

           case IP_ADD_MEMBERSHIP:
           case WS2_IPPROTO_OPT(IP_ADD_MEMBERSHIP_WS2):
               if (optlen < sizeof(struct ip_mreq))
               {
                   dprintf(("IPPROTO_IP, IP_ADD_MEMBERSHIP, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               ret = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP_OS2, (char *)optval, optlen);
               break;

           case IP_DROP_MEMBERSHIP:
           case WS2_IPPROTO_OPT(IP_DROP_MEMBERSHIP_WS2):
               if (optlen < sizeof(struct ip_mreq))
               {
                   dprintf(("IPPROTO_IP, IP_DROP_MEMBERSHIP, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               ret = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP_OS2, (char *)optval, optlen);
               break;

           case IP_MULTICAST_LOOP:
           case WS2_IPPROTO_OPT(IP_MULTICAST_LOOP_WS2):
           {
               u_int flLoop;
               if (optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_MULTICAST_LOOP/IP_MULTICAST_TTL, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               flLoop = (*optval == 0) ? 0 : 1;
               dprintf(("IP_MULTICAST_LOOP %d", *optval));
               ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP_OS2, (char *)&flLoop, sizeof(u_char));
               break;
           }

           case IP_MULTICAST_TTL:
           case WS2_IPPROTO_OPT(IP_MULTICAST_TTL_WS2):
               if (optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_MULTICAST_TTL, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               dprintf(("IP_MULTICAST_TTL %d", *optval));
               ret = setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL_OS2, (char *)optval, sizeof(u_char));
               break;

           case IP_TTL:
           case WS2_IPPROTO_OPT(IP_TTL_WS2):
               if (optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_TTL_WS2, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               dprintf(("IPPROTO_IP, IP_TTL 0x%x", *optval));
               ret = setsockopt(s, IPPROTO_IP, IP_TTL_OS2, (char *)optval, optlen);
               break;

           case IP_TOS:
           case WS2_IPPROTO_OPT(IP_TOS_WS2):
               if (optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_TOS_WS2, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               dprintf(("IPPROTO_IP, IP_TOS 0x%x", *optval));
               ret = setsockopt(s, IPPROTO_IP, IP_TOS_OS2, (char *)optval, optlen);
               break;

           case WS2_IPPROTO_OPT(IP_HDRINCL_WS2):
               if (optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_HDRINCL_WS2, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               val = *optval;
               dprintf(("IPPROTO_IP, IP_HDRINCL 0x%x", val));
               ret = setsockopt(s, IPPROTO_IP, IP_HDRINCL_OS2, (char *)&val, optlen);
               break;

           case IP_DONTFRAGMENT:
           case WS2_IPPROTO_OPT(IP_DONTFRAGMENT_WS2):
               //MSDN says these options are silently ignored
               dprintf(("IPPROTO_IP: IP_DONTFRAGMENT ignored"));
               ret = 0;
               break;

           default:
		dprintf(("setsockopt: IPPROTO_IP, unknown option %x", optname));
       		WSASetLastError(WSAENOPROTOOPT);
      		return SOCKET_ERROR;
       }
   }
   else {
        dprintf(("unknown level code!"));
       	WSASetLastError(WSAEINVAL);
	return SOCKET_ERROR;
   }

   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,OS2getsockopt,
              SOCKET, s,
              int, level,
              int, optname,
              char *, optval,
              int *,optlen)
{
  struct ws_linger *yy;
  struct linger     xx;
  int               ret;
  int               size, options;

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   }
   else
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   }
   if (level == SOL_SOCKET) {
	switch(optname) {
	case SO_DONTLINGER:
	case SO_LINGER:
            	if(optlen == NULL || *optlen < sizeof(ws_linger)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
		size = sizeof(xx);
		ret = getsockopt(s,level,optname,(char *)&xx, &size);
    		yy = (struct ws_linger *)optval;
		yy->l_onoff  = (optname == SO_DONTLINGER) ? !xx.l_onoff : xx.l_onoff;
		yy->l_linger = xx.l_linger;
		*optlen = size;
		break;

	case SO_REUSEADDR:
            	if(optlen == NULL || *optlen < sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                ret = getsockopt(s, level, SO_REUSEADDR, (char *)optval, optlen);
                dprintf(("getsockopt SO_REUSEADDR returned %d", *(int *)optval, optname));
		break;
		break;

        case SO_SNDTIMEO:
        case SO_RCVTIMEO:
        {
            	if(optlen == NULL || *optlen < sizeof(int))
                {
                        dprintf(("SO_RCVTIMEO, SO_SNDTIMEO, optlen too small"));
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                struct timeval tv;
                int size = sizeof(tv);
                ret = getsockopt(s, level, optname, (char *)&tv, &size );

                // convert "struct timeval" to "int"
                *(int *)optval = (tv.tv_sec * 1000) + tv.tv_usec/1000;
                if(optname == SO_SNDTIMEO) {
                     dprintf(("SO_SNDTIMEO: int val %x sec %d, usec %d", *(int *)optval, tv.tv_sec, tv.tv_usec));
                }
                else dprintf(("SO_RCVTIMEO: int val %x sec %d, usec %d", *(int *)optval, tv.tv_sec, tv.tv_usec));

                break;
        }

	case SO_SNDBUF:
	case SO_RCVBUF:
	case SO_BROADCAST:
	case SO_DEBUG:
	case SO_KEEPALIVE:
	case SO_DONTROUTE:
	case SO_OOBINLINE:
	case SO_TYPE:
        case SO_ERROR:
        case SO_SNDLOWAT:
        case SO_RCVLOWAT:
        case SO_USELOOPBACK:
            	if(optlen == NULL || *optlen < sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                ret = getsockopt(s, level, optname, (char *)optval, optlen);
                dprintf(("getsockopt %s returned %d", debugsockopt(optname), *(int *)optval));
		break;

	case SO_ACCEPTCONN:
            	if(optlen == NULL || *optlen < sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                size = sizeof(options);
                ret = getsockopt(s, SOL_SOCKET, SO_OPTIONS, (char *)&options, &size);
                if(ret != SOCKET_ERROR) {
                   	*(BOOL *)optval = (options & SO_ACCEPTCONN) == SO_ACCEPTCONN;
                   	*optlen = sizeof(BOOL);
                        dprintf(("SO_ACCEPTCONN returned %d", *(BOOL *)optval));
                }
		break;
	default:
		dprintf(("getsockopt: unknown option %x", optname));
            	WSASetLastError(WSAENOPROTOOPT);
            	return SOCKET_ERROR;
        }
   }
   else
   if(level == IPPROTO_TCP) {
       	if(optname == TCP_NODELAY) {
            	if(optlen == NULL || *optlen < sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
               	ret = getsockopt(s, level, optname, (char *)optval, optlen);
        }
	else {
		dprintf(("getsockopt: unknown option %x", optname));
       		WSASetLastError(WSAENOPROTOOPT);
      		return SOCKET_ERROR;
        }
   }
   else
   if(level == IPPROTO_IP) {
       switch (optname)
       {
           case IP_MULTICAST_IF:
           case WS2_IPPROTO_OPT(IP_MULTICAST_IF_WS2):
           {
               if (*optlen < sizeof(in_addr))
               {
                   dprintf(("IPPROTO_IP, IP_MULTICAST_IP, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               //TODO convert common interface names!
               ret = getsockopt(s, IPPROTO_IP, IP_MULTICAST_IF_OS2, (char *)optval, optlen);
               break;
           }

           case IP_ADD_MEMBERSHIP:
           case WS2_IPPROTO_OPT(IP_ADD_MEMBERSHIP_WS2):
               if (*optlen < sizeof(struct ip_mreq))
               {
                   dprintf(("IPPROTO_IP, IP_ADD_MEMBERSHIP, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               ret = getsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP_OS2, (char *)optval, optlen);
               break;

           case IP_DROP_MEMBERSHIP:
           case WS2_IPPROTO_OPT(IP_DROP_MEMBERSHIP_WS2):
               if (*optlen < sizeof(struct ip_mreq))
               {
                   dprintf(("IPPROTO_IP, IP_DROP_MEMBERSHIP, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               ret = getsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP_OS2, (char *)optval, optlen);
               break;

           case IP_MULTICAST_LOOP_WS2: //for buggy applications that intended to call ws_32.getsockopt
           case IP_MULTICAST_LOOP:
           case WS2_IPPROTO_OPT(IP_MULTICAST_LOOP_WS2):
           {
               if (*optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_MULTICAST_LOOP/IP_MULTICAST_TTL, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               memset(optval, 0, *optlen);
               ret = getsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP_OS2, (char *)optval, optlen);
               break;
           }

           case IP_MULTICAST_TTL:
           case WS2_IPPROTO_OPT(IP_MULTICAST_TTL_WS2):
               if (*optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_MULTICAST_TTL, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               memset(optval, 0, *optlen);
               ret = getsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL_OS2, (char *)optval, optlen);
               dprintf(("getsockopt IP_MULTICAST_TTL_OS2 returned %d, size %d", (int)*optval, *optlen));
               break;

           case IP_TTL:
           case WS2_IPPROTO_OPT(IP_TTL_WS2):
               if (*optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_TTL_WS2, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               memset(optval, 0, *optlen);
               ret = getsockopt(s, IPPROTO_IP, IP_TTL_OS2, (char *)optval, optlen);
               break;

           case IP_TOS:
           case WS2_IPPROTO_OPT(IP_TOS_WS2):
               if (*optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_TOS_WS2, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               ret = getsockopt(s, IPPROTO_IP, IP_TOS_OS2, (char *)optval, optlen);
               break;

           case WS2_IPPROTO_OPT(IP_HDRINCL_WS2):
               if (*optlen < sizeof(u_int))
               {
                   dprintf(("IPPROTO_IP, IP_HDRINCL_WS2, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               ret = getsockopt(s, IPPROTO_IP, IP_HDRINCL_OS2, (char *)optval, optlen);
               if(ret == 0) {
                   ret = (ret != FALSE) ? TRUE : FALSE;
               }
               break;

           case IP_DONTFRAGMENT:
           case WS2_IPPROTO_OPT(IP_DONTFRAGMENT_WS2):
               //MSDN says these options are silently ignored
               if (*optlen < sizeof(u_int))
               {
                   dprintf(("IP_DONTFRAGMENT, IP_DONTFRAGMENT_WS2, optlen too small"));
                   WSASetLastError(WSAEFAULT);
                   return SOCKET_ERROR;
               }
               dprintf(("IPPROTO_IP: IP_DONTFRAGMENT ignored"));
               *optlen = sizeof(u_int);
               *(int *)optval = 0;
               ret = 0;
               break;


           default:
		dprintf(("getsockopt: IPPROTO_IP, unknown option %x", optname));
       		WSASetLastError(WSAENOPROTOOPT);
      		return SOCKET_ERROR;
       }
   }
   else {
       	WSASetLastError(WSAEINVAL);
	return SOCKET_ERROR;
   }

   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);
   return ret;
}
//******************************************************************************
//******************************************************************************
/* Database function prototypes */
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(int,OS2gethostname,
              char *,name,
              int,namelen)
{
 int ret;

   ret = gethostname(name, namelen);
   if(ret == NULL) {
        dprintf(("gethostname returned %s", name));
   	WSASetLastError(NO_ERROR);
	return 0;
   }
   WSASetLastError((errno == EINVAL) ? WSAEFAULT : wsaErrno());
   return SOCKET_ERROR;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(ws_hostent *,OS2gethostbyaddr,
              const char *,addr,
              int,len,
              int,type)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	struct hostent*	host;
	if( (host = gethostbyaddr((char *)addr, len, type)) != NULL ) {
	    	if( WS_dup_he(pwsi, host) ) {
			WSASetLastError(NO_ERROR);
			return pwsi->he;
		}
		else 	WSASetLastError(WSAENOBUFS);
	}
	else 	WSASetLastError((h_errno < 0) ? wsaErrno() : wsaHerrno());
    }
    else WSASetLastError(WSANOTINITIALISED);
    return NULL;
}
//******************************************************************************
//NOTE: This function can possibly block for a very long time
//      I.e. start ISDNPM without dialing in. gethostbyname will query
//      each name server and retry several times (60+ seconds per name server)
//
static struct ws_hostent localhost;
static DWORD  localhost_address;
static DWORD  localhost_addrlist[2] = {(DWORD)&localhost_address, 0};
static DWORD  localhost_aliaslist[1] = {0};
//******************************************************************************
ODINFUNCTION1(ws_hostent *,OS2gethostbyname,
              const char *,name)
{
  LPWSINFO pwsi = WINSOCK_GetIData();

  if( pwsi )
  {
    struct hostent* host;
    char            localhostname[256];

    dprintf(("gethostbyname %s", name));
    if (!gethostname(localhostname, sizeof(localhostname)))
    {
        /*
         * This is a fast non-blocking path for the hostname of this machine.
         * It's probably not 100% correct though..
         */
        if (!strcmp(name, localhostname))
        {
            /*
             * Lookup lan0 address, and move on to lo address if that fails.
             */
            sock_init();                /* ??? */
            int s = socket(PF_INET, SOCK_STREAM, 0);
            if (s >= 0)
            {
                struct ifreq  ifr = {0};
                strcpy(ifr.ifr_name, "lan0");
                int rc = ioctl(s, SIOCGIFADDR, (char*)&ifr, sizeof(ifr));
                if (rc == -1)
                {
                    strcpy(ifr.ifr_name, "lo");
                    rc = ioctl(s, SIOCGIFADDR, (char*)&ifr, sizeof(ifr));
                }
                soclose(s);

                if (rc != -1)
                {
                    /* ASSUMES: family is AF_INET */
                    /* Doesn't work with aliases on lan0. */
                    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;

                    localhost_address       = *(unsigned long *)&addr->sin_addr;
                    localhost.h_name        = (char *)"localhost"; /* This is what the old workaround did. */
                    localhost.h_addrtype    = AF_INET;
                    localhost.h_length      = 4;
                    localhost.h_addr_list   = (char **)&localhost_addrlist[0];
                    localhost.h_aliases     = (char **)&localhost_aliaslist[0];
                    WSASetLastError(NO_ERROR);
                    return &localhost;
                }
            }

            /*
             * bail out..
             */
            strcpy(localhostname, "localhost");
            name = localhostname;
            dprintf(("using localhost"));
        }
    }

    host = gethostbyname( (char*) name);
    
    if( host != NULL )
    {
      if( WS_dup_he(pwsi, host) )
      {
        WSASetLastError(NO_ERROR);
        return pwsi->he;
      }
      else
        WSASetLastError(WSAENOBUFS);
    }
    else
      WSASetLastError((h_errno < 0) ? wsaErrno() : wsaHerrno());
  }
  else
    WSASetLastError(WSANOTINITIALISED);

  return NULL;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(struct ws_servent *,OS2getservbyport,
              int,              port,
              const char *,     proto)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	struct servent* serv;
	if( (serv = getservbyport(port, (char *)proto)) != NULL ) {
		if( WS_dup_se(pwsi, serv) ) {
			WSASetLastError(NO_ERROR);
		    	return pwsi->se;
		}
		else 	WSASetLastError(WSAENOBUFS);
	}
	else 	WSASetLastError(WSANO_DATA);
    }
    else WSASetLastError(WSANOTINITIALISED);
    return NULL;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(struct ws_servent *,OS2getservbyname,
              const char *,     name,
              const char *,     proto)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	struct servent *serv;
	if( (serv = getservbyname((char *)name, (char *)proto)) != NULL ) {
		if( WS_dup_se(pwsi, serv) ) {
			WSASetLastError(NO_ERROR);
		    	return pwsi->se;
		}
		else 	WSASetLastError(WSAENOBUFS);
    	}
   	else 	WSASetLastError(WSANO_DATA);
    }
    else WSASetLastError(WSANOTINITIALISED);
    return NULL;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(struct ws_protoent *,OS2getprotobynumber,
              int,number)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	struct protoent* proto;
	if( (proto = getprotobynumber(number)) != NULL ) {
	    	if( WS_dup_pe(pwsi, proto) ) {
			WSASetLastError(NO_ERROR);
			return pwsi->pe;
		}
	    	else 	WSASetLastError(WSAENOBUFS);
	}
	else 	WSASetLastError(WSANO_DATA);
    }
    else WSASetLastError(WSANOTINITIALISED);
    return NULL;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(struct ws_protoent *,OS2getprotobyname,
              const char *,name)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	struct protoent * proto;
	if( (proto = getprotobyname((char *)name)) != NULL ) {
	    	if(WS_dup_pe(pwsi, proto)) {
			WSASetLastError(NO_ERROR);
			return pwsi->pe;
	    	}
	    	else 	WSASetLastError(WSAENOBUFS);
	}
	else 	WSASetLastError((h_errno < 0) ? wsaErrno() : wsaHerrno());
    }
    else WSASetLastError(WSANOTINITIALISED);
    return NULL;
}
//******************************************************************************
//******************************************************************************
