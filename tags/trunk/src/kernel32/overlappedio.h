/* $Id: overlappedio.h,v 1.2 2001-12-05 18:06:03 sandervl Exp $ */

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

#define EVENT_READ		1
#define EVENT_WRITE		2
#define EVENT_READWRITE		(EVENT_WRITE|EVENT_READ)
#define EVENT_POLL		4

//forward decl
class OverlappedIOHandler;

typedef struct {
    DWORD                fEvent;
    OverlappedIOHandler *lpOverlappedObj;
} OVERLAPPED_THREAD_PARAM, *LPOVERLAPPED_THREAD_PARAM;

typedef DWORD (* SYSTEM LPOVERLAPPED_HANDLER)(DWORD dwUserData, LPOVERLAPPED lpOverlapped);

enum OverlappedIOError {
  OutOfMemory, EventCreationFailed, ThreadCreationFailed
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
                      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

     BOOL   ReadFile(HANDLE        hOS2Handle,
                     LPCVOID       lpBuffer,
                     DWORD         nNumberOfBytesToRead,
                     LPDWORD       lpNumberOfBytesRead,
                     LPOVERLAPPED  lpOverlapped,
                     LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

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

     //linked list of pending operations
     LPOVERLAPPED pending;


     friend       DWORD CALLBACK OverlappedIOThread(LPVOID lpThreadParam);
};


#endif // __OVERLAPPEDIO_H__
