/* $Id: wprocess.cpp,v 1.98 2000-10-02 13:38:58 sandervl Exp $ */

/*
 * Win32 process functions
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * NOTE: Even though Odin32 OS/2 apps don't switch FS selectors,
 *       we still allocate a TEB to store misc information.
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

#include <unicode.h>
#include "windllbase.h"
#include "winexebase.h"
#include "windllpeldr.h"
#include "winexepeldr.h"
#include "winfakepeldr.h"
#include "windlllx.h"
#include <vmutex.h>
#include <handlemanager.h>

#ifdef __IBMCPP__
#include <builtin.h>
#endif

#include "odin32validate.h"
#include "exceptutil.h"
#include "oslibdos.h"
#include "oslibmisc.h"
#include "oslibdebug.h"

#include "console.h"
#include "cio.h"
#include "versionos2.h"    /*PLF Wed  98-03-18 02:36:51*/
#include <wprocess.h>
#include "mmap.h"

#define DBG_LOCALLOG    DBG_wprocess
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-WPROCESS)


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
BOOL    fIsOS2Image = FALSE;            /* TRUE  -> Odin32 OS/2 application (not converted!) */
                                        /* FALSE -> otherwise */
BOOL    fExitProcess = FALSE;

//Commandlines
PCSTR   pszCmdLineA;                    /* ASCII/ANSII commandline. */
PCWSTR  pszCmdLineW;                    /* Unicode commandline. */

//Process database
PDB     ProcessPDB = {0};
USHORT  ProcessTIBSel = 0;
DWORD  *TIBFlatPtr    = 0;

//list of thread database structures
static THDB     *threadList = 0;
static VMutex    threadListMutex;

//TODO: This should not be here: (need to rearrange NTDLL; kernel32 can't depend on ntdll)
BOOLEAN (* WINAPI RtlAllocateAndInitializeSid) ( PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
                                                 BYTE nSubAuthorityCount,
                                                 DWORD nSubAuthority0,
                                                 DWORD nSubAuthority1,
                                                 DWORD nSubAuthority2,
                                                 DWORD nSubAuthority3,
                                                 DWORD nSubAuthority4,
                                                 DWORD nSubAuthority5,
                                                 DWORD nSubAuthority6,
                                                 DWORD nSubAuthority7,
                                                 PSID *pSid);
static HINSTANCE hInstNTDll = 0;

//******************************************************************************
//******************************************************************************
TEB *WIN32API GetThreadTEB()
{
    if(TIBFlatPtr == NULL) {
        DebugInt3();
        return 0;
    }
    return (TEB *)*TIBFlatPtr;
}
//******************************************************************************
//******************************************************************************
THDB *WIN32API GetThreadTHDB()
{
 TEB  *winteb;
 THDB *thdb;

    if(TIBFlatPtr == NULL)
        return 0;

    winteb = (TEB *)*TIBFlatPtr;
    if(winteb == NULL) {
        return NULL;
    }
    thdb = (THDB *)(winteb+1);

    return thdb;
}
//******************************************************************************
//******************************************************************************
THDB *WIN32API GetTHDBFromThreadId(ULONG threadId)
{
 THDB *thdb = threadList;

    threadListMutex.enter();
    while(thdb) {
        if(thdb->threadId == threadId) {
            break;
        }
        thdb = thdb->next;
    }
    threadListMutex.leave();
    return thdb;
}
//******************************************************************************
//******************************************************************************
THDB *WIN32API GetTHDBFromThreadHandle(HANDLE hThread)
{
 THDB *thdb = threadList;

    threadListMutex.enter();
    while(thdb) {
        if(thdb->hThread == hThread) {
            break;
        }
        thdb = thdb->next;
    }
    threadListMutex.leave();
    return thdb;
}
//******************************************************************************
// Set up the TIB selector and memory for the current thread
//******************************************************************************
TEB *InitializeTIB(BOOL fMainThread)
{
  TEB   *winteb;
  THDB  *thdb;
  ULONG  hThreadMain;
  USHORT tibsel;

    //Allocate one dword to store the flat address of our TEB
    if(fMainThread) {
        TIBFlatPtr = (DWORD *)OSLibAllocThreadLocalMemory(1);
        if(TIBFlatPtr == 0) {
            dprintf(("InitializeTIB: local thread memory alloc failed!!"));
            DebugInt3();
            return NULL;
        }
        //SvL: This doesn't really create a thread, but only sets up the
        //     handle of thread 0
        hThreadMain = HMCreateThread(NULL, 0, 0, 0, 0, 0, TRUE);
    }
    if(OSLibAllocSel(PAGE_SIZE, &tibsel) == FALSE)
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
    memset(winteb, 0, PAGE_SIZE);
    thdb       = (THDB *)(winteb+1);
    *TIBFlatPtr = (DWORD)winteb;

    winteb->except      = (PVOID)-1;               /* 00 Head of exception handling chain */
    winteb->stack_top   = (PVOID)OSLibGetTIB(TIB_STACKTOP); /* 04 Top of thread stack */
    winteb->stack_low   = (PVOID)OSLibGetTIB(TIB_STACKLOW); /* 08 Stack low-water mark */
    winteb->htask16     = (USHORT)OSLibGetPIB(PIB_TASKHNDL); /* 0c Win16 task handle */
    winteb->stack_sel   = getSS();                 /* 0e 16-bit stack selector */
    winteb->self        = winteb;                  /* 18 Pointer to this structure */
    winteb->flags       = TEBF_WIN32;              /* 1c Flags */
    winteb->queue       = 0;                       /* 28 Message queue */
    winteb->tls_ptr     = &thdb->tls_array[0];     /* 2c Pointer to TLS array */
    winteb->process     = &ProcessPDB;             /* 30 owning process (used by NT3.51 applets)*/

    memcpy(&thdb->teb, winteb, sizeof(TEB));
    thdb->process         = &ProcessPDB;
    thdb->exit_code       = 0x103; /* STILL_ACTIVE */
    thdb->teb_sel         = tibsel;
    thdb->OrgTIBSel       = GetFS();
    thdb->pWsockData      = NULL;
    thdb->threadId        = GetCurrentThreadId();
    if(fMainThread) {
        thdb->hThread     = hThreadMain;
    }
    else thdb->hThread    = GetCurrentThread();
    thdb->lcid            = GetUserDefaultLCID();

    threadListMutex.enter();
    THDB *thdblast        = threadList;
    if(!thdblast) {
        threadList = thdb;
    }
    else {
        while(thdblast->next) {
            thdblast = thdblast->next;
        }
        thdblast->next   = thdb;
    }
    thdb->next            = NULL;
    threadListMutex.leave();

    if(OSLibGetPIB(PIB_TASKTYPE) == TASKTYPE_PM)
    {
        thdb->flags      = 0;  //todo gui
    }
    else thdb->flags      = 0;  //todo textmode

    //Initialize thread security objects (TODO: Not complete)
    if(hInstNTDll == 0) {
        hInstNTDll = LoadLibraryA("NTDLL.DLL");
        *(ULONG *)&RtlAllocateAndInitializeSid = (ULONG)GetProcAddress(hInstNTDll, "RtlAllocateAndInitializeSid");
        if(RtlAllocateAndInitializeSid == NULL) {
                DebugInt3();
        }
    }
    SID_IDENTIFIER_AUTHORITY sidIdAuth = {0};
    thdb->threadinfo.dwType = SECTYPE_PROCESS | SECTYPE_INITIALIZED;
    RtlAllocateAndInitializeSid(&sidIdAuth, 1, 0, 0, 0, 0, 0, 0, 0, 0, &thdb->threadinfo.SidUser.User.Sid);
    thdb->threadinfo.SidUser.User.Attributes = 0; //?????????

    thdb->threadinfo.pTokenGroups = (TOKEN_GROUPS*)malloc(sizeof(TOKEN_GROUPS));
    thdb->threadinfo.pTokenGroups->GroupCount = 1;
    RtlAllocateAndInitializeSid(&sidIdAuth, 1, 0, 0, 0, 0, 0, 0, 0, 0, &thdb->threadinfo.PrimaryGroup.PrimaryGroup);
    thdb->threadinfo.pTokenGroups->Groups[0].Sid = thdb->threadinfo.PrimaryGroup.PrimaryGroup;
    thdb->threadinfo.pTokenGroups->Groups[0].Attributes = 0; //????
//        pPrivilegeSet   = NULL;
//        pTokenPrivileges= NULL;
//        TokenOwner      = {0};
//        DefaultDACL     = {0};
//        TokenSource     = {0};
    thdb->threadinfo.TokenType = TokenPrimary;

    if(fMainThread)
    {
        //todo initialize PDB during process creation
        //todo: initialize TLS array if required
        //TLS in executable always TLS index 0?
        ProcessTIBSel = tibsel;
        ProcessPDB.exit_code       = 0x103; /* STILL_ACTIVE */
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

        GetSystemTime(&ProcessPDB.creationTime) ;

        /* Initialize the critical section */
        InitializeCriticalSection( &ProcessPDB.crit_section );
    }
    dprintf(("InitializeTIB setup TEB with selector %x", tibsel));
    dprintf(("InitializeTIB: FS(%x):[0] = %x", GetFS(), QueryExceptionChain()));
    return winteb;
}
//******************************************************************************
// Destroy the TIB selector and memory for the current thread
//******************************************************************************
void DestroyTIB()
{
 SHORT  orgtibsel;
 TEB   *winteb;
 THDB  *thdb;

    dprintf(("DestroyTIB: FS     = %x", GetFS()));
    dprintf(("DestroyTIB: FS:[0] = %x", QueryExceptionChain()));

    winteb = (TEB *)*TIBFlatPtr;
    if(winteb) {
        thdb = (THDB *)(winteb+1);
        orgtibsel = thdb->OrgTIBSel;

        threadListMutex.enter();
        THDB *curthdb        = threadList;
        if(curthdb == thdb) {
            threadList = thdb->next;
        }
        else {
            while(curthdb->next != thdb) {
                curthdb = curthdb->next;
                if(curthdb == NULL) {
                    dprintf(("DestroyTIB: couldn't find thdb %x", thdb));
                    DebugInt3();
                    break;
                }
            }
            if(curthdb) {
                curthdb->next = thdb->next;
            }
        }
        threadListMutex.leave();

        //Restore our original FS selector
        SetFS(orgtibsel);

        //And free our own
        OSLibFreeSel(thdb->teb_sel);

        *TIBFlatPtr = 0;
   }
   else dprintf(("Already destroyed TIB"));

   dprintf(("DestroyTIB: FS(%x):[0] = %x", GetFS(), QueryExceptionChain()));
   return;
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
 THDB  *thdb;

   //If we're running an Odin32 OS/2 application (not converted!), then we
   //we don't switch FS selectors
    if(fIsOS2Image) {
        return;
    }

    winteb = (TEB *)*TIBFlatPtr;
    if(winteb) {
        thdb = (THDB *)(winteb+1);
        orgtibsel = thdb->OrgTIBSel;

        //Restore our original FS selector
        SetFS(orgtibsel);
    }
}
/******************************************************************************/
/******************************************************************************/
USHORT WIN32API SetWin32TIB()
{
 SHORT  win32tibsel;
 TEB   *winteb;
 THDB  *thdb;

    //If we're running an Odin32 OS/2 application (not converted!), then we
    //we don't switch FS selectors
    if(fIsOS2Image) {
        return GetFS();
    }

    winteb = (TEB *)*TIBFlatPtr;
    if(winteb) {
        thdb = (THDB *)(winteb+1);
        win32tibsel = thdb->teb_sel;

        //Restore our win32 FS selector
        return SetReturnFS(win32tibsel);
    }
    else {
        //we didn't create this thread, so allocate a selector now
        //NOTE: Possible memory leak (i.e. DART threads in WINMM)
        winteb = InitializeTIB();
        if(winteb == NULL) {
            DebugInt3();
            return GetFS();
        }
        thdb = (THDB *)(winteb+1);
        win32tibsel = thdb->teb_sel;

        //Restore our win32 FS selector
        return SetReturnFS(win32tibsel);
    }
    // nested calls are OK, OS2ToWinCallback for instance
    //else DebugInt3();

    return GetFS();
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitProcess(DWORD exitcode)
{
    dprintf(("KERNEL32:  ExitProcess %d\n", exitcode));
    dprintf(("KERNEL32:  ExitProcess FS = %x\n", GetFS()));

    fExitProcess = TRUE;

    SetOS2ExceptionChain(-1);

    if(WinExe) {
        delete(WinExe);
        WinExe = NULL;
    }

    //Note: Needs to be done after deleting WinExe (destruction of exe + dll objects)
    //Flush and delete all open memory mapped files
    Win32MemMap::deleteAll();

    //Restore original OS/2 TIB selector
    DestroyTIB();
    SetExceptionChain((ULONG)-1);

    //avoid crashes since win32 & OS/2 exception handler aren't identical
    //(terminate process generates two exceptions)
    /* @@@PH 1998/02/12 Added Console Support */
    if (iConsoleIsActive())
        iConsoleWaitClose();

    O32_ExitProcess(exitcode);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FreeLibrary(HINSTANCE hinstance)
{
 Win32DllBase *winmod;
 BOOL rc;

    //SvL: Ignore FreeLibary for executable
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
    free(pszAsciiLibFile);

    return hDll;
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
 *                  partially implemented yet - imports are resolved it seems.
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
 *                  not implemented yet.
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
HINSTANCE WIN32API LoadLibraryExA(LPCTSTR lpszLibFile, HANDLE hFile, DWORD dwFlags)
{
    HINSTANCE       hDll;
    Win32DllBase *  pModule;
    char            szModname[CCHMAXPATH];
    BOOL            fPath;              /* Flags which is set if the    */
                                        /* lpszLibFile contains a path. */
    ULONG           fPE;                /* isPEImage return value. */

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
    if (WinExe != NULL && WinExe->matchModName(lpszLibFile))
        return WinExe->getInstanceHandle();
    pModule = Win32DllBase::findModule((LPSTR)lpszLibFile);
    if (pModule)
    {
        pModule->incDynamicLib();
        pModule->AddRef();
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): returns 0x%x. Dll found %s",
                 lpszLibFile, hFile, dwFlags, pModule->getInstanceHandle(), pModule->getFullPath()));
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
    fPath = strchr(lpszLibFile, '\\') || strchr(lpszLibFile, '/');
    strcpy(szModname, lpszLibFile);
    Win32DllBase::renameDll(szModname);
    strupr(szModname);

    if (!fPath)
    {
        char    szModName2[CCHMAXPATH];
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
    fPE = Win32ImageBase::isPEImage(szModname);

    /** @sketch
     *  IF dwFlags == 0 && (!fPeLoader || !fPE) THEN
     *      Try load the executable using LoadLibrary
     *      IF successfully loaded THEN
     *          IF LX dll and is using the PE Loader THEN
     *              Set Load library.
     *              Inc reference count.
     *          Endif
     *          Inc dynamic reference count.
     *          RETURN successfully.
     *      Endif
     *  Endif
     */
    //only call Open32 if dwFlags == 0 and (LX binary or win32k process)
    if(dwFlags == 0 && (!fPeLoader || fPE != ERROR_SUCCESS))
    {
        hDll = O32_LoadLibrary(szModname);
        if (hDll)
        {
            /* OS/2 dll, system dll, converted dll or win32k took care of it. */
            pModule = (Win32DllBase *)Win32LxDll::findModuleByOS2Handle(hDll);
            if(pModule)
            {

                if(pModule->isLxDll())
                {
                    ((Win32LxDll *)pModule)->setDllHandleOS2(hDll);
                    if (fPeLoader)
                        pModule->AddRef();
                }
                pModule->incDynamicLib();
            }
            else
                return hDll; //happens when LoadLibrary is called in kernel32's initterm (nor harmful)
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): returns 0x%x. Loaded %s using O32_LoadLibrary.",
                     lpszLibFile, hFile, dwFlags, hDll, szModname));
            return pModule->getInstanceHandle();
        }
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): O32_LoadLibrary(%s) failed. LastError=%d",
                 lpszLibFile, hFile, dwFlags, szModname, GetLastError()));
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
        Win32PeLdrDll * peldrDll;
        /* TODO!
         * We might use the fake loader class to do the LOAD_LIBRARY_AS_DATAFILE and
         * executable image loading. These are both loaded for accessing resource.
         * But this requires that they behaves like Dlls...
        if (dwFlags & LOAD_LIBRARY_AS_DATAFILE || fPE == 2)
        {
            peldrDll = new Win32PeLdrRsrcImg(szModname);
        }
        else
        */

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
        if (dwFlags & (DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE))
        {
            peldrDll->disableThreadLibraryCalls();
            //peldrDll->setDontProcessImports(); not implemented?
        }
        if (dwFlags & LOAD_WITH_ALTERED_SEARCH_PATH)
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): Warning dwFlags LOAD_WITH_ALTERED_SEARCH_PATH is not implemented.",
                      lpszLibFile, hFile, dwFlags));
            //peldrDll->setLoadWithAlteredSearchPath();
        }
        if (dwFlags & LOAD_LIBRARY_AS_DATAFILE)
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): Warning dwFlags LOAD_LIBRARY_AS_DATAFILE is not implemented.",
                     lpszLibFile, hFile, dwFlags));
            //peldrDll->setLoadAsDatafile();
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
        if (peldrDll->init(0))
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
            }
        }
        else
        {
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): Failed to init Win32PeLdrDll instance. error=%d returns NULL.",
                     lpszLibFile, hFile, dwFlags, peldrDll->getError()));
            SetLastError(ERROR_INVALID_EXE_SIGNATURE);
            delete peldrDll;
        }
    }
    else
    {
        dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x) library wasn't found (%s) or isn't loadable; err %x",
                 lpszLibFile, hFile, dwFlags, szModname, fPE));
        SetLastError(fPE);
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
HINSTANCE WIN32API LoadLibraryExW(LPCWSTR lpszLibFile, HANDLE hFile, DWORD dwFlags)
{
    char *      pszAsciiLibFile;
    HINSTANCE   hDll;

    pszAsciiLibFile = UnicodeToAsciiString(lpszLibFile);
    dprintf(("KERNEL32: LoadLibraryExW(%s, 0x%x, 0x%x) --> LoadLibraryExA",
             pszAsciiLibFile, hFile, dwFlags));
    hDll = LoadLibraryExA(pszAsciiLibFile, hFile, dwFlags);
    dprintf(("KERNEL32: LoadLibraryExW(%s, 0x%x, 0x%x) returns 0x%x",
             pszAsciiLibFile, hFile, dwFlags, hDll));
    free(pszAsciiLibFile);

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
        pszCmdLineA = psz = (PSZ)malloc(cch);
        if (psz == NULL)
        {
            dprintf(("KERNEL32: InitCommandLine(%p): malloc(%d) failed\n", pszPeExe, cch));
            return ERROR_NOT_ENOUGH_MEMORY;
        }

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
            while (*psz2 != '\0')
            {
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
        pszCmdLineW = (WCHAR*)malloc(cch * 2);
        if (pszCmdLineW != NULL)
            AsciiToUnicode(pszCmdLineA, (WCHAR*)pszCmdLineW);
        else
        {
            dprintf(("KERNEL32: InitCommandLine(%p): malloc(%d) failed (2)\n", pszPeExe, cch));
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

    dprintf(("KERNEL32:  GetCommandLineW: %s\n", pszCmdLineA));
    return pszCmdLineW;
}


/**
 * GetModuleFileName gets the full path and file name for the specified module.
 * @returns     Bytes written to the buffer (lpszPath). This count includes the
 *              terminating '\0'.
 *              On error 0 is returned. Last error is set.
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
 * @remark      - Do we still have to call Open32?
 *              - Do we set ERROR_BUFFER_OVERFLOW when cch > cchPath?
 *              - Does NT really set the last error?
 */
DWORD WIN32API GetModuleFileNameA(HMODULE hModule, LPTSTR lpszPath, DWORD cchPath)
{
    Win32ImageBase *    pMod;           /* Pointer to the module object. */
    DWORD               cch;            /* Length of the  */

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
            cch = strlen(pszFn) + 1;
            if (cch > cchPath)
                cch = cchPath;
            memcpy(lpszPath, pszFn, cch);
            lpszPath[cch - 1] = '\0';
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
        cch = O32_GetModuleFileName(hModule, lpszPath, cchPath);
        if (cch > 0)    cch++;          /* Open32 doesn't count the terminator. */
    }

    if (cch > 0)
        dprintf(("KERNEL32:  GetModuleFileNameA(%x,...): %s %d\n", hModule, lpszPath, cch));
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
//******************************************************************************
HANDLE WIN32API GetModuleHandleA(LPCTSTR lpszModule)
{
 HANDLE    hMod = 0;
 Win32DllBase *windll;
 char      szModule[CCHMAXPATH];
 BOOL      fDllModule = FALSE;

  if(lpszModule == NULL) {
    if(WinExe)
            hMod = WinExe->getInstanceHandle();
    else    hMod = -1;
  }
  else
  {
    strcpy(szModule, OSLibStripPath((char *)lpszModule));
    strupr(szModule);
    if(strstr(szModule, ".DLL")) {
        fDllModule = TRUE;
    }
    else {
        if(!strstr(szModule, ".")) {
            //if there's no extension or trainling dot, we
                        //assume it's a dll (see Win32 SDK docs)
            fDllModule = TRUE;
        }
    }
    char *dot = strstr(szModule, ".");
    if(dot)
        *dot = 0;

    if(!fDllModule && WinExe && !strcmpi(szModule, WinExe->getModuleName())) {
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
HMODULE WIN32API GetModuleHandleW(LPCWSTR arg1)
{
 HMODULE rc;
 char   *astring;

    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = GetModuleHandleA(astring);
    dprintf(("KERNEL32:  OS2GetModuleHandleW %s returned %X\n", astring, rc));
    FreeAsciiString(astring);
    return(rc);
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
 THDB *pThreadDB = (THDB*)GetThreadTHDB();
 char *cmdline = NULL;
 BOOL  rc;

    dprintf(("KERNEL32: CreateProcessA %s cline:%s inherit:%d cFlags:%x Env:%x CurDir:%s StartupFlags:%x\n",
            lpApplicationName, lpCommandLine, bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory, lpStartupInfo));

    // open32 does not support DEBUG_ONLY_THIS_PROCESS
    if(dwCreationFlags & DEBUG_ONLY_THIS_PROCESS)
      dwCreationFlags |= DEBUG_PROCESS;

    if(O32_CreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes,
                         lpThreadAttributes, bInheritHandles, dwCreationFlags,
                         lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                         lpProcessInfo) == TRUE)
    {
      if (dwCreationFlags & DEBUG_PROCESS && pThreadDB != NULL)
      {
        if(pThreadDB->pidDebuggee != 0)
        {
          // TODO: handle this
          dprintf(("KERNEL32: CreateProcess ERROR: This thread is already a debugger\n"));
        }
        else
        {
          pThreadDB->pidDebuggee = lpProcessInfo->dwProcessId;
          OSLibStartDebugger((ULONG*)&pThreadDB->pidDebuggee);
        }
      }
      else pThreadDB->pidDebuggee = 0;

      return(TRUE);
    }
    //probably a win32 exe, so run it in the pe loader
    if(lpApplicationName) {
        if(lpCommandLine) {
            //skip exe name in lpCommandLine
            while(*lpCommandLine != 0 && *lpCommandLine != ' ')
                lpCommandLine++;

            if(*lpCommandLine != 0) {
                lpCommandLine++;
            }
            cmdline = (char *)malloc(strlen(lpApplicationName)+strlen(lpCommandLine) + 16);
            sprintf(cmdline, "PE.EXE %s %s", lpApplicationName, lpCommandLine);
         }
         else {
            cmdline = (char *)malloc(strlen(lpApplicationName) + 16);
            sprintf(cmdline, "PE.EXE %s", lpApplicationName);
        }
    }
    else {
        cmdline = (char *)malloc(strlen(lpCommandLine) + 16);
        sprintf(cmdline, "PE.EXE %s", lpCommandLine);
    }
    dprintf(("KERNEL32:  CreateProcess %s\n", cmdline));
    rc = O32_CreateProcess("PE.EXE", (LPCSTR)cmdline,lpProcessAttributes,
                         lpThreadAttributes, bInheritHandles, dwCreationFlags,
                         lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                         lpProcessInfo);
    if(rc == TRUE) {
      if (dwCreationFlags & DEBUG_PROCESS && pThreadDB != NULL)
      {
        if(pThreadDB->pidDebuggee != 0)
        {
          // TODO: handle this
          dprintf(("KERNEL32: CreateProcess ERROR: This thread is already a debugger\n"));
        }
        else
        {
          pThreadDB->pidDebuggee = lpProcessInfo->dwProcessId;
          OSLibStartDebugger((ULONG*)&pThreadDB->pidDebuggee);
        }
      }
      else
        pThreadDB->pidDebuggee = 0;
    }
    if(cmdline)
        free(cmdline);

    if(lpProcessInfo)
      dprintf(("KERNEL32:  CreateProcess returned %d hPro:%x hThr:%x pid:%x tid:%x\n",
               rc, lpProcessInfo->hProcess, lpProcessInfo->hThread,
                   lpProcessInfo->dwProcessId,lpProcessInfo->dwThreadId));
    else
      dprintf(("KERNEL32:  CreateProcess returned %d\n", rc));
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

    dprintf(("KERNEL32: WinExec %s\n", lpCmdLine));
    startinfo.dwFlags = nCmdShow;
    if(CreateProcessA(NULL, (LPSTR)lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL,
                      &startinfo, &procinfo) == FALSE)
    {
        return 0;
    }
    //block until the launched app waits for input (or a timeout of 15 seconds)
    //TODO: Shouldn't call Open32, but the api in user32..
    rc = O32_WaitForInputIdle(procinfo.hProcess, 15000);
    if(rc != 0) {
    	dprintf(("WinExec: WaitForInputIdle %x returned %x", procinfo.hProcess, rc));
    }
    return procinfo.hProcess; //correct?
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
        if ( O32_WaitForInputIdle ( info.hProcess, 15000 ) ==  0xFFFFFFFF ) 
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
 FARPROC   proc;
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
        else    proc = (FARPROC)winmod->getApi((char *)lpszProc);
        if(proc == 0) {
#ifdef DEBUG
                if(ulAPIOrdinal <= 0x0000FFFF) {
                        dprintf(("GetProcAddress %x %x not found!", hModule, ulAPIOrdinal));
                }
                else    dprintf(("GetProcAddress %x %s not found!", hModule, lpszProc));
#endif
                SetLastError(ERROR_PROC_NOT_FOUND);
        }
        if(HIWORD(lpszProc))
                dprintf(("KERNEL32:  GetProcAddress %s from %X returned %X\n", lpszProc, hModule, proc));
        else    dprintf(("KERNEL32:  GetProcAddress %x from %X returned %X\n", lpszProc, hModule, proc));
        return proc;
  }
  proc = O32_GetProcAddress(hModule, lpszProc);
  if(HIWORD(lpszProc))
        dprintf(("KERNEL32:  GetProcAddress %s from %X returned %X\n", lpszProc, hModule, proc));
  else  dprintf(("KERNEL32:  GetProcAddress %x from %X returned %X\n", lpszProc, hModule, proc));
  return(proc);
}
//******************************************************************************
//Retrieve the version
//******************************************************************************
BOOL SYSTEM GetVersionStruct(char *lpszModName, char *verstruct, ULONG bufLength)
{
 Win32ImageBase *winimage;
 Win32PeLdrRsrcImg *rsrcimg;

  dprintf(("GetVersionStruct of module %s", lpszModName));
  if(WinExe && !stricmp(WinExe->getFullPath(), lpszModName)) {
        winimage = (Win32ImageBase *)WinExe;
  }
  else {
        winimage = (Win32ImageBase *)Win32DllBase::findModule(lpszModName);
        if(winimage == NULL)
        {
        char modname[CCHMAXPATH];

        strcpy(modname, lpszModName);
        //rename dll if necessary (i.e. OLE32 -> OLE32OS2)
        Win32DllBase::renameDll(modname);

        if(Win32ImageBase::isPEImage(modname) != ERROR_SUCCESS)
        {
         HINSTANCE hInstance;

            //must be an LX dll, just load it (app will probably load it anyway)
            hInstance = LoadLibraryA(modname);
            if(hInstance == 0)
                return 0;
                winimage = (Win32ImageBase *)Win32DllBase::findModule(hInstance);
                if(winimage) {
                return winimage->getVersionStruct(verstruct, bufLength);
            }
            return 0;
        }
        //SvL: Try to load it
        rsrcimg = new Win32PeLdrRsrcImg(modname);
        if(rsrcimg == NULL)
            return 0;

            rsrcimg->init(0);
        if(rsrcimg->getError() != NO_ERROR)
        {
                dprintf(("GetVersionStruct can't load %s\n", modname));
            delete rsrcimg;
                return(FALSE);
        }
        BOOL rc = rsrcimg->getVersionStruct(verstruct, bufLength);
        delete rsrcimg;
        return rc;
        }
  }
  return winimage->getVersionStruct(verstruct, bufLength);
}
//******************************************************************************
//******************************************************************************
ULONG SYSTEM GetVersionSize(char *lpszModName)
{
 Win32ImageBase *winimage;
 Win32PeLdrRsrcImg *rsrcimg;

  dprintf(("GetVersionSize of %s\n", lpszModName));

  if(WinExe && !stricmp(WinExe->getFullPath(), lpszModName)) {
        winimage = (Win32ImageBase *)WinExe;
  }
  else {
        winimage = (Win32ImageBase *)Win32DllBase::findModule(lpszModName);
        if(winimage == NULL)
        {
     char modname[CCHMAXPATH];

        strcpy(modname, lpszModName);
        //rename dll if necessary (i.e. OLE32 -> OLE32OS2)
        Win32DllBase::renameDll(modname);

        if(Win32ImageBase::isPEImage(modname) != ERROR_SUCCESS)
        {
         HINSTANCE hInstance;

            //must be an LX dll, just load it (app will probably load it anyway)
            hInstance = LoadLibraryA(modname);
            if(hInstance == 0)
                return 0;
                winimage = (Win32ImageBase *)Win32DllBase::findModule(hInstance);
                if(winimage) {
                return winimage->getVersionSize();
            }
            return 0;
        }

        //SvL: Try to load it
        rsrcimg = new Win32PeLdrRsrcImg(modname);
        if(rsrcimg == NULL)
            return 0;

            rsrcimg->init(0);
        if(rsrcimg->getError() != NO_ERROR)
        {
                dprintf(("GetVersionSize can't load %s\n", modname));
            delete rsrcimg;
                return(FALSE);
        }
        int size = rsrcimg->getVersionSize();
        delete rsrcimg;
        return size;
        }
  }
  return winimage->getVersionSize();
}
//******************************************************************************
//TODO:What does this do exactly??
//******************************************************************************
ODINFUNCTION1(BOOL,DisableThreadLibraryCalls,HMODULE,hModule)
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
//******************************************************************************
