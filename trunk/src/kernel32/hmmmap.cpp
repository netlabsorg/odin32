/* $Id: hmmmap.cpp,v 1.17 2001-04-26 13:22:46 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#undef DEBUG_LOCAL
//#define DEBUG_LOCAL


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "misc.h"

#include "HandleManager.H"
#include "HMMMap.h"
#include "mmap.h"
#include "heapshared.h"

#define DBG_LOCALLOG	DBG_hmmmap
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/

//******************************************************************************
//******************************************************************************
DWORD HMDeviceMemMapClass::CreateFileMapping(PHMHANDLEDATA         pHMHandleData,
			 		     HFILE hFile, 
                	 		     SECURITY_ATTRIBUTES *sa, /* [in] Optional security attributes*/
                	 		     DWORD protect,   /* [in] Protection for mapping object */
                		   	     DWORD size_high, /* [in] High-order 32 bits of object size */
                		   	     DWORD size_low,  /* [in] Low-order 32 bits of object size */
                		   	     LPCSTR name)     /* [in] Name of file-mapping object */ 
{
 Win32MemMap *map;

  if((hFile == -1 && size_low == 0) || size_high ||
     protect & ~(PAGE_READONLY|PAGE_READWRITE|PAGE_WRITECOPY|SEC_COMMIT|SEC_IMAGE|SEC_RESERVE|SEC_NOCACHE) ||
     (protect & (PAGE_READONLY|PAGE_READWRITE|PAGE_WRITECOPY)) == 0 ||
//     (hFile == -1 && (protect & SEC_COMMIT)) ||
     ((protect & SEC_COMMIT) && (protect & SEC_RESERVE))) 
  {

	dprintf(("CreateFileMappingA: invalid parameter (combination)!"));
        dprintf(("Parameters: %x %x %x %x %s", hFile, protect, size_high, size_low, name));
	return ERROR_INVALID_PARAMETER;
  }

  map = Win32MemMap::findMap((LPSTR)name);
  if(map != NULL) {
	dprintf(("CreateFileMappingA: duplicating map %s!", name));

  	DWORD protflags = map->getProtFlags();
  	switch(protect) {
  	case FILE_MAP_WRITE:
		if(!(protflags & PAGE_WRITECOPY))
			dprintf(("Different flags for duplicate!"));
		break;
  	case FILE_MAP_READ:
		if(!(protflags & (PAGE_READWRITE | PAGE_READONLY)))
			dprintf(("Different flags for duplicate!"));
		break;
  	case FILE_MAP_COPY:
		if(!(protflags & PAGE_WRITECOPY))
			dprintf(("Different flags for duplicate!"));
		break;
  	}
	//TODO:
	//Is it allowed to open an existing view with different flags?
        //(i.e. write access to readonly object)
	dprintf(("CreateFileMapping: duplicate handle of existing file mapping %s", name));

        //if map already exists, we must create a new handle to the existing
        //map object and return ERROR_ALREADY_EXISTS
  	map->AddRef();
        pHMHandleData->dwUserData = (ULONG)map;
        pHMHandleData->dwInternalType = HMTYPE_MEMMAP;
        return ERROR_ALREADY_EXISTS;
  }
  else {
	map = new Win32MemMap(hFile, size_low, protect, (LPSTR)name);

  	if(map == NULL) {
		dprintf(("CreateFileMappingA: can't create Win32MemMap object!"));
		return ERROR_OUTOFMEMORY;
  	} 

  	if(map->Init() == FALSE) {
		dprintf(("CreateFileMappingA: init failed!"));
		delete map;
		return ERROR_GEN_FAILURE;
	}
  }
  map->AddRef();
  pHMHandleData->dwUserData = (ULONG)map;
  pHMHandleData->dwInternalType = HMTYPE_MEMMAP;
  return NO_ERROR;
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceMemMapClass::OpenFileMapping(PHMHANDLEDATA         pHMHandleData,
                                           DWORD access,   /* [in] Access mode */
                			   BOOL inherit,   /* [in] Inherit flag */
                		           LPCSTR name )   /* [in] Name of file-mapping object */
{
 Win32MemMap *map;
 DWORD        protflags;

  if(name == NULL)
	return ERROR_INVALID_PARAMETER;

  map = Win32MemMap::findMap((LPSTR)name);
  if(map == NULL) {
	dprintf(("OpenFileMapping: mapping %s not found", name));
	return ERROR_FILE_NOT_FOUND;
  }
  protflags = map->getProtFlags();
  switch(access) {
  case FILE_MAP_WRITE:
  case FILE_MAP_ALL_ACCESS:
	if(!(protflags & PAGE_WRITECOPY))
		return ERROR_INVALID_PARAMETER;
	break;
  case FILE_MAP_READ:
	if(!(protflags & (PAGE_READWRITE | PAGE_READONLY)))
		return ERROR_INVALID_PARAMETER;
	break;
  case FILE_MAP_COPY:
	if(!(protflags & PAGE_WRITECOPY))
		return ERROR_INVALID_PARAMETER;
	break;
  }
  map->AddRef();
  pHMHandleData->dwUserData = (ULONG)map;
  pHMHandleData->dwInternalType = HMTYPE_MEMMAP;
  return NO_ERROR;
}
//******************************************************************************
//******************************************************************************
LPVOID HMDeviceMemMapClass::MapViewOfFileEx(PHMHANDLEDATA pHMHandleData,
                                      DWORD         dwDesiredAccess,
                                      DWORD         dwFileOffsetHigh,
                                      DWORD         dwFileOffsetLow,
                                      DWORD         dwNumberOfBytesToMap,
		                      LPVOID        lpBaseAddress)
{
 Win32MemMap *map;

  dprintf(("KERNEL32: HMDeviceMemMapClass::MapViewOfFileEx(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwDesiredAccess,
           dwFileOffsetHigh,
           dwFileOffsetLow,
           dwNumberOfBytesToMap, 
           lpBaseAddress));

  if(lpBaseAddress != NULL) 
  {
#if 0
     //No can do. Let us choose the address
     dprintf(("Can't create view to virtual address %x", lpBaseAddress));
     SetLastError(ERROR_OUTOFMEMORY);
     return NULL;
#else
    // PH 2000/05/24 IBM VAJ3 uses this function.
    // I don't think we'll ever succeed in EXACTLY copying the original
    // behaviour of the function. Maybe ignoring the base address helps?
    dprintf(("WARNING: suggested virtual address %x IGNORED! (experimental API violation)",
             lpBaseAddress));
#endif
  }

  if(pHMHandleData->dwUserData == NULL || pHMHandleData->dwInternalType != HMTYPE_MEMMAP) {
	dprintf(("MapViewOfFileEx: invalid handle data!"));
	SetLastError(ERROR_INVALID_HANDLE);
	return NULL;
  }
  map = (Win32MemMap *)pHMHandleData->dwUserData;

  return map->mapViewOfFile(dwNumberOfBytesToMap, dwFileOffsetLow, dwDesiredAccess);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceMemMapClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
 Win32MemMap *map;

  dprintf(("HMDeviceMemMapClass::CloseHandle %x", pHMHandleData->dwUserData));
  if(pHMHandleData->dwUserData == NULL || pHMHandleData->dwInternalType != HMTYPE_MEMMAP) {
	dprintf(("MapViewOfFileEx: invalid handle data!"));
	return FALSE;
  }
  //Although an application may close the file handle used to create a file
  //mapping object, the system keeps the corresponding file open until the last
  //view of the file is unmapped.
  map = (Win32MemMap *)pHMHandleData->dwUserData;
  map->Release();

  return TRUE;
}
//******************************************************************************
//******************************************************************************
