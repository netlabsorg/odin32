/* $Id: windllbase.cpp,v 1.27 2001-07-08 11:02:10 sandervl Exp $ */

/*
 * Win32 Dll base class
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Unloading of a dll always happens in order of dependency (taking nr of
 * loads into account)
 * Unloading of dynamically loaded dll (with LoadLibrary) in deleteAll
 * is done in LIFO order (NT exhibits the same behaviour)
 *
 * RemoveCircularDependency: TODO: Send process detach message here??
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <misc.h>
#include <win32api.h>
#include <pefile.h>
#include <windllbase.h>
#include <winimagepe2lx.h>
#include <windllpe2lx.h>
#include <winimagelx.h>
#include <windlllx.h>
#include <wprocess.h>
#include "exceptions.h"
#include "exceptutil.h"
#include "cio.h"
#include "vmutex.h"
#include "oslibmisc.h"
#include "oslibdos.h"
#include "profile.h"

#define DBG_LOCALLOG    DBG_windllbase
#include "dbglocal.h"

VMutex dlllistmutex;   //protects linked lists of heaps

//******************************************************************************
//******************************************************************************
Win32DllBase::Win32DllBase(HINSTANCE hinstance, WIN32DLLENTRY DllEntryPoint,
                           Win32ImageBase *parent)
                 : Win32ImageBase(hinstance),
                    referenced(0), fSkipThreadEntryCalls(FALSE), next(NULL), fInserted(FALSE),
                    fAttachedToProcess(FALSE), fUnloaded(FALSE),
                    nrDynamicLibRef(0), fDisableUnload(FALSE), fSkipEntryCalls(FALSE)
{
    dllEntryPoint = DllEntryPoint;

    setUnloadOrder(parent);

    dprintf(("Win32DllBase::Win32DllBase %x %s", hinstance, szModule));
}
//******************************************************************************
//******************************************************************************
Win32DllBase::~Win32DllBase()
{
    dprintf(("Win32DllBase::~Win32DllBase %s", szModule));

    if(errorState == NO_ERROR && !fUnloaded)
    {
        detachProcess();
    }

    dlllistmutex.enter();
    if(head == this) {
        head = next;
    }
    else {
        Win32DllBase *dll = head;
        while(dll && dll->next != this) {
            dll = dll->next;
        }
        if(dll == NULL) {
            dprintf(("~Win32DllBase: Can't find dll!\n"));
            dlllistmutex.leave();
            return;
        }
        dll->next = next;
    }
    dlllistmutex.leave();
}
//******************************************************************************
//******************************************************************************
void Win32DllBase::incDynamicLib()
{
    if(nrDynamicLibRef == 0) {
        //NOTE:
        //Must be called *after* attachprocess, since attachprocess may also
        //trigger LoadLibrary calls
        //Those dlls must not be put in front of this dll in the dynamic
        //dll list; or else the unload order is wrong:
        //i.e. RPAP3260 loads PNRS3260 in DLL_PROCESS_ATTACH
        //     this means that in ExitProcess, PNRS3260 needs to be removed
        //     first since RPAP3260 depends on it
        dlllistmutex.enter();
        loadLibDlls.Push((ULONG)this);
        dlllistmutex.leave();
    }
    nrDynamicLibRef++;
}
//******************************************************************************
//******************************************************************************
void Win32DllBase::decDynamicLib()
{
    nrDynamicLibRef--;
    if(nrDynamicLibRef == 0) {
        dlllistmutex.enter();
        loadLibDlls.Remove((ULONG)this);
        dlllistmutex.leave();
    }
}
//******************************************************************************
//unload of dlls needs to be done in reverse order of dependencies
//Note: Only necessary for pe loader; the OS/2 loader takes care of this
//for win32k/pe2lx
//******************************************************************************
void Win32DllBase::setUnloadOrder(Win32ImageBase *parent)
{
 Win32DllBase *dll;
 Win32DllBase *parentdll = NULL;

    dlllistmutex.enter();
    if(parent) {
        dll = head;
        while(dll) {
            if(dll->getInstanceHandle() == parent->getInstanceHandle()) {
                parentdll = dll;
                break;
            }
            dll = dll->next;
        }
    }

    //first check if this dll is already at a lower position (further down the list)
    //than the parent
    if(parentdll && fInserted) {//already in the list?
        dll = parentdll->next;
        while(dll) {
            if(dll->getInstanceHandle() == getInstanceHandle()) {
                dlllistmutex.leave();
                return; //it's at a lower position, so no need to change anything
            }
            dll = dll->next;
        }

        //it's already in the list but not at the right position; remove it now
        if(head == this) {
            head = next;
        }
        else {
        dll = head;
            while(dll->next) {
                if(dll->next == this) {
                    dll->next = next;
                    break;
                }
                dll = dll->next;
            }
        }
    }
    else
    if(fInserted) {//already in the list?
        dlllistmutex.leave();
        return;
    }
    //(re)insert it in the list after it's parent
    if(parentdll) {
        next = parentdll->next;
        parentdll->next = this;
    }
    else {//no parent or exe, just add it at the start of the list
        next = head;
        head = this;
    }
    fInserted = TRUE;

    //Now do the same thing for the child dependencies
    QueueItem *item;

    item = loadedDlls.Head();
    while(item) {
        dll = (Win32DllBase *)loadedDlls.getItem(item);
        //Check for circular dependencies (i.e. in Lotus Notes)
        if(dll != parentdll) {
            dll->setUnloadOrder(this);
        }
        item  = loadedDlls.getNext(item);
    }
    dlllistmutex.leave();
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
ULONG Win32DllBase::AddRef(char *parentname)
#else
ULONG Win32DllBase::AddRef()
#endif
{
 Win32DllBase *dll;

    dprintf(("Win32DllBase::AddRef %s->%s %d", parentname, getModuleName(), referenced+1));
    ++referenced;
#ifdef DEBUG
    if(referenced == 1) {
#ifdef DEBUG_ENABLELOG_LEVEL2
        printListOfDlls();
#endif
        //printDependencies(NULL);
    }
#endif
    return referenced;
}
//******************************************************************************
//******************************************************************************
ULONG Win32DllBase::Release()
{
 Queue         queue;
 QueueItem    *item;
 Win32DllBase *dll;
 LONG          ret;

    dprintf(("Win32DllBase::Release %s %d", getModuleName(), referenced-1));

    ret = --referenced;
    if(ret <= 0) {
        //make copy of linked list of dependencies
        queue = loadedDlls;

        //remove any circular dependencies on this dll that might be present
        item = queue.Head();
        while(item) {
            dll = (Win32DllBase *)queue.getItem(item);
            if(dll == NULL) {
                dprintf(("ERROR: Win32DllBase::Release: dll item == NULL!!"));
                DebugInt3();
                return -1;
            }
            dll->RemoveCircularDependency(this);
            item = queue.getNext(item);
        }
#ifdef DEBUG
        //printDependencies(NULL);
#endif

        dprintf(("Win32DllBase::Release %s referenced == 0", getModuleName()));

        //delete dll object
        delete this;

        //unreference all of it's dependencies
        item = queue.Head();
        while(item) {
            dll = (Win32DllBase *)queue.getItem(item);
            if(dll == NULL) {
                dprintf(("ERROR: Win32DllBase::Release: dll item == NULL!!"));
                DebugInt3();
                return -1;
            }
            dll->Release();
            item = queue.getNext(item);
        }
    }
    return(ret);
}
//******************************************************************************
//Lotus Notes has several ugly circular dependencies in it's dlls.
//Remove them before they cause problems.
//******************************************************************************
BOOL Win32DllBase::RemoveCircularDependency(Win32DllBase *parent)
{
 QueueItem    *item, *tmp;
 Win32DllBase *dll;
 BOOL          ret = FALSE;

    //remove any circular dependencies on this dll that might be present
    item = loadedDlls.Head();
    while(item) {
        dll = (Win32DllBase *)loadedDlls.getItem(item);
        if(dll == NULL) {
            dprintf(("ERROR: Win32DllBase::Release: dll item == NULL!!"));
            DebugInt3();
            return FALSE;
        }
        tmp = loadedDlls.getNext(item);
        if(dll == parent) {
            dprintf(("Removing CIRCULAR dependency %s->%s", parent->getModuleName(), dll->getModuleName()));
            loadedDlls.Remove(item);
            ret = TRUE;
        }
////    else    ret |= dll->RemoveCircularDependency(parent);
        item = tmp;
    }
    //TODO: Send process detach message here??
    return ret;
}
//******************************************************************************
//There's a slight problem with our dependency procedure.
//example: gdi32 is loaded -> depends on kernel32 (which is already loaded)
//         kernel32's reference count isn't updated
//         (when we load gdi32, we don't know which dlls it depends on and which
//          of those are already loaded and which aren't)
//-> solution: Determine reference counts of dependant lx dlls and update those
//             reference counts
//******************************************************************************
void Win32DllBase::updateDependencies()
{
 QueueItem    *item;
 Win32DllBase *dll, *depdll;
 ULONG         refcount;

    dlllistmutex.enter();
    item = loadedDlls.Head();
    while(item) {
        depdll   = (Win32DllBase *)loadedDlls.getItem(item);
        if(depdll == NULL) {
            dprintf(("updateDependencies: depdll == NULL!!"));
            DebugInt3();
            return;
        }
        refcount = 0;
        dll      = head;

        while(dll) {
            if(dll->dependsOn(depdll)) {
                refcount++;
            }
            dll = dll->getNext();
        }
        if(refcount > depdll->referenced) {
            dprintf(("Win32DllBase::updateDependencies changing refcount of %s to %d (old=%d)", depdll->getModuleName(), refcount, depdll->referenced));
            depdll->referenced = refcount;
        }
        item = loadedDlls.getNext(item);
    }
    dlllistmutex.leave();
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
void Win32DllBase::printDependencies(char *parent)
{
 QueueItem    *item;
 Win32DllBase *dll;
 ULONG         ret;

    dprintf(("Dependency list: %s->%s %d", parent, getModuleName(), referenced));
    item = loadedDlls.Head();
    while(item) {
        dll = (Win32DllBase *)loadedDlls.getItem(item);
        if(dll == NULL) {
            return;
        }
        dll->printDependencies(getModuleName());
        item = loadedDlls.getNext(item);
    }
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG_ENABLELOG_LEVEL2
void Win32DllBase::printListOfDlls()
{
 Win32DllBase *dll;

    dll = head;

    dprintf2(("Win32DllBase::Win32DllBase: List of loaded dlls:"));
    while(dll) {
        dprintf2(("DLL %s %d", dll->szModule, dll->referenced));
        dll = dll->next;
    }
}
#endif
#endif
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::attachProcess()
{
 WINEXCEPTION_FRAME exceptFrame;
 USHORT sel;
 TEB *teb;
 BOOL rc, fSetExceptionHandler;

    if(fAttachedToProcess || fSkipEntryCalls)
        return TRUE;

    fAttachedToProcess = TRUE;

    teb = GetThreadTEB();
    fSetExceptionHandler = (!teb || teb->teb_sel != GetFS());

    //Note: The Win32 exception structure references by FS:[0] is the same
    //      in OS/2
    if(fSetExceptionHandler) {
        OS2SetExceptionHandler((void *)&exceptFrame);
        sel = SetWin32TIB();
    }

    if(dllEntryPoint == NULL) {
        dprintf(("attachProcess not required for dll %s", szModule));
        if(fSetExceptionHandler) {
            SetFS(sel);
            OS2UnsetExceptionHandler((void *)&exceptFrame);
        }
        return(TRUE);
    }

    // @@@PH 2000/06/13 lpvReserved, Starcraft STORM.DLL
    // if DLL_PROCESS_ATTACH, lpvReserved is NULL for dynamic loads
    //   and non-NULL for static loads.
    // same goes for process termination
    LPVOID lpvReserved;

    if (isDynamicLib())
        lpvReserved = NULL;
    else
        lpvReserved = (LPVOID)0xdeadface; // some arbitrary value

#ifdef DEBUG
    int time1, time2;
    dprintf(("attachProcess to dll %s", szModule));
    time1 = GetTickCount();
#endif

    rc = dllEntryPoint(hinstance, DLL_PROCESS_ATTACH, lpvReserved);

#ifdef DEBUG
    time2 = GetTickCount();
    dprintf(("attachProcess to dll %s DONE in %x msec", szModule, time2-time1));
#endif

    if(fSetExceptionHandler) {
        SetFS(sel);
        OS2UnsetExceptionHandler((void *)&exceptFrame);
    }
    else
    if(teb) {
        if(teb->teb_sel != GetFS()) {
            dprintf(("Win32DllBase::attachProcess: FS was changed by dll entrypoint!!!!"));
            DebugInt3();
        }
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::detachProcess()
{
 WINEXCEPTION_FRAME exceptFrame;
 USHORT sel;
 BOOL rc;

    if(fSkipEntryCalls) {
        fUnloaded = TRUE;
        return TRUE;
    }

    if(dllEntryPoint == NULL) {
        tlsDetachThread();  //destroy TLS (main thread)
        fUnloaded = TRUE;
        return(TRUE);
    }

    dprintf(("detachProcess from dll %s", szModule));

    //Note: The Win32 exception structure references by FS:[0] is the same
    //      in OS/2
    OS2SetExceptionHandler((void *)&exceptFrame);

    fUnloaded = TRUE;
    sel = SetWin32TIB();

    // @@@PH 2000/06/13 lpvReserved, Starcraft STORM.DLL
    // if DLL_PROCESS_ATTACH, lpvReserved is NULL for dynamic loads
    //   and non-NULL for static loads.
    // same goes for process termination
    LPVOID lpvReserved;

    if (isDynamicLib())
        lpvReserved = NULL;
    else
        lpvReserved = (LPVOID)0xdeadface; // some arbitrary value

    rc = dllEntryPoint(hinstance, DLL_PROCESS_DETACH, lpvReserved);

    SetFS(sel);
    tlsDetachThread();    //destroy TLS (main thread)
    tlsDelete();

    OS2UnsetExceptionHandler((void *)&exceptFrame);

    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::attachThread()
{
 WINEXCEPTION_FRAME exceptFrame;
 BOOL               rc;

    if(fSkipThreadEntryCalls || dllEntryPoint == NULL)
        return(TRUE);

    dprintf(("attachThread to dll %s", szModule));

    rc = dllEntryPoint(hinstance, DLL_THREAD_ATTACH, 0);

    dprintf(("attachThread to dll %s DONE", szModule));

    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::detachThread()
{
 WINEXCEPTION_FRAME exceptFrame;
 BOOL               rc;

    if(fSkipThreadEntryCalls || dllEntryPoint == NULL)
        return(TRUE);

    dprintf(("detachThread from dll %s", szModule));

    rc = dllEntryPoint(hinstance, DLL_THREAD_DETACH, 0);
    return rc;
}
//******************************************************************************
//Send DLL_THREAD_ATTACH message to all dlls for a new thread
//******************************************************************************
void Win32DllBase::attachThreadToAllDlls()
{
    dlllistmutex.enter();
    Win32DllBase *dll = Win32DllBase::head;
    while(dll) {
        dll->attachThread();
        dll = dll->getNext();
    }
    dlllistmutex.leave();
}
//******************************************************************************
//Send DLL_THREAD_DETACH message to all dlls for thread that's about to die
//******************************************************************************
void Win32DllBase::detachThreadFromAllDlls()
{
    dlllistmutex.enter();
    Win32DllBase *dll = Win32DllBase::head;
    while(dll) {
        dll->detachThread();
        dll = dll->getNext();
    }
  dlllistmutex.leave();
}
//******************************************************************************
//Setup TLS structure for all dlls for a new thread
//******************************************************************************
void Win32DllBase::tlsAttachThreadToAllDlls()
{
    dlllistmutex.enter();
    Win32DllBase *dll = Win32DllBase::head;
    while(dll) {
        dll->tlsAttachThread();
        dll = dll->getNext();
    }
    dlllistmutex.leave();
}
//******************************************************************************
//Destroy TLS structure for all dlls for a thread that's about to die
//******************************************************************************
void Win32DllBase::tlsDetachThreadFromAllDlls()
{
    dlllistmutex.enter();
    Win32DllBase *dll = Win32DllBase::head;
    while(dll) {
        dll->tlsDetachThread();
        dll = dll->getNext();
    }
    dlllistmutex.leave();
}
//******************************************************************************
//******************************************************************************
void Win32DllBase::deleteAll()
{
 Win32DllBase *dll = Win32DllBase::head;

    dprintf(("Win32DllBase::deleteAll"));

#ifdef DEBUG_ENABLELOG_LEVEL2
    if(dll) dll->printListOfDlls();
#endif

    dlllistmutex.enter();
    while(dll) {
        dll->Release();
        dll = Win32DllBase::head;
    }
    dlllistmutex.leave();
    dprintf(("Win32DllBase::deleteAll Done!"));
}
//******************************************************************************
//Delete dlls loaded by LoadLibrary(Ex) in LIFO order
//******************************************************************************
void Win32DllBase::deleteDynamicLibs()
{
 Win32DllBase *dll = head;
 QueueItem    *item;

    dprintf(("Win32DllBase::deleteDynamicLibs"));
#ifdef DEBUG_ENABLELOG_LEVEL2
    if(dll) dll->printListOfDlls();
#endif

    dlllistmutex.enter();

    item = loadLibDlls.Head();
    while(item) {
        dll = (Win32DllBase *)loadLibDlls.getItem(item);
        int dynref = dll->nrDynamicLibRef;
        if(dynref) {
            while(dynref) {
                dynref--;
                dll->decDynamicLib();
                dll->Release();
            }
        }
        else    DebugInt3();
        item = loadLibDlls.Head(); //queue could have been changed, so start from the beginning
    }

    dlllistmutex.leave();
    dprintf(("Win32DllBase::deleteDynamicLibs end"));
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::getFirst()
{
    return head;
}
//******************************************************************************
//Add renaming profile strings for ole32 & netapi32 to odin.ini if they aren't
//already there
//******************************************************************************
void Win32DllBase::setDefaultRenaming()
{
 char renameddll[CCHMAXPATH];

    if(PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "OLE32", "", renameddll,
                                sizeof(renameddll)-1) <= 1)
    {
        PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "OLE32", "OLE32OS2");
        PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "OLE32OS2", "OLE32");
    }
    if(PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "OLEAUT32", "", renameddll,
                                sizeof(renameddll)-1) <= 1)
    {
        PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "OLEAUT32", "OLAUTOS2");
        PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "OLAUTOS2", "OLEAUT32");
    }
    if(PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "NETAPI32", "", renameddll,
                                sizeof(renameddll)-1) <= 1)
    {
        PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "NETAPI32", "WNETAP32");
        PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "WNETAP32", "NETAPI32");
    }
    if(PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "WINSPOOL", "", renameddll,
                                sizeof(renameddll)-1) <= 1)
    {
        PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "WINSPOOL", "WINSPOOL.DLL");
        PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "WINSPOOL", "WINSPOOL.DRV");
    }
    if(PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "MCICDA", "", renameddll,
                                sizeof(renameddll)-1) <= 1)
    {
        PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "MCICDA", "MCICDA.DLL");
        PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "MCICDA", "MCICDA.DRV");
    }
    if(PROFILE_GetOdinIniString(DLLRENAMEWIN_SECTION, "CRTDLL", "", renameddll,
                                sizeof(renameddll)-1) <= 1)
    {
        PROFILE_SetOdinIniString(DLLRENAMEWIN_SECTION, "CRTDLL",   "CRTDLL32");
        PROFILE_SetOdinIniString(DLLRENAMEOS2_SECTION, "CRTDLL32", "CRTDLL");
    }
}
//******************************************************************************
//rename dll if necessary:
// Win32 to OS/2 : (i.e. OLE32 -> OLE32OS2)
// or
// OS/2 to Win32 : (i.e. OLE32OS2 -> OLE32)
//******************************************************************************
void Win32DllBase::renameDll(char *dllname, BOOL fWinToOS2)
{
 char modname[CCHMAXPATH];
 char renameddll[CCHMAXPATH];
 char *namestart;
 char *sectionname;

    if(fWinToOS2) {
        sectionname = DLLRENAMEWIN_SECTION;
    }
    else {
        sectionname = DLLRENAMEOS2_SECTION;
    }
    namestart = OSLibStripPath(dllname);
    strcpy(modname, namestart);
    char *dot = strrchr(modname, '.');
    if(dot)
        *dot = 0;
    strupr(modname);
    if(PROFILE_GetOdinIniString(sectionname, modname, "", renameddll,
                                   sizeof(renameddll)-1) > 1)
    {
        if(namestart == dllname) {
            strcpy(dllname, renameddll);
        }
        else {
            *namestart = 0;
            strcat(dllname, renameddll);
        }
        if(!strchr(dllname, '.')) {
            strcat(dllname, DLL_EXTENSION);
        }
        strupr(dllname);
    }
    return;
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModule(char *dllname, BOOL fRenameFirst)
{
 Win32DllBase *dll;
 char szDllName[CCHMAXPATH];
 char *dot, *temp;

////  dprintf2(("findModule %s", dllname));

    strcpy(szDllName, OSLibStripPath(dllname));
    strupr(szDllName);

    if(fRenameFirst) {
        renameDll(szDllName, FALSE);
    }
    dot = strstr(szDllName, ".");
    if(dot == NULL) {
        //if there's no extension or trainling dot, we
        //assume it's a dll (see Win32 SDK docs)
        strcat(szDllName, DLL_EXTENSION);
    }
    else {
        if(dot[1] == 0) {
            //a trailing dot means the module has no extension (SDK docs)
            *dot = 0;
        }
    }

    dlllistmutex.enter();
    dll = head;
    while(dll) {
        if(strcmpi(szDllName, dll->szModule) == 0) {
            dlllistmutex.leave();
            return(dll);
        }
        dll = dll->next;
    }
    dlllistmutex.leave();
    return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModule(WIN32DLLENTRY DllEntryPoint)
{
    dprintf2(("findModule %X", DllEntryPoint));

    dlllistmutex.enter();
    Win32DllBase *mod = Win32DllBase::head;
    while(mod != NULL) {
        dbgCheckObj(mod);
        if(mod->dllEntryPoint == DllEntryPoint) {
            dlllistmutex.leave();
            return(mod);
        }
        mod = mod->next;
    }
    dlllistmutex.leave();
    return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModule(HINSTANCE hinstance)
{
    dlllistmutex.enter();

    Win32DllBase *mod = Win32DllBase::head;
    while(mod != NULL) {
        dbgCheckObj(mod);
        if(mod->hinstance == hinstance) {
            dlllistmutex.leave();
            return(mod);
        }
        mod = mod->next;
    }
    dlllistmutex.leave();
    return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModuleByAddr(ULONG address)
{
    dlllistmutex.enter();

    Win32DllBase *mod = Win32DllBase::head;
    while(mod != NULL) {
        dbgCheckObj(mod);
        if(mod->insideModule(address)) {
            dlllistmutex.leave();
            return(mod);
        }
        mod = mod->next;
    }
    dlllistmutex.leave();
    return(NULL);
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::findModuleByOS2Handle(HINSTANCE hinstance)
{
    dlllistmutex.enter();

    for (Win32DllBase *pMod = Win32DllBase::getFirst(); pMod; pMod = pMod->getNext())
    {
        if (pMod->isLxDll())
        {
            if (((Win32LxDll *)pMod)->getHMOD() == hinstance)
            {
                dlllistmutex.leave();
                return(pMod);
            }
        }
        else if (pMod->isPe2LxDll())
        {
            if (((Win32Pe2LxDll *)pMod)->getHMOD() == hinstance)
            {
                dlllistmutex.leave();
                return(pMod);
            }
        }
    }

    dlllistmutex.leave();
    return(NULL);
}
//******************************************************************************
//******************************************************************************
BOOL Win32DllBase::isDll()
{
    return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32DllBase::head = NULL;
Queue         Win32DllBase::loadLibDlls;
