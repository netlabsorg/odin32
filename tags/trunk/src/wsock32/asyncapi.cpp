/* $Id: asyncapi.cpp,v 1.1 2000-03-22 18:46:14 sandervl Exp $ */

/*
 *
 * Win32 SOCK32 for OS/2 (Async apis)
 *
 * Copyright (C) 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Based on Wine code: (dlls\winsock\async.c)
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
#include "wsastruct.h"

#define DBG_LOCALLOG	DBG_async
#include "dbglocal.h"

ODINDEBUGCHANNEL(WSOCK32-ASYNC)

//******************************************************************************
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
	      	else  	fail = WSANO_DATA;
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
  	PostMessageA(pThreadParm->hwnd, pThreadParm->msg, 
                     (WPARAM)pThreadParm->hAsyncTaskHandle, lParam);
   }
   RemoveFromQueue(pThreadParm); 
}
//******************************************************************************
//******************************************************************************
LHANDLE WSAAsyncRequest(AsyncRequestType requesttype, HWND hwnd, int msg, char *buf, 
                        int buflen, PVOID param1, PVOID param2, PVOID param3,
                        PVOID param4)
{
  PASYNCTHREADPARM pThreadParm;
  LHANDLE          hHost = 0;
  LPSTR            tempname;
  LPSTR            tempproto;
  LPSTR            tempaddr;

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
	pThreadParm->request= requesttype;
        pThreadParm->hwnd   = hwnd;
        pThreadParm->msg    = msg;
        pThreadParm->buf    = buf;
        pThreadParm->buflen = buflen;

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
   	return (LHANDLE)QueueAsyncJob(WSAsyncThreadProc, pThreadParm);
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
ODINFUNCTION4(int,WSAAsyncSelect,
              SOCKET,s,
              HWND,hWnd,
              u_int,wMsg,
              long,lEvent)
{
  return SOCKET_ERROR;
}
//******************************************************************************
//******************************************************************************
