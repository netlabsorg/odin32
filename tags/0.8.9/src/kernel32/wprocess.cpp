/* $Id: wprocess.cpp,v 1.194 2004-02-24 11:46:10 sandervl Exp $ */

/*
 * Win32 process functions
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 *
 * NOTE: Even though Odin32 OS/2 apps don't switch FS selectors,
 *       we still allocate a TEB to store misc information.
 *
 * TODO: What happens when a dll is first loaded as LOAD_LIBRARY_AS_DATAFILE
 *       and then for real? (first one not freed of course)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <odin.h>
#include <odinwrap.h>
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __KLIBC__
#include <klibc/startup.h>
#endif

#include <unicode.h>
#include "windllbase.h"
#include "winexebase.h"
#include "windllpeldr.h"
#include "winexepeldr.h"
#include "windlllx.h"
#include <vmutex.h>
#include <handlemanager.h>
#include <odinpe.h>

#include "odin32validate.h"
#include "exceptutil.h"
#include "asmutil.h"
#include "oslibdos.h"
#include "oslibmisc.h"
#include "oslibdebug.h"
#include "hmcomm.h"

#include "console.h"
#include "wincon.h"
#include "versionos2.h"    /*PLF Wed  98-03-18 02:36:51*/
#include <wprocess.h>
#include "mmap.h"
#include "initterm.h"
#include "directory.h"
#include "shellapi.h"

#include <win/ntddk.h>
#include <win/psapi.h>
#include <win/options.h>

#include <custombuild.h>

#define DBG_LOCALLOG    DBG_wprocess
#include "dbglocal.h"

#ifdef PROFILE
#include <perfview.h>
#include <profiler.h>
#endif /* PROFILE */


ODINDEBUGCHANNEL(KERNEL32-WPROCESS)


//environ.cpp
char *CreateNewEnvironment(char *lpEnvironment);

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
BOOL    fIsOS2Image = FALSE;            /* TRUE  -> Odin32 OS/2 application (not converted!) */
                                        /* FALSE -> otherwise */
BOOL    fSwitchTIBSel = FALSE;          // TRUE  -> switch TIB selectors
                                        // FALSE -> don't
BOOL    fForceWin32TIB = FALSE;         // TRUE  -> force TIB switch
                                        // FALSE -> not enabled
BOOL    fExitProcess = FALSE;

//Commandlines
PCSTR   pszCmdLineA;                    /* ASCII/ANSII commandline. */
PCWSTR  pszCmdLineW;                    /* Unicode commandline. */
char    **__argvA = NULL;               /* command line arguments in ANSI */
int     __argcA = 0;                    /* number of arguments in __argcA */

//Process database
PDB          ProcessPDB = {0};
ENVDB        ProcessENVDB = {0};
CONCTRLDATA  ProcessConCtrlData = {0};
STARTUPINFOA StartupInfo = {0};
CHAR         unknownPDBData[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 ,0 ,0};
USHORT       ProcessTIBSel = 0;
DWORD       *TIBFlatPtr    = 0;

//list of thread database structures
static TEB      *threadList = 0;
static VMutex    threadListMutex;

/**
 * LoadLibraryExA callback for LX Dlls, it's call only on the initial load.
 * Maintained by ODIN_SetLxDllLoadCallback().
 * Note! Because of some hacks it may also be called from Win32LxDll::Release().
 */
PFNLXDLLLOAD pfnLxDllLoadCallback = NULL;

extern "C" {

//******************************************************************************
//******************************************************************************
VOID WIN32API ForceWin32TIB()
{
    if(!fForceWin32TIB) {
        ODIN_SetTIBSwitch(TRUE);
        fForceWin32TIB = TRUE;
    }
    return;
}
//******************************************************************************
//******************************************************************************
TEB *WIN32API GetThreadTEB()
{
    if(TIBFlatPtr == NULL) {
        return 0;
    }
    return (TEB *)*TIBFlatPtr;
}
//******************************************************************************
//******************************************************************************
TEB *WIN32API GetTEBFromThreadId(ULONG threadId)
{
 TEB *teb = threadList;

    threadListMutex.enter();
    while(teb) {
        if(teb->o.odin.threadId == threadId) {
            break;
        }
        teb = teb->o.odin.next;
    }
    threadListMutex.leave();
    return teb;
}
//******************************************************************************
//******************************************************************************
TEB *WIN32API GetTEBFromThreadHandle(HANDLE hThread)
{
 TEB *teb = threadList;

    threadListMutex.enter();
    while(teb) {
        if(teb->o.odin.hThread == hThread) {
            break;
        }
        teb = teb->o.odin.next;
    }
    threadListMutex.leave();
    return teb;
}
//******************************************************************************
//Allocate TEB structure for new thread
//******************************************************************************
TEB *WIN32API CreateTEB(HANDLE hThread, DWORD dwThreadId)
{
    USHORT tibsel;
    TEB   *winteb;

    if(OSLibAllocSel(sizeof(TEB), &tibsel) == FALSE)
    {
        dprintf(("InitializeTIB: selector alloc failed!!"));
        DebugInt3();
        return NULL;
    }
    winteb = (TEB *)OSLibSelToFlat(tibsel);
    if(winteb == NULL)
    {
        dprintf(("InitializeTIB: DosSelToFlat failed!!"));
        DebugInt3();
        return NULL;
    }
    memset(winteb, 0, sizeof(TEB));
    dprintf(("TIB selector %x; linaddr 0x%x", tibsel, winteb));

    threadListMutex.enter();
    TEB *teblast   = threadList;
    if(!teblast) {
        threadList = winteb;
        winteb->o.odin.next = NULL;
    }
    else {
        while(teblast->o.odin.next) {
            teblast = teblast->o.odin.next;
        }
        teblast->o.odin.next = winteb;
    }
    threadListMutex.leave();

    winteb->except      = (PVOID)-1;               /* 00 Head of exception handling chain */
    winteb->htask16     = (USHORT)OSLibGetPIB(PIB_TASKHNDL); /* 0c Win16 task handle */
    winteb->stack_sel   = getSS();                 /* 0e 16-bit stack selector */
    winteb->self        = winteb;                  /* 18 Pointer to this structure */
    winteb->flags       = TEBF_WIN32;              /* 1c Flags */
    winteb->queue       = 0;                       /* 28 Message queue */
    winteb->tls_ptr     = &winteb->tls_array[0];   /* 2c Pointer to TLS array */
    winteb->process     = &ProcessPDB;             /* 30 owning process (used by NT3.51 applets)*/
    winteb->delta_priority  = THREAD_PRIORITY_NORMAL;
    winteb->process     = &ProcessPDB;

    //store selector of new TEB
    winteb->teb_sel = tibsel;

    winteb->o.odin.hThread  = hThread;
    winteb->o.odin.threadId = dwThreadId;

    // Event semaphore (auto-reset) to signal message post to MsgWaitForMultipleObjects
    winteb->o.odin.hPostMsgEvent = CreateEventA(NULL, FALSE, FALSE, NULL);

    return winteb;
}
//******************************************************************************
// Set up the TIB selector and memory for the main thread
//******************************************************************************
TEB *WIN32API InitializeMainThread()
{
    HANDLE hThreadMain;
    TEB   *teb;

    //Allocate one dword to store the flat address of our TEB
    dprintf(("InitializeMainThread Process handle %x, id %x", GetCurrentProcess(), GetCurrentProcessId()));

    TIBFlatPtr = (DWORD *)OSLibAllocThreadLocalMemory(1);
    if(TIBFlatPtr == 0) {
        dprintf(("InitializeTIB: local thread memory alloc failed!!"));
        DebugInt3();
        return NULL;
    }
    //SvL: This doesn't really create a thread, but only sets up the
    //     handle of thread 0
    hThreadMain = HMCreateThread(NULL, 0, 0, 0, 0, 0, TRUE);

    //create and initialize TEB
    teb = CreateTEB(hThreadMain, GetCurrentThreadId());
    if(teb == NULL || InitializeThread(teb, TRUE) == FALSE) {
        DebugInt3();
        return NULL;
    }

    ProcessTIBSel              = teb->teb_sel;

    //todo initialize PDB during process creation
    //todo: initialize TLS array if required
    //TLS in executable always TLS index 0?
////        ProcessPDB.exit_code       = 0x103; /* STILL_ACTIVE */
    ProcessPDB.threads         = 1;
    ProcessPDB.running_threads = 1;
    ProcessPDB.ring0_threads   = 1;
    ProcessPDB.system_heap     = GetProcessHeap();
    ProcessPDB.parent          = 0;
    ProcessPDB.group           = &ProcessPDB;
    ProcessPDB.priority        = 8;  /* Normal */
    ProcessPDB.heap            = ProcessPDB.system_heap;  /* will be changed later on */
    ProcessPDB.next            = NULL;
    ProcessPDB.winver          = 0xffff; /* to be determined */
    ProcessPDB.server_pid      = (void *)GetCurrentProcessId();
    ProcessPDB.tls_bits[0]     = 0; //all tls slots are free
    ProcessPDB.tls_bits[1]     = 0;

    GetSystemTime(&ProcessPDB.creationTime);

    /* Initialize the critical section */
    InitializeCriticalSection(&ProcessPDB.crit_section );

    //initialize the environment db entry.
    ProcessPDB.env_db          = &ProcessENVDB;
    ProcessENVDB.startup_info  = &StartupInfo;
    ProcessENVDB.environ       = GetEnvironmentStringsA();
    ProcessENVDB.cmd_line      = (CHAR*)(void*)pszCmdLineA;
    ProcessENVDB.cmd_lineW     = (WCHAR*)(void*)pszCmdLineW;
    ProcessENVDB.break_handlers = &ProcessConCtrlData;
    ProcessConCtrlData.fIgnoreCtrlC = FALSE; /* TODO! Should be inherited from parent. */
    ProcessConCtrlData.pHead = ProcessConCtrlData.pTail = (PCONCTRL)malloc(sizeof(CONCTRL));
    ProcessConCtrlData.pHead->pfnHandler = (void*)DefaultConsoleCtrlHandler;
    ProcessConCtrlData.pHead->pNext = ProcessConCtrlData.pHead->pPrev = NULL;
    ProcessConCtrlData.pHead->flFlags = ODIN32_CONCTRL_FLAGS_INIT;
    InitializeCriticalSection(&ProcessENVDB.section);

    ProcessPDB.unknown10       = (PVOID)&unknownPDBData[0];
    //initialize the startup info part of the db entry.
    O32_GetStartupInfo(&StartupInfo);
    StartupInfo.cb             = sizeof(StartupInfo);
    /* Set some defaults as GetStartupInfo() used to set... */
    if (!(StartupInfo.dwFlags & STARTF_USESHOWWINDOW))
        StartupInfo.wShowWindow= SW_NORMAL;
    /* must be NULL for VC runtime */
    StartupInfo.lpReserved     = NULL;
    StartupInfo.cbReserved2     = NULL;
    if (!StartupInfo.lpDesktop)
        StartupInfo.lpDesktop  = (LPSTR)"Desktop";
    if (!StartupInfo.lpTitle)
        StartupInfo.lpTitle    = (LPSTR)"Title";
    ProcessENVDB.hStdin  = StartupInfo.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
    ProcessENVDB.hStdout = StartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    ProcessENVDB.hStderr = StartupInfo.hStdError  = GetStdHandle(STD_ERROR_HANDLE);

    return teb;
}
//******************************************************************************
// Set up the TEB structure of the CURRENT (!) thread
//******************************************************************************
BOOL WIN32API InitializeThread(TEB *winteb, BOOL fMainThread)
{
    //store TEB address in thread locale memory for easy retrieval
    *TIBFlatPtr = (DWORD)winteb;

////    winteb->exit_code       = 0x103; /* STILL_ACTIVE */

    winteb->stack_top              = (PVOID)OSLibGetTIB(TIB_STACKTOP); /* 04 Top of thread stack */
    winteb->stack_top              = (PVOID)(((ULONG)winteb->stack_top + 0xFFF) & ~0xFFF);
    //round to next page (OS/2 doesn't return a nice rounded value)
    winteb->stack_low              = (PVOID)OSLibGetTIB(TIB_STACKLOW); /* 08 Stack low-water mark */
    //round to page boundary (OS/2 doesn't return a nice rounded value)
    winteb->stack_low              = (PVOID)((ULONG)winteb->stack_low & ~0xFFF);

    winteb->o.odin.OrgTIBSel       = GetFS();
    winteb->o.odin.pWsockData      = NULL;
#ifdef DEBUG
    winteb->o.odin.dbgCallDepth    = 0;
#endif
    winteb->o.odin.pMessageBuffer  = NULL;
    winteb->o.odin.lcid            = GetUserDefaultLCID();

    if(OSLibGetPIB(PIB_TASKTYPE) == TASKTYPE_PM)
    {
         winteb->flags      = 0;  //todo gui
    }
    else winteb->flags      = 0;  //todo textmode

    //Initialize thread security objects (TODO: Not complete)
    SID_IDENTIFIER_AUTHORITY sidIdAuth = {0};
    winteb->o.odin.threadinfo.dwType = SECTYPE_PROCESS | SECTYPE_INITIALIZED;

    RtlAllocateAndInitializeSid(&sidIdAuth, 1, 0, 0, 0, 0, 0, 0, 0, 0, &winteb->o.odin.threadinfo.SidUser.User.Sid);

    winteb->o.odin.threadinfo.SidUser.User.Attributes = 0; //?????????

    winteb->o.odin.threadinfo.pTokenGroups = (TOKEN_GROUPS*)malloc(sizeof(TOKEN_GROUPS));
    winteb->o.odin.threadinfo.pTokenGroups->GroupCount = 1;

    RtlAllocateAndInitializeSid(&sidIdAuth, 1, 0, 0, 0, 0, 0, 0, 0, 0, &winteb->o.odin.threadinfo.PrimaryGroup.PrimaryGroup);

    winteb->o.odin.threadinfo.pTokenGroups->Groups[0].Sid = winteb->o.odin.threadinfo.PrimaryGroup.PrimaryGroup;
    winteb->o.odin.threadinfo.pTokenGroups->Groups[0].Attributes = 0; //????
//        pPrivilegeSet   = NULL;
//        pTokenPrivileges= NULL;
//        TokenOwner      = {0};
//        DefaultDACL     = {0};
//        TokenSource     = {0};
    winteb->o.odin.threadinfo.TokenType = TokenPrimary;

    dprintf(("InitializeTIB setup TEB with selector %x", winteb->teb_sel));
    dprintf(("InitializeTIB: FS(%x):[0] = %x", GetFS(), QueryExceptionChain()));
    return TRUE;
}
//******************************************************************************
// Destroy the TIB selector and memory for the current thread
//******************************************************************************
void WIN32API DestroyTEB(TEB *winteb)
{
    SHORT orgtibsel;

    dprintf(("DestroyTIB: FS     = %x", GetFS()));
    dprintf(("DestroyTIB: FS:[0] = %x", QueryExceptionChain()));

    orgtibsel = winteb->o.odin.OrgTIBSel;

    dprintf(("DestroyTIB: OSLibFreeSel %x", winteb->teb_sel));

    threadListMutex.enter();
    TEB *curteb        = threadList;
    if(curteb == winteb) {
        threadList = winteb->o.odin.next;
    }
    else {
        while(curteb->o.odin.next != winteb) {
            curteb = curteb->o.odin.next;
            if(curteb == NULL) {
                dprintf(("DestroyTIB: couldn't find teb %x", winteb));
                DebugInt3();
                break;
            }
        }
        if(curteb) {
            curteb->o.odin.next = winteb->o.odin.next;
        }
    }
    threadListMutex.leave();

    // free allocated memory for security structures
    free( winteb->o.odin.threadinfo.pTokenGroups );

    // free PostMessage event semaphore
    if(winteb->o.odin.hPostMsgEvent) {
        CloseHandle(winteb->o.odin.hPostMsgEvent);
    }

    // free shared memory for WM_COPYDATA
    if (winteb->o.odin.pWM_COPYDATA)
    {
        dprintf(("DestroyTEB: freeing WM_COPYDATA: %#p", winteb->o.odin.pWM_COPYDATA));
        _sfree(winteb->o.odin.pWM_COPYDATA);
    }

#ifdef DEBUG
    if (winteb->o.odin.arrstrCallStack != NULL)
        free( winteb->o.odin.arrstrCallStack );
#endif

    //Restore our original FS selector
    SetFS(orgtibsel);

    //And free our own
    OSLibFreeSel(winteb->teb_sel);

    *TIBFlatPtr = 0;

    dprintf(("DestroyTIB: FS(%x):[0] = %x", GetFS(), QueryExceptionChain()));
    return;
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API GetProcessTIBSel()
{
    if(fExitProcess) {
        return 0;
    }
    return ProcessTIBSel;
}
/******************************************************************************/
/******************************************************************************/
void SetPDBInstance(HINSTANCE hInstance)
{
    ProcessPDB.hInstance = hInstance;
}
/******************************************************************************/
/******************************************************************************/
void WIN32API RestoreOS2TIB()
{
 SHORT  orgtibsel;
 TEB   *winteb;

    //If we're running an Odin32 OS/2 application (not converted!), then we
    //we don't switch FS selectors
    if(!fSwitchTIBSel) {
        return;
    }

    winteb = (TEB *)*TIBFlatPtr;
    if(winteb) {
        orgtibsel = winteb->o.odin.OrgTIBSel;

        //Restore our original FS selector
        SetFS(orgtibsel);
    }
}
/******************************************************************************/
//Switch to WIN32 TIB (FS selector)
//NOTE: This is not done for Odin32 applications (LX), unless
//      fForceSwitch is TRUE)
/******************************************************************************/
USHORT WIN32API SetWin32TIB(BOOL fForceSwitch)
{
    SHORT  win32tibsel;
    TEB   *winteb;

    //If we're running an Odin32 OS/2 application (not converted!), then we
    //we don't switch FS selectors
    if(!fSwitchTIBSel && !fForceSwitch) {
        return GetFS();
    }

    winteb = (TEB *)*TIBFlatPtr;
    if(winteb) {
        win32tibsel = winteb->teb_sel;

        //Restore our win32 FS selector
        return SetReturnFS(win32tibsel);
    }
    else {
        return GetFS();
    }
    // nested calls are OK, OS2ToWinCallback for instance
    //else DebugInt3();

    return GetFS();
}
//******************************************************************************
// ODIN_SetTIBSwitch: override TIB switching
//
// Parameters:
//      BOOL fSwitchTIB
//              FALSE  -> no TIB selector switching
//              TRUE   -> force TIB selector switching
//
//******************************************************************************
void WIN32API ODIN_SetTIBSwitch(BOOL fSwitchTIB)
{
    dprintf(("ODIN_SetTIBSwitch %d", fSwitchTIB));
    if (!fForceWin32TIB) {
        fSwitchTIBSel = fSwitchTIB;
        if(fSwitchTIBSel) {
             SetWin32TIB();
        }
        else RestoreOS2TIB();
    } else {
        dprintf(("ODIN_SetTIBSwitch: ignored due to fForceWin32TIB = TRUE"));
    }
}
//******************************************************************************
//******************************************************************************
//#define DEBUG_HEAPSTATE
#ifdef DEBUG_HEAPSTATE
char *pszHeapDump      = NULL;
char *pszHeapDumpStart = NULL;

int _LNK_CONV callback_function(const void *pentry, size_t sz, int useflag, int status,
                                const char *filename, size_t line)
{
    if (_HEAPOK != status) {
//       dprintf(("status is not _HEAPOK."));
       return 1;
    }
    if (_USEDENTRY == useflag && sz && filename && line && pszHeapDump) {
       sprintf(pszHeapDump, "allocated  %08x %u at %s %d\n", pentry, sz, filename, line);
       pszHeapDump += strlen(pszHeapDump);
    }

    return 0;
}
//******************************************************************************
//******************************************************************************
#endif
VOID WIN32API ExitProcess(DWORD exitcode)
{
    HANDLE hThread = GetCurrentThread();
    TEB   *teb;

    dprintf(("KERNEL32:  ExitProcess %d (time %x)", exitcode, GetCurrentTime()));
    dprintf(("KERNEL32:  ExitProcess FS = %x\n", GetFS()));

    // make sure the Win32 exception stack (if there is still any) is unwound
    // before we destroy internal structures including the Win32 TIB
    RtlUnwind(NULL, 0, 0, 0);

    fExitProcess = TRUE;

    // Lower priority of all threads to minimize the chance that they're scheduled
    // during ExitProcess. Can't kill them as that's possibly dangerous (deadlocks
    // in WGSS for instance)
    threadListMutex.enter();
    teb = threadList;
    while(teb) {
        if(teb->o.odin.hThread != hThread) {
            dprintf(("Active thread id %d, handle %x", LOWORD(teb->o.odin.threadId), teb->o.odin.hThread));
            SetThreadPriority(teb->o.odin.hThread, THREAD_PRIORITY_LOWEST);
        }
        teb = teb->o.odin.next;
    }
    threadListMutex.leave();

    HMDeviceCommClass::CloseOverlappedIOHandlers();

    //detach all dlls (LIFO order) before really unloading them; this
    //should take care of circular dependencies (crash while accessing
    //memory of a dll that has just been freed)
    dprintf(("********************************************"));
    dprintf(("**** Detach process from all dlls -- START"));
    Win32DllBase::detachProcessFromAllDlls();
    dprintf(("**** Detach process from all dlls -- END"));
    dprintf(("********************************************"));

    if(WinExe) {
        delete(WinExe);
        WinExe = NULL;
    }

    //Note: Needs to be done after deleting WinExe (destruction of exe + dll objects)
    //Flush and delete all open memory mapped files
    Win32MemMap::deleteAll();

    //SvL: We must make sure no threads are still suspended (with SuspendThread)
    //     OS/2 seems to be unable to terminate the process otherwise (exitlist hang)
    threadListMutex.enter();
    teb = threadList;
    while(teb) {
        dprintf(("Active thread id %d, handle %x", LOWORD(teb->o.odin.threadId), teb->o.odin.hThread));
        if(teb->o.odin.hThread != hThread) {
            if(teb->o.odin.dwSuspend > 0) {
                //kill any threads that are suspended; dangerous, but so is calling
                //SuspendThread; we assume the app knew what it was doing
                TerminateThread(teb->o.odin.hThread, 0);
                ResumeThread(teb->o.odin.hThread);
            }
            else SetThreadPriority(teb->o.odin.hThread, THREAD_PRIORITY_LOWEST);
        }
        teb = teb->o.odin.next;
    }
    threadListMutex.leave();

#ifdef DEBUG_HEAPSTATE
    pszHeapDumpStart = pszHeapDump = (char *)malloc(10*1024*1024);
    _heap_walk(callback_function);
    dprintf((pszHeapDumpStart));
    free(pszHeapDumpStart);
#endif

#ifdef PROFILE
    // Note: after this point we do not expect any more Win32-API calls,
    // so this is probably the best time to dump the gathered profiling
    // information
    PerfView_Write();
    ProfilerWrite();
    ProfilerTerminate();
#endif /* PROFILE */

    //Restore original OS/2 TIB selector
    teb = GetThreadTEB();
    if(teb) DestroyTEB(teb);

    //avoid crashes since win32 & OS/2 exception handler aren't identical
    //(terminate process generates two exceptions)
    /* @@@PH 1998/02/12 Added Console Support */
    if (iConsoleIsActive())
        iConsoleWaitClose();

    dprintf(("KERNEL32:  ExitProcess done (time %x)", GetCurrentTime()));
#ifndef DEBUG
    OSLibDisablePopups();
#endif
    O32_ExitProcess(exitcode);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FreeLibrary(HINSTANCE hinstance)
{
 Win32DllBase *winmod;
 BOOL rc;

    SetLastError(ERROR_SUCCESS);
    //Ignore FreeLibary for executable
    if(WinExe && hinstance == WinExe->getInstanceHandle()) {
        return TRUE;
    }

    winmod = Win32DllBase::findModule(hinstance);
    if(winmod) {
        dprintf(("FreeLibrary %s", winmod->getName()));
        //Only free it when the nrDynamicLibRef != 0
        //This prevent problems after ExitProcess:
        //i.e. dll A is referenced by our exe and loaded with LoadLibrary by dll B
        //     During ExitProcess it's unloaded once (before dll B), dll B calls
        //     FreeLibrary, but our exe also has a reference -> unloaded too many times
        if(winmod->isDynamicLib()) {
            winmod->decDynamicLib();
            winmod->Release();
        }
        else {
            dprintf(("Skipping dynamic unload as nrDynamicLibRef == 0"));
        }
        return(TRUE);
    }
    dprintf(("WARNING: KERNEL32: FreeLibrary %s %x NOT FOUND!", OSLibGetDllName(hinstance), hinstance));
    return(TRUE);
}
/*****************************************************************************
 * Name      : VOID WIN32API FreeLibraryAndExitThread
 * Purpose   : The FreeLibraryAndExitThread function decrements the reference
 *             count of a loaded dynamic-link library (DLL) by one, and then
 *             calls ExitThread to terminate the calling thread.
 *             The function does not return.
 *
 *             The FreeLibraryAndExitThread function gives threads that are
 *             created and executed within a dynamic-link library an opportunity
 *             to safely unload the DLL and terminate themselves.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 *****************************************************************************/
VOID WIN32API FreeLibraryAndExitThread( HMODULE hLibModule, DWORD dwExitCode)
{

  dprintf(("KERNEL32:  FreeLibraryAndExitThread(%08x,%08x)", hLibModule, dwExitCode));
  FreeLibrary(hLibModule);
  ExitThread(dwExitCode);
}
/******************************************************************************/
/******************************************************************************/
/**
 * LoadLibraryA can be used to map a DLL module into the calling process's
 * addressspace. It returns a handle that can be used with GetProcAddress to
 * get addresses of exported entry points (functions and variables).
 *
 * LoadLibraryA can also be used to map executable (.exe) modules into the
 * address to access resources in the module. However, LoadLibrary can't be
 * used to run an executable (.exe) module.
 *
 * @returns   Handle to the library which was loaded.
 * @param     lpszLibFile   Pointer to zero ASCII string giving the name of the
 *                  executable image (either a Dll or an Exe) which is to be
 *                  loaded.
 *
 *                  If no extention is specified the default .DLL extention is
 *                  appended to the name. End the filename with an '.' if the
 *                  file does not have an extention (and don't want the .DLL
 *                  appended).
 *
 *                  If no path is specified, this API will use the Odin32
 *                  standard search strategy to find the file. This strategy
 *                  is described in the method Win32ImageBase::findDLL.
 *
 *                  This API likes to have backslashes (\), but will probably
 *                  accept forward slashes too. Win32 SDK docs says that it
 *                  should not contain forward slashes.
 *
 *                  Win32 SDK docs adds:
 *                      "The name specified is the file name of the module and
 *                       is not related to the name stored in the library module
 *                       itself, as specified by the LIBRARY keyword in the
 *                       module-definition (.def) file."
 *
 * @sketch    Call LoadLibraryExA with flags set to 0.
 * @status    Odin32 Completely Implemented.
 * @author    Sander van Leeuwen (sandervl@xs4all.nl)
 *            knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Forwards to LoadLibraryExA.
 */
HINSTANCE WIN32API LoadLibraryA(LPCTSTR lpszLibFile)
{
    HINSTANCE hDll;

    dprintf(("KERNEL32: LoadLibraryA(%s) --> LoadLibraryExA(lpszLibFile, 0, 0)",
             lpszLibFile));
    hDll = LoadLibraryExA(lpszLibFile, 0, 0);
    dprintf(("KERNEL32: LoadLibraryA(%s) returns 0x%x",
             lpszLibFile, hDll));
    return hDll;
}


/**
 * LoadLibraryW can be used to map a DLL module into the calling process's
 * addressspace. It returns a handle that can be used with GetProcAddress to
 * get addresses of exported entry points (functions and variables).
 *
 * LoadLibraryW can also be used to map executable (.exe) modules into the
 * address to access resources in the module. However, LoadLibrary can't be
 * used to run an executable (.exe) module.
 *
 * @returns   Handle to the library which was loaded.
 * @param     lpszLibFile   Pointer to Unicode string giving the name of
 *                  the executable image (either a Dll or an Exe) which is to
 *                  be loaded.
 *
 *                  If no extention is specified the default .DLL extention is
 *                  appended to the name. End the filename with an '.' if the
 *                  file does not have an extention (and don't want the .DLL
 *                  appended).
 *
 *                  If no path is specified, this API will use the Odin32
 *                  standard search strategy to find the file. This strategy
 *                  is described in the method Win32ImageBase::findDLL.
 *
 *                  This API likes to have backslashes (\), but will probably
 *                  accept forward slashes too. Win32 SDK docs says that it
 *                  should not contain forward slashes.
 *
 *                  Win32 SDK docs adds:
 *                      "The name specified is the file name of the module and
 *                       is not related to the name stored in the library module
 *                       itself, as specified by the LIBRARY keyword in the
 *                       module-definition (.def) file."
 *
 * @sketch    Convert Unicode name to ascii.
 *            Call LoadLibraryExA with flags set to 0.
 *            free ascii string.
 * @status    Odin32 Completely Implemented.
 * @author    Sander van Leeuwen (sandervl@xs4all.nl)
 *            knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Forwards to LoadLibraryExA.
 */
HINSTANCE WIN32API LoadLibraryW(LPCWSTR lpszLibFile)
{
    char *      pszAsciiLibFile;
    HINSTANCE   hDll;

    pszAsciiLibFile = UnicodeToAsciiString(lpszLibFile);
    dprintf(("KERNEL32: LoadLibraryW(%s) --> LoadLibraryExA(lpszLibFile, 0, 0)",
             pszAsciiLibFile));
    hDll = LoadLibraryExA(pszAsciiLibFile, NULL, 0);
    dprintf(("KERNEL32: LoadLibraryW(%s) returns 0x%x",
             pszAsciiLibFile, hDll));
    FreeAsciiString(pszAsciiLibFile);

    return hDll;
}

//******************************************************************************
//Custom build function to disable loading of LX dlls
static BOOL fDisableLXDllLoading = FALSE;
//******************************************************************************
void WIN32API ODIN_DisableLXDllLoading()
{
    fDisableLXDllLoading = TRUE;
}


/**
 * Custombuild API for registering a callback for LX Dll loading thru LoadLibrary*().
 * @returns Success indicator.
 * @param   pfn     Pointer to callback.
 *                  NULL if callback is deregistered.
 */
BOOL WIN32API ODIN_SetLxDllLoadCallback(PFNLXDLLLOAD pfn)
{
    pfnLxDllLoadCallback = pfn;
    return TRUE;
}


/**
 * LoadLibraryExA can be used to map a DLL module into the calling process's
 * addressspace. It returns a handle that can be used with GetProcAddress to
 * get addresses of exported entry points (functions and variables).
 *
 * LoadLibraryExA can also be used to map executable (.exe) modules into the
 * address to access resources in the module. However, LoadLibrary can't be
 * used to run an executable (.exe) module.
 *
 * @returns   Handle to the library which was loaded.
 * @param     lpszLibFile   Pointer to Unicode string giving the name of
 *                  the executable image (either a Dll or an Exe) which is to
 *                  be loaded.
 *
 *                  If no extention is specified the default .DLL extention is
 *                  appended to the name. End the filename with an '.' if the
 *                  file does not have an extention (and don't want the .DLL
 *                  appended).
 *
 *                  If no path is specified, this API will use the Odin32
 *                  standard search strategy to find the file. This strategy
 *                  is described in the method Win32ImageBase::findDLL.
 *                  This may be alterned by the LOAD_WITH_ALTERED_SEARCH_PATH
 *                  flag, see below.
 *
 *                  This API likes to have backslashes (\), but will probably
 *                  accept forward slashes too. Win32 SDK docs says that it
 *                  should not contain forward slashes.
 *
 *                  Win32 SDK docs adds:
 *                      "The name specified is the file name of the module and
 *                       is not related to the name stored in the library module
 *                       itself, as specified by the LIBRARY keyword in the
 *                       module-definition (.def) file."
 *
 * @param     hFile     Reserved. Must be 0.
 *
 * @param     dwFlags   Flags which specifies the taken when loading the module.
 *                  The value 0 makes it identical to LoadLibraryA/W.
 *
 *                  Flags:
 *
 *                  DONT_RESOLVE_DLL_REFERENCES
 *                      (WinNT/2K feature): Don't load imported modules and
 *                      hence don't resolve imported symbols.
 *                      DllMain isn't called either. (Which is obvious since
 *                      it may use one of the importe symbols.)
 *
 *                      On the other hand, if this flag is NOT set, the system
 *                      load imported modules, resolves imported symbols, calls
 *                      DllMain for process and thread init and term (if wished
 *                      by the module).
 *
 *
 *                  LOAD_LIBRARY_AS_DATAFILE
 *                      If this flag is set, the module is mapped into the
 *                      address space but is not prepared for execution. Though
 *                      it's preparted for resource API. Hence, you'll use this
 *                      flag when you want to load a DLL for extracting
 *                      messages or resources from it.
 *
 *                      The resulting handle can be used with any Odin32 API
 *                      which operates on resources.
 *                      (WinNt/2k supports all resource APIs while Win9x don't
 *                      support the specialized resource APIs: LoadBitmap,
 *                      LoadCursor, LoadIcon, LoadImage, LoadMenu.)
 *
 *
 *                  LOAD_WITH_ALTERED_SEARCH_PATH
 *                      If this flag is set and lpszLibFile specifies a path
 *                      we'll use an alternative file search strategy to find
 *                      imported modules. This stratgy is simply to use the
 *                      path of the module being loaded instead of the path
 *                      of the executable module as the first location
 *                      to search for imported modules.
 *
 *                      If this flag is clear, the standard Odin32 standard
 *                      search strategy. See Win32ImageBase::findDll for
 *                      further information.
 *
 *                  not implemented yet.
 *
 * @status    Open32 Partially Implemented.
 * @author    Sander van Leeuwen (sandervl@xs4all.nl)
 *            knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Forwards to LoadLibraryExA.
 */
HINSTANCE WIN32API LoadLibraryExA(LPCTSTR lpszLibFile, HFILE hFile, DWORD dwFlags)
{
    HINSTANCE       hDll;
    Win32DllBase *  pModule;
    char            szModname[CCHMAXPATH];
    BOOL            fPath;              /* Flags which is set if the    */
                                        /* lpszLibFile contains a path. */
    ULONG           fPE;                /* isPEImage return value. */
    DWORD           Characteristics;    //file header's Characteristics
    char           *dot;

    /** @sketch
     * Some parameter validations is probably useful.
     */
    if (!VALID_PSZ(lpszLibFile))
    {
        dprintf(("KERNEL32: LoadLibraryExA(0x%x, 0x%x, 0x%x): invalid pointer lpszLibFile = 0x%x\n",
                 lpszLibFile, hFile, dwFlags, lpszLibFile));
        SetLastError(ERROR_INVALID_PARAMETER); //or maybe ERROR_ACCESS_DENIED is more appropriate?
        return NULL;
    }
    if (!VALID_PSZMAXSIZE(lpszLibFile, CCHMAXPATH))
    {
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): lpszLibFile string too long, %d\n",
                 lpszLibFile, hFile, dwFlags, strlen(lpszLibFile)));
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    if ((dwFlags & ~(DONT_RESOLVE_DLL_REFERENCES | LOAD_WITH_ALTERED_SEARCH_PATH | LOAD_LIBRARY_AS_DATAFILE)) != 0)
    {
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): dwFlags have invalid or unsupported flags\n",
                 lpszLibFile, hFile, dwFlags));
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    /** @sketch
     *  First we'll see if the module is allready loaded - either as the EXE or as DLL.
     *  IF Executable present AND libfile matches the modname of the executable THEN
     *      RETURN instance handle of executable.
     *  Endif
     *  IF allready loaded THEN
     *      IF it's a LX dll which isn't loaded and we're using the PeLoader THEN
     *          Set Load library.
     *      Endif
     *      Inc dynamic reference count.
     *      Inc reference count.
     *      RETURN instance handle.
     *  Endif
     */
    strcpy(szModname, ODINHelperStripUNC((char*)lpszLibFile));
    strupr(szModname);
    dot = strchr(szModname, '.');
    if(dot == NULL) {
        //if there's no extension or trainling dot, we
        //assume it's a dll (see Win32 SDK docs)
        strcat(szModname, DLL_EXTENSION);
    }
    else {
        if(dot[1] == 0) {
            //a trailing dot means the module has no extension (SDK docs)
            *dot = 0;
        }
    }
    if (WinExe != NULL && WinExe->matchModName(szModname))
        return WinExe->getInstanceHandle();

    pModule = Win32DllBase::findModule((LPSTR)szModname);
    if (pModule)
    {
        pModule->incDynamicLib();
        pModule->AddRef();
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): returns 0x%x. Dll found %s",
                 szModname, hFile, dwFlags, pModule->getInstanceHandle(), pModule->getFullPath()));
        return pModule->getInstanceHandle();
    }


    /** @sketch
     *  Test if lpszLibFile has a path or not.
     *  Copy the lpszLibFile to szModname, rename the dll and uppercase the name.
     *  IF it hasn't a path THEN
     *      Issue a findDll to find the dll/executable to be loaded.
     *      IF the Dll isn't found THEN
     *          Set last error and RETURN.
     *      Endif.
     *  Endif
     */
    fPath = strchr(szModname, '\\') || strchr(szModname, '/');
    Win32DllBase::renameDll(szModname);

    if (!fPath)
    {
        char szModName2[CCHMAXPATH];
        strcpy(szModName2, szModname);
        if (!Win32ImageBase::findDll(szModName2, szModname, sizeof(szModname)))
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): module wasn't found. returns NULL",
                     lpszLibFile, hFile, dwFlags));
            SetLastError(ERROR_FILE_NOT_FOUND);
            return NULL;
        }
    }

    //test if dll is in PE or LX format
    fPE = Win32ImageBase::isPEImage(szModname, &Characteristics, NULL);

    /** @sketch
     *  IF (fDisableLXDllLoading && (!fPeLoader || fPE == failure)) THEN
     *      Try load the executable using LoadLibrary
     *      IF successfully loaded THEN
     *          Try find registered/pe2lx object.
     *          IF callback Then
     *              If callback give green light Then
     *                  Find registered lx object.
     *              Else
     *                  Unload it if loaded.
     *              Endif
     *          Endif
     *          IF module object found Then
     *              IF LX dll and is using the PE Loader THEN
     *                  Set Load library.
     *                  Inc reference count.
     *              Endif
     *              Inc dynamic reference count.
     *              RETURN successfully.
     *          Else
     *              fail.
     *          Endif
     *      Endif
     *  Endif
     */
    //only call OS/2 if LX binary or win32k process
    if (!fDisableLXDllLoading && (!fPeLoader || fPE != ERROR_SUCCESS))
    {
        hDll = OSLibDosLoadModule(szModname);
        if (hDll)
        {
            /* OS/2 dll, system dll, converted dll or win32k took care of it. */
            pModule = Win32DllBase::findModuleByOS2Handle(hDll);
            /* Custombuild customizing may take care of it too. */
            if (pfnLxDllLoadCallback)
            {
                /* If callback says yes, continue load it, else fail. */
                if (pfnLxDllLoadCallback(hDll, pModule ? pModule->getInstanceHandle() : NULL))
                    pModule = Win32DllBase::findModuleByOS2Handle(hDll);
                else if (pModule)
                {
                    pModule->Release();
                    pModule = NULL;
                }
            }
            if (pModule)
            {
                if (pModule->isLxDll())
                {
                    ((Win32LxDll *)pModule)->setDllHandleOS2(hDll);
                    if (fPeLoader && pModule->AddRef() == -1)
                    {   //-1 -> load failed (attachProcess)
                        delete pModule;
                        SetLastError(ERROR_INVALID_EXE_SIGNATURE);
                        dprintf(("Dll %s refused to be loaded; aborting", szModname));
                        return 0;
                    }

                }
                pModule->incDynamicLib();
            }
            else if (fExeStarted && !fIsOS2Image) {
                OSLibDosFreeModule(hDll);
                SetLastError(ERROR_INVALID_EXE_SIGNATURE);
                dprintf(("Dll %s is not an Odin dll; unload & return failure", szModname));
                return 0;
            }
            else {
                /* bird 2001-07-10:
                 *  let's fail right away instead of hitting DebugInt3s and fail other places.
                 *  This is very annoying when running Opera on a debug build with netscape/2
                 *  plugins present. We'll make this conditional for the time being.
                 */
                static BOOL fFailIfUnregisteredLX = -1;
                if (fFailIfUnregisteredLX == -1)
                    fFailIfUnregisteredLX = getenv("ODIN32.FAIL_IF_UNREGISTEREDLX") != NULL;
                if (fExeStarted && fFailIfUnregisteredLX)
                {
                    dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): returns 0x%x. Loaded OS/2 dll %s using DosLoadModule. returns NULL.",
                             lpszLibFile, hFile, dwFlags, hDll, szModname));
                    SetLastError(ERROR_INVALID_EXE_SIGNATURE);
                    return NULL;
                }
                dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): returns 0x%x. Loaded OS/2 dll %s using DosLoadModule.",
                         lpszLibFile, hFile, dwFlags, hDll, szModname));
                return hDll; //happens when LoadLibrary is called in kernel32's initterm (nor harmful)
            }
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): returns 0x%x. Loaded %s using DosLoadModule.",
                     lpszLibFile, hFile, dwFlags, hDll, szModname));
            return pModule->getInstanceHandle();
        }
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): DosLoadModule (%s) failed. LastError=%d",
                 lpszLibFile, hFile, dwFlags, szModname, GetLastError()));
        // YD return now for OS/2 dll only
        if (fPE != ERROR_SUCCESS)
            return NULL;
    }
    else
        hDll = NULL;


    /** @sketch
     *  If PE image THEN
     *      IF LOAD_LIBRARY_AS_DATAFILE or Executable THEN
     *
     *
     *      Try load the file using the Win32PeLdrDll class.
     *      <sketch continued further down>
     *  Else
     *      Set last error.
     *      (hDll is NULL)
     *  Endif
     *  return hDll.
     */
    if(fPE == ERROR_SUCCESS)
    {
        Win32PeLdrDll *peldrDll;

        //SvL: If executable -> load as data file (only resources)
        if(!(Characteristics & IMAGE_FILE_DLL))
        {
            dwFlags |= (LOAD_LIBRARY_AS_DATAFILE | DONT_RESOLVE_DLL_REFERENCES);
        }

        peldrDll = new Win32PeLdrDll(szModname);
        if (peldrDll == NULL)
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): Failed to created instance of Win32PeLdrDll. returns NULL.",
                     lpszLibFile, hFile, dwFlags));
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return NULL;
        }

        /** @sketch
         * Process dwFlags
         */
        if (dwFlags & LOAD_LIBRARY_AS_DATAFILE)
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): LOAD_LIBRARY_AS_DATAFILE",
                      lpszLibFile, hFile, dwFlags));
            peldrDll->setLoadAsDataFile();
            peldrDll->disableLibraryCalls();
        }
        if (dwFlags & DONT_RESOLVE_DLL_REFERENCES)
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): DONT_RESOLVE_DLL_REFERENCES",
                      lpszLibFile, hFile, dwFlags));
            peldrDll->disableLibraryCalls();
            peldrDll->disableImportHandling();
        }
        if (dwFlags & LOAD_WITH_ALTERED_SEARCH_PATH)
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): Warning dwFlags LOAD_WITH_ALTERED_SEARCH_PATH is not implemented.",
                      lpszLibFile, hFile, dwFlags));
            //peldrDll->setLoadWithAlteredSearchPath();
        }

        /** @sketch
         *  Initiate the peldr DLL.
         *  IF successful init THEN
         *      Inc dynamic ref count.
         *      Inc ref count.
         *      Attach to process
         *      IF successful THEN
         *          hDLL <- instance handle.
         *      ELSE
         *          set last error
         *          delete Win32PeLdrDll instance.
         *      Endif
         *  ELSE
         *      set last error
         *      delete Win32PeLdrDll instance.
         *  Endif.
         */
        if(peldrDll->init(0) == LDRERROR_SUCCESS)
        {
            peldrDll->AddRef();
            if (peldrDll->attachProcess())
            {
                hDll = peldrDll->getInstanceHandle();
                //Must be called *after* attachprocess, since attachprocess may also
                //trigger LoadLibrary calls
                //Those dlls must not be put in front of this dll in the dynamic
                //dll list; or else the unload order is wrong:
                //i.e. RPAP3260 loads PNRS3260 in DLL_PROCESS_ATTACH
                //     this means that in ExitProcess, PNRS3260 needs to be removed
                //     first since RPAP3260 depends on it
                peldrDll->incDynamicLib();
            }
            else
            {
                dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): attachProcess call to Win32PeLdrDll instance failed. returns NULL.",
                         lpszLibFile, hFile, dwFlags));
                SetLastError(ERROR_DLL_INIT_FAILED);
                delete peldrDll;
                return NULL;
            }
        }
        else
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): Failed to init Win32PeLdrDll instance. error=%d returns NULL.",
                     lpszLibFile, hFile, dwFlags, peldrDll->getError()));
            SetLastError(ERROR_INVALID_EXE_SIGNATURE);
            delete peldrDll;
            return NULL;
        }
    }
    else
    {
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x) library wasn't found (%s) or isn't loadable; err %x",
                 lpszLibFile, hFile, dwFlags, szModname, fPE));
        SetLastError(fPE);
        return NULL;
    }

    return hDll;
}


/**
 * LoadLibraryExW can be used to map a DLL module into the calling process's
 * addressspace. It returns a handle that can be used with GetProcAddress to
 * get addresses of exported entry points (functions and variables).
 *
 * LoadLibraryExW can also be used to map executable (.exe) modules into the
 * address to access resources in the module. However, LoadLibrary can't be
 * used to run an executable (.exe) module.
 *
 * @returns   Handle to the library which was loaded.
 * @param     lpszLibFile   Pointer to Unicode string giving the name of
 *                  the executable image (either a Dll or an Exe) which is to
 *                  be loaded.
 *
 *                  If no extention is specified the default .DLL extention is
 *                  appended to the name. End the filename with an '.' if the
 *                  file does not have an extention (and don't want the .DLL
 *                  appended).
 *
 *                  If no path is specified, this API will use the Odin32
 *                  standard search strategy to find the file. This strategy
 *                  is described in the method Win32ImageBase::findDLL.
 *                  This may be alterned by the LOAD_WITH_ALTERED_SEARCH_PATH
 *                  flag, see below.
 *
 *                  This API likes to have backslashes (\), but will probably
 *                  accept forward slashes too. Win32 SDK docs says that it
 *                  should not contain forward slashes.
 *
 *                  Win32 SDK docs adds:
 *                      "The name specified is the file name of the module and
 *                       is not related to the name stored in the library module
 *                       itself, as specified by the LIBRARY keyword in the
 *                       module-definition (.def) file."
 *
 * @param     hFile     Reserved. Must be 0.
 *
 * @param     dwFlags   Flags which specifies the taken when loading the module.
 *                  The value 0 makes it identical to LoadLibraryA/W.
 *
 *                  Flags:
 *
 *                  DONT_RESOLVE_DLL_REFERENCES
 *                      (WinNT/2K feature): Don't load imported modules and
 *                      hence don't resolve imported symbols.
 *                      DllMain isn't called either. (Which is obvious since
 *                      it may use one of the importe symbols.)
 *
 *                      On the other hand, if this flag is NOT set, the system
 *                      load imported modules, resolves imported symbols, calls
 *                      DllMain for process and thread init and term (if wished
 *                      by the module).
 *
 *                  LOAD_LIBRARY_AS_DATAFILE
 *                      If this flag is set, the module is mapped into the
 *                      address space but is not prepared for execution. Though
 *                      it's preparted for resource API. Hence, you'll use this
 *                      flag when you want to load a DLL for extracting
 *                      messages or resources from it.
 *
 *                      The resulting handle can be used with any Odin32 API
 *                      which operates on resources.
 *                      (WinNt/2k supports all resource APIs while Win9x don't
 *                      support the specialized resource APIs: LoadBitmap,
 *                      LoadCursor, LoadIcon, LoadImage, LoadMenu.)
 *
 *                  LOAD_WITH_ALTERED_SEARCH_PATH
 *                      If this flag is set and lpszLibFile specifies a path
 *                      we'll use an alternative file search strategy to find
 *                      imported modules. This stratgy is simply to use the
 *                      path of the module being loaded instead of the path
 *                      of the executable module as the first location
 *                      to search for imported modules.
 *
 *                      If this flag is clear, the standard Odin32 standard
 *                      search strategy. See Win32ImageBase::findDll for
 *                      further information.
 *
 * @sketch    Convert Unicode name to ascii.
 *            Call LoadLibraryExA.
 *            Free ascii string.
 *            return handle from LoadLibraryExA.
 * @status    Open32 Partially Implemented.
 * @author    Sander van Leeuwen (sandervl@xs4all.nl)
 *            knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Forwards to LoadLibraryExA.
 */
HINSTANCE WIN32API LoadLibraryExW(LPCWSTR lpszLibFile, HFILE hFile, DWORD dwFlags)
{
    char *      pszAsciiLibFile;
    HINSTANCE   hDll;

    pszAsciiLibFile = UnicodeToAsciiString(lpszLibFile);
    dprintf(("KERNEL32: LoadLibraryExW(%s, 0x%x, 0x%x) --> LoadLibraryExA",
             pszAsciiLibFile, hFile, dwFlags));
    hDll = LoadLibraryExA(pszAsciiLibFile, hFile, dwFlags);
    dprintf(("KERNEL32: LoadLibraryExW(%s, 0x%x, 0x%x) returns 0x%x",
             pszAsciiLibFile, hFile, dwFlags, hDll));
    FreeAsciiString(pszAsciiLibFile);

    return hDll;
}
//******************************************************************************
//******************************************************************************
HINSTANCE16 WIN32API LoadLibrary16(LPCTSTR lpszLibFile)
{
    dprintf(("ERROR: LoadLibrary16 %s, not implemented", lpszLibFile));
    return 0;
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FreeLibrary16(HINSTANCE16 hinstance)
{
    dprintf(("ERROR: FreeLibrary16 %x, not implemented", hinstance));
}
//******************************************************************************
//******************************************************************************
FARPROC WIN32API GetProcAddress16(HMODULE hModule, LPCSTR lpszProc)
{
    dprintf(("ERROR: GetProcAddress16 %x %x, not implemented", hModule, lpszProc));
    return 0;
}


/*************************************************************************
 * CommandLineToArgvW (re-exported as [SHELL32.7])
 */
/*************************************************************************
*
* We must interpret the quotes in the command line to rebuild the argv
* array correctly:
* - arguments are separated by spaces or tabs
* - quotes serve as optional argument delimiters
*   '"a b"'   -> 'a b'
* - escaped quotes must be converted back to '"'
*   '\"'      -> '"'
* - an odd number of '\'s followed by '"' correspond to half that number
*   of '\' followed by a '"' (extension of the above)
*   '\\\"'    -> '\"'
*   '\\\\\"'  -> '\\"'
* - an even number of '\'s followed by a '"' correspond to half that number
*   of '\', plus a regular quote serving as an argument delimiter (which
*   means it does not appear in the result)
*   'a\\"b c"'   -> 'a\b c'
*   'a\\\\"b c"' -> 'a\\b c'
* - '\' that are not followed by a '"' are copied literally
*   'a\b'     -> 'a\b'
*   'a\\b'    -> 'a\\b'
*
* Note:
* '\t' == 0x0009
* ' '  == 0x0020
* '"'  == 0x0022
* '\\' == 0x005c
*/
LPWSTR* WINAPI CommandLineToArgvW(LPCWSTR lpCmdline, int* numargs)
{
  DWORD argc;
  HGLOBAL hargv;
  LPWSTR  *argv;
  LPCWSTR cs;
  LPWSTR arg,s,d;
  LPWSTR cmdline;
  int in_quotes,bcount;

  if (*lpCmdline==0) {
  /* Return the path to the executable */
    DWORD size;

    hargv=0;
    size=16;
    do {
      size*=2;
      hargv=GlobalReAlloc(hargv, size, 0);
      argv=(LPWSTR*)GlobalLock(hargv);
    } while (GetModuleFileNameW((HMODULE)0, (LPWSTR)(argv+1), size-sizeof(LPWSTR)) == 0);
    argv[0]=(LPWSTR)(argv+1);
    if (numargs)
      *numargs=2;

    return argv;
  }

  /* to get a writeable copy */
  argc=0;
  bcount=0;
  in_quotes=0;
  cs=lpCmdline;
  while (1) {
    if (*cs==0 || ((*cs==0x0009 || *cs==0x0020) && !in_quotes)) {
    /* space */
      argc++;
      /* skip the remaining spaces */
      while (*cs==0x0009 || *cs==0x0020) {
        cs++;
      }
      if (*cs==0)
        break;
      bcount=0;
      continue;
    } else if (*cs==0x005c) {
    /* '\', count them */
      bcount++;
    } else if ((*cs==0x0022) && ((bcount & 1)==0)) {
    /* unescaped '"' */
      in_quotes=!in_quotes;
      bcount=0;
    } else {
    /* a regular character */
      bcount=0;
    }
    cs++;
  }
  /* Allocate in a single lump, the string array, and the strings that go with it.
  * This way the caller can make a single GlobalFree call to free both, as per MSDN.
    */
    hargv=GlobalAlloc(0, argc*sizeof(LPWSTR)+(strlenW(lpCmdline)+1)*sizeof(WCHAR));
  argv=(LPWSTR*)GlobalLock(hargv);
  if (!argv)
    return NULL;
  cmdline=(LPWSTR)(argv+argc);
  strcpyW(cmdline, lpCmdline);

  argc=0;
  bcount=0;
  in_quotes=0;
  arg=d=s=cmdline;
  while (*s) {
    if ((*s==0x0009 || *s==0x0020) && !in_quotes) {
    /* Close the argument and copy it */
      *d=0;
      argv[argc++]=arg;

      /* skip the remaining spaces */
      do {
        s++;
      } while (*s==0x0009 || *s==0x0020);

      /* Start with a new argument */
      arg=d=s;
      bcount=0;
    } else if (*s==0x005c) {
    /* '\\' */
      *d++=*s++;
      bcount++;
    } else if (*s==0x0022) {
    /* '"' */
      if ((bcount & 1)==0) {
      /* Preceeded by an even number of '\', this is half that
        * number of '\', plus a quote which we erase.
          */
          d-=bcount/2;
        in_quotes=!in_quotes;
        s++;
      } else {
      /* Preceeded by an odd number of '\', this is half that
        * number of '\' followed by a '"'
          */
          d=d-bcount/2-1;
        *d++='"';
        s++;
      }
      bcount=0;
    } else {
    /* a regular character */
      *d++=*s++;
      bcount=0;
    }
  }
  if (*arg) {
    *d='\0';
    argv[argc++]=arg;
  }
  if (numargs)
    *numargs=argc;

  return argv;
}

/**
 * Internal function which gets the commandline (string) used to start the current process.
 * @returns     OS/2 / Windows return code
 *              On successful return (NO_ERROR) the global variables
 *              pszCmdLineA and pszCmdLineW are set.
 *
 * @param       pszPeExe    Pass in the name of the PE exe of this process. We'll
 *                          us this as exename and skip the first argument (ie. argv[1]).
 *                          If NULL we'll use the commandline from OS/2 as it is.
 * @status      Completely implemented and tested.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
ULONG InitCommandLine(const char *pszPeExe)
{
    PCHAR   pib_pchcmd;                 /* PIB pointer to commandline. */
    CHAR    szFilename[CCHMAXPATH];     /* Filename buffer used to get the exe filename in. */
    ULONG   cch;                        /* Commandline string length. (including terminator) */
    PSZ     psz;                        /* Temporary string pointer. */
    PSZ     psz2;                       /* Temporary string pointer. */
    APIRET  rc;                         /* OS/2 return code. */
    BOOL    fQuotes;                    /* Flag used to remember if the exe filename should be in quotes. */
    LPWSTR *argvW;
    int     i;
    ULONG   cb;

    /** @sketch
     * Get commandline from the PIB.
     */
    pib_pchcmd = (PCHAR)OSLibGetPIB(PIB_PCHCMD);

    /** @sketch
     * Two methods of making the commandline:
     *  (1) The first argument is skipped and the second is used as exe filname.
     *      This applies to PE.EXE launched processes only.
     *  (2) No skipping. First argument is the exe filename.
     *      This applies to all but PE.EXE launched processes.
     *
     *  Note: We could do some code size optimization here. Much of the code for
     *        the two methods are nearly identical.
     *
     */
    if(pszPeExe)
    {
        /** @sketch
         * Allocate memory for the commandline.
         * Build commandline:
         *  Copy exe filename.
         *  Add arguments.
         */
        cch = strlen(pszPeExe)+1;

        // PH 2002-04-11
        // Note: intentional memory leak, pszCmdLineW will not be freed
        // or allocated after process startup
        pszCmdLineA = psz = (PSZ)malloc(cch);
        if (psz == NULL)
        {
            dprintf(("KERNEL32: InitCommandLine(%p): malloc(%d) failed\n", pszPeExe, cch));
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        strcpy((char *)pszCmdLineA, pszPeExe);

        rc = NO_ERROR;
    }
    else
    {
        /** @sketch Method (2):
         * First we'll have to determin the size of the commandline.
         *
         * As we don't assume that OS/2 allways puts a fully qualified EXE name
         * as the first string, we'll check if it's empty - and get the modulename
         * in that case - and allways get the fully qualified filename.
         */
        if (pib_pchcmd == NULL || pib_pchcmd[0] == '\0')
        {
            rc = OSLibDosQueryModuleName(OSLibGetPIB(PIB_HMTE), sizeof(szFilename), szFilename);
            if (rc != NO_ERROR)
            {
                dprintf(("KERNEL32: InitCommandLine(%p): OSLibQueryModuleName(0x%x,...) failed with rc=%d\n",
                         pszPeExe, OSLibGetPIB(PIB_HMTE), rc));
                return rc;
            }
        }
        else
        {
            rc = OSLibDosQueryPathInfo(pib_pchcmd, FIL_QUERYFULLNAME, szFilename, sizeof(szFilename));
            if (rc != NO_ERROR)
            {
                dprintf(("KERNEL32: InitCommandLine(%p): (info) OSLibDosQueryPathInfo failed with rc=%d\n", pszPeExe, rc));
                strcpy(szFilename, pib_pchcmd);
                rc = NO_ERROR;
            }
        }

        /** @sketch
         * We're still measuring the size of the commandline:
         *  Check if we have to quote the exe filename.
         *  Determin the length of the executable name including quotes and '\0'-terminator.
         *  Count the length of the arguments. (We here count's all argument strings.)
         */
        fQuotes = strchr(szFilename, ' ') != NULL;
        cch = strlen(szFilename) + fQuotes*2 + 1;
        if (pib_pchcmd != NULL)
        {
            psz2 = pib_pchcmd + strlen(pib_pchcmd) + 1;
            while (*psz2 != '\0')
            {
                register int cchTmp = strlen(psz2) + 1; /* + 1 is for terminator (psz2) and space (cch). */
                psz2 += cchTmp;
                cch += cchTmp;
            }
        }

        /** @sketch
         * Allocate memory for the commandline.
         * Build commandline:
         *  Copy exe filename.
         *  Add arguments.
         */
        pszCmdLineA = psz = (PSZ)malloc(cch);
        if (psz == NULL)
        {
            dprintf(("KERNEL32: InitCommandLine(%p): malloc(%d) failed\n", pszPeExe, cch));
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        if (fQuotes)
            *psz++ = '"';
        strcpy(psz, szFilename);
        psz += strlen(psz);
        if (fQuotes)
        {
            *psz++ = '"';
            *psz = '\0';
        }

        if (pib_pchcmd != NULL)
        {
            psz2 = pib_pchcmd + strlen(pib_pchcmd) + 1;

#ifdef __KLIBC__
            // kLIBC spawn() detects if the process it starts is a kLIBC process
            // and uses special hidden command line arguments to pass additional
            // info to it which are then removed before passing arguments to
            // main(). Since we don't have global argc/argv pointers in kLIBC,
            // we can't access them here and have to cut out these hidden args
            // Yes, it's implementation dependent -- needs to be changed when
            // kLIBC 0.7 (which contains glibal argc/argv) comes out.
            bool isKLIBC = strcmp(psz2, __KLIBC_ARG_SIGNATURE) == 0;
            if (isKLIBC)
                psz2 += strlen(psz2) + 1;
#endif
            while (*psz2 != '\0')
            {
#ifdef __KLIBC__
                // if the first byte is a kLIBC flag, skip it
                if (isKLIBC)
                    psz2++;
#endif
                register int cchTmp = strlen(psz2) + 1; /* + 1 is for terminator (psz). */
                *psz++ = ' ';           /* add space */
                memcpy(psz, psz2, cchTmp);
                psz2 += cchTmp;
                psz += cchTmp - 1;
            }
        }
    }

    /** @sketch
     * If successfully build ASCII commandline then convert it to UniCode.
     */
    if (rc == NO_ERROR)
    {
        // PH 2002-04-11
        // Note: intentional memory leak, pszCmdLineW will not be freed
        // or allocated after process startup
        cch = strlen(pszCmdLineA) + 1;

        pszCmdLineW = (WCHAR*)malloc(cch * 2);
        if (pszCmdLineW != NULL) {
            //Translate from OS/2 to Windows codepage & ascii to unicode
            MultiByteToWideChar(CP_OEMCP, 0, pszCmdLineA, -1, (LPWSTR)pszCmdLineW, cch-1);
            ((LPWSTR)pszCmdLineW)[cch-1] = 0;

            //ascii command line is still in OS/2 codepage, so convert it
            WideCharToMultiByte(CP_ACP, 0, pszCmdLineW, -1, (LPSTR)pszCmdLineA, cch-1, 0, NULL);
            ((LPSTR)pszCmdLineA)[cch-1] = 0;

            // now, initialize __argcA and __argvA. These global variables are for the convenience
            // of applications that want to access the ANSI version of command line arguments w/o
            // using the lpCommandLine parameter of WinMain and parsing it manually
            LPWSTR *argvW = CommandLineToArgvW(pszCmdLineW, &__argcA);
            if (argvW != NULL)
            {
                // Allocate space for both the argument array and the arguments
                // Note: intentional memory leak, pszCmdLineW will not be freed
                // or allocated after process startup
                cb = sizeof(char*) * (__argcA + 1) + cch + __argcA;
                __argvA = (char **)malloc(cb);
                if (__argvA != NULL)
                {
                    psz = ((char *)__argvA) + sizeof(char*) * __argcA;
                    cb -= sizeof(char*) * __argcA;
                    for (i = 0; i < __argcA; ++i)
                    {
                        cch = WideCharToMultiByte(CP_ACP, 0, argvW[i], -1, psz, cb, 0, NULL);
                        if (!cch)
                        {
                            DebugInt3();
                            dprintf(("KERNEL32: InitCommandLine(%p): WideCharToMultiByte() failed\n", pszPeExe));
                            rc = ERROR_NOT_ENOUGH_MEMORY;
                            break;
                        }
                        psz[cch++] = '\0';
                        __argvA[i] = psz;
                        psz += cch;
                        cb -= cch;
                    }
                    // argv[argc] must be NULL
                    __argvA[i] = NULL;
                }
                else
                {
                    DebugInt3();
                    dprintf(("KERNEL32: InitCommandLine(%p): malloc(%d) failed (3)\n", pszPeExe, cch));
                    rc = ERROR_NOT_ENOUGH_MEMORY;
                }
            }
            else
            {
                DebugInt3();
                dprintf(("KERNEL32: InitCommandLine(%p): CommandLineToArgvW() failed\n", pszPeExe));
                rc = ERROR_NOT_ENOUGH_MEMORY;
            }
        }
        else
        {
            DebugInt3();
            dprintf(("KERNEL32: InitCommandLine(%p): malloc(%d) failed (2)\n", pszPeExe, cch * 2));
            rc = ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    return rc;
}

/**
 * Gets the command line of the current process.
 * @returns     On success:
 *                  Command line of the current process. One single string.
 *                  The first part of the command line string is the executable filename
 *                  of the current process. It might be in quotes if it contains spaces.
 *                  The rest of the string is arguments.
 *
 *              On error:
 *                  NULL. Last error set. (does Win32 set last error this?)
 * @sketch      IF not inited THEN
 *                  Init commandline assuming !PE.EXE
 *                  IF init failes THEN set last error.
 *              ENDIF
 *              return ASCII/ANSI commandline.
 * @status      Completely implemented and tested.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      The Ring-3 PeLdr is resposible for calling InitCommandLine before anyone
 *              is able to call this function.
 */
LPCSTR WIN32API GetCommandLineA(VOID)
{
    /*
     * Check if the commandline is initiated.
     * If not we'll have to do it.
     * ASSUMES that if not inited this isn't a PE.EXE lauched process.
     */
    if (pszCmdLineA == NULL)
    {
        APIRET rc;
        rc = InitCommandLine(NULL);
        if (rc != NULL)
            SetLastError(rc);
    }

    dprintf(("KERNEL32:  GetCommandLineA: %s\n", pszCmdLineA));
    return pszCmdLineA;
}


/**
 * Gets the command line of the current process.
 * @returns     On success:
 *                  Command line of the current process. One single string.
 *                  The first part of the command line string is the executable filename
 *                  of the current process. It might be in quotes if it contains spaces.
 *                  The rest of the string is arguments.
 *
 *              On error:
 *                  NULL. Last error set. (does Win32 set last error this?)
 * @sketch      IF not inited THEN
 *                  Init commandline assuming !PE.EXE
 *                  IF init failes THEN set last error.
 *              ENDIF
 *              return Unicode commandline.
 * @status      Completely implemented and tested.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      The Ring-3 PeLdr is resposible for calling InitCommandLine before anyone
 *              is able to call this function.
 */
LPCWSTR WIN32API GetCommandLineW(void)
{
    /*
     * Check if the commandline is initiated.
     * If not we'll have to do it.
     * ASSUMES that if not inited this isn't a PE.EXE lauched process.
     */
    if (pszCmdLineW == NULL)
    {
        APIRET rc;
        rc = InitCommandLine(NULL);
        if (rc != NULL)
            SetLastError(rc);
    }

    dprintf(("KERNEL32:  GetCommandLineW: %ls\n", pszCmdLineW));
    return pszCmdLineW;
}


/**
 * GetModuleFileName gets the full path and file name for the specified module.
 * @returns     Bytes written to the buffer (lpszPath). This count includes the
 *              terminating '\0'.
 *              On error 0 is returned. Last error is set.
 *
 *              2002-04-25 PH
 *              Q - Do we set ERROR_BUFFER_OVERFLOW when cch > cchPath?
 *              Q - Does NT really set the last error?
 *              A > Win2k does not set LastError here, remains OK
 *
 *              While GetModuleFileName does add a trailing termination zero
 *              if there is enough room, the returned number of characters
 *              *MUST NOT* include the zero character!
 *              (Notes R6 Installer on Win2kSP6, verified Testcase)
 *
 * @param       hModule     Handle to the module you like to get the file name to.
 * @param       lpszPath    Output buffer for full path and file name.
 * @param       cchPath     Size of the lpszPath buffer.
 * @sketch      Validate lpszPath.
 *              Find the module object using handle.
 *              If found Then
 *                  Get full path from module object.
 *                  If found path Then
 *                      Copy path to buffer and set the number of bytes written.
 *                  Else
 *                      IPE!
 *              Else
 *                  Call Open32 GetModuleFileName. (kernel32 initterm needs/needed this)
 *              Log result.
 *              Return number of bytes written to the buffer.
 *
 * @status      Completely implemented, Open32.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *              Sander van Leeuwen (sandervl@xs4all.nl)
 *              Patrick Haller     (patrick.haller@innotek.de)
 * @remark      - Do we still have to call Open32?
 */
DWORD WIN32API GetModuleFileNameA(HMODULE hModule, LPTSTR lpszPath, DWORD cchPath)
{
    Win32ImageBase *    pMod;           /* Pointer to the module object. */
    DWORD               cch = 0;        /* Length of the  */

    // PH 2002-04-24 Note:
    // WIN2k just crashes in NTDLL if lpszPath is invalid!
    if (!VALID_PSZ(lpszPath))
    {
        dprintf(("KERNEL32:  GetModuleFileNameA(0x%x, 0x%x, 0x%x): invalid pointer lpszLibFile = 0x%x\n",
                 hModule, lpszPath, cchPath, lpszPath));
        SetLastError(ERROR_INVALID_PARAMETER); //or maybe ERROR_ACCESS_DENIED is more appropriate?
        return 0;
    }

    pMod = Win32ImageBase::findModule(hModule);
    if (pMod != NULL)
    {
        const char *pszFn = pMod->getFullPath();
        if (pszFn)
        {
            cch = strlen(pszFn);
            if (cch >= cchPath)
              cch = cchPath;
            else
              // if there is sufficient room for the zero termination,
              // write it additionally, uncounted
              lpszPath[cch] = '\0';

            memcpy(lpszPath, pszFn, cch);
        }
        else
        {
            dprintf(("KERNEL32:  GetModuleFileNameA(%x,...): IPE - getFullPath returned NULL or empty string\n"));
            DebugInt3();
            SetLastError(ERROR_INVALID_HANDLE);
        }
    }
    else
    {
        SetLastError(ERROR_INVALID_HANDLE);
        //only needed for call inside kernel32's initterm (profile init)
        //(console init only it seems...)
        cch = OSLibDosGetModuleFileName(hModule, lpszPath, cchPath);
    }

    if (cch > 0)
        dprintf(("KERNEL32:  GetModuleFileNameA(%x %x): %s %d\n", hModule, lpszPath, lpszPath, cch));
    else
        dprintf(("KERNEL32:  WARNING: GetModuleFileNameA(%x,...) - not found!", hModule));

    return cch;
}


/**
 * GetModuleFileName gets the full path and file name for the specified module.
 * @returns     Bytes written to the buffer (lpszPath). This count includes the
 *              terminating '\0'.
 *              On error 0 is returned. Last error is set.
 * @param       hModule     Handle to the module you like to get the file name to.
 * @param       lpszPath    Output buffer for full path and file name.
 * @param       cchPath     Size of the lpszPath buffer.
 * @sketch      Find the module object using handle.
 *              If found Then
 *                  get full path from module object.
 *                  If found path Then
 *                      Determin path length.
 *                      Translate the path to into the buffer.
 *                  Else
 *                      IPE.
 *              else
 *                  SetLastError to invalid handle.
 *              Log result.
 *              return number of bytes written to the buffer.
 *
 * @status      Completely implemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      - We do _NOT_ call Open32.
 *              - Do we set ERROR_BUFFER_OVERFLOW when cch > cchPath?
 *              - Does NT really set the last error?
 */
DWORD WIN32API GetModuleFileNameW(HMODULE hModule, LPWSTR lpszPath, DWORD cchPath)
{
    Win32ImageBase *    pMod;
    DWORD               cch = 0;

    if (!VALID_PSZ(lpszPath))
    {
        dprintf(("KERNEL32:  GetModuleFileNameW(0x%x, 0x%x, 0x%x): invalid pointer lpszLibFile = 0x%x\n",
                 hModule, lpszPath, cchPath, lpszPath));
        SetLastError(ERROR_INVALID_PARAMETER); //or maybe ERROR_ACCESS_DENIED is more appropriate?
        return 0;
    }

    pMod = Win32ImageBase::findModule(hModule);
    if (pMod != NULL)
    {
        const char *pszFn = pMod->getFullPath();
        if (pszFn || *pszFn != '\0')
        {
            cch = strlen(pszFn) + 1;
            if (cch > cchPath)
                cch = cchPath;
            AsciiToUnicodeN(pszFn, lpszPath, cch);
        }
        else
        {
            dprintf(("KERNEL32:  GetModuleFileNameW(%x,...): IPE - getFullPath returned NULL or empty string\n"));
            DebugInt3();
            SetLastError(ERROR_INVALID_HANDLE);
        }
    }
    else
        SetLastError(ERROR_INVALID_HANDLE);

    if (cch > 0)
        dprintf(("KERNEL32:  GetModuleFileNameW(%x,...): %s %d\n", hModule, lpszPath, cch));
    else
        dprintf(("KERNEL32:  WARNING: GetModuleFileNameW(%x,...) - not found!", hModule));

    return cch;
}


//******************************************************************************
//NOTE: GetModuleHandleA does NOT support files with multiple dots (i.e.
//      very.weird.exe)
//
//      hinst = LoadLibrary("WINSPOOL.DRV");      -> succeeds
//      hinst2 = GetModuleHandle("WINSPOOL.DRV"); -> succeeds
//      hinst3 = GetModuleHandle("WINSPOOL.");    -> fails
//      hinst4 = GetModuleHandle("WINSPOOL");     -> fails
//      hinst = LoadLibrary("KERNEL32.DLL");      -> succeeds
//      hinst2 = GetModuleHandle("KERNEL32.DLL"); -> succeeds
//      hinst3 = GetModuleHandle("KERNEL32.");    -> fails
//      hinst4 = GetModuleHandle("KERNEL32");     -> succeeds
//      Same behaviour as observed in NT4, SP6
//******************************************************************************
HANDLE WIN32API GetModuleHandleA(LPCTSTR lpszModule)
{
 HANDLE    hMod = 0;
 Win32DllBase *windll;
 char      szModule[CCHMAXPATH];
 char     *dot;

    if(lpszModule == NULL)
    {
        if(WinExe)
                hMod = WinExe->getInstanceHandle();
        else
        {
          // // Just fail this API
          // hMod = 0;
          // SetLastError(ERROR_INVALID_HANDLE);
          // Wrong: in an ODIN32-LX environment, just
          // assume a fake handle
          hMod = -1;
        }
    }
    else
    {
        strcpy(szModule, OSLibStripPath((char *)lpszModule));
        strupr(szModule);
        dot = strchr(szModule, '.');
        if(dot == NULL) {
            //if no extension -> add .DLL (see SDK docs)
            strcat(szModule, DLL_EXTENSION);
        }
        else {
            if(dot[1] == 0) {
                //a trailing dot means the module has no extension (SDK docs)
                *dot = 0;
            }
        }
        if(WinExe && WinExe->matchModName(szModule)) {
            hMod = WinExe->getInstanceHandle();
        }
        else {
            windll = Win32DllBase::findModule(szModule);
            if(windll) {
                  hMod = windll->getInstanceHandle();
            }
        }
    }
    dprintf(("KERNEL32:  GetModuleHandle %s returned %X\n", lpszModule, hMod));
    return(hMod);
}
//******************************************************************************
//******************************************************************************
HMODULE WIN32API GetModuleHandleW(LPCWSTR lpwszModuleName)
{
  HMODULE rc;
  char   *astring = NULL;

  if (NULL != lpwszModuleName)
    astring = UnicodeToAsciiString((LPWSTR)lpwszModuleName);

  rc = GetModuleHandleA(astring);
  dprintf(("KERNEL32:  OS2GetModuleHandleW %s returned %X\n", astring, rc));

  if (NULL != astring)
    FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
//Checks whether program is LX or PE
//******************************************************************************
BOOL WIN32API ODIN_IsWin32App(LPSTR lpszProgramPath)
{
    DWORD Characteristics;

    return Win32ImageBase::isPEImage(lpszProgramPath, &Characteristics, NULL) == NO_ERROR;
}
//******************************************************************************
//******************************************************************************
static char szPECmdLoader[260] = "";
static char szPEGUILoader[260] = "";
static char szNELoader[260]    = "";
//******************************************************************************
//Set default paths for PE & NE loaders
//******************************************************************************
BOOL InitLoaders()
{
    int len;

    len = PROFILE_GetOdinIniString(ODINSYSTEM_SECTION, "PEC_EXE", "",
                                   szPECmdLoader, sizeof(szPECmdLoader));
    if (len == 0)
        sprintf(szPECmdLoader, "%s\\PEC.EXE", InternalGetSystemDirectoryA());

    len = PROFILE_GetOdinIniString(ODINSYSTEM_SECTION, "PE_EXE", "",
                                   szPEGUILoader, sizeof(szPEGUILoader));
    if (len == 0)
        sprintf(szPEGUILoader, "%s\\PE.EXE", InternalGetSystemDirectoryA());


    len = PROFILE_GetOdinIniString(ODINSYSTEM_SECTION, "W16ODIN_EXE", "",
                                   szNELoader, sizeof(szNELoader));
    if (len == 0)
        sprintf(szNELoader, "%s\\W16ODIN.EXE", InternalGetSystemDirectoryA());

    return TRUE;
}
//******************************************************************************
//Override loader names (PEC, PE, W16ODIN)
//******************************************************************************
BOOL WIN32API ODIN_SetLoaders(LPCSTR pszPECmdLoader, LPCSTR pszPEGUILoader,
                              LPCSTR pszNELoader)
{
    if(pszPECmdLoader) dprintf(("PE Cmd %s", pszPECmdLoader));
    if(pszPEGUILoader) dprintf(("PE GUI %s", pszPEGUILoader));
    if(pszNELoader)    dprintf(("NE %s", pszNELoader));
    if(pszPECmdLoader)   strcpy(szPECmdLoader, pszPECmdLoader);
    if(pszPEGUILoader)   strcpy(szPEGUILoader, pszPEGUILoader);
    if(pszNELoader)      strcpy(szNELoader, pszNELoader);

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ODIN_QueryLoaders(LPSTR pszPECmdLoader, INT cchPECmdLoader,
                                LPSTR pszPEGUILoader, INT cchPEGUILoader,
                                LPSTR pszNELoader, INT cchNELoader)
{
    if(pszPECmdLoader)   strncpy(pszPECmdLoader, szPECmdLoader, cchPECmdLoader);
    if(pszPEGUILoader)   strncpy(pszPEGUILoader, szPEGUILoader, cchPEGUILoader);
    if(pszNELoader)      strncpy(pszNELoader, szNELoader, cchNELoader);

    return TRUE;
}
//******************************************************************************
//******************************************************************************
static BOOL WINAPI O32_CreateProcessA(LPCSTR lpApplicationName, LPCSTR lpCommandLine,
                                      LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                      LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                      BOOL bInheritHandles, DWORD dwCreationFlags,
                                      LPVOID lpEnvironment, LPCSTR lpCurrentDirectory,
                                      LPSTARTUPINFOA lpStartupInfo,
                                      LPPROCESS_INFORMATION lpProcessInfo)
{
    dprintf(("KERNEL32: O32_CreateProcessA %s cline:%s inherit:%d cFlags:%x "
             "Env:%x CurDir:%s StartupFlags:%x\n",
            lpApplicationName, lpCommandLine, bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory, lpStartupInfo));

    LPSTR lpstr;
    DWORD cb;
    BOOL rc;

    #define ALLOC_OEM(v) \
        if (v) { \
            lpstr = (LPSTR)_smalloc(strlen(v) + 1); \
            CharToOemA(v,  lpstr); \
            v = lpstr; \
        }
    #define FREE_OEM(v) \
        if (v) \
            _sfree((void*)v); \


    // this converts all string arguments from ANSI to OEM expected by
    // O32_CreateProcess()

    ALLOC_OEM(lpApplicationName)
    ALLOC_OEM(lpCommandLine)
    ALLOC_OEM(lpCurrentDirectory)

    if (lpEnvironment) {
        cb = 0;
        lpstr = (LPSTR)lpEnvironment;
        while (lpstr[cb]) {
            cb += strlen(&lpstr[cb]) + 1;
        }
        ++cb;
        lpstr = (LPSTR)_smalloc(cb);
        CharToOemBuffA((LPSTR)lpEnvironment, lpstr, cb);
        lpEnvironment = lpstr;
    }

    ALLOC_OEM(lpStartupInfo->lpReserved)
    ALLOC_OEM(lpStartupInfo->lpDesktop)
    ALLOC_OEM(lpStartupInfo->lpTitle)

    rc = O32_CreateProcess(lpApplicationName, lpCommandLine,
                           lpProcessAttributes, lpThreadAttributes,
                           bInheritHandles, dwCreationFlags,
                           lpEnvironment, lpCurrentDirectory,
                           lpStartupInfo, lpProcessInfo);

    FREE_OEM(lpStartupInfo->lpTitle)
    FREE_OEM(lpStartupInfo->lpDesktop)
    FREE_OEM(lpStartupInfo->lpReserved)

    FREE_OEM(lpEnvironment)

    FREE_OEM(lpCurrentDirectory)
    FREE_OEM(lpCommandLine)
    FREE_OEM(lpApplicationName)

    #undef FREE_OEM
    #undef ALLOC_OEM

    return rc;
}
//******************************************************************************
//******************************************************************************
static void OSLibSetBeginLibpathA(char *lpszBeginlibpath)
{
    PSZ psz = NULL;
    if (lpszBeginlibpath) {
        psz = (PSZ)malloc(strlen(lpszBeginlibpath) + 1);
        CharToOemA(lpszBeginlibpath, psz);
    }
    OSLibSetBeginLibpath(psz);
    if (psz) {
        free(psz);
    }
}
//******************************************************************************
//******************************************************************************
static void OSLibQueryBeginLibpathA(char *lpszBeginlibpath, int size)
{
    OSLibQueryBeginLibpath(lpszBeginlibpath, size);
    OemToCharA(lpszBeginlibpath, lpszBeginlibpath);
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CreateProcessA( LPCSTR lpApplicationName, LPSTR lpCommandLine,
                            LPSECURITY_ATTRIBUTES lpProcessAttributes,
                            LPSECURITY_ATTRIBUTES lpThreadAttributes,
                            BOOL bInheritHandles, DWORD dwCreationFlags,
                            LPVOID lpEnvironment, LPCSTR lpCurrentDirectory,
                            LPSTARTUPINFOA lpStartupInfo,
                            LPPROCESS_INFORMATION lpProcessInfo )
{
    STARTUPINFOA startinfo;
    TEB *pThreadDB = (TEB*)GetThreadTEB();
    char *cmdline = NULL, *newenv = NULL, *oldlibpath = NULL;
    BOOL  rc;
    LPSTR lpstr;

    dprintf(("KERNEL32: CreateProcessA %s cline:%s inherit:%d cFlags:%x Env:%x CurDir:%s StartupFlags:%x\n",
            lpApplicationName, lpCommandLine, bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory, lpStartupInfo));

#ifdef DEBUG
    if(lpStartupInfo) {
        dprintf(("lpStartupInfo->lpReserved %x", lpStartupInfo->lpReserved));
        dprintf(("lpStartupInfo->lpDesktop %x", lpStartupInfo->lpDesktop));
        dprintf(("lpStartupInfo->lpTitle %s", lpStartupInfo->lpTitle));
        dprintf(("lpStartupInfo->dwX %x", lpStartupInfo->dwX));
        dprintf(("lpStartupInfo->dwY %x", lpStartupInfo->dwY));
        dprintf(("lpStartupInfo->dwXSize %x", lpStartupInfo->dwXSize));
        dprintf(("lpStartupInfo->dwYSize %x", lpStartupInfo->dwYSize));
        dprintf(("lpStartupInfo->dwXCountChars %x", lpStartupInfo->dwXCountChars));
        dprintf(("lpStartupInfo->dwYCountChars %x", lpStartupInfo->dwYCountChars));
        dprintf(("lpStartupInfo->dwFillAttribute %x", lpStartupInfo->dwFillAttribute));
        dprintf(("lpStartupInfo->dwFlags %x", lpStartupInfo->dwFlags));
        dprintf(("lpStartupInfo->wShowWindow %x", lpStartupInfo->wShowWindow));
        dprintf(("lpStartupInfo->hStdInput %x", lpStartupInfo->hStdInput));
        dprintf(("lpStartupInfo->hStdOutput %x", lpStartupInfo->hStdOutput));
        dprintf(("lpStartupInfo->hStdError %x", lpStartupInfo->hStdError));
    }
#endif

    if(bInheritHandles && lpStartupInfo->dwFlags & STARTF_USESTDHANDLES)
    {
        //Translate standard handles if the child needs to inherit them
        int retcode = 0;

        memcpy(&startinfo, lpStartupInfo, sizeof(startinfo));
        if(lpStartupInfo->hStdInput) {
        retcode |= HMHandleTranslateToOS2(lpStartupInfo->hStdInput, &startinfo.hStdInput);
        }
        if(lpStartupInfo->hStdOutput) {
        retcode |= HMHandleTranslateToOS2(lpStartupInfo->hStdOutput, &startinfo.hStdOutput);
        }
        if(lpStartupInfo->hStdError) {
        retcode |= HMHandleTranslateToOS2(lpStartupInfo->hStdError, &startinfo.hStdError);
        }

        if(retcode) {
            SetLastError(ERROR_INVALID_HANDLE);
            rc = FALSE;
            goto finished;
        }

        lpStartupInfo = &startinfo;
    }

    if(lpApplicationName) {
        if(lpCommandLine) {
            //skip exe name in lpCommandLine
            //TODO: doesn't work for directories with spaces!
            while(*lpCommandLine != 0 && *lpCommandLine != ' ')
                lpCommandLine++;

            if(*lpCommandLine != 0) {
                lpCommandLine++;
            }
            cmdline = (char *)malloc(strlen(lpApplicationName)+strlen(lpCommandLine) + 16);
            sprintf(cmdline, "%s %s", lpApplicationName, lpCommandLine);
        }
        else {
            cmdline = (char *)malloc(strlen(lpApplicationName) + 16);
            sprintf(cmdline, "%s", lpApplicationName);
        }
    }
    else {
        cmdline = (char *)malloc(strlen(lpCommandLine) + 16);
        sprintf(cmdline, "%s", lpCommandLine);
    }

    char szAppName[MAX_PATH];
    char buffer[MAX_PATH];
    DWORD fileAttr;
    char *exename;

    szAppName[0] = 0;

    exename = buffer;
    strncpy(buffer, cmdline, sizeof(buffer));
    buffer[MAX_PATH-1] = 0;
    if(*exename == '"') {
        exename++;
        while(*exename != 0 && *exename != '"')
             exename++;

        if(*exename != 0) {
             *exename = 0;
        }
        exename++;
        if (SearchPathA( NULL, &buffer[1], ".exe", sizeof(szAppName), szAppName, NULL ) ||
            SearchPathA( NULL, &buffer[1], NULL, sizeof(szAppName), szAppName, NULL ))
        {
            //
        }
    }
    else {
        BOOL fTerminate = FALSE;
        DWORD fileAttr;

        while(*exename != 0) {
             while(*exename != 0 && *exename != ' ')
                  exename++;

             if(*exename != 0) {
                  *exename = 0;
                  fTerminate = TRUE;
             }
             dprintf(("Trying '%s'", buffer ));
             if (SearchPathA( NULL, buffer, ".exe", sizeof(szAppName), szAppName, NULL ) ||
                 SearchPathA( NULL, buffer, NULL, sizeof(szAppName), szAppName, NULL ))
             {
                 if(fTerminate) exename++;
                 break;
             }
             else
             {//maybe it's a short name
                 if(GetLongPathNameA(buffer, szAppName, sizeof(szAppName)))
                 {
                     if(fTerminate) exename++;
                     break;
                 }
             }
             if(fTerminate) {
                  *exename = ' ';
                  exename++;
                  fTerminate = FALSE;
             }
        }
    }
    lpCommandLine = cmdline + (exename - buffer); //start of command line parameters

    fileAttr = GetFileAttributesA(szAppName);
    if(fileAttr == -1 || (fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
        dprintf(("CreateProcess: can't find executable!"));

        SetLastError(ERROR_FILE_NOT_FOUND);

        rc = FALSE;
        goto finished;
    }

    if(lpEnvironment) {
        char *envA = (char *)lpEnvironment;
        if(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT) {
            // process the CREATE_UNICODE_ENVIRONMENT on our own --
            // O32_CreateProcessA() is not aware of it
            dwCreationFlags &= ~CREATE_UNICODE_ENVIRONMENT;

            WCHAR *tmp = (WCHAR *)lpEnvironment;
            int sizeW = 0;
            while (*tmp) {
                int lenW = lstrlenW(tmp);
                sizeW += lenW + 1;
                tmp += lenW + 1;
            }
            sizeW++; // terminating null
            int sizeA = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lpEnvironment, sizeW,
                                            NULL, 0, 0, NULL);
            envA = (char *)malloc(sizeA);
            if(envA == NULL) {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                rc = FALSE;
                goto finished;
            }
            WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lpEnvironment, sizeW,
                                envA, sizeA, 0, NULL);
        }
        newenv = CreateNewEnvironment(envA);
        if(envA != (char *)lpEnvironment)
            free(envA);
        if(newenv == NULL) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            rc = FALSE;
            goto finished;
        }
        lpEnvironment = newenv;
    }

    DWORD Characteristics, SubSystem, fNEExe, fPEExe;

    fPEExe = Win32ImageBase::isPEImage(szAppName, &Characteristics, &SubSystem, &fNEExe) == 0;

    // open32 does not support DEBUG_ONLY_THIS_PROCESS
    if(dwCreationFlags & DEBUG_ONLY_THIS_PROCESS)
      dwCreationFlags |= DEBUG_PROCESS;

    //Only use WGSS to launch the app if it's not PE or PE & win32k loaded
    if(!fPEExe || (fPEExe && fWin32k))
    {

    trylaunchagain:
      if (O32_CreateProcessA(szAppName, lpCommandLine, lpProcessAttributes,
                             lpThreadAttributes, bInheritHandles, dwCreationFlags,
                             lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                             lpProcessInfo) == TRUE)
      {
        if (dwCreationFlags & DEBUG_PROCESS && pThreadDB != NULL)
        {
          if(pThreadDB->o.odin.pidDebuggee != 0)
          {
            // TODO: handle this
            dprintf(("KERNEL32: CreateProcess ERROR: This thread is already a debugger\n"));
          }
          else
          {
            pThreadDB->o.odin.pidDebuggee = lpProcessInfo->dwProcessId;
            OSLibStartDebugger((ULONG*)&pThreadDB->o.odin.pidDebuggee);
          }
        }
        else pThreadDB->o.odin.pidDebuggee = 0;

        if(lpProcessInfo)
        {
            lpProcessInfo->dwThreadId = MAKE_THREADID(lpProcessInfo->dwProcessId, lpProcessInfo->dwThreadId);
        }

        rc = TRUE;
        goto finished;
      }
        else
        if(!oldlibpath)
        {//might have failed because it wants to load dlls in its current directory
            // Add the application directory to the ENDLIBPATH, so dlls can be found there
            // Only necessary for OS/2 applications
            oldlibpath = (char *)calloc(4096, 1);
            if(oldlibpath)
            {
                OSLibQueryBeginLibpathA(oldlibpath, 4096);

                char *tmp = strrchr(szAppName, '\\');
                if(tmp) *tmp = 0;

                OSLibSetBeginLibpathA(szAppName);
                if(tmp) *tmp = '\\';

                goto trylaunchagain;
            }

        }
      // verify why O32_CreateProcess actually failed.
      // If GetLastError() == 191 (ERROR_INVALID_EXE_SIGNATURE)
      // we can continue to call "PE.EXE".
      // Note: Open32 does not translate ERROR_INVALID_EXE_SIGNATURE,
      // it is also valid in Win32.
      DWORD dwError = GetLastError();
      if (ERROR_INVALID_EXE_SIGNATURE != dwError && ERROR_FILE_NOT_FOUND != dwError && ERROR_ACCESS_DENIED != dwError)
      {
          dprintf(("CreateProcess: O32_CreateProcess failed with rc=%d, not PE-executable !", dwError));

          // the current value of GetLastError() is still valid.
          rc = FALSE;
          goto finished;
      }
    }

    // else ...

    //probably a win32 exe, so run it in the pe loader
    dprintf(("KERNEL32: CreateProcess %s %s", szAppName, lpCommandLine));

    if(fPEExe)
    {
      LPCSTR    lpszExecutable;
      int  iNewCommandLineLength;

      // calculate base length for the new command line
      iNewCommandLineLength = strlen(szAppName) + strlen(lpCommandLine);

      if(SubSystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
        lpszExecutable = szPECmdLoader;
      else
        lpszExecutable = szPEGUILoader;

      // 2002-04-24 PH
      // set the ODIN32.DEBUG_CHILD environment variable to start new PE processes
      // under a new instance of the (IPMD) debugger.
      const char *pszDebugChildArg = "";
#ifdef DEBUG
      char          szDebugChild[512];
      const char   *pszChildDebugger = getenv("ODIN32.DEBUG_CHILD");
      if (pszChildDebugger)
      {
        /*
         * Change the executable to the debugger (icsdebug.exe) and
         * move the previous executable onto the commandline.
         */
        szDebugChild[0] = ' ';
        strcpy(&szDebugChild[1], lpszExecutable);
        iNewCommandLineLength += strlen(&szDebugChild[0]);

        pszDebugChildArg = &szDebugChild[0];
        lpszExecutable = pszChildDebugger;
      }
#endif

        //SvL: Allright. Before we call O32_CreateProcess, we must take care of
        //     lpCurrentDirectory ourselves. (Open32 ignores it!)
        if(lpCurrentDirectory) {
            char *newcmdline;

            newcmdline = (char *)malloc(strlen(lpCurrentDirectory) + iNewCommandLineLength + 64);
            sprintf(newcmdline, "%s /OPT:[CURDIR=%s] %s %s", pszDebugChildArg, lpCurrentDirectory, szAppName, lpCommandLine);
            free(cmdline);
            cmdline = newcmdline;
        }
        else {
            char *newcmdline;

            newcmdline = (char *)malloc(iNewCommandLineLength + 16);
            sprintf(newcmdline, "%s %s %s", pszDebugChildArg, szAppName, lpCommandLine);
            free(cmdline);
            cmdline = newcmdline;
        }

        dprintf(("KERNEL32: CreateProcess starting [%s],[%s]",
                 lpszExecutable,
                 cmdline));

        rc = O32_CreateProcessA(lpszExecutable, (LPCSTR)cmdline,lpProcessAttributes,
                                lpThreadAttributes, bInheritHandles, dwCreationFlags,
                                lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                                lpProcessInfo);
    }
    else
    if(fNEExe) {//16 bits windows app
        char *newcmdline;

        newcmdline = (char *)malloc(strlen(szAppName) + strlen(cmdline) + strlen(szPEGUILoader) + strlen(lpCommandLine) + 32);

        sprintf(newcmdline, " /PELDR=[%s] %s", szPEGUILoader, szAppName, lpCommandLine);
        free(cmdline);
        cmdline = newcmdline;
        //Force Open32 to use DosStartSession (DosExecPgm won't do)
        dwCreationFlags |= CREATE_NEW_PROCESS_GROUP;

        dprintf(("KERNEL32: CreateProcess starting [%s],[%s]",
                 szNELoader,
                 cmdline));
        rc = O32_CreateProcessA(szNELoader, (LPCSTR)cmdline, lpProcessAttributes,
                                lpThreadAttributes, bInheritHandles, dwCreationFlags,
                                lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                                lpProcessInfo);
    }
    else {//os/2 app??
        rc = O32_CreateProcessA(szAppName, (LPCSTR)lpCommandLine, lpProcessAttributes,
                                lpThreadAttributes, bInheritHandles, dwCreationFlags,
                                lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                                lpProcessInfo);
    }
    if(!lpEnvironment) {
        // Restore old ENDLIBPATH variable
        // TODO:
    }

    if(rc == TRUE)
    {
      if (dwCreationFlags & DEBUG_PROCESS && pThreadDB != NULL)
      {
        if(pThreadDB->o.odin.pidDebuggee != 0)
        {
          // TODO: handle this
          dprintf(("KERNEL32: CreateProcess ERROR: This thread is already a debugger\n"));
        }
        else
        {
          pThreadDB->o.odin.pidDebuggee = lpProcessInfo->dwProcessId;
          OSLibStartDebugger((ULONG*)&pThreadDB->o.odin.pidDebuggee);
        }
      }
      else
        pThreadDB->o.odin.pidDebuggee = 0;
    }
    if(lpProcessInfo)
    {
        lpProcessInfo->dwThreadId = MAKE_THREADID(lpProcessInfo->dwProcessId, lpProcessInfo->dwThreadId);
        dprintf(("KERNEL32:  CreateProcess returned %d hPro:%x hThr:%x pid:%x tid:%x\n",
                 rc, lpProcessInfo->hProcess, lpProcessInfo->hThread,
                 lpProcessInfo->dwProcessId,lpProcessInfo->dwThreadId));
    }
    else
      dprintf(("KERNEL32:  CreateProcess returned %d\n", rc));

finished:

    if(oldlibpath) {
        OSLibSetBeginLibpathA(oldlibpath);
        free(oldlibpath);
    }
    if(cmdline) free(cmdline);
    if(newenv)  free(newenv);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
                             PSECURITY_ATTRIBUTES lpProcessAttributes,
                             PSECURITY_ATTRIBUTES lpThreadAttributes,
                             BOOL bInheritHandles, DWORD dwCreationFlags,
                             LPVOID lpEnvironment,
                             LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
                             LPPROCESS_INFORMATION lpProcessInfo)
{
 BOOL rc;
 char *astring1 = 0, *astring2 = 0, *astring3 = 0;

    dprintf(("KERNEL32: CreateProcessW"));
    if(lpApplicationName)
        astring1 = UnicodeToAsciiString((LPWSTR)lpApplicationName);
    if(lpCommandLine)
        astring2 = UnicodeToAsciiString(lpCommandLine);
    if(lpCurrentDirectory)
        astring3 = UnicodeToAsciiString((LPWSTR)lpCurrentDirectory);
    if(lpEnvironment) {
        // use a special flag instead of converting the environment here
        dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
    }
    rc = CreateProcessA(astring1, astring2, lpProcessAttributes, lpThreadAttributes,
                        bInheritHandles, dwCreationFlags, lpEnvironment,
                        astring3, (LPSTARTUPINFOA)lpStartupInfo,
                        lpProcessInfo);
    if(astring3)    FreeAsciiString(astring3);
    if(astring2)    FreeAsciiString(astring2);
    if(astring1)    FreeAsciiString(astring1);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HINSTANCE WIN32API WinExec(LPCSTR lpCmdLine, UINT nCmdShow)
{
 STARTUPINFOA        startinfo = {0};
 PROCESS_INFORMATION procinfo;
 DWORD               rc;
 HINSTANCE           hInstance;

    dprintf(("KERNEL32: WinExec lpCmdLine='%s' nCmdShow=%d\n", lpCmdLine));
    startinfo.cb = sizeof(startinfo);
    startinfo.dwFlags = STARTF_USESHOWWINDOW;
    startinfo.wShowWindow = nCmdShow;
    if(CreateProcessA(NULL, (LPSTR)lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL,
                      &startinfo, &procinfo) == FALSE)
    {
        hInstance = (HINSTANCE)GetLastError();
        if(hInstance >= 32) {
            hInstance = 11;
        }
        dprintf(("KERNEL32: WinExec failed with rc %d", hInstance));
        return hInstance;
    }
    //block until the launched app waits for input (or a timeout of 15 seconds)
    //TODO: Shouldn't call Open32, but the api in user32..
    if(fVersionWarp3) {
        Sleep(1000); //WaitForInputIdle not available in Warp 3
    }
    else {
        dprintf(("Calling WaitForInputIdle %x %d", procinfo.hProcess, 15000));
        rc = WaitForInputIdle(procinfo.hProcess, 15000);
#ifdef DEBUG
        if(rc != 0) {
            dprintf(("WinExec: WaitForInputIdle %x returned %x", procinfo.hProcess, rc));
        }
        else dprintf(("WinExec: WaitForInputIdle successfull"));
#endif
    }
    CloseHandle(procinfo.hThread);
    CloseHandle(procinfo.hProcess);
    return 33;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API WaitForInputIdle(HANDLE hProcess, DWORD dwTimeOut)
{
  dprintf(("USER32: WaitForInputIdle %x %d\n", hProcess, dwTimeOut));

  if(fVersionWarp3) {
        Sleep(1000);
        return 0;
  }
  else  return O32_WaitForInputIdle(hProcess, dwTimeOut);
}
/**********************************************************************
 * LoadModule    (KERNEL32.499)
 *
 * Wine: 20000909
 *
 * Copyright 1995 Alexandre Julliard
 */
HINSTANCE WINAPI LoadModule( LPCSTR name, LPVOID paramBlock )
{
    LOADPARAMS *params = (LOADPARAMS *)paramBlock;
    PROCESS_INFORMATION info;
    STARTUPINFOA startup;
    HINSTANCE hInstance;
    LPSTR cmdline, p;
    char filename[MAX_PATH];
    BYTE len;

    dprintf(("LoadModule %s %x", name, paramBlock));

    if (!name) return ERROR_FILE_NOT_FOUND;

    if (!SearchPathA( NULL, name, ".exe", sizeof(filename), filename, NULL ) &&
        !SearchPathA( NULL, name, NULL, sizeof(filename), filename, NULL ))
        return GetLastError();

    len = (BYTE)params->lpCmdLine[0];
    if (!(cmdline = (LPSTR)HeapAlloc( GetProcessHeap(), 0, strlen(filename) + len + 2 )))
        return ERROR_NOT_ENOUGH_MEMORY;

    strcpy( cmdline, filename );
    p = cmdline + strlen(cmdline);
    *p++ = ' ';
    memcpy( p, params->lpCmdLine + 1, len );
    p[len] = 0;

    memset( &startup, 0, sizeof(startup) );
    startup.cb = sizeof(startup);
    if (params->lpCmdShow)
    {
        startup.dwFlags = STARTF_USESHOWWINDOW;
        startup.wShowWindow = params->lpCmdShow[1];
    }

    if (CreateProcessA( filename, cmdline, NULL, NULL, FALSE, 0,
                        params->lpEnvAddress, NULL, &startup, &info ))
    {
        /* Give 15 seconds to the app to come up */
        if ( WaitForInputIdle ( info.hProcess, 15000 ) ==  0xFFFFFFFF )
            dprintf(("ERROR: WaitForInputIdle failed: Error %ld\n", GetLastError() ));
        hInstance = 33;
        /* Close off the handles */
        CloseHandle( info.hThread );
        CloseHandle( info.hProcess );
    }
    else if ((hInstance = GetLastError()) >= 32)
    {
        dprintf(("ERROR: Strange error set by CreateProcess: %d\n", hInstance ));
        hInstance = 11;
    }

    HeapFree( GetProcessHeap(), 0, cmdline );
    return hInstance;
}
//******************************************************************************
//******************************************************************************
FARPROC WIN32API GetProcAddress(HMODULE hModule, LPCSTR lpszProc)
{
 Win32ImageBase *winmod;
 FARPROC   proc = 0;
 ULONG     ulAPIOrdinal;

  if(hModule == 0 || hModule == -1 || (WinExe && hModule == WinExe->getInstanceHandle())) {
        winmod = WinExe;
  }
  else  winmod = (Win32ImageBase *)Win32DllBase::findModule((HINSTANCE)hModule);

  if(winmod) {
        ulAPIOrdinal = (ULONG)lpszProc;
        if (ulAPIOrdinal <= 0x0000FFFF) {
                proc = (FARPROC)winmod->getApi((int)ulAPIOrdinal);
        }
        else
        if (lpszProc && *lpszProc) {
                proc = (FARPROC)winmod->getApi((char *)lpszProc);
        }
        if(proc == 0) {
#ifdef DEBUG
                if(ulAPIOrdinal <= 0x0000FFFF) {
                        dprintf(("GetProcAddress %x %x not found!", hModule, ulAPIOrdinal));
                }
                else    dprintf(("GetProcAddress %x %s not found!", hModule, lpszProc));
#endif
                SetLastError(ERROR_PROC_NOT_FOUND);
                return 0;
        }
        if(HIWORD(lpszProc))
                dprintf(("KERNEL32:  GetProcAddress %s from %X returned %X\n", lpszProc, hModule, proc));
        else    dprintf(("KERNEL32:  GetProcAddress %x from %X returned %X\n", lpszProc, hModule, proc));

        SetLastError(ERROR_SUCCESS);
        return proc;
  }
  proc = (FARPROC)OSLibDosGetProcAddress(hModule, lpszProc);
  if(HIWORD(lpszProc))
        dprintf(("KERNEL32:  GetProcAddress %s from %X returned %X\n", lpszProc, hModule, proc));
  else  dprintf(("KERNEL32:  GetProcAddress %x from %X returned %X\n", lpszProc, hModule, proc));
  SetLastError(ERROR_SUCCESS);
  return(proc);
}
//******************************************************************************
// ODIN_SetProcAddress: Override a dll export
//
// Parameters:
//      HMODULE hModule		Module handle
//      LPCSTR  lpszProc	Export name or ordinal
//      FARPROC pfnNewProc	New export function address
//
// Returns: Success -> old address of export
//          Failure -> -1
//
//******************************************************************************
FARPROC WIN32API ODIN_SetProcAddress(HMODULE hModule, LPCSTR lpszProc,
                                     FARPROC pfnNewProc)
{
 Win32ImageBase *winmod;
 FARPROC   proc;
 ULONG     ulAPIOrdinal;

  if(hModule == 0 || hModule == -1 || (WinExe && hModule == WinExe->getInstanceHandle())) {
        winmod = WinExe;
  }
  else  winmod = (Win32ImageBase *)Win32DllBase::findModule((HINSTANCE)hModule);

  if(winmod) {
        ulAPIOrdinal = (ULONG)lpszProc;
        if (ulAPIOrdinal <= 0x0000FFFF) {
                proc = (FARPROC)winmod->setApi((int)ulAPIOrdinal, (ULONG)pfnNewProc);
        }
        else    proc = (FARPROC)winmod->setApi((char *)lpszProc, (ULONG)pfnNewProc);
        if(proc == 0) {
#ifdef DEBUG
                if(ulAPIOrdinal <= 0x0000FFFF) {
                        dprintf(("ODIN_SetProcAddress %x %x not found!", hModule, ulAPIOrdinal));
                }
                else    dprintf(("ODIN_SetProcAddress %x %s not found!", hModule, lpszProc));
#endif
                SetLastError(ERROR_PROC_NOT_FOUND);
                return (FARPROC)-1;
        }
        if(HIWORD(lpszProc))
                dprintf(("KERNEL32:  ODIN_SetProcAddress %s from %X returned %X\n", lpszProc, hModule, proc));
        else    dprintf(("KERNEL32:  ODIN_SetProcAddress %x from %X returned %X\n", lpszProc, hModule, proc));

        SetLastError(ERROR_SUCCESS);
        return proc;
  }
  SetLastError(ERROR_INVALID_HANDLE);
  return (FARPROC)-1;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetProcModuleFileNameA(ULONG lpvAddress, LPSTR lpszFileName, UINT cchFileNameMax)
{
    LPSTR lpszModuleName;
    Win32ImageBase *image = NULL;
    int len;

    dprintf(("GetProcModuleFileNameA %x %x %d", lpvAddress, lpszFileName, cchFileNameMax));

    if(WinExe && WinExe->insideModule(lpvAddress) && WinExe->insideModuleCode(lpvAddress)) {
        image = WinExe;
    }
    else {
        Win32DllBase *dll = Win32DllBase::findModuleByAddr(lpvAddress);
        if(dll && dll->insideModuleCode(lpvAddress)) {
            image = dll;
        }
    }
    if(image == NULL) {
        dprintf(("GetProcModuleFileNameA: address not found!!"));
        return 0;
    }
    len = strlen(image->getFullPath());
    if(len < cchFileNameMax) {
        strcpy(lpszFileName, image->getFullPath());
        return len+1; //??
    }
    else {
        dprintf(("GetProcModuleFileNameA: destination string too small!!"));
        return 0;
    }
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DisableThreadLibraryCalls(HMODULE hModule)
{
  Win32DllBase *winmod;
  FARPROC   proc;
  ULONG     ulAPIOrdinal;

  winmod = Win32DllBase::findModule((HINSTANCE)hModule);
  if(winmod)
  {
    // don't call ATTACH/DETACH thread functions in DLL
    winmod->disableThreadLibraryCalls();
    return TRUE;
  }
  else
  {
    // raise error condition
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }
}
//******************************************************************************
// Forwarder for PSAPI.DLL
//
// Returns the handles of all loaded modules
//
//******************************************************************************
BOOL WINAPI PSAPI_EnumProcessModules(HANDLE hProcess, HMODULE *lphModule,
                                     DWORD cb, LPDWORD lpcbNeeded)
{
  DWORD	count;
  DWORD	countMax;
  HMODULE     hModule;

  dprintf(("KERNEL32: EnumProcessModules %p, %ld, %p", lphModule, cb, lpcbNeeded));

  if ( lphModule == NULL )
        cb = 0;

  if ( lpcbNeeded != NULL )
       *lpcbNeeded = 0;

  count = 0;
  countMax = cb / sizeof(HMODULE);

  count = Win32DllBase::enumDlls(lphModule, countMax);

  if ( lpcbNeeded != NULL )
     *lpcbNeeded = sizeof(HMODULE) * count;

  return TRUE;
}
//******************************************************************************
// Forwarder for PSAPI.DLL
//
// Returns some information about the module identified by hModule
//
//******************************************************************************
BOOL WINAPI PSAPI_GetModuleInformation(HANDLE hProcess, HMODULE hModule,
                                       LPMODULEINFO lpmodinfo, DWORD cb)
{
    BOOL ret = FALSE;
    Win32DllBase *winmod = NULL;

    dprintf(("KERNEL32: GetModuleInformation hModule=%x", hModule));

    if (!lpmodinfo || cb < sizeof(MODULEINFO)) return FALSE;

    winmod = Win32DllBase::findModule((HINSTANCE)hModule);
    if (!winmod) {
        dprintf(("GetModuleInformation failed to find module"));
        return FALSE;
    }

    lpmodinfo->SizeOfImage = winmod->getImageSize();
    lpmodinfo->EntryPoint  = (LPVOID)winmod->getEntryPoint();
    lpmodinfo->lpBaseOfDll = (void*)hModule;

    return TRUE;
}
//******************************************************************************
//******************************************************************************
/**
 * Gets the startup info which was passed to CreateProcess when
 * this process was created.
 *
 * @param   lpStartupInfo   Where to put the startup info.
 *                          Please don't change the strings :)
 * @status  Partially Implemented.
 * @author  knut st. osmundsen <bird@anduin.net>
 * @remark  The three pointers of the structure is just fake.
 * @remark  Pretty much identical to current wine code.
 */
void WIN32API GetStartupInfoA(LPSTARTUPINFOA lpStartupInfo)
{
    dprintf2(("KERNEL32: GetStartupInfoA %x\n", lpStartupInfo));
    *lpStartupInfo = StartupInfo;
}


/**
 * Gets the startup info which was passed to CreateProcess when
 * this process was created.
 *
 * @param   lpStartupInfo   Where to put the startup info.
 *                          Please don't change the strings :)
 * @status  Partially Implemented.
 * @author  knut st. osmundsen <bird@anduin.net>
 * @remark  The three pointers of the structure is just fake.
 * @remark  Similar to wine code, but they use RtlCreateUnicodeStringFromAsciiz
 *          for creating the UNICODE strings and are doing so for each call.
 *          As I don't wanna call NTDLL code from kernel32 I take the easy path.
 */
void WIN32API GetStartupInfoW(LPSTARTUPINFOW lpStartupInfo)
{
    /*
     * Converted once, this information shouldn't change...
     */

    dprintf2(("KERNEL32: GetStartupInfoW %x\n", lpStartupInfo));

    //assumes the structs are identical but for the strings pointed to.
    memcpy(lpStartupInfo, &StartupInfo, sizeof(STARTUPINFOA));
    lpStartupInfo->cb = sizeof(STARTUPINFOW); /* this really should be the same size else we're in for trouble.. :) */

    /*
     * First time conversion only as this should be pretty static.
     * See remark!
     */
    static LPWSTR pwcReserved = NULL;
    static LPWSTR pwcDesktop = NULL;
    static LPWSTR pwcTitle = NULL;

    if (lpStartupInfo->lpReserved && pwcReserved)
        pwcReserved = AsciiToUnicodeString((LPCSTR)lpStartupInfo->lpReserved);
    lpStartupInfo->lpReserved = pwcReserved;

    if (lpStartupInfo->lpDesktop && pwcDesktop)
        pwcDesktop = AsciiToUnicodeString((LPCSTR)lpStartupInfo->lpDesktop);
    lpStartupInfo->lpDesktop = pwcDesktop;

    if (lpStartupInfo->lpTitle && pwcTitle)
        pwcTitle = AsciiToUnicodeString((LPCSTR)lpStartupInfo->lpTitle);
    lpStartupInfo->lpTitle = pwcTitle;
}

} // extern "C"

