/* $Id: windll.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

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

#ifndef CCHMAXPATH
#define CCHMAXPATH 260
#endif
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
	          Win32Dll(char *szDllName);
                 ~Win32Dll();

	void      OS2DllInit(HINSTANCE hinstance, int NameTableId, int Win32TableId,
	  	             WIN32DLLENTRY DllEntryPoint);

        BOOL      init();

	ULONG     AddRef() 	     { return ++referenced; };
	//ASSUMPTION: called by FreeLibrary only
	ULONG     Release();
	char     *getName()          { return szModule; };
	void      setNoEntryCalls()  { fSkipEntryCalls = TRUE; };

	Win32Dll *getNext()  { return next; };
static 	Win32Dll *getFirst();

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

protected:
	BOOL      fSystemDll, fSkipEntryCalls, fUnloaded;

	WIN32DLLENTRY dllEntryPoint;
private:
	ULONG     referenced;
	char      szModule[CCHMAXPATH];

	char     *StripPath(char *path);


static  Win32Dll *head;
	Win32Dll *next;
};

#endif