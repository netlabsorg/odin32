/* $Id: wprocess.cpp,v 1.85 2000-08-09 18:59:03 sandervl Exp $ */

/*
 * Win32 process functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <vmutex.h>
#include <handlemanager.h>

#ifdef __IBMCPP__
#include <builtin.h>
#endif

#include "odin32validate.h"
#include "exceptutil.h"
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


//******************************************************************************
//******************************************************************************
BOOL      fFreeLibrary = FALSE;
BOOL      fIsOS2Image = FALSE;  //TRUE  -> Odin32 OS/2 application (not converted!)
                            //FALSE -> otherwise
//Process database
PDB       ProcessPDB = {0};
USHORT    ProcessTIBSel = 0;
DWORD    *TIBFlatPtr    = 0;

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
    dprintf(("KERNEL32: FreeLibrary %s %X\n", OSLibGetDllName(hinstance), hinstance));

    //TODO: Not thread safe
    fFreeLibrary  = TRUE; //ditch dll
    rc = O32_FreeLibrary(hinstance);
    fFreeLibrary = FALSE;
    dprintf(("FreeLibrary returned %X\n", rc));
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
     *  First we'll see if the module is allready loaded.
     *  IF allready loaded THEN
     *      IF it's a LX dll which isn't loaded and we're using the PeLoader THEN
     *          Set Load library.
     *      Endif
     *      Inc dynamic reference count.
     *      Inc reference count.
     *      RETURN instance handle.
     *  Endif
     */
    pModule = Win32DllBase::findModule((LPSTR)lpszLibFile);
    if (pModule)
    {
        if (pModule->isLxDll() && !pModule->isLoaded() && fPeLoader)
        {
            //can happen with i.e. wininet
            //wininet depends on wsock32; when the app loads wsock32 afterwards
            //with LoadLibrary or as a child of another dll, we need to make
            //sure it's loaded once with DosLoadModule
            pModule->setLoadLibrary();
        }
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


    /** @sketch
     *  IF dwFlags == 0 THEN
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
    if (dwFlags == 0)
    {
        hDll = O32_LoadLibrary(szModname);
        if (hDll)
        {
            /* OS/2 dll, system dll, converted dll or win32k took care of it.*/
            pModule = Win32DllBase::findModule(hDll);
            if (pModule)
            {
                if (pModule->isLxDll() && fPeLoader)
                {
                    pModule->setLoadLibrary();
                    pModule->AddRef();
                }
                pModule->incDynamicLib();
            }
            dprintf(("KERNEL32: LoadLibraryExA(%s, 0x%x, 0x%x): returns 0x%x. Loaded %s using O32_LoadLibrary.",
                     lpszLibFile, hFile, dwFlags, hDll, szModname));
            return hDll;
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
    fPE = Win32ImageBase::isPEImage(szModname);
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
            if (peldrDll->attachProcess()) {
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
//******************************************************************************
//******************************************************************************
LPCSTR WIN32API GetCommandLineA()
{
 LPTSTR cmdline = NULL;

  if(WinExe) {
    cmdline = WinExe->getCommandLineA();
  }
  if(cmdline == NULL) //not used for converted exes
    cmdline = O32_GetCommandLine();

  dprintf(("KERNEL32:  GetCommandLine %s\n", cmdline));
  dprintf(("KERNEL32:  FS = %x\n", GetFS()));
  return(cmdline);
}
//******************************************************************************
//******************************************************************************
LPCWSTR WIN32API GetCommandLineW(void)
{
 static WCHAR *UnicodeCmdLine = NULL;
         char *asciicmdline = NULL;

    dprintf(("KERNEL32:  FS = %x\n", GetFS()));

    if(UnicodeCmdLine)
        return(UnicodeCmdLine); //already called before

    if(WinExe) {
        if(WinExe->getCommandLineW())
        return WinExe->getCommandLineW();
    }
    if(asciicmdline == NULL) //not used for converted exes
      asciicmdline = O32_GetCommandLine();

    if(asciicmdline) {
        UnicodeCmdLine = (WCHAR *)malloc(strlen(asciicmdline)*2 + 2);
        AsciiToUnicode(asciicmdline, UnicodeCmdLine);
        dprintf(("KERNEL32:  OS2GetCommandLineW: %s\n", asciicmdline));
        return(UnicodeCmdLine);
    }
    dprintf(("KERNEL32:  OS2GetCommandLineW: asciicmdline == NULL\n"));
    return NULL;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetModuleFileNameA(HMODULE hinstModule, LPTSTR lpszPath, DWORD cchPath)
{
 DWORD rc;
 Win32ImageBase *module;
 char *fpath = NULL;

  dprintf(("GetModuleFileName %X", hinstModule));
  if(hinstModule == 0 || hinstModule == -1 || (WinExe && hinstModule == WinExe->getInstanceHandle())) {
        module = (Win32ImageBase *)WinExe;
  }
  else {
        module = (Win32ImageBase *)Win32DllBase::findModule(hinstModule);
  }

  if(module) {
        fpath = module->getFullPath();
  }
  if(fpath) {
        //SvL: 13-9-98: +1
        rc = min(strlen(fpath)+1, cchPath);
        strncpy(lpszPath, fpath, rc);
	lpszPath[rc-1] = 0;
  }
  else  rc = O32_GetModuleFileName(hinstModule, lpszPath, cchPath);

  if(rc) {
    dprintf(("KERNEL32: GetModuleFileName %s %d\n", lpszPath, hinstModule));
  }
  return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetModuleFileNameW(HMODULE hModule, LPWSTR lpFileName, DWORD nSize)
{
 char *asciifilename = (char *)malloc(nSize+1);
 DWORD rc;

    dprintf(("KERNEL32:  OSLibGetModuleFileNameW\n"));
    rc = GetModuleFileNameA(hModule, asciifilename, nSize);
    if(rc)      AsciiToUnicode(asciifilename, lpFileName);
    free(asciifilename);
    return(rc);
}
//******************************************************************************
//NOTE: GetModuleHandleA does NOT support files with multiple dots (i.e.
//      very.weird.exe)
//******************************************************************************
HANDLE WIN32API GetModuleHandleA(LPCTSTR lpszModule)
{
 HANDLE    hMod;
 Win32DllBase *windll;
 char      szModule[CCHMAXPATH];
 BOOL      fDllModule = FALSE;

  if(lpszModule == NULL) {
    if(WinExe)
        hMod = WinExe->getInstanceHandle();
    else    hMod = -1;
  }
  else {
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
        else    hMod = OSLibiGetModuleHandleA((char *)lpszModule);
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
		else	dprintf(("GetProcAddress %x %s not found!", hModule, lpszProc));
#endif
        	SetLastError(ERROR_PROC_NOT_FOUND);
    	}
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
