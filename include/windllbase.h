/* $Id: windllbase.h,v 1.1 1999-09-15 23:29:36 sandervl Exp $ */

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

class Win32DllBase : public virtual Win32ImageBase
{
public:
                  Win32DllBase(HINSTANCE hInstance, WIN32DLLENTRY DllEntryPoint);
virtual          ~Win32DllBase();

	ULONG     AddRef() 	     { return ++referenced; };
	//ASSUMPTION: called by FreeLibrary only
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

virtual	ULONG     getApi(char *name)  = 0;
virtual ULONG     getApi(int ordinal) = 0;

	BOOL      attachProcess();
	BOOL      detachProcess();
	BOOL      attachThread();
	BOOL      detachThread();

static  void      deleteAll();

static	BOOL      isSystemDll(char *szFileName);

virtual BOOL      isLxDll() = 0;
virtual BOOL      isDll();

static  Win32DllBase *findModule(char *dllname);
static  Win32DllBase *findModule(HINSTANCE hinstance);
static  Win32DllBase *findModule(WIN32DLLENTRY DllEntryPoint);

protected:
	BOOL          fSkipEntryCalls, fUnloaded, fAttachedToProcess;

	WIN32DLLENTRY dllEntryPoint;

	ULONG         referenced;

private:
static  Win32DllBase *head;
	Win32DllBase *next;
};

#endif