/* $Id: overlappedio.cpp,v 1.3 2001-12-05 18:06:57 sandervl Exp $ */

/*
 * Win32 overlapped IO class
 *
 * Copyright 2001 Sander van Leeuwen <sandervl@xs4all.nl>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



#include <os2win.h>
#include <string.h>
#include <handlemanager.h>
#include <heapstring.h>
#include "overlappedio.h"
#include "oslibdos.h"

#define DBG_LOCALLOG  DBG_overlappedio
#include "dbglocal.h"


//******************************************************************************
//******************************************************************************
OverlappedIOHandler::OverlappedIOHandler(LPOVERLAPPED_HANDLER lpReadHandler, 
                                         LPOVERLAPPED_HANDLER lpWriteHandler, 
                                         LPOVERLAPPED_HANDLER lpPollHandler) :
                   hThreadRead(0), hThreadWrite(0), hThreadPoll(0)
{
    OverlappedIOError errcode = OutOfMemory;

    pending                  = NULL;
    this->lpReadHandler      = lpReadHandler;
    this->lpWriteHandler     = lpWriteHandler;
    this->lpPollHandler      = lpPollHandler;

    ::InitializeCriticalSection(&critsect);
    hEventPoll   = ::CreateEventA(NULL, TRUE, FALSE, NULL);
    hEventRead   = ::CreateEventA(NULL, TRUE, FALSE, NULL);
    hEventWrite  = ::CreateEventA(NULL, TRUE, FALSE, NULL);
    hEventExit   = ::CreateEventA(NULL, TRUE, FALSE, NULL);
    if(!hEventPoll || !hEventRead || !hEventWrite || !hEventExit)
    {
        DebugInt3();
        errcode = EventCreationFailed;
        goto failed;
    }
    
    DWORD dwThreadId;
    LPOVERLAPPED_THREAD_PARAM threadparam;

    threadparam = (LPOVERLAPPED_THREAD_PARAM)malloc(sizeof(OVERLAPPED_THREAD_PARAM));
    if(!threadparam) goto outofmem;

    threadparam->fEvent          = (lpWriteHandler) ? EVENT_READ : EVENT_READWRITE;
    threadparam->lpOverlappedObj = this;
    hThreadRead  = ::CreateThread(NULL, 32*1024, OverlappedIOThread, (LPVOID)threadparam, 0, &dwThreadId);
    if(lpWriteHandler) {
        threadparam = (LPOVERLAPPED_THREAD_PARAM)malloc(sizeof(OVERLAPPED_THREAD_PARAM));
        if(!threadparam) goto outofmem;
        threadparam->fEvent          = EVENT_WRITE;
        threadparam->lpOverlappedObj = this;
        hThreadWrite = ::CreateThread(NULL, 32*1024, OverlappedIOThread, (LPVOID)threadparam, 0, &dwThreadId);
    }

    if(lpPollHandler) {
        threadparam = (LPOVERLAPPED_THREAD_PARAM)malloc(sizeof(OVERLAPPED_THREAD_PARAM));
        if(!threadparam) goto outofmem;
        threadparam->fEvent          = EVENT_POLL;
        threadparam->lpOverlappedObj = this;
        hThreadPoll  = ::CreateThread(NULL, 32*1024, OverlappedIOThread, (LPVOID)threadparam, 0, &dwThreadId);
    }

    if((lpPollHandler && !hThreadPoll) || !hThreadRead || (lpWriteHandler && !hThreadWrite)) 
    {
        DebugInt3();
        errcode = ThreadCreationFailed;
        goto failed;
    }
    return;

outofmem: 
    errcode = OutOfMemory;
    //fall through
failed:
    //SvL: NOTE: We might not fail gracefully when threads have already been 
    //           created. (thread accessing memory that has been freed)
    //           Don't feel like wasting time to fix this as this should never
    //           happen anyway.
    if(hEventExit) {
        ::SetEvent(hEventExit);
        ::CloseHandle(hEventExit);
    }

    if(hEventRead)   ::CloseHandle(hEventRead);
    if(hEventWrite)  ::CloseHandle(hEventWrite);
    if(hEventPoll)   ::CloseHandle(hEventPoll);

    if(hThreadRead)  ::CloseHandle(hThreadRead);
    if(hThreadPoll)  ::CloseHandle(hThreadPoll);
    if(hThreadWrite) ::CloseHandle(hThreadWrite);
    ::DeleteCriticalSection(&critsect);

    throw(errcode);
}
//******************************************************************************
//******************************************************************************
OverlappedIOHandler::~OverlappedIOHandler()
{
    dprintf(("~OverlappedIOHandler: signalling overlapped serial threads"));
    ::SetEvent(hEventExit);

    ::CloseHandle(hEventExit);
    ::CloseHandle(hEventRead);
    ::CloseHandle(hEventWrite);
    ::CloseHandle(hEventPoll);

    ::CloseHandle(hThreadRead);
    if(hThreadPoll)  ::CloseHandle(hThreadPoll);
    if(hThreadWrite) ::CloseHandle(hThreadWrite);

    DeleteCriticalSection(&critsect);
}
//******************************************************************************
//******************************************************************************
DWORD CALLBACK OverlappedIOThread(LPVOID lpThreadParam)
{
    LPOVERLAPPED_THREAD_PARAM threadparam = (LPOVERLAPPED_THREAD_PARAM)lpThreadParam;
    DWORD fEvent;
    OverlappedIOHandler *lpOverlappedObj;

    if(threadparam == NULL) {
        DebugInt3();
        return 0;
    }
    lpOverlappedObj = threadparam->lpOverlappedObj;
    fEvent          = threadparam->fEvent;
    //free thread parameter first
    free(threadparam);

    return lpOverlappedObj->threadHandler(fEvent);
}
//******************************************************************************
//******************************************************************************
DWORD OverlappedIOHandler::threadHandler(DWORD fEvent)
{
    HANDLE hEvents[2];
    DWORD  ret;

    dprintf(("OverlappedIOThread: started for event %d", fEvent));
    switch(fEvent) {
    case EVENT_READ:
    case EVENT_READWRITE:
        hEvents[0] = hEventRead;
        break;

    case EVENT_WRITE:
        hEvents[0] = hEventWrite;
        break;

    case EVENT_POLL:
        hEvents[0] = hEventPoll;
        break;
    default:
        DebugInt3();
    }
    hEvents[1] = hEventExit;

    while(TRUE) {
        ret = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
        if(ret == WAIT_FAILED) {
            dprintf(("!WARNING!: WaitForMultipleObjects -> WAIT_FAILED!"));
            break;
        }
        if(ret == WAIT_FAILED) {
            dprintf(("!WARNING!: WaitForMultipleObjects -> WAIT_FAILED!"));
            break;
        }
        //if hEventExit has been signalled, then we are told to exit
        if(ret == (WAIT_OBJECT_0+1)) {
            dprintf(("end of threadHandler signalled"));
            break;
        }
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::WriteFile(HANDLE        hOS2Handle,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToWrite,
                                    LPDWORD       lpNumberOfBytesWritten,
                                    LPOVERLAPPED  lpOverlapped,
                                    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::ReadFile(HANDLE        hOS2Handle,
                                   LPCVOID       lpBuffer,
                                   DWORD         nNumberOfBytesToRead,
                                   LPDWORD       lpNumberOfBytesRead,
                                   LPOVERLAPPED  lpOverlapped,
                                   LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::CancelIo(HANDLE hOS2Handle)
{
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::GetOverlappedResult(HANDLE        hOS2Handle,
                                              LPOVERLAPPED  lpoOverlapped,
                                              LPDWORD       lpcbTransfer,
                                              DWORD         dwTimeout)
{
    return FALSE;
}
//******************************************************************************
//******************************************************************************
