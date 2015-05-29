/* $Id: initkernel32.cpp,v 1.28 2004-05-24 08:56:06 sandervl Exp $
 *
 * KERNEL32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

//~ #undef  DBG_CON
//~ #define DBG_CON

#define  INCL_DOSMODULEMGR
#define  INCL_DOSMISC
#define  INCL_DOSPROCESS
#define  INCL_DOSSEMAPHORES
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include <wprocess.h>
#include "HandleManager.h"
#include "profile.h"
#include <options.h>
#include "initterm.h"
#include <win32type.h>
#include <win32api.h>
#include <odinlx.h>
#include "oslibmisc.h"
#include <heapshared.h>
#include <heapcode.h>
#include "mmap.h"
#include "directory.h"
#include "hmdevio.h"
#include "hmcomm.h"
#include "windllbase.h"
#include "winexepe2lx.h"
#include <exitlist.h>
#include "oslibdos.h"
#include "osliblvm.h"
#include <cpuhlp.h>
#include <win32k.h>
#include <initdll.h>
#include <codepage.h>
#include <process.h>
#include <stats.h>
#include <heapshared.h>
#include <heapstring.h>
#include <_ras.h>

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

PVOID   SYSTEM _O32_GetEnvironmentStrings( VOID );

// Win32 resource table (produced by wrc)
extern DWORD kernel32_PEResTab;

static HMODULE dllHandle = 0;

extern PFN pfnImSetMsgQueueProperty;

BOOL    fVersionWarp3 = FALSE;
BOOL    fCustomBuild  = FALSE;

ULONG   flAllocMem = 0;    /* flag to optimize DosAllocMem to use all the memory on SMP machines */
ULONG   ulMaxAddr = 0x20000000; /* end of user address space. */
char    kernel32Path[CCHMAXPATH] = "";
BOOL    fInit     = FALSE;
BOOL    fWin32k   = FALSE;
HMODULE imHandle = 0;
char    szModName[ 256 ] = "";

/// Exported dummy function to indicate swt-branch -- to be removed later.
BOOL WINAPI odin32swt() {
    return TRUE;
}

static ULONG DLL_InitKernel32_internal(ULONG hModule)
{
    size_t i;
    APIRET rc;
    ULONG  ulSysinfo, version[2];

    if (fInit)
        return EXITLIST_KERNEL32; // already initialized

    rc = DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MINOR,
                         version, sizeof(version));
    if (rc == 0)
        if(version[0] >= 20 && version[1] <= 30)
            fVersionWarp3 = TRUE;

    // This always must be the first thing to do.
    RasInitialize (hModule);
#ifdef RAS
    extern void rasInitVirtual (void);
    rasInitVirtual ();
#endif

    ParseLogStatusKERNEL32();

    /*
     * Init the win32k library.
     * We will also need to tell win32k where the Odin32 environment is
     * located. Currently that is within Open32. I'm quite sure that it's
     * not relocated during run, so we're pretty well off.
     */
    //Note: we do NOT want to use any win32k services with custom builds
    if (fCustomBuild == FALSE && !libWin32kInit())
    {
        rc = libWin32kSetEnvironment((PSZ)_O32_GetEnvironmentStrings(), 0, 0);
        if (rc)
        {
            dprintf(("KERNEL32: initterm: libWin32kSetEnvironment failed with rc=%d\n", rc));
        }
        else fWin32k = TRUE;
    }

    char *kernel32Name = OSLibGetDllName(hModule);
    if (!kernel32Name)
        return -1; // failure

    strcpy(kernel32Path, kernel32Name);
    char *endofpath = strrchr(kernel32Path, '\\');
    *(endofpath+1) = 0;

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    /* knut: check for high memory support */
    rc = DosQuerySysInfo(QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT, &ulSysinfo, sizeof(ulSysinfo));
    if (rc == 0 && ulSysinfo > 512)   //VirtualAddresslimit is in MB
    {
        flAllocMem = PAG_ANY;      // high memory support. Let's use it!
        ulMaxAddr = ulSysinfo * (1024*1024);
    }
    else
        flAllocMem = 0;        // no high memory support

    dprintf(("kernel32 init %s %s (%x) Win32k - %s", __DATE__, __TIME__, DLL_InitKernel32_internal,
             libWin32kInstalled() ? "Installed" : "Not Installed"));

    OpenPrivateLogFiles();

    //SvL: Do it here instead of during the exe object creation
    //(std handles can be used in win32 dll initialization routines
    if (HMInitialize() != NO_ERROR)
        return -1;

    // VP: Shared heap should be initialized before call to PROFILE_*
    // because they use a critical section which in turn uses smalloc
    // in debug build
    if (!InitializeSharedHeap())
        return -1;

    // VP: initialize profile internal data (critical section actually).
    // This was done in PROFILE_LoadOdinIni but PROFILE_GetOdinIniInt
    // is called earlier and this lead to a handle leak.
    PROFILE_Initialize();

    if(flAllocMem == PAG_ANY)
    {
        OSLibInitWSeBFileIO();
        if (PROFILE_GetOdinIniInt(ODINSYSTEM_SECTION, HIGHMEM_KEY, 1) == 0)
        {
            dprintf(("WARNING: OS/2 kernel supports high memory, but support is DISABLED because of HIGHMEM odin.ini key"));
            flAllocMem = 0;
        }
    }

    if (!InitializeCodeHeap())
        return -1;

    InitializeMemMaps();

    PROFILE_LoadOdinIni();
    dllHandle = RegisterLxDll(hModule, 0, (PVOID)&kernel32_PEResTab);
    if (dllHandle == 0)
        return -1;

    //SvL: Kernel32 is a special case; pe.exe loads it, so increase
    //     the reference count here
    Win32DllBase *module = Win32DllBase::findModule(dllHandle);
    if (module)
    {
        module->AddRef();
        module->DisableUnload();
    }

    OSLibDosSetInitialMaxFileHandles(ODIN_DEFAULT_MAX_FILEHANDLES);

#ifdef DEBUG
    {
        LPSTR WIN32API GetEnvironmentStringsA();

        char *tmpenvnew = GetEnvironmentStringsA();
        dprintf(("Environment:"));
        while(*tmpenvnew) {
            dprintf(("%s", tmpenvnew));
            tmpenvnew += strlen(tmpenvnew)+1;
        }
    }
#endif

    // Must be done before InitializeTIB (which loads NTDLL -> USER32)
    InitDirectories();

    // Must be done after HMInitialize!
    if (InitializeMainThread() == NULL)
        return -1;

    RegisterDevices();
    Win32DllBase::setDefaultRenaming();

    rc = DosQuerySysInfo(QSV_NUMPROCESSORS, QSV_NUMPROCESSORS,
                         &ulSysinfo, sizeof(ulSysinfo));
    if (rc != 0)
        ulSysinfo = 1;

    // Setup codepage info
    CODEPAGE_Init();

    if (IsDBCSEnv() && DosLoadModule(szModName, sizeof( szModName ),
                                     "OS2IM", &imHandle) == 0)
        DosQueryProcAddr(imHandle, 140, NULL, &pfnImSetMsgQueueProperty);

    InitSystemInfo(ulSysinfo);

    // Set up environment as found in NT
    InitEnvironment(ulSysinfo);

    // InitDynamicRegistry creates/changes keys that may change (i.e.
    // odin.ini keys that affect windows version)
    InitDynamicRegistry();

    // Set the process affinity mask to the system affinity mask
    DWORD dwProcessAffinityMask, dwSystemAffinityMask;
    GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinityMask,
                           &dwSystemAffinityMask);
    SetProcessAffinityMask(GetCurrentProcess(), dwSystemAffinityMask);

    // Set default paths for PE & NE loaders
    if (!InitLoaders())
        return -1;

    RasEntry(RAS_EVENT_Kernel32InitComplete,
             &dllHandle, sizeof (dllHandle));

    fInit = TRUE;

    return EXITLIST_KERNEL32;
}

ULONG SYSTEM DLL_InitKernel32(ULONG hModule)
{
    __con_debug(2,"%s::%s@%08X(%08X)\n","kernel32.dll",__FUNCTION__,DLL_InitKernel32,hModule);
    ULONG code = DLL_InitKernel32_internal(hModule);

    if (code == -1)
        ReportFatalDllInitError("KERNEL32");

    return code;
}

void SYSTEM DLL_TermKernel32(ULONG hModule)
{
    __con_debug(2,"%s::%s@%08X(%08X)\n","kernel32.dll",__FUNCTION__,DLL_TermKernel32,hModule);
    if (!fInit)
    {
        // The initialization sequence was not complete; attempting to
        // uninitialize some things may crash (yeah, dirty code that doesn't
        // analyze its own state)
        return;
    }

    dprintf(("kernel32 exit"));

    if( IsDBCSEnv() && imHandle )
        DosFreeModule( imHandle );

    //Flush and delete all open memory mapped files
    Win32MemMap::deleteAll();
    WinExe = NULL;

    FinalizeMemMaps();

    WriteOutProfiles();
    //Unload LVM subsystem for volume/mountpoint win32 functions
    OSLibLVMExit();

    // Note: unwinding win32 exceptions before destroying TEB like we do in
    // ExitThread()/ExitProcess() is impossible here since the stack is already
    // vanished at this point. In cases where process termination is not coming
    // from ExitThread()/ExitProcess(), unwinding is to be done by
    // OS2ExceptionHandler2ndLevel() in responce to the normal unwind procedure.

    TEB *teb = GetThreadTEB();
    if(teb) DestroyTEB(teb);

    DestroySharedHeap();
    DestroyCodeHeap();

    HMTerminate(); /* shutdown handlemanager */

#ifdef DEBUG
    extern void printCriticalSectionStatistic (void);
    printCriticalSectionStatistic ();
#endif

#if defined(DEBUG) && defined(__IBMCPP__) && __IBMCPP__ == 300
    ULONG totalmemalloc, nrcalls_malloc, nrcalls_free;

    getcrtstat(&nrcalls_malloc, &nrcalls_free, &totalmemalloc);
    dprintf(("*************  KERNEL32 STATISTICS BEGIN *****************"));
    dprintf(("Total nr of malloc calls %d", nrcalls_malloc));
    dprintf(("Total nr of free   calls %d", nrcalls_free));
    dprintf(("Leaked memory: %d bytes", totalmemalloc));
    dprintf(("*************  KERNEL32 STATISTICS END   *****************"));

    //SvL: This can cause an exitlist hang; disabled for now
////    _dump_allocated(0);
#endif

    //NOTE: Must be done after DestroyTIB
    ClosePrivateLogFiles();

#ifndef DEBUG
    //if we do a dump of the shared heap, then we'll need the logging facility
    //for a little while longer
    CloseLogFile();
#endif

    /*
     * Terminate win32k library.
     */
    libWin32kSetEnvironment(NULL, 0, 0);
    libWin32kTerm();

    RasUninitialize ();

    if (dllHandle)
        UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitKernel32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermKernel32(hModule);
    DLL_TermDefault(hModule);
}

ULONG APIENTRY _O32__DLL_InitTerm(ULONG handle, ULONG flag);

BOOL APIENTRY InitializeKernel32()
{
    HMODULE hModule;

    BOOL WGSS_OK = FALSE;

    if (DosQueryModuleHandleStrict("WGSS50", &hModule) == NO_ERROR)
    {
        if (_O32__DLL_InitTerm(hModule, 0) != 0)
        {
            WGSS_OK = TRUE;

            if (DosQueryModuleHandleStrict("KERNEL32", &hModule) == NO_ERROR &&
                DLL_Init(hModule) != -1)
                return TRUE;

            ReportFatalDllInitError("KERNEL32");
        }
    }

    if (!WGSS_OK)
        ReportFatalDllInitError("WGSS50");

    return FALSE; // failure
}

VOID APIENTRY ReportFatalDllInitError(PCSZ pszModName)
{
    static const char msg1[] =
        "Failed to initialize the ";
    static const char msg2[] =
        " library while starting \"";
    static const char msg3[] =
        "\".\n\r"
        "\n\r"
        "It is possible that there is not enough memory in the system to "
        "run this application. Please close other applications and try "
        "again. If the problem persists, please report the details by "
        "creating a ticket at http://svn.netlabs.org/odin32/.\n\r";

    char msg[sizeof(msg1) + 8 + sizeof(msg2) + CCHMAXPATH + sizeof(msg3)];

    strcpy(msg, msg1);
    strncat(msg, pszModName, 8);
    strcat(msg, msg2);

    PPIB ppib;
    DosGetInfoBlocks(NULL, &ppib);
    if (DosQueryModuleName(ppib->pib_hmte, CCHMAXPATH,
                           msg + strlen(msg)) != NO_ERROR)
        strcat(msg, "<unknown executable>");
    strcat(msg, msg3);

    BOOL haveHMQ = FALSE;
    MQINFO mqinfo;
    if (WinQueryQueueInfo(1 /*HMQ_CURRENT*/, &mqinfo, sizeof(mqinfo)) == FALSE)
    {
        // attempt to initialize PM and try again
        HAB hab = WinInitialize(0);
        if (hab)
        {
            HMQ hmq = WinCreateMsgQueue(hab, 0);
            if (hmq)
                haveHMQ = TRUE;
        }
    }
    else
        haveHMQ = TRUE;

    WinMessageBox(HWND_DESKTOP, NULL, msg, "Odin: Fatal Error", 0,
                  MB_APPLMODAL | MB_MOVEABLE | MB_ERROR | MB_OK);

    // duplicate the message to the console just in case (PM may be not
    // available)
    ULONG dummy;
    DosWrite((HFILE)1, (PVOID)&msg, strlen(msg), &dummy);
}
