/* $Id: asyncthread.cpp,v 1.1 2000-03-22 20:01:04 sandervl Exp $ */

/*
 * Async thread help functions
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * 
 * Not everything is thread safe 
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#define INCL_BASE
#include <os2wrap.h>
#include <os2sel.h>
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
static void APIENTRY AsyncThread(ULONG arg)
{
 PASYNCTHREADPARM pThreadParm = (PASYNCTHREADPARM)arg;

  pThreadParm->asyncProc((PVOID)arg);

  WSASetBlocking(TRUE, pThreadParm->hThread);

  DosExit(EXIT_THREAD, 0);
}
//******************************************************************************
//******************************************************************************
ULONG QueueAsyncJob(ASYNCTHREADPROC asyncproc, PASYNCTHREADPARM pThreadParm)
{
 APIRET rc;
 TID    tid;

   pThreadParm->asyncProc         = asyncproc;
   pThreadParm->fActive           = TRUE;
   pThreadParm->fCancelled        = FALSE;
   pThreadParm->next              = NULL;
   pThreadParm->hAsyncTaskHandle  = 0;

   pThreadParm->hThread           = GetCurrentThread();
   AddToQueue(pThreadParm);

   WSASetBlocking(TRUE);

   rc = DosCreateThread(&tid, AsyncThread, (ULONG)pThreadParm, CREATE_READY, 16384);
   if(rc)
   {
   	dprintf(("QueueAsyncJob: DosCreateThread failed with error %x", rc));
   	WSASetBlocking(FALSE);
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
 PASYNCTHREADPARM parm;

   asyncThreadMutex.enter();
   parm = threadList;

   if(parm == pThreadParm) {
	threadList = pThreadParm->next;
   }
   else {
	while(parm->next) {
		if(parm->next == pThreadParm) {
			parm->next = pThreadParm->next;
			break;
		}
		parm = parm->next;
	}
	if(parm == NULL) {
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
 PASYNCTHREADPARM parm;
 BOOL             found = FALSE;

   dprintf(("WSACancelAsyncRequest: cancel task %x", hAsyncTaskHandle));
   asyncThreadMutex.enter();
   parm = threadList;

   while(parm) {
	if(parm->hAsyncTaskHandle == hAsyncTaskHandle) {
		parm->fCancelled = TRUE;
		found = TRUE;
		break;
	}
	parm = parm->next;
   }
   asyncThreadMutex.leave();
   if(found == FALSE) {
      	WSASetLastError(WSAEINVAL);
	dprintf(("WSACancelAsyncRequest: task not found!!"));
   }
   return (found) ? NO_ERROR : SOCKET_ERROR;
}
//******************************************************************************
//******************************************************************************
void EnableAsyncEvent(SOCKET s, ULONG flags)
{
}
//******************************************************************************
//******************************************************************************
BOOL QueryAsyncEvent(SOCKET s, HWND *pHwnd, int *pMsg, ULONG *plEvent)
{
   return FALSE;
}
//******************************************************************************
//******************************************************************************
