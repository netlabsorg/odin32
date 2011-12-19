/* $Id: oslibdebug.cpp,v 1.8 2004-01-30 22:17:00 bird Exp $ */

/*
 * OS/2 debug apis
 *
 * Copyright 2000 Sander van Leeuwen
 * Copyright 2000 Edgar Buerkle
 * Copyright 2000 Michal Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSQUEUES
#define INCL_DOSMODULEMGR
#define INCL_DOSEXCEPTIONS
#define INCL_DOSERRORS
#include <os2wrap.h>
#include <process.h>
#include <os2sel.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include "windllbase.h"
#include <winconst.h>
#include "oslibdebug.h"
#include <stdio.h>

#define DBG_LOCALLOG	DBG_oslibdebug
#include "dbglocal.h"
static int superpid = 0;

#define DEBUG_SEMNAME   "\\SEM32\\ODINTRACE\\"
#define DEBUG_QUEUENAME "\\QUEUES\\ODINTRACE\\"
#define DEBUG_QSEMNAME  "\\SEM32\\ODINTRACEQ\\"

#define DEBUG_WINQSEMNAME  "\\SEM32\\WINTRACEQ\\"
#define DEBUG_WINQUEUENAME "\\QUEUES\\WINTRACE\\"

#ifdef DEBUG
typedef struct
{
    LPCSTR pszMsg;
    UINT msg;
} MSGDESC, *PMSGDESC;

//
// Message description table.  Describes each message that can be spied on.
// This table must be kept in sorted order.
//
MSGDESC debugMsgs[] =
{
    { "EXCEPTION_DEBUG_EVENT", EXCEPTION_DEBUG_EVENT},
    { "CREATE_THREAD_DEBUG_EVENT", CREATE_THREAD_DEBUG_EVENT},                               // 0x0001
    { "CREATE_PROCESS_DEBUG_EVENT", CREATE_PROCESS_DEBUG_EVENT},
    { "EXIT_THREAD_DEBUG_EVENT", EXIT_THREAD_DEBUG_EVENT},
    { "EXIT_PROCESS_DEBUG_EVENT", EXIT_PROCESS_DEBUG_EVENT},                                   // 0x0005
    { "LOAD_DLL_DEBUG_EVENT", LOAD_DLL_DEBUG_EVENT},
    { "UNLOAD_DLL_DEBUG_EVENT", UNLOAD_DLL_DEBUG_EVENT},
    { "OUTPUT_DEBUG_STRING_EVENT", OUTPUT_DEBUG_STRING_EVENT},
    { "RIP_EVENT", RIP_EVENT}
};

INT gcMessages = sizeof(debugMsgs) / sizeof(MSGDESC);

const char *GetDebugMsgText(int Msg)
{
 static char msgtxt[64];
 int i;

  for(i=0;i<gcMessages;i++) {
        if(debugMsgs[i].msg == Msg)
                return(debugMsgs[i].pszMsg);
  }
  sprintf(msgtxt, "%s %X ","Unknown Message ", Msg);
  return(msgtxt);
}
#endif

//******************************************************************************
//******************************************************************************
VOID _Optlink DebugThread(VOID *argpid)
{
  BOOL   fTerminate       = FALSE;
  CHAR   QueueName[30]    = DEBUG_QUEUENAME;
  CHAR   WinQueueName[30] = DEBUG_WINQUEUENAME;
  CHAR   SemName[30]      = DEBUG_SEMNAME;
  CHAR   QSemName[30]     = DEBUG_QSEMNAME;
  CHAR   WinQSemName[30]  = DEBUG_WINQSEMNAME;
  HQUEUE QueueHandle      = 0;
  HQUEUE WinQueueHandle   = 0;
  HEV    hevSem           = 0,
         hevQSem          = 0,
         hevWinQSem	  = 0;	
  uDB_t  DbgBuf           = {0};
  int    rc, rc2;
  char   path[CCHMAXPATH];
  Win32DllBase *winmod;
  REQUESTDATA Request     = {0};
  LPDEBUG_EVENT lpde,lpde2;
  ULONG  *pid = (ULONG*)argpid;
  ULONG  staticPid = *pid;
  ULONG  ulDataLen = 0, ulElemCode = 0, ulNumCalled = 0;
  PVOID  DataBuffer;
  BYTE   Priority = 0;
  char   tmp[12];

  dprintf(("KERNEL32: DebugThread pid:%d", *pid));
  //------------ Output queue ----------------
  strcat(QueueName, itoa(*pid, tmp, 10));
  rc = DosCreateQueue( &QueueHandle , QUE_FIFO, QueueName);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create output queue:%s rc:%d", QueueName, rc));
     return;
  }
  dprintf(("DebugThread: Output queue %s created", QueueName));
  //------------ Odin internal queue ----------------
  strcat(WinQueueName, itoa(*pid, tmp, 10));
  rc = DosCreateQueue( &WinQueueHandle , QUE_FIFO, WinQueueName);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create Odin queue:%s rc:%d", WinQueueName, rc));
     return;
  }
  dprintf(("DebugThread: Odin internal win32 queue %s created", WinQueueName));
  //------------- Main Debug Semaphore -----------------
  strcat(SemName, itoa(*pid, tmp, 10));
  rc = DosCreateEventSem(SemName, &hevSem, 0, TRUE);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create main debug sem:%s rc:%d", SemName, rc));
     DosCloseQueue(QueueHandle);
     DosCloseQueue(WinQueueHandle);
     return;
  }
  dprintf(("DebugThread: Main debug semaphore %s created", SemName));

  //------------- Odin internal queue semaphor ---------------
  strcat(WinQSemName, itoa(*pid, tmp, 10));
  rc = DosCreateEventSem(WinQSemName, &hevWinQSem, 0, FALSE);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create odin internal queue sem:%s rc:%d", QSemName, rc));
     DosCloseEventSem(hevSem);
     DosCloseQueue(WinQueueHandle);
     DosCloseQueue(QueueHandle);
     return;
  }
  dprintf(("DebugThread: Odin internal queue semaphore %s created", WinQSemName));

  //------------- Output queue semaphor ---------------
  strcat(QSemName, itoa(*pid, tmp, 10));
  rc = DosCreateEventSem(QSemName, &hevQSem, 0, FALSE);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create event output queue sem:%s rc:%d", QSemName, rc));
     DosCloseEventSem(hevSem);
     DosCloseEventSem(hevWinQSem);
     DosCloseQueue(WinQueueHandle);
     DosCloseQueue(QueueHandle);
     return;
  }

  // connect to debuggee
  DbgBuf.Cmd = DBG_C_Connect;
  DbgBuf.Pid = *pid;
  DbgBuf.Tid = 0;
  DbgBuf.Value = DBG_L_386;
  DbgBuf.Addr  = 1;
  rc = DosDebug(&DbgBuf);
  if (rc != 0)
  {
    dprintf(("DosDebug error: rc = %d error:%d", rc, DbgBuf.Value));
    DosCloseQueue(WinQueueHandle);
    DosCloseQueue(QueueHandle);
    DosCloseEventSem(hevSem);
    return;
  }

  while (rc == 0)
  {
    DosWaitEventSem(hevSem, SEM_INDEFINITE_WAIT);
    DosResetEventSem(hevSem,&ulNumCalled);

DosDebug_GO:
    DbgBuf.Cmd = DBG_C_Go;
    DbgBuf.Pid = *pid;

    DebugApi:
    rc = DosDebug(&DbgBuf);
    if (rc != 0)
      dprintf(("DosDebug error: rc = %d", rc));

    if (fTerminate) // break out of the while loop
       break;

    switch (DbgBuf.Cmd)
    {
      case DBG_N_Success:
        dprintf(("DosDebug: GO ok"));
        goto DosDebug_GO;

      case DBG_N_Error:
        dprintf(("DosDebug: Error %d", DbgBuf.Value));
        // if(DbgBuf.Value == ERROR_INVALID_PROCID) connect ?
        if(DbgBuf.Value == ERROR_EXCL_SEM_ALREADY_OWNED)
        {
          rc = 0; // continue
          goto DosDebug_GO;
        }
        break;    // end thread !!!

      case DBG_N_ProcTerm:
        dprintf(("DosDebug: Process terminated with rc %d\n", DbgBuf.Value));
        lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
        lpde->dwDebugEventCode = EXIT_PROCESS_DEBUG_EVENT_W;
        lpde->dwProcessId = *pid;
        lpde->dwThreadId = 0;
        lpde->u.ExitThread.dwExitCode = DbgBuf.Value;
        DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        // We should now fire final DBG_C_Go and end processing. We shouldn't
        // get any more debug events.
        fTerminate = TRUE;
        // goto DosDebug_GO; *pid is invalid?!?
        DbgBuf.Cmd = DBG_C_Go;
        DbgBuf.Pid = staticPid;
        goto DebugApi;
        break;

      case DBG_N_Exception:
        dprintf(("DosDebug: Exception"));
        // lpde = malloc(sizeof(DEBUG_EVENT));
        // TODO: fill union
        // DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        // break;
        if (DbgBuf.Value == 0 && DbgBuf.Buffer == XCPT_BREAKPOINT)
        {
          dprintf(("Breakpoint encountered"));
          // This may be win32 event exception as well as common int3
          Priority = 0;
          ulElemCode = 0;
          rc2 = DosPeekQueue(WinQueueHandle,&Request, &ulDataLen, (PPVOID)&lpde, &ulElemCode,DCWW_NOWAIT, &Priority, hevWinQSem);
          if(rc2 == 0)
          {
             //There is a win32 event here
             rc = DosReadQueue(WinQueueHandle, &Request, &ulDataLen, (PPVOID) &lpde, 0, DCWW_NOWAIT,
                            &Priority, hevWinQSem);
             if (rc != 0)
             dprintf(("DebugThread - DosReadQueue failed!"));
             //Forward it to receiver
             lpde2 = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
             OSLibDebugReadMemory ( lpde, lpde2,sizeof(DEBUG_EVENT),NULL);
             #ifdef DEBUG
             dprintf(("DebugThread Win32 Event %s",GetDebugMsgText(lpde2->dwDebugEventCode)));
             #endif
             DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde2, 0);
             //Stay stopped
          }
          dprintf(("DebugThread - waiting for continue signal"));
          DosWaitEventSem(hevSem, SEM_INDEFINITE_WAIT);
          DosResetEventSem(hevSem,&ulNumCalled);
          DbgBuf.Cmd = DBG_C_ReadReg;
          rc = DosDebug(&DbgBuf);
          if (rc != 0)
             dprintf(("DosDebug error: rc = %d", rc));
          DbgBuf.EIP++;
          DbgBuf.Cmd = DBG_C_WriteReg;
          rc = DosDebug(&DbgBuf);
          if (rc != 0)
             dprintf(("DosDebug error: rc = %d", rc));
          DbgBuf.Cmd = DBG_C_Continue;
          DbgBuf.Value = XCPT_CONTINUE_EXECUTION;
          goto DebugApi;
       }
       DbgBuf.Cmd = DBG_C_Continue;
       DbgBuf.Value = XCPT_CONTINUE_SEARCH;
       goto DebugApi;

      case DBG_N_ModuleLoad:
        DosQueryModuleName(DbgBuf.Value, CCHMAXPATH, path);
        dprintf(("DosDebug: module loaded [%s]", path));

        winmod = Win32DllBase::findModule(path);
        // only odin32(win32) modules, hide OS/2 ones
        if(!winmod)
        {
          dprintf(("DosDebug: os/2 module [%s], suppress", path));
          goto DosDebug_GO;
        }
        dprintf(("DosDebug: win32 module [%s], inform", path));
        lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
        lpde->dwDebugEventCode = LOAD_DLL_DEBUG_EVENT_W;
        lpde->dwProcessId = *pid;
        lpde->dwThreadId = 0;
        lpde->u.LoadDll.hFile = 0;
        // TODO: make a pe fakeheader in our DLL's (kernel32,...)
        lpde->u.LoadDll.lpBaseOfDll = (PVOID)winmod->getInstanceHandle();
        lpde->u.LoadDll.dwDebugInfoFileOffset = 0;
        lpde->u.LoadDll.nDebugInfoSize = 0;
        lpde->u.LoadDll.lpImageName = path;
        lpde->u.LoadDll.fUnicode = FALSE;
        DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        break;

      case DBG_N_CoError:
        dprintf(("DosDebug: Coprocessor Error"));
        // TODO: create an exception ?
        goto DosDebug_GO;
        break;

      case DBG_N_ThreadTerm:
        dprintf(("DosDebug: Thread %d terminated with rc %d", DbgBuf.Tid,DbgBuf.Value));
        lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
        lpde->dwDebugEventCode = EXIT_THREAD_DEBUG_EVENT_W;
        lpde->dwProcessId = *pid;
        lpde->dwThreadId = DbgBuf.Tid;
        lpde->u.ExitThread.dwExitCode = DbgBuf.Value;
        DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        break;

      case DBG_N_AsyncStop:
        dprintf(("DosDebug: Async stop"));
        goto DosDebug_GO;
        break;

      case DBG_N_NewProc:
        dprintf(("DosDebug: Debuggee started new Pid %d",DbgBuf.Value));
        lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
        lpde->dwDebugEventCode = CREATE_PROCESS_DEBUG_EVENT_W;
        lpde->dwProcessId = *pid;
        lpde->dwThreadId = 0;
        //TODO: fill union
        lpde->u.CreateProcessInfo.hFile = 0;
        lpde->u.CreateProcessInfo.hProcess = 0;
        lpde->u.CreateProcessInfo.hThread = 0;
        lpde->u.CreateProcessInfo.lpBaseOfImage = NULL;
        lpde->u.CreateProcessInfo.dwDebugInfoFileOffset = 0;
        lpde->u.CreateProcessInfo.nDebugInfoSize = 0;
        lpde->u.CreateProcessInfo.lpThreadLocalBase = NULL;
        lpde->u.CreateProcessInfo.lpStartAddress = NULL;
        lpde->u.CreateProcessInfo.lpImageName = NULL;
        lpde->u.CreateProcessInfo.fUnicode = FALSE;
        DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        break;

      case DBG_N_AliasFree:
        dprintf(("DosDebug: AliasFree"));
        goto DosDebug_GO;
        break;

      case DBG_N_Watchpoint:
        dprintf(("DosDebug: WatchPoint"));
        goto DosDebug_GO;
        break;

      case DBG_N_ThreadCreate:
        // Note: Win32 debuggers expect a process creation event first!
        dprintf(("DosDebug: Thread %d created",DbgBuf.Tid));

        if (DbgBuf.Tid == 1) { // Is this the first thread of a process?
            // If so, fake a process creation event
            dprintf(("DosDebug: Faking process creation event"));
            lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
            lpde->dwDebugEventCode = CREATE_PROCESS_DEBUG_EVENT_W;
            lpde->dwProcessId = *pid;
            lpde->dwThreadId = 0;
            //TODO: fill union
            lpde->u.CreateProcessInfo.hFile = 0;
            lpde->u.CreateProcessInfo.hProcess = 0;
            lpde->u.CreateProcessInfo.hThread = 10;
            lpde->u.CreateProcessInfo.lpBaseOfImage = NULL;
            lpde->u.CreateProcessInfo.dwDebugInfoFileOffset = 0;
            lpde->u.CreateProcessInfo.nDebugInfoSize = 0;
            lpde->u.CreateProcessInfo.lpThreadLocalBase = NULL;
            lpde->u.CreateProcessInfo.lpStartAddress = NULL;
            lpde->u.CreateProcessInfo.lpImageName = NULL;
            lpde->u.CreateProcessInfo.fUnicode = FALSE;
            DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        }
        lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
        lpde->dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT_W;
        lpde->dwProcessId = *pid;
        lpde->dwThreadId = DbgBuf.Tid;
        //TODO: fill union
        lpde->u.CreateThread.hThread = 0;
        lpde->u.CreateThread.lpThreadLocalBase = NULL;
        lpde->u.CreateThread.lpStartAddress = NULL;
        DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        break;

      case DBG_N_ModuleFree:
        DosQueryModuleName(DbgBuf.Value, CCHMAXPATH, path);
        dprintf(("DosDebug: ModuleFree [%s]", path));
        winmod = Win32DllBase::findModule(path);
        // only odin32(win32) modules, hide OS/2 ones
        if(!winmod)
        {
          dprintf(("DosDebug: os/2 module [%s], suppress", path));
          goto DosDebug_GO;
        }
        lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
        lpde->dwDebugEventCode = UNLOAD_DLL_DEBUG_EVENT_W;
        lpde->dwProcessId = *pid;
        lpde->dwThreadId = 0;
        lpde->u.UnloadDll.lpBaseOfDll = (PVOID)winmod->getInstanceHandle();
        DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
        break;

      case DBG_N_RangeStep:
        dprintf(("DosDebug: RangeStep"));
        goto DosDebug_GO;
        break;

      default:
        dprintf(("DosDebug: Unkown Notify %d", DbgBuf.Cmd));
        goto DosDebug_GO;
        break;
    }
  }

  dprintf(("DosDebug - ending the service thread"));
  DosCloseQueue(QueueHandle);
  DosCloseQueue(WinQueueHandle);
  DosCloseEventSem(hevSem);
  DosCloseEventSem(hevQSem);
//  *pid = 0;  No can do - for some reason *pid is invalid by now
}

//******************************************************************************
//******************************************************************************
BOOL OSLibWaitForDebugEvent(LPDEBUG_EVENT lpde, DWORD dwTimeout)
{
  CHAR QueueName[30]=DEBUG_QUEUENAME;
  CHAR   SemName[30]=DEBUG_QSEMNAME;
  LPDEBUG_EVENT lpde_queue;
  int    rc, req;
  PID    pidOwner;
  HQUEUE QueueHandle=0;
  REQUESTDATA Request = {0};
  ULONG  len;
  BYTE   prio;
  HEV    hevQSem=0;
  char   tmp[12];
  USHORT sel = RestoreOS2FS();

  strcat(SemName, itoa(superpid,tmp, 10));
  rc = DosOpenEventSem(SemName, &hevQSem);
  if(rc != 0)
    goto fail;

  // get a DebugEvent from our DebugThread
  strcat(QueueName, itoa(superpid, tmp, 10));
  rc = DosOpenQueue(&pidOwner, &QueueHandle, QueueName);
  Request.pid = pidOwner;
  rc = DosReadQueue(QueueHandle, &Request, &len, (PPVOID) &lpde_queue, 0, DCWW_NOWAIT,
                    &prio, hevQSem);
  if(rc == ERROR_QUE_EMPTY)
  {
    if(DosWaitEventSem(hevQSem, dwTimeout) == 0)
      rc = DosReadQueue(QueueHandle, &Request, &len, (PPVOID) &lpde_queue, 0, DCWW_NOWAIT,
                        &prio, hevQSem);
  }
  if(rc != 0)
    goto fail;

  // copy DebugEvent to user space and free queue pointer
  memcpy(lpde, lpde_queue, len);
  // free our lpd
  free(lpde_queue);
  // DosCloseEventSem(hevSem);
  SetFS(sel);
  return TRUE;

fail:
  // DosCloseEventSem(hevSem);
  SetFS(sel);
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibContinueDebugEvent(DWORD dwProcessId, DWORD dwThreadId, DWORD dwContinueStatus)
{
  CHAR   SemName[30]=DEBUG_SEMNAME;
  CHAR   QueueName[30]=DEBUG_QUEUENAME;
  PID    pidOwner;
  HQUEUE QueueHandle=0;
  HEV    hev=0;
  int    rc;
  char   tmp[12];
  ULONG  QEntries=0;
  USHORT sel = RestoreOS2FS();

  // only continue DebugThread, if queue is empty
  strcat(QueueName, itoa(superpid, tmp, 10));
  rc = DosOpenQueue(&pidOwner, &QueueHandle, QueueName);
  rc = DosQueryQueue(QueueHandle, &QEntries);
  if(QEntries > 0) {
	SetFS(sel);
    	return TRUE;
  }
  // continue DebugThread
  strcat(SemName, itoa(superpid, tmp, 10));
  rc = DosOpenEventSem(SemName, &hev);
  if (rc != 0)
  {
    dprintf(("OSLibContinueDebugEvent: Failed to open even semaphore rc:%d",rc));
    return FALSE;
  }
  rc = DosPostEventSem(hev);
  if (rc != 0)
  {
    dprintf(("OSLibContinueDebugEvent: Failed to trigger semaphore rc:%d",rc));
    return FALSE;
  }
  // DosCloseEventSem(hev);
  SetFS(sel);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibAddModuleDebugEvent(char *name, BOOL fLoad)
{
  Win32DllBase  *winmod;
  LPDEBUG_EVENT lpde;
  int           rc;
  CHAR          QueueName[30]=DEBUG_QUEUENAME;
  PID           pidOwner;
  HQUEUE        QueueHandle=0;
  char          tmp[12];
  USHORT        sel = RestoreOS2FS();

  winmod = Win32DllBase::findModule(name);
  if(!winmod)
  {
    dprintf(("OSLibAddModuleDebugEvent: ERROR could not find module [%s]", name));
    SetFS(sel);
    return FALSE;
  }

//  strcat(QueueName, itoa(getDebuggerPID(), tmp, 10));
  rc = DosOpenQueue(&pidOwner, &QueueHandle, QueueName);

  if(fLoad == TRUE)
  {
    lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
    lpde->dwDebugEventCode = LOAD_DLL_DEBUG_EVENT_W;
    lpde->dwProcessId = getpid(); // debuggee pid
    lpde->dwThreadId = 0;
    lpde->u.LoadDll.hFile = 0;
    lpde->u.LoadDll.lpBaseOfDll = (PVOID)winmod->getInstanceHandle();
    lpde->u.LoadDll.dwDebugInfoFileOffset = 0;
    lpde->u.LoadDll.nDebugInfoSize = 0;
    lpde->u.LoadDll.lpImageName = name;
    lpde->u.LoadDll.fUnicode = FALSE;
    DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
  }
  else
  {
    lpde = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
    lpde->dwDebugEventCode = UNLOAD_DLL_DEBUG_EVENT_W;
    lpde->dwProcessId = getpid(); // debuggee pid
    lpde->dwThreadId = 0;
    lpde->u.UnloadDll.lpBaseOfDll = (PVOID)winmod->getInstanceHandle();
    DosWriteQueue(QueueHandle, 0, sizeof(DEBUG_EVENT), lpde, 0);
  }
  SetFS(sel);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
VOID OSLibStartDebugger(ULONG *pid)
{
 USHORT sel = RestoreOS2FS();
 TID tid;

   tid = _beginthread(DebugThread, NULL, 1024, (PVOID) pid);
   superpid = *pid;
   if (tid == 0)
   {
      dprintf(("OSLibStartDebugger: Could not create debug thread!"));
      SetFS(sel);
      return;
   }
   DosSleep(128);
   SetFS(sel);
}
//******************************************************************************
//******************************************************************************
VOID OSLibDebugReadMemory(LPCVOID lpBaseAddress,LPVOID lpBuffer, DWORD cbRead, LPDWORD lpNumberOfBytesRead)
{
  uDB_t  DbgBuf        = {0};
  USHORT sel = RestoreOS2FS();
  APIRET rc;
  dprintf(("OSLibDebugReadMemory - reading from pid %d",superpid));	
  DbgBuf.Pid = superpid;
  DbgBuf.Cmd = DBG_C_ReadMemBuf;
  DbgBuf.Addr  = (ULONG)lpBaseAddress;
  DbgBuf.Buffer  = (ULONG)lpBuffer;
  DbgBuf.Len  = cbRead;
  rc = DosDebug(&DbgBuf);
  if (rc != 0)
  {
    dprintf(("OSLibDebugReadMemory(DosDebug) error: rc = %d error:%d", rc, DbgBuf.Value));
    SetFS(sel);
    return;
  }
 if (lpNumberOfBytesRead)
  *lpNumberOfBytesRead = cbRead;
 SetFS(sel);
 return;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibAddWin32Event(LPDEBUG_EVENT lpde)
{
  uDB_t  DbgBuf        = {0};
  USHORT sel = RestoreOS2FS();
  APIRET rc;
  CHAR WinQueueName[30] = DEBUG_WINQUEUENAME;
  CHAR SemName[30]      = DEBUG_SEMNAME;
  HEV hevSem = 0;
  HANDLE WinQueueHandle;
  LPDEBUG_EVENT lpde_copy = NULL;
  char   tmp[12];
  PID    pidOwner;

  dprintf(("OSLibAddWin32Event"));	
  // open main debug semaphore
  strcat(SemName, itoa(getpid(),tmp, 10));
  rc = DosOpenEventSem(SemName, &hevSem);
  if(rc != 0)
  {
    dprintf(("OSLibAddWin32Event failed to open semaphore %s - rc %d",SemName, rc));
    goto fail;
  }

  // open Queues
  strcat(WinQueueName, itoa(getpid(), tmp, 10));
  rc = DosOpenQueue(&pidOwner, &WinQueueHandle, WinQueueName);
  if (rc != 0)
  {
    dprintf(("OSLibAddWin32Event failed to open queue - rc %d",rc));
    goto fail;
  }

  // copy data to our buffer
  lpde_copy = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
  memcpy(lpde_copy,lpde,sizeof(DEBUG_EVENT));
  rc = DosWriteQueue(WinQueueHandle, 0, sizeof(DEBUG_EVENT), lpde_copy, 0);
  if (rc !=0 )
  {
    dprintf(("OSLibAddWin32Event failed to write to queue - rc %d",rc));
    goto fail;
  }

  // and post notification
  rc = DosPostEventSem(hevSem);
  if (rc != 0)
  {
    dprintf(("OSLibAddWin32Event failed to trigger semaphore - rc %d",rc));
    goto fail;
  }
  _interrupt(3);
  free(lpde_copy);
  DosCloseEventSem(hevSem);
  DosCloseQueue(WinQueueHandle);
  SetFS(sel);
  return TRUE;
fail:
  if (lpde_copy) free(lpde_copy);
  DosCloseEventSem(hevSem);
  DosCloseQueue(WinQueueHandle);
  SetFS(sel);
  return FALSE;
}
