/* $Id: windlllx.h,v 1.1 2000-03-09 19:03:21 sandervl Exp $ */

/*
 * Win32 LX Dll class (compiled in OS/2 using Odin32 api)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINDLLLX_H__
#define __WINDLLLX_H__

#include <windllbase.h>
#include <winimagelx.h>

class Win32LxDll : public Win32LxImage, public Win32DllBase
{
public:
	Win32LxDll(HINSTANCE hInstance, WIN32DLLENTRY DllEntryPoint, PVOID pResData);
virtual ~Win32LxDll();

#ifdef DEBUG
virtual	ULONG     AddRef(char *parentname = NULL);
#else
virtual ULONG     AddRef();
#endif
virtual	ULONG     Release();

	// Loaded by DosLoadModule (only relevant for LX dlls)
virtual	void      setLoadLibrary() { fLoadLibrary = TRUE; };

	// isLoaded returns TRUE when a dll has been loaded with DosLoadModule
virtual BOOL      isLoaded()       { return fLoadLibrary; };

	//Should only be called to make sure DosLoadModule is called at least 
        //once for a dll (to make sure OS/2 doesn't unload the dll when it's
        //still needed)
virtual	void      loadLibrary();

virtual BOOL      isLxDll();
    
protected:

private:
};

#endif //__WINDLLLX_H__
