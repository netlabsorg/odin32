/* $Id: cdlg32.cpp,v 1.5 2000-05-16 20:00:13 sandervl Exp $ */
/*
 *  Common Dialog Boxes interface (32 bit)
 *  Find/Replace
 *
 * Copyright 1999 Bertho A. Stultiens
 */

#ifdef __WIN32OS2__
// ><DJR 17.05.99 Force to use C-interfaces for now to prevent CALLBACK definition compiler error
#define CINTERFACE
#include <os2win.h>
#else
#include "winbase.h"
#endif
#include "commdlg.h"
#include "cderr.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(commdlg)

#include "cdlg.h"


HINSTANCE	COMDLG32_hInstance = 0;

static DWORD	COMDLG32_TlsIndex;
static int	COMDLG32_Attach = 0;

HINSTANCE	COMCTL32_hInstance = 0;

#ifndef __WIN32OS2__
HINSTANCE	SHELL32_hInstance = 0;
HINSTANCE       SHLWAPI_hInstance = 0;

/* DPA */
HDPA	(WINAPI* COMDLG32_DPA_Create) (INT);  
LPVOID	(WINAPI* COMDLG32_DPA_GetPtr) (const HDPA, INT);   
LPVOID	(WINAPI* COMDLG32_DPA_DeletePtr) (const HDPA hdpa, INT i);
LPVOID	(WINAPI* COMDLG32_DPA_DeleteAllPtrs) (const HDPA hdpa);
INT	(WINAPI* COMDLG32_DPA_InsertPtr) (const HDPA, INT, LPVOID); 
BOOL	(WINAPI* COMDLG32_DPA_Destroy) (const HDPA); 

/* IMAGELIST */
HICON	(WINAPI* COMDLG32_ImageList_GetIcon) (HIMAGELIST, INT, UINT);
HIMAGELIST (WINAPI *COMDLG32_ImageList_LoadImageA) (HINSTANCE, LPCSTR, INT, INT, COLORREF, UINT, UINT);
BOOL	(WINAPI* COMDLG32_ImageList_Draw) (HIMAGELIST himl, int i, HDC hdcDest, int x, int y, UINT fStyle);
BOOL	(WINAPI* COMDLG32_ImageList_Destroy) (HIMAGELIST himl);

/* ITEMIDLIST */
LPITEMIDLIST (WINAPI *COMDLG32_PIDL_ILClone) (LPCITEMIDLIST);
LPITEMIDLIST (WINAPI *COMDLG32_PIDL_ILCombine)(LPCITEMIDLIST,LPCITEMIDLIST);
LPITEMIDLIST (WINAPI *COMDLG32_PIDL_ILGetNext)(LPITEMIDLIST);
BOOL (WINAPI *COMDLG32_PIDL_ILRemoveLastID)(LPCITEMIDLIST);
BOOL (WINAPI *COMDLG32_PIDL_ILIsEqual)(LPCITEMIDLIST, LPCITEMIDLIST);

/* SHELL */
BOOL (WINAPI *COMDLG32_SHGetPathFromIDListA) (LPCITEMIDLIST,LPSTR);
HRESULT (WINAPI *COMDLG32_SHGetSpecialFolderLocation)(HWND,INT,LPITEMIDLIST *);
DWORD (WINAPI *COMDLG32_SHGetDesktopFolder)(IShellFolder **);
DWORD	(WINAPI *COMDLG32_SHGetFileInfoA)(LPCSTR,DWORD,SHFILEINFOA*,UINT,UINT);
DWORD (WINAPI *COMDLG32_SHFree)(LPVOID);

/* PATH */
BOOL (WINAPI *COMDLG32_PathIsRootA)(LPCSTR x);
LPCSTR (WINAPI *COMDLG32_PathFindFilenameA)(LPCSTR path);
DWORD (WINAPI *COMDLG32_PathRemoveFileSpecA)(LPSTR fn);
BOOL (WINAPI *COMDLG32_PathMatchSpecW)(LPCWSTR x, LPCWSTR y);
LPSTR (WINAPI *COMDLG32_PathAddBackslashA)(LPSTR path);
#endif

/***********************************************************************
 *	COMDLG32_DllEntryPoint			(COMDLG32.entry)
 *
 *    Initialization code for the COMDLG32 DLL
 *
 * RETURNS:
 *	FALSE if sibling could not be loaded or instantiated twice, TRUE
 *	otherwise.
 */
BOOL WINAPI COMDLG32_DllEntryPoint(HINSTANCE hInstance, DWORD Reason, LPVOID Reserved)
{
	TRACE("(%08x, %08lx, %p)\n", hInstance, Reason, Reserved);

	switch(Reason)
	{
	case DLL_PROCESS_ATTACH:
		COMDLG32_Attach++;
		if(COMDLG32_hInstance)
		{
			dprintf(("comdlg32.dll instantiated twice in one address space!\n"));
			/*
			 * We should return FALSE here, but that will break
			 * most apps that use CreateProcess because we do
			 * not yet support seperate address spaces.
			 */
			return TRUE;
		}

		COMDLG32_hInstance = hInstance;
		DisableThreadLibraryCalls(hInstance);

		if((COMDLG32_TlsIndex = TlsAlloc()) == 0xffffffff)
		{
			dprintf(("No space for COMDLG32 TLS\n"));
			return FALSE;
		}

#ifndef __WIN32OS2__
		COMCTL32_hInstance = LoadLibraryA("COMCTL32.DLL");	
		SHELL32_hInstance = LoadLibraryA("SHELL32.DLL");
		SHLWAPI_hInstance = LoadLibraryA("SHLWAPI.DLL");
		
		if (!COMCTL32_hInstance || !SHELL32_hInstance || !SHLWAPI_hInstance)
		{
			dprintf(("loading of comctl32 or shell32 or shlwapi failed\n"));
			return FALSE;
		}
		/* DPA */
		*(VOID **)&COMDLG32_DPA_Create=(void*)GetProcAddress(COMCTL32_hInstance, (LPCSTR)328L);
		*(VOID **)&COMDLG32_DPA_Destroy=(void*)GetProcAddress(COMCTL32_hInstance, (LPCSTR)329L);
		*(VOID **)&COMDLG32_DPA_GetPtr=(void*)GetProcAddress(COMCTL32_hInstance, (LPCSTR)332L);
		*(VOID **)&COMDLG32_DPA_InsertPtr=(void*)GetProcAddress(COMCTL32_hInstance, (LPCSTR)334L);
		*(VOID **)&COMDLG32_DPA_DeletePtr=(void*)GetProcAddress(COMCTL32_hInstance, (LPCSTR)336L);
		*(VOID **)&COMDLG32_DPA_DeleteAllPtrs=(void*)GetProcAddress(COMCTL32_hInstance, (LPCSTR)337L);

		/* IMAGELIST */
		*(VOID **)&COMDLG32_ImageList_GetIcon=(void*)GetProcAddress(COMCTL32_hInstance,"ImageList_GetIcon");
		*(VOID **)&COMDLG32_ImageList_LoadImageA=(void*)GetProcAddress(COMCTL32_hInstance,"ImageList_LoadImageA");
		*(VOID **)&COMDLG32_ImageList_Draw=(void*)GetProcAddress(COMCTL32_hInstance,"ImageList_Draw");
		*(VOID **)&COMDLG32_ImageList_Destroy=(void*)GetProcAddress(COMCTL32_hInstance,"ImageList_Destroy");
		
		/* ITEMISLIST */
		
		*(VOID **)&COMDLG32_PIDL_ILIsEqual =(void*)GetProcAddress(SHELL32_hInstance, (LPCSTR)21L);
		*(VOID **)&COMDLG32_PIDL_ILCombine =(void*)GetProcAddress(SHELL32_hInstance, (LPCSTR)25L);
		*(VOID **)&COMDLG32_PIDL_ILGetNext =(void*)GetProcAddress(SHELL32_hInstance, (LPCSTR)153L);
		*(VOID **)&COMDLG32_PIDL_ILClone =(void*)GetProcAddress(SHELL32_hInstance, (LPCSTR)18L);
		*(VOID **)&COMDLG32_PIDL_ILRemoveLastID =(void*)GetProcAddress(SHELL32_hInstance, (LPCSTR)17L);
		
		/* SHELL */
		
		*(VOID **)&COMDLG32_SHFree = (void*)GetProcAddress(SHELL32_hInstance,"SHFree");
		*(VOID **)&COMDLG32_SHGetSpecialFolderLocation = (void*)GetProcAddress(SHELL32_hInstance,"SHGetSpecialFolderLocation");
		*(VOID **)&COMDLG32_SHGetPathFromIDListA = (void*)GetProcAddress(SHELL32_hInstance,"SHGetPathFromIDListA");
		*(VOID **)&COMDLG32_SHGetDesktopFolder = (void*)GetProcAddress(SHELL32_hInstance,"SHGetDesktopFolder");
		*(VOID **)&COMDLG32_SHGetFileInfoA = (void*)GetProcAddress(SHELL32_hInstance,"SHGetFileInfoA");

		/* ### WARINIG ### 
		We can't do a GetProcAddress to link to  StrRetToBuf[A|W] sine not all 
		versions of the shlwapi are exporting these functions. When we are 
		seperating the dlls then we have to dublicate code from shell32 into comdlg32. 
		Till then just call these functions. These functions don't have any side effects 
		so it won't break the use of any combination of native and buildin dll's (jsch) */

		/* PATH */
		*(VOID **)&COMDLG32_PathMatchSpecW = (void*)GetProcAddress(SHLWAPI_hInstance,"PathMatchSpecW");
		*(VOID **)&COMDLG32_PathIsRootA = (void*)GetProcAddress(SHLWAPI_hInstance,"PathIsRootA");
		*(VOID **)&COMDLG32_PathRemoveFileSpecA = (void*)GetProcAddress(SHLWAPI_hInstance,"PathRemoveFileSpecA");
		*(VOID **)&COMDLG32_PathFindFilenameA = (void*)GetProcAddress(SHLWAPI_hInstance,"PathFindFileNameA");
		*(VOID **)&COMDLG32_PathAddBackslashA = (void*)GetProcAddress(SHLWAPI_hInstance,"PathAddBackslashA");
#endif
		break;

	case DLL_PROCESS_DETACH:
		if(!--COMDLG32_Attach)
		{
			TlsFree(COMDLG32_TlsIndex);
			COMDLG32_hInstance = 0;

		}
#ifndef __WIN32OS2__
		FreeLibrary(COMCTL32_hInstance);
		FreeLibrary(SHELL32_hInstance);
		FreeLibrary(SHLWAPI_hInstance);
#endif
		break;
	}
	return TRUE;
}


/***********************************************************************
 *	COMDLG32_AllocMem 			(internal)
 * Get memory for internal datastructure plus stringspace etc.
 *	RETURNS
 *		Pointer to a heap block: Succes
 *		NULL: Failure
 */
LPVOID COMDLG32_AllocMem(
	int size	/* [in] Block size to allocate */
) {
        LPVOID ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
        if(!ptr)
        {
        	COMDLG32_SetCommDlgExtendedError(CDERR_MEMALLOCFAILURE);
                return NULL;
        }
        return ptr;
}


/***********************************************************************
 *	COMDLG32_SetCommDlgExtendedError	(internal)
 *
 * Used to set the thread's local error value if a comdlg32 function fails.
 */
void COMDLG32_SetCommDlgExtendedError(DWORD err)
{
	TRACE("(%08lx)\n", err);
	TlsSetValue(COMDLG32_TlsIndex, (void *)err);
}


/***********************************************************************
 *	CommDlgExtendedError			(COMDLG32.5)
 *
 * Get the thread's local error value if a comdlg32 function fails.
 *	RETURNS
 *		Current error value which might not be valid
 *		if a previous call succeeded.
 */
DWORD WINAPI CommDlgExtendedError(void)
{
	return (DWORD)TlsGetValue(COMDLG32_TlsIndex);
}
