/* $Id: overlappedio.h,v 1.3 2001-12-05 19:24:37 sandervl Exp $ */

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
  HANDLE              hOS2Handle;
  LPCVOID             lpBuffer;
  ULONG               nNumberOfBytes;
  LPOVERLAPPED        lpOverlapped;
  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
  DWORD              *lpResult;
  DWORD               dwUserData;
  tagOVERLAPPED_ODIN *next;
} ASYNCIOREQUEST, *LPASYNCIOREQUEST;

typedef DWORD (* SYSTEM LPOVERLAPPED_HANDLER)(LPASYNCIOREQUEST lpRequest);

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

     BOOL   WriteFile(HANDLE        hOS2Handle,
                      LPCVOID       lpBuffer,
                      DWORD         nNumberOfBytesToWrite,
                      LPDWORD       lpNumberOfBytesWritten,
                      LPOVERLAPPED  lpOverlapped,
                      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                      DWORD         dwUserData);

     BOOL   ReadFile(HANDLE        hOS2Handle,
                     LPCVOID       lpBuffer,
                     DWORD         nNumberOfBytesToRead,
                     LPDWORD       lpNumberOfBytesRead,
                     LPOVERLAPPED  lpOverlapped,
                     LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
                     DWORD         dwUserData);

     BOOL   CancelIo(HANDLE hOS2Handle);

     BOOL   GetOverlappedResult(HANDLE        hOS2Handle,
                                LPOVERLAPPED  lpoOverlapped,
                                LPDWORD       lpcbTransfer,
                                DWORD         dwTimeout);

protected:

private:

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
