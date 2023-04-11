/* $Id: hmmmap.cpp,v 1.24 2003-04-02 11:03:31 sandervl Exp $ */

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
			 		     HANDLE hFile,
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

  //It's not allowed to map a file of length 0 according to MSDN. This time
  //the docs are correct. (verified in NT4 SP6)
  //bird: It's not allowed to create a non-file based mapping with size 0.
  //TODO: also need to verify access rights of the file handle (write maps need
  //      write access obviously)
  if(hFile != -1) {
      DWORD dwFileSizeLow = 0, dwFileSizeHigh = 0;

      dwFileSizeLow = ::GetFileSize(hFile, &dwFileSizeHigh);
      if(dwFileSizeHigh == 0 && dwFileSizeLow == 0) {
          /*
           * Two actions, if we can grow the file we should if not fail.
           */
          if (!(protect & PAGE_READWRITE)) /* TODO: check this and verify flags */
          {
          dprintf(("CreateFileMappingA: not allowed to map a file with length 0!!"));
              return ERROR_NOT_ENOUGH_MEMORY; /* XP returns this if the mapping is readonly, odd.. */
          }
          /*
           * Try extend the file.
           * (Not sure if we need to preserve the filepointer, but it doesn't hurt I think.)
           */
          LONG  lFilePosHigh = 0;
          DWORD dwFilePosLow = ::SetFilePointer(hFile, 0, &lFilePosHigh, FILE_CURRENT);
          LONG  lFileSizeHigh = size_high;
          if (   ::SetFilePointer(hFile, size_low, &lFileSizeHigh, FILE_BEGIN) == INVALID_SET_FILE_POINTER
              || !::SetEndOfFile(hFile))
          {
              ::SetFilePointer(hFile, dwFilePosLow, &lFilePosHigh, FILE_BEGIN);
              dprintf(("CreateFileMappingA: unable to grow file to 0x%#08x%08x bytes.\n", size_high, size_low));
              return ERROR_DISK_FULL;
          }
          ::SetFilePointer(hFile, dwFilePosLow, &lFilePosHigh, FILE_BEGIN);
      }
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
        // -> for the same file handle, yes

        //if map already exists, we must create a new handle to the existing
        //map object and return ERROR_ALREADY_EXISTS
        pHMHandleData->dwUserData = (ULONG)map;
        pHMHandleData->dwInternalType = HMTYPE_MEMMAP;

        //findMap already incremented the reference count, so we simply don't
        //release it here
        return ERROR_ALREADY_EXISTS;
  }

  //We reuse the original memory map object if another one is created for
  //the same file handle
  //TODO: different file handles can exist for the same file (DuplicateHandle)
  map = Win32MemMap::findMapByFile(hFile);
  if(map) {
     Win32MemMapDup *dupmap;

     dprintf(("CreateFileMappingA: duplicating map with file %x!", hFile));

     dupmap = new Win32MemMapDup(map, hFile, size_low, protect, (LPSTR)name);

     if(dupmap == NULL) {
         dprintf(("CreateFileMappingA: can't create Win32MemMap object!"));
         return ERROR_OUTOFMEMORY;
     }
     map->Release(); //findMapByFile increases the refcount, so decrease it here
     map = dupmap;
  }
  else {
     map = new Win32MemMap(hFile, size_low, protect, (LPSTR)name);

     if(map == NULL) {
         dprintf(("CreateFileMappingA: can't create Win32MemMap object!"));
         return ERROR_OUTOFMEMORY;
     }
  }
  if(map->Init(size_low) == FALSE) {
      dprintf(("CreateFileMappingA: init failed!"));
      delete map;
      return ERROR_GEN_FAILURE;
  }
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
 DWORD        ret;

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
	if(!(protflags & (PAGE_WRITECOPY|PAGE_READWRITE))) {
            ret = ERROR_INVALID_PARAMETER;
            goto fail;
        }
	break;
  case FILE_MAP_READ:
	if(!(protflags & (PAGE_READWRITE | PAGE_READONLY))) {
            ret = ERROR_INVALID_PARAMETER;
            goto fail;
        }
	break;
  case FILE_MAP_COPY:
	if(!(protflags & PAGE_WRITECOPY)) {
            ret = ERROR_INVALID_PARAMETER;
            goto fail;
        }
	break;
  }
  //findMap already incremented the reference count, so we simply don't
  //release it here
  pHMHandleData->dwUserData = (ULONG)map;
  pHMHandleData->dwInternalType = HMTYPE_MEMMAP;
  return NO_ERROR;

fail:
  map->Release();
  return ret;
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

  if(pHMHandleData->dwUserData == (ULONG) NULL || pHMHandleData->dwInternalType != HMTYPE_MEMMAP) {
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
  if(pHMHandleData->dwUserData == (ULONG) NULL || pHMHandleData->dwInternalType != HMTYPE_MEMMAP) {
	dprintf(("HMDeviceMemMapClass::CloseHandle: invalid handle data!"));
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
