/* $Id: oslibdebug.cpp,v 1.6 2001-10-09 20:25:20 sandervl Exp $ */

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
#include <windllbase.h>
#include <winconst.h>
#include "oslibdebug.h"

#define DBG_LOCALLOG	DBG_oslibdebug
#include "dbglocal.h"

#define DEBUG_QUEUENAME "\\QUEUES\\ODINTRACE\\"
#define DEBUG_QSEMNAME  "\\SEM32\\ODINTRACEQ\\"
#define DEBUG_SEMNAME   "\\SEM32\\ODINTRACE\\"

//******************************************************************************
//******************************************************************************
VOID _Optlink DebugThread(VOID *argpid)
{
  BOOL   fTerminate    = FALSE;
  CHAR   QueueName[30] = DEBUG_QUEUENAME;
  CHAR   SemName[30]   = DEBUG_SEMNAME;
  CHAR   QSemName[30]  = DEBUG_QSEMNAME;
  HQUEUE QueueHandle   = 0;
  HEV    hevSem        = 0,
         hevQSem       = 0;
  uDB_t  DbgBuf        = {0};
  int    rc;
  char   path[CCHMAXPATH];
  Win32DllBase *winmod;
  LPDEBUG_EVENT lpde;
  ULONG  *pid = (ULONG*)argpid;
  ULONG  staticPid = *pid;
  char   tmp[12];

  dprintf(("KERNEL32: DebugThread pid:%d", *pid));

  strcat(QueueName, itoa(getpid(), tmp, 10));
  rc = DosCreateQueue( &QueueHandle , QUE_FIFO, QueueName);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create queue:%s rc:%d", QueueName, rc));
     return;
  }
  strcat(SemName, itoa(getpid(), tmp, 10));
  rc = DosCreateEventSem(SemName, &hevSem, 0, TRUE);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create event sem:%s rc:%d", SemName, rc));
     DosCloseQueue(QueueHandle);
     return;
  }
  strcat(QSemName, itoa(getpid(), tmp, 10));
  rc = DosCreateEventSem(QSemName, &hevQSem, 0, FALSE);
  if(rc != 0)
  {
     dprintf(("DebugThread: Could not create event sem:%s rc:%d", QSemName, rc));
     DosCloseEventSem(hevSem);
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
    DosCloseQueue(QueueHandle);
    DosCloseEventSem(hevSem);
    return;
  }

  while (rc == 0)
  {
    DosWaitEventSem(hevSem, SEM_INDEFINITE_WAIT);

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
            lpde->u.CreateProcessInfo.hThread = 0;
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
          break;
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

  strcat(SemName, itoa(getpid(),tmp, 10));
  rc = DosOpenEventSem(SemName, &hevQSem);
  if(rc != 0)
    goto fail;

  // get a DebugEvent from our DebugThread
  strcat(QueueName, itoa(getpid(), tmp, 10));
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
  strcat(QueueName, itoa(getpid(), tmp, 10));
  rc = DosOpenQueue(&pidOwner, &QueueHandle, QueueName);
  rc = DosQueryQueue(QueueHandle, &QEntries);
  if(QEntries > 0) {
	SetFS(sel);
    	return TRUE;
  }
  // continue DebugThread
  strcat(SemName, itoa(getpid(), tmp, 10));
  rc = DosOpenEventSem(SemName, &hev);
  rc = DosPostEventSem(hev);
  // DosCloseEventSem(hev);
  SetFS(sel);
  return (rc == 0) ? TRUE : FALSE;
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
