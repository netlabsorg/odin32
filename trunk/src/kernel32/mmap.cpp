/* $Id: mmap.cpp,v 1.12 1999-08-25 15:27:19 sandervl Exp $ */

/*
 * Win32 Memory mapped file class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * NOTE: Memory mapping DOES NOT work when kernel-mode code causes
 *       a pagefault in the memory mapped object. (exceptions aren't
 *       dispatched to our exception handler until after the kernel mode
 *       call returns (too late))
 *
 * NOTE: There's no easy way to determine which pages were modified. Temporary
 *       solution is to write all paged-in memory to disk.
 *
 * NOTE: Are apps allowed to change the protection flags of memory mapped pages?
 *       I'm assuming they aren't for now.
 *
 * TODO: Handles returned should be usable by all apis that accept file handles
 * TODO: Sharing memory mapped files between multiple processes
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <win\virtual.h>
#include <vmutex.h>
#include <handlemanager.h>
#include "mmap.h"

VMutex globalmapMutex;

//******************************************************************************
//TODO: sharing between processes
//******************************************************************************
Win32MemMap::Win32MemMap(HFILE hfile, ULONG size, ULONG fdwProtect, LPSTR lpszName)
               : fMapped(FALSE), pMapping(NULL), mMapAccess(0), referenced(0)
{
  globalmapMutex.enter();
  next    = memmaps;
  memmaps = this;
  globalmapMutex.leave();

  hMemFile   = hfile;

  mSize      = size;
  mProtFlags = fdwProtect;

  if(lpszName) {
	lpszMapName = (char *)malloc(strlen(lpszName)+1);
	strcpy(lpszMapName, lpszName);
  }
  else	lpszMapName = NULL;
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMap::Init(HANDLE hMemMap)
{
  mapMutex.enter();
  if(hMemFile != -1) 
  {
     	if(DuplicateHandle(GetCurrentProcess(), hMemFile, GetCurrentProcess(),
                           &hMemFile, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) 
     	{
		dprintf(("Win32MemMap::Init: DuplicateHandle failed!"));
		goto fail;
     	}
	mSize = SetFilePointer(hMemFile, 0, NULL, FILE_END);
	if(mSize == -1) {
		dprintf(("Win32MemMap::init: SetFilePointer failed to set pos end"));
		goto fail;
	}
  }
  this->hMemMap = hMemMap;
  mapMutex.leave();
  return TRUE;
fail:
  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
//******************************************************************************
Win32MemMap::~Win32MemMap()
{
  unmapViewOfFile();
  if(lpszMapName) {
	free(lpszMapName);
  }
  mapMutex.enter();
  if(pMapping) {
	VirtualFree(pMapping, mSize, MEM_RELEASE);
	pMapping = NULL;
  }
  if(hMemFile != -1) {
  	CloseHandle(hMemFile);
	hMemFile = -1;
  }
  mapMutex.leave();

  globalmapMutex.enter();
  Win32MemMap *map = memmaps;

  if(map == this) {
	memmaps = next;
  }
  else {
  	while(map->next) {
		if(map->next == this)
			break;
		map = map->next;
	}
	if(map->next) {
		map->next = next;
	}
	else	dprintf(("Win32MemMap::~Win32MemMap: map not found!! (%x)", this));
  }
  globalmapMutex.leave();
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMap::hasReadAccess()
{
  return TRUE; //should have at least this
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMap::hasWriteAccess()
{
  return !(mProtFlags & PAGE_READONLY);
}
//******************************************************************************
//Might want to add this feature for memory mapping executable & dll files in 
//the loader (done in Win32 with the SEC_IMAGE flag?)
//******************************************************************************
BOOL Win32MemMap::hasExecuteAccess()
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMap::commitPage(LPVOID lpPageFaultAddr, ULONG nrpages)
{
 DWORD pageAddr = (DWORD)lpPageFaultAddr & ~0xFFF;
 DWORD oldProt, newProt, nrBytesRead, offset, size;
 
//  mapMutex.enter();
  newProt  = mProtFlags & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY);

  dprintf(("Win32MemMap::commitPage %x (faultaddr %x), nr of pages %d", pageAddr, lpPageFaultAddr, nrpages));
  if(hMemFile != -1) {
  	if(VirtualAlloc((LPVOID)pageAddr, nrpages*PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE) == FALSE) {
		goto fail;
  	}
	offset = pageAddr - (ULONG)pMapping;
	size   = nrpages*PAGE_SIZE;
	if(offset + size > mSize) {
		size = mSize - offset;
	}
	if(SetFilePointer(hMemFile, offset, NULL, FILE_BEGIN) != offset) {
		dprintf(("Win32MemMap::commitPage: SetFilePointer failed to set pos to %x", offset));
		goto fail;
	}
	if(ReadFile(hMemFile, (LPSTR)pageAddr, size, &nrBytesRead, NULL) == FALSE) {
		dprintf(("Win32MemMap::commitPage: ReadFile failed for %x", pageAddr));
		goto fail;
	}
	if(nrBytesRead != size) {
		dprintf(("Win32MemMap::commitPage: ReadFile didn't read all bytes for %x", pageAddr));
		goto fail;
	}
	if(mProtFlags & PAGE_READONLY) {
//DosSetMem returns flags with EXECUTE bit set, even though the initial allocation is without this bit set!
//Also returns access denied when trying to set it back to READONLY
  		VirtualProtect((LPVOID)pageAddr, nrpages*PAGE_SIZE, newProt, &oldProt);
//  		if(VirtualProtect((LPVOID)pageAddr, nrpages*PAGE_SIZE, newProt, &oldProt) == FALSE) {
//			goto fail;
//		}
	}
  }
  else {
  	if(VirtualAlloc((LPVOID)pageAddr, nrpages*PAGE_SIZE, MEM_COMMIT, newProt) == FALSE) {
		goto fail;
  	}
  }

//  mapMutex.leave();
  return TRUE;
fail:
//  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32MemMap::unmapViewOfFile()
{
  if(fMapped == FALSE)
	return FALSE;

  flushView(pMapping, mSize);
  mapMutex.enter();
  if(pMapping) {
	VirtualFree(pMapping, mSize, MEM_RELEASE);
  }
  pMapping = NULL;
  fMapped = FALSE;
  mapMutex.leave();
  return TRUE;
}
//******************************************************************************
//******************************************************************************
LPVOID Win32MemMap::mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess)
{
  mapMutex.enter();
  ULONG memFlags = (mProtFlags & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY));
  ULONG fAlloc   = 0;
  LPVOID mapview;

  if((fdwAccess & FILE_MAP_WRITE) && !(mProtFlags & PAGE_READWRITE)) 
	goto parmfail;
  if((fdwAccess & FILE_MAP_READ) && !(mProtFlags & (PAGE_READWRITE|PAGE_READONLY))) 
	goto parmfail;
  if((fdwAccess & FILE_MAP_COPY) && !(mProtFlags & PAGE_WRITECOPY)) 
	goto parmfail;

//TODO: If committed, read file into memory
#if 0  
  if(mProtFlags & SEC_COMMIT)
	fAlloc |= MEM_COMMIT;
  else
  if(mProtFlags & SEC_RESERVE)
 	fAlloc |= MEM_RESERVE;
#else
  fAlloc = MEM_RESERVE;
#endif

  if(fMapped == FALSE) {//if not mapped, reserve/commit entire view
  	pMapping = VirtualAlloc(0, mSize, fAlloc, memFlags);
  	if(pMapping == NULL) {
		dprintf(("Win32MemMap::mapFileView: VirtualAlloc %x %x %x failed!", mSize, fAlloc, memFlags));
		goto fail;
  	}
	fMapped = TRUE;
  }
  mapview = (LPVOID)((ULONG)pMapping + offset);
  mapMutex.leave();
  return mapview;

parmfail:
  dprintf(("Win32MemMap::mapFileView: ERROR_INVALID_PARAMETER"));
  SetLastError(ERROR_INVALID_PARAMETER);
fail:
  mapMutex.leave();
  return 0;
}
//******************************************************************************
// NOTE: There's no easy way to determine which pages were modified. Temporary
//       solution is to write all paged-in memory to disk.
//TODO: Are apps allowed to change the protection flags of memory mapped pages?
//      I'm assuming they aren't for now.
//******************************************************************************
BOOL Win32MemMap::flushView(LPVOID lpvBase, ULONG cbFlush)
{
 MEMORY_BASIC_INFORMATION memInfo;
 ULONG nrpages, nrBytesWritten, offset, size;
 int   i;

//  mapMutex.enter();
  dprintf(("Win32MemMap::flushView: %x %x", lpvBase, cbFlush));
  if(fMapped == FALSE)
	goto parmfail;

  if(cbFlush == 0) 
 	cbFlush = mSize;

  if(lpvBase < pMapping || (ULONG)lpvBase+cbFlush > (ULONG)pMapping+mSize)
	goto parmfail;

  if(mProtFlags & PAGE_READONLY)
	goto parmfail;

  if(hMemFile == -1)
	goto success; //TODO: Return an error here?

  nrpages = cbFlush/PAGE_SIZE;
  if(cbFlush & 0xFFF)  nrpages++;

  for(i=0;i<nrpages;i++) {
	if(VirtualQuery((LPSTR)lpvBase+i*PAGE_SIZE, &memInfo, PAGE_SIZE) == 0) {
		dprintf(("Win32MemMap::flushView: VirtualQuery (%x,%x) failed for %x", lpvBase, cbFlush, (ULONG)lpvBase+i*PAGE_SIZE));
		goto fail;
	}
	//If a page is reserved or write protected, we won't bother flushing it to disk
	if(memInfo.State & MEM_COMMIT && 
           memInfo.AllocationProtect & (PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)) 
        {//committed and allowed for writing?
		offset = (ULONG)lpvBase+i*PAGE_SIZE - (ULONG)pMapping;
		size   = PAGE_SIZE;
		if(offset + size > mSize) {
			size = mSize - offset;
		}
		dprintf(("Win32MemMap::flushView for offset %x, size %d", offset, size));

		if(SetFilePointer(hMemFile, offset, NULL, FILE_BEGIN) != offset) {
			dprintf(("Win32MemMap::flushView: SetFilePointer failed to set pos to %x", offset));
			goto fail;
		}
		if(WriteFile(hMemFile, (LPSTR)lpvBase+i*PAGE_SIZE, size, &nrBytesWritten, NULL) == FALSE) {
			dprintf(("Win32MemMap::flushView: WriteFile failed for %x", (ULONG)lpvBase+i*PAGE_SIZE));
			goto fail;
		}
		if(nrBytesWritten != size) {
			dprintf(("Win32MemMap::flushView: WriteFile didn't write all bytes for %x", (ULONG)lpvBase+i*PAGE_SIZE));
			goto fail;
		}
	}
  }
success:
//  mapMutex.leave();
  return TRUE;
parmfail:
  SetLastError(ERROR_INVALID_PARAMETER);
//  mapMutex.leave();
  return FALSE;
fail:
//  mapMutex.leave();
  return FALSE;
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::findMap(LPSTR lpszName)
{
  globalmapMutex.enter();
  Win32MemMap *map = memmaps;

  if(map != NULL) {
  	while(map) {
		if(map->lpszMapName && !strcmp(map->lpszMapName, lpszName))
			break;
		map = map->next;
	}
  }
  globalmapMutex.leave();
  dprintf(("Win32MemMap::findMap: couldn't find map %s", lpszName));
  return map;
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::findMap(ULONG address)
{
  globalmapMutex.enter();
  Win32MemMap *map = memmaps;

  if(map != NULL) {
  	while(map) {
		if(map->pMapping && (ULONG)map->pMapping <= address &&
                   (ULONG)map->pMapping + map->mSize > address)
		{
			break;
		}
		map = map->next;
	}
  }
  globalmapMutex.leave();
  return map;
}
//******************************************************************************
//******************************************************************************
void Win32MemMap::deleteAll()
{
  while(memmaps) {
	CloseHandle(memmaps->hMemMap);
  }
}
//******************************************************************************
//******************************************************************************
Win32MemMap *Win32MemMap::memmaps = NULL;
