/* $Id: asyncthread.cpp,v 1.7 2001-04-28 16:15:18 sandervl Exp $ */

/*
 * Async thread help functions
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: Not everything is 100% thread safe  (i.e. async parameter updates)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#include <os2wrap.h>
#include <os2sel.h>
#include <stdlib.h>
#include <wprocess.h>
#include <win32api.h>
#include <misc.h>
#include <string.h>
#include <vmutex.h>
#include "wsock32.h"
#include "asyncthread.h"

static PASYNCTHREADPARM threadList = NULL;
static VMutex           asyncThreadMutex;

static void AddToQueue(PASYNCTHREADPARM pThreadParm);

//******************************************************************************
//******************************************************************************
static void _Optlink AsyncThread(void *arg)
{
 PASYNCTHREADPARM pThreadParm = (PASYNCTHREADPARM)arg;

  pThreadParm->asyncProc((PVOID)arg);

//only for blocking hooks (currently not implemented)
////  if(pThreadParm->request == ASYNC_BLOCKHOOK)
////	WSASetBlocking(FALSE, pThreadParm->hThread);

  free((PVOID)pThreadParm);

  DosExit(EXIT_THREAD, 0);
}
//******************************************************************************
//******************************************************************************
ULONG QueueAsyncJob(ASYNCTHREADPROC asyncproc, PASYNCTHREADPARM pThreadParm, BOOL fSetBlocking)
{
 TID tid;

   pThreadParm->asyncProc         = asyncproc;
   pThreadParm->fActive           = TRUE;
   pThreadParm->fCancelled        = FALSE;
   pThreadParm->next              = NULL;
   pThreadParm->hAsyncTaskHandle  = 0;

   pThreadParm->hThread           = GetCurrentThread();
   AddToQueue(pThreadParm);

   if(fSetBlocking) WSASetBlocking(TRUE);

   tid = _beginthread(AsyncThread, NULL, 16384, (PVOID)pThreadParm);
   if (tid == -1) {
   	dprintf(("QueueAsyncJob: _beginthread failed"));
   	if(fSetBlocking) WSASetBlocking(FALSE);
	RemoveFromQueue(pThreadParm);
   	WSASetLastError(WSAEFAULT);
	return 0;
   }
   pThreadParm->hAsyncTaskHandle = tid;
   WSASetLastError(NO_ERROR);
   return pThreadParm->hAsyncTaskHandle;
}
//******************************************************************************
//******************************************************************************
void AddToQueue(PASYNCTHREADPARM pThreadParm)
{
   asyncThreadMutex.enter();
   pThreadParm->next = threadList;
   threadList        = pThreadParm;
   asyncThreadMutex.leave();
}
//******************************************************************************
//******************************************************************************
void RemoveFromQueue(PASYNCTHREADPARM pThreadParm)
{
 PASYNCTHREADPARM pThreadInfo;

   asyncThreadMutex.enter();
   pThreadInfo = threadList;

   if(pThreadInfo == pThreadParm) {
	threadList = pThreadParm->next;
   }
   else {
	while(pThreadInfo->next) {
		if(pThreadInfo->next == pThreadParm) {
			pThreadInfo->next = pThreadParm->next;
			break;
		}
		pThreadInfo = pThreadInfo->next;
	}
	if(pThreadInfo == NULL) {
		dprintf(("RemoveFromQueue: parm %x not found!!", pThreadParm));
		DebugInt3();
	}
   }   
   memset(pThreadParm, 0, sizeof(*pThreadParm));
   asyncThreadMutex.leave();
}
//******************************************************************************
//******************************************************************************
int WIN32API WSACancelAsyncRequest(LHANDLE hAsyncTaskHandle)
{
 PASYNCTHREADPARM pThreadInfo;
 BOOL             found = FALSE;

   dprintf(("WSACancelAsyncRequest: cancel task %x", hAsyncTaskHandle));
   asyncThreadMutex.enter();
   pThreadInfo = threadList;

   while(pThreadInfo) {
	if(pThreadInfo->hAsyncTaskHandle == hAsyncTaskHandle) {
		pThreadInfo->fCancelled = TRUE;
		found = TRUE;
		break;
	}
	pThreadInfo = pThreadInfo->next;
   }
   asyncThreadMutex.leave();
   if(found == FALSE) {
      	WSASetLastError(WSAEINVAL);
	dprintf(("WSACancelAsyncRequest: task not found!!"));
   }
   return (found) ? NO_ERROR : SOCKET_ERROR;
}
//******************************************************************************
//Only to cancel blocking hooks
//******************************************************************************
int WIN32API WSACancelBlockingCall()
{
 HANDLE hThread = GetCurrentThread();

   dprintf(("WSACancelBlockingCall"));
#if 0
   asyncThreadMutex.enter();
   pThreadInfo = threadList;

   while(pThreadInfo) {
	if(pThreadInfo->hThread == hThread) {
		pThreadInfo->fCancelled = TRUE;

		if(pThreadInfo->request == ASYNC_BLOCKHOOK) {
            		ret = so_cancel(pThreadInfo->blockedsocket);
		}

		found = TRUE;
		break;
	}
	pThreadInfo = pThreadInfo->next;
   }
   asyncThreadMutex.leave();
#endif
   return SOCKET_ERROR;
}
//******************************************************************************
//dump queue
//******************************************************************************
void DumpQueue(void)
{
 HANDLE hThread = GetCurrentThread();
 PASYNCTHREADPARM pThreadInfo;

   dprintf(("DumpQueue"));

   asyncThreadMutex.enter();
   pThreadInfo = threadList;

   while(pThreadInfo) {
	dprintf(( "SOCKET %08xh thread#%d events %xh pending %xh, select %d", 
		pThreadInfo->u.asyncselect.s,
		pThreadInfo->hAsyncTaskHandle,
		pThreadInfo->u.asyncselect.lEvents,
		pThreadInfo->u.asyncselect.lEventsPending,
		pThreadInfo->fWaitSelect));
	pThreadInfo = pThreadInfo->next;
   }
   asyncThreadMutex.leave();
   dprintf(("DumpQueue done"));
}
//******************************************************************************
//Assumes caller owns async thread mutex!
//******************************************************************************
static PASYNCTHREADPARM FindAsyncEvent(SOCKET s)
{
 PASYNCTHREADPARM pThreadInfo;

   pThreadInfo = threadList;
   while(pThreadInfo) {
	if(pThreadInfo->u.asyncselect.s == s && !pThreadInfo->fRemoved) {
		return pThreadInfo;
	}
	pThreadInfo = pThreadInfo->next;
   }
   return NULL;
}
//******************************************************************************
//******************************************************************************
BOOL FindAndSetAsyncEvent(SOCKET s, HWND hwnd, int msg, ULONG lEvent)
{
 PASYNCTHREADPARM pThreadInfo;
 
   asyncThreadMutex.enter();
   pThreadInfo = FindAsyncEvent(s);
   if(pThreadInfo) {
	pThreadInfo->u.asyncselect.lEvents        = lEvent;
        pThreadInfo->u.asyncselect.lEventsPending = lEvent;
	pThreadInfo->hwnd                  = hwnd;
	pThreadInfo->msg                   = msg;
	if(lEvent == 0) {
		//make sure this thread isn't used anymore
		pThreadInfo->fRemoved = TRUE;
	}
	if(pThreadInfo->fWaitSelect) {
		//cancel pending select in async select thread (if any)
		so_cancel(s);
	}

	//unblock async thread if it was waiting
	pThreadInfo->u.asyncselect.asyncSem->post();
   }
   asyncThreadMutex.leave();
   return(pThreadInfo != NULL);
}
//******************************************************************************
//******************************************************************************
void EnableAsyncEvent(SOCKET s, ULONG flags)
{
 PASYNCTHREADPARM pThreadInfo;

   asyncThreadMutex.enter();
   pThreadInfo = FindAsyncEvent(s);
   if(pThreadInfo) {
	pThreadInfo->u.asyncselect.lEventsPending |= (pThreadInfo->u.asyncselect.lEvents & flags);
	//unblock async thread if it was waiting
	pThreadInfo->u.asyncselect.asyncSem->post();

	//cancel pending select in async select thread (if any)
	if(pThreadInfo->fWaitSelect) {
		so_cancel(s);
	}
   }
   asyncThreadMutex.leave();
}
//******************************************************************************
//******************************************************************************
BOOL QueryAsyncEvent(SOCKET s, HWND *pHwnd, int *pMsg, ULONG *plEvent)
{
 PASYNCTHREADPARM pThreadInfo;

   asyncThreadMutex.enter();
   pThreadInfo = FindAsyncEvent(s);
   if(pThreadInfo) {
	*pHwnd   = pThreadInfo->hwnd;
	*pMsg    = pThreadInfo->msg;
	*plEvent = pThreadInfo->u.asyncselect.lEvents;
   }
   asyncThreadMutex.leave();
   return(pThreadInfo != NULL);
}
//******************************************************************************
//******************************************************************************
