/* $Id: overlappedio.h,v 1.4 2001-12-06 15:57:52 sandervl Exp $ */

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

#define NR_ASYNC_OPERATIONS	3
#define ASYNC_INDEX_READ        0
#define ASYNC_INDEX_WRITE       1
#define ASYNC_INDEX_POLL        2

#define ASYNCIO_READ		1
#define ASYNCIO_WRITE		2
#define ASYNCIO_READWRITE	4
#define ASYNCIO_POLL		8

//forward decl
class OverlappedIOHandler;

typedef struct {
    DWORD                dwOperation;
    OverlappedIOHandler *lpOverlappedObj;
} OVERLAPPED_THREAD_PARAM, *LPOVERLAPPED_THREAD_PARAM;

typedef struct tagOVERLAPPED_ODIN{
  DWORD               dwAsyncType;
  HANDLE              hHandle;
  LPCVOID             lpBuffer;
  ULONG               nNumberOfBytes;
  LPOVERLAPPED        lpOverlapped;
  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
  DWORD              *lpdwResult;
  DWORD               dwLastError;
  DWORD               dwUserData;
  tagOVERLAPPED_ODIN *next;
} ASYNCIOREQUEST, *LPASYNCIOREQUEST;

typedef BOOL (* SYSTEM LPOVERLAPPED_HANDLER)(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut);

enum OverlappedIOError {
  InvalidParameter, OutOfMemory, EventCreationFailed, ThreadCreationFailed
};

class OverlappedIOHandler
{
public:
     OverlappedIOHandler(LPOVERLAPPED_HANDLER lpReadHandler, 
                         LPOVERLAPPED_HANDLER lpWriteHandler, 
                         LPOVERLAPPED_HANDLER lpPollHandler = NULL);
    ~OverlappedIOHandler();

     BOOL   WriteFile(HANDLE        hHandle,
                      LPCVOID       lpBuffer,
                      DWORD         nNumberOfBytesToWrite,
                      LPDWORD       lpNumberOfBytesWritten,
                      LPOVERLAPPED  lpOverlapped,
                      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                      DWORD         dwUserData);

     BOOL   ReadFile(HANDLE        hHandle,
                     LPCVOID       lpBuffer,
                     DWORD         nNumberOfBytesToRead,
                     LPDWORD       lpNumberOfBytesRead,
                     LPOVERLAPPED  lpOverlapped,
                     LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                     DWORD         dwUserData);

     BOOL   WaitForEvent(HANDLE        hHandle,
                         LPDWORD       lpfdwEvtMask,
                         LPOVERLAPPED  lpOverlapped,
                         LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                         DWORD         dwUserData);

     BOOL   CancelIo(HANDLE hHandle);

     BOOL   GetOverlappedResult(HANDLE        hHandle,
                                LPOVERLAPPED  lpoOverlapped,
                                LPDWORD       lpcbTransfer,
                                DWORD         dwTimeout);

protected:

private:
     LPASYNCIOREQUEST findAndRemoveRequest(int index, HANDLE hHandle);

     DWORD        threadHandler(DWORD fEvent);

     HANDLE       hThreadPoll;
     HANDLE       hThreadRead;
     HANDLE       hThreadWrite;
     HANDLE       hEventExit;
     HANDLE       hEventPoll;
     HANDLE       hEventRead;
     HANDLE       hEventWrite;

     LPOVERLAPPED_HANDLER lpReadHandler;
     LPOVERLAPPED_HANDLER lpWriteHandler;
     LPOVERLAPPED_HANDLER lpPollHandler;

     CRITICAL_SECTION     critsect;

     DWORD                dwAsyncType;
     //[ASYNC_INDEX_READ]  list of pending read (+ write if half-duplex mode) operations
     //[ASYNC_INDEX_WRITE] list of pending write (full-duplex mode) operations
     //[ASYNC_INDEX_POLL]  list of pending poll operations
     LPASYNCIOREQUEST     pending[NR_ASYNC_OPERATIONS];

     friend       DWORD CALLBACK OverlappedIOThread(LPVOID lpThreadParam);
};


#endif // __OVERLAPPEDIO_H__
