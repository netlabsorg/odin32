/* $Id: library.cpp,v 1.1 2001-04-26 19:26:11 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * COM/OLE misc. functions.
 * 
 * 1/7/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Some portions from Wine Implementation
 *   Copyright 1995  Martin von Loewis
 *   Copyright 1998  Justin Bradford
 *   Copyright 1999  Francis Beaudet
 *   Copyright 1999  Sylvain St-Germain
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>


#include "ole32.h"

#include "oString.h"
#include "moniker.h"	// RunningObjectTableImpl_***


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

ODINDEBUGCHANNEL(OLE32-LIBRARY)


// ======================================================================
// Local Data
// ======================================================================

/*
 * this open DLL table belongs in a per process table, but my guess is that
 * it shouldn't live in the kernel, so I'll put them out here in DLL
 * space assuming that there is one OLE32 per process.
 */
typedef struct tagOpenDll 
{
    char *	DllName;                /* really only needed for debugging */
    HINSTANCE	hLibrary;       
    struct tagOpenDll * next;
} OpenDll;

static OpenDll * openDllList = NULL;	/* linked list of open dlls */

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// CoLoadLibrary
// ----------------------------------------------------------------------
//HINSTANCE WIN32API CoLoadLibrary(LPSTR lpszLibName, BOOL bAutoFree)
ODINFUNCTION2(HINSTANCE, CoLoadLibrary,
              LPSTR, lpszLibName,
              BOOL, bAutoFree)
{
    HINSTANCE 	hLibrary;
    OpenDll *	ptr;
    OpenDll *	tmp;
    HANDLE	hdl = GetProcessHeap();
  
    dprintf(("OLE32: CoLoadLibrary(%s)", lpszLibName));

    hLibrary = LoadLibraryExA(lpszLibName, 0, LOAD_WITH_ALTERED_SEARCH_PATH);

    if (!bAutoFree)
	return hLibrary;

    if (openDllList == NULL)
    {
        /* empty list -- add first node */
        openDllList = (OpenDll*)HeapAlloc(hdl, 0, sizeof(OpenDll));
	openDllList->DllName = (char *)HeapAlloc(hdl, 0, strlen(lpszLibName));
	strcpy(openDllList->DllName, lpszLibName);
	openDllList->hLibrary = hLibrary;
	openDllList->next = NULL;
    }
    else
    {
        /* search for this dll */
        int found = FALSE;
        for (ptr = openDllList; ptr->next != NULL; ptr=ptr->next)
        {
  	    if (ptr->hLibrary == hLibrary)
  	    {
	        found = TRUE;
		break;
	    }
        }
	if (!found)
	{
	    /* dll not found, add it */
 	    tmp = openDllList;
	    openDllList = (OpenDll*)HeapAlloc(hdl, 0, sizeof(OpenDll));
	    openDllList->DllName = (char *)HeapAlloc(hdl, 0, strlen(lpszLibName));
	    strcpy(openDllList->DllName, lpszLibName);
	    openDllList->hLibrary = hLibrary;
	    openDllList->next = tmp;
	}
    }
     
    return hLibrary;
}

// ----------------------------------------------------------------------
// CoFreeAllLibraries
// ----------------------------------------------------------------------
void WIN32API CoFreeAllLibraries()
{
    OpenDll *	ptr;
    OpenDll *	tmp;

    dprintf(("OLE32: CoFreeAllLibraries"));

    for (ptr = openDllList; ptr != NULL; )
    {
	tmp=ptr->next;
	CoFreeLibrary(ptr->hLibrary);
	ptr = tmp;
    }
}

// ----------------------------------------------------------------------
// CoFreeLibrary
// ----------------------------------------------------------------------
void WIN32API CoFreeLibrary(HINSTANCE hLibrary)
{
    OpenDll *	ptr;
    OpenDll *	prev;
    OpenDll *	tmp;

    dprintf(("OLE32: CoFreeLibrary"));

    /* lookup library in linked list */
    prev = NULL;
    for (ptr = openDllList; ptr != NULL; ptr=ptr->next)
    {
	if (ptr->hLibrary == hLibrary)
	    break;

	prev = ptr;
    }

    if (ptr == NULL)
	return;

    /* assert: ptr points to the library entry to free */

    /* free library and remove node from list */
    FreeLibrary(hLibrary);
    if (ptr == openDllList)
    {
	tmp = openDllList->next;
	HeapFree(GetProcessHeap(), 0, openDllList->DllName);
	HeapFree(GetProcessHeap(), 0, openDllList);
	openDllList = tmp;
    }
    else
    {
	tmp = ptr->next;
	HeapFree(GetProcessHeap(), 0, ptr->DllName);
	HeapFree(GetProcessHeap(), 0, ptr);
	prev->next = tmp;
    }

}

// ----------------------------------------------------------------------
// CoFreeUnusedLibraries
// ----------------------------------------------------------------------
void WIN32API CoFreeUnusedLibraries()
{
    OpenDll *ptr, *tmp;
    typedef HRESULT(*DllCanUnloadNowFunc)(void);
    DllCanUnloadNowFunc DllCanUnloadNow;

    dprintf(("OLE32: CoFreeUnusedLibraries"));

    for (ptr = openDllList; ptr != NULL; )
    {
	DllCanUnloadNow = (DllCanUnloadNowFunc) GetProcAddress(ptr->hLibrary, "DllCanUnloadNow");
	
	if ( (DllCanUnloadNow != NULL) && (DllCanUnloadNow() == S_OK) )
	{
	    tmp=ptr->next;
	    CoFreeLibrary(ptr->hLibrary);
	    ptr = tmp;
	}
	else
	{
	    ptr=ptr->next;
	}
    }
}

