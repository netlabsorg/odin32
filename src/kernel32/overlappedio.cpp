/* $Id: overlappedio.cpp,v 1.11 2001-12-10 12:55:12 sandervl Exp $ */

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
                                         LPOVERLAPPED_HANDLER lpPollHandler,
                                         BOOL fFullDuplex) :
                   hThreadRead(0), hThreadWrite(0), hThreadPoll(0)
{
    OverlappedIOError errcode = OutOfMemory;

    this->fFullDuplex = fFullDuplex;

    if(lpReadHandler == NULL || lpPollHandler == NULL) {
        throw(InvalidParameter);
    }

    pending[ASYNC_INDEX_READ] = pending[ASYNC_INDEX_WRITE]  = NULL;
    pending [ASYNC_INDEX_POLL] = pending [ASYNC_INDEX_BUSY] = NULL;

    this->lpReadHandler       = lpReadHandler;
    this->lpWriteHandler      = lpWriteHandler;
    this->lpPollHandler       = lpPollHandler;

    ::InitializeCriticalSection(&critsect);
    //poll, read & write event semaphores are auto-reset (one thread wakes up
    //after a SetEvent call)
    hEventPoll   = ::CreateEventA(NULL, FALSE, FALSE, NULL);
    hEventRead   = ::CreateEventA(NULL, FALSE, FALSE, NULL);
    hEventWrite  = ::CreateEventA(NULL, FALSE, FALSE, NULL);

    dprintf(("OverlappedIOThread: hEventRead %x hEventWrite %x hEventPoll %x", hEventRead, hEventWrite, hEventPoll));

    //the exit & cancel event semaphores are manual reset, because these events
    //must be able to wake up multiple threads
    hEventExit   = ::CreateEventA(NULL, TRUE, FALSE, NULL);
    hEventCancel = ::CreateEventA(NULL, TRUE, FALSE, NULL);
    if(!hEventPoll || !hEventRead || !hEventWrite || !hEventExit || !hEventCancel)
    {
        DebugInt3();
        errcode = EventCreationFailed;
        goto failed;
    }

    DWORD dwThreadId;
    LPOVERLAPPED_THREAD_PARAM threadparam;

    dwAsyncType = (lpWriteHandler && fFullDuplex) ? ASYNCIO_READ : ASYNCIO_READWRITE;
    threadparam = (LPOVERLAPPED_THREAD_PARAM)malloc(sizeof(OVERLAPPED_THREAD_PARAM));
    if(!threadparam) goto outofmem;
    threadparam->dwOperation     = dwAsyncType;
    threadparam->lpOverlappedObj = this;
    hThreadRead  = ::CreateThread(NULL, 32*1024, OverlappedIOThread, (LPVOID)threadparam, 0, &dwThreadId);

    if(lpWriteHandler && fFullDuplex) {
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

    if((lpPollHandler && !hThreadPoll) || !hThreadRead || (lpWriteHandler && fFullDuplex && !hThreadWrite))
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
    HANDLE hEventsWait[2];
    HANDLE hHandle;
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

    while(TRUE)
    {
        ret = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
        if(ret == WAIT_FAILED) {
            dprintf(("!WARNING!: WaitForMultipleObjects -> WAIT_FAILED!"));
            break;
        }
        //if hEventExit has been signalled, then we are told to exit
        if(ret == (WAIT_OBJECT_0+1)) {
            dprintf(("end of threadHandler signalled"));
            break;
        }
        //process all pending jobs
        while(TRUE)
        {
            ::EnterCriticalSection(&critsect);
            if(pending[index] == NULL) {
                ::LeaveCriticalSection(&critsect);
                break;
            }
            lpRequest       = pending[index];
            pending[index]  = lpRequest->next;

            //add to in process list
            lpRequest->next = pending[ASYNC_INDEX_BUSY];
            pending[ASYNC_INDEX_BUSY] = lpRequest;
            ::LeaveCriticalSection(&critsect);

            lpOverlapped = lpRequest->lpOverlapped;;
            hHandle      = lpRequest->hHandle;

            switch(dwOperation) {
            case ASYNCIO_READ:
            case ASYNCIO_READWRITE:
            case ASYNCIO_WRITE:
                if(lpRequest->dwAsyncType == ASYNCIO_READ || lpWriteHandler == NULL) {
                     lpRequest->dwLastError = lpReadHandler(lpRequest, &dwResult, NULL);
                }
                else lpRequest->dwLastError = lpWriteHandler(lpRequest, &dwResult, NULL);

                lpOverlapped->Internal     = lpRequest->dwLastError;
                lpOverlapped->InternalHigh = dwResult;
                if(lpRequest->lpdwResult) {
                    *lpRequest->lpdwResult = dwResult;
                }
#ifdef DEBUG
                if(lpRequest->dwAsyncType == ASYNCIO_READ) {
                     dprintf(("ASYNCIO_READ %x finished; result %x, last error %d", lpOverlapped, dwResult, lpRequest->dwLastError));
                }
                else dprintf(("ASYNCIO_WRITE %x finished; result %x, last error %d", lpOverlapped, dwResult, lpRequest->dwLastError));
#endif
                //wake up user thread
                ::SetEvent(lpOverlapped->hEvent);
                break;

            case ASYNCIO_POLL:
                hEventsWait[0] = hEventCancel;
                hEventsWait[1] = hEventExit;
                ret = WAIT_TIMEOUT;
                while(TRUE)
                {
                    dwTimeOut = 0;
                    lpRequest->dwLastError = lpPollHandler(lpRequest, &dwResult, &dwTimeOut);
                    if(lpRequest->dwLastError != ERROR_IO_PENDING) {
                        break;
                    }
                    if(dwTimeOut == 0) {
                        dprintf(("!ERROR!: lpPollHandler returned timeout 0!!"));
                        DebugInt3();
                        break;
                    }
                    //sleep a while to avoid wasting too many cpu cycles; we are woken up when a timeout occurs,
                    //when the operation is cancelled or when the process exits
                    ret = WaitForMultipleObjects(2, hEventsWait, FALSE, dwTimeOut);
                    if(ret != WAIT_TIMEOUT) {
                        dprintf(("ASYNCIO_POLL: WaitForSingleObject didn't time out, abort (ret = %x)", ret));
                        break;
                    }
                }
                //Don't access the overlapped & result memory when CancelIo was used to cancel the operation
                if(ret == WAIT_TIMEOUT)
                {
                    dprintf(("ASYNCIO_POLL %x: result %x, last error %d", lpOverlapped, dwResult, lpRequest->dwLastError));
                    lpOverlapped->Internal     = lpRequest->dwLastError;
                    lpOverlapped->InternalHigh = dwResult;
                    if(lpRequest->lpdwResult) {
                        *lpRequest->lpdwResult = dwResult;
                    }
                    //wake up user thread
                    ::SetEvent(lpOverlapped->hEvent);
                }
                break;
            }
            //remove from in-process list and delete async request object
            findAndRemoveRequest(ASYNC_INDEX_BUSY, hHandle);
            delete lpRequest;
        } //while(TRUE)
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
                                    DWORD         dwUserData,
                                    DWORD         dwTimeOut)
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
    lpRequest->dwTimeOut           = dwTimeOut;
    lpRequest->next                = NULL;

    if(dwAsyncType & ASYNCIO_READWRITE) {
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
    ::SetEvent((dwAsyncType & ASYNCIO_READWRITE) ? hEventRead : hEventWrite);

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
                                   DWORD         dwUserData,
                                   DWORD         dwTimeOut)
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
    lpRequest->dwTimeOut           = dwTimeOut;
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
                                       DWORD         dwEventMask,
                                       LPDWORD       lpfdwEvtMask,
                                       LPOVERLAPPED  lpOverlapped,
                                       LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                                       DWORD         dwUserData,
                                       DWORD         dwTimeOut)
{
    LPASYNCIOREQUEST lpRequest, current;
    DWORD            dwLastError, dwResult;

    if(!lpOverlapped || lpOverlapped->hEvent == 0) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(!lpPollHandler) {
        DebugInt3();
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
    lpRequest->dwTimeOut           = dwTimeOut;
    lpRequest->dwEventMask         = dwEventMask;
    lpRequest->next                = NULL;

    lpOverlapped->Internal     = STATUS_PENDING;
    lpOverlapped->InternalHigh = 0;
    lpOverlapped->Offset       = 0;
    lpOverlapped->OffsetHigh   = 0;
    //reset overlapped semaphore to non-signalled
    ::ResetEvent(lpOverlapped->hEvent);

    //first check if the event has already occured; if so, return result
    //immediately
    dwLastError = lpPollHandler(lpRequest, &dwResult, &dwTimeOut);
    if(dwLastError != ERROR_IO_PENDING)
    {
        dprintf(("OverlappedIOHandler::WaitForEvent %x: result %x, last error %d", lpOverlapped, dwResult, dwLastError));
        lpOverlapped->Internal     = dwLastError;
        lpOverlapped->InternalHigh = dwResult;
        if(lpfdwEvtMask) {
            *lpfdwEvtMask = dwResult;
        }
        //wake up user thread
        ::SetEvent(lpOverlapped->hEvent);

        delete lpRequest;
        ::SetLastError(dwLastError);
        return (dwLastError == ERROR_SUCCESS);
    }

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
        while(TRUE) 
        {
            lpRequest = findAndRemoveRequest(i, hHandle);

            if(lpRequest) {
                 //TODO: This doesn't work if multiple handles share the
                 //      same OverlappedIOHandler
                 lpRequest->fCancelled = TRUE;
                 ::SetEvent(hEventCancel);   //cancel pending operation
                 if(i != ASYNC_INDEX_BUSY) {//thread that handles the request will delete it
                    delete lpRequest;
                 }
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
                                              BOOL          fWait)
{
    DWORD ret;

    ret = ::WaitForSingleObject(lpOverlapped->hEvent, (fWait) ? INFINITE : 0);

    if(lpcbTransfer)
        *lpcbTransfer = lpOverlapped->InternalHigh;

    ::SetLastError(lpOverlapped->Internal);

    dprintf(("GetOverlappedResult %x -> result %d last error %d", hHandle, lpOverlapped->InternalHigh, lpOverlapped->Internal));
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

