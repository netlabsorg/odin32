/* $Id: wprocess.cpp,v 1.64 2000-01-02 22:09:02 sandervl Exp $ */

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
#include <windllbase.h>
#include <winexebase.h>
#include <windllpeldr.h>
#include <winfakepeldr.h>
#include <vmutex.h>

#ifdef __IBMCPP__
#include <builtin.h>
#endif

#include "exceptutil.h"
#include "oslibmisc.h"
#include "oslibdebug.h"

#include "console.h"
#include "cio.h"
#include "versionos2.h"    /*PLF Wed  98-03-18 02:36:51*/
#include <wprocess.h>
#include "mmap.h"


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
//******************************************************************************
//******************************************************************************
TEB *WIN32API GetThreadTEB()
{
  if(TIBFlatPtr == NULL)
    return 0;

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
// Set up the TIB selector and memory for the current thread
//******************************************************************************
TEB *InitializeTIB(BOOL fMainThread)
{
  TEB   *winteb;
  THDB  *thdb;

  USHORT tibsel;

   //Allocate one dword to store the flat address of our TEB
   if(fMainThread) {
    TIBFlatPtr = (DWORD *)OSLibAllocThreadLocalMemory(1);
    if(TIBFlatPtr == 0) {
        dprintf(("InitializeTIB: local thread memory alloc failed!!"));
        DebugInt3();
        return NULL;
    }
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

    	GetSystemTime(&ProcessPDB.creationTime);

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
void _System Win32DllExitList(ULONG reason)
{
  dprintf(("Win32DllExitList %d\n", reason));

  if(WinExe) {
    delete(WinExe);
    WinExe = NULL;
  }
  return;
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitProcess(DWORD exitcode)
{
  dprintf(("KERNEL32:  ExitProcess %d\n", exitcode));
  dprintf(("KERNEL32:  ExitProcess FS = %x\n", GetFS()));

  SetOS2ExceptionChain(-1);

  Win32DllExitList(0);

  //Note: Needs to be done after Win32DllExitList (destruction of exe + dll objects)
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

  dprintf(("FreeLibrary"));
  winmod = Win32DllBase::findModule(hinstance);
  if(winmod) {
        winmod->Release();
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
static HINSTANCE iLoadLibraryA(LPCTSTR lpszLibFile, DWORD dwFlags)
{
 char        modname[CCHMAXPATH];
 HINSTANCE   hDll;
 Win32DllBase *module;

  module = Win32DllBase::findModule((LPSTR)lpszLibFile);
  if(module) {
        module->AddRef();
    dprintf(("iLoadLibrary: found %s -> handle %x", lpszLibFile, module->getInstanceHandle()));
        return module->getInstanceHandle();
  }

  strcpy(modname, lpszLibFile);
  strupr(modname);
  //rename dll if necessary (i.e. OLE32 -> OLE32OS2)
  Win32DllBase::renameDll(modname);

  hDll = O32_LoadLibrary(modname);
  dprintf(("KERNEL32:  iLoadLibraryA %s returned %X (%d)\n",
           lpszLibFile,
           hDll,
           GetLastError()));
  if(hDll)
  {
    return hDll;    //converted dll or win32k took care of it
  }

  if(!strstr(modname, ".")) {
    strcat(modname,".DLL");
  }

  if(Win32ImageBase::isPEImage((char *)modname))
  {
        module = Win32DllBase::findModule((char *)modname);
        if(module) {//don't load it again
            module->AddRef();
            return module->getInstanceHandle();
        }

        Win32PeLdrDll *peldrDll = new Win32PeLdrDll((char *)modname);
        if(peldrDll == NULL)
            return(0);

        peldrDll->init(0);
        if(peldrDll->getError() != NO_ERROR) {
            dprintf(("LoadLibary %s failed (::init)\n", lpszLibFile));
            delete(peldrDll);
            return(0);
        }
        if(dwFlags & DONT_RESOLVE_DLL_REFERENCES) {
            peldrDll->setNoEntryCalls();
        }

        if(peldrDll->attachProcess() == FALSE) {
            dprintf(("LoadLibary %s failed (::attachProcess)\n", lpszLibFile));
            delete(peldrDll);
            return(0);
        }
        peldrDll->AddRef();
        return peldrDll->getInstanceHandle();
  }
  else  return(0);
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
HINSTANCE WIN32API LoadLibraryA(LPCTSTR lpszLibFile)
{
  HINSTANCE hDll;

  dprintf(("KERNEL32:  LoadLibraryA(%s)\n",
           lpszLibFile));
  dprintf(("KERNEL32: LoadLibrary %x FS = %x\n", GetCurrentThreadId(), GetFS()));

  hDll = iLoadLibraryA(lpszLibFile, 0);
  if (hDll == 0)
  {
    char * pszName;

    // remove path from the image name
    pszName = strrchr((char *)lpszLibFile,
                      '\\');
    if (pszName != NULL)
    {
      pszName++;                // skip backslash

      // now try again without fully qualified path
      hDll = iLoadLibraryA(pszName, 0);
    }
  }

  return hDll;
}
//******************************************************************************
//******************************************************************************
HINSTANCE WIN32API LoadLibraryExA(LPCTSTR lpszLibFile, HANDLE hFile, DWORD dwFlags)
{
 HINSTANCE     hDll;

  dprintf(("KERNEL32:  LoadLibraryExA %s (%X)\n", lpszLibFile, dwFlags));
  hDll = iLoadLibraryA(lpszLibFile, dwFlags);
  if (hDll == 0)
  {
    char * pszName;

    // remove path from the image name
    pszName = strrchr((char *)lpszLibFile,
                      '\\');
    if (pszName != NULL)
    {
      pszName++;                // skip backslash

      // now try again without fully qualified path
      hDll = iLoadLibraryA(pszName, dwFlags);
    }
  }

  return hDll;
}
//******************************************************************************
//******************************************************************************
HINSTANCE WIN32API LoadLibraryW(LPCWSTR lpModule)
{
 char     *asciimodule;
 HINSTANCE rc;

    asciimodule = UnicodeToAsciiString((LPWSTR)lpModule);
    dprintf(("KERNEL32:  OS2LoadLibraryW %s\n", asciimodule));
    rc = LoadLibraryA(asciimodule);
    free(asciimodule);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HINSTANCE WIN32API LoadLibraryExW(LPCWSTR lpModule, HANDLE hFile, DWORD dwFlags)
{
 char     *asciimodule;
 HINSTANCE rc;

    asciimodule = UnicodeToAsciiString((LPWSTR)lpModule);
    dprintf(("KERNEL32:  OS2LoadLibraryExW %s (%d)\n", asciimodule, dwFlags));
    rc = LoadLibraryExA(asciimodule, hFile, dwFlags);
    free(asciimodule);
    return(rc);
}
//******************************************************************************
//******************************************************************************
LPCSTR WIN32API GetCommandLineA()
{
 LPTSTR cmdline = NULL;

  if(WinExe) {
    cmdline = WinExe->getCommandLine();
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
      asciicmdline = WinExe->getCommandLine();
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
    if(astring3)	FreeAsciiString(astring3);
    if(astring2)	FreeAsciiString(astring2);
    if(astring1)	FreeAsciiString(astring1);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HINSTANCE WIN32API WinExec(LPCSTR lpCmdLine, UINT nCmdShow)
{
 STARTUPINFOA        startinfo = {0};
 PROCESS_INFORMATION procinfo;

    dprintf(("KERNEL32: WinExec %s\n", lpCmdLine));
    startinfo.dwFlags = nCmdShow;
    if(CreateProcessA(NULL, (LPSTR)lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL,
                      &startinfo, &procinfo) == FALSE)
    {
    return 0;
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

        if(Win32ImageBase::isPEImage(modname) == FALSE)
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

        if(Win32ImageBase::isPEImage(modname) == FALSE)
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
    winmod->setThreadLibraryCalls(FALSE);
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
