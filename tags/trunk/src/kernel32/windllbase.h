/* $Id: windllbase.h,v 1.2 2000-05-02 20:53:14 sandervl Exp $ */

/*
 * Win32 Dll base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINDLLBASE_H__
#define __WINDLLBASE_H__

#include <winimagebase.h>
#include <odinlx.h>
#include <vmutex.h>

#ifndef HINSTANCE
#define HINSTANCE ULONG
#endif

#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3
#define DLL_PROCESS_DETACH 0

#define DONT_RESOLVE_DLL_REFERENCES     0x00000001
#define LOAD_LIBRARY_AS_DATAFILE        0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH   0x00000008

//odin.ini section names to lookup renamed dlls
//i.e. OLE32 -> OLE32OS2
#define DLLRENAMEWIN_SECTION 	"DLLRENAMEWIN"
//i.e. OLE32OS2 -> OLE32
#define DLLRENAMEOS2_SECTION    "DLLRENAMEOS2"

class Win32DllBase : public virtual Win32ImageBase
{
public:
                  Win32DllBase(HINSTANCE hInstance, WIN32DLLENTRY DllEntryPoint, Win32ImageBase *parent = NULL);
virtual          ~Win32DllBase();

#ifdef DEBUG
virtual	ULONG     AddRef(char *parentname = NULL);
#else
virtual ULONG     AddRef();
#endif
virtual	ULONG     Release();

	char     *getName()          { return szModule; };
	void      setNoEntryCalls()  { fSkipEntryCalls = TRUE; };

	Win32DllBase *getNext()  { return next; };
static 	Win32DllBase *getFirst();

//Send DLL_THREAD_ATTACH message to all dlls for a new thread
static  void      attachThreadToAllDlls();

//Send DLL_THREAD_DETACH message to all dlls for thread that's about to die
static  void      detachThreadFromAllDlls();

//Setup TLS structure for all dlls for a new thread
static  void      tlsAttachThreadToAllDlls();

//Destroy TLS structure for all dlls for a thread that's about to die
static  void      tlsDetachThreadFromAllDlls();

	BOOL      attachProcess();
	BOOL      detachProcess();
	BOOL      attachThread();
	BOOL      detachThread();

	// enable / disable thread attach/detach calls
	void      setThreadLibraryCalls(BOOL fEnable);

	//This counter is incremented when the dll has been loaded with LoadLibrary(Ex)
        //(== not loaded on behalf of another dll or the main exe)
	void      incDynamicLib();
	void      decDynamicLib();
	BOOL      isDynamicLib()   { return nrDynamicLibRef != 0; };

	void      setUnloadOrder(Win32ImageBase *parent);

	void      updateDependencies();

	BOOL      RemoveCircularDependency(Win32DllBase *parent);

	// Loaded by DosLoadModule
virtual	void      setLoadLibrary() { fLoadLibrary = TRUE; };

	// isLoaded returns TRUE when a dll has been loaded with DosLoadModule
virtual BOOL      isLoaded()       { return fLoadLibrary; };

	//Should only be called to make sure DosLoadModule is called at least 
        //once for a dll (to make sure OS/2 doesn't unload the dll when it's
        //still needed)
virtual	void      loadLibrary();

	//Only called for kernel32
	void      DisableUnload()  { fDisableUnload = TRUE; };

static  void      deleteDynamicLibs();
static  void      deleteAll();

static	BOOL      isSystemDll(char *szFileName);

virtual BOOL      isLxDll() = 0;
virtual BOOL      isDll();

static  void      renameDll(char *dllname, BOOL fWinToOS2=TRUE);
static  void      setDefaultRenaming();

static  Win32DllBase *findModule(char *dllname, BOOL fRenameFirst = FALSE);
static  Win32DllBase *findModule(HINSTANCE hinstance);
static  Win32DllBase *findModule(WIN32DLLENTRY DllEntryPoint);
static  Win32DllBase *findModuleByAddr(ULONG address);

#ifdef DEBUG_ENABLELOG_LEVEL2
	void          printListOfDlls();
#endif

protected:
#ifdef DEBUG
	void          printDependencies(char *parent);
#endif

	BOOL          fSkipEntryCalls, fUnloaded, fAttachedToProcess;

	WIN32DLLENTRY dllEntryPoint;

	LONG          referenced;

	//This counter is incremented when the dll has been loaded with LoadLibrary(Ex)
        //(== not loaded on behalf of another dll or the main exe)
	BOOL          nrDynamicLibRef;

	BOOL          fInserted; //inserted in dll list

	//This flag is set when a dll has been loaded with DosLoadModule
        BOOL          fLoadLibrary;
	BOOL	      fDisableUnload;

private:
static  Queue         loadLibDlls;
static  Win32DllBase *head;
	Win32DllBase *next;
};

extern VMutex dlllistmutex;   //protects linked lists of heaps

#endif
