/* $Id: overlappedio.h,v 1.3 2002-07-05 17:56:33 sandervl Exp $ */

/*
 * Win32 overlapped IO class
 *
 * Copyright 2001 Sander van Leeuwen <sandervl@xs4all.nl>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __OVERLAPPEDIO_H__
#define __OVERLAPPEDIO_H__

#define NR_ASYNC_OPERATIONS     4
#define ASYNC_INDEX_READ        0
#define ASYNC_INDEX_WRITE       1
#define ASYNC_INDEX_POLL        2
#define ASYNC_INDEX_BUSY        3

#define ASYNC_TYPE_HALFDUPLEX   0
#define ASYNC_TYPE_FULLDUPLEX   1

#define ASYNCIO_READ        1
#define ASYNCIO_WRITE       2
#define ASYNCIO_READWRITE   4
#define ASYNCIO_POLL        8

//forward decl
class OverlappedIOHandler;

typedef struct {
    DWORD                dwOperation;
    OverlappedIOHandler *lpOverlappedObj;
} OVERLAPPED_THREAD_PARAM, *LPOVERLAPPED_THREAD_PARAM;


class ASYNCIOREQUEST
{
public:
    ASYNCIOREQUEST()
    {
        memset(this, 0, sizeof(ASYNCIOREQUEST));
    };

    DWORD               dwAsyncType;
    HANDLE              hHandle;
    LPCVOID             lpBuffer;
    DWORD               nNumberOfBytes;
    DWORD               dwTimeOut;
    LPOVERLAPPED        lpOverlapped;
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
    DWORD              *lpdwResult;
    DWORD               dwLastError;
    DWORD               dwEventMask;
    DWORD               dwUserData;
    BOOL                fCancelled;
    ASYNCIOREQUEST     *next;
};

typedef ASYNCIOREQUEST *LPASYNCIOREQUEST;

typedef DWORD (* LPOVERLAPPED_HANDLER)(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut);

enum OverlappedIOError {
  InvalidParameter, OutOfMemory, EventCreationFailed, ThreadCreationFailed
};

class OverlappedIOHandler
{
public:
     OverlappedIOHandler(LPOVERLAPPED_HANDLER lpReadHandler,
                         LPOVERLAPPED_HANDLER lpWriteHandler,
                         LPOVERLAPPED_HANDLER lpPollHandler = NULL,
                         BOOL fFullDuplex = ASYNC_TYPE_HALFDUPLEX);
    ~OverlappedIOHandler();

     DWORD  AddRef();
     DWORD  Release(BOOL fSignalExit = FALSE);

     BOOL   WriteFile(HANDLE        hHandle,
                      LPCVOID       lpBuffer,
                      DWORD         nNumberOfBytesToWrite,
                      LPDWORD       lpNumberOfBytesWritten,
                      LPOVERLAPPED  lpOverlapped,
                      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                      DWORD         dwUserData,
                      DWORD         dwTimeOut = INFINITE);

     BOOL   ReadFile(HANDLE        hHandle,
                     LPCVOID       lpBuffer,
                     DWORD         nNumberOfBytesToRead,
                     LPDWORD       lpNumberOfBytesRead,
                     LPOVERLAPPED  lpOverlapped,
                     LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                     DWORD         dwUserData,
                     DWORD         dwTimeOut = INFINITE);

     BOOL   WaitForEvent(HANDLE        hHandle,
                         DWORD         dwEventMask,
                         LPDWORD       lpfdwEvtMask,
                         LPOVERLAPPED  lpOverlapped,
                         LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                         DWORD         dwUserData,
                         DWORD         dwTimeOut = INFINITE);

     BOOL   CancelIo(HANDLE hHandle);

     BOOL   GetOverlappedResult(HANDLE        hHandle,
                                LPOVERLAPPED  lpoOverlapped,
                                LPDWORD       lpcbTransfer,
                                BOOL          fWait);

protected:

private:
     LPASYNCIOREQUEST findAndRemoveRequest(int index, HANDLE hHandle);
     void             addRequest(int index, LPASYNCIOREQUEST lpRequest);
     void             removeRequest(int index, LPASYNCIOREQUEST lpRequest);

     DWORD        threadHandler(DWORD fEvent);

     HANDLE       hThreadPoll;
     HANDLE       hThreadRead;
     HANDLE       hThreadWrite;
     HANDLE       hEventExit;
     HANDLE       hEventCancel;
     HANDLE       hEventPoll;
     HANDLE       hEventRead;
     HANDLE       hEventWrite;

     LPOVERLAPPED_HANDLER lpReadHandler;
     LPOVERLAPPED_HANDLER lpWriteHandler;
     LPOVERLAPPED_HANDLER lpPollHandler;

     CRITICAL_SECTION     critsect;

     DWORD                dwAsyncType;
     BOOL                 fFullDuplex;
     LONG                 refCount;

     //[ASYNC_INDEX_READ]  list of pending read (+ write if half-duplex mode) operations
     //[ASYNC_INDEX_WRITE] list of pending write (full-duplex mode) operations
     //[ASYNC_INDEX_POLL]  list of pending poll operations
     //[ASYNC_INDEX_BUSY]  list of operations that are being handled
     LPASYNCIOREQUEST     pending[NR_ASYNC_OPERATIONS];

     static DWORD CALLBACK OverlappedIOThread(LPVOID lpThreadParam);
};


#endif // __OVERLAPPEDIO_H__
