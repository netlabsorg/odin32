/* $Id: process.cpp,v 1.11 2004-02-10 15:37:54 sandervl Exp $ */

/*
 * Win32 process functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (OS/2 Port)
 *
 * TODO: Not done yet! (setting up process structures)
 *
 * Based on Wine code (scheduler\process.c)
 *
 * Copyright 1996, 1998 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winnt.h>
#include <winnls.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <wprocess.h>
#include <win/task.h>
#include "winimagebase.h"
#include "oslibdos.h"

#define DBG_LOCALLOG	DBG_process
#include "dbglocal.h"

#define SHUTDOWN_NORETRY 1

static unsigned int shutdown_noretry = 0;
static unsigned int shutdown_priority = 0x280L;
static        DWORD ProcessAffinityMask = 1;
static PDB *PROCESS_First = &ProcessPDB;

/***********************************************************************
 *           PROCESS_IdToPDB
 *
 * Convert a process id to a PDB, making sure it is valid.
 */
PDB *PROCESS_IdToPDB( DWORD id )
{
    PDB *pdb;

    if (!id) return PROCESS_Current();
    pdb = PROCESS_First;
    while (pdb)
    {
        if ((DWORD)pdb->server_pid == id) return pdb;
        pdb = pdb->next;
    }
    SetLastError( ERROR_INVALID_PARAMETER );
    return NULL;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API OpenProcess(DWORD arg1, BOOL arg2, DWORD arg3)
{
    dprintf(("KERNEL32:  OS2OpenProcess\n"));
    return O32_OpenProcess(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetExitCodeProcess(HANDLE hProcess, LPDWORD  arg2)
{
 BOOL rc;

    rc = O32_GetExitCodeProcess(hProcess, arg2);
    dprintf(("KERNEL32:  GetExitCodeProcess %x returned %d (%x)", hProcess, rc, (arg2) ? *arg2 : 0));
    return rc;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetCurrentProcess(void)
{
    dprintf2(("KERNEL32:  GetCurrentProcess\n"));
    return O32_GetCurrentProcess();
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetCurrentProcessId(void)
{
    dprintf2(("KERNEL32:  GetCurrentProcessId\n"));
    return O32_GetCurrentProcessId();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TerminateProcess( HANDLE arg1, DWORD arg2)
{
    dprintf(("KERNEL32:  TerminateProcess\n"));
    return O32_TerminateProcess(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetProcessVersion(DWORD Processid)
{
 Win32ImageBase *image;
 PDB *process = PROCESS_IdToPDB( Processid );
 DWORD version;

  if(process == NULL) {
  	dprintf(("GetProcessVersion: can't find process (%d)", Processid));
	return 0;
  }
  image = Win32ImageBase::findModule(process->hInstance);
  if(image) {
	version = image->getVersion();
  	dprintf(("GetProcessVersion of %x = %x", Processid, version));
  	return version;
  }
  dprintf(("GetProcessVersion: can't find module %x (%d)", process->hInstance, Processid));
  return 0;
}
/***********************************************************************
 *          SetProcessAffinityMask   (KERNEL32.662)
 */
BOOL WINAPI SetProcessAffinityMask( HANDLE hProcess, DWORD affmask )
{
    dprintf(("SetProcessAffinityMask %x %x", hProcess, affmask));
    ProcessAffinityMask = affmask;
    //TODO: should update all threads, but that doesn't seem possible in OS/2
    SetThreadAffinityMask(GetCurrentThread(), ProcessAffinityMask);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetProcessAffinityMask(HANDLE  hProcess,
                                     LPDWORD lpProcessAffinityMask,
                                     LPDWORD lpSystemAffinityMask)
{
    dprintf(("GetProcessAffinityMask %x %x %x", hProcess, lpProcessAffinityMask, lpSystemAffinityMask));

    /* It is definitely important for a process to know on what processor
       it is running :-) */
    if(lpProcessAffinityMask)
    	*lpProcessAffinityMask=ProcessAffinityMask;

    if(lpSystemAffinityMask) {
        OSLibDosQueryAffinity(MASK_SYSTEM, lpSystemAffinityMask);
    }
    return TRUE;
}
/***********************************************************************
 * GetProcessHeaps [KERNEL32.376]
 */
DWORD WINAPI GetProcessHeaps(DWORD nrofheaps,HANDLE *heaps)
{
    dprintf(("GetProcessHeaps: (%ld,%p), incomplete implementation.\n",nrofheaps,heaps));

    if (nrofheaps) {
	heaps[0] = GetProcessHeap();
	/* ... probably SystemHeap too ? */
	return 1;
    }
    /* number of available heaps */
    return 1;
}
/***********************************************************************
 *           RegisterServiceProcess             (KERNEL, KERNEL32)
 *
 * A service process calls this function to ensure that it continues to run
 * even after a user logged off.
 */
DWORD WINAPI RegisterServiceProcess(DWORD dwProcessId, DWORD dwType)
{
    dprintf(("RegisterServiceProcess %x %x", dwProcessId, dwType));
    /* I don't think that Wine needs to do anything in that function */
    return 1; /* success */
}
/***********************************************************************
 * Name      : BOOL SetProcessShutdownParameters
 * Purpose   : The SetProcessShutdownParameters function sets shutdown parameters
 *             for the currently calling process. This function sets a shutdown
 *             order for a process relative to the other processes in the system.
 * Parameters: DWORD dwLevel  shutdown priority
 *             DWORD dwFlags  shutdown flags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 *
 *           SetProcessShutdownParameters    (KERNEL32)
 *
 * CHANGED - James Sutherland (JamesSutherland@gmx.de)
 * Now tracks changes made (but does not act on these changes)
 * NOTE: the definition for SHUTDOWN_NORETRY was done on guesswork.
 * It really shouldn't be here, but I'll move it when it's been checked!
 */
BOOL WINAPI SetProcessShutdownParameters(DWORD level,DWORD flags)
{
    if (flags & SHUTDOWN_NORETRY)
      shutdown_noretry = 1;
    else
      shutdown_noretry = 0;
    if (level > 0x100L && level < 0x3FFL)
      shutdown_priority = level;
    else
      {
	dprintf(("SetProcessShutdownParameters: invalid priority level 0x%08lx\n", level));
	return FALSE;
      }
    return TRUE;
}
/***********************************************************************
 * GetProcessShutdownParameters                 (KERNEL32)
 * Name      : BOOL GetProcessShutdownParameters
 * Purpose   : The GetProcessShutdownParameters function retrieves shutdown
 *             parameters for the currently calling process.
 * Parameters: LPDWORD lpdwLevel
 *             LPDWORD lpdwFlags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 *
 */
BOOL WINAPI GetProcessShutdownParameters( LPDWORD lpdwLevel,
	  		 	          LPDWORD lpdwFlags )
{
    dprintf(("GetProcessShutdownParameters"));
    (*lpdwLevel) = shutdown_priority;
    (*lpdwFlags) = (shutdown_noretry * SHUTDOWN_NORETRY);
    return TRUE;
}
/***********************************************************************
 *           SetProcessPriorityBoost    (KERNEL32)
 */
BOOL WINAPI SetProcessPriorityBoost(HANDLE hprocess,BOOL disableboost)
{
    dprintf(("SetProcessPriorityBoost: (%d,%d): stub\n",hprocess,disableboost));
    /* Say we can do it. I doubt the program will notice that we don't. */
    return TRUE;
}
/***********************************************************************
 *		SetProcessWorkingSetSize	[KERNEL32.662]
 * Sets the min/max working set sizes for a specified process.
 *
 * PARAMS
 *    hProcess [I] Handle to the process of interest
 *    minset   [I] Specifies minimum working set size
 *    maxset   [I] Specifies maximum working set size
 *
 * RETURNS  STD
 */
BOOL WINAPI SetProcessWorkingSetSize(HANDLE hProcess,DWORD minset,
                                       DWORD maxset)
{
    dprintf(("SetProcessWorkingSetSize (0x%08x,%ld,%ld): stub - harmless\n",hProcess,minset,maxset));
    if(( minset == -1) && (maxset == -1)) {
        /* Trim the working set to zero */
        /* Swap the process out of physical RAM */
    }
    return TRUE;
}

/***********************************************************************
 *           GetProcessFlags    (KERNEL32)
 */
DWORD WINAPI GetProcessFlags( DWORD processid )
{
//    PDB *pdb = PROCESS_IdToPDB( processid );
//    if (!pdb) return 0;
//    return pdb->flags;
    dprintf(("STUB: GetProcessFlags %x", processid));
    return 0;
}

/***********************************************************************
 *           GetProcessWorkingSetSize    (KERNEL32)
 * Name      : BOOL SetProcessWorkingSetSize
 * Purpose   : The SetProcessWorkingSetSize function sets the minimum and
 *             maximum working set sizes for a specified process.
 *             The working set of a process is the set of memory pages currently
 *             visible to the process in physical RAM memory. These pages are
 *             resident and available for an application to use without triggering
 *             a page fault. The size of the working set of a process is specified
 *             in bytes. The minimum and maximum working set sizes affect the
 *             virtual memory paging behavior of a process.
 * Parameters: HANDLE hProcess                open handle to the process of interest
 *             DWORD  dwMinimumWorkingSetSize specifies minimum working set size
 *             DWORD  dwMaximumWorkingSetSize specifies maximum working set size
 * Variables :
 * Result    : TRUE / FALSE
 */
BOOL WINAPI GetProcessWorkingSetSize(HANDLE hProcess,LPDWORD minset,
                                     LPDWORD maxset)
{
	dprintf(("GetProcessWorkingSetSize 0x%08x,%p,%p): stub\n",hProcess,minset,maxset));
	/* 32 MB working set size */
	if (minset) *minset = 32*1024*1024;
	if (maxset) *maxset = 32*1024*1024;
	return TRUE;
}
/***********************************************************************
 *           GetProcessDword    (KERNEL32.18) (KERNEL.485)
 * 'Of course you cannot directly access Windows internal structures'
 */
DWORD WINAPI GetProcessDword( DWORD dwProcessID, INT offset )
{
    PDB *process = PROCESS_IdToPDB( dwProcessID );
    TDB *pTask;
    DWORD x, y;

    dprintf(("GetProcessDword: (%ld, %d)\n", dwProcessID, offset ));
    if ( !process ) return 0;

    switch ( offset )
    {
    case GPD_APP_COMPAT_FLAGS:
        pTask = (TDB *)GlobalLock( process->task );
        return pTask? pTask->compat_flags : 0;

    case GPD_LOAD_DONE_EVENT:
        return process->load_done_evt;

    case GPD_HINSTANCE16:
        pTask = (TDB *)GlobalLock( process->task );
        return pTask? pTask->hInstance : 0;

    case GPD_WINDOWS_VERSION:
        pTask = (TDB *)GlobalLock( process->task );
        return pTask? pTask->version : 0;

    case GPD_THDB:
        if ( process != PROCESS_Current() ) return 0;
        return (DWORD)GetThreadTEB() + 0x30;

    case GPD_PDB:
        return (DWORD)process;

    case GPD_STARTF_SHELLDATA: /* return stdoutput handle from startupinfo ??? */
        return process->env_db->startup_info->hStdOutput;

    case GPD_STARTF_HOTKEY: /* return stdinput handle from startupinfo ??? */
        return process->env_db->startup_info->hStdInput;

    case GPD_STARTF_SHOWWINDOW:
        return process->env_db->startup_info->wShowWindow;

    case GPD_STARTF_SIZE:
        x = process->env_db->startup_info->dwXSize;
        if ( x == CW_USEDEFAULT ) x = CW_USEDEFAULT16;
        y = process->env_db->startup_info->dwYSize;
        if ( y == CW_USEDEFAULT ) y = CW_USEDEFAULT16;
        return MAKELONG( x, y );

    case GPD_STARTF_POSITION:
        x = process->env_db->startup_info->dwX;
        if ( x == CW_USEDEFAULT ) x = CW_USEDEFAULT16;
        y = process->env_db->startup_info->dwY;
        if ( y == CW_USEDEFAULT ) y = CW_USEDEFAULT16;
        return MAKELONG( x, y );

    case GPD_STARTF_FLAGS:
        return process->env_db->startup_info->dwFlags;

    case GPD_PARENT:
	if(process->parent)
        	return (DWORD)process->parent->server_pid;
	return 0;

    case GPD_FLAGS:
        return process->flags;

    case GPD_USERDATA:
        return process->process_dword;

    default:
        dprintf(("GetProcessDword: Unknown offset %d\n", offset ));
        return 0;
    }
}


/***********************************************************************
 *           ODIN_SetProcessDword
 *
 * SvL: Special version that allows the caller to change some values
 *
 */
void WINAPI ODIN_SetProcessDword( DWORD dwProcessID, INT offset, DWORD value )
{
    PDB *process = PROCESS_IdToPDB( dwProcessID );

    dprintf(("SetProcessDword: (%ld, %d)\n", dwProcessID, offset));
    if ( !process ) return;

    switch ( offset )
    {
    case GPD_STARTF_SHOWWINDOW:
        process->env_db->startup_info->wShowWindow = value;
        break;

    case GPD_STARTF_SIZE:
        process->env_db->startup_info->dwXSize = LOWORD(value);
        process->env_db->startup_info->dwYSize = HIWORD(value);
        break;

    case GPD_STARTF_POSITION:
        process->env_db->startup_info->dwX = LOWORD(value);
        process->env_db->startup_info->dwY = HIWORD(value);
        break;

    default:
        dprintf(("SetProcessDword: Unknown offset %d\n", offset));
        break;
    }
}

/***********************************************************************
 *           SetProcessDword    (KERNEL.484)
 * 'Of course you cannot directly access Windows internal structures'
 */
void WINAPI SetProcessDword( DWORD dwProcessID, INT offset, DWORD value )
{
    PDB *process = PROCESS_IdToPDB( dwProcessID );

    dprintf(("SetProcessDword: (%ld, %d)\n", dwProcessID, offset));
    if ( !process ) return;

    switch ( offset )
    {
    case GPD_APP_COMPAT_FLAGS:
    case GPD_LOAD_DONE_EVENT:
    case GPD_HINSTANCE16:
    case GPD_WINDOWS_VERSION:
    case GPD_THDB:
    case GPD_PDB:
    case GPD_STARTF_SHELLDATA:
    case GPD_STARTF_HOTKEY:
    case GPD_STARTF_SHOWWINDOW:
    case GPD_STARTF_SIZE:
    case GPD_STARTF_POSITION:
    case GPD_STARTF_FLAGS:
    case GPD_PARENT:
    case GPD_FLAGS:
        dprintf(("SetProcessDword: Not allowed to modify offset %d\n", offset ));
        break;

    case GPD_USERDATA:
        process->process_dword = value;
        break;

    default:
        dprintf(("SetProcessDword: Unknown offset %d\n", offset));
        break;
    }
}
/*****************************************************************************
 * Name      : BOOL GetProcessTimes
 * Purpose   : The GetProcessTimes function obtains timing information about a specified process.
 * Parameters: HANDLE     hProcess       specifies the process of interest
 *             LPFILETIME lpCreationTime when the process was created
 *             LPFILETIME lpExitTime     when the process exited
 *             LPFILETIME lpKernelTime   time the process has spent in kernel mode
 *             LPFILETIME lpUserTime     time the process has spent in user mode
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

ULONG (WINAPI *NtdllRtlExtendedIntegerMultiply)(LARGE_INTEGER factor1,
                                                INT    factor2) = 0;

BOOL WIN32API GetProcessTimes(HANDLE     hProcess,
                              LPFILETIME lpCreationTime,
                              LPFILETIME lpExitTime,
                              LPFILETIME lpKernelTime,
                              LPFILETIME lpUserTime)
{
 LARGE_INTEGER *kerneltime, *usertime;

  dprintf(("Kernel32: GetProcessTimes(%08xh,%08xh,%08xh,%08xh,%08xh) partly implemented",
           hProcess,
           lpCreationTime,
           lpExitTime,
           lpKernelTime,
           lpUserTime));

  if(!NtdllRtlExtendedIntegerMultiply) {
   	HINSTANCE hInstance = LoadLibraryA("NTDLL.DLL");
   	if(hInstance)
    		*(VOID **)&NtdllRtlExtendedIntegerMultiply=(void*)GetProcAddress(hInstance, (LPCSTR)"RtlExtendedIntegerMultiply");
  }
  if(!lpCreationTime || !lpExitTime || !lpKernelTime || !lpUserTime) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  if(hProcess != GetCurrentProcess()) {
	dprintf(("GetProcessTimes unknown process"));
	return FALSE;
  }

  SystemTimeToFileTime(&ProcessPDB.creationTime, lpCreationTime);
  memset(lpExitTime, 0, sizeof(FILETIME));
  memset(lpKernelTime, 0, sizeof(FILETIME));
  memset(lpUserTime, 0, sizeof(FILETIME));

  kerneltime = (LARGE_INTEGER *)lpKernelTime;
  usertime   = (LARGE_INTEGER *)lpUserTime;
  OSLibDosQueryProcTimes(GetCurrentProcessId(), &lpKernelTime->dwLowDateTime, &lpUserTime->dwLowDateTime);
  //TODO: Isn't correct -> (if result's high dword != 0)
  kerneltime->LowPart = NtdllRtlExtendedIntegerMultiply(*kerneltime, 10);
  usertime->LowPart   = NtdllRtlExtendedIntegerMultiply(*usertime, 10);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
