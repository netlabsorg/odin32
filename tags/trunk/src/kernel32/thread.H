/* $Id: thread.H,v 1.3 1999-06-10 19:11:31 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Thread API functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __THREAD_H__
#define __THREAD_H__

//Why the @#%@#$ didn't IBM use _stdcall??

typedef DWORD (* WIN32API WIN32THREADPROC)(LPVOID);

class Win32Thread
{
public:
    Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData);
    ~Win32Thread();

    PTHREAD_START_ROUTINE_O32 GetOS2Callback();
private:

 LPVOID      lpUserData;
 LPTHREAD_START_ROUTINE pCallback;
 friend static DWORD OPEN32API Win32ThreadProc(LPVOID lpData);
};

#endif
