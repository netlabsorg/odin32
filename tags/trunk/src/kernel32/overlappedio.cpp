/* $Id: overlappedio.cpp,v 1.5 2001-12-06 15:57:52 sandervl Exp $ */

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

    if(lpReadHandler == NULL || lpPollHandler == NULL) {
        throw(InvalidParameter);
    }

    pending[ASYNC_INDEX_READ] = pending[ASYNC_INDEX_WRITE] = pending [ASYNC_INDEX_POLL] = NULL;

    this->lpReadHandler       = lpReadHandler;
    this->lpWriteHandler      = lpWriteHandler;
    this->lpPollHandler       = lpPollHandler;

    ::InitializeCriticalSection(&critsect);
    //poll, read & write event semaphores are auto-reset (one thread wakes up
    //after a SetEvent call)
    hEventPoll   = ::CreateEventA(NULL, FALSE, FALSE, NULL);
    hEventRead   = ::CreateEventA(NULL, FALSE, FALSE, NULL);
    hEventWrite  = ::CreateEventA(NULL, FALSE, FALSE, NULL);

    //the exit event semaphore is manual reset, because signalling this event
    //must be able to wake up multiple threads
    hEventExit   = ::CreateEventA(NULL, TRUE, FALSE, NULL);
    if(!hEventPoll || !hEventRead || !hEventWrite || !hEventExit)
    {
        DebugInt3();
        errcode = EventCreationFailed;
        goto failed;
    }
    
    DWORD dwThreadId;
    LPOVERLAPPED_THREAD_PARAM threadparam;

    dwAsyncType = (lpWriteHandler) ? ASYNCIO_READ : ASYNCIO_READWRITE;
    threadparam = (LPOVERLAPPED_THREAD_PARAM)malloc(sizeof(OVERLAPPED_THREAD_PARAM));
    if(!threadparam) goto outofmem;
    threadparam->dwOperation     = dwAsyncType;
    threadparam->lpOverlappedObj = this;
    hThreadRead  = ::CreateThread(NULL, 32*1024, OverlappedIOThread, (LPVOID)threadparam, 0, &dwThreadId);

    if(lpWriteHandler) {
        dwAsyncType |= ASYNCIO_WRITE;

        threadparam = (LPOVERLAPPED_THREAD_PARAM)malloc(sizeof(OVERLAPPED_THREAD_PARAM));
        if(!threadparam) goto outofmem;
        threadparam->dwOperation     = ASYNCIO_WRITE;
        threadparam->lpOverlappedObj = this;
        hThreadWrite = ::CreateThread(NULL, 32*1024, OverlappedIOThread, (LPVOID)threadparam, 0, &dwThreadId);
    }

    if(lpPollHandler) {
        dwAsyncType |= ASYNCIO_POLL;

        threadparam = (LPOVERLAPPED_THREAD_PARAM)malloc(sizeof(OVERLAPPED_THREAD_PARAM));
        if(!threadparam) goto outofmem;
        threadparam->dwOperation     = ASYNCIO_POLL;
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
    dprintf(("~OverlappedIOHandler: signalling overlapped threads"));
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
    DWORD dwOperation;
    OverlappedIOHandler *lpOverlappedObj;

    if(threadparam == NULL) {
        DebugInt3();
        return 0;
    }
    lpOverlappedObj = threadparam->lpOverlappedObj;
    dwOperation     = threadparam->dwOperation;
    //free thread parameter first
    free(threadparam);

    return lpOverlappedObj->threadHandler(dwOperation);
}
//******************************************************************************
//******************************************************************************
DWORD OverlappedIOHandler::threadHandler(DWORD dwOperation)
{
    LPASYNCIOREQUEST lpRequest;
    LPOVERLAPPED     lpOverlapped;
    HANDLE hEvents[2];
    DWORD  ret, dwTimeOut, dwResult;
    int    index;

    dprintf(("OverlappedIOThread: started for event %d", dwOperation));
    switch(dwOperation) {
    case ASYNCIO_READ:
    case ASYNCIO_READWRITE:
        hEvents[0] = hEventRead;
        index      = ASYNC_INDEX_READ;
        break;

    case ASYNCIO_WRITE:
        hEvents[0] = hEventWrite;
        index      = ASYNC_INDEX_WRITE;
        break;

    case ASYNCIO_POLL:
        hEvents[0] = hEventPoll;
        index      = ASYNC_INDEX_POLL;
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
        ::EnterCriticalSection(&critsect);
        if(pending[index] == NULL) {
            //oh, oh
            ::LeaveCriticalSection(&critsect);
            dprintf(("!ERROR!: overlapped thread woken up, but no tasks pending!!"));
            DebugInt3();
            continue;
        }
        lpRequest       = pending[index];
        pending[index]  = lpRequest->next;
        lpRequest->next = NULL;
        ::LeaveCriticalSection(&critsect);

        lpOverlapped = lpRequest->lpOverlapped;;

        switch(dwOperation) {
        case ASYNCIO_READ:
        case ASYNCIO_READWRITE:
	    lpReadHandler(lpRequest, &dwResult, NULL);
            lpOverlapped->Internal     = lpRequest->dwLastError;
            lpOverlapped->InternalHigh = dwResult;
            if(lpRequest->lpdwResult) {
                *lpRequest->lpdwResult = dwResult;
            }
            //wake up user thread
            ::SetEvent(lpOverlapped->hEvent);
            delete lpRequest;
            break;

        case ASYNCIO_WRITE:
	    lpWriteHandler(lpRequest, &dwResult, NULL);
            lpOverlapped->Internal     = lpRequest->dwLastError;
            lpOverlapped->InternalHigh = dwResult;
            if(lpRequest->lpdwResult) {
                *lpRequest->lpdwResult = dwResult;
            }
            //wake up user thread
            ::SetEvent(lpOverlapped->hEvent);
            delete lpRequest;
            break;

        case ASYNCIO_POLL:
            while(TRUE) 
            {
                dwTimeOut = 0;
                if(lpPollHandler(lpRequest, &dwResult, &dwTimeOut) == TRUE) {
                    break;
                }
                if(dwTimeOut == 0) {
                    dprintf(("!ERROR!: lpPollHandler returned timeout 0!!"));
                    DebugInt3();
                    break;
                }
                Sleep(dwTimeOut);
            }
            lpOverlapped->Internal     = lpRequest->dwLastError;
            lpOverlapped->InternalHigh = dwResult;
            if(lpRequest->lpdwResult) {
                *lpRequest->lpdwResult = dwResult;
            }
            //wake up user thread
            ::SetEvent(lpOverlapped->hEvent);
            delete lpRequest;
            break;
        }
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::WriteFile(HANDLE        hHandle,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToWrite,
                                    LPDWORD       lpNumberOfBytesWritten,
                                    LPOVERLAPPED  lpOverlapped,
                                    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                                    DWORD         dwUserData)
{
    LPASYNCIOREQUEST lpRequest, current;
    int              index;

    if(!lpOverlapped || lpOverlapped->hEvent == 0) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
 
    lpRequest = new ASYNCIOREQUEST;
    if(lpRequest == NULL) {
        ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    lpRequest->dwAsyncType         = ASYNCIO_WRITE;
    lpRequest->hHandle             = hHandle;
    lpRequest->lpBuffer            = lpBuffer;
    lpRequest->nNumberOfBytes      = nNumberOfBytesToWrite;
    lpRequest->lpdwResult          = lpNumberOfBytesWritten;
    lpRequest->lpOverlapped        = lpOverlapped;
    lpRequest->lpCompletionRoutine = lpCompletionRoutine;
    lpRequest->dwUserData          = dwUserData;
    lpRequest->next                = NULL;

    if(dwAsyncType == ASYNCIO_READWRITE) {
         index = ASYNC_INDEX_READ;
    }
    else index = ASYNC_INDEX_WRITE;

    ::EnterCriticalSection(&critsect);
    if(pending[index]) {
         current = pending[index];
         while(current->next) {
             current = current->next;
         }
         current->next = lpRequest;
    }
    else pending[index] = lpRequest;
    ::LeaveCriticalSection(&critsect);

    lpOverlapped->Internal     = STATUS_PENDING;
    lpOverlapped->InternalHigh = 0;
    lpOverlapped->Offset       = 0;
    lpOverlapped->OffsetHigh   = 0;
    //reset overlapped semaphore to non-signalled
    ::ResetEvent(lpOverlapped->hEvent);

    //wake up async thread
    ::SetEvent((dwAsyncType == ASYNCIO_READWRITE) ? hEventRead : hEventWrite);

    ::SetLastError(ERROR_IO_PENDING);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::ReadFile(HANDLE        hHandle,
                                   LPCVOID       lpBuffer,
                                   DWORD         nNumberOfBytesToRead,
                                   LPDWORD       lpNumberOfBytesRead,
                                   LPOVERLAPPED  lpOverlapped,
                                   LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                                   DWORD         dwUserData)
{
    LPASYNCIOREQUEST lpRequest, current;

    if(!lpOverlapped || lpOverlapped->hEvent == 0) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
 
    lpRequest = new ASYNCIOREQUEST;
    if(lpRequest == NULL) {
        ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    lpRequest->dwAsyncType         = ASYNCIO_READ;
    lpRequest->hHandle             = hHandle;
    lpRequest->lpBuffer            = lpBuffer;
    lpRequest->nNumberOfBytes      = nNumberOfBytesToRead;
    lpRequest->lpdwResult          = lpNumberOfBytesRead;
    lpRequest->lpOverlapped        = lpOverlapped;
    lpRequest->lpCompletionRoutine = lpCompletionRoutine;
    lpRequest->dwUserData          = dwUserData;
    lpRequest->next                = NULL;

    ::EnterCriticalSection(&critsect);
    if(pending[ASYNC_INDEX_READ]) {
         current = pending[ASYNC_INDEX_READ];
         while(current->next) {
             current = current->next;
         }
         current->next = lpRequest;
    }
    else pending[ASYNC_INDEX_READ] = lpRequest;
    ::LeaveCriticalSection(&critsect);

    lpOverlapped->Internal     = STATUS_PENDING;
    lpOverlapped->InternalHigh = 0;
    lpOverlapped->Offset       = 0;
    lpOverlapped->OffsetHigh   = 0;
    //reset overlapped semaphore to non-signalled
    ::ResetEvent(lpOverlapped->hEvent);

    //wake up async thread
    ::SetEvent(hEventRead);
    ::SetLastError(ERROR_IO_PENDING);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::WaitForEvent(HANDLE        hHandle,
                                       LPDWORD       lpfdwEvtMask,
                                       LPOVERLAPPED  lpOverlapped,
                                       LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                                       DWORD         dwUserData)
{
    LPASYNCIOREQUEST lpRequest, current;

    if(!lpOverlapped || lpOverlapped->hEvent == 0) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
 
    lpRequest = new ASYNCIOREQUEST;
    if(lpRequest == NULL) {
        ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    lpRequest->dwAsyncType         = ASYNCIO_POLL;
    lpRequest->hHandle             = hHandle;
    lpRequest->lpBuffer            = NULL;
    lpRequest->nNumberOfBytes      = 0;
    lpRequest->lpdwResult          = lpfdwEvtMask;
    lpRequest->lpOverlapped        = lpOverlapped;
    lpRequest->lpCompletionRoutine = lpCompletionRoutine;
    lpRequest->dwUserData          = dwUserData;
    lpRequest->next                = NULL;

    ::EnterCriticalSection(&critsect);
    if(pending[ASYNC_INDEX_POLL]) {
         current = pending[ASYNC_INDEX_READ];
         while(current->next) {
             current = current->next;
         }
         current->next = lpRequest;
    }
    else pending[ASYNC_INDEX_POLL] = lpRequest;
    ::LeaveCriticalSection(&critsect);

    lpOverlapped->Internal     = STATUS_PENDING;
    lpOverlapped->InternalHigh = 0;
    lpOverlapped->Offset       = 0;
    lpOverlapped->OffsetHigh   = 0;
    //reset overlapped semaphore to non-signalled
    ::ResetEvent(lpOverlapped->hEvent);

    //wake up async thread
    ::SetEvent(hEventPoll);
    ::SetLastError(ERROR_IO_PENDING);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::CancelIo(HANDLE hHandle)
{
    LPASYNCIOREQUEST lpRequest;

    for(int i=ASYNC_INDEX_READ;i<NR_ASYNC_OPERATIONS;i++) 
    {
        while(TRUE) {
            lpRequest = findAndRemoveRequest(i, hHandle);
            if(lpRequest) {
                 delete lpRequest;
            }
            else break;
        }
    }
    //TODO: return error if there were no pending requests???
    ::SetLastError(ERROR_SUCCESS);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OverlappedIOHandler::GetOverlappedResult(HANDLE        hHandle,
                                              LPOVERLAPPED  lpOverlapped,
                                              LPDWORD       lpcbTransfer,
                                              DWORD         dwTimeout)
{
    DWORD ret;

    ret = ::WaitForSingleObject(lpOverlapped->hEvent, dwTimeout);

    if(lpcbTransfer)
        *lpcbTransfer = lpOverlapped->InternalHigh;

    ::SetLastError(lpOverlapped->Internal);

    return (ret == WAIT_OBJECT_0);
}
//******************************************************************************
//******************************************************************************
LPASYNCIOREQUEST OverlappedIOHandler::findAndRemoveRequest(int index, HANDLE hHandle)
{
    LPASYNCIOREQUEST lpRequest, lpFound = NULL;

    ::EnterCriticalSection(&critsect);
    if(pending[index]) 
    {
        if(pending[index]->hHandle != hHandle) 
        {
            lpRequest = pending[index];
            while(lpRequest->next) {
                if(lpRequest->next->hHandle == hHandle) {
                        lpFound = lpRequest->next;
                        lpRequest->next = lpFound->next;
                        break;
                }
                lpRequest = lpRequest->next;
            }
        }
        else {
            lpFound = pending[index];
            pending[index] = lpFound->next;
        }
    }
    ::LeaveCriticalSection(&critsect);
    return lpFound;
}
//******************************************************************************
//******************************************************************************
