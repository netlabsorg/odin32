/* $Id: thread.H,v 1.7 2001-02-11 10:34:44 sandervl Exp $ */

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
    Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData, DWORD dwFlags, HANDLE hThread);

    PTHREAD_START_ROUTINE_O32 GetOS2Callback()  { return Win32ThreadProc; };

private:

 LPVOID      lpUserData;
 LPTHREAD_START_ROUTINE pCallback;
 DWORD       dwFlags;
 HANDLE      hThread;

 friend DWORD OPEN32API Win32ThreadProc(LPVOID lpData);
};

#endif
