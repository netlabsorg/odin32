/* $Id: windll.h,v 1.7 1999-08-23 18:06:45 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 PE Dll class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __WINDLL_H__
#define __WINDLL_H__

#include "winimage.h"

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

typedef ULONG (* WIN32API WIN32DLLENTRY)(ULONG hInstance, ULONG reason, ULONG reserved);

class Win32Dll : public Win32Image
{
public:
                  Win32Dll(HINSTANCE hinstance, int NameTableId, int Win32TableId, WIN32DLLENTRY DllEntryPoint);
	          Win32Dll(char *szDllName, Win32Image *parentImage = NULL);
                 ~Win32Dll();

	void      OS2DllInit(HINSTANCE hinstance, int NameTableId, int Win32TableId,
	  	             WIN32DLLENTRY DllEntryPoint);

        BOOL      init(ULONG reservedMem);

	ULONG     AddRef() 	     { return ++referenced; };
	//ASSUMPTION: called by FreeLibrary only
	ULONG     Release();
	char     *getName()          { return szModule; };
	void      setNoEntryCalls()  { fSkipEntryCalls = TRUE; };

	Win32Dll *getNext()  { return next; };
static 	Win32Dll *getFirst();

//Send DLL_THREAD_ATTACH message to all dlls for a new thread
static  void      attachThreadToAllDlls();

//Send DLL_THREAD_DETACH message to all dlls for thread that's about to die
static  void      detachThreadFromAllDlls();

//Setup TLS structure for all dlls for a new thread
static  void      tlsAttachThreadToAllDlls();

//Destroy TLS structure for all dlls for a thread that's about to die
static  void      tlsDetachThreadFromAllDlls();

	ULONG     getApi(char *name);
	ULONG     getApi(int ordinal);
	BOOL      attachProcess();
	BOOL      detachProcess();
	BOOL      attachThread();
	BOOL      detachThread();

static  void      deleteAll();

static	BOOL      isSystemDll(char *szFileName);

static  Win32Dll *findModule(char *dllname);
static  Win32Dll *findModule(HINSTANCE hinstance);
static  Win32Dll *findModule(WIN32DLLENTRY DllEntryPoint);


virtual BOOL  isDll();

protected:
	BOOL      fSystemDll, fSkipEntryCalls, fUnloaded, fAttachedToProcess;

	WIN32DLLENTRY dllEntryPoint;
private:
	ULONG     referenced;


static  Win32Dll *head;
	Win32Dll *next;
};

#endif