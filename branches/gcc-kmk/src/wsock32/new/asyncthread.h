/* $Id: asyncthread.h,v 1.2 2000-03-23 19:21:54 sandervl Exp $ */

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
#include <vsemaphore.h>

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
	AsyncRequestType request;
	LHANDLE		hAsyncTaskHandle;
        HANDLE          hThread; //handle of thread that started the async request
	ASYNCTHREADPROC asyncProc;
        VMutex         *parmmutex;
	HWND 		hwnd;
	DWORD		msg;
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
		} asyncselect;
        } u;
        _ASYNCTHREADPARM *next;
} ASYNCTHREADPARM, *PASYNCTHREADPARM;

ULONG QueueAsyncJob(ASYNCTHREADPROC asyncproc, PASYNCTHREADPARM pThreadParm, BOOL fSetBlocking = FALSE);
void  RemoveFromQueue(PASYNCTHREADPARM pThreadParm);

void  EnableAsyncEvent(SOCKET s, ULONG flags);
BOOL  QueryAsyncEvent(SOCKET s, HWND *pHwnd, int *pMsg, ULONG *plEvent);
BOOL  FindAndSetAsyncEvent(SOCKET s, HWND hwnd, int msg, ULONG lEvent);

#endif  //__ASYNCTHREAD_H__
