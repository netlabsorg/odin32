/* $Id: wprocess.cpp,v 1.6 1999-06-17 18:21:43 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 process functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "windll.h"
#include "winexe.h"

#ifdef __IBMCPP__
#include <builtin.h>
#endif

#include "except.h"
#include "os2util.h"
#include "console.h"
#include "cio.h"
#include "versionos2.h"    /*PLF Wed  98-03-18 02:36:51*/

BOOL      fExeStarted = FALSE;
BOOL      fFreeLibrary = FALSE;

/******************************************************************************/
//SvL: 4-10-'98: Put in separate procedure, as ICC changes FS:0 when there
//               are new or delete calls present.
//******************************************************************************
void RegisterExe(LONG Win32TableId, LONG NameTableId, LONG VersionResId,
                 LONG Pe2lxVersion, HINSTANCE hinstance)
{
  if(WinExe != NULL) //should never happen
    delete(WinExe);

  //SvL: Use 0 instead of the real instance handle (for resource lookup)
  Win32Exe *winexe = new Win32Exe(0, NameTableId, Win32TableId);
  if(winexe) {
    winexe->setVersionId(VersionResId);
    winexe->setOS2InstanceHandle(hinstance);
  }
  else {
    eprintf(("Win32Exe creation failed!\n"));
    DebugInt3();
  }

  char *modname = getenv("WIN32MODULE");

  if(modname != NULL)
  {
    dprintf(("Set full path for exe to %s", modname));
    winexe->setFullPath(modname);
  }

  fExeStarted  = TRUE;
}
//******************************************************************************
//******************************************************************************
VOID WIN32API RegisterResourceUsage(LONG Win32TableId, LONG NameTableId,
                    LONG VersionResId, LONG Pe2lxVersion,
                    HINSTANCE hinstance)
{
  if(getenv("WIN32_IOPL2")) {
    io_init1();
  }
  dprintf(("RegisterResourceUsage %X resid = %d\n", hinstance, VersionResId));

  CheckVersion(Pe2lxVersion, OS2GetDllName(hinstance));

  RegisterExe(Win32TableId, NameTableId, VersionResId, Pe2lxVersion, hinstance);

  //NOTE: Breaks Quake 2 if enabled
  if(getenv("WIN32_NOEXCEPTION")) {
    ChangeTIBStack();   //disable exceptions
  }
}
//******************************************************************************
//******************************************************************************
void CreateDll(LONG Win32TableId, LONG NameTableId, LONG VersionResId,
               HINSTANCE hinstance, WIN32DLLENTRY pfnDllEntry)
{
  Win32Dll *winmod = Win32Dll::findModule(hinstance);

  if(winmod != NULL) {
    //dll manually loaded by PE loader (Win32Dll::init)
    winmod->OS2DllInit(hinstance, NameTableId, Win32TableId, pfnDllEntry);
    return;
  }

  //converted win32 dll loaded by OS/2 loader
  winmod = new Win32Dll(hinstance, NameTableId, Win32TableId, pfnDllEntry);
  if(winmod == NULL) {
    eprintf(("Failed to allocate module object!\n"));
    DebugInt3();
    return;
  }
  //SvL: 19-8-'98
  winmod->AddRef();
  winmod->setVersionId(VersionResId);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API RegisterDll(LONG Win32TableId, LONG NameTableId,
                  LONG VersionResId, LONG Pe2lxVersion,
              HINSTANCE hinstance)
{
 WIN32DLLENTRY  pfnDllEntry;
 char *name;

  pfnDllEntry = (WIN32DLLENTRY)GetDllEntryPoint();  //== return address

  if(getenv("WIN32_IOPL2")) {
    io_init1();
  }
  name = OS2GetDllName(hinstance);
  CheckVersion(Pe2lxVersion, name);

  dprintf(("RegisterDll %X %s\n", hinstance, name));

  CreateDll(Win32TableId, NameTableId, VersionResId, hinstance, pfnDllEntry);

  /* @@@PH 1998/03/17 console devices initialization */
  iConsoleDevicesRegister();
}
//******************************************************************************
//******************************************************************************
void _System Win32DllExitList(ULONG reason)
{
  dprintf(("Win32DllExitList %d\n", reason));

  if(WinExe) {
    delete WinExe;
    WinExe = NULL;
  }
  return;
}
//******************************************************************************
//Called when a dll is detached (either at process exit or when FreeLibrary is called)
//******************************************************************************
BOOL WIN32API DLLExitList(HINSTANCE hInstance)
{
//  dprintf(("DLLExitList"));
  Win32Dll *winmod = Win32Dll::findModule(hInstance);

  if(winmod == NULL) {//probably called after we manually unloaded it in ExitProcess
    return(1);  //remove it from memory
  }
  dprintf(("DllExitList for %s (%X)\n", OS2GetDllName(winmod->getInstanceHandle()), winmod->getInstanceHandle()));
  delete(winmod);

  if(fFreeLibrary) {
    dprintf(("KERNEL32: DLLExitList Ditched by FreeLibrary\n"));
    return(1);  //remove it as we no longer need it
  }
  return(0);    //don't remove it (OS/2 can unload them at process exit in the wrong order!)
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitProcess(DWORD exitcode)
{
  dprintf(("KERNEL32:  ExitProcess %d\n", exitcode));

  //avoid crashes since win32 & OS/2 exception handler aren't identical
  //(terminate process generates two exceptions)
  /* @@@PH 1998/02/12 Added Console Support */
  if (iConsoleIsActive())
    iConsoleWaitClose();

  try {
     Win32DllExitList(-1);
  }
  catch(...) {
    dprintf(("dll exitlist exception\n"));
  }
  SetExceptionChain((ULONG)-1);
  O32_ExitProcess(exitcode);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FreeLibrary(HINSTANCE hinstance)
{
 Win32Dll *winmod;
 BOOL rc;

  dprintf(("FreeLibrary"));
  winmod = Win32Dll::findModule(hinstance);
  if(winmod) {
    winmod->Release();
    return(TRUE);
  }
  dprintf(("KERNEL32: FreeLibrary %s %X\n", OS2GetDllName(hinstance), hinstance));

  //TODO: Not thread safe
  fFreeLibrary  = TRUE; //ditch dll
  rc = O32_FreeLibrary(hinstance);
  fFreeLibrary = FALSE;
  dprintf(("FreeLibrary returned %X\n", rc));
  return(TRUE);
}
/******************************************************************************/
/******************************************************************************/
static HINSTANCE iLoadLibraryA(LPCTSTR lpszLibFile)
{
 HINSTANCE   hDll;
 Win32Dll   *module;

  hDll = O32_LoadLibrary(lpszLibFile);
  dprintf(("KERNEL32:  iLoadLibraryA %s returned %X (%d)\n",
           lpszLibFile,
           hDll,
           GetLastError()));
  if(hDll)
  {
    return hDll;    //converted dll or win32k took care of it
  }

  if(Win32Image::isPEImage((char *)lpszLibFile)) {
    module = Win32Dll::findModule((char *)lpszLibFile);
    if(module) {//don't load it again
        module->AddRef();
        return module->getInstanceHandle();
    }
    module = new Win32Dll((char *)lpszLibFile);
    if(module == NULL)
        return(0);

    module->init();
    if(module->getError() != NO_ERROR) {
        dprintf(("LoadLibary %s failed (::init)\n", lpszLibFile));
        delete module;
        return(0);
    }
    if(module->attachProcess() == FALSE) {
        dprintf(("LoadLibary %s failed (::attachProcess)\n", lpszLibFile));
        delete module;
        return(0);
    }
    module->AddRef();
    return module->getInstanceHandle();
  }
  else
    return(0);
}


HINSTANCE WIN32API LoadLibraryA(LPCTSTR lpszLibFile)
{
  HINSTANCE hDll;

  dprintf(("KERNEL32:  LoadLibraryA(%s)\n",
           lpszLibFile));

  hDll = iLoadLibraryA(lpszLibFile);
  if (hDll == 0)
  {
    PSZ pszName;

    // remove path from the image name
    pszName = strrchr((PSZ)lpszLibFile,
                      '\\');
    if (pszName != NULL)
    {
      pszName++;                // skip backslash

      // now try again without fully qualified path
      hDll = iLoadLibraryA(pszName);
    }
  }

  return hDll;
}


//******************************************************************************
//******************************************************************************
HINSTANCE WIN32API LoadLibraryExA(LPCTSTR lpszLibFile, HANDLE hFile, DWORD dwFlags)
{
 Win32Dll   *module;
 HINSTANCE   hDll;

  dprintf(("KERNEL32:  LoadLibraryExA %s (%X)\n", lpszLibFile, dwFlags));
  hDll = O32_LoadLibrary(lpszLibFile);
  if(hDll) {
    return hDll;    //converted dll or win32k took care of it
  }

  if(Win32Image::isPEImage((char *)lpszLibFile)) {
    module = Win32Dll::findModule((char *)lpszLibFile);
    if(module) {//don't load it again
        module->AddRef();
        return module->getInstanceHandle();
    }
    module = new Win32Dll((char *)lpszLibFile);
    if(module == NULL)
        return(0);

    module->init();
    if(module->getError() != NO_ERROR) {
        dprintf(("LoadLibary %s failed (::init)\n", lpszLibFile));
        delete module;
        return(0);
    }
    if(dwFlags & DONT_RESOLVE_DLL_REFERENCES) {
        module->setNoEntryCalls();
    }
    if(module->attachProcess() == FALSE) {
        dprintf(("LoadLibary %s failed (::attachProcess)\n", lpszLibFile));
        delete module;
        return(0);
    }
    module->AddRef();
    return module->getInstanceHandle();
  }
  return(0);
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
  //SvL: Replace original startup code exception handler
  ReplaceExceptionHandler();
  return(cmdline);
}
//******************************************************************************
//******************************************************************************
LPCWSTR WIN32API GetCommandLineW(void)
{
 static WCHAR *UnicodeCmdLine = NULL;
         char *asciicmdline = NULL;

    //SvL: Replace original startup code exception handler
    ReplaceExceptionHandler();

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
 Win32Image *module;
 char *fpath = NULL;

  dprintf(("GetModuleFileName %X", hinstModule));
  if(hinstModule == 0 || hinstModule == -1 || (WinExe && hinstModule == WinExe->getOS2InstanceHandle())) {
    module = (Win32Image *)WinExe;
  }
  else {
    module = (Win32Image *)Win32Dll::findModule(hinstModule);
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

    dprintf(("KERNEL32:  OS2GetModuleFileNameW\n"));
    rc = GetModuleFileNameA(hModule, asciifilename, nSize);
    if(rc)      AsciiToUnicode(asciifilename, lpFileName);
    free(asciifilename);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CreateProcessA(LPCSTR lpszImageName, LPSTR lpszCommandLine,
              		     PSECURITY_ATTRIBUTES arg3,
                             PSECURITY_ATTRIBUTES arg4, BOOL arg5, DWORD arg6,
                             PVOID arg7, LPCSTR arg8, LPSTARTUPINFOA arg9,
                             LPPROCESS_INFORMATION arg10)
{
 BOOL  rc;
 char *cmdline;
 BOOL  fAllocStr = FALSE;

    if(O32_CreateProcess(lpszImageName, lpszCommandLine, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) == TRUE)
    return(TRUE);

    //probably a win32 exe, so run it in the pe loader
    if(lpszImageName) {
     if(lpszCommandLine) {
        cmdline = (char *)malloc(strlen(lpszImageName)+strlen(lpszCommandLine) + 16);
        sprintf(cmdline, "%s %s", lpszImageName, lpszCommandLine);
        fAllocStr = TRUE;
     }
     else   cmdline = (char *)lpszImageName;
    }
    else cmdline = (char *)lpszCommandLine;

    dprintf(("KERNEL32:  CreateProcess %s\n", cmdline));
    rc = O32_CreateProcess("PE.EXE", (LPCSTR)cmdline, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    if(fAllocStr)
    free(cmdline);

    dprintf(("KERNEL32:  CreateProcess returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CreateProcessW(LPCWSTR arg1, LPWSTR arg2,
                             PSECURITY_ATTRIBUTES arg3,
                             PSECURITY_ATTRIBUTES arg4,
                             BOOL arg5, DWORD arg6, PVOID arg7,
                             LPCWSTR arg8, LPSTARTUPINFOW arg9,
                             LPPROCESS_INFORMATION  arg10)
{
 BOOL rc;
 char *astring1, *astring2, *astring3;

    dprintf(("KERNEL32:  OS2CreateProcessW DOESN't WORK"));
    astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    astring2 = UnicodeToAsciiString(arg2);
    astring3 = UnicodeToAsciiString((LPWSTR)arg8);
    // NOTE: This will not work as is (needs UNICODE support)
    rc = CreateProcessA(astring1, astring2, arg3, arg4, arg5, arg6, arg7,
                           astring3, (LPSTARTUPINFOA)arg9, arg10);
    FreeAsciiString(astring3);
    FreeAsciiString(astring2);
    FreeAsciiString(astring1);
    return(rc);
}
//******************************************************************************
//******************************************************************************
FARPROC WIN32API GetProcAddress(HMODULE hModule, LPCSTR lpszProc)
{
 Win32Dll *winmod;
 FARPROC   proc;
 ULONG     ulAPIOrdinal;

  winmod = Win32Dll::findModule((HINSTANCE)hModule);
  if(winmod) {
    ulAPIOrdinal = (ULONG)lpszProc;
    if (ulAPIOrdinal <= 0x0000FFFF) {
        return (FARPROC)winmod->getApi((int)ulAPIOrdinal);
    }
    else    return (FARPROC)winmod->getApi((char *)lpszProc);
  }
  proc = O32_GetProcAddress(hModule, lpszProc);
  dprintf(("KERNEL32:  GetProcAddress %s from %X returned %X\n", lpszProc, hModule, proc));
  return(proc);
}
//******************************************************************************
//Retrieve the version
//******************************************************************************
BOOL SYSTEM GetVersionStruct(char *modname, char *verstruct, ULONG bufLength)
{
 HINSTANCE   hinstance;
 Win32Image *winimage;

  dprintf(("GetVersionStruct"));
  hinstance = OS2QueryModuleHandle(modname);
  if(hinstance == 0) {
    dprintf(("GetVersionStruct can't find handle for %s\n", modname));
    return(FALSE);
  }
  if(WinExe && WinExe->getOS2InstanceHandle() == hinstance) {
    winimage = (Win32Image *)WinExe;
  }
  else {
    winimage = (Win32Image *)Win32Dll::findModule(hinstance);
    if(winimage == NULL) {
        dprintf(("GetVersionStruct can't find Win32Image for %s\n", modname));
        return(FALSE);
    }
  }
  if(winimage->getVersionId() == -1) {
    dprintf(("GetVersionStruct: %s has no version resource!\n", modname));
    return(FALSE);
  }
  return OS2GetResource(hinstance, winimage->getVersionId(), verstruct, bufLength);
}
//******************************************************************************
//******************************************************************************
ULONG SYSTEM GetVersionSize(char *modname)
{
 HINSTANCE   hinstance;
 Win32Image *winimage;

  dprintf(("GetVersionSize of %s\n", modname));
  hinstance = OS2QueryModuleHandle(modname);
  if(hinstance == 0) {
    dprintf(("GetVersionSize can't find handle for %s\n", modname));
    return(FALSE);
  }

  if(WinExe && WinExe->getOS2InstanceHandle() == hinstance) {
    winimage = (Win32Image *)WinExe;
  }
  else {
    winimage = (Win32Image *)Win32Dll::findModule(hinstance);
    if(winimage == NULL) {
        dprintf(("GetVersionSize can't find Win32Image for %s\n", modname));
        return(FALSE);
    }
  }
  if(winimage->getVersionId() == -1) {
    dprintf(("GetVersionSize: %s has no version resource!\n", modname));
    return(FALSE);
  }
  ULONG size = OS2GetResourceSize(hinstance, winimage->getVersionId());

  dprintf(("Version resource size = %d, id %d\n", size, winimage->getVersionId()));
  return(size);
}
//******************************************************************************
//******************************************************************************
