/* $Id: asyncapi.cpp,v 1.12 2001-07-07 17:53:15 achimha Exp $ */

/*
 *
 * Win32 SOCK32 for OS/2 (Async apis)
 *
 * Copyright (C) 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Parts based on Wine code: (dlls\winsock\async.c)
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 * (C) 1999 Marcus Meissner
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#include <os2wrap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <misc.h>
#include <win32api.h>
#include "wsock32.h"
#include "asyncthread.h"
#include "wsastruct.h"

#define DBG_LOCALLOG	DBG_async
#include "dbglocal.h"

//kso: dirty fix to make this compile! not permanent!
BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER *p);
#define LowPart u.LowPart


ODINDEBUGCHANNEL(WSOCK32-ASYNC)

//******************************************************************************
//NOTE: Must execute RemoveFromQueue before returning
//******************************************************************************
void ASYNCCNV WSAsyncThreadProc(void *pparm)
{
 PASYNCTHREADPARM pThreadParm = (PASYNCTHREADPARM)pparm;
 LPARAM           lParam;
 int              size = 0;
 int              fail = NO_ERROR;

   if(!pThreadParm->fCancelled)
   {
	switch(pThreadParm->request)
	{
	case ASYNC_GETHOSTBYNAME:
	case ASYNC_GETHOSTBYADDR:
	{
	  struct hostent  *ret;

		if(pThreadParm->request == ASYNC_GETHOSTBYNAME) {
	      		ret = gethostbyname(pThreadParm->u.gethostbyname.name);

   			free(pThreadParm->u.gethostbyname.name);
			pThreadParm->u.gethostbyname.name = 0;
		}
		else {
			ret = gethostbyaddr(pThreadParm->u.gethostbyaddr.addr,
                                            pThreadParm->u.gethostbyaddr.len,
                                            pThreadParm->u.gethostbyaddr.type);

   			free(pThreadParm->u.gethostbyaddr.addr);
			pThreadParm->u.gethostbyaddr.addr = 0;
		}
	      	if (ret != NULL) {
			size = WS_copy_he((struct ws_hostent *)pThreadParm->buf,
	                                  pThreadParm->buf,
	                                  pThreadParm->buflen,
	                                  ret);
			if(size < 0) {
	            		fail = WSAENOBUFS;
				size = -size;
	         	}
	      	}
	      	else  	fail = wsaHerrno();
		break;
	}

	case ASYNC_GETPROTOBYNAME:
	case ASYNC_GETPROTOBYNUMBER:
	{
	  struct protoent *ret;

		if(pThreadParm->request == ASYNC_GETPROTOBYNAME) {
	      		ret = getprotobyname(pThreadParm->u.getprotobyname.name);

			free(pThreadParm->u.getprotobyname.name);
			pThreadParm->u.getprotobyname.name = 0;
		}
      		else	ret = getprotobynumber(pThreadParm->u.getprotobynumber.number);

	      	if (ret != NULL) {
			size = WS_copy_pe((struct ws_protoent *)pThreadParm->buf,
	                                  pThreadParm->buf,
	                                  pThreadParm->buflen,
	                                  ret);
			if(size < 0) {
	            		fail = WSAENOBUFS;
				size = -size;
	         	}
	      	}
	      	else  	fail = WSANO_DATA;

		break;
	}

	case ASYNC_GETSERVBYNAME:
	case ASYNC_GETSERVBYPORT:
	{
	  struct servent  *ret;

		if(pThreadParm->request == ASYNC_GETSERVBYNAME) {
	      		ret = getservbyname(pThreadParm->u.getservbyname.name, pThreadParm->u.getservbyname.proto);

   			free(pThreadParm->u.getservbyname.name);
			pThreadParm->u.getservbyname.name = 0;

	   		if(pThreadParm->u.getservbyname.proto) {
				free(pThreadParm->u.getservbyname.proto);
				pThreadParm->u.getservbyname.proto = 0;
   			}

		}
		else {
			ret = getservbyport(pThreadParm->u.getservbyport.port, pThreadParm->u.getservbyport.proto);
	   		if(pThreadParm->u.getservbyport.proto) {
				free(pThreadParm->u.getservbyport.proto);
				pThreadParm->u.getservbyport.proto = 0;
   			}
		}

	      	if (ret != NULL) {
			size = WS_copy_se((struct ws_servent *)pThreadParm->buf,
	                                  pThreadParm->buf,
	                                  pThreadParm->buflen,
        	                          ret);
			if(size < 0) {
	            		fail = WSAENOBUFS;
				size = -size;
	         	}
	      	}
	      	else  	fail = WSANO_DATA;
		break;
	}
	}
   }
   lParam = (fail << 16) | size;

   if(!pThreadParm->fCancelled) {
   	dprintf(("WSAsyncThreadProc %x %x %x %x", pThreadParm->notifyHandle, pThreadParm->notifyData, pThreadParm->hAsyncTaskHandle, lParam));
  	PostMessageA((HWND)pThreadParm->notifyHandle, (DWORD)pThreadParm->notifyData,
                     (WPARAM)pThreadParm->hAsyncTaskHandle, lParam);
   }
   pThreadParm->fActive = FALSE;
   RemoveFromQueue(pThreadParm);
}
//******************************************************************************
//******************************************************************************
LHANDLE WSAAsyncRequest(AsyncRequestType requesttype, HWND hwnd, int msg, char *buf,
                        int buflen, PVOID param1, PVOID param2, PVOID param3,
                        PVOID param4)
{
  PASYNCTHREADPARM pThreadParm;
  LHANDLE          hAsyncRequest = 0;
  LPSTR            tempname  = 0;
  LPSTR            tempproto = 0;
  LPSTR            tempaddr  = 0;

   if(!fWSAInitialized)
   {
      	WSASetLastError(WSANOTINITIALISED);
   }
   else
   if(WSAIsBlocking())
   {
        WSASetLastError(WSAEINPROGRESS);      	// blocking call in progress
   }
   else
   if(!IsWindow(hwnd))
   {
        WSASetLastError(WSAEINVAL);           	// invalid parameter
   }
   else
   if(buf == NULL || buflen == 0) {
        WSASetLastError(WSAENOBUFS);         	// invalid parameter
   }
   else
   {
	pThreadParm = (PASYNCTHREADPARM)malloc(sizeof(ASYNCTHREADPARM));
	if(pThreadParm == NULL) {
		dprintf(("WSAAsyncRequest: malloc failure!"));
		DebugInt3();
		WSASetLastError(WSAEFAULT);
		return 0;
	}
	memset(pThreadParm, 0, sizeof(*pThreadParm));
	pThreadParm->request      = requesttype;
        pThreadParm->notifyHandle = (int)hwnd;
        pThreadParm->notifyData   = (int)msg;
        pThreadParm->buf          = buf;
        pThreadParm->buflen       = buflen;

	switch(requesttype) {
	case ASYNC_GETHOSTBYNAME:
		tempname = (LPSTR)malloc(strlen((char *)param1)+1);
        	if(tempname == NULL)
		{
			dprintf(("WSAAsyncGetHostByName: malloc failure!"));
			DebugInt3();
			WSASetLastError(WSAEFAULT);
			return 0;
		}
		pThreadParm->u.gethostbyname.name   = tempname;
		strcpy(pThreadParm->u.gethostbyname.name, (char *)param1);
		break;

	case ASYNC_GETHOSTBYADDR:
		tempaddr = (LPSTR)malloc(strlen((char *)param1)+1);
	        if(tempaddr == NULL)
		{
			dprintf(("WSAAsyncGetHostByAddr: malloc failure!"));
			DebugInt3();
			WSASetLastError(WSAEFAULT);
			return 0;
		}

        	pThreadParm->u.gethostbyaddr.len  = (int)param2;
	        pThreadParm->u.gethostbyaddr.type = (int)param3;
		pThreadParm->u.gethostbyaddr.addr = tempaddr;
		strcpy(pThreadParm->u.gethostbyaddr.addr, (char *)param1);
		break;

	case ASYNC_GETPROTOBYNAME:
		tempname = (LPSTR)malloc(strlen((char *)param1)+1);
	        if(tempname == NULL)
		{
			dprintf(("WSAAsyncGetProtoByName: malloc failure!"));
			DebugInt3();
			WSASetLastError(WSAEFAULT);
			return 0;
		}
		pThreadParm->u.getprotobyname.name = tempname;
		strcpy(pThreadParm->u.getprotobyname.name, (char *)param1);
		break;

	case ASYNC_GETPROTOBYNUMBER:
		pThreadParm->u.getprotobynumber.number = (int)param1;
		break;

	case ASYNC_GETSERVBYNAME:
		tempname    = (LPSTR)malloc(strlen((char *)param1)+1);
		tempproto   = NULL;
		if(param2) {
			tempproto = (LPSTR)malloc(strlen((char *)param2)+1);
		}
	        if(tempname == NULL || (param2 && !tempproto))
		{
			dprintf(("WSAAsyncGetServByName: malloc failure!"));
			DebugInt3();
			WSASetLastError(WSAEFAULT);
			return 0;
		}
		pThreadParm->u.getservbyname.name   = tempname;
		strcpy(pThreadParm->u.getservbyname.name, (char *)param1);
		pThreadParm->u.getservbyname.proto   = tempproto;
		if(param2)
			strcpy(pThreadParm->u.getservbyname.proto, (char *)param2);
		break;

	case ASYNC_GETSERVBYPORT:
		if(param2) {
			tempproto = (LPSTR)malloc(strlen((char *)param2)+1);
		}
	        if(param2 && !tempproto)
		{
			dprintf(("WSAAsyncGetServByPort: malloc failure!"));
			DebugInt3();
			WSASetLastError(WSAEFAULT);
			return 0;
		}
		pThreadParm->u.getservbyport.port   = (int)param1;
		pThreadParm->u.getservbyport.proto  = tempproto;
		if(param2)
			strcpy(pThreadParm->u.getservbyport.proto, (char *)param2);
		break;
	}
   	hAsyncRequest = (LHANDLE)QueueAsyncJob(WSAsyncThreadProc, pThreadParm);
	if(hAsyncRequest == 0) {
		free(pThreadParm);
		if(tempname)	free(tempname);
		if(tempaddr)	free(tempaddr);
		if(tempproto)	free(tempproto);

		dprintf(("WSAAsyncRequest: QueueAsyncJob failure!"));
		DebugInt3();
		WSASetLastError(WSAEFAULT);
	 	return 0;
	}
	return hAsyncRequest;
   }
   return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LHANDLE,WSAAsyncGetHostByName,
              HWND,hWnd,
              u_int,wMsg,
              const char *,name,
              char *,buf,
              int,buflen)
{
   dprintf(("WSAAsyncGetHostByName %s", name));

   return WSAAsyncRequest(ASYNC_GETHOSTBYNAME, hWnd, wMsg, buf, buflen,
                          (PVOID)name, 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION7(LHANDLE,WSAAsyncGetHostByAddr,
              HWND,hWnd,
              u_int,wMsg,
              const char *,addr,
              int,len,
              int,type,
              char *,buf,
              int,buflen)
{
   dprintf(("WSAAsyncGetHostByAddr %s", addr));

   return WSAAsyncRequest(ASYNC_GETHOSTBYADDR, hWnd, wMsg, buf, buflen,
                          (PVOID)addr, (PVOID)len, (PVOID)type, 0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LHANDLE,WSAAsyncGetProtoByName,
              HWND,hWnd,
              u_int,wMsg,
              const char *,name,
              char *,buf,
              int,buflen)
{
   dprintf(("WSAAsyncGetProtoByName %s", name));

   return WSAAsyncRequest(ASYNC_GETPROTOBYNAME, hWnd, wMsg, buf, buflen,
                          (PVOID)name, 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LHANDLE,WSAAsyncGetProtoByNumber,
              HWND,hWnd,
              u_int,wMsg,
              int,number,
              char *,buf,
              int,buflen)
{
   dprintf(("WSAAsyncGetProtoByNumber %d", number));

   return WSAAsyncRequest(ASYNC_GETPROTOBYNUMBER, hWnd, wMsg, buf, buflen,
                          (PVOID)number, 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(LHANDLE,WSAAsyncGetServByName,
              HWND,hWnd,
              u_int,wMsg,
              const char *,name,
              const char *,proto,
              char *,buf,
              int,buflen)
{
   dprintf(("WSAAsyncGetServByName %s", name));

   return WSAAsyncRequest(ASYNC_GETSERVBYNAME, hWnd, wMsg, buf, buflen,
                          (PVOID)name, (PVOID)proto, 0, 0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(LHANDLE,WSAAsyncGetServByPort,
              HWND,hWnd,
              u_int,wMsg,
              int,port,
              const char *,proto,
              char *,buf,
              int,buflen)
{
   dprintf(("WSAAsyncGetServByPort %d %s", port, proto));

   return WSAAsyncRequest(ASYNC_GETSERVBYPORT, hWnd, wMsg, buf, buflen,
                          (PVOID)port, (PVOID)proto, 0, 0);
}
//******************************************************************************
//******************************************************************************
void AsyncSelectNotifyEvent(PASYNCTHREADPARM pThreadParm, ULONG event, ULONG socket_error)
{
    pThreadParm->u.asyncselect.lEventsPending &= ~event;

    event = WSAMAKESELECTREPLY(event, socket_error);

    if (pThreadParm->u.asyncselect.mode == WSA_SELECT_HWND)
    {
        dprintf(("AsyncSelectNotifyEvent: notifying window %x %x %x %x", pThreadParm->u.asyncselect.s, pThreadParm->notifyHandle, pThreadParm->notifyData, event));
        PostMessageA((HWND)pThreadParm->notifyHandle, (DWORD)pThreadParm->notifyData, (WPARAM)pThreadParm->u.asyncselect.s,
                     (LPARAM)event);
    }
    else
    if (pThreadParm->u.asyncselect.mode == WSA_SELECT_HEVENT)
    {
        dprintf(("AsyncSelectNotifyEvent: notifying event semaphore %x %x %x %x", pThreadParm->u.asyncselect.s, pThreadParm->notifyHandle, pThreadParm->notifyData, event));
        SetEvent(pThreadParm->notifyHandle);
    }
    else
    {
        dprintf(("AsyncSelectNotifyEvent: error, unknown mode"));
    }
}
//******************************************************************************
#define  nr(i) 		((i != -1) ? 1 : 0)
#define  ready(i)       ((i != -1) && (sockets[i] != -1))
//NOTE: Must execute RemoveFromQueue before returning
//******************************************************************************
void ASYNCCNV WSAsyncSelectThreadProc(void *pparm)
{
 PASYNCTHREADPARM pThreadParm = (PASYNCTHREADPARM)pparm;
 SOCKET           sockets[3];
 SOCKET           s = pThreadParm->u.asyncselect.s;
 int       	  noread, nowrite, noexcept, state, sockoptlen, sockoptval;
 int              tmp, i, lEventsPending, ret, bytesread;

   while(TRUE)
   {
asyncloopstart:
	i      = 0;
	noread = nowrite = noexcept = -1;

	//break if user cancelled request
      	if(pThreadParm->u.asyncselect.lEvents == 0) {
		break;
      	}

        lEventsPending = pThreadParm->u.asyncselect.lEventsPending;
      	//block if no events are pending
      	if(lEventsPending == 0)
      	{
		dprintf2(("WSAsyncSelectThreadProc: waiting for new events"));
		//wait for events to be enabled
		pThreadParm->u.asyncselect.asyncSem->wait();
		//reset event semaphore
		pThreadParm->u.asyncselect.asyncSem->reset();
         	continue;
      	}

	if(lEventsPending & (FD_READ | FD_CLOSE | FD_ACCEPT)) {
		noread = i++;
		sockets[noread] = s;
	}
	if((lEventsPending & FD_CONNECT) ||
          ((lEventsPending & FD_WRITE) &&
           (!(ioctl(s, FIOBSTATUS, (char *)&tmp, sizeof(tmp)) & SS_CANTSENDMORE))))
        {
		nowrite = i++;
		sockets[nowrite] = s;
	}
	if(lEventsPending & FD_OOB) {
		noexcept = i++;
		sockets[noexcept] = s;
	}

        dprintf2(("WSAsyncSelectThreadProc %x rds=%d, wrs=%d, oos =%d, pending = %x", pThreadParm->u.asyncselect.s, noread, nowrite, noexcept, lEventsPending));

	pThreadParm->fWaitSelect = TRUE;
	ret = select((int *)sockets, nr(noread), nr(nowrite), nr(noexcept), -1);
	pThreadParm->fWaitSelect = FALSE;
	if(ret == SOCKET_ERROR) {
		int selecterr = sock_errno();
        	dprintf2(("WSAsyncSelectThreadProc %x rds=%d, wrs=%d, oos =%d, pending = %x select returned %x", pThreadParm->u.asyncselect.s, noread, nowrite, noexcept, lEventsPending, selecterr));
		if(selecterr && selecterr < SOCBASEERR) {
			selecterr += SOCBASEERR;
		}
		switch(selecterr)
		{
		case SOCEINTR:
////        		state = ioctl(s, FIOBSTATUS, (char *)&tmp, sizeof(tmp));
////			dprintf(("SOCEINTR; state = %x", state));
			goto asyncloopstart;	//so_cancel was called

		case SOCECONNRESET:
		case SOCEPIPE:
       			if(lEventsPending & FD_CLOSE) {
				dprintf(("FD_CLOSE; broken connection"));
       				AsyncSelectNotifyEvent(pThreadParm, FD_CLOSE, WSAECONNRESET);
			}

			//remote connection broken (so can't receive data anymore)
                        //but can still send
       			pThreadParm->u.asyncselect.lEventsPending &= ~(FD_READ | FD_ACCEPT);
			goto asyncloopstart;

		case SOCEINVAL:
       			if(lEventsPending & FD_CLOSE) {
				dprintf(("FD_CLOSE; SOCEINVAL"));
       				AsyncSelectNotifyEvent(pThreadParm, FD_CLOSE, selecterr);
			}
      			break;
		default:
			dprintf(("WSAsyncSelectThreadProc: select SOCKET_ERROR %x", selecterr));
			break; //something bad happened
		}
		break;
	}

      	if(ready(nowrite))
 	{
        	state = ioctl(s, FIOBSTATUS, (char *)&tmp, sizeof(tmp));

         	if(lEventsPending & FD_CONNECT) {
	    		if(state & SS_ISCONNECTED) {
				AsyncSelectNotifyEvent(pThreadParm, FD_CONNECT, NO_ERROR);
	    		}
            		else {
            			sockoptlen = sizeof(int);

            			ret = getsockopt(s, SOL_SOCKET, SO_ERROR,
                	        		 (char *) &sockoptval, &sockoptlen);
				//SvL: WSeB returns SOCECONNREFUSED, Warp 4 0x3d
				if(sockoptval == SOCECONNREFUSED || sockoptval == (SOCECONNREFUSED - SOCBASEERR)) {
					AsyncSelectNotifyEvent(pThreadParm, FD_CONNECT, WSAECONNREFUSED);
				}
            		}
        	}
		else
		if(!(state & SS_CANTSENDMORE) && (lEventsPending & FD_WRITE)) {
			AsyncSelectNotifyEvent(pThreadParm, FD_WRITE, NO_ERROR);
		}
        }

      	if(ready(noread))
      	{
         	state = ioctl(s, FIONREAD, (CHAR *) &bytesread, sizeof(bytesread));
         	if(state == SOCKET_ERROR)
 		{
            		if(lEventsPending & FD_CLOSE)
			{
				dprintf(("FD_CLOSE; ioctl; socket error"));
				AsyncSelectNotifyEvent(pThreadParm, FD_CLOSE, NO_ERROR);
				//remote connection broken (so can't receive data anymore)
                        	//but can still send
       				pThreadParm->u.asyncselect.lEventsPending &= ~(FD_READ | FD_ACCEPT);
				continue;
			}
			else {
				dprintf(("WSAsyncSelectThreadProc: ioctl SOCKET_ERROR!"));
				break; //todo: correct???
			}
		}
         	if(lEventsPending & FD_ACCEPT)
 		{
            		sockoptlen = sizeof(sockoptlen);

            		ret = getsockopt(s, SOL_SOCKET, SO_OPTIONS,
                                         (char *) &sockoptval, &sockoptlen);
		        if(ret == SOCKET_ERROR) {
				dprintf(("WSAsyncSelectThreadProc: getsockopt SOCKET_ERROR!"));
               			break;
			}
			if((sockoptval & SO_ACCEPTCONN) == SO_ACCEPTCONN) {
				AsyncSelectNotifyEvent(pThreadParm, FD_ACCEPT, NO_ERROR);
			}
		}
		if((lEventsPending & FD_READ) && bytesread >= 0) {
			AsyncSelectNotifyEvent(pThreadParm, FD_READ, NO_ERROR);
		}
#if 0
//SvL: This generates FD_CLOSE messages when the connection is just fine
//     (recv executed in another thread when select returns)
       		else
		if((lEventsPending & FD_CLOSE) && (state == 0 && bytesread == 0)) {
			dprintf(("FD_CLOSE; state == 0 && bytesread == 0"));
			AsyncSelectNotifyEvent(pThreadParm, FD_CLOSE, NO_ERROR);
		}
#endif
	}
      	if(ready(noexcept))
      	{
		if(lEventsPending & FD_OOB) {
         		AsyncSelectNotifyEvent(pThreadParm, FD_OOB, NO_ERROR);
		}
      	}
	if((pThreadParm->u.asyncselect.lEventsPending & (FD_ACCEPT|FD_CLOSE|FD_CONNECT)) ==
            (lEventsPending & (FD_ACCEPT|FD_CLOSE|FD_CONNECT))) {
		DosSleep(10);
	}
   }
   //remove it first, then delete semaphore object
   pThreadParm->fActive = FALSE;
   RemoveFromQueue(pThreadParm);
   delete pThreadParm->u.asyncselect.asyncSem;
   pThreadParm->u.asyncselect.asyncSem = 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(int,WSAAsyncSelect,
              SOCKET,s,
              HWND,hWnd,
              u_int,wMsg,
              long,lEvent)
{
   /* just forward call to worker method */
   return WSAAsyncSelectWorker(s, WSA_SELECT_HWND, (int)hWnd, (int)wMsg, lEvent);
}
//******************************************************************************
//******************************************************************************
int WSAAsyncSelectWorker(SOCKET s, int mode, int notifyHandle, int notifyData, long lEventMask)
{
  PASYNCTHREADPARM pThreadParm;
  int              nonblock = 1;
  int              ret;

#ifdef DEBUG
    // log all event bits that are set
    char tmpbuf[300];
    strcpy(tmpbuf, "");
    if (lEventMask & FD_READ)
        strcat(tmpbuf, " FD_READ");
    if (lEventMask & FD_WRITE)
        strcat(tmpbuf, " FD_WRITE");
    if (lEventMask & FD_OOB)
        strcat(tmpbuf, " FD_OOB");    
    if (lEventMask & FD_ACCEPT)
        strcat(tmpbuf, " FD_ACCEPT");
    if (lEventMask & FD_CONNECT)
        strcat(tmpbuf, " FD_CONNECT");
    if (lEventMask & FD_CLOSE)
        strcat(tmpbuf, " FD_CLOSE");
    if (lEventMask & FD_QOS)
        strcat(tmpbuf, " FD_QOS");
    if (lEventMask & FD_GROUP_QOS)
        strcat(tmpbuf, " FD_GROUP_QOS");
    if (lEventMask & FD_ROUTING_INTERFACE_CHANGE)
        strcat(tmpbuf, " FD_ROUTING_INTERFACE_CHANGE");
    if (lEventMask & FD_ADDRESS_LIST_CHANGE)
        strcat(tmpbuf, " FD_ADDRESS_LIST_CHANGE");
    dprintf(("event bits:%s", tmpbuf));
#endif

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
   if((mode == WSA_SELECT_HWND) && (HWND)notifyHandle && !IsWindow((HWND)notifyHandle))
   {
        dprintf(("invalid window handle"));
        WSASetLastError(WSAEINVAL);           	// invalid parameter
	return SOCKET_ERROR;
   }
#if 0
// AH: null sempahore is ok when clearing request
   else
   if ((mode == WSA_SELECT_HEVENT) && !(WSAEVENT)notifyHandle)
   {
        dprintf(("invalid event semaphore handle"));
        WSASetLastError(WSAEINVAL);           	// invalid parameter
	return SOCKET_ERROR;
   }
#endif

   //Set socket to non-blocking mode
   ret = ioctl(s, FIONBIO, (char *) &nonblock, sizeof(nonblock));
   if(ret == SOCKET_ERROR) {
        dprintf(("setting socket to non blocking mode failed"));
 	WSASetLastError(wsaErrno());
	return SOCKET_ERROR;
   }
   if(FindAndSetAsyncEvent(s, mode, notifyHandle, notifyData, lEventMask) == TRUE)
   {
        dprintf(("already got socket, just changing event mask"));
	//found and changed active async event
	WSASetLastError(NO_ERROR);
	return NO_ERROR;
   }
   pThreadParm = (PASYNCTHREADPARM)malloc(sizeof(ASYNCTHREADPARM));
   if(pThreadParm == NULL) {
	dprintf(("WSAAsyncSelect: malloc failure!"));
	DebugInt3();
	WSASetLastError(WSAEFAULT);
 	return SOCKET_ERROR;
   }
   memset(pThreadParm, 0, sizeof(*pThreadParm));
   pThreadParm->request                      = ASYNC_SELECT;
   pThreadParm->notifyHandle                 = notifyHandle;
   pThreadParm->notifyData                   = notifyData;
   pThreadParm->u.asyncselect.mode           = mode;
   pThreadParm->u.asyncselect.lEvents        = lEventMask;
   pThreadParm->u.asyncselect.lEventsPending = lEventMask;
   pThreadParm->u.asyncselect.s              = s;
   pThreadParm->u.asyncselect.asyncSem       = new VSemaphore;
   if(pThreadParm->u.asyncselect.asyncSem == NULL) {
	dprintf(("WSAAsyncSelect: VSemaphore alloc failure!"));
	DebugInt3();
	WSASetLastError(WSAEFAULT);
 	return SOCKET_ERROR;
   }
   if(QueueAsyncJob(WSAsyncSelectThreadProc, pThreadParm) == 0) {
	delete pThreadParm->u.asyncselect.asyncSem;
	free(pThreadParm);
	dprintf(("WSAAsyncSelect: QueueAsyncJob failure!"));
	DebugInt3();
	WSASetLastError(WSAEFAULT);
 	return SOCKET_ERROR;
   }
   WSASetLastError(NO_ERROR);
   return NO_ERROR;
}
//******************************************************************************
//******************************************************************************
int WSAEnumNetworkEventsWorker(SOCKET s, WSAEVENT hEvent, LPWSANETWORKEVENTS lpEvent)
{
   PASYNCTHREADPARM pThreadInfo;

   asyncThreadMutex.enter();
   pThreadInfo = FindAsyncEvent(s);
   if (pThreadInfo)
   {
        // TODO return correct errors!!!
        for (int i = 0; i < FD_MAX_EVENTS; i++)
        {
           lpEvent->iErrorCode[i] = 0;
        }

        lpEvent->lNetworkEvents = pThreadInfo->u.asyncselect.lEventsPending;

        // TODO is this correct? API says they're cleared
        pThreadInfo->u.asyncselect.lEventsPending = 0;
   }
   asyncThreadMutex.leave();
   if (!pThreadInfo)
   {
      dprintf(("no async registration for socket %x", s));
      // TODO: correct behavior?
      WSASetLastError(WSAEINVAL);
      return SOCKET_ERROR;
   }

   // reset event semaphore if present
   if (hEvent)
   {
//      dprintf(("posting event semaphore 0x%x", hEvent));
      ResetEvent(hEvent);
   }

   WSASetLastError(NO_ERROR);
   return NO_ERROR;
}
//******************************************************************************
//******************************************************************************