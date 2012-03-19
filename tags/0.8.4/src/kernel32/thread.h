/* $Id: thread.H,v 1.8 2002-06-11 16:36:54 sandervl Exp $ */

/*
 * Win32 Thread API functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __THREAD_H__
#define __THREAD_H__

typedef DWORD (* WIN32API WIN32THREADPROC)(LPVOID);

class Win32Thread
{
public:
    Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData, DWORD dwFlags, HANDLE hThread, DWORD cbCommitStack);

    PTHREAD_START_ROUTINE_O32 GetOS2Callback()  { return Win32ThreadProc; };
    /* this is to be privately used by hmthread.cpp only */
    LPVOID _GetTEB()  { return teb; };

private:

    LPVOID      lpUserData;
    LPTHREAD_START_ROUTINE pCallback;
    DWORD       dwFlags;
    HANDLE      hThread;
    LPVOID      teb;
    DWORD       cbCommitStack;

    static DWORD OPEN32API Win32ThreadProc(LPVOID lpData);
};

#endif
