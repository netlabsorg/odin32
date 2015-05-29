/*
 * DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

//
// @todo Custom Build is broken ATM:
//
// 1. inittermXXX()/cleanupXXX() are now DLL_InitXXX/DLL_TermXXX
//    (see the respective initterm.cpp files for more info).
// 2. There is no inittermXXX.cpp files any longer (all initialization functions
//    are in XXX/initterm.cpp) so a define (e.g. CUSTOMBUILD) is necessary to
//    disable compilation multiple versions of DLL_Init()/DLL_Term().
// N. ...
//

#define  INCL_DOSMODULEMGR
#define  INCL_DOSMISC
#define  INCL_DOSPROCESS
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <win32api.h>
#include <win32type.h>
#include <odinapi.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>
#include <exitlist.h>
#include "ordinals.h"

BOOL  fVersionWarp3 = FALSE;
static HKEY hKeyClassesRoot  = 0;
static HKEY hKeyCurrentUser  = 0;
static HKEY hKeyLocalMachine = 0;
static HKEY hKeyUsers        = 0;

static HMODULE hDllAdvapi32  = 0;
static HMODULE hDllGdi32     = 0;

#ifdef __IBMCPP__
extern "C" {

ULONG APIENTRY inittermKernel32(ULONG hModule);
void  APIENTRY cleanupKernel32(ULONG hModule);

ULONG APIENTRY inittermUser32(ULONG hModule, ULONG ulFlag);
void  APIENTRY cleanupUser32(ULONG ulReason);
ULONG APIENTRY inittermOdinCtrl(ULONG hModule, ULONG ulFlag);

ULONG APIENTRY inittermWinmm(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermShell32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermOle32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermComdlg32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermComctl32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermGdi32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermNTDLL(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermWsock32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermWininet(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermRpcrt4(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermAvifil32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermQuartz(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermRiched32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermWnaspi32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermUxTheme(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermUsp10(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermDInput(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermDSound(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermWinSpool(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermDDraw(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermNTDLL(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermMSVCRT(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermImm32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermCrypt32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermOleacc(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermmscms(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermRsaenh(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermSecur32(ULONG hModule, ULONG ulFlag);

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);
}

/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
ULONG SYSTEM _DLL_InitTerm(ULONG hModule, ULONG ulFlag)
{
   size_t i;
   APIRET rc;
   ULONG  version[2];
   static BOOL fInit = FALSE, fExit = FALSE;

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
      {
         /*******************************************************************/
         /* The C run-time environment initialization function must be      */
         /* called before any calls to C run-time functions that are not    */
         /* inlined.                                                        */
         /*******************************************************************/

         if (_CRT_init() == -1)
            return 0UL;
         ctordtorInit();

         rc = DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MINOR, version, sizeof(version));
         if(rc == 0){
             if(version[0] >= 20 && version[1] <= 30) {
                 fVersionWarp3 = TRUE;
             }
         }

         /*******************************************************************/
         /* A DosExitList routine must be used to clean up if runtime calls */
         /* are required and the runtime is dynamically linked.             */
         /*******************************************************************/
         rc = DosExitList(EXITLIST_KERNEL32|EXLST_ADD, cleanup);
         if(rc)
                return 0UL;

         char szErrName[CCHMAXPATH];
         rc = DosLoadModule(szErrName, sizeof(szErrName), "XXODIN32", &hModule);
         if(rc != 0) {
             return 0;
         }

         if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\XXOdin32\\REGROOT_HKEY_ClassesRoot",&hKeyClassesRoot)!=ERROR_SUCCESS_W) {
             return 0;
         }
         if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\XXOdin32\\REGROOT_HKEY_CurrentUser",&hKeyCurrentUser)!=ERROR_SUCCESS_W) {
             return 0;
         }
         if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\XXOdin32\\REGROOT_HKEY_LocalMachine",&hKeyLocalMachine)!=ERROR_SUCCESS_W) {
             return 0;
         }
         if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\XXOdin32\\REGROOT_HKEY_Users",&hKeyUsers)!=ERROR_SUCCESS_W) {
             return 0;
         }
         SetRegistryRootKey(HKEY_CLASSES_ROOT, hKeyClassesRoot);
         SetRegistryRootKey(HKEY_CURRENT_USER, hKeyCurrentUser);
         SetRegistryRootKey(HKEY_LOCAL_MACHINE, hKeyLocalMachine);
         SetRegistryRootKey(HKEY_USERS, hKeyUsers);

         SetCustomBuildName("NTDLL.DLL", 0);
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("KERNEL32.DLL", ORDINALBASE_KERNEL32);
         rc = inittermKernel32(hModule, ulFlag);
         if(rc == 0)
             return 0UL;

         SetCustomBuildName("USER32.DLL", ORDINALBASE_USER32);
         rc = inittermUser32(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("GDI32.DLL", ORDINALBASE_GDI32);
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("ADVAPI32.DLL", 0);
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("VERSION.DLL", 0);
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("WSOCK32.DLL", ORDINALBASE_WSOCK32);
         rc = inittermWsock32(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("WINMM.DLL", 0);
         rc = inittermWinmm(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("RPCRT4.DLL", 0);
         rc = inittermRpcrt4(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("OLE32.DLL", ORDINALBASE_OLE32);
         rc = inittermOle32(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("COMCTL32.DLL", ORDINALBASE_COMCTL32);
         rc = inittermComctl32(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("SHLWAPI.DLL", ORDINALBASE_SHLWAPI);
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("SHELL32.DLL", ORDINALBASE_SHELL32);
         rc = inittermShell32(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("COMDLG32.DLL", 0);
         rc = inittermComdlg32(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName("RICHED32.DLL", 0);
         rc = inittermRiched32(hModule, ulFlag);
         if(rc == 0)
                return 0UL;

         SetCustomBuildName(NULL, 0);
         break;
      }

      case 1 :
      {
         inittermComdlg32(hModule, ulFlag);
         inittermShell32(hModule, ulFlag);
         inittermComctl32(hModule, ulFlag);
         inittermOle32(hModule, ulFlag);
         inittermRpcrt4(hModule, ulFlag);
         inittermWinmm(hModule, ulFlag);
         inittermWsock32(hModule, ulFlag);
         inittermUser32(hModule, ulFlag);
         inittermKernel32(hModule, ulFlag);
         break;
      }

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
static void APIENTRY cleanup(ULONG ulReason)
{
   cleanupUser32(ulReason);
   cleanupKernel32(ulReason);
   ctordtorTerm();
   _CRT_term();
   DosExitList(EXLST_EXIT, cleanup);
   return ;
}
//******************************************************************************
//******************************************************************************
ULONG APIENTRY O32__DLL_InitTerm(ULONG handle, ULONG flag);
//******************************************************************************
BOOL APIENTRY InitializeKernel32()
{
    HMODULE hModule;

    DosQueryModuleHandleStrict("WGSS50", &hModule);
    return O32__DLL_InitTerm(hModule, 0) != 0;
}
//******************************************************************************
//******************************************************************************
#else
#error message("compiler is not supported");
#endif
