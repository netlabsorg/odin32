/* $Id: asyncthread.cpp,v 1.17 2004-01-30 22:02:12 bird Exp $ */

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
VMutex asyncThreadMutex;

static void AddToQueue(PASYNCTHREADPARM pThreadParm);

//******************************************************************************
//******************************************************************************
static void _Optlink AsyncThread(void *arg)
{
 PASYNCTHREADPARM pThreadParm = (PASYNCTHREADPARM)arg;
#ifdef DEBUG_LOGGING
 ULONG ulSocket    = pThreadParm->u.asyncselect.s;
 ULONG hTaskHandle = pThreadParm->hAsyncTaskHandle;
#endif

  pThreadParm->asyncProc((PVOID)arg);

//only for blocking hooks (currently not implemented)
////  if(pThreadParm->request == ASYNC_BLOCKHOOK)
////	WSASetBlocking(FALSE, pThreadParm->hThread);

  dprintf(("AsyncThread %x socket %x exit", hTaskHandle, ulSocket));
  free((PVOID)pThreadParm);
}
//******************************************************************************
//******************************************************************************
ULONG QueueAsyncJob(ASYNCTHREADPROC asyncproc, PASYNCTHREADPARM pThreadParm, BOOL fSetBlocking)
{
 TID tid;

   dprintf(("QueueAsyncJob for socket %x", pThreadParm->u.asyncselect.s));

   pThreadParm->asyncProc         = asyncproc;
   pThreadParm->fActive           = TRUE;
   pThreadParm->fCancelled        = FALSE;
   pThreadParm->next              = NULL;
   pThreadParm->hAsyncTaskHandle  = 0;

   pThreadParm->hThread           = GetCurrentThread();
   AddToQueue(pThreadParm);

   if(fSetBlocking) WSASetBlocking(TRUE);

   USHORT sel = GetFS();
   tid = _beginthread(AsyncThread, NULL, 16384, (PVOID)pThreadParm);
   SetFS(sel);
   if (tid == -1) {
   	dprintf(("QueueAsyncJob: _beginthread failed"));
   	if(fSetBlocking) WSASetBlocking(FALSE);
	RemoveFromQueue(pThreadParm);
   	WSASetLastError(WSAEFAULT);
	return 0;
   }
   dprintf(("QueueAsyncJob: started thread %x", tid));
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
   asyncThreadMutex.leave();
}
//******************************************************************************
//******************************************************************************
void WSACancelAllAsyncRequests()
{
   PASYNCTHREADPARM pThreadInfo;

   dprintf(("WSACancelAllAsyncRequests"));
   asyncThreadMutex.enter();
   pThreadInfo = threadList;

   while(pThreadInfo) {
        dprintf(("WSACancelAllAsyncRequests %x", pThreadInfo->hAsyncTaskHandle));
        pThreadInfo->fCancelled = TRUE;
        pThreadInfo->u.asyncselect.asyncSem->post();
	pThreadInfo = pThreadInfo->next;
   }
   asyncThreadMutex.leave();
}
//******************************************************************************
//******************************************************************************
void WSAWaitForAllAsyncRequests()
{
   int wait = 0;

   dprintf(("WSAWaitForAllAsyncRequests"));

   //We don't want to wait forever until all async request threads have died
   //(just in case something goes wrong in select())
   //So wait up to one second for all threads to end.
   while(threadList && wait < 1000) {
       DosSleep(20);
       wait += 20;
   }
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
PASYNCTHREADPARM FindAsyncEvent(SOCKET s)
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
BOOL FindAndSetAsyncEvent(SOCKET s, int mode, int notifyHandle, int notifyData, ULONG lEventMask)
{
 PASYNCTHREADPARM pThreadInfo;

   asyncThreadMutex.enter();
   pThreadInfo = FindAsyncEvent(s);
   if(pThreadInfo)
   {
        int size, state, tmp;
        state = ioctl(s, FIOBSTATUS, (char *)&tmp, sizeof(tmp));
        dprintf(("FindAndSetAsyncEvent: state %x", state));

        //Don't send FD_CONNECT is socket was already connected (accept returns connected socket)
        if(state & SS_ISCONNECTED) {
             pThreadInfo->fConnected = TRUE;
        }
        else pThreadInfo->fConnected = FALSE;

        pThreadInfo->u.asyncselect.mode           = mode;
	pThreadInfo->u.asyncselect.lEvents        = lEventMask;
        pThreadInfo->u.asyncselect.lEventsPending = lEventMask;
	pThreadInfo->notifyHandle                 = notifyHandle;
	pThreadInfo->notifyData                   = notifyData;
	if(lEventMask == 0) {
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
BOOL QueryAsyncEvent(SOCKET s, int *pMode, ULONG *pNofityHandle, ULONG *pNofityData,
                     ULONG *plEvent)
{
 PASYNCTHREADPARM pThreadInfo;

   asyncThreadMutex.enter();
   pThreadInfo = FindAsyncEvent(s);
   if(pThreadInfo) {
        *pMode         = pThreadInfo->u.asyncselect.mode;
	*pNofityHandle = pThreadInfo->notifyHandle;
	*pNofityData   = pThreadInfo->notifyData;
	*plEvent       = pThreadInfo->u.asyncselect.lEvents;
   }
   asyncThreadMutex.leave();
   return(pThreadInfo != NULL);
}
//******************************************************************************
//******************************************************************************
