/* $Id: overlappedio.h,v 1.1 2001-12-05 14:16:38 sandervl Exp $ */

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


class OverlappedIOHandler
{
public:
     OverlappedIOHandler(

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
                                DWORD         dwTimeout)

protected:

private:

     HANDLE       hThreadEvent;
     HANDLE       hThreadRead;
     HANDLE       hThreadWrite;
     HANDLE       hEventExit;
     HANDLE       hEventPoll;
     HANDLE       hEventRead;
     HANDLE       hEventWrite;

     //linked list of pending operations
     LPOVERLAPPED pending;

};


#endif // __OVERLAPPEDIO_H__
