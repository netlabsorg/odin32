/*
 * DLL entry point
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

BOOL  fVersionWarp3 = FALSE;
static HKEY hKeyClassesRoot  = 0;
static HKEY hKeyCurrentUser  = 0;
static HKEY hKeyLocalMachine = 0;
static HKEY hKeyUsers        = 0;

static HMODULE hDllAdvapi32  = 0;
static HMODULE hDllGdi32     = 0;

#ifdef __IBMCPP__
extern "C" {

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
ULONG DLLENTRYPOINT_CCONV DLLENTRYPOINT_NAME(ULONG hModule, ULONG ulFlag)
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
         rc = DosLoadModule(szErrName, sizeof(szErrName), "XXODIN32.DLL", &hModule);
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

         SetCustomBuildName("NTDLL.DLL");
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("KERNEL32.DLL");
         rc = inittermKernel32(hModule, ulFlag);
         if(rc == 0) 
             return 0UL;

         SetCustomBuildName("USER32.DLL");
         rc = inittermUser32(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName("GDI32.DLL");
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("ADVAPI32.DLL");
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("VERSION.DLL");        
         if(RegisterLxDll(hModule, NULL, (PVOID)NULL) == 0)
            return 0UL;

         SetCustomBuildName("WSOCK32.DLL");
         rc = inittermWsock32(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName("WINMM.DLL");
         rc = inittermWinmm(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName("RPCRT4.DLL");
         rc = inittermRpcrt4(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName("OLE32.DLL");
         rc = inittermOle32(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName("COMCTL32.DLL");
         rc = inittermComctl32(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName("SHELL32.DLL");
         rc = inittermShell32(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName("COMDLG32.DLL");
         rc = inittermComdlg32(hModule, ulFlag);
         if(rc == 0) 
                return 0UL;

         SetCustomBuildName(NULL);
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
ULONG APIENTRY InitializeKernel32()
{
    HMODULE hModule;

    DosQueryModuleHandle("WGSS50", &hModule);
    O32__DLL_InitTerm(hModule, 0);
    DosQueryModuleHandle("XXODIN32", &hModule);
    return inittermKernel32(hModule, 0);
}
//******************************************************************************
//******************************************************************************
#else
#error message("compiler is not supported");
#endif
