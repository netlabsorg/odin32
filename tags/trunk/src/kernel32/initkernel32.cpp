/* $Id: initkernel32.cpp,v 1.28 2004-05-24 08:56:06 sandervl Exp $
 *
 * KERNEL32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*-------------------------------------------------------------*/
/* INITERM.C -- Source for a custom dynamic link library       */
/*              initialization and termination (_DLL_InitTerm) */
/*              function.                                      */
/*                                                             */
/* When called to perform initialization, this sample function */
/* gets storage for an array of integers, and initializes its  */
/* elements with random integers.  At termination time, it     */
/* frees the array.  Substitute your own special processing.   */
/*-------------------------------------------------------------*/


/* Include files */
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
#include "handlemanager.h"
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
#include <windllbase.h>
#include "winexepe2lx.h"
#include <exitlist.h>
#include "oslibdos.h"
#include "osliblvm.h"
#include <cpuhlp.h>
#include <Win32k.h>
#include <initdll.h>
#include <codepage.h>
#include <process.h>
#include <stats.h>
#include <heapshared.h>
#include <heapstring.h>

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

PVOID   SYSTEM _O32_GetEnvironmentStrings( VOID );

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD kernel32_PEResTab;
}

extern PFN pfnImSetMsgQueueProperty;

       ULONG   flAllocMem = 0;    /* flag to optimize DosAllocMem to use all the memory on SMP machines */
       ULONG   ulMaxAddr = 0x20000000; /* end of user address space. */
       int     loadNr = 0;
       char    kernel32Path[CCHMAXPATH] = "";
static HMODULE dllHandle = 0;
       BOOL    fInit     = FALSE;
       BOOL    fWin32k   = FALSE;
       HMODULE imHandle = 0;
       char    szModName[ 256 ] = "";

/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
ULONG APIENTRY inittermKernel32(ULONG hModule, ULONG ulFlag)
{
    size_t i;
    APIRET rc;
    ULONG  ulSysinfo, version[2];

    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    if(fInit == TRUE && ulFlag == 0) {
        return 1; //already initialized
    }
    fInit = TRUE;

    switch (ulFlag)
    {
        case 0 :
        {
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
            strcpy(kernel32Path, OSLibGetDllName(hModule));
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

            dprintf(("kernel32 init %s %s (%x) Win32k - %s", __DATE__, __TIME__, inittermKernel32,
                     libWin32kInstalled() ? "Installed" : "Not Installed"));

            OpenPrivateLogFiles();

            //SvL: Do it here instead of during the exe object creation
            //(std handles can be used in win32 dll initialization routines
            HMInitialize();             /* store standard handles within HandleManager */

            // VP: Shared heap should be initialized before call to PROFILE_*
            // because they use a critical section which in turn uses smalloc
            // in debug build
            if (InitializeSharedHeap() == FALSE)
                return 0UL;

            // VP: initialize profile internal data (critical section actually).
            // This was done in PROFILE_LoadOdinIni but PROFILE_GetOdinIniInt
            // is called earlier and this lead to a handle leak.
            PROFILE_Initialize();

            if(flAllocMem == PAG_ANY) {
                OSLibInitWSeBFileIO();
                if (PROFILE_GetOdinIniInt(ODINSYSTEM_SECTION, HIGHMEM_KEY, 1) == 0) {
                    dprintf(("WARNING: OS/2 kernel supports high memory, but support is DISABLED because of HIGHMEM odin.ini key"));
                    flAllocMem = 0;
                }
            }

            if (InitializeCodeHeap() == FALSE)
                return 0UL;

            InitializeMemMaps();

            PROFILE_LoadOdinIni();
            dllHandle = RegisterLxDll(hModule, 0, (PVOID)&kernel32_PEResTab);
            if (dllHandle == 0)
                return 0UL;

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

            InitDirectories();          //Must be done before InitializeTIB (which loads NTDLL -> USER32)
            InitializeMainThread();     //Must be done after HMInitialize!
            RegisterDevices();
            Win32DllBase::setDefaultRenaming();
            rc = DosQuerySysInfo(QSV_NUMPROCESSORS, QSV_NUMPROCESSORS, &ulSysinfo, sizeof(ulSysinfo));
            if (rc != 0)
                ulSysinfo = 1;

            /* Setup codepage info */
            CODEPAGE_Init();

            if( IsDBCSEnv() && DosLoadModule( szModName, sizeof( szModName ), "OS2IM.DLL", &imHandle ) == 0 )
                DosQueryProcAddr( imHandle, 140, NULL, &pfnImSetMsgQueueProperty );

            InitSystemInfo(ulSysinfo);
            //Set up environment as found in NT
            InitEnvironment(ulSysinfo);

            //InitDynamicRegistry creates/changes keys that may change (i.e. odin.ini
            //keys that affect windows version)
            InitDynamicRegistry();

            //Set the process affinity mask to the system affinity mask
            DWORD dwProcessAffinityMask, dwSystemAffinityMask;
            GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinityMask, &dwSystemAffinityMask);
            SetProcessAffinityMask(GetCurrentProcess(), dwSystemAffinityMask);

            //Set default paths for PE & NE loaders
            InitLoaders();
            break;
        }

        case 1 :
            if (dllHandle)
            {
                UnregisterLxDll(dllHandle);
            }
            break;

        default  :
            return 0UL;
    }

    /***********************************************************/
    /* A non-zero value must be returned to indicate success.  */
    /***********************************************************/
    return 1UL;
}
//******************************************************************************
//******************************************************************************
void APIENTRY cleanupKernel32(ULONG ulReason)
{
    dprintf(("kernel32 exit %d\n", ulReason));

    if( IsDBCSEnv() && imHandle )
        DosFreeModule( imHandle );

    //Flush and delete all open memory mapped files
    Win32MemMap::deleteAll();
    WinExe = NULL;

    WriteOutProfiles();
    //Unload LVM subsystem for volume/mountpoint win32 functions
    OSLibLVMExit();

    TEB *teb = GetThreadTEB();
    if(teb) DestroyTEB(teb);
    DestroySharedHeap();
    DestroyCodeHeap();

    HMTerminate(); /* shutdown handlemanager */

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
    CloseLogFile();

    /*
     * Terminate win32k library.
     */
    libWin32kSetEnvironment(NULL, 0, 0);
    libWin32kTerm();
    return ;
}
//******************************************************************************
//******************************************************************************
