/* $Id: wsock32.cpp,v 1.18 2000-03-23 23:07:23 sandervl Exp $ */

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
#include <win32wnd.h>
#include <wprocess.h>
#include <misc.h>

#include "wsock32.h"
#include "wsastruct.h"
#include "asyncthread.h"

#define DBG_LOCALLOG	DBG_wsock32
#include "dbglocal.h"


ODINDEBUGCHANNEL(WSOCK32-WSOCK32)


/*****************************************************************************
 * Local variables                                                           *
 *****************************************************************************/

static LPWSINFO lpFirstIData = NULL;

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
    LPWSINFO ppid, iData;

    if (iData) {
	ppid = lpFirstIData;
	while(ppid) 
	{
		iData = ppid;
		ppid  = ppid->lpNextIData;

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
}
//******************************************************************************
//******************************************************************************
void WIN32API WSASetLastError(int iError)
{
  // according to the docs, WSASetLastError() is just a call-through
  // to SetLastError()
  if(iError) {
	dprintf(("WSASetLastError %x", iError));
  }
  SetLastError(iError);
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
   ret = soclose(s);
   //Close WSAAsyncSelect thread if one was created for this socket
   FindAndSetAsyncEvent(s, 0, 0, 0);
  
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
   ret = connect(s, (sockaddr *)name, namelen);
   // map BSD error codes
   if(ret == SOCKET_ERROR) {
      	if(sock_errno() == SOCEINPROGRESS) {
         	WSASetLastError(WSAEWOULDBLOCK);
      	} 
	else 
	if (sock_errno() == SOCEOPNOTSUPP) {
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
	HWND  hwnd;
	int   msg;
 	ULONG lEvent;

	if(QueryAsyncEvent(s, &hwnd, &msg, &lEvent) == TRUE) {
		if(*argp != 0) {
			//nothing to do; already non-blocking
			return NO_ERROR;
		}
		else {
			dprintf(("Trying to set socket to blocking mode while async select active -> return error!"));
                  	WSASetLastError(WSAEINVAL);
                  	return SOCKET_ERROR;
		}
	}
   }
   ret = ioctl(s, cmd, (char *)argp, sizeof(int));

   // Map EOPNOTSUPP to EINVAL
   if(ret == SOCKET_ERROR && sock_errno() == SOCEOPNOTSUPP)
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
 int   ret, msg;
 HWND  hwnd;
 ULONG lEvent;

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
	if(QueryAsyncEvent(s, &hwnd, &msg, &lEvent) == TRUE) {
        	if(WSAAsyncSelect(ret, hwnd, msg, lEvent) == SOCKET_ERROR) {
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
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);

   //Reset FD_READ event flagfor  WSAAsyncSelect thread if one was created for this socket
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
   else 
   if(fromlen == NULL || *fromlen < (int)sizeof(struct sockaddr_in)) {
      	WSASetLastError(WSAEFAULT);
      	return SOCKET_ERROR;
   }
   ret = recvfrom(s, buf, len, flags, from, fromlen);

   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);

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

   if(!fWSAInitialized) {
      	WSASetLastError(WSANOTINITIALISED);
      	return SOCKET_ERROR;
   } 
   else 
   if(WSAIsBlocking()) {
      	WSASetLastError(WSAEINPROGRESS);
      	return SOCKET_ERROR;
   } 
   ret = send(s, (char *)buf, len, flags);

   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
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
   if(tolen < (int)sizeof(struct sockaddr_in)) {
      	WSASetLastError(WSAEFAULT);
      	return SOCKET_ERROR;
   }
   ret = sendto(s, (char *)buf, len, flags, (struct sockaddr *)to, tolen);

   if(ret == SOCKET_ERROR) {
 	WSASetLastError(wsaErrno());
   }
   else WSASetLastError(NO_ERROR);

   //Reset FD_WRITE event flagfor  WSAAsyncSelect thread if one was created for this socket
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
      	if(nrread + nrwrite + nrexcept  == 0) {
         	WSASetLastError(WSAEINVAL);
         	return SOCKET_ERROR;
	}
      	if(timeout != NULL && (timeout->tv_sec < 0 || timeout->tv_usec < 0)) {
         	WSASetLastError(WSAEINVAL);
         	return SOCKET_ERROR;
      	}
        if(timeout == NULL) {
            	ttimeout = -1L; // no timeout
        } 
	else    ttimeout = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
	
	sockets = (int *)malloc(sizeof(int) * (nrread+nrwrite+nrexcept));
	if(readfds) {
		memcpy(&sockets[0], readfds->fd_array, nrread * sizeof(SOCKET));
	}
	if(writefds) {
		memcpy(&sockets[nrread], writefds->fd_array, nrwrite * sizeof(SOCKET));
	}
	if(exceptfds) {
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
               		for(i=0;i<nrread;i++) {
                  		if(socktmp[i] != -1) {
	                     		readfds->fd_array[j] = socktmp[i];
				}
	                }
	               	readfds->fd_count = i;
			socktmp += nrread;
	        }

            	if(writefds != NULL) {
               		for(i=0;i<nrwrite;i++) {
                  		if(socktmp[i] != -1) {
	                     		writefds->fd_array[j] = socktmp[i];
				}
	                }
	               	writefds->fd_count = i;
			socktmp += nrwrite;
            	}
            	if(exceptfds != NULL) {
               		for(i=0;i<nrexcept;i++) {
                  		if(socktmp[i] != -1) {
	                     		exceptfds->fd_array[j] = socktmp[i];
				}
	                }
	               	exceptfds->fd_count = i;
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
  int               ret;
  ULONG             size;

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
            	if(optlen < (int)sizeof(ws_linger)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
    		yy = (struct ws_linger *)optval;
		xx.l_onoff  = (optname == SO_DONTLINGER) ? !yy->l_onoff : yy->l_onoff;
		xx.l_linger = yy->l_linger;

		ret = setsockopt(s,level,optname,(char *)&xx, sizeof(xx));
		break;
	case SO_SNDBUF:
	case SO_RCVBUF:
            	if(optlen < (int)sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}

		size = *(ULONG *)optval;
tryagain:
		ret = setsockopt(s,level,optname, (char *)&size, sizeof(ULONG));
		if(ret == SOCKET_ERROR && size > 65535) {
			dprintf(("setsockopt: change size from %d to 65000", size));
			//SvL: Limit send & receive buffer length to 64k
	                //     (only happens with 16 bits tcpip stack?)
			size = 65000;
			goto tryagain;
		}
		break;

	case SO_BROADCAST:
	case SO_DEBUG:
	case SO_KEEPALIVE:
	case SO_DONTROUTE:
	case SO_OOBINLINE:
	case SO_REUSEADDR:
            	if(optlen < (int)sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                ret = setsockopt(s, level, optname, (char *)optval, optlen);
		break;
	default: 
		dprintf(("setsockopt: unknown option %x", optname));
            	WSASetLastError(WSAENOPROTOOPT);
            	return SOCKET_ERROR;
        } 
   }
   else 
   if(level == IPPROTO_TCP) {
       	if(optname == TCP_NODELAY) {
            	if(optlen < (int)sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
               	ret = setsockopt(s, level, optname, (char *)optval, optlen);
        } 
	else {
		dprintf(("setsockopt: unknown option %x", optname));
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

	case SO_SNDBUF:
	case SO_RCVBUF:
	case SO_BROADCAST:
	case SO_DEBUG:
	case SO_KEEPALIVE:
	case SO_DONTROUTE:
	case SO_OOBINLINE:
	case SO_REUSEADDR:
	case SO_TYPE:
            	if(optlen == NULL || *optlen < sizeof(int)) {
               		WSASetLastError(WSAEFAULT);
               		return SOCKET_ERROR;
            	}
                ret = getsockopt(s, level, optname, (char *)optval, optlen);
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
//******************************************************************************
ODINFUNCTION1(ws_hostent *,OS2gethostbyname,
              const char *,name)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	struct hostent*     host;
	if( (host = gethostbyname((char *)name)) != NULL ) {
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
//******************************************************************************
ODINFUNCTION2(struct ws_servent *,OS2getservbyport,
              int,              port,
              const char *,     proto)
{
    LPWSINFO pwsi = WINSOCK_GetIData();

    if( pwsi )
    {
	struct servent* serv;
	if( (serv = getservbyport(port, pwsi->buffer)) != NULL ) {
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
	struct servent*     serv;
	if( (serv = getservbyname(pwsi->buffer, pwsi->buffer)) != NULL ) {
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
