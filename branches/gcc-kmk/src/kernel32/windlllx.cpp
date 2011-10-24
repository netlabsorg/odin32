/* $Id: windlllx.cpp,v 1.29 2004-01-15 10:39:07 sandervl Exp $ */

/*
 * Win32 LX Dll class (compiled in OS/2 using Odin32 api)
 *
 * Copyright 1999-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 * TODO: Unloading of dlls probably needs to be fixed due to OS/2 bug
 *       (wrong unload order of dlls)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <os2newapi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef __GNUC__
#include <iostream.h>
#include <fstream.h>
#endif
#include <misc.h>
#include <win32type.h>
#include "windllbase.h"
#include "windlllx.h"
#include "winexepe2lx.h"
#include "winexepeldr.h"
#include <odinlx.h>
#include <wprocess.h>
#include "oslibmisc.h"
#include <custombuild.h>

#include <exe386.h>

#define DBG_LOCALLOG    DBG_windlllx
#include "dbglocal.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
char *lpszCustomDllName      = NULL;
PIMAGE_FILE_HEADER lpCustomDllPEHdr = NULL;

/**
 * FreeLibrary callback for LX Dlls, it's call only when the library is actually
 * being unloaded.
 * Maintained by ODIN_SetLxDllUnLoadCallback().
 */
PFNLXDLLUNLOAD pfnLxDllUnLoadCallback = NULL;

//******************************************************************************
//******************************************************************************
void WIN32API SetCustomBuildName(char *lpszName, PIMAGE_FILE_HEADER pfh)
{
    lpszCustomDllName      = lpszName;
    lpCustomDllPEHdr       = pfh;
}
//******************************************************************************
//Create LX Dll object and send process attach message
//System dlls set EntryPoint to 0
//Parameters:
//  HINSTANCE hInstance     - OS/2 module handle
//  WIN32DLLENTRY EntryPoint    - Win32 dll entrypoint address
//  PVOID pResData              - pointer to win32 resource data
//  DWORD MajorImageVersion     - major image/os version (for fake win32 header)
//  DWORD MinorImageVersion     - minor image/os version (for fake win32 header)
//  DWORD Subsystem             - subsystem type (for fake win32 header)
//                                (IMAGE_SUBSYSTEM_WINDOWS_CUI/IMAGE_SUBSYSTEM_WINDOWS_GUI)
//
//Returns: Odin32 module handle
//******************************************************************************
DWORD WIN32API RegisterLxDll(HINSTANCE hInstance, WIN32DLLENTRY EntryPoint,
                             PVOID pResData,
                             DWORD MajorImageVersion,
                             DWORD MinorImageVersion,
                             DWORD Subsystem)
{
 APIRET        rc;
 Win32LxDll    *windll;
 Win32DllBase *windlldep;
 char          szFileName[CCHMAXPATH], szErrName[CCHMAXPATH];

   if(!lpszCustomDllName) {
       if(OSLibGetDllName(hInstance, szFileName, sizeof(szFileName)) == FALSE) {
           dprintf(("ERROR: RegisterLxDll: OSLibGetDllName %x failed!!", hInstance));
           return 0;
       }
       dprintf(("RegisterLxDll %x %s", hInstance, szFileName));
       //Make sure DosLoadModule is called at least once for a dll (to make sure
       //OS/2 doesn't unload the dll when it's still needed)
       rc = DosLoadModule(szErrName, sizeof(szErrName), szFileName, &hInstance);
       if(rc != 0) {
           dprintf(("ERROR: RegisterLxDll: DosLoadModule %s failed (rc=%d)!!", szFileName, rc));
           return 0;
       }
   }
   else {
       //make sure this dll hasn't already been loaded
       if(Win32DllBase::findModule(lpszCustomDllName) != NULL) {
           dprintf(("ERROR: RegisterLxDll: module %s already loaded!!", lpszCustomDllName));
           DebugInt3();
           return 0;
       }
   }
   windll = new Win32LxDll(hInstance, EntryPoint, pResData, MajorImageVersion,
                           MinorImageVersion, Subsystem);
   if(windll == NULL) {
       dprintf(("RegisterLxDll: windll == NULL!!!"));
       return 0;
   }
   //clear name override in case dll init loads another dll
   lpszCustomDllName = NULL;
   lpCustomDllPEHdr  = NULL;

   if(!fPeLoader) {
       windll->AddRef();

       if(windll->attachProcess() == 0)
           return 0;

       return windll->getInstanceHandle();
   }
   IMAGE_DOS_HEADER doshdr;
   struct e32_exe   lxhdr;
   ULONG            offset;
   char             modulename[CCHMAXPATH];
   char             modsize;
   int              i;

   //SvL: This code reads the import name table of the dll to get the dependencies
   //     on other dlls.
   //DosQueryHeaderInfo is an undocumented api, but works very well.
   //(no need to save FS here as we'll return to OS/2 immediately)
   rc = DosQueryHeaderInfo(hInstance, 0, &doshdr, sizeof(IMAGE_DOS_HEADER), QHINF_READFILE);
   if(rc) {
       goto hdrerror;
   }
   rc = DosQueryHeaderInfo(hInstance, doshdr.e_lfanew, &lxhdr, sizeof(e32_exe), QHINF_READFILE);
   if(rc) {
       goto hdrerror;
   }
   offset = doshdr.e_lfanew + lxhdr.e32_impmod;
   for(i=0;i<lxhdr.e32_impmodcnt;i++) {
       rc = DosQueryHeaderInfo(hInstance, offset, &modsize, 1, QHINF_READFILE);
       if(rc) {
           goto hdrerror;
       }
       rc = DosQueryHeaderInfo(hInstance, offset+1, &modulename, min(modsize, sizeof(modulename)), QHINF_READFILE);
       if(rc) {
           goto hdrerror;
       }
       modulename[modsize] = 0;
       windlldep = Win32DllBase::findModule(modulename, TRUE);
       if(windlldep && strcmp(windlldep->getModuleName(), windll->getModuleName())) {
            dprintf(("RegisterLxDll: Add dependency %s -> %s", windll->getModuleName(), modulename));
            windll->addDependency(windlldep);
       }
       else dprintf(("HARMLESS WARNING: Can't find dll %s referenced by %s", modulename, windll->getModuleName()));

       offset += modsize + 1;
   }
#ifdef HACK_NEVER_UNLOAD_LX_DLLS
   //HACK ALERT!!
   //This makes sure the LX dll never gets unloaded.
   //Necessary since unloading doesn't work due to dependencies on dlls
   //with exitlist handlers.
   windll->AddRef();
#endif
   return windll->getInstanceHandle();

hdrerror:
   dprintf(("DosQueryHeaderInfo returned %d", rc));
   return windll->getInstanceHandle();
}
//******************************************************************************
//Destroy LX Dll object
//******************************************************************************
BOOL WIN32API UnregisterLxDll(HINSTANCE hInstance)
{
 Win32DllBase *windll;

   //Don't proceed for pe2lx/win32k (os/2 dll unload dependency bug)
   //Don't do it either after ExitProcess has been called
   if(!fPeLoader || WinExe == NULL)
    return TRUE;

   windll = Win32DllBase::findModule(hInstance);
   if(!windll) {
    dprintf(("UnregisterLxDll: Can't find dll with handle %x (already deleted)", hInstance));
    return TRUE; //already deleted by Win32LxDll::Release
   }
   dprintf(("UnregisterLxDll %s", windll->getModuleName()));
   //This can only happen for LX dependencies (i.e. wininet loads wsock32)
   delete windll;
   return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32LxDll::Win32LxDll(HINSTANCE hInstance, WIN32DLLENTRY EntryPoint, PVOID pResData,
                       DWORD MajorImageVersion, DWORD MinorImageVersion,
                       DWORD Subsystem)
                : Win32ImageBase(hInstance),
                  Win32LxImage(hInstance, pResData),
                  Win32DllBase(hInstance, EntryPoint)
{
  this->MajorImageVersion = MajorImageVersion;
  this->MinorImageVersion = MinorImageVersion;
  this->Subsystem         = Subsystem;

  if(EntryPoint == NULL) {
    fSkipThreadEntryCalls    = TRUE;
    fAttachedToProcess = TRUE;
  }
  hinstanceOS2 = hInstance;
  //new win32 instance handle must be pointer to PE header
  hinstance = (HINSTANCE)buildHeader(MajorImageVersion, MinorImageVersion,
                                     Subsystem);
  dprintf(("Fake PE header %x for dll %s", hinstance, getModuleName()));
}
//******************************************************************************
//******************************************************************************
Win32LxDll::~Win32LxDll()
{
}
//******************************************************************************
//Returns reference count or -1 if load failed (PE loader only!)
//******************************************************************************
#ifdef DEBUG
ULONG Win32LxDll::AddRef(char *parentname)
#else
ULONG Win32LxDll::AddRef()
#endif
{
 Win32DllBase *dll;
 QueueItem    *item;
 ULONG         ret;

#ifdef DEBUG
  ret = Win32DllBase::AddRef(parentname);
#else
  ret = Win32DllBase::AddRef();
#endif

  if(!fPeLoader)
    return ret;

  if(referenced == 1)
  {
    item = loadedDlls.Head();
    while(item) {
        dll = (Win32DllBase *)loadedDlls.getItem(item);
        if(dll == NULL) {
            dprintf(("ERROR: Win32DllBase::AddRef: dll item == NULL!!"));
            DebugInt3();
            return -1;
        }
#ifdef DEBUG
        dll->AddRef(getModuleName());
#else
        dll->AddRef();
#endif
        item = loadedDlls.getNext(item);
    }
    if(attachProcess() == 0) {
        dprintf(("WARNING: Dll %s refused to be loaded; aborting", getName()));
        return -1;
    }
  }
  return ret;
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxDll::Release()
{
 HINSTANCE     hinst                = hinstanceOS2;
 HINSTANCE     hinstOdin            = hinstance;
 WIN32DLLENTRY EntryPointTmp        = dllEntryPoint;
 PVOID         pResDataTmp          = (PVOID)pResRootDir;
 DWORD         MajorImageVersionTmp = MajorImageVersion;
 DWORD         MinorImageVersionTmp = MinorImageVersion;
 DWORD         SubsystemTmp         = Subsystem;
 BOOL          fNoUnload            = fDisableUnload; //only set for kernel32.dll
 BOOL          fDynLoaded           = isDynamicLib();
 ULONG         ret;
 APIRET        rc;

  ret = Win32DllBase::Release();

  /** @sketch
   * If this module is not unreference and it was loaded using LoadLibrary() then
   *    call the custombuild callback if present.
   *    The callback should call UnRegisterLxDll().
   * Endif
   *
   * @remark
   * #ifdef HACK_NEVER_UNLOAD_LX_DLLS
   * This will never be called!
   * #endif
   *
   * @todo: call pfnLxDllLoadCallback if DosFreeModule failes.
   *        It's not implemented because it's complex and at the moment we will
   *        never actually get here.
   *        So, this callback is here just as a reminder eventhough it's working..
   */
  if (!ret && fDynLoaded && pfnLxDllUnLoadCallback)
  {
    pfnLxDllUnLoadCallback(hinstanceOS2, hinstance);
  }

  if (ret == 0 && !fNoUnload) {//only set for kernel32.dll (fDisableUnload)
    //DosFreeModule sends a termination message to the dll.
    //The LX dll informs us when it's removed (UnregisterDll call)
    rc = DosFreeModule(hinst);
    if(rc) {
            dprintf(("Win32LxDll::Release: DosFreeModule %x returned %d", hinst, rc));
            if(rc == ERROR_INVALID_ACCESS && !fExitProcess)
            {
                #ifndef HACK_NEVER_UNLOAD_LX_DLLS
                #error  @todo: call pfnLxDllLoadCallback and let it do the registering if it did that on the initial load..
                #endif
                //Dll refused to unload because it has an active exitlist handler
                //or depends on a dll that registered an exitlist handler
                //In this case the handle remains valid and the entrypoint of
                //the dll is NOT called for DLL_PROCESS_DETACH
                //WORKAROUND: Re-register the dll so future LoadLibrary calls
                //            don't fail!
                dprintf(("WORKAROUND: Re-register the dll so future LoadLibrary calls don't fail!"));
                RegisterLxDll(hinst, EntryPointTmp, pResDataTmp,
                              MajorImageVersionTmp,
                              MinorImageVersionTmp,
                              SubsystemTmp);

                /* OS/2 dll, system dll, converted dll or win32k took care of it. */
                Win32LxDll *pModule = Win32LxDll::findModuleByOS2Handle(hinst);
                if(pModule)
                {
                        pModule->setDllHandleOS2(hinst);
                        if(fPeLoader)
                        {
                            if(pModule->AddRef() == -1) {//-1 -> load failed (attachProcess)
                                dprintf(("ERROR Dll refused to be loaded; aborting"));
                                DebugInt3();
                                delete pModule;
                                return 0;
                            }
                        }
                        pModule->incDynamicLib();
                }
                else    DebugInt3();
            }
    }
  }
  return(ret);
}
//******************************************************************************
//******************************************************************************
BOOL Win32LxDll::isPe2LxDll() const
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32LxDll::isLxDll() const
{
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void Win32LxDll::setDllHandleOS2(HINSTANCE hInstanceOS2)
{
  //Loaded with LoadLibrary(Ex); no need for a 2nd DosLoadModule
  //Dlls that are indirectly loaded (i.e. GDI32->KERNEL32 dependancy) need
  //this additional DosLoadModule (and setDllHandleOS2 isn't called for those)
  if(this->hinstanceOS2) {
    DosFreeModule(this->hinstanceOS2);
  }
  this->hinstanceOS2 = hInstanceOS2;
}
//******************************************************************************
//******************************************************************************
Win32LxDll *Win32LxDll::findModuleByOS2Handle(HINSTANCE hinstance)
{
    dlllistmutex.enter();

    Win32DllBase *mod = Win32DllBase::getFirst();
    while (mod != NULL)
    {
        if (mod->isLxDll())
        {
            Win32LxDll *lxdll = (Win32LxDll *)mod;
            if (lxdll->hinstanceOS2 == hinstance)
            {
                dlllistmutex.leave();
                return(lxdll);
            }
        }
        mod = mod->getNext();
    }
    dlllistmutex.leave();
    return(NULL);
}
//******************************************************************************
//******************************************************************************

/**
 * Custombuild API for registering a callback for LX Dll loading thru LoadLibrary*().
 * @returns Success indicator.
 * @param   pfn     Pointer to callback.
 *                  NULL if callback is deregistered.
 */
BOOL WIN32API ODIN_SetLxDllUnLoadCallback(PFNLXDLLUNLOAD pfn)
{
    pfnLxDllUnLoadCallback = pfn;
    return TRUE;
}

