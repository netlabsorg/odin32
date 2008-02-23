/* $Id: asyncthread.h,v 1.14 2003-01-06 13:05:40 sandervl Exp $ */

/*
 * Async thread help functions
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __ASYNCTHREAD_H__
#define __ASYNCTHREAD_H__

#include <vmutex.h>
#include "vsemaphore.h"

#define	ASYNCCNV	_Optlink

typedef void (* ASYNCCNV ASYNCTHREADPROC)(void *);

enum AsyncRequestType {
  ASYNC_GETHOSTBYNAME,
  ASYNC_GETHOSTBYADDR,
  ASYNC_GETPROTOBYNAME,
  ASYNC_GETPROTOBYNUMBER,
  ASYNC_GETSERVBYNAME,
  ASYNC_GETSERVBYPORT,
  ASYNC_SELECT
} ;

typedef struct _ASYNCTHREADPARM
{
	BOOL 		fActive;
	BOOL            fCancelled;
	BOOL            fConnected;
        BOOL            fWaitSelect;       
        BOOL            fRemoved;  //async select
	AsyncRequestType request;
	LHANDLE		hAsyncTaskHandle;
        HANDLE          hThread; //handle of thread that started the async request
	ASYNCTHREADPROC asyncProc;
        VMutex         *parmmutex;
	DWORD 		notifyHandle;
	DWORD		notifyData;
	LPSTR		buf;
	DWORD		buflen;
	union {
		struct {
			LPSTR	name;
		} gethostbyname;
		struct {
			LPSTR	addr;
			int	len;
			int	type;
		} gethostbyaddr;
		struct {
			LPSTR	name;
		} getprotobyname;
		struct {
			int	number;
		} getprotobynumber;
		struct {
			LPSTR	name;
			LPSTR   proto;
		} getservbyname;
		struct {
			int	port;
			LPSTR   proto;
		} getservbyport;
		struct {
			VSemaphore *asyncSem;
			DWORD       lEvents;
                        DWORD       lEventsPending;
			SOCKET      s;
			int         mode;
			// event bits to signal which event has occured
			DWORD       lLastEvent;
			// error codes for all events
			int         iErrorCode[FD_MAX_EVENTS];
		} asyncselect;
        } u;
        _ASYNCTHREADPARM *next;
} ASYNCTHREADPARM, *PASYNCTHREADPARM;

extern VMutex asyncThreadMutex;
PASYNCTHREADPARM FindAsyncEvent(SOCKET s);

ULONG QueueAsyncJob(ASYNCTHREADPROC asyncproc, PASYNCTHREADPARM pThreadParm, BOOL fSetBlocking = FALSE);
void  RemoveFromQueue(PASYNCTHREADPARM pThreadParm);

void  EnableAsyncEvent(SOCKET s, ULONG flags);
BOOL  QueryAsyncEvent(SOCKET s, int *pMode, ULONG *pNofityHandle, ULONG *pNofityData, ULONG *plEvent);
BOOL  FindAndSetAsyncEvent(SOCKET s, int mode, int notifyHandle, int notifyData, ULONG lEventMask);

void  WSACancelAllAsyncRequests();
void  WSAWaitForAllAsyncRequests();

#endif  //__ASYNCTHREAD_H__
