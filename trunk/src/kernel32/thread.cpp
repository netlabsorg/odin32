/* $Id: thread.cpp,v 1.31 2001-06-27 19:09:35 sandervl Exp $ */

/*
 * Win32 Thread API functions
 *
 * TODO: Initialize threadInfo structure during thread creation
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdarg.h>
#include <string.h>
#include "thread.h"
#include <misc.h>
#include <cpuhlp.h>
#include <wprocess.h>
#include <windllbase.h>
#include <winexebase.h>
#include "exceptutil.h"
#include "oslibmisc.h"
#include <handlemanager.h>

#define DBG_LOCALLOG	DBG_thread
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-THREAD)

//******************************************************************************
//******************************************************************************
DWORD WIN32API GetCurrentThreadId()
{
////  dprintf(("GetCurrentThreadId\n"));
  return(O32_GetCurrentThreadId());
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetCurrentThread()
{
 TEB *teb;

    teb = GetThreadTEB();
    if(teb == 0) {
	SetLastError(ERROR_INVALID_HANDLE); //todo 
	return 0;
    }
    return teb->o.odin.hThread;
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitThread(DWORD exitcode)
{
 EXCEPTION_FRAME *exceptFrame;
 TEB             *teb;

  dprintf(("ExitThread %x (%x)", GetCurrentThread(), exitcode));

  teb = GetThreadTEB();
  if(teb != 0) {
	exceptFrame = (EXCEPTION_FRAME *)teb->o.odin.exceptFrame;
  }
  else  DebugInt3();

  HMSetThreadTerminated(GetCurrentThread());
  Win32DllBase::detachThreadFromAllDlls(); 	//send DLL_THREAD_DETACH message to all dlls
  Win32DllBase::tlsDetachThreadFromAllDlls(); //destroy TLS structures of all dlls
  WinExe->tlsDetachThread();			//destroy TLS structure of main exe
  DestroyTIB();

  if(exceptFrame) OS2UnsetExceptionHandler((void *)exceptFrame);

  O32_ExitThread(exitcode);
}
//******************************************************************************
//******************************************************************************
Win32Thread::Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData, DWORD dwFlags, HANDLE hThread)
{
  lpUserData = lpData;
  pCallback  = pUserCallback;
  this->dwFlags = dwFlags;
  this->hThread = hThread;
}
//******************************************************************************
//******************************************************************************
DWORD OPEN32API Win32ThreadProc(LPVOID lpData)
{
 EXCEPTION_FRAME exceptFrame;
 Win32Thread     *me = (Win32Thread *)lpData;
 ULONG            winthread = (ULONG)me->pCallback;
 LPVOID           userdata  = me->lpUserData;
 HANDLE           hThread   = me->hThread;
 DWORD            rc;

  delete(me);    //only called once
  dprintf(("Win32ThreadProc %d\n", GetCurrentThreadId()));

  TEB *winteb = (TEB *)InitializeTIB();
  if(winteb == NULL) {
	dprintf(("Win32ThreadProc: InitializeTIB failed!!"));
	DebugInt3();
	return 0;
  }
  winteb->flags = me->dwFlags;

  winteb->entry_point = (void *)winthread;
  winteb->entry_arg   = (void *)userdata;
  winteb->o.odin.hThread = hThread;

  winteb->o.odin.hab = OSLibWinInitialize();
  winteb->o.odin.hmq = OSLibWinQueryMsgQueue(winteb->o.odin.hab);
  dprintf(("Win32ThreadProc: hab %x hmq %x", winteb->o.odin.hab, winteb->o.odin.hmq));

  //Note: The Win32 exception structure referenced by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);
  winteb->o.odin.exceptFrame = (ULONG)&exceptFrame;

  SetWin32TIB();
  WinExe->tlsAttachThread();		  //setup TLS structure of main exe
  Win32DllBase::tlsAttachThreadToAllDlls(); //setup TLS structures of all dlls
  Win32DllBase::attachThreadToAllDlls();	  //send DLL_THREAD_ATTACH message to all dlls

  //Set FPU control word to 0x27F (same as in NT)
  CONTROL87(0x27F, 0xFFF);
  rc = AsmCallThreadHandler(winthread, userdata);

  HMSetThreadTerminated(GetCurrentThread());
  winteb->o.odin.exceptFrame = 0;
  Win32DllBase::detachThreadFromAllDlls();  //send DLL_THREAD_DETACH message to all dlls
  Win32DllBase::tlsDetachThreadFromAllDlls(); //destroy TLS structures of all dlls
  WinExe->tlsDetachThread();		  //destroy TLS structure of main exe
  DestroyTIB();
  OS2UnsetExceptionHandler((void *)&exceptFrame);

  return rc;
}
//******************************************************************************
//******************************************************************************
