/* $Id: hmthread.h,v 1.4 2001-12-03 12:13:09 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 thread handle class
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */

#ifndef __HMTHREAD_H__
#define __HMTHREAD_H__

#include "HMDevice.h"
#include "HMOpen32.h"

#define THREAD_TERMINATED	0
#define THREAD_ALIVE		1

class HMDeviceThreadClass : public HMDeviceOpen32Class
{
public:
  HMDeviceThreadClass(LPCSTR lpDeviceName) : HMDeviceOpen32Class(lpDeviceName) {}

  virtual HANDLE CreateThread(PHMHANDLEDATA          pHMHandleData,
                              LPSECURITY_ATTRIBUTES  lpsa,
                              DWORD                  cbStack,
                              LPTHREAD_START_ROUTINE lpStartAddr,
                              LPVOID                 lpvThreadParm,
                              DWORD                  fdwCreate,
                              LPDWORD                lpIDThread, 
                              BOOL                   fFirstThread);

  /* this is a handler method for calls to WaitForSingleObject */
  virtual DWORD  WaitForSingleObject(PHMHANDLEDATA pHMHandleData,
                                     DWORD  dwTimeout);

  /* this is a handler method for calls to WaitForSingleObjectEx */
  virtual DWORD  WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                       DWORD  dwTimeout,
                                       BOOL   fAlertable);

  virtual INT    GetThreadPriority(HANDLE hThread, PHMHANDLEDATA pHMHandleData);
  virtual DWORD  SuspendThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData);
  virtual BOOL   SetThreadPriority(HANDLE hThread, PHMHANDLEDATA pHMHandleData, int priority);

  virtual BOOL   GetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, PCONTEXT lpContext);
  virtual BOOL   SetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, const CONTEXT *lpContext);
 
  virtual BOOL   TerminateThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, DWORD exitcode);
  virtual DWORD  ResumeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData);
  virtual BOOL   GetExitCodeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, LPDWORD lpExitCode);
  virtual BOOL   SetThreadTerminated(HANDLE hThread, PHMHANDLEDATA pHMHandleData);

  virtual BOOL   CloseHandle(PHMHANDLEDATA pHMHandleData);
};


#endif // __HMTHREAD_H__
